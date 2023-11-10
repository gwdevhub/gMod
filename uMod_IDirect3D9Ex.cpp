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



#define IDirect3D9 IDirect3D9Ex
#define uMod_IDirect3D9 uMod_IDirect3D9Ex
#define m_pIDirect3D9 m_pIDirect3D9Ex
#define PRE_MESSAGE "uMod_IDirect3D9Ex"

#include "uMod_IDirect3D9.cpp"


HRESULT __stdcall uMod_IDirect3D9Ex::CreateDeviceEx( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode, IDirect3DDevice9Ex **ppReturnedDeviceInterface)
{
  Message( "uMod_IDirect3D9Ex::CreateDeviceEx: %lu\n", this);
  // we intercept this call and provide our own "fake" Device Object
  HRESULT hres = m_pIDirect3D9Ex->CreateDeviceEx( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);

  int count = 1;
  if (pPresentationParameters!=NULL) count  = pPresentationParameters->BackBufferCount;
  uMod_IDirect3DDevice9Ex *pIDirect3DDevice9Ex = new uMod_IDirect3DDevice9Ex(*ppReturnedDeviceInterface,  uMod_Server, count);

  // store our pointer (the fake one) for returning it to the calling program
  *ppReturnedDeviceInterface = pIDirect3DDevice9Ex;

  return(hres);
  return (m_pIDirect3D9Ex->CreateDeviceEx( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface));
}

HRESULT __stdcall uMod_IDirect3D9Ex::EnumAdapterModesEx( UINT Adapter, const D3DDISPLAYMODEFILTER *pFilter, UINT Mode, D3DDISPLAYMODEEX *pMode)
{
  return (m_pIDirect3D9Ex->EnumAdapterModesEx( Adapter, pFilter, Mode, pMode));
}

HRESULT __stdcall uMod_IDirect3D9Ex::GetAdapterDisplayModeEx( UINT Adapter, D3DDISPLAYMODEEX *pMode, D3DDISPLAYROTATION *pRotation)
{
  return (m_pIDirect3D9Ex->GetAdapterDisplayModeEx( Adapter, pMode, pRotation));
}

HRESULT __stdcall uMod_IDirect3D9Ex::GetAdapterLUID( UINT Adapter, LUID *pLUID)
{
  return (m_pIDirect3D9Ex->GetAdapterLUID( Adapter, pLUID));
}

UINT __stdcall uMod_IDirect3D9Ex::GetAdapterModeCountEx( UINT Adapter, const D3DDISPLAYMODEFILTER *pFilter)
{
  return (m_pIDirect3D9Ex->GetAdapterModeCountEx( Adapter, pFilter));
}

