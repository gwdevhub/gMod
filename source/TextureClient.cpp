#include <filesystem>
#include <fstream>

#include "Main.h"

TextureClient::TextureClient(IDirect3DDevice9* device)
{
    Message("uMod_TextureClient::uMod_TextureClient(): %p\n", this);
    D3D9Device = device;

    void* cpy;
    isDirectXExDevice = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy) == 0x01000001L;

    Mutex = CreateMutex(nullptr, false, nullptr);

    FontColour = D3DCOLOR_ARGB(255, 255, 0, 0);
    TextureColour = D3DCOLOR_ARGB(255, 0, 255, 0);
}

TextureClient::~TextureClient()
{
    Message("uMod_TextureClient::~uMod_TextureClient(): %p\n", this);
    if (Mutex != nullptr) {
        CloseHandle(Mutex);
    }
    for (auto it : modded_textures) {
        delete it.second;
    }
    modded_textures.clear();
}


int TextureClient::AddTexture(uMod_IDirect3DTexture9* pTexture)
{
    SetLastCreatedTexture(nullptr);

    if (pTexture->FAKE) {
        return RETURN_OK; // this is a fake texture
    }

    Message("uMod_TextureClient::AddTexture( %p): %p (thread: %p)\n", pTexture, this, GetCurrentThreadId());

    pTexture->Hash = pTexture->GetHash();
    if (!pTexture->Hash) {
        return RETURN_FATAL_ERROR;
    }

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    OriginalTextures.push_back(pTexture); // add the texture to the list of original texture

    return LookUpToMod(pTexture); // check if this texture should be modded
}

int TextureClient::AddTexture(uMod_IDirect3DVolumeTexture9* pTexture)
{
    SetLastCreatedVolumeTexture(nullptr);

    if (pTexture->FAKE) {
        return RETURN_OK; // this is a fake texture
    }

    Message("uMod_TextureClient::AddTexture( Volume: %p): %p (thread: %p)\n", pTexture, this, GetCurrentThreadId());

    pTexture->Hash = pTexture->GetHash();
    if (!pTexture->Hash) {
        return RETURN_FATAL_ERROR;
    }

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    OriginalVolumeTextures.push_back(pTexture); // add the texture to the list of original texture

    return LookUpToMod(pTexture); // check if this texture should be modded
}

int TextureClient::AddTexture(uMod_IDirect3DCubeTexture9* pTexture)
{
    SetLastCreatedCubeTexture(nullptr);

    if (pTexture->FAKE) {
        return RETURN_OK; // this is a fake texture
    }

    Message("uMod_TextureClient::AddTexture( Cube: %p): %p (thread: %p)\n", pTexture, this, GetCurrentThreadId());

    pTexture->Hash = pTexture->GetHash();
    if (!pTexture->Hash) {
        return RETURN_FATAL_ERROR;
    }

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    OriginalCubeTextures.push_back(pTexture); // add the texture to the list of original texture

    return LookUpToMod(pTexture); // check if this texture should be modded
}


int TextureClient::RemoveTexture(uMod_IDirect3DTexture9* pTexture) // is called from a texture, if it is finally released
{
    Message("uMod_TextureClient::RemoveTexture( %p, %#lX): %p\n", pTexture, pTexture->Hash, this);

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }
    if (!pTexture->FAKE)
        utils::erase_first(OriginalTextures, pTexture);
    if (!pTexture->Reference)
        return RETURN_OK; // Should this ever happen?
    utils::erase_first(pTexture->Reference->Textures, static_cast<IDirect3DBaseTexture9*>(pTexture));
    return RETURN_OK;
}

int TextureClient::RemoveTexture(uMod_IDirect3DVolumeTexture9* pTexture) // is called from a texture, if it is finally released
{
    Message("uMod_TextureClient::RemoveTexture( Volume %p, %#lX): %p\n", pTexture, pTexture->Hash, this);

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }
    if (!pTexture->FAKE)
        utils::erase_first(OriginalVolumeTextures, pTexture);
    if (!pTexture->Reference)
        return RETURN_OK; // Should this ever happen?
    utils::erase_first(pTexture->Reference->Textures, static_cast<IDirect3DBaseTexture9*>(pTexture));
    return RETURN_OK;
}

