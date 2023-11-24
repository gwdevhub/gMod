#pragma once

#include <map>

#include "FileLoader.h"
#include "uMod_IDirect3DTexture9.h"
#include <DDSTextureLoader/DDSTextureLoader9.h>
#include <WICTextureLoader/WICTextureLoader9.h>
#include <d3dx9.h>

extern unsigned int gl_ErrorState;

struct TextureFileStruct {
    std::vector<BYTE> data{};
    HashType crc_hash = 0; // hash value
    bool is_wic_texture = false;
};

template <typename T>
concept uModTexturePtr = requires(T a)
{
    std::same_as<uMod_IDirect3DTexture9*, T> ||
    std::same_as<uMod_IDirect3DVolumeTexture9*, T> ||
    std::same_as<uMod_IDirect3DCubeTexture9*, T>;
};

template <typename T>
concept uModTexturePtrPtr = uModTexturePtr<std::remove_pointer_t<T>>;

/*
 *  An object of this class is owned by each d3d9 device.
 *  All other functions are called from the render thread instance of the game itself.
 */
class TextureClient {
public:
    TextureClient(IDirect3DDevice9* device);
    ~TextureClient();

    int AddTexture(uModTexturePtr auto pTexture);
    int RemoveTexture(uModTexturePtr auto pTexture); //called from  uMod_IDirect3DTexture9::Release()
    int LookUpToMod(uModTexturePtr auto pTexture);
    // called at the end AddTexture(...) and from Device->UpdateTexture(...)

    int MergeUpdate(); //called from uMod_IDirect3DDevice9::BeginScene()
    void Initialize();

    // Add TextureFileStruct data, return size of data added. 0 on failure.
    unsigned long AddFile(TextureFileStruct& entry);

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
    uMod_IDirect3DTexture9* GetSingleTexture();
    uMod_IDirect3DVolumeTexture9* GetSingleVolumeTexture();
    uMod_IDirect3DCubeTexture9* GetSingleCubeTexture();
    int SetLastCreatedTexture(uMod_IDirect3DTexture9*);
    int SetLastCreatedVolumeTexture(uMod_IDirect3DVolumeTexture9*);
    int SetLastCreatedCubeTexture(uMod_IDirect3DCubeTexture9*);

    bool should_update = false;

    int LockMutex();
    int UnlockMutex();
    HANDLE mutex;

    std::unordered_map<HashType, TextureFileStruct*> modded_textures;
    // array which stores the file in memory and the hash of each texture to be modded

    // called if a target texture is found
    int LoadTexture(TextureFileStruct* file_in_memory, uModTexturePtrPtr auto ppTexture);

    void LoadModsFromFile(const char* source);
};

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
    if (file_in_memory->is_wic_texture) {
        if constexpr (std::same_as<decltype(ppTexture), uMod_IDirect3DTexture9**>) {
            if (D3D_OK != D3DXCreateTextureFromFileInMemoryEx(
                    D3D9Device, file_in_memory->data.data(),
                    file_in_memory->data.size(), D3DX_DEFAULT, D3DX_DEFAULT,
                    D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
                    D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr,
                    reinterpret_cast<IDirect3DTexture9**>(ppTexture))) {
                *ppTexture = nullptr;
                Message("LoadWICTexture( %p, %#lX): FAILED\n", *ppTexture, file_in_memory->crc_hash);
                return RETURN_TEXTURE_NOT_LOADED;
            }
        }
        else if constexpr (std::same_as<decltype(ppTexture), uMod_IDirect3DVolumeTexture9**>) {
            if (D3D_OK != D3DXCreateVolumeTextureFromFileInMemoryEx(
                    D3D9Device, file_in_memory->data.data(),
                    file_in_memory->data.size(), D3DX_DEFAULT, D3DX_DEFAULT,
                    D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
                    D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,
                    nullptr,
                    nullptr,
                    reinterpret_cast<IDirect3DVolumeTexture9**>(ppTexture))) {
                *ppTexture = nullptr;
                Message("LoadWICTexture( %p, %#lX): FAILED\n", *ppTexture, file_in_memory->crc_hash);
                return RETURN_TEXTURE_NOT_LOADED;
            }
            SetLastCreatedVolumeTexture(nullptr);
        }
        else if constexpr (std::same_as<decltype(ppTexture), uMod_IDirect3DCubeTexture9**>) {
            if (D3D_OK != D3DXCreateCubeTextureFromFileInMemoryEx(
                    D3D9Device, file_in_memory->data.data(), file_in_memory->data.size(), D3DX_DEFAULT, D3DX_DEFAULT, 0,
                    D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
                    D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr,
                    reinterpret_cast<IDirect3DCubeTexture9**>(ppTexture))) {
                *ppTexture = nullptr;
                Message("LoadWICTexture( %p, %#lX): FAILED\n", *ppTexture, file_in_memory->crc_hash);
                return RETURN_TEXTURE_NOT_LOADED;
            }
        }
    }
    else if (D3D_OK != DirectX::CreateDDSTextureFromMemoryEx(
                 D3D9Device,
                 file_in_memory->data.data(),
                 file_in_memory->data.size(),
                 0, D3DPOOL_MANAGED, false, reinterpret_cast<LPDIRECT3DTEXTURE9*>(ppTexture))) {
        *ppTexture = nullptr;
        Message("LoadDDSTexture( %p, 0x%#lX): FAILED\n", *ppTexture, file_in_memory->crc_hash);
        return RETURN_TEXTURE_NOT_LOADED;
    }
    SetLastCreatedTexture(nullptr);
    (*ppTexture)->FAKE = true;

    Message("LoadTexture( %p, %#lX): DONE\n", *ppTexture, file_in_memory->crc_hash);
    return RETURN_OK;
}

template <typename T> requires uModTexturePtr<T>
void UnswitchTextures(T pTexture)
{
    decltype(pTexture) CrossRef = pTexture->CrossRef_D3Dtex;
    if (CrossRef != nullptr) {
        std::swap(pTexture->m_D3Dtex, CrossRef->m_D3Dtex);

        // cancel the link
        CrossRef->CrossRef_D3Dtex = nullptr;
        pTexture->CrossRef_D3Dtex = nullptr;
    }
}

template <typename T> requires uModTexturePtr<T>
int SwitchTextures(T pTexture1, T pTexture2)
{
    if (pTexture1->m_D3Ddev == pTexture2->m_D3Ddev && pTexture1->CrossRef_D3Dtex == nullptr && pTexture2->CrossRef_D3Dtex == nullptr) {
        // make cross reference
        pTexture1->CrossRef_D3Dtex = pTexture2;
        pTexture2->CrossRef_D3Dtex = pTexture1;

        // switch textures
        std::swap(pTexture1->m_D3Dtex, pTexture2->m_D3Dtex);
        return RETURN_OK;
    }
    return RETURN_TEXTURE_NOT_SWITCHED;
}
