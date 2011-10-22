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

  NumberToMod = 0;
  FileToMod = NULL;

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
  MyTypeHash hash = GetCRC32( (char*) d3dlr.pBits, size); //calculate the crc32 of the texture

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
  Message("OTM_TextureClient::RemoveTexture( %lu, %#lX): %lu\n", pTexture, pTexture->Hash, this);

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

  for (int i=0; i<NumberOfUpdate; i++) Update[i].pTexture = NULL; // this is already done, but safety comes first ^^

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
    if (FileToMod[pos_old].Hash > Update[pos_new].Hash) // this fake texture is new
    {
      to_lookup[num_to_lookup++] = pos_new++; // keep this fake texture in mind, we must search later for it through all original textures
      // we increase only the new counter by one
    }
    else if (FileToMod[pos_old].Hash < Update[pos_new].Hash) // this fake texture is not in the update
    {
      if (FileToMod[pos_old].pTexture!=NULL) FileToMod[pos_old].pTexture->Release(); // we release the fake texture
      pos_old++; // we increase only the old counter by one
    }
    else // the hash value is the same, thus this texture is in the array FileToMod as well as in the array Update
    {
      if (FileToMod[pos_old].pTexture!=NULL)
      {
        if (Update[pos_new].ForceReload)
        {
          OTM_IDirect3DTexture9 *pTexture = FileToMod[pos_old].pTexture->CrossRef_D3Dtex;
          FileToMod[pos_old].pTexture->Release(); // release the old fake texture
          FileToMod[pos_old].pTexture = NULL;
          if (pTexture!=NULL) // should always be the case
          {
            OTM_IDirect3DTexture9 *fake_Texture;
            if (int ret = LoadTexture( & (Update[pos_new]), &fake_Texture)) return (ret);
            if (SwitchTextures( fake_Texture, pTexture))
            {
              Message("MergeUpdate(): textures not switched %#lX\n", pTexture->Hash);
              fake_Texture->Release();
            }
            else
            {
              Update[pos_new].pTexture = fake_Texture;
              fake_Texture->Reference = pos_new;
            }
          }
        }
        else
        {
          Update[pos_new].pTexture = FileToMod[pos_old].pTexture;
          Update[pos_new].pTexture->Reference = pos_new; // set the new reference, needed for a fast delete
        }
      }
      /*
      else
      {
        // This texture is not loaded, because the game did not load the target texture,
        // thus we need not to look later for this hash.
        // -> There is nothing to do.
      }
      */
      // we increase both counters by one
      pos_old++;
      pos_new++;
    }
  }

  while (pos_old<NumberToMod) //this fake textures are not in the Update
  {
    if (FileToMod[pos_old].pTexture!=NULL) FileToMod[pos_old].pTexture->Release();
    pos_old++;
  }
  while (pos_new<NumberOfUpdate) //this fake textures are newly added
  {
    to_lookup[num_to_lookup++] = pos_new++; //keep this fake texture in mind, we must search later for it through all original textures
  }


  /*
   * if (num_to_lookup>0) we need to look through all original textures
   * because there were newly added textures and we don't know
   * if the corresponding target textures are loaded by the game or not.
   *
   * Note: to_lookup[num_to_lookup++] = pos_new++; is in ascending order,
   * thus Update[to_lookup[pos]].Hash is also sorted ascending!
   */

  if (num_to_lookup>0)
  {
    int num = OriginalTextures.GetNumber();
    for (int i=0; i<num; i++)
    {
      MyTypeHash hash = OriginalTextures[i]->Hash;

      if (hash<Update[to_lookup[0]].Hash || hash>Update[to_lookup[num_to_lookup-1]].Hash) continue;

      int index = -1;
      int pos = num_to_lookup/2;
      int old_pos = -1;
      int begin = 0;
      int end = num_to_lookup-1;

      // we look always in the middle of the interval and each step we halve the interval
      while (old_pos!=pos) // if (old_pos==pos) the interval has a size of one or two -> we are finished
      {
        old_pos=pos;
        if (hash > Update[to_lookup[pos]].Hash) // the new interval is the right half of the actual interval
        {
          begin = pos;
          pos = (begin + end)/2;
        }
        else if (hash < Update[to_lookup[pos]].Hash) // the new interval is the left half of the actual interval
        {
          end = pos;
          pos = (begin + end)/2;
        }
        else {index = to_lookup[pos]; break;} // we hit the correct hash
      }
      if (index<0) // if we did not find the hash, it might be in the last interval, which has a size of one or two
      {
        if (Update[to_lookup[pos]].Hash==hash) index = to_lookup[pos];
        else if (++pos<num_to_lookup && Update[to_lookup[pos]].Hash==hash) index = to_lookup[pos];
      }

      if (index>=0) // target texture is loaded by the game
      {
        if (Update[index].pTexture==NULL) // if not this is a bug!!
        {
          OTM_IDirect3DTexture9 *fake_Texture;
          if (int ret = LoadTexture( & (Update[index]), &fake_Texture)) return (ret);
          if (SwitchTextures( fake_Texture, OriginalTextures[i]))
          {
            Message("OTM_TextureClient::LookUpToMod(): textures not switched %#lX\n", FileToMod[index].Hash);
            fake_Texture->Release();
          }
          else
          {
            Update[index].pTexture = fake_Texture;
            fake_Texture->Reference = index;
          }
        }
      }
    }
  }

  if (to_lookup != NULL) delete [] to_lookup;
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
  int index = -1;
  if(NumberToMod>0)
  {
    MyTypeHash hash = pTexture->Hash;
    if (hash<FileToMod[0].Hash || hash>FileToMod[NumberToMod-1].Hash) return (RETURN_OK);
    int pos = NumberToMod/2;
    int old_pos = -1;
    int begin = 0;
    int end = NumberToMod-1;

    // we look always in the middle of the interval and each step we halve the interval
    while (old_pos!=pos) // if (old_pos==pos) the interval has a size of one or two -> we are finished
    {
      old_pos=pos;
      if (hash > FileToMod[pos].Hash) // the new interval is the right half of the actual interval
      {
        begin = pos;
        pos = (begin + end)/2;
      }
      else if (hash < FileToMod[pos].Hash) // the new interval is the left half of the actual interval
      {
        end = pos;
        pos = (begin + end)/2;
      }
      else {index = pos; break;} // we hit the correct hash
    }
    if (index<0) // if we did not find the hash, it might be in the last interval, which has a size of one or two
    {
      if (FileToMod[pos].Hash==hash) index = pos;
      else if (++pos<NumberToMod && FileToMod[pos].Hash==hash) index = pos;
    }
  }

  if (index>=0)
  {
    // if (FileToMod[index].pTexture!=NULL)    the corresponding fake texture is already switched, thus the game has loaded textures with the same hash
    if (FileToMod[index].pTexture==NULL)
    {
      OTM_IDirect3DTexture9 *fake_Texture;
      if (int ret = LoadTexture( & (FileToMod[index]), &fake_Texture)) return (ret);
      if (SwitchTextures( fake_Texture, pTexture))
      {
        Message("OTM_TextureClient::LookUpToMod(): textures not switched %#lX\n", FileToMod[index].Hash);
        fake_Texture->Release();
      }
      else
      {
        FileToMod[index].pTexture = fake_Texture;
        fake_Texture->Reference = index;
      }
    }
  }

  /*
  MyTypeHash hash = pTexture->Hash;
  for (int i=0; i<NumberToMod; i++) if (hash == FileToMod[i].Hash)
  {
    // if (FileToMod[i].pTexture!=NULL)    the corresponding fake texture is already switched, thus the game has loaded textures with the same hash
    if (FileToMod[i].pTexture==NULL)
    {
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
    break;
  }
  */
  return (RETURN_OK);
}


int OTM_TextureClient::LoadTexture( TextureFileStruct* file_in_memory, OTM_IDirect3DTexture9 **ppTexture) // to load fake texture from a file in memory
{
  Message("LoadTexture( %lu, %lu, %#lX): %lu\n", file_in_memory, ppTexture, file_in_memory->Hash, this);
  if (D3D_OK != D3DXCreateTextureFromFileInMemory( D3D9Device, file_in_memory->pData, file_in_memory->Size, (IDirect3DTexture9 **) ppTexture))
  {
    *ppTexture=NULL;
    return (RETURN_TEXTURE_NOT_LOADED);
  }
  (*ppTexture)->FAKE = true;
  ((OTM_IDirect3DDevice9*)D3D9Device)->SetLastCreatedTexture(NULL); //this texture is a fake texture and must not be added
  Message("LoadTexture( %lu, %#lX): DONE\n", *ppTexture, file_in_memory->Hash);
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

