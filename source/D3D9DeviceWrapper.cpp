#include "Main.h"
#include "D3D9DeviceWrapper.h"
#include "D3D9TextureWrappers.h"

import TextureClient;

namespace {

    // Unwraps a WrappedTexture9/WrappedVolumeTexture9/WrappedCubeTexture9 to the real
    // pointer the real device needs; passes non-wrapper (or null) pointers through as-is.
    IDirect3DBaseTexture9* Unwrap(IDirect3DBaseTexture9* texture)
    {
        if (texture == nullptr) return nullptr;
        switch (texture->GetType()) {
            case D3DRTYPE_TEXTURE: return static_cast<WrappedTexture9*>(texture)->RealTexture();
            case D3DRTYPE_VOLUMETEXTURE: return static_cast<WrappedVolumeTexture9*>(texture)->RealTexture();
            case D3DRTYPE_CUBETEXTURE: return static_cast<WrappedCubeTexture9*>(texture)->RealTexture();
            default: return texture;
        }
    }

}

// ---- WrappedDirect3DDevice9 ----------------------------------------------

WrappedDirect3DDevice9::WrappedDirect3DDevice9(IDirect3DDevice9* real) : real_(real), ref_count_(1), client_(nullptr)
{
    real_->AddRef();
}

HRESULT WrappedDirect3DDevice9::QueryInterface(REFIID riid, void** ppvObj)
{
    if (riid == IID_IUnknown || riid == IID_IDirect3DDevice9) {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }
    // Unrecognized-but-successful QueryInterface hands back an unwrapped real pointer.
    return real_->QueryInterface(riid, ppvObj);
}

ULONG WrappedDirect3DDevice9::AddRef()
{
    return ++ref_count_;
}

void WrappedDirect3DDevice9::TeardownTextureClient()
{
    if (auto* client = TextureClient::CurrentClient())
        delete client;
}

ULONG WrappedDirect3DDevice9::Release()
{
    const ULONG count = --ref_count_;
    if (count == 0) {
        TeardownTextureClient();
        real_->Release();
        delete this;
        return 0;
    }
    return count;
}

HRESULT WrappedDirect3DDevice9::TestCooperativeLevel() { return real_->TestCooperativeLevel(); }
UINT WrappedDirect3DDevice9::GetAvailableTextureMem() { return real_->GetAvailableTextureMem(); }
HRESULT WrappedDirect3DDevice9::EvictManagedResources() { return real_->EvictManagedResources(); }
HRESULT WrappedDirect3DDevice9::GetDirect3D(IDirect3D9** ppD3D9) { return real_->GetDirect3D(ppD3D9); }
HRESULT WrappedDirect3DDevice9::GetDeviceCaps(D3DCAPS9* pCaps) { return real_->GetDeviceCaps(pCaps); }
HRESULT WrappedDirect3DDevice9::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) { return real_->GetDisplayMode(iSwapChain, pMode); }
HRESULT WrappedDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) { return real_->GetCreationParameters(pParameters); }
HRESULT WrappedDirect3DDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) { return real_->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap); }
void WrappedDirect3DDevice9::SetCursorPosition(int X, int Y, DWORD Flags) { real_->SetCursorPosition(X, Y, Flags); }
BOOL WrappedDirect3DDevice9::ShowCursor(BOOL bShow) { return real_->ShowCursor(bShow); }
HRESULT WrappedDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) { return real_->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain); }
HRESULT WrappedDirect3DDevice9::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) { return real_->GetSwapChain(iSwapChain, pSwapChain); }
UINT WrappedDirect3DDevice9::GetNumberOfSwapChains() { return real_->GetNumberOfSwapChains(); }
HRESULT WrappedDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) { return real_->Reset(pPresentationParameters); }
HRESULT WrappedDirect3DDevice9::Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) { return real_->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion); }
HRESULT WrappedDirect3DDevice9::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) { return real_->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer); }
HRESULT WrappedDirect3DDevice9::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) { return real_->GetRasterStatus(iSwapChain, pRasterStatus); }
HRESULT WrappedDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs) { return real_->SetDialogBoxMode(bEnableDialogs); }
void WrappedDirect3DDevice9::SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp) { real_->SetGammaRamp(iSwapChain, Flags, pRamp); }
void WrappedDirect3DDevice9::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) { real_->GetGammaRamp(iSwapChain, pRamp); }

HRESULT WrappedDirect3DDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
    IDirect3DTexture9* real_texture = nullptr;
    const HRESULT hr = real_->CreateTexture(Width, Height, Levels, Usage, Format, Pool, &real_texture, pSharedHandle);
    if (FAILED(hr) || real_texture == nullptr) {
        if (ppTexture) *ppTexture = nullptr;
        return hr;
    }
    const auto wrapped = new WrappedTexture9(real_texture);
    real_texture->Release(); // the wrapper holds its own AddRef'd reference
    if (client_) client_->OnCreateTexture(wrapped, TexType::Tex2D);
    *ppTexture = wrapped;
    return hr;
}

