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



#ifndef OTM_TEXTURESERVER_H_
#define OTM_TEXTURESERVER_H_

#include "../OTM_GlobalDefines.h"
#include "OTM_ArrayHandler.h"




class OTM_TextureClient;

class OTM_TextureServer
{
public:
  OTM_TextureServer(void);
  ~OTM_TextureServer(void);

  int AddClient(OTM_TextureClient *client, TextureFileStruct** update, int* number);
  int RemoveClient(OTM_TextureClient *client);

  int OpenPipe( wchar_t *game);
  int ClosePipe(void);
  int MainLoop(void);


  //following functions are public for testing purpose !!
  int AddFile( wchar_t* file_name, MyTypeHash hash, bool force);
  int RemoveFile( MyTypeHash hash);

  int SaveAllTextures(bool val);
  int SaveSingleTexture(bool val);

  int SetSaveDirectory( wchar_t *dir);
  int SaveTexture(OTM_IDirect3DTexture9* pTexture);


  int SetKeyBack( int key);
  int SetKeySave( int key);
  int SetKeyNext( int key);

private:
  bool BoolSaveAllTextures;
  bool BoolSaveSingleTexture;
  wchar_t SavePath[MAX_PATH];

  int PropagateUpdate(OTM_TextureClient* client=NULL);
  int PrepareUpdate(TextureFileStruct** update, int* number);

  int LockMutex();
  int UnlockMutex();


  int KeyBack;
  int KeySave;
  int KeyNext;

  HANDLE Mutex;

  PipeStruct Pipe;

  OTM_TextureClient** Clients;
  int NumberOfClients;
  int LenghtOfClients;

  OTM_FileHandler CurrentMod;
  OTM_FileHandler OldMod;
};


#endif /* OTM_TEXTURESERVER_H_ */
