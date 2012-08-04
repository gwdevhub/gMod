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

DEFINE_EVENT_TYPE(uMod_EVENT_TYPE)

BEGIN_EVENT_TABLE(uMod_Frame, wxFrame)
  EVT_CLOSE(uMod_Frame::OnClose)


  EVT_MENU(ID_Menu_Help, uMod_Frame::OnMenuHelp)
  EVT_MENU(ID_Menu_About, uMod_Frame::OnMenuAbout)
  EVT_MENU(ID_Menu_Acknowledgement, uMod_Frame::OnMenuAcknowledgement)


  EVT_MENU(ID_Menu_StartGame, uMod_Frame::OnMenuStartGame)
  EVT_MENU(ID_Menu_StartGameCMD, uMod_Frame::OnMenuStartGame)

  EVT_MENU(ID_Menu_AddGame, uMod_Frame::OnMenuAddGame)
  EVT_MENU(ID_Menu_DeleteGame, uMod_Frame::OnMenuDeleteGame)
  EVT_MENU(ID_Menu_UseHook, uMod_Frame::OnMenuUseHook)

  EVT_MENU(ID_Menu_LoadTemplate, uMod_Frame::OnMenuOpenTemplate)
  EVT_MENU(ID_Menu_SaveTemplate, uMod_Frame::OnMenuSaveTemplate)
  EVT_MENU(ID_Menu_SaveTemplateAs, uMod_Frame::OnMenuSaveTemplate)
  EVT_MENU(ID_Menu_SetDefaultTemplate, uMod_Frame::OnMenuSetDefaultTemplate)

  EVT_MENU(ID_Menu_Lang, uMod_Frame::OnMenuLanguage)
  EVT_MENU(ID_Menu_Exit, uMod_Frame::OnMenuExit)

  EVT_COMMAND  (ID_Add_Game, uMod_EVENT_TYPE, uMod_Frame::OnAddGame)
  EVT_COMMAND  (ID_Delete_Game, uMod_EVENT_TYPE, uMod_Frame::OnDeleteGame)
  EVT_COMMAND  (ID_Add_Device, uMod_EVENT_TYPE, uMod_Frame::OnAddDevice)
  EVT_COMMAND  (ID_Delete_Device, uMod_EVENT_TYPE, uMod_Frame::OnRemoveDevice)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)
#endif

MyApp::~MyApp(void)
{
  if (CheckForSingleRun!=NULL) CloseHandle( CheckForSingleRun);
}


bool MyApp::OnInit(void)
{
  uMod_Settings set;
  set.Load();

  Language = new uMod_Language(set.Language);
  CheckForSingleRun = CreateMutex( NULL, true, L"Global\\uMod_CheckForSingleRun");
  if (ERROR_ALREADY_EXISTS == GetLastError())
  {
    wxMessageBox( Language->Error_AlreadyRunning, "ERROR", wxOK|wxICON_ERROR);
    return false;
  }
  uMod_Frame *frame = new uMod_Frame( uMod_VERSION L"  by  ROTA", set);
  SetTopWindow( frame );

  return true;
}


