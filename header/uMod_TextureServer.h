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


#ifndef uMod_TEXTURESERVER_H_
#define uMod_TEXTURESERVER_H_

#include "../uMod_GlobalDefines.h"
#include "uMod_ArrayHandler.h"


/*
 *  An object of this class is created only once.
 *  The Mainloop functions is executed by a server thread,
 *  which listen on a pipe.
 *
 *  Functions called by the Client are called from the a thread instance of the game itself.
 *  Nearly all other functions are called from the server thread instance.
 */


class uMod_TextureClient;

class uMod_TextureServer
{
public:
  uMod_TextureServer(wchar_t *name);
  ~uMod_TextureServer(void);

  int AddClient(uMod_TextureClient *client, TextureFileStruct** update, int* number); // called from a Client
  int RemoveClient(uMod_TextureClient *client); // called from a Client

  int OpenPipe(wchar_t *name); // called on initialization of our d3d9 fake dll
  int ClosePipe(void); // called on exit of our d3d9 fake dll
  int MainLoop(void); // is executed in a server thread


  // following functions are only public for testing purpose !!
  // they should be private and only be called from the Mainloop

  int AddFile( char* buffer, unsigned int size,  MyTypeHash hash, bool force); // called from Mainloop(), if the content of the texture is sent
  int AddFile( wchar_t* file_name, MyTypeHash hash, bool force); // called from Mainloop(), if the name and the path to the file is sent
  int RemoveFile( MyTypeHash hash); // called from Mainloop()

  int SaveAllTextures(bool val); // called from Mainloop()
  int SaveSingleTexture(bool val); // called from Mainloop()

  int SetSaveDirectory( wchar_t *dir); // called from Mainloop()


  int SetKeyBack( int key); // called from Mainloop()
  int SetKeySave( int key); // called from Mainloop()
  int SetKeyNext( int key); // called from Mainloop()

  int SetFontColour(DWORD colour); // called from Mainloop()
  int SetTextureColour(DWORD colour); // called from Mainloop()

private:
  bool BoolSaveAllTextures;
  bool BoolSaveSingleTexture;
  wchar_t SavePath[MAX_PATH];
  wchar_t GameName[MAX_PATH];

  int PropagateUpdate(uMod_TextureClient* client=NULL); // called from Mainloop() if texture are loaded or removed
  int PrepareUpdate(TextureFileStruct** update, int* number); // called from PropagateUpdate() and AddClient()
  // generate a copy of the current texture to be modded
  // the file content of the textures are not copied, the clients get the pointer to the file content
  // but the arrays allocate by this function, must be deleted by the client

  int LockMutex();
  int UnlockMutex();
  HANDLE Mutex;


  int KeyBack;
  int KeySave;
  int KeyNext;

  DWORD FontColour;
  DWORD TextureColour;


  PipeStruct Pipe;

  uMod_TextureClient** Clients;
  int NumberOfClients;
  int LenghtOfClients;

  uMod_FileHandler CurrentMod;  // hold the file content of texture
  uMod_FileHandler OldMod; // hold the file content of texture which were added previously but are not needed any more
  // this is needed, because a texture clients might not have merged the last update and thus hold pointers to the file content of old textures
};


#endif /* uMod_TEXTURESERVER_H_ */
