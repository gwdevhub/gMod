/*
This file is part of Universal Modding Engine.


Universal Modding Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Universal Modding Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Universal Modding Engine.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef uMod_LANGUAGE_H_
#define uMod_LANGUAGE_H_

class uMod_Language
{
public:
  uMod_Language(void);
  uMod_Language(const wxString &name);


  int LoadLanguage(const wxString &name);
  int GetLanguages(wxArrayString &lang);
  int GetHelpMessage(wxString &help);
  wxString GetCurrentLanguage(void) {return CurrentLanguage;}

  wxString MenuLanguage;
  wxString MenuHelp;
  wxString MenuAbout;
  wxString MenuAcknowledgement;
  wxString MenuStartGame;
  wxString MenuStartGameCMD;
  wxString MenuUseHook;
  wxString MenuAddGame;
  wxString MenuDeleteGame;
  wxString MenuLoadTemplate;
  wxString MenuSaveTemplate;
  wxString MenuSaveTemplateAs;
  wxString MenuSetDefaultTemplate;
  wxString MenuExit;

  wxString MainMenuMain;
  wxString MainMenuHelp;

  wxString MenuOpenPackage;
  wxString MenuRemovePackage;
  wxString MenuRemoveSelectedPackages;
  wxString MenuUpdate;
  wxString MenuReload;
  wxString MenuSupportTPF;
  wxString ButtonUpdate;
  wxString ButtonDirectory;

  wxString ChooseFile;
  wxString ChooseDir;

  wxString MultipleSingleFiles;

  wxString HookInjection;
  wxString DirectInjection;
  wxString NoInjection;
  wxString InvalidGamePage;
  wxString TextCtrlTemplate;

  wxString CollapseTextureCapture;
  wxString CheckBoxSaveSingleTexture;
  wxString CheckBoxShowStringSaveSingleTexture;
  wxString CheckBoxShowSingleTexture;
  wxString CheckBoxSaveAllTextures;

  wxString SelectLanguage;

  wxString StartGame;
  wxString CommandLine;

  wxString ChooseTemplate;
  wxString OpenTemplate;
  wxString SaveTemplate;
  wxString SetTemplateName;
  wxString DefaultTemplate;
  wxString AutoSaveTemplate;

  wxString ChooseGame;
  wxString DeleteGame;
  wxString GameAlreadyAdded;
  wxString ExitGameAnyway;
  wxString Title;
  wxString Author;
  wxString Comment;
  wxString SingleTextureNode;

  wxString Error_GameIsHooked;
  wxString Error_ProcessNotStarted;
  wxString Error_RemoveHook;

  wxString Error_FileNotSupported;
  wxString Error_FileIsEmpty;
  wxString Error_FktNotFound;
  wxString Error_D3DX9NotFound;
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
  wxString Error_NoTemplates;
  wxString Error_Memory;
  wxString Error_Unzip;
  wxString Error_ZipEntry;

  wxString CheckBoxUseSizeFilter;
  wxString CheckBoxUseFormatFilter;
  wxString SetFormatFilter;
  wxString WidthSpin;
  wxString HeightSpin;
  wxString DepthSpin;

  wxString KeyBack;
  wxString KeySave;
  wxString KeyNext;
  wxString AskForKey;
  wxString KeyNotSet;
  wxString UnknownKey;

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


extern uMod_Language *Language;


#endif /* uMod_LANGUAGE_H_ */
