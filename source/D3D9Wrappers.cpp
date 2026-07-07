#include "Main.h"
#include "D3D9Wrappers.h"
#include "D3D9DeviceWrapper.h"

import TextureClient;

namespace {

    // Constructs the device wrapper + its TextureClient. The client must be built with
    // the wrapper pointer (not the real device) so DirectX::CreateDDSTextureFromMemoryEx's
    // internal CreateTexture call re-enters the wrapper's intercepted CreateTexture, which
    // is what makes loading_fake-gated registration (see TextureClient::OnCreateTexture)
    // see that call at all.
    template <class Wrapper, class Real>
    Wrapper* WrapDevice(Real* real_device)
    {
        const auto wrapper = new Wrapper(real_device);
        const auto client = new TextureClient(wrapper);
        wrapper->SetClient(client);
        client->Initialize();
        return wrapper;
    }

}

WrappedDirect3D9::WrappedDirect3D9(IDirect3D9* real) : real_(real), ref_count_(1)
{
    real_->AddRef();
}

HRESULT WrappedDirect3D9::QueryInterface(REFIID riid, void** ppvObj)
{
    if (riid == IID_IUnknown || riid == IID_IDirect3D9) {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }
    // Unrecognized-but-successful QueryInterface hands back an unwrapped real pointer.
    return real_->QueryInterface(riid, ppvObj);
}

ULONG WrappedDirect3D9::AddRef()
{
    return ++ref_count_;
}

ULONG WrappedDirect3D9::Release()
{
    const ULONG count = --ref_count_;
    if (count == 0) {
        real_->Release();
        delete this;
        return 0;
    }
    return count;
}

HRESULT WrappedDirect3D9::RegisterSoftwareDevice(void* pInitializeFunction) { return real_->RegisterSoftwareDevice(pInitializeFunction); }
UINT WrappedDirect3D9::GetAdapterCount() { return real_->GetAdapterCount(); }
HRESULT WrappedDirect3D9::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) { return real_->GetAdapterIdentifier(Adapter, Flags, pIdentifier); }
UINT WrappedDirect3D9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) { return real_->GetAdapterModeCount(Adapter, Format); }
HRESULT WrappedDirect3D9::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) { return real_->EnumAdapterModes(Adapter, Format, Mode, pMode); }
HRESULT WrappedDirect3D9::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) { return real_->GetAdapterDisplayMode(Adapter, pMode); }
HRESULT WrappedDirect3D9::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) { return real_->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed); }
HRESULT WrappedDirect3D9::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) { return real_->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat); }
HRESULT WrappedDirect3D9::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) { return real_->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels); }
HRESULT WrappedDirect3D9::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) { return real_->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat); }
HRESULT WrappedDirect3D9::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) { return real_->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat); }
HRESULT WrappedDirect3D9::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) { return real_->GetDeviceCaps(Adapter, DeviceType, pCaps); }
HMONITOR WrappedDirect3D9::GetAdapterMonitor(UINT Adapter) { return real_->GetAdapterMonitor(Adapter); }

HRESULT WrappedDirect3D9::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    IDirect3DDevice9* real_device = nullptr;
    const HRESULT hr = real_->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &real_device);
    if (FAILED(hr) || real_device == nullptr) {
        if (ppReturnedDeviceInterface) *ppReturnedDeviceInterface = nullptr;
        return hr;
    }
    const auto wrapped = WrapDevice<WrappedDirect3DDevice9>(real_device);
    real_device->Release(); // the wrapper holds its own AddRef'd reference
    *ppReturnedDeviceInterface = wrapped;
    return hr;
}

// ---- WrappedDirect3D9Ex ---------------------------------------------------

WrappedDirect3D9Ex::WrappedDirect3D9Ex(IDirect3D9Ex* real) : real_(real), ref_count_(1)
{
    real_->AddRef();
}

HRESULT WrappedDirect3D9Ex::QueryInterface(REFIID riid, void** ppvObj)
{
    if (riid == IID_IUnknown || riid == IID_IDirect3D9 || riid == IID_IDirect3D9Ex) {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }
    // Unrecognized-but-successful QueryInterface hands back an unwrapped real pointer.
    return real_->QueryInterface(riid, ppvObj);
}

