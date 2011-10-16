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


OTM_TextureClient::OTM_TextureClient(OTM_TextureServer* server, IDirect3DDevice9* device)
{
  Message("OTM_TextureClient(void): %lu\n", this);
  Server = server;
  D3D9Device = device;
  BoolSaveAllTextures = false;
  BoolSaveSingleTexture = false;
  KeyBack = 0;
  KeySave = 0;
  KeyNext = 0;
  SavePath[0]=0;
  GameName[0]=0;
  if (Server!=NULL)
  {
    if (Server->AddClient( this, &FileToMod, &NumberToMod))
    {
      Server=NULL;
      NumberToMod = 0;
      FileToMod = NULL;
    }
  }
  Mutex = CreateMutex(NULL, false, NULL);

  Update = NULL;
  NumberOfUpdate = -1;
  FontColour = D3DCOLOR_ARGB(255,255,0,0);
  TextureColour = D3DCOLOR_ARGB(255,0,255,0);
}

OTM_TextureClient::~OTM_TextureClient(void)
{
  Message("~OTM_TextureClient(void): %lu\n", this);
  if (Server!=NULL) Server->RemoveClient(this);

  if (Mutex!=NULL) CloseHandle(Mutex);

  if (Update!=NULL) delete [] Update;
  if (FileToMod!=NULL) delete [] FileToMod;
}


int OTM_TextureClient::AddTexture( OTM_IDirect3DTexture9* pTexture)
{
  ((OTM_IDirect3DDevice9*)D3D9Device)->SetLastCreatedTexture(NULL); //this texture must no be added twice
  if (pTexture->FAKE) return (RETURN_OK); // this is a fake texture

  Message("OTM_TextureClient::AddTexture( %lu): %lu (thread: %lu)\n", pTexture, this, GetCurrentThread());

  D3DLOCKED_RECT d3dlr;
  if (pTexture->LockRect( 0, &d3dlr, NULL, 0)!=D3D_OK) //get the raw data of the texture
  {
    return (RETURN_LockRect_FAILED);
  }
  D3DSURFACE_DESC desc;
  if (pTexture->GetLevelDesc(0, &desc)!=D3D_OK) //get the format and the size of the texture
  {
    pTexture->UnlockRect(0);
    return (RETURN_GetLevelDesc_FAILED);
  }

  int size=0;
  switch(desc.Format) //switch trough the formats to calculate the size of the raw data
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

  //MyTypeHash hash = GetHash( (unsigned char*) d3dlr.pBits, size);
  MyTypeHash hash = GetCRC32( (char*) d3dlr.pBits, size); //calaculate the crc32 of the texture

  if (pTexture->UnlockRect(0)!=D3D_OK) //unlock the raw data
  {
    return (RETURN_UnlockRect_FAILED);
  }

  pTexture->Hash = hash;
  if (BoolSaveAllTextures) SaveTexture(pTexture);

  if (gl_ErrorState & OTM_ERROR_FATAL) return (RETURN_FATAL_ERROR);

  OriginalTextures.Add( pTexture); // add the texture to the list of original texture

  return (LookUpToMod(pTexture)); // check if this texture should be modded
}



int OTM_TextureClient::RemoveTexture( OTM_IDirect3DTexture9* pTexture) // is called from a texture, if it is finally released
{
  Message("RemoveTexture( %lu): %lu\n", pTexture, this);

  IDirect3DDevice9 *dev = NULL;
  if (pTexture != NULL && pTexture->GetDevice(&dev) == D3D_OK)
  {
    // this condition is senseless, since this function is only called if a OTM_IDirect3DTexture9 object is released
    // and OTM_IDirect3DTexture9 object are created only from a OTM_IDirect3DDevice9 object, hence the pointers must be equal
    if (dev == D3D9Device)
    {
      if (gl_ErrorState & OTM_ERROR_FATAL) return (RETURN_FATAL_ERROR);
      if (pTexture->FAKE)
      {
        // we need to set the corresponding FileToMod[X].pTexture to NULL, to avoid a link to a non existing texture object
        int ref = pTexture->Reference;
        if (ref>=0 && ref<NumberToMod && FileToMod[ref].pTexture==pTexture) FileToMod[ref].pTexture = NULL;
      }
      else
      {
        /* this is already done in the Release function of the original texture
        if (pTexture->CrossRef_D3Dtex!=NULL)
        {
          OTM_IDirect3DTexture9* fake_texture = pTexture->CrossRef_D3Dtex;
          UnswitchTextures(fake_texture);
          fake_texture ->Release(); //this will call this->RemoveTexture again and the fake_texture will also be deleted from the FileToMod.
        }
        */
        return (OriginalTextures.Remove( pTexture)); //remove this texture form the list
      }
    }
  }
  return (RETURN_OK);
}


int OTM_TextureClient::SaveAllTextures(bool val)
{
  Message("OTM_TextureClient::SaveAllTextures( %d): %lu\n", val, this);
  BoolSaveAllTextures=val;
  return (RETURN_OK);
}

