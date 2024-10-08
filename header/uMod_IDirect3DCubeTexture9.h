#pragma once

#include <d3d9.h>

interface uMod_IDirect3DCubeTexture9 : IDirect3DCubeTexture9 {
    uMod_IDirect3DCubeTexture9(IDirect3DCubeTexture9** ppTex, IDirect3DDevice9* pIDirect3DDevice9)
        : m_D3Dtex(*ppTex),
          m_D3Ddev(pIDirect3DDevice9)
    {}

    virtual ~uMod_IDirect3DCubeTexture9() = default;

    // callback interface
    IDirect3DCubeTexture9* m_D3Dtex = nullptr;
    uMod_IDirect3DCubeTexture9* CrossRef_D3Dtex = nullptr;
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

    STDMETHOD(AddDirtyRect)(D3DCUBEMAP_FACES FaceType, CONST RECT* pDirtyRect) override;
    STDMETHOD(GetLevelDesc)(UINT Level, D3DSURFACE_DESC* pDesc) override;
    STDMETHOD(GetCubeMapSurface)(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9** ppCubeMapSurface) override;
    STDMETHOD(LockRect)(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect, DWORD Flags) override;
    STDMETHOD(UnlockRect)(D3DCUBEMAP_FACES FaceType, UINT Level) override;


    [[nodiscard]] HashTuple GetHash() const;
};
