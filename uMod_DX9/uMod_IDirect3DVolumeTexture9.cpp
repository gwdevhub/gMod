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
#include "uMod_IDirect3DVolumeTexture9.h"

HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::QueryInterface(REFIID riid, void** ppvObj)
{
  if (riid==IID_IDirect3D9)
  {
    // This function should never be called with IID_IDirect3D9 by the game
    // thus this call comes from our own dll to ask for the texture type
    // 0x01000000L == uMod_IDirect3DTexture9
    // 0x01000001L == uMod_IDirect3DVolumeTexture9
    // 0x01000002L == uMod_IDirect3DCubeTexture9

    *ppvObj = this;
    return (0x01000001L);
  }
  HRESULT hRes = m_D3Dtex->QueryInterface(riid, ppvObj);
  if (*ppvObj==m_D3Dtex) *ppvObj=this;

  return (hRes);
}

//this function yields for the non switched texture object
ULONG APIENTRY uMod_IDirect3DVolumeTexture9::AddRef()
{
  if (FAKE) return (1); //bug, this case should never happen
  return (m_D3Dtex->AddRef());
}

//this function yields for the non switched texture object
ULONG APIENTRY uMod_IDirect3DVolumeTexture9::Release()
{
  Message("uMod_IDirect3DVolumeTexture9::Release(): %p\n", this);

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
	return (count);
}

HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{
	*ppDevice = m_D3Ddev;
	return D3D_OK;
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::SetPrivateData(REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
	return (m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::GetPrivateData(REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
	return (m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::FreePrivateData(REFGUID refguid)
{
	return (m_D3Dtex->FreePrivateData(refguid));
}

DWORD APIENTRY uMod_IDirect3DVolumeTexture9::SetPriority(DWORD PriorityNew)
{
	return (m_D3Dtex->SetPriority(PriorityNew));
}

DWORD APIENTRY uMod_IDirect3DVolumeTexture9::GetPriority()
{
	return (m_D3Dtex->GetPriority());
}

void APIENTRY uMod_IDirect3DVolumeTexture9::PreLoad()
{
	m_D3Dtex->PreLoad();
}

D3DRESOURCETYPE APIENTRY uMod_IDirect3DVolumeTexture9::GetType()
{
	return (m_D3Dtex->GetType());
}

DWORD APIENTRY uMod_IDirect3DVolumeTexture9::SetLOD(DWORD LODNew)
{
	return (m_D3Dtex->SetLOD(LODNew));
}

DWORD APIENTRY uMod_IDirect3DVolumeTexture9::GetLOD()
{
	return (m_D3Dtex->GetLOD());
}

DWORD APIENTRY uMod_IDirect3DVolumeTexture9::GetLevelCount()
{
	return (m_D3Dtex->GetLevelCount());
}

HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	return (m_D3Dtex->SetAutoGenFilterType(FilterType));
}

D3DTEXTUREFILTERTYPE APIENTRY uMod_IDirect3DVolumeTexture9::GetAutoGenFilterType()
{
	return (m_D3Dtex->GetAutoGenFilterType());
}

void APIENTRY uMod_IDirect3DVolumeTexture9::GenerateMipSubLevels()
{
	m_D3Dtex->GenerateMipSubLevels();
}



//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::AddDirtyBox(CONST D3DBOX *pDirtyBox)
{
  return (m_D3Dtex->AddDirtyBox(pDirtyBox));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::GetLevelDesc(UINT Level, D3DVOLUME_DESC *pDesc)
{
	return (m_D3Dtex->GetLevelDesc(Level, pDesc));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::GetVolumeLevel(UINT Level, IDirect3DVolume9 **ppVolumeLevel)
{
  Dirty=1;
	return (m_D3Dtex->GetVolumeLevel(Level, ppVolumeLevel));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::LockBox(UINT Level, D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox ,DWORD Flags)
{
	return (m_D3Dtex->LockBox(Level, pLockedVolume, pBox, Flags));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::UnlockBox(UINT Level)
{
  Dirty=1;
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->UnlockBox(Level));
	return (m_D3Dtex->UnlockBox(Level));
}


int uMod_IDirect3DVolumeTexture9::ComputetHash( bool compute_crc)
{
  if (FAKE) return (RETURN_BAD_ARGUMENT);
  IDirect3DVolumeTexture9 *pTexture = m_D3Dtex;

  //IDirect3DVolume9  *pOffscreenSurface = NULL;
  //IDirect3DVolumeTexture9 *pOffscreenTexture = NULL;
  IDirect3DVolume9  *pResolvedSurface = NULL;
  D3DLOCKED_BOX d3dlr;
  D3DVOLUME_DESC desc;

  if (pTexture->GetLevelDesc(0, &desc)!=D3D_OK) //get the format and the size of the texture
  {
    Message("uMod_IDirect3DVolumeTexture9::GetHash() Failed: GetLevelDesc \n");
    return (RETURN_GetLevelDesc_FAILED);
  }

  Message("uMod_IDirect3DVolumeTexture9::GetHash() (%d %d %d) %d\n", desc.Width, desc.Height, desc.Depth, desc.Format);


  if (pTexture->LockBox( 0, &d3dlr, NULL, D3DLOCK_READONLY)!=D3D_OK)
  {
    Message("uMod_IDirect3DVolumeTexture9::GetHash() Failed: LockRect 1\n");
    if (pTexture->GetVolumeLevel(0, &pResolvedSurface)!=D3D_OK)
    {
      Message("uMod_IDirect3DVolumeTexture9::GetHash() Failed: GetSurfaceLevel\n");
      return (RETURN_LockRect_FAILED);
    }
    if (pResolvedSurface->LockBox( &d3dlr, NULL, D3DLOCK_READONLY)!=D3D_OK)
    {
      pResolvedSurface->Release();
      Message("uMod_IDirect3DVolumeTexture9::GetHash() Failed: LockRect 2\n");
      return (RETURN_LockRect_FAILED);
    }
  }


  int bits_per_pixel = GetBitsFromFormat( desc.Format);

  {
    InitCRC64(CRC64);
    unsigned char *data = (unsigned char*) d3dlr.pBits;

    unsigned int size;
    unsigned int h_max = desc.Height;
    if (desc.Format == D3DFMT_DXT1) // 8 bytes per block
    {
      h_max /= 4; // divide by block size
      size = desc.Width*2; // desc.Width/4 * 8
    }
    else if ( desc.Format==D3DFMT_DXT2 || desc.Format==D3DFMT_DXT3 || desc.Format==D3DFMT_DXT4 || desc.Format==D3DFMT_DXT5 ) // 16 bytes per block
    {
      h_max /= 4; // divide by block size
      size = desc.Width*4; // desc.Width/4 * 16
    }
    else size = (bits_per_pixel * desc.Width)/8;

    //int size = (bits_per_pixel * desc.Width)/8;
    for (unsigned int d=0; d<desc.Depth; d++)
    {
      unsigned char* data_inner_loop = data;
      for (unsigned int h=0; h<h_max; h++)
      {
        GetCRC64( CRC64, data_inner_loop, size);
        data_inner_loop += d3dlr.RowPitch;
      }
      data += d3dlr.SlicePitch;
    }
  }

  if (compute_crc)
  {
    InitCRC32(CRC32);
    int size = (bits_per_pixel * desc.Width*desc.Height*desc.Depth)/8;
    GetCRC32( CRC32, (unsigned char*) d3dlr.pBits, size); //calculate the crc32 of the texture
  }


  if (pResolvedSurface!=NULL)
  {
    pResolvedSurface->UnlockBox();
    pResolvedSurface->Release();
  }
  else pTexture->UnlockBox(0);

  Message("uMod_IDirect3DVolumeTexture9::GetHash() %#llX %#LX (%d %d %d) %d\n", CRC64, CRC32, desc.Width, desc.Height, desc.Depth, desc.Format);
  return (RETURN_OK);
}
