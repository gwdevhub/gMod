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
  bool ForceReload; // to force a reload of the texture (if it is already modded)
  char* pData; // store texture file as file in memory
  unsigned int Size; // size of file
  int NumberOfTextures;
  int Reference; // for a fast delete in the FileHandler
  IDirect3DBaseTexture9 **Textures; // pointer to the fake textures
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



template <class T>
class OTM_TextureHandler  // array to store OTM_IDirect3DTexture9, OTM_IDirect3DVolumeTexture9 or OTM_IDirect3DCubeTexture9
{
public:
  OTM_TextureHandler(void);
  ~OTM_TextureHandler(void);

  int Add( T* texture);
  int Remove( T* texture);

  int GetNumber(void) {return (Number);}
  T *operator [] (int i) {if (i<0||i>=Number) return (NULL); else return (Textures[i/FieldLength][i%FieldLength]);}

private:
  static const int FieldLength = 1024;
  long Number;
  int FieldCounter;
  T*** Textures;
};













template <class T>
OTM_TextureHandler<T>::OTM_TextureHandler(void)
{
  Message("OTM_TextureHandler(void): %lu\n", this);
  Number = 0;
  FieldCounter = 0;

  Textures = NULL;
}

template <class T>
OTM_TextureHandler<T>::~OTM_TextureHandler(void)
{
  Message("~OTM_TextureHandler(void): %lu\n", this);
  if (Textures!=NULL)
  {
    for (int i=0; i<FieldCounter; i++) if (Textures[i] != NULL) delete [] Textures[i];
    delete [] Textures;
  }
}

template <class T>
int OTM_TextureHandler<T>::Add(T* pTexture)
{
  Message("OTM_TextureHandler::Add( %lu): %lu\n", pTexture, this);
  if (gl_ErrorState & OTM_ERROR_FATAL) return (RETURN_FATAL_ERROR);

  if (pTexture->Reference>=0) return (RETURN_TEXTURE_ALLREADY_ADDED);

  if (Number/FieldLength==FieldCounter)
  {
    T*** temp = NULL;
    try {temp = new T**[FieldCounter+10];}
    catch (...)
    {
      gl_ErrorState |= OTM_ERROR_MEMORY | OTM_ERROR_TEXTURE;
      return (RETURN_NO_MEMORY);
    }

    for (int i=0; i<FieldCounter; i++) temp[i] = Textures[i];

    for (int i=FieldCounter; i<FieldCounter+10; i++) temp[i] = NULL;

    FieldCounter += 10;

    if (Textures!=NULL) delete [] Textures;

    Textures = temp;
  }
  if (Number%FieldLength==0)
  {
    try {if (Textures[Number/FieldLength]==NULL) Textures[Number/FieldLength] = new T*[FieldLength];}
    catch (...)
    {
      Textures[Number/FieldLength]=NULL;
      gl_ErrorState |= OTM_ERROR_MEMORY | OTM_ERROR_TEXTURE;
      return (RETURN_NO_MEMORY);
    }
  }

  Textures[Number/FieldLength][Number%FieldLength] = pTexture;
  pTexture->Reference = Number++;

  return (RETURN_OK);
}


template <class T>
int OTM_TextureHandler<T>::Remove(T* pTexture) //will be called, if a texture is completely released
{
  Message("OTM_TextureHandler::Remove( %lu): %lu\n", pTexture, this);
  if (gl_ErrorState & OTM_ERROR_FATAL) return (RETURN_FATAL_ERROR);

  int ref = pTexture->Reference;
  if (ref<0) return (RETURN_OK); // returning if no TextureHandlerRef is set

  if (ref<(--Number))
  {
    Textures[ref/FieldLength][ref%FieldLength] = Textures[Number/FieldLength][Number%FieldLength];
    Textures[ref/FieldLength][ref%FieldLength]->Reference = ref;
  }
  return (RETURN_OK);
}






#endif /* OTM_FIELDHANDLER_H_ */
