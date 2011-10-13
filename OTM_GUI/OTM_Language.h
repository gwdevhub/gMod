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
  OTM_Language(const wxString &name);


  int LoadLanguage(const wxString &name);
  int GetLanguages(wxArrayString &lang);
  int GetHelpMessage(wxString &help);
  wxString GetCurrentLanguage(void) {return CurrentLanguage;}

  wxString MenuLanguage;
  wxString MenuHelp;
  wxString MenuAbout;
  wxString MenuAcknowledgement;
  wxString MenuAddGame;
  wxString MenuDeleteGame;
  wxString MenuLoadTemplate;
  wxString MenuSaveTemplate;
  wxString MenuSaveTemplateAs;
  wxString MenuSetDefaultTemplate;
  wxString MenuExit;

  wxString MainMenuMain;
  wxString MainMenuHelp;

  wxString ButtonOpen;
  wxString ButtonDirectory;
  wxString ButtonUpdate;
  wxString ButtonReload;

  wxString ChooseFile;
  wxString ChooseDir;

  wxString TextCtrlTemplate;
  wxString CheckBoxSaveSingleTexture;
  wxString CheckBoxSaveAllTextures;
  wxString TextCtrlSavePath;

  wxString SelectLanguage;

  wxString ChooseGame;
  wxString DeleteGame;
  wxString GameAlreadyAdded;
  wxString ExitGameAnyway;
  wxString NoComment;
  wxString Author;

  wxString Error_FileNotSupported;
  wxString Error_FktNotFound;
  wxString Error_DLLNotFound;
  wxString Error_AlreadyRunning;

  wxString Error_Send;
  wxString Error_KeyTwice;
  wxString Error_NoSavePath;
  wxString Error_KeyNotSet;
  wxString Error_SaveFile;
  wxString Error_NoPipe;
  wxString Error_WritePipe;
  wxString Error_FlushPipe;
  wxString Error_Hash;
  wxString Error_FileOpen;
  wxString Error_FileRead;
  wxString Error_Memory;
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
  int LoadDefault(void);
  int LoadKeys(void);

  wxString CurrentLanguage;
};


extern OTM_Language *Language;


#endif /* OTM_LANGUAGE_H_ */
