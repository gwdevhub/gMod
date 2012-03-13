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



#ifndef uMod_TEXTUREHANDLER_HPP
#define uMod_TEXTUREHANDLER_HPP

#include <d3d9.h>
#include <d3d10.h>
#include "../uMod_GlobalDefines.h"
#include "..\uMod_Error.h"


class uMod_TextureServer;

/*
 *  An object of this class is owned by each d3dXX device.
 *  functions called by the Server are called from the server thread instance.
 *  All other functions are called from the render thread instance of the game itself.
 */

class uMod_TextureClient
{
public:
  uMod_TextureClient(uMod_TextureServer* server);
  ~uMod_TextureClient(void);

  /**
   * Enable/Disable the "save all texture" mode. (called from the server)
   * @param val
   * @return RETURN_OK on success
   */
  int SaveAllTextures(bool val);

  /**
   * Enable/Disable the "save all texture" mode. (called from the server)
   * @param val
   * @return RETURN_OK on success
   */
  int SaveSingleTexture(bool val);

  /**
   * Set the directory, wher the texture should be stored (called from the server)
   * @param dir
   * @return RETURN_OK on success
   */
  int SetSaveDirectory( wchar_t *dir);

  /**
   * Set the name of the game (default name of executabel without the extension .exe) (called from the server)
   * @param name
   * @return RETURN_OK on success
   */
  int SetGameName( wchar_t *name);

  /**
   * Set the key to got to the previous texture (called from the server)
   * @param key
   * @return RETURN_OK
   */
  int SetKeyBack( int key) {if (key>0) KeyBack = key; return (RETURN_OK);}
  /**
   * Set the key for saving the texture (called from the server)
   * @param key
   * @return RETURN_OK
   */
  int SetKeySave( int key) {if (key>0) KeySave = key; return (RETURN_OK);}
  /**
   * Set the key to go to the next texture (called from the server)
   * @param key
   * @return RETURN_OK
   */
  int SetKeyNext( int key) {if (key>0) KeyNext = key; return (RETURN_OK);}

  /**
   * Set the font color "during save single" texture mode.  (called from the server)
   * @param r red value (0..255)
   * @param g green value (0..255)
   * @param b blue value (0..255)
   * @return
   */
  int SetFontColour( DWORD r, DWORD g, DWORD b) {FontColour = D3DCOLOR_ARGB(255, r,g,b); return (RETURN_OK);}
  /**
   * Set the texture color "during save single" texture mode. (called from the server)
   * @param r red value (0..255)
   * @param g green value (0..255)
   * @param b blue value (0..255)
   * @return RETURN_OK
   */
  int SetTextureColour( DWORD r, DWORD g, DWORD b) {TextureColour = D3DCOLOR_ARGB(255, r,g,b); return (RETURN_OK);}


  /**
   * The server add an update to the client.(called from server)
   * @param update Pointer to an array of TextureFileStruct, the client \b must delete this array!
   * @param number number of entries
   * @return RETURN_OK on success
   */
  int AddUpdate(TextureFileStruct* update, int number);
  /**
   * Merge the latest update (called from client -> e.g. uMod_IDirect3DDevice9::BeginScene())
   * @return RETURN_OK on success
   */
  int MergeUpdate(void);


  bool BoolSaveAllTextures; //< true if all textures should be saved
  bool BoolSaveSingleTexture; //< true if "save single texture" mode is enabled
  int KeyBack; //< key value for going to the previous texture
  int KeySave; //< key value for saving the current texture
  int KeyNext; //< key value for going to the next texture

  D3DCOLOR FontColour; //< color of the message ("save single texture" mode)
  D3DCOLOR TextureColour; //< color of the texture ("save single texture" mode)

private:
  uMod_TextureServer* Server; //< Pointer to the server
  wchar_t SavePath[MAX_PATH]; //< saving directory
  wchar_t GameName[MAX_PATH]; //< game name

  TextureFileStruct* Update; //< array which stores the file in memory and the hash of each texture to be modded
  int NumberOfUpdate; //< number of texture to be modded

  /**
   * Lock the mutex. The mutex protect the AddUpdate and MergeUpdate function to be called simultaneously.
   * @return RETURN_OK on success
   */
  int LockMutex();
  /**
   * Unloock the mutex. The mutex protect the AddUpdate and MergeUpdate function to be called simultaneously.
   * @return RETURN_OK on success
   */
  int UnlockMutex();
  HANDLE Mutex; //< The mutex protect the AddUpdate and MergeUpdate function to be called simultaneously.

  int NumberToMod; //< number of texture to be modded
  TextureFileStruct* FileToMod; //< array which stores the file in memory and the hash of each texture to be modded


  int LookUpToMod( MyTypeHash hash, int num_index_list, int *index_list); // called from LookUpToMod(...);


};



#endif /* uMod_TEXTUREHANDLER_HPP_ */