int OTM_TextureClient::SaveSingleTexture(bool val)
{
  Message("OTM_TextureClient::SaveSingleTexture( %d): %lu\n", val, this);
  if (BoolSaveSingleTexture && !val) //if BoolSaveSingleTexture is set to false and was previously true we switch the SingleTexture back
  {
    OTM_IDirect3DTexture9* pTexture = ((OTM_IDirect3DDevice9*)D3D9Device)->GetSingleTexture();
    if (pTexture!=NULL) UnswitchTextures(pTexture);
  }
  BoolSaveSingleTexture = val;
  return (RETURN_OK);
}


int OTM_TextureClient::SetSaveDirectory( wchar_t *dir)
{
  Message("OTM_TextureClient::SetSaveDirectory( %ls): %lu\n", dir, this);
  int i = 0;
  for (i=0; i<MAX_PATH && (dir[i]); i++) SavePath[i] = dir[i];
  if (i==MAX_PATH)
  {
    SavePath[0]=0;
    return (RETURN_BAD_ARGUMENT);
  }
  else SavePath[i]=0;
  return (RETURN_OK);
}

int OTM_TextureClient::SetGameName( wchar_t *name)
{
  Message("OTM_TextureClient::SetGameName( %ls): %lu\n", name, this);
  int i = 0;
  for (i=0; i<MAX_PATH && (name[i]); i++) GameName[i] = name[i];
  if (i==MAX_PATH)
  {
    GameName[0]=0;
    return (RETURN_BAD_ARGUMENT);
  }
  else GameName[i]=0;
  return (RETURN_OK);
}

int OTM_TextureClient::SaveTexture(OTM_IDirect3DTexture9* pTexture)
{
  if (pTexture==NULL) return (RETURN_BAD_ARGUMENT);
  if (SavePath[0]==0) {Message("OTM_TextureClient::SaveTexture( %#lX, %lu): %lu,   SavePath not set\n", pTexture->Hash, pTexture->m_D3Dtex, this); return (RETURN_TEXTURE_NOT_SAVED);}

  wchar_t file[MAX_PATH];
  if (GameName[0]) swprintf_s( file, MAX_PATH, L"%ls\\%ls_%#lX.dds", SavePath, GameName, pTexture->Hash);
  else swprintf_s( file, MAX_PATH, L"%ls\\%#lX.dds", SavePath, pTexture->Hash);
  Message("OTM_TextureClient::SaveTexture( %ls): %lu\n", file, this);

  if (D3D_OK!=D3DXSaveTextureToFileW( file, D3DXIFF_DDS, pTexture->m_D3Dtex, NULL)) return (RETURN_TEXTURE_NOT_SAVED);
  return (RETURN_OK);
}



int OTM_TextureClient::AddUpdate(TextureFileStruct* update, int number)  //client must delete the update array
{
  Message("AddUpdate( %lu, %d): %lu\n", update, number, this);
  if (int ret = LockMutex()) {gl_ErrorState |= OTM_ERROR_TEXTURE; return (ret);}
  if (Update!=NULL) delete [] Update;
  Update = update;
  NumberOfUpdate = number;
  return (UnlockMutex());
}



int OTM_TextureClient::MergeUpdate(void)
{
  if (NumberOfUpdate<0) {return (RETURN_OK);}
  if (int ret = LockMutex()) {gl_ErrorState |= OTM_ERROR_TEXTURE ; return (ret);}

  Message("MergeUpdate(): %lu\n", this);

  for (int i=0; i<NumberOfUpdate; i++) Update[i].pTexture = NULL; //this is already done, but safety comes first ^^


  // looking through all current modified texture and removing if texture is not any more in the update or take it over otherwise
  for (int i=0; i<NumberToMod; i++) //if NumberToMod is zero, we must merge nothing
  {
    MyTypeHash hash = FileToMod[i].Hash;
    bool found=false;
    for (int u=0; u<NumberOfUpdate; u++) //if NumberOfUpdate is zero, each fake texture will be released
    {
      if (hash==Update[u].Hash)
      {
        found = true;
        Update[u].pTexture = FileToMod[i].pTexture; //might also be a NULL pointer


        if (Update[u].pTexture!=NULL)
        {
          if (Update[u].ForceReload) // if force is enable, we reload the texture
          {
            OTM_IDirect3DTexture9 *pTexture = Update[u].pTexture->CrossRef_D3Dtex;
            Update[u].pTexture->Release(); //release the old fake texture
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
                Update[u].pTexture = fake_Texture;
                fake_Texture->Reference = u;
              }
            }
          }
          else Update[u].pTexture->Reference = u; //set the new reference, needed for a fast delete
        }
        break;
      }
    }
    if (!found)
    {
      if (FileToMod[i].pTexture!=NULL) FileToMod[i].pTexture->Release(); //release the fake texture, if it is not included in the update
    }
  }

  // looking through the non switched texture in the update and search for a target hash in the list of original game textures
  int num = OriginalTextures.GetNumber();
  for (int u=0; u<NumberOfUpdate; u++) if (Update[u].pTexture==NULL) //for this fake texture the target texture was not found
  {
    MyTypeHash hash = Update[u].Hash;
    for (int i=0; i<num; i++) if (hash==OriginalTextures[i]->Hash)
    {
      if (OriginalTextures[i]->CrossRef_D3Dtex!=NULL)
      {
        //if the texture is already switched, it might be the SingleTexture (for saving a single texture)
        if (OriginalTextures[i]->CrossRef_D3Dtex==((OTM_IDirect3DDevice9*)D3D9Device)->GetSingleTexture()) UnswitchTextures(OriginalTextures[i]);
        else continue; //bug, this might happen if a hash is twice in the update list or due to an other bug^^
      }
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
          fake_Texture->Reference = u;
        }
      }

      break;
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
  if (( gl_ErrorState & (OTM_ERROR_FATAL | OTM_ERROR_MUTEX) )) return (RETURN_NO_MUTEX);
  if (WAIT_OBJECT_0!=WaitForSingleObject( Mutex, 100)) return (RETURN_MUTEX_LOCK); //waiting 100ms, to wait infinite pass INFINITE
  return  (RETURN_OK);
}

