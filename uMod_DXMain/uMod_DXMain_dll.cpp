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

#ifdef __CDT_PARSER__
#define INJECTION_METHOD 0
#endif



#include "uMod_Main.h"
/*
 * global variable which are not linked external
 */

HINSTANCE             gl_hThisInstance = NULL;
uMod_TextureServer*    gl_TextureServer = NULL;
HANDLE                gl_ServerThread = NULL;


/*
 * global variable which are linked external
 */
unsigned int          gl_ErrorState = 0u;

#ifdef LOG_MESSAGE
FILE*                 gl_File = NULL;
#endif

#if INJECTION_METHOD == DIRECT_INJECTION
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

  wchar_t game[10*MAX_PATH];
  if (HookThisProgram( game, 10*MAX_PATH)) //ask if we need to hook this program
  {
    OpenMessage();
    Message("InitInstance: %p (%ls)\n", hModule, game);

    gl_TextureServer = new uMod_TextureServer(game); //create the server which listen on the pipe and prepare the update for the texture clients

#ifdef DEF_USE_DX9
    InitDX9();
#endif
#ifdef DEF_USE_DX10
    InitDX10();
#endif


    if (gl_TextureServer->OpenPipe(game, INJECTION_METHOD)) //open the pipe and send the name+path of this executable
    {
      Message("InitInstance: Pipe not opened\n");
      return;
    }

    gl_ServerThread = CreateThread( NULL, 0, ServerThread, NULL, 0, NULL); //creating a thread for the mainloop
    if (gl_ServerThread==NULL) {Message("InitInstance: Serverthread not started\n");}

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

#ifdef DEF_USE_DX9
  ExitDX9();
#endif
#ifdef DEF_USE_DX10
  ExitDX10();
#endif

  CloseMessage();
}


bool HookThisProgram( wchar_t *ret, const int ret_len)
{
  const int max_len = ret_len < MAX_PATH ? MAX_PATH : ret_len;
  wchar_t *Executable = NULL;
  if (GetMemory( Executable, max_len))
  {
    ret[0]=0;
    return false;
  }
  GetModuleFileNameW( GetModuleHandle( NULL ), Executable, max_len ); //ask for name and path of this executable

#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==NO_INJECTION
  // we inject directly
  int i=0;
  while (i<max_len && i<ret_len-1 && Executable[i]) {ret[i]=Executable[i]; i++;}
  ret[i]=0;
  delete [] Executable;
  return true;

#elif INJECTION_METHOD==HOOK_INJECTION
  //we use the gloabal hook

  wchar_t *Game = NULL;
  if (GetMemory( Game, max_len))
  {
    delete [] Executable;
    ret[0]=0;
    return false;
  }

  FILE* file;
  wchar_t *app_path = _wgetenv( L"APPDATA"); //asc for the user application directory
  wchar_t *file_name = NULL;
  if (GetMemory( file_name, max_len))
  {
    delete [] Executable;
    delete [] Game;
    ret[0]=0;
    return false;
  }
  swprintf_s( file_name, max_len, L"%ls\\%ls\\%ls", app_path, uMod_APP_DIR, uMod_APP_DX9);
  if (_wfopen_s( &file, file_name, L"rt,ccs=UTF-16LE")) return (false); // open the file in utf-16 LE mode
  delete [] file_name;

  while (!feof(file))
  {
    if ( fgetws( Game, MAX_PATH, file) != NULL ) //get each line of the file
    {
      int len = 0;
      while (len<max_len-1 &&Game[len])
      {
        if (Game[len]==L'\r' || Game[len]==L'\n') {Game[len]=0; break;} //removing the new line symbols
        len++;
      }
      if ( _wcsicmp( Executable, Game ) == 0 ) //compare both strings
      {
        for (int i=0; i<len && i<ret_len-1; i++) ret[i] = Game[i];
        ret[len] = 0;
        fclose(file);
        delete [] Executable;
        delete [] Game;
        return (true);
      }
    }
  }
  delete [] Executable;
  delete [] Game;
  fclose(file);
  return (false);
#else
#error No injection method was set.
  return false;
#endif
}

#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION

void *DetourFunc(BYTE *src, const BYTE *dst, const int len)
{
  BYTE *jmp = (BYTE*)malloc(len+5);
  DWORD dwback = 0;
  //(jmp, len+5, PAGE_EXECUTE_READWRITE, &dwback); //This is the addition needed for Windows 7 RC
  VirtualProtect(src, len, PAGE_READWRITE, &dwback);
  memcpy(jmp, src, len);    jmp += len;
  jmp[0] = 0xE9;
  *(DWORD*)(jmp+1) = (DWORD)(src+len - jmp) - 5;
  //memset(src, 0x90, len);
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
#endif


#if INJECTION_METHOD==HOOK_INJECTION
/*
 * We do not change something, if our hook function is called.
 * We need this hook only to get our dll loaded into a starting program.
 */

HHOOK gl_hHook = NULL;
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

