#pragma once

#include <map>

#include "gMod_FileLoader.h"
#include "uMod_IDirect3DTexture9.h"
#include "uMod_ArrayHandler.h"

/*
 *  An object of this class is owned by each d3d9 device.
 *  functions called by the Server are called from the server thread instance.
 *  All other functions are called from the render thread instance of the game itself.
 */

class uMod_TextureClient {
public:
    uMod_TextureClient(IDirect3DDevice9* device);
    ~uMod_TextureClient();

    int AddTexture(uMod_IDirect3DTexture9* tex); //called from uMod_IDirect3DDevice9::CreateTexture(...) or uMod_IDirect3DDevice9::BeginScene()
    int AddTexture(uMod_IDirect3DVolumeTexture9* tex); //called from uMod_IDirect3DVolumeTexture9::CreateTexture(...) or uMod_IDirect3DDevice9::BeginScene()
    int AddTexture(uMod_IDirect3DCubeTexture9* tex); //called from uMod_IDirect3DCubeTexture9::CreateTexture(...) or uMod_IDirect3DDevice9::BeginScene()

    int RemoveTexture(uMod_IDirect3DTexture9* tex); //called from  uMod_IDirect3DTexture9::Release()
    int RemoveTexture(uMod_IDirect3DVolumeTexture9* tex); //called from  uMod_IDirect3DVolumeTexture9::Release()
    int RemoveTexture(uMod_IDirect3DCubeTexture9* tex); //called from  uMod_IDirect3DCubeTexture9::Release()

    int MergeUpdate(); //called from uMod_IDirect3DDevice9::BeginScene()

    int LookUpToMod(uMod_IDirect3DTexture9* pTexture, int num_index_list = 0, int* index_list = nullptr); // called at the end AddTexture(...) and from Device->UpdateTexture(...)
    int LookUpToMod(uMod_IDirect3DVolumeTexture9* pTexture, int num_index_list = 0, int* index_list = nullptr); // called at the end AddTexture(...) and from Device->UpdateTexture(...)
    int LookUpToMod(uMod_IDirect3DCubeTexture9* pTexture, int num_index_list = 0, int* index_list = nullptr); // called at the end AddTexture(...) and from Device->UpdateTexture(...)
    bool AddFile(TpfEntry& entry);

    uMod_TextureHandler<uMod_IDirect3DTexture9> OriginalTextures; // stores the pointer to the uMod_IDirect3DTexture9 objects created by the game
    uMod_TextureHandler<uMod_IDirect3DVolumeTexture9> OriginalVolumeTextures; // stores the pointer to the uMod_IDirect3DVolumeTexture9 objects created by the game
    uMod_TextureHandler<uMod_IDirect3DCubeTexture9> OriginalCubeTextures; // stores the pointer to the uMod_IDirect3DCubeTexture9 objects created by the game

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


    TextureFileStruct* Update;
    int NumberOfUpdate;
    bool should_update = false;

    int LockMutex();
    int UnlockMutex();
    HANDLE Mutex;

    int NumberToMod; // number of texture to be modded
    TextureFileStruct* FileToMod; // array which stores the file in memory and the hash of each texture to be modded
    std::map<HashType, TextureFileStruct> modded_textures; // array which stores the file in memory and the hash of each texture to be modded

    int LookUpToMod(HashType hash, int num_index_list, int* index_list); // called from LookUpToMod(...);
    int LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DTexture9** ppTexture); // called if a target texture is found
    int LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DVolumeTexture9** ppTexture); // called if a target texture is found
    int LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DCubeTexture9** ppTexture); // called if a target texture is found
    void LoadModsFromFile(const char* source);

    // and the corresponding fake texture should be loaded

    //HashType GetHash(unsigned char *str, int len);
    //unsigned int GetCRC32(char *pcDatabuf, unsigned int ulDatalen);
};

