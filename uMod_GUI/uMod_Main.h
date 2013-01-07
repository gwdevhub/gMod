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



#ifndef uMod_MAIN_H_
#define uMod_MAIN_H_


// I use eclipse and somehow i need these define or many of the wx classes are unknown to the CDT parser
#ifdef __CDT_PARSER__
#define HAVE_W32API_H
#define __WXMSW__
#define NOPCH
#define _UNICODE
#define wxUSE_GUI 1
#define wxUSE_NOTEBOOK 1
#define wxUSE_CHECKBOX 1
#define wxUSE_THREADS 1
#define wxUSE_MSGDLG 1
#define wxUSE_MENUS 1
#define wxUSE_BUTTON 1
#define wxUSE_FILEDLG 1
#define wxUSE_DIRDLG 1
#define wxUSE_CHOICE 1
#define wxUSE_FILE 1
#define wxUSE_TEXTCTRL 1
#define wxUSE_CHOICEDLG 1
#define wxUSE_TOOLTIPS 1
#define wxUSE_DATAVIEWCTRL 1
#define wxUSE_COLLPANE 1
#define wxUSE_COLOURDLG 1
#define wxUSE_HYPERLINKCTRL 1
#endif
#define wxUSE_SPINCTRL 1

#define WINVER _WIN32_WINNT_WINXP
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define NTDDI_VERSION NTDDI_WINXP
#define WIN32_LEAN_AND_MEAN


#include <wx\wx.h>
#include <wx\notebook.h>
#include <wx/file.h>
#include <wx/dir.h>
#include <wx/tokenzr.h>
#include <wx/dynlib.h>
#include <wx/dataview.h>
#include <wx/thread.h>
#include <wx/checkbox.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/choice.h>
#include <wx/textctrl.h>
#include <wx/choicdlg.h>
#include <wx/collpane.h>
#include <wx/colourdata.h>
#include <wx/colordlg.h>
#include <wx/spinctrl.h>
#include <wx/hyperlink.h>
//#include <windows.h>

#include "../uMod_GlobalDefines.h"
#include "../uMod_Error.h"
#include "../uMod_Utils.h"


class uMod_Frame;

#define MAX_TEXTURES 1024
enum
{
  ID_Menu_Exit = wxID_HIGHEST,
  ID_Menu_Lang,
  ID_Menu_Help,
  ID_Menu_About,
  ID_Menu_Acknowledgement,
  ID_Menu_StartGame,
  ID_Menu_StartGameCMD,
  ID_Menu_UseHook,
  ID_Menu_AddGame,
  ID_Menu_DeleteGame,
  ID_Menu_LoadTemplate,
  ID_Menu_SaveTemplate,
  ID_Menu_SaveTemplateAs,
  ID_Menu_SetDefaultTemplate,
  ID_Add_Game,
  ID_Delete_Game,
  ID_Add_Device,
  ID_Delete_Device,

  ID_OpenPackage,
  ID_RemovePackage,
  ID_RemoveSelectedPackages,
  ID_Update,
  ID_Reload,
  ID_SupportTPF,
  ID_Button_Update,

  ID_CollPane,
  ID_SaveSingleTexture,
  ID_SaveAllTexture,
  ID_ShowSingleTextureString,
  ID_FontColour,
  ID_TextureColour,

  ID_KeyPress,

  ID_UseSizeFilter,
  ID_SpinMin, ID_SpinMax,
  ID_UseFormatFilter,
  ID_SetFormatFilter,
  ID_Button_SavePath,

  ID_Button_ExtractPath,

  ID_ExtractTextures,

  ID_DataViewCtrl,
  ID_ContextMenu,
};

#define ABORT_SERVER L"uMod_Abort_Server"
#define uMod_d3d9_Hook_dll L"uMod_d3d9_HI.dll"
#define uMod_d3d9_DI_dll L"uMod_d3d9_DI.dll"

#include "uMod_Settings.h"
#include "uMod_ModElement.h"
#include "uMod_TreeView.h"
#include "uMod_AddTexture.h"
#include "uMod_Language.h"
#include "uMod_Dialogs.h"
#include "uMod_Event.h"
#include "uMod_Client.h"
#include "uMod_GameInfo.h"
#include "uMod_File.h"
#include "uMod_Sender.h"
#include "uMod_Server.h"
#include "uMod_MiniPanels.h"
#include "uMod_GamePage.h"
#include "uMod_DirectInjection.h"
#include "uMod_GUI.h"

#endif
