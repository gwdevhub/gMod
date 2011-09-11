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



#include "OTM_Main.h"

FILE *file;




OTM_TextureClient::OTM_TextureClient(OTM_TextureServer* server, IDirect3DDevice9* device)
{
  Message("OTM_TextureClient(void): %lu\n", this);
  Server = server;
  D3D9Device = device;
  BoolSaveAllTextures = false;
  SavePath[0]=0;
  if (Server!=NULL)
  {
    if (Server->AddClient( this, &FileToMod, &NumberToMod))
    {
      Server=NULL;
      NumberToMod = 0;
      FileToMod = NULL;
    }
  }
  Mutex = NULL;//CreateMutex(NULL, false, NULL);

  Update = NULL;
  NumberOfUpdate = -1;

  //Message("end OTM_TextureClient(void): %lu\n", this);
}

OTM_TextureClient::~OTM_TextureClient(void)
{
  Message("~OTM_TextureClient(void): %lu\n", this);
  if (Server!=NULL) Server->RemoveClient(this);

 //Message("after Server ~OTM_TextureClient(void): %lu\n", this);

  if (Mutex!=NULL) CloseHandle(Mutex);
  //Message("after CloseHandle ~OTM_TextureClient(void): %lu\n", this);

  if (Update!=NULL) delete [] Update;
  //Message("after Update ~OTM_TextureClient(void): %lu\n", this);

  if (FileToMod!=NULL) delete [] FileToMod;
  //Message("end ~OTM_TextureClient(void): %lu\n", this);
}

