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
OTM_GamePage::OTM_GamePage( wxNotebook *parent, OTM_GameInfo *game, PipeStruct &pipe, OTM_Language &lang) : wxPanel(parent), Language(lang), Sender(pipe)
{
  MainSizer = new wxBoxSizer(wxVERTICAL);
  Game = game;

  wxArrayString array;

  array = Language.Keys;
  array[0] = Language.KeyBack;
  ChoiceKeyBack = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, array);
  MainSizer->Add( (wxWindow*) ChoiceKeyBack, 1, wxEXPAND, 0);

  array = Language.Keys;
  array[0] = Language.KeySave;
  ChoiceKeySave = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, array);
  MainSizer->Add( (wxWindow*) ChoiceKeySave, 1, wxEXPAND, 0);

  array = Language.Keys;
  array[0] = Language.KeyNext;
  ChoiceKeyNext = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, array);
  MainSizer->Add( (wxWindow*) ChoiceKeyNext, 1, wxEXPAND, 0);


  Game->GetTextures( array);

  NumberOfEntry = array.GetCount()+2;
  MaxNumberOfEntry = NumberOfEntry+100;
  CheckBoxes = new wxCheckBox*[MaxNumberOfEntry];


  CheckBoxes[0] = new wxCheckBox( this, -1, Language.CheckBoxSaveSingleTexture);
  CheckBoxes[0]->SetValue( Game->GetSaveSingleTexture());
  MainSizer->Add( (wxWindow*) CheckBoxes[0], 1, wxEXPAND, 0);

  CheckBoxes[1] = new wxCheckBox( this, -1, Language.CheckBoxSaveAllTextures);
  CheckBoxes[1]->SetValue( Game->GetSaveAllTextures());
  MainSizer->Add( (wxWindow*) CheckBoxes[1], 1, wxEXPAND, 0);



  wxString name;
  for (int i=2; i<NumberOfEntry;i++)
  {
    name = array[i-1];
    name = name.AfterLast( '/');
    CheckBoxes[i] = new wxCheckBox( this, -1, name);
    CheckBoxes[i]->SetValue( true);
    MainSizer->Add( (wxWindow*) CheckBoxes[i], 1, wxEXPAND, 0);
  }
  SetSizer(MainSizer);
}

OTM_GamePage::~OTM_GamePage(void)
{

}

void OTM_GamePage::SetPath( const wxString &path)
{
  if (path!=Game->GetPath())
  {
    Game->SetPath( path);
    Sender.SendPath( path);
  }
}
void OTM_GamePage::AddTexture( const wxString &file_name)
{
  if (NumberOfEntry>=MaxNumberOfEntry) return;
  Game->AddTexture( file_name);

  wxString file = file_name.AfterLast( '/');
  CheckBoxes[NumberOfEntry] = new wxCheckBox( this, -1, file);
  CheckBoxes[NumberOfEntry]->SetValue( true);
  MainSizer->Add( (wxWindow*) CheckBoxes[NumberOfEntry], 1, wxEXPAND, 0);
  NumberOfEntry++;

  MainSizer->Layout();
}

int OTM_GamePage::UpdateGame(void)
{
  bool save_tex = CheckBoxes[0]->GetValue();
  if (save_tex != Game->GetSaveSingleTexture())
  {
    Sender.SendSaveSingleTexture(save_tex);
    Game->SetSaveSingleTexture(save_tex);
    if (save_tex)
    {
      //to add get choices
      int key_back = VK_DIVIDE;
      int key_save = VK_MULTIPLY;
      int key_next = VK_SUBTRACT;

      if (key_back!=Game->GetKeyBack())
      {
        Game->SetKeyBack( key_back);
        Sender.SendKeyBack(key_back);
      }
      if (key_save!=Game->GetKeySave())
      {
        Game->SetKeySave( key_save);
        Sender.SendKeySave(key_save);
      }
      if (key_next!=Game->GetKeyNext())
      {
        Game->SetKeyNext( key_next);
        Sender.SendKeyNext(key_next);
      }
    }
  }

  save_tex = CheckBoxes[1]->GetValue();
  if (save_tex != Game->GetSaveAllTextures())
  {
    Sender.SendSaveAllTextures(save_tex);
    Game->SetSaveAllTextures(save_tex);
  }

  if (NumberOfEntry<=2) return 0;

  wxArrayString textures;

  Game->GetTextures( textures);
  int num = textures.GetCount();

  if (num!=NumberOfEntry-2) return -1;

  AddTextureClass *tex = new AddTextureClass[num];

  wxString file_type;
  unsigned long temp_hash;
  for (int i=0; i<num; i++)
  {
    file_type = textures[i];
    file_type = file_type.AfterLast( '.');


    if (file_type == L"zip")
    {
      AddZip( &tex[i], textures[i], CheckBoxes[i+2]->GetValue(), true, false);
    }
    else if (file_type == L"tpf")
    {
      AddZip( &tex[i], textures[i], CheckBoxes[i+2]->GetValue(), true, true);
    }
    else if (file_type == L"dds")
    {
      AddFile( &tex[i], textures[i], CheckBoxes[i+2]->GetValue(), true);
    }
    else
    {
      wxString msg = Language.FileNotSupported;
      msg << textures[i];
      wxMessageBox(msg, "ERROR", wxOK);
    }
  }
  Sender.SendTextures( num, tex);


  return 0;
}


