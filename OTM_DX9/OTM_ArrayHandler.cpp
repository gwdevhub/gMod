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





OTM_FileHandler::OTM_FileHandler(void)
{
  Message("OTM_FileHandler(void): %lu\n", this);
  Number = 0;
  FieldCounter = 0;
  Files = NULL;
}

OTM_FileHandler::~OTM_FileHandler(void)
{
  Message("~OTM_FileHandler(void): %lu\n", this);
  if (Files!=NULL)
  {
    for (int i=0; i<FieldCounter; i++) if (Files[i] != NULL) delete [] Files[i];
    delete [] Files;
  }
}

int OTM_FileHandler::Add(TextureFileStruct* file)
{
  Message("OTM_FileHandler::Add(%lu): %lu\n", file, this);
  if (gl_ErrorState & OTM_ERROR_FATAL) return (RETURN_FATAL_ERROR);

  if (file->Reference>=0) return (RETURN_UPDATE_ALLREADY_ADDED);

  if (Number/FieldLength==FieldCounter)
  {
    TextureFileStruct*** temp = NULL;
    try {temp = new TextureFileStruct**[FieldCounter+10];}
    catch (...)
    {
      gl_ErrorState |= OTM_ERROR_MEMORY | OTM_ERROR_TEXTURE;
      return (RETURN_NO_MEMORY);
    }

    for (int i=0; i<FieldCounter; i++) temp[i] = Files[i];

    for (int i=FieldCounter; i<FieldCounter+10; i++) temp[i] = NULL;

    FieldCounter += 10;

    if (Files!=NULL) delete [] Files;

    Files = temp;
  }
  if (Number%FieldLength==0)
  {
    try {if (Files[Number/FieldLength]==NULL) Files[Number/FieldLength] = new TextureFileStruct*[FieldLength];}
    catch (...)
    {
      Files[Number/FieldLength] = NULL;
      gl_ErrorState |= OTM_ERROR_MEMORY | OTM_ERROR_TEXTURE;
      return (RETURN_NO_MEMORY);
    }
  }

  Files[Number/FieldLength][Number%FieldLength] = file;
  file->Reference = Number++;

  return (RETURN_OK);
}


int OTM_FileHandler::Remove(TextureFileStruct* file)
{
  Message("OTM_FileHandler::Remove(%lu): %lu\n", file, this);
  if (gl_ErrorState & OTM_ERROR_FATAL) return (RETURN_FATAL_ERROR);
  int ref = file->Reference;
  if (ref<0) return (RETURN_OK); // returning if no Reference is set
  file->Reference = -1;
  if (ref<(--Number))
  {
    Files[ref/FieldLength][ref%FieldLength] = Files[Number/FieldLength][Number%FieldLength];
    Files[ref/FieldLength][ref%FieldLength]->Reference = ref;
  }
  return (RETURN_OK);
}

































OTM_TextureHandler::OTM_TextureHandler(void)
{
  Message("OTM_TextureHandler(void): %lu\n", this);
  Number = 0;
  FieldCounter = 0;

  Textures = NULL;
}

OTM_TextureHandler::~OTM_TextureHandler(void)
{
  Message("~OTM_TextureHandler(void): %lu\n", this);
  if (Textures!=NULL)
  {
    for (int i=0; i<FieldCounter; i++) if (Textures[i] != NULL) delete [] Textures[i];
    delete [] Textures;
  }
}

int OTM_TextureHandler::Add(OTM_IDirect3DTexture9* pTexture)
{
  Message("OTM_TextureHandler::Add( %lu): %lu\n", pTexture, this);
  if (gl_ErrorState & OTM_ERROR_FATAL) return (RETURN_FATAL_ERROR);

  if (pTexture->Reference>=0) return (RETURN_TEXTURE_ALLREADY_ADDED);

  if (Number/FieldLength==FieldCounter)
  {
    OTM_IDirect3DTexture9*** temp = NULL;
    try {temp = new OTM_IDirect3DTexture9**[FieldCounter+10];}
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
    try {if (Textures[Number/FieldLength]==NULL) Textures[Number/FieldLength] = new OTM_IDirect3DTexture9*[FieldLength];}
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


int OTM_TextureHandler::Remove(OTM_IDirect3DTexture9* pTexture) //will be called, if a texture is completely released
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

/*
int OTM_TextureHandler::RemoveAll(void) //only for deleting all fake textures
{
  Message("OTM_TextureHandler::RemoveAll() %d: %lu\n", Number, this);
  for (int i=0; i<Number; i++) Textures[i/FieldLength][i%FieldLength]->Release();
  Number=0;
  return (RETURN_OK);
}
*/
