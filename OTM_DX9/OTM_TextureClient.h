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

/*
 *  An object of this class is owned by each d3d9 device.
 *  functions called by the Server are called from the server thread instance.
 *  All other functions are called from the render thread instance of the game itself.
 */

class OTM_TextureClient
{
public:
  OTM_TextureClient(OTM_TextureServer* server, IDirect3DDevice9* device);
  ~OTM_TextureClient(void);

  int AddTexture( OTM_IDirect3DTexture9* tex); //called from OTM_IDirect3DDevice9::CreateTexture(...) or OTM_IDirect3DDevice9::BeginScene()
  int AddTexture( OTM_IDirect3DVolumeTexture9* tex); //called from OTM_IDirect3DVolumeTexture9::CreateTexture(...) or OTM_IDirect3DDevice9::BeginScene()
  int AddTexture( OTM_IDirect3DCubeTexture9* tex); //called from OTM_IDirect3DCubeTexture9::CreateTexture(...) or OTM_IDirect3DDevice9::BeginScene()

  int RemoveTexture( OTM_IDirect3DTexture9* tex); //called from  OTM_IDirect3DTexture9::Release()
  int RemoveTexture( OTM_IDirect3DVolumeTexture9* tex); //called from  OTM_IDirect3DVolumeTexture9::Release()
  int RemoveTexture( OTM_IDirect3DCubeTexture9* tex); //called from  OTM_IDirect3DCubeTexture9::Release()

  int SaveAllTextures(bool val); //called from the Server
  int SaveSingleTexture(bool val); //called from the Server

  int SetSaveDirectory( wchar_t *dir); //called from the Server
  int SetGameName( wchar_t *dir); //called from the Server

  int SaveTexture(OTM_IDirect3DTexture9* pTexture); //called from OTM_IDirect3DDevice9::BeginScene() (save button) or from AddTexture(...) (SaveAllTextures)
  int SaveTexture(OTM_IDirect3DVolumeTexture9* pTexture); //called from OTM_IDirect3DDevice9::BeginScene() (save button) or from AddTexture(...) (SaveAllTextures)
  int SaveTexture(OTM_IDirect3DCubeTexture9* pTexture); //called from OTM_IDirect3DDevice9::BeginScene() (save button) or from AddTexture(...) (SaveAllTextures)



  int SetKeyBack( int key) {if (key>0) KeyBack = key; return (RETURN_OK);} //called from the Server
  int SetKeySave( int key) {if (key>0) KeySave = key; return (RETURN_OK);} //called from the Server
  int SetKeyNext( int key) {if (key>0) KeyNext = key; return (RETURN_OK);} //called from the Server

  int SetFontColour( DWORD r, DWORD g, DWORD b) {FontColour = D3DCOLOR_ARGB(255, r,g,b); return (RETURN_OK);} //called from the Server
  int SetTextureColour( DWORD r, DWORD g, DWORD b) {TextureColour = D3DCOLOR_ARGB(255, r,g,b); return (RETURN_OK);} //called from the Server


  int AddUpdate(TextureFileStruct* update, int number);  //called from the Server, client object must delete update array
  int MergeUpdate(void); //called from OTM_IDirect3DDevice9::BeginScene()

  int LookUpToMod( OTM_IDirect3DTexture9* pTexture); // called at the end AddTexture(...) and from Device->UpdateTexture(...)
  int LookUpToMod( OTM_IDirect3DVolumeTexture9* pTexture); // called at the end AddTexture(...) and from Device->UpdateTexture(...)
  int LookUpToMod( OTM_IDirect3DCubeTexture9* pTexture); // called at the end AddTexture(...) and from Device->UpdateTexture(...)

  OTM_TextureHandler<OTM_IDirect3DTexture9> OriginalTextures; // stores the pointer to the OTM_IDirect3DTexture9 objects created by the game
  OTM_TextureHandler<OTM_IDirect3DVolumeTexture9> OriginalVolumeTextures; // stores the pointer to the OTM_IDirect3DVolumeTexture9 objects created by the game
  OTM_TextureHandler<OTM_IDirect3DCubeTexture9> OriginalCubeTextures; // stores the pointer to the OTM_IDirect3DCubeTexture9 objects created by the game

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

  int NumberToMod; // number of texture to be modded
  TextureFileStruct* FileToMod; // array which stores the file in memory and the hash of each texture to be modded


  int LookUpToMod( MyTypeHash hash); // called from LookUpToMod(...);
  int LoadTexture( TextureFileStruct* file_in_memory, OTM_IDirect3DTexture9 **ppTexture); // called if a target texture is found
  int LoadTexture( TextureFileStruct* file_in_memory, OTM_IDirect3DVolumeTexture9 **ppTexture); // called if a target texture is found
  int LoadTexture( TextureFileStruct* file_in_memory, OTM_IDirect3DCubeTexture9 **ppTexture); // called if a target texture is found

  // and the corresponding fake texture should be loaded

  //MyTypeHash GetHash(unsigned char *str, int len);
  //unsigned int GetCRC32(char *pcDatabuf, unsigned int ulDatalen);
};



#endif /* OTM_TEXTUREHANDLER_HPP_ */
