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
 * some function (e.g. AddReff()) are presumed to work on the texture object which belong to them
 * if this texture was switched, we must redirect this calls to the CrossRef_D3Dtex texture object
 */


#include "../uMod_DXMain/uMod_Main.h"
#include "../uMod_DXMain/uMod_TextureFunction.h"
#include "uMod_IDirect3DDevice9.h"
#include "uMod_IDirect3DTexture9.h"
#include "uMod_IDirect3DSurface9.h"

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::QueryInterface(REFIID riid, void** ppvObj)
{
  if (riid==IID_IDirect3D9)
  {
    // This function should never be called with IID_IDirect3D9 by the game
    // thus this call comes from our own dll to ask for the texture type
    // 0x01000000L == uMod_IDirect3DTexture9
    // 0x01000001L == uMod_IDirect3DVolumeTexture9
    // 0x01000002L == uMod_IDirect3DCubeTexture9

    *ppvObj = this;
    return (0x01000000L);
  }
  HRESULT hRes = m_D3Dtex->QueryInterface(riid, ppvObj);
  if (*ppvObj==m_D3Dtex) *ppvObj=this;

  return (hRes);
}

//this function yields for the non switched texture object
ULONG APIENTRY uMod_IDirect3DTexture9::AddRef()
{
  if (FAKE) return (1); //bug, this case should never happen
  return (m_D3Dtex->AddRef());
}

//this function yields for the non switched texture object
ULONG APIENTRY uMod_IDirect3DTexture9::Release()
{
  //Message("uMod_IDirect3DTexture9::Release(): %p\n", this);

  void *cpy;
  long ret = m_D3Ddev->QueryInterface( IID_IDirect3DTexture9, &cpy);

  ULONG count;
  if (FAKE)
  {
    UnswitchTextures( this);
    count = m_D3Dtex->Release(); //count must be zero, cause we don't call AddRef of fake_textures
  }
  else
  {
    count = m_D3Dtex->Release();
  }

  if (count==0) //if this texture is released, we clean up
  {
    if (ret == 0x01000000L)
    {
      ((uMod_IDirect3DDevice9*) m_D3Ddev)->GetuMod_Client()->RemoveTexture(this); // remove this texture from the texture client
    }
    else
    {
      ((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->GetuMod_Client()->RemoveTexture(this); // remove this texture from the texture client
    }

    delete(this);
  }

  //Message("uMod_IDirect3DTexture9::Release() end: %p\n", this);
	return (count);
}

HRESULT APIENTRY uMod_IDirect3DTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{
	*ppDevice = m_D3Ddev;
	return D3D_OK;
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::SetPrivateData(REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
	return (m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::GetPrivateData(REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
	return (m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::FreePrivateData(REFGUID refguid)
{
	return (m_D3Dtex->FreePrivateData(refguid));
}

DWORD APIENTRY uMod_IDirect3DTexture9::SetPriority(DWORD PriorityNew)
{
	return (m_D3Dtex->SetPriority(PriorityNew));
}

DWORD APIENTRY uMod_IDirect3DTexture9::GetPriority()
{
	return (m_D3Dtex->GetPriority());
}

void APIENTRY uMod_IDirect3DTexture9::PreLoad()
{
	m_D3Dtex->PreLoad();
}

D3DRESOURCETYPE APIENTRY uMod_IDirect3DTexture9::GetType()
{
	return (m_D3Dtex->GetType());
}

DWORD APIENTRY uMod_IDirect3DTexture9::SetLOD(DWORD LODNew)
{
	return (m_D3Dtex->SetLOD(LODNew));
}

DWORD APIENTRY uMod_IDirect3DTexture9::GetLOD()
{
	return (m_D3Dtex->GetLOD());
}

DWORD APIENTRY uMod_IDirect3DTexture9::GetLevelCount()
{
	return (m_D3Dtex->GetLevelCount());
}

HRESULT APIENTRY uMod_IDirect3DTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	return (m_D3Dtex->SetAutoGenFilterType(FilterType));
}

D3DTEXTUREFILTERTYPE APIENTRY uMod_IDirect3DTexture9::GetAutoGenFilterType()
{
	return (m_D3Dtex->GetAutoGenFilterType());
}

void APIENTRY uMod_IDirect3DTexture9::GenerateMipSubLevels()
{
	m_D3Dtex->GenerateMipSubLevels();
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc)
{
	return (m_D3Dtex->GetLevelDesc(Level, pDesc));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel)
{
  Message( "uMod_IDirect3DTexture9::GetSurfaceLevel( %u, %p): %p\n", Level, *ppSurfaceLevel, this);
  IDirect3DSurface9* pSurf;
  uMod_IDirect3DSurface9* puMod_Surf;
  HRESULT ret = (m_D3Dtex->GetSurfaceLevel(Level, &pSurf));

  if (SUCCEEDED(ret))
  {
    puMod_Surf = new uMod_IDirect3DSurface9( pSurf, this, NULL, m_D3Ddev);
    *ppSurfaceLevel = puMod_Surf;
  }
  else *ppSurfaceLevel = pSurf;

  return ret;
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::LockRect(UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
	return (m_D3Dtex->LockRect(Level, pLockedRect, pRect, Flags));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::UnlockRect(UINT Level)
{
  Dirty =1;
  return (m_D3Dtex->UnlockRect(Level));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::AddDirtyRect(CONST RECT* pDirtyRect)
{
	return (m_D3Dtex->AddDirtyRect(pDirtyRect));
}


int uMod_IDirect3DTexture9::ComputetHash( bool compute_crc)
{
  if (FAKE) return (RETURN_BAD_ARGUMENT);
  IDirect3DTexture9 *pTexture = m_D3Dtex;


  IDirect3DSurface9 *surface = NULL;
  if (pTexture->GetSurfaceLevel(0, &surface)!=D3D_OK)
  {
    Message("uMod_IDirect3DTexture9::GetHash() Failed: GetSurfaceLevel\n");
    return (RETURN_LockRect_FAILED);
  }

  if (compute_crc) InitCRC32(CRC32);
  else CRC32 = 0u;
  InitCRC64(CRC64);

  void *cpy;
  long ret = m_D3Ddev->QueryInterface( IID_IDirect3DTexture9, &cpy);
  if (ret == 0x01000000L) ret = ((uMod_IDirect3DDevice9*)m_D3Ddev)->ComputeCRC( CRC64, CRC32, surface, compute_crc);
  else ret = ((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->ComputeCRC( CRC64, CRC32, surface, compute_crc);

  surface->Release();
  return (ret);
}
