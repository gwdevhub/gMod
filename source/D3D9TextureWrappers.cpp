#include "Main.h"
#include "D3D9TextureWrappers.h"

#include <mutex>
#include <unordered_map>

import TextureClient;

namespace {
    std::mutex g_real_to_wrapper_mutex;
    std::unordered_map<IDirect3DBaseTexture9*, IDirect3DBaseTexture9*> g_real_to_wrapper;
}

IDirect3DBaseTexture9* FindWrapperForRealTexture(IDirect3DBaseTexture9* real)
{
    if (real == nullptr) return nullptr;
    std::lock_guard lk(g_real_to_wrapper_mutex);
    const auto it = g_real_to_wrapper.find(real);
    return it != g_real_to_wrapper.end() ? it->second : nullptr;
}

void RegisterWrapperForRealTexture(IDirect3DBaseTexture9* real, IDirect3DBaseTexture9* wrapper)
{
    std::lock_guard lk(g_real_to_wrapper_mutex);
    g_real_to_wrapper[real] = wrapper;
}

void UnregisterWrapperForRealTexture(IDirect3DBaseTexture9* real)
{
    std::lock_guard lk(g_real_to_wrapper_mutex);
    g_real_to_wrapper.erase(real);
}

// ---- WrappedTexture9 ----------------------------------------------------

WrappedTexture9::WrappedTexture9(IDirect3DTexture9* real) : real_(real), ref_count_(1)
{
    real_->AddRef();
    RegisterWrapperForRealTexture(real_, this);
}

HRESULT WrappedTexture9::QueryInterface(REFIID riid, void** ppvObj)
{
    if (riid == IID_IUnknown || riid == IID_IDirect3DTexture9) {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }
    // Unrecognized-but-successful QueryInterface hands back an unwrapped real pointer.
    return real_->QueryInterface(riid, ppvObj);
}

ULONG WrappedTexture9::AddRef()
{
    return ++ref_count_;
}

ULONG WrappedTexture9::Release()
{
    const ULONG count = --ref_count_;
    if (count == 0) {
        if (auto* client = TextureClient::CurrentClient())
            client->OnReleaseTexture(this);
        UnregisterWrapperForRealTexture(real_);
        real_->Release();
        delete this;
        return 0;
    }
    return count;
}

HRESULT WrappedTexture9::GetDevice(IDirect3DDevice9** ppDevice) { return real_->GetDevice(ppDevice); }
HRESULT WrappedTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) { return real_->SetPrivateData(refguid, pData, SizeOfData, Flags); }
HRESULT WrappedTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return real_->GetPrivateData(refguid, pData, pSizeOfData); }
HRESULT WrappedTexture9::FreePrivateData(REFGUID refguid) { return real_->FreePrivateData(refguid); }
DWORD WrappedTexture9::SetPriority(DWORD PriorityNew) { return real_->SetPriority(PriorityNew); }
DWORD WrappedTexture9::GetPriority() { return real_->GetPriority(); }
void WrappedTexture9::PreLoad() { real_->PreLoad(); }
D3DRESOURCETYPE WrappedTexture9::GetType() { return real_->GetType(); }

DWORD WrappedTexture9::SetLOD(DWORD LODNew) { return real_->SetLOD(LODNew); }
DWORD WrappedTexture9::GetLOD() { return real_->GetLOD(); }
DWORD WrappedTexture9::GetLevelCount() { return real_->GetLevelCount(); }
HRESULT WrappedTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) { return real_->SetAutoGenFilterType(FilterType); }
D3DTEXTUREFILTERTYPE WrappedTexture9::GetAutoGenFilterType() { return real_->GetAutoGenFilterType(); }
void WrappedTexture9::GenerateMipSubLevels() { real_->GenerateMipSubLevels(); }

