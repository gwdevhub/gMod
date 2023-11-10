#pragma once

#include <d3d9.h>

void InitInstance(HINSTANCE hModule);
void ExitInstance();
void LoadOriginalDll();
bool FindLoadedDll();
bool IsDesiredModule(HMODULE hModule, HANDLE hProcess);
bool HasDesiredMethods(HMODULE hModule, HANDLE hProcess);
bool HookThisProgram(char* ret);
DWORD WINAPI ServerThread(LPVOID lpParam);

void* DetourFunc(BYTE* src, const BYTE* dst, int len);
bool RetourFunc(BYTE* src, BYTE* restore, int len);
IDirect3D9*APIENTRY uMod_Direct3DCreate9(UINT SDKVersion);
HRESULT APIENTRY uMod_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3D);


#ifdef DIRECT_INJECTION
void Nothing();
#endif
