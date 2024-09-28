#pragma once

#include <d3d9.h>

interface uMod_IDirect3DVolumeTexture9 : IDirect3DVolumeTexture9 {
    uMod_IDirect3DVolumeTexture9(IDirect3DVolumeTexture9** ppTex, IDirect3DDevice9* pIDirect3DDevice9)
        : m_D3Dtex(*ppTex),
          m_D3Ddev(pIDirect3DDevice9)
    {}

    virtual ~uMod_IDirect3DVolumeTexture9() = default;

    // callback interface
    IDirect3DVolumeTexture9* m_D3Dtex = nullptr;
    uMod_IDirect3DVolumeTexture9* CrossRef_D3Dtex = nullptr;
    IDirect3DDevice9* m_D3Ddev = nullptr;
    TextureFileStruct* Reference = nullptr;
    HashTuple Hash = {};
    bool FAKE = false;

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


    [[nodiscard]] HashTuple GetHash() const;
};
