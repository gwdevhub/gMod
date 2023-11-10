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




#include "uMod_GlobalDefines.h"
#include "uMod_Error.h"
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
