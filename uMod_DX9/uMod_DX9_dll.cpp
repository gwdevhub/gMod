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

/*


 NEVER USE THIS CODE FOR ILLEGAL PURPOSE


*/


#include "uMod_DX9_dll.h"
#include "uMod_IDirect3D9.h"
#include "uMod_IDirect3D9Ex.h"


/*
 * global variable which are not linked external
 */
#if INJECTION_METHOD==NO_INJECTION
HINSTANCE             gl_hOriginal_DX9_Dll = NULL;
#endif


#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION

typedef IDirect3D9 *(APIENTRY *Direct3DCreate9_type)(UINT);
typedef HRESULT (APIENTRY *Direct3DCreate9Ex_type)(UINT SDKVersion, IDirect3D9Ex **ppD3D);

uMod_Detour_Entry<Direct3DCreate9_type> Detour_Direct3DCreate9(5);
uMod_Detour_Entry<Direct3DCreate9Ex_type> Detour_Direct3DCreate9Ex(5);

#endif



void InitDX9(void)
{

#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION

  char buffer[MAX_PATH];
  wchar_t buffer_w[MAX_PATH];
  GetSystemDirectory(buffer,MAX_PATH); //get the system directory, we need to open the original d3d9.dll

  swprintf_s( buffer_w, MAX_PATH, L"%s\\d3d9.dll", buffer);
  strcat_s( buffer, MAX_PATH,"\\d3d9.dll");

  Detour_Direct3DCreate9.SetFunctionName( "Direct3DCreate9");
  Detour_Direct3DCreate9.SetTargetFunction( uMod_Direct3DCreate9);
  Detour_Direct3DCreate9.SetLibName( "d3d9.dll");
  Detour_Direct3DCreate9.SetFullLibName( buffer);
  Detour_Direct3DCreate9.SetLibName( L"d3d9.dll"); // set also for wide character
  Detour_Direct3DCreate9.SetFullLibName( buffer_w); // set also for wide character

  Detour_Direct3DCreate9Ex.SetFunctionName( "Direct3DCreate9Ex");
  Detour_Direct3DCreate9Ex.SetTargetFunction( uMod_Direct3DCreate9Ex);
  Detour_Direct3DCreate9Ex.SetLibName( "d3d9.dll");
  Detour_Direct3DCreate9Ex.SetFullLibName( buffer);
  Detour_Direct3DCreate9Ex.SetLibName( L"d3d9.dll"); // set also for wide character
  Detour_Direct3DCreate9Ex.SetFullLibName( buffer_w); // set also for wide character

  GlobalDetour.AddEntry(&Detour_Direct3DCreate9);
  GlobalDetour.AddEntry(&Detour_Direct3DCreate9Ex);
  //char buffer[MAX_PATH];
  //if (gl_hOriginal_DX9_Dll==NULL)
  {
    GetSystemDirectory(buffer,MAX_PATH); //get the system directory, we need to open the original d3d9.dll

    // Append dll name
    strcat_s( buffer, MAX_PATH,"\\d3d9.dll");

    // try to load the system's d3d9.dll
    HANDLE gl_hOriginal_DX9_Dll = LoadLibrary(buffer);
  }
#endif

#if INJECTION_METHOD==NO_INJECTION
  LoadOriginal_DX9_Dll();
#endif
}

void ExitDX9(void)
{
  // Release the system's d3d9.dll
#if INJECTION_METHOD==NO_INJECTION
  if (gl_hOriginal_DX9_Dll!=NULL)
  {
    FreeLibrary(gl_hOriginal_DX9_Dll);
    gl_hOriginal_DX9_Dll = NULL;
  }
#endif


#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION
#endif

}




#if INJECTION_METHOD==NO_INJECTION
/*
 * We do not inject, the game loads this dll by itself thus we must include the Direct3DCreate9 function
 */
void LoadOriginal_DX9_Dll(void)
{
  char buffer[MAX_PATH];
  if (gl_hOriginal_DX9_Dll==NULL)
  {
    GetSystemDirectory(buffer,MAX_PATH); //get the system directory, we need to open the original d3d9.dll

    // Append dll name
    strcat_s( buffer, MAX_PATH,"\\d3d9.dll");

    // try to load the system's d3d9.dll
    gl_hOriginal_DX9_Dll = LoadLibrary(buffer);
  }
}

