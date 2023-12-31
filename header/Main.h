#pragma once

#include <windows.h>

#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <future>
#include <map>
#include <ranges>
#include "Utils.h"

#include <d3d9.h>
#include <gsl/gsl>

#include "Defines.h"
#include "Error.h"
#include "Defines.h"

#include "uMod_IDirect3D9.h"
#include "uMod_IDirect3D9Ex.h"

#include "uMod_IDirect3DDevice9.h"
#include "uMod_IDirect3DDevice9Ex.h"

#include "uMod_IDirect3DCubeTexture9.h"
#include "uMod_IDirect3DTexture9.h"
#include "uMod_IDirect3DVolumeTexture9.h"

#pragma warning(disable : 4477)

extern unsigned int gl_ErrorState;
extern HINSTANCE gl_hThisInstance;
