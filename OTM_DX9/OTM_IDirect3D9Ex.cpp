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
along with OpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/





#include "OTM_Main.h"



#define IDirect3D9 IDirect3D9Ex
#define OTM_IDirect3D9 OTM_IDirect3D9Ex
#define m_pIDirect3D9 m_pIDirect3D9Ex
#define PRE_MESSAGE "OTM_IDirect3D9Ex"

#include "OTM_IDirect3D9.cpp"


HRESULT __stdcall OTM_IDirect3D9Ex::CreateDeviceEx( UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode, IDirect3DDevice9Ex **ppReturnedDeviceInterface)
{
  Message( "OTM_IDirect3D9Ex::CreateDeviceEx: %lu\n", this);
  // we intercept this call and provide our own "fake" Device Object
  HRESULT hres = m_pIDirect3D9Ex->CreateDeviceEx( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);

  OTM_IDirect3DDevice9Ex *pIDirect3DDevice9Ex = new OTM_IDirect3DDevice9Ex(*ppReturnedDeviceInterface,  OTM_Server);

  // store our pointer (the fake one) for returning it to the calling program
  *ppReturnedDeviceInterface = pIDirect3DDevice9Ex;

  return(hres);
  return (m_pIDirect3D9Ex->CreateDeviceEx( Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface));
}

HRESULT __stdcall OTM_IDirect3D9Ex::EnumAdapterModesEx( UINT Adapter, const D3DDISPLAYMODEFILTER *pFilter, UINT Mode, D3DDISPLAYMODEEX *pMode)
{
  return (m_pIDirect3D9Ex->EnumAdapterModesEx( Adapter, pFilter, Mode, pMode));
}

HRESULT __stdcall OTM_IDirect3D9Ex::GetAdapterDisplayModeEx( UINT Adapter, D3DDISPLAYMODEEX *pMode, D3DDISPLAYROTATION *pRotation)
{
  return (m_pIDirect3D9Ex->GetAdapterDisplayModeEx( Adapter, pMode, pRotation));
}

HRESULT __stdcall OTM_IDirect3D9Ex::GetAdapterLUID( UINT Adapter, LUID *pLUID)
{
  return (m_pIDirect3D9Ex->GetAdapterLUID( Adapter, pLUID));
}

UINT __stdcall OTM_IDirect3D9Ex::GetAdapterModeCountEx( UINT Adapter, const D3DDISPLAYMODEFILTER *pFilter)
{
  return (m_pIDirect3D9Ex->GetAdapterModeCountEx( Adapter, pFilter));
}

