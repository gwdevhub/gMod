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



#ifndef OTM_TEXTUREHANDLER_HPP
#define OTM_TEXTUREHANDLER_HPP

#include "OTM_IDirect3DTexture9.h"
#include "OTM_IDirect3DDevice9.h"
#include "..\OTM_Error.h"


class OTM_TextureServer;



class OTM_TextureClient
{
public:
  OTM_TextureClient(OTM_TextureServer* server, IDirect3DDevice9* device);
  ~OTM_TextureClient(void);

  int AddTexture( OTM_IDirect3DTexture9* tex); //called from OTM_IDirect3DDevice9::CreateTexture(...)
  int RemoveTexture( OTM_IDirect3DTexture9* tex); //called from  OTM_IDirect3DTexture9::Release()

  //int ReleaseAllFakeTexture(void); // called when the Game is closed

  int SaveAllTextures(bool val); //called from the Server
  int SaveSingleTexture(bool val); //called from the Server

  int SetSaveDirectory( wchar_t *dir); //called from the Server
  int SetGameName( wchar_t *dir); //called from the Server

  int SaveTexture(OTM_IDirect3DTexture9* pTexture);


  int SetKeyBack( int key) {if (key>0) KeyBack = key; return (RETURN_OK);}
  int SetKeySave( int key) {if (key>0) KeySave = key; return (RETURN_OK);}
  int SetKeyNext( int key) {if (key>0) KeyNext = key; return (RETURN_OK);}

  int SetFontColour( DWORD r, DWORD g, DWORD b) {FontColour = D3DCOLOR_ARGB(255, r,g,b); return (RETURN_OK);}
  int SetTextureColour( DWORD r, DWORD g, DWORD b) {TextureColour = D3DCOLOR_ARGB(255, r,g,b); return (RETURN_OK);}


  int AddUpdate(TextureFileStruct* update, int number);  //client must delete the temp array
  int MergeUpdate(void);


  //OTM_TextureHandler FakeTextures;
  OTM_TextureHandler OriginalTextures;
  bool BoolSaveAllTextures;
  bool BoolSaveSingleTexture;
  int KeyBack;
  int KeySave;
  int KeyNext;

  D3DCOLOR FontColour;
  D3DCOLOR TextureColour;

private:
  OTM_TextureServer* Server;
  IDirect3DDevice9* D3D9Device;
  wchar_t SavePath[MAX_PATH];
  wchar_t GameName[MAX_PATH];

  TextureFileStruct* Update;
  int NumberOfUpdate;

  int LockMutex();
  int UnlockMutex();
  HANDLE Mutex;

  int NumberToMod;
  TextureFileStruct* FileToMod;


  int LookUpToMod( OTM_IDirect3DTexture9* pTexture);
  int LoadTexture( TextureFileStruct* file_in_memory, OTM_IDirect3DTexture9 **ppTexture);
  MyTypeHash GetHash(unsigned char *str, int len);
  unsigned int GetCRC32(char *pcDatabuf, unsigned int ulDatalen);

};



#endif /* OTM_TEXTUREHANDLER_HPP_ */
