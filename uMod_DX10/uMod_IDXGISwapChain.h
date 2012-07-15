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


#ifndef UMOD_IDXGISWAPCHAIN_H_
#define UMOD_IDXGISWAPCHAIN_H_

#include <d3d10_1.h>
#include <d3d10.h>
#include <D3D10Misc.h>
#include <d3dx10.h>

class uMod_ID3D10Device;

class uMod_IDXGISwapChain : public IDXGISwapChain
{
public:
  uMod_IDXGISwapChain(IDXGISwapChain *pOriginal, uMod_ID3D10Device *dev);
  virtual ~uMod_IDXGISwapChain();

  HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid,  void** ppvObject);
  ULONG STDMETHODCALLTYPE AddRef(void);
  ULONG STDMETHODCALLTYPE Release( void);


  HRESULT STDMETHODCALLTYPE SetPrivateData( REFGUID Name, UINT DataSize, const void *pData);
  HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( REFGUID Name, const IUnknown *pUnknown);
  HRESULT STDMETHODCALLTYPE GetPrivateData( REFGUID Name, UINT *pDataSize, void *pData);
  HRESULT STDMETHODCALLTYPE GetParent( REFIID riid, void **ppParent);


  HRESULT STDMETHODCALLTYPE GetDevice( REFIID riid, void **ppDevice);



  HRESULT STDMETHODCALLTYPE Present( UINT SyncInterval, UINT Flags);

  HRESULT STDMETHODCALLTYPE GetBuffer( UINT Buffer, REFIID riid, void **ppSurface);

  HRESULT STDMETHODCALLTYPE SetFullscreenState( BOOL Fullscreen, IDXGIOutput *pTarget);

  HRESULT STDMETHODCALLTYPE GetFullscreenState( BOOL *pFullscreen, IDXGIOutput **ppTarget);

  HRESULT STDMETHODCALLTYPE GetDesc( DXGI_SWAP_CHAIN_DESC *pDesc);

  HRESULT STDMETHODCALLTYPE ResizeBuffers( UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

  HRESULT STDMETHODCALLTYPE ResizeTarget( const DXGI_MODE_DESC *pNewTargetParameters);

  HRESULT STDMETHODCALLTYPE GetContainingOutput( IDXGIOutput **ppOutput);

  HRESULT STDMETHODCALLTYPE GetFrameStatistics( DXGI_FRAME_STATISTICS *pStats);

  HRESULT STDMETHODCALLTYPE GetLastPresentCount( UINT *pLastPresentCount);


  IDXGISwapChain *m_IDXGISwapChain;
  uMod_ID3D10Device *u_ID3D10Device;

private:
  unsigned int RefCounter;
};


#endif /* UMOD_IDXGISWAPCHAIN_H_ */
