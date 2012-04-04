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

//#include "disasm-lib/cpu.c"
//#include "disasm-lib/disasm.c"
//#include "disasm-lib/disasm_x86.c"

//#include "disasm-lib/misc.c"
//#include "mhook-lib/mhook.cpp"
//#include "mhook-lib/mhook.h"

//#include "uMod_ID3D10Device.h"
//#include "uMod_ID3D10Device1.h"
//#include "uMod_IDXGISwapChain.h"

//#include "detours.h"
//#include "detourxs/detourxs/detourxs.h"

/*
#include "detourxs/detourxs/ADE32.cpp"
#include "detourxs/detourxs/detourxs.cpp"
*/
/*
 * global variable which are not linked external
 */
HINSTANCE             gl_hOriginal_DX10_Dll = NULL;
HINSTANCE             gl_hOriginal_DX101_Dll = NULL;

typedef HRESULT (APIENTRY *D3D10CreateDeviceAndSwapChain_type)( IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D10Device**);
typedef HRESULT (APIENTRY *D3D10CreateDeviceAndSwapChain1_type)( IDXGIAdapter*, D3D10_DRIVER_TYPE, HMODULE, UINT, D3D10_FEATURE_LEVEL1,  UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D10Device1**);

#ifndef NO_INJECTION
D3D10CreateDeviceAndSwapChain_type D3D10CreateDeviceAndSwapChain_fn = NULL;
D3D10CreateDeviceAndSwapChain1_type D3D10CreateDeviceAndSwapChain1_fn = NULL;
#endif




/*
 * global variable which are linked external
 */

void InitDX10(void)
{
  LoadOriginal_DX10_Dll();
/*
  HRESULT ret = E_OUTOFMEMORY;
  //IDXGIAdapter Adap;#

  D3D10CreateDeviceAndSwapChain_fn = (D3D10CreateDeviceAndSwapChain_type) GetProcAddress( gl_hOriginal_DX10_Dll, "D3D10CreateDeviceAndSwapChain");

  IDXGISwapChain *pIDXGISwapChain_orig = NULL;
  ID3D10Device *pID3D10Device_orig = NULL;
  if (D3D10CreateDeviceAndSwapChain_fn!=NULL)
  {
  Message("PRE: create original device\n");
  ret = D3D10CreateDeviceAndSwapChain_fn( NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_SINGLETHREADED,
      D3D10_SDK_VERSION, NULL, &pIDXGISwapChain_orig, &pID3D10Device_orig);
  if (ret == S_OK )
  {
    Message("PRE: created\n");
    pIDXGISwapChain_orig->Release();
    pID3D10Device_orig->Release();
    Message("PRE: released\n");
  }
  else
  {
    Message("PRE: failed\n");
  }
  }
  else
  {
    Message("PRE: no function\n");
  }
*/


#ifndef NO_INJECTION
  // we detour the original D3D10CreateDeviceAndSwapChain to our uMod_D3D10CreateDeviceAndSwapChain
  if (gl_hOriginal_DX10_Dll!=NULL)
  {
    D3D10CreateDeviceAndSwapChain_fn = (D3D10CreateDeviceAndSwapChain_type) GetProcAddress(gl_hOriginal_DX10_Dll, "D3D10CreateDeviceAndSwapChain");
    if (D3D10CreateDeviceAndSwapChain_fn!=NULL)
    {
      Message("Detour: D3D10CreateDeviceAndSwapChain\n");
      //D3D10CreateDeviceAndSwapChain_fn = (D3D10CreateDeviceAndSwapChain_type)DetourFunc( (BYTE*)D3D10CreateDeviceAndSwapChain_fn, (BYTE*)uMod_D3D10CreateDeviceAndSwapChain, number_of_byte);


      //Mhook_SetHook((PVOID*)&D3D10CreateDeviceAndSwapChain_fn, uMod_D3D10CreateDeviceAndSwapChain);
      D3D10CreateDeviceAndSwapChain_fn = (D3D10CreateDeviceAndSwapChain_type) DetourFunction( (BYTE*)D3D10CreateDeviceAndSwapChain_fn, (BYTE*)uMod_D3D10CreateDeviceAndSwapChain);

      //DetourTransactionBegin();
      //Messagebox_orig = (Messagebox_typ)DetourAttach(&(PVOID&)MessageBoxA, (PVOID)MyMessageBox);
      //D3D10CreateDeviceAndSwapChain_fn = (D3D10CreateDeviceAndSwapChain_type) DetourAttach( (BYTE*)D3D10CreateDeviceAndSwapChain_fn, (BYTE*)uMod_D3D10CreateDeviceAndSwapChain);
      //DetourTransactionCommit();
    }
  }
  if (gl_hOriginal_DX101_Dll!=NULL)
  {
    //D3D10CreateDeviceAndSwapChain1_fn = (D3D10CreateDeviceAndSwapChain1_type) GetProcAddress(gl_hOriginal_DX101_Dll, "D3D10CreateDeviceAndSwapChain1");
    if (D3D10CreateDeviceAndSwapChain1_fn!=NULL)
    {
      Message("Detour: D3D10CreateDeviceAndSwapChain1\n");
      D3D10CreateDeviceAndSwapChain1_fn = (D3D10CreateDeviceAndSwapChain1_type)DetourFunc( (BYTE*)D3D10CreateDeviceAndSwapChain1_fn, (BYTE*)uMod_D3D10CreateDeviceAndSwapChain1, 5);
    }
  }
#endif
}

void ExitDX10(void)
{
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
}
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


#ifdef NO_INJECTION
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
#else

/*
 * We inject the dll into the game, thus we retour the original Direct3DCreate9 function to our MyDirect3DCreate9 function
 */

