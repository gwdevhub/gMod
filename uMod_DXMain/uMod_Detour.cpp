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

#include "uMod_Detour.h"
#include "uMod_Main.h"



uMod_Detour::uMod_Detour() : NumOfEntries(0), Entries(NULL),
    Detour_FreeLibrary(5),
    Detour_LoadLibraryA(5),
    Detour_LoadLibraryW(5),
    Detour_LoadLibraryExA(5),
    Detour_LoadLibraryExW(5)
{

}

uMod_Detour::~uMod_Detour()
{
  if (Entries!=NULL) delete [] Entries;
}


void uMod_Detour::Init(void)
{
  Detour_FreeLibrary.SetOrigFunction(FreeLibrary);
  Detour_FreeLibrary.SetTargetFunction(uMod_FreeLibrary);
  Detour_FreeLibrary.Detour();

  Detour_LoadLibraryA.SetOrigFunction(LoadLibraryA);
  Detour_LoadLibraryA.SetTargetFunction(uMod_LoadLibraryA);
  Detour_LoadLibraryA.Detour();

  Detour_LoadLibraryW.SetOrigFunction(LoadLibraryW);
  Detour_LoadLibraryW.SetTargetFunction(uMod_LoadLibraryW);
  Detour_LoadLibraryW.Detour();

  Detour_LoadLibraryExA.SetOrigFunction(LoadLibraryExA);
  Detour_LoadLibraryExA.SetTargetFunction(uMod_LoadLibraryExA);
  Detour_LoadLibraryExA.Detour();

  Detour_LoadLibraryExW.SetOrigFunction(LoadLibraryExW);
  Detour_LoadLibraryExW.SetTargetFunction(uMod_LoadLibraryExW);
  Detour_LoadLibraryExW.Detour();


  //for (int i=0; i<NumOfEntries; i++)
  //  Entries[i]->Detour();
}

void uMod_Detour::Exit(void)
{
  Detour_FreeLibrary.Retour();
  Detour_LoadLibraryA.Retour();
  Detour_LoadLibraryW.Retour();
  Detour_LoadLibraryExA.Retour();
  Detour_LoadLibraryExW.Retour();

  //for (int i=0; i<NumOfEntries; i++)
  //  Entries[i]->Retour();
}

int uMod_Detour::AddEntry( uMod_Detour_Entry_Base* entry)
{
  uMod_Detour_Entry_Base** temp = new uMod_Detour_Entry_Base*[NumOfEntries+1];

  if (Entries!=NULL)
  {
    for (int i=0; i<NumOfEntries; i++)
      temp[i] = Entries[i];
    delete [] Entries;
  }
  Entries = temp;

  Entries[NumOfEntries] = entry;

  NumOfEntries++;

  return 0;
}


void uMod_Detour::FreeLib( HMODULE plib)
{
  Message("uMod_Detour::FreeLib( %p )\n", plib);
  if (plib==NULL) return;

  for (int i=0; i<NumOfEntries; i++)
    Entries[i]->FreeLib( plib);
}

void uMod_Detour::TestLib(const char* lib_name, HMODULE plib)
{
  Message("uMod_Detour::TestLib( %s, %p )\n", lib_name, plib);
  if (lib_name==NULL) return;
  if (plib==NULL) return;

  for (int i=0; i<NumOfEntries; i++)
    Entries[i]->TestLib( lib_name, plib);
}

void uMod_Detour::TestLib(const wchar_t* lib_name, HMODULE plib)
{
  Message("uMod_Detour::TestLib( %ls, %p )\n", lib_name, plib);
  if (lib_name==NULL) return;
  if (plib==NULL) return;

  for (int i=0; i<NumOfEntries; i++)
    Entries[i]->TestLib( lib_name, plib);
}


uMod_Detour GlobalDetour;


BOOL WINAPI uMod_FreeLibrary( HMODULE hModule)
{
  GlobalDetour.Detour_FreeLibrary.Retour(); //retour the FreeLibrary function

  BOOL ret = FreeLibrary( hModule); // now call the original function
  Message("%d = uMod_FreeLibrary( %p )\n", ret, hModule);

  GlobalDetour.FreeLib(hModule); // notify all detoured function, that this library was once more freed

  GlobalDetour.Detour_FreeLibrary.Detour(); // detour again the FreeLibrary function

  return ret;
}

HMODULE WINAPI uMod_LoadLibraryA( LPCSTR lpFileName)
{
  GlobalDetour.Detour_LoadLibraryA.Retour(); //retour the LoadLibraryA function

  HMODULE ret = LoadLibraryA( lpFileName); // now call the original function
  Message("%p = uMod_LoadLibraryA( %s )\n", ret, lpFileName);

  GlobalDetour.TestLib( lpFileName, ret); // notify all detoured function, that this library was once more loaded

  GlobalDetour.Detour_LoadLibraryA.Detour(); // detour again the LoadLibraryA function

  return ret;
}

HMODULE WINAPI uMod_LoadLibraryW( LPCWSTR lpFileName)
{
  GlobalDetour.Detour_LoadLibraryW.Retour(); //retour the LoadLibraryW function

  HMODULE ret = LoadLibraryW( lpFileName); // now call the original function
  GlobalDetour.TestLib( lpFileName, ret); // notify all detoured function, that this library was once more loaded
  Message("%p = uMod_LoadLibraryW( %ls )\n", ret, lpFileName);

  GlobalDetour.Detour_LoadLibraryW.Detour(); // detour again the LoadLibraryW function

  return ret;
}

HMODULE WINAPI uMod_LoadLibraryExA( LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
  GlobalDetour.Detour_LoadLibraryExA.Retour(); //retour the LoadLibraryExA function

  HMODULE ret = LoadLibraryExA( lpFileName, hFile, dwFlags); // now call the original function
  Message("%p = uMod_LoadLibraryExA( %s, %p, %d)\n", ret, lpFileName, hFile, dwFlags);

  if (dwFlags==0)
  {
    GlobalDetour.TestLib( lpFileName, ret); // notify all detoured function, that this library was once more loaded
  }

  GlobalDetour.Detour_LoadLibraryExA.Detour(); // detour again the LoadLibraryExA function

  return ret;
}

HMODULE WINAPI uMod_LoadLibraryExW( LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
  GlobalDetour.Detour_LoadLibraryExW.Retour(); //retour the LoadLibraryExA function

  HMODULE ret = LoadLibraryExW( lpFileName, hFile, dwFlags); // now call the original function
  Message("%p = uMod_LoadLibraryExW( %ls, %p, %d)\n", ret, lpFileName, hFile, dwFlags);

  if (dwFlags==0)
  {
    GlobalDetour.TestLib( lpFileName, ret); // notify all detoured function, that this library was once more loaded
  }

  GlobalDetour.Detour_LoadLibraryExW.Detour(); // detour again the LoadLibraryExA function

  return ret;
}

