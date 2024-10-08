#include "Main.h"

import ModfileLoader;
import TextureClient;
import TextureFunction;

HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::QueryInterface(REFIID riid, void** ppvObj)
{
    if (riid == IID_IDirect3D9) {
        // This function should never be called with IID_IDirect3D9 by the game
        // thus this call comes from our own dll to ask for the texture type
        // 0x01000000L == uMod_IDirect3DTexture9
        // 0x01000001L == uMod_IDirect3DVolumeTexture9
        // 0x01000002L == uMod_IDirect3DCubeTexture9

        *ppvObj = this;
        return 0x01000001L;
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
ULONG APIENTRY uMod_IDirect3DVolumeTexture9::AddRef()
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
ULONG APIENTRY uMod_IDirect3DVolumeTexture9::Release()
{
    Message("uMod_IDirect3DVolumeTexture9::Release(): %p\n", this);

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
            uMod_IDirect3DVolumeTexture9* fake_texture = CrossRef_D3Dtex;
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
            if (static_cast<uMod_IDirect3DDevice9*>(m_D3Ddev)->GetLastCreatedVolumeTexture() == this) {
                static_cast<uMod_IDirect3DDevice9*>(m_D3Ddev)->SetLastCreatedVolumeTexture(nullptr);
            }
            else {
                static_cast<uMod_IDirect3DDevice9*>(m_D3Ddev)->GetuMod_Client()->RemoveTexture(this); // remove this texture from the texture client
            }
        }
        else {
            if (static_cast<uMod_IDirect3DDevice9Ex*>(m_D3Ddev)->GetLastCreatedVolumeTexture() == this) {
                static_cast<uMod_IDirect3DDevice9Ex*>(m_D3Ddev)->SetLastCreatedVolumeTexture(nullptr);
            }
            else {
                static_cast<uMod_IDirect3DDevice9Ex*>(m_D3Ddev)->GetuMod_Client()->RemoveTexture(this); // remove this texture from the texture client
            }
        }

        delete this;
    }
    return count;
}

HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::GetDevice(IDirect3DDevice9** ppDevice)
{
    *ppDevice = m_D3Ddev;
    return D3D_OK;
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::SetPrivateData(REFGUID refguid,CONST void* pData, DWORD SizeOfData, DWORD Flags)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags);
    }
    return m_D3Dtex->SetPrivateData(refguid, pData, SizeOfData, Flags);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::GetPrivateData(REFGUID refguid, void* pData, DWORD* pSizeOfData)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData);
    }
    return m_D3Dtex->GetPrivateData(refguid, pData, pSizeOfData);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::FreePrivateData(REFGUID refguid)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->FreePrivateData(refguid);
    }
    return m_D3Dtex->FreePrivateData(refguid);
}

DWORD APIENTRY uMod_IDirect3DVolumeTexture9::SetPriority(DWORD PriorityNew)
{
    return m_D3Dtex->SetPriority(PriorityNew);
}

DWORD APIENTRY uMod_IDirect3DVolumeTexture9::GetPriority()
{
    return m_D3Dtex->GetPriority();
}

void APIENTRY uMod_IDirect3DVolumeTexture9::PreLoad()
{
    m_D3Dtex->PreLoad();
}

D3DRESOURCETYPE APIENTRY uMod_IDirect3DVolumeTexture9::GetType()
{
    return m_D3Dtex->GetType();
}

DWORD APIENTRY uMod_IDirect3DVolumeTexture9::SetLOD(DWORD LODNew)
{
    return m_D3Dtex->SetLOD(LODNew);
}

DWORD APIENTRY uMod_IDirect3DVolumeTexture9::GetLOD()
{
    return m_D3Dtex->GetLOD();
}

DWORD APIENTRY uMod_IDirect3DVolumeTexture9::GetLevelCount()
{
    return m_D3Dtex->GetLevelCount();
}

HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType)
{
    return m_D3Dtex->SetAutoGenFilterType(FilterType);
}

D3DTEXTUREFILTERTYPE APIENTRY uMod_IDirect3DVolumeTexture9::GetAutoGenFilterType()
{
    return m_D3Dtex->GetAutoGenFilterType();
}

