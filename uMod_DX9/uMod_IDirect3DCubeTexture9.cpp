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
#include "uMod_IDirect3DCubeTexture9.h"
#include "uMod_IDirect3DSurface9.h"

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DCubeTexture9::QueryInterface(REFIID riid, void** ppvObj)
{
  if (riid==IID_IDirect3D9)
  {
    // This function should never be called with IID_IDirect3D9 by the game
    // thus this call comes from our own dll to ask for the texture type
    // 0x01000000L == uMod_IDirect3DTexture9
    // 0x01000001L == uMod_IDirect3DVolumeTexture9
    // 0x01000002L == uMod_IDirect3DCubeTexture9

    *ppvObj = this;
    return (0x01000002L);
  }
  HRESULT hRes;
  if (CrossRef_D3Dtex!=NULL)
  {
    hRes = CrossRef_D3Dtex->m_D3Dtex->QueryInterface(riid, ppvObj);
    if (*ppvObj==CrossRef_D3Dtex->m_D3Dtex) *ppvObj=this;
  }
  else
  {
    hRes = m_D3Dtex->QueryInterface(riid, ppvObj);
    if (*ppvObj==m_D3Dtex) *ppvObj=this;
  }
  return (hRes);
}

//this function yields for the non switched texture object
ULONG APIENTRY uMod_IDirect3DCubeTexture9::AddRef()
{
  if (FAKE) return (1); //bug, this case should never happen
  if (CrossRef_D3Dtex!=NULL)
  {
    return (CrossRef_D3Dtex->m_D3Dtex->AddRef());
  }
  else return (m_D3Dtex->AddRef());
}