int OTM_TextureClient::AddTexture( OTM_IDirect3DTexture9* pTexture)
{
  if (pTexture->FAKE) return  (RETURN_OK); // this is a fake texture
  Message("AddTexture( %lu): %lu (%lu)\n", pTexture, this, GetCurrentThread());

  D3DLOCKED_RECT d3dlr;
  if (pTexture->LockRect( 0, &d3dlr, NULL, 0)!=D3D_OK)
  {
    return (RETURN_LockRect_FAILED);
  }
  D3DSURFACE_DESC desc;
  if (pTexture->GetLevelDesc(0, &desc)!=D3D_OK)
  {
    pTexture->UnlockRect(0);
    return (RETURN_GetLevelDesc_FAILED);
  }

  int size=0;
  switch(desc.Format)
  {
  case D3DFMT_A1: // 1-bit monochrome.
  {
    size = desc.Width*desc.Height/8;
    break;
  }

  case D3DFMT_R3G3B2: // 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue.
  case D3DFMT_A8: // 8-bit alpha only.
  case D3DFMT_A8P8: // 8-bit color indexed with 8 bits of alpha.
  case D3DFMT_P8: // 8-bit color indexed.
  case D3DFMT_L8: // 8-bit luminance only.
  case D3DFMT_A4L4: // 8-bit using 4 bits each for alpha and luminance.
  case D3DFMT_FORCE_DWORD:
  {
    size = desc.Width*desc.Height;
    break;
  }

  case D3DFMT_L6V5U5: // 16-bit bump-map format with luminance using 6 bits for luminance, and 5 bits each for v and u.
  case D3DFMT_V8U8: // 16-bit bump-map format using 8 bits each for u and v data.
  case D3DFMT_R5G6B5: // 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue.
  case D3DFMT_X1R5G5B5: // 16-bit pixel format where 5 bits are reserved for each color.
  case D3DFMT_A1R5G5B5: // 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha.
  case D3DFMT_A4R4G4B4: // 16-bit ARGB pixel format with 4 bits for each channel.
  case D3DFMT_A8R3G3B2: // 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue.
  case D3DFMT_X4R4G4B4: // 16-bit RGB pixel format using 4 bits for each color.
  case D3DFMT_L16: // 16-bit luminance only.
  case D3DFMT_A8L8: // 16-bit using 8 bits each for alpha and luminance.
  {
    size = 2*desc.Width*desc.Height;
    break;
  }


  case D3DFMT_R8G8B8: //24-bit RGB pixel format with 8 bits per channel.
  {
    size = 3*desc.Width*desc.Height;
    break;
  }

  case D3DFMT_R32F: // 32-bit float format using 32 bits for the red channel.
  case D3DFMT_X8L8V8U8: // 32-bit bump-map format with luminance using 8 bits for each channel.
  case D3DFMT_A2W10V10U10: // 32-bit bump-map format using 2 bits for alpha and 10 bits each for w, v, and u.
  case D3DFMT_Q8W8V8U8: // 32-bit bump-map format using 8 bits for each channel.
  case D3DFMT_V16U16: // 32-bit bump-map format using 16 bits for each channel.
  case D3DFMT_A8R8G8B8: // 32-bit ARGB pixel format with alpha, using 8 bits per channel.
  case D3DFMT_X8R8G8B8: // 32-bit RGB pixel format, where 8 bits are reserved for each color.
  case D3DFMT_A2B10G10R10: // 32-bit pixel format using 10 bits for each color and 2 bits for alpha.
  case D3DFMT_A8B8G8R8: // 32-bit ARGB pixel format with alpha, using 8 bits per channel.
  case D3DFMT_X8B8G8R8: // 32-bit RGB pixel format, where 8 bits are reserved for each color.
  case D3DFMT_G16R16: // 32-bit pixel format using 16 bits each for green and red.
  case D3DFMT_A2R10G10B10: // 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha.
  {
    size = 4*desc.Width*desc.Height;
    break;
  }

  case D3DFMT_G32R32F: // 64-bit float format using 32 bits for the red channel and 32 bits for the green channel.
  case D3DFMT_Q16W16V16U16: // 64-bit bump-map format using 16 bits for each component.
  case D3DFMT_A16B16G16R16: // 64-bit pixel format using 16 bits for each component.
  {
    size = 8*desc.Width*desc.Height;
    break;
  }

  case D3DFMT_A32B32G32R32F: // 128-bit float format using 32 bits for the each channel (alpha, blue, green, red).
  {
    size = 16*desc.Width*desc.Height;
    break;
  }
  default: //compressed formats
  {
    size = desc.Width*desc.Height/2;
    break;
  }
  }
  MyTypeHash hash = GetHash( (unsigned char*) d3dlr.pBits, size);

  if (pTexture->UnlockRect(0)!=D3D_OK)
  {
    return (RETURN_UnlockRect_FAILED);
  }
  pTexture->Hash = hash; // note: this will only be done for original textures
  if (BoolSaveAllTextures) SaveTexture(pTexture);

  if (gl_ErrorState & OTM_ERROR_FATAL) return (RETURN_FATAL_ERROR);

  OriginalTextures.Add( pTexture);

  return (LookUpToMod(pTexture)); // check if this texture should be modded
}



int OTM_TextureClient::RemoveTexture( OTM_IDirect3DTexture9* pTexture) // is called from a texture, if it is finally released
{
  Message("RemoveTexture( %lu): %lu\n", pTexture, this);

  IDirect3DDevice9 *dev = NULL;
  if (pTexture != NULL && pTexture->GetDevice(&dev) == D3D_OK)
  {
    if (dev == D3D9Device)
    {
      if (gl_ErrorState & OTM_ERROR_FATAL) return (RETURN_FATAL_ERROR);
      if (pTexture->FAKE)
      {
        for (int i=0; i<NumberToMod; i++) if (FileToMod[i].pTexture==pTexture) {FileToMod[i].pTexture = NULL; break;}
        return (FakeTextures.Remove( pTexture));
      }
      else return (OriginalTextures.Remove( pTexture));
    }
  }
  return (RETURN_OK);
}

int OTM_TextureClient::ReleaseAllFakeTexture(void)
{
  Message("ReleaseAllFakeTexture(): %lu\n", this);
  return (FakeTextures.RemoveAll());
}

int OTM_TextureClient::SaveAllTextures(bool val)
{
  Message("SaveAllTextures( %d): %lu\n", val, this);
  BoolSaveAllTextures=val;
  return (RETURN_OK);
}

