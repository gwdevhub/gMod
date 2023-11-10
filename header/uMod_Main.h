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

#define WINVER _WIN32_WINNT_WINXP
#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define _WIN32_WINDOWS _WIN32_WINNT_WINXP
#define NTDDI_VERSION NTDDI_WINXP
//#define NTDDI_LONGHORN NTDDI_WINXP
#define WIN32_LEAN_AND_MEAN


#include <windows.h>

#include <cstdlib>
#include <cstdio>

#include <d3d9.h>
#include <d3dx9.h>




#include "../uMod_GlobalDefines.h"
#include "../uMod_Error.h"
#include "uMod_Defines.h"
#include "uMod_DX9_dll.h"
#include "uMod_TextureFunction.h"

#include "uMod_IDirect3D9.h"
#include "uMod_IDirect3D9Ex.h"

#include "uMod_IDirect3DDevice9.h"
#include "uMod_IDirect3DDevice9Ex.h"

#include "uMod_IDirect3DCubeTexture9.h"
#include "uMod_IDirect3DTexture9.h"
#include "uMod_IDirect3DVolumeTexture9.h"

#include "uMod_ArrayHandler.h"
#include "uMod_TextureServer.h"
#include "uMod_TextureClient.h"


extern unsigned int gl_ErrorState;

#endif
