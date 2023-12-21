module;

#include "Main.h"
#include "Error.h"
#include "uMod_IDirect3DTexture9.h"
#include <DDSTextureLoader/DDSTextureLoader9.h>
#include <DirectXTex/DirectXTex.h>

export module TextureClient;
import TextureFunction;
import ModfileLoader;

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

    std::unordered_map<HashType, gsl::owner<TextureFileStruct*>> modded_textures;
    // array which stores the file in memory and the hash of each texture to be modded

    // called if a target texture is found
    int LoadTexture(TextureFileStruct* file_in_memory, uModTexturePtrPtr auto ppTexture);

    void LoadModsFromFile(const char* source);
    std::filesystem::path exe_path; // path to gw.exe
    std::filesystem::path dll_path; // path to gmod dll
};

TextureClient::TextureClient(IDirect3DDevice9* device)
{
    Message("TextureClient::TextureClient(): %p\n", this);
    D3D9Device = device;

    void* cpy;
    isDirectXExDevice = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy) == 0x01000001L;

    hMutex = CreateMutex(nullptr, false, nullptr);
}

TextureClient::~TextureClient()
{
    Message("TextureClient::~TextureClient(): %p\n", this);
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
    if (!should_update) return RETURN_OK;
    should_update = false;
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_TEXTURE;
        return ret;
    }

    Message("MergeUpdate(): %p\n", this);

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

    return UnlockMutex();
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

gsl::owner<TextureFileStruct*> AddFile(TexEntry& entry, const bool compress, const std::filesystem::path& dll_path)
{
    const auto texture_file_struct = new TextureFileStruct();
    texture_file_struct->crc_hash = entry.crc_hash;
    const auto dds_blob = TextureFunction::ConvertToCompressedDDS(entry, compress, dll_path);
    texture_file_struct->data.assign(static_cast<BYTE*>(dds_blob.GetBufferPointer()), static_cast<BYTE*>(dds_blob.GetBufferPointer()) + dds_blob.GetBufferSize());
    return texture_file_struct;
}

std::vector<gsl::owner<TextureFileStruct*>> ProcessModfile(const std::filesystem::path& modfile, const std::filesystem::path& dll_path, const bool compress)
{
    const auto hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return {};
    Message("Initialize: loading file %s... ", modfile.c_str());
    auto file_loader = ModfileLoader(modfile);
    auto entries = file_loader.GetContents();
    if (entries.empty()) {
        Message("No entries found.\n");
        return {};
    }
    Message("%zu textures... ", entries.size());
    std::vector<gsl::owner<TextureFileStruct*>> texture_file_structs;
    texture_file_structs.reserve(entries.size());
    unsigned file_bytes_loaded = 0;
    for (auto& tpf_entry : entries) {
        const auto tex_file_struct = AddFile(tpf_entry, compress, dll_path);
        texture_file_structs.push_back(tex_file_struct);
        file_bytes_loaded += texture_file_structs.back()->data.size();
    }
    entries.clear();
    Message("%d bytes loaded.\n", file_bytes_loaded);
    CoUninitialize();
    return texture_file_structs;
}

void TextureClient::LoadModsFromFile(const char* source)
{
    static std::vector<std::filesystem::path> loaded_modfiles{};
    Message("Initialize: searching in %s\n", source);

    std::locale::global(std::locale(""));
    std::ifstream file(source, std::ios::binary);
    if (!file.is_open()) {
        Warning("LoadModsFromFile: failed to open modlist.txt for reading; aborting!!!");
        return;
    }
    Message("Initialize: found modlist.txt. Reading\n");
    std::string line;
    std::vector<std::filesystem::path> modfiles;
    while (std::getline(file, line)) {
        // Remove newline character
        line.erase(std::ranges::remove(line, '\r').begin(), line.end());
        line.erase(std::ranges::remove(line, '\n').begin(), line.end());

        const auto wline = utils::utf8_to_wstring(line);
        const auto fsline = std::filesystem::path(wline);

        if (!std::ranges::contains(loaded_modfiles, fsline)) {
            modfiles.push_back(fsline);
            loaded_modfiles.push_back(fsline);
        }
    }
    auto files_size = 0u;
    for (const auto modfile : modfiles) {
        if (std::filesystem::exists(modfile)) {
            files_size += std::filesystem::file_size(modfile);
        }
    }
    std::vector<std::future<std::vector<gsl::owner<TextureFileStruct*>>>> futures;
    for (const auto modfile : modfiles) {
        futures.emplace_back(std::async(std::launch::async, ProcessModfile, modfile, dll_path, files_size > 400'000'000));
    }
    auto loaded_size = 0u;
    for (auto& future : futures) {
        const auto texture_file_structs = future.get();
        for (const auto texture_file_struct : texture_file_structs) {
            if (!texture_file_struct->crc_hash) continue;
            if (!modded_textures.contains(texture_file_struct->crc_hash)) {
                modded_textures.emplace(texture_file_struct->crc_hash, texture_file_struct);
                loaded_size += texture_file_struct->data.size();
            }
            else {
                delete texture_file_struct;
            }
        }
        should_update = true;
    }
    Message("Finished loading mods from %s: Loaded %u bytes (%u mb)", source, loaded_size, loaded_size / 1024 / 1024);
}

void TextureClient::Initialize()
{
    const auto t1 = std::chrono::high_resolution_clock::now();
    Info("Initialize: begin\n");
    Message("Initialize: searching for modlist.txt\n");
    char gwpath[MAX_PATH]{};
    GetModuleFileName(GetModuleHandle(nullptr), gwpath, MAX_PATH); //ask for name and path of this executable
    char dllpath[MAX_PATH]{};
    GetModuleFileName(gl_hThisInstance, dllpath, MAX_PATH); //ask for name and path of this dll
    exe_path = std::filesystem::path(gwpath).parent_path();
    dll_path = std::filesystem::path(dllpath).parent_path();
    for (const auto& path : {exe_path, dll_path}) {
        const auto modlist = path / "modlist.txt";
        if (std::filesystem::exists(modlist)) {
            Message("Initialize: found %s\n", modlist.string().c_str());
            LoadModsFromFile(modlist.string().c_str());
        }
    }
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
    Message("TextureClient::LookUpToMod( %p): hash: %#lX,  %p\n", pTexture, pTexture->Hash, this);
    int ret = RETURN_OK;

    if (pTexture->CrossRef_D3Dtex != nullptr)
        return ret; // bug, this texture is already switched

    const auto found = modded_textures.find(pTexture->Hash);
    if (found == modded_textures.end())
        return ret;

    const auto textureFileStruct = found->second;

    decltype(pTexture) fake_Texture;
    ret = LoadTexture(textureFileStruct, &fake_Texture);
    if (ret != RETURN_OK)
        return ret;

    ret = SwitchTextures(fake_Texture, pTexture);
    if (ret != RETURN_OK) {
        Message("TextureClient::LookUpToMod(): textures not switched %#lX\n", textureFileStruct->crc_hash);
        fake_Texture->Release();
        return ret;
    }
    fake_Texture->Reference = textureFileStruct;
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