void APIENTRY uMod_IDirect3DVolumeTexture9::GenerateMipSubLevels()
{
    m_D3Dtex->GenerateMipSubLevels();
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::AddDirtyBox(CONST D3DBOX* pDirtyBox)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->AddDirtyBox(pDirtyBox);
    }
    return m_D3Dtex->AddDirtyBox(pDirtyBox);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::GetLevelDesc(UINT Level, D3DVOLUME_DESC* pDesc)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->GetLevelDesc(Level, pDesc);
    }
    return m_D3Dtex->GetLevelDesc(Level, pDesc);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::GetVolumeLevel(UINT Level, IDirect3DVolume9** ppVolumeLevel)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->GetVolumeLevel(Level, ppVolumeLevel);
    }
    return m_D3Dtex->GetVolumeLevel(Level, ppVolumeLevel);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::LockBox(UINT Level, D3DLOCKED_BOX* pLockedVolume, CONST D3DBOX* pBox, DWORD Flags)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->LockBox(Level, pLockedVolume, pBox, Flags);
    }
    return m_D3Dtex->LockBox(Level, pLockedVolume, pBox, Flags);
}

//this function yields for the non switched texture object
HRESULT APIENTRY uMod_IDirect3DVolumeTexture9::UnlockBox(UINT Level)
{
    if (CrossRef_D3Dtex != nullptr) {
        return CrossRef_D3Dtex->m_D3Dtex->UnlockBox(Level);
    }
    return m_D3Dtex->UnlockBox(Level);
}

HashTuple uMod_IDirect3DVolumeTexture9::GetHash() const
{
    if (FAKE) {
        return {};
    }
    IDirect3DVolumeTexture9* pTexture = m_D3Dtex;
    if (CrossRef_D3Dtex != nullptr) {
        pTexture = CrossRef_D3Dtex->m_D3Dtex;
    }

    IDirect3DVolume9* pResolvedSurface = nullptr;
    D3DLOCKED_BOX d3dlr;
    D3DVOLUME_DESC desc;

    if (pTexture->GetLevelDesc(0, &desc) != D3D_OK) //get the format and the size of the texture
    {
        Warning("uMod_IDirect3DVolumeTexture9::GetHash() Failed: GetLevelDesc \n");
        return {};
    }

    Message("uMod_IDirect3DVolumeTexture9::GetHash() (%d %d %d) %d\n", desc.Width, desc.Height, desc.Depth, desc.Format);
    if (pTexture->LockBox(0, &d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
        Warning("uMod_IDirect3DVolumeTexture9::GetHash() Failed: LockRect 1\n");
        if (pTexture->GetVolumeLevel(0, &pResolvedSurface) != D3D_OK) {
            Warning("uMod_IDirect3DVolumeTexture9::GetHash() Failed: GetSurfaceLevel\n");
            return {};
        }
        if (pResolvedSurface->LockBox(&d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
            pResolvedSurface->Release();
            Warning("uMod_IDirect3DVolumeTexture9::GetHash() Failed: LockRect 2\n");
            return {};
        }
    }

    const int size = (TextureFunction::GetBitsFromFormat(desc.Format) * desc.Width * desc.Height * desc.Depth) / 8;
    const auto crc32 = TextureFunction::get_crc32(static_cast<char*>(d3dlr.pBits), size);
    const auto crc64 = HashCheck::Use64BitCrc() ? TextureFunction::get_crc64(static_cast<char*>(d3dlr.pBits), size) : 0;

    // Only release surfaces after we're finished with d3dlr
    if (pResolvedSurface != nullptr) {
        pResolvedSurface->UnlockBox();
        pResolvedSurface->Release();
    }
    else {
        pTexture->UnlockBox(0);
    }

    Message("uMod_IDirect3DVolumeTexture9::GetHash() crc32 %#lX (%d %d) %d = %d\n", crc32, desc.Width, desc.Height, desc.Format, size);
    Message("uMod_IDirect3DVolumeTexture9::GetHash() crc64 %#llX (%d %d) %d = %d\n", crc64, desc.Width, desc.Height, desc.Format, size);
    return {crc32, crc64};
}
