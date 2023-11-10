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

#ifndef RETURN_QueryInterface
#define RETURN_QueryInterface 0x01000000L
#endif

#ifndef PRE_MESSAGE
#define PRE_MESSAGE "uMod_IDirect3DDevice9"
#endif


int uMod_IDirect3DDevice9::CreateSingleTexture(void)
{
  if (SingleTexture!=NULL && SingleVolumeTexture!=NULL && SingleCubeTexture!=NULL && TextureColour==uMod_Client->TextureColour) return (RETURN_OK);
  TextureColour = uMod_Client->TextureColour;
  if (SingleTexture==NULL) //create texture
  {
    if( D3D_OK != CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, (IDirect3DTexture9**) &SingleTexture, NULL))
    {
      Message( PRE_MESSAGE "::CreateSingleTexture(): CreateTexture Failed\n");
      SingleTexture = NULL;
      return (RETURN_TEXTURE_NOT_LOADED);
    }
    LastCreatedTexture = NULL; // set LastCreatedTexture to NULL, cause LastCreatedTexture is equal SingleTexture
    SingleTexture->FAKE = true; //this is no texture created from by game
    SingleTexture->Reference = -2;
  }

  {
    D3DLOCKED_RECT d3dlr;
    IDirect3DTexture9 *pD3Dtex = SingleTexture->m_D3Dtex;

    if (D3D_OK!=pD3Dtex->LockRect(0, &d3dlr, 0, 0))
    {
      Message( PRE_MESSAGE "::CreateSingleTexture(): LockRect Failed\n");
      SingleTexture->Release();
      SingleTexture=NULL;
      return (RETURN_TEXTURE_NOT_LOADED);
    }
    DWORD *pDst = (DWORD*)d3dlr.pBits;

    for (int i=0; i<8*8; i++) pDst[i] = TextureColour;
    pD3Dtex->UnlockRect(0);
  }

  if (SingleVolumeTexture==NULL) //create texture
  {
    if( D3D_OK != CreateVolumeTexture(8, 8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, (IDirect3DVolumeTexture9**) &SingleVolumeTexture, NULL))
    {
      Message( PRE_MESSAGE "::CreateSingleTexture(): CreateVolumeTexture Failed\n");
      SingleVolumeTexture = NULL;
      return (RETURN_TEXTURE_NOT_LOADED);
    }
    LastCreatedVolumeTexture = NULL; // set LastCreatedTexture to NULL, cause LastCreatedTexture is equal SingleTexture
    SingleVolumeTexture->FAKE = true; //this is no texture created from by game
    SingleVolumeTexture->Reference = -2;
  }

  {
    D3DLOCKED_BOX d3dlr;
    IDirect3DVolumeTexture9 *pD3Dtex = SingleVolumeTexture->m_D3Dtex;
    //LockBox)(UINT Level, D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox,
    if (D3D_OK!=pD3Dtex->LockBox(0, &d3dlr, 0, 0))
    {
      Message( PRE_MESSAGE "::CreateSingleTexture(): LockBox Failed\n");
      SingleVolumeTexture->Release();
      SingleVolumeTexture=NULL;
      return (RETURN_TEXTURE_NOT_LOADED);
    }
    DWORD *pDst = (DWORD*)d3dlr.pBits;

    for (int i=0; i<8*8*8; i++) pDst[i] = TextureColour;
    pD3Dtex->UnlockBox(0);
  }
  if (SingleCubeTexture==NULL) //create texture
  {
    if( D3D_OK != CreateCubeTexture(8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, (IDirect3DCubeTexture9**) &SingleCubeTexture, NULL))
    {
      Message( PRE_MESSAGE "::CreateSingleTexture(): CreateCubeTexture Failed\n");
      SingleCubeTexture = NULL;
      return (RETURN_TEXTURE_NOT_LOADED);
    }
    LastCreatedCubeTexture = NULL; // set LastCreatedTexture to NULL, cause LastCreatedTexture is equal SingleTexture
    SingleCubeTexture->FAKE = true; //this is no texture created from by game
    SingleCubeTexture->Reference = -2;
  }

  {
    D3DLOCKED_RECT d3dlr;
    IDirect3DCubeTexture9 *pD3Dtex = SingleCubeTexture->m_D3Dtex;

    for (int c=0; c<6; c++)
    {
      if (D3D_OK!=pD3Dtex->LockRect( (D3DCUBEMAP_FACES) c, 0, &d3dlr, 0, 0))
      {
        Message( PRE_MESSAGE "::CreateSingleTexture(): LockRect (Cube) Failed\n");
        SingleCubeTexture->Release();
        SingleCubeTexture=NULL;
        return (RETURN_TEXTURE_NOT_LOADED);
      }
      DWORD *pDst = (DWORD*)d3dlr.pBits;

      for (int i=0; i<8*8; i++) pDst[i] = TextureColour;
      pD3Dtex->UnlockRect((D3DCUBEMAP_FACES)c, 0);
    }
  }

  return (RETURN_OK);
}

uMod_IDirect3DDevice9::uMod_IDirect3DDevice9( IDirect3DDevice9* pOriginal, uMod_TextureServer* server, int back_buffer_count)
{
  Message( PRE_MESSAGE "::" PRE_MESSAGE  "( %lu, %lu): %lu\n", pOriginal, server, this);

  BackBufferCount = back_buffer_count;
  NormalRendering = true;

  uMod_Server = server;
  uMod_Client = new uMod_TextureClient(  uMod_Server, this); //get a new texture client for this device

  LastCreatedTexture = NULL;
  LastCreatedVolumeTexture = NULL;
  LastCreatedCubeTexture = NULL;
	m_pIDirect3DDevice9 = pOriginal; // store the pointer to original object
  TextureColour = D3DCOLOR_ARGB(255,0,255,0);

  CounterSaveSingleTexture = -20;

  SingleTextureMod = 0;
  SingleTexture = NULL;
  SingleVolumeTexture = NULL;
  SingleCubeTexture = NULL;
  OSD_Font = NULL;
  uMod_Reference = 1;
}

