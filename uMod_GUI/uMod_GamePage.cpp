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



#ifndef __CDT_PARSER__


BEGIN_EVENT_TABLE(uMod_GamePage, wxPanel)

EVT_COLLAPSIBLEPANE_CHANGED(ID_CollPane, uMod_GamePage::OnCollPane)

EVT_CHECKBOX( ID_SaveSingleTexture, uMod_GamePage::OnCheckBox)
EVT_CHECKBOX( ID_SaveAllTexture, uMod_GamePage::OnCheckBox)

EVT_CHECKBOX( ID_ShowSingleTextureString, uMod_GamePage::OnCheckBox)
EVT_CHECKBOX( ID_UseSizeFilter, uMod_GamePage::OnCheckBox)
EVT_CHECKBOX( ID_UseFormatFilter, uMod_GamePage::OnCheckBox)
EVT_BUTTON(ID_SetFormatFilter, uMod_GamePage::OnButtonFormatFilter)

EVT_BUTTON(ID_Button_SavePath, uMod_GamePage::OnButtonSavePath)


EVT_BUTTON(ID_FontColour, uMod_GamePage::OnButtonColour)
EVT_BUTTON(ID_TextureColour, uMod_GamePage::OnButtonColour)

EVT_DATAVIEW_ITEM_CONTEXT_MENU(ID_DataViewCtrl, uMod_GamePage::OnContextMenu)
//EVT_MENU(ID_RemovePackage, uMod_GamePage::OnRemovePackage)


EVT_DATAVIEW_ITEM_BEGIN_DRAG( ID_DataViewCtrl, uMod_GamePage::OnBeginDrag )
EVT_DATAVIEW_ITEM_DROP_POSSIBLE( ID_DataViewCtrl, uMod_GamePage::OnDropPossible )
EVT_DATAVIEW_ITEM_DROP( ID_DataViewCtrl, uMod_GamePage::OnDrop )

EVT_DROP_FILES(uMod_GamePage::OnDropFile)

END_EVENT_TABLE()

#endif

