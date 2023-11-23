Requirements:
- DirectX SDK (June 2010)
- Visual Studio 2022
- CMake 3.16+, integrated into the Developer Powershell for VS 2022

Compile:
- cmake -B build
- cmake --open build
- compile

Usage:
- Create a file called modlist.txt in either the Guild Wars (Gw.exe) folder, or the gMod.dll folder.
Each line in the modlist.txt is the full path to a mod you want to load (eg. D:\uMod\Mods\Borderless Cartography Made Easy 2015 1.3.tpf)
gMod will load all these files on startup
