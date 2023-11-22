#pragma once

//#define HashType DWORD64
using HashType = DWORD32;

#ifdef LOG_MESSAGE
#include <iostream>

#ifdef _DEBUG
#define Message(...) { printf(__VA_ARGS__); }
#else
#define Message(...)
#endif

#endif