uMod_GamePage::uMod_GamePage( wxNotebook *parent, const wxString &exe, int injection_method, const wxString &save, PipeStruct &pipe)
  : wxPanel(parent, wxID_ANY)//, wxDefaultPosition, wxDefaultSize, wxVSCROLL)
    , InjectionMethod(injection_method), Sender(pipe)
{
  ExeName = exe;
  TemplateName = save;
  ViewCtrl = (wxDataViewCtrl*)0;

  CounterDX9 = 0; CounterDX9EX = 0;
  CounterDX10 = 0; CounterDX101 = 0;
  //SetBackgroundColour( *wxLIGHT_GREY);
  //SetBackgroundColour( wxColour( "LIGHT GREY"));


  MainSizer = new wxBoxSizer(wxVERTICAL);

  //initialize injection string
  switch (InjectionMethod)
  {
  case HOOK_INJECTION:
    DX_DLL_Info = new wxTextCtrl(this, wxID_ANY, Language->HookInjection, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    break;
  case DIRECT_INJECTION:
    DX_DLL_Info = new wxTextCtrl(this, wxID_ANY,  Language->DirectInjection, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    break;
  case NO_INJECTION:
    DX_DLL_Info = new wxTextCtrl(this, wxID_ANY,  Language->NoInjection, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    break;
  case INVALID_GAME_PAGE:
    DX_DLL_Info = new wxTextCtrl(this, wxID_ANY,  Language->InvalidGamePage, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    break;
  default:
    DX_DLL_Info = new wxTextCtrl(this, wxID_ANY, "BUG^^ injection:", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    break;
  }
  MainSizer->Add( (wxWindow*) DX_DLL_Info, 0, wxEXPAND, 0);

  // initialize template string
  TemplateFile = new wxTextCtrl(this, wxID_ANY, Language->TextCtrlTemplate, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  MainSizer->Add( (wxWindow*) TemplateFile, 0, wxEXPAND, 0);
  MainSizer->AddSpacer(10);

  //
  // capture texture part
  //

  // create collapsible pane
  CollPane = new  wxCollapsiblePane(this, ID_CollPane, Language->CollapseTextureCapture,wxDefaultPosition,wxDefaultSize,wxCP_NO_TLW_RESIZE);


  CollSizer = new wxBoxSizer(wxVERTICAL);

  // get window inside the collapsible pane
  wxWindow *win = CollPane->GetPane();

  // this sizer contains the left save-single-texture part and the right save-all-texture part
  TextureSizer = new wxBoxSizer(wxHORIZONTAL);


  // this sizer contains the left save-single-texture part
  SingleTexturePanel = new wxPanel(win);
  SingleTextureSizer = new wxBoxSizer(wxVERTICAL);

  // check box for save-single-texture mode
  SaveSingleTexture = new wxCheckBox( SingleTexturePanel, ID_SaveSingleTexture, Language->CheckBoxSaveSingleTexture);
  SingleTextureSizer->Add( (wxWindow*) SaveSingleTexture, 0, wxEXPAND, 0);

  // check box for showing the string
  ShowSingleTextureString = new wxCheckBox( SingleTexturePanel, ID_ShowSingleTextureString, Language->CheckBoxShowStringSaveSingleTexture);
  SingleTextureSizer->Add( (wxWindow*) ShowSingleTextureString, 0, wxEXPAND|wxALL, 5);


  //set the two colour buttons into one horizontal sizer
  wxBoxSizer *temp_sizer = new wxBoxSizer(wxHORIZONTAL);
  FontColour = new wxButton( SingleTexturePanel, ID_FontColour, Language->FontColour, wxDefaultPosition, wxSize(100,24));
  FontColour->Enable(false); //siable the font color, per default ShowSingleTextureString is not checked
  temp_sizer->Add( FontColour, 0, wxEXPAND, 0);

  temp_sizer->AddSpacer(10);
  TextureColour = new wxButton( SingleTexturePanel, ID_TextureColour, Language->TextureColour, wxDefaultPosition, wxSize(100,24));
  temp_sizer->Add( TextureColour, 0, wxEXPAND, 0);

  temp_sizer->AddStretchSpacer();
  SingleTextureSizer->Add( temp_sizer, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  // set the key selector
  KeyBack = new uMod_KeyPanel(SingleTexturePanel, Language->KeyBack, -1);
  SingleTextureSizer->Add( KeyBack, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
  KeySave = new uMod_KeyPanel(SingleTexturePanel, Language->KeySave, -1);
  SingleTextureSizer->Add( KeySave, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
  KeyNext = new uMod_KeyPanel(SingleTexturePanel, Language->KeyNext, -1);
  SingleTextureSizer->Add( KeyNext, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  // append an empty strechable sizer at the end
  SingleTextureSizer->AddStretchSpacer();
  SingleTexturePanel->SetSizer(SingleTextureSizer);



  // this sizer contains the right save-all-texture part
  AllTexturePanel = new wxPanel(win);
  AllTextureSizer = new wxBoxSizer(wxVERTICAL);

  SaveAllTextures = new wxCheckBox( AllTexturePanel, ID_SaveAllTexture, Language->CheckBoxSaveAllTextures);
  AllTextureSizer->Add( (wxWindow*) SaveAllTextures, 0, wxEXPAND, 0);


  UseSizeFilter = new wxCheckBox( AllTexturePanel, ID_UseSizeFilter, Language->CheckBoxUseSizeFilter);
  AllTextureSizer->Add( (wxWindow*) UseSizeFilter, 0, wxEXPAND|wxALL, 5);


  //temp_sizer = new wxBoxSizer(wxVERTICAL);
  SpinWidth = new uMod_SpinPanel(AllTexturePanel);
  SpinWidth->SetLabel( Language->WidthSpin);
  SpinWidth->SetValue( Game.WidthMin(), Game.WidthMax());
  SpinWidth->Enable(false);
  //temp_sizer->Add( (wxWindow*) SpinWidth, 0, wxEXPAND, 0);
  AllTextureSizer->Add( SpinWidth, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

  SpinHeight = new uMod_SpinPanel(AllTexturePanel);
  SpinHeight->SetLabel( Language->HeightSpin);
  SpinHeight->SetValue( Game.HeightMin(), Game.HeightMax());
  SpinHeight->Enable(false);
  //temp_sizer->Add( (wxWindow*) SpinHeight, 0, wxEXPAND, 0);
  AllTextureSizer->Add( SpinHeight, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

  SpinDepth = new uMod_SpinPanel(AllTexturePanel);
  SpinDepth->SetLabel( Language->DepthSpin);
  SpinDepth->SetValue( Game.DepthMin(), Game.DepthMax());
  SpinDepth->Enable(false);
  //temp_sizer->Add( (wxWindow*) SpinDepth, 0, wxEXPAND, 0);
  AllTextureSizer->Add( SpinDepth, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);
  //AllTextureSizer->Add( temp_sizer, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);



  //wxCheckBox *UseFormatFilter;
  UseFormatFilter = new wxCheckBox( AllTexturePanel, ID_UseFormatFilter, Language->CheckBoxUseFormatFilter);
  AllTextureSizer->Add( (wxWindow*) UseFormatFilter, 0, wxEXPAND, 0);

  temp_sizer = new wxBoxSizer(wxHORIZONTAL);
  FormatFilter = new wxButton( AllTexturePanel, ID_SetFormatFilter, Language->SetFormatFilter, wxDefaultPosition, wxSize(100,24));
  FormatFilter->Enable(false);
  temp_sizer->Add( (wxWindow*) FormatFilter, 0, wxEXPAND|wxALL, 5);
  temp_sizer->AddStretchSpacer();
  AllTextureSizer->Add( temp_sizer, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);



  AllTextureSizer->AddStretchSpacer();
  AllTexturePanel->SetSizer(AllTextureSizer);




  //combine both texture sizer
  TextureSizer->Add( SingleTexturePanel, 1, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);
  TextureSizer->Add( AllTexturePanel, 1, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);

  CollSizer->Add( TextureSizer, 1, wxEXPAND, 0);

  wxArrayString choices;
  choices.Add("BMP");
  choices.Add("JPG");
  choices.Add("TGA");
  choices.Add("PNG");
  choices.Add("DDS");
  choices.Add("PPM");
  FileFormats = new uMod_CheckBoxArray( win, choices, -1);
  FileFormats->SetValue( Game.FileFormat());
  CollSizer->Add( (wxWindow*) FileFormats, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);



  // add save path string
  temp_sizer = new wxBoxSizer(wxHORIZONTAL);
  SavePathButton = new wxButton( win, ID_Button_SavePath, Language->ButtonDirectory, wxDefaultPosition, wxSize(100,24));
  temp_sizer->Add( (wxWindow*) SavePathButton, 0, wxEXPAND|wxALL, 0);

  temp_sizer->AddSpacer(10);
  SavePath = new wxTextCtrl(win, wxID_ANY, Language->TextCtrlSavePath, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
  temp_sizer->Add( (wxWindow*) SavePath, 1, wxEXPAND, 0);
  CollSizer->Add( temp_sizer, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 10);


  win->SetSizer(CollSizer);
  CollSizer->SetSizeHints(win);

  MainSizer->Add(CollPane, 0, wxEXPAND, 5);
  MainSizer->AddSpacer(10);




  //
  // initialize the treeview for the mod packages
  //

  ViewCtrl = new wxDataViewCtrl( (wxWindow*) this, ID_DataViewCtrl, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE);
  ViewModel = new uMod_TreeViewModel();
  ViewCtrl->AssociateModel(ViewModel);
  ViewModel->DecRef();


  //first row
  wxDataViewTextRenderer *tr =
      new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
  wxDataViewColumn *column =
      new wxDataViewColumn( Language->Title, tr, 0, 200, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
  ViewCtrl->AppendColumn( column );


  //second row
  wxDataViewToggleRenderer *toglle =
      new wxDataViewToggleRenderer( "bool", wxDATAVIEW_CELL_ACTIVATABLE);
  column = new wxDataViewColumn( "A", toglle, 1, 20, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
  ViewCtrl->AppendColumn( column );


  //third row
  toglle = new wxDataViewToggleRenderer("bool", wxDATAVIEW_CELL_ACTIVATABLE);
  column = new wxDataViewColumn( "D", toglle, 2, 20, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
  ViewCtrl->AppendColumn( column );


  //4-th row
  tr = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
  column = new wxDataViewColumn( Language->Author, tr, 3, 80, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
  ViewCtrl->AppendColumn( column );


  //5-th row
  tr = new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
  column = new wxDataViewColumn( Language->Comment, tr, 4, 400, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE );
  ViewCtrl->AppendColumn( column );


  //enable drag and drop inside the ViewCtrl (for the items of the ViewCtrl)
  ViewCtrl->EnableDragSource( wxDF_UNICODETEXT);
  ViewCtrl->EnableDropTarget(wxDF_UNICODETEXT );

  //enables drag and drop for files
  DragAcceptFiles(true);
  MainSizer->Add(ViewCtrl, 1, wxEXPAND, 0);

  SetSizer(MainSizer);

  if (LastError.IsEmpty())
  {
    if (TemplateName.Len()>0) LoadTemplate(TemplateName);
    LastError.Empty();
  }
}

uMod_GamePage::~uMod_GamePage(void)
{

}

int uMod_GamePage::SetSavePath( const wxString &path)
{
  wxString save_path = Language->TextCtrlSavePath;
  save_path << path;
  SavePath->SetValue(save_path);
  Game.SetSavePath( path);
  return 0;
}

int uMod_GamePage::AddPackage( const wxString &file_name)
{
  if (ViewModel->AddPackage(file_name))
  {
    LastError << ViewModel->LastError;
    return -1;
  }
  return 0;
}

int uMod_GamePage::AddPackages(const wxString *files, int num)
{
  wxArrayString strings( num, files);
  if (ViewModel->AddPackages( strings))
  {
    LastError << ViewModel->LastError;
    return -1;
  }
  return 0;
}

int uMod_GamePage::AddPackages(const wxArrayString &files)
{
  if (ViewModel->AddPackages( files))
  {
    LastError << ViewModel->LastError;
    return -1;
  }
  return 0;
}

wxString uMod_GamePage::GetPageName(void)
{
  if (InjectionMethod == INVALID_GAME_PAGE) return "uMod";

  wxString ret;
  ret = ExeName;

  ret = ret.AfterLast('\\');
  ret = ret.AfterLast('/');
  ret = ret.BeforeLast('.');
  return ret;
}

int uMod_GamePage::AddDXDevice(int version)
{
  switch (version)
  {
  case VERSION_DX9:
    CounterDX9++; break;
  case VERSION_DX9EX:
    CounterDX9EX++; break;
  case VERSION_DX10:
    CounterDX10++; break;
  case VERSION_DX101:
    CounterDX101++; break;
  }
  return SetInfo();
}

int uMod_GamePage::RemoveDXDevice(int version)
{
  switch (version)
  {
  case VERSION_DX9:
    if (CounterDX9>0) CounterDX9--; break;
  case VERSION_DX9EX:
    if (CounterDX9EX>0) CounterDX9EX--; break;
  case VERSION_DX10:
    if (CounterDX10>0) CounterDX10--; break;
  case VERSION_DX101:
    if (CounterDX101>0) CounterDX101--; break;
  }
  return SetInfo();
}

int uMod_GamePage::SetInfo(void)
{
  wxString info;
  switch (InjectionMethod)
  {
  case HOOK_INJECTION:
    info = Language->HookInjection;
    break;
  case DIRECT_INJECTION:
    info = Language->DirectInjection;
    break;
  case NO_INJECTION:
    info = Language->NoInjection;
    break;
  default:
    info = "BUG^^ injection:";
    break;
  }

  if (CounterDX9 || CounterDX9EX || CounterDX10 || CounterDX101)
  {
    wxString temp;
    info.append(" (");
    if (CounterDX9)
    {
      if (CounterDX9==1) temp.Printf("DX9");
      else temp.Printf("DX9: %d", CounterDX9);
      info.append(temp);
    }
    if (CounterDX9EX)
    {
      if (!temp.IsEmpty()) info.append(",");
      if (CounterDX9EX==1) temp.Printf("DX9EX");
      else temp.Printf("DX9EX: %d", CounterDX9EX);
      info.append(temp);
    }
    if (CounterDX10)
    {
      if (!temp.IsEmpty()) info.append(",");
      if (CounterDX10==1) temp.Printf("DX10");
      else temp.Printf("DX10: %d", CounterDX10);
      info.append(temp);
    }
    if (CounterDX101)
    {
      if (!temp.IsEmpty()) info.append(",");
      if (CounterDX101==1) temp.Printf("DX10.1");
      else temp.Printf("DX10.1: %d", CounterDX101);
      info.append(temp);
    }
    info.append(")");
  }
  DX_DLL_Info->SetValue(info);
  return 0;
}

int uMod_GamePage::GetSettings(void)
{
  Game.ShowCollPane() = CollPane->IsExpanded();

  int key_back = KeyBack->GetKey();
  int key_save = KeySave->GetKey();
  int key_next = KeyNext->GetKey();

  if (key_back==key_save && key_back>=0) {LastError << Language->Error_KeyTwice; return 1;}
  if (key_back==key_next && key_back>=0) {LastError << Language->Error_KeyTwice; return 1;}
  if (key_save==key_next && key_save>=0) {LastError << Language->Error_KeyTwice; return 1;}


  bool save_single = SaveSingleTexture->GetValue();
  bool save_all = SaveAllTextures->GetValue();
  wxString path = Game.GetSavePath();
  if ( (save_single || save_all) && path.Len()==0) {LastError << Language->Error_NoSavePath; return 1;}

  if ( save_single && ( key_back==wxNOT_FOUND || key_save==wxNOT_FOUND || key_next==wxNOT_FOUND) ) {LastError << Language->Error_KeyNotSet; return 1;}

  Game.SetKeyBack(key_back);
  Game.SetKeySave(key_save);
  Game.SetKeyNext(key_next);

  Game.SetSaveSingleTexture( save_single);
  Game.SetSaveAllTextures( save_all);

  Game.ShowSingleTextureString() = ShowSingleTextureString->GetValue();

  Game.UseSizeFilter() = UseSizeFilter->GetValue();
  SpinHeight->GetValue( Game.HeightMin(), Game.HeightMax());
  SpinWidth->GetValue( Game.WidthMin(), Game.WidthMax());
  SpinDepth->GetValue( Game.DepthMin(), Game.DepthMax());

  Game.UseFormatFilter() = UseFormatFilter->GetValue();
  FileFormats->GetValue(Game.FileFormat());
  return 0;
}

int uMod_GamePage::UpdateGame(void)
{
  if (InjectionMethod==INVALID_GAME_PAGE) return 0;
  if (int ret = GetSettings()) return ret;

  uMod_TextureElement_SortedArrayPtr* current_textures = new uMod_TextureElement_SortedArrayPtr(Compare_uMod_TextureElement);

  ViewModel->GetActiveTexture( *current_textures);
  if (int ret = Sender.Send( Game, GameOld, current_textures, false))
  {
    LastError = Language->Error_Send;
    LastError << "\n" << Sender.LastError;
    Sender.LastError.Empty();
    return ret;
  }

  GameOld = Game;
  return 0;
}


int uMod_GamePage::ReloadGame(void)
{
  if (InjectionMethod==INVALID_GAME_PAGE) return 0;
  if (int ret = GetSettings()) return ret;

  wxString content;
  SaveTemplateToString(content);
  return LoadTemplateFromString(content);
}

int uMod_GamePage::SaveTemplate( const wxString &file_name)
{
  wxFile file;

  file.Open(file_name, wxFile::write);
  if (!file.IsOpened())
  {
    LastError = Language->Error_FileOpen << "\n" << file_name;
    return -1;
  }

  wxString content;
  if (int ret = SaveTemplateToString(content)) return ret;

  file.Write( content.char_str(), content.Len());
  file.Close();

  TemplateName = file_name;
  wxString path;
  path = Language->TextCtrlTemplate;
  path << TemplateName;
  TemplateFile->SetValue( path);

  return 0;
}

int uMod_GamePage::SaveTemplateToString( wxString &content)
{
  if (int ret = GetSettings()) return ret;

  Game.SaveToString( content );

  content << "\n\n";

  wxString temp;
  for (unsigned int i=0; i<ViewModel->myPackages.GetCount(); i++)
  {
    temp.Empty();
    ViewModel->Node2String( temp, ViewModel->myPackages[i]);
    content << temp << "\n\n";
  }
  return 0;
}

int uMod_GamePage::LoadTemplate( const wxString &file_name)
{
  LastError.Empty();

  if (file_name.Len()==0) return -1;

  wxFile file;
  if (!file.Access(file_name, wxFile::read))
  {
    LastError = Language->Error_FileOpen << "\n" << file_name;
    return -1;
  }
  file.Open(file_name, wxFile::read);
  if (!file.IsOpened())
  {
    LastError = Language->Error_FileOpen << "\n" << file_name;
    return -1;
  }

  unsigned len = file.Length();

  unsigned char* buffer = (unsigned char*)0;
  if (GetMemory( buffer, len+1))
  {
    LastError = Language->Error_Memory;
    return -1;
  }
  unsigned int result = file.Read( buffer, len);
  file.Close();

  if (result != len)
  {
    LastError = Language->Error_FileRead << "\n" << file_name;
    delete [] buffer;
    return -1;
  }

  buffer[len]=0;

  wxString content;
  content = buffer;
  delete [] buffer;

  if (int ret = LoadTemplateFromString(content)) return ret;

  TemplateName = file_name;
  wxString path;
  path = Language->TextCtrlTemplate;
  path << TemplateName;
  TemplateFile->SetValue( path);

  return 0;
}

int uMod_GamePage::LoadTemplateFromString( const wxString &content_orig)
{
  wxString content = content_orig;
  wxArrayString packages;
  int index = content.find("\nPACKAGE|");
  if (index!=wxNOT_FOUND)
  {
    Game.LoadFromString( content.Mid(0,index));
    content = content.Mid(index);
    while (1)
    {
      index = content.find("\nPACKAGE|",10);
      if (index==wxNOT_FOUND)
      {
        packages.Add(content);
        break;
      }
      else
      {
        packages.Add(content.Mid(0,index));
        content = content.Mid(index);
      }
    }
  }
  else
  {
    Game.LoadFromString(content);
  }

  if (ViewModel->AddPackagesFromTemplate( packages))
  {
    LastError << ViewModel->LastError;
  }


  //if (Sender.Send( Game, GameOld, true)==0) GameOld = Game;

  CollPane->Collapse( !Game.ShowCollPane());


  if (Game.ShowSingleTextureString())
  {
    ShowSingleTextureString->SetValue(true);
    FontColour->Enable();
  }
  else
  {
    ShowSingleTextureString->SetValue(false);
    FontColour->Enable(false);
  }

  KeyBack->SetKey(Game.GetKeyBack());
  KeySave->SetKey(Game.GetKeySave());
  KeyNext->SetKey(Game.GetKeyNext());

  SpinWidth->SetValue( Game.WidthMin(), Game.WidthMax());
  SpinHeight->SetValue( Game.HeightMin(), Game.HeightMax());
  SpinDepth->SetValue( Game.DepthMin(), Game.DepthMax());

  if (Game.UseSizeFilter())
  {
    UseSizeFilter->SetValue(true);
    SpinWidth->Enable();
    SpinHeight->Enable();
    SpinDepth->Enable();
  }
  else
  {
    UseSizeFilter->SetValue(false);
    SpinWidth->Enable(false);
    SpinHeight->Enable(false);
    SpinDepth->Enable(false);
  }

  if (Game.UseFormatFilter())
  {
    UseFormatFilter->SetValue(true);
    FormatFilter->Enable();
  }
  else
  {
    UseFormatFilter->SetValue(false);
    FormatFilter->Enable(false);
  }


  if (Game.GetSaveSingleTexture())
  {
    SaveSingleTexture->SetValue( true);
    SaveAllTextures->SetValue(false);
    SingleTexturePanel->Enable();
    AllTexturePanel->Disable();
  }
  else if (Game.GetSaveAllTextures())
  {
    SaveSingleTexture->SetValue( false);
    SaveAllTextures->SetValue( true);
    SingleTexturePanel->Disable();
    AllTexturePanel->Enable();
  }
  else
  {
    SaveSingleTexture->SetValue( false);
    SaveAllTextures->SetValue(false);
    SingleTexturePanel->Enable();
    AllTexturePanel->Enable();
  }

  FileFormats->SetValue(Game.FileFormat());

  wxString path = Language->TextCtrlSavePath;
  path << Game.GetSavePath();
  SavePath->SetValue( path);


  // The wxCollapsiblePane could be hide/shown thus we must manually layout and refresh the sizer and the window
  // let the main sizer refresh its content
  MainSizer->Layout();

  // VieCtrl must repaint otherwise some graphical glitches ocure
  ViewCtrl->Refresh();

  if (LastError.IsEmpty())
  {
    return UpdateGame();
  }
  else return -1;
}

void uMod_GamePage::OnCollPane(wxCollapsiblePaneEvent& WXUNUSED(event))
{
  // let the main sizer refresh its content
  MainSizer->Layout();

  // VieCtrl must repaint otherwise some graphical glitches ocure
  ViewCtrl->Refresh();
}

void uMod_GamePage::OnButtonColour(wxCommandEvent& event)
{
  int id = event.GetId();

  unsigned char colour[4];
  if (id==ID_FontColour)
    Game.GetFontColour(colour);
  else if (id==ID_TextureColour)
    Game.GetTextureColour(colour);
  else return;

  wxColourData data;
  data.SetColour(wxColour(colour[0],colour[1],colour[2],colour[3]));

  wxColourDialog *dialog = new wxColourDialog( this, &data);
  if (dialog->ShowModal() == wxID_OK)
  {
    data = dialog->GetColourData();
    colour[0] = data.GetColour().Red();
    colour[1] = data.GetColour().Green();
    colour[2] = data.GetColour().Blue();
    colour[3] = data.GetColour().Alpha();

    if (id==ID_FontColour)
      Game.SetFontColour(colour);
    else if (id==ID_TextureColour)
      Game.SetTextureColour(colour);
  }
}

void uMod_GamePage::OnCheckBox(wxCommandEvent& event)
{
  int id = event.GetId();
  switch (id)
  {
  case ID_SaveSingleTexture:
  {
    if (SaveSingleTexture->GetValue())
      AllTexturePanel->Disable();
    else
      AllTexturePanel->Enable();
    break;
  }
  case ID_SaveAllTexture:
  {
    if (SaveAllTextures->GetValue())
      SingleTexturePanel->Disable();
    else
      SingleTexturePanel->Enable();
    break;
  }
  case ID_UseSizeFilter:
  {
    if (UseSizeFilter->GetValue())
    {
      SpinWidth->Enable();
      SpinHeight->Enable();
      SpinDepth->Enable();
    }
    else
    {
      SpinWidth->Enable(false);
      SpinHeight->Enable(false);
      SpinDepth->Enable(false);
    }
    break;
  }
  case ID_UseFormatFilter:
  {
    if (UseFormatFilter->GetValue())
      FormatFilter->Enable();
    else
      FormatFilter->Enable(false);
    break;
  }
  case ID_ShowSingleTextureString:
  {
    if (ShowSingleTextureString->GetValue())
      FontColour->Enable();
    else
      FontColour->Enable(false);
    break;
  }
  default:
    break;
  }
}

void uMod_GamePage::OnButtonFormatFilter(wxCommandEvent& WXUNUSED(event))
{
  wxArrayString choices;
  choices.Add("D3DFMT_R8G8B8 (20)");
  choices.Add("D3DFMT_A8R8G8B8 (21)");
  choices.Add("D3DFMT_X8R8G8B8 (22)");
  choices.Add("D3DFMT_R5G6B5 (23)");
  choices.Add("D3DFMT_X1R5G5B5 (24)");
  choices.Add("D3DFMT_A1R5G5B5 (25)");
  choices.Add("D3DFMT_A4R4G4B4 (26)");
  choices.Add("D3DFMT_R3G3B2 (27)");
  choices.Add("D3DFMT_A8 (28)");
  choices.Add("D3DFMT_A8R3G3B2 (29)");
  choices.Add("D3DFMT_X4R4G4B4 (30)");
  choices.Add("D3DFMT_A2B10G10R10 (31)");
  choices.Add("D3DFMT_A8B8G8R8 (32)");
  choices.Add("D3DFMT_X8B8G8R8 (33)");
  choices.Add("D3DFMT_G16R16 (34)");
  choices.Add("D3DFMT_A2R10G10B10 (35)");
  choices.Add("D3DFMT_A16B16G16R16 (36)");

  choices.Add("D3DFMT_A8P8 (40)");
  choices.Add("D3DFMT_P8 (41)");

  choices.Add("D3DFMT_L8 (50)");
  choices.Add("D3DFMT_A8L8 (51)");
  choices.Add("D3DFMT_A4L4 (52)");

  choices.Add("D3DFMT_V8U8 (60)");
  choices.Add("D3DFMT_L6V5U5 (61)");
  choices.Add("D3DFMT_X8L8V8U8 (62)");
  choices.Add("D3DFMT_Q8W8V8U8 (63)");
  choices.Add("D3DFMT_V16U16 (64)");
  choices.Add("D3DFMT_A2W10V10U10 (67)");

  choices.Add("D3DFMT_D16_LOCKABLE (70)");
  choices.Add("D3DFMT_D32 (71)");
  choices.Add("D3DFMT_D15S1 (73)");
  choices.Add("D3DFMT_D24S8 (75)");
  choices.Add("D3DFMT_D24X8 (77)");
  choices.Add("D3DFMT_D24X4S4 (79)");
  choices.Add("D3DFMT_D16 (80)");

  choices.Add("D3DFMT_D32F_LOCKABLE (82)");
  choices.Add("D3DFMT_D24FS8 (83)");

  choices.Add("D3DFMT_D32_LOCKABLE (84)");
  choices.Add("D3DFMT_S8_LOCKABLE (85)");

  choices.Add("D3DFMT_L16 (81)");

  choices.Add("D3DFMT_VERTEXDATA (100)");
  choices.Add("D3DFMT_INDEX16 (101)");
  choices.Add("D3DFMT_INDEX32 (102)");

  choices.Add("D3DFMT_Q16W16V16U16 (110)");


  choices.Add("D3DFMT_R16F (111)");
  choices.Add("D3DFMT_G16R16F (112)");
  choices.Add("D3DFMT_A16B16G16R16F (113)");

  choices.Add("D3DFMT_R32F (114)");
  choices.Add("D3DFMT_G32R32F (115)");
  choices.Add("D3DFMT_A32B32G32R32F (116)");

  choices.Add("D3DFMT_CxV8U8 (117)");

  choices.Add("D3DFMT_A1 (118)");
  choices.Add("D3DFMT_A2B10G10R10_XR_BIAS (119)");
  choices.Add("D3DFMT_BINARYBUFFER (199)");

#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

  wxString temp;
  temp << "D3DFMT_DXT1 (" << MAKEFOURCC('D', 'X', 'T', '1') << ")";
  choices.Add(temp);
  temp.Empty();

  temp << "D3DFMT_DXT2 (" << MAKEFOURCC('D', 'X', 'T', '2') << ")";
  choices.Add(temp);
  temp.Empty();

  temp << "D3DFMT_DXT3 (" << MAKEFOURCC('D', 'X', 'T', '3') << ")";
  choices.Add(temp);
  temp.Empty();

  temp << "D3DFMT_DXT4 (" << MAKEFOURCC('D', 'X', 'T', '4') << ")";
  choices.Add(temp);
  temp.Empty();

  temp << "D3DFMT_DXT5 (" << MAKEFOURCC('D', 'X', 'T', '5') << ")";
  choices.Add(temp);
  temp.Empty();

  temp << "D3DFMT_G8R8_G8B8 (" << MAKEFOURCC('G', 'R', 'G', 'B') << ")";
  choices.Add(temp);
  temp.Empty();

  temp << "D3DFMT_R8G8_B8G8 (" << MAKEFOURCC('R', 'G', 'B', 'G') << ")";
  choices.Add(temp);
  temp.Empty();

  temp << "D3DFMT_MULTI2_ARGB8 (" << MAKEFOURCC('M','E','T','1') << ")";
  choices.Add(temp);
  temp.Empty();

  temp << "D3DFMT_UYVY (" << MAKEFOURCC('U', 'Y', 'V', 'Y') << ")";
  choices.Add(temp);
  temp.Empty();

  temp << "D3DFMT_YUY2 (" << MAKEFOURCC('Y', 'U', 'Y', '2') << ")";
  choices.Add(temp);
  temp.Empty();
  /*
  choices.Add("D3DFMT_DXT1 (827611204)"); 31545844
  choices.Add("D3DFMT_DXT2 (844388420)"); 32545844
  choices.Add("D3DFMT_DXT3 (861165636"); 33545844
  choices.Add("D3DFMT_DXT4 (877942852)"); 34545844
  choices.Add("D3DFMT_DXT5 (894720068)"); 35545844
  choices.Add("D3DFMT_G8R8_G8B8 (MAKEFOURCC('G', 'R', 'G', 'B')");
  choices.Add("D3DFMT_MULTI2_ARGB8 (MAKEFOURCC('M','E','T','1')"); 31
  choices.Add("D3DFMT_R8G8_B8G8 (MAKEFOURCC('R', 'G', 'B', 'G')");
  choices.Add("D3DFMT_UYVY (MAKEFOURCC('U', 'Y', 'V', 'Y')");
  choices.Add("D3DFMT_YUY2 (MAKEFOURCC('Y', 'U', 'Y', '2')");
*/
/*
  choices.Add("R8G8B8");
  choices.Add("A8R8G8B8");
  choices.Add("X8R8G8B8");
  choices.Add("R5G6B");
  choices.Add("X1R5G5B5");
  choices.Add("A1R5G5B5");
  choices.Add("A4R4G4B4");
  choices.Add("R3G3B2");
  choices.Add("A8");
  choices.Add("A8R3G3B2");
  choices.Add("X4R4G4B4");
  choices.Add("A2B10G10R10");
  choices.Add("A8B8G8R8");
  choices.Add("X8B8G8R8");
  choices.Add("G16R16");
  choices.Add("A2R10G10B10");
  choices.Add("A16B16G16R16");

  choices.Add("A8P8");
  choices.Add("P8");

  choices.Add("L8");
  choices.Add("A8L8");
  choices.Add("A4L4");

  choices.Add("V8U8");
  choices.Add("L6V5U5");
  choices.Add("X8L8V8U8");
  choices.Add("Q8W8V8U8");
  choices.Add("V16U16");
  choices.Add("A2W10V10U10");

  choices.Add("UYVY");
  choices.Add("R8G8_B8G8");
  choices.Add("YUY2");
  choices.Add("G8R8_G8B8");
  choices.Add("DXT1");
  choices.Add("DXT2");
  choices.Add("DXT3");
  choices.Add("DXT4");
  choices.Add("DXT5");

  choices.Add("D16_LOCKABLE");
  choices.Add("D32");
  choices.Add("D15S1");
  choices.Add("D24S8");
  choices.Add("D24X8");
  choices.Add("D24X4S4");
  choices.Add("D16");

  choices.Add("D32F_LOCKABLE");
  choices.Add("D24FS8");

  choices.Add("D32_LOCKABLE");
  choices.Add("S8_LOCKABLE");

  choices.Add("L16");

  choices.Add("VERTEXDATA");
  choices.Add("INDEX16");
  choices.Add("INDEX32");

  choices.Add("Q16W16V16U16");

  choices.Add("MULTI2_ARGB8");

  choices.Add("R16F");
  choices.Add("G16R16F");
  choices.Add("A16B16G16R16F");

  choices.Add("R32F");
  choices.Add("G32R32F");
  choices.Add("A32B32G32R32F");

  choices.Add("CxV8U8");

  choices.Add("A1");
  choices.Add("A2B10G10R10_XR_BIAS");
  choices.Add("BINARYBUFFER");
*/
  wxArrayInt selections;
  unsigned long selected = Game.FormatFilter();

  for (unsigned int i=0; i<choices.GetCount(); i++)
  {
    if (selected & 1ull<<i) selections.Add(i);
  }
  if (wxGetSelectedChoices( selections, Language->DeleteGame, Language->DeleteGame, choices)!=-1)
  {
    selected = 0u;
    for (unsigned int i=0; i<selections.GetCount(); i++)
    {
      selected |= 1ull << selections[i];
    }
    Game.FormatFilter() = selected;
  }

}

void uMod_GamePage::OnButtonSavePath(wxCommandEvent& WXUNUSED(event))
{
  wxString dir = wxDirSelector( Language->ChooseDir, Game.GetSavePath());
  if ( !dir.empty() )
  {
    Game.SetSavePath(dir);

    wxString path;
    path = Language->TextCtrlSavePath;
    path << dir;
    SavePath->SetValue( path);
  }
}

void uMod_GamePage::OnBeginDrag( wxDataViewEvent &event)
{
  uMod_TreeViewNode *node = (uMod_TreeViewNode*) event.GetItem().GetID();
  // no item -> no drag and drop
  if (node == (uMod_TreeViewNode*)0)
  {
    event.Veto();
    return;
  }
  // drag and drop only for root nodes
  else if (node->GetParent() != (uMod_TreeViewNode*) 0 )
  {
    event.Veto();
    return;
  }

  wxString index;
  index << ViewModel->myPackages.Index(node);
  wxTextDataObject *obj = new wxTextDataObject;
  obj->SetText( index );
  event.SetDataObject( obj );
}

void uMod_GamePage::OnDropPossible( wxDataViewEvent &event)
{
  // not our data format
  if (event.GetDataFormat()!=wxDF_UNICODETEXT)
  {
    event.Veto();
    return;
  }

  uMod_TreeViewNode *node = (uMod_TreeViewNode*) event.GetItem().GetID();
  // drag and drop only on root nodes
  if (node!=(uMod_TreeViewNode*)0 && node->GetParent() != (uMod_TreeViewNode*) 0 )
  {
    event.Veto();
    return;
  }
}

void uMod_GamePage::OnDrop( wxDataViewEvent &event)
{
  // not our data format
  if (event.GetDataFormat()!=wxDF_UNICODETEXT)
  {
    event.Veto();
    return;
  }

  uMod_TreeViewNode *node = (uMod_TreeViewNode*) event.GetItem().GetID();
  // drag and drop only on root nodes
  if (node!=(uMod_TreeViewNode*)0 && node->GetParent() != (uMod_TreeViewNode*) 0 )
  {
    event.Veto();
    return;
  }

  wxTextDataObject obj;
  obj.SetData( wxDF_UNICODETEXT, event.GetDataSize(), event.GetDataBuffer() );
  wxString msg = obj.GetText();

  //if we cannot convert the message into a number
  long int drag_index;
  if (!msg.ToLong( &drag_index))
  {
    wxMessageBox("ToLong");
    event.Veto();
    return;
  }

  // wrong index !!
  if (drag_index<0 || drag_index>=(int) ViewModel->myPackages.GetCount())
  {
    wxMessageBox("outside range");
    event.Veto();
    return;
  }

  long int drop_index = ViewModel->myPackages.Index( node);

  // if this happens somehting went terrible wrong
  if (drop_index==wxNOT_FOUND)
  {
    event.Veto();
    return;
  }

  // get the draged node
  uMod_TreeViewNode *draged_node =  ViewModel->myPackages.Item(drag_index);

  //remove the draged node
  ViewModel->myPackages.RemoveAt(drag_index);

  //no insert again at the drop index
  ViewModel->myPackages.Insert(draged_node,drop_index);

  // set the model to cleared -> it rereads all data -> changes become effect
  ViewModel->Cleared();
}


void uMod_GamePage::OnDropFile(wxDropFilesEvent& event)
{
  wxString *files = event.GetFiles();
  int num = event.GetNumberOfFiles();

  LastError.Empty();

  AddPackages(files, num);

  if (!LastError.IsEmpty())
  {
    wxMessageBox(LastError, "ERROR", wxOK|wxICON_ERROR);
    LastError.Empty();
  }
  if (num>0) UpdateGame();
}

void uMod_GamePage::OnContextMenu( wxDataViewEvent &event )
{
  LastError.Empty();
  uMod_TreeViewNode *node = (uMod_TreeViewNode*) event.GetItem().GetID();

  wxMenu menu;
  menu.Append( ID_OpenPackage, Language->MenuOpenPackage);
  menu.Append( ID_RemovePackage, Language->MenuRemovePackage);
  menu.Append( ID_RemoveSelectedPackages, Language->MenuRemoveSelectedPackages);
  menu.Append( ID_Update, Language->MenuUpdate);
  menu.Append( ID_Reload, Language->MenuReload);
  menu.AppendCheckItem( ID_SupportTPF, Language->MenuSupportTPF);
  menu.Check(ID_SupportTPF, Game.SupportTPF());

  // if no node is selected, there is nothing to delete ^^
  if (node ==  (uMod_TreeViewNode*) 0)
    menu.Enable(ID_RemovePackage, false);
  // only allow root-nodes to be deleted
  else if (node->GetParent() != (uMod_TreeViewNode*) 0 )
    menu.Enable(ID_RemovePackage, false);

  // if nothing is selected, we cannot deleted selected packages ;)
  if (ViewCtrl->GetSelectedItemsCount()<=0)
    menu.Enable(ID_RemoveSelectedPackages, false);

  int id = ViewCtrl->GetPopupMenuSelectionFromUser(menu);
  switch (id)
  {
  case ID_OpenPackage:
    OpenPackage();
    break;
  case ID_RemovePackage:
    ViewModel->DeletePackage( event.GetItem());
    break;
  case ID_RemoveSelectedPackages:
    {
      wxDataViewItemArray items;
      ViewCtrl->GetSelections(items);
      ViewModel->DeletePackages( items);
    }
    break;
  case ID_Update:
    UpdateGame();
    break;
  case ID_Reload:
    ReloadGame();
    break;
  case ID_SupportTPF:
    Game.SupportTPF() = menu.IsChecked(ID_SupportTPF);
    break;
  default:
      break;
  }

  if (!LastError.IsEmpty())
  {
    wxMessageBox(LastError, "ERROR", wxOK|wxICON_ERROR);
    LastError.Empty();
  }
}


int uMod_GamePage::OpenPackage(void)
{
  wxFileDialog *dialog = new wxFileDialog(this, Language->ChooseFile, Game.GetOpenPath(), wxEmptyString,
      wxFileSelectorDefaultWildcardStr, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

  if (dialog->ShowModal() == wxID_OK)
  {
    wxMessageBox("here");
    wxArrayString files;
    dialog->GetPaths( files); // get the filenames + the path

    if (files.GetCount()>0)
    {
      wxMessageBox("add");
      AddPackages(files);
      if (!LastError.IsEmpty())
      {
        wxMessageBox(LastError, "ERROR", wxOK|wxICON_ERROR);
        LastError.Empty();
      }
      UpdateGame();
    }
  }
  return 0;
}


int uMod_GamePage::UpdateLanguage(void)
{
  CollPane->SetLabel(Language->CollapseTextureCapture);

  SaveSingleTexture->SetLabel( Language->CheckBoxSaveSingleTexture);
  ShowSingleTextureString->SetLabel( Language->CheckBoxShowStringSaveSingleTexture);

  KeyBack->SetLabel( Language->KeyBack);
  KeySave->SetLabel( Language->KeySave);
  KeyNext->SetLabel( Language->KeyNext);
  FontColour->SetLabel( Language->FontColour);
  TextureColour->SetLabel( Language->TextureColour);

  SpinWidth->SetLabel( Language->WidthSpin);
  SpinHeight->SetLabel( Language->HeightSpin);
  SpinDepth->SetLabel( Language->DepthSpin);

  SaveAllTextures->SetLabel( Language->CheckBoxSaveAllTextures);
  wxString temp = Language->TextCtrlSavePath;
  temp << Game.GetSavePath();
  SavePath->SetValue( temp);
  return 0;
}

