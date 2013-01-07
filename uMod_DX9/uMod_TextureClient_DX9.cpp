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



#include "..\uMod_DXMain\uMod_Main.h"
#include "uMod_TextureClient_DX9.h"


uMod_TextureClient_DX9::uMod_TextureClient_DX9( IDirect3DDevice9* device, const int version)
  : uMod_TextureClient( version)
{
  Message("uMod_TextureClient_DX9::uMod_TextureClient_DX9(void): %p\n", this);
  D3D9Device = device;
  BoolComputeCRC32 = false;
  //Bool_CheckAgainNonAdded = false;
}

uMod_TextureClient_DX9::~uMod_TextureClient_DX9(void)
{
  Message("uMod_TextureClient_DX9::~uMod_TextureClient(void): %p\n", this);
}


int uMod_TextureClient_DX9::AddTexture( uMod_IDirect3DTexture9* pTexture)
{
  if (pTexture->FAKE) return (RETURN_OK); // this is a fake texture

  Message("uMod_TextureClient_DX9::AddTexture( %p): %p (thread: %lu)\n", pTexture, this, GetCurrentThreadId());
  OriginalTextures.Add( pTexture); // add the texture to the list of original texture

  return (RETURN_OK);
}

int uMod_TextureClient_DX9::AddTexture( uMod_IDirect3DVolumeTexture9* pTexture)
{
  if (pTexture->FAKE) return (RETURN_OK); // this is a fake texture

  Message("uMod_TextureClient_DX9::AddTexture( Volume: %p): %p (thread: %lu)\n", pTexture, this, GetCurrentThreadId());
  OriginalVolumeTextures.Add( pTexture); // add the texture to the list of original texture

  return (RETURN_OK);
}

int uMod_TextureClient_DX9::AddTexture( uMod_IDirect3DCubeTexture9* pTexture)
{
  if (pTexture->FAKE) return (RETURN_OK); // this is a fake texture

  Message("uMod_TextureClient_DX9::AddTexture( Cube: %p): %p (thread: %lu)\n", pTexture, this, GetCurrentThreadId());
  OriginalCubeTextures.Add( pTexture); // add the texture to the list of original texture

  return (RETURN_OK);
}

int uMod_TextureClient_DX9::RemoveTexture( uMod_IDirect3DTexture9* pTexture) // is called from a texture, if it is finally released
{
  Message("uMod_TextureClient_DX9::RemoveTexture( %p, %#llX): %p\n", pTexture, pTexture->CRC64, this);

  if (gl_ErrorState & uMod_ERROR_FATAL) return (RETURN_FATAL_ERROR);
  if (pTexture->FAKE)
  {
    // we need to set the corresponding FileToMod[X].pTexture to NULL, to avoid a link to a non existing texture object
    int ref = pTexture->Reference;
    if (ref>=0 && ref<NumberToMod)
    {
      FileToMod[ref].RemoveTexture(pTexture);
      /*
      for (int i=0; i<FileToMod[ref].NumberOfTextures; i++) if (FileToMod[ref].Textures[i] == pTexture)
      {
        FileToMod[ref].NumberOfTextures--;
        for (int j=i; j<FileToMod[ref].NumberOfTextures; j++) FileToMod[ref].Textures[j] = FileToMod[ref].Textures[j+1];
        FileToMod[ref].Textures[FileToMod[ref].NumberOfTextures] = NULL;
        break;
      }
      */
    }
  }
  else
  {
    if (int ret = OriginalTextures.Remove( pTexture)) return (ret); // remove this texture form the original list
    //return (NonAdded_OriginalTextures.Remove( pTexture)); // also try to remove this texture form the non_added list
  }
  return (RETURN_OK);
}

int uMod_TextureClient_DX9::RemoveTexture( uMod_IDirect3DVolumeTexture9* pTexture) // is called from a texture, if it is finally released
{
  Message("uMod_TextureClient_DX9::RemoveTexture( Volume %p, %#llX): %p\n", pTexture, pTexture->CRC64, this);

  if (gl_ErrorState & uMod_ERROR_FATAL) return (RETURN_FATAL_ERROR);
  if (pTexture->FAKE)
  {
    // we need to set the corresponding FileToMod[X].pTexture to NULL, to avoid a link to a non existing texture object
    int ref = pTexture->Reference;
    if (ref>=0 && ref<NumberToMod)
    {
      FileToMod[ref].RemoveTexture(pTexture);
      /*
      for (int i=0; i<FileToMod[ref].NumberOfTextures; i++) if (FileToMod[ref].Textures[i] == pTexture)
      {
        FileToMod[ref].NumberOfTextures--;
        for (int j=i; j<FileToMod[ref].NumberOfTextures; j++) FileToMod[ref].Textures[j] = FileToMod[ref].Textures[j+1];
        FileToMod[ref].Textures[FileToMod[ref].NumberOfTextures] = NULL;
        break;
      }
      */
    }
  }
  else
  {
    if (int ret = OriginalVolumeTextures.Remove( pTexture)) return (ret); // remove this texture form the original list
    //return (NonAdded_OriginalVolumeTextures.Remove( pTexture)); // also try to remove this texture form the non_added list
  }
  return (RETURN_OK);
}

int uMod_TextureClient_DX9::RemoveTexture( uMod_IDirect3DCubeTexture9* pTexture) // is called from a texture, if it is finally released
{
  Message("uMod_TextureClient_DX9::RemoveTexture( Cube %p, %#llX): %p\n", pTexture, pTexture->CRC64, this);

  if (gl_ErrorState & uMod_ERROR_FATAL) return (RETURN_FATAL_ERROR);
  if (pTexture->FAKE)
  {
    // we need to set the corresponding FileToMod[X].pTexture to NULL, to avoid a link to a non existing texture object
    int ref = pTexture->Reference;
    if (ref>=0 && ref<NumberToMod)
    {
      FileToMod[ref].RemoveTexture(pTexture);
      /*
      for (int i=0; i<FileToMod[ref].NumberOfTextures; i++) if (FileToMod[ref].Textures[i] == pTexture)
      {
        FileToMod[ref].NumberOfTextures--;
        for (int j=i; j<FileToMod[ref].NumberOfTextures; j++) FileToMod[ref].Textures[j] = FileToMod[ref].Textures[j+1];
        FileToMod[ref].Textures[FileToMod[ref].NumberOfTextures] = NULL;
        break;
      }
      */
    }
  }
  else
  {
    if (int ret = OriginalCubeTextures.Remove( pTexture)) return (ret); // remove this texture form the original list
    //return (NonAdded_OriginalCubeTextures.Remove( pTexture)); // also try to remove this texture form the non_added list
  }
  return (RETURN_OK);
}

