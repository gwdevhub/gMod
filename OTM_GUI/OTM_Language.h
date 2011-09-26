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



#ifndef OTM_LANGUAGE_H_
#define OTM_LANGUAGE_H_

class OTM_Language
{
public:
  OTM_Language(void);


  int LoadLanguage(const wxString &name);
  int GetLanguages(wxArrayString &lang);
  int GetHelpMessage(wxString &help);

  wxString MenuLanguage;
  wxString MenuHelp;
  wxString MenuAbout;
  wxString MenuAddGame;
  wxString MenuDeleteGame;

  wxString MainMenuGame;
  wxString MainMenuHelp;

  wxString ButtonOpen;
  wxString ButtonDirectory;
  wxString ButtonUpdate;
  wxString ButtonSave;

  wxString ChooseFile;
  wxString ChooseDir;

  wxString CheckBoxSaveSingleTexture;
  wxString CheckBoxSaveAllTextures;
  wxString TextCtrlSavePath;

  wxString SelectLanguage;

  wxString ChooseGame;
  wxString DeleteGame;
  wxString GameAlreadyAdded;
  wxString FileNotSupported;
  wxString ExitGameAnyway;

  wxString Error_FktNotFound;
  wxString Error_DLLNotFound;
  wxString Error_Send;
  wxString Error_KeyTwice;
  wxString Error_NoSavePath;
  wxString Error_SaveFile;
  wxString Error_NoPipe;
  wxString Error_WritePipe;
  wxString Error_FlushPipe;
  wxString Error_Hash;
  wxString Error_FileOpen;
  wxString Error_FileRead;
  wxString Error_Memory;
  wxString Error_FileformatNotSupported;
  wxString Error_Unzip;
  wxString Error_ZipEntry;

  wxString KeyBack;
  wxString KeySave;
  wxString KeyNext;
  wxArrayString KeyStrings;
  wxArrayInt KeyValues;


  wxString FontColour;
  wxString TextureColour;


  wxString LastError;

private:
  int LoadCurrentLanguage(void);
  int SaveCurrentLanguage(void);

  int LoadDefault(void);
  int LoadKeys(void);

  wxString CurrentLanguage;
  const wxString DefaultLanguage;
};





#endif /* OTM_LANGUAGE_H_ */
