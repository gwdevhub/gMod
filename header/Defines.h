#pragma once

//#define HashType DWORD64
using HashType = DWORD32;

inline void Message(const char* format, ...)
{
#ifdef _DEBUG
#if 0
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#endif
#endif
}

inline void Warning(const char* format, ...)
{
#ifdef _DEBUG
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
#endif
}