HRESULT WrappedDirect3DDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
    IDirect3DVolumeTexture9* real_texture = nullptr;
    const HRESULT hr = real_->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, &real_texture, pSharedHandle);
    if (FAILED(hr) || real_texture == nullptr) {
        if (ppVolumeTexture) *ppVolumeTexture = nullptr;
        return hr;
    }
    const auto wrapped = new WrappedVolumeTexture9(real_texture);
    real_texture->Release();
    if (client_) client_->OnCreateTexture(wrapped, TexType::Volume);
    *ppVolumeTexture = wrapped;
    return hr;
}

HRESULT WrappedDirect3DDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
    IDirect3DCubeTexture9* real_texture = nullptr;
    const HRESULT hr = real_->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, &real_texture, pSharedHandle);
    if (FAILED(hr) || real_texture == nullptr) {
        if (ppCubeTexture) *ppCubeTexture = nullptr;
        return hr;
    }
    const auto wrapped = new WrappedCubeTexture9(real_texture);
    real_texture->Release();
    if (client_) client_->OnCreateTexture(wrapped, TexType::Cube);
    *ppCubeTexture = wrapped;
    return hr;
}

HRESULT WrappedDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) { return real_->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle); }
HRESULT WrappedDirect3DDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) { return real_->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle); }
HRESULT WrappedDirect3DDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { return real_->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle); }
HRESULT WrappedDirect3DDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { return real_->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle); }
HRESULT WrappedDirect3DDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestPoint) { return real_->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint); }

HRESULT WrappedDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
    const HRESULT hr = real_->UpdateTexture(Unwrap(pSourceTexture), Unwrap(pDestinationTexture));
    if (SUCCEEDED(hr) && client_)
        client_->OnUpdateTexture(pSourceTexture, pDestinationTexture); // re-hash by wrapper identity
    return hr;
}

HRESULT WrappedDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) { return real_->GetRenderTargetData(pRenderTarget, pDestSurface); }
HRESULT WrappedDirect3DDevice9::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) { return real_->GetFrontBufferData(iSwapChain, pDestSurface); }
HRESULT WrappedDirect3DDevice9::StretchRect(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) { return real_->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter); }
HRESULT WrappedDirect3DDevice9::ColorFill(IDirect3DSurface9* pSurface, const RECT* pRect, D3DCOLOR color) { return real_->ColorFill(pSurface, pRect, color); }
HRESULT WrappedDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { return real_->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle); }
HRESULT WrappedDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) { return real_->SetRenderTarget(RenderTargetIndex, pRenderTarget); }
HRESULT WrappedDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) { return real_->GetRenderTarget(RenderTargetIndex, ppRenderTarget); }
HRESULT WrappedDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) { return real_->SetDepthStencilSurface(pNewZStencil); }
HRESULT WrappedDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) { return real_->GetDepthStencilSurface(ppZStencilSurface); }

HRESULT WrappedDirect3DDevice9::BeginScene()
{
    if (client_) client_->OnBeginScene();
    return real_->BeginScene();
}

HRESULT WrappedDirect3DDevice9::EndScene() { return real_->EndScene(); }
HRESULT WrappedDirect3DDevice9::Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) { return real_->Clear(Count, pRects, Flags, Color, Z, Stencil); }
HRESULT WrappedDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) { return real_->SetTransform(State, pMatrix); }
HRESULT WrappedDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) { return real_->GetTransform(State, pMatrix); }
HRESULT WrappedDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX* matrix) { return real_->MultiplyTransform(state, matrix); }
HRESULT WrappedDirect3DDevice9::SetViewport(const D3DVIEWPORT9* pViewport) { return real_->SetViewport(pViewport); }
HRESULT WrappedDirect3DDevice9::GetViewport(D3DVIEWPORT9* pViewport) { return real_->GetViewport(pViewport); }
HRESULT WrappedDirect3DDevice9::SetMaterial(const D3DMATERIAL9* pMaterial) { return real_->SetMaterial(pMaterial); }
HRESULT WrappedDirect3DDevice9::GetMaterial(D3DMATERIAL9* pMaterial) { return real_->GetMaterial(pMaterial); }
HRESULT WrappedDirect3DDevice9::SetLight(DWORD Index, const D3DLIGHT9* light) { return real_->SetLight(Index, light); }
HRESULT WrappedDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9* light) { return real_->GetLight(Index, light); }
HRESULT WrappedDirect3DDevice9::LightEnable(DWORD Index, BOOL Enable) { return real_->LightEnable(Index, Enable); }
HRESULT WrappedDirect3DDevice9::GetLightEnable(DWORD Index, BOOL* pEnable) { return real_->GetLightEnable(Index, pEnable); }
HRESULT WrappedDirect3DDevice9::SetClipPlane(DWORD Index, const float* pPlane) { return real_->SetClipPlane(Index, pPlane); }
HRESULT WrappedDirect3DDevice9::GetClipPlane(DWORD Index, float* pPlane) { return real_->GetClipPlane(Index, pPlane); }
HRESULT WrappedDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) { return real_->SetRenderState(State, Value); }
HRESULT WrappedDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) { return real_->GetRenderState(State, pValue); }
HRESULT WrappedDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) { return real_->CreateStateBlock(Type, ppSB); }
HRESULT WrappedDirect3DDevice9::BeginStateBlock() { return real_->BeginStateBlock(); }
HRESULT WrappedDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB) { return real_->EndStateBlock(ppSB); }
HRESULT WrappedDirect3DDevice9::SetClipStatus(const D3DCLIPSTATUS9* pClipStatus) { return real_->SetClipStatus(pClipStatus); }
HRESULT WrappedDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9* pClipStatus) { return real_->GetClipStatus(pClipStatus); }

