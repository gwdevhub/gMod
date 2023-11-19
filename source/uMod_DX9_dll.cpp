#include "uMod_DX9_dll.h"

#include <array>
#include <Windows.h>
#include "uMod_Main.h"
#include <Psapi.h>

HINSTANCE gl_hOriginalDll = nullptr;
HINSTANCE gl_hThisInstance = nullptr;

using Direct3DCreate9_type = IDirect3D9* (APIENTRY*)(UINT);
using Direct3DCreate9Ex_type = HRESULT(APIENTRY*)(UINT SDKVersion, IDirect3D9Ex** ppD3D);

Direct3DCreate9_type Direct3DCreate9_fn; // we need to store the pointer to the original Direct3DCreate9 function after we have done a detour
Direct3DCreate9Ex_type Direct3DCreate9Ex_fn; // we need to store the pointer to the original Direct3DCreate9 function after we have done a detour
HHOOK gl_hHook = nullptr;

static FILE* stdout_proxy;
static FILE* stderr_proxy;

/*
 * global variable which are linked external
 */
unsigned int gl_ErrorState = 0u;

#ifdef LOG_MESSAGE
FILE* gl_File = nullptr;
#endif


#ifdef DIRECT_INJECTION
void Nothing() { (void)NULL; }
#endif
/*
 * dll entry routine, here we initialize or clean up
 */
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
#ifdef _DEBUG
        AllocConsole();
        SetConsoleTitleA("gMod Console");
        freopen_s(&stdout_proxy, "CONOUT$", "w", stdout);
        freopen_s(&stderr_proxy, "CONOUT$", "w", stderr);
#endif
            InitInstance(hModule);
            break;
        }
        case DLL_PROCESS_DETACH: {
            ExitInstance();
            break;
        }
        default: break;
    }

    return true;
}