int OTM_TextureClient::SaveSingleTexture(bool val)
{
  Message("SaveSingleTexture( %d): %lu\n", val, this);
  if (D3D9Device!=NULL) return (((OTM_IDirect3DDevice9*)D3D9Device)->SaveSingleTexture(val));
  else return (RETURN_NO_IDirect3DDevice9);
}


int OTM_TextureClient::SetSaveDirectory( wchar_t *dir)
{
  Message("SetSaveDirectory( %ls): %lu\n", dir, this);
  int i = 0;
  for (i=0; i<MAX_PATH && (dir[i]); i++) SavePath[i] = dir[i];
  if (i==MAX_PATH)
  {
    SavePath[0]=0;
    return (RETURN_BAD_ARGUMENT);
  }
  SavePath[i]=0;
  return (RETURN_OK);
}

int OTM_TextureClient::SaveTexture(OTM_IDirect3DTexture9* pTexture)
{
  Message("SaveTexture( %lu): %lu\n", pTexture, this);
  if (pTexture==NULL) return (RETURN_BAD_ARGUMENT);
  wchar_t file[MAX_PATH];
  swprintf_s( file, MAX_PATH, L"%ls\\%#lX.dds", SavePath, pTexture->Hash);

  if (D3D_OK!=D3DXSaveTextureToFileW( file, D3DXIFF_DDS, pTexture->m_D3Dtex, NULL)) return (RETURN_TEXTURE_NOT_SAVED);
  return (RETURN_OK);
}



int OTM_TextureClient::AddUpdate(TextureFileStruct* update, int number)  //client must delete the temp array
{
  Message("AddUpdate( %lu, %d): %lu\n", update, number, this);
  //if (update==NULL || number<=0) return (RETURN_OK);
  if (int ret = LockMutex()) {gl_ErrorState |= OTM_ERROR_TEXTURE; return (ret);}
  if (Update!=NULL) delete [] Update;
  Update = update;
  NumberOfUpdate = number;
  return (UnlockMutex());
}



int OTM_TextureClient::MergeUpdate(void)
{
  if (NumberOfUpdate<0) {Update=NULL; return  (RETURN_OK);}
  if (int ret = LockMutex()) {gl_ErrorState |= OTM_ERROR_TEXTURE ; return (ret);}

  Message("MergeUpdate(): %lu\n", this);

  for (int i=0; i<NumberOfUpdate; i++) Update[i].Checked=false;

  // looking through all current modified texture and removing if texture is not any more in the update or take it over otherwise
  for (int i=0; i<NumberToMod; i++)
  {
    MyTypeHash hash = FileToMod[i].Hash;
    bool found=false;
    for (int u=0; u<NumberOfUpdate; u++)
    {
      if (hash==Update[u].Hash)
      {
        Update[u].Checked = true;
        found = true;
        Update[u].pTexture = FileToMod[i].pTexture; //might also be a NULL pointer
        if (Update[u].ForceReload && Update[i].pTexture!=NULL)
        {
          OTM_IDirect3DTexture9 *pTexture = Update[u].pTexture->CrossRef_D3Dtex;
          RemoveTexture( Update[u].pTexture);
          Update[u].pTexture = NULL;
          if (pTexture!=NULL) //should always be the case
          {
            OTM_IDirect3DTexture9 *fake_Texture;
            if (int ret = LoadTexture( & (Update[u]), &fake_Texture)) return (ret);
            if (SwitchTextures( fake_Texture, pTexture))
            {
              Message("MergeUpdate(): textures not switched %#lX\n", hash);
              fake_Texture->Release();
            }
            else
            {
              FakeTextures.Add(fake_Texture);
              Update[u].pTexture = fake_Texture;
            }
          }
        }
        break;
      }
    }
    if (!found)
    {
      FakeTextures.Remove(FileToMod[i].pTexture);
    }
  }

  // looking through the update and switch the textures if they are currently used in the game
  int num = OriginalTextures.GetNumber();
  for (int u=0; u<NumberOfUpdate; u++) if (!Update[u].Checked && Update[u].pTexture==NULL)
  {
    MyTypeHash hash = Update[u].Hash;
    for (int i=0; i<num; i++) if (hash==OriginalTextures[i]->Hash && OriginalTextures[i]->CrossRef_D3Dtex==NULL)
    {
      Update[u].Checked = true;
      OTM_IDirect3DTexture9 *pTexture = OriginalTextures[i];
      OTM_IDirect3DTexture9 *fake_Texture;
      if (RETURN_OK == LoadTexture( & (Update[u]), &fake_Texture))
      {
        if (SwitchTextures( fake_Texture, pTexture))
        {
          Message("MergeUpdate(): textures not switched %#lX\n", hash);
          fake_Texture->Release();
        }
        else
        {
          Update[u].pTexture = fake_Texture;
          FakeTextures.Add(fake_Texture);
        }
      }
    }
  }

  if (FileToMod!=NULL) delete [] FileToMod;
  FileToMod = Update;
  NumberToMod = NumberOfUpdate;

  NumberOfUpdate = -1;
  Update = NULL;
  return (UnlockMutex());
}



