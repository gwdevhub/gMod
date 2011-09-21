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





#ifndef __CDT_PARSER__

DEFINE_EVENT_TYPE(OTM_EVENT_TYPE)

BEGIN_EVENT_TABLE(OTM_Frame, wxFrame)
  EVT_CLOSE(OTM_Frame::OnClose)

  EVT_BUTTON(ID_Button_Open, OTM_Frame::OnButtonOpen)
  EVT_BUTTON(ID_Button_Path, OTM_Frame::OnButtonPath)
  EVT_BUTTON(ID_Button_Update, OTM_Frame::OnButtonUpdate)
  EVT_BUTTON(ID_Button_Save, OTM_Frame::OnButtonSave)

  //EVT_MENU(ID_Menu_Pref, OTM_Frame::OnMenuPref)
  //EVT_MENU(ID_Menu_Quit, OTM_Frame::OnMenuQuit)
  EVT_MENU(ID_Menu_Help, OTM_Frame::OnMenuHelp)
  EVT_MENU(ID_Menu_About, OTM_Frame::OnMenuAbout)
  EVT_MENU(ID_Menu_AddGame, OTM_Frame::OnMenuAddGame)
  EVT_MENU(ID_Menu_DeleteGame, OTM_Frame::OnMenuDeleteGame)

  EVT_COMMAND  (ID_Add_Game, OTM_EVENT_TYPE, OTM_Frame::OnAddGame)
  EVT_COMMAND  (ID_Delete_Game, OTM_EVENT_TYPE, OTM_Frame::OnDeleteGame)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)
#endif

bool MyApp::OnInit(void)
{

  OTM_Frame *frame = new OTM_Frame( "OpenTexMod V0.9 alpha by ROTA", wxDefaultPosition, wxSize(600,400));
  SetTopWindow( frame );

  return true;
}


OTM_Frame::OTM_Frame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
  SetIcon(wxICON(MAINICON));
  if (Language.LoadLanguage(1)!=0)
  {
    wxMessageBox("Could not load language", "Error", wxOK);
    Destroy();
  }
  Server = new OTM_Server( this);
  Server->Create();
  Server->Run();

  MenuBar = new wxMenuBar;
  //MenuMain = new wxMenu;
  MenuGame = new wxMenu;
  MenuHelp = new wxMenu;

  //MenuMain->Append( ID_Menu_Pref, Language.MenuPref, Language.MenuPref );
  //MenuMain->Append( ID_Menu_Quit, Language.MenuQuit, Language.MenuQuit );

  MenuGame->Append( ID_Menu_AddGame, Language.MenuAddGame, Language.MenuAddGame );
  MenuGame->Append( ID_Menu_DeleteGame, Language.MenuDeleteGame, Language.MenuDeleteGame );

  MenuHelp->Append( ID_Menu_Help, Language.MenuHelp, Language.MenuHelp );
  MenuHelp->Append( ID_Menu_About, Language.MenuAbout, Language.MenuAbout );

  //MenuBar->Append( MenuMain, Language.MainMenuStart );
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
  Clients = new OTM_Client*[MaxNumberOfGames];

  Show( true );


  HMODULE hMod = LoadLibraryW(OTM_d3d9_dll);
  typedef void (*fkt_typ)(void);
  fkt_typ InstallHook = (fkt_typ) GetProcAddress( hMod, "InstallHook");
  InstallHook();
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

  HMODULE hMod = LoadLibraryW( OTM_d3d9_dll);
  typedef void (*fkt_typ)(void);
  fkt_typ RemoveHook = (fkt_typ) GetProcAddress( hMod, "RemoveHook");
  RemoveHook();
  FreeLibrary(hMod);

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
  if (NumberOfGames>=MaxNumberOfGames) return;

  wxString name = ((OTM_Event&)event).GetName();
  PipeStruct pipe;

  pipe.In = ((OTM_Event&)event).GetPipeIn();
  pipe.Out = ((OTM_Event&)event).GetPipeOut();

  OTM_Client *client = new OTM_Client( pipe, this);
  client->Create();
  client->Run();

  OTM_GamePage *page = new OTM_GamePage( Notebook, name, client->Pipe, Language);

  Notebook->AddPage( page, name, true);

  Clients[NumberOfGames] = client;
  NumberOfGames++;
}

