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



#ifndef OTM_IDirect3DTexture9_H
#define OTM_IDirect3DTexture9_H

#include <d3d9.h>
#include <d3dx9.h>

#include "OTM_Defines.h"




//DWORD QuickChecksum(DWORD *pData, int size);

interface OTM_IDirect3DTexture9 : public IDirect3DTexture9
{
	OTM_IDirect3DTexture9(IDirect3DTexture9 **ppTex, IDirect3DDevice9 *pIDirect3DDevice9, UINT Width, UINT Height, D3DFORMAT Format)
	{
	  //Message("OTM_IDirect3DTexture9( %lu, %lu): %lu\n", ppTex, pIDirect3DDevice9, this);
		m_D3Dtex = *ppTex; //Texture which will be displayed and will be passed to the game
		m_D3Ddev = pIDirect3DDevice9; //device pointer
		CrossRef_D3Dtex = NULL; //cross reference
		// fake texture: store the pointer to the original OTM_IDirect3DTexture9 object, needed if a fake texture is deselected
		// original texture: stores the pointer to the fake texture object, is needed if original texture is deleted (that fake texture can be replaced, if original texture is loaded again)
		Reference = -1; //need for fast deleting
    Hash = 0u;
    FAKE = false;
	}

	// callback interface
	IDirect3DTexture9 *m_D3Dtex;
	OTM_IDirect3DTexture9 *CrossRef_D3Dtex;
	IDirect3DDevice9 *m_D3Ddev;
	int Reference;
	MyTypeHash Hash;
  bool FAKE;

	// original interface
    STDMETHOD(QueryInterface) (REFIID riid, void** ppvObj);
    STDMETHOD_(ULONG,AddRef)();
    STDMETHOD_(ULONG,Release)();
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice);
    STDMETHOD(SetPrivateData)(REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags);
    STDMETHOD(GetPrivateData)(REFGUID refguid,void* pData,DWORD* pSizeOfData);
    STDMETHOD(FreePrivateData)(REFGUID refguid);
    STDMETHOD_(DWORD, SetPriority)(DWORD PriorityNew);
    STDMETHOD_(DWORD, GetPriority)();
    STDMETHOD_(void, PreLoad)();
    STDMETHOD_(D3DRESOURCETYPE, GetType)();
    STDMETHOD_(DWORD, SetLOD)(DWORD LODNew);
    STDMETHOD_(DWORD, GetLOD)();
    STDMETHOD_(DWORD, GetLevelCount)();
    STDMETHOD(SetAutoGenFilterType)(D3DTEXTUREFILTERTYPE FilterType);
    STDMETHOD_(D3DTEXTUREFILTERTYPE, GetAutoGenFilterType)();
    STDMETHOD_(void, GenerateMipSubLevels)();
    STDMETHOD(GetLevelDesc)(UINT Level,D3DSURFACE_DESC *pDesc);
    STDMETHOD(GetSurfaceLevel)(UINT Level,IDirect3DSurface9** ppSurfaceLevel);
    STDMETHOD(LockRect)(UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
    STDMETHOD(UnlockRect)(UINT Level);
    STDMETHOD(AddDirtyRect)(CONST RECT* pDirtyRect);
};



inline void UnswitchTextures(OTM_IDirect3DTexture9 *pTexture)
{
  OTM_IDirect3DTexture9* CrossRef = pTexture->CrossRef_D3Dtex;
  if (CrossRef!=NULL)
  {
    //switch textures back
    IDirect3DTexture9* cpy = pTexture->m_D3Dtex;
    pTexture->m_D3Dtex = CrossRef->m_D3Dtex;
    CrossRef->m_D3Dtex = cpy;

    // cancel the link
    CrossRef->CrossRef_D3Dtex = NULL;
    pTexture->CrossRef_D3Dtex = NULL;
  }
}

inline int SwitchTextures( OTM_IDirect3DTexture9 *pTexture1, OTM_IDirect3DTexture9 *pTexture2)
{
  if (pTexture1->m_D3Ddev == pTexture2->m_D3Ddev && pTexture1->CrossRef_D3Dtex == NULL && pTexture2->CrossRef_D3Dtex == NULL)
  {
    //make cross reference
    pTexture1->CrossRef_D3Dtex = pTexture2;
    pTexture2->CrossRef_D3Dtex = pTexture1;

    //switch textures
    IDirect3DTexture9* cpy = pTexture2->m_D3Dtex;
    pTexture2->m_D3Dtex = pTexture1->m_D3Dtex;
    pTexture1->m_D3Dtex = cpy;
    return (RETURN_OK);
  }
  else return (RETURN_TEXTURE_NOT_SWITCHED);
}


#endif
