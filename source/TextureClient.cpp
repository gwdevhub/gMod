#include <filesystem>
#include <fstream>

#include "Main.h"

TextureClient::TextureClient(IDirect3DDevice9* device)
{
    Message("TextureClient::TextureClient(): %p\n", this);
    D3D9Device = device;

    void* cpy;
    isDirectXExDevice = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy) == 0x01000001L;

    Mutex = CreateMutex(nullptr, false, nullptr);

    FontColour = D3DCOLOR_ARGB(255, 255, 0, 0);
    TextureColour = D3DCOLOR_ARGB(255, 0, 255, 0);
}

TextureClient::~TextureClient()
{
    Message("TextureClient::~TextureClient(): %p\n", this);
    if (Mutex != nullptr) {
        CloseHandle(Mutex);
    }
    for (const auto& it : modded_textures) {
        delete it.second;
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

    const auto single_texture = GetSingleTexture();
    for (const auto pTexture : OriginalTextures) {
        if (pTexture->CrossRef_D3Dtex == nullptr || pTexture->CrossRef_D3Dtex == single_texture) {
            UnswitchTextures(pTexture); //this we can do always, so we unswitch the single texture
            LookUpToMod(pTexture);
        }
    }
    const auto single_volume_texture = GetSingleVolumeTexture();
    for (const auto pTexture : OriginalVolumeTextures) {
        if (pTexture->CrossRef_D3Dtex == nullptr || pTexture->CrossRef_D3Dtex == single_volume_texture) {
            UnswitchTextures(pTexture); //this we can do always, so we unswitch the single texture
            LookUpToMod(pTexture);
        }
    }
    const auto single_cube_texture = GetSingleCubeTexture();
    for (const auto pTexture : OriginalCubeTextures) {
        if (pTexture->CrossRef_D3Dtex == nullptr || pTexture->CrossRef_D3Dtex == single_cube_texture) {
            UnswitchTextures(pTexture); //this we can do always, so we unswitch the single texture
            LookUpToMod(pTexture);
        }
    }

    return UnlockMutex();
}

uMod_IDirect3DTexture9* TextureClient::GetSingleTexture()
{
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->GetSingleTexture();
    //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->GetSingleTexture();
}

uMod_IDirect3DVolumeTexture9* TextureClient::GetSingleVolumeTexture()
{
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->GetSingleVolumeTexture();
    //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->GetSingleVolumeTexture();
}

uMod_IDirect3DCubeTexture9* TextureClient::GetSingleCubeTexture()
{
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->GetSingleCubeTexture();
    //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->GetSingleCubeTexture();
}

int TextureClient::SetLastCreatedTexture(uMod_IDirect3DTexture9* texture)
{
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedTexture(texture);
    //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedTexture(texture);
}

int TextureClient::SetLastCreatedVolumeTexture(uMod_IDirect3DVolumeTexture9* texture)
{
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedVolumeTexture(texture);
    //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedVolumeTexture(texture);
}

int TextureClient::SetLastCreatedCubeTexture(uMod_IDirect3DCubeTexture9* texture)
{
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedCubeTexture(texture);
    //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedCubeTexture(texture);
}


int TextureClient::LockMutex()
{
    if ((gl_ErrorState & (uMod_ERROR_FATAL | uMod_ERROR_MUTEX))) {
        return RETURN_NO_MUTEX;
    }
    if (WAIT_OBJECT_0 != WaitForSingleObject(Mutex, 100)) {
        return RETURN_MUTEX_LOCK; //waiting 100ms, to wait infinite pass INFINITE
    }
    return RETURN_OK;
}

int TextureClient::UnlockMutex()
{
    if (ReleaseMutex(Mutex) == 0) {
        return RETURN_MUTEX_UNLOCK;
    }
    return RETURN_OK;
}

unsigned long TextureClient::AddFile(TextureFileStruct& entry)
{
    if (modded_textures.contains(entry.crc_hash)) {
        return 0;
    }
    TextureFileStruct* texture_file_struct = new TextureFileStruct();
    texture_file_struct->data = std::move(entry.data);
    texture_file_struct->crc_hash = entry.crc_hash;
    modded_textures.emplace(entry.crc_hash, texture_file_struct);
    should_update = true;
    return texture_file_struct->data.size();
}

void TextureClient::LoadModsFromFile(const char* source)
{
    static unsigned long loaded_size = 0;
    Message("Initialize: searching in %s\n", source);

    std::ifstream file(source);
    if (!file.is_open()) {
        Message("LoadModsFromFile: failed to open modlist.txt for reading; aborting!!!");
        return;
    }
    Message("Initialize: found modlist.txt. Reading\n");
    std::string line;
    while (std::getline(file, line)) {
        Message("Initialize: loading file %s... ", line.c_str());

        // Remove newline character
        line.erase(std::ranges::remove(line, '\n').begin(), line.end());

        auto file_loader = FileLoader(line);
        auto entries = file_loader.GetContents();
        if (loaded_size > 1'500'000'000) {
            Message("LoadModsFromFile: Loaded %d bytes, aborting!!!\n", loaded_size);
            return;
        }
        if (entries.empty()) {
            Message("No entries found.\n");
            continue;
        }
        Message("%zu textures... ", entries.size());
        unsigned long file_bytes_loaded = 0;
        for (auto& tpf_entry : entries) {
            file_bytes_loaded += AddFile(tpf_entry);
        }
        entries.clear();
        Message("%d bytes loaded.\n", file_bytes_loaded);
        loaded_size += file_bytes_loaded;
    }
    Message("Finished loading mods from %s: Loaded %u bytes (%u mb)", source, loaded_size, loaded_size / 1024 / 1024);
}

void TextureClient::Initialize()
{
    Message("Initialize: begin\n");
    Message("Initialize: searching for modlist.txt\n");
    char gwpath[MAX_PATH]{};
    GetModuleFileName(GetModuleHandle(nullptr), gwpath, MAX_PATH); //ask for name and path of this executable
    char dllpath[MAX_PATH]{};
    GetModuleFileName(gl_hThisInstance, dllpath, MAX_PATH); //ask for name and path of this dll
    const auto exe = std::filesystem::path(gwpath).parent_path();
    const auto dll = std::filesystem::path(dllpath).parent_path();
    for (const auto& path : {exe, dll}) {
        const auto modlist = path / "modlist.txt";
        if (std::filesystem::exists(modlist)) {
            Message("Initialize: found %s\n", modlist.string().c_str());
            LoadModsFromFile(modlist.string().c_str());
        }
    }

    Message("Initialize: end\n");
}
