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

TextureFileContent::TextureFileContent() : ForceReload(false), pFileName(NULL), pData(NULL), Size(0u), Reference(-1), Hash(0u), RefCounter(1)
{
}

TextureFileContent::~TextureFileContent()
{
  if (pFileName!=NULL) delete [] pFileName;
  if (pData!=NULL) delete [] pData;
}




TextureEntry::TextureEntry(void) : TexContent(NULL), NumberOfTextures(0), Textures(NULL)
{
}

TextureEntry::~TextureEntry()
{
  //for (int i=0; i<NumberOfTextures; i++) Textures[i]->Release();
  if (Textures!=NULL) delete [] Textures;
  if (TexContent!=NULL) TexContent->Release();
}


void TextureEntry::SetContent(TextureFileContent *content)
{
  if (TexContent!=NULL) TexContent->Release();
  TexContent = content;
  if (TexContent!=NULL) TexContent->AddRef();
}

int TextureEntry::AddTexture(IUnknown* tex)
{
  IUnknown** temp = new IUnknown*[NumberOfTextures+1];
  for (int i=0; i<NumberOfTextures; i++) temp[i] = Textures[i];

  if (Textures!=NULL) delete [] Textures;
  Textures = temp;
  return 0;
}

int TextureEntry::RemoveTexture(IUnknown* tex)
{
  for (int i=0; i<NumberOfTextures; i++) if (Textures[i] == tex)
  {
    NumberOfTextures--;
    for (int j=i; j<NumberOfTextures; j++)
      Textures[j] = Textures[j+1];
    break;
  }
  return 0;
}

