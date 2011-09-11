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
  MainSizer->Add( ChoiceKeyBack, 1, wxEXPAND, 0);

  array = Language.Keys;
  array[0] = Language.KeySave;
  ChoiceKeySave = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, array);
  MainSizer->Add( ChoiceKeySave, 1, wxEXPAND, 0);

  array = Language.Keys;
  array[0] = Language.KeyNext;
  ChoiceKeyNext = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, array);
  MainSizer->Add( ChoiceKeyNext, 1, wxEXPAND, 0);


  Game->GetTextures( array);

  NumberOfEntry = array.GetCount()+2;
  MaxNumberOfEntry = NumberOfEntry+100;
  CheckBoxes = new wxCheckBox*[MaxNumberOfEntry];


  CheckBoxes[0] = new wxCheckBox( this, -1, Language.CheckBoxSaveSingleTexture);
  CheckBoxes[0]->SetValue( Game->GetSaveSingleTexture());
  MainSizer->Add( CheckBoxes[0], 1, wxEXPAND, 0);

  CheckBoxes[1] = new wxCheckBox( this, -1, Language.CheckBoxSaveAllTextures);
  CheckBoxes[1]->SetValue( Game->GetSaveAllTextures());
  MainSizer->Add( CheckBoxes[1], 1, wxEXPAND, 0);



  wxString name;
  for (int i=2; i<NumberOfEntry;i++)
  {
    name = array[i-1];
    name = name.AfterLast( '/');
    CheckBoxes[i] = new wxCheckBox( this, -1, name);
    CheckBoxes[i]->SetValue( true);
    MainSizer->Add( CheckBoxes[i], 1, wxEXPAND, 0);
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
  MainSizer->Add( CheckBoxes[NumberOfEntry], 1, wxEXPAND, 0);
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
  wxArrayString send_tex;
  Game->GetTextures( textures);
  int num = textures.GetCount();
  unsigned long hash [num];
  bool add[num];
  bool force[num];
  if (num!=NumberOfEntry-2) return -1;

  wxString name;
  unsigned long temp_hash;
  int count = 0;
  for (int i=0; i<num; i++)
  {

    name = textures[i];
    name = name.AfterLast( '\\');
    name = name.BeforeLast( '.');
    if (name.ToULong( &temp_hash, 16))
    {
      send_tex.Add( textures[i]);
      add[count] =  CheckBoxes[i+2]->GetValue();
      force[count] = true;
      hash[count++] = temp_hash;
    }
    else wxMessageBox("UpdateGame(): no Ulong", "Info", wxOK);

  }
  if (count) return Sender.SendTextures( send_tex, hash, add, force);

  return 0;
}



