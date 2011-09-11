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



#ifndef OTM_GAME_H_
#define OTM_GAME_H_
#include "OTM_Main.h"

//this class is intended as a storing object for each game
// one should ad an assignment operator,  loading and saving default values, ...
class OTM_GameInfo
{
public:
  OTM_GameInfo(const wxString &name);
  ~OTM_GameInfo(void);

  wxString Name;


  int SetSaveSingleTexture(bool val);
  bool GetSaveSingleTexture(void) {return SaveSingleTexture;}

  int SetSaveAllTextures(bool val);
  bool GetSaveAllTextures(void) {return SaveAllTextures;}

  void SetTextures(const wxArrayString &textures);
  void GetTextures( wxArrayString &textures);
  void AddTexture( const wxString &textures);

  int SendTextures(void);

  int GetKeyBack() {return KeyBack;}
  int SetKeyBack(int key) {KeyBack=key; return 0;}

  int GetKeySave() {return KeySave;}
  int SetKeySave(int key) {KeySave=key; return 0;}

  int GetKeyNext() {return KeyNext;}
  int SetKeyNext(int key) {KeyNext=key; return 0;}

  int SetPath(const wxString &path) {Path=path; return 0;}
  wxString GetPath(void) {return Path;}

private:
  bool SaveSingleTexture;
  bool SaveAllTextures;
  wxArrayString Textures;

  int KeyBack;
  int KeySave;
  int KeyNext;

  wxString Path;
};


#endif /* OTM_SERVER_H_ */
