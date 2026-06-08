#pragma once

#include <d3d9.h>

void InstallD3D9Hooks(IDirect3D9* d3d9, bool is_ex);

void RemoveAllD3D9Hooks();

// Delete every per-device TextureClient, releasing its replacement textures. Only safe
// on a real FreeLibrary (device still alive), not during process termination.
void DestroyAllTextureClients();

bool RegisterExistingDevice(IDirect3DDevice9* device);
