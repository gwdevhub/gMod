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


#ifndef uMod_IDirect3DTexture9_H
#define uMod_IDirect3DTexture9_H

#include <d3d9.h>
#include <d3dx9.h>

#include "..\uMod_DXMain\uMod_Defines.h"



class uMod_IDirect3DTexture9 : public IDirect3DTexture9
{
public:
	uMod_IDirect3DTexture9(IDirect3DTexture9 **ppTex, IDirect3DDevice9 *pIDirect3DDevice9)
	{
    //Message( "uMod_IDirect3DTexture9::uMod_IDirect3DTexture9(%p %p): %p\n", *ppTex, pIDirect3DDevice9, this);
		m_D3Dtex = *ppTex; //Texture which will be displayed and will be passed to the game
		m_D3Ddev = pIDirect3DDevice9; //device pointer
		CrossRef_D3Dtex = NULL; //cross reference
		// fake texture: store the pointer to the original uMod_IDirect3DTexture9 object, needed if a fake texture is unselected
		// original texture: stores the pointer to the fake texture object, is needed if original texture is deleted,
		// thus the fake texture can also be deleted
		Reference = -1; //need for fast deleting
    CRC64 = 0u;
    CRC32 = 0u;
    FAKE = false;
    Dirty = 1;
	}

  IDirect3DTexture9 *m_D3Dtex; //!< pointer to the real IDirect3DCubeTexture9 object
  uMod_IDirect3DTexture9 *CrossRef_D3Dtex; //!< cross reference from the fake texture to the game texture and vice versa
  IDirect3DDevice9 *m_D3Ddev; //!< pointer to the device
  int Reference; //!< Index in the vector (needed for a fast delete).
  DWORD64 CRC64; //!< computed hash value for this game texture.
  DWORD32 CRC32; //!< computed crc32 value for this game texture.
  bool FAKE; //!< True if this texture is was loaded by uMod (fake texture)
  unsigned char Dirty;

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

    int ComputetHash( bool compute_crc);
};

#endif
