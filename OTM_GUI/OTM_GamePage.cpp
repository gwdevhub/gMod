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

/*
#ifndef __CDT_PARSER__


BEGIN_EVENT_TABLE(OTM_GamePage, wxFrame)
  EVT_BUTTON(ID_Button_Open, OTM_Frame::OnButtonUp)
  EVT_BUTTON(ID_Button_Path, OTM_Frame::OnButtonDown)
  EVT_BUTTON(ID_Button_Update, OTM_Frame::OnButtonDelete)
END_EVENT_TABLE()
#endif
*/

OTM_GamePage::OTM_GamePage( wxNotebook *parent, const wxString &name, PipeStruct &pipe, OTM_Language &lang) : wxPanel(parent), Language(lang), Game(name), GameOld(name), Sender(pipe, lang)
{
  MainSizer = new wxBoxSizer(wxVERTICAL);

  SizerKeys[0] = new wxBoxSizer(wxHORIZONTAL);
  SizerKeys[1] = new wxBoxSizer(wxHORIZONTAL);

  TextKeyBack = new wxTextCtrl(this, wxID_ANY, Language.KeyBack, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  SizerKeys[0]->Add( (wxWindow*) TextKeyBack, 1, wxEXPAND, 0);
  ChoiceKeyBack = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, Language.KeyStrings);
  SizerKeys[1]->Add( (wxWindow*) ChoiceKeyBack, 1, wxEXPAND, 0);

  TextKeySave = new wxTextCtrl(this, wxID_ANY, Language.KeySave, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  SizerKeys[0]->Add( (wxWindow*) TextKeySave, 1, wxEXPAND, 0);
  ChoiceKeySave = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, Language.KeyStrings);
  SizerKeys[1]->Add( (wxWindow*) ChoiceKeySave, 1, wxEXPAND, 0);

  TextKeyNext = new wxTextCtrl(this, wxID_ANY, Language.KeyNext, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  SizerKeys[0]->Add( (wxWindow*) TextKeyNext, 1, wxEXPAND, 0);
  ChoiceKeyNext = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, Language.KeyStrings);
  SizerKeys[1]->Add( (wxWindow*) ChoiceKeyNext, 1, wxEXPAND, 0);

  MainSizer->Add( SizerKeys[0], 0, wxEXPAND, 0);
  MainSizer->Add( SizerKeys[1], 0, wxEXPAND, 0);

  SaveSingleTexture = new wxCheckBox( this, -1, Language.CheckBoxSaveSingleTexture);
  MainSizer->Add( (wxWindow*) SaveSingleTexture, 0, wxEXPAND, 0);

  SaveAllTextures = new wxCheckBox( this, -1, Language.CheckBoxSaveAllTextures);
  MainSizer->Add( (wxWindow*) SaveAllTextures, 0, wxEXPAND, 0);

  SavePath =  new wxTextCtrl(this, wxID_ANY, Language.TextCtrlSavePath, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  MainSizer->Add( (wxWindow*) SavePath, 0, wxEXPAND, 0);

  MainSizer->AddSpacer(10);


  if (Game.LoadFromFile())
  {
    NumberOfEntry = 0;
    MaxNumberOfEntry = 100;
    CheckBoxes = new wxCheckBox*[MaxNumberOfEntry];
    CheckBoxVSizer = new wxBoxSizer(wxVERTICAL);
    CheckBoxHSizers = new wxBoxSizer*[MaxNumberOfEntry];
    CheckButtonUp = new wxButton*[MaxNumberOfEntry];
    CheckButtonDown = new wxButton*[MaxNumberOfEntry];
    CheckButtonDelete = new wxButton*[MaxNumberOfEntry];


    SavePath->SetValue(Language.TextCtrlSavePath);
  }
  else
  {
    if (Sender.Send( Game, GameOld)==0) GameOld = Game;

    int key = Game.GetKeyBack();
    if (key>=0) ChoiceKeyBack->SetSelection( key);
    key = Game.GetKeySave();
    if (key>=0) ChoiceKeySave->SetSelection( key);
    key = Game.GetKeyNext();
    if (key>=0) ChoiceKeyNext->SetSelection( key);

    SaveSingleTexture->SetValue( Game.GetSaveSingleTexture());
    SaveAllTextures->SetValue( Game.GetSaveAllTextures());

    wxString save_path = Language.TextCtrlSavePath;
    save_path << Game.GetSavePath();
    SavePath->SetValue(save_path);


    wxArrayString array;
    Game.GetTextures( array);

    NumberOfEntry = array.GetCount();
    MaxNumberOfEntry = NumberOfEntry+100;
    CheckBoxes = new wxCheckBox*[MaxNumberOfEntry];
    bool *checked = new bool[NumberOfEntry];
    Game.GetChecked( checked, NumberOfEntry);

    CheckBoxVSizer = new wxBoxSizer(wxVERTICAL);
    CheckBoxHSizers = new wxBoxSizer*[MaxNumberOfEntry];
    CheckButtonUp = new wxButton*[MaxNumberOfEntry];
    CheckButtonDown = new wxButton*[MaxNumberOfEntry];
    CheckButtonDelete = new wxButton*[MaxNumberOfEntry];

    wxString name;
    for (int i=0; i<NumberOfEntry;i++)
    {
      CheckBoxHSizers[i] = new wxBoxSizer(wxHORIZONTAL);
      CheckBoxes[i] = new wxCheckBox( this, -1, array[i]);
      CheckBoxes[i]->SetValue( checked[i]);

      wchar_t button_txt[2];
      button_txt[0] = 8657;
      button_txt[1] = 0;

      CheckButtonUp[i] = new wxButton( this, ID_Button_Texture+3*i, button_txt, wxDefaultPosition, wxSize(24,24));
      Bind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonUp, this, ID_Button_Texture+3*i);

      button_txt[0] = 8659;
      CheckButtonDown[i] = new wxButton( this, ID_Button_Texture+3*i+1, button_txt, wxDefaultPosition, wxSize(24,24));
      Bind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonDown, this, ID_Button_Texture+3*i+1);

      CheckButtonDelete[i] = new wxButton( this, ID_Button_Texture+3*i+2, L"X", wxDefaultPosition, wxSize(24,24));
      Bind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonDelete, this, ID_Button_Texture+3*i+2);

      CheckBoxHSizers[i]->Add( (wxWindow*) CheckBoxes[i], 1, wxEXPAND, 0);
      CheckBoxHSizers[i]->Add( (wxWindow*) CheckButtonUp[i], 0, wxEXPAND, 0);
      CheckBoxHSizers[i]->Add( (wxWindow*) CheckButtonDown[i], 0, wxEXPAND, 0);
      CheckBoxHSizers[i]->Add( (wxWindow*) CheckButtonDelete[i], 0, wxEXPAND, 0);

      MainSizer->Add( CheckBoxHSizers[i], 0, wxEXPAND, 0);
    }
    delete [] checked;
  }
  SetSizer(MainSizer);
}