uMod_IDirect3DDevice9::~uMod_IDirect3DDevice9(void)
{
  Message( PRE_MESSAGE "::~" PRE_MESSAGE "(): %lu\n", this);
}

HRESULT uMod_IDirect3DDevice9::QueryInterface(REFIID riid, void** ppvObj)
{
  // check if original dll can provide interface. then send *our* address
	if (riid==IID_IDirect3DTexture9)
	{
	  // This function should never be called with IDirect3DTexture9 by the game
    *ppvObj = this;
	  return (RETURN_QueryInterface);
	}

	*ppvObj = NULL;
  Message( PRE_MESSAGE "::QueryInterface(): %lu\n", this);
	HRESULT hRes = m_pIDirect3DDevice9->QueryInterface(riid, ppvObj); 

	if (*ppvObj == m_pIDirect3DDevice9)
	{
	  uMod_Reference++; //increasing our counter
		*ppvObj = this;
	}
	
	return (hRes);
}

ULONG uMod_IDirect3DDevice9::AddRef(void)
{
  uMod_Reference++; //increasing our counter
  Message("%lu = " PRE_MESSAGE "::AddRef(): %lu\n", uMod_Reference, this);
  return (m_pIDirect3DDevice9->AddRef());
}

ULONG uMod_IDirect3DDevice9::Release(void)
{
  if (--uMod_Reference==0) //if our counter drops to zero, the real device will be deleted, so we clean up before
  {
    // we must not release the fake textures, cause they are released if the target textures are released
    // and the target textures are released by the game.

    if (SingleTexture!=NULL) SingleTexture->Release(); //this is the only texture we must release by ourself
    if (SingleVolumeTexture!=NULL) SingleVolumeTexture->Release(); //this is the only texture we must release by ourself
    if (SingleCubeTexture!=NULL) SingleCubeTexture->Release(); //this is the only texture we must release by ourself
    if (OSD_Font!=NULL) OSD_Font->Release();

    if (uMod_Client!=NULL) delete uMod_Client; //must be deleted at the end, because other releases might call a function of this object

    uMod_Client = NULL;
    SingleTexture = NULL;
    OSD_Font = NULL;
  }

	ULONG count = m_pIDirect3DDevice9->Release();
  Message("%lu = " PRE_MESSAGE "::Release(): %lu\n", count, this);
  if (uMod_Reference!=count) //bug
  {
    Message("Error in " PRE_MESSAGE "::Release(): %lu!=%lu\n", uMod_Reference, count);
  }

	if (count==0u)  delete(this);
	return (count);
}

HRESULT uMod_IDirect3DDevice9::TestCooperativeLevel(void)
{
  return(m_pIDirect3DDevice9->TestCooperativeLevel());
}

UINT uMod_IDirect3DDevice9::GetAvailableTextureMem(void)
{
  return(m_pIDirect3DDevice9->GetAvailableTextureMem());
}

HRESULT uMod_IDirect3DDevice9::EvictManagedResources(void)
{
  return(m_pIDirect3DDevice9->EvictManagedResources());
}

HRESULT uMod_IDirect3DDevice9::GetDirect3D(IDirect3D9** ppD3D9)
{
  return(m_pIDirect3DDevice9->GetDirect3D(ppD3D9));
}

HRESULT uMod_IDirect3DDevice9::GetDeviceCaps(D3DCAPS9* pCaps)
{
  return(m_pIDirect3DDevice9->GetDeviceCaps(pCaps));
}

HRESULT uMod_IDirect3DDevice9::GetDisplayMode(UINT iSwapChain,D3DDISPLAYMODE* pMode)
{
  return(m_pIDirect3DDevice9->GetDisplayMode(iSwapChain, pMode));
}

HRESULT uMod_IDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
  return(m_pIDirect3DDevice9->GetCreationParameters(pParameters));
}

HRESULT uMod_IDirect3DDevice9::SetCursorProperties(UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap)
{
  return(m_pIDirect3DDevice9->SetCursorProperties(XHotSpot,YHotSpot,pCursorBitmap));
}

void uMod_IDirect3DDevice9::SetCursorPosition(int X,int Y,DWORD Flags)
{
	 m_pIDirect3DDevice9->SetCursorPosition(X,Y,Flags);
}

BOOL uMod_IDirect3DDevice9::ShowCursor(BOOL bShow)
{
  return(m_pIDirect3DDevice9->ShowCursor(bShow));
}

HRESULT uMod_IDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain)
{
  return(m_pIDirect3DDevice9->CreateAdditionalSwapChain(pPresentationParameters,pSwapChain));
}

HRESULT uMod_IDirect3DDevice9::GetSwapChain(UINT iSwapChain,IDirect3DSwapChain9** pSwapChain)
{
  return(m_pIDirect3DDevice9->GetSwapChain(iSwapChain,pSwapChain));
}

UINT uMod_IDirect3DDevice9::GetNumberOfSwapChains(void)
{
  return(m_pIDirect3DDevice9->GetNumberOfSwapChains());
}

HRESULT uMod_IDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
  if(OSD_Font!=NULL) {OSD_Font->Release(); OSD_Font=NULL;} //the game will crashes if the font is not released before the game is minimized!
  return(m_pIDirect3DDevice9->Reset(pPresentationParameters));
}

HRESULT uMod_IDirect3DDevice9::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	return (m_pIDirect3DDevice9->Present( pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion));
}

HRESULT uMod_IDirect3DDevice9::GetBackBuffer(UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer)
{
  return(m_pIDirect3DDevice9->GetBackBuffer(iSwapChain,iBackBuffer,Type,ppBackBuffer));
}

HRESULT uMod_IDirect3DDevice9::GetRasterStatus(UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus)
{
  return(m_pIDirect3DDevice9->GetRasterStatus(iSwapChain,pRasterStatus));
}

HRESULT uMod_IDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
  return(m_pIDirect3DDevice9->SetDialogBoxMode(bEnableDialogs));
}

void uMod_IDirect3DDevice9::SetGammaRamp(UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp)
{
  (m_pIDirect3DDevice9->SetGammaRamp(iSwapChain,Flags,pRamp));
}

