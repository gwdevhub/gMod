module;

#include "Main.h"
#include "Error.h"
#include "D3D9State.h"
#include <DDSTextureLoader/DDSTextureLoader9.h>
#include <DirectXTex/DirectXTex.h>
#include <mutex>
#include <unordered_map>

export module TextureClient;
import TextureFunction;
import ModfileLoader;

export std::vector<std::pair<std::string, std::string>> modlists_contents;

struct PendingOp {
    enum class Kind {
        Add,
        Remove
    };
    Kind kind;
    HashType hash;
    std::vector<BYTE> data;
};

// Couples a value with the mutex guarding it; the value is only reachable while
// the lock is held.
template <class T>
class Guarded {
    std::mutex mutex;
    T value;

public:
    template <class Fn>
    auto with(Fn&& fn)
    {
        std::lock_guard lock(mutex);
        return fn(value);
    }
};

/*
 *  Owned by each d3d9 device (see D3D9Hooks.cpp). Holds the side-state for every
 *  texture and performs the actual modding. The On*() methods are the vtable-hook
 *  entry points, all driven by the game's render thread.
 */
export class TextureClient {
public:
    TextureClient(IDirect3DDevice9* device);
    ~TextureClient();

    // Called from the hooked vtable slots:
    TexState* OnCreateTexture(IDirect3DBaseTexture9* texture, TexType type);
    void OnBeginScene();
    // The fake replacement when `texture` is a modded original, else `texture`.
    IDirect3DBaseTexture9* ResolveBinding(IDirect3DBaseTexture9* texture);
    // For GetTexture: the original behind one of our fakes, or nullptr.
    IDirect3DBaseTexture9* ResolveOriginalFromFake(IDirect3DBaseTexture9* texture);
    void OnUpdateTexture(IDirect3DBaseTexture9* source, IDirect3DBaseTexture9* destination);
    void OnReleaseTexture(IDirect3DBaseTexture9* texture); // called once the real refcount hit zero

    void Initialize();

    void EnqueueAdd(HashType hash, std::vector<BYTE> data);
    void EnqueueRemove(HashType hash);

    static int AddFile(const std::filesystem::path& path);
    static int RemoveFile(const std::filesystem::path& path);
    static std::vector<std::filesystem::path> GetFiles();

    // The texture Release hook has only a texture pointer, so it finds its client here.
    static TextureClient* CurrentClient();

private:
    IDirect3DDevice9* D3D9Device;

    // Side-state keyed by real texture pointer: the game's textures vs. our fakes.
    std::recursive_mutex registry_mutex;
    std::unordered_map<IDirect3DBaseTexture9*, TexState*> originals;
    std::unordered_map<IDirect3DBaseTexture9*, TexState*> fakes;

    // Hashed one creation later, once the game has filled it with data.
    IDirect3DBaseTexture9* last_created[3] = {};

    bool loading_fake = false;  // set while LoadTexture() drives CreateTexture internally
    bool shutting_down = false; // set in ~TextureClient so the Release hook stops touching us
    bool should_update = false;

    int AddTexture(IDirect3DBaseTexture9* texture); // hash a freshly filled original + look up its mod
    int LookUpToMod(TexState* state);               // switch in a fake if a mod matches the hash
    int LoadTexture(TextureFileStruct* file_in_memory, IDirect3DBaseTexture9** ppTexture, TexState** ppState);

    static void Switch(TexState* original, TexState* fake);
    static void Unswitch(TexState* original);
    void UnswitchAndRelease(TexState* original); // unswitch + release our fake

    int MergeUpdate();

    int LockMutex();
    int UnlockMutex();
    HANDLE hMutex;

    Guarded<std::vector<PendingOp>> pending_ops;
    void ProcessPendingOps();
    void RemoveModdedTexture(HashType hash);

    std::unordered_map<HashType, gsl::owner<TextureFileStruct*>> modded_textures; // hash -> mod file in memory