int TextureClient::RemoveTexture(uMod_IDirect3DCubeTexture9* pTexture) // is called from a texture, if it is finally released
{
    Message("uMod_TextureClient::RemoveTexture( Cube %p, %#lX): %p\n", pTexture, pTexture->Hash, this);

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }
    if (!pTexture->FAKE)
        utils::erase_first(OriginalCubeTextures, pTexture);
    if (!pTexture->Reference)
        return RETURN_OK; // Should this ever happen?
    utils::erase_first(pTexture->Reference->Textures, static_cast<IDirect3DBaseTexture9*>(pTexture));
    return RETURN_OK;
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
    for (auto pTexture : OriginalTextures) {
        if (pTexture->CrossRef_D3Dtex == nullptr || pTexture->CrossRef_D3Dtex == single_texture) {
            UnswitchTextures(pTexture); //this we can do always, so we unswitch the single texture
            LookUpToMod(pTexture);
        }
    }
    const auto single_volume_texture = GetSingleVolumeTexture();
    for (auto pTexture : OriginalVolumeTextures) {
        if (pTexture->CrossRef_D3Dtex == nullptr || pTexture->CrossRef_D3Dtex == single_volume_texture) {
            UnswitchTextures(pTexture); //this we can do always, so we unswitch the single texture
            LookUpToMod(pTexture);
        }
    }
    const auto single_cube_texture = GetSingleCubeTexture();
    for (auto pTexture : OriginalCubeTextures) {
        if (pTexture->CrossRef_D3Dtex == nullptr || pTexture->CrossRef_D3Dtex == single_cube_texture) {
            UnswitchTextures(pTexture); //this we can do always, so we unswitch the single texture
            LookUpToMod(pTexture);
        }
    }

    return UnlockMutex();
}

