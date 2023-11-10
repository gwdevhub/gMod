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


#include "uMod_Main.h"

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
ULONG APIENTRY uMod_IDirect3DTexture9::AddRef()
{
  if (FAKE) return (1); //bug, this case should never happen
  if (CrossRef_D3Dtex!=NULL)
  {
    return (CrossRef_D3Dtex->m_D3Dtex->AddRef());
  }
  else return (m_D3Dtex->AddRef());
}

//this function yields for the non switched texture object
ULONG APIENTRY uMod_IDirect3DTexture9::Release()
{
  Message("uMod_IDirect3DTexture9::Release(): %lu\n", this);

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
      uMod_IDirect3DTexture9 *fake_texture = CrossRef_D3Dtex;
      count = fake_texture->m_D3Dtex->Release(); //release the original texture
      if (count==0) //if texture is released we switch the textures back
      {
        UnswitchTextures(this);
        if (ret == 0x01000000L)
        {
          if (((uMod_IDirect3DDevice9*) m_D3Ddev)->GetSingleTexture()!=fake_texture) fake_texture->Release(); // we release the fake texture
        }
        else
        {
          if (((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->GetSingleTexture()!=fake_texture) fake_texture->Release(); // we release the fake texture
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
      if (((uMod_IDirect3DDevice9*) m_D3Ddev)->GetLastCreatedTexture()==this) ((uMod_IDirect3DDevice9*) m_D3Ddev)->SetLastCreatedTexture( NULL);
      else ((uMod_IDirect3DDevice9*) m_D3Ddev)->GetuMod_Client()->RemoveTexture(this); // remove this texture from the texture client
    }
    else
    {
      if (((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->GetLastCreatedTexture()==this) ((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->SetLastCreatedTexture( NULL);
      else ((uMod_IDirect3DDevice9Ex*) m_D3Ddev)->GetuMod_Client()->RemoveTexture(this); // remove this texture from the texture client
    }

    delete(this);
  }

  Message("uMod_IDirect3DTexture9::Release() end: %lu\n", this);
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
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags));
	return (m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::GetPrivateData(REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData));
	return (m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::FreePrivateData(REFGUID refguid)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->FreePrivateData(refguid));
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
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->GetLevelDesc(Level, pDesc));
	return (m_D3Dtex->GetLevelDesc(Level, pDesc));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::GetSurfaceLevel(UINT Level,IDirect3DSurface9** ppSurfaceLevel)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->GetSurfaceLevel(Level, ppSurfaceLevel));
	return (m_D3Dtex->GetSurfaceLevel(Level, ppSurfaceLevel));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::LockRect(UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->LockRect(Level, pLockedRect, pRect, Flags));
	return (m_D3Dtex->LockRect(Level, pLockedRect, pRect, Flags));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::UnlockRect(UINT Level)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->UnlockRect(Level));
	return (m_D3Dtex->UnlockRect(Level));
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::AddDirtyRect(CONST RECT* pDirtyRect)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->AddDirtyRect(pDirtyRect));
	return (m_D3Dtex->AddDirtyRect(pDirtyRect));
}


