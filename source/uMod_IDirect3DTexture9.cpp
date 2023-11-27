#include "Main.h"

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::QueryInterface(REFIID riid, void** ppvObj)
{
    if (riid == IID_IDirect3D9) {
        // This function should never be called with IID_IDirect3D9 by the game
        // thus this call comes from our own dll to ask for the texture type
        // 0x01000000L == uMod_IDirect3DTexture9
        // 0x01000001L == uMod_IDirect3DVolumeTexture9
        // 0x01000002L == uMod_IDirect3DCubeTexture9

        *ppvObj = this;
        return 0x01000000L;
    }
    HRESULT hRes;
    if (CrossRef_D3Dtex != nullptr) {
        hRes = CrossRef_D3Dtex->m_D3Dtex->QueryInterface(riid, ppvObj);
        if (*ppvObj == CrossRef_D3Dtex->m_D3Dtex) {
            *ppvObj = this;
        }
    }
    else {
        hRes = m_D3Dtex->QueryInterface(riid, ppvObj);
        if (*ppvObj == m_D3Dtex) {
            *ppvObj = this;
        }
    }
    return hRes;
}

//this function yields for the non switched texture object
ULONG APIENTRY uMod_IDirect3DTexture9::AddRef()
{
    if (FAKE) {
        return 1; //bug, this case should never happen
    }
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->AddRef();
    }
    return m_D3Dtex->AddRef();
}

//this function yields for the non switched texture object
ULONG APIENTRY uMod_IDirect3DTexture9::Release()
{
    Message("uMod_IDirect3DTexture9::Release(): %p\n", this);

    void* cpy;
    const long ret = m_D3Ddev->QueryInterface(IID_IDirect3DTexture9, &cpy);

    ULONG count;
    if (FAKE) {
        UnswitchTextures(this);
        count = m_D3Dtex->Release(); //count must be zero, cause we don't call AddRef of fake_textures
    }
    else {
        if (CrossRef_D3Dtex != nullptr) //if this texture is switched with a fake texture
        {
            uMod_IDirect3DTexture9* fake_texture = CrossRef_D3Dtex;
            count = fake_texture->m_D3Dtex->Release(); //release the original texture
            if (count == 0) //if texture is released we switch the textures back
            {
                UnswitchTextures(this);
                fake_texture->Release(); // we release the fake texture
            }
        }
        else {
            count = m_D3Dtex->Release();
        }
    }

    if (count == 0) //if this texture is released, we clean up
    {
        // if this texture is the LastCreatedTexture, the next time LastCreatedTexture would be added,
        // the hash of a non existing texture would be calculated
        if (ret == 0x01000000L) {
            if (static_cast<uMod_IDirect3DDevice9*>(m_D3Ddev)->GetLastCreatedTexture() == this) {
                static_cast<uMod_IDirect3DDevice9*>(m_D3Ddev)->SetLastCreatedTexture(nullptr);
            }
            else {
                static_cast<uMod_IDirect3DDevice9*>(m_D3Ddev)->GetuMod_Client()->RemoveTexture(this); // remove this texture from the texture client
            }
        }
        else {
            if (static_cast<uMod_IDirect3DDevice9Ex*>(m_D3Ddev)->GetLastCreatedTexture() == this) {
                static_cast<uMod_IDirect3DDevice9Ex*>(m_D3Ddev)->SetLastCreatedTexture(nullptr);
            }
            else {
                static_cast<uMod_IDirect3DDevice9Ex*>(m_D3Ddev)->GetuMod_Client()->RemoveTexture(this); // remove this texture from the texture client
            }
        }

        delete this;
    }

    Message("uMod_IDirect3DTexture9::Release() end: %p\n", this);
    return count;
}

HRESULT APIENTRY uMod_IDirect3DTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{
    *ppDevice = m_D3Ddev;
    return D3D_OK;
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::SetPrivateData(REFGUID refguid,CONST void* pData, DWORD SizeOfData, DWORD Flags)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags);
    }
    return m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData);
    }
    return m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::FreePrivateData(REFGUID refguid)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->FreePrivateData(refguid);
    }
    return m_D3Dtex->FreePrivateData(refguid);
}

