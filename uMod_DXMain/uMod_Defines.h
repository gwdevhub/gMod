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


#ifdef LOG_MESSAGE
extern FILE *gl_File;

#define Message(...) {if (gl_File!=NULL) {fprintf( gl_File, __VA_ARGS__); fflush(gl_File);}}
#if INJECTION_METHOD == HOOK_INJECTION
#define OpenMessage(...) {if (fopen_s( &gl_File, "uMod_log.txt", "wt")) gl_File=NULL; else fprintf( gl_File, "HI " uMod_VERSION_char ": 0000001\n");}
#endif

#if INJECTION_METHOD == DIRECT_INJECTION
#define OpenMessage(...) {if (fopen_s( &gl_File, "uMod_log.txt", "wt")) gl_File=NULL; else fprintf( gl_File, "DI " uMod_VERSION_char ": 0000001\n");}
#endif

#if INJECTION_METHOD == NO_INJECTION
#define OpenMessage(...) {if (fopen_s( &gl_File, "uMod_log.txt", "wt")) gl_File=NULL; else fprintf( gl_File, "NI " uMod_VERSION_char ": 0000001\n");}
#endif

#define CloseMessage(...) {if (gl_File!=NULL) fclose(gl_File);}


#else
/**
 * Open the file for logging (if LOG_MESSAGE=1 was passed during compilation)
 */
#define OpenMessage(...)
/**
 * Print a message (like printf) into the file ant flush the content to disk (if LOG_MESSAGE=1 was passed during compilation)
 */
#define Message(...)
/**
 * Close the file for logging (if LOG_MESSAGE=1 was passed during compilation)
 */
#define CloseMessage(...)
#endif


#ifdef __CDT_PARSER__
typedef unsigned long DWORD64;
typedef unsigned long DWORD32;

#define STDMETHOD(method)     virtual HRESULT method
#define STDMETHOD_(ret, method)  virtual ret method
#define sprintf_s(...)
#define swprintf_s(...)
#define strcat_s(...)
#define fprintf(...)
#define fclose(...)
#define fseek(...)
#define ftell(...) 0
#define fflush(...)
#define qsort(...)
typedef LONG HRESULT;

#define UNREFERENCED_PARAMETER(...)
#endif


#endif /* uMod_DEFINES_H_ */
