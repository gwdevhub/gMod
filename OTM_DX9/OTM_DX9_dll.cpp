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

/*


 NEVER USE THIS CODE FOR ILLEGAL PURPOSE


*/


#include "OTM_Main.h"
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")


OTM_IDirect3DDevice9* gl_pIDirect3DDevice9 = NULL;
OTM_IDirect3D9*       gl_pIDirect3D9 = NULL;
HINSTANCE             gl_hOriginalDll = NULL;
HINSTANCE             gl_hThisInstance = NULL;
unsigned int          gl_ErrorState = 0u;
OTM_TextureServer*    gl_TextureServer = NULL;
FILE*                 gl_File =NULL;
HANDLE                gl_ServerThread = NULL;

BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  UNREFERENCED_PARAMETER(lpReserved);
    

  switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
	  InitInstance(hModule);
		break;
	}
	case DLL_PROCESS_DETACH:
	{
	  ExitInstance();
	  break;
	}
  case DLL_THREAD_ATTACH:  break;
	case DLL_THREAD_DETACH:  break;
	}
    
  return (true);
}



#ifdef NO_INJECTION
IDirect3D9* WINAPI  Direct3DCreate9(UINT SDKVersion)
{
  Message("WINAPI  Direct3DCreate9\n");

	if (!gl_hOriginalDll) LoadOriginalDll(); // looking for the "right d3d9.dll"
	
	// Hooking IDirect3D Object from Original Library
	typedef IDirect3D9 *(WINAPI* D3D9_Type)(UINT SDKVersion);
	D3D9_Type D3DCreate9_fn = (D3D9_Type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9");
    
    // Debug
	if (!D3DCreate9_fn) 
  {
	  Message("Direct3DCreate9: original function not found in dll\n");
    ExitProcess(0); // exit the hard way
  }
	

  if (gl_pIDirect3D9!=NULL) gl_ErrorState |= OTM_ERROR_MULTIPLE_IDirect3D9;

	// Request pointer from Original Dll. 
	IDirect3D9 *pIDirect3D9_orig = D3DCreate9_fn(SDKVersion);
	gl_pIDirect3D9 = new OTM_IDirect3D9( pIDirect3D9_orig, gl_TextureServer);

	// Return pointer to hooking Object instead of "real one"
	return (gl_pIDirect3D9);
}

bool HookThisProgramm( wchar_t *ret)
{
  wchar_t Executable[MAX_PATH];
  GetModuleFileNameW( GetModuleHandle( NULL ), Executable, MAX_PATH );
  PathStripPathW( Executable );
  int len = 0;
  while (Executable[len]) {ret[len] = Executable[len]; len++;}
  Executable[len] = 0;
  return (true);
}





#else


typedef IDirect3D9 *(APIENTRY *Direct3DCreate9_type)(UINT);
Direct3DCreate9_type Direct3DCreate9_fn;

bool RetourFunc(BYTE *src, BYTE *restore, const int len);
void *DetourFunc(BYTE *src, const BYTE *dst, const int len);

IDirect3D9 *APIENTRY MyDirect3DCreate9(UINT SDKVersion)
{
  Message("Direct3DCreate9_fn %lu, my %lu\n", Direct3DCreate9_fn ,MyDirect3DCreate9);

  // in the Internet are many tutorials for detouring functions and all of them will work without the following 3 marked lines
  // but somehow, for me it only works, if I retour the function and calling afterward the original function

  // BEGIN
  LoadOriginalDll();

  RetourFunc((BYTE*) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9"), (BYTE*)Direct3DCreate9_fn, 5);

  Direct3DCreate9_fn = (Direct3DCreate9_type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9");
  // END

  IDirect3D9 *pIDirect3D9_orig = NULL;
  if (Direct3DCreate9_fn)
  {
    pIDirect3D9_orig = Direct3DCreate9_fn(SDKVersion);
  }
  else return (NULL);
  if (pIDirect3D9_orig)
  {
    gl_pIDirect3D9 = new OTM_IDirect3D9( pIDirect3D9_orig, gl_TextureServer);
  }
  return (gl_pIDirect3D9);
}

bool HookThisProgramm( wchar_t *ret)
{
  //This method should be changed. Maybe the directory should be the user application directory.
  FILE* file;
  wchar_t *app_path = _wgetenv( L"APPDATA");
  wchar_t file_name[MAX_PATH];
  swprintf_s( file_name, MAX_PATH, L"%ls\\%ls\\%ls", app_path, OTM_APP_DIR, OTM_APP_DX9);
  if (_wfopen_s( &file, file_name, L"rt")) return (false);

  wchar_t Executable[MAX_PATH];
  wchar_t Game[MAX_PATH];
  GetModuleFileNameW( GetModuleHandle( NULL ), Executable, MAX_PATH );
  PathStripPathW( Executable );

  while (!feof(file))
  {
    if ( fgetws ( Game, MAX_PATH, file) != NULL )
    {
      int len = 0;
      while (Game[len])
      {
        if (Game[len]=='\r' || Game[len]=='\n') {Game[len]=0; break;}
        len++;
      }
      if ( _wcsicmp( Executable, Game ) == 0 )
      {
        for (int i=0; i<len; i++) ret[i] = Game[i];
        fclose(file);
        return (true);
      }
    }
  }
  fclose(file);
  return (false);
}
#endif


DWORD WINAPI ServerThread( LPVOID lpParam )
{
  UNREFERENCED_PARAMETER(lpParam);
  if (gl_TextureServer!=NULL) gl_TextureServer->MainLoop();
  return (0);
}

void InitInstance(HINSTANCE hModule)
{
  DisableThreadLibraryCalls( hModule );

  gl_hThisInstance = (HINSTANCE)  hModule;

  wchar_t game[MAX_PATH];
  if (HookThisProgramm( game))
  {
    OpenMessage();
    Message("InitInstance: %lu\n", hModule);

	  gl_TextureServer = new OTM_TextureServer(game);
	  if (gl_TextureServer!=NULL)
	  {
	    if (gl_TextureServer->OpenPipe())
	    {
	      Message("InitInstance: Pipe not opened\n");
	      return;
	    }
	    gl_ServerThread = CreateThread( NULL, 0, ServerThread, NULL, 0, NULL);
	    if (gl_ServerThread==NULL) Message("InitInstance: Serverthread not started\n");

	    //this is for testing purpose, these function should be called from the server thread, provoked by the OTM_GUI
	    //gl_TextureServer->SaveAllTextures(true);
      //gl_TextureServer->SetSaveDirectory("tex\\");
	    /*
	    gl_TextureServer->AddFile("BF_0xbc2a9196.dds", 0xbc2a9196ul);
      gl_TextureServer->AddFile("0X1FD33669.dds", 0X1FD33669ul);
      gl_TextureServer->AddFile("0X26D19B9A.dds", 0X26D19B9Aul);
      gl_TextureServer->AddFile("0X72E92068.dds", 0X72E92068ul);
      gl_TextureServer->AddFile("0X714DFA26.dds", 0X714DFA26ul);
      gl_TextureServer->AddFile("0X74499208.dds", 0X74499208ul);
      gl_TextureServer->AddFile("0XA3BFD8EA.dds", 0XA3BFD8EAul);
      */
	  }
	  LoadOriginalDll();

#ifndef NO_INJECTION
	  // we detour the original Direct3DCreate9 to our MyDirect3DCreate9
	  Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc(
	          (BYTE*)GetProcAddress(gl_hOriginalDll, "Direct3DCreate9"),
	          (BYTE*)MyDirect3DCreate9,
	          5);
#endif
  }
}


void LoadOriginalDll(void)
{
  char buffer[MAX_PATH];
	GetSystemDirectory(buffer,MAX_PATH); //get the system directory, we need to open the original d3d9.dll

	// Append dll name
	strcat_s( buffer, MAX_PATH,"\\d3d9.dll");
	
	// try to load the system's d3d9.dll, if pointer empty
	if (!gl_hOriginalDll) gl_hOriginalDll = LoadLibrary(buffer);

	// Debug
	if (!gl_hOriginalDll)
	{
		ExitProcess(0); // exit the hard way
	}
}

void ExitInstance() 
{
  if (gl_TextureServer!=NULL)
  {
    gl_TextureServer->ClosePipe(); //This must be done before the server thread is killed, because the server thread will endless wait on the ReadFile()
  }
  if (gl_ServerThread!=NULL)
  {
    CloseHandle(gl_ServerThread); // kill the server thread
    gl_ServerThread = NULL;
  }
  if (gl_TextureServer!=NULL)
  {
    delete gl_TextureServer; //delete the texture server
    gl_TextureServer = NULL;
  }

	// Release the system's d3d9.dll
	if (gl_hOriginalDll!=NULL)
	{
		FreeLibrary(gl_hOriginalDll);
	  gl_hOriginalDll = NULL;
	}

  CloseMessage();
}


#ifndef NO_INJECTION
void *DetourFunc(BYTE *src, const BYTE *dst, const int len)
{
  BYTE *jmp = (BYTE*)malloc(len+5);
  DWORD dwback = 0;
  VirtualProtect(jmp, len+5, PAGE_EXECUTE_READWRITE, &dwback); //This is the addition needed for Windows 7 RC
  VirtualProtect(src, len, PAGE_READWRITE, &dwback);
  memcpy(jmp, src, len);    jmp += len;
  jmp[0] = 0xE9;
  *(DWORD*)(jmp+1) = (DWORD)(src+len - jmp) - 5;
  memset(src, 0x90, len);
  src[0] = 0xE9;
  *(DWORD*)(src+1) = (DWORD)(dst - src) - 5;
  VirtualProtect(src, len, dwback, &dwback);
  return (jmp-len);
}

bool RetourFunc(BYTE *src, BYTE *restore, const int len)
{
  DWORD dwback;
  if(!VirtualProtect(src, len, PAGE_READWRITE, &dwback))  { return (false); }
  if(!memcpy(src, restore, len))              { return (false); }
  restore[0] = 0xE9;
  *(DWORD*)(restore+1) = (DWORD)(src - restore) - 5;
  if(!VirtualProtect(src, len, dwback, &dwback))      { return (false); }
  return (true);
}



HHOOK gl_hHook = NULL;
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    return (CallNextHookEx( gl_hHook, nCode, wParam, lParam));
}

void InstallHook()
{
  gl_hHook = SetWindowsHookEx( WH_CBT, HookProc, gl_hThisInstance, 0 );
}

void RemoveHook()
{
    UnhookWindowsHookEx( gl_hHook );
}
#endif
