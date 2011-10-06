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


  FontColourSizer = new wxBoxSizer(wxHORIZONTAL);
  FontColour[0] = new wxTextCtrl(this, wxID_ANY, Language.FontColour, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  FontColour[1] = new wxTextCtrl(this, wxID_ANY, "255", wxDefaultPosition, wxDefaultSize);
  FontColour[2] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize);
  FontColour[3] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize);
  for (int i=0; i<4; i++) FontColourSizer->Add( (wxWindow*) FontColour[i], 1, wxEXPAND, 0);

  TextureColourSizer = new wxBoxSizer(wxHORIZONTAL);
  TextureColour[0] = new wxTextCtrl(this, wxID_ANY, Language.TextureColour, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  TextureColour[1] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize);
  TextureColour[2] = new wxTextCtrl(this, wxID_ANY, "255", wxDefaultPosition, wxDefaultSize);
  TextureColour[3] = new wxTextCtrl(this, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize);
  for (int i=0; i<4; i++) TextureColourSizer->Add( (wxWindow*) TextureColour[i], 1, wxEXPAND, 0);


  MainSizer->Add( FontColourSizer, 0, wxEXPAND, 0);
  MainSizer->Add( TextureColourSizer, 0, wxEXPAND, 0);

  SaveSingleTexture = new wxCheckBox( this, -1, Language.CheckBoxSaveSingleTexture);
  MainSizer->Add( (wxWindow*) SaveSingleTexture, 0, wxEXPAND, 0);

  SaveAllTextures = new wxCheckBox( this, -1, Language.CheckBoxSaveAllTextures);
  MainSizer->Add( (wxWindow*) SaveAllTextures, 0, wxEXPAND, 0);

  SavePath = new wxTextCtrl(this, wxID_ANY, Language.TextCtrlSavePath, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  MainSizer->Add( (wxWindow*) SavePath, 0, wxEXPAND, 0);

  MainSizer->AddSpacer(10);


  if (Game.LoadFromFile())
  {
    NumberOfEntry = 0;
    MaxNumberOfEntry = 100;
    CheckBoxes = new wxCheckBox*[MaxNumberOfEntry];
    CheckBoxHSizers = new wxBoxSizer*[MaxNumberOfEntry];
    CheckButtonUp = new wxButton*[MaxNumberOfEntry];
    CheckButtonDown = new wxButton*[MaxNumberOfEntry];
    CheckButtonDelete = new wxButton*[MaxNumberOfEntry];


    SavePath->SetValue(Language.TextCtrlSavePath);
  }
  else
  {
    wxArrayString comments;
    if (Sender.Send( Game, GameOld, &comments)==0) GameOld = Game;

    int key = Game.GetKeyBack();
    if (key>=0) ChoiceKeyBack->SetSelection( key);
    key = Game.GetKeySave();
    if (key>=0) ChoiceKeySave->SetSelection( key);
    key = Game.GetKeyNext();
    if (key>=0) ChoiceKeyNext->SetSelection( key);

    int colour[3];
    Game.GetFontColour( colour);
    SetColour( &FontColour[1], colour);
    Game.GetTextureColour( colour);
    SetColour( &TextureColour[1], colour);

    SaveSingleTexture->SetValue( Game.GetSaveSingleTexture());
    SaveAllTextures->SetValue( Game.GetSaveAllTextures());

    wxString save_path = Language.TextCtrlSavePath;
    save_path << Game.GetSavePath();
    SavePath->SetValue(save_path);

    NumberOfEntry = Game.GetNumberOfFiles();
    Files.Alloc(NumberOfEntry+100);

    Game.GetFiles( Files);

    NumberOfEntry = Files.GetCount();
    MaxNumberOfEntry = NumberOfEntry+100;
    bool *checked = new bool[NumberOfEntry];
    Game.GetChecked( checked, NumberOfEntry);

    CheckBoxes = new wxCheckBox*[MaxNumberOfEntry];
    CheckBoxHSizers = new wxBoxSizer*[MaxNumberOfEntry];
    CheckButtonUp = new wxButton*[MaxNumberOfEntry];
    CheckButtonDown = new wxButton*[MaxNumberOfEntry];
    CheckButtonDelete = new wxButton*[MaxNumberOfEntry];

    for (int i=0; i<NumberOfEntry; i++)
    {
      CheckBoxHSizers[i] = new wxBoxSizer(wxHORIZONTAL);
      CheckBoxes[i] = new wxCheckBox( this, -1, Files[i]);
      CheckBoxes[i]->SetValue( checked[i]);
      CheckBoxes[i]->SetToolTip( comments[i]);

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
  for (int i=0; i<NumberOfEntry; i++)
  {
    Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonUp, this, ID_Button_Texture+3*i);
    Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonDown, this, ID_Button_Texture+3*i+1);
    Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonDelete, this, ID_Button_Texture+3*i+2);
  }

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
  OTM_File file( Language, file_name);
  wxString tool_tip;
  if (file.GetComment( tool_tip))
  {
    LastError = file.LastError;
    file.LastError.Empty();
  }

  CheckBoxHSizers[NumberOfEntry] = new wxBoxSizer(wxHORIZONTAL);
  CheckBoxes[NumberOfEntry] = new wxCheckBox( this, -1, file_name);
  CheckBoxes[NumberOfEntry]->SetValue( true);
  CheckBoxes[NumberOfEntry]->SetToolTip( tool_tip);

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
  Files.Add( file_name);
  NumberOfEntry++;
  MainSizer->Layout();
}

