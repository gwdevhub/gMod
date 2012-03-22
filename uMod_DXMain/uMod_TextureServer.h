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
  /**
   * The server is created from the dll entry routine.
   * @param name Name of the game executable (without the extension)
   */
  uMod_TextureServer(wchar_t *name);
  ~uMod_TextureServer(void);

  /**
   * Each client connect itself to the server via this function.
   * @param client This-pointer of the client
   * @param update Current texture to be modded
   * @param number Number of modded textures
   * @param Version Version of DirectX.
   * @return RETURN_OK on success
   */
  int AddClient(uMod_TextureClient *client, TextureFileStruct* &update, int &number, const int version); // called from a Client

  /**
   * On destruction of client, it disconnect from the server.
   * @param client This-pointer of the client
   * @return RETURN_OK on success
   */
  int RemoveClient(uMod_TextureClient *client, const int version); // called from a Client

  /**
   * Opens the pipe to the GUI (called from the dll entry routine)
   * @param name Name of the game
   * @return RETURN_OK on success
   */
  int OpenPipe(wchar_t *name); // called on initialization of our d3d9 fake dll

  /**
   * Close the Pipe to the GUI (called if dll entry is called to attach from the game)
   * @return RETURN_OK on success
   */
  int ClosePipe(void); // called on exit of our d3d9 fake dll

  /**
   * The mainloop reads from the pipe (blocking reading). It is running in a separate server thread.
   * @return RETURN_OK on success
   */
  int MainLoop(void); // is executed in a server thread



private:
  /**
   * Add a file to the list of texture to be modded (called from the mainloop).
   * @param buffer hold the file content of the texture
   * @param size size of the file content
   * @param hash hash of the texture to be replaced
   * @param force set to TRUE to force a reload of the texture
   * @return RETURN_OK on success
   */
  int AddFile( char* buffer, unsigned int size,  MyTypeHash hash, bool force); // called from Mainloop(), if the content of the texture is sent

  /**
   * Add a file to the list of texture to be modded (called from the mainloop).
   * @param file_name Name and path to the file to be loaded.
   * @param hash hash of the texture to be replaced
   * @param force set to TRUE to force a reload of the texture
   * @return RETURN_OK on success
   */
  int AddFile( wchar_t* file_name, MyTypeHash hash, bool force); // called from Mainloop(), if the name and the path to the file is sent

  /**
   * Remove a texture (called from the mainloop).
   * @param hash Hash of the target texture.
   * @return RETURN_OK on success
   */
  int RemoveFile( MyTypeHash hash); // called from Mainloop()

  /**
   * Save all texture, which are loade by the game (called from the mainloop).
   * @param val Set TRUE to enable the mode.
   * @return
   */
  int SaveAllTextures(bool val); // called from Mainloop()

  /**
   *  (called from the mainloop).
   * @param val
   * @return
   */
  int SaveSingleTexture(bool val); // called from Mainloop()

  /**
   *  (called from the mainloop).
   * @param dir
   * @return
   */
  int SetSaveDirectory( wchar_t *dir); // called from Mainloop()

  /**
   *  (called from the mainloop).
   * @param key
   * @return
   */
  int SetKeyBack( int key); // called from Mainloop()

  /**
   *  (called from the mainloop).
   * @param key
   * @return
   */
  int SetKeySave( int key); // called from Mainloop()

  /**
   *  (called from the mainloop).
   * @param key
   * @return
   */
  int SetKeyNext( int key); // called from Mainloop()

  /**
   *  (called from the mainloop).
   * @param colour
   * @return
   */
  int SetFontColour(DWORD colour); // called from Mainloop()

  /**
   *  (called from the mainloop).
   * @param colour
   * @return
   */
  int SetTextureColour(DWORD colour); // called from Mainloop()

  bool BoolSaveAllTextures;
  bool BoolSaveSingleTexture;
  wchar_t SavePath[MAX_PATH];
  wchar_t GameName[MAX_PATH];

  /**
   * Send the files to be modded (Update) to a client (called from the mainloop).
   * @param client Pointer to a client (if NULL is passed, the data is send to all clients)
   * @return
   */
  int PropagateUpdate(uMod_TextureClient* client=NULL); // called from Mainloop() if texture are loaded or removed

  /**
   * Prepare the texture data for the clients (e.g. Load the texture from disk, sort the texture according the hash values) (called from the mainloop).
   * @param update
   * @param number
   * @return
   */
  int PrepareUpdate(TextureFileStruct* &update, int &number); // called from PropagateUpdate() and AddClient()
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
