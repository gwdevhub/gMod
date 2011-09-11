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



#include "OTM_Main.h"



OTM_GameInfo::OTM_GameInfo(const wxString &name)
{
  Name = name;
  SaveSingleTexture = false;
  SaveAllTextures = false;

  KeyBack = 0;
  KeySave = 0;
  KeyNext = 0;
}


OTM_GameInfo::~OTM_GameInfo(void)
{
}

int OTM_GameInfo::SetSaveSingleTexture(bool val)
{
  SaveSingleTexture = val;
  return 0;
}

int OTM_GameInfo::SetSaveAllTextures(bool val)
{
  SaveAllTextures = val;
  return 0;
}

void OTM_GameInfo::SetTextures(const wxArrayString &textures)
{
  Textures = textures;
}

void OTM_GameInfo::GetTextures( wxArrayString &textures)
{
  textures = Textures;
}

void OTM_GameInfo::AddTexture( const wxString &textures)
{
  Textures.Add(textures);
}



