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



#ifndef OTM_GAMEPAGE_H_
#define OTM_GAMEPAGE_H_
#include "OTM_Main.h"


// this page is opened if a game is started.
class OTM_GamePage : public wxPanel
{
public:
  OTM_GamePage( wxNotebook *parent, const wxString &name, PipeStruct &pipe, OTM_Language &lang);
  virtual ~OTM_GamePage(void);

  void AddTexture( const wxString &file_name);

  int UpdateGame(void);

  int SaveToFile() {return SaveToFile(Game.Name);}
  int SaveToFile( const wxString &file_name);
  //int LoadFromFile( const wxString &file_name) {return Game->LoadFromFile( file_name);}


  int SetOpenPath(const wxString &path) {return Game.SetOpenPath(path);}
  wxString GetOpenPath(void) {return Game.GetOpenPath();}

  int SetSavePath(const wxString &path);
  wxString GetSavePath(void) {return Game.GetSavePath();}


  void OnButtonUp(wxCommandEvent& WXUNUSED(event));
  void OnButtonDown(wxCommandEvent& WXUNUSED(event));
  void OnButtonDelete(wxCommandEvent& WXUNUSED(event));

  int UpdateLanguage(void);

  wxString LastError;

private:

  int GetSettings(void);
  int SetColour( wxTextCtrl** txt, int *colour);
  int GetColour( wxTextCtrl* txt, int def);

  wxBoxSizer *SizerKeys[2];
  wxTextCtrl *TextKeyBack;
  wxTextCtrl *TextKeySave;
  wxTextCtrl *TextKeyNext;
  wxChoice *ChoiceKeyBack;
  wxChoice *ChoiceKeySave;
  wxChoice *ChoiceKeyNext;

  wxBoxSizer *FontColourSizer;
  wxTextCtrl *FontColour[4];
  wxBoxSizer *TextureColourSizer;
  wxTextCtrl *TextureColour[4];

  wxBoxSizer *MainSizer;
  wxCheckBox *SaveAllTextures;
  wxCheckBox *SaveSingleTexture;
  wxTextCtrl *SavePath;

  wxBoxSizer **CheckBoxHSizers;
  //wxBoxSizer *CheckBoxVSizer;
  wxButton **CheckButtonUp;
  wxButton **CheckButtonDown;
  wxButton **CheckButtonDelete;

  wxCheckBox **CheckBoxes;
  int NumberOfEntry;
  int MaxNumberOfEntry;

  OTM_Language &Language;

  wxArrayString Files;
  OTM_GameInfo Game;
  OTM_GameInfo GameOld;

  OTM_Sender Sender;


  //DECLARE_EVENT_TABLE();
};

#endif /* OTM_GAMEPAGE_H_ */