    static void LoadStartupModlists();

    // Enqueue an Add per distinct non-zero hash, free the structs, return the hashes
    // in load order; accumulates moved bytes into loaded_bytes when given.
    static std::vector<HashType> IngestModfile(std::vector<gsl::owner<TextureFileStruct*>>& texture_file_structs, uint64_t* loaded_bytes = nullptr);

    // Kept in load order, which is priority order: on a hash collision the
    // earlier file wins (see ProcessPendingOps).
    struct LoadedFile {
        std::filesystem::path path;
        std::vector<HashType> hashes;
    };

    static inline std::mutex global_mutex;
    static inline TextureClient* current_client = nullptr;
    // A vector, not a map: tens of mods at most, and their order is meaningful.
    static inline std::vector<LoadedFile> loaded_files;

    // Caller holds global_mutex; path must already be absolute.
    static std::vector<LoadedFile>::iterator FindLoadedFile(const std::filesystem::path& absolute_path)
    {
        return std::ranges::find(loaded_files, absolute_path, &LoadedFile::path);
    }
};

TextureClient* TextureClient::CurrentClient()
{
    std::lock_guard lk(global_mutex);
    return current_client;
}

TextureClient::TextureClient(IDirect3DDevice9* device)
{
    Message("TextureClient::TextureClient(): %p\n", this);
    D3D9Device = device;

    hMutex = CreateMutex(nullptr, false, nullptr);

    std::lock_guard lk(global_mutex);
    ASSERT(current_client == nullptr); // gMod assumes a single d3d9 device per process
    current_client = this;
}

TextureClient::~TextureClient()
{
    Message("TextureClient::~TextureClient(): %p\n", this);
    {
        std::lock_guard lk(global_mutex);
        if (current_client == this) current_client = nullptr;
    }

    {
        std::lock_guard lk(registry_mutex);
        shutting_down = true; // the texture Release hook now no-ops for our textures

        // Drop side-state but don't Release the textures: the game already released
        // its originals (and our fakes with them), and the device may be gone.
        for (const auto state : fakes | std::views::values) {
            delete state;
        }
        fakes.clear();
        for (const auto state : originals | std::views::values) {
            delete state;
        }
        originals.clear();
    }

    if (hMutex != nullptr) {
        CloseHandle(hMutex);
    }
    for (const auto texture_file_struct : modded_textures | std::views::values) {
        delete texture_file_struct;
    }
    modded_textures.clear();
}

void TextureClient::Switch(TexState* original, TexState* fake)
{
    original->partner = fake;
    fake->partner = original;
}

void TextureClient::Unswitch(TexState* original)
{
    if (original->partner != nullptr) {
        original->partner->partner = nullptr;
        original->partner = nullptr;
    }
}

void TextureClient::UnswitchAndRelease(TexState* original)
{
    TexState* fake = original->partner;
    if (fake == nullptr) return;
    Unswitch(original);                    // detach before releasing so the fake's Release hook won't touch `original`
    if (fake->real) fake->real->Release(); // re-enters OnReleaseTexture(fake), which frees its state
}

TexState* TextureClient::OnCreateTexture(IDirect3DBaseTexture9* texture, TexType type)
{
    std::lock_guard lk(registry_mutex);

    const auto state = new TexState();
    state->real = texture;
    state->device = D3D9Device;
    state->type = type;

    if (loading_fake) {
        // A replacement we're creating: track as a fake, skip the originals bookkeeping.
        state->isFake = true;
        fakes.emplace(texture, state);
        return state;
    }

    originals.emplace(texture, state);

    // The previous texture of this type is now filled, so hash it.
    if (const auto last = last_created[static_cast<int>(type)]) {
        AddTexture(last);
    }
    last_created[static_cast<int>(type)] = texture;
    return state;
}