void InitInstance(HINSTANCE hModule)
{
    DisableThreadLibraryCalls(hModule); //reduce overhead
    gl_hThisInstance = hModule;

    char game[MAX_PATH];
    if (HookThisProgram(game)) //ask if we need to hook this program
    {
        OpenMessage();
        Message("InitInstance: %p\n", hModule);
        std::array<char, MAX_PATH> uMod{};

        GetModuleFileNameA(hModule, uMod.data(), MAX_PATH);
        Message("InitInstance: %s\n", uMod.data());
        LoadOriginalDll();
        if (gl_hOriginalDll) {
            Direct3DCreate9_fn = reinterpret_cast<Direct3DCreate9_type>(GetProcAddress(gl_hOriginalDll, "Direct3DCreate9"));
            if (Direct3DCreate9_fn != nullptr) {
                Message("Detour: Direct3DCreate9\n");
                Direct3DCreate9_fn = static_cast<Direct3DCreate9_type>(DetourFunc((BYTE*)Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9, 5));
            }

            Direct3DCreate9Ex_fn = reinterpret_cast<Direct3DCreate9Ex_type>(GetProcAddress(gl_hOriginalDll, "Direct3DCreate9Ex"));
            if (Direct3DCreate9Ex_fn != nullptr) {
                Message("Detour: Direct3DCreate9Ex\n");
                Direct3DCreate9Ex_fn = static_cast<Direct3DCreate9Ex_type>(DetourFunc((BYTE*)Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex, 7));
            }
        }
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

bool FindLoadedDll()
{
    HMODULE hModules[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;

    // Get a handle to the current process.
    hProcess = GetCurrentProcess();
    if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            if (IsDesiredModule(hModules[i], hProcess)) {
                // If the module is d3d9.dll, store the handle or do your hooking here.
                gl_hOriginalDll = hModules[i];
                break;
            }
            if (HasDesiredMethods(hModules[i], hProcess)) {
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

void LoadOriginalDll()
{
    if (FindLoadedDll()) {
        return;
    }

    char buffer[MAX_PATH];
    GetSystemDirectory(buffer, MAX_PATH); //get the system directory, we need to open the original d3d9.dll

    // Append dll name
    strcat_s(buffer, MAX_PATH, "\\d3d9.dll");

    // try to load the system's d3d9.dll, if pointer empty
    if (!gl_hOriginalDll) {
        gl_hOriginalDll = LoadLibrary(buffer);
    }

    if (!gl_hOriginalDll) {
        ExitProcess(0); // exit the hard way
    }
}

void ExitInstance()
{
    // Release the system's d3d9.dll
    if (gl_hOriginalDll != nullptr) {
        FreeLibrary(gl_hOriginalDll);
        gl_hOriginalDll = nullptr;
    }

#ifdef _DEBUG
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
    Message("uMod_Direct3DCreate9:  original %p, uMod %p\n", Direct3DCreate9_fn, uMod_Direct3DCreate9);

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

    IDirect3D9* pIDirect3D9_orig = nullptr;
    if (Direct3DCreate9_fn) {
        pIDirect3D9_orig = Direct3DCreate9_fn(SDKVersion); //creating the original IDirect3D9 object
    }
    else {
        return nullptr;
    }
    uMod_IDirect3D9* pIDirect3D9;
    if (pIDirect3D9_orig) {
        pIDirect3D9 = new uMod_IDirect3D9(pIDirect3D9_orig); //creating our uMod_IDirect3D9 object
    }

    // we detour again
    Direct3DCreate9_fn = static_cast<Direct3DCreate9_type>(DetourFunc((BYTE*)Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9, 5));
    /*
    if (Direct3DCreate9Ex_fn!=NULL)
    {
      Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)DetourFunc( (BYTE*) Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex,7);
    }
  */
    return pIDirect3D9; //return our object instead of the "real one"
}

HRESULT APIENTRY uMod_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3D)
{
    Message("uMod_Direct3DCreate9Ex:  original %p, uMod %p\n", Direct3DCreate9Ex_fn, uMod_Direct3DCreate9Ex);

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

    IDirect3D9Ex* pIDirect3D9Ex_orig = nullptr;
    HRESULT ret;
    if (Direct3DCreate9Ex_fn) {
        ret = Direct3DCreate9Ex_fn(SDKVersion, &pIDirect3D9Ex_orig); //creating the original IDirect3D9 object
    }
    else {
        return D3DERR_NOTAVAILABLE;
    }

    if (ret != S_OK) {
        return ret;
    }

    uMod_IDirect3D9Ex* pIDirect3D9Ex;
    if (pIDirect3D9Ex_orig) {
        pIDirect3D9Ex = new uMod_IDirect3D9Ex(pIDirect3D9Ex_orig); //creating our uMod_IDirect3D9 object
    }

    // we detour again
    /*
      if (Direct3DCreate9_fn!=NULL)
      {
        Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc( (BYTE*) Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9,5);
      }
      */
    Direct3DCreate9Ex_fn = static_cast<Direct3DCreate9Ex_type>(DetourFunc((BYTE*)Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex, 7));
    ppD3D = (IDirect3D9Ex**)&pIDirect3D9Ex; //return our object instead of the "real one"
    return ret;
}

bool HookThisProgram(char* ret)
{
    char Game[MAX_PATH];
    GetModuleFileName(GetModuleHandle(nullptr), Game, MAX_PATH); //ask for name and path of this executable

    // we inject directly
    strcpy(ret, Game);
    return true;
}

void* DetourFunc(BYTE* src, const BYTE* dst, const int len)
{
    auto jmp = static_cast<BYTE*>(malloc(len + 5));
    DWORD dwback = 0;
    VirtualProtect(jmp, len + 5, PAGE_EXECUTE_READWRITE, &dwback); //This is the addition needed for Windows 7 RC
    VirtualProtect(src, len, PAGE_READWRITE, &dwback);
    memcpy(jmp, src, len);
    jmp += len;
    jmp[0] = 0xE9;
    *(DWORD*)(jmp + 1) = static_cast<DWORD>(src + len - jmp) - 5;
    memset(src, 0x90, len);
    src[0] = 0xE9;
    *(DWORD*)(src + 1) = static_cast<DWORD>(dst - src) - 5;
    VirtualProtect(src, len, dwback, &dwback);
    return jmp - len;
}

bool RetourFunc(BYTE* src, BYTE* restore, const int len)
{
    DWORD dwback;
    if (!VirtualProtect(src, len, PAGE_READWRITE, &dwback)) { return false; }
    if (!memcpy(src, restore, len)) { return false; }
    restore[0] = 0xE9;
    *(DWORD*)(restore + 1) = static_cast<DWORD>(src - restore) - 5;
    if (!VirtualProtect(src, len, dwback, &dwback)) { return false; }
    return true;
}
