#include "uMod_Main.h"

#ifndef RETURN_QueryInterface
#define RETURN_QueryInterface 0x01000000L
#endif

#ifndef PRE_MESSAGE
#define PRE_MESSAGE "uMod_IDirect3DDevice9"
#endif


int uMod_IDirect3DDevice9::CreateSingleTexture()
{
    if (SingleTexture != nullptr && SingleVolumeTexture != nullptr && SingleCubeTexture != nullptr && TextureColour == uMod_Client->TextureColour) {
        return RETURN_OK;
    }
    TextureColour = uMod_Client->TextureColour;
    if (SingleTexture == nullptr) //create texture
    {
        if (D3D_OK != CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, (IDirect3DTexture9**)&SingleTexture, nullptr)) {
            Message(PRE_MESSAGE "::CreateSingleTexture(): CreateTexture Failed\n");
            SingleTexture = nullptr;
            return RETURN_TEXTURE_NOT_LOADED;
        }
        LastCreatedTexture = nullptr; // set LastCreatedTexture to NULL, cause LastCreatedTexture is equal SingleTexture
        SingleTexture->FAKE = true; //this is no texture created from by game
        SingleTexture->Reference = -2;
    }

    {
        D3DLOCKED_RECT d3dlr;
        IDirect3DTexture9* pD3Dtex = SingleTexture->m_D3Dtex;

        if (D3D_OK != pD3Dtex->LockRect(0, &d3dlr, nullptr, 0)) {
            Message(PRE_MESSAGE "::CreateSingleTexture(): LockRect Failed\n");
            SingleTexture->Release();
            SingleTexture = nullptr;
            return RETURN_TEXTURE_NOT_LOADED;
        }
        const auto pDst = static_cast<DWORD*>(d3dlr.pBits);

        for (int i = 0; i < 8 * 8; i++) {
            pDst[i] = TextureColour;
        }
        pD3Dtex->UnlockRect(0);
    }

    if (SingleVolumeTexture == nullptr) //create texture
    {
        if (D3D_OK != CreateVolumeTexture(8, 8, 8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, (IDirect3DVolumeTexture9**)&SingleVolumeTexture, nullptr)) {
            Message(PRE_MESSAGE "::CreateSingleTexture(): CreateVolumeTexture Failed\n");
            SingleVolumeTexture = nullptr;
            return RETURN_TEXTURE_NOT_LOADED;
        }
        LastCreatedVolumeTexture = nullptr; // set LastCreatedTexture to NULL, cause LastCreatedTexture is equal SingleTexture
        SingleVolumeTexture->FAKE = true; //this is no texture created from by game
        SingleVolumeTexture->Reference = -2;
    }

    {
        D3DLOCKED_BOX d3dlr;
        IDirect3DVolumeTexture9* pD3Dtex = SingleVolumeTexture->m_D3Dtex;
        //LockBox)(UINT Level, D3DLOCKED_BOX *pLockedVolume, CONST D3DBOX *pBox,
        if (D3D_OK != pD3Dtex->LockBox(0, &d3dlr, nullptr, 0)) {
            Message(PRE_MESSAGE "::CreateSingleTexture(): LockBox Failed\n");
            SingleVolumeTexture->Release();
            SingleVolumeTexture = nullptr;
            return RETURN_TEXTURE_NOT_LOADED;
        }
        const auto pDst = static_cast<DWORD*>(d3dlr.pBits);

        for (int i = 0; i < 8 * 8 * 8; i++) {
            pDst[i] = TextureColour;
        }
        pD3Dtex->UnlockBox(0);
    }
    if (SingleCubeTexture == nullptr) //create texture
    {
        if (D3D_OK != CreateCubeTexture(8, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, (IDirect3DCubeTexture9**)&SingleCubeTexture, nullptr)) {
            Message(PRE_MESSAGE "::CreateSingleTexture(): CreateCubeTexture Failed\n");
            SingleCubeTexture = nullptr;
            return RETURN_TEXTURE_NOT_LOADED;
        }
        LastCreatedCubeTexture = nullptr; // set LastCreatedTexture to NULL, cause LastCreatedTexture is equal SingleTexture
        SingleCubeTexture->FAKE = true; //this is no texture created from by game
        SingleCubeTexture->Reference = -2;
    }

    {
        D3DLOCKED_RECT d3dlr;
        IDirect3DCubeTexture9* pD3Dtex = SingleCubeTexture->m_D3Dtex;

        for (int c = 0; c < 6; c++) {
            if (D3D_OK != pD3Dtex->LockRect(static_cast<D3DCUBEMAP_FACES>(c), 0, &d3dlr, nullptr, 0)) {
                Message(PRE_MESSAGE "::CreateSingleTexture(): LockRect (Cube) Failed\n");
                SingleCubeTexture->Release();
                SingleCubeTexture = nullptr;
                return RETURN_TEXTURE_NOT_LOADED;
            }
            const auto pDst = static_cast<DWORD*>(d3dlr.pBits);

            for (int i = 0; i < 8 * 8; i++) {
                pDst[i] = TextureColour;
            }
            pD3Dtex->UnlockRect(static_cast<D3DCUBEMAP_FACES>(c), 0);
        }
    }

    return RETURN_OK;
}

