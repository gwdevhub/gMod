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
  EVT_BUTTON(ID_Button_Save, OTM_Frame::OnButtonSave)

  EVT_MENU(ID_Menu_Lang, OTM_Frame::OnMenuLanguage)
  EVT_MENU(ID_Menu_Help, OTM_Frame::OnMenuHelp)
  EVT_MENU(ID_Menu_About, OTM_Frame::OnMenuAbout)
  EVT_MENU(ID_Menu_AddGame, OTM_Frame::OnMenuAddGame)
  EVT_MENU(ID_Menu_DeleteGame, OTM_Frame::OnMenuDeleteGame)

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
  CheckForSingleRun = CreateMutex( NULL, true, L"Global\\OTM_CheckForSingleRun");
  if (ERROR_ALREADY_EXISTS == GetLastError())
  {
    wxMessageBox( L"An instance of OpenTexMod already exists.", "ERROR", wxOK|wxICON_ERROR);
    return false;
  }
  OTM_Frame *frame = new OTM_Frame( OTM_VERSION, wxDefaultPosition, wxSize(600,400));
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
  MenuGame = new wxMenu;
  MenuHelp = new wxMenu;


  MenuGame->Append( ID_Menu_AddGame, Language.MenuAddGame );
  MenuGame->Append( ID_Menu_DeleteGame, Language.MenuDeleteGame );

  MenuHelp->Append( ID_Menu_Lang, Language.MenuLanguage );
  MenuHelp->Append( ID_Menu_Help, Language.MenuHelp );
  MenuHelp->Append( ID_Menu_About, Language.MenuAbout );

  MenuBar->Append( MenuGame, Language.MainMenuGame );
  MenuBar->Append( MenuHelp, Language.MainMenuHelp );

  SetMenuBar(MenuBar);


  MainSizer = new wxBoxSizer(wxVERTICAL);

  Notebook = new wxNotebook( this, wxID_ANY);
  MainSizer->Add( (wxWindow*) Notebook, 1, wxEXPAND , 0 );

  ButtonSizer = new wxBoxSizer(wxHORIZONTAL);

  OpenButton = new wxButton( this, ID_Button_Open, Language.ButtonOpen, wxDefaultPosition, wxSize(100,24));
  DirectoryButton = new wxButton( this, ID_Button_Path, Language.ButtonDirectory, wxDefaultPosition, wxSize(100,24));
  UpdateButton = new wxButton( this, ID_Button_Update, Language.ButtonUpdate, wxDefaultPosition, wxSize(100,24));
  SaveButton = new wxButton( this, ID_Button_Save, Language.ButtonSave, wxDefaultPosition, wxSize(100,24));

  ButtonSizer->Add( (wxWindow*) OpenButton, 1, wxEXPAND, 0);
  ButtonSizer->Add( (wxWindow*) DirectoryButton, 1, wxEXPAND, 0);
  ButtonSizer->Add( (wxWindow*) UpdateButton, 1, wxEXPAND, 0);
  ButtonSizer->Add( (wxWindow*) SaveButton, 1, wxEXPAND, 0);
  MainSizer->Add( ButtonSizer, 0, wxEXPAND , 0 );


  SetSizer( MainSizer);

  NumberOfGames = 0;
  MaxNumberOfGames  = 10;
  Clients = NULL;
  if (GetMemory( Clients, MaxNumberOfGames))
  {
    wxMessageBox( Language.Error_Memory, "ERROR", wxOK|wxICON_ERROR);
  }

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
      wxString temp = Language.Error_DLLNotFound;
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
    wxString temp = Language.Error_DLLNotFound;
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
      wxMessageBox( Language.Error_Memory, "ERROR", wxOK|wxICON_ERROR);
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

  OTM_GamePage *page = new OTM_GamePage( Notebook, name, client->Pipe, Language);
  if (page->LastError.Len()>0)
  {
    wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
    delete page;
    return;
  }
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
    if (wxMessageBox(Language.ExitGameAnyway, "ERROR", wxYES_NO|wxICON_ERROR)!=wxYES) {event.Veto(); return;}
  }
  event.Skip();
  Destroy();
}

