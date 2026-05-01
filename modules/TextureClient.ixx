module;

#include "Main.h"
#include "Error.h"
#include "uMod_IDirect3DTexture9.h"
#include <DDSTextureLoader/DDSTextureLoader9.h>
#include <DirectXTex/DirectXTex.h>
#include <mutex>

export module TextureClient;
import TextureFunction;
import ModfileLoader;

export std::vector<std::pair<std::string, std::string>> modlists_contents;

struct PendingOp {
    enum class Kind { Add, Remove };
    Kind kind;
    HashType hash;
    std::vector<BYTE> data;
};

/*
 *  An object of this class is owned by each d3d9 device.
 *  All other functions are called from the render thread instance of the game itself.
 */
export class TextureClient {
public:
    TextureClient(IDirect3DDevice9* device);
    ~TextureClient();

    int AddTexture(uModTexturePtr auto pTexture);
    int RemoveTexture(uModTexturePtr auto pTexture); //called from  uMod_IDirect3DTexture9::Release()
    int LookUpToMod(uModTexturePtr auto pTexture);
    // called at the end AddTexture(...) and from Device->UpdateTexture(...)

    int MergeUpdate(); //called from uMod_IDirect3DDevice9::BeginScene()
    void Initialize();

    void EnqueueAdd(HashType hash, std::vector<BYTE> data);
    void EnqueueRemove(HashType hash);

    static int AddFile(const std::filesystem::path& path);
    static int RemoveFile(const std::filesystem::path& path);
    static std::vector<std::filesystem::path> GetFiles();

    std::vector<uMod_IDirect3DTexture9*> OriginalTextures;
    // stores the pointer to the uMod_IDirect3DTexture9 objects created by the game
    std::vector<uMod_IDirect3DVolumeTexture9*> OriginalVolumeTextures;
    // stores the pointer to the uMod_IDirect3DVolumeTexture9 objects created by the game
    std::vector<uMod_IDirect3DCubeTexture9*> OriginalCubeTextures;
    // stores the pointer to the uMod_IDirect3DCubeTexture9 objects created by the game

private:
    IDirect3DDevice9* D3D9Device;
    // Cached info about whether this id a dx9ex device or not; used for proxy functions
    bool isDirectXExDevice = false;

    // DX9 proxy functions
    void SetLastCreatedTexture(uMod_IDirect3DTexture9*);
    void SetLastCreatedVolumeTexture(uMod_IDirect3DVolumeTexture9*);
    void SetLastCreatedCubeTexture(uMod_IDirect3DCubeTexture9*);

    bool should_update = false;

    int LockMutex();
    int UnlockMutex();
    HANDLE hMutex;

    std::mutex pending_mutex;
    std::vector<PendingOp> pending_ops;
    void ProcessPendingOps();
    void RemoveModdedTexture(HashType hash);

    std::unordered_map<HashType, gsl::owner<TextureFileStruct*>> modded_textures;
    // array which stores the file in memory and the hash of each texture to be modded

    // called if a target texture is found
    int LoadTexture(TextureFileStruct* file_in_memory, uModTexturePtrPtr auto ppTexture);

    static void LoadStartupModlists();

    static inline std::mutex global_mutex;
    static inline TextureClient* current_client = nullptr;
    static inline std::map<std::filesystem::path, std::vector<HashType>> loaded_files;
};

TextureClient::TextureClient(IDirect3DDevice9* device)
{
    Message("TextureClient::TextureClient(): %p\n", this);
    D3D9Device = device;

    void* cpy;
    isDirectXExDevice = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy) == 0x01000001L;

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
    if (hMutex != nullptr) {
        CloseHandle(hMutex);
    }
    for (const auto texture_file_struct : modded_textures | std::views::values) {
        delete texture_file_struct;
    }
    modded_textures.clear();
}