uMod_IDirect3DDevice9::uMod_IDirect3DDevice9(IDirect3DDevice9* pOriginal, int back_buffer_count)
{
    Message(PRE_MESSAGE "::" PRE_MESSAGE "( %p, %p): %p\n", pOriginal, server, this);

    BackBufferCount = back_buffer_count;
    NormalRendering = true;

    uMod_Client = new uMod_TextureClient(this); //get a new texture client for this device
    uMod_Client->Initialize();

    LastCreatedTexture = nullptr;
    LastCreatedVolumeTexture = nullptr;
    LastCreatedCubeTexture = nullptr;
    m_pIDirect3DDevice9 = pOriginal; // store the pointer to original object
    TextureColour = D3DCOLOR_ARGB(255, 0, 255, 0);

    CounterSaveSingleTexture = -20;

    SingleTextureMod = 0;
    SingleTexture = nullptr;
    SingleVolumeTexture = nullptr;
    SingleCubeTexture = nullptr;
    OSD_Font = nullptr;
    uMod_Reference = 1;
}

uMod_IDirect3DDevice9::~uMod_IDirect3DDevice9()
{
    Message(PRE_MESSAGE "::~" PRE_MESSAGE "(): %p\n", this);
}

HRESULT uMod_IDirect3DDevice9::QueryInterface(REFIID riid, void** ppvObj)
{
    // check if original dll can provide interface. then send *our* address
    if (riid == IID_IDirect3DTexture9) {
        // This function should never be called with IDirect3DTexture9 by the game
        *ppvObj = this;
        return RETURN_QueryInterface;
    }

    *ppvObj = nullptr;
    Message(PRE_MESSAGE "::QueryInterface(): %p\n", this);
    const HRESULT hRes = m_pIDirect3DDevice9->QueryInterface(riid, ppvObj);

    if (*ppvObj == m_pIDirect3DDevice9) {
        uMod_Reference++; //increasing our counter
        *ppvObj = this;
    }

    return hRes;
}

ULONG uMod_IDirect3DDevice9::AddRef()
{
    uMod_Reference++; //increasing our counter
    Message("%p = " PRE_MESSAGE "::AddRef(): %p\n", uMod_Reference, this);
    return m_pIDirect3DDevice9->AddRef();
}

ULONG uMod_IDirect3DDevice9::Release()
{
    if (--uMod_Reference == 0) //if our counter drops to zero, the real device will be deleted, so we clean up before
    {
        // we must not release the fake textures, cause they are released if the target textures are released
        // and the target textures are released by the game.

        if (SingleTexture != nullptr) {
            SingleTexture->Release(); //this is the only texture we must release by ourself
        }
        if (SingleVolumeTexture != nullptr) {
            SingleVolumeTexture->Release(); //this is the only texture we must release by ourself
        }
        if (SingleCubeTexture != nullptr) {
            SingleCubeTexture->Release(); //this is the only texture we must release by ourself
        }
        if (OSD_Font != nullptr) {
            OSD_Font->Release();
        }

        delete uMod_Client; //must be deleted at the end, because other releases might call a function of this object
        uMod_Client = nullptr;
        SingleTexture = nullptr;
        OSD_Font = nullptr;
    }

    const ULONG count = m_pIDirect3DDevice9->Release();
    Message("%p = " PRE_MESSAGE "::Release(): %p\n", count, this);
    if (uMod_Reference != count) //bug
    {
        Message("Error in " PRE_MESSAGE "::Release(): %p!=%p\n", uMod_Reference, count);
    }

    if (count == 0u) {
        delete this;
    }
    return count;
}

HRESULT uMod_IDirect3DDevice9::TestCooperativeLevel()
{
    return m_pIDirect3DDevice9->TestCooperativeLevel();
}

UINT uMod_IDirect3DDevice9::GetAvailableTextureMem()
{
    return m_pIDirect3DDevice9->GetAvailableTextureMem();
}

HRESULT uMod_IDirect3DDevice9::EvictManagedResources()
{
    return m_pIDirect3DDevice9->EvictManagedResources();
}

HRESULT uMod_IDirect3DDevice9::GetDirect3D(IDirect3D9** ppD3D9)
{
    return m_pIDirect3DDevice9->GetDirect3D(ppD3D9);
}