IDirect3D9* WINAPI  Direct3DCreate9(UINT SDKVersion)
{
  Message("WINAPI  Direct3DCreate9\n");

	if (gl_hOriginal_DX9_Dll==NULL) LoadOriginal_DX9_Dll(); // looking for the "original d3d9.dll"
	if (gl_hOriginal_DX9_Dll==NULL) return (NULL);
	
	// find original function in original d3d9.dll
	Direct3DCreate9_type D3DCreate9_fn = (Direct3DCreate9_type) GetProcAddress( gl_hOriginal_DX9_Dll, "Direct3DCreate9");
    

	if (!D3DCreate9_fn) 
  {
	  Message("Direct3DCreate9: original function not found in dll\n");
    return (NULL);
  }
	

	//Create originale IDirect3D9 object
	IDirect3D9 *pIDirect3D9_orig = D3DCreate9_fn(SDKVersion);

	//create our uMod_IDirect3D9 object
	uMod_IDirect3D9 *pIDirect3D9 = new uMod_IDirect3D9( pIDirect3D9_orig, gl_TextureServer);

	// Return pointer to our object instead of "real one"
	return (pIDirect3D9);
}

HRESULT WINAPI  Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex **ppD3D)
{
  Message("WINAPI  Direct3DCreate9Ex\n");

  if (gl_hOriginal_DX9_Dll==NULL) LoadOriginal_DX9_Dll(); // looking for the "original d3d9.dll"
  if (gl_hOriginal_DX9_Dll==NULL) return (NULL);

  // find original function in original d3d9.dll
  Direct3DCreate9Ex_type D3DCreate9Ex_fn = (Direct3DCreate9Ex_type) GetProcAddress( gl_hOriginal_DX9_Dll, "Direct3DCreate9Ex");


  if (!D3DCreate9Ex_fn)
  {
    Message("Direct3DCreate9Ex: original function not found in dll\n");
    return (D3DERR_NOTAVAILABLE);
  }


  //Create originale IDirect3D9 object
  IDirect3D9Ex *pIDirect3D9Ex_orig;
  HRESULT ret = D3DCreate9Ex_fn( SDKVersion, &pIDirect3D9Ex_orig);
  if (ret!=S_OK) return (ret);

  //create our uMod_IDirect3D9 object
  uMod_IDirect3D9Ex *pIDirect3D9Ex = new uMod_IDirect3D9Ex( pIDirect3D9Ex_orig, gl_TextureServer);

  ppD3D = &pIDirect3D9Ex_orig; // Return pointer to our object instead of "real one"
  return (ret);
}

typedef IDirect3D9 *(APIENTRY *Direct3DCreate9_type)(UINT);
typedef HRESULT (APIENTRY *Direct3DCreate9Ex_type)(UINT SDKVersion, IDirect3D9Ex **ppD3D);

typedef int (WINAPI *D3DPERF_BeginEvent_type)( D3DCOLOR , LPCWSTR  );
int WINAPI D3DPERF_BeginEvent( D3DCOLOR col, LPCWSTR wszName )
{
  D3DPERF_BeginEvent_type fn = (D3DPERF_BeginEvent_type) GetProcAddress( gl_hOriginal_DX9_Dll, "D3DPERF_BeginEvent");
  return fn( col, wszName);
}

typedef int (WINAPI *D3DPERF_EndEvent_type)( void );
int WINAPI D3DPERF_EndEvent( void )
{
  D3DPERF_EndEvent_type fn = (D3DPERF_EndEvent_type) GetProcAddress( gl_hOriginal_DX9_Dll, "D3DPERF_EndEvent");
  return fn();
}

typedef void (WINAPI *D3DPERF_SetMarker_type)( D3DCOLOR , LPCWSTR );
void WINAPI D3DPERF_SetMarker( D3DCOLOR col, LPCWSTR wszName )
{
  D3DPERF_SetMarker_type fn = (D3DPERF_SetMarker_type) GetProcAddress( gl_hOriginal_DX9_Dll, "D3DPERF_SetMarker");
  return fn( col, wszName);
}