void OTM_Frame::OnDeleteGame( wxCommandEvent &event)
{
  OTM_Client *client = ((OTM_Event&)event).GetClient();
  int index = -1;
  for (int i=0; i<NumberOfGames; i++) if (Clients[i]==client)
  {
    Notebook->DeletePage(i);
    Clients[i]->Delete();
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

    if (wxMessageBox(Language.ExitGameAnyway, "ERROR", wxYES_NO)!=wxYES) {event.Veto(); return;}
  }
  event.Skip();
  Destroy();
}

void OTM_Frame::OnButtonOpen(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;

  wxString file_name = wxFileSelector( Language.ChooseFile, page->GetOpenPath(), "", "*.*",  "textures (*.dds)|*.dds|zip (*.zip)|*.zip|tpf (*.tpf)|*.tpf", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if ( !file_name.empty() )
  {
    page->SetOpenPath(file_name.BeforeLast( '/'));
    page->AddTexture( file_name);
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
  page->UpdateGame();
}

void OTM_Frame::OnButtonSave(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;
  page->SaveToFile();
}

/*
void OTM_Frame::OnMenuPref(wxCommandEvent& WXUNUSED(event)) {}
void OTM_Frame::OnMenuQuit(wxCommandEvent& WXUNUSED(event)) {Destroy();}
*/

void OTM_Frame::OnMenuHelp(wxCommandEvent& WXUNUSED(event))
{
  wxFile dat;
  if (!dat.Access( "README.txt", wxFile::read)) return;
  dat.Open("README.txt", wxFile::read);
  if (!dat.IsOpened())  {return;}
  unsigned len = dat.Length();

  char* buffer;
  try {buffer = new char [len+1];}
  catch (...) {return ;}

  unsigned int result = dat.Read( buffer, len);
  dat.Close();
  if (len!=result) {delete [] buffer; return;}

  buffer[len] = 0;

  wxString msg = buffer;
  wxMessageBox(msg, "help", wxOK);
}

void OTM_Frame::OnMenuAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(L"OpenTexMod V0.9 alpha by ROTA\nhttp://code.google.com/p/texmod/", "Info", wxOK);
};

void OTM_Frame::OnMenuAddGame(wxCommandEvent& WXUNUSED(event))
{
  wxString file_name = wxFileSelector( Language.ChooseFile, "", "", "exe",  "binary (*.exe)|*.exe", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if ( !file_name.empty() )
  {
    file_name = file_name.AfterLast('\\');

    wxArrayString array;
    GetHookedGames( array);
    int num = array.GetCount();
    for (int i=0; i<num; i++) if (array[i] == file_name)
    {
      wxMessageBox(Language.GameAlreadyAdded, "ERROR", wxOK);
      return;
    }
    array.Add(file_name);
    SetHookedGames( array);
  }
}

void OTM_Frame::OnMenuDeleteGame(wxCommandEvent& WXUNUSED(event))
{
  wxArrayInt selections;
  wxArrayString array;
  GetHookedGames(array);
  wxGetSelectedChoices( selections, Language.DeleteGame, Language.DeleteGame, array);

  int num = selections.GetCount();
  for (int i=0; i<num; i++)
  {
    array.RemoveAt( selections[i]-i); //this will work only if selections is sorted !!
  }

  SetHookedGames(array);
}



int OTM_Frame::GetHookedGames( wxArrayString &array)
{
  wxFile file;
  wxString name;
  wchar_t *app_path = _wgetenv( L"APPDATA");
  name.Printf("%ls\\%ls\\%ls", app_path, OTM_APP_DIR, OTM_APP_DX9);

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
  if (!file.IsOpened())  {return -1;}
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
