#pragma once

#include <d3d9.h>

void InstallD3D9Hooks(IDirect3D9* d3d9, bool is_ex);

void RemoveAllD3D9Hooks();

bool RegisterExistingDevice(IDirect3DDevice9* device);
