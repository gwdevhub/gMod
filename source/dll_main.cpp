#include "Main.h"
#include <Psapi.h>
#include "MinHook.h"
#include "D3D9Hooks.h"
#include <atomic>

import TextureClient;

void ExitInstance(bool is_unloading);
void InitInstance(HINSTANCE hModule);

namespace {

#define DISABLE_HOOK(var) \
    if (var) { MH_DisableHook(var); }

    using Direct3DCreate9_type = IDirect3D9*(APIENTRY*)(UINT);
    using Direct3DCreate9Ex_type = HRESULT(APIENTRY*)(UINT SDKVersion, IDirect3D9Ex** ppD3D);
    using GetProcAddress_type = FARPROC(APIENTRY*)(HMODULE, LPCSTR);

    // Pointer to original address of Direct3DCreate9
    Direct3DCreate9_type Direct3DCreate9_ret = nullptr;

    // Pointer to original address of Direct3DCreate9
    Direct3DCreate9Ex_type Direct3DCreate9Ex_ret = nullptr;

    GetProcAddress_type GetProcAddress_fn = nullptr;
    GetProcAddress_type GetProcAddress_ret = nullptr;

    FILE* stdout_proxy;
    FILE* stderr_proxy;

    HMODULE gMod_Loaded_d3d9_Module_Handle = nullptr;

    HMODULE FindLoadedModuleByName(const char* name, bool include_this_module = false)
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
            if (hModules[i] == gl_hThisInstance && !include_this_module)
                continue;
            TCHAR szModuleName[MAX_PATH];
            ASSERT(GetModuleFileName(hModules[i], szModuleName, _countof(szModuleName)) > 0);
            const auto basename = strrchr(szModuleName, '\\');
            if (basename && _stricmp(basename + 1, name) == 0)
                return hModules[i];
        }
        return nullptr;
    }

    HMODULE LoadD3d9Dll()
    {
        HMODULE found = FindLoadedModuleByName("d3d9.dll");
        if (!found) {
            char executable_path[MAX_PATH]{};
            ASSERT(GetModuleFileName(GetModuleHandle(nullptr), executable_path, _countof(executable_path)) > 0);

            char dll_path[MAX_PATH]{};
            ASSERT(GetModuleFileName(gl_hThisInstance, dll_path, _countof(dll_path)) > 0);

            const auto exe_path = std::filesystem::path(executable_path);
            const auto gmod_path = std::filesystem::path(dll_path);

            if (exe_path.parent_path() != gmod_path.parent_path() || gmod_path.filename() != "d3d9.dll") {
                // Call basic LoadLibrary function; we're not in the same directory as the exe.
                gMod_Loaded_d3d9_Module_Handle = LoadLibrary("d3d9.dll");
            }
            if (!gMod_Loaded_d3d9_Module_Handle) {
                // Tried resolving d3d9.dll locally, didn't work. Try system directory
                char buffer[MAX_PATH];
                ASSERT(GetSystemDirectory(buffer, _countof(buffer)) > 0); // get the system directory, we need to open the original d3d9.dll

                // Append dll name
                strcat_s(buffer, _countof(buffer), "\\d3d9.dll");
                gMod_Loaded_d3d9_Module_Handle = LoadLibrary(buffer);
            }

            ASSERT(gMod_Loaded_d3d9_Module_Handle);

            found = FindLoadedModuleByName("d3d9.dll");
            ASSERT(found && found == gMod_Loaded_d3d9_Module_Handle);
        }

        DISABLE_HOOK(GetProcAddress_fn);
        // GetProcAddress, hooked via OnGetProcAddress
        Direct3DCreate9_ret = reinterpret_cast<Direct3DCreate9_type>(GetProcAddress(found, "Direct3DCreate9"));
        Direct3DCreate9Ex_ret = reinterpret_cast<Direct3DCreate9Ex_type>(GetProcAddress(found, "Direct3DCreate9Ex"));

        return found;
    }

    FARPROC APIENTRY OnGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
    {
        ASSERT(GetProcAddress_ret);
        if ((int)lpProcName < 0xffff)
            return GetProcAddress_ret(hModule, lpProcName); // lpProcName is ordinal offset, not string

        if (strcmp(lpProcName, "Direct3DCreate9") == 0) {
            Direct3DCreate9_ret = reinterpret_cast<Direct3DCreate9_type>(GetProcAddress_ret(hModule, lpProcName));
            return reinterpret_cast<FARPROC>(Direct3DCreate9);
        }
        if (strcmp(lpProcName, "Direct3DCreate9Ex") == 0) {
            Direct3DCreate9Ex_ret = reinterpret_cast<Direct3DCreate9Ex_type>(GetProcAddress_ret(hModule, lpProcName));
            return reinterpret_cast<FARPROC>(Direct3DCreate9Ex);
        }
        return GetProcAddress_ret(hModule, lpProcName);
    }

    // If the original d3d9 function is nullptr or points to gMod, load the actual d3d9 dll and redirect the addresses
    void CheckLoadD3d9Dll()
    {
        if (!(Direct3DCreate9_ret && Direct3DCreate9_ret != Direct3DCreate9)) {
            ASSERT(LoadD3d9Dll());
            ASSERT(Direct3DCreate9_ret && Direct3DCreate9_ret != Direct3DCreate9);
        }
        if (!(Direct3DCreate9Ex_ret && Direct3DCreate9Ex_ret != Direct3DCreate9Ex)) {
            ASSERT(LoadD3d9Dll());
            ASSERT(Direct3DCreate9Ex_ret && Direct3DCreate9Ex_ret != Direct3DCreate9Ex);
        }
    }

    // There may be a sitation where more than 1 gmod is loaded; avoid recursions!
    bool creating_d3d9 = false;
}

