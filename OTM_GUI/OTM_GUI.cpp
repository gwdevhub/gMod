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





#ifndef __CDT_PARSER__

DEFINE_EVENT_TYPE(OTM_EVENT_TYPE)

BEGIN_EVENT_TABLE(OTM_Frame, wxFrame)
  EVT_CLOSE(OTM_Frame::OnClose)

  EVT_BUTTON(ID_Button_Open, OTM_Frame::OnButtonOpen)
  EVT_BUTTON(ID_Button_Path, OTM_Frame::OnButtonPath)
  EVT_BUTTON(ID_Button_Update, OTM_Frame::OnButtonUpdate)
  EVT_BUTTON(ID_Button_Reload, OTM_Frame::OnButtonReload)

  EVT_MENU(ID_Menu_Help, OTM_Frame::OnMenuHelp)
  EVT_MENU(ID_Menu_About, OTM_Frame::OnMenuAbout)
  EVT_MENU(ID_Menu_Acknowledgement, OTM_Frame::OnMenuAcknowledgement)


  EVT_MENU(ID_Menu_AddGame, OTM_Frame::OnMenuAddGame)
  EVT_MENU(ID_Menu_DeleteGame, OTM_Frame::OnMenuDeleteGame)

  EVT_MENU(ID_Menu_LoadTemplate, OTM_Frame::OnMenuOpenTemplate)
  EVT_MENU(ID_Menu_SaveTemplate, OTM_Frame::OnMenuSaveTemplate)
  EVT_MENU(ID_Menu_SaveTemplateAs, OTM_Frame::OnMenuSaveTemplateAs)
  EVT_MENU(ID_Menu_SetDefaultTemplate, OTM_Frame::OnMenuSetDefaultTemplate)

  EVT_MENU(ID_Menu_Lang, OTM_Frame::OnMenuLanguage)
  EVT_MENU(ID_Menu_Exit, OTM_Frame::OnMenuExit)

  EVT_COMMAND  (ID_Add_Game, OTM_EVENT_TYPE, OTM_Frame::OnAddGame)
  EVT_COMMAND  (ID_Delete_Game, OTM_EVENT_TYPE, OTM_Frame::OnDeleteGame)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)
#endif

MyApp::~MyApp(void)
{
  if (CheckForSingleRun!=NULL) CloseHandle( CheckForSingleRun);
}


bool MyApp::OnInit(void)
{
  OTM_Settings set;
  set.Load();

  Language = new OTM_Language(set.Language);
  CheckForSingleRun = CreateMutex( NULL, true, L"Global\\OTM_CheckForSingleRun");
  if (ERROR_ALREADY_EXISTS == GetLastError())
  {
    wxMessageBox( Language->Error_AlreadyRunning, "ERROR", wxOK|wxICON_ERROR);
    return false;
  }
  OTM_Frame *frame = new OTM_Frame( OTM_VERSION, wxPoint(set.XPos,set.YPos), wxSize(set.XSize,set.YSize));
  SetTopWindow( frame );

  return true;
}


