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



#ifndef OTM_GAMEPAGE_H_
#define OTM_GAMEPAGE_H_
#include "OTM_Main.h"


// this page is opened if a game is started.
class OTM_GamePage : public wxPanel
{
public:
  OTM_GamePage( wxNotebook *parent, OTM_GameInfo *game, PipeStruct &pipe, OTM_Language &lang);
  virtual ~OTM_GamePage(void);

  void AddTexture( const wxString &file_name);
  void SetPath( const wxString &file_name);
  int UpdateGame(void);

private:
  wxChoice *ChoiceKeyBack;
  wxChoice *ChoiceKeySave;
  wxChoice *ChoiceKeyNext;

  wxBoxSizer *MainSizer;
  wxCheckBox **CheckBoxes;
  int NumberOfEntry;
  int MaxNumberOfEntry;
  OTM_Language &Language;

  OTM_GameInfo *Game;

  OTM_Sender Sender;
};

#endif /* OTM_GAMEPAGE_H_ */