typedef void (WINAPI *D3DPERF_SetRegion_type)( D3DCOLOR, LPCWSTR );
void WINAPI D3DPERF_SetRegion( D3DCOLOR col, LPCWSTR wszName )
{
  D3DPERF_SetRegion_type fn = (D3DPERF_SetRegion_type) GetProcAddress( gl_hOriginal_DX9_Dll, "D3DPERF_SetRegion");
  return fn( col, wszName);
}

typedef BOOL (WINAPI *D3DPERF_QueryRepeatFrame_type)( void );
BOOL WINAPI D3DPERF_QueryRepeatFrame( void )
{
  D3DPERF_QueryRepeatFrame_type fn = (D3DPERF_QueryRepeatFrame_type) GetProcAddress( gl_hOriginal_DX9_Dll, "D3DPERF_QueryRepeatFrame");
  return fn( );
}

typedef void (WINAPI *D3DPERF_SetOptions_type)( DWORD );
void WINAPI D3DPERF_SetOptions( DWORD dwOptions )
{
  D3DPERF_SetOptions_type fn = (D3DPERF_SetOptions_type) GetProcAddress( gl_hOriginal_DX9_Dll, "D3DPERF_SetOptions");
  return fn( dwOptions);
}

typedef DWORD (WINAPI *D3DPERF_GetStatus_type)( void );
DWORD WINAPI D3DPERF_GetStatus( void )
{
  D3DPERF_GetStatus_type fn = (D3DPERF_GetStatus_type) GetProcAddress( gl_hOriginal_DX9_Dll, "D3DPERF_GetStatus");
  return fn( );
}

#endif




#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION

/*
 * We inject the dll into the game, thus we retour the original Direct3DCreate9 function to our MyDirect3DCreate9 function
 */

IDirect3D9 *APIENTRY uMod_Direct3DCreate9(UINT SDKVersion)
{
  Message("uMod_Direct3DCreate9:  original %p, uMod %p\n", Detour_Direct3DCreate9.Function(), uMod_Direct3DCreate9);

  IDirect3D9 *pIDirect3D9_orig = NULL;
  uMod_IDirect3D9 *pIDirect3D9 = NULL;

  Detour_Direct3DCreate9.Retour();

  pIDirect3D9_orig = Detour_Direct3DCreate9.Function()(SDKVersion); //creating the original IDirect3D9 object

  if (pIDirect3D9_orig)
  {
    pIDirect3D9 = new uMod_IDirect3D9( pIDirect3D9_orig, gl_TextureServer); //creating our uMod_IDirect3D9 object
  }
  Detour_Direct3DCreate9.Detour();

  return (pIDirect3D9); //return our object instead of the "real one"
}


HRESULT APIENTRY uMod_Direct3DCreate9Ex( UINT SDKVersion, IDirect3D9Ex **ppD3D)
{
  Message( "uMod_Direct3DCreate9Ex:  original %p, uMod %p\n", Detour_Direct3DCreate9Ex.Function(), uMod_Direct3DCreate9Ex);

  IDirect3D9Ex *pIDirect3D9Ex_orig = NULL;
  uMod_IDirect3D9Ex *pIDirect3D9Ex = NULL;
  HRESULT ret = D3DERR_NOTAVAILABLE;

  Detour_Direct3DCreate9Ex.Retour();
  ret = Detour_Direct3DCreate9Ex.Function()(SDKVersion, &pIDirect3D9Ex_orig); //creating the original IDirect3D9 object

  if (ret!=S_OK) return (ret);

  if (pIDirect3D9Ex_orig)
  {
    pIDirect3D9Ex = new uMod_IDirect3D9Ex( pIDirect3D9Ex_orig, gl_TextureServer); //creating our uMod_IDirect3D9 object
  }
  Detour_Direct3DCreate9Ex.Detour();

  ppD3D = (IDirect3D9Ex**) &pIDirect3D9Ex; //return our object instead of the "real one"
  return (ret);
}


#endif
