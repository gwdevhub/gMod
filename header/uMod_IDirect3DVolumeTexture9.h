#pragma once

#include <d3d9.h>

interface uMod_IDirect3DVolumeTexture9 : IDirect3DVolumeTexture9 {
    uMod_IDirect3DVolumeTexture9(IDirect3DVolumeTexture9** ppTex, IDirect3DDevice9* pIDirect3DDevice9)
    {
        m_D3Dtex = *ppTex; //Texture which will be displayed and will be passed to the game
        m_D3Ddev = pIDirect3DDevice9; //device pointer
        CrossRef_D3Dtex = nullptr; //cross reference
        // fake texture: store the pointer to the original uMod_IDirect3DVolumeTexture9 object, needed if a fake texture is unselected
        // original texture: stores the pointer to the fake texture object, is needed if original texture is deleted,
        // thus the fake texture can also be deleted
        Reference = -1; //need for fast deleting
        Hash = 0u;
        FAKE = false;
    }

    // callback interface
    IDirect3DVolumeTexture9* m_D3Dtex;
    uMod_IDirect3DVolumeTexture9* CrossRef_D3Dtex;
    IDirect3DDevice9* m_D3Ddev;
    int Reference;
    MyTypeHash Hash;
    bool FAKE;

    // original interface
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj) override;
    STDMETHOD_(ULONG, AddRef)() override;
    STDMETHOD_(ULONG, Release)() override;
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override;
    STDMETHOD(SetPrivateData)(REFGUID refguid,CONST void* pData, DWORD SizeOfData, DWORD Flags) override;
    STDMETHOD(GetPrivateData)(REFGUID refguid, void* pData, DWORD* pSizeOfData) override;
    STDMETHOD(FreePrivateData)(REFGUID refguid) override;
    STDMETHOD_(DWORD, SetPriority)(DWORD PriorityNew) override;
    STDMETHOD_(DWORD, GetPriority)() override;
    STDMETHOD_(void, PreLoad)() override;
    STDMETHOD_(D3DRESOURCETYPE, GetType)() override;
    STDMETHOD_(DWORD, SetLOD)(DWORD LODNew) override;
    STDMETHOD_(DWORD, GetLOD)() override;
    STDMETHOD_(DWORD, GetLevelCount)() override;
    STDMETHOD(SetAutoGenFilterType)(D3DTEXTUREFILTERTYPE FilterType) override;
    STDMETHOD_(D3DTEXTUREFILTERTYPE, GetAutoGenFilterType)() override;
    STDMETHOD_(void, GenerateMipSubLevels)() override;
    STDMETHOD(AddDirtyBox)(CONST D3DBOX* pDirtyBox) override;
    STDMETHOD(GetLevelDesc)(UINT Level, D3DVOLUME_DESC* pDesc) override;
    STDMETHOD(GetVolumeLevel)(UINT Level, IDirect3DVolume9** ppVolumeLevel) override;
    STDMETHOD(LockBox)(UINT Level, D3DLOCKED_BOX* pLockedVolume, CONST D3DBOX* pBox, DWORD Flags) override;
    STDMETHOD(UnlockBox)(UINT Level) override;


    int GetHash(MyTypeHash& hash);
};



inline void UnswitchTextures(uMod_IDirect3DVolumeTexture9* pTexture)
{
    uMod_IDirect3DVolumeTexture9* CrossRef = pTexture->CrossRef_D3Dtex;
    if (CrossRef != nullptr) {
        // switch textures back
        IDirect3DVolumeTexture9* cpy = pTexture->m_D3Dtex;
        pTexture->m_D3Dtex = CrossRef->m_D3Dtex;
        CrossRef->m_D3Dtex = cpy;

        // cancel the link
        CrossRef->CrossRef_D3Dtex = nullptr;
        pTexture->CrossRef_D3Dtex = nullptr;
    }
}

inline int SwitchTextures(uMod_IDirect3DVolumeTexture9* pTexture1, uMod_IDirect3DVolumeTexture9* pTexture2)
{
    if (pTexture1->m_D3Ddev == pTexture2->m_D3Ddev && pTexture1->CrossRef_D3Dtex == nullptr && pTexture2->CrossRef_D3Dtex == nullptr) {
        // make cross reference
        pTexture1->CrossRef_D3Dtex = pTexture2;
        pTexture2->CrossRef_D3Dtex = pTexture1;

        // switch textures
        IDirect3DVolumeTexture9* cpy = pTexture2->m_D3Dtex;
        pTexture2->m_D3Dtex = pTexture1->m_D3Dtex;
        pTexture1->m_D3Dtex = cpy;
        return RETURN_OK;
    }
    return RETURN_TEXTURE_NOT_SWITCHED;
}