HRESULT uMod_IDirect3DDevice9::GetDeviceCaps(D3DCAPS9* pCaps)
{
    return m_pIDirect3DDevice9->GetDeviceCaps(pCaps);
}

HRESULT uMod_IDirect3DDevice9::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode)
{
    return m_pIDirect3DDevice9->GetDisplayMode(iSwapChain, pMode);
}

HRESULT uMod_IDirect3DDevice9::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters)
{
    return m_pIDirect3DDevice9->GetCreationParameters(pParameters);
}

HRESULT uMod_IDirect3DDevice9::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
    return m_pIDirect3DDevice9->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

void uMod_IDirect3DDevice9::SetCursorPosition(int X, int Y, DWORD Flags)
{
    m_pIDirect3DDevice9->SetCursorPosition(X, Y, Flags);
}

BOOL uMod_IDirect3DDevice9::ShowCursor(BOOL bShow)
{
    return m_pIDirect3DDevice9->ShowCursor(bShow);
}

HRESULT uMod_IDirect3DDevice9::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** pSwapChain)
{
    return m_pIDirect3DDevice9->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

HRESULT uMod_IDirect3DDevice9::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain)
{
    return m_pIDirect3DDevice9->GetSwapChain(iSwapChain, pSwapChain);
}

UINT uMod_IDirect3DDevice9::GetNumberOfSwapChains()
{
    return m_pIDirect3DDevice9->GetNumberOfSwapChains();
}

HRESULT uMod_IDirect3DDevice9::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    if (OSD_Font != nullptr) {
        OSD_Font->Release();
        OSD_Font = nullptr;
    } //the game will crashes if the font is not released before the game is minimized!
    return m_pIDirect3DDevice9->Reset(pPresentationParameters);
}

HRESULT uMod_IDirect3DDevice9::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect, HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
    return m_pIDirect3DDevice9->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT uMod_IDirect3DDevice9::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer)
{
    return m_pIDirect3DDevice9->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT uMod_IDirect3DDevice9::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus)
{
    return m_pIDirect3DDevice9->GetRasterStatus(iSwapChain, pRasterStatus);
}

HRESULT uMod_IDirect3DDevice9::SetDialogBoxMode(BOOL bEnableDialogs)
{
    return m_pIDirect3DDevice9->SetDialogBoxMode(bEnableDialogs);
}

void uMod_IDirect3DDevice9::SetGammaRamp(UINT iSwapChain, DWORD Flags,CONST D3DGAMMARAMP* pRamp)
{
    return m_pIDirect3DDevice9->SetGammaRamp(iSwapChain, Flags, pRamp);
}

void uMod_IDirect3DDevice9::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp)
{
    return m_pIDirect3DDevice9->GetGammaRamp(iSwapChain, pRamp);
}

HRESULT uMod_IDirect3DDevice9::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
    //create real texture
    //Message("uMod_IDirect3DDevice9::CreateTexture()\n");
    const HRESULT ret = m_pIDirect3DDevice9->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
    if (ret != D3D_OK) {
        return ret;
    }

    //create fake texture
    const auto texture = new uMod_IDirect3DTexture9(ppTexture, this);
    if (texture) {
        *ppTexture = texture;
    }

    if (LastCreatedTexture != nullptr) //if a texture was loaded before, hopefully this texture contains now the data, so we can add it
    {
        if (uMod_Client != nullptr) {
            uMod_Client->AddTexture(LastCreatedTexture);
        }
    }
    LastCreatedTexture = texture;
    return ret;
}

HRESULT uMod_IDirect3DDevice9::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
    //create real texture
    //Message("uMod_IDirect3DDevice9::CreateVolumeTexture()\n");
    const HRESULT ret = m_pIDirect3DDevice9->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
    if (ret != D3D_OK) {
        return ret;
    }

    //create fake texture
    const auto texture = new uMod_IDirect3DVolumeTexture9(ppVolumeTexture, this);
    if (texture) {
        *ppVolumeTexture = texture;
    }

    if (LastCreatedVolumeTexture != nullptr) //if a texture was loaded before, hopefully this texture contains now the data, so we can add it
    {
        if (uMod_Client != nullptr) {
            uMod_Client->AddTexture(LastCreatedVolumeTexture);
        }
    }
    LastCreatedVolumeTexture = texture;
    return ret;
}

