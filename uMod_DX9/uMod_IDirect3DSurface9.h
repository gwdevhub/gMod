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


#ifndef UMOD_IDIRECT3DSURFACE9_H_
#define UMOD_IDIRECT3DSURFACE9_H_

#include <d3d9.h>
#include <d3dx9.h>

#include "..\uMod_DXMain\uMod_Defines.h"



class uMod_IDirect3DSurface9 : public IDirect3DSurface9
{
public:
  uMod_IDirect3DSurface9(IDirect3DSurface9 *pSurf, uMod_IDirect3DTexture9 *pTex, uMod_IDirect3DCubeTexture9 *pCubeTex, IDirect3DDevice9 *pIDirect3DDevice9)
  {
    Message( "uMod_IDirect3DSurface9::uMod_IDirect3DSurface9(%p, %p, %p %p): %p\n", pSurf, pTex, pCubeTex, pIDirect3DDevice9, this);
    m_D3Dsurf = pSurf;
    m_D3DTex = pTex; //Texture which will be displayed and will be passed to the game
    m_D3DCubeTex = pCubeTex; //Texture which will be displayed and will be passed to the game
    m_D3Ddev = pIDirect3DDevice9; //device pointer
    RefCounter = 1;
  }

  IDirect3DSurface9 *m_D3Dsurf; //!< pointer to the real IDirect3DSurface9 object
  uMod_IDirect3DTexture9 *m_D3DTex; //!< pointer to the parent uMod_IDirect3DTexture9 object
  uMod_IDirect3DCubeTexture9 *m_D3DCubeTex; //!< pointer to the parent uMod_IDirect3DTexture9 object
  IDirect3DDevice9 *m_D3Ddev; //!< pointer to the device
  int RefCounter;

  // original interface
    STDMETHOD(QueryInterface) (REFIID riid, void** ppvObj);
    STDMETHOD_(ULONG,AddRef)();
    STDMETHOD_(ULONG,Release)();

    STDMETHOD(GetDevice)( IDirect3DDevice9** ppDevice);
    STDMETHOD(SetPrivateData)( REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags);
    STDMETHOD(GetPrivateData)( REFGUID refguid,void* pData,DWORD* pSizeOfData);
    STDMETHOD(FreePrivateData)( REFGUID refguid);
    STDMETHOD_(DWORD, SetPriority)(DWORD PriorityNew);
    STDMETHOD_(DWORD, GetPriority)();
    STDMETHOD_(void, PreLoad)() ;
    STDMETHOD_(D3DRESOURCETYPE, GetType)();
    STDMETHOD(GetContainer)( REFIID riid,void** ppContainer);
    STDMETHOD(GetDesc)( D3DSURFACE_DESC *pDesc);
    STDMETHOD(LockRect)( D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
    STDMETHOD(UnlockRect)();
    STDMETHOD(GetDC)( HDC *phdc);
    STDMETHOD(ReleaseDC)( HDC hdc);
};


#endif /* UMOD_IDIRECT3DSURFACE9_H_ */
