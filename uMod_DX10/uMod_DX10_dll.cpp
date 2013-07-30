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

#define number_of_byte 5

#include "uMod_DX10_dll.h"
#include "uMod_ID3D10Device.h"
#include "uMod_ID3D10Device1.h"
#include "uMod_IDXGISwapChain.h"

/*
 * global variable which are not linked external
 */
#if INJECTION_METHOD==NO_INJECTION
HINSTANCE             gl_hOriginal_DX10_Dll = NULL;
HINSTANCE             gl_hOriginal_DX101_Dll = NULL;
#endif

#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION
typedef HRESULT (APIENTRY *D3D10CreateDeviceAndSwapChain_type)( IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D10Device**);
typedef HRESULT (APIENTRY *D3D10CreateDeviceAndSwapChain1_type)( IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, D3D10_FEATURE_LEVEL1,  UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D10Device1**);

uMod_Detour_Entry<D3D10CreateDeviceAndSwapChain_type> Detour_D3D10CreateDeviceAndSwapChain(5);
uMod_Detour_Entry<D3D10CreateDeviceAndSwapChain1_type> Detour_D3D10CreateDeviceAndSwapChain1(5);
#endif




/*
 * global variable which are linked external
 */

void InitDX10(void)
{

#if INJECTION_METHOD==NO_INJECTION
  LoadOriginal_DX10_Dll();
#endif

#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION

  char buffer[MAX_PATH];
  wchar_t buffer_w[MAX_PATH];
  GetSystemDirectory(buffer,MAX_PATH); //get the system directory, we need to open the original d3d9.dll
  swprintf_s( buffer_w, MAX_PATH, L"%s\\d3d10.dll", buffer);
  strcat_s( buffer, MAX_PATH,"\\d3d10.dll");

  Detour_D3D10CreateDeviceAndSwapChain.SetFunctionName( "D3D10CreateDeviceAndSwapChain");
  Detour_D3D10CreateDeviceAndSwapChain.SetTargetFunction( uMod_D3D10CreateDeviceAndSwapChain);
  Detour_D3D10CreateDeviceAndSwapChain.SetLibName( "d3d10.dll");
  Detour_D3D10CreateDeviceAndSwapChain.SetFullLibName( buffer);
  Detour_D3D10CreateDeviceAndSwapChain.SetLibName( L"d3d10.dll"); // set also for wide character
  Detour_D3D10CreateDeviceAndSwapChain.SetFullLibName( buffer_w); // set also for wide character

  GetSystemDirectory(buffer,MAX_PATH); //get the system directory, we need to open the original d3d9.dll
  swprintf_s( buffer_w, MAX_PATH, L"%s\\d3d10_1.dll", buffer);
  strcat_s( buffer, MAX_PATH,"\\d3d10_1.dll");

  Detour_D3D10CreateDeviceAndSwapChain1.SetFunctionName( "D3D10CreateDeviceAndSwapChain1");
  Detour_D3D10CreateDeviceAndSwapChain1.SetTargetFunction( uMod_D3D10CreateDeviceAndSwapChain1);
  Detour_D3D10CreateDeviceAndSwapChain1.SetLibName( "d3d10_1.dll");
  Detour_D3D10CreateDeviceAndSwapChain1.SetFullLibName( buffer);
  Detour_D3D10CreateDeviceAndSwapChain1.SetLibName( L"d3d10_1.dll"); // set also for wide character
  Detour_D3D10CreateDeviceAndSwapChain1.SetFullLibName( buffer_w); // set also for wide character

  GlobalDetour.AddEntry(&Detour_D3D10CreateDeviceAndSwapChain);
  GlobalDetour.AddEntry(&Detour_D3D10CreateDeviceAndSwapChain1);

#endif
}

void ExitDX10(void)
{
#if INJECTION_METHOD==NO_INJECTION
  // Release the system's d3d9.dll
  if (gl_hOriginal_DX10_Dll!=NULL)
  {
    FreeLibrary(gl_hOriginal_DX10_Dll);
    gl_hOriginal_DX10_Dll = NULL;
  }

  if (gl_hOriginal_DX101_Dll!=NULL)
  {
    FreeLibrary(gl_hOriginal_DX101_Dll);
    gl_hOriginal_DX101_Dll = NULL;
  }
#endif

#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION
#endif
}