HRESULT uMod_D3D10CreateDeviceAndSwapChain(
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
  Message("uMod_D3D10CreateDeviceAndSwapChain: bla original %lu, uMod %lu\n", D3D10CreateDeviceAndSwapChain_fn, uMod_D3D10CreateDeviceAndSwapChain);

  // in the Internet are many tutorials for detouring functions and all of them will work without the following 5 marked lines
  // but somehow, for me it only works, if I retour the function and calling afterward the original function

  // BEGIN

  //LoadOriginal_DX10_Dll();

  //RetourFunc((BYTE*) GetProcAddress( gl_hOriginal_DX9_Dll, "Direct3DCreate9"), (BYTE*)Direct3DCreate9_fn, 5);
  //Direct3DCreate9_fn = (Direct3DCreate9_type) GetProcAddress( gl_hOriginal_DX9_Dll, "Direct3DCreate9");

  RetourFunc((BYTE*) GetProcAddress( gl_hOriginal_DX10_Dll, "D3D10CreateDeviceAndSwapChain"), (BYTE*)D3D10CreateDeviceAndSwapChain_fn, number_of_byte);
  D3D10CreateDeviceAndSwapChain_fn = (D3D10CreateDeviceAndSwapChain_type) GetProcAddress( gl_hOriginal_DX10_Dll, "D3D10CreateDeviceAndSwapChain");
  // END

  //DetourRemove( (BYTE*) GetProcAddress( gl_hOriginal_DX10_Dll, "D3D10CreateDeviceAndSwapChain"), (BYTE*)D3D10CreateDeviceAndSwapChain_fn);
  //D3D10CreateDeviceAndSwapChain_fn = (D3D10CreateDeviceAndSwapChain_type) GetProcAddress( gl_hOriginal_DX10_Dll, "D3D10CreateDeviceAndSwapChain");

  HRESULT ret = E_OUTOFMEMORY;
  IDXGISwapChain *pIDXGISwapChain_orig = NULL;
  ID3D10Device *pID3D10Device_orig = NULL;
  if (D3D10CreateDeviceAndSwapChain_fn)
  {
    Message("create original device\n");
    ret = D3D10CreateDeviceAndSwapChain_fn(pAdapter,DriverType,Software,Flags,SDKVersion,pSwapChainDesc, &pIDXGISwapChain_orig, &pID3D10Device_orig);
  }
  /*
  uMod_IDirect3D9 *pIDirect3D9;
  if (pIDirect3D9_orig)
  {
    pIDirect3D9 = new uMod_IDirect3D9( pIDirect3D9_orig, gl_TextureServer); //creating our uMod_IDirect3D9 object
  }
  */

  // we detour again
  Message("detour again\n");
  //D3D10CreateDeviceAndSwapChain_fn = (D3D10CreateDeviceAndSwapChain_type)DetourFunc( (BYTE*)D3D10CreateDeviceAndSwapChain_fn, (BYTE*)uMod_D3D10CreateDeviceAndSwapChain, number_of_byte);


  //*ppSwapChain = pIDXGISwapChain_orig;
  //*ppDevice = pID3D10Device_orig;
  Message("Fine\n");
  return (ret);
}

HRESULT uMod_D3D10CreateDeviceAndSwapChain1(
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
  Message("uMod_D3D10CreateDeviceAndSwapChain1:  original %lu, uMod %lu\n", D3D10CreateDeviceAndSwapChain1_fn, uMod_D3D10CreateDeviceAndSwapChain1);

  // in the Internet are many tutorials for detouring functions and all of them will work without the following 5 marked lines
  // but somehow, for me it only works, if I retour the function and calling afterward the original function

  // BEGIN

  //LoadOriginal_DX10_Dll();

  RetourFunc((BYTE*) GetProcAddress( gl_hOriginal_DX101_Dll, "D3D10CreateDeviceAndSwapChain1"), (BYTE*)D3D10CreateDeviceAndSwapChain1_fn, 5);
  D3D10CreateDeviceAndSwapChain1_fn = (D3D10CreateDeviceAndSwapChain1_type) GetProcAddress( gl_hOriginal_DX101_Dll, "D3D10CreateDeviceAndSwapChain1");
  // END

  HRESULT ret = E_OUTOFMEMORY;
  IDXGISwapChain *pIDXGISwapChain_orig = NULL;
  ID3D10Device1 *pID3D10Device1_orig = NULL;
  if (D3D10CreateDeviceAndSwapChain1_fn)
  {
    ret = D3D10CreateDeviceAndSwapChain1_fn(pAdapter,DriverType,Software,Flags,HardwareLevel,SDKVersion,pSwapChainDesc, &pIDXGISwapChain_orig, &pID3D10Device1_orig);
  }
  /*
  uMod_IDirect3D9 *pIDirect3D9;
  if (pIDirect3D9_orig)
  {
    pIDirect3D9 = new uMod_IDirect3D9( pIDirect3D9_orig, gl_TextureServer); //creating our uMod_IDirect3D9 object
  }
  */

  // we detour again
  D3D10CreateDeviceAndSwapChain1_fn = (D3D10CreateDeviceAndSwapChain1_type)DetourFunc( (BYTE*)D3D10CreateDeviceAndSwapChain1_fn, (BYTE*)uMod_D3D10CreateDeviceAndSwapChain1, 5);
  //D3D10CreateDeviceAndSwapChain1_fn = (D3D10CreateDeviceAndSwapChain1_type)DetourFunc( (BYTE*)D3D10CreateDeviceAndSwapChain1_fn, (BYTE*)uMod_D3D10CreateDeviceAndSwapChain1, 5);


  *ppSwapChain = pIDXGISwapChain_orig;
  *ppDevice = pID3D10Device1_orig;
  return (ret);
}

#endif
