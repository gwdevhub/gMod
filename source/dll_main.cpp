#include "dll_main.h"

#include <Windows.h>
#include "Main.h"
#include <Psapi.h>

#include "MinHook.h"
#include <filesystem>

namespace {

#define DISABLE_HOOK(var) if(var) { MH_DisableHook(var); var = nullptr;}

    using Direct3DCreate9_type = IDirect3D9* (APIENTRY*)(UINT);
    using Direct3DCreate9Ex_type = HRESULT(APIENTRY*)(UINT SDKVersion, IDirect3D9Ex** ppD3D);
    using GetProcAddress_type = FARPROC(APIENTRY*)(HMODULE, LPCSTR);

    // Pointer to original address of Direct3DCreate9
    Direct3DCreate9_type Direct3DCreate9_ret = nullptr;

    // Pointer to original address of Direct3DCreate9
    Direct3DCreate9Ex_type Direct3DCreate9Ex_ret = nullptr;

    GetProcAddress_type GetProcAddress_fn = nullptr;
    GetProcAddress_type GetProcAddress_ret = nullptr;

    FARPROC APIENTRY OnGetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
        ASSERT(GetProcAddress_ret);
        if ((int)lpProcName < 0xffff)
            return GetProcAddress_ret(hModule, lpProcName); // lpProcName is ordinal offset, not string

        if (strcmp(lpProcName, "Direct3DCreate9") == 0) {
            Direct3DCreate9_ret = (Direct3DCreate9_type)GetProcAddress_ret(hModule, lpProcName);
            return (FARPROC)Direct3DCreate9;
        }
        if (strcmp(lpProcName, "Direct3DCreate9Ex") == 0) {
            Direct3DCreate9Ex_ret = (Direct3DCreate9Ex_type)GetProcAddress_ret(hModule, lpProcName);
            return (FARPROC)Direct3DCreate9Ex;
        }
        return GetProcAddress_ret(hModule, lpProcName);
    }


    FILE* stdout_proxy;
    FILE* stderr_proxy;

    HMODULE FindLoadedModuleByName(const char* name) {
        HMODULE hModules[1024];
        HANDLE hProcess;
        DWORD cbNeeded;
        unsigned int i;

        // Get a handle to the current process.
        hProcess = GetCurrentProcess();
        if (!EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded))
            return nullptr;
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
            if (hModules[i] == gl_hThisInstance)
                continue;
            TCHAR szModuleName[MAX_PATH];
            ASSERT(GetModuleFileName(hModules[i], szModuleName, _countof(szModuleName)) > 0);
            const auto basename = strrchr(szModuleName, '\\');
            if (basename && stricmp(basename + 1, name) == 0)
                return hModules[i];
        }
        return nullptr;
    }
}

unsigned int gl_ErrorState = 0;
HINSTANCE gl_hThisInstance = nullptr;

IDirect3D9* APIENTRY Direct3DCreate9(UINT SDKVersion)
{
    DISABLE_HOOK(GetProcAddress_fn);

    Message("uMod_Direct3DCreate9: uMod %p\n", Direct3DCreate9);

    ASSERT(Direct3DCreate9_ret);

    IDirect3D9* pIDirect3D9_orig = Direct3DCreate9_ret(SDKVersion); //creating the original IDirect3D9 object
    ASSERT(pIDirect3D9_orig);

    return new uMod_IDirect3D9(pIDirect3D9_orig); //return our object instead of the "real one"
}

HRESULT APIENTRY Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3D)
{
    DISABLE_HOOK(GetProcAddress_fn);

    Message("uMod_Direct3DCreate9Ex: uMod %p\n", Direct3DCreate9Ex);

    ASSERT(Direct3DCreate9Ex_ret);

    IDirect3D9Ex* pIDirect3D9Ex_orig = nullptr;
    HRESULT ret = Direct3DCreate9Ex_ret(SDKVersion, &pIDirect3D9Ex_orig); //creating the original IDirect3D9 object

    if (ret != S_OK)
        return ret;

    // @Cleanup: should be we freeing pIDirect3D9Ex at the end of our own lifecycle?
    const auto pIDirect3D9Ex = new uMod_IDirect3D9Ex(pIDirect3D9Ex_orig);
    // original umod does not do this for some reason
    *ppD3D = static_cast<IDirect3D9Ex*>(pIDirect3D9Ex);
    return ret;
}

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
    Message("InitInstance: %p\n", hModule);
    DisableThreadLibraryCalls(hModule); //reduce overhead

    // Store the handle to this module
    gl_hThisInstance = hModule;

    // d3d9.dll shouldn't be loaded at this point.
    const auto d3d9_loaded = FindLoadedModuleByName("d3d9.dll");
    ASSERT(!d3d9_loaded);

    MH_Initialize();

    // Hook into LoadLibraryA - we'll do our hooks on the flip side
    GetProcAddress_fn = reinterpret_cast<GetProcAddress_type>(GetProcAddress);
    ASSERT(GetProcAddress_fn);
    if (GetProcAddress_fn) {
        MH_CreateHook(GetProcAddress_fn, OnGetProcAddress, (void**)&GetProcAddress_ret);
        MH_EnableHook(GetProcAddress_fn);
    }
}

void ExitInstance()
{
    DISABLE_HOOK(GetProcAddress_fn);

    MH_Uninitialize();

#ifdef _DEBUG
    if (stdout_proxy)
        fclose(stdout_proxy);
    if (stderr_proxy)
        fclose(stderr_proxy);
    __try {
        FreeConsole();
    }
    __except (EXCEPTION_CONTINUE_EXECUTION) { }
#endif
}
