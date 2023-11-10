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

/*


 NEVER USE THIS CODE FOR ILLEGAL PURPOSE


*/


#include "uMod_Main.h"
//#include "detours.h"
//#include "detourxs/detourxs/detourxs.h"

/*
#include "detourxs/detourxs/ADE32.cpp"
#include "detourxs/detourxs/detourxs.cpp"
*/
/*
 * global variable which are not linked external
 */
HINSTANCE             gl_hOriginalDll = NULL;
HINSTANCE             gl_hThisInstance = NULL;
uMod_TextureServer*    gl_TextureServer = NULL;
HANDLE                gl_ServerThread = NULL;

typedef IDirect3D9 *(APIENTRY *Direct3DCreate9_type)(UINT);
typedef HRESULT (APIENTRY *Direct3DCreate9Ex_type)(UINT SDKVersion, IDirect3D9Ex **ppD3D);

#ifndef NO_INJECTION
Direct3DCreate9_type Direct3DCreate9_fn; // we need to store the pointer to the original Direct3DCreate9 function after we have done a detour
Direct3DCreate9Ex_type Direct3DCreate9Ex_fn; // we need to store the pointer to the original Direct3DCreate9 function after we have done a detour
HHOOK gl_hHook = NULL;
#endif




/*
 * global variable which are linked external
 */
unsigned int          gl_ErrorState = 0u;

#ifdef LOG_MESSAGE
FILE*                 gl_File = NULL;
#endif


#ifdef DIRECT_INJECTION
void Nothing(void) {(void)NULL;}
#endif
/*
 * dll entry routine, here we initialize or clean up
 */
BOOL WINAPI DllMain( HINSTANCE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
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
  default:  break;
	}
    
  return (true);
}


DWORD WINAPI ServerThread( LPVOID lpParam )
{
  UNREFERENCED_PARAMETER(lpParam);
  if (gl_TextureServer!=NULL) gl_TextureServer->MainLoop(); //This is and endless mainloop, it sleep till something is written into the pipe.
  return (0);
}

void InitInstance(HINSTANCE hModule)
{

  DisableThreadLibraryCalls( hModule ); //reduce overhead

  gl_hThisInstance = (HINSTANCE)  hModule;

  wchar_t game[MAX_PATH];
  if (HookThisProgram( game)) //ask if we need to hook this program
  {
    OpenMessage();
    Message("InitInstance: %lu\n", hModule);

    gl_TextureServer = new uMod_TextureServer(game); //create the server which listen on the pipe and prepare the update for the texture clients

    LoadOriginalDll();

#ifndef NO_INJECTION
    // we detour the original Direct3DCreate9 to our MyDirect3DCreate9
    Direct3DCreate9_fn = (Direct3DCreate9_type) GetProcAddress(gl_hOriginalDll, "Direct3DCreate9");
    if (Direct3DCreate9_fn!=NULL)
    {
      Message("Detour: Direct3DCreate9\n");
      Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc( (BYTE*)Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9, 5);
    }

    Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type) GetProcAddress(gl_hOriginalDll, "Direct3DCreate9Ex");
    if (Direct3DCreate9Ex_fn!=NULL)
    {
      Message("Detour: Direct3DCreate9Ex\n");
      Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)DetourFunc( (BYTE*)Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex, 7);
    }
#endif

    if (gl_TextureServer->OpenPipe(game)) //open the pipe and send the name+path of this executable
    {
      Message("InitInstance: Pipe not opened\n");
      return;
    }

    gl_ServerThread = CreateThread( NULL, 0, ServerThread, NULL, 0, NULL); //creating a thread for the mainloop
    if (gl_ServerThread==NULL) {Message("InitInstance: Serverthread not started\n");}

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

#ifdef NO_INJECTION
/*
 * We do not inject, the game loads this dll by itself thus we must include the Direct3DCreate9 function
 */

