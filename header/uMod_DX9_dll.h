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

#pragma once
#include "uMod_Main.h"

/**
 * Load the official d3d9.dll from the system path.
 */
void LoadOriginal_DX9_Dll(void);

/**
 * Initialize dx9 -> loads d3d9.dll and set detour if this dll is compiled for "Direct Injection" or "Hook Injection"
 */
void InitDX9();

/**
 * Unload the d3d9.dll
 */
void ExitDX9();

#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION

/**
 * Direct3DCreate9 function must be exported if compiled for "No Injection"
 */
IDirect3D9 *APIENTRY uMod_Direct3DCreate9(UINT SDKVersion);

/**
 * Direct3DCreate9Ex function must be exported if compiled for "No Injection"
 */
HRESULT APIENTRY uMod_Direct3DCreate9Ex( UINT SDKVersion, IDirect3D9Ex **ppD3D);

#endif