HRESULT WrappedTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) { return real_->GetLevelDesc(Level, pDesc); }
HRESULT WrappedTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel) { return real_->GetSurfaceLevel(Level, ppSurfaceLevel); }
HRESULT WrappedTexture9::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) { return real_->LockRect(Level, pLockedRect, pRect, Flags); }
HRESULT WrappedTexture9::UnlockRect(UINT Level) { return real_->UnlockRect(Level); }
HRESULT WrappedTexture9::AddDirtyRect(const RECT* pDirtyRect) { return real_->AddDirtyRect(pDirtyRect); }

// ---- WrappedVolumeTexture9 -----------------------------------------------

WrappedVolumeTexture9::WrappedVolumeTexture9(IDirect3DVolumeTexture9* real) : real_(real), ref_count_(1)
{
    real_->AddRef();
    RegisterWrapperForRealTexture(real_, this);
}

HRESULT WrappedVolumeTexture9::QueryInterface(REFIID riid, void** ppvObj)
{
    if (riid == IID_IUnknown || riid == IID_IDirect3DVolumeTexture9) {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }
    // Unrecognized-but-successful QueryInterface hands back an unwrapped real pointer.
    return real_->QueryInterface(riid, ppvObj);
}

ULONG WrappedVolumeTexture9::AddRef()
{
    return ++ref_count_;
}

ULONG WrappedVolumeTexture9::Release()
{
    const ULONG count = --ref_count_;
    if (count == 0) {
        if (auto* client = TextureClient::CurrentClient())
            client->OnReleaseTexture(this);
        UnregisterWrapperForRealTexture(real_);
        real_->Release();
        delete this;
        return 0;
    }
    return count;
}

HRESULT WrappedVolumeTexture9::GetDevice(IDirect3DDevice9** ppDevice) { return real_->GetDevice(ppDevice); }
HRESULT WrappedVolumeTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) { return real_->SetPrivateData(refguid, pData, SizeOfData, Flags); }
HRESULT WrappedVolumeTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return real_->GetPrivateData(refguid, pData, pSizeOfData); }
HRESULT WrappedVolumeTexture9::FreePrivateData(REFGUID refguid) { return real_->FreePrivateData(refguid); }
DWORD WrappedVolumeTexture9::SetPriority(DWORD PriorityNew) { return real_->SetPriority(PriorityNew); }
DWORD WrappedVolumeTexture9::GetPriority() { return real_->GetPriority(); }
void WrappedVolumeTexture9::PreLoad() { real_->PreLoad(); }
D3DRESOURCETYPE WrappedVolumeTexture9::GetType() { return real_->GetType(); }

DWORD WrappedVolumeTexture9::SetLOD(DWORD LODNew) { return real_->SetLOD(LODNew); }
DWORD WrappedVolumeTexture9::GetLOD() { return real_->GetLOD(); }
DWORD WrappedVolumeTexture9::GetLevelCount() { return real_->GetLevelCount(); }
HRESULT WrappedVolumeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) { return real_->SetAutoGenFilterType(FilterType); }
D3DTEXTUREFILTERTYPE WrappedVolumeTexture9::GetAutoGenFilterType() { return real_->GetAutoGenFilterType(); }
void WrappedVolumeTexture9::GenerateMipSubLevels() { real_->GenerateMipSubLevels(); }

HRESULT WrappedVolumeTexture9::GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc) { return real_->GetLevelDesc(Level, pDesc); }
HRESULT WrappedVolumeTexture9::GetVolumeLevel(UINT Level, IDirect3DVolume9** ppVolumeLevel) { return real_->GetVolumeLevel(Level, ppVolumeLevel); }
HRESULT WrappedVolumeTexture9::LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, const D3DBOX* pBox, DWORD Flags) { return real_->LockBox(Level, pLockedVolume, pBox, Flags); }
HRESULT WrappedVolumeTexture9::UnlockBox(UINT Level) { return real_->UnlockBox(Level); }
HRESULT WrappedVolumeTexture9::AddDirtyBox(const D3DBOX* pDirtyBox) { return real_->AddDirtyBox(pDirtyBox); }