int OTM_GamePage::GetSettings(void)
{
  int key_back = ChoiceKeyBack->GetSelection();
  int key_save = ChoiceKeySave->GetSelection();
  int key_next = ChoiceKeyNext->GetSelection();

  if (key_back==key_save && key_back!=wxNOT_FOUND) {LastError << Language.Error_KeyTwice; return 1;}
  if (key_back==key_next && key_back!=wxNOT_FOUND) {LastError << Language.Error_KeyTwice; return 1;}
  if (key_save==key_next && key_save!=wxNOT_FOUND) {LastError << Language.Error_KeyTwice; return 1;}

  bool save_single = SaveSingleTexture->GetValue();
  bool save_all = SaveAllTextures->GetValue();
  wxString path = Game.GetSavePath();
  if ( (save_single || save_all) && path.Len()==0) {LastError << Language.Error_NoSavePath; return 1;}

  if ( save_single && ( key_back==wxNOT_FOUND || key_save==wxNOT_FOUND || key_next==wxNOT_FOUND) ) {LastError << Language.Error_KeyNotSet; return 1;}

  if (key_back!=wxNOT_FOUND) Game.SetKeyBack(key_back);
  if (key_save!=wxNOT_FOUND) Game.SetKeySave(key_save);
  if (key_next!=wxNOT_FOUND) Game.SetKeyNext(key_next);

  Game.SetSaveSingleTexture( save_single);
  Game.SetSaveAllTextures( save_all);

  int colour[3];
  colour[0] = GetColour( FontColour[1], 255);
  colour[1] = GetColour( FontColour[2], 0);
  colour[2] = GetColour( FontColour[3], 0);
  SetColour( &FontColour[1], colour);
  Game.SetFontColour(colour);

  colour[0] = GetColour( TextureColour[1], 0);
  colour[1] = GetColour( TextureColour[2], 255);
  colour[2] = GetColour( TextureColour[3], 0);
  SetColour( &TextureColour[1], colour);
  Game.SetTextureColour(colour);

  Game.SetFiles( Files);

  bool *checked = new bool[NumberOfEntry];
  for (int i=0; i<NumberOfEntry; i++) checked[i] = CheckBoxes[i]->GetValue();
  Game.SetChecked( checked, NumberOfEntry);
  delete [] checked;

  return 0;
}

int OTM_GamePage::UpdateGame(void)
{
  if (int ret = GetSettings()) return ret;

  if (int ret = Sender.Send( Game, GameOld))
  {
    LastError = Language.Error_Send;
    LastError << "\n" << Sender.LastError;
    Sender.LastError.Empty();
    return ret;
  }

  GameOld = Game;
  return 0;
}

int OTM_GamePage::SaveToFile( const wxString &file_name)
{
  if (int ret = GetSettings()) return ret;
  if (int ret = Game.SaveToFile( file_name))
  {
    LastError = Language.Error_SaveFile;
    LastError <<"\n" << file_name;
    return ret;
  }
  return 0;
}

int OTM_GamePage::SetColour( wxTextCtrl** txt, int *colour)
{
  wxString temp;
  for (int i=0; i<3; i++)
  {
    temp.Empty();
    temp << colour[i];
    txt[i]->SetValue( temp);
  }
  return 0;
}

int OTM_GamePage::GetColour( wxTextCtrl* txt, int def)
{
  wxString temp = txt->GetValue();
  long colour;
  if (temp.ToLong(&colour))
  {
    if (colour<0) colour=0;
    else if (colour>255) colour=255;
  }
  else colour = def;
  return colour;
}


