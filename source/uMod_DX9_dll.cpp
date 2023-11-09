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

#include "../header/uMod_DX9_dll.h"
#include "uMod_Main.h"
#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
#pragma comment(lib, "Psapi.lib")
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
uMod_TextureServer* gl_TextureServer = NULL;
HANDLE                gl_ServerThread = NULL;

typedef IDirect3D9* (APIENTRY* Direct3DCreate9_type)(UINT);
typedef HRESULT(APIENTRY* Direct3DCreate9Ex_type)(UINT SDKVersion, IDirect3D9Ex** ppD3D);

Direct3DCreate9_type Direct3DCreate9_fn; // we need to store the pointer to the original Direct3DCreate9 function after we have done a detour
Direct3DCreate9Ex_type Direct3DCreate9Ex_fn; // we need to store the pointer to the original Direct3DCreate9 function after we have done a detour
HHOOK gl_hHook = NULL;

static FILE* stdout_proxy;
static FILE* stderr_proxy;

/*
 * global variable which are linked external
 */
unsigned int          gl_ErrorState = 0u;

#ifdef LOG_MESSAGE
FILE* gl_File = NULL;
#endif


#ifdef DIRECT_INJECTION
void Nothing(void) { (void)NULL; }
#endif
/*
 * dll entry routine, here we initialize or clean up
 */
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
#ifdef BUILD_TYPE_DEBUG
        AllocConsole();
        SetConsoleTitleA("uMod Console");
        freopen_s(&stdout_proxy, "CONOUT$", "w", stdout);
        freopen_s(&stderr_proxy, "CONOUT$", "w", stderr);
#endif
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


DWORD WINAPI ServerThread(LPVOID lpParam)
{
    UNREFERENCED_PARAMETER(lpParam);
    if (gl_TextureServer != NULL) gl_TextureServer->MainLoop(); //This is and endless mainloop, it sleep till something is written into the pipe.
    return (0);
}

void InitInstance(HINSTANCE hModule)
{
    DisableThreadLibraryCalls(hModule); //reduce overhead
    gl_hThisInstance = (HINSTANCE)hModule;

    char game[MAX_PATH];
    if (HookThisProgram(game)) //ask if we need to hook this program
    {
        OpenMessage();
        Message("InitInstance: %lu\n", hModule);
        char uMod[MAX_PATH];
        for (auto i = 0; i < MAX_PATH; i++) {
            uMod[i] = 0;
        }

        GetModuleFileName(hModule, uMod, MAX_PATH);
        Message("InitInstance: %s\n", uMod);
        gl_TextureServer = new uMod_TextureServer(game, uMod); //create the server which listen on the pipe and prepare the update for the texture clients
        LoadOriginalDll();
        if (gl_hOriginalDll) {
            Direct3DCreate9_fn = (Direct3DCreate9_type)GetProcAddress(gl_hOriginalDll, "Direct3DCreate9");
            if (Direct3DCreate9_fn != NULL)
            {
                Message("Detour: Direct3DCreate9\n");
                Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc((BYTE*)Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9, 5);
            }

            Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)GetProcAddress(gl_hOriginalDll, "Direct3DCreate9Ex");
            if (Direct3DCreate9Ex_fn != NULL)
            {
                Message("Detour: Direct3DCreate9Ex\n");
                Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)DetourFunc((BYTE*)Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex, 7);
            }
        }
        gl_ServerThread = CreateThread(NULL, 0, ServerThread, NULL, 0, NULL); //creating a thread for the mainloop
        if (gl_ServerThread == NULL) { Message("InitInstance: Serverthread not started\n"); }

    }
}

bool HasDesiredMethods(HMODULE hModule, HANDLE hProcess)
{
    const auto d3dcreate9Addr = GetProcAddress(hModule, "Direct3DCreate9");
    if (!d3dcreate9Addr) {
        return false;
    }

    const auto d3dcreate9ExAddr = GetProcAddress(hModule, "Direct3DCreate9Ex");
    if (!d3dcreate9ExAddr) {
        return false;
    }

    return true;
}