void uMod_IDirect3DDevice9::GetGammaRamp(UINT iSwapChain,D3DGAMMARAMP* pRamp)
{
  (m_pIDirect3DDevice9->GetGammaRamp(iSwapChain,pRamp));
}

HRESULT uMod_IDirect3DDevice9::CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle)
{
  //create real texture
  //Message("uMod_IDirect3DDevice9::CreateTexture()\n");
	HRESULT ret = (m_pIDirect3DDevice9->CreateTexture(Width,Height,Levels,Usage,Format,Pool,ppTexture,pSharedHandle));
	if(ret != D3D_OK) return (ret);

	//create fake texture
	uMod_IDirect3DTexture9 *texture  = new uMod_IDirect3DTexture9( ppTexture, this);
	if (texture) *ppTexture = texture;
	
	if (LastCreatedTexture!=NULL) //if a texture was loaded before, hopefully this texture contains now the data, so we can add it
	{
	  if ( uMod_Client!=NULL) uMod_Client->AddTexture( LastCreatedTexture);
	}
	LastCreatedTexture = texture;
  return (ret);
}

HRESULT uMod_IDirect3DDevice9::CreateVolumeTexture(UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle)
{
  //create real texture
  //Message("uMod_IDirect3DDevice9::CreateVolumeTexture()\n");
  HRESULT ret = (m_pIDirect3DDevice9->CreateVolumeTexture(Width,Height,Depth, Levels,Usage,Format,Pool,ppVolumeTexture,pSharedHandle));
  if(ret != D3D_OK) return (ret);

  //create fake texture
  uMod_IDirect3DVolumeTexture9 *texture  = new uMod_IDirect3DVolumeTexture9( ppVolumeTexture, this);
  if (texture) *ppVolumeTexture = texture;

  if (LastCreatedVolumeTexture!=NULL) //if a texture was loaded before, hopefully this texture contains now the data, so we can add it
  {
    if ( uMod_Client!=NULL) uMod_Client->AddTexture( LastCreatedVolumeTexture);
  }
  LastCreatedVolumeTexture = texture;
  return (ret);
}

HRESULT uMod_IDirect3DDevice9::CreateCubeTexture(UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle)
{
  //create real texture
  //Message("uMod_IDirect3DDevice9::CreateCubeTexture()\n");
  HRESULT ret = (m_pIDirect3DDevice9->CreateCubeTexture(EdgeLength, Levels,Usage,Format,Pool,ppCubeTexture,pSharedHandle));
  if(ret != D3D_OK) return (ret);

  //create fake texture
  uMod_IDirect3DCubeTexture9 *texture  = new uMod_IDirect3DCubeTexture9( ppCubeTexture, this);
  if (texture) *ppCubeTexture = texture;

  if (LastCreatedCubeTexture!=NULL) //if a texture was loaded before, hopefully this texture contains now the data, so we can add it
  {
    if ( uMod_Client!=NULL) uMod_Client->AddTexture( LastCreatedCubeTexture);
  }
  LastCreatedCubeTexture = texture;
  return (ret);
}

HRESULT uMod_IDirect3DDevice9::CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle)
{
  return(m_pIDirect3DDevice9->CreateVertexBuffer(Length,Usage,FVF,Pool,ppVertexBuffer,pSharedHandle));
}

HRESULT uMod_IDirect3DDevice9::CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle)
{
  return(m_pIDirect3DDevice9->CreateIndexBuffer(Length,Usage,Format,Pool,ppIndexBuffer,pSharedHandle));
}

HRESULT uMod_IDirect3DDevice9::CreateRenderTarget(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
  return(m_pIDirect3DDevice9->CreateRenderTarget(Width,Height,Format,MultiSample,MultisampleQuality,Lockable,ppSurface,pSharedHandle));
}

HRESULT uMod_IDirect3DDevice9::CreateDepthStencilSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
  return(m_pIDirect3DDevice9->CreateDepthStencilSurface(Width,Height,Format,MultiSample,MultisampleQuality,Discard,ppSurface,pSharedHandle));
}

HRESULT uMod_IDirect3DDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint)
{
  return(m_pIDirect3DDevice9->UpdateSurface(pSourceSurface,pSourceRect,pDestinationSurface,pDestPoint));
}

