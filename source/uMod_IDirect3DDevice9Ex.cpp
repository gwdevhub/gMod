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



#include "uMod_Main.h"

#define uMod_IDirect3DDevice9 uMod_IDirect3DDevice9Ex
#define IDirect3DDevice9 IDirect3DDevice9Ex
#define m_pIDirect3DDevice9 m_pIDirect3DDevice9Ex

#define RETURN_QueryInterface 0x01000001L
#define PRE_MESSAGE "uMod_IDirect3DDevice9Ex"


#include "uMod_IDirect3DDevice9.cpp"



HRESULT __stdcall uMod_IDirect3DDevice9Ex::CheckDeviceState( HWND hWindow)
{
  return(m_pIDirect3DDevice9Ex->CheckDeviceState( hWindow));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::CheckResourceResidency( IDirect3DResource9 **ppResourceArray, UINT32 NumResources)
{
  return(m_pIDirect3DDevice9Ex->CheckResourceResidency( ppResourceArray, NumResources));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::ComposeRects( IDirect3DSurface9 *pSource, IDirect3DSurface9 *pDestination, IDirect3DVertexBuffer9 *pSrcRectDescriptors, UINT NumRects, IDirect3DVertexBuffer9 *pDstRectDescriptors, D3DCOMPOSERECTSOP Operation, INT XOffset, INT YOffset)
{
  return(m_pIDirect3DDevice9Ex->ComposeRects( pSource, pDestination, pSrcRectDescriptors, NumRects, pDstRectDescriptors, Operation, XOffset, YOffset));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::CreateDepthStencilSurfaceEx( UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage)
{
  return(m_pIDirect3DDevice9Ex->CreateDepthStencilSurfaceEx( Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle, Usage));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::CreateOffscreenPlainSurfaceEx( UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage)
{
  return(m_pIDirect3DDevice9Ex->CreateOffscreenPlainSurfaceEx( Width, Height, Format, Pool, ppSurface, pSharedHandle, Usage));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::CreateRenderTargetEx( UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage)
{
  return(m_pIDirect3DDevice9Ex->CreateRenderTargetEx( Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle, Usage));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::GetDisplayModeEx( UINT  iSwapChain, D3DDISPLAYMODEEX *pMode, D3DDISPLAYROTATION *pRotation)
{
  return(m_pIDirect3DDevice9Ex->GetDisplayModeEx(  iSwapChain, pMode, pRotation));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::GetGPUThreadPriority( INT *pPriority)
{
  return(m_pIDirect3DDevice9Ex->GetGPUThreadPriority( pPriority));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::GetMaximumFrameLatency( UINT *pMaxLatency)
{
  return(m_pIDirect3DDevice9Ex->GetMaximumFrameLatency( pMaxLatency));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::PresentEx( const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion, DWORD dwFlags)
{
  return(m_pIDirect3DDevice9Ex->PresentEx( pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::ResetEx( D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode)
{
  return(m_pIDirect3DDevice9Ex->ResetEx( pPresentationParameters, pFullscreenDisplayMode));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::SetConvolutionMonoKernel( UINT Width, UINT Height, float *RowWeights, float *ColumnWeights)
{
  return(m_pIDirect3DDevice9Ex->SetConvolutionMonoKernel( Width, Height, RowWeights, ColumnWeights));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::SetGPUThreadPriority( INT pPriority)
{
  return(m_pIDirect3DDevice9Ex->SetGPUThreadPriority( pPriority));
}

HRESULT __stdcall uMod_IDirect3DDevice9Ex::SetMaximumFrameLatency( UINT pMaxLatency)
{
  return(m_pIDirect3DDevice9Ex->SetMaximumFrameLatency( pMaxLatency));
}

/*
HRESULT __stdcall uMod_IDirect3DDevice9Ex::TestCooperativeLevel()
{
  return(m_pIDirect3DDevice9Ex->TestCooperativeLevel());
}
*/

HRESULT __stdcall uMod_IDirect3DDevice9Ex::WaitForVBlank( UINT SwapChainIndex)
{
  return(m_pIDirect3DDevice9Ex->WaitForVBlank( SwapChainIndex));
}