bool IsDesiredModule(HMODULE hModule, HANDLE hProcess)
{
    TCHAR szModuleName[MAX_PATH];
    GetModuleBaseName(hProcess, hModule, szModuleName, sizeof(szModuleName) / sizeof(TCHAR));
    return strcmp(szModuleName, TEXT("d3d9.dll")) == 0;
}

bool FindLoadedDll(void)
{
    HMODULE hModules[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;

    // Get a handle to the current process.
    hProcess = GetCurrentProcess();
    if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded))
    {
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            if (IsDesiredModule(hModules[i], hProcess))
            {
                // If the module is d3d9.dll, store the handle or do your hooking here.
                gl_hOriginalDll = hModules[i];
                break;
            }
            else if (HasDesiredMethods(hModules[i], hProcess))
            {
                // If the module has the two specific methods, store the handle or do your hooking here.
                gl_hOriginalDll = hModules[i];
                break;
            }
        }
    }

    if (gl_hOriginalDll) {
        return true;
    }

    return false;
}

void LoadOriginalDll(void)
{
    if (FindLoadedDll()) {
        return;
    }

    char buffer[MAX_PATH];
    GetSystemDirectory(buffer, MAX_PATH); //get the system directory, we need to open the original d3d9.dll

    // Append dll name
    strcat_s(buffer, MAX_PATH, "\\d3d9.dll");

    // try to load the system's d3d9.dll, if pointer empty
    if (!gl_hOriginalDll) gl_hOriginalDll = LoadLibrary(buffer);

    if (!gl_hOriginalDll)
    {
        ExitProcess(0); // exit the hard way
    }
}

void ExitInstance()
{
    if (gl_ServerThread != NULL)
    {
        CloseHandle(gl_ServerThread); // kill the server thread
        gl_ServerThread = NULL;
    }
    if (gl_TextureServer != NULL)
    {
        delete gl_TextureServer; //delete the texture server
        gl_TextureServer = NULL;
    }

    // Release the system's d3d9.dll
    if (gl_hOriginalDll != NULL)
    {
        FreeLibrary(gl_hOriginalDll);
        gl_hOriginalDll = NULL;
    }

#ifdef BUILD_TYPE_DEBUG
    if (stdout_proxy)
        fclose(stdout_proxy);
    if (stderr_proxy)
        fclose(stderr_proxy);
    FreeConsole();
#endif
    CloseMessage();
}

/*
 * We inject the dll into the game, thus we retour the original Direct3DCreate9 function to our MyDirect3DCreate9 function
 */

IDirect3D9* APIENTRY uMod_Direct3DCreate9(UINT SDKVersion)
{
    Message("uMod_Direct3DCreate9:  original %lu, uMod %lu\n", Direct3DCreate9_fn, uMod_Direct3DCreate9);

    // in the Internet are many tutorials for detouring functions and all of them will work without the following 5 marked lines
    // but somehow, for me it only works, if I retour the function and calling afterward the original function

    // BEGIN

    LoadOriginalDll();

    RetourFunc((BYTE*)GetProcAddress(gl_hOriginalDll, "Direct3DCreate9"), (BYTE*)Direct3DCreate9_fn, 5);
    Direct3DCreate9_fn = (Direct3DCreate9_type)GetProcAddress(gl_hOriginalDll, "Direct3DCreate9");

    /*
      if (Direct3DCreate9Ex_fn!=NULL)
      {
        RetourFunc((BYTE*) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9Ex"), (BYTE*)Direct3DCreate9Ex_fn, 7);
        Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9Ex");
      }
      */
      // END

    IDirect3D9* pIDirect3D9_orig = NULL;
    if (Direct3DCreate9_fn)
    {
        pIDirect3D9_orig = Direct3DCreate9_fn(SDKVersion); //creating the original IDirect3D9 object
    }
    else return (NULL);
    uMod_IDirect3D9* pIDirect3D9;
    if (pIDirect3D9_orig)
    {
        pIDirect3D9 = new uMod_IDirect3D9(pIDirect3D9_orig, gl_TextureServer); //creating our uMod_IDirect3D9 object
    }

    // we detour again
    Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc((BYTE*)Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9, 5);
    /*
    if (Direct3DCreate9Ex_fn!=NULL)
    {
      Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)DetourFunc( (BYTE*) Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex,7);
    }
  */
    return (pIDirect3D9); //return our object instead of the "real one"
    }