int uMod_TextureClient_DX9::ComputeCRC( uMod_IDirect3DTexture9* tex)
{
  if (tex->FAKE) return (RETURN_OK);
  if (tex->Dirty>10) return (RETURN_OK);

  DWORD64 crc64 = tex->CRC64;
  DWORD64 crc32 = tex->CRC32;
  if (tex->ComputetHash(BoolComputeCRC32)!=(RETURN_OK))
  {
    tex->Dirty++;
    return (RETURN_OK);
  }
  tex->Dirty = 0;

  if (crc64!=tex->CRC64 || crc32!=tex->CRC32)
  {
    if (tex->CrossRef_D3Dtex!=NULL)
    {
      if (tex->CrossRef_D3Dtex->Reference<0)
        UnswitchTextures( tex);
      else
        tex->CrossRef_D3Dtex->Release();
    }
    if (BoolSaveAllTextures) SaveTexture( tex, true);
    LookUpToMod( tex);
  }

  return (RETURN_OK);
}

int uMod_TextureClient_DX9::ComputeCRC( uMod_IDirect3DVolumeTexture9* tex)
{
  if (tex->FAKE) return (RETURN_OK);
  if (tex->Dirty>10) return (RETURN_OK);

  DWORD64 crc64 = tex->CRC64;
  DWORD64 crc32 = tex->CRC32;
  if (tex->ComputetHash(BoolComputeCRC32)!=(RETURN_OK))
  {
    tex->Dirty++;
    return (RETURN_OK);
  }
  tex->Dirty = 0;

  if (crc64!=tex->CRC64)
  {
    if (tex->CrossRef_D3Dtex!=NULL)
    {
      if (tex->CrossRef_D3Dtex->Reference<0)
        UnswitchTextures( tex);
      else
        tex->CrossRef_D3Dtex->Release();
    }
    if (BoolSaveAllTextures) SaveTexture( tex, true);
    LookUpToMod( tex);
  }

  return (RETURN_OK);
}

int uMod_TextureClient_DX9::ComputeCRC( uMod_IDirect3DCubeTexture9* tex)
{
  if (tex->FAKE) return (RETURN_OK);
  if (tex->Dirty>10) return (RETURN_OK);

  DWORD64 crc64 = tex->CRC64;
  DWORD64 crc32 = tex->CRC32;
  if (tex->ComputetHash(BoolComputeCRC32)!=(RETURN_OK))
  {
    tex->Dirty++;
    return (RETURN_OK);
  }
  tex->Dirty = 0;

  if (crc64!=tex->CRC64)
  {
    if (tex->CrossRef_D3Dtex!=NULL)
    {
      if (tex->CrossRef_D3Dtex->Reference<0)
        UnswitchTextures( tex);
      else
        tex->CrossRef_D3Dtex->Release();
    }
    if (BoolSaveAllTextures) SaveTexture( tex, true);
    LookUpToMod( tex);
  }

  return (RETURN_OK);
}

int uMod_TextureClient_DX9::SaveAllTextures(bool val)
{
  Message("uMod_TextureClient_DX9::SaveAllTextures( %d): %p\n", val, this);
  BoolSaveAllTextures=val;
  return (RETURN_OK);
}

int uMod_TextureClient_DX9::SaveSingleTexture(bool val)
{
  Message("uMod_TextureClient_DX9::SaveSingleTexture( %d): %p\n", val, this);
  if (BoolSaveSingleTexture && !val) //if BoolSaveSingleTexture is set to false and was previously true we switch the SingleTexture back
  {
    uMod_IDirect3DTexture9* pTexture;
    void *cpy;
    long ret = D3D9Device->QueryInterface( IID_IDirect3DTexture9, &cpy);
    if (ret == 0x01000000L) pTexture = ((uMod_IDirect3DDevice9*)D3D9Device)->GetSingleTexture(); //this texture must no be added twice
    else pTexture = ((uMod_IDirect3DDevice9Ex*) D3D9Device)->GetSingleTexture(); //this texture must no be added twice

    if (pTexture!=NULL) UnswitchTextures(pTexture);
  }
  BoolSaveSingleTexture = val;
  return (RETURN_OK);
}

int uMod_TextureClient_DX9::SupportTPF(bool val)
{
  BoolComputeCRC32 = val;
  int num = OriginalTextures.GetNumber();
  for (int i=0; i<num; i++) OriginalTextures[i]->Dirty = true;

  num = OriginalVolumeTextures.GetNumber();
  for (int i=0; i<num; i++) OriginalVolumeTextures[i]->Dirty = true;

  num = OriginalCubeTextures.GetNumber();
  for (int i=0; i<num; i++) OriginalCubeTextures[i]->Dirty = true;

  return (RETURN_OK);
}

int uMod_TextureClient_DX9::SaveTexture(uMod_IDirect3DTexture9* pTexture, bool save_all)
{
  if (pTexture==NULL) return (RETURN_BAD_ARGUMENT);
  if (SavePath[0]==0)
  {
    Message("uMod_TextureClient_DX9::SaveTexture( %#llX, %p): %p,   SavePath not set\n", pTexture->CRC64, pTexture->m_D3Dtex, this);
    return (RETURN_TEXTURE_NOT_SAVED);
  }
  D3DSURFACE_DESC desc;
  if (pTexture->m_D3Dtex->GetLevelDesc(0, &desc)!=D3D_OK) //get the format and the size of the texture
  {
    Message("uMod_TextureClient_DX9::SaveTexture() Failed: GetLevelDesc \n");
    return (RETURN_GetLevelDesc_FAILED);
  }

  if (save_all)
  {
    if (WidthFilter)
    {
      unsigned int min = WidthFilter >> 32;
      unsigned int max = (WidthFilter<<32) >> 32;
      if (desc.Width<min || desc.Width> max) return (RETURN_OK);
    }
    if (HeightFilter)
    {
      unsigned int min = HeightFilter >> 32;
      unsigned int max = (HeightFilter<<32) >> 32;
      if (desc.Height<min || desc.Height> max) return (RETURN_OK);
    }
    if (!SaveTextureFilterFormat(desc.Format))  return (RETURN_OK);
  }


  wchar_t file[MAX_PATH];
  if (GameName[0]) swprintf_s( file, MAX_PATH, L"%ls\\%ls_W%u_H%u_F%u_T_%#llX", SavePath, GameName, desc.Width, desc.Height,desc.Format, pTexture->CRC64);
  else swprintf_s( file, MAX_PATH, L"%ls\\W%u_H%u_F%u_T_%#llX", SavePath, desc.Width, desc.Height, desc.Format, pTexture->CRC64);
  Message("uMod_TextureClient_DX9::SaveTexture( %ls): %p\n", file, this);

  return (SaveTexture( pTexture->m_D3Dtex, file));
}

