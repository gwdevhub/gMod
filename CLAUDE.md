# gMod

A 32-bit Windows DLL that hooks Direct3D 9 to load texture mods for Guild Wars. Continuation of uMod. Integrated with [GW Launcher](https://github.com/gwdevhub/gwlauncher) and [Daybreak](https://github.com/gwdevhub/Daybreak).

## Project structure

- `header/` — header files (D3D9Hooks, D3D9State, Defines, Error, Main, Utils)
- `source/` — implementation files (D3D9Hooks.cpp, dll_main.cpp, Error.cpp)
- `modules/` — C++23 module files (ModfileLoader, ModfileLoader_TpfReader, TextureClient, TextureFunction)
- `TpfConvert/` — standalone utility to fix broken .tpf files
- `cmake/` — CMake helper scripts
- `build/` — CMake build tree (generated, ignored)
- `bin/` — compiled output (generated, ignored)

## Build

Requires Visual Studio 2022, CMake 3.29+, and vcpkg — all via the VS 2022 Developer PowerShell.

```
cmake --preset=vcpkg
cmake --open build
# then compile inside Visual Studio, or:
cmake --build build --config Release
```

- Target: 32-bit only (`-A Win32` / `CMAKE_GENERATOR_PLATFORM=win32`)
- Standard: C++23
- Output: `bin/` (gMod.dll, TpfConvert.exe)
- Version: set in `CMakeLists.txt` (`VERSION_MAJOR/MINOR/PATCH/TWEAK`)

## Key dependencies (via vcpkg)

- `minhook` — D3D9 function hooking
- `libzippp` / `libzip` / `zlib` — reading .tpf/.zip mod files
- `directxtex` — texture format conversion
- `Microsoft.GSL` — Guidelines Support Library

## How it works

1. `dll_main.cpp` — DLL entry point, hooks d3d9.dll via MinHook
2. `D3D9Hooks.cpp` — intercepts `Direct3DCreate9` and IDirect3DDevice9 methods
3. `ModfileLoader` — reads `modlist.txt`, loads .tpf/.zip mod archives
4. `TextureClient` / `TextureFunction` — matches and replaces textures by hash at runtime

## Notes

- Must be injected before d3d9.dll loads, or renamed to d3d9.dll in the GW folder
- Reshade > 5.0.1 causes glitches; recommend 5.0.1 or 4.9.1
- `modlist.txt` contains full paths to .tpf/.zip mod files, one per line