int OTM_GamePage::AddFile( AddTextureClass *tex, wxString file, bool add, bool force)
{
  tex->SetSize(1);
  unsigned long temp_hash;

  wxString name = file.AfterLast( '_');
  name = name.BeforeLast( '.');
  if (!name.ToULong( &temp_hash, 16)) return -1; // return if hash could not be extracted

  tex->Add[0] = add;
  if (add)
  {
    wxFile dat(file, wxFile::read);
    if (!dat.IsOpened())  {return -1;}
    unsigned len = file.Length();

    try {tex->Textures[0] = new char [len];}
    catch (...) {tex->Textures[0] = NULL; return -1;}

    unsigned int result = dat.Read( (void*) tex->Textures[0], len);
    dat.Close();

    if (result != len) return -1;
    tex->Size[0] = len;
  }
  else {tex->Size[0] = 0; tex->Textures[0] = NULL;}

  tex->Num = 1;
  tex->Force[0] = true;
  tex->Hash[0] = temp_hash;
  return 0;
}

int OTM_GamePage::AddZip( AddTextureClass *tex, wxString file, bool add, bool force, bool tpf)
{
  wxFile dat(file, wxFile::read);
  if (!dat.IsOpened())  {return -1;}
  unsigned len = dat.Length();

  unsigned char* buffer;
  try {buffer = new unsigned char [len];}
  catch (...) {return -1;}

  unsigned int result = dat.Read( buffer, len);
  dat.Close();

  if (result != len) return -1;

  if (tpf)
  {
    /*
     *
     * BIG THANKS TO Tonttu
     * (TPFcreate 1.5)
     *
     */
    unsigned int TPF_XOR = 0x3FA43FA4u;
    const char pw[] = {0x73, 0x2A, 0x63, 0x7D, 0x5F, 0x0A, 0xA6, 0xBD,
          0x7D, 0x65, 0x7E, 0x67, 0x61, 0x2A, 0x7F, 0x7F,
          0x74, 0x61, 0x67, 0x5B, 0x60, 0x70, 0x45, 0x74,
          0x5C, 0x22, 0x74, 0x5D, 0x6E, 0x6A, 0x73, 0x41,
          0x77, 0x6E, 0x46, 0x47, 0x77, 0x49, 0x0C, 0x4B,
          0x46, 0x6F, '\0'};



    unsigned int j=0;
    while ( j <= result - 4 )
    {
      *( unsigned int* )( &buffer[j] ) ^= TPF_XOR;
      j += 4;
    }
    while ( j < result )
    {
      buffer[j] ^= (unsigned char )( TPF_XOR >> 24 );
      TPF_XOR <<= 4;
      j++;
    }

    if (add) AddContent( (char*) buffer, len, pw, tex, add,  force);
    else RemoveContent( (char*) buffer, len, pw, tex, add,  force);
  }
  else
  {
    if (add) AddContent( (char*) buffer, len, NULL, tex, add,  force);
    else RemoveContent( (char*) buffer, len, NULL, tex, add,  force);
  }

  delete [] buffer;

  return 0;
}


