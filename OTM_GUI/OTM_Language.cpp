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


OTM_Language::OTM_Language(void)
{
  LoadLanguage(0);  //English
}

int OTM_Language::LoadLanguage(int lang)
{
  if (lang==0) //English
  {
    //MenuPref = L"Preferences";
    //MenuQuit = L"Quit";
    MenuHelp  = L"Help";
    MenuAbout  = L"About";
    MenuAddGame = L"Add game";
    MenuDeleteGame = L"Delete Game";

    //MainMenuStart = L"Start";
    MainMenuGame = L"Game";
    MainMenuHelp = L"Help";

    ButtonOpen = L"Open texture";
    ButtonDirectory = L"Set save directory";
    ButtonUpdate = L"Update";
    ButtonSave = L"Save as default";

    ChooseFile = L"Choose a texture file";
    ChooseDir = L"Choose a directory";

    CheckBoxSaveSingleTexture = L"Save single texture";
    CheckBoxSaveAllTextures = L"Save all textures";
    TextCtrlSavePath = L"Save path: ";

    DeleteGame = L"Select the games to be deleted.";

    GameAlreadyAdded = L"Game has been already added.";
    FileNotSupported = L"This file type is not supported:\n";
    ExitGameAnyway = L"Closing OpenTexMod while a game is running might lead to a crash of the game.\nExit anyway?";

    Error_DLLNotFound = L"Could not load the dll.\nThe dll injection won't work.\nThis might happen if D3DX9_43.dll is not installed on your system.\nPlease install the newest DirectX End-User Runtime Web Installer.";
    Error_FktNotFound = L"Could not load function out of dll.\nThe dll injection won't work.";

    Error_Send = L"Could not send to game.";
    Error_SaveFile = L"Could not save to file.";
    Error_NoPipe = L"Pipe is not opened.";
    Error_WritePipe = L"Could not write in pipe.";
    Error_FlushPipe = L"Could not flush pipe buffer.";
    Error_Hash = L"Could not find hash, maybe file is not named as *_HASH.dds";
    Error_FileOpen = L"Could not open file.";
    Error_FileRead = L"Could not read file.";
    Error_Memory = L"Could not allocate enough memory";
    Error_FileformatNotSupported = L"This file format is not supported.";
    Error_Unzip = L"Could not unzip.";
    Error_ZipEntry = L"Could not find zip entry.";

    FontColour = L"Font colour (RGB):";
    TextureColour = L"Texture colour (RGB):";
  }

  LoadKeys(lang);
  return 0;
}
#define QUOTES(x) #x

#define AddKey( name, key ) \
{ \
  KeyStrings.Add( name ); \
  KeyValues.Add( key ); \
}