//this function yields for the non switched texture object
ULONG APIENTRY uMod_IDirect3DCubeTexture9::Release()
{
  Message("uMod_IDirect3DCubeTexture9::Release(): %lu\n", this);

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
    if (CrossRef_D3Dtex!=NULL) //if this texture is switched with a fake texture
    {
      uMod_IDirect3DCubeTexture9 *fake_texture = CrossRef_D3Dtex;
      count = fake_texture->m_D3Dtex->Release(); //release the original texture
      if (count==0) //if texture is released we switch the textures back
      {
        UnswitchTextures(this);
        if (ret == 0x01000000L)
        {
          if (((uMod_IDirect3DDevice9*) m_D3Ddev)->GetSingleCubeTexture()!=fake_texture) fake_texture->Release(); // we release the fake texture
        }
        else
        {
          if (((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->GetSingleCubeTexture()!=fake_texture) fake_texture->Release(); // we release the fake texture
        }
      }
    }
    else
    {
      count = m_D3Dtex->Release();
    }
  }

  if (count==0) //if this texture is released, we clean up
  {
    // if this texture is the LastCreatedTexture, the next time LastCreatedTexture would be added,
    // the hash of a non existing texture would be calculated
    if (ret == 0x01000000L)
    {
      if (((uMod_IDirect3DDevice9*) m_D3Ddev)->GetLastCreatedCubeTexture()==this) ((uMod_IDirect3DDevice9*) m_D3Ddev)->SetLastCreatedCubeTexture( NULL);
      else ((uMod_IDirect3DDevice9*) m_D3Ddev)->GetuMod_Client()->RemoveTexture(this); // remove this texture from the texture client
    }
    else
    {
      if (((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->GetLastCreatedCubeTexture()==this) ((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->SetLastCreatedCubeTexture( NULL);
      else ((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->GetuMod_Client()->RemoveTexture(this); // remove this texture from the texture client
    }

    delete(this);
  }
	return (count);
}

HRESULT APIENTRY uMod_IDirect3DCubeTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{
	*ppDevice = m_D3Ddev;
	return D3D_OK;
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DCubeTexture9::SetPrivateData(REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags));
	return (m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DCubeTexture9::GetPrivateData(REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData));
	return (m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DCubeTexture9::FreePrivateData(REFGUID refguid)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->FreePrivateData(refguid));
	return (m_D3Dtex->FreePrivateData(refguid));
}

DWORD APIENTRY uMod_IDirect3DCubeTexture9::SetPriority(DWORD PriorityNew)
{
	return (m_D3Dtex->SetPriority(PriorityNew));
}

DWORD APIENTRY uMod_IDirect3DCubeTexture9::GetPriority()
{
	return (m_D3Dtex->GetPriority());
}

void APIENTRY uMod_IDirect3DCubeTexture9::PreLoad()
{
	m_D3Dtex->PreLoad();
}

D3DRESOURCETYPE APIENTRY uMod_IDirect3DCubeTexture9::GetType()
{
	return (m_D3Dtex->GetType());
}

DWORD APIENTRY uMod_IDirect3DCubeTexture9::SetLOD(DWORD LODNew)
{
	return (m_D3Dtex->SetLOD(LODNew));
}

DWORD APIENTRY uMod_IDirect3DCubeTexture9::GetLOD()
{
	return (m_D3Dtex->GetLOD());
}

DWORD APIENTRY uMod_IDirect3DCubeTexture9::GetLevelCount()
{
	return (m_D3Dtex->GetLevelCount());
}

HRESULT APIENTRY uMod_IDirect3DCubeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	return (m_D3Dtex->SetAutoGenFilterType(FilterType));
}

D3DTEXTUREFILTERTYPE APIENTRY uMod_IDirect3DCubeTexture9::GetAutoGenFilterType()
{
	return (m_D3Dtex->GetAutoGenFilterType());
}

void APIENTRY uMod_IDirect3DCubeTexture9::GenerateMipSubLevels()
{
	m_D3Dtex->GenerateMipSubLevels();
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DCubeTexture9::AddDirtyRect(D3DCUBEMAP_FACES FaceType, CONST RECT* pDirtyRect)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->AddDirtyRect( FaceType, pDirtyRect));
  return (m_D3Dtex->AddDirtyRect( FaceType, pDirtyRect));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DCubeTexture9::GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->GetLevelDesc(Level, pDesc));
	return (m_D3Dtex->GetLevelDesc(Level, pDesc));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DCubeTexture9::GetCubeMapSurface(D3DCUBEMAP_FACES FaceType, UINT Level, IDirect3DSurface9 **ppCubeMapSurface)
{
  Message( "uMod_IDirect3DCubeTexture9::GetCubeMapSurface( %d, %u, %p): %p\n", FaceType,  Level, *ppCubeMapSurface, this);
  IDirect3DSurface9* pSurf;
  uMod_IDirect3DSurface9* puMod_Surf;
  HRESULT ret;
  if (CrossRef_D3Dtex!=NULL) ret = (CrossRef_D3Dtex->m_D3Dtex->GetCubeMapSurface( FaceType, Level, &pSurf));
  else ret = (m_D3Dtex->GetCubeMapSurface( FaceType, Level, &pSurf));

  if (SUCCEEDED(ret))
  {
    puMod_Surf = new uMod_IDirect3DSurface9( pSurf, NULL, this, m_D3Ddev);
    *ppCubeMapSurface = puMod_Surf;
  }
  else *ppCubeMapSurface = pSurf;

  return ret;
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DCubeTexture9::LockRect( D3DCUBEMAP_FACES FaceType, UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->LockRect( FaceType, Level, pLockedRect, pRect, Flags));
	return (m_D3Dtex->LockRect( FaceType, Level, pLockedRect, pRect, Flags));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DCubeTexture9::UnlockRect( D3DCUBEMAP_FACES FaceType, UINT Level)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->UnlockRect( FaceType, Level));
	return (m_D3Dtex->UnlockRect( FaceType, Level));

  HRESULT ret;
  if (CrossRef_D3Dtex!=NULL) ret = (CrossRef_D3Dtex->m_D3Dtex->UnlockRect( FaceType, Level));
  ret = (m_D3Dtex->UnlockRect( FaceType, Level));

  DWORD64 crc64 = CRC64;
  DWORD64 crc32 = CRC32;
  ComputetHash(crc32>0);
  if (crc64!=CRC64)
  {
    // device can be uMod_IDirect3DDevice9 object or a uMod_IDirect3DDevice9Ex object !
    void *cpy;
    long ret = m_D3Ddev->QueryInterface( IID_IDirect3DTexture9, &cpy);
    if (ret == 0x01000000L)
      ((uMod_IDirect3DDevice9*) m_D3Ddev)->GetuMod_Client()->CRCHasChanged(this);
    else
      ((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->GetuMod_Client()->CRCHasChanged(this);
  }

  return ret;
}


int uMod_IDirect3DCubeTexture9::ComputetHash( bool compute_crc)
{
  if (FAKE) return (RETURN_BAD_ARGUMENT);
  IDirect3DCubeTexture9 *pTexture = m_D3Dtex;
  if (CrossRef_D3Dtex!=NULL) pTexture = CrossRef_D3Dtex->m_D3Dtex;


  IDirect3DSurface9 *surface = NULL;
  if (compute_crc) InitCRC32(CRC32);
  InitCRC64(CRC64);

  for (int site = 0; site<6; site++)
  {
    if (pTexture->GetCubeMapSurface( (D3DCUBEMAP_FACES) site, 0, &surface)!=D3D_OK)
    {
      Message("uMod_IDirect3DCubeTexture9::GetHash() Failed: GetSurfaceLevel\n");
      return (RETURN_LockRect_FAILED);
    }
    ((uMod_IDirect3DDevice9*)(m_D3Ddev))->ComputetHash( CRC64, CRC32, surface, compute_crc);

    surface->Release();
  }
  return (RETURN_OK);
}

/*
int uMod_IDirect3DCubeTexture9::ComputetHash( bool compute_crc)
{
  if (FAKE) return (RETURN_BAD_ARGUMENT);
  IDirect3DCubeTexture9 *pTexture = m_D3Dtex;
  if (CrossRef_D3Dtex!=NULL) pTexture = CrossRef_D3Dtex->m_D3Dtex;

  IDirect3DSurface9 *pResolvedSurface = NULL;
  D3DLOCKED_RECT d3dlr;
  D3DSURFACE_DESC desc;

  if (pTexture->GetLevelDesc(0, &desc)!=D3D_OK) //get the format and the size of the texture
  {
    Message("uMod_IDirect3DCubeTexture9::GetHash() Failed: GetLevelDesc \n");
    return (RETURN_GetLevelDesc_FAILED);
  }
  int bits_per_pixel = GetBitsFromFormat( desc.Format);
  unsigned int size;
  unsigned int h_max = desc.Height;
  if (desc.Format == D3DFMT_DXT1) // 8 bytes per block
  {
    h_max /= 4; // divided by block size
    size = desc.Width*2; // desc.Width/4 * 8
  }
  else if ( desc.Format==D3DFMT_DXT2 || desc.Format==D3DFMT_DXT3 || desc.Format==D3DFMT_DXT4 || desc.Format==D3DFMT_DXT5 ) // 16 bytes per block
  {
    h_max /= 4; // divided by block size
    size = desc.Width*4; // desc.Width/4 * 16
  }
  else size = (bits_per_pixel * desc.Width)/8;

  Hash = HASH_INIT_VALUE;

  Message("uMod_IDirect3DCubeTexture9::GetHash() (%d %d) %d\n", desc.Width, desc.Height, desc.Format);

  for (int site = 0; site<6; site++)
  {
    if (pTexture->LockRect( ( _D3DCUBEMAP_FACES) site, 0, &d3dlr, NULL, D3DLOCK_READONLY)!=D3D_OK)
    {
      Message("uMod_IDirect3DCubeTexture9::GetHash() Failed: LockRect 1\n");
      if (pTexture->GetCubeMapSurface( ( _D3DCUBEMAP_FACES) site, 0, &pResolvedSurface)!=D3D_OK)
      {
        Message("uMod_IDirect3DCubeTexture9::GetHash() Failed: GetSurfaceLevel\n");
        return (RETURN_LockRect_FAILED);
      }
      if (pResolvedSurface->LockRect( &d3dlr, NULL, D3DLOCK_READONLY)!=D3D_OK)
      {
        pResolvedSurface->Release();
        Message("uMod_IDirect3DCubeTexture9::GetHash() Failed: LockRect 2\n");
        return (RETURN_LockRect_FAILED);
      }
    }

    {
      unsigned char *data = (unsigned char*) d3dlr.pBits;

      for (unsigned int h=0; h<h_max; h++)
      {
        GetHash( data, size, Hash);
        data += d3dlr.Pitch;
      }
    }

    if (compute_crc && site==0)
    {
      int size = (bits_per_pixel * desc.Width*desc.Height)/8;
      CRC = GetCRC32( (unsigned char*) d3dlr.pBits, size); //calculate the crc32 of the texture
    }

    if (pResolvedSurface!=NULL)
    {
      pResolvedSurface->UnlockRect();
      pResolvedSurface->Release();
      pResolvedSurface=NULL;
    }
    else
    {
      pTexture->UnlockRect( ( _D3DCUBEMAP_FACES) site, 0); //unlock the raw data
    }
  }

  Message("uMod_IDirect3DCubeTexture9::GetHash() %#llX %#LX (%d %d) %d\n", Hash, CRC, desc.Width, desc.Height, desc.Format);
  return (RETURN_OK);
}
*/

