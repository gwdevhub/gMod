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

#include "D3D9State.h"

#pragma warning(disable : 4477)

extern unsigned int gl_ErrorState;
extern HINSTANCE gl_hThisInstance;
inline std::filesystem::path gmod_dll_path;