OTM_GamePage::~OTM_GamePage(void)
{
  delete [] CheckBoxHSizers;
  delete [] CheckButtonUp;
  delete [] CheckButtonDown;
  delete [] CheckButtonDelete;
  delete [] CheckBoxes;
}

int OTM_GamePage::SetSavePath( const wxString &path)
{
  wxString save_path = path;
  save_path.Prepend(Language.TextCtrlSavePath);
  SavePath->SetValue(save_path);
  Game.SetSavePath( path);
  return 0;
}


void OTM_GamePage::AddTexture( const wxString &file_name)
{
  if (NumberOfEntry>=MaxNumberOfEntry) return;

  CheckBoxHSizers[NumberOfEntry] = new wxBoxSizer(wxHORIZONTAL);
  CheckBoxes[NumberOfEntry] = new wxCheckBox( this, -1, file_name);
  CheckBoxes[NumberOfEntry]->SetValue( true);

  wchar_t button_txt[2];
  button_txt[0] = 8657;
  button_txt[1] = 0;
  CheckButtonUp[NumberOfEntry] = new wxButton( this, ID_Button_Texture+3*NumberOfEntry, button_txt, wxDefaultPosition, wxSize(24,24));
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonUp, this, ID_Button_Texture+3*NumberOfEntry);

  button_txt[0] = 8659;
  CheckButtonDown[NumberOfEntry] = new wxButton( this, ID_Button_Texture+3*NumberOfEntry+1, button_txt, wxDefaultPosition, wxSize(24,24));
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonDown, this, ID_Button_Texture+3*NumberOfEntry+1);

  CheckButtonDelete[NumberOfEntry] = new wxButton( this, ID_Button_Texture+3*NumberOfEntry+2, L"X", wxDefaultPosition, wxSize(24,24));
  Bind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonDelete, this, ID_Button_Texture+3*NumberOfEntry+2);

  CheckBoxHSizers[NumberOfEntry]->Add( (wxWindow*) CheckBoxes[NumberOfEntry], 1, wxEXPAND, 0);
  CheckBoxHSizers[NumberOfEntry]->Add( (wxWindow*) CheckButtonUp[NumberOfEntry], 0, wxEXPAND, 0);
  CheckBoxHSizers[NumberOfEntry]->Add( (wxWindow*) CheckButtonDown[NumberOfEntry], 0, wxEXPAND, 0);
  CheckBoxHSizers[NumberOfEntry]->Add( (wxWindow*) CheckButtonDelete[NumberOfEntry], 0, wxEXPAND, 0);

  MainSizer->Add( CheckBoxHSizers[NumberOfEntry], 0, wxEXPAND, 0);
  NumberOfEntry++;
  Game.AddTexture( file_name);
  MainSizer->Layout();
}