int uMod_TextureClient_DX9::SaveTexture(uMod_IDirect3DVolumeTexture9* pTexture, bool save_all)
{
  if (pTexture==NULL) return (RETURN_BAD_ARGUMENT);
  if (SavePath[0]==0) {Message("uMod_TextureClient_DX9::SaveTexture( %#llX, %p): %p,   SavePath not set\n", pTexture->CRC64, pTexture->m_D3Dtex, this); return (RETURN_TEXTURE_NOT_SAVED);}

  D3DVOLUME_DESC desc;
  if (pTexture->m_D3Dtex->GetLevelDesc(0, &desc)!=D3D_OK) //get the format and the size of the texture
  {
    Message("uMod_TextureClient_DX9::SaveTexture() Failed: GetLevelDesc \n");
    return (RETURN_GetLevelDesc_FAILED);
  }

  if (save_all)
  {
    if (WidthFilter)
    {
      unsigned int min = WidthFilter >> 32;
      unsigned int max = (WidthFilter<<32) >> 32;
      if (desc.Width<min || desc.Width> max) return (RETURN_OK);
    }
    if (HeightFilter)
    {
      unsigned int min = HeightFilter >> 32;
      unsigned int max = (HeightFilter<<32) >> 32;
      if (desc.Height<min || desc.Height> max) return (RETURN_OK);
    }
    if (DepthFilter)
    {
      unsigned int min = DepthFilter >> 32;
      unsigned int max = (DepthFilter<<32) >> 32;
      if (desc.Depth<min || desc.Depth> max) return (RETURN_OK);
    }
    if (!SaveTextureFilterFormat(desc.Format))  return (RETURN_OK);
  }

  wchar_t file[MAX_PATH];
  if (GameName[0]) swprintf_s( file, MAX_PATH, L"%ls\\%ls_W%u_H%u_D%u_F%u_V_%#llX", SavePath, GameName, desc.Width, desc.Height, desc.Depth, desc.Format, pTexture->CRC64);
  else swprintf_s( file, MAX_PATH, L"%ls\\W%u_H%u_D%u_F%u_V_%#llX", SavePath, desc.Width, desc.Height, desc.Depth, desc.Format, pTexture->CRC64);
  Message("uMod_TextureClient_DX9::SaveTexture( %ls): %p\n", file, this);

  return (SaveTexture( pTexture->m_D3Dtex, file));
}

int uMod_TextureClient_DX9::SaveTexture(uMod_IDirect3DCubeTexture9* pTexture, bool save_all)
{
  if (pTexture==NULL) return (RETURN_BAD_ARGUMENT);
  if (SavePath[0]==0) {Message("uMod_TextureClient_DX9::SaveTexture( %#llX, %p): %p,   SavePath not set\n", pTexture->CRC64, pTexture->m_D3Dtex, this); return (RETURN_TEXTURE_NOT_SAVED);}

  D3DSURFACE_DESC desc;
  if (pTexture->m_D3Dtex->GetLevelDesc(0, &desc)!=D3D_OK) //get the format and the size of the texture
  {
    Message("uMod_TextureClient_DX9::SaveTexture() Failed: GetLevelDesc \n");
    return (RETURN_GetLevelDesc_FAILED);
  }

  if (save_all)
  {
    if (WidthFilter)
    {
      unsigned int min = WidthFilter >> 32;
      unsigned int max = (WidthFilter<<32) >> 32;
      if (desc.Width<min || desc.Width> max) return (RETURN_OK);
    }
    if (!SaveTextureFilterFormat(desc.Format))  return (RETURN_OK);
  }

  wchar_t file[MAX_PATH];
  if (GameName[0]) swprintf_s( file, MAX_PATH, L"%ls\\%ls_W%u_F%u_C_%#llX.dds", SavePath, GameName, desc.Width, desc.Format, pTexture->CRC64);
  else swprintf_s( file, MAX_PATH, L"%ls\\W%u_F%u_C_%#llX.dds", SavePath, desc.Width, desc.Format, pTexture->CRC64);
  Message("uMod_TextureClient_DX9::SaveTexture( %ls): %p\n", file, this);


  return (SaveTexture( pTexture->m_D3Dtex, file));
}


int uMod_TextureClient_DX9::SaveTexture(IDirect3DBaseTexture9* pTexture, wchar_t *file)
{
  unsigned int len=0;
  while (file[len]) len++;
  if (FileFormat & uMod_D3DXIFF_BMP)
  {
    file[len] = '.';file[len+1] = 'b';file[len+2] = 'm';file[len+3] = 'p';file[len+4] = '\0';
    if (D3D_OK!=D3DXSaveTextureToFileW( file, D3DXIFF_BMP, pTexture, NULL)) return (RETURN_TEXTURE_NOT_SAVED);
  }
  if (FileFormat & uMod_D3DXIFF_JPG)
  {
    file[len] = '.';file[len+1] = 'j';file[len+2] = 'p';file[len+3] = 'g';file[len+4] = '\0';
    if (D3D_OK!=D3DXSaveTextureToFileW( file, D3DXIFF_JPG, pTexture, NULL)) return (RETURN_TEXTURE_NOT_SAVED);
  }
  if (FileFormat & uMod_D3DXIFF_TGA)
  {
    file[len] = '.';file[len+1] = 't';file[len+2] = 'g';file[len+3] = 'a';file[len+4] = '\0';
    if (D3D_OK!=D3DXSaveTextureToFileW( file, D3DXIFF_TGA, pTexture, NULL)) return (RETURN_TEXTURE_NOT_SAVED);
  }
  if (FileFormat & uMod_D3DXIFF_PNG)
  {
    file[len] = '.';file[len+1] = 'p';file[len+2] = 'n';file[len+3] = 'g';file[len+4] = '\0';
    if (D3D_OK!=D3DXSaveTextureToFileW( file, D3DXIFF_PNG, pTexture, NULL)) return (RETURN_TEXTURE_NOT_SAVED);
  }
  if (FileFormat & uMod_D3DXIFF_DDS)
  {
    file[len] = '.';file[len+1] = 'd';file[len+2] = 'd';file[len+3] = 's';file[len+4] = '\0';
    if (D3D_OK!=D3DXSaveTextureToFileW( file, D3DXIFF_DDS, pTexture, NULL)) return (RETURN_TEXTURE_NOT_SAVED);
  }
  if (FileFormat & uMod_D3DXIFF_PPM)
  {
    file[len] = '.';file[len+1] = 'p';file[len+2] = 'p';file[len+3] = 'm';file[len+4] = '\0';
    if (D3D_OK!=D3DXSaveTextureToFileW( file, D3DXIFF_PPM, pTexture, NULL)) return (RETURN_TEXTURE_NOT_SAVED);
  }
  return (RETURN_OK);
}

