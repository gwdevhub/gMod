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



#define WIN32_LEAN_AND_MEAN		
#include <windows.h>

#include <cstdlib>
#include <cstdio>

#include <d3d9.h>
#include <d3dx9.h>



#include "../OTM_GlobalDefines.h"
#include "../OTM_Error.h"
#include "OTM_Defines.h"
#include "OTM_DX9_dll.h"
#include "OTM_IDirect3D9.h"
#include "OTM_IDirect3DDevice9.h"
#include "OTM_IDirect3DTexture9.h"
#include "OTM_ArrayHandler.h"
#include "OTM_TextureServer.h"
#include "OTM_TextureClient.h"



extern OTM_IDirect3DDevice9 *gl_pIDirect3DDevice9;
extern OTM_IDirect3D9       *gl_pIDirect3D9;
extern HINSTANCE             gl_hOriginalDll;
extern HINSTANCE             gl_hThisInstance;
extern unsigned int          gl_ErrorState;
extern FILE                 *gl_File;

#endif
