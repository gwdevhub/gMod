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



#ifndef uMod_DX9_DLL_H_
#define uMod_DX9_DLL_H_

void InitInstance(HINSTANCE hModule);
void ExitInstance(void);
void LoadOriginalDll(void);
bool HookThisProgram( wchar_t *ret);
DWORD WINAPI ServerThread( LPVOID lpParam);



#ifndef NO_INJECTION

void *DetourFunc(BYTE *src, const BYTE *dst, const int len);
bool RetourFunc(BYTE *src, BYTE *restore, const int len);
IDirect3D9 *APIENTRY uMod_Direct3DCreate9(UINT SDKVersion);
HRESULT APIENTRY uMod_Direct3DCreate9Ex( UINT SDKVersion, IDirect3D9Ex **ppD3D);

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