HRESULT uMod_IDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture)
{
  Message( PRE_MESSAGE "::UpdateTexture( %lu, %lu): %lu\n", pSourceTexture, pDestinationTexture, this);
  // we must pass the real texture objects


  uMod_IDirect3DTexture9* pSource = NULL;
  uMod_IDirect3DVolumeTexture9* pSourceVolume = NULL;
  uMod_IDirect3DCubeTexture9* pSourceCube = NULL;
  IDirect3DBaseTexture9* cpy;
  if( pSourceTexture != NULL )
  {
    long int ret = pSourceTexture->QueryInterface( IID_IDirect3D9, (void**) &cpy);
    switch (ret)
    {
      case 0x01000000L:
      {
        MyTypeHash hash;
        pSource = (uMod_IDirect3DTexture9*)(pSourceTexture);
        if (pSource->GetHash( hash) == RETURN_OK)
        {
          if (hash != pSource->Hash) // this hash has changed !!
          {
            pSource->Hash = hash;
            if (pSource->CrossRef_D3Dtex!=NULL) UnswitchTextures(pSource);
            uMod_Client->LookUpToMod( pSource);
          }
        }
        else if (pSource->CrossRef_D3Dtex!=NULL) UnswitchTextures(pSource); // we better unswitch

        // the source must be the original texture if not switched and the fake texture if it is switched
        if (pSource->CrossRef_D3Dtex!=NULL) pSourceTexture = pSource->CrossRef_D3Dtex->m_D3Dtex;
        else pSourceTexture = pSource->m_D3Dtex;
        break;
      }
      case 0x01000001L:
      {
        MyTypeHash hash;
        pSourceVolume = (uMod_IDirect3DVolumeTexture9*)(pSourceTexture);
        if (pSourceVolume->GetHash( hash) == RETURN_OK)
        {
          if (hash != pSourceVolume->Hash) // this hash has changed !!
          {
            pSourceVolume->Hash = hash;
            if (pSourceVolume->CrossRef_D3Dtex!=NULL) UnswitchTextures(pSourceVolume);
            uMod_Client->LookUpToMod( pSourceVolume);
          }
        }
        else if (pSourceVolume->CrossRef_D3Dtex!=NULL) UnswitchTextures(pSourceVolume); // we better unswitch

        // the source must be the original texture if not switched and the fake texture if it is switched
        if (pSourceVolume->CrossRef_D3Dtex!=NULL) pSourceTexture = pSourceVolume->CrossRef_D3Dtex->m_D3Dtex;
        else pSourceTexture = pSourceVolume->m_D3Dtex;
        break;
      }
      case 0x01000002L:
      {
        MyTypeHash hash;
        pSourceCube = (uMod_IDirect3DCubeTexture9*)(pSourceTexture);
        if (pSourceCube->GetHash( hash) == RETURN_OK)
        {
          if (hash != pSourceCube->Hash) // this hash has changed !!
          {
            pSourceCube->Hash = hash;
            if (pSourceCube->CrossRef_D3Dtex!=NULL) UnswitchTextures(pSourceCube);
            uMod_Client->LookUpToMod( pSourceCube);
          }
        }
        else if (pSourceCube->CrossRef_D3Dtex!=NULL) UnswitchTextures(pSourceCube); // we better unswitch

        // the source must be the original texture if not switched and the fake texture if it is switched
        if (pSourceCube->CrossRef_D3Dtex!=NULL) pSourceTexture = pSourceCube->CrossRef_D3Dtex->m_D3Dtex;
        else pSourceTexture = pSourceCube->m_D3Dtex;
        break;
      }
      default:
        break; // this is no fake texture and QueryInterface failed, because IDirect3DBaseTexture9 object cannot be a IDirect3D9 object ;)
    }
  }


  if (pDestinationTexture != NULL)
  {
    long int ret = pSourceTexture->QueryInterface( IID_IDirect3D9, (void**) &cpy);
    switch (ret)
    {
      case 0x01000000L:
      {
        uMod_IDirect3DTexture9* pDest = (uMod_IDirect3DTexture9*)(pDestinationTexture);

        if (pSource!=NULL && pDest->Hash!=pSource->Hash)
        {
          pDest->Hash = pSource->Hash; // take over the hash
          UnswitchTextures(pDest);
          if (pSource->CrossRef_D3Dtex!=NULL)
          {
            uMod_IDirect3DTexture9 *cpy = pSource->CrossRef_D3Dtex;
            UnswitchTextures(pSource);
            SwitchTextures( cpy, pDest);
          }
        }
        if (pDest->CrossRef_D3Dtex!=NULL) pDestinationTexture = pDest->CrossRef_D3Dtex->m_D3Dtex; // make sure to copy into the original texture
        else pDestinationTexture = pDest->m_D3Dtex;
        break;
      }
      case 0x01000001L:
      {
        uMod_IDirect3DVolumeTexture9* pDest = (uMod_IDirect3DVolumeTexture9*)(pDestinationTexture);

        if (pSourceVolume!=NULL && pDest->Hash!=pSourceVolume->Hash)
        {
          pDest->Hash = pSourceVolume->Hash; // take over the hash
          UnswitchTextures(pDest);
          if (pSourceVolume->CrossRef_D3Dtex!=NULL)
          {
            uMod_IDirect3DVolumeTexture9 *cpy = pSourceVolume->CrossRef_D3Dtex;
            UnswitchTextures(pSourceVolume);
            SwitchTextures( cpy, pDest);
          }
        }
        if (pDest->CrossRef_D3Dtex!=NULL) pDestinationTexture = pDest->CrossRef_D3Dtex->m_D3Dtex; // make sure to copy into the original texture
        else pDestinationTexture = pDest->m_D3Dtex;
        break;
      }
      case 0x01000002L:
      {
        uMod_IDirect3DCubeTexture9* pDest = (uMod_IDirect3DCubeTexture9*)(pDestinationTexture);

        if (pSourceCube!=NULL && pDest->Hash!=pSourceCube->Hash)
        {
          pDest->Hash = pSourceCube->Hash; // take over the hash
          UnswitchTextures(pDest);
          if (pSourceCube->CrossRef_D3Dtex!=NULL)
          {
            uMod_IDirect3DCubeTexture9 *cpy = pSourceCube->CrossRef_D3Dtex;
            UnswitchTextures(pSourceCube);
            SwitchTextures( cpy, pDest);
          }
        }
        if (pDest->CrossRef_D3Dtex!=NULL) pDestinationTexture = pDest->CrossRef_D3Dtex->m_D3Dtex; // make sure to copy into the original texture
        else pDestinationTexture = pDest->m_D3Dtex;
        break;
      }
    }
  }
	return(m_pIDirect3DDevice9->UpdateTexture(pSourceTexture,pDestinationTexture));
}

HRESULT uMod_IDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface)
{
  return(m_pIDirect3DDevice9->GetRenderTargetData(pRenderTarget,pDestSurface));
}

HRESULT uMod_IDirect3DDevice9::GetFrontBufferData(UINT iSwapChain,IDirect3DSurface9* pDestSurface)
{
  return(m_pIDirect3DDevice9->GetFrontBufferData(iSwapChain,pDestSurface));
}

HRESULT uMod_IDirect3DDevice9::StretchRect(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter)
{
  return(m_pIDirect3DDevice9->StretchRect(pSourceSurface,pSourceRect,pDestSurface,pDestRect,Filter));
}

HRESULT uMod_IDirect3DDevice9::ColorFill(IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color)
{
  return(m_pIDirect3DDevice9->ColorFill(pSurface,pRect,color));
}

HRESULT uMod_IDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle)
{
  return(m_pIDirect3DDevice9->CreateOffscreenPlainSurface(Width,Height,Format,Pool,ppSurface,pSharedHandle));
}

