#pragma once

#include <vector>
#include "uMod_GlobalDefines.h"
#include "uMod_IDirect3DTexture9.h"
extern unsigned int gl_ErrorState;

struct TextureFileStruct {
    std::vector<char> data{};
    char* pData; // store texture file as file in memory
    unsigned int Size; // size of file
    int NumberOfTextures = 0;
    int Reference = -1; // for a fast delete in the FileHandler
    IDirect3DBaseTexture9** Textures = nullptr; // pointer to the fake textures
    HashType crc_hash = 0; // hash value
};

class uMod_FileHandler  // array to store TextureFileStruct
{
public:
    uMod_FileHandler();
    ~uMod_FileHandler();

    int Add(TextureFileStruct* file);
    int Remove(TextureFileStruct* file);

    int GetNumber() { return Number; }

    TextureFileStruct* operator [](int i)
    {
        if (i < 0 || i >= Number) {
            return nullptr;
        }
        return Files[i / FieldLength][i % FieldLength];
    }

protected:
    static constexpr int FieldLength = 1024;
    long Number = 0;
    int FieldCounter = 0;
    TextureFileStruct*** Files = nullptr;
};


template <class T>
class uMod_TextureHandler  // array to store uMod_IDirect3DTexture9, uMod_IDirect3DVolumeTexture9 or uMod_IDirect3DCubeTexture9
{
public:
    uMod_TextureHandler();
    ~uMod_TextureHandler();

    int Add(T* texture);
    int Remove(T* texture);

    int GetNumber() { return Number; }

    T* operator [](int i)
    {
        if (i < 0 || i >= Number) {
            return nullptr;
        }
        return Textures[i / FieldLength][i % FieldLength];
    }

private:
    static constexpr int FieldLength = 1024;
    long Number;
    int FieldCounter;
    T*** Textures;
};

template <class T>
uMod_TextureHandler<T>::uMod_TextureHandler()
{
    Message("uMod_TextureHandler(): %p\n", this);
    Number = 0;
    FieldCounter = 0;

    Textures = NULL;
}

template <class T>
uMod_TextureHandler<T>::~uMod_TextureHandler()
{
    Message("~uMod_TextureHandler(): %p\n", this);
    if (Textures != nullptr) {
        for (int i = 0; i < FieldCounter; i++) {
            delete [] Textures[i];
        }
        delete [] Textures;
    }
}

template <class T>
int uMod_TextureHandler<T>::Add(T* pTexture)
{
    Message("uMod_TextureHandler::Add( %p): %p\n", pTexture, this);
    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    if (pTexture->Reference >= 0) {
        return RETURN_TEXTURE_ALLREADY_ADDED;
    }

    if (Number / FieldLength == FieldCounter) {
        T*** temp = nullptr;
        try { temp = new T**[FieldCounter + 10]; }
        catch (...) {
            gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_TEXTURE;
            return RETURN_NO_MEMORY;
        }

        for (int i = 0; i < FieldCounter; i++) {
            temp[i] = Textures[i];
        }

        for (int i = FieldCounter; i < FieldCounter + 10; i++) {
            temp[i] = NULL;
        }

        FieldCounter += 10;

        delete [] Textures;

        Textures = temp;
    }
    if (Number % FieldLength == 0) {
        try {
            if (Textures[Number / FieldLength] == NULL) {
                Textures[Number / FieldLength] = new T*[FieldLength];
            }
        }
        catch (...) {
            Textures[Number / FieldLength] = NULL;
            gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_TEXTURE;
            return RETURN_NO_MEMORY;
        }
    }

    Textures[Number / FieldLength][Number % FieldLength] = pTexture;
    pTexture->Reference = Number++;

    return RETURN_OK;
}


template <class T>
int uMod_TextureHandler<T>::Remove(T* pTexture) //will be called, if a texture is completely released
{
    Message("uMod_TextureHandler::Remove(%p): %p\n", pTexture, this);
    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    int ref = pTexture->Reference;
    if (ref < 0) {
        return RETURN_OK; // returning if no TextureHandlerRef is set
    }

    if (ref < (--Number)) {
        Textures[ref / FieldLength][ref % FieldLength] = Textures[Number / FieldLength][Number % FieldLength];
        Textures[ref / FieldLength][ref % FieldLength]->Reference = ref;
    }
    return RETURN_OK;
}