#define DEF_FormatCheck( format ) \
  case format: \
  { \
    if (FormatFilter & uMod_ ## format) return (true); \
    else return (false); \
  }


bool uMod_TextureClient_DX9::SaveTextureFilterFormat(D3DFORMAT format)
{
  if (FormatFilter == 0u) return (true);
  switch(format) //switch trough the formats to calculate the size of the raw data
  {
  DEF_FormatCheck( D3DFMT_A1) // 1-bit monochrome.
  DEF_FormatCheck( D3DFMT_R3G3B2) // 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue.
  DEF_FormatCheck( D3DFMT_A8) // 8-bit alpha only.

  DEF_FormatCheck( D3DFMT_A8P8) // 8-bit color indexed with 8 bits of alpha.
  DEF_FormatCheck( D3DFMT_P8) // 8-bit color indexed.
  DEF_FormatCheck( D3DFMT_L8) // 8-bit luminance only.
  DEF_FormatCheck( D3DFMT_A4L4) // 8-bit using 4 bits each for alpha and luminance.

  //DEF_FormatCheck( D3DFMT_FORCE_DWORD)
  DEF_FormatCheck( D3DFMT_S8_LOCKABLE) // A lockable 8-bit stencil buffer.

  DEF_FormatCheck( D3DFMT_D16_LOCKABLE) //16-bit z-buffer bit depth.
  DEF_FormatCheck( D3DFMT_D15S1) // 16-bit z-buffer bit depth where 15 bits are reserved for the depth channel and 1 bit is reserved for the stencil channel.
  DEF_FormatCheck( D3DFMT_L6V5U5) // 16-bit bump-map format with luminance using 6 bits for luminance, and 5 bits each for v and u.
  DEF_FormatCheck( D3DFMT_V8U8) // 16-bit bump-map format using 8 bits each for u and v data.
  DEF_FormatCheck( D3DFMT_CxV8U8) // 16-bit normal compression format. The texture sampler computes the C channel from) C = sqrt(1 - U2 - V2).
  DEF_FormatCheck( D3DFMT_R5G6B5) // 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue.
  DEF_FormatCheck( D3DFMT_X1R5G5B5) // 16-bit pixel format where 5 bits are reserved for each color.
  DEF_FormatCheck( D3DFMT_A1R5G5B5) // 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha.
  DEF_FormatCheck( D3DFMT_A4R4G4B4) // 16-bit ARGB pixel format with 4 bits for each channel.
  DEF_FormatCheck( D3DFMT_A8R3G3B2) // 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue.
  DEF_FormatCheck( D3DFMT_X4R4G4B4) // 16-bit RGB pixel format using 4 bits for each color.
  DEF_FormatCheck( D3DFMT_L16) // 16-bit luminance only.
  DEF_FormatCheck( D3DFMT_R16F) // 16-bit float format using 16 bits for the red channel.
  DEF_FormatCheck( D3DFMT_A8L8) // 16-bit using 8 bits each for alpha and luminance.
  DEF_FormatCheck( D3DFMT_D16) // 16-bit z-buffer bit depth.
  DEF_FormatCheck( D3DFMT_INDEX16) // 16-bit index buffer bit depth.
  DEF_FormatCheck( D3DFMT_G8R8_G8B8) // ??
  DEF_FormatCheck( D3DFMT_R8G8_B8G8) // ??
  DEF_FormatCheck( D3DFMT_UYVY) // ??
  DEF_FormatCheck( D3DFMT_YUY2) // ??

  DEF_FormatCheck( D3DFMT_R8G8B8) //24-bit RGB pixel format with 8 bits per channel.

  DEF_FormatCheck( D3DFMT_R32F) // 32-bit float format using 32 bits for the red channel.
  DEF_FormatCheck( D3DFMT_X8L8V8U8) // 32-bit bump-map format with luminance using 8 bits for each channel.
  DEF_FormatCheck( D3DFMT_A2W10V10U10) // 32-bit bump-map format using 2 bits for alpha and 10 bits each for w, v, and u.
  DEF_FormatCheck( D3DFMT_Q8W8V8U8) // 32-bit bump-map format using 8 bits for each channel.
  DEF_FormatCheck( D3DFMT_V16U16) // 32-bit bump-map format using 16 bits for each channel.
  DEF_FormatCheck( D3DFMT_A8R8G8B8) // 32-bit ARGB pixel format with alpha, using 8 bits per channel.
  DEF_FormatCheck( D3DFMT_X8R8G8B8) // 32-bit RGB pixel format, where 8 bits are reserved for each color.
  DEF_FormatCheck( D3DFMT_A2B10G10R10) // 32-bit pixel format using 10 bits for each color and 2 bits for alpha.
  DEF_FormatCheck( D3DFMT_A8B8G8R8) // 32-bit ARGB pixel format with alpha, using 8 bits per channel.
  DEF_FormatCheck( D3DFMT_X8B8G8R8) // 32-bit RGB pixel format, where 8 bits are reserved for each color.
  DEF_FormatCheck( D3DFMT_G16R16) // 32-bit pixel format using 16 bits each for green and red.
  DEF_FormatCheck( D3DFMT_G16R16F) // 32-bit float format using 16 bits for the red channel and 16 bits for the green channel.
  DEF_FormatCheck( D3DFMT_A2R10G10B10) // 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha.
  DEF_FormatCheck( D3DFMT_D32) // 32-bit z-buffer bit depth.
  DEF_FormatCheck( D3DFMT_D24S8) // 32-bit z-buffer bit depth using 24 bits for the depth channel and 8 bits for the stencil channel.
  DEF_FormatCheck( D3DFMT_D24X8) //32-bit z-buffer bit depth using 24 bits for the depth channel.
  DEF_FormatCheck( D3DFMT_D24X4S4) // 32-bit z-buffer bit depth using 24 bits for the depth channel and 4 bits for the stencil channel.
  DEF_FormatCheck( D3DFMT_D32F_LOCKABLE) // A lockable format where the depth value is represented as a standard IEEE floating-point number.
  DEF_FormatCheck( D3DFMT_D24FS8) // A non-lockable format that contains 24 bits of depth (in a 24-bit floating point format - 20e4) and 8 bits of stencil.
  DEF_FormatCheck( D3DFMT_D32_LOCKABLE) // A lockable 32-bit depth buffer.
  DEF_FormatCheck( D3DFMT_INDEX32) // 32-bit index buffer bit depth.

  DEF_FormatCheck( D3DFMT_G32R32F) // 64-bit float format using 32 bits for the red channel and 32 bits for the green channel.
  DEF_FormatCheck( D3DFMT_Q16W16V16U16) // 64-bit bump-map format using 16 bits for each component.
  DEF_FormatCheck( D3DFMT_A16B16G16R16) // 64-bit pixel format using 16 bits for each component.
  DEF_FormatCheck( D3DFMT_A16B16G16R16F) // 64-bit float format using 16 bits for the each channel (alpha, blue, green, red).

  DEF_FormatCheck( D3DFMT_A32B32G32R32F) // 128-bit float format using 32 bits for the each channel (alpha, blue, green, red).
  DEF_FormatCheck( D3DFMT_DXT2)
  DEF_FormatCheck( D3DFMT_DXT3)
  DEF_FormatCheck( D3DFMT_DXT4)
  DEF_FormatCheck( D3DFMT_DXT5)
  DEF_FormatCheck( D3DFMT_DXT1)
  default: //compressed formats
  {
    return (false);
  }
  }
  return (false);
}
int uMod_TextureClient_DX9::MergeUpdate(void)
{
  if (NumberOfUpdate<0) {return (RETURN_OK);}
  if (int ret = LockMutex()) {gl_ErrorState |= uMod_ERROR_TEXTURE ; return (ret);}

  Message("uMod_TextureClient_DX9::MergeUpdate(): %p\n", this);

  for (int i=0; i<NumberOfUpdate; i++) {Update[i].NumberOfTextures=0; Update[i].Textures = NULL;} // this is already done, but safety comes first ^^

  int pos_old=0;
  int pos_new=0;
  int *to_lookup = NULL;
  if (NumberOfUpdate>0) to_lookup = new int[NumberOfUpdate];
  int num_to_lookup = 0;

  /*
   * FileToMod contains the old files (textures) which should replace the target textures (if they are loaded by the game)
   * Update contains the new files (textures) which should replace the target textures (if they are loaded by the game)
   *
   * Both arrays (FileToMod and Update) are sorted according to their hash values.
   *
   * First we go through both arrays linearly and
   * 1) take over the old entry if the hash is the same,
   * 2) release old fake texture (if target texture exist and is not in the Update)
   * 3) or mark newly added fake texture (if they are not in FileToMod)
   */

  while (pos_old<NumberToMod && pos_new<NumberOfUpdate)
  {
    if (FileToMod[pos_old].Content()->Hash > Update[pos_new].Content()->Hash) // this fake texture is new
    {
      to_lookup[num_to_lookup++] = pos_new++; // keep this fake texture in mind, we must search later for it through all original textures
      // we increase only the new counter by one
    }
    else if (FileToMod[pos_old].Content()->Hash < Update[pos_new].Content()->Hash) // this fake texture is not in the update
    {
      /*
      for (int i=FileToMod[pos_old].NumberOfTextures-1; i>=0; i--)
          ((IDirect3DBaseTexture9*) FileToMod[pos_old].Textures[i])->Release(); // we release the fake textures
      if (FileToMod[pos_old].Textures!=NULL) delete [] FileToMod[pos_old].Textures; // we delete the memory
      FileToMod[pos_old].NumberOfTextures = 0;
      FileToMod[pos_old].Textures = NULL;
      */
      pos_old++; // we increase only the old counter by one
    }
    else // the hash value is the same, thus this texture is in the array FileToMod as well as in the array Update
    {
      if (Update[pos_new].Content()->ForceReload)
      {
        if (FileToMod[pos_old].NumberOfTextures>0)
        {
          Update[pos_new].Textures = (IUnknown**) new IDirect3DBaseTexture9*[FileToMod[pos_old].NumberOfTextures];
        }
        for (int i=0; i<FileToMod[pos_old].NumberOfTextures; i++)
        {
          IDirect3DBaseTexture9 *base_texture;
          int ret = ((IDirect3DBaseTexture9*) FileToMod[pos_old].Textures[i])->QueryInterface( IID_IDirect3D9, (void**)&base_texture);
          switch (ret)
          {
            case 0x01000000L:
            {
              uMod_IDirect3DTexture9 *pTexture = (uMod_IDirect3DTexture9*) FileToMod[pos_old].Textures[i];//
              uMod_IDirect3DTexture9 *pRefTexture = pTexture->CrossRef_D3Dtex;
              pTexture->Release();
              i--; //after the Release of the old fake texture FileToMod[pos_old].Textures[i] is overwritten by entries with index greater than i

              uMod_IDirect3DTexture9 *fake_Texture;
              if (int ret = LoadTexture( Update[pos_new].Content(), &fake_Texture)) return (ret);
              if (SwitchTextures( fake_Texture, pRefTexture))
              {
                Message("MergeUpdate(): textures not switched %#llX\n", pRefTexture->CRC64);
                fake_Texture->Release();
              }
              else
              {
                Update[pos_new].Textures[Update[pos_new].NumberOfTextures++] = fake_Texture;
                fake_Texture->Reference = pos_new;
              }
              break;
            }
            case 0x01000001L:
            {
              uMod_IDirect3DVolumeTexture9 *pTexture = (uMod_IDirect3DVolumeTexture9*) FileToMod[pos_old].Textures[i];//
              uMod_IDirect3DVolumeTexture9 *pRefTexture = pTexture->CrossRef_D3Dtex;
              pTexture->Release();
              i--; //after the Release of the old fake texture FileToMod[pos_old].Textures[i] is overwritten by entries with index greater than i

              uMod_IDirect3DVolumeTexture9 *fake_Texture;
              if (int ret = LoadTexture( Update[pos_new].Content(), &fake_Texture)) return (ret);
              if (SwitchTextures( fake_Texture, pRefTexture))
              {
                Message("MergeUpdate(): textures not switched %#llX\n", pRefTexture->CRC64);
                fake_Texture->Release();
              }
              else
              {
                Update[pos_new].Textures[Update[pos_new].NumberOfTextures++] = fake_Texture;
                fake_Texture->Reference = pos_new;
              }
              break;
            }
            case 0x01000002L:
            {
              uMod_IDirect3DCubeTexture9 *pTexture = (uMod_IDirect3DCubeTexture9*) FileToMod[pos_old].Textures[i];//
              uMod_IDirect3DCubeTexture9 *pRefTexture = pTexture->CrossRef_D3Dtex;
              pTexture->Release();
              i--; //after the Release of the old fake texture FileToMod[pos_old].Textures[i] is overwritten by entries with index greater than i

              uMod_IDirect3DCubeTexture9 *fake_Texture;
              if (int ret = LoadTexture( Update[pos_new].Content(), &fake_Texture)) return (ret);
              if (SwitchTextures( fake_Texture, pRefTexture))
              {
                Message("MergeUpdate(): textures not switched %#llX\n", pRefTexture->CRC64);
                fake_Texture->Release();
              }
              else
              {
                Update[pos_new].Textures[Update[pos_new].NumberOfTextures++] = fake_Texture;
                fake_Texture->Reference = pos_new;
              }
              break;
            }
            default:
              break; // this is no fake texture and QueryInterface failed, because IDirect3DBaseTexture9 object cannot be a IDirect3D9 object ;)
          }
        }
      }
      else // the texture might be loaded or not
      {
        Update[pos_new].NumberOfTextures = FileToMod[pos_old].NumberOfTextures;
        Update[pos_new].Textures = FileToMod[pos_old].Textures;
        FileToMod[pos_old].NumberOfTextures = 0;
        FileToMod[pos_old].Textures = NULL;
      }
      // we increase both counters by one
      pos_old++;
      pos_new++;
    }
  }
/*
  while (pos_old<NumberToMod) //this fake textures are not in the Update
  {
    for (int i=FileToMod[pos_old].NumberOfTextures-1; i>=0; i--)
      ((IDirect3DBaseTexture9*) FileToMod[pos_old].Textures[i])->Release(); // we release the fake textures
    //for (int i=0; i<FileToMod[pos_old].NumberOfTextures; i++) FileToMod[pos_old].Textures[i]->Release(); // we release the fake textures
    if (FileToMod[pos_old].Textures!=NULL) delete [] FileToMod[pos_old].Textures; // we delete the memory
    FileToMod[pos_old].Textures = NULL;
    pos_old++;
  }
  */
  while (pos_new<NumberOfUpdate) //this fake textures are newly added
  {
    to_lookup[num_to_lookup++] = pos_new++; //keep this fake texture in mind, we must search later for it through all original textures
  }


  if (FileToMod!=NULL)
  {
    delete [] FileToMod;
  }

  FileToMod = Update;
  NumberToMod = NumberOfUpdate;

  NumberOfUpdate = -1;
  Update = NULL;


  if (num_to_lookup>0)
  {
    uMod_IDirect3DTexture9* single_texture;
    void *cpy;
    long ret = D3D9Device->QueryInterface( IID_IDirect3DTexture9, &cpy);
    if (ret == 0x01000000L) single_texture = ((uMod_IDirect3DDevice9*)D3D9Device)->GetSingleTexture(); //this texture must no be added twice
    else single_texture = ((uMod_IDirect3DDevice9Ex*) D3D9Device)->GetSingleTexture(); //this texture must no be added twice

    int num = OriginalTextures.GetNumber();
    for (int i=0; i<num; i++) if (OriginalTextures[i]->CrossRef_D3Dtex==NULL || OriginalTextures[i]->CrossRef_D3Dtex==single_texture)
    {
      UnswitchTextures(OriginalTextures[i]); //this we can do always, so we unswitch the single texture
      LookUpToMod( OriginalTextures[i], num_to_lookup, to_lookup);
    }

    uMod_IDirect3DVolumeTexture9 *single_volume_texture;
    if (ret == 0x01000000L) single_volume_texture = ((uMod_IDirect3DDevice9*)D3D9Device)->GetSingleVolumeTexture(); //this texture must no be added twice
    else single_volume_texture = ((uMod_IDirect3DDevice9Ex*) D3D9Device)->GetSingleVolumeTexture(); //this texture must no be added twice
    num = OriginalVolumeTextures.GetNumber();
    for (int i=0; i<num; i++) if (OriginalVolumeTextures[i]->CrossRef_D3Dtex==NULL || OriginalVolumeTextures[i]->CrossRef_D3Dtex==single_volume_texture)
    {
      UnswitchTextures(OriginalVolumeTextures[i]); //this we can do always, so we unswitch the single texture
      LookUpToMod( OriginalVolumeTextures[i], num_to_lookup, to_lookup);
    }

    uMod_IDirect3DCubeTexture9 *single_cube_texture;
    if (ret == 0x01000000L) single_cube_texture = ((uMod_IDirect3DDevice9*)D3D9Device)->GetSingleCubeTexture(); //this texture must no be added twice
    else single_cube_texture = ((uMod_IDirect3DDevice9Ex*) D3D9Device)->GetSingleCubeTexture(); //this texture must no be added twice
    num = OriginalCubeTextures.GetNumber();
    for (int i=0; i<num; i++) if (OriginalCubeTextures[i]->CrossRef_D3Dtex==NULL || OriginalCubeTextures[i]->CrossRef_D3Dtex==single_cube_texture)
    {
      UnswitchTextures(OriginalCubeTextures[i]); //this we can do always, so we unswitch the single texture
      LookUpToMod( OriginalCubeTextures[i], num_to_lookup, to_lookup);
    }
  }



  if (to_lookup != NULL) delete [] to_lookup;

  Message("uMod_TextureClient_DX9::MergeUpdate() END: %p\n", this);

  return (UnlockMutex());
}


int uMod_TextureClient_DX9::LookUpToMod( uMod_IDirect3DTexture9* pTexture, int num_index_list, int *index_list) // should only be called for original textures
{
  Message("uMod_TextureClient_DX9::LookUpToMod( %p): hash: %#llX,  %p\n", pTexture, pTexture->CRC64, this);
  if (pTexture->CrossRef_D3Dtex!=NULL) return (RETURN_OK); // bug, this texture is already switched
  if (pTexture->CRC64==0) return (RETURN_OK);

  int index = GetIndex( pTexture->CRC64, num_index_list, index_list);
  if (index>=0)
  {
    uMod_IDirect3DTexture9 *fake_Texture;
    if (int ret = LoadTexture( FileToMod[index].Content(), &fake_Texture)) return (ret);
    if (SwitchTextures( fake_Texture, pTexture))
    {
      Message("uMod_TextureClient_DX9::LookUpToMod(): textures not switched %#llX\n", FileToMod[index].Content()->Hash);
      fake_Texture->Release();
    }
    else
    {
      fake_Texture->Reference = index;
      FileToMod[index].AddTexture( fake_Texture);
      /*
      IDirect3DBaseTexture9 **temp = new IDirect3DBaseTexture9*[FileToMod[index].NumberOfTextures+1];
      for (int j=0; j<FileToMod[index].NumberOfTextures; j++) temp[j] =  (IDirect3DBaseTexture9*) FileToMod[index].Textures[j];

      if (FileToMod[index].Textures!=NULL) delete [] FileToMod[index].Textures;
      FileToMod[index].Textures = (void**) temp;

      FileToMod[index].Textures[FileToMod[index].NumberOfTextures++] = fake_Texture;
      */
    }
  }
  else if (BoolComputeCRC32 &&  pTexture->CRC32>0)
  {
    index = GetIndex( pTexture->CRC32, num_index_list, index_list);
    if (index>=0)
    {
      uMod_IDirect3DTexture9 *fake_Texture;
      if (int ret = LoadTexture( FileToMod[index].Content(), &fake_Texture)) return (ret);
      if (SwitchTextures( fake_Texture, pTexture))
      {
        Message("uMod_TextureClient_DX9::LookUpToMod(): textures not switched %#llX\n", FileToMod[index].Content()->Hash);
        fake_Texture->Release();
      }
      else
      {
        fake_Texture->Reference = index;
        FileToMod[index].AddTexture(fake_Texture);
        /*
        IDirect3DBaseTexture9 **temp = new IDirect3DBaseTexture9*[FileToMod[index].NumberOfTextures+1];
        for (int j=0; j<FileToMod[index].NumberOfTextures; j++) temp[j] =  (IDirect3DBaseTexture9*) FileToMod[index].Textures[j];

        if (FileToMod[index].Textures!=NULL) delete [] FileToMod[index].Textures;
        FileToMod[index].Textures = (void**) temp;

        FileToMod[index].Textures[FileToMod[index].NumberOfTextures++] = fake_Texture;
        fake_Texture->Reference = index;
        */
      }
    }
  }
  return (RETURN_OK);
}

int uMod_TextureClient_DX9::LookUpToMod( uMod_IDirect3DVolumeTexture9* pTexture, int num_index_list, int *index_list) // should only be called for original textures
{
  Message("uMod_TextureClient_DX9::LookUpToMod( Volume %p): hash: %#llX,  %p\n", pTexture, pTexture->CRC64, this);
  if (pTexture->CrossRef_D3Dtex!=NULL) return (RETURN_OK); // bug, this texture is already switched
  int index = GetIndex( pTexture->CRC64, num_index_list, index_list);
  if (index>=0)
  {
    uMod_IDirect3DVolumeTexture9 *fake_Texture;
    if (int ret = LoadTexture( FileToMod[index].Content(), &fake_Texture)) return (ret);
    if (SwitchTextures( fake_Texture, pTexture))
    {
      Message("uMod_TextureClient_DX9::LookUpToMod(): textures not switched %#llX\n", FileToMod[index].Content()->Hash);
      fake_Texture->Release();
    }
    else
    {
      fake_Texture->Reference = index;
      FileToMod[index].AddTexture(fake_Texture);
      /*
      IDirect3DBaseTexture9 **temp = new IDirect3DBaseTexture9*[FileToMod[index].NumberOfTextures+1];
      for (int j=0; j<FileToMod[index].NumberOfTextures; j++) temp[j] =  (IDirect3DBaseTexture9*) FileToMod[index].Textures[j];

      if (FileToMod[index].Textures!=NULL) delete [] FileToMod[index].Textures;
      FileToMod[index].Textures = (void**) temp;

      FileToMod[index].Textures[FileToMod[index].NumberOfTextures++] = fake_Texture;
      fake_Texture->Reference = index;
      */
    }
  }
  else if (BoolComputeCRC32 &&  pTexture->CRC32>0)
  {
    index = GetIndex( pTexture->CRC32, num_index_list, index_list);
    if (index>=0)
    {
      uMod_IDirect3DVolumeTexture9 *fake_Texture;
      if (int ret = LoadTexture( FileToMod[index].Content(), &fake_Texture)) return (ret);
      if (SwitchTextures( fake_Texture, pTexture))
      {
        Message("uMod_TextureClient_DX9::LookUpToMod(): textures not switched %#llX\n", FileToMod[index].Content()->Hash);
        fake_Texture->Release();
      }
      else
      {
        fake_Texture->Reference = index;
        FileToMod[index].AddTexture(fake_Texture);
        /*
        IDirect3DBaseTexture9 **temp = new IDirect3DBaseTexture9*[FileToMod[index].NumberOfTextures+1];
        for (int j=0; j<FileToMod[index].NumberOfTextures; j++) temp[j] =  (IDirect3DBaseTexture9*) FileToMod[index].Textures[j];

        if (FileToMod[index].Textures!=NULL) delete [] FileToMod[index].Textures;
        FileToMod[index].Textures = (void**) temp;

        FileToMod[index].Textures[FileToMod[index].NumberOfTextures++] = fake_Texture;
        fake_Texture->Reference = index;
        */
      }
    }
  }
  return (RETURN_OK);
}

int uMod_TextureClient_DX9::LookUpToMod( uMod_IDirect3DCubeTexture9* pTexture, int num_index_list, int *index_list) // should only be called for original textures
{
  Message("uMod_TextureClient_DX9::LookUpToMod( Cube %p): hash: %#llX,  %p\n", pTexture, pTexture->CRC64, this);
  if (pTexture->CrossRef_D3Dtex!=NULL) return (RETURN_OK); // bug, this texture is already switched
  int index = GetIndex( pTexture->CRC64, num_index_list, index_list);
  if (index>=0)
  {
    uMod_IDirect3DCubeTexture9 *fake_Texture;
    if (int ret = LoadTexture( FileToMod[index].Content(), &fake_Texture)) return (ret);
    if (SwitchTextures( fake_Texture, pTexture))
    {
      Message("uMod_TextureClient_DX9::LookUpToMod(): textures not switched %#llX\n", FileToMod[index].Content()->Hash);
      fake_Texture->Release();
    }
    else
    {
      fake_Texture->Reference = index;
      FileToMod[index].AddTexture(fake_Texture);
      /*
      IDirect3DBaseTexture9 **temp = new IDirect3DBaseTexture9*[FileToMod[index].NumberOfTextures+1];
      for (int j=0; j<FileToMod[index].NumberOfTextures; j++) temp[j] =  (IDirect3DBaseTexture9*) FileToMod[index].Textures[j];

      if (FileToMod[index].Textures!=NULL) delete [] FileToMod[index].Textures;
      FileToMod[index].Textures = (void**) temp;

      FileToMod[index].Textures[FileToMod[index].NumberOfTextures++] = fake_Texture;
      fake_Texture->Reference = index;
      */
    }
  }
  else if (BoolComputeCRC32 &&  pTexture->CRC32>0)
  {
    index = GetIndex( pTexture->CRC32, num_index_list, index_list);
    if (index>=0)
    {
      uMod_IDirect3DCubeTexture9 *fake_Texture;
      if (int ret = LoadTexture( FileToMod[index].Content(), &fake_Texture)) return (ret);
      if (SwitchTextures( fake_Texture, pTexture))
      {
        Message("uMod_TextureClient_DX9::LookUpToMod(): textures not switched %#llX\n", FileToMod[index].Content()->Hash);
        fake_Texture->Release();
      }
      else
      {
        fake_Texture->Reference = index;
        FileToMod[index].AddTexture(fake_Texture);
        /*
        IDirect3DBaseTexture9 **temp = new IDirect3DBaseTexture9*[FileToMod[index].NumberOfTextures+1];
        for (int j=0; j<FileToMod[index].NumberOfTextures; j++) temp[j] =  (IDirect3DBaseTexture9*) FileToMod[index].Textures[j];

        if (FileToMod[index].Textures!=NULL) delete [] FileToMod[index].Textures;
        FileToMod[index].Textures = (void**) temp;

        FileToMod[index].Textures[FileToMod[index].NumberOfTextures++] = fake_Texture;
        fake_Texture->Reference = index;
        */
      }
    }
  }
  return (RETURN_OK);
}




int uMod_TextureClient_DX9::LoadTexture( const TextureFileContent* file, uMod_IDirect3DTexture9 **ppTexture) // to load fake texture from a file in memory
{
  Message("uMod_TextureClient_DX9::LoadTexture( %p, %p, %#llX): %p\n", file, ppTexture, file->Hash, this);

  void *cpy;
  long ret = D3D9Device->QueryInterface( IID_IDirect3DTexture9, &cpy);
  if (ret == 0x01000000L)
  {
    uMod_IDirect3DDevice9* device = ((uMod_IDirect3DDevice9*)D3D9Device);
    device->SetNextTextureIsFake(true);
  }
  else
  {
    uMod_IDirect3DDevice9Ex* deviceEx = ((uMod_IDirect3DDevice9Ex*) D3D9Device);
    deviceEx->SetNextTextureIsFake(true);
  }

  HRESULT res = D3DERR_INVALIDCALL;

  if (file->pData!=NULL) res = D3DXCreateTextureFromFileInMemoryEx( D3D9Device, file->pData, file->Size, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DTexture9 **) ppTexture);
  else if (file->pFileName != NULL) res = D3DXCreateTextureFromFileExW( D3D9Device, file->pFileName, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DTexture9 **) ppTexture);

  if (res != D3D_OK)
  {
    *ppTexture=NULL;
    return (RETURN_TEXTURE_NOT_LOADED);
  }

  Message("uMod_TextureClient_DX9::LoadTexture( %p, %#llX): DONE\n", *ppTexture, file->Hash);
  return (RETURN_OK);
}

