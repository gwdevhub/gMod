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
#include "uMod_ID3D10Device.h"

#ifndef PRE_MESSAGE
#define PRE_MESSAGE "uMod_ID3D10Device"
#endif


uMod_ID3D10Device::uMod_ID3D10Device( ID3D10Device* pOriginal, uMod_TextureServer* server)
{
  Message( PRE_MESSAGE "::" PRE_MESSAGE "( %lu, %lu): %lu\n", pOriginal, server, this);
  m_ID3D10Device = pOriginal;
  uMod_Server = server;
  RefCounter = 1;
}

uMod_ID3D10Device::~uMod_ID3D10Device()
{
  Message( PRE_MESSAGE "::~" PRE_MESSAGE "(): %lu\n", this);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::QueryInterface( REFIID riid, void** ppvObject)
{
  *ppvObject = NULL;
  Message( PRE_MESSAGE "::QueryInterface(): %lu\n", this);
  HRESULT hRes = m_ID3D10Device->QueryInterface(riid, ppvObject);

  if (ppvObject!=NULL)
  {if (*ppvObject == m_ID3D10Device)// && (riid==IID_ID3D10Device || riid==IID_ID3D10Device1))
  {
    RefCounter++; //increasing our counter
    //*ppvObject = this;
    Message("hit: %lu, %lu, %lu,   %lu  %lu\n", riid, IID_ID3D10Device, IID_ID3D10Device1, *ppvObject, this)
  }}

  return (hRes);
}

ULONG STDMETHODCALLTYPE uMod_ID3D10Device::AddRef(void)
{
  Message( PRE_MESSAGE "::AddRef(): %lu\n", this);
  RefCounter++;
  return m_ID3D10Device->AddRef();
}

ULONG STDMETHODCALLTYPE uMod_ID3D10Device::Release( void)
{
  Message( PRE_MESSAGE "::Release(): (%d) %lu\n", RefCounter, this);
  if (--RefCounter==0) //if our counter drops to zero, the real device will be deleted, so we clean up before
   {
     // we must not release the fake textures, cause they are released if the target textures are released
     // and the target textures are released by the game.
   }

  // call original routine
  ULONG count = m_ID3D10Device->Release();

  // in case no further Ref is there, the Original Object has deleted itself
  if (count == 0)
  {
    delete(this);
  }

  return(count);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::VSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D10Buffer *const *ppConstantBuffers)
{
  return m_ID3D10Device->VSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::PSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView *const *ppShaderResourceViews)
{
  return m_ID3D10Device->PSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::PSSetShader( ID3D10PixelShader *pPixelShader)
{
  return m_ID3D10Device->PSSetShader( pPixelShader);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::PSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D10SamplerState *const *ppSamplers)
{
  return m_ID3D10Device->PSSetSamplers( StartSlot, NumSamplers, ppSamplers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::VSSetShader( ID3D10VertexShader *pVertexShader)
{
  return m_ID3D10Device->VSSetShader( pVertexShader);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::DrawIndexed( UINT IndexCount,UINT StartIndexLocation, INT BaseVertexLocation)
{
  return m_ID3D10Device->DrawIndexed( IndexCount, StartIndexLocation, BaseVertexLocation);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::Draw( UINT VertexCount, UINT StartVertexLocation)
{
  return m_ID3D10Device->Draw( VertexCount, StartVertexLocation);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::PSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D10Buffer *const *ppConstantBuffers)
{
  return m_ID3D10Device->PSSetConstantBuffers( StartSlot, NumBuffers,  ppConstantBuffers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::IASetInputLayout( ID3D10InputLayout *pInputLayout)
{
  return m_ID3D10Device->IASetInputLayout( pInputLayout);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::IASetVertexBuffers( UINT StartSlot, UINT NumBuffers, ID3D10Buffer *const *ppVertexBuffers, const UINT *pStrides, const UINT *pOffsets)
{
  return m_ID3D10Device->IASetVertexBuffers( StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::IASetIndexBuffer( ID3D10Buffer *pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
{
  return m_ID3D10Device->IASetIndexBuffer( pIndexBuffer, Format, Offset);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
  return m_ID3D10Device->DrawIndexedInstanced( IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::DrawInstanced( UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
  return m_ID3D10Device->DrawInstanced( VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::GSSetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D10Buffer *const *ppConstantBuffers)
{
  return m_ID3D10Device->GSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers);
}
void STDMETHODCALLTYPE uMod_ID3D10Device::GSSetShader( ID3D10GeometryShader *pShader)
{
  return m_ID3D10Device->GSSetShader( pShader);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY Topology)
{
  return m_ID3D10Device->IASetPrimitiveTopology( Topology);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::VSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView *const *ppShaderResourceViews)
{
  return m_ID3D10Device->VSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::VSSetSamplers( UINT StartSlot, UINT NumSamplers, ID3D10SamplerState *const *ppSamplers)
{
  return m_ID3D10Device->VSSetSamplers( StartSlot, NumSamplers, ppSamplers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::SetPredication( ID3D10Predicate *pPredicate, BOOL PredicateValue)
{
  return m_ID3D10Device->SetPredication( pPredicate, PredicateValue);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::GSSetShaderResources( UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView *const *ppShaderResourceViews)
{
  return m_ID3D10Device->GSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::GSSetSamplers(UINT StartSlot, UINT NumSamplers, ID3D10SamplerState *const *ppSamplers)
{
  return m_ID3D10Device->GSSetSamplers( StartSlot, NumSamplers, ppSamplers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::OMSetRenderTargets( UINT NumViews, ID3D10RenderTargetView *const *ppRenderTargetViews, ID3D10DepthStencilView *pDepthStencilView)
{
  return m_ID3D10Device->OMSetRenderTargets( NumViews, ppRenderTargetViews, pDepthStencilView);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::OMSetBlendState( ID3D10BlendState *pBlendState, const FLOAT BlendFactor[ 4 ], UINT SampleMask)
{
  return m_ID3D10Device->OMSetBlendState( pBlendState, BlendFactor, SampleMask);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::OMSetDepthStencilState( ID3D10DepthStencilState *pDepthStencilState, UINT StencilRef)
{
  return m_ID3D10Device->OMSetDepthStencilState( pDepthStencilState, StencilRef);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::SOSetTargets(UINT NumBuffers, ID3D10Buffer *const *ppSOTargets, const UINT *pOffsets)
{
  return m_ID3D10Device->SOSetTargets( NumBuffers, ppSOTargets, pOffsets);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::DrawAuto(void)
{
  return m_ID3D10Device->DrawAuto();
}

void STDMETHODCALLTYPE uMod_ID3D10Device::RSSetState( ID3D10RasterizerState *pRasterizerState)
{
  return m_ID3D10Device->RSSetState( pRasterizerState);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::RSSetViewports( UINT NumViewports, const D3D10_VIEWPORT *pViewports)
{
  return m_ID3D10Device->RSSetViewports( NumViewports, pViewports);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::RSSetScissorRects( UINT NumRects, const D3D10_RECT *pRects)
{
  return m_ID3D10Device->RSSetScissorRects( NumRects, pRects);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::CopySubresourceRegion( ID3D10Resource *pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ, ID3D10Resource *pSrcResource, UINT SrcSubresource, const D3D10_BOX *pSrcBox)
{
  return m_ID3D10Device->CopySubresourceRegion( pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::CopyResource( ID3D10Resource *pDstResource, ID3D10Resource *pSrcResource)
{
  return m_ID3D10Device->CopyResource( pDstResource, pSrcResource);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::UpdateSubresource( ID3D10Resource *pDstResource, UINT DstSubresource, const D3D10_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch)
{
  return m_ID3D10Device->UpdateSubresource( pDstResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::ClearRenderTargetView( ID3D10RenderTargetView *pRenderTargetView, const FLOAT ColorRGBA[ 4 ])
{
  return m_ID3D10Device->ClearRenderTargetView( pRenderTargetView, ColorRGBA);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::ClearDepthStencilView( ID3D10DepthStencilView *pDepthStencilView, UINT ClearFlags, FLOAT Depth, UINT8 Stencil)
{
  return m_ID3D10Device->ClearDepthStencilView( pDepthStencilView, ClearFlags, Depth, Stencil);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::GenerateMips( ID3D10ShaderResourceView *pShaderResourceView)
{
  return m_ID3D10Device->GenerateMips( pShaderResourceView);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::ResolveSubresource( ID3D10Resource *pDstResource, UINT DstSubresource, ID3D10Resource *pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format)
{
  return m_ID3D10Device->ResolveSubresource( pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::VSGetConstantBuffers( UINT StartSlot,UINT NumBuffers, ID3D10Buffer **ppConstantBuffers)
{
  return m_ID3D10Device->VSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::PSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView **ppShaderResourceViews)
{
  return m_ID3D10Device->PSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::PSGetShader( ID3D10PixelShader **ppPixelShader)
{
  return m_ID3D10Device->PSGetShader( ppPixelShader);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::PSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D10SamplerState **ppSamplers)
{
  return m_ID3D10Device->PSGetSamplers( StartSlot, NumSamplers, ppSamplers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::VSGetShader( ID3D10VertexShader **ppVertexShader)
{
  return m_ID3D10Device->VSGetShader( ppVertexShader);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::PSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D10Buffer **ppConstantBuffers)
{
  return m_ID3D10Device->PSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::IAGetInputLayout( ID3D10InputLayout **ppInputLayout)
{
  return m_ID3D10Device->IAGetInputLayout( ppInputLayout);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::IAGetVertexBuffers( UINT StartSlot, UINT NumBuffers, ID3D10Buffer **ppVertexBuffers, UINT *pStrides, UINT *pOffsets)
{
  return m_ID3D10Device->IAGetVertexBuffers( StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::IAGetIndexBuffer( ID3D10Buffer **pIndexBuffer, DXGI_FORMAT *Format, UINT *Offset)
{
  return m_ID3D10Device->IAGetIndexBuffer( pIndexBuffer, Format, Offset);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::GSGetConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D10Buffer **ppConstantBuffers)
{
  return m_ID3D10Device->GSGetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::GSGetShader( ID3D10GeometryShader **ppGeometryShader)
{
  return m_ID3D10Device->GSGetShader( ppGeometryShader);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::IAGetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY *pTopology)
{
  return m_ID3D10Device->IAGetPrimitiveTopology( pTopology);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::VSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView **ppShaderResourceViews)
{
  return m_ID3D10Device->VSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::VSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D10SamplerState **ppSamplers)
{
  return m_ID3D10Device->VSGetSamplers( StartSlot, NumSamplers, ppSamplers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::GetPredication( ID3D10Predicate **ppPredicate, BOOL *pPredicateValue)
{
  return m_ID3D10Device->GetPredication( ppPredicate, pPredicateValue);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::GSGetShaderResources( UINT StartSlot, UINT NumViews, ID3D10ShaderResourceView **ppShaderResourceViews)
{
  return m_ID3D10Device->GSGetShaderResources( StartSlot, NumViews, ppShaderResourceViews);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::GSGetSamplers( UINT StartSlot, UINT NumSamplers, ID3D10SamplerState **ppSamplers)
{
  return m_ID3D10Device->GSGetSamplers( StartSlot, NumSamplers, ppSamplers);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::OMGetRenderTargets( UINT NumViews, ID3D10RenderTargetView **ppRenderTargetViews, ID3D10DepthStencilView **ppDepthStencilView)
{
  return m_ID3D10Device->OMGetRenderTargets( NumViews, ppRenderTargetViews, ppDepthStencilView);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::OMGetBlendState( ID3D10BlendState **ppBlendState, FLOAT BlendFactor[ 4 ], UINT *pSampleMask)
{
  return m_ID3D10Device->OMGetBlendState( ppBlendState, BlendFactor, pSampleMask);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::OMGetDepthStencilState( ID3D10DepthStencilState **ppDepthStencilState, UINT *pStencilRef)
{
  return m_ID3D10Device->OMGetDepthStencilState( ppDepthStencilState, pStencilRef);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::SOGetTargets( UINT NumBuffers, ID3D10Buffer **ppSOTargets, UINT *pOffsets)
{
  return m_ID3D10Device->SOGetTargets( NumBuffers, ppSOTargets, pOffsets);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::RSGetState( ID3D10RasterizerState **ppRasterizerState)
{
  return m_ID3D10Device->RSGetState( ppRasterizerState);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::RSGetViewports( UINT *NumViewports, D3D10_VIEWPORT *pViewports)
{
  return m_ID3D10Device->RSGetViewports( NumViewports, pViewports);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::RSGetScissorRects( UINT *NumRects, D3D10_RECT *pRects)
{
  return m_ID3D10Device->RSGetScissorRects( NumRects, pRects);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::GetDeviceRemovedReason(void)
{
  return m_ID3D10Device->GetDeviceRemovedReason();
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::SetExceptionMode( UINT RaiseFlags)
{
  return m_ID3D10Device->SetExceptionMode( RaiseFlags);
}

UINT STDMETHODCALLTYPE uMod_ID3D10Device::GetExceptionMode(void)
{
  return m_ID3D10Device->GetExceptionMode();
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::GetPrivateData( REFGUID guid, UINT *pDataSize, void *pData)
{
  return m_ID3D10Device->GetPrivateData( guid, pDataSize, pData);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::SetPrivateData( REFGUID guid, UINT DataSize, const void *pData)
{
  return m_ID3D10Device->SetPrivateData( guid, DataSize, pData);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::SetPrivateDataInterface( REFGUID guid, const IUnknown *pData)
{
  return m_ID3D10Device->SetPrivateDataInterface( guid, pData);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::ClearState( void)
{
  return m_ID3D10Device->ClearState();
}

void STDMETHODCALLTYPE uMod_ID3D10Device::Flush( void)
{
  return m_ID3D10Device->Flush();
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateBuffer( const D3D10_BUFFER_DESC *pDesc, const D3D10_SUBRESOURCE_DATA *pInitialData, ID3D10Buffer **ppBuffer)
{
  return m_ID3D10Device->CreateBuffer( pDesc, pInitialData, ppBuffer);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateTexture1D( const D3D10_TEXTURE1D_DESC *pDesc, const D3D10_SUBRESOURCE_DATA *pInitialData, ID3D10Texture1D **ppTexture1D)
{
  HRESULT hRes = m_ID3D10Device->CreateTexture1D( pDesc, pInitialData, ppTexture1D);
  if( SUCCEEDED( hRes ) )
  {
    *ppTexture1D = new uMod_ID3D10Texture1D( *ppTexture1D);
  }
  return hRes;
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateTexture2D( const D3D10_TEXTURE2D_DESC *pDesc, const D3D10_SUBRESOURCE_DATA *pInitialData, ID3D10Texture2D **ppTexture2D)
{
  HRESULT hRes = m_ID3D10Device->CreateTexture2D( pDesc, pInitialData, ppTexture2D);
  if( SUCCEEDED( hRes ) )
  {
    *ppTexture2D = new uMod_ID3D10Texture2D( *ppTexture2D);
  }
  return hRes;
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateTexture3D( const D3D10_TEXTURE3D_DESC *pDesc, const D3D10_SUBRESOURCE_DATA *pInitialData, ID3D10Texture3D **ppTexture3D)
{
  HRESULT hRes = m_ID3D10Device->CreateTexture3D( pDesc, pInitialData, ppTexture3D);
  if( SUCCEEDED( hRes ) )
  {
    *ppTexture3D = new uMod_ID3D10Texture3D( *ppTexture3D);
  }
  return hRes;
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateShaderResourceView( ID3D10Resource *pResource, const D3D10_SHADER_RESOURCE_VIEW_DESC *pDesc, ID3D10ShaderResourceView **ppSRView)
{
  return m_ID3D10Device->CreateShaderResourceView( pResource, pDesc, ppSRView);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateRenderTargetView( ID3D10Resource *pResource, const D3D10_RENDER_TARGET_VIEW_DESC *pDesc, ID3D10RenderTargetView **ppRTView)
{
  return m_ID3D10Device->CreateRenderTargetView( pResource, pDesc, ppRTView);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateDepthStencilView( ID3D10Resource *pResource, const D3D10_DEPTH_STENCIL_VIEW_DESC *pDesc, ID3D10DepthStencilView **ppDepthStencilView)
{
  return m_ID3D10Device->CreateDepthStencilView( pResource, pDesc, ppDepthStencilView);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateInputLayout( const D3D10_INPUT_ELEMENT_DESC *pInputElementDescs, UINT NumElements, const void *pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D10InputLayout **ppInputLayout)
{
  return m_ID3D10Device->CreateInputLayout( pInputElementDescs,  NumElements, pShaderBytecodeWithInputSignature,  BytecodeLength, ppInputLayout);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateVertexShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10VertexShader **ppVertexShader)
{
  return m_ID3D10Device->CreateVertexShader( pShaderBytecode,  BytecodeLength, ppVertexShader);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateGeometryShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10GeometryShader **ppGeometryShader)
{
  return m_ID3D10Device->CreateGeometryShader( pShaderBytecode, BytecodeLength, ppGeometryShader);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateGeometryShaderWithStreamOutput( const void *pShaderBytecode, SIZE_T BytecodeLength, const D3D10_SO_DECLARATION_ENTRY *pSODeclaration, UINT NumEntries, UINT OutputStreamStride, ID3D10GeometryShader **ppGeometryShader)
{
  return m_ID3D10Device->CreateGeometryShaderWithStreamOutput( pShaderBytecode, BytecodeLength, pSODeclaration, NumEntries, OutputStreamStride, ppGeometryShader);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreatePixelShader( const void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10PixelShader **ppPixelShader)
{
  return m_ID3D10Device->CreatePixelShader( pShaderBytecode, BytecodeLength, ppPixelShader);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateBlendState( const D3D10_BLEND_DESC *pBlendStateDesc, ID3D10BlendState **ppBlendState)
{
  return m_ID3D10Device->CreateBlendState( pBlendStateDesc, ppBlendState);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateDepthStencilState( const D3D10_DEPTH_STENCIL_DESC *pDepthStencilDesc, ID3D10DepthStencilState **ppDepthStencilState)
{
  return m_ID3D10Device->CreateDepthStencilState( pDepthStencilDesc, ppDepthStencilState);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateRasterizerState( const D3D10_RASTERIZER_DESC *pRasterizerDesc, ID3D10RasterizerState **ppRasterizerState)
{
  return m_ID3D10Device->CreateRasterizerState( pRasterizerDesc, ppRasterizerState);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateSamplerState( const D3D10_SAMPLER_DESC *pSamplerDesc, ID3D10SamplerState **ppSamplerState)
{
  return m_ID3D10Device->CreateSamplerState( pSamplerDesc, ppSamplerState);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateQuery( const D3D10_QUERY_DESC *pQueryDesc, ID3D10Query **ppQuery)
{
  return m_ID3D10Device->CreateQuery( pQueryDesc, ppQuery);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreatePredicate( const D3D10_QUERY_DESC *pPredicateDesc, ID3D10Predicate **ppPredicate)
{
  return m_ID3D10Device->CreatePredicate( pPredicateDesc, ppPredicate);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CreateCounter( const D3D10_COUNTER_DESC *pCounterDesc, ID3D10Counter **ppCounter)
{
  return m_ID3D10Device->CreateCounter( pCounterDesc, ppCounter);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CheckFormatSupport( DXGI_FORMAT Format, UINT *pFormatSupport)
{
  return m_ID3D10Device->CheckFormatSupport( Format, pFormatSupport);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CheckMultisampleQualityLevels( DXGI_FORMAT Format, UINT SampleCount, UINT *pNumQualityLevels)
{
  return m_ID3D10Device->CheckMultisampleQualityLevels( Format, SampleCount, pNumQualityLevels);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::CheckCounterInfo( D3D10_COUNTER_INFO *pCounterInfo)
{
  return m_ID3D10Device->CheckCounterInfo( pCounterInfo);
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::CheckCounter( const D3D10_COUNTER_DESC *pDesc, D3D10_COUNTER_TYPE *pType, UINT *pActiveCounters, LPSTR szName, UINT *pNameLength, LPSTR szUnits, UINT *pUnitsLength, LPSTR szDescription, UINT *pDescriptionLength)
{
  return m_ID3D10Device->CheckCounter( pDesc, pType, pActiveCounters, szName, pNameLength, szUnits, pUnitsLength, szDescription, pDescriptionLength);
}

UINT STDMETHODCALLTYPE uMod_ID3D10Device::GetCreationFlags( void)
{
  return m_ID3D10Device->GetCreationFlags();
}

HRESULT STDMETHODCALLTYPE uMod_ID3D10Device::OpenSharedResource( HANDLE hResource, REFIID ReturnedInterface, void **ppResource)
{
  return m_ID3D10Device->OpenSharedResource( hResource, ReturnedInterface, ppResource);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::SetTextFilterSize( UINT Width, UINT Height)
{
  return m_ID3D10Device->SetTextFilterSize( Width, Height);
}

void STDMETHODCALLTYPE uMod_ID3D10Device::GetTextFilterSize( UINT *pWidth, UINT *pHeight)
{
  return m_ID3D10Device->GetTextFilterSize( pWidth, pHeight);
}