void OTM_Frame::OnButtonOpen(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;


  //wxString file_name = wxFileSelector( Language.ChooseFile, page->GetOpenPath(), "", "*.*",  "textures (*.dds)|*.dds|zip (*.zip)|*.zip|tpf (*.tpf)|*.tpf", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  wxString file_name = wxFileSelector( Language.ChooseFile, page->GetOpenPath(), "", "",  "", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
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

  wxString dir = wxDirSelector( Language.ChooseDir, page->GetSavePath());
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

void OTM_Frame::OnButtonSave(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;
  if (page->SaveToFile())
  {
    wxMessageBox(page->LastError, "ERROR", wxOK|wxICON_ERROR);
    page->LastError.Empty();
  }
}

void OTM_Frame::OnMenuLanguage(wxCommandEvent& WXUNUSED(event))
{
  wxArrayString lang;
  Language.GetLanguages( lang);
  wxString choice = wxGetSingleChoice( Language.SelectLanguage, Language.SelectLanguage, lang);
  if (choice.Len()>0)
  {
    if (Language.LoadLanguage(choice))
    {
      wxMessageBox(Language.LastError, "ERROR", wxOK|wxICON_ERROR);
      Language.LastError.Empty();
      return;
    }
    MenuBar->SetMenuLabel( 0, Language.MainMenuGame);
    MenuGame->SetLabel( ID_Menu_AddGame, Language.MenuAddGame);
    MenuGame->SetLabel( ID_Menu_DeleteGame, Language.MenuDeleteGame);

    MenuBar->SetMenuLabel( 1, Language.MainMenuHelp);
    MenuHelp->SetLabel( ID_Menu_Lang, Language.MenuLanguage);
    MenuHelp->SetLabel( ID_Menu_Help, Language.MenuHelp);
    MenuHelp->SetLabel( ID_Menu_About, Language.MenuAbout);

    OpenButton->SetLabel( Language.ButtonOpen);
    DirectoryButton->SetLabel( Language.ButtonDirectory);
    UpdateButton->SetLabel( Language.ButtonUpdate);
    SaveButton->SetLabel( Language.ButtonSave);

    int num = Notebook->GetPageCount();
    for (int i=0; i<num; i++)
    {
      OTM_GamePage* page = (OTM_GamePage*) Notebook->GetPage(i);
      page->UpdateLanguage();
    }
  }
}

void OTM_Frame::OnMenuHelp(wxCommandEvent& WXUNUSED(event))
{
  wxString help;
  if (Language.GetHelpMessage( help))
  {
    wxMessageBox(Language.LastError, "ERROR", wxOK|wxICON_ERROR);
    Language.LastError.Empty();
    return;
  }

  wxMessageBox( help, Language.MenuHelp, wxOK);
}

void OTM_Frame::OnMenuAbout(wxCommandEvent& WXUNUSED(event))
{
  wxString msg;
  msg << OTM_VERSION << " by ROTA\nhttp://code.google.com/p/texmod/";
  wxMessageBox( msg, "Info", wxOK);
};

void OTM_Frame::OnMenuAddGame(wxCommandEvent& WXUNUSED(event))
{
  wxString file_name = wxFileSelector( Language.ChooseGame, "", "", "exe",  "binary (*.exe)|*.exe", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if ( !file_name.empty() )
  {
    file_name = file_name.AfterLast('\\');

    wxArrayString array;
    if (GetHookedGames( array)) array.Empty();

    int num = array.GetCount();
    for (int i=0; i<num; i++) if (array[i] == file_name)
    {
      wxMessageBox(Language.GameAlreadyAdded, "ERROR", wxOK|wxICON_ERROR);
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
  wxGetSelectedChoices( selections, Language.DeleteGame, Language.DeleteGame, array);

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

  if (!file.Access(name, wxFile::read)) {LastError << Language.Error_FileOpen << "\n" << name; return -1;}
  file.Open(name, wxFile::read);
  if (!file.IsOpened()) {LastError << Language.Error_FileOpen << "\n" << name ; return -1;}

  unsigned len = file.Length();

  unsigned char* buffer;
  try {buffer = new unsigned char [len+1];}
  catch (...) {LastError << Language.Error_Memory; return -1;}

  unsigned int result = file.Read( buffer, len);
  file.Close();

  if (result != len) {delete [] buffer; LastError << Language.Error_FileRead<<"\n" << name; return -1;}

  buffer[len]=0;

  wxString content;
  content =  buffer;
  delete [] buffer;

  wxStringTokenizer token( content, "\n");

  int num = token.CountTokens();

  array.Empty();

  for (int i=0; i<num; i++)
  {
    array.Add( token.GetNextToken());
  }

  file.Close();
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
  if (!file.IsOpened()) {LastError << Language.Error_FileOpen << "\n" << name ; return -1;}
  wxString content;

  int num = array.GetCount();
  for (int i=0; i<num; i++)
  {
    content = array[i];
    content << "\n";
    file.Write( content.char_str(), content.Len());
  }
  file.Close();
  return 0;
}