HRESULT uMod_IDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget)
{
  {
    IDirect3DSurface9 *back_buffer;
    NormalRendering = false;
    for (int i=0; !NormalRendering && i<BackBufferCount; i++)
    {
      m_pIDirect3DDevice9->GetBackBuffer( 0, i, D3DBACKBUFFER_TYPE_MONO, &back_buffer);
      if (back_buffer == pRenderTarget) NormalRendering = true;
      back_buffer->Release();
    }
  }
  return (m_pIDirect3DDevice9->SetRenderTarget(RenderTargetIndex,pRenderTarget));
}

HRESULT uMod_IDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget)
{
  return (m_pIDirect3DDevice9->GetRenderTarget(RenderTargetIndex,ppRenderTarget));
}

HRESULT uMod_IDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
  return(m_pIDirect3DDevice9->SetDepthStencilSurface(pNewZStencil));
}

HRESULT uMod_IDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
  return(m_pIDirect3DDevice9->GetDepthStencilSurface(ppZStencilSurface));
}

HRESULT uMod_IDirect3DDevice9::BeginScene(void)
{
  //if ( NormalRendering )
  {
    if (LastCreatedTexture!=NULL) // add the last created texture
    {
      uMod_Client->AddTexture( LastCreatedTexture);
    }
    if (LastCreatedVolumeTexture!=NULL) // add the last created texture
    {
      uMod_Client->AddTexture( LastCreatedVolumeTexture);
    }
    if (LastCreatedCubeTexture!=NULL) // add the last created texture
    {
      uMod_Client->AddTexture( LastCreatedCubeTexture);
    }
    uMod_Client->MergeUpdate(); // merge an update, if present

    if (uMod_Client->BoolSaveSingleTexture)
    {
      if (CreateSingleTexture()==0)
      {
        if (uMod_Client->KeyBack>0 && (GetAsyncKeyState( uMod_Client->KeyBack ) &1) ) //ask for the status of the back key
        {
          UnswitchTextures( SingleTexture); // can be called, even if texture is not switched
          UnswitchTextures( SingleVolumeTexture); // can be called, even if texture is not switched
          UnswitchTextures( SingleCubeTexture); // can be called, even if texture is not switched

          if (CounterSaveSingleTexture<-10) {CounterSaveSingleTexture = 0; SingleTextureMod=0;} //first initialization of the counter
          else if (--CounterSaveSingleTexture<0)
          {
            if (--SingleTextureMod<0) SingleTextureMod=2;
            switch (SingleTextureMod)
            {
              case 0:
                CounterSaveSingleTexture = uMod_Client->OriginalTextures.GetNumber() - 1;
                break;
              case 1:
                CounterSaveSingleTexture = uMod_Client->OriginalVolumeTextures.GetNumber() - 1;
                break;
              case 2:
                CounterSaveSingleTexture = uMod_Client->OriginalCubeTextures.GetNumber() - 1;
                break;
            }
          }

          if (CounterSaveSingleTexture >= 0)
          {
            switch (SingleTextureMod)
            {
              case 0:
                SwitchTextures( SingleTexture,  uMod_Client->OriginalTextures[CounterSaveSingleTexture]);
                SingleTexture->Hash = uMod_Client->OriginalTextures[CounterSaveSingleTexture]->Hash; //set the hash for the display
                break;
              case 1:
                SwitchTextures( SingleVolumeTexture,  uMod_Client->OriginalVolumeTextures[CounterSaveSingleTexture]);
                SingleVolumeTexture->Hash = uMod_Client->OriginalVolumeTextures[CounterSaveSingleTexture]->Hash; //set the hash for the display
                break;
              case 2:
                SwitchTextures( SingleCubeTexture,  uMod_Client->OriginalCubeTextures[CounterSaveSingleTexture]);
                SingleCubeTexture->Hash = uMod_Client->OriginalCubeTextures[CounterSaveSingleTexture]->Hash; //set the hash for the display
                break;
            }
          }
        }

        if (uMod_Client->KeySave>0 && (GetAsyncKeyState( uMod_Client->KeySave ) &1) ) //ask for the status of the save key
        {
          switch (SingleTextureMod)
          {
            case 0:
              uMod_Client->SaveTexture( SingleTexture); //after switching the SingleTexture holds the pointer to the original texture object
              break;
            case 1:
              uMod_Client->SaveTexture( SingleVolumeTexture); //after switching the SingleTexture holds the pointer to the original texture object
              break;
            case 2:
              uMod_Client->SaveTexture( SingleCubeTexture); //after switching the SingleTexture holds the pointer to the original texture object
              break;
          }
        }

        if (uMod_Client->KeyNext>0 && (GetAsyncKeyState( uMod_Client->KeyNext ) &1) ) //ask for the status of the next key
        {
          UnswitchTextures( SingleTexture); // can be called, even if texture is not switched
          UnswitchTextures( SingleVolumeTexture); // can be called, even if texture is not switched
          UnswitchTextures( SingleCubeTexture); // can be called, even if texture is not switched

          if (CounterSaveSingleTexture<-10)  {CounterSaveSingleTexture = 0; SingleTextureMod=0;} //first initialization of the counter
          else
          {
            int num = 0;
            switch (SingleTextureMod)
            {
              case 0:
                num = uMod_Client->OriginalTextures.GetNumber();
                break;
              case 1:
                num = uMod_Client->OriginalVolumeTextures.GetNumber();
                break;
              case 2:
                num = uMod_Client->OriginalCubeTextures.GetNumber();
                break;
            }
            if (++CounterSaveSingleTexture>=num)
            {
              if (++SingleTextureMod>2) SingleTextureMod=0;
              switch (SingleTextureMod)
              {
                case 0:
                  CounterSaveSingleTexture = uMod_Client->OriginalTextures.GetNumber()>0 ? 0 : - 1;
                  break;
                case 1:
                  CounterSaveSingleTexture = uMod_Client->OriginalVolumeTextures.GetNumber()>0 ? 0 : - 1;
                  break;
                case 2:
                  CounterSaveSingleTexture = uMod_Client->OriginalCubeTextures.GetNumber()>0 ? 0 : - 1;
                  break;
              }
            }
          }

          if (CounterSaveSingleTexture >= 0)
          {
            switch (SingleTextureMod)
            {
              case 0:
                SwitchTextures( SingleTexture,  uMod_Client->OriginalTextures[CounterSaveSingleTexture]);
                SingleTexture->Hash = uMod_Client->OriginalTextures[CounterSaveSingleTexture]->Hash; //set the hash for the display
                break;
              case 1:
                SwitchTextures( SingleVolumeTexture,  uMod_Client->OriginalVolumeTextures[CounterSaveSingleTexture]);
                SingleVolumeTexture->Hash = uMod_Client->OriginalVolumeTextures[CounterSaveSingleTexture]->Hash; //set the hash for the display
                break;
              case 2:
                SwitchTextures( SingleCubeTexture,  uMod_Client->OriginalCubeTextures[CounterSaveSingleTexture]);
                SingleCubeTexture->Hash = uMod_Client->OriginalCubeTextures[CounterSaveSingleTexture]->Hash; //set the hash for the display
                break;
            }
          }
        }
      }
    }
  }

  return (m_pIDirect3DDevice9->BeginScene());
}

