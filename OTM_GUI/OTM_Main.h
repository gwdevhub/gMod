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



#ifndef OTM_MAIN_H_
#define OTM_MAIN_H_


// I use eclipse and somehow i need these define or many of the wx classes are unknown to the CDT parser
#ifdef __CDT_PARSER__
#define HAVE_W32API_H
#define __WXMSW__
#define NOPCH
#define _UNICODE
#define wxUSE_NOTEBOOK 1
#define wxUSE_CHECKBOX 1
#define wxUSE_THREADS 1
#define wxUSE_MSGDLG 1
#define wxUSE_MENUS 1
#define wxUSE_BUTTON 1
#define wxUSE_FILEDLG 1
#define wxUSE_DIRDLG 1
#define wxUSE_CHOICE 1
#endif


#include "wx\wx.h"
#include "wx\notebook.h"
//#include <wx/thread.h>
//#include "wx/checkbox.h"
//#include <wx/msgdlg.h>
//#include <wx/menu.h>
//#include <wx/button.h>
//#include  <wx/filedlg.h>
//#include <wx/choice.h>

#include <windows.h>

#include "../OTM_GlobalDefines.h"
#include "../OTM_Error.h"


class OTM_Frame;

enum
{
  ID_Button_Open = wxID_HIGHEST,
  ID_Button_Path,
  ID_Button_Update,
  ID_Menu_Pref,
  ID_Menu_Quit,
  ID_Menu_Help,
  ID_Menu_About,
  ID_Add_Game,
  ID_Delete_Game
};

#define ABORT_SERVER L"OTM_Abort_Server"
#define OTM_d3d9_dll L"OTM_d3d9.dll"

#include "OTM_Language.h"
#include "OTM_Event.h"
#include "OTM_Sender.h"
#include "OTM_Client.h"
#include "OTM_GameInfo.h"
#include "OTM_Server.h"
#include "OTM_GamePage.h"
#include "OTM_GUI.h"

#endif
