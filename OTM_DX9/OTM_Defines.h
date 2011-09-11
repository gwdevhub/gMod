/*
This file is part of OpenTexMod.


OpenTexMod is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenTexMod is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FooOpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/



#ifndef OTM_DEFINES_H_
#define OTM_DEFINES_H_


#define Message(...) {if (gl_File!=NULL) {fprintf( gl_File, __VA_ARGS__); fflush(gl_File);}}
#define OpenMessage(...) {if (fopen_s( &gl_File, "OTM_log.txt", "wt")) gl_File=NULL;}
#define CloseMessage(...) {if (gl_File!=NULL) fclose(gl_File);}

//#define OpenMessage(...)
//#define Message(...)
//#define CloseMessage(...)

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


#endif /* OTM_DEFINES_H_ */
