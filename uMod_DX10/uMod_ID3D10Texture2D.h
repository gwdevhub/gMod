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


#ifndef UMOD_ID3D10TEXTURE2D_H_
#define UMOD_ID3D10TEXTURE2D_H_

#include <d3d10_1.h>
#include <d3d10.h>
#include <D3D10Misc.h>
#include <d3dx10.h>

class uMod_ID3D10Texture2D : public  ID3D10Texture2D
{
public:
  uMod_ID3D10Texture2D(ID3D10Texture2D *pOriginal);
  virtual ~uMod_ID3D10Texture2D();

  HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid,  void** ppvObject);
  ULONG STDMETHODCALLTYPE AddRef(void);
  ULONG STDMETHODCALLTYPE Release( void);

  void STDMETHODCALLTYPE GetDevice( ID3D10Device **ppDevice);
  HRESULT STDMETHODCALLTYPE GetPrivateData( REFGUID guid, UINT *pDataSize, void *pData);
  HRESULT STDMETHODCALLTYPE SetPrivateData( REFGUID guid, UINT DataSize, const void *pData);
  HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( REFGUID guid, const IUnknown *pData);

  void STDMETHODCALLTYPE GetType( D3D10_RESOURCE_DIMENSION *rType);
  void STDMETHODCALLTYPE SetEvictionPriority( UINT EvictionPriority);
  UINT STDMETHODCALLTYPE GetEvictionPriority(void);

  HRESULT STDMETHODCALLTYPE Map( UINT Subresource, D3D10_MAP MapType, UINT MapFlags, D3D10_MAPPED_TEXTURE2D *ppData);
  void STDMETHODCALLTYPE Unmap( UINT Subresource);
  void STDMETHODCALLTYPE GetDesc( D3D10_TEXTURE2D_DESC *pDesc);

  ID3D10Texture2D *m_ID3D10Texture2D;
};



#endif /* UMOD_ID3D10TEXTURE2D_H_ */