IDirect3D9* WINAPI  Direct3DCreate9(UINT SDKVersion)
{
  Message("WINAPI  Direct3DCreate9\n");

	if (!gl_hOriginalDll) LoadOriginalDll(); // looking for the "right d3d9.dll"
	
	// find original function in original d3d9.dll
	Direct3DCreate9_type D3DCreate9_fn = (Direct3DCreate9_type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9");
    

	if (!D3DCreate9_fn) 
  {
	  Message("Direct3DCreate9: original function not found in dll\n");
    return (NULL);
  }
	

	//Create originale IDirect3D9 object
	IDirect3D9 *pIDirect3D9_orig = D3DCreate9_fn(SDKVersion);

	//create our uMod_IDirect3D9 object
	uMod_IDirect3D9 *pIDirect3D9 = new uMod_IDirect3D9( pIDirect3D9_orig, gl_TextureServer);

	// Return pointer to our object instead of "real one"
	return (pIDirect3D9);
}

HRESULT WINAPI  Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex **ppD3D)
{
  Message("WINAPI  Direct3DCreate9Ex\n");

  if (!gl_hOriginalDll) LoadOriginalDll(); // looking for the "right d3d9.dll"

  // find original function in original d3d9.dll
  Direct3DCreate9Ex_type D3DCreate9Ex_fn = (Direct3DCreate9Ex_type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9Ex");


  if (!D3DCreate9Ex_fn)
  {
    Message("Direct3DCreate9Ex: original function not found in dll\n");
    return (D3DERR_NOTAVAILABLE);
  }


  //Create originale IDirect3D9 object
  IDirect3D9Ex *pIDirect3D9Ex_orig;
  HRESULT ret = D3DCreate9Ex_fn( SDKVersion, &pIDirect3D9Ex_orig);
  if (ret!=S_OK) return (ret);

  //create our uMod_IDirect3D9 object
  uMod_IDirect3D9Ex *pIDirect3D9Ex = new uMod_IDirect3D9Ex( pIDirect3D9Ex_orig, gl_TextureServer);

  ppD3D = &pIDirect3D9Ex_orig; // Return pointer to our object instead of "real one"
  return (ret);
}

bool HookThisProgram( wchar_t *ret) //this function always return true, it is needed for the name and path of the executable
{
  wchar_t Executable[MAX_PATH];
  GetModuleFileNameW( GetModuleHandle( NULL ), Executable, MAX_PATH ); //ask for name and path of this executable

  int len = 0;
  while (Executable[len]) {ret[len] = Executable[len]; len++;}
  ret[len] = 0;
  return (true);
}



#else

/*
 * We inject the dll into the game, thus we retour the original Direct3DCreate9 function to our MyDirect3DCreate9 function
 */

IDirect3D9 *APIENTRY uMod_Direct3DCreate9(UINT SDKVersion)
{
  Message("uMod_Direct3DCreate9:  original %lu, uMod %lu\n", Direct3DCreate9_fn, uMod_Direct3DCreate9);

  // in the Internet are many tutorials for detouring functions and all of them will work without the following 5 marked lines
  // but somehow, for me it only works, if I retour the function and calling afterward the original function

  // BEGIN

  LoadOriginalDll();

  RetourFunc((BYTE*) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9"), (BYTE*)Direct3DCreate9_fn, 5);
  Direct3DCreate9_fn = (Direct3DCreate9_type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9");

/*
  if (Direct3DCreate9Ex_fn!=NULL)
  {
    RetourFunc((BYTE*) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9Ex"), (BYTE*)Direct3DCreate9Ex_fn, 7);
    Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9Ex");
  }
  */
  // END

  IDirect3D9 *pIDirect3D9_orig = NULL;
  if (Direct3DCreate9_fn)
  {
    pIDirect3D9_orig = Direct3DCreate9_fn(SDKVersion); //creating the original IDirect3D9 object
  }
  else return (NULL);
  uMod_IDirect3D9 *pIDirect3D9;
  if (pIDirect3D9_orig)
  {
    pIDirect3D9 = new uMod_IDirect3D9( pIDirect3D9_orig, gl_TextureServer); //creating our uMod_IDirect3D9 object
  }

  // we detour again
  Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc( (BYTE*) Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9,5);
  /*
  if (Direct3DCreate9Ex_fn!=NULL)
  {
    Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)DetourFunc( (BYTE*) Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex,7);
  }
*/
  return (pIDirect3D9); //return our object instead of the "real one"
}

HRESULT APIENTRY uMod_Direct3DCreate9Ex( UINT SDKVersion, IDirect3D9Ex **ppD3D)
{
  Message( "uMod_Direct3DCreate9Ex:  original %lu, uMod %lu\n", Direct3DCreate9Ex_fn, uMod_Direct3DCreate9Ex);

  // in the Internet are many tutorials for detouring functions and all of them will work without the following 5 marked lines
  // but somehow, for me it only works, if I retour the function and calling afterward the original function

  // BEGIN

  LoadOriginalDll();
  /*
  if (Direct3DCreate9_fn!=NULL)
  {
    RetourFunc((BYTE*) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9"), (BYTE*)Direct3DCreate9_fn, 5);
    Direct3DCreate9_fn = (Direct3DCreate9_type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9");
  }
*/
  RetourFunc((BYTE*) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9Ex"), (BYTE*)Direct3DCreate9Ex_fn, 7);
  Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9Ex");
  // END

  IDirect3D9Ex *pIDirect3D9Ex_orig = NULL;
  HRESULT ret;
  if (Direct3DCreate9Ex_fn)
  {
    ret = Direct3DCreate9Ex_fn(SDKVersion, &pIDirect3D9Ex_orig); //creating the original IDirect3D9 object
  }
  else return (D3DERR_NOTAVAILABLE);

  if (ret!=S_OK) return (ret);

  uMod_IDirect3D9Ex *pIDirect3D9Ex;
  if (pIDirect3D9Ex_orig)
  {
    pIDirect3D9Ex = new uMod_IDirect3D9Ex( pIDirect3D9Ex_orig, gl_TextureServer); //creating our uMod_IDirect3D9 object
  }

  // we detour again
/*
  if (Direct3DCreate9_fn!=NULL)
  {
    Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc( (BYTE*) Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9,5);
  }
  */
  Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)DetourFunc( (BYTE*) Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex,7);
  ppD3D = (IDirect3D9Ex**) &pIDirect3D9Ex; //return our object instead of the "real one"
  return (ret);
}

bool HookThisProgram( wchar_t *ret)
{
  wchar_t Executable[MAX_PATH];
  wchar_t Game[MAX_PATH];
  GetModuleFileNameW( GetModuleHandle( NULL ), Executable, MAX_PATH ); //ask for name and path of this executable

#ifdef HOOK_INJECTION
  //we use the gloabal hook

  FILE* file;
  wchar_t *app_path = _wgetenv( L"APPDATA"); //asc for the user application directory
  wchar_t file_name[MAX_PATH];
  swprintf_s( file_name, MAX_PATH, L"%ls\\%ls\\%ls", app_path, uMod_APP_DIR, uMod_APP_DX9);
  if (_wfopen_s( &file, file_name, L"rt,ccs=UTF-16LE")) return (false); // open the file in utf-16 LE mode


  //MessageBoxW( NULL, Executable, L"test", 0);
  while (!feof(file))
  {
    if ( fgetws( Game, MAX_PATH, file) != NULL ) //get each line of the file
    {
      //MessageBoxW( NULL, Game, L"test", 0);
      int len = 0;
      while (Game[len])
      {
        if (Game[len]==L'\r' || Game[len]==L'\n') {Game[len]=0; break;} //removing the new line symbols
        len++;
      }
      if ( _wcsicmp( Executable, Game ) == 0 ) //compare both strings
      {
        for (int i=0; i<len; i++) ret[i] = Game[i];
        ret[len] = 0;
        fclose(file);
        return (true);
      }
    }
  }
  fclose(file);
  return (false);
#endif
#ifdef DIRECT_INJECTION
  // we inject directly
  int i=0;
  while ( Game[i]) {ret[i]=Game[i]; i++;}
  ret[i]=0;
  return true;
#endif
}

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

#ifdef HOOK_INJECTION
/*
 * We do not change something, if our hook function is called.
 * We need this hook only to get our dll loaded into a starting program.
 */
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  return (CallNextHookEx( gl_hHook, nCode, wParam, lParam));
}

void InstallHook(void)
{
  gl_hHook = SetWindowsHookEx( WH_CBT, HookProc, gl_hThisInstance, 0 );
}

void RemoveHook(void)
{
  UnhookWindowsHookEx( gl_hHook );
}
#endif
#endif
