***gMod***
Continuation of the uMod project to improve performance and stability. Integrated with [Guild Wars Launcher](https://github.com/gwdevhub/gwlauncher) and [Daybreak](https://github.com/gwdevhub/Daybreak).

*Usage is primarily intended with GW Launcher or Daybreak, but it can be used without.*

**Usage with manual gMod.dll injection:**
- Create a file called modlist.txt in either the Guild Wars (Gw.exe) folder, or the gMod.dll folder.
- Inject gMod.dll before d3d9.dll is loaded.

**Usage without dll injection:**
- Create a file called modlist.txt in the Guild Wars (Gw.exe) folder.
- Place gMod.dll in the Guild Wars folder
- Rename gMod.dll to d3d9.dll
- Launch Guild Wars

**Format of the modlist.txt file:**

Each line in the modlist.txt is the full path to a mod you want to load (eg. `D:\uMod\Borderless Cartography Made Easy 2015 1.3.tpf`)
gMod will load all these files on startup

**Build from source**

Requirements:
- Visual Studio 2022
- CMake 3.16+, integrated into the Developer Powershell for VS 2022

Compile:
- cmake -B build
- cmake --open build
- compile
