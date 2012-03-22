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



#ifndef uMod_DXMAIN_DLL_H_
#define uMod_DXMAIN_DLL_H_

#ifdef DEF_USE_DX9
#include "../uMod_DX9/uMod_DX9_dll.h"
#endif

void InitInstance(HINSTANCE hModule);
void ExitInstance(void);

bool HookThisProgram( wchar_t *ret);
DWORD WINAPI ServerThread( LPVOID lpParam);



#ifndef NO_INJECTION

void *DetourFunc(BYTE *src, const BYTE *dst, const int len);
bool RetourFunc(BYTE *src, BYTE *restore, const int len);


#ifdef HOOK_INJECTION
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
void InstallHook(void);
void RemoveHook(void);
#endif


#ifdef DIRECT_INJECTION
void Nothing(void);
#endif

#endif

#endif
