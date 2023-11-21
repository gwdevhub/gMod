#include "uMod_DX9_dll.h"

#include <array>
#include <Windows.h>
#include "uMod_Main.h"
#include <Psapi.h>

#include "MinHook.h"

namespace {

    using Direct3DCreate9_type = IDirect3D9* (APIENTRY*)(UINT);
    using Direct3DCreate9Ex_type = HRESULT(APIENTRY*)(UINT SDKVersion, IDirect3D9Ex** ppD3D);

    // Pointer to original address of Direct3DCreate9
    Direct3DCreate9_type Direct3DCreate9_fn = nullptr;
    Direct3DCreate9_type Direct3DCreate9_ret = nullptr;

    // Pointer to original address of Direct3DCreate9
    Direct3DCreate9Ex_type Direct3DCreate9Ex_fn = nullptr;
    Direct3DCreate9Ex_type Direct3DCreate9Ex_ret = nullptr;

    static FILE* stdout_proxy;
    static FILE* stderr_proxy;

    /*
    * global variable which are linked external
    */
    unsigned int gl_ErrorState = 0u;

#ifdef LOG_MESSAGE
    FILE* gl_File = nullptr;
#endif

    // If not nullptr, we're responsible for freeing this library on termination
    HMODULE gl_hOriginalDll = nullptr;

    // If this hModule called d3d9.dll?
    bool IsD3d9Module(HMODULE hModule, HANDLE hProcess)
    {
        TCHAR szModuleName[MAX_PATH];
        GetModuleBaseName(hProcess, hModule, szModuleName, sizeof(szModuleName) / sizeof(TCHAR));
        return strcmp(szModuleName, TEXT("d3d9.dll")) == 0;
    }
    // Does this module contain exported function calls for creating a d3d9 device?
    bool HasD3d9Methods(HMODULE hModule, HANDLE hProcess)
    {
        return GetProcAddress(hModule, "Direct3DCreate9")
            && GetProcAddress(hModule, "Direct3DCreate9Ex");
    }


    HMODULE FindLoadedDll()
    {
        HMODULE hModules[1024];
        HANDLE hProcess;
        DWORD cbNeeded;
        unsigned int i;

        // Get a handle to the current process.
        hProcess = GetCurrentProcess();
        if (!EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded))
            return nullptr;
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            if (IsD3d9Module(hModules[i], hProcess)
                || HasD3d9Methods(hModules[i], hProcess)) {
                return hModules[i];
            }
        }
        return nullptr;
    }
}

unsigned int gl_ErrorState = 0;
HINSTANCE gl_hThisInstance = nullptr;

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
    OpenMessage();
    Message("InitInstance: %p\n", hModule);

    const auto d3d9_dll = LoadOriginalDll();
    ASSERT(d3d9_dll);

    Direct3DCreate9_fn = reinterpret_cast<Direct3DCreate9_type>(GetProcAddress(d3d9_dll, "Direct3DCreate9"));
    ASSERT(Direct3DCreate9_fn);

    Direct3DCreate9Ex_fn = reinterpret_cast<Direct3DCreate9Ex_type>(GetProcAddress(d3d9_dll, "Direct3DCreate9Ex"));
    ASSERT(Direct3DCreate9Ex_fn);

    MH_Initialize();

    if (Direct3DCreate9_fn) {
        MH_CreateHook(Direct3DCreate9_fn, uMod_Direct3DCreate9, (void**)&Direct3DCreate9_ret);
        MH_EnableHook(Direct3DCreate9_fn);
    }

    if (Direct3DCreate9Ex_fn) {
        MH_CreateHook(Direct3DCreate9Ex_fn, uMod_Direct3DCreate9Ex, (void**)&Direct3DCreate9Ex_ret);
        MH_EnableHook(Direct3DCreate9Ex_fn);
    }

}
void ExitInstance()
{
    if(Direct3DCreate9_fn)
        MH_DisableHook(Direct3DCreate9_fn);
    if(Direct3DCreate9Ex_fn)
        MH_DisableHook(Direct3DCreate9Ex_fn);

    MH_Uninitialize();

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

HMODULE LoadOriginalDll()
{
    HMODULE found = FindLoadedDll();
    if (found)
        return found;

    char buffer[MAX_PATH];
    GetSystemDirectory(buffer, MAX_PATH); //get the system directory, we need to open the original d3d9.dll

    // Append dll name
    strcat_s(buffer, MAX_PATH, "\\d3d9.dll");
    gl_hOriginalDll = LoadLibrary(buffer);
    found = FindLoadedDll();
    ASSERT(found && found == gl_hOriginalDll);

    return found;
}

/*
 * We inject the dll into the game, thus we retour the original Direct3DCreate9 function to our MyDirect3DCreate9 function
 */

IDirect3D9* APIENTRY uMod_Direct3DCreate9(UINT SDKVersion)
{
    Message("uMod_Direct3DCreate9:  original %p, uMod %p\n", Direct3DCreate9_fn, uMod_Direct3DCreate9);

    ASSERT(Direct3DCreate9_ret);

    IDirect3D9* pIDirect3D9_orig = Direct3DCreate9_ret(SDKVersion); //creating the original IDirect3D9 object
    ASSERT(pIDirect3D9_orig);

    return new uMod_IDirect3D9(pIDirect3D9_orig); //return our object instead of the "real one"
}

HRESULT APIENTRY uMod_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3D)
{
    Message("uMod_Direct3DCreate9Ex:  original %p, uMod %p\n", Direct3DCreate9Ex_fn, uMod_Direct3DCreate9Ex);

    ASSERT(Direct3DCreate9Ex_ret);

    IDirect3D9Ex* pIDirect3D9Ex_orig = nullptr;
    HRESULT ret = Direct3DCreate9Ex_fn(SDKVersion, &pIDirect3D9Ex_orig); //creating the original IDirect3D9 object

    if (ret != S_OK)
        return ret;

    // @Cleanup: should be we freeing pIDirect3D9Ex at the end of our own lifecycle?
    uMod_IDirect3D9Ex* pIDirect3D9Ex = new uMod_IDirect3D9Ex(pIDirect3D9Ex_orig);
    ppD3D = (IDirect3D9Ex**)&pIDirect3D9Ex;
    return ret;
}

#ifdef DIRECT_INJECTION
void Nothing() { (void)NULL; }
#endif