HRESULT WrappedDirect3DDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
    const HRESULT hr = real_->GetTexture(Stage, ppTexture);
    if (FAILED(hr) || !ppTexture || !*ppTexture) return hr;

    // The real call only ever returns real pointers; recover the wrapper identity.
    auto* wrapper = FindWrapperForRealTexture(*ppTexture);
    if (wrapper == nullptr) {
        // Not one of ours (created before hooks/pass-through path) - nothing to translate.
        return hr;
    }
    (*ppTexture)->Release(); // drop the real ref the runtime handed back
    IDirect3DBaseTexture9* result = wrapper;

    if (client_) {
        if (auto* original = client_->ResolveOriginalFromFake(wrapper)) {
            original->AddRef();
            result = original;
        }
        else {
            wrapper->AddRef();
        }
    }
    else {
        wrapper->AddRef();
    }
    *ppTexture = result;
    return hr;
}

HRESULT WrappedDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
    IDirect3DBaseTexture9* bind = client_ ? client_->ResolveBinding(pTexture) : pTexture;
    return real_->SetTexture(Stage, Unwrap(bind));
}

HRESULT WrappedDirect3DDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) { return real_->GetTextureStageState(Stage, Type, pValue); }
HRESULT WrappedDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) { return real_->SetTextureStageState(Stage, Type, Value); }
HRESULT WrappedDirect3DDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) { return real_->GetSamplerState(Sampler, Type, pValue); }
HRESULT WrappedDirect3DDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) { return real_->SetSamplerState(Sampler, Type, Value); }
HRESULT WrappedDirect3DDevice9::ValidateDevice(DWORD* pNumPasses) { return real_->ValidateDevice(pNumPasses); }
HRESULT WrappedDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries) { return real_->SetPaletteEntries(PaletteNumber, pEntries); }
HRESULT WrappedDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) { return real_->GetPaletteEntries(PaletteNumber, pEntries); }
HRESULT WrappedDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber) { return real_->SetCurrentTexturePalette(PaletteNumber); }
HRESULT WrappedDirect3DDevice9::GetCurrentTexturePalette(UINT* PaletteNumber) { return real_->GetCurrentTexturePalette(PaletteNumber); }
HRESULT WrappedDirect3DDevice9::SetScissorRect(const RECT* pRect) { return real_->SetScissorRect(pRect); }
HRESULT WrappedDirect3DDevice9::GetScissorRect(RECT* pRect) { return real_->GetScissorRect(pRect); }
HRESULT WrappedDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware) { return real_->SetSoftwareVertexProcessing(bSoftware); }
BOOL WrappedDirect3DDevice9::GetSoftwareVertexProcessing() { return real_->GetSoftwareVertexProcessing(); }
HRESULT WrappedDirect3DDevice9::SetNPatchMode(float nSegments) { return real_->SetNPatchMode(nSegments); }
float WrappedDirect3DDevice9::GetNPatchMode() { return real_->GetNPatchMode(); }
HRESULT WrappedDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) { return real_->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount); }
HRESULT WrappedDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) { return real_->DrawIndexedPrimitive(type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount); }
HRESULT WrappedDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) { return real_->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride); }
HRESULT WrappedDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) { return real_->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); }
HRESULT WrappedDirect3DDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) { return real_->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags); }
HRESULT WrappedDirect3DDevice9::CreateVertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) { return real_->CreateVertexDeclaration(pVertexElements, ppDecl); }
HRESULT WrappedDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) { return real_->SetVertexDeclaration(pDecl); }
HRESULT WrappedDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) { return real_->GetVertexDeclaration(ppDecl); }
HRESULT WrappedDirect3DDevice9::SetFVF(DWORD FVF) { return real_->SetFVF(FVF); }
HRESULT WrappedDirect3DDevice9::GetFVF(DWORD* pFVF) { return real_->GetFVF(pFVF); }
HRESULT WrappedDirect3DDevice9::CreateVertexShader(const DWORD* pFunction, IDirect3DVertexShader9** ppShader) { return real_->CreateVertexShader(pFunction, ppShader); }
HRESULT WrappedDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9* pShader) { return real_->SetVertexShader(pShader); }
HRESULT WrappedDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader) { return real_->GetVertexShader(ppShader); }
HRESULT WrappedDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount) { return real_->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT WrappedDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) { return real_->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT WrappedDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount) { return real_->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT WrappedDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) { return real_->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT WrappedDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount) { return real_->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT WrappedDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) { return real_->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT WrappedDirect3DDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) { return real_->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride); }
HRESULT WrappedDirect3DDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride) { return real_->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride); }
HRESULT WrappedDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT Setting) { return real_->SetStreamSourceFreq(StreamNumber, Setting); }
HRESULT WrappedDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting) { return real_->GetStreamSourceFreq(StreamNumber, pSetting); }
HRESULT WrappedDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData) { return real_->SetIndices(pIndexData); }
HRESULT WrappedDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9** ppIndexData) { return real_->GetIndices(ppIndexData); }
HRESULT WrappedDirect3DDevice9::CreatePixelShader(const DWORD* pFunction, IDirect3DPixelShader9** ppShader) { return real_->CreatePixelShader(pFunction, ppShader); }
HRESULT WrappedDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9* pShader) { return real_->SetPixelShader(pShader); }
HRESULT WrappedDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader) { return real_->GetPixelShader(ppShader); }
HRESULT WrappedDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount) { return real_->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT WrappedDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) { return real_->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT WrappedDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount) { return real_->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT WrappedDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) { return real_->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT WrappedDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount) { return real_->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT WrappedDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) { return real_->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT WrappedDirect3DDevice9::DrawRectPatch(UINT Handle, const float* pNumSegs, const D3DRECTPATCH_INFO* pRectPatchInfo) { return real_->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo); }
HRESULT WrappedDirect3DDevice9::DrawTriPatch(UINT Handle, const float* pNumSegs, const D3DTRIPATCH_INFO* pTriPatchInfo) { return real_->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo); }
HRESULT WrappedDirect3DDevice9::DeletePatch(UINT Handle) { return real_->DeletePatch(Handle); }
HRESULT WrappedDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) { return real_->CreateQuery(Type, ppQuery); }

