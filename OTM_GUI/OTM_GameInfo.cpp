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

  Checked = NULL;
  NumberOfChecked = 0;
  LengthOfChecked = 0;
  Init();
}


OTM_GameInfo::~OTM_GameInfo(void)
{
}

void OTM_GameInfo::Init(void)
{
  SaveSingleTexture = false;
  SaveAllTextures = false;

  KeyBack = -1;
  KeySave = -1;
  KeyNext = -1;
  NumberOfChecked = 0;
  SavePath.Empty();
  OpenPath.Empty();
  Textures.Empty();
}

int OTM_GameInfo::SaveToFile( const wxString &file_name)
{
  wxString name = file_name;
  name += L"_save.txt";
  wxFile file;

  //if (!file.Access(name, wxFile::write)) return -1;
  file.Open(name, wxFile::write);
  if (!file.IsOpened())  {return -1;}

  wxString content;
  if (SavePath.Len()>0)
  {
    content.Printf( L"SavePath:%ls\n", SavePath.wc_str());
    file.Write( content.char_str(), content.Len());
  }

  if (OpenPath.Len()>0)
  {
    content.Printf( L"OpenPath:%ls\n", OpenPath.wc_str());
    file.Write( content.char_str(), content.Len());
  }

  content.Printf( L"SaveAllTextures:%d\nSaveSingeTexture:%d\n", SaveAllTextures, SaveSingleTexture);
  file.Write( content.char_str(), content.Len());

  if (KeyBack>=0)
  {
    content.Printf( L"KeyBack:%d\n", KeyBack);
    file.Write( content.char_str(), content.Len());
  }
  if (KeySave>=0)
  {
    content.Printf( L"KeySave:%d\n", KeySave);
    file.Write( content.char_str(), content.Len());
  }
  if (KeyNext>=0)
  {
    content.Printf( L"KeyNext:%d\n", KeyNext);
    file.Write( content.char_str(), content.Len());
  }

  int num = Textures.GetCount();

  for (int i=0; i<num; i++)
  {
    if (i<NumberOfChecked)
    {
      if (Checked[i]) content.Printf( L"Add_true:%ls\n", Textures[i].wc_str());
      else content.Printf( L"Add_false:%ls\n", Textures[i].wc_str());
    }
    else content.Printf( L"Add_true:%ls\n", Textures[i].wc_str());
    file.Write( content.char_str(), content.Len());
  }

  file.Close();
  return 0;
}


int OTM_GameInfo::LoadFromFile( const wxString &file_name)
{
  Init();

  wxString name = file_name;
  name += L"_save.txt";
  wxFile file;

  if (!file.Access(name, wxFile::read)) return -1;
  file.Open(name, wxFile::read);
  if (!file.IsOpened())  {return -1;}

  unsigned len = file.Length();

  unsigned char* buffer;
  try {buffer = new unsigned char [len+1];}
  catch (...) {return -1;}

  unsigned int result = file.Read( buffer, len);
  file.Close();

  if (result != len) return -1;

  buffer[len]=0;

  wxString content;
  content =  buffer;
  delete [] buffer;

  wxStringTokenizer token( content, "\n");

  int num = token.CountTokens();

  if (LengthOfChecked<num)
  {
    if (Checked!=NULL) delete [] Checked;
    try {Checked = new bool [num+100];}
    catch (...) {Checked=NULL; return -1;}
    LengthOfChecked = num+100;
  }

  wxString line;
  wxString command;
  wxString temp;
  for (int i=0; i<num; i++)
  {
    line = token.GetNextToken();
    command = line.BeforeFirst(':');


    if (command == L"Add_true")
    {
      Checked[NumberOfChecked++] = true;
      Textures.Add(line.AfterFirst(':'));
    }
    else if (command == L"Add_false")
    {
      Checked[NumberOfChecked++] = false;
      Textures.Add(line.AfterFirst(':'));
    }
    else if (command == L"SavePath") SavePath = line.AfterFirst(':');
    else if (command == L"OpenPath") OpenPath = line.AfterFirst(':');
    else if (command == L"SaveAllTextures")
    {
      temp = line.AfterFirst(':');
      if (temp[0]=='0') SaveAllTextures = false;
      else SaveAllTextures = true;
    }
    else if (command == L"SaveSingleTexture")
    {
      temp = line.AfterFirst(':');
      if (temp[0]=='0') SaveSingleTexture = false;
      else SaveSingleTexture = true;
    }
    else if  (command == L"KeyBack")
    {
      temp = line.AfterFirst(':');
      long key;
      if (temp.ToLong( &key)) KeyBack = key;
      else KeyBack = -1;
    }
    else if  (command == L"KeySave")
    {
      temp = line.AfterFirst(':');
      long key;
      if (temp.ToLong( &key)) KeySave = key;
      else KeySave = -1;
    }
    else if  (command == L"KeyNext")
    {
      temp = line.AfterFirst(':');
      long key;
      if (temp.ToLong( &key)) KeyNext = key;
      else KeyNext = -1;
    }


    if (NumberOfChecked>=LengthOfChecked)
    {
      bool *t_bool;
      try {t_bool = new bool [LengthOfChecked+100];}
      catch (...) {return -1;}
      for (int i=0; i<LengthOfChecked; i++) t_bool[i]=Checked[i];
      delete [] Checked;
      Checked = t_bool;
      LengthOfChecked +=100;
    }
  }
  return 0;
}


int OTM_GameInfo::GetChecked( bool* array, int num) const
{
  for (int i=0; i<num && i<NumberOfChecked; i++) array[i] = Checked[i];
  return 0;
}

int OTM_GameInfo::SetChecked( bool* array, int num)
{
  if (num>NumberOfChecked)
  {
    if (Checked!=NULL) delete [] Checked;
    try {Checked = new bool [num+100];}
    catch (...) {Checked=NULL; return -1;}
    LengthOfChecked = num+100;
  }
  for (int i=0; i<num; i++) Checked[i] = array[i];
  return 0;
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

void OTM_GameInfo::GetTextures( wxArrayString &textures) const
{
  textures = Textures;
}

void OTM_GameInfo::AddTexture( const wxString &textures)
{
  Textures.Add(textures);
}


OTM_GameInfo& OTM_GameInfo::operator = (const  OTM_GameInfo &rhs)
{
  SaveSingleTexture = rhs.SaveSingleTexture;
  SaveAllTextures = rhs.SaveAllTextures;

  KeyBack = rhs.KeyBack;
  KeySave = rhs.KeySave;
  KeyNext = rhs.KeyNext;
  if (LengthOfChecked<rhs.LengthOfChecked)
  {
    if (Checked!=NULL) delete [] Checked;
    Checked = new bool [rhs.LengthOfChecked];
    LengthOfChecked = rhs.LengthOfChecked;
  }
  NumberOfChecked = rhs.NumberOfChecked;
  for (int i=0; i<NumberOfChecked; i++) Checked[i] = rhs.Checked[i];

  SavePath = rhs.SavePath;
  OpenPath = rhs.OpenPath;
  Textures = rhs.Textures;

  return *this;
}

