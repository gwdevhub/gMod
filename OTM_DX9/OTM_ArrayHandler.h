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



#ifndef OTM_FIELDHANDLER_H_
#define OTM_FIELDHANDLER_H_

#include "..\OTM_GlobalDefines.h"
#include "OTM_IDirect3DTexture9.h"

typedef struct
{
  bool Checked; // to check if texture was already update; used by TextureClient::MergeUpdate()
  bool ForceReload; // to force a reload of the texture (if it is already modded)
  char* pData; // store texture file as file in memory
  unsigned int Size; // size of file
  int Reference; // for a fast delete in the FileHandler
  OTM_IDirect3DTexture9 *pTexture; // pointer to the fake texture
  MyTypeHash Hash; // hash value
} TextureFileStruct;



class OTM_FileHandler  // array to store TextureFileStruct
{
public:
  OTM_FileHandler(void);
  ~OTM_FileHandler(void);

  int Add( TextureFileStruct* file);
  int Remove( TextureFileStruct* file);

  int GetNumber(void) {return (Number);}

  TextureFileStruct *operator [] (int i) {if (i<0||i>=Number) return (NULL); else return (Files[i/FieldLength][i%FieldLength]);}

protected:
  static const int FieldLength = 1024;
  long Number;
  int FieldCounter;
  TextureFileStruct*** Files;
};

class OTM_TextureHandler  // array to store OTM_IDirect3DTexture9
{
public:
  OTM_TextureHandler(void);
  ~OTM_TextureHandler(void);

  int Add( OTM_IDirect3DTexture9* texture);
  int Remove( OTM_IDirect3DTexture9* texture);
  //int RemoveAll(void); //only for deleting all fake textures

  int GetNumber(void) {return (Number);}
  OTM_IDirect3DTexture9 *operator [] (int i) {if (i<0||i>=Number) return (NULL); else return (Textures[i/FieldLength][i%FieldLength]);}

private:
  static const int FieldLength = 1024;
  long Number;
  int FieldCounter;
  OTM_IDirect3DTexture9*** Textures;
};


#endif /* OTM_FIELDHANDLER_H_ */
