/*
This file is part of Universal Modding Engine.


Universal Modding Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Universal Modding Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Universal Modding Engine.  If not, see <http://www.gnu.org/licenses/>.
*/





#include "uMod_Main.h"

#ifndef PRE_MESSAGE
#define PRE_MESSAGE "uMod_IDirect3D9"
#endif

uMod_IDirect3D9::uMod_IDirect3D9( IDirect3D9 *pOriginal, uMod_TextureServer* server)
{
  Message( PRE_MESSAGE "::" PRE_MESSAGE "( %lu, %lu): %lu\n", pOriginal, server, this);
  m_pIDirect3D9 = pOriginal;
  uMod_Server = server;
}

uMod_IDirect3D9::~uMod_IDirect3D9(void)
{
  Message( PRE_MESSAGE "::~" PRE_MESSAGE "(): %lu\n", this);
}

HRESULT  __stdcall uMod_IDirect3D9::QueryInterface(REFIID riid, void** ppvObj)
{
  *ppvObj = NULL;

	// call this to increase AddRef at original object
	// and to check if such an interface is there

	HRESULT hRes = m_pIDirect3D9->QueryInterface(riid, ppvObj); 

	if (hRes == NOERROR) // if OK, send our "fake" address
	{
		*ppvObj = this;
	}
	
	return (hRes);
}

ULONG    __stdcall uMod_IDirect3D9::AddRef(void)
{
  return(m_pIDirect3D9->AddRef());
}

ULONG    __stdcall uMod_IDirect3D9::Release(void)
{
	// call original routine
	ULONG count = m_pIDirect3D9->Release();
	
  // in case no further Ref is there, the Original Object has deleted itself
	if (count == 0) 
	{
  	delete(this); 
	}

	return(count);
}

HRESULT __stdcall uMod_IDirect3D9::RegisterSoftwareDevice(void* pInitializeFunction)
{
  return(m_pIDirect3D9->RegisterSoftwareDevice(pInitializeFunction));
}

UINT __stdcall uMod_IDirect3D9::GetAdapterCount(void)
{
  return(m_pIDirect3D9->GetAdapterCount());
}

HRESULT __stdcall uMod_IDirect3D9::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier)
{
  return(m_pIDirect3D9->GetAdapterIdentifier(Adapter,Flags,pIdentifier));
}

UINT __stdcall uMod_IDirect3D9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
  return(m_pIDirect3D9->GetAdapterModeCount(Adapter, Format));
}

HRESULT __stdcall uMod_IDirect3D9::EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode)
{
  return(m_pIDirect3D9->EnumAdapterModes(Adapter,Format,Mode,pMode));
}

HRESULT __stdcall uMod_IDirect3D9::GetAdapterDisplayMode( UINT Adapter,D3DDISPLAYMODE* pMode)
{
  return(m_pIDirect3D9->GetAdapterDisplayMode(Adapter,pMode));
}

HRESULT __stdcall uMod_IDirect3D9::CheckDeviceType(UINT iAdapter,D3DDEVTYPE DevType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed)
{
  return(m_pIDirect3D9->CheckDeviceType(iAdapter,DevType,DisplayFormat,BackBufferFormat,bWindowed));
}

HRESULT __stdcall uMod_IDirect3D9::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
  return(m_pIDirect3D9->CheckDeviceFormat(Adapter,DeviceType,AdapterFormat,Usage,RType,CheckFormat));
}

HRESULT __stdcall uMod_IDirect3D9::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels)
{
  return(m_pIDirect3D9->CheckDeviceMultiSampleType(Adapter,DeviceType,SurfaceFormat,Windowed,MultiSampleType,pQualityLevels));
}

HRESULT __stdcall uMod_IDirect3D9::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
  return(m_pIDirect3D9->CheckDepthStencilMatch(Adapter,DeviceType,AdapterFormat,RenderTargetFormat,DepthStencilFormat));
}

HRESULT __stdcall uMod_IDirect3D9::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
  return(m_pIDirect3D9->CheckDeviceFormatConversion(Adapter,DeviceType,SourceFormat,TargetFormat));
}

HRESULT __stdcall uMod_IDirect3D9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps)
{
  return(m_pIDirect3D9->GetDeviceCaps(Adapter,DeviceType,pCaps));
}

HMONITOR __stdcall uMod_IDirect3D9::GetAdapterMonitor(UINT Adapter)
{
  return(m_pIDirect3D9->GetAdapterMonitor(Adapter));
}

HRESULT __stdcall uMod_IDirect3D9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
{
  Message( PRE_MESSAGE "::CreateDevice(): %lu\n", this);
	// we intercept this call and provide our own "fake" Device Object
	HRESULT hres = m_pIDirect3D9->CreateDevice( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

	int count = 1;
	if (pPresentationParameters!=NULL) count  = pPresentationParameters->BackBufferCount;
	uMod_IDirect3DDevice9 *pIDirect3DDevice9 =  new uMod_IDirect3DDevice9(*ppReturnedDeviceInterface,  uMod_Server, count);
	
	// store our pointer (the fake one) for returning it to the calling program
	*ppReturnedDeviceInterface = pIDirect3DDevice9;

	return(hres); 
}