HRESULT uMod_IDirect3DDevice9::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
{
    //create real texture
    //Message("uMod_IDirect3DDevice9::CreateCubeTexture()\n");
    const HRESULT ret = m_pIDirect3DDevice9->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
    if (ret != D3D_OK) {
        return ret;
    }

    //create fake texture
    const auto texture = new uMod_IDirect3DCubeTexture9(ppCubeTexture, this);
    if (texture) {
        *ppCubeTexture = texture;
    }

    if (LastCreatedCubeTexture != nullptr) //if a texture was loaded before, hopefully this texture contains now the data, so we can add it
    {
        if (uMod_Client != nullptr) {
            uMod_Client->AddTexture(LastCreatedCubeTexture);
        }
    }
    LastCreatedCubeTexture = texture;
    return ret;
}

HRESULT uMod_IDirect3DDevice9::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle)
{
    return m_pIDirect3DDevice9->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, pSharedHandle);
}

HRESULT uMod_IDirect3DDevice9::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle)
{
    return m_pIDirect3DDevice9->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, pSharedHandle);
}

HRESULT uMod_IDirect3DDevice9::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    return m_pIDirect3DDevice9->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle);
}

HRESULT uMod_IDirect3DDevice9::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    return m_pIDirect3DDevice9->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle);
}

HRESULT uMod_IDirect3DDevice9::UpdateSurface(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint)
{
    return m_pIDirect3DDevice9->UpdateSurface(pSourceSurface, pSourceRect, pDestinationSurface, pDestPoint);
}