HRESULT uMod_IDirect3DDevice9::EndScene(void)
{
  if ( NormalRendering && uMod_Client->BoolSaveSingleTexture && SingleTexture!=NULL && SingleVolumeTexture!=NULL && SingleCubeTexture!=NULL)
  {
    if (OSD_Font==NULL) // create the font
    {
      if (D3D_OK!=D3DXCreateFontA( m_pIDirect3DDevice9, 20, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &OSD_Font))
      {
        OSD_Font=NULL;
        return(m_pIDirect3DDevice9->EndScene());
      }
    }

    char buffer[100];
    buffer[0]=0;
    switch (SingleTextureMod)
    {
      case 0:
      {
        if (SingleTexture->CrossRef_D3Dtex!=NULL) sprintf_s( buffer, 100, "normal texture: %4d (1..%d): %#lX", CounterSaveSingleTexture+1, uMod_Client->OriginalTextures.GetNumber(), SingleTexture->Hash);
        else
        {
          if (uMod_Client->OriginalTextures.GetNumber()>0) sprintf_s( buffer, 100, "normal texture: nothing selected (1..%d)", uMod_Client->OriginalTextures.GetNumber());
          else sprintf_s( buffer, 100, "normal texture: nothing loaded");
        }
        break;
      }
      case 1:
      {
        if (SingleVolumeTexture->CrossRef_D3Dtex!=NULL) sprintf_s( buffer, 100, "volume texture: %4d (1..%d): %#lX", CounterSaveSingleTexture+1, uMod_Client->OriginalVolumeTextures.GetNumber(), SingleVolumeTexture->Hash);
        else
        {
          if (uMod_Client->OriginalVolumeTextures.GetNumber()>0) sprintf_s( buffer, 100, "volume texture: nothing selected (1..%d)", uMod_Client->OriginalVolumeTextures.GetNumber());
          else sprintf_s( buffer, 100, "volume texture: nothing loaded");
        }
        break;
      }
      case 2:
      {
        if (SingleCubeTexture->CrossRef_D3Dtex!=NULL) sprintf_s( buffer, 100, "cube texture: %4d (1..%d): %#lX", CounterSaveSingleTexture+1, uMod_Client->OriginalCubeTextures.GetNumber(), SingleCubeTexture->Hash);
        else
        {
          if (uMod_Client->OriginalCubeTextures.GetNumber()>0) sprintf_s( buffer, 100, "cube texture: nothing selected (1..%d)", uMod_Client->OriginalCubeTextures.GetNumber());
          else sprintf_s( buffer, 100, "cube   texture: nothing loaded");
        }
        break;
      }
    }
    D3DVIEWPORT9 viewport;
    GetViewport( &viewport);
    RECT rct;
    rct.left=viewport.X + 10;
    rct.right=0; //size of box is calculated automatically (DT_NOCLIP)
    rct.top=viewport.Y + 10;
    rct.bottom=0; //size of box is calculated automatically (DT_NOCLIP)
    OSD_Font->DrawTextA(NULL, buffer, -1, &rct, DT_NOCLIP, uMod_Client->FontColour);
  }
  return(m_pIDirect3DDevice9->EndScene());
}

HRESULT uMod_IDirect3DDevice9::Clear(DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
{
  return(m_pIDirect3DDevice9->Clear(Count,pRects,Flags,Color,Z,Stencil));
}

HRESULT uMod_IDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
  return(m_pIDirect3DDevice9->SetTransform(State,pMatrix));
}

HRESULT uMod_IDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix)
{
  return(m_pIDirect3DDevice9->GetTransform(State,pMatrix));
}

HRESULT uMod_IDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
  return(m_pIDirect3DDevice9->MultiplyTransform(State,pMatrix));
}

HRESULT uMod_IDirect3DDevice9::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
  return(m_pIDirect3DDevice9->SetViewport(pViewport));
}

HRESULT uMod_IDirect3DDevice9::GetViewport(D3DVIEWPORT9* pViewport)
{
  return(m_pIDirect3DDevice9->GetViewport(pViewport));
}

HRESULT uMod_IDirect3DDevice9::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
  return(m_pIDirect3DDevice9->SetMaterial(pMaterial));
}

HRESULT uMod_IDirect3DDevice9::GetMaterial(D3DMATERIAL9* pMaterial)
{
  return(m_pIDirect3DDevice9->GetMaterial(pMaterial));
}

HRESULT uMod_IDirect3DDevice9::SetLight(DWORD Index,CONST D3DLIGHT9* pLight)
{
  return(m_pIDirect3DDevice9->SetLight(Index,pLight));
}

HRESULT uMod_IDirect3DDevice9::GetLight(DWORD Index,D3DLIGHT9* pLight)
{
  return(m_pIDirect3DDevice9->GetLight(Index,pLight));
}

