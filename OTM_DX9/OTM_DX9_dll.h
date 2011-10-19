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
along with OpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef OTM_DX9_DLL_H_
#define OTM_DX9_DLL_H_

void InitInstance(HINSTANCE hModule);
void ExitInstance(void);
void LoadOriginalDll(void);
bool HookThisProgram( wchar_t *ret);
DWORD WINAPI ServerThread( LPVOID lpParam);



#ifndef NO_INJECTION

void *DetourFunc(BYTE *src, const BYTE *dst, const int len);
bool RetourFunc(BYTE *src, BYTE *restore, const int len);
IDirect3D9 *APIENTRY MyDirect3DCreate9(UINT SDKVersion);

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam);
void InstallHook(void);
void RemoveHook(void);

#endif

#endif
