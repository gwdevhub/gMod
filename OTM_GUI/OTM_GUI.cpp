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

  EVT_MENU(ID_Menu_Pref, OTM_Frame::OnMenuPref)
  EVT_MENU(ID_Menu_Quit, OTM_Frame::OnMenuQuit)
  EVT_MENU(ID_Menu_Help, OTM_Frame::OnMenuHelp)
  EVT_MENU(ID_Menu_About, OTM_Frame::OnMenuAbout)

  EVT_COMMAND  (ID_Add_Game, OTM_EVENT_TYPE, OTM_Frame::OnAddGame)
  EVT_COMMAND  (ID_Delete_Game, OTM_EVENT_TYPE, OTM_Frame::OnDeleteGame)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)
#endif

bool MyApp::OnInit(void)
{

  OTM_Frame *frame = new OTM_Frame( "OpenTexMod", wxDefaultPosition, wxSize(600,400));
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
  MenuMain = new wxMenu;
  MenuHelp = new wxMenu;

  MenuMain->Append( ID_Menu_Pref, Language.MenuPref, Language.MenuPref );
  MenuMain->Append( ID_Menu_Quit, Language.MenuQuit, Language.MenuQuit );

  MenuHelp->Append( ID_Menu_Help, Language.MenuHelp, Language.MenuHelp );
  MenuHelp->Append( ID_Menu_About, Language.MenuAbout, Language.MenuAbout );

  MenuBar->Append( MenuMain, Language.MainMenuStart );
  MenuBar->Append( MenuHelp, Language.MainMenuHelp );

  SetMenuBar(MenuBar);


  MainSizer = new wxBoxSizer(wxVERTICAL);

  Notebook = new wxNotebook( this, wxID_ANY);
  MainSizer->Add( Notebook, 1, wxEXPAND , 0 );

  ButtonSizer = new wxBoxSizer(wxHORIZONTAL);

  OpenButton = new wxButton( this, ID_Button_Open, Language.ButtonOpen, wxDefaultPosition, wxSize(100,24));
  DirectoryButton = new wxButton( this, ID_Button_Path, Language.ButtonDirectory, wxDefaultPosition, wxSize(100,24));
  UpdateButton = new wxButton( this, ID_Button_Update, Language.ButtonUpdate, wxDefaultPosition, wxSize(100,24));
  ButtonSizer->Add( OpenButton, 1, wxEXPAND, 0);
  ButtonSizer->Add( DirectoryButton, 1, wxEXPAND, 0);
  ButtonSizer->Add( UpdateButton, 1, wxEXPAND, 0);
  MainSizer->Add( ButtonSizer, 0, wxEXPAND , 0 );


  SetSizer( MainSizer);

  NumberOfGames = 0;
  MaxNumberOfGames  = 10;
  Games = new OTM_GameInfo*[MaxNumberOfGames];
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

  OTM_GameInfo *game = new OTM_GameInfo( name);
  OTM_Client *client = new OTM_Client( pipe, this);
  client->Create();
  client->Run();

  OTM_GamePage *page = new OTM_GamePage( Notebook, game, client->Pipe, Language);

  Notebook->AddPage( page, name, true);

  Games[NumberOfGames] = game;
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
    //Games[i]->Wait();
    Clients[i]->Delete();
    delete Clients[i];
    delete Games[i];
    NumberOfGames--;
    for (int j=i; j<NumberOfGames; j++) Games[j] = Games[j+1];
    for (int j=i; j<NumberOfGames; j++) Clients[j] = Clients[j+1];
    return;
  }
}


void OTM_Frame::OnClose(wxCloseEvent& event)
{
  if (event.CanVeto() && NumberOfGames>0)
  {
    event.Veto();
    wxMessageBox("Very bad idea to close OpenTexMod before closing the hooked Games!", "ERROR", wxOK);
    return;
  }
  event.Skip();
  Destroy();
}

void OTM_Frame::OnButtonOpen(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;

  wxString file_name = wxFileSelector( Language.ChooseFile,  TexturePath, "", "dds",  "textures (*.dds)|*.dds", wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
  if ( !file_name.empty() )
  {
    TexturePath =  file_name.BeforeLast( '/');
    page->AddTexture( file_name);
  }
}

void OTM_Frame::OnButtonPath(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;

  wxString dir = wxDirSelector(Language.ChooseDir);
  if ( !dir.empty() )
  {
    page->SetPath( dir);
  }
}

void OTM_Frame::OnButtonUpdate(wxCommandEvent& WXUNUSED(event))
{
  if (Notebook->GetPageCount()==0) return;
  OTM_GamePage *page = (OTM_GamePage*) Notebook->GetCurrentPage();
  if (page==NULL) return;
  page->UpdateGame();
}

void OTM_Frame::OnMenuPref(wxCommandEvent& WXUNUSED(event)) {}
void OTM_Frame::OnMenuQuit(wxCommandEvent& WXUNUSED(event)) {Destroy();}
void OTM_Frame::OnMenuHelp(wxCommandEvent& WXUNUSED(event)) {}

void OTM_Frame::OnMenuAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox("OpenTexMod V0.0 by ROTA", "Info", wxOK);
};