HRESULT uMod_IDirect3DDevice9::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
{
    Message(PRE_MESSAGE "::UpdateTexture( %p, %p): %p\n", pSourceTexture, pDestinationTexture, this);
    // we must pass the real texture objects


    uMod_IDirect3DTexture9* pSource = nullptr;
    uMod_IDirect3DVolumeTexture9* pSourceVolume = nullptr;
    uMod_IDirect3DCubeTexture9* pSourceCube = nullptr;
    IDirect3DBaseTexture9* cpy;
    if (pSourceTexture != nullptr) {
        long int ret = pSourceTexture->QueryInterface(IID_IDirect3D9, (void**)&cpy);
        switch (ret) {
            case 0x01000000L: {
                pSource = static_cast<uMod_IDirect3DTexture9*>(pSourceTexture);
                if (const auto hash = pSource->GetHash()) {
                    if (hash != pSource->Hash) // this hash has changed !!
                    {
                        pSource->Hash = hash;
                        if (pSource->CrossRef_D3Dtex != nullptr) {
                            UnswitchTextures(pSource);
                        }
                        uMod_Client->LookUpToMod(pSource);
                    }
                }
                else if (pSource->CrossRef_D3Dtex != nullptr) {
                    UnswitchTextures(pSource); // we better unswitch
                }

                // the source must be the original texture if not switched and the fake texture if it is switched
                if (pSource->CrossRef_D3Dtex != nullptr) {
                    pSourceTexture = pSource->CrossRef_D3Dtex->m_D3Dtex;
                }
                else {
                    pSourceTexture = pSource->m_D3Dtex;
                }
                break;
            }
            case 0x01000001L: {
                pSourceVolume = static_cast<uMod_IDirect3DVolumeTexture9*>(pSourceTexture);
                if (pSourceVolume->GetHash()) {
                    if (const auto hash = pSourceVolume->Hash) // this hash has changed !!
                    {
                        pSourceVolume->Hash = hash;
                        if (pSourceVolume->CrossRef_D3Dtex != nullptr) {
                            UnswitchTextures(pSourceVolume);
                        }
                        uMod_Client->LookUpToMod(pSourceVolume);
                    }
                }
                else if (pSourceVolume->CrossRef_D3Dtex != nullptr) {
                    UnswitchTextures(pSourceVolume); // we better unswitch
                }

                // the source must be the original texture if not switched and the fake texture if it is switched
                if (pSourceVolume->CrossRef_D3Dtex != nullptr) {
                    pSourceTexture = pSourceVolume->CrossRef_D3Dtex->m_D3Dtex;
                }
                else {
                    pSourceTexture = pSourceVolume->m_D3Dtex;
                }
                break;
            }
            case 0x01000002L: {
                pSourceCube = static_cast<uMod_IDirect3DCubeTexture9*>(pSourceTexture);
                if (const auto hash = pSourceCube->GetHash()) {
                    if (hash != pSourceCube->Hash) // this hash has changed !!
                    {
                        pSourceCube->Hash = hash;
                        if (pSourceCube->CrossRef_D3Dtex != nullptr) {
                            UnswitchTextures(pSourceCube);
                        }
                        uMod_Client->LookUpToMod(pSourceCube);
                    }
                }
                else if (pSourceCube->CrossRef_D3Dtex != nullptr) {
                    UnswitchTextures(pSourceCube); // we better unswitch
                }

                // the source must be the original texture if not switched and the fake texture if it is switched
                if (pSourceCube->CrossRef_D3Dtex != nullptr) {
                    pSourceTexture = pSourceCube->CrossRef_D3Dtex->m_D3Dtex;
                }
                else {
                    pSourceTexture = pSourceCube->m_D3Dtex;
                }
                break;
            }
            default:
                break; // this is no fake texture and QueryInterface failed, because IDirect3DBaseTexture9 object cannot be a IDirect3D9 object ;)
        }
    }


    if (pDestinationTexture != nullptr) {
        long int ret = pSourceTexture->QueryInterface(IID_IDirect3D9, (void**)&cpy);
        switch (ret) {
            case 0x01000000L: {
                auto pDest = static_cast<uMod_IDirect3DTexture9*>(pDestinationTexture);

                if (pSource != nullptr && pDest->Hash != pSource->Hash) {
                    pDest->Hash = pSource->Hash; // take over the hash
                    UnswitchTextures(pDest);
                    if (pSource->CrossRef_D3Dtex != nullptr) {
                        uMod_IDirect3DTexture9* cpy = pSource->CrossRef_D3Dtex;
                        UnswitchTextures(pSource);
                        SwitchTextures(cpy, pDest);
                    }
                }
                if (pDest->CrossRef_D3Dtex != nullptr) {
                    pDestinationTexture = pDest->CrossRef_D3Dtex->m_D3Dtex; // make sure to copy into the original texture
                }
                else {
                    pDestinationTexture = pDest->m_D3Dtex;
                }
                break;
            }
            case 0x01000001L: {
                auto pDest = static_cast<uMod_IDirect3DVolumeTexture9*>(pDestinationTexture);

                if (pSourceVolume != nullptr && pDest->Hash != pSourceVolume->Hash) {
                    pDest->Hash = pSourceVolume->Hash; // take over the hash
                    UnswitchTextures(pDest);
                    if (pSourceVolume->CrossRef_D3Dtex != nullptr) {
                        uMod_IDirect3DVolumeTexture9* cpy = pSourceVolume->CrossRef_D3Dtex;
                        UnswitchTextures(pSourceVolume);
                        SwitchTextures(cpy, pDest);
                    }
                }
                if (pDest->CrossRef_D3Dtex != nullptr) {
                    pDestinationTexture = pDest->CrossRef_D3Dtex->m_D3Dtex; // make sure to copy into the original texture
                }
                else {
                    pDestinationTexture = pDest->m_D3Dtex;
                }
                break;
            }
            case 0x01000002L: {
                auto pDest = static_cast<uMod_IDirect3DCubeTexture9*>(pDestinationTexture);

                if (pSourceCube != nullptr && pDest->Hash != pSourceCube->Hash) {
                    pDest->Hash = pSourceCube->Hash; // take over the hash
                    UnswitchTextures(pDest);
                    if (pSourceCube->CrossRef_D3Dtex != nullptr) {
                        uMod_IDirect3DCubeTexture9* cpy = pSourceCube->CrossRef_D3Dtex;
                        UnswitchTextures(pSourceCube);
                        SwitchTextures(cpy, pDest);
                    }
                }
                if (pDest->CrossRef_D3Dtex != nullptr) {
                    pDestinationTexture = pDest->CrossRef_D3Dtex->m_D3Dtex; // make sure to copy into the original texture
                }
                else {
                    pDestinationTexture = pDest->m_D3Dtex;
                }
                break;
            }
        }
    }
    return m_pIDirect3DDevice9->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT uMod_IDirect3DDevice9::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface)
{
    return m_pIDirect3DDevice9->GetRenderTargetData(pRenderTarget, pDestSurface);
}

HRESULT uMod_IDirect3DDevice9::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface)
{
    return m_pIDirect3DDevice9->GetFrontBufferData(iSwapChain, pDestSurface);
}

HRESULT uMod_IDirect3DDevice9::StretchRect(IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
    return m_pIDirect3DDevice9->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT uMod_IDirect3DDevice9::ColorFill(IDirect3DSurface9* pSurface,CONST RECT* pRect, D3DCOLOR color)
{
    return m_pIDirect3DDevice9->ColorFill(pSurface, pRect, color);
}

HRESULT uMod_IDirect3DDevice9::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle)
{
    return m_pIDirect3DDevice9->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT uMod_IDirect3DDevice9::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
    {
        IDirect3DSurface9* back_buffer;
        NormalRendering = false;
        for (int i = 0; !NormalRendering && i < BackBufferCount; i++) {
            m_pIDirect3DDevice9->GetBackBuffer(0, i, D3DBACKBUFFER_TYPE_MONO, &back_buffer);
            if (back_buffer == pRenderTarget) {
                NormalRendering = true;
            }
            back_buffer->Release();
        }
    }
    return m_pIDirect3DDevice9->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT uMod_IDirect3DDevice9::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
    return m_pIDirect3DDevice9->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT uMod_IDirect3DDevice9::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
    return m_pIDirect3DDevice9->SetDepthStencilSurface(pNewZStencil);
}