int TextureClient::MergeUpdate()
{
    bool has_pending;
    {
        std::lock_guard lk(pending_mutex);
        has_pending = !pending_ops.empty();
    }
    if (!should_update && !has_pending) return RETURN_OK;
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_TEXTURE;
        return ret;
    }

    Message("MergeUpdate(): %p\n", this);

    ProcessPendingOps();

    for (const auto pTexture : OriginalTextures) {
        if (pTexture->CrossRef_D3Dtex == nullptr) {
            LookUpToMod(pTexture);
        }
    }
    for (const auto pTexture : OriginalVolumeTextures) {
        if (pTexture->CrossRef_D3Dtex == nullptr) {
            LookUpToMod(pTexture);
        }
    }
    for (const auto pTexture : OriginalCubeTextures) {
        if (pTexture->CrossRef_D3Dtex == nullptr) {
            LookUpToMod(pTexture);
        }
    }

    should_update = false;
    return UnlockMutex();
}

void TextureClient::EnqueueAdd(HashType hash, std::vector<BYTE> data)
{
    if (!hash) return;
    std::lock_guard lk(pending_mutex);
    pending_ops.push_back(PendingOp{PendingOp::Kind::Add, hash, std::move(data)});
}

void TextureClient::EnqueueRemove(HashType hash)
{
    if (!hash) return;
    std::lock_guard lk(pending_mutex);
    pending_ops.push_back(PendingOp{PendingOp::Kind::Remove, hash, {}});
}

void TextureClient::ProcessPendingOps()
{
    std::vector<PendingOp> ops;
    {
        std::lock_guard lk(pending_mutex);
        ops.swap(pending_ops);
    }
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

    auto unswitch_in = [texture_file_struct](auto& vec) {
        for (auto* original_texture : vec) {
            auto* fake_texture = original_texture->CrossRef_D3Dtex;
            if (fake_texture && fake_texture->Reference == texture_file_struct) {
                UnswitchTextures(original_texture);
                fake_texture->Release();
            }
        }
    };
    unswitch_in(OriginalTextures);
    unswitch_in(OriginalVolumeTextures);
    unswitch_in(OriginalCubeTextures);

    modded_textures.erase(it);
    delete texture_file_struct;
}

void TextureClient::SetLastCreatedTexture(uMod_IDirect3DTexture9* texture)
{
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedTexture(texture);
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedTexture(texture);
}

void TextureClient::SetLastCreatedVolumeTexture(uMod_IDirect3DVolumeTexture9* texture)
{
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedVolumeTexture(texture);
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedVolumeTexture(texture);
}

void TextureClient::SetLastCreatedCubeTexture(uMod_IDirect3DCubeTexture9* texture)
{
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedCubeTexture(texture);
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedCubeTexture(texture);
}