uMod_Frame::uMod_Frame(const wxString& title, uMod_Settings &set)
       : wxFrame((wxFrame *)NULL, -1, title, wxPoint(set.XPos,set.YPos), wxSize(set.XSize,set.YSize)), Settings(set)
{
  SetIcon(wxICON(MAINICON));
  H_DX9_DLL = NULL;

  Server = new uMod_Server( this);
  Server->Create();
  Server->Run();

  MenuBar = new wxMenuBar;
  //MenuMain = new wxMenu;
  MenuMain = new wxMenu;
  MenuHelp = new wxMenu;

  MenuMain->Append ( ID_Menu_StartGame, Language->MenuStartGame);
  MenuMain->Append ( ID_Menu_StartGameCMD, Language->MenuStartGameCMD);
  MenuMain->AppendSeparator();

  MenuMain->Append( ID_Menu_AddGame, Language->MenuAddGame );
  MenuMain->Append( ID_Menu_DeleteGame, Language->MenuDeleteGame );
  MenuMain->AppendCheckItem ( ID_Menu_UseHook, Language->MenuUseHook);
  MenuMain->Check( ID_Menu_UseHook, Settings.UseHook);

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

  Notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
  Notebook->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
  MainSizer->Add( (wxWindow*) Notebook, 1, wxEXPAND , 0 );

  PipeStruct pipe = {INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE};
  uMod_GamePage *page = new uMod_GamePage( Notebook, "uMod", INVALID_GAME_PAGE, "uMod", pipe);
  if (page->LastError.Len()>0)
  {
    wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
    delete page;
  }
  else Notebook->AddPage( page, page->GetPageName(), true);

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

  {
    HMODULE dll = LoadLibraryW( L"D3DX9_43.dll");
    if (dll==NULL)
    {
      wxMessageBox( Language->Error_D3DX9NotFound, "ERROR", wxOK|wxICON_ERROR);
    }
    else FreeLibrary(dll);
  }

  if (Settings.UseHook) InstallHook();
  DeactivateGamesControl();
}

uMod_Frame::~uMod_Frame(void)
{
  if (Server!=(uMod_Server*)0)
  {
    KillServer();
    Server->Wait();
    delete Server;
    Server = NULL;
  }

  RemoveHook();

  if (Clients!=NULL) delete [] Clients;

  Settings.Language = Language->GetCurrentLanguage();
  GetSize( &Settings.XSize, &Settings.YSize);
  GetPosition( &Settings.XPos, &Settings.YPos);
  Settings.Save();
}

int uMod_Frame::KillServer(void)
{
  HANDLE pipe = CreateFileW( PIPE_Game2uMod,// pipe name
                 GENERIC_WRITE,
                 0,              // no sharing
                 NULL,           // default security attributes
                 OPEN_EXISTING,  // opens existing pipe
                 0,              // default attributes
                 NULL);          // no template file

  if (pipe == INVALID_HANDLE_VALUE) return -1;

  // prepend a zero int value as dummy injection method
  const wchar_t *str = ABORT_SERVER;
  unsigned int len=0u;
  while (str[len]) len++;
  len++; //to send also the zero

  char *buffer=(char*)0;
  GetMemory(buffer, sizeof(int) + len * sizeof(wchar_t));
  *((int*) buffer) = 0;
  char *p_game = (char*) str;

  for (unsigned int i=0; i<len*sizeof(wchar_t); i++) buffer[i+sizeof(int)] = p_game[i];
  unsigned long num;
  //WriteFile( pipe, (const void*) &len, sizeof(int), &num, NULL); //dummy injection method
  WriteFile( pipe, (const void*) buffer, len*sizeof(wchar_t)+sizeof(int), &num, NULL);
  delete [] buffer;
  CloseHandle(pipe);
  return 0;
}



void uMod_Frame::OnAddGame( wxCommandEvent &event)
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

  wxString name = ((uMod_Event&)event).GetName();
  PipeStruct pipe;

  pipe.In = ((uMod_Event&)event).GetPipeIn();
  pipe.Out = ((uMod_Event&)event).GetPipeOut();
  int injection_method = ((uMod_Event&)event).GetValue();

  uMod_Client *client = new uMod_Client( pipe, this);
  client->Create();
  client->Run();

  wxString save_file = "auto_save";
  int num = SaveFile_Exe.GetCount();
  for (int i=0; i<num; i++) if (name==SaveFile_Exe[i])
  {
    save_file = SaveFile_Name[i];
    break;
  }

  uMod_GamePage *page = new uMod_GamePage( Notebook, name, injection_method, save_file, client->Pipe);
  if (page->LastError.Len()>0)
  {
    wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
    delete page;
    return;
  }
  Notebook->AddPage( page, page->GetPageName(), true);

  Clients[NumberOfGames] = client;
  NumberOfGames++;
  if (NumberOfGames==1) ActivateGamesControl();
}