HRESULT uMod_IDirect3DDevice9::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
    return m_pIDirect3DDevice9->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT uMod_IDirect3DDevice9::BeginScene()
{
    if (LastCreatedTexture != nullptr) // add the last created texture
    {
        uMod_Client->AddTexture(LastCreatedTexture);
    }
    if (LastCreatedVolumeTexture != nullptr) // add the last created texture
    {
        uMod_Client->AddTexture(LastCreatedVolumeTexture);
    }
    if (LastCreatedCubeTexture != nullptr) // add the last created texture
    {
        uMod_Client->AddTexture(LastCreatedCubeTexture);
    }
    uMod_Client->MergeUpdate(); // merge an update, if present

    return m_pIDirect3DDevice9->BeginScene();
}

HRESULT uMod_IDirect3DDevice9::EndScene()
{
    return m_pIDirect3DDevice9->EndScene();
}

HRESULT uMod_IDirect3DDevice9::Clear(DWORD Count,CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
    return m_pIDirect3DDevice9->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT uMod_IDirect3DDevice9::SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
    return m_pIDirect3DDevice9->SetTransform(State, pMatrix);
}

HRESULT uMod_IDirect3DDevice9::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
    return m_pIDirect3DDevice9->GetTransform(State, pMatrix);
}

HRESULT uMod_IDirect3DDevice9::MultiplyTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
    return m_pIDirect3DDevice9->MultiplyTransform(State, pMatrix);
}

HRESULT uMod_IDirect3DDevice9::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
    return m_pIDirect3DDevice9->SetViewport(pViewport);
}

HRESULT uMod_IDirect3DDevice9::GetViewport(D3DVIEWPORT9* pViewport)
{
    return m_pIDirect3DDevice9->GetViewport(pViewport);
}

HRESULT uMod_IDirect3DDevice9::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
    return m_pIDirect3DDevice9->SetMaterial(pMaterial);
}

HRESULT uMod_IDirect3DDevice9::GetMaterial(D3DMATERIAL9* pMaterial)
{
    return m_pIDirect3DDevice9->GetMaterial(pMaterial);
}

HRESULT uMod_IDirect3DDevice9::SetLight(DWORD Index,CONST D3DLIGHT9* pLight)
{
    return m_pIDirect3DDevice9->SetLight(Index, pLight);
}

HRESULT uMod_IDirect3DDevice9::GetLight(DWORD Index, D3DLIGHT9* pLight)
{
    return m_pIDirect3DDevice9->GetLight(Index, pLight);
}

HRESULT uMod_IDirect3DDevice9::LightEnable(DWORD Index, BOOL Enable)
{
    return m_pIDirect3DDevice9->LightEnable(Index, Enable);
}

HRESULT uMod_IDirect3DDevice9::GetLightEnable(DWORD Index, BOOL* pEnable)
{
    return m_pIDirect3DDevice9->GetLightEnable(Index, pEnable);
}

HRESULT uMod_IDirect3DDevice9::SetClipPlane(DWORD Index,CONST float* pPlane)
{
    return m_pIDirect3DDevice9->SetClipPlane(Index, pPlane);
}

HRESULT uMod_IDirect3DDevice9::GetClipPlane(DWORD Index, float* pPlane)
{
    return m_pIDirect3DDevice9->GetClipPlane(Index, pPlane);
}

HRESULT uMod_IDirect3DDevice9::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
    return m_pIDirect3DDevice9->SetRenderState(State, Value);
}

HRESULT uMod_IDirect3DDevice9::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue)
{
    return m_pIDirect3DDevice9->GetRenderState(State, pValue);
}

HRESULT uMod_IDirect3DDevice9::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
    return m_pIDirect3DDevice9->CreateStateBlock(Type, ppSB);
}

HRESULT uMod_IDirect3DDevice9::BeginStateBlock()
{
    return m_pIDirect3DDevice9->BeginStateBlock();
}

HRESULT uMod_IDirect3DDevice9::EndStateBlock(IDirect3DStateBlock9** ppSB)
{
    return m_pIDirect3DDevice9->EndStateBlock(ppSB);
}

HRESULT uMod_IDirect3DDevice9::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus)
{
    return m_pIDirect3DDevice9->SetClipStatus(pClipStatus);
}

HRESULT uMod_IDirect3DDevice9::GetClipStatus(D3DCLIPSTATUS9* pClipStatus)
{
    return m_pIDirect3DDevice9->GetClipStatus(pClipStatus);
}