int OTM_Language::LoadKeys(int lang)
{
  if (lang==0) //English
  {
    KeyBack = L"Back";
    KeySave = L"Save";
    KeyNext = L"Next";
  }

  KeyStrings.Empty();
  KeyValues.Empty();
/*

#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    // NOT contiguous with L & RBUTTON

#if(_WIN32_WINNT >= 0x0500)
#define VK_XBUTTON1       0x05    // NOT contiguous with L & RBUTTON
#define VK_XBUTTON2       0x06    // NOT contiguous with L & RBUTTON
#endif // _WIN32_WINNT >= 0x0500


// * 0x07 : unassigned

 */
  AddKey( "VK_BACK", VK_BACK );
  AddKey( "VK_TAB", VK_TAB );
  AddKey( "VK_CLEAR", VK_CLEAR );
  AddKey( "VK_RETURN", VK_RETURN );
  AddKey( "VK_SHIFT", VK_SHIFT );
  AddKey( "VK_CONTROL", VK_CONTROL );
  AddKey( "VK_MENU", VK_MENU );
  AddKey( "VK_PAUSE", VK_PAUSE );
  AddKey( "VK_CAPITAL", VK_CAPITAL );
  //AddKey(  );
  //AddKey(  );
  //AddKey(  );
  //AddKey(  );
  /*
#define VK_BACK           0x08
#define VK_TAB            0x09


// 0x0A - 0x0B : reserved


#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D

#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14

#define VK_KANA           0x15
#define VK_HANGEUL        0x15  // old name - should be here for compatibility
#define VK_HANGUL         0x15
#define VK_JUNJA          0x17
#define VK_FINAL          0x18
#define VK_HANJA          0x19
#define VK_KANJI          0x19

*/

  AddKey( "VK_ESCAPE", VK_ESCAPE );
/*
#define VK_ESCAPE         0x1B

#define VK_CONVERT        0x1C
#define VK_NONCONVERT     0x1D
#define VK_ACCEPT         0x1E
#define VK_MODECHANGE     0x1F
*/

  AddKey( "VK_SPACE", VK_SPACE );
  AddKey( "VK_PRIOR", VK_PRIOR );
  AddKey( "VK_NEXT", VK_NEXT );
  AddKey( "VK_END", VK_END );
  AddKey( "VK_HOME", VK_HOME );
  AddKey( "VK_LEFT", VK_LEFT );
  AddKey( "VK_UP", VK_UP );
  AddKey( "VK_RIGHT", VK_RIGHT );
  AddKey( "VK_DOWN", VK_DOWN );
  AddKey( "VK_SELECT", VK_SELECT );
  AddKey( "VK_PRINT", VK_PRINT );
  AddKey( "VK_EXECUTE", VK_EXECUTE );
  AddKey( "VK_SNAPSHOT", VK_SNAPSHOT );
  AddKey( "VK_INSERT", VK_INSERT );
  AddKey( "VK_DELETE", VK_DELETE );
  AddKey( "VK_HELP", VK_HELP );
/*
#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F


// * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
// * 0x40 : unassigned
// * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
*/
  int count  = 0x30;
  AddKey( "0", count++);
  AddKey( "1", count++);
  AddKey( "2", count++);
  AddKey( "3", count++);
  AddKey( "4", count++);
  AddKey( "5", count++);
  AddKey( "6", count++);
  AddKey( "7", count++);
  AddKey( "8", count++);
  AddKey( "9", count++);

  count = 0x41;
  AddKey( "a", count++);
  AddKey( "b", count++);
  AddKey( "c", count++);
  AddKey( "d", count++);
  AddKey( "e", count++);
  AddKey( "f", count++);
  AddKey( "g", count++);
  AddKey( "h", count++);
  AddKey( "i", count++);
  AddKey( "j", count++);
  AddKey( "k", count++);
  AddKey( "l", count++);
  AddKey( "m", count++);
  AddKey( "n", count++);
  AddKey( "o", count++);
  AddKey( "p", count++);
  AddKey( "q", count++);
  AddKey( "r", count++);
  AddKey( "s", count++);
  AddKey( "t", count++);
  AddKey( "u", count++);
  AddKey( "v", count++);
  AddKey( "w", count++);
  AddKey( "x", count++);
  AddKey( "y", count++);
  AddKey( "z", count++);

/*

#define VK_LWIN           0x5B
#define VK_RWIN           0x5C
#define VK_APPS           0x5D


// 0x5E : reserved
*/


  AddKey( "VK_SLEEP", VK_SLEEP );
  AddKey( "VK_NUMPAD0", VK_NUMPAD0 );
  AddKey( "VK_NUMPAD1", VK_NUMPAD1 );
  AddKey( "VK_NUMPAD2", VK_NUMPAD2 );
  AddKey( "VK_NUMPAD3", VK_NUMPAD3 );
  AddKey( "VK_NUMPAD4", VK_NUMPAD4 );
  AddKey( "VK_NUMPAD5", VK_NUMPAD5 );
  AddKey( "VK_NUMPAD6", VK_NUMPAD6 );
  AddKey( "VK_NUMPAD7", VK_NUMPAD7 );
  AddKey( "VK_NUMPAD8", VK_NUMPAD8 );
  AddKey( "VK_NUMPAD9", VK_NUMPAD9 );
  AddKey( "VK_MULTIPLY", VK_MULTIPLY );
  AddKey( "VK_ADD", VK_ADD );
  AddKey( "VK_SEPARATOR", VK_SEPARATOR );
  AddKey( "VK_SUBTRACT", VK_SUBTRACT );
  AddKey( "VK_DECIMAL", VK_DECIMAL );
  AddKey( "VK_DIVIDE", VK_DIVIDE );
  AddKey( "VK_F1", VK_F1 );
  AddKey( "VK_F2", VK_F2 );
  AddKey( "VK_F3", VK_F3 );
  AddKey( "VK_F4", VK_F4 );
  AddKey( "VK_F5", VK_F5 );
  AddKey( "VK_F6", VK_F6 );
  AddKey( "VK_F7", VK_F7 );
  AddKey( "VK_F8", VK_F8 );
  AddKey( "VK_F9", VK_F9 );
  AddKey( "VK_F10", VK_F10 );
  AddKey( "VK_F12", VK_F12 );
  AddKey( "VK_F12", VK_F12 );

  /*

#define VK_SLEEP          0x5F

#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87


// 0x88 - 0x8F : unassigned
*/

  AddKey( "VK_NUMLOCK", VK_NUMLOCK );
  AddKey( "VK_SCROLL", VK_SCROLL );
/*

#define VK_NUMLOCK        0x90
#define VK_SCROLL         0x91


// NEC PC-9800 kbd definitions

#define VK_OEM_NEC_EQUAL  0x92   // '=' key on numpad


// Fujitsu/OASYS kbd definitions

#define VK_OEM_FJ_JISHO   0x92   // 'Dictionary' key
#define VK_OEM_FJ_MASSHOU 0x93   // 'Unregister word' key
#define VK_OEM_FJ_TOUROKU 0x94   // 'Register word' key
#define VK_OEM_FJ_LOYA    0x95   // 'Left OYAYUBI' key
#define VK_OEM_FJ_ROYA    0x96   // 'Right OYAYUBI' key

//
// 0x97 - 0x9F : unassigned
//

//
// VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
// Used only as parameters to GetAsyncKeyState() and GetKeyState().
// No other API or message will distinguish left and right keys in this way.

#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5

#if(_WIN32_WINNT >= 0x0500)
#define VK_BROWSER_BACK        0xA6
#define VK_BROWSER_FORWARD     0xA7
#define VK_BROWSER_REFRESH     0xA8
#define VK_BROWSER_STOP        0xA9
#define VK_BROWSER_SEARCH      0xAA
#define VK_BROWSER_FAVORITES   0xAB
#define VK_BROWSER_HOME        0xAC

#define VK_VOLUME_MUTE         0xAD
#define VK_VOLUME_DOWN         0xAE
#define VK_VOLUME_UP           0xAF
#define VK_MEDIA_NEXT_TRACK    0xB0
#define VK_MEDIA_PREV_TRACK    0xB1
#define VK_MEDIA_STOP          0xB2
#define VK_MEDIA_PLAY_PAUSE    0xB3
#define VK_LAUNCH_MAIL         0xB4
#define VK_LAUNCH_MEDIA_SELECT 0xB5
#define VK_LAUNCH_APP1         0xB6
#define VK_LAUNCH_APP2         0xB7

#endif // _WIN32_WINNT >= 0x0500


// 0xB8 - 0xB9 : reserved


#define VK_OEM_1          0xBA   // ';:' for US
#define VK_OEM_PLUS       0xBB   // '+' any country
#define VK_OEM_COMMA      0xBC   // ',' any country
#define VK_OEM_MINUS      0xBD   // '-' any country
#define VK_OEM_PERIOD     0xBE   // '.' any country
#define VK_OEM_2          0xBF   // '/?' for US
#define VK_OEM_3          0xC0   // '`~' for US


// 0xC1 - 0xD7 : reserved


// 0xD8 - 0xDA : unassigned


#define VK_OEM_4          0xDB  //  '[{' for US
#define VK_OEM_5          0xDC  //  '\|' for US
#define VK_OEM_6          0xDD  //  ']}' for US
#define VK_OEM_7          0xDE  //  ''"' for US
#define VK_OEM_8          0xDF


// 0xE0 : reserved


/
// Various extended or enhanced keyboards

#define VK_OEM_AX         0xE1  //  'AX' key on Japanese AX kbd
#define VK_OEM_102        0xE2  //  "<>" or "\|" on RT 102-key kbd.
#define VK_ICO_HELP       0xE3  //  Help key on ICO
#define VK_ICO_00         0xE4  //  00 key on ICO

#if(WINVER >= 0x0400)
#define VK_PROCESSKEY     0xE5
#endif // WINVER >= 0x0400

#define VK_ICO_CLEAR      0xE6


#if(_WIN32_WINNT >= 0x0500)
#define VK_PACKET         0xE7
#endif // _WIN32_WINNT >= 0x0500


// 0xE8 : unassigned


//Nokia/Ericsson definitions

#define VK_OEM_RESET      0xE9
#define VK_OEM_JUMP       0xEA
#define VK_OEM_PA1        0xEB
#define VK_OEM_PA2        0xEC
#define VK_OEM_PA3        0xED
#define VK_OEM_WSCTRL     0xEE
#define VK_OEM_CUSEL      0xEF
#define VK_OEM_ATTN       0xF0
#define VK_OEM_FINISH     0xF1
#define VK_OEM_COPY       0xF2
#define VK_OEM_AUTO       0xF3
#define VK_OEM_ENLW       0xF4
#define VK_OEM_BACKTAB    0xF5

#define VK_ATTN           0xF6
#define VK_CRSEL          0xF7
#define VK_EXSEL          0xF8
#define VK_EREOF          0xF9
#define VK_PLAY           0xFA
#define VK_ZOOM           0xFB
#define VK_NONAME         0xFC
#define VK_PA1            0xFD
#define VK_OEM_CLEAR      0xFE


 */
  return 0;
}