// ---- WrappedCubeTexture9 -------------------------------------------------

WrappedCubeTexture9::WrappedCubeTexture9(IDirect3DCubeTexture9* real) : real_(real), ref_count_(1)
{
    real_->AddRef();
    RegisterWrapperForRealTexture(real_, this);
}

HRESULT WrappedCubeTexture9::QueryInterface(REFIID riid, void** ppvObj)
{
    if (riid == IID_IUnknown || riid == IID_IDirect3DCubeTexture9) {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }
    // Unrecognized-but-successful QueryInterface hands back an unwrapped real pointer.
    return real_->QueryInterface(riid, ppvObj);
}

ULONG WrappedCubeTexture9::AddRef()
{
    return ++ref_count_;
}

ULONG WrappedCubeTexture9::Release()
{
    const ULONG count = --ref_count_;
    if (count == 0) {
        if (auto* client = TextureClient::CurrentClient())
            client->OnReleaseTexture(this);
        UnregisterWrapperForRealTexture(real_);
        real_->Release();
        delete this;
        return 0;
    }
    return count;
}

HRESULT WrappedCubeTexture9::GetDevice(IDirect3DDevice9** ppDevice) { return real_->GetDevice(ppDevice); }
HRESULT WrappedCubeTexture9::SetPrivateData(REFGUID refguid, const void* pData, DWORD SizeOfData, DWORD Flags) { return real_->SetPrivateData(refguid, pData, SizeOfData, Flags); }
HRESULT WrappedCubeTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData) { return real_->GetPrivateData(refguid, pData, pSizeOfData); }
HRESULT WrappedCubeTexture9::FreePrivateData(REFGUID refguid) { return real_->FreePrivateData(refguid); }
DWORD WrappedCubeTexture9::SetPriority(DWORD PriorityNew) { return real_->SetPriority(PriorityNew); }
DWORD WrappedCubeTexture9::GetPriority() { return real_->GetPriority(); }
void WrappedCubeTexture9::PreLoad() { real_->PreLoad(); }
D3DRESOURCETYPE WrappedCubeTexture9::GetType() { return real_->GetType(); }

DWORD WrappedCubeTexture9::SetLOD(DWORD LODNew) { return real_->SetLOD(LODNew); }
DWORD WrappedCubeTexture9::GetLOD() { return real_->GetLOD(); }
DWORD WrappedCubeTexture9::GetLevelCount() { return real_->GetLevelCount(); }
HRESULT WrappedCubeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) { return real_->SetAutoGenFilterType(FilterType); }
D3DTEXTUREFILTERTYPE WrappedCubeTexture9::GetAutoGenFilterType() { return real_->GetAutoGenFilterType(); }
void WrappedCubeTexture9::GenerateMipSubLevels() { real_->GenerateMipSubLevels(); }

HRESULT WrappedCubeTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc) { return real_->GetLevelDesc(Level, pDesc); }
HRESULT WrappedCubeTexture9::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9** ppCubeMapSurface) { return real_->GetCubeMapSurface(FaceType, Level, ppCubeMapSurface); }
HRESULT WrappedCubeTexture9::LockRect(D3DCUBEMAP_FACES FaceType, UINT Level, D3DLOCKED_RECT* pLockedRect, const RECT* pRect, DWORD Flags) { return real_->LockRect(FaceType, Level, pLockedRect, pRect, Flags); }
HRESULT WrappedCubeTexture9::UnlockRect(D3DCUBEMAP_FACES FaceType, UINT Level) { return real_->UnlockRect(FaceType, Level); }
HRESULT WrappedCubeTexture9::AddDirtyRect(D3DCUBEMAP_FACES FaceType, const RECT* pDirtyRect) { return real_->AddDirtyRect(FaceType, pDirtyRect); }