OTM_Frame::OTM_Frame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  SetIcon(wxICON(MAINICON));

  Server = new OTM_Server( this);
  Server->Create();
  Server->Run();

  MenuBar = new wxMenuBar;
  //MenuMain = new wxMenu;
  MenuMain = new wxMenu;
  MenuHelp = new wxMenu;


  MenuMain->Append( ID_Menu_AddGame, Language->MenuAddGame );
  MenuMain->Append( ID_Menu_DeleteGame, Language->MenuDeleteGame );
  MenuMain->AppendSeparator();
  MenuMain->Append( ID_Menu_LoadTemplate, Language->MenuLoadTemplate );
  MenuMain->Append( ID_Menu_SaveTemplate, Language->MenuSaveTemplate );
  MenuMain->Append( ID_Menu_SaveTemplateAs, Language->MenuSaveTemplateAs );
  MenuMain->Append( ID_Menu_SetDefaultTemplate, Language->MenuSetDefaultTemplate );
  MenuMain->AppendSeparator();
  MenuMain->Append( ID_Menu_Lang, Language->MenuLanguage );
  MenuMain->Append( ID_Menu_Exit, Language->MenuExit );

  MenuHelp->Append( ID_Menu_Help, Language->MenuHelp );
  MenuHelp->Append( ID_Menu_About, Language->MenuAbout );
  MenuHelp->Append( ID_Menu_Acknowledgement, Language->MenuAcknowledgement );

  MenuBar->Append( MenuMain, Language->MainMenuMain );
  MenuBar->Append( MenuHelp, Language->MainMenuHelp );

  SetMenuBar(MenuBar);


  MainSizer = new wxBoxSizer(wxVERTICAL);

  Notebook = new wxNotebook( this, wxID_ANY);
  Notebook->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
  MainSizer->Add( (wxWindow*) Notebook, 1, wxEXPAND , 0 );

  ButtonSizer = new wxBoxSizer(wxHORIZONTAL);

  OpenButton = new wxButton( this, ID_Button_Open, Language->ButtonOpen, wxDefaultPosition, wxSize(100,24));
  DirectoryButton = new wxButton( this, ID_Button_Path, Language->ButtonDirectory, wxDefaultPosition, wxSize(100,24));
  UpdateButton = new wxButton( this, ID_Button_Update, Language->ButtonUpdate, wxDefaultPosition, wxSize(100,24));
  ReloadButton = new wxButton( this, ID_Button_Reload, Language->ButtonReload, wxDefaultPosition, wxSize(100,24));

  ButtonSizer->Add( (wxWindow*) OpenButton, 1, wxEXPAND, 0);
  ButtonSizer->Add( (wxWindow*) DirectoryButton, 1, wxEXPAND, 0);
  ButtonSizer->Add( (wxWindow*) UpdateButton, 1, wxEXPAND, 0);
  ButtonSizer->Add( (wxWindow*) ReloadButton, 1, wxEXPAND, 0);
  MainSizer->Add( ButtonSizer, 0, wxEXPAND , 0 );


  SetSizer( MainSizer);

  NumberOfGames = 0;
  MaxNumberOfGames  = 10;
  Clients = NULL;
  if (GetMemory( Clients, MaxNumberOfGames))
  {
    wxMessageBox( Language->Error_Memory, "ERROR", wxOK|wxICON_ERROR);
  }
  LoadTemplate();

  Show( true );

  H_DX9_DLL = LoadLibraryW(OTM_d3d9_dll);
  if (H_DX9_DLL!=NULL)
  {
    typedef void (*fkt_typ)(void);
    fkt_typ InstallHook = (fkt_typ) GetProcAddress( H_DX9_DLL, "InstallHook");
    if (InstallHook!=NULL) InstallHook();
    else
    {
      DWORD error = GetLastError();
      wchar_t *error_msg;
      FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
              NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &error_msg, 0, NULL );
      wxString temp = Language->Error_DLLNotFound;
      temp << "\n" << OTM_d3d9_dll;
      temp << "\n" << error_msg << "Code: " << error;
      wxMessageBox( temp, "ERROR", wxOK);
    }
  }
  else
  {
    DWORD error = GetLastError();
    wchar_t *error_msg;
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &error_msg, 0, NULL );
    wxString temp = Language->Error_DLLNotFound;
    temp << "\n" << OTM_d3d9_dll;
    temp << "\n" << error_msg << "Code: " << error;
    wxMessageBox(temp, "ERROR", wxOK|wxICON_ERROR);
  }
}

OTM_Frame::~OTM_Frame(void)
{
  if (Server!=(OTM_Server*)0)
  {
    KillServer();
    Server->Wait();
    delete Server;
    Server = NULL;
  }

  if (H_DX9_DLL!=NULL)
  {
    typedef void (*fkt_typ)(void);
    fkt_typ RemoveHook = (fkt_typ) GetProcAddress( H_DX9_DLL, "RemoveHook");
    if (RemoveHook!=NULL) RemoveHook();
    FreeLibrary(H_DX9_DLL);
  }

  if (Clients!=NULL) delete [] Clients;

  OTM_Settings set;
  set.Language = Language->GetCurrentLanguage();
  GetSize( &set.XSize, &set.YSize);
  GetPosition( &set.XPos, &set.YPos);
  set.Save();
}

