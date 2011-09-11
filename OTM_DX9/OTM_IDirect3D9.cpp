/*
This file is part of OpenTexMod.


OpenTexMod is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenTexMod is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FooOpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/





#include "OTM_Main.h"

OTM_IDirect3D9::OTM_IDirect3D9(IDirect3D9 *pOriginal, OTM_TextureServer* server)
{
  Message("OTM_IDirect3D9( %lu, %lu): %lu\n", pOriginal, server, this);
	//MessageBox( NULL, TEXT( "Init OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
  m_pIDirect3D9 = pOriginal;
  OTM_Server = server;
}

OTM_IDirect3D9::~OTM_IDirect3D9(void)
{
}

HRESULT  __stdcall OTM_IDirect3D9::QueryInterface(REFIID riid, void** ppvObj)
{
	//MessageBox( NULL, TEXT( "QueryInterface OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
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

ULONG    __stdcall OTM_IDirect3D9::AddRef(void)
{
	//MessageBox( NULL, TEXT( "AddRef OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->AddRef());
}

ULONG    __stdcall OTM_IDirect3D9::Release(void)
{
	//MessageBox( NULL, TEXT( "Release OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );

	// call original routine
	ULONG count = m_pIDirect3D9->Release();
	
    // in case no further Ref is there, the Original Object has deleted itself
	// so do we here
	if (count == 0) 
	{
		//MessageBox( NULL, TEXT( "Release everything" ), TEXT( "Timer" ), MB_OK );
		//extern OTM_IDirect3D9* gl_pOTM_IDirect3D9;
		gl_pIDirect3D9 = NULL;
  	delete(this); 
	}

	return(count);
}

HRESULT __stdcall OTM_IDirect3D9::RegisterSoftwareDevice(void* pInitializeFunction)
{
	//MessageBox( NULL, TEXT( "RegisterSoftwareDevice OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->RegisterSoftwareDevice(pInitializeFunction));
}

UINT __stdcall OTM_IDirect3D9::GetAdapterCount(void)
{
	//MessageBox( NULL, TEXT( "GetAdapterCount OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->GetAdapterCount());
}

HRESULT __stdcall OTM_IDirect3D9::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	//MessageBox( NULL, TEXT( "GetAdapterIdentifier OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->GetAdapterIdentifier(Adapter,Flags,pIdentifier));
}

UINT __stdcall OTM_IDirect3D9::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format)
{
	//MessageBox( NULL, TEXT( "GetAdapterModeCount OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->GetAdapterModeCount(Adapter, Format));
}

HRESULT __stdcall OTM_IDirect3D9::EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode)
{
	//MessageBox( NULL, TEXT( "QueryInterface OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->EnumAdapterModes(Adapter,Format,Mode,pMode));
}

HRESULT __stdcall OTM_IDirect3D9::GetAdapterDisplayMode( UINT Adapter,D3DDISPLAYMODE* pMode)
{
	//MessageBox( NULL, TEXT( "EnumAdapterModes OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->GetAdapterDisplayMode(Adapter,pMode));
}

HRESULT __stdcall OTM_IDirect3D9::CheckDeviceType(UINT iAdapter,D3DDEVTYPE DevType,D3DFORMAT DisplayFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed)
{
	//MessageBox( NULL, TEXT( "CheckDeviceType OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->CheckDeviceType(iAdapter,DevType,DisplayFormat,BackBufferFormat,bWindowed));
}

HRESULT __stdcall OTM_IDirect3D9::CheckDeviceFormat(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	//MessageBox( NULL, TEXT( "CheckDeviceFormat OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->CheckDeviceFormat(Adapter,DeviceType,AdapterFormat,Usage,RType,CheckFormat));
}

HRESULT __stdcall OTM_IDirect3D9::CheckDeviceMultiSampleType(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels)
{
	//MessageBox( NULL, TEXT( "CheckDeviceMultiSampleType OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->CheckDeviceMultiSampleType(Adapter,DeviceType,SurfaceFormat,Windowed,MultiSampleType,pQualityLevels));
}

HRESULT __stdcall OTM_IDirect3D9::CheckDepthStencilMatch(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	//MessageBox( NULL, TEXT( "CheckDepthStencilMatch OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->CheckDepthStencilMatch(Adapter,DeviceType,AdapterFormat,RenderTargetFormat,DepthStencilFormat));
}

HRESULT __stdcall OTM_IDirect3D9::CheckDeviceFormatConversion(UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
	//MessageBox( NULL, TEXT( "CheckDeviceFormatConversion OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->CheckDeviceFormatConversion(Adapter,DeviceType,SourceFormat,TargetFormat));
}

HRESULT __stdcall OTM_IDirect3D9::GetDeviceCaps(UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps)
{
	//MessageBox( NULL, TEXT( "GetDeviceCaps OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->GetDeviceCaps(Adapter,DeviceType,pCaps));
}

HMONITOR __stdcall OTM_IDirect3D9::GetAdapterMonitor(UINT Adapter)
{
	//MessageBox( NULL, TEXT( "GetAdapterMonitor OTM_IDirect3D9" ), TEXT( "Timer" ), MB_OK );
    return(m_pIDirect3D9->GetAdapterMonitor(Adapter));
}

HRESULT __stdcall OTM_IDirect3D9::CreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
{

	// we intercept this call and provide our own "fake" Device Object
	HRESULT hres = m_pIDirect3D9->CreateDevice( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
    
	// Create our own Device object and store it in global pointer
	// note: the object will delete itself once Ref count is zero (similar to COM objects)

	if (gl_pIDirect3DDevice9!=NULL) gl_ErrorState |= OTM_ERROR_MULTIPLE_IDirect3DDevice9;
	gl_pIDirect3DDevice9 =  new OTM_IDirect3DDevice9(*ppReturnedDeviceInterface,  OTM_Server);
	

	// store our pointer (the fake one) for returning it to the calling progam
	*ppReturnedDeviceInterface = gl_pIDirect3DDevice9;

	return(hres); 
}