int TextureClient::AddTexture(IDirect3DBaseTexture9* texture)
{
    const auto it = originals.find(texture);
    if (it == originals.end()) return RETURN_OK;
    const auto state = it->second;

    // No longer pending.
    if (last_created[static_cast<int>(state->type)] == texture) {
        last_created[static_cast<int>(state->type)] = nullptr;
    }

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    state->hash = GetTextureHash(state);
    if (!state->hash) {
        return RETURN_FATAL_ERROR;
    }

    return LookUpToMod(state); // check if this texture should be modded
}

int TextureClient::LookUpToMod(TexState* state)
{
    Message("TextureClient::LookUpToMod( %p): hash: %#lX\n", state->real, state->hash);
    if (state->partner != nullptr)
        return RETURN_OK; // already switched

    auto found = modded_textures.find(state->hash.crc32);
    if (found == modded_textures.end())
        if (found = modded_textures.find(state->hash.crc64), !state->hash.crc64 || found == modded_textures.end())
            return RETURN_OK;

    const auto textureFileStruct = found->second;

    IDirect3DBaseTexture9* fake_texture = nullptr;
    TexState* fake_state = nullptr;
    if (const int ret = LoadTexture(textureFileStruct, &fake_texture, &fake_state); ret != RETURN_OK)
        return ret;

    Switch(state, fake_state);
    fake_state->reference = textureFileStruct;
    return RETURN_OK;
}

int TextureClient::LoadTexture(TextureFileStruct* file_in_memory, IDirect3DBaseTexture9** ppTexture, TexState** ppState)
{
    Message("LoadTexture( %p, %#lX): %p\n", file_in_memory, file_in_memory->crc_hash, this);
    *ppTexture = nullptr;
    *ppState = nullptr;

    // CreateDDSTextureFromMemoryEx calls the hooked CreateTexture; loading_fake
    // makes that hook register the result as a fake, not a game original.
    IDirect3DTexture9* texture = nullptr;
    loading_fake = true;
    const auto ret = DirectX::CreateDDSTextureFromMemoryEx(
        D3D9Device,
        file_in_memory->data.data(),
        file_in_memory->data.size(),
        0, D3DPOOL_MANAGED, false,
        &texture);
    loading_fake = false;

    if (ret != D3D_OK || texture == nullptr) {
        Warning("LoadDDSTexture (%p, %#lX): FAILED ret: \n", file_in_memory->data.data(), file_in_memory->crc_hash, ret);
        return RETURN_TEXTURE_NOT_LOADED;
    }

    const auto fake = static_cast<IDirect3DBaseTexture9*>(texture);
    const auto state_it = fakes.find(fake);
    ASSERT(state_it != fakes.end()); // must have been registered by the CreateTexture hook
    *ppTexture = fake;
    *ppState = state_it->second;

    Message("LoadTexture (%p, %#lX): DONE\n", fake, file_in_memory->crc_hash);
    return RETURN_OK;
}

IDirect3DBaseTexture9* TextureClient::ResolveBinding(IDirect3DBaseTexture9* texture)
{
    if (texture == nullptr) return nullptr;
    std::lock_guard lk(registry_mutex);
    const auto it = originals.find(texture);
    if (it != originals.end() && it->second->partner != nullptr) {
        return it->second->partner->real; // bind the fake in place of the original
    }
    return texture;
}

IDirect3DBaseTexture9* TextureClient::ResolveOriginalFromFake(IDirect3DBaseTexture9* texture)
{
    if (texture == nullptr) return nullptr;
    std::lock_guard lk(registry_mutex);
    const auto it = fakes.find(texture);
    if (it != fakes.end() && it->second->partner != nullptr) {
        return it->second->partner->real;
    }
    return nullptr;
}

void TextureClient::OnBeginScene()
{
    {
        std::lock_guard lk(registry_mutex);
        for (int type = 0; type < 3; ++type) {
            if (last_created[type] != nullptr) {
                AddTexture(last_created[type]); // hashes + clears the slot
            }
        }
    }
    MergeUpdate();
}

