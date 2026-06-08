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

> **Cannot be built or run in this agent environment** (Linux, no MSVC, no 32-bit
> D3D9, no Guild Wars to inject into). Don't burn turns attempting `cmake`/builds
> here — validate changes by reading and matching existing patterns, and lean on
> Windows CI (`ci.yaml`) for the real build. When an actual compile/test is needed,
> ask the user to do it in Visual Studio rather than trying locally.

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

### Hooking model (read before touching D3D9 code)

gMod uses **MinHook vtable hooks**, not proxy/subclass wrappers (the old uMod model
was replaced — see `git log` "Replace D3D9 proxy wrappers with vtable hooks"). The
game keeps the **real** `IDirect3D9` / `IDirect3DDevice9` / texture objects byte-for-byte
untouched; gMod patches only the individual vtable slots it cares about (CreateDevice,
CreateTexture/Volume/Cube, UpdateTexture, BeginScene, Set/GetTexture, Release).

- Per-texture side-state lives **out of band** in `header/D3D9State.h` (`TexState`,
  keyed by the real texture pointer), so reverting the hooks fully detaches gMod.
- `RemoveAllD3D9Hooks()` must leave the host process pristine — preserve that invariant.
- A vtable is shared by all instances of a class, so each slot is hooked once; texture
  Release slots are hooked lazily on first instance. Don't double-hook.

## Conventions

- **Formatting is CI-enforced** by clang-format `22.1.5` (`.clang-format`, applied by
  `lint.yaml` on push to `dev`). Don't hand-reformat or "tidy" whole files — it just
  churns the diff. Notable rules: `ColumnLimit: 0` (never reflow long D3D signatures /
  vtable typedefs), `SortIncludes: Never` (include order is load-bearing in Windows
  headers — never reorder), 4-space indent, braces on their own line after functions.
- **C++23 named modules** (`modules/*.ixx`). Editing a module *interface* triggers wide
  rebuilds; prefer changing implementation over interface when possible.
- **Public exported API is a fixed surface** — four `extern "C" __declspec(dllexport)`
  functions in `dll_main.cpp`: `SetDevice`, `AddFile`, `RemoveFile`, `GetFiles`. These
  are consumed by GW Launcher and Daybreak; do **not** change their names, signatures,
  or calling convention (`__cdecl`) without coordinating downstream.
- **Commits**: short imperative summaries, lowercase, no trailing period
  (e.g. "Keep loaded_files in load order so GetFiles returns priority order").
- **Branching**: `dev` is the integration branch; PRs target `master`. CI builds on
  push to `dev` and on PRs to `master`.

### Where things live (by size / how often they change)

- `modules/TextureClient.ixx` (~670 lines) — texture match/replace bookkeeping, hot
- `source/D3D9Hooks.cpp` (~580) — the vtable detours and install/remove
- `modules/TextureFunction.ixx` (~380) — texture creation/conversion helpers
- `source/dll_main.cpp` (~345) — entry point + exported C API
- `modules/ModfileLoader*.ixx` — .tpf/.zip parsing
- `header/Defines.h` — shared structs/constants (`TextureFileStruct`, `HashTuple`, etc.)

## Notes

- Must be injected before d3d9.dll loads, or renamed to d3d9.dll in the GW folder
- Reshade > 5.0.1 causes glitches; recommend 5.0.1 or 4.9.1
- `modlist.txt` contains full paths to .tpf/.zip mod files, one per line
