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



#ifndef uMod_TEXTURECLIENT_HPP
#define uMod_TEXTURECLIENT_HPP

#include <d3d9.h>
#include <d3d10.h>
#include "..\uMod_GlobalDefines.h"
#include "..\uMod_Error.h"


class uMod_TextureServer;

/**
 *  An object of this class is owned by each d3dXX device.
 *  functions called by the Server are called from the server thread instance.
 *  All other functions are called from the render thread instance of the game itself.
 */

class uMod_TextureClient
{
public:
  uMod_TextureClient(const int version);
  virtual ~uMod_TextureClient(void);

  /**
   * Connect to the server. (called from the instance, which creates the client, e.g. uMod_IDirect3DDevice9::uMod_IDirect3DDevice9)
   * @return RETURN_OK on success
   */
  int ConnectToServer(uMod_TextureServer* server);

  /**
   * Enable/Disable the "save all texture" mode. (called from the server)
   * @param val
   * @return RETURN_OK on success
   */
  virtual int SaveAllTextures(bool val) = 0;

  /**
   * Enable/Disable the "save all texture" mode. (called from the server)
   * @param val
   * @return RETURN_OK on success
   */
  virtual int SaveSingleTexture(bool val) = 0;

  /**
   * Enable/Disable the string in the left upper corner during save single texture mode (called from the mainloop).
   * @param[in] val
   * @return
   */
  int ShowTextureString(bool val) {BoolShowTextureString = val; return (RETURN_OK);}

  /**
   * Enable/Disable the string in the left upper corner during save single texture mode (called from the mainloop).
   * @param[in] val
   * @return
   */
  int ShowSingleTexture(bool val) {BoolShowSingleTexture = val; return (RETURN_OK);}

  /**
   * Enable/Disable the hashing with CRC32, which is needed to support also tpf mods (called from the mainloop).
   * @param[in] val
   * @return
   */
  virtual int SupportTPF(bool val) = 0;

  /**
   * Enable/Disable the computation of the hash of render targets.
   * @param[in] val
   * @return
   */
  int ComputeRenderTargets(bool val) {BoolComputeRenderTargets = val; return (RETURN_OK);}

  /**ComputeRenderTargets
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
   * @param colour
   * @return
   */
  int SetFontColour( DWORD64 colour) {FontColour = (D3DCOLOR) colour; return (RETURN_OK);}

  /**
   * Set the texture color "during save single" texture mode. (called from the server)
   * @param colour
   * @return RETURN_OK
   */
  int SetTextureColour( DWORD64 colour) {TextureColour = (D3DCOLOR) colour; return (RETURN_OK);}

  /**
    * @param[in] format
    * @return
    */
   int SetFileFormat(DWORD64 format) {FileFormat = format;  return (RETURN_OK);}

   /**
    * @param[in] format
    * @return
    */
   int SetFormatFilter(DWORD64 format) {FormatFilter = format;  return (RETURN_OK);}

   /**
    * @param[in] size
    * @return
    */
   int SetWidthFilter(DWORD64 size) {WidthFilter = size;  return (RETURN_OK);}

   /**
    * @param[in] size
    * @return
    */
   int SetHeightFilter(DWORD64 size) {HeightFilter = size;  return (RETURN_OK);}

   /**
    * @param[in] size
    * @return
    */
   int SetDepthFilter(DWORD64 size) {DepthFilter = size;  return (RETURN_OK);}

  /**
   * The server add an update to the client.(called from server)
   * @param update Pointer to an array of TextureFileStruct, the client \b must delete this array!
   * @param number number of entries
   * @return RETURN_OK on success
   */
  int AddUpdate(TextureEntry* update, int number);


  /**
   * Merge the latest update (called from client -> e.g. uMod_IDirect3DDevice9::BeginScene())
   * @return RETURN_OK on success
   */
  virtual int MergeUpdate(void) = 0;


  bool BoolSaveAllTextures; //!< true if all textures should be saved
  bool BoolSaveSingleTexture; //!< true if "save single texture" mode is enabled
  bool BoolShowTextureString; //!< true if a string should be displayed during "save single texture" mode is enabled
  bool BoolShowSingleTexture; //!< true if the texture should be displayed during "save single texture" mode is enabled
  bool BoolComputeRenderTargets; //!< true if the hash of render targets should be computed (very slow because each frame the hash must be recomputed)


  int KeyBack; //!< key value for going to the previous texture
  int KeySave; //!< key value for saving the current texture
  int KeyNext; //!< key value for going to the next texture

  D3DCOLOR FontColour; //!< color of the message ("save single texture" mode)
  D3DCOLOR TextureColour; //!< color of the texture ("save single texture" mode)


  DWORD64 FileFormat; //!< file format to which the texture should be saved
  DWORD64 FormatFilter; //!< texture format which should be saved (all==0, else bitwise 1=save,0=don't save)
  DWORD64 WidthFilter; //!< filter min<<32 max, texture size must be min<=size<=max
  DWORD64 HeightFilter; //!< filter min<<32 max, texture size must be min<=size<=max
  DWORD64 DepthFilter; //!< filter min<<32 max, texture size must be min<=size<=max

  const int Version;
  uMod_TextureServer* Server; //!< Pointer to the server
  wchar_t SavePath[MAX_PATH]; //!< saving directory
  wchar_t GameName[MAX_PATH]; //!< game name

  TextureEntry* Update; //!< array which stores the file in memory and the hash of each texture to be modded
  int NumberOfUpdate; //!< number of texture to be modded

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
  HANDLE Mutex; //!< The mutex protect the AddUpdate and MergeUpdate function to be called simultaneously.

  int NumberToMod; //!< number of texture to be modded
  TextureEntry* FileToMod; //!< array which stores the file in memory and the hash of each texture to be modded

  /**
   * Find the given hash value in the \a FileToMod list. A vector with index can be passed. If so, only these index are considered in the search
   * @param hash hash value
   * @param num_index_list number of index vector
   * @param index_list pointer to the index vector
   * @return index or a negative value if the hash value was not found
   */
  int GetIndex( DWORD64 hash, int num_index_list=0, int *index_list=(int*)0); // called from LookUpToMod(...);


};



#endif /* uMod_TEXTUREHANDLER_HPP_ */