HRESULT uMod_IDirect3DDevice9::LightEnable(DWORD Index,BOOL Enable)
{
  return(m_pIDirect3DDevice9->LightEnable(Index,Enable));
}

HRESULT uMod_IDirect3DDevice9::GetLightEnable(DWORD Index,BOOL* pEnable)
{
  return(m_pIDirect3DDevice9->GetLightEnable(Index, pEnable));
}

HRESULT uMod_IDirect3DDevice9::SetClipPlane(DWORD Index,CONST float* pPlane)
{
  return(m_pIDirect3DDevice9->SetClipPlane(Index, pPlane));
}

HRESULT uMod_IDirect3DDevice9::GetClipPlane(DWORD Index,float* pPlane)
{
  return(m_pIDirect3DDevice9->GetClipPlane(Index,pPlane));
}

HRESULT uMod_IDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
  return(m_pIDirect3DDevice9->SetRenderState(State, Value));
}

HRESULT uMod_IDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State,DWORD* pValue)
{
  return(m_pIDirect3DDevice9->GetRenderState(State, pValue));
}

HRESULT uMod_IDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB)
{
  return(m_pIDirect3DDevice9->CreateStateBlock(Type,ppSB));
}

HRESULT uMod_IDirect3DDevice9::BeginStateBlock(void)
{
  return(m_pIDirect3DDevice9->BeginStateBlock());
}

HRESULT uMod_IDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
  return(m_pIDirect3DDevice9->EndStateBlock(ppSB));
}

HRESULT uMod_IDirect3DDevice9::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
  return(m_pIDirect3DDevice9->SetClipStatus(pClipStatus));
}

HRESULT uMod_IDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
  return(m_pIDirect3DDevice9->GetClipStatus( pClipStatus));
}

HRESULT uMod_IDirect3DDevice9::GetTexture(DWORD Stage,IDirect3DBaseTexture9** ppTexture)
{
  return(m_pIDirect3DDevice9->GetTexture(Stage,ppTexture));
}

HRESULT uMod_IDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
  // we must pass the real texture objects
  // if (dev != this) this texture was not initialized through our device and is thus no fake texture object

	//IDirect3DDevice9 *dev = NULL;
  IDirect3DBaseTexture9* cpy;
	if( pTexture != NULL )
	{
	  long int ret = pTexture->QueryInterface( IID_IDirect3D9, (void**) &cpy);
	  switch (ret)
	  {
	    case 0x01000000L:
	      pTexture = ((uMod_IDirect3DTexture9*)(pTexture))->m_D3Dtex; break;
      case 0x01000001L:
        pTexture = ((uMod_IDirect3DVolumeTexture9*)(pTexture))->m_D3Dtex; break;
      case 0x01000002L:
        pTexture = ((uMod_IDirect3DCubeTexture9*)(pTexture))->m_D3Dtex; break;
	    default:
	      break; // this is no fake texture and QueryInterface failed, because IDirect3DBaseTexture9 object cannot be a IDirect3D9 object ;)
	  }
	}
	/*
  if (pTexture != NULL && ((uMod_IDirect3DTexture9*)(pTexture))->GetDevice(&dev) == D3D_OK)
  {
		if(dev == this)	pTexture = ((uMod_IDirect3DTexture9*)(pTexture))->m_D3Dtex;
  }
  */
  return (m_pIDirect3DDevice9->SetTexture(Stage, pTexture));
}

HRESULT uMod_IDirect3DDevice9::GetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue)
{
  return(m_pIDirect3DDevice9->GetTextureStageState(Stage,Type, pValue));
}

HRESULT uMod_IDirect3DDevice9::SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value)
{
  return(m_pIDirect3DDevice9->SetTextureStageState(Stage,Type,Value));
}

HRESULT uMod_IDirect3DDevice9::GetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue)
{
  return(m_pIDirect3DDevice9->GetSamplerState(Sampler,Type, pValue));
}

HRESULT uMod_IDirect3DDevice9::SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value)
{
  return(m_pIDirect3DDevice9->SetSamplerState(Sampler,Type,Value));
}

HRESULT uMod_IDirect3DDevice9::ValidateDevice(DWORD* pNumPasses)
{
  return(m_pIDirect3DDevice9->ValidateDevice( pNumPasses));
}

HRESULT uMod_IDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber,CONST PALETTEENTRY* pEntries)
{
  return(m_pIDirect3DDevice9->SetPaletteEntries(PaletteNumber, pEntries));
}

HRESULT uMod_IDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries)
{
  return(m_pIDirect3DDevice9->GetPaletteEntries(PaletteNumber, pEntries));
}

HRESULT uMod_IDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
  return(m_pIDirect3DDevice9->SetCurrentTexturePalette(PaletteNumber));
}

HRESULT uMod_IDirect3DDevice9::GetCurrentTexturePalette(UINT *PaletteNumber)
{
  return(m_pIDirect3DDevice9->GetCurrentTexturePalette(PaletteNumber));
}

HRESULT uMod_IDirect3DDevice9::SetScissorRect(CONST RECT* pRect)
{
  return(m_pIDirect3DDevice9->SetScissorRect( pRect));
}

HRESULT uMod_IDirect3DDevice9::GetScissorRect( RECT* pRect)
{
  return(m_pIDirect3DDevice9->GetScissorRect( pRect));
}

HRESULT uMod_IDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
  return(m_pIDirect3DDevice9->SetSoftwareVertexProcessing(bSoftware));
}

BOOL uMod_IDirect3DDevice9::GetSoftwareVertexProcessing(void)
{
  return(m_pIDirect3DDevice9->GetSoftwareVertexProcessing());
}

HRESULT uMod_IDirect3DDevice9::SetNPatchMode(float nSegments)
{
  return(m_pIDirect3DDevice9->SetNPatchMode(nSegments));
}

float	uMod_IDirect3DDevice9::GetNPatchMode(void)
{
  return(m_pIDirect3DDevice9->GetNPatchMode());
}

HRESULT uMod_IDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount)
{
  return(m_pIDirect3DDevice9->DrawPrimitive(PrimitiveType,StartVertex,PrimitiveCount));
}