#if INJECTION_METHOD==NO_INJECTION
void LoadOriginal_DX10_Dll(void)
{
  char buffer[MAX_PATH];

  if (gl_hOriginal_DX10_Dll==NULL)
  {
    GetSystemDirectory(buffer,MAX_PATH); //get the system directory, we need to open the original d3d10.dll

    // Append dll name
    strcat_s( buffer, MAX_PATH,"\\d3d10.dll");

    // try to load the system's d3d10.dll
    gl_hOriginal_DX10_Dll = LoadLibrary(buffer);
  }

  if (gl_hOriginal_DX101_Dll==NULL)
  {
    GetSystemDirectory(buffer,MAX_PATH);
    strcat_s( buffer, MAX_PATH,"\\d3d10_1.dll");
    gl_hOriginal_DX101_Dll = LoadLibrary(buffer);
  }

}

/*
 * We do not inject, the game loads this dll by itself thus we must include the Direct3DCreate9 function
 */
/*
IDirect3D9* WINAPI  Direct3DCreate9(UINT SDKVersion)
{
  Message("WINAPI  Direct3DCreate9\n");

	if (!gl_hOriginal_DX9_Dll) LoadOriginal_DX9_Dll(); // looking for the "right d3d9.dll"
	
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

  if (!gl_hOriginal_DX9_Dll) LoadOriginal_DX9_Dll(); // looking for the "right d3d9.dll"

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

*/
#endif


#if INJECTION_METHOD==DIRECT_INJECTION || INJECTION_METHOD==HOOK_INJECTION

/*
 * We inject the dll into the game, thus we retour the original Direct3DCreate9 function to our MyDirect3DCreate9 function
 */

HRESULT APIENTRY uMod_D3D10CreateDeviceAndSwapChain(
  IDXGIAdapter *pAdapter,
  D3D10_DRIVER_TYPE DriverType,
  HMODULE Software,
  UINT Flags,
  UINT SDKVersion,
  DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
  IDXGISwapChain **ppSwapChain,
  ID3D10Device **ppDevice
)
{
  Detour_D3D10CreateDeviceAndSwapChain.Retour();
  HRESULT ret = Detour_D3D10CreateDeviceAndSwapChain.Function()(pAdapter,DriverType,Software,Flags,SDKVersion,pSwapChainDesc, ppSwapChain, ppDevice);
  if (ret==S_OK)
  {
    uMod_ID3D10Device *dev = new uMod_ID3D10Device( *ppDevice, gl_TextureServer);
    *ppDevice = (ID3D10Device*) dev;

    if (ppSwapChain!=NULL)
    {
      uMod_IDXGISwapChain *swap = new uMod_IDXGISwapChain( *ppSwapChain, dev);
      *ppSwapChain = swap;
    }
  }
  else Message("D3D10CreateDeviceAndSwapChain: Failed\n");
  Detour_D3D10CreateDeviceAndSwapChain.Detour();
  return ret;
}

HRESULT APIENTRY uMod_D3D10CreateDeviceAndSwapChain1(
  IDXGIAdapter *pAdapter,
  D3D10_DRIVER_TYPE DriverType,
  HMODULE Software,
  UINT Flags,
  D3D10_FEATURE_LEVEL1 HardwareLevel,
  UINT SDKVersion,
  DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
  IDXGISwapChain **ppSwapChain,
  ID3D10Device1 **ppDevice
)
{
  Detour_D3D10CreateDeviceAndSwapChain1.Retour();
  HRESULT ret = Detour_D3D10CreateDeviceAndSwapChain1.Function()(pAdapter,DriverType,Software,Flags,HardwareLevel,SDKVersion,pSwapChainDesc, ppSwapChain, ppDevice);
  if (ret==S_OK)
  {
    uMod_ID3D10Device1 *dev = new uMod_ID3D10Device1( *ppDevice, gl_TextureServer);
    *ppDevice = (ID3D10Device1*) dev;

    if (ppSwapChain!=NULL)
    {
      uMod_IDXGISwapChain *swap = new uMod_IDXGISwapChain( *ppSwapChain, (uMod_ID3D10Device*)dev);
      *ppSwapChain = swap;
    }
  }
  else Message("D3D10CreateDeviceAndSwapChain1: Failed\n");
  Detour_D3D10CreateDeviceAndSwapChain1.Detour();
  return (ret);
}

#endif