int uMod_TextureClient_DX9::LoadTexture( const TextureFileContent* file, uMod_IDirect3DVolumeTexture9 **ppTexture) // to load fake texture from a file in memory
{
  Message("uMod_TextureClient_DX9::LoadTexture( Volume %p, %p, %#llX): %p\n", file, ppTexture, file->Hash, this);

  void *cpy;
  long ret = D3D9Device->QueryInterface( IID_IDirect3DTexture9, &cpy);
  if (ret == 0x01000000L)
  {
    uMod_IDirect3DDevice9* device = ((uMod_IDirect3DDevice9*)D3D9Device);
    device->SetNextTextureIsFake(true);
  }
  else
  {
    uMod_IDirect3DDevice9Ex* deviceEx = ((uMod_IDirect3DDevice9Ex*) D3D9Device);
    deviceEx->SetNextTextureIsFake(true);
  }

  HRESULT res = D3DERR_INVALIDCALL;
  if (file->pData!=NULL) res = D3DXCreateVolumeTextureFromFileInMemoryEx( D3D9Device, file->pData, file->Size, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DVolumeTexture9 **) ppTexture);
  else if (file->pFileName != NULL) res = D3DXCreateVolumeTextureFromFileExW( D3D9Device, file->pFileName, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DVolumeTexture9 **) ppTexture);

  if (res != D3D_OK)
  {
    *ppTexture=NULL;
    return (RETURN_TEXTURE_NOT_LOADED);
  }

  Message("uMod_TextureClient_DX9::LoadTexture( Volume %p, %#llX): DONE\n", *ppTexture, file->Hash);
  return (RETURN_OK);
}

