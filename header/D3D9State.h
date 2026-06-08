#pragma once

#include <d3d9.h>
#include "Defines.h"

// Per-texture side-state, keyed by the real texture pointer (see D3D9Hooks.h).
// Held out-of-band so reverting the vtable hooks fully detaches gMod.

enum class TexType {
    Tex2D,
    Volume,
    Cube,
};

// State for one texture: an "original" the game created, or a "fake" we loaded
// from a mod to stand in for one.
struct TexState {
    IDirect3DBaseTexture9* real = nullptr;
    // An original points at the fake bound in its place (or nullptr); a fake
    // points back at the original it replaces.
    TexState* partner = nullptr;
    IDirect3DDevice9* device = nullptr;
    TextureFileStruct* reference = nullptr; // the modfile backing a fake
    HashTuple hash = {};
    TexType type = TexType::Tex2D;
    bool isFake = false;
};

// Content hash (CRC32 + optional CRC64) of an original texture. Defined in
// D3D9Hooks.cpp.
HashTuple GetTextureHash(const TexState* state);