HRESULT uMod_IDirect3DDevice9::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture)
{
    return m_pIDirect3DDevice9->GetTexture(Stage, ppTexture);
}

HRESULT uMod_IDirect3DDevice9::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
    // we must pass the real texture objects
    // if (dev != this) this texture was not initialized through our device and is thus no fake texture object

    //IDirect3DDevice9 *dev = NULL;
    IDirect3DBaseTexture9* cpy;
    if (pTexture != nullptr) {
        long int ret = pTexture->QueryInterface(IID_IDirect3D9, (void**)&cpy);
        switch (ret) {
            case 0x01000000L:
                pTexture = static_cast<uMod_IDirect3DTexture9*>(pTexture)->m_D3Dtex;
                break;
            case 0x01000001L:
                pTexture = static_cast<uMod_IDirect3DVolumeTexture9*>(pTexture)->m_D3Dtex;
                break;
            case 0x01000002L:
                pTexture = static_cast<uMod_IDirect3DCubeTexture9*>(pTexture)->m_D3Dtex;
                break;
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
    return m_pIDirect3DDevice9->SetTexture(Stage, pTexture);
}

HRESULT uMod_IDirect3DDevice9::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
    return m_pIDirect3DDevice9->GetTextureStageState(Stage, Type, pValue);
}

HRESULT uMod_IDirect3DDevice9::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
    return m_pIDirect3DDevice9->SetTextureStageState(Stage, Type, Value);
}

HRESULT uMod_IDirect3DDevice9::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue)
{
    return m_pIDirect3DDevice9->GetSamplerState(Sampler, Type, pValue);
}

HRESULT uMod_IDirect3DDevice9::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
{
    return m_pIDirect3DDevice9->SetSamplerState(Sampler, Type, Value);
}

HRESULT uMod_IDirect3DDevice9::ValidateDevice(DWORD* pNumPasses)
{
    return m_pIDirect3DDevice9->ValidateDevice(pNumPasses);
}

HRESULT uMod_IDirect3DDevice9::SetPaletteEntries(UINT PaletteNumber,CONST PALETTEENTRY* pEntries)
{
    return m_pIDirect3DDevice9->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT uMod_IDirect3DDevice9::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries)
{
    return m_pIDirect3DDevice9->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT uMod_IDirect3DDevice9::SetCurrentTexturePalette(UINT PaletteNumber)
{
    return m_pIDirect3DDevice9->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT uMod_IDirect3DDevice9::GetCurrentTexturePalette(UINT* PaletteNumber)
{
    return m_pIDirect3DDevice9->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT uMod_IDirect3DDevice9::SetScissorRect(CONST RECT* pRect)
{
    return m_pIDirect3DDevice9->SetScissorRect(pRect);
}

HRESULT uMod_IDirect3DDevice9::GetScissorRect(RECT* pRect)
{
    return m_pIDirect3DDevice9->GetScissorRect(pRect);
}

HRESULT uMod_IDirect3DDevice9::SetSoftwareVertexProcessing(BOOL bSoftware)
{
    return m_pIDirect3DDevice9->SetSoftwareVertexProcessing(bSoftware);
}

BOOL uMod_IDirect3DDevice9::GetSoftwareVertexProcessing()
{
    return m_pIDirect3DDevice9->GetSoftwareVertexProcessing();
}

HRESULT uMod_IDirect3DDevice9::SetNPatchMode(float nSegments)
{
    return m_pIDirect3DDevice9->SetNPatchMode(nSegments);
}

float uMod_IDirect3DDevice9::GetNPatchMode()
{
    return m_pIDirect3DDevice9->GetNPatchMode();
}

HRESULT uMod_IDirect3DDevice9::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
    return m_pIDirect3DDevice9->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT uMod_IDirect3DDevice9::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    return m_pIDirect3DDevice9->DrawIndexedPrimitive(PrimitiveType, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

HRESULT uMod_IDirect3DDevice9::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount,CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    return m_pIDirect3DDevice9->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT uMod_IDirect3DDevice9::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount,CONST void* pIndexData, D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,
                                                      UINT VertexStreamZeroStride)
{
    return m_pIDirect3DDevice9->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT uMod_IDirect3DDevice9::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags)
{
    return m_pIDirect3DDevice9->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT uMod_IDirect3DDevice9::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl)
{
    return m_pIDirect3DDevice9->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT uMod_IDirect3DDevice9::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl)
{
    return m_pIDirect3DDevice9->SetVertexDeclaration(pDecl);
}

HRESULT uMod_IDirect3DDevice9::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl)
{
    return m_pIDirect3DDevice9->GetVertexDeclaration(ppDecl);
}

HRESULT uMod_IDirect3DDevice9::SetFVF(DWORD FVF)
{
    return m_pIDirect3DDevice9->SetFVF(FVF);
}

HRESULT uMod_IDirect3DDevice9::GetFVF(DWORD* pFVF)
{
    return m_pIDirect3DDevice9->GetFVF(pFVF);
}

HRESULT uMod_IDirect3DDevice9::CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader)
{
    return m_pIDirect3DDevice9->CreateVertexShader(pFunction, ppShader);
}

HRESULT uMod_IDirect3DDevice9::SetVertexShader(IDirect3DVertexShader9* pShader)
{
    return m_pIDirect3DDevice9->SetVertexShader(pShader);
}

HRESULT uMod_IDirect3DDevice9::GetVertexShader(IDirect3DVertexShader9** ppShader)
{
    return m_pIDirect3DDevice9->GetVertexShader(ppShader);
}

HRESULT uMod_IDirect3DDevice9::SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData, UINT Vector4fCount)
{
    return m_pIDirect3DDevice9->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT uMod_IDirect3DDevice9::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    return m_pIDirect3DDevice9->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT uMod_IDirect3DDevice9::SetVertexShaderConstantI(UINT StartRegister,CONST int* pConstantData, UINT Vector4iCount)
{
    return m_pIDirect3DDevice9->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT uMod_IDirect3DDevice9::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    return m_pIDirect3DDevice9->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT uMod_IDirect3DDevice9::SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount)
{
    return m_pIDirect3DDevice9->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT uMod_IDirect3DDevice9::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
    return m_pIDirect3DDevice9->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT uMod_IDirect3DDevice9::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride)
{
    return m_pIDirect3DDevice9->SetStreamSource(StreamNumber, pStreamData, OffsetInBytes, Stride);
}

HRESULT uMod_IDirect3DDevice9::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride)
{
    return m_pIDirect3DDevice9->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride);
}

