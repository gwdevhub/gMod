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


#ifndef uMod_IDirect3DDevice9Ex_H_
#define uMod_IDirect3DDevice9Ex_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "uMod_IDirect3DTexture9.h"
#include "uMod_IDirect3DVolumeTexture9.h"
#include "uMod_IDirect3DCubeTexture9.h"


class uMod_IDirect3DDevice9Ex : public IDirect3DDevice9Ex
{
public:
	uMod_IDirect3DDevice9Ex( IDirect3DDevice9Ex* pOriginal, uMod_TextureServer* server, int back_buffer_count);
	virtual ~uMod_IDirect3DDevice9Ex(void);

	// START: The original DX9 function definitions
	HRESULT __stdcall QueryInterface (REFIID riid, void** ppvObj);
	ULONG   __stdcall AddRef(void);
	ULONG   __stdcall Release(void);
	HRESULT __stdcall TestCooperativeLevel(void);
	UINT	__stdcall GetAvailableTextureMem(void);
	HRESULT __stdcall EvictManagedResources(void);
	HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9);
	HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps);
	HRESULT __stdcall GetDisplayMode(UINT iSwapChain,D3DDISPLAYMODE* pMode);
	HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters);
	HRESULT __stdcall SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap);
	void	__stdcall SetCursorPosition(int X,int Y,DWORD Flags);
	BOOL	__stdcall ShowCursor(BOOL bShow);
	HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain)  ;
	HRESULT __stdcall GetSwapChain(UINT iSwapChain,IDirect3DSwapChain9** pSwapChain);
	UINT	__stdcall GetNumberOfSwapChains(void);
	HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);
	HRESULT __stdcall Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
	HRESULT __stdcall GetBackBuffer(UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer);
	HRESULT __stdcall GetRasterStatus(UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus);
	HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs);
	void	__stdcall SetGammaRamp(UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp);
	void	__stdcall GetGammaRamp(UINT iSwapChain,D3DGAMMARAMP* pRamp);
	HRESULT __stdcall CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle);
	HRESULT __stdcall CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle);
	HRESULT __stdcall CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle);
	HRESULT __stdcall CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle);
	HRESULT __stdcall CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle);
	HRESULT __stdcall CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	HRESULT __stdcall CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint);
	HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture);
	HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface);
	HRESULT __stdcall GetFrontBufferData(UINT iSwapChain,IDirect3DSurface9* pDestSurface);
	HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter);
	HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color);
	HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle);
	HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget);
	HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget);
	HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil);
	HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface);
	HRESULT __stdcall BeginScene(void);
	HRESULT __stdcall EndScene(void);
	HRESULT __stdcall Clear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil);
	HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
	HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix);
	HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);
	HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport);
	HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport);
	HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial);
	HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial);
	HRESULT __stdcall SetLight(DWORD Index,CONST D3DLIGHT9* pLight);
	HRESULT __stdcall GetLight(DWORD Index,D3DLIGHT9* pLight);
	HRESULT __stdcall LightEnable(DWORD Index,BOOL Enable);
	HRESULT __stdcall GetLightEnable(DWORD Index,BOOL* pEnable);
	HRESULT __stdcall SetClipPlane(DWORD Index,CONST float* pPlane);
	HRESULT __stdcall GetClipPlane(DWORD Index,float* pPlane);
	HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State,DWORD Value);
	HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State,DWORD* pValue);
	HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB);
	HRESULT __stdcall BeginStateBlock(void);
	HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB);
	HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus);
	HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus);
	HRESULT __stdcall GetTexture(DWORD Stage,IDirect3DBaseTexture9** ppTexture);
	HRESULT __stdcall SetTexture(DWORD Stage,IDirect3DBaseTexture9* pTexture);
	HRESULT __stdcall GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue);
	HRESULT __stdcall SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
	HRESULT __stdcall GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue);
	HRESULT __stdcall SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);
	HRESULT __stdcall ValidateDevice(DWORD* pNumPasses);
	HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber,CONST PALETTEENTRY* pEntries);
	HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries);
	HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber);
	HRESULT __stdcall GetCurrentTexturePalette(UINT *PaletteNumber);
	HRESULT __stdcall SetScissorRect(CONST RECT* pRect);
	HRESULT __stdcall GetScissorRect( RECT* pRect);
	HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware);
	BOOL	__stdcall GetSoftwareVertexProcessing(void);
	HRESULT __stdcall SetNPatchMode(float nSegments);
	float   __stdcall GetNPatchMode(void);
	HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
	HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount);
	HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
	HRESULT __stdcall ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags);
	HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl);
	HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl);
	HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl);
	HRESULT __stdcall SetFVF(DWORD FVF);
	HRESULT __stdcall GetFVF(DWORD* pFVF);
	HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader);
	HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader);
	HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader);
	HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount);
	HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
	HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount);
	HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
	HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
	HRESULT __stdcall SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride);
	HRESULT __stdcall GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* OffsetInBytes,UINT* pStride);
	HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber,UINT Divider);
	HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber,UINT* Divider);
	HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData);
	HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData);
	HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader);
	HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader);
	HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader);
	HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount);
	HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount);
	HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount);
	HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount);
	HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount);
	HRESULT __stdcall DrawRectPatch(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo);
	HRESULT __stdcall DrawTriPatch(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo);
	HRESULT __stdcall DeletePatch(UINT Handle);
	HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery);



	HRESULT __stdcall CheckDeviceState( HWND hWindow);
	HRESULT __stdcall CheckResourceResidency( IDirect3DResource9 **pResourceArray, UINT32 NumResources);
	HRESULT __stdcall ComposeRects( IDirect3DSurface9 *pSource, IDirect3DSurface9 *pDestination, IDirect3DVertexBuffer9 *pSrcRectDescriptors, UINT NumRects, IDirect3DVertexBuffer9 *pDstRectDescriptors, D3DCOMPOSERECTSOP Operation, INT XOffset, INT YOffset);
	HRESULT __stdcall CreateDepthStencilSurfaceEx( UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage);
	HRESULT __stdcall CreateOffscreenPlainSurfaceEx( UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage);
 	HRESULT __stdcall CreateRenderTargetEx( UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9 **ppSurface, HANDLE *pSharedHandle, DWORD Usage);
 	HRESULT __stdcall GetDisplayModeEx( UINT  iSwapChain, D3DDISPLAYMODEEX *pMode, D3DDISPLAYROTATION *pRotation);
 	HRESULT __stdcall GetGPUThreadPriority( INT *pPriority);
 	HRESULT __stdcall GetMaximumFrameLatency( UINT *pMaxLatency);
 	HRESULT __stdcall PresentEx( const RECT *pSourceRect, const RECT *pDestRect, HWND hDestWindowOverride, const RGNDATA *pDirtyRegion, DWORD dwFlags);
 	HRESULT __stdcall ResetEx( D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDISPLAYMODEEX *pFullscreenDisplayMode);
 	HRESULT __stdcall SetConvolutionMonoKernel( UINT Width, UINT Height, float *RowWeights, float *ColumnWeights);
 	HRESULT __stdcall SetGPUThreadPriority( INT pPriority);
 	HRESULT __stdcall SetMaximumFrameLatency( UINT pMaxLatency);
 	//HRESULT __stdcall TestCooperativeLevel();
 	HRESULT __stdcall WaitForVBlank( UINT SwapChainIndex);


	// END: The original DX9 function definitions


  uMod_TextureClient* GetuMod_Client(void) {return (uMod_Client);}

  uMod_IDirect3DTexture9* GetLastCreatedTexture(void) {return (LastCreatedTexture);}
  int SetLastCreatedTexture(uMod_IDirect3DTexture9* pTexture) {LastCreatedTexture=pTexture; return (RETURN_OK);}

  uMod_IDirect3DVolumeTexture9* GetLastCreatedVolumeTexture(void) {return (LastCreatedVolumeTexture);}
  int SetLastCreatedVolumeTexture(uMod_IDirect3DVolumeTexture9* pTexture) {LastCreatedVolumeTexture=pTexture; return (RETURN_OK);}

  uMod_IDirect3DCubeTexture9* GetLastCreatedCubeTexture(void) {return (LastCreatedCubeTexture);}
  int SetLastCreatedCubeTexture(uMod_IDirect3DCubeTexture9* pTexture) {LastCreatedCubeTexture=pTexture; return (RETURN_OK);}


  uMod_IDirect3DTexture9* GetSingleTexture(void) {return (SingleTexture);}
  uMod_IDirect3DVolumeTexture9* GetSingleVolumeTexture(void) {return (SingleVolumeTexture);}
  uMod_IDirect3DCubeTexture9* GetSingleCubeTexture(void) {return (SingleCubeTexture);}



 private:
  int CreateSingleTexture(void);
  IDirect3DDevice9Ex* m_pIDirect3DDevice9Ex;

  int CounterSaveSingleTexture;
  uMod_IDirect3DTexture9* SingleTexture;
  uMod_IDirect3DVolumeTexture9* SingleVolumeTexture;
  uMod_IDirect3DCubeTexture9* SingleCubeTexture;
  char SingleTextureMod;

  D3DCOLOR TextureColour;
  ID3DXFont *OSD_Font;
  //D3DCOLOR FontColour;
  int BackBufferCount;
  bool NormalRendering;

  int uMod_Reference;

  uMod_IDirect3DTexture9* LastCreatedTexture;
  uMod_IDirect3DVolumeTexture9* LastCreatedVolumeTexture;
  uMod_IDirect3DCubeTexture9* LastCreatedCubeTexture;

  uMod_TextureServer* uMod_Server;
  uMod_TextureClient* uMod_Client;
};

#endif