int OTM_Frame::KillServer(void)
{
  HANDLE pipe = CreateFileW( PIPE_Game2OTM,// pipe name
                 GENERIC_WRITE,
                 0,              // no sharing
                 NULL,           // default security attributes
                 OPEN_EXISTING,  // opens existing pipe
                 0,              // default attributes
                 NULL);          // no template file

  if (pipe == INVALID_HANDLE_VALUE) return -1;

  const wchar_t *str = ABORT_SERVER;
  unsigned int len=0u;
  while (str[len]) len++;
  len++; //to send also the zero
  unsigned long num;
  WriteFile( pipe, (const void*) str, len*sizeof(wchar_t), &num, NULL);
  CloseHandle(pipe);
  return 0;
}



void OTM_Frame::OnAddGame( wxCommandEvent &event)
{
  if (NumberOfGames>=MaxNumberOfGames)
  {
    if (GetMoreMemory( Clients, MaxNumberOfGames, MaxNumberOfGames+10))
    {
      wxMessageBox( Language->Error_Memory, "ERROR", wxOK|wxICON_ERROR);
      return;
    }
    MaxNumberOfGames += 10;
  }

  wxString name = ((OTM_Event&)event).GetName();
  PipeStruct pipe;

  pipe.In = ((OTM_Event&)event).GetPipeIn();
  pipe.Out = ((OTM_Event&)event).GetPipeOut();

  OTM_Client *client = new OTM_Client( pipe, this);
  client->Create();
  client->Run();

  wxString save_file;
  int num = SaveFile_Exe.GetCount();
  for (int i=0; i<num; i++) if (name==SaveFile_Exe[i])
  {
    save_file = SaveFile_Name[i];
    break;
  }

  OTM_GamePage *page = new OTM_GamePage( Notebook, name, save_file, client->Pipe);
  if (page->LastError.Len()>0)
  {
    wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
    delete page;
    return;
  }
  name = name.AfterLast('\\');
  name = name.AfterLast('/');
  name = name.BeforeLast('.');
  Notebook->AddPage( page, name, true);

  Clients[NumberOfGames] = client;
  NumberOfGames++;
}

void OTM_Frame::OnDeleteGame( wxCommandEvent &event)
{
  OTM_Client *client = ((OTM_Event&)event).GetClient();
  for (int i=0; i<NumberOfGames; i++) if (Clients[i]==client)
  {
    Notebook->DeletePage(i);
    Clients[i]->Wait();
    delete Clients[i];
    NumberOfGames--;
    for (int j=i; j<NumberOfGames; j++) Clients[j] = Clients[j+1];
    return;
  }
}


void OTM_Frame::OnClose(wxCloseEvent& event)
{
  if (event.CanVeto() && NumberOfGames>0)
  {
    if (wxMessageBox(Language->ExitGameAnyway, "ERROR", wxYES_NO|wxICON_ERROR)!=wxYES) {event.Veto(); return;}
  }
  event.Skip();
  Destroy();
}

void OTM_Frame::OnButtonOpen(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;


  //wxString file_name = wxFileSelector( Language->ChooseFile, page->GetOpenPath(), "", "*.*",  "textures (*.dds)|*.dds|zip (*.zip)|*.zip|tpf (*.tpf)|*.tpf", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  wxString file_name = wxFileSelector( Language->ChooseFile, page->GetOpenPath(), "", "",  "", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if ( !file_name.empty() )
  {
    page->SetOpenPath(file_name.BeforeLast( '/'));
    if (page->AddTexture( file_name))
    {
      wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
      page->LastError.Empty();
    }
  }
}

void OTM_Frame::OnButtonPath(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;

  wxString dir = wxDirSelector( Language->ChooseDir, page->GetSavePath());
  if ( !dir.empty() )
  {
    page->SetSavePath( dir);
  }
}