// ---- WrappedDirect3DDevice9Ex ---------------------------------------------

WrappedDirect3DDevice9Ex::WrappedDirect3DDevice9Ex(IDirect3DDevice9Ex* real) : real_(real), ref_count_(1), client_(nullptr)
{
    real_->AddRef();
}

HRESULT WrappedDirect3DDevice9Ex::QueryInterface(REFIID riid, void** ppvObj)
{
    if (riid == IID_IUnknown || riid == IID_IDirect3DDevice9 || riid == IID_IDirect3DDevice9Ex) {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }
    // Unrecognized-but-successful QueryInterface hands back an unwrapped real pointer.
    return real_->QueryInterface(riid, ppvObj);
}

ULONG WrappedDirect3DDevice9Ex::AddRef()
{
    return ++ref_count_;
}

void WrappedDirect3DDevice9Ex::TeardownTextureClient()
{
    if (auto* client = TextureClient::CurrentClient())
        delete client;
}

ULONG WrappedDirect3DDevice9Ex::Release()
{
    const ULONG count = --ref_count_;
    if (count == 0) {
        TeardownTextureClient();
        real_->Release();
        delete this;
        return 0;
    }
    return count;
}

HRESULT WrappedDirect3DDevice9Ex::TestCooperativeLevel() { return real_->TestCooperativeLevel(); }
UINT WrappedDirect3DDevice9Ex::GetAvailableTextureMem() { return real_->GetAvailableTextureMem(); }
HRESULT WrappedDirect3DDevice9Ex::EvictManagedResources() { return real_->EvictManagedResources(); }
HRESULT WrappedDirect3DDevice9Ex::GetDirect3D(IDirect3D9** ppD3D9) { return real_->GetDirect3D(ppD3D9); }
HRESULT WrappedDirect3DDevice9Ex::GetDeviceCaps(D3DCAPS9* pCaps) { return real_->GetDeviceCaps(pCaps); }
HRESULT WrappedDirect3DDevice9Ex::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) { return real_->GetDisplayMode(iSwapChain, pMode); }
HRESULT WrappedDirect3DDevice9Ex::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) { return real_->GetCreationParameters(pParameters); }
HRESULT WrappedDirect3DDevice9Ex::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap) { return real_->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap); }
void WrappedDirect3DDevice9Ex::SetCursorPosition(int X, int Y, DWORD Flags) { real_->SetCursorPosition(X, Y, Flags); }
BOOL WrappedDirect3DDevice9Ex::ShowCursor(BOOL bShow) { return real_->ShowCursor(bShow); }
HRESULT WrappedDirect3DDevice9Ex::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain) { return real_->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain); }
HRESULT WrappedDirect3DDevice9Ex::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) { return real_->GetSwapChain(iSwapChain, pSwapChain); }
UINT WrappedDirect3DDevice9Ex::GetNumberOfSwapChains() { return real_->GetNumberOfSwapChains(); }
HRESULT WrappedDirect3DDevice9Ex::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) { return real_->Reset(pPresentationParameters); }
HRESULT WrappedDirect3DDevice9Ex::Present(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) { return real_->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion); }
HRESULT WrappedDirect3DDevice9Ex::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) { return real_->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer); }
HRESULT WrappedDirect3DDevice9Ex::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) { return real_->GetRasterStatus(iSwapChain, pRasterStatus); }
HRESULT WrappedDirect3DDevice9Ex::SetDialogBoxMode(BOOL bEnableDialogs) { return real_->SetDialogBoxMode(bEnableDialogs); }
void WrappedDirect3DDevice9Ex::SetGammaRamp(UINT iSwapChain, DWORD Flags, const D3DGAMMARAMP* pRamp) { real_->SetGammaRamp(iSwapChain, Flags, pRamp); }
void WrappedDirect3DDevice9Ex::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) { real_->GetGammaRamp(iSwapChain, pRamp); }

