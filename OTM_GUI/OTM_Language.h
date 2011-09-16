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



#ifndef OTM_LANGUAGE_H_
#define OTM_LANGUAGE_H_

class OTM_Language
{
public:
  OTM_Language(void);


  int LoadLanguage(int lang);

  wxString MenuPref;
  wxString MenuQuit;
  wxString MenuHelp;
  wxString MenuAbout;

  wxString MainMenuStart;
  wxString MainMenuHelp;

  wxString ButtonOpen;
  wxString ButtonDirectory;
  wxString ButtonUpdate;

  wxString ChooseFile;
  wxString ChooseDir;

  wxString CheckBoxSaveSingleTexture;
  wxString CheckBoxSaveAllTextures;


  wxString KeyBack;
  wxString KeySave;
  wxString KeyNext;
  wxArrayString Keys;


  wxString FileNotSupported;

private:
  int LoadKeys(int lang);
};





#endif /* OTM_LANGUAGE_H_ */
