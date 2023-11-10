#include "uMod_Main.h"

#define IDirect3D9 IDirect3D9Ex
#define uMod_IDirect3D9 uMod_IDirect3D9Ex
#define m_pIDirect3D9 m_pIDirect3D9Ex
#define PRE_MESSAGE "uMod_IDirect3D9Ex"

// ReSharper disable once CppUnusedIncludeDirective
#include "uMod_IDirect3D9.cpp"

HRESULT __stdcall uMod_IDirect3D9Ex::CreateDeviceEx(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode,
                                                    IDirect3DDevice9Ex** ppReturnedDeviceInterface)
{
    Message("uMod_IDirect3D9Ex::CreateDeviceEx: %lu\n", this);
    // we intercept this call and provide our own "fake" Device Object
    const HRESULT hres = m_pIDirect3D9Ex->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);

    int count = 1;
    if (pPresentationParameters != nullptr) {
        count = pPresentationParameters->BackBufferCount;
    }
    const auto pIDirect3DDevice9Ex = new uMod_IDirect3DDevice9Ex(*ppReturnedDeviceInterface, uMod_Server, count);

    // store our pointer (the fake one) for returning it to the calling program
    *ppReturnedDeviceInterface = pIDirect3DDevice9Ex;

    return hres;
}

HRESULT __stdcall uMod_IDirect3D9Ex::EnumAdapterModesEx(UINT Adapter, const D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode)
{
    return m_pIDirect3D9Ex->EnumAdapterModesEx(Adapter, pFilter, Mode, pMode);
}

HRESULT __stdcall uMod_IDirect3D9Ex::GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation)
{
    return m_pIDirect3D9Ex->GetAdapterDisplayModeEx(Adapter, pMode, pRotation);
}

HRESULT __stdcall uMod_IDirect3D9Ex::GetAdapterLUID(UINT Adapter, LUID* pLUID)
{
    return m_pIDirect3D9Ex->GetAdapterLUID(Adapter, pLUID);
}

UINT __stdcall uMod_IDirect3D9Ex::GetAdapterModeCountEx(UINT Adapter, const D3DDISPLAYMODEFILTER* pFilter)
{
    return m_pIDirect3D9Ex->GetAdapterModeCountEx(Adapter, pFilter);
}
