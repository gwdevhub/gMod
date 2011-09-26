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




#include "OTM_Main.h"

HRESULT APIENTRY OTM_IDirect3DTexture9::QueryInterface(REFIID riid, void** ppvObj)
{
	return (m_D3Dtex->QueryInterface(riid, ppvObj));
}

ULONG APIENTRY OTM_IDirect3DTexture9::AddRef()
{
  if (FAKE) return (1); //this case should never happen
  if (CrossRef_D3Dtex!=NULL)
  {
    return (CrossRef_D3Dtex->m_D3Dtex->AddRef());
  }
  else return (m_D3Dtex->AddRef());
}

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
    if (CrossRef_D3Dtex!=NULL)
    {
      OTM_IDirect3DTexture9 *fake_texture = CrossRef_D3Dtex;
      count = fake_texture->m_D3Dtex->Release(); //release the original texture
      if (count==0)
      {
        UnswitchTextures(this);
        if (((OTM_IDirect3DDevice9*)m_D3Ddev)->GetSingleTexture()!=fake_texture) fake_texture->Release();
      }
    }
    else
    {
      count = m_D3Dtex->Release();
    }
  }

  if (count==0)
  {
    ((OTM_IDirect3DDevice9*) m_D3Ddev)-> GetOTM_Client()->RemoveTexture(this); //this is also valid for textures, which are released before we have add them
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

HRESULT APIENTRY OTM_IDirect3DTexture9::SetPrivateData(REFGUID refguid,CONST void* pData,DWORD SizeOfData,DWORD Flags)
{
	return (m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags));
}

HRESULT APIENTRY OTM_IDirect3DTexture9::GetPrivateData(REFGUID refguid,void* pData,DWORD* pSizeOfData)
{
	return (m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData));
}

HRESULT APIENTRY OTM_IDirect3DTexture9::FreePrivateData(REFGUID refguid)
{
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

HRESULT APIENTRY OTM_IDirect3DTexture9::GetLevelDesc(UINT Level,D3DSURFACE_DESC *pDesc)
{
	return (m_D3Dtex->GetLevelDesc(Level, pDesc));
}

HRESULT APIENTRY OTM_IDirect3DTexture9::GetSurfaceLevel(UINT Level,IDirect3DSurface9** ppSurfaceLevel)
{
	return (m_D3Dtex->GetSurfaceLevel(Level, ppSurfaceLevel));
}

HRESULT APIENTRY OTM_IDirect3DTexture9::LockRect(UINT Level,D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect,DWORD Flags)
{
	return (m_D3Dtex->LockRect(Level, pLockedRect, pRect, Flags));
}

HRESULT APIENTRY OTM_IDirect3DTexture9::UnlockRect(UINT Level)
{
	return (m_D3Dtex->UnlockRect(Level));
}

HRESULT APIENTRY OTM_IDirect3DTexture9::AddDirtyRect(CONST RECT* pDirtyRect)
{
	return (m_D3Dtex->AddDirtyRect(pDirtyRect));
}

/*
//////////////////////////////
// Useful Functions
//////////////////////////////
DWORD QuickChecksum(DWORD *pData, int size)
{
	if(!pData) { return (0x0); }

	DWORD sum;
	DWORD tmp;
	sum = *pData;

	for(int i = 1; i < (size/4); i++)
	{
		tmp = pData[i];
		tmp = (DWORD)(sum >> 29) + tmp;
		tmp = (DWORD)(sum >> 17) + tmp;
		sum = (DWORD)(sum << 3)  ^ tmp;
	}

	return sum;
}
*/