HRESULT uMod_IDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount)
{
  return(m_pIDirect3DDevice9->DrawIndexedPrimitive(PrimitiveType,BaseVertexIndex,MinVertexIndex,NumVertices,startIndex,primCount));
}

HRESULT uMod_IDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
  return(m_pIDirect3DDevice9->DrawPrimitiveUP(PrimitiveType,PrimitiveCount,pVertexStreamZeroData,VertexStreamZeroStride));
}

HRESULT uMod_IDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride)
{
  return(m_pIDirect3DDevice9->DrawIndexedPrimitiveUP(PrimitiveType,MinVertexIndex,NumVertices,PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData,VertexStreamZeroStride));
}

HRESULT uMod_IDirect3DDevice9::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags)
{
  return(m_pIDirect3DDevice9->ProcessVertices( SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags));
}

HRESULT uMod_IDirect3DDevice9::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl)
{
  return(m_pIDirect3DDevice9->CreateVertexDeclaration( pVertexElements,ppDecl));
}

HRESULT uMod_IDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
  return(m_pIDirect3DDevice9->SetVertexDeclaration(pDecl));
}

HRESULT uMod_IDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
  return(m_pIDirect3DDevice9->GetVertexDeclaration(ppDecl));
}

HRESULT uMod_IDirect3DDevice9::SetFVF(DWORD FVF)
{
  return(m_pIDirect3DDevice9->SetFVF(FVF));
}

HRESULT uMod_IDirect3DDevice9::GetFVF(DWORD* pFVF)
{
  return(m_pIDirect3DDevice9->GetFVF(pFVF));
}

HRESULT uMod_IDirect3DDevice9::CreateVertexShader(CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader)
{
  return(m_pIDirect3DDevice9->CreateVertexShader(pFunction,ppShader));
}

HRESULT uMod_IDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9* pShader)
{
  return(m_pIDirect3DDevice9->SetVertexShader(pShader));
}

HRESULT uMod_IDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
  return(m_pIDirect3DDevice9->GetVertexShader(ppShader));
}

HRESULT uMod_IDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
{
  return(m_pIDirect3DDevice9->SetVertexShaderConstantF(StartRegister,pConstantData, Vector4fCount));
}

HRESULT uMod_IDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount)
{
  return(m_pIDirect3DDevice9->GetVertexShaderConstantF(StartRegister,pConstantData,Vector4fCount));
}

HRESULT uMod_IDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
{
  return(m_pIDirect3DDevice9->SetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount));
}

HRESULT uMod_IDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount)
{
  return(m_pIDirect3DDevice9->GetVertexShaderConstantI(StartRegister,pConstantData,Vector4iCount));
}

HRESULT uMod_IDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
{
  return(m_pIDirect3DDevice9->SetVertexShaderConstantB(StartRegister,pConstantData,BoolCount));
}

HRESULT uMod_IDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
{
  return(m_pIDirect3DDevice9->GetVertexShaderConstantB(StartRegister,pConstantData,BoolCount));
}

HRESULT uMod_IDirect3DDevice9::SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride)
{
  return(m_pIDirect3DDevice9->SetStreamSource(StreamNumber,pStreamData,OffsetInBytes,Stride));
}

HRESULT uMod_IDirect3DDevice9::GetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* OffsetInBytes,UINT* pStride)
{
  return(m_pIDirect3DDevice9->GetStreamSource(StreamNumber,ppStreamData,OffsetInBytes,pStride));
}

HRESULT uMod_IDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber,UINT Divider)
{
  return(m_pIDirect3DDevice9->SetStreamSourceFreq(StreamNumber,Divider));
}

HRESULT uMod_IDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber,UINT* Divider)
{
  return(m_pIDirect3DDevice9->GetStreamSourceFreq(StreamNumber,Divider));
}

HRESULT uMod_IDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
  return(m_pIDirect3DDevice9->SetIndices(pIndexData));
}

HRESULT uMod_IDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
  return(m_pIDirect3DDevice9->GetIndices(ppIndexData));
}

HRESULT uMod_IDirect3DDevice9::CreatePixelShader(CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader)
{
  return(m_pIDirect3DDevice9->CreatePixelShader(pFunction,ppShader));
}

HRESULT uMod_IDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9* pShader)
{
  return(m_pIDirect3DDevice9->SetPixelShader(pShader));
}

HRESULT uMod_IDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
  return(m_pIDirect3DDevice9->GetPixelShader(ppShader));
}

HRESULT uMod_IDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
{
  return(m_pIDirect3DDevice9->SetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount));
}

HRESULT uMod_IDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount)
{
  return(m_pIDirect3DDevice9->GetPixelShaderConstantF(StartRegister,pConstantData,Vector4fCount));
}

HRESULT uMod_IDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
{
  return(m_pIDirect3DDevice9->SetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount));
}

HRESULT uMod_IDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount)
{
  return(m_pIDirect3DDevice9->GetPixelShaderConstantI(StartRegister,pConstantData,Vector4iCount));
}

HRESULT uMod_IDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
{
  return(m_pIDirect3DDevice9->SetPixelShaderConstantB(StartRegister,pConstantData,BoolCount));
}

HRESULT uMod_IDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount)
{
  return(m_pIDirect3DDevice9->GetPixelShaderConstantB(StartRegister,pConstantData,BoolCount));
}

HRESULT uMod_IDirect3DDevice9::DrawRectPatch(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
  return(m_pIDirect3DDevice9->DrawRectPatch(Handle,pNumSegs, pRectPatchInfo));
}

HRESULT uMod_IDirect3DDevice9::DrawTriPatch(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
  return(m_pIDirect3DDevice9->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo));
}

HRESULT uMod_IDirect3DDevice9::DeletePatch(UINT Handle)
{
  return(m_pIDirect3DDevice9->DeletePatch(Handle));
}

HRESULT uMod_IDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery)
{
  return(m_pIDirect3DDevice9->CreateQuery(Type,ppQuery));
}


