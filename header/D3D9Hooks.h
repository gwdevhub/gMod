#pragma once

#include <d3d9.h>

// Hooks the IDirect3D9(Ex) CreateDevice slot on the real object returned by
// Direct3DCreate9(Ex); `is_ex` also hooks CreateDeviceEx. Requires a prior
// MH_Initialize() (done in dll_main).
void InstallD3D9Hooks(IDirect3D9* d3d9, bool is_ex);

// Reverts every gMod hook, leaving the host process as it was. Safe if nothing
// was hooked.
void RemoveAllD3D9Hooks();

// Registers a device that already existed when gMod was injected, so the
// CreateDevice hook never ran: hooks its vtable and stands up a TextureClient.
// Textures created from here on are modded; pre-existing ones stay unknown.
// Returns false if the device is null or already registered.
bool RegisterExistingDevice(IDirect3DDevice9* device);