void uMod_Frame::OnDeleteGame( wxCommandEvent &event)
{
  uMod_Client *client = ((uMod_Event&)event).GetClient();
  for (int i=0; i<NumberOfGames; i++) if (Clients[i]==client)
  {
    Notebook->DeletePage(i+1);
    Clients[i]->Wait();
    delete Clients[i];
    NumberOfGames--;
    for (int j=i; j<NumberOfGames; j++) Clients[j] = Clients[j+1];

    if (NumberOfGames==0) DeactivateGamesControl();
    return;
  }
}

void uMod_Frame::OnAddDevice( wxCommandEvent &event)
{

  uMod_Client *client = ((uMod_Event&)event).GetClient();
  for (int i=0; i<NumberOfGames; i++) if (Clients[i]==client)
  {
    uMod_GamePage *page = (uMod_GamePage*) Notebook->GetPage(i+1);
    page->AddDXDevice(((uMod_Event&)event).GetValue());
    return;
  }
}

void uMod_Frame::OnRemoveDevice( wxCommandEvent &event)
{
  /*
  uMod_Client *client = ((uMod_Event&)event).GetClient();
  for (int i=0; i<NumberOfGames; i++) if (Clients[i]==client)
  {
    wxMessageBox("removed");
    uMod_GamePage *page = (uMod_GamePage*) Notebook->GetPage(i+1);
    page->RemoveDXDevice(((uMod_Event&)event).GetValue());
    return;
  }
  */
}

void uMod_Frame::OnClose(wxCloseEvent& event)
{
  if (event.CanVeto() && NumberOfGames>0)
  {
    if (wxMessageBox(Language->ExitGameAnyway, "ERROR", wxYES_NO|wxICON_ERROR)!=wxYES)
    {
      event.Veto();
      return;
    }
  }
  //event.Skip();
  Destroy();
}



int uMod_Frame::ChooseTemplate( const wxString &message, wxString &file)
{
  file.Empty();

  wxArrayString files;
  if (wxDir::GetAllFiles( "templates/", &files, "*.txt", wxDIR_FILES)<=0)
  {
    wxMessageBox( Language->Error_NoTemplates, "ERROR", wxOK|wxICON_ERROR);
    return -1;
  }

  for (unsigned int i=0; i<files.GetCount();i++)
    files[i] =  files[i].AfterFirst('\\').BeforeLast('.');


  files.Insert("auto_save", 0);

  wxSingleChoiceDialog *dialog = new wxSingleChoiceDialog(this, Language->ChooseTemplate, message, files);

  if (dialog->ShowModal ()==wxID_OK)
  {
    file = dialog->GetStringSelection();
    return 0;
  }
  return 1;
}

