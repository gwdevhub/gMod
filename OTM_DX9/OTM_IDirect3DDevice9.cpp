/*
This file is part of OpenTexMod.


OpenTexMod is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenTexMod is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "OTM_Main.h"



int OTM_IDirect3DDevice9::CreateSingleTexture(void)
{
  if (SingleTexture==NULL) //Create green texture
  {
    if( D3D_OK != CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, (IDirect3DTexture9**) &SingleTexture, NULL)) return (RETURN_TEXTURE_NOT_LOADED);
    LastCreatedTexture = NULL;
    SingleTexture->FAKE = true;
    SingleTexture->Reference = -2;

    OTM_Client->TextureColour;
    TextureColour = OTM_Client->TextureColour;
    D3DLOCKED_RECT d3dlr;
    IDirect3DTexture9 *pD3Dtex = SingleTexture->m_D3Dtex;

    if (D3D_OK!=pD3Dtex->LockRect(0, &d3dlr, 0, 0))
    {
      SingleTexture->Release();
      SingleTexture=NULL;
      return (RETURN_TEXTURE_NOT_LOADED);
    }
    DWORD *pDst = (DWORD*)d3dlr.pBits;

    for (int xy=0; xy < 8*8; xy++) *(pDst++) = TextureColour;
    pD3Dtex->UnlockRect(0);
  }
  return (RETURN_OK);
}

OTM_IDirect3DDevice9::OTM_IDirect3DDevice9(IDirect3DDevice9* pOriginal, OTM_TextureServer* server)
{
  Message("OTM_IDirect3DDevice9( %lu, %lu): %lu\n", pOriginal, server, this);
  OTM_Server = server;
  OTM_Client = new OTM_TextureClient(  OTM_Server, this);
  LastCreatedTexture = NULL;
	m_pIDirect3DDevice9 = pOriginal; // store the pointer to original object
  TextureColour = D3DCOLOR_ARGB(255,0,255,0);

  CounterSaveSingleTexture = -1;
  SingleTexture = NULL;
  OSD_Font = NULL;
  FontColour = D3DCOLOR_ARGB(255,255,0,0);
  //Message("end OTM_IDirect3DDevice9( %lu, %lu): %lu\n", pOriginal, server, this);
}

OTM_IDirect3DDevice9::~OTM_IDirect3DDevice9(void)
{
}

HRESULT OTM_IDirect3DDevice9::QueryInterface (REFIID riid, void** ppvObj)
{
	 // check if original dll can provide interface. then send *our* address
	*ppvObj = NULL;

	HRESULT hRes = m_pIDirect3DDevice9->QueryInterface(riid, ppvObj); 

	if (hRes == NOERROR)
	{
		*ppvObj = this;
	}
	
	return (hRes);
}

ULONG OTM_IDirect3DDevice9::AddRef(void)
{
	 return (m_pIDirect3DDevice9->AddRef());
}

ULONG OTM_IDirect3DDevice9::Release(void)
{
	// ATTENTION: This is a booby-trap ! Watch out !
	// If we create our own sprites, surfaces, etc. (thus increasing the ref counter
	// by external action), we need to delete that objects before calling the original
	// Release function	
	

	 // release/delete own objects
	 // .....
	
	// Calling original function now
	ULONG count = m_pIDirect3DDevice9->Release();
  Message("%lu = IDirect3DDevice9::Release(): %lu\n", count, this);
		
	if (count==0u)
	{
	  // now, the Original Object has deleted itself, so do we here
	  if (OTM_Client!=NULL)
	  {
	    //OTM_Client->ReleaseAllFakeTexture();
	    if (SingleTexture!=NULL) SingleTexture->Release();
	    if (OSD_Font!=NULL) OSD_Font->Release();
	    delete OTM_Client;
	    OTM_Client = NULL;
	  }

	  //if (gl_pTextureHandler!=NULL) gl_pTextureHandler->ReleaseAllFakeTexture(); // release all fake textures
		//gl_pIDirect3DDevice9 = NULL;
		delete(this);  // destructor will be called automatically
	}

	return (count);
}

HRESULT OTM_IDirect3DDevice9::TestCooperativeLevel(void)
{
	 return(m_pIDirect3DDevice9->TestCooperativeLevel());
}

UINT OTM_IDirect3DDevice9::GetAvailableTextureMem(void)
{
	 return(m_pIDirect3DDevice9->GetAvailableTextureMem());
}

HRESULT OTM_IDirect3DDevice9::EvictManagedResources(void)
{
	 return(m_pIDirect3DDevice9->EvictManagedResources());
}

HRESULT OTM_IDirect3DDevice9::GetDirect3D(IDirect3D9** ppD3D9)
{
	 return(m_pIDirect3DDevice9->GetDirect3D(ppD3D9));
}

HRESULT OTM_IDirect3DDevice9::GetDeviceCaps(D3DCAPS9* pCaps)
{
	 return(m_pIDirect3DDevice9->GetDeviceCaps(pCaps));
}

HRESULT OTM_IDirect3DDevice9::GetDisplayMode(UINT iSwapChain,D3DDISPLAYMODE* pMode)
{
	 return(m_pIDirect3DDevice9->GetDisplayMode(iSwapChain, pMode));
}

HRESULT OTM_IDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	 return(m_pIDirect3DDevice9->GetCreationParameters(pParameters));
}

HRESULT OTM_IDirect3DDevice9::SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap)
{
	 return(m_pIDirect3DDevice9->SetCursorProperties(XHotSpot,YHotSpot,pCursorBitmap));
}

void	 OTM_IDirect3DDevice9::SetCursorPosition(int X,int Y,DWORD Flags)
{
	 m_pIDirect3DDevice9->SetCursorPosition(X,Y,Flags);
}

BOOL	 OTM_IDirect3DDevice9::ShowCursor(BOOL bShow)
{
	 return(m_pIDirect3DDevice9->ShowCursor(bShow));
}

HRESULT OTM_IDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain)
{
	 return(m_pIDirect3DDevice9->CreateAdditionalSwapChain(pPresentationParameters,pSwapChain));
}

HRESULT OTM_IDirect3DDevice9::GetSwapChain(UINT iSwapChain,IDirect3DSwapChain9** pSwapChain)
{
	 return(m_pIDirect3DDevice9->GetSwapChain(iSwapChain,pSwapChain));
}

UINT	 OTM_IDirect3DDevice9::GetNumberOfSwapChains(void)
{
	 return(m_pIDirect3DDevice9->GetNumberOfSwapChains());
}

HRESULT OTM_IDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
  if(OSD_Font!=NULL) {OSD_Font->Release(); OSD_Font=NULL;} //the game will crashes if the font is not released before the game is minimized!
  return(m_pIDirect3DDevice9->Reset(pPresentationParameters));
}

HRESULT OTM_IDirect3DDevice9::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	 // we may want to draw own things here before flipping surfaces
	 // ... draw own stuff ...
	//this->ShowWeAreHere();
	 
	 // call original routine
	HRESULT hres = m_pIDirect3DDevice9->Present( pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);

	return (hres);
}

HRESULT OTM_IDirect3DDevice9::GetBackBuffer(UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer)
{
	 return(m_pIDirect3DDevice9->GetBackBuffer(iSwapChain,iBackBuffer,Type,ppBackBuffer));
}

HRESULT OTM_IDirect3DDevice9::GetRasterStatus(UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus)
{
	 return(m_pIDirect3DDevice9->GetRasterStatus(iSwapChain,pRasterStatus));
}

HRESULT OTM_IDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
	 return(m_pIDirect3DDevice9->SetDialogBoxMode(bEnableDialogs));
}

void	 OTM_IDirect3DDevice9::SetGammaRamp(UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp)
{
	 (m_pIDirect3DDevice9->SetGammaRamp(iSwapChain,Flags,pRamp));
}

void	 OTM_IDirect3DDevice9::GetGammaRamp(UINT iSwapChain,D3DGAMMARAMP* pRamp)
{
  (m_pIDirect3DDevice9->GetGammaRamp(iSwapChain,pRamp));
}

HRESULT OTM_IDirect3DDevice9::CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle)
{
  //Message("CreateTexture(  ): %lu\n", this);
	HRESULT ret = (m_pIDirect3DDevice9->CreateTexture(Width,Height,Levels,Usage,Format,Pool,ppTexture,pSharedHandle));
	if(ret != D3D_OK) return (ret);

	OTM_IDirect3DTexture9 *texture  = new OTM_IDirect3DTexture9(ppTexture, this, Width, Height, Format);
	if (texture) *ppTexture = texture;

	
	if (LastCreatedTexture!=NULL)
	{
	  if ( OTM_Client!=NULL)  OTM_Client->AddTexture( LastCreatedTexture);
	}

	LastCreatedTexture = texture;

  //Message("end CreateTexture(  ): %lu\n", this);
  return (ret);
}

HRESULT OTM_IDirect3DDevice9::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle)
{
	 return (m_pIDirect3DDevice9->CreateVolumeTexture(Width,Height,Depth,Levels,Usage,Format,Pool,ppVolumeTexture,pSharedHandle));
}

HRESULT OTM_IDirect3DDevice9::CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle)
{
	 return(m_pIDirect3DDevice9->CreateCubeTexture(EdgeLength,Levels,Usage,Format,Pool,ppCubeTexture,pSharedHandle));
}

HRESULT OTM_IDirect3DDevice9::CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle)
{
	 return(m_pIDirect3DDevice9->CreateVertexBuffer(Length,Usage,FVF,Pool,ppVertexBuffer,pSharedHandle));
}

HRESULT OTM_IDirect3DDevice9::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle)
{
	 return(m_pIDirect3DDevice9->CreateIndexBuffer(Length,Usage,Format,Pool,ppIndexBuffer,pSharedHandle));
}

HRESULT OTM_IDirect3DDevice9::CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
	 return(m_pIDirect3DDevice9->CreateRenderTarget(Width,Height,Format,MultiSample,MultisampleQuality,Lockable,ppSurface,pSharedHandle));
}

HRESULT OTM_IDirect3DDevice9::CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
	 return(m_pIDirect3DDevice9->CreateDepthStencilSurface(Width,Height,Format,MultiSample,MultisampleQuality,Discard,ppSurface,pSharedHandle));
}

HRESULT OTM_IDirect3DDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint)
{
	 return(m_pIDirect3DDevice9->UpdateSurface(pSourceSurface,pSourceRect,pDestinationSurface,pDestPoint));
}

HRESULT OTM_IDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture)
{
	 return(m_pIDirect3DDevice9->UpdateTexture(pSourceTexture,pDestinationTexture));
}

HRESULT OTM_IDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface)
{
	 return(m_pIDirect3DDevice9->GetRenderTargetData(pRenderTarget,pDestSurface));
}

HRESULT OTM_IDirect3DDevice9::GetFrontBufferData(UINT iSwapChain,IDirect3DSurface9* pDestSurface)
{
	 return(m_pIDirect3DDevice9->GetFrontBufferData(iSwapChain,pDestSurface));
}

HRESULT OTM_IDirect3DDevice9::StretchRect(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter)
{
	 return(m_pIDirect3DDevice9->StretchRect(pSourceSurface,pSourceRect,pDestSurface,pDestRect,Filter));
}

HRESULT OTM_IDirect3DDevice9::ColorFill(IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color)
{
	 return(m_pIDirect3DDevice9->ColorFill(pSurface,pRect,color));
}

HRESULT OTM_IDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
	 return(m_pIDirect3DDevice9->CreateOffscreenPlainSurface(Width,Height,Format,Pool,ppSurface,pSharedHandle));
}

HRESULT OTM_IDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget)
{
	 return(m_pIDirect3DDevice9->SetRenderTarget(RenderTargetIndex,pRenderTarget));
}

HRESULT OTM_IDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget)
{
	 return(m_pIDirect3DDevice9->GetRenderTarget(RenderTargetIndex,ppRenderTarget));
}

HRESULT OTM_IDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	 return(m_pIDirect3DDevice9->SetDepthStencilSurface(pNewZStencil));
}

HRESULT OTM_IDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
	 return(m_pIDirect3DDevice9->GetDepthStencilSurface(ppZStencilSurface));
}

HRESULT OTM_IDirect3DDevice9::BeginScene(void)
{
  //Message("BeginScene(): %lu\n", this);
  if ( OTM_Client!=NULL)
  {
    OTM_Client->MergeUpdate();
    if (LastCreatedTexture!=NULL)
    {
      OTM_Client->AddTexture( LastCreatedTexture);
      LastCreatedTexture = NULL;
    }
    if (OTM_Client->BoolSaveSingleTexture)
    {
      if (SingleTexture==NULL) CreateSingleTexture();

      if (SingleTexture!=NULL)
      {
        if (TextureColour!=OTM_Client->TextureColour)
        {
          D3DLOCKED_RECT d3dlr;
          IDirect3DTexture9 *pD3Dtex;
          if (SingleTexture->CrossRef_D3Dtex==NULL) pD3Dtex = SingleTexture->m_D3Dtex;
          else pD3Dtex = SingleTexture->CrossRef_D3Dtex->m_D3Dtex;

          if (D3D_OK==pD3Dtex->LockRect(0, &d3dlr, 0, 0))
          {
            DWORD *pDst = (DWORD*)d3dlr.pBits;
            TextureColour = OTM_Client->TextureColour;
            for (int xy=0; xy < 8*8; xy++) *(pDst++) = TextureColour;
            pD3Dtex->UnlockRect(0);
          }
        }
        if (OTM_Client->KeyBack>0 && (GetAsyncKeyState( OTM_Client->KeyBack ) &1) )
        {
          UnswitchTextures( SingleTexture);
          if (CounterSaveSingleTexture<0) CounterSaveSingleTexture = 0; //first initialization
          else if (--CounterSaveSingleTexture<0) CounterSaveSingleTexture = OTM_Client->OriginalTextures.GetNumber() - 1;
          if (CounterSaveSingleTexture >= 0) SwitchTextures( SingleTexture,  OTM_Client->OriginalTextures[CounterSaveSingleTexture]);
        }
        if (OTM_Client->KeySave>0 && (GetAsyncKeyState( OTM_Client->KeySave ) &1) )
        {
          SingleTexture->Hash = SingleTexture->CrossRef_D3Dtex->Hash;
          OTM_Client->SaveTexture( SingleTexture);
          SingleTexture->Hash = 0u;
        }
        if (OTM_Client->KeyNext>0 && (GetAsyncKeyState( OTM_Client->KeyNext ) &1) )
        {
          UnswitchTextures( SingleTexture);
          if (CounterSaveSingleTexture<0) CounterSaveSingleTexture = 0; //first initialization
          else if (++CounterSaveSingleTexture>=OTM_Client->OriginalTextures.GetNumber()) CounterSaveSingleTexture = 0;
          if (CounterSaveSingleTexture < OTM_Client->OriginalTextures.GetNumber()) SwitchTextures( SingleTexture,  OTM_Client->OriginalTextures[CounterSaveSingleTexture]);
        }
      }
    }
  }
  Return_BeginScene = m_pIDirect3DDevice9->BeginScene();
  return (Return_BeginScene);
  //return (m_pIDirect3DDevice9->BeginScene());
}

HRESULT OTM_IDirect3DDevice9::EndScene(void)
{
  if ( Return_BeginScene==D3D_OK && OTM_Client!=NULL && OTM_Client->BoolSaveSingleTexture && SingleTexture!=NULL && SingleTexture->CrossRef_D3Dtex!=NULL)
  {
    if (OSD_Font==NULL || FontColour != OTM_Client->FontColour)
    {
      FontColour = OTM_Client->FontColour;
      if (OSD_Font!=NULL) OSD_Font->Release();
      if (D3D_OK!=D3DXCreateFontA( m_pIDirect3DDevice9, 20, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &OSD_Font))
      {
        OSD_Font=NULL;
        return(m_pIDirect3DDevice9->EndScene());
      }
    }

    char buffer[100];
    sprintf_s( buffer, 100, "Actual texture: %4d (1..%d): %#lX", CounterSaveSingleTexture+1, OTM_Client->OriginalTextures.GetNumber(), SingleTexture->CrossRef_D3Dtex->Hash);
    D3DVIEWPORT9 viewport;
    GetViewport( &viewport);
    RECT rct;
    rct.left=viewport.X + 10;
    rct.right=0;
    rct.top=viewport.Y + 10;
    rct.bottom=0;
     OSD_Font->DrawTextA(NULL, buffer, -1, &rct, DT_NOCLIP, FontColour);

  }
  //Message("EndScene(): %lu\n", this);
  return(m_pIDirect3DDevice9->EndScene());
}

HRESULT OTM_IDirect3DDevice9::Clear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
	 return(m_pIDirect3DDevice9->Clear(Count,pRects,Flags,Color,Z,Stencil));
}

HRESULT OTM_IDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
	 return(m_pIDirect3DDevice9->SetTransform(State,pMatrix));
}

HRESULT OTM_IDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
	 return(m_pIDirect3DDevice9->GetTransform(State,pMatrix));
}

HRESULT OTM_IDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
	 return(m_pIDirect3DDevice9->MultiplyTransform(State,pMatrix));
}

HRESULT OTM_IDirect3DDevice9::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
	 return(m_pIDirect3DDevice9->SetViewport(pViewport));
}

HRESULT OTM_IDirect3DDevice9::GetViewport(D3DVIEWPORT9* pViewport)
{
	 return(m_pIDirect3DDevice9->GetViewport(pViewport));
}

HRESULT OTM_IDirect3DDevice9::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
	 return(m_pIDirect3DDevice9->SetMaterial(pMaterial));
}

HRESULT OTM_IDirect3DDevice9::GetMaterial(D3DMATERIAL9* pMaterial)
{
	 return(m_pIDirect3DDevice9->GetMaterial(pMaterial));
}

HRESULT OTM_IDirect3DDevice9::SetLight(DWORD Index,CONST D3DLIGHT9* pLight)
{
	 return(m_pIDirect3DDevice9->SetLight(Index,pLight));
}

HRESULT OTM_IDirect3DDevice9::GetLight(DWORD Index,D3DLIGHT9* pLight)
{
	 return(m_pIDirect3DDevice9->GetLight(Index,pLight));
}

HRESULT OTM_IDirect3DDevice9::LightEnable(DWORD Index,BOOL Enable)
{
	 return(m_pIDirect3DDevice9->LightEnable(Index,Enable));
}

HRESULT OTM_IDirect3DDevice9::GetLightEnable(DWORD Index,BOOL* pEnable)
{
	 return(m_pIDirect3DDevice9->GetLightEnable(Index, pEnable));
}

HRESULT OTM_IDirect3DDevice9::SetClipPlane(DWORD Index,CONST float* pPlane)
{
	 return(m_pIDirect3DDevice9->SetClipPlane(Index, pPlane));
}

HRESULT OTM_IDirect3DDevice9::GetClipPlane(DWORD Index,float* pPlane)
{
	 return(m_pIDirect3DDevice9->GetClipPlane(Index,pPlane));
}

HRESULT OTM_IDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
	 return(m_pIDirect3DDevice9->SetRenderState(State, Value));
}

HRESULT OTM_IDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State,DWORD* pValue)
{
	 return(m_pIDirect3DDevice9->GetRenderState(State, pValue));
}

HRESULT OTM_IDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB)
{
	 return(m_pIDirect3DDevice9->CreateStateBlock(Type,ppSB));
}

HRESULT OTM_IDirect3DDevice9::BeginStateBlock(void)
{
	 return(m_pIDirect3DDevice9->BeginStateBlock());
}

HRESULT OTM_IDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
	 return(m_pIDirect3DDevice9->EndStateBlock(ppSB));
}

HRESULT OTM_IDirect3DDevice9::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
	 return(m_pIDirect3DDevice9->SetClipStatus(pClipStatus));
}

HRESULT OTM_IDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
	 return(m_pIDirect3DDevice9->GetClipStatus( pClipStatus));
}

HRESULT OTM_IDirect3DDevice9::GetTexture(DWORD Stage,IDirect3DBaseTexture9** ppTexture)
{
	 return(m_pIDirect3DDevice9->GetTexture(Stage,ppTexture));
}

HRESULT OTM_IDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{

	IDirect3DDevice9 *dev = NULL;
  if (pTexture != NULL && ((OTM_IDirect3DTexture9*)(pTexture))->GetDevice(&dev) == D3D_OK)
  {
		if(dev == this)	return (m_pIDirect3DDevice9->SetTexture(Stage, ((OTM_IDirect3DTexture9*)(pTexture))->m_D3Dtex));
  }
  return (m_pIDirect3DDevice9->SetTexture(Stage, pTexture));
}

HRESULT OTM_IDirect3DDevice9::GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue)
{
	 return(m_pIDirect3DDevice9->GetTextureStageState(Stage,Type, pValue));
}

HRESULT OTM_IDirect3DDevice9::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
	 return(m_pIDirect3DDevice9->SetTextureStageState(Stage,Type,Value));
}

HRESULT OTM_IDirect3DDevice9::GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue)
{
	 return(m_pIDirect3DDevice9->GetSamplerState(Sampler,Type, pValue));
}

HRESULT OTM_IDirect3DDevice9::SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
	 return(m_pIDirect3DDevice9->SetSamplerState(Sampler,Type,Value));
}

HRESULT OTM_IDirect3DDevice9::ValidateDevice(DWORD* pNumPasses)
{
	 return(m_pIDirect3DDevice9->ValidateDevice( pNumPasses));
}

HRESULT OTM_IDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber,CONST PALETTEENTRY* pEntries)
{
	 return(m_pIDirect3DDevice9->SetPaletteEntries(PaletteNumber, pEntries));
}

HRESULT OTM_IDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries)
{
	 return(m_pIDirect3DDevice9->GetPaletteEntries(PaletteNumber, pEntries));
}

HRESULT OTM_IDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
	 return(m_pIDirect3DDevice9->SetCurrentTexturePalette(PaletteNumber));
}

HRESULT OTM_IDirect3DDevice9::GetCurrentTexturePalette(UINT *PaletteNumber)
{
	 return(m_pIDirect3DDevice9->GetCurrentTexturePalette(PaletteNumber));
}

HRESULT OTM_IDirect3DDevice9::SetScissorRect(CONST RECT* pRect)
{
	 return(m_pIDirect3DDevice9->SetScissorRect( pRect));
}

HRESULT OTM_IDirect3DDevice9::GetScissorRect( RECT* pRect)
{
	 return(m_pIDirect3DDevice9->GetScissorRect( pRect));
}

HRESULT OTM_IDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
	 return(m_pIDirect3DDevice9->SetSoftwareVertexProcessing(bSoftware));
}

BOOL	 OTM_IDirect3DDevice9::GetSoftwareVertexProcessing(void)
{
	 return(m_pIDirect3DDevice9->GetSoftwareVertexProcessing());
}

HRESULT OTM_IDirect3DDevice9::SetNPatchMode(float nSegments)
{
	 return(m_pIDirect3DDevice9->SetNPatchMode(nSegments));
}

float	OTM_IDirect3DDevice9::GetNPatchMode(void)
{
	 return(m_pIDirect3DDevice9->GetNPatchMode());
}

HRESULT OTM_IDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
	 return(m_pIDirect3DDevice9->DrawPrimitive(PrimitiveType,StartVertex,PrimitiveCount));
}

HRESULT OTM_IDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount)
{
	 return(m_pIDirect3DDevice9->DrawIndexedPrimitive(PrimitiveType,BaseVertexIndex,MinVertexIndex,NumVertices,startIndex,primCount));
}

HRESULT OTM_IDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	 return(m_pIDirect3DDevice9->DrawPrimitiveUP(PrimitiveType,PrimitiveCount,pVertexStreamZeroData,VertexStreamZeroStride));
}

HRESULT OTM_IDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
	 return(m_pIDirect3DDevice9->DrawIndexedPrimitiveUP(PrimitiveType,MinVertexIndex,NumVertices,PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData,VertexStreamZeroStride));
}

HRESULT OTM_IDirect3DDevice9::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags)
{
	 return(m_pIDirect3DDevice9->ProcessVertices( SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags));
}

HRESULT OTM_IDirect3DDevice9::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl)
{
	 return(m_pIDirect3DDevice9->CreateVertexDeclaration( pVertexElements,ppDecl));
}

HRESULT OTM_IDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
	 return(m_pIDirect3DDevice9->SetVertexDeclaration(pDecl));
}

HRESULT OTM_IDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
	 return(m_pIDirect3DDevice9->GetVertexDeclaration(ppDecl));
}

HRESULT OTM_IDirect3DDevice9::SetFVF(DWORD FVF)
{
	 return(m_pIDirect3DDevice9->SetFVF(FVF));
}

HRESULT OTM_IDirect3DDevice9::GetFVF(DWORD* pFVF)
{
	 return(m_pIDirect3DDevice9->GetFVF(pFVF));
}

HRESULT OTM_IDirect3DDevice9::CreateVertexShader(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader)
{
	 return(m_pIDirect3DDevice9->CreateVertexShader(pFunction,ppShader));
}

HRESULT OTM_IDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9* pShader)
{
	 return(m_pIDirect3DDevice9->SetVertexShader(pShader));
}

HRESULT OTM_IDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
	 return(m_pIDirect3DDevice9->GetVertexShader(ppShader));
}

HRESULT OTM_IDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
{
	 return(m_pIDirect3DDevice9->SetVertexShaderConstantF(StartRegister,pConstantData, Vector4fCount));
}

HRESULT OTM_IDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount)
{
	 return(m_pIDirect3DDevice9->GetVertexShaderConstantF(StartRegister,pConstantData,Vector4fCount));
}

HRESULT OTM_IDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
{
	 return(m_pIDirect3DDevice9->SetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount));
}

HRESULT OTM_IDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount)
{
	 return(m_pIDirect3DDevice9->GetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount));
}

HRESULT OTM_IDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
{
	 return(m_pIDirect3DDevice9->SetVertexShaderConstantB(StartRegister,pConstantData,BoolCount));
}

HRESULT OTM_IDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
{
	 return(m_pIDirect3DDevice9->GetVertexShaderConstantB(StartRegister,pConstantData,BoolCount));
}

HRESULT OTM_IDirect3DDevice9::SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride)
{
	 return(m_pIDirect3DDevice9->SetStreamSource(StreamNumber,pStreamData,OffsetInBytes,Stride));
}

HRESULT OTM_IDirect3DDevice9::GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* OffsetInBytes,UINT* pStride)
{
	 return(m_pIDirect3DDevice9->GetStreamSource(StreamNumber,ppStreamData,OffsetInBytes,pStride));
}

HRESULT OTM_IDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber,UINT Divider)
{
	 return(m_pIDirect3DDevice9->SetStreamSourceFreq(StreamNumber,Divider));
}

HRESULT OTM_IDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber,UINT* Divider)
{
	 return(m_pIDirect3DDevice9->GetStreamSourceFreq(StreamNumber,Divider));
}

HRESULT OTM_IDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
	 return(m_pIDirect3DDevice9->SetIndices(pIndexData));
}

HRESULT OTM_IDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
	 return(m_pIDirect3DDevice9->GetIndices(ppIndexData));
}

HRESULT OTM_IDirect3DDevice9::CreatePixelShader(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader)
{
	 return(m_pIDirect3DDevice9->CreatePixelShader(pFunction,ppShader));
}

HRESULT OTM_IDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9* pShader)
{
	 return(m_pIDirect3DDevice9->SetPixelShader(pShader));
}

HRESULT OTM_IDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
	 return(m_pIDirect3DDevice9->GetPixelShader(ppShader));
}

HRESULT OTM_IDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
{
	 return(m_pIDirect3DDevice9->SetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount));
}

HRESULT OTM_IDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount)
{
	 return(m_pIDirect3DDevice9->GetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount));
}

HRESULT OTM_IDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
{
	 return(m_pIDirect3DDevice9->SetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount));
}

HRESULT OTM_IDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount)
{
	 return(m_pIDirect3DDevice9->GetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount));
}

HRESULT OTM_IDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
{
	 return(m_pIDirect3DDevice9->SetPixelShaderConstantB(StartRegister,pConstantData,BoolCount));
}

HRESULT OTM_IDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
{
	 return(m_pIDirect3DDevice9->GetPixelShaderConstantB(StartRegister,pConstantData,BoolCount));
}

HRESULT OTM_IDirect3DDevice9::DrawRectPatch(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	 return(m_pIDirect3DDevice9->DrawRectPatch(Handle,pNumSegs, pRectPatchInfo));
}

HRESULT OTM_IDirect3DDevice9::DrawTriPatch(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	 return(m_pIDirect3DDevice9->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo));
}

HRESULT OTM_IDirect3DDevice9::DeletePatch(UINT Handle)
{
	 return(m_pIDirect3DDevice9->DeletePatch(Handle));
}

HRESULT OTM_IDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery)
{
  return(m_pIDirect3DDevice9->CreateQuery(Type,ppQuery));
}