int OTM_GamePage::UpdateGame(void)
{
  Game.SetSaveSingleTexture(SaveSingleTexture->GetValue());
  Game.SetSaveAllTextures(SaveAllTextures->GetValue());

  int key = ChoiceKeyBack->GetSelection();
  if (key!=wxNOT_FOUND) Game.SetKeyBack(key);

  key = ChoiceKeySave->GetSelection();
  if (key!=wxNOT_FOUND) Game.SetKeySave(key);

  key = ChoiceKeyNext->GetSelection();
  if (key!=wxNOT_FOUND) Game.SetKeyNext(key);

  bool *checked = new bool[NumberOfEntry];
  for (int i=0; i<NumberOfEntry; i++) checked[i] = CheckBoxes[i]->GetValue();
  Game.SetChecked( checked, NumberOfEntry);
  delete [] checked;

  if (int ret = Sender.Send( Game, GameOld)) return ret;
  GameOld = Game;
  return 0;
}


int OTM_GamePage::SaveToFile( const wxString &file_name)
{
  Game.SetSaveSingleTexture(SaveSingleTexture->GetValue());
  Game.SetSaveAllTextures(SaveAllTextures->GetValue());

  int key = ChoiceKeyBack->GetSelection();
  if (key!=wxNOT_FOUND) Game.SetKeyBack(key);

  key = ChoiceKeySave->GetSelection();
  if (key!=wxNOT_FOUND) Game.SetKeySave(key);

  key = ChoiceKeyNext->GetSelection();
  if (key!=wxNOT_FOUND) Game.SetKeyNext(key);

  bool *checked = new bool[NumberOfEntry];
  for (int i=0; i<NumberOfEntry; i++) checked[i] = CheckBoxes[i]->GetValue();
  Game.SetChecked( checked, NumberOfEntry);
  delete [] checked;

  return Game.SaveToFile( file_name);
}


void OTM_GamePage::OnButtonUp(wxCommandEvent& event)
{
  int id = (event.GetId() - ID_Button_Texture)/3;
  if (id <=0 || id>= NumberOfEntry) return;

  wxArrayString array;
  Game.GetTextures( array);

  wxString cpy = array[id];
  array[id] = array[id-1];
  array[id-1] = cpy;

  Game.SetTextures( array);

  CheckBoxes[id]->SetLabel(array[id]);
  CheckBoxes[id-1]->SetLabel(array[id-1]);

  bool cpy_checked = CheckBoxes[id]->GetValue();
  CheckBoxes[id]->SetValue(CheckBoxes[id-1]);
  CheckBoxes[id-1]->SetValue(cpy_checked);
}

void OTM_GamePage::OnButtonDown(wxCommandEvent& event)
{
  int id = (event.GetId() - ID_Button_Texture-1)/3;
  if (id <0 || id>= NumberOfEntry-1) return;

  wxArrayString array;
  Game.GetTextures( array);

  wxString cpy = array[id];
  array[id] = array[id+1];
  array[id+1] = cpy;

  Game.SetTextures( array);

  CheckBoxes[id]->SetLabel(array[id]);
  CheckBoxes[id+1]->SetLabel(array[id+1]);

  bool cpy_checked = CheckBoxes[id]->GetValue();
  CheckBoxes[id]->SetValue(CheckBoxes[id+1]);
  CheckBoxes[id+1]->SetValue(cpy_checked);
}

void OTM_GamePage::OnButtonDelete(wxCommandEvent& event)
{
  int id = (event.GetId() - ID_Button_Texture-2)/3;
  if (id <0 || id>= NumberOfEntry) return;

  wxArrayString array;
  Game.GetTextures( array);

  for (int i=id+1; i<NumberOfEntry; i++) CheckBoxes[i-1]->SetLabel(array[i]);

  array.RemoveAt(id, 1);
  Game.SetTextures( array);
  NumberOfEntry--;


  Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonUp, this, 3*id);
  Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonDown, this, 3*id+1);
  Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonDelete, this, 3*id+2);

  CheckBoxVSizer->Detach( CheckBoxHSizers[NumberOfEntry]);

  CheckBoxHSizers[NumberOfEntry]->Detach( (wxWindow*) CheckBoxes[NumberOfEntry]);
  CheckBoxHSizers[NumberOfEntry]->Detach( (wxWindow*) CheckButtonUp[NumberOfEntry]);
  CheckBoxHSizers[NumberOfEntry]->Detach( (wxWindow*) CheckButtonDown[NumberOfEntry]);
  CheckBoxHSizers[NumberOfEntry]->Detach( (wxWindow*) CheckButtonDelete[NumberOfEntry]);

  delete CheckBoxHSizers[NumberOfEntry];
  CheckBoxHSizers[NumberOfEntry] = NULL;
  delete CheckBoxes[NumberOfEntry];
  CheckBoxes[NumberOfEntry] = NULL;
  delete CheckButtonUp[NumberOfEntry];
  CheckButtonUp[NumberOfEntry] = NULL;
  delete CheckButtonDown[NumberOfEntry];
  CheckButtonDown[NumberOfEntry] = NULL;
  delete CheckButtonDelete[NumberOfEntry];
  CheckButtonDelete[NumberOfEntry] = NULL;
}
