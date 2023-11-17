#include "uMod_DX9_dll.h"

#include <array>
#include <Windows.h>
#include "uMod_Main.h"
#include <Psapi.h>

HINSTANCE gl_hOriginalDll = nullptr;
HINSTANCE gl_hThisInstance = nullptr;
std::unique_ptr<uMod_TextureServer> gl_TextureServer = nullptr;
HANDLE gl_ServerThread = nullptr;

using Direct3DCreate9_type = IDirect3D9* (APIENTRY*)(UINT);
using Direct3DCreate9Ex_type = HRESULT(APIENTRY*)(UINT SDKVersion, IDirect3D9Ex** ppD3D);

Direct3DCreate9_type OriginalDirect3DCreate9Trampoline = nullptr;
Direct3DCreate9Ex_type OriginalDirect3DCreate9ExTrampoline = nullptr;
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
        SetConsoleTitleA("uMod Console");
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


DWORD WINAPI ServerThread(LPVOID lpParam)
{
    UNREFERENCED_PARAMETER(lpParam);
    if (gl_TextureServer != nullptr) {
        gl_TextureServer->Initialize(); //This is and endless mainloop, it sleep till something is written into the pipe.
    }
    return 0;
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
        gl_TextureServer = std::make_unique<uMod_TextureServer>(game, uMod.data()); //create the server which listen on the pipe and prepare the update for the texture clients
        LoadOriginalDll();
        if (gl_hOriginalDll) {
            // we detour the original Direct3DCreate9 to our MyDirect3DCreate9
            Direct3DCreate9_fn = (Direct3DCreate9_type)GetProcAddress(gl_hOriginalDll, "Direct3DCreate9");
            if (Direct3DCreate9_fn != NULL)
            {
                Message("Detour: Direct3DCreate9\n");
                OriginalDirect3DCreate9Trampoline = (Direct3DCreate9_type)DetourFunc((BYTE*)Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9, 5);
            }

            Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)GetProcAddress(gl_hOriginalDll, "Direct3DCreate9Ex");
            if (Direct3DCreate9Ex_fn != NULL)
            {
                Message("Detour: Direct3DCreate9Ex\n");
                OriginalDirect3DCreate9ExTrampoline = (Direct3DCreate9Ex_type)DetourFunc((BYTE*)Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex, 7);
            }
        }
        gl_TextureServer->Initialize();
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

IDirect3D9* WINAPI uMod_Direct3DCreate9(UINT SDKVersion)
{
    Message("WINAPI  Direct3DCreate9\n");

    if (!gl_hOriginalDll) LoadOriginalDll(); // looking for the "right d3d9.dll"

    // find original function in original d3d9.dll
    
    //Create originale IDirect3D9 object
    IDirect3D9* pIDirect3D9_orig = OriginalDirect3DCreate9Trampoline(SDKVersion);

    //create our uMod_IDirect3D9 object
    uMod_IDirect3D9* pIDirect3D9 = new uMod_IDirect3D9(pIDirect3D9_orig, gl_TextureServer.get());

    // Return pointer to our object instead of "real one"
    return (pIDirect3D9);
}

HRESULT WINAPI uMod_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3D)
{
    Message("WINAPI  Direct3DCreate9Ex\n");

    if (!gl_hOriginalDll) LoadOriginalDll(); // looking for the "right d3d9.dll"

    // find original function in original d3d9.dll

    //Create originale IDirect3D9 object
    IDirect3D9Ex* pIDirect3D9Ex_orig;
    HRESULT ret = OriginalDirect3DCreate9ExTrampoline(SDKVersion, &pIDirect3D9Ex_orig);
    if (ret != S_OK) return (ret);

    //create our uMod_IDirect3D9 object
    uMod_IDirect3D9Ex* pIDirect3D9Ex = new uMod_IDirect3D9Ex(pIDirect3D9Ex_orig, gl_TextureServer.get());

    ppD3D = &pIDirect3D9Ex_orig; // Return pointer to our object instead of "real one"
    return (ret);
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