int OTM_GamePage::AddContent( char* buffer, unsigned int len, const char* pw, AddTextureClass *tex, bool add, bool force)
{
  HZIP hz = OpenZip( (void*) buffer, len, pw);

  ZIPENTRY ze;
  int index;
  FindZipItem( hz, L"texmod.def", false, &index, &ze);
  if (index>=0) //if texmod.def is present in the zip file
  {
     char* def;
    int len = ze.unc_size;
    try {def=new char[len+1];}
    catch(...) {CloseZip(hz); return -1;}
    ZRESULT zr = UnzipItem( hz,index, def, len);

    if (zr!=ZR_OK && zr!=ZR_MORE) {delete [] def; CloseZip(hz); return -1;}
    def[len]=0;

    int count = 0;
    for (int i=0; i<len; i++) if (def[i]=='\n') count++;
    if (def[len-1]!='\n') count++;

    tex->SetSize(count+1);

    unsigned long temp_hash;
    int pos = 0;
    count = 0;
    wxString def_file;
    def_file = def;
    wxString conv;

    while (1)
    {
      if (def_file.Len()<11) break;
      conv = def_file.BeforeFirst( '|');
      if (!conv.ToULong( &temp_hash, 16))
      {
        def_file = def_file.AfterFirst( '\n');
        continue;
      }
      def_file = def_file.AfterFirst( '|');
      conv = def_file.BeforeFirst( '\n');
      conv.Replace( "\r", "");

      FindZipItem( hz, conv.wc_str(), false, &index, &ze); // look for texture
      if (index>=0)
      {
        try {tex->Textures[count] = new char[ze.unc_size];}
        catch(...)
        {
          tex->Textures[count] = NULL;
          def_file = def_file.AfterFirst( '\n');
          continue;
        }

        if (ZR_OK!=UnzipItem( hz, index, tex->Textures[count], ze.unc_size))
        {
          delete [] tex->Textures[count];
          tex->Textures[count] = NULL;
        }
        else
        {
          tex->Hash[count] = temp_hash;
          tex->Add[count] = true;
          tex->Size[count] = ze.unc_size;
          tex->Force[count] = force;
          count++;
        }
      }
      def_file = def_file.AfterFirst( '\n');
    }
    tex->Num = count;
  }
  else // texmod.def is not present in the zip file
  {
    wxString name;
    wxString file;
    GetZipItem( hz, -1, &ze); //ask for number of entries
    index = ze.index;

    tex->SetSize(index);
    int count = 0;
    unsigned long temp_hash;
    for (int i=0; i<index; i++)
    {
      if (GetZipItem( hz, i, &ze)!=ZR_OK) continue; //ask for name and size
      file = ze.name;

      name = file.AfterLast( '.');
      if (name!="dds") continue; //if this is not texture file, continue

      name = file.AfterLast( '_');
      name = name.BeforeLast( '.');
      if (!name.ToULong( &temp_hash, 16)) continue; //if hash couldt not be extracted

      try {tex->Textures[count] = new char[ze.unc_size];}
      catch(...)
      {
        tex->Textures[count] = NULL;
        continue;
      }

      ZRESULT rz = UnzipItem( hz, i, tex->Textures[count], ze.unc_size);
      if (ZR_OK!=rz)
      {
        delete [] tex->Textures[count];
        tex->Textures[count] = NULL;
      }
      else
      {
        tex->Hash[count] = temp_hash;
        tex->Add[count] = true;
        tex->Size[count] = ze.unc_size;
        tex->Force[count] = force;
        count++;
      }
    }
    tex->Num = count;
  }
  CloseZip(hz);
  return 0;
}

int OTM_GamePage::RemoveContent( char* buffer, unsigned int len, const char* pw, AddTextureClass *tex, bool add, bool force)
{
  HZIP hz = OpenZip( (void*) buffer, len, pw);

  ZIPENTRY ze;
  int index;
  FindZipItem( hz, L"texmod.def", true, &index, &ze);

  if (index>=0)
  {
    char* def;
    int len = ze.unc_size;
    try {def=new char[len+1];}
    catch(...) {return -1;}

    if (ZR_OK!=UnzipItem( hz,index, def, len)) {delete [] def; return -1;}
    def[len]=0;

    int count = 0;
    for (int i=0; i<len; i++) if (def[i]=='\n') count++;
    if (def[len-1]!='\n') count++;

    tex->SetSize(count);


    unsigned long temp_hash;
    int pos = 0;
    count = 0;
    wxString def_file;
    def_file = def;
    wxString conv;

    while (1)
    {
      if (def_file.Len()<11) break;
      conv = def_file.BeforeFirst( '|');
      if (!conv.ToULong( &temp_hash, 16))
      {
        def_file = def_file.AfterFirst( '\n');
        continue;
      }

      tex->Hash[count] = temp_hash;
      tex->Add[count] = false;
      tex->Size[count] = 0;
      tex->Force[count] = force;
      count++;

      def_file = def_file.AfterFirst( '\n');
    }
    tex->Num = count;
  }
  else
  {
    wxString name;
    wxString file;
    GetZipItem( hz, -1, &ze);
    index = ze.index;

    tex->SetSize(index);
    int count = 0;
    unsigned long temp_hash;
    for (int i=0; i<index; i++)
    {
      if (GetZipItem( hz, i, &ze)!=ZR_OK) continue;
      file = ze.name;
      name = file.AfterLast( '.');
      if (name!="dds") continue;

      name = file.AfterLast( '_');
      name = name.BeforeLast( '.');
      if (!name.ToULong( &temp_hash, 16)) continue;

      tex->Hash[count] = temp_hash;
      tex->Add[count] = false;
      tex->Size[count] = 0;
      tex->Force[count] = force;
      count++;
    }
    tex->Num = count;
  }
  CloseZip(hz);
  return 0;
}
