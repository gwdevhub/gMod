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

#ifndef UMOD_DETOUR_H_
#define UMOD_DETOUR_H_

#include "uMod_DetourEntry.h"
#include <windows.h>

#ifdef __CDT_PARSER__
#define NULL 0
#endif

typedef BOOL (WINAPI *FreeLibrary_type)( HMODULE hModule);
typedef HMODULE (WINAPI *LoadLibraryA_type)( LPCSTR lpFileName);
typedef HMODULE (WINAPI *LoadLibraryW_type)( LPCWSTR lpFileName);
typedef HMODULE (WINAPI *LoadLibraryExA_type)( LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags);
typedef HMODULE (WINAPI *LoadLibraryExW_type)( LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags);

BOOL WINAPI uMod_FreeLibrary( HMODULE hModule);
HMODULE WINAPI uMod_LoadLibraryA( LPCSTR lpFileName);
HMODULE WINAPI uMod_LoadLibraryW( LPCWSTR lpFileName);
HMODULE WINAPI uMod_LoadLibraryExA( LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags);
HMODULE WINAPI uMod_LoadLibraryExW( LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags);

class uMod_Detour
{
public:
  uMod_Detour();
  ~uMod_Detour();


  void Init();
  void Exit();

  int AddEntry( uMod_Detour_Entry_Base* entry);


  void FreeLib( HMODULE plib);
  void TestLib(const char* lib_name, HMODULE plib);
  void TestLib(const wchar_t* lib_name, HMODULE plib);


  uMod_Detour_Entry<FreeLibrary_type> Detour_FreeLibrary;
  uMod_Detour_Entry<LoadLibraryA_type> Detour_LoadLibraryA;
  uMod_Detour_Entry<LoadLibraryW_type> Detour_LoadLibraryW;
  uMod_Detour_Entry<LoadLibraryExA_type> Detour_LoadLibraryExA;
  uMod_Detour_Entry<LoadLibraryExW_type> Detour_LoadLibraryExW;

private:
  int NumOfEntries;
  uMod_Detour_Entry_Base** Entries;
};

extern uMod_Detour GlobalDetour;

#endif /* UMOD_DETOUR_H_ */
