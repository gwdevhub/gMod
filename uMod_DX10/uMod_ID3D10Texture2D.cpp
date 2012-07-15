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
#include "uMod_ID3D10Texture2D.h"


uMod_ID3D10Texture2D::uMod_ID3D10Texture2D(ID3D10Texture2D *pOriginal)
{
  Message( "uMod_ID3D10Texture1D::uMod_ID3D10Texture1D( %lu): %lu\n", pOriginal, this);
  m_ID3D10Texture2D = pOriginal;
}
uMod_ID3D10Texture2D::~uMod_ID3D10Texture2D()
{
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Texture2D::QueryInterface( REFIID riid,  void** ppvObject)
{
  return m_ID3D10Texture2D->QueryInterface( riid, ppvObject);
}

ULONG STDMETHODCALLTYPE uMod_ID3D10Texture2D::AddRef(void)
{
  return m_ID3D10Texture2D->AddRef();
}
ULONG STDMETHODCALLTYPE uMod_ID3D10Texture2D::Release( void)
{
  ULONG count = m_ID3D10Texture2D->Release();

  // in case no further Ref is there, the Original Object has deleted itself
  if (count == 0)
  {
    delete(this);
  }
  return(count);
}

void STDMETHODCALLTYPE uMod_ID3D10Texture2D::GetDevice( ID3D10Device **ppDevice)
{
  return m_ID3D10Texture2D->GetDevice( ppDevice);
}
HRESULT STDMETHODCALLTYPE uMod_ID3D10Texture2D::GetPrivateData( REFGUID guid, UINT *pDataSize, void *pData)
{
  return m_ID3D10Texture2D->GetPrivateData( guid, pDataSize, pData);
}
HRESULT STDMETHODCALLTYPE uMod_ID3D10Texture2D::SetPrivateData( REFGUID guid, UINT DataSize, const void *pData)
{
  return m_ID3D10Texture2D->SetPrivateData( guid, DataSize, pData);
}
HRESULT STDMETHODCALLTYPE uMod_ID3D10Texture2D::SetPrivateDataInterface( REFGUID guid, const IUnknown *pData)
{
  return m_ID3D10Texture2D->SetPrivateDataInterface( guid, pData);
}

void STDMETHODCALLTYPE uMod_ID3D10Texture2D::GetType( D3D10_RESOURCE_DIMENSION *rType)
{
  return m_ID3D10Texture2D->GetType( rType);
}
void STDMETHODCALLTYPE uMod_ID3D10Texture2D::SetEvictionPriority( UINT EvictionPriority)
{
  return m_ID3D10Texture2D->SetEvictionPriority( EvictionPriority);
}
UINT STDMETHODCALLTYPE uMod_ID3D10Texture2D::GetEvictionPriority(void)
{
  return m_ID3D10Texture2D->GetEvictionPriority();
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Texture2D::Map( UINT Subresource, D3D10_MAP MapType, UINT MapFlags, D3D10_MAPPED_TEXTURE2D *pMappedTex2D)
{
  return m_ID3D10Texture2D->Map( Subresource, MapType, MapFlags, pMappedTex2D);
}
void STDMETHODCALLTYPE uMod_ID3D10Texture2D::Unmap( UINT Subresource)
{
  return m_ID3D10Texture2D->Unmap( Subresource);
}
void STDMETHODCALLTYPE uMod_ID3D10Texture2D::GetDesc( D3D10_TEXTURE2D_DESC *pDesc)
{
  return m_ID3D10Texture2D->GetDesc( pDesc);
}