void OTM_Frame::OnButtonUpdate(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;
  if (page->UpdateGame())
  {
    wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
    page->LastError.Empty();
  }
}

void OTM_Frame::OnButtonReload(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;
  if (page->ReloadGame())
  {
    wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
    page->LastError.Empty();
  }
}




void OTM_Frame::OnMenuOpenTemplate(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;


  //wxString file_name = wxFileSelector( Language->ChooseFile, page->GetOpenPath(), "", "*.*",  "textures (*.dds)|*.dds|zip (*.zip)|*.zip|tpf (*.tpf)|*.tpf", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

  wxString dir = wxGetCwd();
  dir << "/templates";
  wxString file_name = wxFileSelector( Language->ChooseFile, dir, "", "*.txt",  "text (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if ( !file_name.empty() )
  {
    if (page->LoadTemplate( file_name))
    {
      wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
      page->LastError.Empty();
    }
  }
}

void OTM_Frame::OnMenuSaveTemplate(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;

  wxString file_name = page->GetTemplateName();

  if ( file_name.empty() )
  {
    wxString dir = wxGetCwd();
    dir << "/templates";
    file_name = wxFileSelector( Language->ChooseFile, dir, "", "*.txt",  "text (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);
  }
  if ( !file_name.empty() )
  {
    if (page->SaveTemplate(file_name))
    {
      wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
      page->LastError.Empty();
    }
  }
}

void OTM_Frame::OnMenuSaveTemplateAs(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;


  wxString dir = wxGetCwd();
  dir << "/templates";
  wxString file_name = wxFileSelector( Language->ChooseFile, dir, "", "*.txt",  "text (*.txt)|*.txt", wxFD_SAVE | wxFD_OVERWRITE_PROMPT, this);
  if ( !file_name.empty() )
  {
    if (page->SaveTemplate(file_name))
    {
      wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
      page->LastError.Empty();
    }
  }
}

void OTM_Frame::OnMenuSetDefaultTemplate(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;

  wxString exe = page->GetExeName();
  wxString file = page->GetTemplateName();

  int num = SaveFile_Exe.GetCount();
  bool hit = false;
  for (int i=0; i<num; i++) if (SaveFile_Exe[i]==exe)
  {
    SaveFile_Name[i] = file;
    hit = true;
    break;
  }
  if (!hit)
  {
    SaveFile_Exe.Add(exe);
    SaveFile_Name.Add(file);
  }
  if (SaveTemplate())
  {
    wxMessageBox(LastError, "ERROR", wxOK|wxICON_ERROR);
    LastError.Empty();
  }
}

void OTM_Frame::OnMenuLanguage(wxCommandEvent& WXUNUSED(event))
{
  wxArrayString lang;
  Language->GetLanguages( lang);
  wxString choice = wxGetSingleChoice( Language->SelectLanguage, Language->SelectLanguage, lang);
  if (choice.Len()>0)
  {
    if (Language->LoadLanguage(choice))
    {
      wxMessageBox(Language->LastError, "ERROR", wxOK|wxICON_ERROR);
      Language->LastError.Empty();
      return;
    }
    MenuBar->SetMenuLabel( 0, Language->MainMenuMain);
    MenuMain->SetLabel( ID_Menu_AddGame, Language->MenuAddGame);
    MenuMain->SetLabel( ID_Menu_DeleteGame, Language->MenuDeleteGame);

    MenuMain->SetLabel( ID_Menu_LoadTemplate, Language->MenuLoadTemplate );
    MenuMain->SetLabel( ID_Menu_SaveTemplate, Language->MenuSaveTemplate );
    MenuMain->SetLabel( ID_Menu_SaveTemplateAs, Language->MenuSaveTemplateAs );
    MenuMain->SetLabel( ID_Menu_SetDefaultTemplate, Language->MenuSetDefaultTemplate );

    MenuMain->SetLabel( ID_Menu_Lang, Language->MenuLanguage);
    MenuMain->SetLabel( ID_Menu_Exit, Language->MenuExit );

    MenuBar->SetMenuLabel( 1, Language->MainMenuHelp);
    MenuHelp->SetLabel( ID_Menu_Help, Language->MenuHelp);
    MenuHelp->SetLabel( ID_Menu_About, Language->MenuAbout);
    MenuHelp->SetLabel( ID_Menu_Acknowledgement, Language->MenuAcknowledgement);


    OpenButton->SetLabel( Language->ButtonOpen);
    DirectoryButton->SetLabel( Language->ButtonDirectory);
    UpdateButton->SetLabel( Language->ButtonUpdate);
    ReloadButton->SetLabel( Language->ButtonReload);

    int num = Notebook->GetPageCount();
    for (int i=0; i<num; i++)
    {
      OTM_GamePage* page = (OTM_GamePage*) Notebook->GetPage(i);
      page->UpdateLanguage();
    }
  }
}

void OTM_Frame::OnMenuExit(wxCommandEvent& WXUNUSED(event))
{
  Close();
}

void OTM_Frame::OnMenuHelp(wxCommandEvent& WXUNUSED(event))
{
  wxString help;
  if (Language->GetHelpMessage( help))
  {
    wxMessageBox(Language->LastError, "ERROR", wxOK|wxICON_ERROR);
    Language->LastError.Empty();
    return;
  }

  wxMessageBox( help, Language->MenuHelp, wxOK);
}

void OTM_Frame::OnMenuAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString msg;
  msg << OTM_VERSION << " by ROTA\nhttp://code.google.com/p/texmod/";
  wxMessageBox( msg, "Info", wxOK);
}

void OTM_Frame::OnMenuAcknowledgement(wxCommandEvent& WXUNUSED(event))
{
  wxString title;
  title << OTM_VERSION << " by ROTA";
  wxString msg;
  msg << "RS for coding the original TexMod and for information about the used hashing algorithm\n";
  msg << "EvilAlex for translation into Russian and bug fixing";
  wxMessageBox( msg, title, wxOK);
}


void OTM_Frame::OnMenuAddGame(wxCommandEvent& WXUNUSED(event))
{
  wxString file_name = wxFileSelector( Language->ChooseGame, "", "", "exe",  "binary (*.exe)|*.exe", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if ( !file_name.empty() )
  {
    wxArrayString array;
    if (GetHookedGames( array)) array.Empty();

    int num = array.GetCount();
    for (int i=0; i<num; i++) if (array[i] == file_name)
    {
      wxMessageBox(Language->GameAlreadyAdded, "ERROR", wxOK|wxICON_ERROR);
      return;
    }
    array.Add(file_name);
    if (SetHookedGames( array))
    {
      wxMessageBox( LastError, "ERROR", wxOK|wxICON_ERROR);
      LastError.Empty();
      return;
    }
  }
}

void OTM_Frame::OnMenuDeleteGame(wxCommandEvent& WXUNUSED(event))
{
  wxArrayInt selections;
  wxArrayString array;
  if (GetHookedGames( array))
  {
    wxMessageBox( LastError, "ERROR", wxOK|wxICON_ERROR);
    LastError.Empty();
    return;
  }
  wxGetSelectedChoices( selections, Language->DeleteGame, Language->DeleteGame, array);

  int num = selections.GetCount();
  for (int i=0; i<num; i++)
  {
    array.RemoveAt( selections[i]-i); //this will work only if selections is sorted !!
  }

  if (SetHookedGames( array))
  {
    wxMessageBox( LastError, "ERROR", wxOK|wxICON_ERROR);
    LastError.Empty();
    return;
  }
}



int OTM_Frame::GetHookedGames( wxArrayString &array)
{
  wxFile file;
  wxString name;
  wchar_t *app_path = _wgetenv( L"APPDATA");
  name.Printf("%ls\\%ls\\%ls", app_path, OTM_APP_DIR, OTM_APP_DX9);

  if (!file.Access(name, wxFile::read)) {LastError << Language->Error_FileOpen << "\n" << name; return -1;}
  file.Open(name, wxFile::read);
  if (!file.IsOpened()) {LastError << Language->Error_FileOpen << "\n" << name ; return -1;}

  unsigned len = file.Length();

  unsigned char* buffer;
  try {buffer = new unsigned char [len+2];}
  catch (...) {LastError << Language->Error_Memory; return -1;}

  unsigned int result = file.Read( buffer, len);
  file.Close();

  if (result != len) {delete [] buffer; LastError << Language->Error_FileRead<<"\n" << name; return -1;}

  wchar_t *buff = (wchar_t*)buffer;
  len/=2;
  buff[len]=0;

  wxString content;
  content =  buff;
  delete [] buffer;

  wxStringTokenizer token( content, "\n");

  int num = token.CountTokens();

  array.Empty();

  for (int i=0; i<num; i++)
  {
    array.Add( token.GetNextToken());
  }
  return 0;
}

int OTM_Frame::SetHookedGames( const wxArrayString &array)
{
  wxFile file;
  wxString name;
  wchar_t *app_path = _wgetenv( L"APPDATA");
  name.Printf("%ls\\%ls", app_path, OTM_APP_DIR);

  if (! wxDir::Exists(name))
  {
    wxDir::Make(name);
  }

  name.Printf("%ls\\%ls\\%ls", app_path, OTM_APP_DIR, OTM_APP_DX9);
  file.Open(name, wxFile::write);
  if (!file.IsOpened()) {LastError << Language->Error_FileOpen << "\n" << name ; return -1;}
  wxString content;

  int num = array.GetCount();
  for (int i=0; i<num; i++)
  {
    content = array[i];
    content << "\n";
    file.Write( content.wc_str(), content.Len()*2);
  }
  file.Close();
  return 0;
}

#define SAVE_FILE "OTM_SaveFiles.txt"

int OTM_Frame::LoadTemplate(void)
{
  wxFile file;
  if (!file.Access(SAVE_FILE, wxFile::read)) {LastError << Language->Error_FileOpen << "\n" << SAVE_FILE; return -1;}
  file.Open(SAVE_FILE, wxFile::read);
  if (!file.IsOpened()) {LastError << Language->Error_FileOpen << "\n" << SAVE_FILE ; return -1;}

  unsigned len = file.Length();

  unsigned char* buffer;
  try {buffer = new unsigned char [len+2];}
  catch (...) {LastError << Language->Error_Memory; return -1;}

  unsigned int result = file.Read( buffer, len);
  file.Close();

  if (result != len) {delete [] buffer; LastError << Language->Error_FileRead<<"\n" << SAVE_FILE; return -1;}

  wchar_t *buff = (wchar_t*)buffer;
  len/=2;
  buff[len]=0;

  wxString content;
  content =  buff;
  delete [] buffer;

  wxStringTokenizer token( content, "\n");

  int num = token.CountTokens();

  SaveFile_Exe.Empty();
  SaveFile_Exe.Alloc(num+10);
  SaveFile_Name.Empty();
  SaveFile_Name.Alloc(num+10);

  wxString line;
  wxString exe;
  wxString name;
  for (int i=0; i<num; i++)
  {
    line = token.GetNextToken();
    exe = line.BeforeFirst('|');
    name = line.AfterFirst('|');
    name.Replace("\r","");
    SaveFile_Exe.Add( exe);
    SaveFile_Name.Add( name);
  }
  return 0;
}

int OTM_Frame::SaveTemplate(void)
{
  wxFile file;
  file.Open(SAVE_FILE, wxFile::write);
  if (!file.IsOpened()) {LastError << Language->Error_FileOpen << "\n" << SAVE_FILE ; return -1;}
  wxString content;

  int num = SaveFile_Exe.GetCount();
  for (int i=0; i<num; i++)
  {
    content = SaveFile_Exe[i];
    content << "|" << SaveFile_Name[i] << "\n";
    file.Write( content.wc_str(), content.Len()*2);
  }
  file.Close();
  return 0;
}