uMod_IDirect3DTexture9* TextureClient::GetSingleTexture() {
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->GetSingleTexture(); //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->GetSingleTexture();
}
uMod_IDirect3DVolumeTexture9* TextureClient::GetSingleVolumeTexture() {
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->GetSingleVolumeTexture(); //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->GetSingleVolumeTexture();
}
uMod_IDirect3DCubeTexture9* TextureClient::GetSingleCubeTexture() {
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->GetSingleCubeTexture(); //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->GetSingleCubeTexture();
}
int TextureClient::SetLastCreatedTexture(uMod_IDirect3DTexture9* texture) {
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedTexture(texture); //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedTexture(texture);
}
int TextureClient::SetLastCreatedVolumeTexture(uMod_IDirect3DVolumeTexture9* texture) {
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedVolumeTexture(texture); //this texture must no be added twice
    return static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedVolumeTexture(texture);
}
int TextureClient::SetLastCreatedCubeTexture(uMod_IDirect3DCubeTexture9* texture) {
    if (isDirectXExDevice)
        return static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedCubeTexture(texture); //this texture must no be added twice
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


TextureFileStruct* TextureClient::LookUpToMod(HashType hash)
{
    const auto found = modded_textures.find(hash);
    return found == modded_textures.end() ? nullptr : found->second;
}

int TextureClient::LookUpToMod(uMod_IDirect3DTexture9* pTexture)
{
    Message("uMod_TextureClient::LookUpToMod( %p): hash: %#lX,  %p\n", pTexture, pTexture->Hash, this);
    int ret = RETURN_OK;

    if (pTexture->CrossRef_D3Dtex != nullptr)
        return ret; // bug, this texture is already switched

    const auto found = modded_textures.find(pTexture->Hash);
    if (found == modded_textures.end())
        return ret;

    const auto textureFileStruct = found->second;

    uMod_IDirect3DTexture9* fake_Texture;
    ret = LoadTexture(textureFileStruct, &fake_Texture);
    if (ret != RETURN_OK)
        return ret;

    ret = SwitchTextures(fake_Texture, pTexture);
    if (ret != RETURN_OK) {
        Message("uMod_TextureClient::LookUpToMod(): textures not switched %#lX\n", textureFileStruct->crc_hash);
        fake_Texture->Release();
        return ret;
    }
    textureFileStruct->Textures.push_back(fake_Texture);
    fake_Texture->Reference = textureFileStruct;
    return ret;
}

int TextureClient::LookUpToMod(uMod_IDirect3DVolumeTexture9* pTexture) // should only be called for original textures
{
    Message("uMod_TextureClient::LookUpToMod(Volume %p): hash: %#lX,  %p\n", pTexture, pTexture->Hash, this);
    int ret = RETURN_OK;

    if (pTexture->CrossRef_D3Dtex != nullptr)
        return ret; // bug, this texture is already switched

    const auto found = modded_textures.find(pTexture->Hash);
    if (found == modded_textures.end())
        return ret;

    const auto textureFileStruct = found->second;

    uMod_IDirect3DVolumeTexture9* fake_Texture;
    ret = LoadTexture(textureFileStruct, &fake_Texture);
    if (ret != RETURN_OK)
        return ret;

    ret = SwitchTextures(fake_Texture, pTexture);
    if (ret != RETURN_OK) {
        Message("uMod_TextureClient::LookUpToMod(): textures not switched %#lX\n", textureFileStruct->crc_hash);
        fake_Texture->Release();
        return ret;
    }
    textureFileStruct->Textures.push_back(fake_Texture);
    return ret;
}

int TextureClient::LookUpToMod(uMod_IDirect3DCubeTexture9* pTexture) // should only be called for original textures
{
    Message("uMod_TextureClient::LookUpToMod(Cube %p): hash: %#lX,  %p\n", pTexture, pTexture->Hash, this);
    int ret = RETURN_OK;

    if (pTexture->CrossRef_D3Dtex != nullptr)
        return ret; // bug, this texture is already switched

    const auto found = modded_textures.find(pTexture->Hash);
    if (found == modded_textures.end())
        return ret;

    const auto textureFileStruct = found->second;

    uMod_IDirect3DCubeTexture9* fake_Texture;
    ret = LoadTexture(textureFileStruct, &fake_Texture);
    if (ret != RETURN_OK)
        return ret;

    ret = SwitchTextures(fake_Texture, pTexture);
    if (ret != RETURN_OK) {
        Message("uMod_TextureClient::LookUpToMod(): textures not switched %#lX\n", textureFileStruct->crc_hash);
        fake_Texture->Release();
        return ret;
    }
    textureFileStruct->Textures.push_back(fake_Texture);
    return ret;
}

int TextureClient::LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DTexture9** ppTexture) // to load fake texture from a file in memory
{
    Message("LoadTexture( %p, %p, %#lX): %p\n", file_in_memory, ppTexture, file_in_memory->crc_hash, this);
    if (D3D_OK != D3DXCreateTextureFromFileInMemoryEx(D3D9Device, file_in_memory->data.data(), file_in_memory->data.size(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr,
                                                      (IDirect3DTexture9**)ppTexture))
    //if (D3D_OK != D3DXCreateTextureFromFileInMemory( D3D9Device, file_in_memory->pData, file_in_memory->Size, (IDirect3DTexture9 **) ppTexture))
    {
        *ppTexture = nullptr;
        return RETURN_TEXTURE_NOT_LOADED;
    }
    (*ppTexture)->FAKE = true;

    SetLastCreatedTexture(nullptr);

    Message("LoadTexture( %p, %#lX): DONE\n", *ppTexture, file_in_memory->crc_hash);
    return RETURN_OK;
}

int TextureClient::LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DVolumeTexture9** ppTexture) // to load fake texture from a file in memory
{
    Message("LoadTexture( Volume %p, %p, %#lX): %p\n", file_in_memory, ppTexture, file_in_memory->crc_hash, this);
    if (D3D_OK != D3DXCreateVolumeTextureFromFileInMemoryEx(D3D9Device, file_in_memory->data.data(), file_in_memory->data.size(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                                            nullptr,
                                                            nullptr,
                                                            (IDirect3DVolumeTexture9**)ppTexture))
    //if (D3D_OK != D3DXCreateVolumeTextureFromFileInMemory( D3D9Device, file_in_memory->pData, file_in_memory->Size, (IDirect3DVolumeTexture9 **) ppTexture))
    {
        *ppTexture = nullptr;
        return RETURN_TEXTURE_NOT_LOADED;
    }
    (*ppTexture)->FAKE = true;

    SetLastCreatedVolumeTexture(nullptr);

    Message("LoadTexture( Volume %p, %#lX): DONE\n", *ppTexture, file_in_memory->crc_hash);
    return RETURN_OK;
}

int TextureClient::LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DCubeTexture9** ppTexture) // to load fake texture from a file in memory
{
    Message("LoadTexture( Cube %p, %p, %#lX): %p\n", file_in_memory, ppTexture, file_in_memory->crc_hash, this);
    if (D3D_OK != D3DXCreateCubeTextureFromFileInMemoryEx(D3D9Device, file_in_memory->data.data(), file_in_memory->data.size(), D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr,
                                                          (IDirect3DCubeTexture9**)ppTexture))
    //if (D3D_OK != D3DXCreateCubeTextureFromFileInMemory( D3D9Device, file_in_memory->pData, file_in_memory->Size, (IDirect3DCubeTexture9 **) ppTexture))
    {
        *ppTexture = nullptr;
        return RETURN_TEXTURE_NOT_LOADED;
    }
    (*ppTexture)->FAKE = true;

    SetLastCreatedCubeTexture(nullptr);

    Message("LoadTexture( Cube %p, %#lX): DONE\n", *ppTexture, file_in_memory->crc_hash);
    return RETURN_OK;
}

unsigned long TextureClient::AddFile(TpfEntry& entry)
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

unsigned long loaded_size = 0;

void TextureClient::LoadModsFromFile(const char* source)
{
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
    Message("Finished loading mods: Loaded %u bytes (%u mb)", loaded_size, loaded_size / 1024 / 1024);
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
