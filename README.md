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

**Disclaimer about [Reshade](https://github.com/crosire/reshade)**

Reshade in versions > 5.0.1 is known to cause glitches with TexMod, uMod and also gMod.
If you would like to use Reshade in combination with gMod, we recommend running version [5.0.1](https://github.com/crosire/reshade/releases/tag/v5.0.1) or [4.9.1](https://github.com/crosire/reshade/releases/tag/v4.9.1).

**Build from source**

Requirements:
- Visual Studio 2022
- CMake 3.16+, integrated into the Developer Powershell for VS 2022
- vcpkg, integrated into the Developer Powershell for VS 2022

Compile:
- cmake --preset=vcpkg
- cmake --open build
- compile

**TpfConvert**
Small utility to convert old .tpf files with invalid images into .zip files with working images.
Usage:
- put TpfConvert.exe and d3dx9.dll anywhere
- create a folder "plugins" in that folder and put your .tpf/.zip files with invalid images there
- run TpfConvert.exe, My_Texmod.tpf is fixed and copied into My_Texmod_.zip
- copy My_Texmod_.zip into your GW Launcher plugins folder, delete My_Texmod.tpf from it, if it still exists