HRESULT WrappedDirect3DDevice9Ex::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
    IDirect3DTexture9* real_texture = nullptr;
    const HRESULT hr = real_->CreateTexture(Width, Height, Levels, Usage, Format, Pool, &real_texture, pSharedHandle);
    if (FAILED(hr) || real_texture == nullptr) {
        if (ppTexture) *ppTexture = nullptr;
        return hr;
    }
    const auto wrapped = new WrappedTexture9(real_texture);
    real_texture->Release();
    if (client_) client_->OnCreateTexture(wrapped, TexType::Tex2D);
    *ppTexture = wrapped;
    return hr;
}

HRESULT WrappedDirect3DDevice9Ex::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
    IDirect3DVolumeTexture9* real_texture = nullptr;
    const HRESULT hr = real_->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, &real_texture, pSharedHandle);
    if (FAILED(hr) || real_texture == nullptr) {
        if (ppVolumeTexture) *ppVolumeTexture = nullptr;
        return hr;
    }
    const auto wrapped = new WrappedVolumeTexture9(real_texture);
    real_texture->Release();
    if (client_) client_->OnCreateTexture(wrapped, TexType::Volume);
    *ppVolumeTexture = wrapped;
    return hr;
}

HRESULT WrappedDirect3DDevice9Ex::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
    IDirect3DCubeTexture9* real_texture = nullptr;
    const HRESULT hr = real_->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, &real_texture, pSharedHandle);
    if (FAILED(hr) || real_texture == nullptr) {
        if (ppCubeTexture) *ppCubeTexture = nullptr;
        return hr;
    }
    const auto wrapped = new WrappedCubeTexture9(real_texture);
    real_texture->Release();
    if (client_) client_->OnCreateTexture(wrapped, TexType::Cube);
    *ppCubeTexture = wrapped;
    return hr;
}

HRESULT WrappedDirect3DDevice9Ex::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) { return real_->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle); }
HRESULT WrappedDirect3DDevice9Ex::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) { return real_->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle); }
HRESULT WrappedDirect3DDevice9Ex::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { return real_->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle); }
HRESULT WrappedDirect3DDevice9Ex::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { return real_->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle); }
HRESULT WrappedDirect3DDevice9Ex::UpdateSurface(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, const POINT* pDestPoint) { return real_->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint); }

HRESULT WrappedDirect3DDevice9Ex::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
    const HRESULT hr = real_->UpdateTexture(Unwrap(pSourceTexture), Unwrap(pDestinationTexture));
    if (SUCCEEDED(hr) && client_)
        client_->OnUpdateTexture(pSourceTexture, pDestinationTexture);
    return hr;
}

HRESULT WrappedDirect3DDevice9Ex::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) { return real_->GetRenderTargetData(pRenderTarget, pDestSurface); }
HRESULT WrappedDirect3DDevice9Ex::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) { return real_->GetFrontBufferData(iSwapChain, pDestSurface); }
HRESULT WrappedDirect3DDevice9Ex::StretchRect(IDirect3DSurface9* pSourceSurface, const RECT* pSourceRect, IDirect3DSurface9* pDestSurface, const RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter) { return real_->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter); }
HRESULT WrappedDirect3DDevice9Ex::ColorFill(IDirect3DSurface9* pSurface, const RECT* pRect, D3DCOLOR color) { return real_->ColorFill(pSurface, pRect, color); }
HRESULT WrappedDirect3DDevice9Ex::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) { return real_->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle); }
HRESULT WrappedDirect3DDevice9Ex::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget) { return real_->SetRenderTarget(RenderTargetIndex, pRenderTarget); }
HRESULT WrappedDirect3DDevice9Ex::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget) { return real_->GetRenderTarget(RenderTargetIndex, ppRenderTarget); }
HRESULT WrappedDirect3DDevice9Ex::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) { return real_->SetDepthStencilSurface(pNewZStencil); }
HRESULT WrappedDirect3DDevice9Ex::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) { return real_->GetDepthStencilSurface(ppZStencilSurface); }

HRESULT WrappedDirect3DDevice9Ex::BeginScene()
{
    if (client_) client_->OnBeginScene();
    return real_->BeginScene();
}