HRESULT uMod_IDirect3DDevice9::SetStreamSourceFreq(UINT StreamNumber, UINT Divider)
{
    return m_pIDirect3DDevice9->SetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT uMod_IDirect3DDevice9::GetStreamSourceFreq(UINT StreamNumber, UINT* Divider)
{
    return m_pIDirect3DDevice9->GetStreamSourceFreq(StreamNumber, Divider);
}

HRESULT uMod_IDirect3DDevice9::SetIndices(IDirect3DIndexBuffer9* pIndexData)
{
    return m_pIDirect3DDevice9->SetIndices(pIndexData);
}

HRESULT uMod_IDirect3DDevice9::GetIndices(IDirect3DIndexBuffer9** ppIndexData)
{
    return m_pIDirect3DDevice9->GetIndices(ppIndexData);
}

HRESULT uMod_IDirect3DDevice9::CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader)
{
    return m_pIDirect3DDevice9->CreatePixelShader(pFunction, ppShader);
}

HRESULT uMod_IDirect3DDevice9::SetPixelShader(IDirect3DPixelShader9* pShader)
{
    return m_pIDirect3DDevice9->SetPixelShader(pShader);
}

HRESULT uMod_IDirect3DDevice9::GetPixelShader(IDirect3DPixelShader9** ppShader)
{
    return m_pIDirect3DDevice9->GetPixelShader(ppShader);
}

HRESULT uMod_IDirect3DDevice9::SetPixelShaderConstantF(UINT StartRegister,CONST float* pConstantData, UINT Vector4fCount)
{
    return m_pIDirect3DDevice9->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT uMod_IDirect3DDevice9::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount)
{
    return m_pIDirect3DDevice9->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

HRESULT uMod_IDirect3DDevice9::SetPixelShaderConstantI(UINT StartRegister,CONST int* pConstantData, UINT Vector4iCount)
{
    return m_pIDirect3DDevice9->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT uMod_IDirect3DDevice9::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount)
{
    return m_pIDirect3DDevice9->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
}

HRESULT uMod_IDirect3DDevice9::SetPixelShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount)
{
    return m_pIDirect3DDevice9->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT uMod_IDirect3DDevice9::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount)
{
    return m_pIDirect3DDevice9->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
}

HRESULT uMod_IDirect3DDevice9::DrawRectPatch(UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
    return m_pIDirect3DDevice9->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT uMod_IDirect3DDevice9::DrawTriPatch(UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
    return m_pIDirect3DDevice9->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT uMod_IDirect3DDevice9::DeletePatch(UINT Handle)
{
    return m_pIDirect3DDevice9->DeletePatch(Handle);
}

HRESULT uMod_IDirect3DDevice9::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
    return m_pIDirect3DDevice9->CreateQuery(Type, ppQuery);
}
