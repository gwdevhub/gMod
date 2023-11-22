#pragma once

#include <d3d9.h>

class uMod_IDirect3D9Ex : public IDirect3D9Ex {
public:
    uMod_IDirect3D9Ex(IDirect3D9Ex* pOriginal);
    virtual ~uMod_IDirect3D9Ex();

    // The original DX9 function definitions
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;
    HRESULT __stdcall RegisterSoftwareDevice(void* pInitializeFunction) override;
    UINT __stdcall GetAdapterCount() override;
    HRESULT __stdcall GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER9* pIdentifier) override;
    UINT __stdcall GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) override;
    HRESULT __stdcall EnumAdapterModes(UINT Adapter, D3DFORMAT Format, UINT Mode, D3DDISPLAYMODE* pMode) override;
    HRESULT __stdcall GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) override;
    HRESULT __stdcall CheckDeviceType(UINT iAdapter, D3DDEVTYPE DevType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL bWindowed) override;
    HRESULT __stdcall CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat) override;
    HRESULT __stdcall CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD* pQualityLevels) override;
    HRESULT __stdcall CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat) override;
    HRESULT __stdcall CheckDeviceFormatConversion(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SourceFormat, D3DFORMAT TargetFormat) override;
    HRESULT __stdcall GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS9* pCaps) override;
    HMONITOR __stdcall GetAdapterMonitor(UINT Adapter) override;
    HRESULT __stdcall CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface) override;

    HRESULT __stdcall CreateDeviceEx(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode,
                                     IDirect3DDevice9Ex** ppReturnedDeviceInterface) override;
    HRESULT __stdcall EnumAdapterModesEx(UINT Adapter, const D3DDISPLAYMODEFILTER* pFilter, UINT Mode, D3DDISPLAYMODEEX* pMode) override;
    HRESULT __stdcall GetAdapterDisplayModeEx(UINT Adapter, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) override;
    HRESULT __stdcall GetAdapterLUID(UINT Adapter, LUID* pLUID) override;
    UINT __stdcall GetAdapterModeCountEx(UINT Adapter, const D3DDISPLAYMODEFILTER* pFilter) override;

private:
    IDirect3D9Ex* m_pIDirect3D9Ex;
};

