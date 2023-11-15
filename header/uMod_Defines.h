#pragma once

#include <iostream>

#ifdef LOG_MESSAGE
extern FILE* gl_File;

#ifdef _DEBUG
#define Message(...) { printf(__VA_ARGS__); }
#else
#define Message(...)
#endif
#ifdef HOOK_INJECTION
#define OpenMessage(...) {if (fopen_s( &gl_File, "uMod_log.txt", "wt")) gl_File=NULL; else fprintf( gl_File, "HI R40: 0000000\n");}
#endif

#ifdef DIRECT_INJECTION
#define OpenMessage(...)
#endif

#ifdef NO_INJECTION
#define OpenMessage(...) {if (fopen_s( &gl_File, "uMod_log.txt", "wt")) gl_File=NULL; else fprintf( gl_File, "NI R40: 0000000\n");}
#endif

#define CloseMessage(...)


#else
#define OpenMessage(...)
#define Message(...)
#define CloseMessage(...)
#endif


#ifdef __CDT_PARSER__
typedef unsigned long DWORD64;
typedef unsigned long DWORD32;

#define STDMETHOD(method)     virtual HRESULT method
#define STDMETHOD_(ret, method)  virtual ret method
#define sprintf_s(...)
#define fprintf(...)
#define fclose(...)
#define fseek(...)
#define ftell(...) 0
#define fflush(...)
typedef LONG HRESULT;

#define UNREFERENCED_PARAMETER(...)
#endif