DWORD APIENTRY uMod_IDirect3DTexture9::SetPriority(DWORD PriorityNew)
{
    return m_D3Dtex->SetPriority(PriorityNew);
}

DWORD APIENTRY uMod_IDirect3DTexture9::GetPriority()
{
    return m_D3Dtex->GetPriority();
}

void APIENTRY uMod_IDirect3DTexture9::PreLoad()
{
    m_D3Dtex->PreLoad();
}

D3DRESOURCETYPE APIENTRY uMod_IDirect3DTexture9::GetType()
{
    return m_D3Dtex->GetType();
}

DWORD APIENTRY uMod_IDirect3DTexture9::SetLOD(DWORD LODNew)
{
    return m_D3Dtex->SetLOD(LODNew);
}

DWORD APIENTRY uMod_IDirect3DTexture9::GetLOD()
{
    return m_D3Dtex->GetLOD();
}

DWORD APIENTRY uMod_IDirect3DTexture9::GetLevelCount()
{
    return m_D3Dtex->GetLevelCount();
}

HRESULT APIENTRY uMod_IDirect3DTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
    return m_D3Dtex->SetAutoGenFilterType(FilterType);
}

D3DTEXTUREFILTERTYPE APIENTRY uMod_IDirect3DTexture9::GetAutoGenFilterType()
{
    return m_D3Dtex->GetAutoGenFilterType();
}

void APIENTRY uMod_IDirect3DTexture9::GenerateMipSubLevels()
{
    m_D3Dtex->GenerateMipSubLevels();
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::GetLevelDesc(UINT Level, D3DSURFACE_DESC* pDesc)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->GetLevelDesc(Level, pDesc);
    }
    return m_D3Dtex->GetLevelDesc(Level, pDesc);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->GetSurfaceLevel(Level, ppSurfaceLevel);
    }
    return m_D3Dtex->GetSurfaceLevel(Level, ppSurfaceLevel);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect,CONST RECT* pRect, DWORD Flags)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->LockRect(Level, pLockedRect, pRect, Flags);
    }
    return m_D3Dtex->LockRect(Level, pLockedRect, pRect, Flags);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::UnlockRect(UINT Level)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->UnlockRect(Level);
    }
    return m_D3Dtex->UnlockRect(Level);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DTexture9::AddDirtyRect(CONST RECT* pDirtyRect)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->AddDirtyRect(pDirtyRect);
    }
    return m_D3Dtex->AddDirtyRect(pDirtyRect);
}