void TextureClient::OnUpdateTexture(IDirect3DBaseTexture9* source, IDirect3DBaseTexture9* destination)
{
    // The copy already happened; re-hash both textures and re-run the mod lookup.
    std::lock_guard lk(registry_mutex);

    auto refresh = [this](IDirect3DBaseTexture9* texture) {
        const auto it = originals.find(texture);
        if (it == originals.end()) return;
        const auto state = it->second;
        const auto hash = GetTextureHash(state);
        if (hash == state->hash) return; // unchanged
        state->hash = hash;
        if (state->partner != nullptr) UnswitchAndRelease(state);
        if (hash) LookUpToMod(state);
    };
    refresh(source);
    refresh(destination);
}

void TextureClient::OnReleaseTexture(IDirect3DBaseTexture9* texture)
{
    std::lock_guard lk(registry_mutex);
    if (shutting_down) return; // ~TextureClient is tearing everything down itself

    // A fake of ours being released (either by us during cleanup, or transitively).
    if (const auto fit = fakes.find(texture); fit != fakes.end()) {
        const auto state = fit->second;
        if (state->partner != nullptr) state->partner->partner = nullptr;
        fakes.erase(fit);
        delete state;
        return;
    }

    // A game texture (original) reaching zero references.
    if (const auto oit = originals.find(texture); oit != originals.end()) {
        const auto state = oit->second;
        if (last_created[static_cast<int>(state->type)] == texture) {
            last_created[static_cast<int>(state->type)] = nullptr;
        }
        originals.erase(oit);
        UnswitchAndRelease(state); // release the fake we held for it, if any
        delete state;
    }
}

int TextureClient::MergeUpdate()
{
    const bool has_pending = pending_ops.with([](auto& ops) { return !ops.empty(); });
    if (!should_update && !has_pending) return RETURN_OK;
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_TEXTURE;
        return ret;
    }

    Message("MergeUpdate(): %p\n", this);

    ProcessPendingOps();

    {
        std::lock_guard lk(registry_mutex);
        for (const auto state : originals | std::views::values) {
            if (state->partner == nullptr && state->hash) {
                LookUpToMod(state);
            }
        }
    }

    should_update = false;
    return UnlockMutex();
}

void TextureClient::EnqueueAdd(HashType hash, std::vector<BYTE> data)
{
    if (!hash) return;
    pending_ops.with([&](auto& ops) { ops.push_back(PendingOp{PendingOp::Kind::Add, hash, std::move(data)}); });
}

void TextureClient::EnqueueRemove(HashType hash)
{
    if (!hash) return;
    pending_ops.with([&](auto& ops) { ops.push_back(PendingOp{PendingOp::Kind::Remove, hash, {}}); });
}

void TextureClient::ProcessPendingOps()
{
    std::vector<PendingOp> ops;
    pending_ops.with([&](auto& pending) { ops.swap(pending); });
    for (auto& op : ops) {
        if (op.kind == PendingOp::Kind::Add) {
            if (modded_textures.contains(op.hash)) continue;
            const auto texture_file_struct = new TextureFileStruct();
            texture_file_struct->crc_hash = op.hash;
            texture_file_struct->data = std::move(op.data);
            modded_textures.emplace(op.hash, texture_file_struct);
            should_update = true;
        }
        else {
            RemoveModdedTexture(op.hash);
        }
    }
}

void TextureClient::RemoveModdedTexture(HashType hash)
{
    const auto it = modded_textures.find(hash);
    if (it == modded_textures.end()) return;
    const auto texture_file_struct = it->second;

    {
        std::lock_guard lk(registry_mutex);
        for (const auto original : originals | std::views::values) {
            const auto fake = original->partner;
            if (fake != nullptr && fake->reference == texture_file_struct) {
                UnswitchAndRelease(original);
            }
        }
    }

    modded_textures.erase(it);
    delete texture_file_struct;
}