HRESULT APIENTRY uMod_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3D)
{
    Message("uMod_Direct3DCreate9Ex:  original %lu, uMod %lu\n", Direct3DCreate9Ex_fn, uMod_Direct3DCreate9Ex);

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
    RetourFunc((BYTE*)GetProcAddress(gl_hOriginalDll, "Direct3DCreate9Ex"), (BYTE*)Direct3DCreate9Ex_fn, 7);
    Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)GetProcAddress(gl_hOriginalDll, "Direct3DCreate9Ex");
    // END

    IDirect3D9Ex* pIDirect3D9Ex_orig = NULL;
    HRESULT ret;
    if (Direct3DCreate9Ex_fn)
    {
        ret = Direct3DCreate9Ex_fn(SDKVersion, &pIDirect3D9Ex_orig); //creating the original IDirect3D9 object
    }
    else return (D3DERR_NOTAVAILABLE);

    if (ret != S_OK) return (ret);

    uMod_IDirect3D9Ex* pIDirect3D9Ex;
    if (pIDirect3D9Ex_orig)
    {
        pIDirect3D9Ex = new uMod_IDirect3D9Ex(pIDirect3D9Ex_orig, gl_TextureServer); //creating our uMod_IDirect3D9 object
    }

    // we detour again
  /*
    if (Direct3DCreate9_fn!=NULL)
    {
      Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc( (BYTE*) Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9,5);
    }
    */
    Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)DetourFunc((BYTE*)Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex, 7);
    ppD3D = (IDirect3D9Ex**)&pIDirect3D9Ex; //return our object instead of the "real one"
    return (ret);
}

bool HookThisProgram(char* ret)
{
    char Game[MAX_PATH];
    GetModuleFileName(GetModuleHandle(NULL), Game, MAX_PATH); //ask for name and path of this executable

    // we inject directly
    int i = 0;
    while (Game[i]) { ret[i] = Game[i]; i++; }
    ret[i] = 0;
    return true;
}

void* DetourFunc(BYTE* src, const BYTE* dst, const int len)
{
    BYTE* jmp = (BYTE*)malloc(len + 5);
    DWORD dwback = 0;
    VirtualProtect(jmp, len + 5, PAGE_EXECUTE_READWRITE, &dwback); //This is the addition needed for Windows 7 RC
    VirtualProtect(src, len, PAGE_READWRITE, &dwback);
    memcpy(jmp, src, len);    jmp += len;
    jmp[0] = 0xE9;
    *(DWORD*)(jmp + 1) = (DWORD)(src + len - jmp) - 5;
    memset(src, 0x90, len);
    src[0] = 0xE9;
    *(DWORD*)(src + 1) = (DWORD)(dst - src) - 5;
    VirtualProtect(src, len, dwback, &dwback);
    return (jmp - len);
        }

bool RetourFunc(BYTE* src, BYTE* restore, const int len)
{
    DWORD dwback;
    if (!VirtualProtect(src, len, PAGE_READWRITE, &dwback)) { return (false); }
    if (!memcpy(src, restore, len)) { return (false); }
    restore[0] = 0xE9;
    *(DWORD*)(restore + 1) = (DWORD)(src - restore) - 5;
    if (!VirtualProtect(src, len, dwback, &dwback)) { return (false); }
    return (true);
}