void OTM_GamePage::OnButtonUp(wxCommandEvent& event)
{
  int id = (event.GetId() - ID_Button_Texture)/3;
  if (id <=0 || id>= NumberOfEntry) return;

  wxString cpy_str = Files[id];
  Files[id] = Files[id-1];
  Files[id-1] = cpy_str;

  CheckBoxes[id]->SetLabel(Files[id]);
  CheckBoxes[id-1]->SetLabel(Files[id-1]);

  bool cpy_checked = CheckBoxes[id]->GetValue();
  CheckBoxes[id]->SetValue(CheckBoxes[id-1]->GetValue());
  CheckBoxes[id-1]->SetValue(cpy_checked);

  cpy_str = CheckBoxes[id]->GetToolTip()->GetTip();
  wxString cpy_str2 = CheckBoxes[id-1]->GetToolTip()->GetTip();
  CheckBoxes[id]->SetToolTip(cpy_str2);
  CheckBoxes[id-1]->SetToolTip(cpy_str);

}

void OTM_GamePage::OnButtonDown(wxCommandEvent& event)
{
  int id = (event.GetId() - ID_Button_Texture-1)/3;
  if (id <0 || id>= NumberOfEntry-1) return;

  wxString cpy_str = Files[id];
  Files[id] = Files[id+1];
  Files[id+1] = cpy_str;

  CheckBoxes[id]->SetLabel(Files[id]);
  CheckBoxes[id+1]->SetLabel(Files[id+1]);

  bool cpy_checked = CheckBoxes[id]->GetValue();
  CheckBoxes[id]->SetValue(CheckBoxes[id+1]->GetValue());
  CheckBoxes[id+1]->SetValue(cpy_checked);

  cpy_str = CheckBoxes[id]->GetToolTip()->GetTip();
  wxString cpy_str2 = CheckBoxes[id+1]->GetToolTip()->GetTip();
  CheckBoxes[id]->SetToolTip(cpy_str2);
  CheckBoxes[id+1]->SetToolTip(cpy_str);
}

void OTM_GamePage::OnButtonDelete(wxCommandEvent& event)
{
  int id = (event.GetId() - ID_Button_Texture-2)/3;
  if (id <0 || id>= NumberOfEntry) return;

  for (int i=id+1; i<NumberOfEntry; i++) CheckBoxes[i-1]->SetLabel(Files[i]);
  for (int i=id+1; i<NumberOfEntry; i++) CheckBoxes[i-1]->SetValue(CheckBoxes[i]->GetValue());
  wxString cpy_str;
  for (int i=id+1; i<NumberOfEntry; i++)
  {
    cpy_str = CheckBoxes[i]->GetToolTip()->GetTip();
    CheckBoxes[i-1]->SetToolTip(cpy_str);
  }

  Files.RemoveAt(id, 1);
  NumberOfEntry--;


  Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonUp, this, ID_Button_Texture+3*NumberOfEntry);
  Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonDown, this, ID_Button_Texture+3*NumberOfEntry+1);
  Unbind( wxEVT_COMMAND_BUTTON_CLICKED, &OTM_GamePage::OnButtonDelete, this, ID_Button_Texture+3*NumberOfEntry+2);


  CheckBoxHSizers[NumberOfEntry]->Detach( (wxWindow*) CheckBoxes[NumberOfEntry]);
  CheckBoxHSizers[NumberOfEntry]->Detach( (wxWindow*) CheckButtonUp[NumberOfEntry]);
  CheckBoxHSizers[NumberOfEntry]->Detach( (wxWindow*) CheckButtonDown[NumberOfEntry]);
  CheckBoxHSizers[NumberOfEntry]->Detach( (wxWindow*) CheckButtonDelete[NumberOfEntry]);

  MainSizer->Detach( CheckBoxHSizers[NumberOfEntry]);

  delete CheckBoxes[NumberOfEntry];
  CheckBoxes[NumberOfEntry] = NULL;

  delete CheckButtonUp[NumberOfEntry];
  CheckButtonUp[NumberOfEntry] = NULL;

  delete CheckButtonDown[NumberOfEntry];
  CheckButtonDown[NumberOfEntry] = NULL;

  delete CheckButtonDelete[NumberOfEntry];
  CheckButtonDelete[NumberOfEntry] = NULL;

  delete CheckBoxHSizers[NumberOfEntry];
  CheckBoxHSizers[NumberOfEntry] = NULL;
}


int OTM_GamePage::UpdateLanguage(void)
{
  TextKeyBack->SetValue( Language.KeyBack);
  TextKeySave->SetValue( Language.KeySave);
  TextKeyNext->SetValue( Language.KeyNext);
  FontColour[0]->SetValue( Language.FontColour);
  TextureColour[0]->SetValue( Language.TextureColour);
  SaveAllTextures->SetLabel( Language.CheckBoxSaveAllTextures);
  SaveSingleTexture->SetLabel( Language.CheckBoxSaveSingleTexture);
  wxString temp = Language.TextCtrlSavePath;
  temp << Game.GetSavePath();
  SavePath->SetValue( temp);
  return 0;
}