int uMod_TextureClient_DX9::LoadTexture( const TextureFileContent* file, uMod_IDirect3DCubeTexture9 **ppTexture) // to load fake texture from a file in memory
{
  Message("uMod_TextureClient_DX9::LoadTexture( Cube %p, %p, %#llX): %p\n", file, ppTexture, file->Hash, this);

  void *cpy;
  long ret = D3D9Device->QueryInterface( IID_IDirect3DTexture9, &cpy);
  if (ret == 0x01000000L)
  {
    uMod_IDirect3DDevice9* device = ((uMod_IDirect3DDevice9*)D3D9Device);
    device->SetNextTextureIsFake(true);
  }
  else
  {
    uMod_IDirect3DDevice9Ex* deviceEx = ((uMod_IDirect3DDevice9Ex*) D3D9Device);
    deviceEx->SetNextTextureIsFake(true);
  }

  HRESULT res = D3DERR_INVALIDCALL;
  if (file->pData!=NULL) res = D3DXCreateCubeTextureFromFileInMemoryEx( D3D9Device, file->pData, file->Size, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DCubeTexture9 **) ppTexture);
  else if (file->pFileName != NULL) res = D3DXCreateCubeTextureFromFileExW( D3D9Device, file->pFileName, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, (IDirect3DCubeTexture9 **) ppTexture);

  if (res != D3D_OK)
  {
    *ppTexture=NULL;
    return (RETURN_TEXTURE_NOT_LOADED);
  }

  Message("uMod_TextureClient_DX9::LoadTexture( Cube %p, %#llX): DONE\n", *ppTexture, file->Hash);
  return (RETURN_OK);
}




