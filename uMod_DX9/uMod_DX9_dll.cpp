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
//#include "detours.h"
//#include "detourxs/detourxs/detourxs.h"

/*
#include "detourxs/detourxs/ADE32.cpp"
#include "detourxs/detourxs/detourxs.cpp"
*/
/*
 * global variable which are not linked external
 */
HINSTANCE             gl_hOriginal_DX9_Dll = NULL;

typedef IDirect3D9 *(APIENTRY *Direct3DCreate9_type)(UINT);
typedef HRESULT (APIENTRY *Direct3DCreate9Ex_type)(UINT SDKVersion, IDirect3D9Ex **ppD3D);

#ifndef NO_INJECTION
Direct3DCreate9_type Direct3DCreate9_fn = NULL; // we need to store the pointer to the original Direct3DCreate9 function after we have done a detour
Direct3DCreate9Ex_type Direct3DCreate9Ex_fn = NULL; // we need to store the pointer to the original Direct3DCreate9 function after we have done a detour
#endif




/*
 * global variable which are linked external
 */

void InitDX9(void)
{
  LoadOriginal_DX9_Dll();

#ifndef NO_INJECTION
  // we detour the original Direct3DCreate9 to our MyDirect3DCreate9
  if (gl_hOriginal_DX9_Dll!=NULL)
  {
    Direct3DCreate9_fn = (Direct3DCreate9_type) GetProcAddress(gl_hOriginal_DX9_Dll, "Direct3DCreate9");
    if (Direct3DCreate9_fn!=NULL)
    {
      Message("Detour: Direct3DCreate9\n");
      Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc( (BYTE*)Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9, 5);
    }

    Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type) GetProcAddress(gl_hOriginal_DX9_Dll, "Direct3DCreate9Ex");
    if (Direct3DCreate9Ex_fn!=NULL)
    {
      Message("Detour: Direct3DCreate9Ex\n");
      Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)DetourFunc( (BYTE*)Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex, 7);
    }
  }
#endif
}

void ExitDX9(void)
{
  // Release the system's d3d9.dll
  if (gl_hOriginal_DX9_Dll!=NULL)
  {
    FreeLibrary(gl_hOriginal_DX9_Dll);
    gl_hOriginal_DX9_Dll = NULL;
  }
}
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


#ifdef NO_INJECTION
/*
 * We do not inject, the game loads this dll by itself thus we must include the Direct3DCreate9 function
 */

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


#else

/*
 * We inject the dll into the game, thus we retour the original Direct3DCreate9 function to our MyDirect3DCreate9 function
 */

IDirect3D9 *APIENTRY uMod_Direct3DCreate9(UINT SDKVersion)
{
  Message("uMod_Direct3DCreate9:  original %lu, uMod %lu\n", Direct3DCreate9_fn, uMod_Direct3DCreate9);

  // in the Internet are many tutorials for detouring functions and all of them will work without the following 5 marked lines
  // but somehow, for me it only works, if I retour the function and calling afterward the original function

  // BEGIN

  LoadOriginal_DX9_Dll();

  RetourFunc((BYTE*) GetProcAddress( gl_hOriginal_DX9_Dll, "Direct3DCreate9"), (BYTE*)Direct3DCreate9_fn, 5);
  Direct3DCreate9_fn = (Direct3DCreate9_type) GetProcAddress( gl_hOriginal_DX9_Dll, "Direct3DCreate9");

/*
  if (Direct3DCreate9Ex_fn!=NULL)
  {
    RetourFunc((BYTE*) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9Ex"), (BYTE*)Direct3DCreate9Ex_fn, 7);
    Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9Ex");
  }
  */
  // END

  IDirect3D9 *pIDirect3D9_orig = NULL;
  if (Direct3DCreate9_fn)
  {
    pIDirect3D9_orig = Direct3DCreate9_fn(SDKVersion); //creating the original IDirect3D9 object
  }
  else return (NULL);
  uMod_IDirect3D9 *pIDirect3D9;
  if (pIDirect3D9_orig)
  {
    pIDirect3D9 = new uMod_IDirect3D9( pIDirect3D9_orig, gl_TextureServer); //creating our uMod_IDirect3D9 object
  }

  // we detour again
  Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc( (BYTE*) Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9,5);
  /*
  if (Direct3DCreate9Ex_fn!=NULL)
  {
    Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)DetourFunc( (BYTE*) Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex,7);
  }
*/
  return (pIDirect3D9); //return our object instead of the "real one"
}

HRESULT APIENTRY uMod_Direct3DCreate9Ex( UINT SDKVersion, IDirect3D9Ex **ppD3D)
{
  Message( "uMod_Direct3DCreate9Ex:  original %lu, uMod %lu\n", Direct3DCreate9Ex_fn, uMod_Direct3DCreate9Ex);

  // in the Internet are many tutorials for detouring functions and all of them will work without the following 5 marked lines
  // but somehow, for me it only works, if I retour the function and calling afterward the original function

  // BEGIN

  LoadOriginal_DX9_Dll();
  /*
  if (Direct3DCreate9_fn!=NULL)
  {
    RetourFunc((BYTE*) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9"), (BYTE*)Direct3DCreate9_fn, 5);
    Direct3DCreate9_fn = (Direct3DCreate9_type) GetProcAddress( gl_hOriginalDll, "Direct3DCreate9");
  }
*/
  RetourFunc((BYTE*) GetProcAddress( gl_hOriginal_DX9_Dll, "Direct3DCreate9Ex"), (BYTE*)Direct3DCreate9Ex_fn, 7);
  Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type) GetProcAddress( gl_hOriginal_DX9_Dll, "Direct3DCreate9Ex");
  // END

  IDirect3D9Ex *pIDirect3D9Ex_orig = NULL;
  HRESULT ret;
  if (Direct3DCreate9Ex_fn)
  {
    ret = Direct3DCreate9Ex_fn(SDKVersion, &pIDirect3D9Ex_orig); //creating the original IDirect3D9 object
  }
  else return (D3DERR_NOTAVAILABLE);

  if (ret!=S_OK) return (ret);

  uMod_IDirect3D9Ex *pIDirect3D9Ex;
  if (pIDirect3D9Ex_orig)
  {
    pIDirect3D9Ex = new uMod_IDirect3D9Ex( pIDirect3D9Ex_orig, gl_TextureServer); //creating our uMod_IDirect3D9 object
  }

  // we detour again
/*
  if (Direct3DCreate9_fn!=NULL)
  {
    Direct3DCreate9_fn = (Direct3DCreate9_type)DetourFunc( (BYTE*) Direct3DCreate9_fn, (BYTE*)uMod_Direct3DCreate9,5);
  }
  */
  Direct3DCreate9Ex_fn = (Direct3DCreate9Ex_type)DetourFunc( (BYTE*) Direct3DCreate9Ex_fn, (BYTE*)uMod_Direct3DCreate9Ex,7);
  ppD3D = (IDirect3D9Ex**) &pIDirect3D9Ex; //return our object instead of the "real one"
  return (ret);
}


#endif
