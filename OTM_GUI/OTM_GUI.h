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



#ifndef OTM_GUI_H_
#define OTM_GUI_H_


#include "OTM_Main.h"

class OTM_Frame : public wxFrame
{
public:
  OTM_Frame(const wxString& title, OTM_Settings &set);
  ~OTM_Frame(void);



  void OnAddGame( wxCommandEvent &event);
  void OnDeleteGame( wxCommandEvent &event);

  void OnClose(wxCloseEvent& WXUNUSED(event));


  void OnButtonOpen(wxCommandEvent& WXUNUSED(event));
  void OnButtonPath(wxCommandEvent& WXUNUSED(event));
  void OnButtonUpdate(wxCommandEvent& WXUNUSED(event));
  void OnButtonReload(wxCommandEvent& WXUNUSED(event));

  void OnMenuStartGame( wxCommandEvent &event);

  void OnMenuUseHook( wxCommandEvent &event);
  void OnMenuAddGame(wxCommandEvent& WXUNUSED(event));
  void OnMenuDeleteGame(wxCommandEvent& WXUNUSED(event));

  void OnMenuOpenTemplate(wxCommandEvent& WXUNUSED(event));
  void OnMenuSaveTemplate(wxCommandEvent& WXUNUSED(event));
  void OnMenuSaveTemplateAs(wxCommandEvent& WXUNUSED(event));
  void OnMenuSetDefaultTemplate(wxCommandEvent& WXUNUSED(event));
  void OnMenuLanguage(wxCommandEvent& WXUNUSED(event));

  void OnMenuExit(wxCommandEvent& WXUNUSED(event));

  void OnMenuHelp(wxCommandEvent& WXUNUSED(event));
  void OnMenuAbout(wxCommandEvent& WXUNUSED(event));
  void OnMenuAcknowledgement(wxCommandEvent& WXUNUSED(event));

private:


  OTM_Settings Settings;
  int KillServer(void);
  int GetHookedGames( wxArrayString &array);
  int SetHookedGames( const wxArrayString &array);

  int GetInjectedGames( wxArrayString &games, wxArrayString &cmd);
  int SetInjectedGames( wxArrayString &games, wxArrayString &cmd);

  OTM_Server *Server;

  wxNotebook *Notebook;


  wxButton *OpenButton;
  wxButton *DirectoryButton;
  wxButton *UpdateButton;
  wxButton *ReloadButton;


  wxMenuBar *MenuBar;
  wxMenu *MenuMain;
  wxMenu *MenuHelp;

  wxBoxSizer *MainSizer;
  wxBoxSizer *ButtonSizer;


  int NumberOfGames;
  int MaxNumberOfGames;
  OTM_Client **Clients;

  int LoadTemplate(void);
  int SaveTemplate(void);
  wxArrayString SaveFile_Exe;
  wxArrayString SaveFile_Name;


  void InstallHook(void);
  void RemoveHook(void);

  HMODULE H_DX9_DLL;

  wxString LastError;

  DECLARE_EVENT_TABLE();
};

class MyApp : public wxApp
{
public:
    virtual ~MyApp();
    virtual bool OnInit();

private:
    HANDLE CheckForSingleRun;
};




#endif