unsigned int gl_ErrorState = 0;
HINSTANCE gl_hThisInstance = nullptr;

IDirect3D9* APIENTRY Direct3DCreate9(UINT SDKVersion)
{
    Message("uMod_Direct3DCreate9: uMod %p\n", Direct3DCreate9);

    ASSERT(!creating_d3d9);
    creating_d3d9 = true;

    DISABLE_HOOK(GetProcAddress_fn);
    CheckLoadD3d9Dll();

    IDirect3D9* pIDirect3D9_orig = Direct3DCreate9_ret(SDKVersion); // creating the original IDirect3D9 object
    ASSERT(pIDirect3D9_orig);

    creating_d3d9 = false;

    // Hook the vtable and hand the game back the real object untouched.
    InstallD3D9Hooks(pIDirect3D9_orig, false);
    return pIDirect3D9_orig;
}

HRESULT APIENTRY Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3D)
{
    Message("uMod_Direct3DCreate9Ex: uMod %p\n", Direct3DCreate9Ex);

    ASSERT(!creating_d3d9);
    creating_d3d9 = true;


    DISABLE_HOOK(GetProcAddress_fn);
    CheckLoadD3d9Dll();

    IDirect3D9Ex* pIDirect3D9Ex_orig = nullptr;
    HRESULT ret = Direct3DCreate9Ex_ret(SDKVersion, &pIDirect3D9Ex_orig); // creating the original IDirect3D9 object

    creating_d3d9 = false;

    if (ret != S_OK)
        return ret;

    // Hook the vtable (CreateDevice + CreateDeviceEx) and return the real object untouched.
    InstallD3D9Hooks(pIDirect3D9Ex_orig, true);
    *ppD3D = pIDirect3D9Ex_orig;
    return ret;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
#ifdef _DEBUG
            wchar_t dllFilePath[512 + 1]{};
            GetModuleFileNameW(hModule, dllFilePath, 512);
            gmod_dll_path = dllFilePath;
            AllocConsole();
            SetConsoleTitleA("gMod Console");
            freopen_s(&stdout_proxy, "CONOUT$", "w", stdout);
            freopen_s(&stderr_proxy, "CONOUT$", "w", stderr);
#endif
            InitInstance(hModule);
            break;
        }
        case DLL_PROCESS_DETACH: {
            // Process exit (lpReserved != nullptr): other threads are gone and the OS
            // reclaims everything, so touching MinHook/the device here only risks a hang.
            if (lpReserved != nullptr)
                break;
            // FreeLibrary unload: the device is still live, so tear down cleanly.
            ExitInstance(true);
            break;
        }
        default: break;
    }

    return true;
}

void LoadModlists()
{
    Message("Initialize: searching for modlist.txt\n");
    char gwpath[MAX_PATH]{};
    GetModuleFileName(GetModuleHandle(nullptr), gwpath, MAX_PATH); // ask for name and path of this executable
    char dllpath[MAX_PATH]{};
    GetModuleFileName(gl_hThisInstance, dllpath, MAX_PATH); // ask for name and path of this dll
    const auto exe_path = std::filesystem::path(gwpath).parent_path();
    const auto dll_path = std::filesystem::path(dllpath).parent_path();
    for (const auto& path : {exe_path, dll_path}) {
        const auto modlist = path / "modlist.txt";
        if (std::filesystem::exists(modlist)) {
            Message("Initialize: found %s\n", modlist.string().c_str());
            std::ifstream t(modlist, std::ios::binary);
            std::stringstream buffer;
            buffer << t.rdbuf();
            modlists_contents.emplace_back(modlist.string(), buffer.str());
        }
    }
}

