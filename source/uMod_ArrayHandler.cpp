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




#include "uMod_Main.h"





uMod_FileHandler::uMod_FileHandler(void)
{
  Message("uMod_FileHandler(void): %lu\n", this);
  Number = 0;
  FieldCounter = 0;
  Files = NULL;
}

uMod_FileHandler::~uMod_FileHandler(void)
{
  Message("~uMod_FileHandler(void): %lu\n", this);
  if (Files!=NULL)
  {
    for (int i=0; i<FieldCounter; i++) if (Files[i] != NULL) delete [] Files[i];
    delete [] Files;
  }
}

int uMod_FileHandler::Add(TextureFileStruct* file)
{
  Message("uMod_FileHandler::Add(%lu): %lu\n", file, this);
  if (gl_ErrorState & uMod_ERROR_FATAL) return (RETURN_FATAL_ERROR);

  if (file->Reference>=0) return (RETURN_UPDATE_ALLREADY_ADDED);

  if (Number/FieldLength==FieldCounter) // get more memory
  {
    TextureFileStruct*** temp = NULL;
    try {temp = new TextureFileStruct**[FieldCounter+10];}
    catch (...)
    {
      gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_TEXTURE;
      return (RETURN_NO_MEMORY);
    }

    for (int i=0; i<FieldCounter; i++) temp[i] = Files[i]; //copy to new allocated memory

    for (int i=FieldCounter; i<FieldCounter+10; i++) temp[i] = NULL; // initialize unused parts to zero

    FieldCounter += 10;

    if (Files!=NULL) delete [] Files;

    Files = temp;
  }
  if (Number%FieldLength==0) // maybe we need to get more memory
  {
    try {if (Files[Number/FieldLength]==NULL) Files[Number/FieldLength] = new TextureFileStruct*[FieldLength];}
    catch (...)
    {
      Files[Number/FieldLength] = NULL;
      gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_TEXTURE;
      return (RETURN_NO_MEMORY);
    }
  }

  Files[Number/FieldLength][Number%FieldLength] = file;
  file->Reference = Number++; //set the reference for a fast deleting

  return (RETURN_OK);
}


int uMod_FileHandler::Remove(TextureFileStruct* file)
{
  Message("uMod_FileHandler::Remove(%lu): %lu\n", file, this);
  if (gl_ErrorState & uMod_ERROR_FATAL) return (RETURN_FATAL_ERROR);
  int ref = file->Reference;

  if (ref<0) return (RETURN_OK); // returning if no Reference is set
  file->Reference = -1; //set reference outside of bound
  if (ref<(--Number)) //if reference is unequal to Number-1 we copy the last entry to the index "ref"
  {
    Files[ref/FieldLength][ref%FieldLength] = Files[Number/FieldLength][Number%FieldLength];
    Files[ref/FieldLength][ref%FieldLength]->Reference = ref; //set the new reference entry
  }
  return (RETURN_OK);
}