int OTM_TextureClient::UnlockMutex(void)
{
  if (ReleaseMutex(Mutex)==0) return (RETURN_MUTEX_UNLOCK);
  return  (RETURN_OK);
}





int OTM_TextureClient::LookUpToMod( OTM_IDirect3DTexture9* pTexture) // should only be called for original textures
{
  Message("OTM_TextureClient::LookUpToMod( %lu): hash: %#lX,  %lu\n", pTexture, pTexture->Hash, this);
  if (pTexture->CrossRef_D3Dtex!=NULL) return (RETURN_OK); // bug, this texture is already switched

  MyTypeHash hash = pTexture->Hash;
  for (int i=0; i<NumberToMod; i++)
  {
    if (hash == FileToMod[i].Hash && FileToMod[i].pTexture==NULL)
    { // if (FileToMod[i].pTexture!=NULL)    the corresponding fake texture is already switched, thus the game has loaded textures with the same hash
      OTM_IDirect3DTexture9 *fake_Texture;
      if (int ret = LoadTexture( & (FileToMod[i]), &fake_Texture)) return (ret);
      if (SwitchTextures( fake_Texture, pTexture))
      {
        Message("OTM_TextureClient::LookUpToMod(): textures not switched %#lX\n", hash);
        fake_Texture->Release();
      }
      else
      {
        FileToMod[i].pTexture = fake_Texture;
        fake_Texture->Reference = i;
      }
    }
  }
  return (RETURN_OK);
}


int OTM_TextureClient::LoadTexture( TextureFileStruct* file_in_memory, OTM_IDirect3DTexture9 **ppTexture) // to load fake texture from a file in memory
{
  Message("LoadTexture( %lu, %lu): %lu\n", file_in_memory, ppTexture, this);
  if (D3D_OK != D3DXCreateTextureFromFileInMemory( D3D9Device, file_in_memory->pData, file_in_memory->Size, (IDirect3DTexture9 **) ppTexture))
  {
    *ppTexture=NULL;
    return (RETURN_TEXTURE_NOT_LOADED);
  }
  (*ppTexture)->FAKE = true;
  ((OTM_IDirect3DDevice9*)D3D9Device)->SetLastCreatedTexture(NULL); //this texture is a fake texture and must not be added
  Message("LoadTexture(): DONE\n", file_in_memory, ppTexture, this);
  return (RETURN_OK);
}


/*
MyTypeHash OTM_TextureClient::GetHash(unsigned char *str, int len) // estimate the hash
{
  MyTypeHash hash = 0;
  for (int i=0; i<len; i++) hash = str[i] + (hash << 6) + (hash << 16) - hash;
  return (hash);
}
*/




/*
 *
 * BIG THANKS TO RS !!
 *
 * who gave me his hashing algorithm (well or crc32 algorithm^^)
 *
The hash function is CRC32 using polynomial 0xEDB88320.
However, the hashed data is calculated incorrectly in TexMod: it's simply BytesPerPixel * Width * Height, from the beginning of the data (that is mapped using LockRect).
The problem is that it doesn't take the pitch into account and BytesPerPixel may be wrong for some rare formats (not sure about that).
*/


#define CRC32POLY 0xEDB88320u /* CRC-32 Polynom */
#define ulCrc_in 0xffffffff

unsigned int OTM_TextureClient::GetCRC32( char *pcDatabuf, unsigned int ulDatalen)
{
  unsigned int crc = ulCrc_in;
  for (unsigned int idx = 0u; idx<ulDatalen; idx++)
  {
    unsigned int data = *pcDatabuf++;
    for (unsigned int bit = 0u; bit<8u; bit++, data >>=1)
    {
      crc = (crc >> 1) ^ (((crc ^ data) & 1) ? CRC32POLY : 0);
    }
  }
  return (crc);
}

