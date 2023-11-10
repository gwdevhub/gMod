/*
This file is part of Universal Modding Engine.


Universal Modding Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Universal Modding Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Universal Modding Engine.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef uMod_DEFINES_H_
#define uMod_DEFINES_H_

#include <iostream>

#ifdef LOG_MESSAGE
extern FILE *gl_File;

#define Message(...) { printf(__VA_ARGS__); }
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


#endif /* uMod_DEFINES_H_ */