ULONG WrappedDirect3D9Ex::AddRef()
{
    return ++ref_count_;
}

ULONG WrappedDirect3D9Ex::Release()
{
    const ULONG count = --ref_count_;
    if (count == 0) {
        real_->Release();
        delete this;
        return 0;
    }
    return count;
}

HRESULT WrappedDirect3D9Ex::RegisterSoftwareDevice(void* pInitializeFunction) { return real_->RegisterSoftwareDevice(pInitializeFunction); }
UINT WrappedDirect3D9Ex::GetAdapterCount() { return real_->GetAdapterCount(); }
HRESULT WrappedDirect3D9Ex::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) { return real_->GetAdapterIdentifier(Adapter, Flags, pIdentifier); }
UINT WrappedDirect3D9Ex::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) { return real_->GetAdapterModeCount(Adapter, Format); }
HRESULT WrappedDirect3D9Ex::EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) { return real_->EnumAdapterModes(Adapter, Format, Mode, pMode); }
HRESULT WrappedDirect3D9Ex::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) { return real_->GetAdapterDisplayMode(Adapter, pMode); }
HRESULT WrappedDirect3D9Ex::CheckDeviceType(UINT Adapter, D3DDEVTYPE DevType, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) { return real_->CheckDeviceType(Adapter, DevType, AdapterFormat, BackBufferFormat, bWindowed); }
HRESULT WrappedDirect3D9Ex::CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) { return real_->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat); }
HRESULT WrappedDirect3D9Ex::CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) { return real_->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, pQualityLevels); }
HRESULT WrappedDirect3D9Ex::CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) { return real_->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat); }
HRESULT WrappedDirect3D9Ex::CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) { return real_->CheckDeviceFormatConversion(Adapter, DeviceType, SourceFormat, TargetFormat); }
HRESULT WrappedDirect3D9Ex::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) { return real_->GetDeviceCaps(Adapter, DeviceType, pCaps); }
HMONITOR WrappedDirect3D9Ex::GetAdapterMonitor(UINT Adapter) { return real_->GetAdapterMonitor(Adapter); }

HRESULT WrappedDirect3D9Ex::CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
{
    IDirect3DDevice9* real_device = nullptr;
    const HRESULT hr = real_->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, &real_device);
    if (FAILED(hr) || real_device == nullptr) {
        if (ppReturnedDeviceInterface) *ppReturnedDeviceInterface = nullptr;
        return hr;
    }
    const auto wrapped = WrapDevice<WrappedDirect3DDevice9>(real_device);
    real_device->Release();
    *ppReturnedDeviceInterface = wrapped;
    return hr;
}

UINT WrappedDirect3D9Ex::GetAdapterModeCountEx(UINT Adapter, const D3DDISPLAYMODEFILTER* pFilter) { return real_->GetAdapterModeCountEx(Adapter, pFilter); }
HRESULT WrappedDirect3D9Ex::EnumAdapterModesEx(UINT Adapter, const D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode) { return real_->EnumAdapterModesEx(Adapter, pFilter, Mode, pMode); }
HRESULT WrappedDirect3D9Ex::GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) { return real_->GetAdapterDisplayModeEx(Adapter, pMode, pRotation); }

HRESULT WrappedDirect3D9Ex::CreateDeviceEx(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
    IDirect3DDevice9Ex* real_device = nullptr;
    const HRESULT hr = real_->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, &real_device);
    if (FAILED(hr) || real_device == nullptr) {
        if (ppReturnedDeviceInterface) *ppReturnedDeviceInterface = nullptr;
        return hr;
    }
    const auto wrapped = WrapDevice<WrappedDirect3DDevice9Ex>(real_device);
    real_device->Release();
    *ppReturnedDeviceInterface = wrapped;
    return hr;
}

HRESULT WrappedDirect3D9Ex::GetAdapterLUID(UINT Adapter, LUID* pLUID) { return real_->GetAdapterLUID(Adapter, pLUID); }