int OTM_TextureClient::LockMutex(void)
{
  //if (( gl_ErrorState & (OTM_ERROR_FATAL | OTM_ERROR_MUTEX) )) return (RETURN_NO_MUTEX);
  //if (WAIT_OBJECT_0!=WaitForSingleObject( Mutex, 100)) return (RETURN_MUTEX_LOCK); //waiting 100ms, to wait infinite pass INFINITE
  return  (RETURN_OK);
}

int OTM_TextureClient::UnlockMutex(void)
{
  //if (ReleaseMutex(Mutex)==0) return (RETURN_MUTEX_UNLOCK);
  return  (RETURN_OK);
}





int OTM_TextureClient::LookUpToMod( OTM_IDirect3DTexture9* pTexture) // should only be called for original textures
{
  Message("LookUpToMod( %lu): hash: %lu,  %lu\n", pTexture, pTexture->Hash, this);
  if (pTexture->CrossRef_D3Dtex!=NULL) return (RETURN_OK); // to avoid broken links
  MyTypeHash hash = pTexture->Hash;
  for (int i=0; i<NumberToMod; i++)
  {
    if (hash == FileToMod[i].Hash && FileToMod[i].pTexture==NULL) // to avoid broken links
    {
      OTM_IDirect3DTexture9 *fake_Texture;
      if (int ret = LoadTexture( & (FileToMod[i]), &fake_Texture)) return (ret);
      if (SwitchTextures( fake_Texture, pTexture))
      {
        Message("MergeUpdate(): textures not switched %#lX\n", hash);
        fake_Texture->Release();
      }
      else
      {
        FakeTextures.Add(fake_Texture);
        FileToMod[i].pTexture = fake_Texture;
      }
    }
  }
  return (RETURN_OK);
}


int OTM_TextureClient::LoadTexture( TextureFileStruct* file_in_memory, OTM_IDirect3DTexture9 **ppTexture) // to load fake texture from file in memory
{
  Message("LoadTexture( %lu, %lu): %lu\n", file_in_memory, ppTexture, this);

  if (D3D_OK != D3DXCreateTextureFromFileInMemory( D3D9Device, file_in_memory->pData, file_in_memory->Size, (IDirect3DTexture9 **) ppTexture))
  {
    *ppTexture=NULL;
    return (RETURN_TEXTURE_NOT_LOADED);
  }
  (*ppTexture)->FAKE = true;
  Message("LoadTexture(): DONE\n", file_in_memory, ppTexture, this);
  return (RETURN_OK);
}


MyTypeHash OTM_TextureClient::GetHash(unsigned char *str, int len) // estimate the hash
{
  MyTypeHash hash = 0;
  for (int i=0; i<len; i++) hash = str[i] + (hash << 6) + (hash << 16) - hash;
  return (hash);
}