HashType uMod_IDirect3DTexture9::GetHash() const
{
    ASSERT(!FAKE);
    IDirect3DTexture9* pTexture = m_D3Dtex;
    if (CrossRef_D3Dtex != nullptr) {
        pTexture = CrossRef_D3Dtex->m_D3Dtex;
    }

    IDirect3DSurface9* pOffscreenSurface = nullptr;
    //IDirect3DTexture9 *pOffscreenTexture = NULL;
    IDirect3DSurface9* pResolvedSurface = nullptr;
    D3DLOCKED_RECT d3dlr;
    D3DSURFACE_DESC desc;

    if (pTexture->GetLevelDesc(0, &desc) != D3D_OK) //get the format and the size of the texture
    {
        Warning("uMod_IDirect3DTexture9::GetHash() Failed: GetLevelDesc \n");
        return 0;
    }

    Message("uMod_IDirect3DTexture9::GetHash() (%d %d) %d\n", desc.Width, desc.Height, desc.Format);


    if (desc.Pool == D3DPOOL_DEFAULT) //get the raw data of the texture
    {
        //Message("uMod_IDirect3DTexture9::GetHash() (D3DPOOL_DEFAULT)\n");

        IDirect3DSurface9* pSurfaceLevel_orig = nullptr;
        if (pTexture->GetSurfaceLevel(0, &pSurfaceLevel_orig) != D3D_OK) {
            Warning("uMod_IDirect3DTexture9::GetHash() Failed: GetSurfaceLevel 1  (D3DPOOL_DEFAULT)\n");
            return 0;
        }

        if (desc.MultiSampleType != D3DMULTISAMPLE_NONE) {
            //Message("uMod_IDirect3DTexture9::GetHash() MultiSampleType\n");
            if (D3D_OK != m_D3Ddev->CreateRenderTarget(desc.Width, desc.Height, desc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &pResolvedSurface, nullptr)) {
                pSurfaceLevel_orig->Release();
                Warning("uMod_IDirect3DTexture9::GetHash() Failed: CreateRenderTarget  (D3DPOOL_DEFAULT)\n");
                return 0;
            }
            if (D3D_OK != m_D3Ddev->StretchRect(pSurfaceLevel_orig, nullptr, pResolvedSurface, nullptr, D3DTEXF_NONE)) {
                pSurfaceLevel_orig->Release();
                Warning("uMod_IDirect3DTexture9::GetHash() Failed: StretchRect  (D3DPOOL_DEFAULT)\n");
                return 0;
            }

            pSurfaceLevel_orig = pResolvedSurface;
        }

        if (D3D_OK != m_D3Ddev->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &pOffscreenSurface, nullptr)) {
            pSurfaceLevel_orig->Release();
            if (pResolvedSurface != nullptr) {
                pResolvedSurface->Release();
            }
            Warning("uMod_IDirect3DTexture9::GetHash() Failed: CreateOffscreenPlainSurface (D3DPOOL_DEFAULT)\n");
            return 0;
        }

        if (D3D_OK != m_D3Ddev->GetRenderTargetData(pSurfaceLevel_orig, pOffscreenSurface)) {
            pSurfaceLevel_orig->Release();
            if (pResolvedSurface != nullptr) {
                pResolvedSurface->Release();
            }
            pOffscreenSurface->Release();
            Warning("uMod_IDirect3DTexture9::GetHash() Failed: GetRenderTargetData (D3DPOOL_DEFAULT)\n");
            return 0;
        }
        pSurfaceLevel_orig->Release();

        if (pOffscreenSurface->LockRect(&d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
            if (pResolvedSurface != nullptr) {
                pResolvedSurface->Release();
            }
            pOffscreenSurface->Release();
            Warning("uMod_IDirect3DTexture9::GetHash() Failed: LockRect (D3DPOOL_DEFAULT)\n");
            return 0;
        }
    }
    else if (pTexture->LockRect(0, &d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
        Warning("uMod_IDirect3DTexture9::GetHash() Failed: LockRect 1\n");
        if (pTexture->GetSurfaceLevel(0, &pResolvedSurface) != D3D_OK) {
            Warning("uMod_IDirect3DTexture9::GetHash() Failed: GetSurfaceLevel\n");
            return 0;
        }
        if (pResolvedSurface->LockRect(&d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
            pResolvedSurface->Release();
            Warning("uMod_IDirect3DTexture9::GetHash() Failed: LockRect 2\n");
            return 0;
        }
    }

    const int size = (GetBitsFromFormat(desc.Format) * desc.Width * desc.Height) / 8;
    const auto hash = GetCRC32(static_cast<char*>(d3dlr.pBits), size); //calculate the crc32 of the texture

    // Only release surfaces after we're finished with d3dlr
    if (pOffscreenSurface != nullptr) {
        pOffscreenSurface->UnlockRect();
        pOffscreenSurface->Release();
        if (pResolvedSurface != nullptr) {
            pResolvedSurface->Release();
        }
    }
    else if (pResolvedSurface != nullptr) {
        pResolvedSurface->UnlockRect();
        pResolvedSurface->Release();
    }
    else {
        pTexture->UnlockRect(0);
    }
    Message("uMod_IDirect3DTexture9::GetHash() %#lX (%d %d) %d = %d\n", hash, desc.Width, desc.Height, desc.Format, size);
    return hash;
}
