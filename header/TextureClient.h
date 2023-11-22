#pragma once

#include <map>

#include "FileLoader.h"
#include "uMod_IDirect3DTexture9.h"


struct TextureFileStruct {
    std::vector<char> data{};
    HashType crc_hash = 0; // hash value
};

/*
 *  An object of this class is owned by each d3d9 device.
 *  functions called by the Server are called from the server thread instance.
 *  All other functions are called from the render thread instance of the game itself.
 */
class TextureClient {
public:
    TextureClient(IDirect3DDevice9* device);
    ~TextureClient();

    int AddTexture(uMod_IDirect3DTexture9* tex); //called from uMod_IDirect3DDevice9::CreateTexture(...) or uMod_IDirect3DDevice9::BeginScene()
    int AddTexture(uMod_IDirect3DVolumeTexture9* tex); //called from uMod_IDirect3DVolumeTexture9::CreateTexture(...) or uMod_IDirect3DDevice9::BeginScene()
    int AddTexture(uMod_IDirect3DCubeTexture9* tex); //called from uMod_IDirect3DCubeTexture9::CreateTexture(...) or uMod_IDirect3DDevice9::BeginScene()

    int RemoveTexture(uMod_IDirect3DTexture9* tex); //called from  uMod_IDirect3DTexture9::Release()
    int RemoveTexture(uMod_IDirect3DVolumeTexture9* tex); //called from  uMod_IDirect3DVolumeTexture9::Release()
    int RemoveTexture(uMod_IDirect3DCubeTexture9* tex); //called from  uMod_IDirect3DCubeTexture9::Release()

    int MergeUpdate(); //called from uMod_IDirect3DDevice9::BeginScene()

    int LookUpToMod(uMod_IDirect3DTexture9* pTexture); // called at the end AddTexture(...) and from Device->UpdateTexture(...)
    int LookUpToMod(uMod_IDirect3DVolumeTexture9* pTexture); // called at the end AddTexture(...) and from Device->UpdateTexture(...)
    int LookUpToMod(uMod_IDirect3DCubeTexture9* pTexture); // called at the end AddTexture(...) and from Device->UpdateTexture(...)
    // Add TpfEntry data, return size of data added. 0 on failure.
    unsigned long AddFile(TpfEntry& entry);

    std::vector<uMod_IDirect3DTexture9*> OriginalTextures; // stores the pointer to the uMod_IDirect3DTexture9 objects created by the game
    std::vector<uMod_IDirect3DVolumeTexture9*> OriginalVolumeTextures; // stores the pointer to the uMod_IDirect3DVolumeTexture9 objects created by the game
    std::vector<uMod_IDirect3DCubeTexture9*> OriginalCubeTextures; // stores the pointer to the uMod_IDirect3DCubeTexture9 objects created by the game

    D3DCOLOR FontColour;
    D3DCOLOR TextureColour;

    void Initialize();

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
    HANDLE Mutex;

    std::unordered_map<HashType, TextureFileStruct*> modded_textures; // array which stores the file in memory and the hash of each texture to be modded

    int LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DTexture9** ppTexture); // called if a target texture is found
    int LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DVolumeTexture9** ppTexture); // called if a target texture is found
    int LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DCubeTexture9** ppTexture); // called if a target texture is found
    void LoadModsFromFile(const char* source);

    // and the corresponding fake texture should be loaded

    //HashType GetHash(unsigned char *str, int len);
    //unsigned int GetCRC32(char *pcDatabuf, unsigned int ulDatalen);
};

