#pragma once

//#define HashType DWORD64
using HashType = DWORD32;

#ifdef LOG_MESSAGE
#include <iostream>

#ifdef _DEBUG

#if 1
#define Message(...) { printf(__VA_ARGS__); }
#else
#define Message(...)
#endif
#define Warning(...) { fprintf(stderr, __VA_ARGS__); }

#else

#define Message(...)
#define Warning(...)

#endif

#endif

#ifndef _countof
#define _countof(arr) sizeof(arr) / sizeof(*arr)
#endif
