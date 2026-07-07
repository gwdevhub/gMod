#pragma once

#include <d3d9.h>
#include <atomic>

class TextureClient;

// Wraps a real IDirect3D*Texture9 so every method is ordinary compiled virtual dispatch
// instead of a MinHook-patched vtable slot. See D3D9DeviceWrapper.h for why this matters.
class WrappedTexture9 : public IDirect3DTexture9 {
public:
    explicit WrappedTexture9(IDirect3DTexture9* real);

    IDirect3DTexture9* RealTexture() const { return real_; }

    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj) override;
    STDMETHOD_(ULONG, AddRef)() override;
    STDMETHOD_(ULONG, Release)() override;

    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override;
    STDMETHOD(SetPrivateData)(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) override;
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

    STDMETHOD(GetLevelDesc)(UINT Level, D3DSURFACE_DESC* pDesc) override;
    STDMETHOD(GetSurfaceLevel)(UINT Level, IDirect3DSurface9** ppSurfaceLevel) override;
    STDMETHOD(LockRect)(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) override;
    STDMETHOD(UnlockRect)(UINT Level) override;
    STDMETHOD(AddDirtyRect)(const RECT* pDirtyRect) override;

private:
    IDirect3DTexture9* real_;
    std::atomic<ULONG> ref_count_;
};

class WrappedVolumeTexture9 : public IDirect3DVolumeTexture9 {
public:
    explicit WrappedVolumeTexture9(IDirect3DVolumeTexture9* real);

    IDirect3DVolumeTexture9* RealTexture() const { return real_; }

    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj) override;
    STDMETHOD_(ULONG, AddRef)() override;
    STDMETHOD_(ULONG, Release)() override;

    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override;
    STDMETHOD(SetPrivateData)(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) override;
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

    STDMETHOD(GetLevelDesc)(UINT Level, D3DVOLUME_DESC* pDesc) override;
    STDMETHOD(GetVolumeLevel)(UINT Level, IDirect3DVolume9** ppVolumeLevel) override;
    STDMETHOD(LockBox)(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags) override;
    STDMETHOD(UnlockBox)(UINT Level) override;
    STDMETHOD(AddDirtyBox)(const D3DBOX* pDirtyBox) override;

private:
    IDirect3DVolumeTexture9* real_;
    std::atomic<ULONG> ref_count_;
};

class WrappedCubeTexture9 : public IDirect3DCubeTexture9 {
public:
    explicit WrappedCubeTexture9(IDirect3DCubeTexture9* real);

    IDirect3DCubeTexture9* RealTexture() const { return real_; }

    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObj) override;
    STDMETHOD_(ULONG, AddRef)() override;
    STDMETHOD_(ULONG, Release)() override;

    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override;
    STDMETHOD(SetPrivateData)(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) override;
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

    STDMETHOD(GetLevelDesc)(UINT Level, D3DSURFACE_DESC* pDesc) override;
    STDMETHOD(GetCubeMapSurface)(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9** ppCubeMapSurface) override;
    STDMETHOD(LockRect)(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) override;
    STDMETHOD(UnlockRect)(D3DCUBEMAP_FACES FaceType, UINT Level) override;
    STDMETHOD(AddDirtyRect)(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect) override;

private:
    IDirect3DCubeTexture9* real_;
    std::atomic<ULONG> ref_count_;
};

// Real texture pointer -> wrapper, for the D3D9 calls that only ever hand back real
// pointers (e.g. IDirect3DDevice9::GetTexture) so they can be turned back into the
// wrapper identity the game must see. Populated by each wrapper's constructor, erased
// in its destructor/Release.
IDirect3DBaseTexture9* FindWrapperForRealTexture(IDirect3DBaseTexture9* real);
void RegisterWrapperForRealTexture(IDirect3DBaseTexture9* real, IDirect3DBaseTexture9* wrapper);
void UnregisterWrapperForRealTexture(IDirect3DBaseTexture9* real);
