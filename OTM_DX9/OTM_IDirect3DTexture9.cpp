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
along with OpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * some function (e.g. AddReff()) are presumed to work on the texture object which belong to them
 * if this texture was switched, we must redirect this calls to the CrossRef_D3Dtex texture object
 */


#include "OTM_Main.h"

HRESULT APIENTRY OTM_IDirect3DTexture9::QueryInterface(REFIID riid, void** ppvObj)
{
	return (m_D3Dtex->QueryInterface(riid, ppvObj));
}

//this function yields for the non switched texture object
ULONG APIENTRY OTM_IDirect3DTexture9::AddRef()
{
  if (FAKE) return (1); //bug, this case should never happen
  if (CrossRef_D3Dtex!=NULL)
  {
    return (CrossRef_D3Dtex->m_D3Dtex->AddRef());
  }
  else return (m_D3Dtex->AddRef());
}

//this function yields for the non switched texture object
ULONG APIENTRY OTM_IDirect3DTexture9::Release()
{
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
      OTM_IDirect3DTexture9 *fake_texture = CrossRef_D3Dtex;
      count = fake_texture->m_D3Dtex->Release(); //release the original texture
      if (count==0) //if texture is released we switch the textures back
      {
        UnswitchTextures(this);
        if (((OTM_IDirect3DDevice9*)m_D3Ddev)->GetSingleTexture()!=fake_texture) fake_texture->Release(); // we release the fake texture
      }
    }
    else
    {
      count = m_D3Dtex->Release();
    }
  }

  if (count==0) //if this texture is released, we clean up
  {
    // remove this texture from the texture client
    ((OTM_IDirect3DDevice9*) m_D3Ddev)->GetOTM_Client()->RemoveTexture(this); //this is also valid for textures, which are released before we have add them

    // if this texture is the LastCreatedTexture, the next time LastCreatedTexture would be added,
    // the hash of a non existing texture would be calculated
    if (((OTM_IDirect3DDevice9*)m_D3Ddev)->GetLastCreatedTexture()==this) ((OTM_IDirect3DDevice9*)m_D3Ddev)->SetLastCreatedTexture( NULL);
    delete(this);
  }
	return (count);
}

HRESULT APIENTRY OTM_IDirect3DTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{
	*ppDevice = m_D3Ddev;
	return D3D_OK;
}

//this function yields for the non switched texture object
HRESULT APIENTRY OTM_IDirect3DTexture9::SetPrivateData(REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags));
	return (m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags));
}

//this function yields for the non switched texture object
HRESULT APIENTRY OTM_IDirect3DTexture9::GetPrivateData(REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData));
	return (m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData));
}

//this function yields for the non switched texture object
HRESULT APIENTRY OTM_IDirect3DTexture9::FreePrivateData(REFGUID refguid)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->FreePrivateData(refguid));
	return (m_D3Dtex->FreePrivateData(refguid));
}

DWORD APIENTRY OTM_IDirect3DTexture9::SetPriority(DWORD PriorityNew)
{
	return (m_D3Dtex->SetPriority(PriorityNew));
}

DWORD APIENTRY OTM_IDirect3DTexture9::GetPriority()
{
	return (m_D3Dtex->GetPriority());
}

void APIENTRY OTM_IDirect3DTexture9::PreLoad()
{
	m_D3Dtex->PreLoad();
}

D3DRESOURCETYPE APIENTRY OTM_IDirect3DTexture9::GetType()
{
	return (m_D3Dtex->GetType());
}

DWORD APIENTRY OTM_IDirect3DTexture9::SetLOD(DWORD LODNew)
{
	return (m_D3Dtex->SetLOD(LODNew));
}

DWORD APIENTRY OTM_IDirect3DTexture9::GetLOD()
{
	return (m_D3Dtex->GetLOD());
}

DWORD APIENTRY OTM_IDirect3DTexture9::GetLevelCount()
{
	return (m_D3Dtex->GetLevelCount());
}

HRESULT APIENTRY OTM_IDirect3DTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
	return (m_D3Dtex->SetAutoGenFilterType(FilterType));
}

D3DTEXTUREFILTERTYPE APIENTRY OTM_IDirect3DTexture9::GetAutoGenFilterType()
{
	return (m_D3Dtex->GetAutoGenFilterType());
}

void APIENTRY OTM_IDirect3DTexture9::GenerateMipSubLevels()
{
	m_D3Dtex->GenerateMipSubLevels();
}

//this function yields for the non switched texture object
HRESULT APIENTRY OTM_IDirect3DTexture9::GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->GetLevelDesc(Level, pDesc));
	return (m_D3Dtex->GetLevelDesc(Level, pDesc));
}

//this function yields for the non switched texture object
HRESULT APIENTRY OTM_IDirect3DTexture9::GetSurfaceLevel(UINT Level,IDirect3DSurface9** ppSurfaceLevel)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->GetSurfaceLevel(Level, ppSurfaceLevel));
	return (m_D3Dtex->GetSurfaceLevel(Level, ppSurfaceLevel));
}

//this function yields for the non switched texture object
HRESULT APIENTRY OTM_IDirect3DTexture9::LockRect(UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->LockRect(Level, pLockedRect, pRect, Flags));
	return (m_D3Dtex->LockRect(Level, pLockedRect, pRect, Flags));
}

//this function yields for the non switched texture object
HRESULT APIENTRY OTM_IDirect3DTexture9::UnlockRect(UINT Level)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->UnlockRect(Level));
	return (m_D3Dtex->UnlockRect(Level));
}

//this function yields for the non switched texture object
HRESULT APIENTRY OTM_IDirect3DTexture9::AddDirtyRect(CONST RECT* pDirtyRect)
{
  if (CrossRef_D3Dtex!=NULL) return (CrossRef_D3Dtex->m_D3Dtex->AddDirtyRect(pDirtyRect));
	return (m_D3Dtex->AddDirtyRect(pDirtyRect));
}