void InitInstance(HINSTANCE hModule)
{
    Message("InitInstance: %p\n", hModule);

    // Store the handle to this module
    gl_hThisInstance = hModule;

    LoadModlists();
    DisableThreadLibraryCalls(hModule); // reduce overhead

    // d3d9.dll shouldn't be loaded at this point.
    [[maybe_unused]] const auto d3d9_loaded = FindLoadedModuleByName("d3d9.dll");
    // ASSERT(!d3d9_loaded);

    MH_Initialize();

    // Hook into LoadLibraryA - we'll do our hooks on the flip side
    GetProcAddress_fn = reinterpret_cast<GetProcAddress_type>(GetProcAddress);
    ASSERT(GetProcAddress_fn);
    if (GetProcAddress_fn) {
        MH_CreateHook(GetProcAddress_fn, OnGetProcAddress, (void**)&GetProcAddress_ret);
        MH_EnableHook(GetProcAddress_fn);
        // Re-JIT the patched prologue + trampoline under the x86-on-ARM64 emulator (no-op native).
        const HANDLE proc = GetCurrentProcess();
        FlushInstructionCache(proc, (void*)GetProcAddress_fn, 64);
        if (GetProcAddress_ret) FlushInstructionCache(proc, (void*)GetProcAddress_ret, 64);
    }
}

// Exported entry for late injection: hand gMod an existing device so it hooks
// the vtable and mods textures from here on.
extern "C" __declspec(dllexport) int __cdecl SetDevice(IDirect3DDevice9* device)
{
    if (!device) return RETURN_BAD_ARGUMENT;
    try {
        return RegisterExistingDevice(device) ? RETURN_OK : RETURN_EXISTS;
    }
    catch (...) {
        return RETURN_FATAL_ERROR;
    }
}

extern "C" __declspec(dllexport) int __cdecl AddFile(const wchar_t* path)
{
    if (!path) return RETURN_BAD_ARGUMENT;
    try {
        return TextureClient::AddFile(std::filesystem::path(path));
    }
    catch (...) {
        return RETURN_FATAL_ERROR;
    }
}

extern "C" __declspec(dllexport) int __cdecl RemoveFile(const wchar_t* path)
{
    if (!path) return RETURN_BAD_ARGUMENT;
    try {
        return TextureClient::RemoveFile(std::filesystem::path(path));
    }
    catch (...) {
        return RETURN_FATAL_ERROR;
    }
}

// nullptr first arg = return required size; paths come back in load (priority) order
// returns paths separated by null terminator, e.g. "C:\foo.tpf\0C:\bar.zip\0\0"
extern "C" __declspec(dllexport) int __cdecl GetFiles(wchar_t* buffer, const size_t buffer_size_chars)
{
    try {
        const auto files = TextureClient::GetFiles();
        size_t required = 1;
        for (const auto& path : files) {
            required += path.native().size() + 1;
        }

        if (buffer && buffer_size_chars > 0 && buffer_size_chars >= required) {
            wchar_t* out = buffer;
            for (const auto& path : files) {
                const auto& native = path.native();
                std::ranges::copy(native, out);
                out += native.size();
                *out++ = L'\0';
            }
            *out = L'\0';
        }
        return static_cast<int>(required);
    }
    catch (...) {
        return RETURN_FATAL_ERROR;
    }
}

// Optional clean-shutdown entry: call this from the host (on a normal thread) BEFORE
// FreeLibrary so teardown runs off the loader lock, where MinHook can safely suspend
// the render thread. A later FreeLibrary then unloads with nothing left to undo.
extern "C" __declspec(dllexport) void __cdecl Shutdown()
{
    ExitInstance(true);
}

void ExitInstance(bool is_unloading)
{
    // Teardown must run exactly once, whether reached via Shutdown() or DllMain detach.
    static std::atomic<bool> torn_down{false};
    if (torn_down.exchange(true)) return;

    DISABLE_HOOK(GetProcAddress_fn);

    // Revert every D3D9 vtable hook so the original objects are left pristine.
    RemoveAllD3D9Hooks();

    // On a real unload the device is still alive; release our replacement textures.
    if (is_unloading) {
        DestroyAllTextureClients();
    }

    MH_Uninitialize();

    if (gMod_Loaded_d3d9_Module_Handle)
        FreeLibrary(gMod_Loaded_d3d9_Module_Handle);

#ifdef _DEBUG
    if (stdout_proxy)
        fclose(stdout_proxy);
    if (stderr_proxy)
        fclose(stderr_proxy);
    __try {
        FreeConsole();
    }
    __except (EXCEPTION_CONTINUE_EXECUTION) {
    }
#endif
}
