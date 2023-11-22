#pragma once

#include <d3d9.h>

void InitInstance(HINSTANCE hModule);
void ExitInstance();
HMODULE LoadOriginalDll();

IDirect3D9* APIENTRY uMod_Direct3DCreate9(UINT SDKVersion);
HRESULT APIENTRY uMod_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3D);
