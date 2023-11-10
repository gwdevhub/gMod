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


/*
 *
 * BIG THANKS TO Matthew L (Azorbix)
 * (Direct3D StarterKit v3.0)
 *
 */


#ifndef uMod_IDirect3DCubeTexture9_H
#define uMod_IDirect3DCubeTexture9_H

#include <d3d9.h>
#include <d3dx9.h>

#include "uMod_Defines.h"



interface uMod_IDirect3DCubeTexture9 : public IDirect3DCubeTexture9
{
	uMod_IDirect3DCubeTexture9(IDirect3DCubeTexture9 **ppTex, IDirect3DDevice9 *pIDirect3DDevice9)
	{
		m_D3Dtex = *ppTex; //Texture which will be displayed and will be passed to the game
		m_D3Ddev = pIDirect3DDevice9; //device pointer
		CrossRef_D3Dtex = NULL; //cross reference
		// fake texture: store the pointer to the original uMod_IDirect3DCubeTexture9 object, needed if a fake texture is unselected
		// original texture: stores the pointer to the fake texture object, is needed if original texture is deleted,
		// thus the fake texture can also be deleted
		Reference = -1; //need for fast deleting
    Hash = 0u;
    FAKE = false;
	}

	// callback interface
	IDirect3DCubeTexture9 *m_D3Dtex;
	uMod_IDirect3DCubeTexture9 *CrossRef_D3Dtex;
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

    STDMETHOD(AddDirtyRect)(D3DCUBEMAP_FACES FaceType, CONST RECT* pDirtyRect);
    STDMETHOD(GetLevelDesc)(UINT Level, D3DSURFACE_DESC *pDesc);
    STDMETHOD(GetCubeMapSurface)(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9 **ppCubeMapSurface);
    STDMETHOD(LockRect)( D3DCUBEMAP_FACES FaceType, UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags);
    STDMETHOD(UnlockRect)(D3DCUBEMAP_FACES FaceType, UINT Level);


    int GetHash(MyTypeHash &hash);
};



inline void UnswitchTextures(uMod_IDirect3DCubeTexture9 *pTexture)
{
  uMod_IDirect3DCubeTexture9* CrossRef = pTexture->CrossRef_D3Dtex;
  if (CrossRef!=NULL)
  {
    // switch textures back
    IDirect3DCubeTexture9* cpy = pTexture->m_D3Dtex;
    pTexture->m_D3Dtex = CrossRef->m_D3Dtex;
    CrossRef->m_D3Dtex = cpy;

    // cancel the link
    CrossRef->CrossRef_D3Dtex = NULL;
    pTexture->CrossRef_D3Dtex = NULL;
  }
}

inline int SwitchTextures( uMod_IDirect3DCubeTexture9 *pTexture1, uMod_IDirect3DCubeTexture9 *pTexture2)
{
  if (pTexture1->m_D3Ddev == pTexture2->m_D3Ddev && pTexture1->CrossRef_D3Dtex == NULL && pTexture2->CrossRef_D3Dtex == NULL)
  {
    // make cross reference
    pTexture1->CrossRef_D3Dtex = pTexture2;
    pTexture2->CrossRef_D3Dtex = pTexture1;

    // switch textures
    IDirect3DCubeTexture9* cpy = pTexture2->m_D3Dtex;
    pTexture2->m_D3Dtex = pTexture1->m_D3Dtex;
    pTexture1->m_D3Dtex = cpy;
    return (RETURN_OK);
  }
  else return (RETURN_TEXTURE_NOT_SWITCHED);
}


#endif