int TextureClient::LockMutex()
{
    if ((gl_ErrorState & (uMod_ERROR_FATAL | uMod_ERROR_MUTEX))) {
        return RETURN_NO_MUTEX;
    }
    if (WAIT_OBJECT_0 != WaitForSingleObject(hMutex, 100)) {
        return RETURN_MUTEX_LOCK; // waiting 100ms, to wait infinite pass INFINITE
    }
    return RETURN_OK;
}

int TextureClient::UnlockMutex()
{
    if (ReleaseMutex(hMutex) == 0) {
        return RETURN_MUTEX_UNLOCK;
    }
    return RETURN_OK;
}

gsl::owner<TextureFileStruct*> MakeTextureFileStruct(TexEntry& entry, const bool compress)
{
    const auto texture_file_struct = new TextureFileStruct();
    texture_file_struct->crc_hash = entry.crc_hash;
    const auto dds_blob = TextureFunction::ConvertToCompressedDDS(entry, compress);
    texture_file_struct->data.assign(static_cast<BYTE*>(dds_blob.GetBufferPointer()), static_cast<BYTE*>(dds_blob.GetBufferPointer()) + dds_blob.GetBufferSize());
    return texture_file_struct;
}

std::vector<gsl::owner<TextureFileStruct*>> ProcessModfile(const std::filesystem::path& modfile, const bool compress)
{
    const auto hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return {};
    const auto modfile_str = modfile.string();
    Message("Initialize: loading file %s... ", modfile_str.c_str());
    auto file_loader = ModfileLoader(modfile);
    auto entries = file_loader.GetContents();
    if (entries.empty()) {
        Message("No entries found.\n");
        CoUninitialize();
        return {};
    }
    Message("%zu textures... ", entries.size());
    std::vector<gsl::owner<TextureFileStruct*>> texture_file_structs;
    texture_file_structs.reserve(entries.size());
    unsigned file_bytes_loaded = 0;
    for (auto& tpf_entry : entries) {
        const auto texture_file_struct = MakeTextureFileStruct(tpf_entry, compress);
        texture_file_structs.push_back(texture_file_struct);
        file_bytes_loaded += static_cast<unsigned>(texture_file_structs.back()->data.size());
    }
    entries.clear();
    Message("%d bytes loaded.\n", file_bytes_loaded);
    CoUninitialize();
    return texture_file_structs;
}

std::vector<HashType> TextureClient::IngestModfile(std::vector<gsl::owner<TextureFileStruct*>>& texture_file_structs, uint64_t* loaded_bytes)
{
    std::vector<HashType> hashes;
    for (auto* texture_file_struct : texture_file_structs) {
        const auto hash = texture_file_struct->crc_hash;
        if (hash && std::ranges::find(hashes, hash) == hashes.end()) {
            hashes.push_back(hash);
            if (loaded_bytes) *loaded_bytes += texture_file_struct->data.size();
            if (current_client) {
                current_client->EnqueueAdd(hash, std::move(texture_file_struct->data));
            }
        }
        delete texture_file_struct;
    }
    texture_file_structs.clear();
    return hashes;
}

std::vector<std::filesystem::path> TextureClient::GetFiles()
{
    std::lock_guard lk(global_mutex);
    std::vector<std::filesystem::path> result;
    result.reserve(loaded_files.size());
    for (const auto& loaded_file : loaded_files) {
        result.push_back(loaded_file.path);
    }
    return result;
}

int TextureClient::RemoveFile(const std::filesystem::path& path)
{
    const auto absolute_path = std::filesystem::absolute(path);
    std::lock_guard lk(global_mutex);
    const auto it = FindLoadedFile(absolute_path);
    if (it == loaded_files.end()) return RETURN_FILE_NOT_LOADED;
    if (current_client) {
        for (const auto hash : it->hashes) {
            current_client->EnqueueRemove(hash);
        }
    }
    loaded_files.erase(it);
    return RETURN_OK;
}