HRESULT WrappedDirect3DDevice9Ex::EndScene() { return real_->EndScene(); }
HRESULT WrappedDirect3DDevice9Ex::Clear(DWORD Count, const D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) { return real_->Clear(Count, pRects, Flags, Color, Z, Stencil); }
HRESULT WrappedDirect3DDevice9Ex::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) { return real_->SetTransform(State, pMatrix); }
HRESULT WrappedDirect3DDevice9Ex::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) { return real_->GetTransform(State, pMatrix); }
HRESULT WrappedDirect3DDevice9Ex::MultiplyTransform(D3DTRANSFORMSTATETYPE state, const D3DMATRIX* matrix) { return real_->MultiplyTransform(state, matrix); }
HRESULT WrappedDirect3DDevice9Ex::SetViewport(const D3DVIEWPORT9* pViewport) { return real_->SetViewport(pViewport); }
HRESULT WrappedDirect3DDevice9Ex::GetViewport(D3DVIEWPORT9* pViewport) { return real_->GetViewport(pViewport); }
HRESULT WrappedDirect3DDevice9Ex::SetMaterial(const D3DMATERIAL9* pMaterial) { return real_->SetMaterial(pMaterial); }
HRESULT WrappedDirect3DDevice9Ex::GetMaterial(D3DMATERIAL9* pMaterial) { return real_->GetMaterial(pMaterial); }
HRESULT WrappedDirect3DDevice9Ex::SetLight(DWORD Index, const D3DLIGHT9* light) { return real_->SetLight(Index, light); }
HRESULT WrappedDirect3DDevice9Ex::GetLight(DWORD Index, D3DLIGHT9* light) { return real_->GetLight(Index, light); }
HRESULT WrappedDirect3DDevice9Ex::LightEnable(DWORD Index, BOOL Enable) { return real_->LightEnable(Index, Enable); }
HRESULT WrappedDirect3DDevice9Ex::GetLightEnable(DWORD Index, BOOL* pEnable) { return real_->GetLightEnable(Index, pEnable); }
HRESULT WrappedDirect3DDevice9Ex::SetClipPlane(DWORD Index, const float* pPlane) { return real_->SetClipPlane(Index, pPlane); }
HRESULT WrappedDirect3DDevice9Ex::GetClipPlane(DWORD Index, float* pPlane) { return real_->GetClipPlane(Index, pPlane); }
HRESULT WrappedDirect3DDevice9Ex::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) { return real_->SetRenderState(State, Value); }
HRESULT WrappedDirect3DDevice9Ex::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) { return real_->GetRenderState(State, pValue); }
HRESULT WrappedDirect3DDevice9Ex::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB) { return real_->CreateStateBlock(Type, ppSB); }
HRESULT WrappedDirect3DDevice9Ex::BeginStateBlock() { return real_->BeginStateBlock(); }
HRESULT WrappedDirect3DDevice9Ex::EndStateBlock(IDirect3DStateBlock9** ppSB) { return real_->EndStateBlock(ppSB); }
HRESULT WrappedDirect3DDevice9Ex::SetClipStatus(const D3DCLIPSTATUS9* pClipStatus) { return real_->SetClipStatus(pClipStatus); }
HRESULT WrappedDirect3DDevice9Ex::GetClipStatus(D3DCLIPSTATUS9* pClipStatus) { return real_->GetClipStatus(pClipStatus); }

HRESULT WrappedDirect3DDevice9Ex::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
    const HRESULT hr = real_->GetTexture(Stage, ppTexture);
    if (FAILED(hr) || !ppTexture || !*ppTexture) return hr;

    auto* wrapper = FindWrapperForRealTexture(*ppTexture);
    if (wrapper == nullptr) {
        return hr;
    }
    (*ppTexture)->Release();
    IDirect3DBaseTexture9* result = wrapper;

    if (client_) {
        if (auto* original = client_->ResolveOriginalFromFake(wrapper)) {
            original->AddRef();
            result = original;
        }
        else {
            wrapper->AddRef();
        }
    }
    else {
        wrapper->AddRef();
    }
    *ppTexture = result;
    return hr;
}

HRESULT WrappedDirect3DDevice9Ex::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
    IDirect3DBaseTexture9* bind = client_ ? client_->ResolveBinding(pTexture) : pTexture;
    return real_->SetTexture(Stage, Unwrap(bind));
}

