#include "dll_main.h"

#include <array>
#include <Windows.h>
#include "Main.h"
#include <Psapi.h>

#include "MinHook.h"
#include <filesystem>

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

    // If not nullptr, we're responsible for freeing this library on termination
    HMODULE gMod_Loaded_d3d9_Module_Handle = nullptr;

    // If this hModule called d3d9.dll?
    bool IsD3d9Dll(HMODULE hModule)
    {
        TCHAR szModuleName[MAX_PATH];
        ASSERT(GetModuleFileName(hModule, szModuleName, sizeof(szModuleName) / sizeof(*szModuleName)) > 0);
        const auto basename = strrchr(szModuleName, '\\');
        return basename && strcmp(basename + 1, "d3d9.dll") == 0;
    }
    // Does this module contain exported function calls for creating a d3d9 device?
    bool HasD3d9Methods(HMODULE hModule)
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
            if (hModules[i] == gMod_Module_Handle)
                continue;
            if (IsD3d9Dll(hModules[i])) {
                return hModules[i];
            }
        }
        return nullptr;
    }
}

unsigned int gl_ErrorState = 0;
HINSTANCE gl_hThisInstance = nullptr;

IDirect3D9 * APIENTRY Direct3DCreate9(UINT SDKVersion)
{
    Message("uMod_Direct3DCreate9:  original %p, uMod %p\n", Direct3DCreate9_fn, Direct3DCreate9);

    ASSERT(Direct3DCreate9_ret);

    IDirect3D9* pIDirect3D9_orig = Direct3DCreate9_ret(SDKVersion); //creating the original IDirect3D9 object
    ASSERT(pIDirect3D9_orig);

    return new uMod_IDirect3D9(pIDirect3D9_orig); //return our object instead of the "real one"
}
HRESULT APIENTRY Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3D)
{
    Message("uMod_Direct3DCreate9Ex:  original %p, uMod %p\n", Direct3DCreate9Ex_fn, Direct3DCreate9Ex);

    ASSERT(Direct3DCreate9Ex_ret);

    IDirect3D9Ex* pIDirect3D9Ex_orig = nullptr;
    HRESULT ret = Direct3DCreate9Ex_ret(SDKVersion, &pIDirect3D9Ex_orig); //creating the original IDirect3D9 object

    if (ret != S_OK)
        return ret;

    // @Cleanup: should be we freeing pIDirect3D9Ex at the end of our own lifecycle?
    uMod_IDirect3D9Ex* pIDirect3D9Ex = new uMod_IDirect3D9Ex(pIDirect3D9Ex_orig);
    ppD3D = (IDirect3D9Ex**)&pIDirect3D9Ex;
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
    gMod_Module_Handle = hModule;

    const auto d3d9_dll = LoadOriginalDll();
    ASSERT(d3d9_dll);

    MH_Initialize();

    // Hook our loaded Dll's dx9 calls though to uMod's functions
    Direct3DCreate9_fn = reinterpret_cast<Direct3DCreate9_type>(GetProcAddress(d3d9_dll, "Direct3DCreate9"));
    ASSERT(Direct3DCreate9_fn);
    if (Direct3DCreate9_fn) {
        MH_CreateHook(Direct3DCreate9_fn, Direct3DCreate9, (void**)&Direct3DCreate9_ret);
        MH_EnableHook(Direct3DCreate9_fn);
    }

    Direct3DCreate9Ex_fn = reinterpret_cast<Direct3DCreate9Ex_type>(GetProcAddress(d3d9_dll, "Direct3DCreate9Ex"));
    ASSERT(Direct3DCreate9Ex_fn);
    if (Direct3DCreate9Ex_fn) {
        MH_CreateHook(Direct3DCreate9Ex_fn, Direct3DCreate9Ex, (void**)&Direct3DCreate9Ex_ret);
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
    if (gMod_Loaded_d3d9_Module_Handle != nullptr) {
        ASSERT(FreeLibrary(gMod_Loaded_d3d9_Module_Handle));
        gMod_Loaded_d3d9_Module_Handle = nullptr;
    }

#ifdef _DEBUG
    if (stdout_proxy)
        fclose(stdout_proxy);
    if (stderr_proxy)
        fclose(stderr_proxy);
    __try {
        FreeConsole();
    }
    __except(EXCEPTION_CONTINUE_EXECUTION) {
    }
#endif
}

HMODULE LoadOriginalDll()
{
    HMODULE found = FindLoadedDll();
    if (found)
        return found;

    char executable_path[MAX_PATH]{};
    ASSERT(GetModuleFileName(GetModuleHandle(nullptr), executable_path, _countof(executable_path)) > 0);

    char gMod_path[MAX_PATH]{};
    ASSERT(GetModuleFileName(gl_hThisInstance, gMod_path, _countof(gMod_path)) > 0);

    const auto exe_fs_path = std::filesystem::path(executable_path);
    const auto gMod_fs_path = std::filesystem::path(gMod_path);

    if (exe_fs_path.parent_path() != gMod_fs_path.parent_path()
        || gMod_fs_path.filename() != "d3d9.dll") {
        // Call basic LoadLibrary function; we're not in the same directory as the exe.
        gMod_Loaded_d3d9_Module_Handle = LoadLibrary("d3d9.dll");
    }
    else {
        // We're in the same directory as the exe, and we're called 'd3d9.dll'. Calling vanilla "LoadLibrary" will be recursive!
        char buffer[MAX_PATH];
        ASSERT(GetSystemDirectory(buffer, _countof(buffer) > 0)); //get the system directory, we need to open the original d3d9.dll

        // Append dll name
        strcat_s(buffer, _countof(buffer), "\\d3d9.dll");
        gMod_Loaded_d3d9_Module_Handle = LoadLibrary(buffer);
    }

    ASSERT(gMod_Loaded_d3d9_Module_Handle);

    found = FindLoadedDll();
    ASSERT(found && found == gMod_Loaded_d3d9_Module_Handle);

    return found;
}

/*
 * We inject the dll into the game, thus we retour the original Direct3DCreate9 function to our MyDirect3DCreate9 function
 */