void uMod_Frame::OnMenuOpenTemplate(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  uMod_GamePage *page = (uMod_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;

  wxString file_name;
  if (ChooseTemplate( Language->OpenTemplate, file_name)!=0)
    return;
  if (page->LoadTemplate( file_name))
  {
    wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
    page->LastError.Empty();
  }
}

void uMod_Frame::OnMenuSaveTemplate(wxCommandEvent& event)
{
  int id = event.GetId();

  if (Notebook->GetPageCount()==0) return;
  uMod_GamePage *page = (uMod_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;

  wxString file_name = page->GetTemplateName();

  if (id==ID_Menu_SaveTemplateAs || file_name.IsEmpty())
  {
    file_name = file_name.AfterFirst('\\').BeforeLast('.');
    wxTextEntryDialog *dialog = new wxTextEntryDialog( this, Language->ChooseTemplate , Language->SaveTemplate, file_name);
    if (dialog->ShowModal ()==wxID_OK)
    {
      file_name = dialog->GetValue();
    }

    delete dialog;
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

void uMod_Frame::OnMenuSetDefaultTemplate(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  uMod_GamePage *page = (uMod_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;
  if (page == (uMod_GamePage*) Notebook->GetPage(0)) return;

  wxString file_name;
  if (ChooseTemplate( Language->DefaultTemplate, file_name)!=0)
    return;

  wxString exe = page->GetExeName();

  int num = SaveFile_Exe.GetCount();
  bool hit = false;
  for (int i=0; i<num; i++) if (SaveFile_Exe[i]==exe)
  {
    SaveFile_Name[i] = file_name;
    hit = true;
    break;
  }
  if (!hit)
  {
    SaveFile_Exe.Add(exe);
    SaveFile_Name.Add(file_name);
  }
  if (SaveTemplate())
  {
    wxMessageBox(LastError, "ERROR", wxOK|wxICON_ERROR);
    LastError.Empty();
  }
}

void uMod_Frame::OnMenuLanguage(wxCommandEvent& WXUNUSED(event))
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
    MenuMain->SetLabel( ID_Menu_StartGame, Language->MenuStartGame);
    MenuMain->SetLabel( ID_Menu_StartGameCMD, Language->MenuStartGameCMD);

    MenuMain->SetLabel( ID_Menu_AddGame, Language->MenuAddGame);
    MenuMain->SetLabel( ID_Menu_DeleteGame, Language->MenuDeleteGame);
    MenuMain->SetLabel( ID_Menu_UseHook, Language->MenuUseHook);

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

/*
    OpenButton->SetLabel( Language->ButtonOpen);
    DirectoryButton->SetLabel( Language->ButtonDirectory);
    UpdateButton->SetLabel( Language->ButtonUpdate);
    ReloadButton->SetLabel( Language->ButtonReload);
*/
    int num = Notebook->GetPageCount();
    for (int i=0; i<num; i++)
    {
      uMod_GamePage* page = (uMod_GamePage*) Notebook->GetPage(i);
      page->UpdateLanguage();
    }
  }
}

void uMod_Frame::OnMenuExit(wxCommandEvent& WXUNUSED(event))
{
  Close();
}

void uMod_Frame::OnMenuHelp(wxCommandEvent& WXUNUSED(event))
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

void uMod_Frame::OnMenuAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString msg;
  msg << uMod_VERSION << "\n\nProject members:\n\nROTA (developer)\n\nKing Brace Blane (PR)\n\nhttp://code.google.com/p/texmod/";
  wxMessageBox( msg, "Info", wxOK);
}

void uMod_Frame::OnMenuAcknowledgement(wxCommandEvent& WXUNUSED(event))
{
  wxString msg;
  msg << "ROTA thanks:\n\n";
  msg << "RS for coding the original TexMod and for information about the used hashing algorithm\n\n";
  msg << "Vergil for the uMod logo and useful hints\n\n";
  msg << "EvilAlex for translation into Russian and bug fixing\n";
  msg << "ReRRemi for translation into French\n";
  msg << "mirHL for transaltion into Italian\n";
  //msg << "\n\n";
  //msg << "King Brace Blane for a tutorial video on YouTube and bug fixing";
  wxMessageBox( msg, Language->MenuAcknowledgement, wxOK);
}

void uMod_Frame::OnMenuStartGame(wxCommandEvent& event)
{

  bool use_cmd = false;
  if (event.GetId() ==  ID_Menu_StartGameCMD) use_cmd = true;

  wxArrayString games, cmd, choices;

  GetInjectedGames( games, cmd);
  int num = games.GetCount();

  choices = games;
  choices.Add( Language->StartGame);

  int index = wxGetSingleChoiceIndex( Language->MenuStartGame, Language->MenuStartGame, choices);

  if (index < 0) return;
  else if (index==num)
  {
    wxString file_name = wxFileSelector( Language->ChooseGame, "", "", "exe",  "binary (*.exe)|*.exe", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
    if ( !file_name.empty() )
    {
      bool hit = false;
      for (int i=0; i<num; i++) if (file_name==games[i]) {hit=true; index=i; break;}

      if (!hit)
      {
        games.Add(file_name);
        cmd.Add("");
      }
    }
    else return;
  }

  wxString command_line;
  if (use_cmd)
  {
    command_line = cmd[index];
    command_line = wxGetTextFromUser( Language->CommandLine, Language->CommandLine, command_line);
    if (!command_line.IsEmpty()) cmd[index] = command_line;
  }

  SetInjectedGames( games, cmd);

  if (Settings.UseHook)
  {
    wxArrayString array;
    if (GetHookedGames( array)) array.Empty();

    int num = array.GetCount();
    for (int i=0; i<num; i++) if (array[i] == games[index])
    {
      wxMessageBox(Language->Error_GameIsHooked, "ERROR", wxOK|wxICON_ERROR);
      return;
    }
  }

  STARTUPINFOW si = {0};
  si.cb = sizeof(STARTUPINFO);
  PROCESS_INFORMATION pi = {0};

  wxString path = games[index].BeforeLast('\\');
  wxString exe;

  if (use_cmd) exe << "\"" << games[index] << "\" " << command_line;
  else exe = games[index];


  bool result = CreateProcess(NULL, (wchar_t*) exe.wc_str(), NULL, NULL, FALSE,
                              CREATE_SUSPENDED, NULL, path.wc_str(), &si, &pi);
  if(!result)
  {
    wxMessageBox( Language->Error_ProcessNotStarted, "ERROR",  wxOK|wxICON_ERROR);
    return ;
  }


  wxString dll = wxGetCwd();
  dll.Append( L"\\" uMod_d3d9_DI_dll);
  //dll.Append( L"\\uMod_d3d10_DI.dll");

  Inject(pi.hProcess, dll.wc_str(), "Nothing");
  ResumeThread(pi.hThread);
}

void uMod_Frame::OnMenuUseHook(wxCommandEvent& WXUNUSED(event))
{
  bool use_hook = MenuMain->IsChecked(ID_Menu_UseHook);

  if (Settings.UseHook!=use_hook)
  {
    if (Settings.UseHook)
    {
      if (NumberOfGames>0)
      {
        MenuMain->Check(ID_Menu_UseHook, true);
        wxMessageBox(Language->Error_RemoveHook, "ERROR", wxOK|wxICON_ERROR);
        return;
      }
      RemoveHook();
    }
    else
    {
      InstallHook();
    }
    Settings.UseHook=use_hook;
  }
}

void uMod_Frame::OnMenuAddGame(wxCommandEvent& WXUNUSED(event))
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

void uMod_Frame::OnMenuDeleteGame(wxCommandEvent& WXUNUSED(event))
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


int uMod_Frame::ActivateGamesControl(void)
{
  /*
  MenuMain->Enable( ID_Menu_LoadTemplate, true);
  MenuMain->Enable( ID_Menu_SaveTemplate, true);
  MenuMain->Enable( ID_Menu_SaveTemplateAs, true);
  */
  MenuMain->Enable( ID_Menu_SetDefaultTemplate, true);
  return 0;
}

int uMod_Frame::DeactivateGamesControl(void)
{
  /*
  MenuMain->Enable( ID_Menu_LoadTemplate, false);
  MenuMain->Enable( ID_Menu_SaveTemplate, false);
  MenuMain->Enable( ID_Menu_SaveTemplateAs, false);
*/
  MenuMain->Enable( ID_Menu_SetDefaultTemplate, false);
  return 0;
}

int uMod_Frame::GetHookedGames( wxArrayString &array)
{
  wxFile file;
  wxString name;
  wchar_t *app_path = _wgetenv( L"APPDATA");
  name.Printf("%ls\\%ls\\%ls", app_path, uMod_APP_DIR, uMod_APP_DX9);

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

int uMod_Frame::SetHookedGames( const wxArrayString &array)
{
  wxFile file;
  wxString name;
  wchar_t *app_path = _wgetenv( L"APPDATA");
  name.Printf("%ls\\%ls", app_path, uMod_APP_DIR);

  if (! wxDir::Exists(name))
  {
    wxDir::Make(name);
  }

  name.Printf("%ls\\%ls\\%ls", app_path, uMod_APP_DIR, uMod_APP_DX9);
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

#define DI_FILE "uMod_DI_Games.txt"
int uMod_Frame::GetInjectedGames( wxArrayString &games, wxArrayString &cmd)
{
  wxFile file;

  if (!file.Access( DI_FILE, wxFile::read)) {LastError << Language->Error_FileOpen << "\n" << DI_FILE; return -1;}
  file.Open( DI_FILE, wxFile::read);
  if (!file.IsOpened()) {LastError << Language->Error_FileOpen << "\n" << DI_FILE ; return -1;}

  unsigned len = file.Length();

  unsigned char* buffer;
  try {buffer = new unsigned char [len+2];}
  catch (...) {LastError << Language->Error_Memory; return -1;}

  unsigned int result = file.Read( buffer, len);
  file.Close();

  if (result != len) {delete [] buffer; LastError << Language->Error_FileRead<<"\n" << DI_FILE; return -1;}

  wchar_t *buff = (wchar_t*)buffer;
  len/=2;
  buff[len]=0;

  wxString content;
  content =  buff;
  delete [] buffer;

  wxStringTokenizer token( content, "\n");

  int num = token.CountTokens();

  games.Empty();
  games.Alloc(num);
  cmd.Empty();
  cmd.Alloc(num);
  wxString entry;

  for (int i=0; i<num; i++)
  {
    entry = token.GetNextToken();
    games.Add( entry.BeforeFirst('|'));
    cmd.Add( entry.AfterFirst('|'));
  }
  return 0;
}

int uMod_Frame::SetInjectedGames( wxArrayString &games, wxArrayString &cmd)
{
  wxFile file;

  file.Open( DI_FILE, wxFile::write);
  if (!file.IsOpened()) {LastError << Language->Error_FileOpen << "\n" << DI_FILE ; return -1;}
  wxString content;

  int num = games.GetCount();
  for (int i=0; i<num; i++)
  {
    content = games[i];
    content << "|" <<  cmd[i] << "\n";
    file.Write( content.wc_str(), content.Len()*2);
  }
  file.Close();
  return 0;
}


#define SAVE_FILE "uMod_SaveFiles.txt"

int uMod_Frame::LoadTemplate(void)
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

int uMod_Frame::SaveTemplate(void)
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



void uMod_Frame::InstallHook(void)
{
  if (H_DX9_DLL==NULL)
  {
    H_DX9_DLL = LoadLibraryW(uMod_d3d9_Hook_dll);
    if (H_DX9_DLL!=NULL)
    {
      typedef void (*fkt_typ)(void);
      fkt_typ install_hook = (fkt_typ) GetProcAddress( H_DX9_DLL, "InstallHook");
      if (install_hook!=NULL) install_hook();
      else
      {
        DWORD error = GetLastError();
        wchar_t *error_msg;
        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
              NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &error_msg, 0, NULL );
        wxString temp = Language->Error_DLLNotFound;
        temp << "\n" << uMod_d3d9_Hook_dll;
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
      temp << "\n" << uMod_d3d9_Hook_dll;
      temp << "\n" << error_msg << "Code: " << error;
      wxMessageBox(temp, "ERROR", wxOK|wxICON_ERROR);
    }
  }
}

void uMod_Frame::RemoveHook(void)
{
  if (H_DX9_DLL!=NULL)
  {
    typedef void (*fkt_typ)(void);
    fkt_typ remove_hook = (fkt_typ) GetProcAddress( H_DX9_DLL, "RemoveHook");
    if (remove_hook!=NULL) remove_hook();
    FreeLibrary(H_DX9_DLL);
  }
  H_DX9_DLL = NULL;
}