HRESULT WrappedDirect3DDevice9Ex::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) { return real_->GetTextureStageState(Stage, Type, pValue); }
HRESULT WrappedDirect3DDevice9Ex::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) { return real_->SetTextureStageState(Stage, Type, Value); }
HRESULT WrappedDirect3DDevice9Ex::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) { return real_->GetSamplerState(Sampler, Type, pValue); }
HRESULT WrappedDirect3DDevice9Ex::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) { return real_->SetSamplerState(Sampler, Type, Value); }
HRESULT WrappedDirect3DDevice9Ex::ValidateDevice(DWORD* pNumPasses) { return real_->ValidateDevice(pNumPasses); }
HRESULT WrappedDirect3DDevice9Ex::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries) { return real_->SetPaletteEntries(PaletteNumber, pEntries); }
HRESULT WrappedDirect3DDevice9Ex::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) { return real_->GetPaletteEntries(PaletteNumber, pEntries); }
HRESULT WrappedDirect3DDevice9Ex::SetCurrentTexturePalette(UINT PaletteNumber) { return real_->SetCurrentTexturePalette(PaletteNumber); }
HRESULT WrappedDirect3DDevice9Ex::GetCurrentTexturePalette(UINT* PaletteNumber) { return real_->GetCurrentTexturePalette(PaletteNumber); }
HRESULT WrappedDirect3DDevice9Ex::SetScissorRect(const RECT* pRect) { return real_->SetScissorRect(pRect); }
HRESULT WrappedDirect3DDevice9Ex::GetScissorRect(RECT* pRect) { return real_->GetScissorRect(pRect); }
HRESULT WrappedDirect3DDevice9Ex::SetSoftwareVertexProcessing(BOOL bSoftware) { return real_->SetSoftwareVertexProcessing(bSoftware); }
BOOL WrappedDirect3DDevice9Ex::GetSoftwareVertexProcessing() { return real_->GetSoftwareVertexProcessing(); }
HRESULT WrappedDirect3DDevice9Ex::SetNPatchMode(float nSegments) { return real_->SetNPatchMode(nSegments); }
float WrappedDirect3DDevice9Ex::GetNPatchMode() { return real_->GetNPatchMode(); }
HRESULT WrappedDirect3DDevice9Ex::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) { return real_->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount); }
HRESULT WrappedDirect3DDevice9Ex::DrawIndexedPrimitive(D3DPRIMITIVETYPE type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) { return real_->DrawIndexedPrimitive(type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount); }
HRESULT WrappedDirect3DDevice9Ex::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) { return real_->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride); }
HRESULT WrappedDirect3DDevice9Ex::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, const void* pIndexData, D3DFORMAT IndexDataFormat, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride) { return real_->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); }
HRESULT WrappedDirect3DDevice9Ex::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) { return real_->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags); }
HRESULT WrappedDirect3DDevice9Ex::CreateVertexDeclaration(const D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) { return real_->CreateVertexDeclaration(pVertexElements, ppDecl); }
HRESULT WrappedDirect3DDevice9Ex::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) { return real_->SetVertexDeclaration(pDecl); }
HRESULT WrappedDirect3DDevice9Ex::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) { return real_->GetVertexDeclaration(ppDecl); }
HRESULT WrappedDirect3DDevice9Ex::SetFVF(DWORD FVF) { return real_->SetFVF(FVF); }
HRESULT WrappedDirect3DDevice9Ex::GetFVF(DWORD* pFVF) { return real_->GetFVF(pFVF); }
HRESULT WrappedDirect3DDevice9Ex::CreateVertexShader(const DWORD* pFunction, IDirect3DVertexShader9** ppShader) { return real_->CreateVertexShader(pFunction, ppShader); }
HRESULT WrappedDirect3DDevice9Ex::SetVertexShader(IDirect3DVertexShader9* pShader) { return real_->SetVertexShader(pShader); }
HRESULT WrappedDirect3DDevice9Ex::GetVertexShader(IDirect3DVertexShader9** ppShader) { return real_->GetVertexShader(ppShader); }
HRESULT WrappedDirect3DDevice9Ex::SetVertexShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount) { return real_->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT WrappedDirect3DDevice9Ex::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) { return real_->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT WrappedDirect3DDevice9Ex::SetVertexShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount) { return real_->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT WrappedDirect3DDevice9Ex::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) { return real_->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT WrappedDirect3DDevice9Ex::SetVertexShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount) { return real_->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT WrappedDirect3DDevice9Ex::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) { return real_->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT WrappedDirect3DDevice9Ex::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) { return real_->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride); }
HRESULT WrappedDirect3DDevice9Ex::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride) { return real_->GetStreamSource(StreamNumber, ppStreamData, pOffsetInBytes, pStride); }
HRESULT WrappedDirect3DDevice9Ex::SetStreamSourceFreq(UINT StreamNumber, UINT Setting) { return real_->SetStreamSourceFreq(StreamNumber, Setting); }
HRESULT WrappedDirect3DDevice9Ex::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting) { return real_->GetStreamSourceFreq(StreamNumber, pSetting); }
HRESULT WrappedDirect3DDevice9Ex::SetIndices(IDirect3DIndexBuffer9* pIndexData) { return real_->SetIndices(pIndexData); }
HRESULT WrappedDirect3DDevice9Ex::GetIndices(IDirect3DIndexBuffer9** ppIndexData) { return real_->GetIndices(ppIndexData); }
HRESULT WrappedDirect3DDevice9Ex::CreatePixelShader(const DWORD* pFunction, IDirect3DPixelShader9** ppShader) { return real_->CreatePixelShader(pFunction, ppShader); }
HRESULT WrappedDirect3DDevice9Ex::SetPixelShader(IDirect3DPixelShader9* pShader) { return real_->SetPixelShader(pShader); }
HRESULT WrappedDirect3DDevice9Ex::GetPixelShader(IDirect3DPixelShader9** ppShader) { return real_->GetPixelShader(ppShader); }
HRESULT WrappedDirect3DDevice9Ex::SetPixelShaderConstantF(UINT StartRegister, const float* pConstantData, UINT Vector4fCount) { return real_->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT WrappedDirect3DDevice9Ex::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) { return real_->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
HRESULT WrappedDirect3DDevice9Ex::SetPixelShaderConstantI(UINT StartRegister, const int* pConstantData, UINT Vector4iCount) { return real_->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT WrappedDirect3DDevice9Ex::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) { return real_->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount); }
HRESULT WrappedDirect3DDevice9Ex::SetPixelShaderConstantB(UINT StartRegister, const BOOL* pConstantData, UINT BoolCount) { return real_->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT WrappedDirect3DDevice9Ex::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) { return real_->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount); }
HRESULT WrappedDirect3DDevice9Ex::DrawRectPatch(UINT Handle, const float* pNumSegs, const D3DRECTPATCH_INFO* pRectPatchInfo) { return real_->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo); }
HRESULT WrappedDirect3DDevice9Ex::DrawTriPatch(UINT Handle, const float* pNumSegs, const D3DTRIPATCH_INFO* pTriPatchInfo) { return real_->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo); }
HRESULT WrappedDirect3DDevice9Ex::DeletePatch(UINT Handle) { return real_->DeletePatch(Handle); }
HRESULT WrappedDirect3DDevice9Ex::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) { return real_->CreateQuery(Type, ppQuery); }