int TextureClient::LockMutex()
{
    if ((gl_ErrorState & (uMod_ERROR_FATAL | uMod_ERROR_MUTEX))) {
        return RETURN_NO_MUTEX;
    }
    if (WAIT_OBJECT_0 != WaitForSingleObject(hMutex, 100)) {
        return RETURN_MUTEX_LOCK; //waiting 100ms, to wait infinite pass INFINITE
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

std::vector<std::filesystem::path> TextureClient::GetFiles()
{
    std::lock_guard lk(global_mutex);
    std::vector<std::filesystem::path> result;
    result.reserve(loaded_files.size());
    for (const auto& key : loaded_files | std::views::keys) {
        result.push_back(key);
    }
    return result;
}

int TextureClient::RemoveFile(const std::filesystem::path& path)
{
    const auto absolute_path = std::filesystem::absolute(path);
    std::lock_guard lk(global_mutex);
    const auto it = loaded_files.find(absolute_path);
    if (it == loaded_files.end()) return RETURN_FILE_NOT_LOADED;
    if (current_client) {
        for (const auto hash : it->second) {
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
        if (loaded_files.contains(absolute_path)) return RETURN_EXISTS;
    }
    if (!std::filesystem::exists(absolute_path)) return RETURN_FILE_NOT_LOADED;

    const auto file_size = std::filesystem::file_size(absolute_path);
    const auto texture_file_structs = ProcessModfile(absolute_path, file_size > 400'000'000);
    if (texture_file_structs.empty()) return RETURN_FILE_NOT_LOADED;

    std::lock_guard lk(global_mutex);
    // Re-check under lock; another thread may have loaded the same path concurrently.
    if (loaded_files.contains(absolute_path)) {
        for (auto* texture_file_struct : texture_file_structs) delete texture_file_struct;
        return RETURN_EXISTS;
    }

    std::vector<HashType> hashes;
    for (auto* texture_file_struct : texture_file_structs) {
        const auto hash = texture_file_struct->crc_hash;
        if (hash && std::ranges::find(hashes, hash) == hashes.end()) {
            hashes.push_back(hash);
            if (current_client) {
                current_client->EnqueueAdd(hash, std::move(texture_file_struct->data));
            }
        }
        delete texture_file_struct;
    }

    if (hashes.empty()) return RETURN_TEXTURE_NOT_LOADED;
    loaded_files.emplace(absolute_path, std::move(hashes));
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
        if (loaded_files.contains(modfiles[i])) {
            for (const auto* texture_file_struct : texture_file_structs) delete texture_file_struct;
            continue;
        }

        std::vector<HashType> hashes;
        for (auto* texture_file_struct : texture_file_structs) {
            const auto hash = texture_file_struct->crc_hash;
            if (hash && std::ranges::find(hashes, hash) == hashes.end()) {
                hashes.push_back(hash);
                loaded_size += texture_file_struct->data.size();
                if (current_client) {
                    current_client->EnqueueAdd(hash, std::move(texture_file_struct->data));
                }
            }
            delete texture_file_struct;
        }

        if (!hashes.empty()) {
            loaded_files.emplace(modfiles[i], std::move(hashes));
        }
    }
    Info("LoadStartupModlists: %llu bytes (%llu MB)\n", loaded_size, loaded_size / 1024 / 1024);
}

void TextureClient::Initialize()
{
    const auto t1 = std::chrono::high_resolution_clock::now();
    Info("Initialize: begin\n");

    // now load files that were added by AddFile() before the device existed
    std::vector<std::filesystem::path> pre_existing_paths;
    {
        std::lock_guard lk(global_mutex);
        pre_existing_paths.reserve(loaded_files.size());
        for (const auto& key : loaded_files | std::views::keys) pre_existing_paths.push_back(key);
    }
    for (const auto& path : pre_existing_paths) {
        if (!std::filesystem::exists(path)) continue;
        const auto file_size = std::filesystem::file_size(path);
        auto texture_file_structs = ProcessModfile(path, file_size > 400'000'000);
        std::vector<HashType> seen;
        for (auto* texture_file_struct : texture_file_structs) {
            const auto hash = texture_file_struct->crc_hash;
            if (hash && std::ranges::find(seen, hash) == seen.end()) {
                seen.push_back(hash);
                EnqueueAdd(hash, std::move(texture_file_struct->data));
            }
            delete texture_file_struct;
        }
    }

    LoadStartupModlists();

    const auto t2 = std::chrono::high_resolution_clock::now();
    const auto ms = duration_cast<std::chrono::milliseconds>(t2 - t1);
    Info("Initialize: end, took %d ms\n", ms);
}

int TextureClient::AddTexture(uModTexturePtr auto pTexture)
{
    if constexpr (std::same_as<decltype(pTexture), uMod_IDirect3DTexture9*>) {
        SetLastCreatedTexture(nullptr);
    }
    else if constexpr (std::same_as<decltype(pTexture), uMod_IDirect3DVolumeTexture9*>) {
        SetLastCreatedVolumeTexture(nullptr);
    }
    else if constexpr (std::same_as<decltype(pTexture), uMod_IDirect3DCubeTexture9*>) {
        SetLastCreatedCubeTexture(nullptr);
    }

    if (pTexture->FAKE) {
        return RETURN_OK; // this is a fake texture
    }

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    Message("TextureClient::AddTexture( Cube: %p): %p (thread: %u)\n", pTexture, this, GetCurrentThreadId());

    pTexture->Hash = pTexture->GetHash();
    if (!pTexture->Hash) {
        return RETURN_FATAL_ERROR;
    }

    if constexpr (std::same_as<decltype(pTexture), uMod_IDirect3DTexture9*>) {
        OriginalTextures.push_back(pTexture);
    }
    else if constexpr (std::same_as<decltype(pTexture), uMod_IDirect3DVolumeTexture9*>) {
        OriginalVolumeTextures.push_back(pTexture);
    }
    else if constexpr (std::same_as<decltype(pTexture), uMod_IDirect3DCubeTexture9*>) {
        OriginalCubeTextures.push_back(pTexture);
    }

    return LookUpToMod(pTexture); // check if this texture should be modded
}

int TextureClient::RemoveTexture(uModTexturePtr auto pTexture)
{
    Message("TextureClient::RemoveTexture( %p, %#lX): %p\n", pTexture, pTexture->Hash, this);

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    if (!pTexture->FAKE) {
        if constexpr (std::same_as<decltype(pTexture), uMod_IDirect3DTexture9*>) {
            utils::erase_first(OriginalTextures, pTexture);
        }
        else if constexpr (std::same_as<decltype(pTexture), uMod_IDirect3DVolumeTexture9*>) {
            utils::erase_first(OriginalVolumeTextures, pTexture);
        }
        else if constexpr (std::same_as<decltype(pTexture), uMod_IDirect3DCubeTexture9*>) {
            utils::erase_first(OriginalCubeTextures, pTexture);
        }
    }
    if (!pTexture->Reference)
        return RETURN_OK; // Should this ever happen?
    return RETURN_OK;
}

int TextureClient::LookUpToMod(uModTexturePtr auto pTexture)
{
    Message("TextureClient::LookUpToMod( %p): hash: %#lX\n", pTexture, pTexture->Hash);
    int ret = RETURN_OK;

    if (pTexture->CrossRef_D3Dtex != nullptr)
        return ret; // bug, this texture is already switched

    auto found = modded_textures.find(pTexture->Hash.crc32);
    if (found == modded_textures.end())
        if (found = modded_textures.find(pTexture->Hash.crc64), !pTexture->Hash.crc64 || found == modded_textures.end())
            return ret;

    const auto textureFileStruct = found->second;

    decltype(pTexture) fake_texture;
    ret = LoadTexture(textureFileStruct, &fake_texture);
    if (ret != RETURN_OK)
        return ret;

    ret = SwitchTextures(fake_texture, pTexture);
    if (ret != RETURN_OK) {
        Message("TextureClient::LookUpToMod(): textures not switched %#lX\n", textureFileStruct->crc_hash);
        fake_texture->Release();
        return ret;
    }
    fake_texture->Reference = textureFileStruct;
    return ret;
}

int TextureClient::LoadTexture(TextureFileStruct* file_in_memory, uModTexturePtrPtr auto ppTexture)
{
    Message("LoadTexture( %p, %p, %#lX): %p\n", file_in_memory, ppTexture, file_in_memory->crc_hash, this);
    if (const auto ret = DirectX::CreateDDSTextureFromMemoryEx(
        D3D9Device,
        file_in_memory->data.data(),
        file_in_memory->data.size(),
        0, D3DPOOL_MANAGED, false,
        reinterpret_cast<LPDIRECT3DTEXTURE9*>(ppTexture)); ret != D3D_OK) {
        *ppTexture = nullptr;
        Warning("LoadDDSTexture (%p, %#lX): FAILED ret: \n", file_in_memory->data.data(), file_in_memory->crc_hash, ret);
        return RETURN_TEXTURE_NOT_LOADED;
    }
    if constexpr (std::same_as<decltype(ppTexture), uMod_IDirect3DTexture9**>) {
        SetLastCreatedTexture(nullptr);
    }
    else if constexpr (std::same_as<decltype(ppTexture), uMod_IDirect3DVolumeTexture9**>) {
        SetLastCreatedVolumeTexture(nullptr);
    }
    else if constexpr (std::same_as<decltype(ppTexture), uMod_IDirect3DCubeTexture9**>) {
        SetLastCreatedCubeTexture(nullptr);
    }
    (*ppTexture)->FAKE = true;

    Message("LoadTexture (%p, %#lX): DONE\n", *ppTexture, file_in_memory->crc_hash);
    return RETURN_OK;
}