int uMod_IDirect3DTexture9::GetHash(MyTypeHash &hash)
{
  hash=0u;
  if (FAKE) return (RETURN_BAD_ARGUMENT);
  IDirect3DTexture9 *pTexture = m_D3Dtex;
  if (CrossRef_D3Dtex!=NULL) pTexture = CrossRef_D3Dtex->m_D3Dtex;

  IDirect3DSurface9 *pOffscreenSurface = NULL;
  //IDirect3DTexture9 *pOffscreenTexture = NULL;
  IDirect3DSurface9 *pResolvedSurface = NULL;
  D3DLOCKED_RECT d3dlr;
  D3DSURFACE_DESC desc;

  if (pTexture->GetLevelDesc(0, &desc)!=D3D_OK) //get the format and the size of the texture
  {
    Message("uMod_IDirect3DTexture9::GetHash() Failed: GetLevelDesc \n");
    return (RETURN_GetLevelDesc_FAILED);
  }

  Message("uMod_IDirect3DTexture9::GetHash() (%d %d) %d\n", desc.Width, desc.Height, desc.Format);


  if (desc.Pool==D3DPOOL_DEFAULT) //get the raw data of the texture
  {
    //Message("uMod_IDirect3DTexture9::GetHash() (D3DPOOL_DEFAULT)\n");

    IDirect3DSurface9 *pSurfaceLevel_orig = NULL;
    if (pTexture->GetSurfaceLevel( 0, &pSurfaceLevel_orig)!=D3D_OK)
    {
      Message("uMod_IDirect3DTexture9::GetHash() Failed: GetSurfaceLevel 1  (D3DPOOL_DEFAULT)\n");
      return (RETURN_LockRect_FAILED);
    }

    if (desc.MultiSampleType != D3DMULTISAMPLE_NONE)
    {
      //Message("uMod_IDirect3DTexture9::GetHash() MultiSampleType\n");
      if (D3D_OK!=m_D3Ddev->CreateRenderTarget( desc.Width, desc.Height, desc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &pResolvedSurface, NULL ))
      {
        pSurfaceLevel_orig->Release();
        Message("uMod_IDirect3DTexture9::GetHash() Failed: CreateRenderTarget  (D3DPOOL_DEFAULT)\n");
        return (RETURN_LockRect_FAILED);
      }
      if (D3D_OK!=m_D3Ddev->StretchRect( pSurfaceLevel_orig, NULL, pResolvedSurface, NULL, D3DTEXF_NONE ))
      {
        pSurfaceLevel_orig->Release();
        Message("uMod_IDirect3DTexture9::GetHash() Failed: StretchRect  (D3DPOOL_DEFAULT)\n");
        return (RETURN_LockRect_FAILED);
      }

      pSurfaceLevel_orig = pResolvedSurface;
    }

    if (D3D_OK!=m_D3Ddev->CreateOffscreenPlainSurface( desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &pOffscreenSurface, NULL))
    {
      pSurfaceLevel_orig->Release();
      if (pResolvedSurface!=NULL) pResolvedSurface->Release();
      Message("uMod_IDirect3DTexture9::GetHash() Failed: CreateOffscreenPlainSurface (D3DPOOL_DEFAULT)\n");
      return (RETURN_TEXTURE_NOT_LOADED);
    }

    if (D3D_OK!=m_D3Ddev->GetRenderTargetData( pSurfaceLevel_orig, pOffscreenSurface))
    {
      pSurfaceLevel_orig->Release();
      if (pResolvedSurface!=NULL) pResolvedSurface->Release();
      pOffscreenSurface->Release();
      Message("uMod_IDirect3DTexture9::GetHash() Failed: GetRenderTargetData (D3DPOOL_DEFAULT)\n");
      return (RETURN_LockRect_FAILED);
    }
    pSurfaceLevel_orig->Release();

    if (pOffscreenSurface->LockRect( &d3dlr, NULL, D3DLOCK_READONLY)!=D3D_OK)
    {
      if (pResolvedSurface!=NULL) pResolvedSurface->Release();
      pOffscreenSurface->Release();
      Message("uMod_IDirect3DTexture9::GetHash() Failed: LockRect (D3DPOOL_DEFAULT)\n");
      return (RETURN_LockRect_FAILED);
    }
  }
  else if (pTexture->LockRect( 0, &d3dlr, NULL, D3DLOCK_READONLY)!=D3D_OK)
  {
    Message("uMod_IDirect3DTexture9::GetHash() Failed: LockRect 1\n");
    if (pTexture->GetSurfaceLevel(0, &pResolvedSurface)!=D3D_OK)
    {
      Message("uMod_IDirect3DTexture9::GetHash() Failed: GetSurfaceLevel\n");
      return (RETURN_LockRect_FAILED);
    }
    if (pResolvedSurface->LockRect( &d3dlr, NULL, D3DLOCK_READONLY)!=D3D_OK)
    {
      pResolvedSurface->Release();
      Message("uMod_IDirect3DTexture9::GetHash() Failed: LockRect 2\n");
      return (RETURN_LockRect_FAILED);
    }
  }

  int size = (GetBitsFromFormat( desc.Format) * desc.Width*desc.Height)/8;

  hash = GetCRC32( (char*) d3dlr.pBits, size); //calculate the crc32 of the texture

  if (pOffscreenSurface!=NULL)
  {
    pOffscreenSurface->UnlockRect();
    pOffscreenSurface->Release();
    if (pResolvedSurface!=NULL) pResolvedSurface->Release();
  }
  else if (pResolvedSurface!=NULL)
  {
    pResolvedSurface->UnlockRect();
    pResolvedSurface->Release();
  }
  else pTexture->UnlockRect(0);

  Message("uMod_IDirect3DTexture9::GetHash() %#lX (%d %d) %d = %d\n", hash, desc.Width, desc.Height, desc.Format, size);
  return (RETURN_OK);
}
