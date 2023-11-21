#pragma once

#include <vector>
#include "uMod_GlobalDefines.h"
#include "uMod_IDirect3DTexture9.h"
extern unsigned int gl_ErrorState;

struct TextureFileStruct {
    std::vector<char> data{};
    char* pData; // store texture file as file in memory
    unsigned int Size; // size of file
    int Reference = -1; // for a fast delete in the FileHandler
    std::vector<IDirect3DBaseTexture9*> Textures;
    HashType crc_hash = 0; // hash value

    void RemoveTexture(IDirect3DBaseTexture9* pTexture);
};
