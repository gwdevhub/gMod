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


#include "../uMod_DXMain/uMod_Main.h"

#include "uMod_IDirect3DDevice9.h"
#include "uMod_IDirect3DTexture9.h"
#include "uMod_IDirect3DSurface9.h"


HRESULT uMod_IDirect3DSurface9::QueryInterface(REFIID riid, void** ppvObj)
{
  //Message( "uMod_IDirect3DSurface9::uMod_IDirect3DSurface9(): %p\n", this);
  if (riid==IID_IDirect3D9)
  {
    // This function should never be called with IID_IDirect3D9 by the game
    // thus this call comes from our own dll to ask for the texture type

    *ppvObj = this;
    return (0x01000000L);
  }
  HRESULT hRes = m_D3Dsurf->QueryInterface(riid, ppvObj);

  // return this object
  if (*ppvObj==m_D3Dsurf)
  {
    RefCounter++;
    *ppvObj=this;
  }


  return (hRes);
}

ULONG uMod_IDirect3DSurface9::AddRef()
{
  //Message( "uMod_IDirect3DSurface9::AddRef(): %p\n", this);
  RefCounter++;
  return m_D3Dsurf->AddRef();
}

ULONG uMod_IDirect3DSurface9::Release()
{
  //Message( "uMod_IDirect3DSurface9::Release(): %p\n", this);
  // this object might be created by uMod_IDirect3DTexture9::GetSurfaceLevel
  // thus the RefCounter might be zero although m_D3Dsurf->Release() does not delete
  // the m_D3Dsurf object
  ULONG counter = m_D3Dsurf->Release();
  if (--RefCounter==0)  delete this;
  return counter;
}

HRESULT uMod_IDirect3DSurface9::GetDevice( IDirect3DDevice9** ppDevice)
{
  //Message( "uMod_IDirect3DSurface9::GetDevice(): %p\n", this);
  *ppDevice = m_D3Ddev;
  return 0;
}

HRESULT uMod_IDirect3DSurface9::SetPrivateData( REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
  //Message( "uMod_IDirect3DSurface9::SetPrivateData(): %p\n", this);
  return m_D3Dsurf->SetPrivateData( refguid, pData, SizeOfData, Flags);
}

HRESULT uMod_IDirect3DSurface9::GetPrivateData( REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
  //Message( "uMod_IDirect3DSurface9::GetPrivateData(): %p\n", this);
  return m_D3Dsurf->GetPrivateData( refguid, pData, pSizeOfData);
}

HRESULT uMod_IDirect3DSurface9::FreePrivateData( REFGUID refguid)
{
  //Message( "uMod_IDirect3DSurface9::FreePrivateData(): %p\n", this);
  return m_D3Dsurf->FreePrivateData( refguid);
}

DWORD uMod_IDirect3DSurface9::SetPriority(DWORD PriorityNew)
{
  //Message( "uMod_IDirect3DSurface9::SetPriority(): %p\n", this);
  return m_D3Dsurf->SetPriority(PriorityNew);
}
DWORD uMod_IDirect3DSurface9::GetPriority()
{
  //Message( "uMod_IDirect3DSurface9::GetPriority(): %p\n", this);
  return m_D3Dsurf->GetPriority();
}

void uMod_IDirect3DSurface9::PreLoad()
{
  //Message( "uMod_IDirect3DSurface9::PreLoad(): %p\n", this);
  return m_D3Dsurf->PreLoad();
}

D3DRESOURCETYPE uMod_IDirect3DSurface9::GetType()
{
  //Message( "uMod_IDirect3DSurface9::GetType(): %p\n", this);
  return m_D3Dsurf->GetType();
}

HRESULT uMod_IDirect3DSurface9::GetContainer( REFIID riid,void** ppContainer)
{
  //Message( "uMod_IDirect3DSurface9::GetContainer(): %p\n", this);
  HRESULT ret = m_D3Dsurf->GetContainer( riid, ppContainer);

  //return the uMod_IDirect3DTexture9 object
  if (m_D3DTex!=NULL && *ppContainer==m_D3DTex->m_D3Dtex)
  {
    RefCounter++;
    *ppContainer = m_D3DTex;
  }
  if (m_D3DCubeTex!=NULL && *ppContainer==m_D3DCubeTex->m_D3Dtex)
  {
    RefCounter++;
    *ppContainer = m_D3DCubeTex;
  }

  return ret;
}

HRESULT uMod_IDirect3DSurface9::GetDesc( D3DSURFACE_DESC *pDesc)
{
  //Message( "uMod_IDirect3DSurface9::GetDesc(): %p\n", this);
  return m_D3Dsurf->GetDesc( pDesc);
}

HRESULT uMod_IDirect3DSurface9::LockRect( D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
  //Message( "uMod_IDirect3DSurface9::LockRect(): %p\n", this);
  return m_D3Dsurf->LockRect( pLockedRect, pRect, Flags);
}

HRESULT uMod_IDirect3DSurface9::UnlockRect()
{
  //Message( "uMod_IDirect3DSurface9::UnlockRect(): %p\n", this);
  HRESULT ret = m_D3Dsurf->UnlockRect();

  if (m_D3DTex!=NULL && !m_D3DTex->FAKE)
    m_D3DTex->Dirty=1;
  else if (m_D3DCubeTex!=NULL && !m_D3DCubeTex->FAKE)
    m_D3DCubeTex->Dirty = 1;

  return ret;
}

HRESULT uMod_IDirect3DSurface9::GetDC( HDC *phdc)
{
  //Message( "uMod_IDirect3DSurface9::GetDC(): %p\n", this);
  return m_D3Dsurf->GetDC( phdc);
}

HRESULT uMod_IDirect3DSurface9::ReleaseDC( HDC hdc)
{
  //Message( "uMod_IDirect3DSurface9::ReleaseDC(): %p\n", this);
  HRESULT ret = m_D3Dsurf->ReleaseDC( hdc);

  if (m_D3DTex!=NULL && !m_D3DTex->FAKE)
    m_D3DTex->Dirty=1;
  else if (m_D3DCubeTex!=NULL && !m_D3DCubeTex->FAKE)
    m_D3DCubeTex->Dirty = 1;

  return ret;
}


