#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include "uMod_IDirect3DTexture9.h"
#include "uMod_IDirect3DVolumeTexture9.h"
#include "uMod_IDirect3DCubeTexture9.h"


class uMod_IDirect3DDevice9Ex : public IDirect3DDevice9Ex {
public:
    uMod_IDirect3DDevice9Ex(IDirect3DDevice9Ex* pOriginal, uMod_TextureServer* server, int back_buffer_count);
    virtual ~uMod_IDirect3DDevice9Ex();

    // START: The original DX9 function definitions
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObj) override;
    ULONG __stdcall AddRef() override;
    ULONG __stdcall Release() override;
    HRESULT __stdcall TestCooperativeLevel() override;
    UINT __stdcall GetAvailableTextureMem() override;
    HRESULT __stdcall EvictManagedResources() override;
    HRESULT __stdcall GetDirect3D(IDirect3D9** ppD3D9) override;
    HRESULT __stdcall GetDeviceCaps(D3DCAPS9* pCaps) override;
    HRESULT __stdcall GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) override;
    HRESULT __stdcall GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) override;
    HRESULT __stdcall SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) override;
    void __stdcall SetCursorPosition(int X, int Y, DWORD Flags) override;
    BOOL __stdcall ShowCursor(BOOL bShow) override;
    HRESULT __stdcall CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) override;
    HRESULT __stdcall GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) override;
    UINT __stdcall GetNumberOfSwapChains() override;
    HRESULT __stdcall Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) override;
    HRESULT __stdcall Present(CONST RECT* pSourceRect,CONST RECT* pDestRect, HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion) override;
    HRESULT __stdcall GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) override;
    HRESULT __stdcall GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) override;
    HRESULT __stdcall SetDialogBoxMode(BOOL bEnableDialogs) override;
    void __stdcall SetGammaRamp(UINT iSwapChain, DWORD Flags,CONST D3DGAMMARAMP* pRamp) override;
    void __stdcall GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) override;
    HRESULT __stdcall CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle) override;
    HRESULT __stdcall CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle) override;
    HRESULT __stdcall CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) override;
    HRESULT __stdcall CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) override;
    HRESULT __stdcall CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) override;
    HRESULT __stdcall CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;
    HRESULT __stdcall CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;
    HRESULT __stdcall UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint) override;
    HRESULT __stdcall UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) override;
    HRESULT __stdcall GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) override;
    HRESULT __stdcall GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) override;
    HRESULT __stdcall StretchRect(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) override;
    HRESULT __stdcall ColorFill(IDirect3DSurface9* pSurface,CONST RECT* pRect, D3DCOLOR color) override;
    HRESULT __stdcall CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) override;
    HRESULT __stdcall SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) override;
    HRESULT __stdcall GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) override;
    HRESULT __stdcall SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) override;
    HRESULT __stdcall GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) override;
    HRESULT __stdcall BeginScene() override;
    HRESULT __stdcall EndScene() override;
    HRESULT __stdcall Clear(DWORD Count,CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) override;
    HRESULT __stdcall SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix) override;
    HRESULT __stdcall GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) override;
    HRESULT __stdcall MultiplyTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix) override;
    HRESULT __stdcall SetViewport(CONST D3DVIEWPORT9* pViewport) override;
    HRESULT __stdcall GetViewport(D3DVIEWPORT9* pViewport) override;
    HRESULT __stdcall SetMaterial(CONST D3DMATERIAL9* pMaterial) override;
    HRESULT __stdcall GetMaterial(D3DMATERIAL9* pMaterial) override;
    HRESULT __stdcall SetLight(DWORD Index,CONST D3DLIGHT9* pLight) override;
    HRESULT __stdcall GetLight(DWORD Index, D3DLIGHT9* pLight) override;
    HRESULT __stdcall LightEnable(DWORD Index, BOOL Enable) override;
    HRESULT __stdcall GetLightEnable(DWORD Index, BOOL* pEnable) override;
    HRESULT __stdcall SetClipPlane(DWORD Index,CONST float* pPlane) override;
    HRESULT __stdcall GetClipPlane(DWORD Index, float* pPlane) override;
    HRESULT __stdcall SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) override;
    HRESULT __stdcall GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) override;
    HRESULT __stdcall CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) override;
    HRESULT __stdcall BeginStateBlock() override;
    HRESULT __stdcall EndStateBlock(IDirect3DStateBlock9** ppSB) override;
    HRESULT __stdcall SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) override;
    HRESULT __stdcall GetClipStatus(D3DCLIPSTATUS9* pClipStatus) override;
    HRESULT __stdcall GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) override;
    HRESULT __stdcall SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) override;
    HRESULT __stdcall GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) override;
    HRESULT __stdcall SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) override;
    HRESULT __stdcall GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) override;
    HRESULT __stdcall SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) override;
    HRESULT __stdcall ValidateDevice(DWORD* pNumPasses) override;
    HRESULT __stdcall SetPaletteEntries(UINT PaletteNumber,CONST PALETTEENTRY* pEntries) override;
    HRESULT __stdcall GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) override;
    HRESULT __stdcall SetCurrentTexturePalette(UINT PaletteNumber) override;
    HRESULT __stdcall GetCurrentTexturePalette(UINT* PaletteNumber) override;
    HRESULT __stdcall SetScissorRect(CONST RECT* pRect) override;
    HRESULT __stdcall GetScissorRect(RECT* pRect) override;
    HRESULT __stdcall SetSoftwareVertexProcessing(BOOL bSoftware) override;
    BOOL __stdcall GetSoftwareVertexProcessing() override;
    HRESULT __stdcall SetNPatchMode(float nSegments) override;
    float __stdcall GetNPatchMode() override;
    HRESULT __stdcall DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) override;
    HRESULT __stdcall DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) override;
    HRESULT __stdcall DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount,CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) override;
    HRESULT __stdcall DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount,CONST void* pIndexData, D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,
                                             UINT VertexStreamZeroStride) override;
    HRESULT __stdcall ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) override;
    HRESULT __stdcall CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) override;
    HRESULT __stdcall SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) override;
    HRESULT __stdcall GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) override;
    HRESULT __stdcall SetFVF(DWORD FVF) override;
    HRESULT __stdcall GetFVF(DWORD* pFVF) override;
    HRESULT __stdcall CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader) override;
    HRESULT __stdcall SetVertexShader(IDirect3DVertexShader9* pShader) override;
    HRESULT __stdcall GetVertexShader(IDirect3DVertexShader9** ppShader) override;
    HRESULT __stdcall SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData, UINT Vector4fCount) override;
    HRESULT __stdcall GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) override;
    HRESULT __stdcall SetVertexShaderConstantI(UINT StartRegister,CONST int* pConstantData, UINT Vector4iCount) override;
    HRESULT __stdcall GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) override;
    HRESULT __stdcall SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount) override;
    HRESULT __stdcall GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) override;
    HRESULT __stdcall SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) override;
    HRESULT __stdcall GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride) override;
    HRESULT __stdcall SetStreamSourceFreq(UINT StreamNumber, UINT Divider) override;
    HRESULT __stdcall GetStreamSourceFreq(UINT StreamNumber, UINT* Divider) override;
    HRESULT __stdcall SetIndices(IDirect3DIndexBuffer9* pIndexData) override;
    HRESULT __stdcall GetIndices(IDirect3DIndexBuffer9** ppIndexData) override;
    HRESULT __stdcall CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader) override;
    HRESULT __stdcall SetPixelShader(IDirect3DPixelShader9* pShader) override;
    HRESULT __stdcall GetPixelShader(IDirect3DPixelShader9** ppShader) override;
    HRESULT __stdcall SetPixelShaderConstantF(UINT StartRegister,CONST float* pConstantData, UINT Vector4fCount) override;
    HRESULT __stdcall GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) override;
    HRESULT __stdcall SetPixelShaderConstantI(UINT StartRegister,CONST int* pConstantData, UINT Vector4iCount) override;
    HRESULT __stdcall GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) override;
    HRESULT __stdcall SetPixelShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount) override;
    HRESULT __stdcall GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) override;
    HRESULT __stdcall DrawRectPatch(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo) override;
    HRESULT __stdcall DrawTriPatch(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo) override;
    HRESULT __stdcall DeletePatch(UINT Handle) override;
    HRESULT __stdcall CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) override;



    HRESULT __stdcall CheckDeviceState(HWND hWindow) override;
    HRESULT __stdcall CheckResourceResidency(IDirect3DResource9** pResourceArray, UINT32 NumResources) override;
    HRESULT __stdcall ComposeRects(IDirect3DSurface9* pSource, IDirect3DSurface9* pDestination, IDirect3DVertexBuffer9* pSrcRectDescriptors, UINT NumRects, IDirect3DVertexBuffer9* pDstRectDescriptors, D3DCOMPOSERECTSOP Operation, INT XOffset,
                                   INT YOffset) override;
    HRESULT __stdcall CreateDepthStencilSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) override;
    HRESULT __stdcall CreateOffscreenPlainSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) override;
    HRESULT __stdcall CreateRenderTargetEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) override;
    HRESULT __stdcall GetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) override;
    HRESULT __stdcall GetGPUThreadPriority(INT* pPriority) override;
    HRESULT __stdcall GetMaximumFrameLatency(UINT* pMaxLatency) override;
    HRESULT __stdcall PresentEx(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags) override;
    HRESULT __stdcall ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode) override;
    HRESULT __stdcall SetConvolutionMonoKernel(UINT Width, UINT Height, float* RowWeights, float* ColumnWeights) override;
    HRESULT __stdcall SetGPUThreadPriority(INT pPriority) override;
    HRESULT __stdcall SetMaximumFrameLatency(UINT pMaxLatency) override;
    //HRESULT __stdcall TestCooperativeLevel();
    HRESULT __stdcall WaitForVBlank(UINT SwapChainIndex) override;


    // END: The original DX9 function definitions


    uMod_TextureClient* GetuMod_Client() { return uMod_Client; }

    uMod_IDirect3DTexture9* GetLastCreatedTexture() { return LastCreatedTexture; }

    int SetLastCreatedTexture(uMod_IDirect3DTexture9* pTexture)
    {
        LastCreatedTexture = pTexture;
        return RETURN_OK;
    }

    uMod_IDirect3DVolumeTexture9* GetLastCreatedVolumeTexture() { return LastCreatedVolumeTexture; }

    int SetLastCreatedVolumeTexture(uMod_IDirect3DVolumeTexture9* pTexture)
    {
        LastCreatedVolumeTexture = pTexture;
        return RETURN_OK;
    }

    uMod_IDirect3DCubeTexture9* GetLastCreatedCubeTexture() { return LastCreatedCubeTexture; }

    int SetLastCreatedCubeTexture(uMod_IDirect3DCubeTexture9* pTexture)
    {
        LastCreatedCubeTexture = pTexture;
        return RETURN_OK;
    }


    uMod_IDirect3DTexture9* GetSingleTexture() { return SingleTexture; }
    uMod_IDirect3DVolumeTexture9* GetSingleVolumeTexture() { return SingleVolumeTexture; }
    uMod_IDirect3DCubeTexture9* GetSingleCubeTexture() { return SingleCubeTexture; }

private:
    int CreateSingleTexture();
    IDirect3DDevice9Ex* m_pIDirect3DDevice9Ex;

    int CounterSaveSingleTexture;
    uMod_IDirect3DTexture9* SingleTexture;
    uMod_IDirect3DVolumeTexture9* SingleVolumeTexture;
    uMod_IDirect3DCubeTexture9* SingleCubeTexture;
    char SingleTextureMod;

    D3DCOLOR TextureColour;
    ID3DXFont* OSD_Font;
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
