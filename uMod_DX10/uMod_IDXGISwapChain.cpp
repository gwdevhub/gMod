/*
This file is part of Universal Modding Engine.


Universal Modding Engine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Universal Modding Engine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Universal Modding Engine.If not, see <http://www.gnu.org/licenses/>.
*/

#include "../uMod_DXMain/uMod_Main.h"
#include "uMod_IDXGISwapChain.h"
#include "uMod_ID3D10Device.h"


uMod_IDXGISwapChain::uMod_IDXGISwapChain(IDXGISwapChain *pOriginal, uMod_ID3D10Device *dev)
{
  Message( "uMod_IDXGISwapChain::uMod_IDXGISwapChain( %lu, %lu): %lu\n", pOriginal, dev, this);
  m_IDXGISwapChain = pOriginal;
  u_ID3D10Device = dev;
  RefCounter = 1;
}

uMod_IDXGISwapChain::~uMod_IDXGISwapChain()
{
  Message( "IDXGISwapChain::~IDXGISwapChain(): %lu\n", this);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::QueryInterface( REFIID riid, void** ppvObject)
{
  *ppvObject = NULL;
  Message( "uMod_IDXGISwapChain::QueryInterface(): %lu\n", this);
  HRESULT hRes = m_IDXGISwapChain->QueryInterface(riid, ppvObject);

  if (*ppvObject == m_IDXGISwapChain)
  {
    RefCounter++; //increasing our counter
    *ppvObject = this;
  }

  return (hRes);
}

ULONG STDMETHODCALLTYPE uMod_IDXGISwapChain::AddRef(void)
{
  Message( "uMod_IDXGISwapChain::AddRef(): %lu\n", this);
  RefCounter++;
  return m_IDXGISwapChain->AddRef();
}

ULONG STDMETHODCALLTYPE uMod_IDXGISwapChain::Release( void)
{
  Message( "uMod_IDXGISwapChain::Release(): %lu\n", this);
  if (--RefCounter==0) //if our counter drops to zero, the real device will be deleted, so we clean up before
   {
     // we must not release the fake textures, cause they are released if the target textures are released
     // and the target textures are released by the game.
   }

  // call original routine
  ULONG count = m_IDXGISwapChain->Release();

  // in case no further Ref is there, the Original Object has deleted itself
  if (count == 0)
  {
    delete(this);
  }

  return(count);
}



HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::SetPrivateData(REFGUID Name, UINT DataSize, const void *pData)
{
  return m_IDXGISwapChain->SetPrivateData( Name, DataSize, pData);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::SetPrivateDataInterface( REFGUID Name, const IUnknown *pUnknown)
{
  return m_IDXGISwapChain->SetPrivateDataInterface( Name, pUnknown);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::GetPrivateData( REFGUID Name, UINT *pDataSize, void *pData)
{
  return m_IDXGISwapChain->GetPrivateData( Name, pDataSize, pData);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::GetParent( REFIID riid, void **ppParent)
{
  return m_IDXGISwapChain->GetParent( riid, ppParent);
}



HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::GetDevice( REFIID riid, void **ppDevice)
{
  // ToDO:
  // Increas RefCounter from u_ID3D10Device but not from u_ID3D10Device->m_ID3D10Device ? ? ? ? ?
  HRESULT hRes = m_IDXGISwapChain->GetDevice( riid, ppDevice);
  if (*ppDevice == u_ID3D10Device->m_ID3D10Device) *ppDevice = u_ID3D10Device;
  return hRes;
}


HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::Present( UINT SyncInterval, UINT Flags)
{
  Message( "uMod_IDXGISwapChain::Present(): %lu\n", this);
  return m_IDXGISwapChain->Present( SyncInterval, Flags);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::GetBuffer( UINT Buffer, REFIID riid, void **ppSurface)
{
  Message( "uMod_IDXGISwapChain::GetBuffer(): %lu\n", this);
  return m_IDXGISwapChain->GetBuffer( Buffer, riid, ppSurface);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::SetFullscreenState( BOOL Fullscreen, IDXGIOutput *pTarget)
{
  return m_IDXGISwapChain->SetFullscreenState( Fullscreen, pTarget);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::GetFullscreenState( BOOL *pFullscreen, IDXGIOutput **ppTarget)
{
  return m_IDXGISwapChain->GetFullscreenState( pFullscreen, ppTarget);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::GetDesc( DXGI_SWAP_CHAIN_DESC *pDesc)
{
  return m_IDXGISwapChain->GetDesc( pDesc);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::ResizeBuffers( UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
  return m_IDXGISwapChain->ResizeBuffers( BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::ResizeTarget( const DXGI_MODE_DESC *pNewTargetParameters)
{
  return m_IDXGISwapChain->ResizeTarget( pNewTargetParameters);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::GetContainingOutput( IDXGIOutput **ppOutput)
{
  return m_IDXGISwapChain->GetContainingOutput( ppOutput);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::GetFrameStatistics( DXGI_FRAME_STATISTICS *pStats)
{
  return m_IDXGISwapChain->GetFrameStatistics( pStats);
}

HRESULT STDMETHODCALLTYPE uMod_IDXGISwapChain::GetLastPresentCount( UINT *pLastPresentCount)
{
  return m_IDXGISwapChain->GetLastPresentCount( pLastPresentCount);
}