HRESULT WrappedDirect3DDevice9Ex::SetConvolutionMonoKernel(UINT width, UINT height, float* rows, float* columns) { return real_->SetConvolutionMonoKernel(width, height, rows, columns); }
HRESULT WrappedDirect3DDevice9Ex::ComposeRects(IDirect3DSurface9* pSrc, IDirect3DSurface9* pDst, IDirect3DVertexBuffer9* pSrcRectDescs, UINT NumRects, IDirect3DVertexBuffer9* pDstRectDescs, D3DCOMPOSERECTSOP Operation, int Xoffset, int Yoffset) { return real_->ComposeRects(pSrc, pDst, pSrcRectDescs, NumRects, pDstRectDescs, Operation, Xoffset, Yoffset); }
HRESULT WrappedDirect3DDevice9Ex::PresentEx(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags) { return real_->PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags); }
HRESULT WrappedDirect3DDevice9Ex::GetGPUThreadPriority(INT* pPriority) { return real_->GetGPUThreadPriority(pPriority); }
HRESULT WrappedDirect3DDevice9Ex::SetGPUThreadPriority(INT Priority) { return real_->SetGPUThreadPriority(Priority); }
HRESULT WrappedDirect3DDevice9Ex::WaitForVBlank(UINT iSwapChain) { return real_->WaitForVBlank(iSwapChain); }
HRESULT WrappedDirect3DDevice9Ex::CheckResourceResidency(IDirect3DResource9** pResourceArray, UINT32 NumResources) { return real_->CheckResourceResidency(pResourceArray, NumResources); }
HRESULT WrappedDirect3DDevice9Ex::SetMaximumFrameLatency(UINT MaxLatency) { return real_->SetMaximumFrameLatency(MaxLatency); }
HRESULT WrappedDirect3DDevice9Ex::GetMaximumFrameLatency(UINT* pMaxLatency) { return real_->GetMaximumFrameLatency(pMaxLatency); }
HRESULT WrappedDirect3DDevice9Ex::CheckDeviceState(HWND hDestinationWindow) { return real_->CheckDeviceState(hDestinationWindow); }
HRESULT WrappedDirect3DDevice9Ex::CreateRenderTargetEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) { return real_->CreateRenderTargetEx(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle, Usage); }
HRESULT WrappedDirect3DDevice9Ex::CreateOffscreenPlainSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) { return real_->CreateOffscreenPlainSurfaceEx(Width, Height, Format, Pool, ppSurface, pSharedHandle, Usage); }
HRESULT WrappedDirect3DDevice9Ex::CreateDepthStencilSurfaceEx(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle, DWORD Usage) { return real_->CreateDepthStencilSurfaceEx(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle, Usage); }
HRESULT WrappedDirect3DDevice9Ex::ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode) { return real_->ResetEx(pPresentationParameters, pFullscreenDisplayMode); }
HRESULT WrappedDirect3DDevice9Ex::GetDisplayModeEx(UINT iSwapChain, D3DDISPLAYMODEEX* pMode, D3DDISPLAYROTATION* pRotation) { return real_->GetDisplayModeEx(iSwapChain, pMode, pRotation); }