int TextureClient::AddFile(const std::filesystem::path& path)
{
    const auto absolute_path = std::filesystem::absolute(path);
    {
        std::lock_guard lk(global_mutex);
        if (FindLoadedFile(absolute_path) != loaded_files.end()) return RETURN_EXISTS;
    }
    if (!std::filesystem::exists(absolute_path)) return RETURN_FILE_NOT_LOADED;

    const auto file_size = std::filesystem::file_size(absolute_path);
    auto texture_file_structs = ProcessModfile(absolute_path, file_size > 400'000'000);
    if (texture_file_structs.empty()) return RETURN_FILE_NOT_LOADED;

    std::lock_guard lk(global_mutex);
    // Re-check under lock; another thread may have loaded the same path concurrently.
    if (FindLoadedFile(absolute_path) != loaded_files.end()) {
        for (auto* texture_file_struct : texture_file_structs)
            delete texture_file_struct;
        return RETURN_EXISTS;
    }

    auto hashes = IngestModfile(texture_file_structs);
    if (hashes.empty()) return RETURN_TEXTURE_NOT_LOADED;
    loaded_files.push_back({absolute_path, std::move(hashes)});
    return RETURN_OK;
}

void TextureClient::LoadStartupModlists()
{
    std::locale::global(std::locale(""));
    std::vector<std::filesystem::path> modfiles;
    for (const auto& content : modlists_contents | std::views::values) {
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.starts_with("//") || line.starts_with("#") || line.empty()) {
                continue;
            }
            // Remove newline character
            line.erase(std::ranges::remove(line, '\r').begin(), line.end());
            line.erase(std::ranges::remove(line, '\n').begin(), line.end());
            if (line.empty()) continue;

            auto modfile = std::filesystem::absolute(std::filesystem::path(utils::utf8_to_wstring(line)));
            if (!std::ranges::contains(modfiles, modfile)) {
                modfiles.push_back(std::move(modfile));
            }
        }
    }

    auto total_size = 0ull;
    for (const auto& modfile : modfiles) {
        if (std::filesystem::exists(modfile)) total_size += std::filesystem::file_size(modfile);
    }
    const bool compress = total_size > 400'000'000;

    std::vector<std::future<std::vector<gsl::owner<TextureFileStruct*>>>> futures;
    futures.reserve(modfiles.size());
    for (const auto& modfile : modfiles) {
        futures.emplace_back(std::async(std::launch::async, ProcessModfile, modfile, compress));
    }
    auto loaded_size = 0ull;
    for (size_t i = 0; i < modfiles.size(); ++i) {
        auto texture_file_structs = futures[i].get();

        std::lock_guard lk(global_mutex);
        if (FindLoadedFile(modfiles[i]) != loaded_files.end()) {
            for (const auto* texture_file_struct : texture_file_structs)
                delete texture_file_struct;
            continue;
        }

        auto hashes = IngestModfile(texture_file_structs, &loaded_size);
        if (!hashes.empty()) {
            loaded_files.push_back({modfiles[i], std::move(hashes)});
        }
    }
    Info("LoadStartupModlists: %llu bytes (%llu MB)\n", loaded_size, loaded_size / 1024 / 1024);
}

void TextureClient::Initialize()
{
    const auto t1 = std::chrono::high_resolution_clock::now();
    Info("Initialize: begin\n");

    // AddFile() before the device existed recorded files but couldn't enqueue them
    // (no client yet); enqueue them now.
    for (const auto& path : GetFiles()) {
        if (!std::filesystem::exists(path)) continue;
        const auto file_size = std::filesystem::file_size(path);
        auto texture_file_structs = ProcessModfile(path, file_size > 400'000'000);
        IngestModfile(texture_file_structs);
    }

    LoadStartupModlists();

    const auto t2 = std::chrono::high_resolution_clock::now();
    const auto ms = duration_cast<std::chrono::milliseconds>(t2 - t1);
    Info("Initialize: end, took %d ms\n", ms);
}
