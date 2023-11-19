#include <filesystem>
#include <fstream>

#include "uMod_Main.h"

uMod_TextureClient::uMod_TextureClient(IDirect3DDevice9* device)
{
    Message("uMod_TextureClient::uMod_TextureClient(): %p\n", this);
    D3D9Device = device;
    NumberToMod = 0;
    FileToMod = nullptr;

    for (int i = 0; i < NumberToMod; i++) {
        FileToMod[i].NumberOfTextures = 0;
        FileToMod[i].Textures = nullptr;
    }
    Mutex = CreateMutex(nullptr, false, nullptr);

    Update = nullptr;
    NumberOfUpdate = -1;
    FontColour = D3DCOLOR_ARGB(255, 255, 0, 0);
    TextureColour = D3DCOLOR_ARGB(255, 0, 255, 0);
}

uMod_TextureClient::~uMod_TextureClient()
{
    Message("uMod_TextureClient::~uMod_TextureClient(): %p\n", this);
    if (Mutex != nullptr) {
        CloseHandle(Mutex);
    }

    delete [] Update;

    if (FileToMod != nullptr) {
        for (int i = 0; i < NumberToMod; i++) {

            delete [] FileToMod[i].Textures;

        }
        delete [] FileToMod;
    }
}


int uMod_TextureClient::AddTexture(uMod_IDirect3DTexture9* pTexture)
{
    void* cpy;
    const long ret = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy);
    if (ret == 0x01000000L) {
        static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedTexture(nullptr); //this texture must no be added twice
    }
    else {
        static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedTexture(nullptr); //this texture must no be added twice
    }

    if (pTexture->FAKE) {
        return RETURN_OK; // this is a fake texture
    }

    Message("uMod_TextureClient::AddTexture( %p): %p (thread: %p)\n", pTexture, this, GetCurrentThreadId());

    pTexture->Hash = pTexture->GetHash();
    if (!pTexture->Hash) {
        return RETURN_FATAL_ERROR;
    }

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    OriginalTextures.Add(pTexture); // add the texture to the list of original texture

    return LookUpToMod(pTexture); // check if this texture should be modded
}

int uMod_TextureClient::AddTexture(uMod_IDirect3DVolumeTexture9* pTexture)
{
    void* cpy;
    const long ret = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy);
    if (ret == 0x01000000L) {
        static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedVolumeTexture(nullptr); //this texture must no be added twice
    }
    else {
        static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedVolumeTexture(nullptr); //this texture must no be added twice
    }

    if (pTexture->FAKE) {
        return RETURN_OK; // this is a fake texture
    }

    Message("uMod_TextureClient::AddTexture( Volume: %p): %p (thread: %p)\n", pTexture, this, GetCurrentThreadId());

    pTexture->Hash = pTexture->GetHash();
    if (!pTexture->Hash) {
        return RETURN_FATAL_ERROR;
    }

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    OriginalVolumeTextures.Add(pTexture); // add the texture to the list of original texture

    return LookUpToMod(pTexture); // check if this texture should be modded
}

int uMod_TextureClient::AddTexture(uMod_IDirect3DCubeTexture9* pTexture)
{
    void* cpy;
    const long ret = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy);
    if (ret == 0x01000000L) {
        static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedCubeTexture(nullptr); //this texture must no be added twice
    }
    else {
        static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedCubeTexture(nullptr); //this texture must no be added twice
    }

    if (pTexture->FAKE) {
        return RETURN_OK; // this is a fake texture
    }

    Message("uMod_TextureClient::AddTexture( Cube: %p): %p (thread: %p)\n", pTexture, this, GetCurrentThreadId());

    pTexture->Hash = pTexture->GetHash();
    if (!pTexture->Hash) {
        return RETURN_FATAL_ERROR;
    }

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }

    OriginalCubeTextures.Add(pTexture); // add the texture to the list of original texture

    return LookUpToMod(pTexture); // check if this texture should be modded
}


int uMod_TextureClient::RemoveTexture(uMod_IDirect3DTexture9* pTexture) // is called from a texture, if it is finally released
{
    Message("uMod_TextureClient::RemoveTexture( %p, %#lX): %p\n", pTexture, pTexture->Hash, this);

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }
    if (pTexture->FAKE) {
        // we need to set the corresponding FileToMod[X].pTexture to NULL, to avoid a link to a non existing texture object
        const int ref = pTexture->Reference;
        if (ref >= 0 && ref < NumberToMod) {
            for (int i = 0; i < FileToMod[ref].NumberOfTextures; i++) {
                if (FileToMod[ref].Textures[i] == pTexture) {
                    FileToMod[ref].NumberOfTextures--;
                    for (int j = i; j < FileToMod[ref].NumberOfTextures; j++) {
                        FileToMod[ref].Textures[j] = FileToMod[ref].Textures[j + 1];
                    }
                    FileToMod[ref].Textures[FileToMod[ref].NumberOfTextures] = nullptr;
                    break;
                }
            }
        }
    }
    else {
        return OriginalTextures.Remove(pTexture); //remove this texture form the list
    }
    return RETURN_OK;
}

int uMod_TextureClient::RemoveTexture(uMod_IDirect3DVolumeTexture9* pTexture) // is called from a texture, if it is finally released
{
    Message("uMod_TextureClient::RemoveTexture( Volume %p, %#lX): %p\n", pTexture, pTexture->Hash, this);

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }
    if (pTexture->FAKE) {
        // we need to set the corresponding FileToMod[X].pTexture to NULL, to avoid a link to a non existing texture object
        const int ref = pTexture->Reference;
        if (ref >= 0 && ref < NumberToMod) {
            for (int i = 0; i < FileToMod[ref].NumberOfTextures; i++) {
                if (FileToMod[ref].Textures[i] == pTexture) {
                    FileToMod[ref].NumberOfTextures--;
                    for (int j = i; j < FileToMod[ref].NumberOfTextures; j++) {
                        FileToMod[ref].Textures[j] = FileToMod[ref].Textures[j + 1];
                    }
                    FileToMod[ref].Textures[FileToMod[ref].NumberOfTextures] = nullptr;
                    break;
                }
            }
        }
    }
    else {
        return OriginalVolumeTextures.Remove(pTexture); //remove this texture form the list
    }
    return RETURN_OK;
}

int uMod_TextureClient::RemoveTexture(uMod_IDirect3DCubeTexture9* pTexture) // is called from a texture, if it is finally released
{
    Message("uMod_TextureClient::RemoveTexture( Cube %p, %#lX): %p\n", pTexture, pTexture->Hash, this);

    if (gl_ErrorState & uMod_ERROR_FATAL) {
        return RETURN_FATAL_ERROR;
    }
    if (pTexture->FAKE) {
        // we need to set the corresponding FileToMod[X].pTexture to NULL, to avoid a link to a non existing texture object
        const int ref = pTexture->Reference;
        if (ref >= 0 && ref < NumberToMod) {
            for (int i = 0; i < FileToMod[ref].NumberOfTextures; i++) {
                if (FileToMod[ref].Textures[i] == pTexture) {
                    FileToMod[ref].NumberOfTextures--;
                    for (int j = i; j < FileToMod[ref].NumberOfTextures; j++) {
                        FileToMod[ref].Textures[j] = FileToMod[ref].Textures[j + 1];
                    }
                    FileToMod[ref].Textures[FileToMod[ref].NumberOfTextures] = nullptr;
                    break;
                }
            }
        }
    }
    else {
        return OriginalCubeTextures.Remove(pTexture); //remove this texture form the list
    }
    return RETURN_OK;
}


int uMod_TextureClient::AddUpdate(TextureFileStruct* update, int number)  //client must delete the update array
{
    Message("AddUpdate( %p, %d): %p\n", update, number, this);
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_TEXTURE;
        return ret;
    }

    delete [] Update;

    Update = update;
    NumberOfUpdate = number;
    return UnlockMutex();
}


int uMod_TextureClient::MergeUpdate()
{
    if (NumberOfUpdate < 0) { return RETURN_OK; }
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_TEXTURE;
        return ret;
    }

    Message("MergeUpdate(): %p\n", this);

    for (int i = 0; i < NumberOfUpdate; i++) {
        Update[i].NumberOfTextures = 0;
        Update[i].Textures = nullptr;
    } // this is already done, but safety comes first ^^

    int pos_old = 0;
    int pos_new = 0;
    int* to_lookup = nullptr;
    if (NumberOfUpdate > 0) {
        to_lookup = new int[NumberOfUpdate];
    }
    int num_to_lookup = 0;

    /*
     * FileToMod contains the old files (textures) which should replace the target textures (if they are loaded by the game)
     * Update contains the new files (textures) which should replace the target textures (if they are loaded by the game)
     *
     * Both arrays (FileToMod and Update) are sorted according to their hash values.
     *
     * First we go through both arrays linearly and
     * 1) take over the old entry if the hash is the same,
     * 2) release old fake texture (if target texture exist and is not in the Update)
     * 3) or mark newly added fake texture (if they are not in FileToMod)
     */

    while (pos_old < NumberToMod && pos_new < NumberOfUpdate) {
        if (FileToMod[pos_old].crc_hash > Update[pos_new].crc_hash) // this fake texture is new
        {
            to_lookup[num_to_lookup++] = pos_new++; // keep this fake texture in mind, we must search later for it through all original textures
            // we increase only the new counter by one
        }
        else if (FileToMod[pos_old].crc_hash < Update[pos_new].crc_hash) // this fake texture is not in the update
        {
            for (int i = FileToMod[pos_old].NumberOfTextures - 1; i >= 0; i--) {
                FileToMod[pos_old].Textures[i]->Release(); // we release the fake textures
            }

            delete [] FileToMod[pos_old].Textures; // we delete the memory

            FileToMod[pos_old].NumberOfTextures = 0;
            FileToMod[pos_old].Textures = nullptr;

            pos_old++; // we increase only the old counter by one
        }
        else // the hash value is the same, thus this texture is in the array FileToMod as well as in the array Update
        {
            // the texture might be loaded or not
            Update[pos_new].NumberOfTextures = FileToMod[pos_old].NumberOfTextures;
            Update[pos_new].Textures = FileToMod[pos_old].Textures;
            FileToMod[pos_old].NumberOfTextures = 0;
            FileToMod[pos_old].Textures = nullptr;
            // we increase both counters by one
            pos_old++;
            pos_new++;
        }
    }

    while (pos_old < NumberToMod) //this fake textures are not in the Update
    {
        for (int i = FileToMod[pos_old].NumberOfTextures - 1; i >= 0; i--) {
            FileToMod[pos_old].Textures[i]->Release(); // we release the fake textures
        }
        //for (int i=0; i<FileToMod[pos_old].NumberOfTextures; i++) FileToMod[pos_old].Textures[i]->Release(); // we release the fake textures

        delete [] FileToMod[pos_old].Textures; // we delete the memory

        FileToMod[pos_old].Textures = nullptr;
        pos_old++;
    }
    while (pos_new < NumberOfUpdate) //this fake textures are newly added
    {
        to_lookup[num_to_lookup++] = pos_new++; //keep this fake texture in mind, we must search later for it through all original textures
    }

    delete [] FileToMod;

    FileToMod = Update;
    NumberToMod = NumberOfUpdate;

    NumberOfUpdate = -1;
    Update = nullptr;


    if (num_to_lookup > 0) {
        uMod_IDirect3DTexture9* single_texture;
        void* cpy;
        const long ret = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy);
        if (ret == 0x01000000L) {
            single_texture = static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->GetSingleTexture(); //this texture must no be added twice
        }
        else {
            single_texture = static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->GetSingleTexture(); //this texture must no be added twice
        }

        int num = OriginalTextures.GetNumber();
        for (int i = 0; i < num; i++) {
            if (OriginalTextures[i]->CrossRef_D3Dtex == nullptr || OriginalTextures[i]->CrossRef_D3Dtex == single_texture) {
                UnswitchTextures(OriginalTextures[i]); //this we can do always, so we unswitch the single texture
                LookUpToMod(OriginalTextures[i], num_to_lookup, to_lookup);
            }
        }

        uMod_IDirect3DVolumeTexture9* single_volume_texture;
        if (ret == 0x01000000L) {
            single_volume_texture = static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->GetSingleVolumeTexture(); //this texture must no be added twice
        }
        else {
            single_volume_texture = static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->GetSingleVolumeTexture(); //this texture must no be added twice
        }
        num = OriginalVolumeTextures.GetNumber();
        for (int i = 0; i < num; i++) {
            if (OriginalVolumeTextures[i]->CrossRef_D3Dtex == nullptr || OriginalVolumeTextures[i]->CrossRef_D3Dtex == single_volume_texture) {
                UnswitchTextures(OriginalVolumeTextures[i]); //this we can do always, so we unswitch the single texture
                LookUpToMod(OriginalVolumeTextures[i], num_to_lookup, to_lookup);
            }
        }

        uMod_IDirect3DCubeTexture9* single_cube_texture;
        if (ret == 0x01000000L) {
            single_cube_texture = static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->GetSingleCubeTexture(); //this texture must no be added twice
        }
        else {
            single_cube_texture = static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->GetSingleCubeTexture(); //this texture must no be added twice
        }
        num = OriginalCubeTextures.GetNumber();
        for (int i = 0; i < num; i++) {
            if (OriginalCubeTextures[i]->CrossRef_D3Dtex == nullptr || OriginalCubeTextures[i]->CrossRef_D3Dtex == single_cube_texture) {
                UnswitchTextures(OriginalCubeTextures[i]); //this we can do always, so we unswitch the single texture
                LookUpToMod(OriginalCubeTextures[i], num_to_lookup, to_lookup);
            }
        }
    }

    delete [] to_lookup;

    return UnlockMutex();
}



int uMod_TextureClient::LockMutex()
{
    if ((gl_ErrorState & (uMod_ERROR_FATAL | uMod_ERROR_MUTEX))) {
        return RETURN_NO_MUTEX;
    }
    if (WAIT_OBJECT_0 != WaitForSingleObject(Mutex, 100)) {
        return RETURN_MUTEX_LOCK; //waiting 100ms, to wait infinite pass INFINITE
    }
    return RETURN_OK;
}

int uMod_TextureClient::UnlockMutex()
{
    if (ReleaseMutex(Mutex) == 0) {
        return RETURN_MUTEX_UNLOCK;
    }
    return RETURN_OK;
}


int uMod_TextureClient::LookUpToMod(HashType hash, int num_index_list, int* index_list)
{
    if (NumberToMod > 0) {
        if (index_list == nullptr || num_index_list == 0) {
            if (hash < FileToMod[0].crc_hash || hash > FileToMod[NumberToMod - 1].crc_hash) {
                return -1;
            }
            int pos = NumberToMod / 2;
            int begin = 0;
            int end = NumberToMod - 1;

            // We look in the middle of the interval and each step we halve the interval,
            // unless we find the texture or the size of the interval is less than 3.
            // Note: contradicting to normal C-code here the interval includes the index "begin" and "end"!
            while (begin + 1 < end) // as long as the interval is longer than two
            {
                if (hash > FileToMod[pos].crc_hash) // the new interval is the right half of the actual interval
                {
                    begin = pos + 1; // the new interval does not contain the index "pos"
                    pos = (begin + end) / 2; // set "pos" somewhere inside the new interval
                }
                else if (hash < FileToMod[pos].crc_hash) // the new interval is the left half of the actual interval
                {
                    end = pos - 1; // the new interval does not contain the index "pos"
                    pos = (begin + end) / 2; // set "pos" somewhere inside the new interval
                }
                else {
                    return pos;
                    break;
                } // we hit the correct hash
            }
            for (pos = begin; pos <= end; pos++) {
                if (FileToMod[pos].crc_hash == hash) {
                    return pos;
                }
            }
        }
        else {
            if (hash < FileToMod[index_list[0]].crc_hash || hash > FileToMod[index_list[num_index_list - 1]].crc_hash) {
                return -1;
            }
            int pos = num_index_list / 2;
            int begin = 0;
            int end = num_index_list - 1;

            // We look in the middle of the interval and each step we halve the interval,
            // unless we find the texture or the size of the interval is less than 3.
            // Note: contradicting to normal C-code here the interval includes the index "begin" and "end"!
            while (begin + 1 < end) // as long as the interval is longer than two
            {
                if (hash > FileToMod[index_list[pos]].crc_hash) // the new interval is the right half of the actual interval
                {
                    begin = pos + 1; // the new interval does not contain the index "pos"
                    pos = (begin + end) / 2; // set "pos" somewhere inside the new interval
                }
                else if (hash < FileToMod[index_list[pos]].crc_hash) // the new interval is the left half of the actual interval
                {
                    end = pos - 1; // the new interval does not contain the index "pos"
                    pos = (begin + end) / 2; // set "pos" somewhere inside the new interval
                }
                else {
                    return index_list[pos];
                    break;
                } // we hit the correct hash
            }
            for (pos = begin; pos <= end; pos++) {
                if (FileToMod[index_list[pos]].crc_hash == hash) {
                    return index_list[pos];
                }
            }
        }
    }
    return -1;
}

int uMod_TextureClient::LookUpToMod(uMod_IDirect3DTexture9* pTexture, int num_index_list, int* index_list) // should only be called for original textures
{
    Message("uMod_TextureClient::LookUpToMod( %p): hash: %#lX,  %p\n", pTexture, pTexture->Hash, this);
    if (pTexture->CrossRef_D3Dtex != nullptr) {
        return RETURN_OK; // bug, this texture is already switched
    }
    const int index = LookUpToMod(pTexture->Hash, num_index_list, index_list);
    if (index >= 0) {
        uMod_IDirect3DTexture9* fake_Texture;
        if (const int ret = LoadTexture(&(FileToMod[index]), &fake_Texture)) {
            return ret;
        }
        if (SwitchTextures(fake_Texture, pTexture)) {
            Message("uMod_TextureClient::LookUpToMod(): textures not switched %#lX\n", FileToMod[index].crc_hash);
            fake_Texture->Release();
        }
        else {
            const auto temp = new IDirect3DBaseTexture9*[FileToMod[index].NumberOfTextures + 1];
            for (int j = 0; j < FileToMod[index].NumberOfTextures; j++) {
                temp[j] = FileToMod[index].Textures[j];
            }

            delete [] FileToMod[index].Textures;

            FileToMod[index].Textures = temp;

            FileToMod[index].Textures[FileToMod[index].NumberOfTextures++] = fake_Texture;
            fake_Texture->Reference = index;
        }
    }
    return RETURN_OK;
}

int uMod_TextureClient::LookUpToMod(uMod_IDirect3DVolumeTexture9* pTexture, int num_index_list, int* index_list) // should only be called for original textures
{
    Message("uMod_TextureClient::LookUpToMod( Volume %p): hash: %#lX,  %p\n", pTexture, pTexture->Hash, this);
    if (pTexture->CrossRef_D3Dtex != nullptr) {
        return RETURN_OK; // bug, this texture is already switched
    }
    const int index = LookUpToMod(pTexture->Hash, num_index_list, index_list);
    if (index >= 0) {
        uMod_IDirect3DVolumeTexture9* fake_Texture;
        if (const int ret = LoadTexture(&(FileToMod[index]), &fake_Texture)) {
            return ret;
        }
        if (SwitchTextures(fake_Texture, pTexture)) {
            Message("uMod_TextureClient::LookUpToMod(): textures not switched %#lX\n", FileToMod[index].crc_hash);
            fake_Texture->Release();
        }
        else {
            const auto temp = new IDirect3DBaseTexture9*[FileToMod[index].NumberOfTextures + 1];
            for (int j = 0; j < FileToMod[index].NumberOfTextures; j++) {
                temp[j] = FileToMod[index].Textures[j];
            }

            delete [] FileToMod[index].Textures;

            FileToMod[index].Textures = temp;

            FileToMod[index].Textures[FileToMod[index].NumberOfTextures++] = fake_Texture;
            fake_Texture->Reference = index;
        }
    }
    return RETURN_OK;
}

int uMod_TextureClient::LookUpToMod(uMod_IDirect3DCubeTexture9* pTexture, int num_index_list, int* index_list) // should only be called for original textures
{
    Message("uMod_TextureClient::LookUpToMod( Cube %p): hash: %#lX,  %p\n", pTexture, pTexture->Hash, this);
    if (pTexture->CrossRef_D3Dtex != nullptr) {
        return RETURN_OK; // bug, this texture is already switched
    }
    const int index = LookUpToMod(pTexture->Hash, num_index_list, index_list);
    if (index >= 0) {
        uMod_IDirect3DCubeTexture9* fake_Texture;
        if (const int ret = LoadTexture(&(FileToMod[index]), &fake_Texture)) {
            return ret;
        }
        if (SwitchTextures(fake_Texture, pTexture)) {
            Message("uMod_TextureClient::LookUpToMod(): textures not switched %#lX\n", FileToMod[index].crc_hash);
            fake_Texture->Release();
        }
        else {
            const auto temp = new IDirect3DBaseTexture9*[FileToMod[index].NumberOfTextures + 1];
            for (int j = 0; j < FileToMod[index].NumberOfTextures; j++) {
                temp[j] = FileToMod[index].Textures[j];
            }

            delete [] FileToMod[index].Textures;
            FileToMod[index].Textures = temp;

            FileToMod[index].Textures[FileToMod[index].NumberOfTextures++] = fake_Texture;
            fake_Texture->Reference = index;
        }
    }
    return RETURN_OK;
}

int uMod_TextureClient::LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DTexture9** ppTexture) // to load fake texture from a file in memory
{
    Message("LoadTexture( %p, %p, %#lX): %p\n", file_in_memory, ppTexture, file_in_memory->crc_hash, this);
    if (D3D_OK != D3DXCreateTextureFromFileInMemoryEx(D3D9Device, file_in_memory->data.data(), file_in_memory->data.size(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr,
                                                      (IDirect3DTexture9**)ppTexture))
    //if (D3D_OK != D3DXCreateTextureFromFileInMemory( D3D9Device, file_in_memory->pData, file_in_memory->Size, (IDirect3DTexture9 **) ppTexture))
    {
        *ppTexture = nullptr;
        return RETURN_TEXTURE_NOT_LOADED;
    }
    (*ppTexture)->FAKE = true;

    void* cpy;
    const long ret = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy);
    if (ret == 0x01000000L) {
        static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedTexture(nullptr); //this texture must no be added twice
    }
    else {
        static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedTexture(nullptr); //this texture must no be added twice
    }

    Message("LoadTexture( %p, %#lX): DONE\n", *ppTexture, file_in_memory->crc_hash);
    return RETURN_OK;
}

int uMod_TextureClient::LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DVolumeTexture9** ppTexture) // to load fake texture from a file in memory
{
    Message("LoadTexture( Volume %p, %p, %#lX): %p\n", file_in_memory, ppTexture, file_in_memory->crc_hash, this);
    if (D3D_OK != D3DXCreateVolumeTextureFromFileInMemoryEx(D3D9Device, file_in_memory->data.data(), file_in_memory->data.size(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                                            nullptr,
                                                            nullptr,
                                                            (IDirect3DVolumeTexture9**)ppTexture))
    //if (D3D_OK != D3DXCreateVolumeTextureFromFileInMemory( D3D9Device, file_in_memory->pData, file_in_memory->Size, (IDirect3DVolumeTexture9 **) ppTexture))
    {
        *ppTexture = nullptr;
        return RETURN_TEXTURE_NOT_LOADED;
    }
    (*ppTexture)->FAKE = true;

    void* cpy;
    const long ret = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy);
    if (ret == 0x01000000L) {
        static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedVolumeTexture(nullptr); //this texture must no be added twice
    }
    else {
        static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedVolumeTexture(nullptr); //this texture must no be added twice
    }

    Message("LoadTexture( Volume %p, %#lX): DONE\n", *ppTexture, file_in_memory->crc_hash);
    return RETURN_OK;
}

int uMod_TextureClient::LoadTexture(TextureFileStruct* file_in_memory, uMod_IDirect3DCubeTexture9** ppTexture) // to load fake texture from a file in memory
{
    Message("LoadTexture( Cube %p, %p, %#lX): %p\n", file_in_memory, ppTexture, file_in_memory->crc_hash, this);
    if (D3D_OK != D3DXCreateCubeTextureFromFileInMemoryEx(D3D9Device, file_in_memory->data.data(), file_in_memory->data.size(), D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, nullptr, nullptr,
                                                          (IDirect3DCubeTexture9**)ppTexture))
    //if (D3D_OK != D3DXCreateCubeTextureFromFileInMemory( D3D9Device, file_in_memory->pData, file_in_memory->Size, (IDirect3DCubeTexture9 **) ppTexture))
    {
        *ppTexture = nullptr;
        return RETURN_TEXTURE_NOT_LOADED;
    }
    (*ppTexture)->FAKE = true;

    void* cpy;
    const long ret = D3D9Device->QueryInterface(IID_IDirect3DTexture9, &cpy);
    if (ret == 0x01000000L) {
        static_cast<uMod_IDirect3DDevice9*>(D3D9Device)->SetLastCreatedCubeTexture(nullptr); //this texture must no be added twice
    }
    else {
        static_cast<uMod_IDirect3DDevice9Ex*>(D3D9Device)->SetLastCreatedCubeTexture(nullptr); //this texture must no be added twice
    }

    Message("LoadTexture( Cube %p, %#lX): DONE\n", *ppTexture, file_in_memory->crc_hash);
    return RETURN_OK;
}

bool uMod_TextureClient::AddFile(TpfEntry entry)
{
    TextureFileStruct texture_file_struct;
    if (modded_textures.contains(entry.crc_hash)) {
        return false;
    }
    texture_file_struct.data = std::move(entry.data);
    texture_file_struct.crc_hash = entry.crc_hash;
    texture_file_struct.NumberOfTextures = 0;
    texture_file_struct.Reference = -1;
    modded_textures.emplace(entry.crc_hash, texture_file_struct);
    return true;
}

unsigned long loaded_size = 0;

void uMod_TextureClient::LoadModsFromFile(const char* source)
{
    Message("Initialize: searching in %s\n", source);

    std::ifstream file(source);
    if (file.is_open()) {
        Message("Initialize: found modlist.txt. Reading\n");
        std::string line;
        while (std::getline(file, line)) {
            Message("Initialize: loading file %s\n", line.c_str());

            // Remove newline character
            line.erase(std::ranges::remove(line, '\n').begin(), line.end());

            auto file_loader = gMod_FileLoader(line);
            const auto entries = file_loader.GetContents();
            if (loaded_size > 1'500'000'000) {
                Message("LoadModsFromFile: Loaded %d bytes, aborting!!!", loaded_size);
                return;
            }
            if (!entries.empty()) {
                Message("Initialize: Texture count %zu %s\n", entries.size(), line.c_str());
                for (const auto& tpf_entry : entries) {
                    if (AddFile(tpf_entry)) {
                        loaded_size += tpf_entry.data.size();
                    }

                    Message("LoadModsFromFile: Loaded %d bytes", loaded_size);
                }
            }
            else {
                Message("Initialize: Failed to load any textures for %s\n", line.c_str());
            }
        }
        Message("Finished loading mods: Loaded %u bytes (%u mb)", loaded_size, loaded_size / 1024 / 1024);
    }
}

void uMod_TextureClient::Initialize()
{
    Message("Initialize: begin\n");
    Message("Initialize: searching for modlist.txt\n");
    char gwpath[MAX_PATH]{};
    GetModuleFileName(GetModuleHandle(nullptr), gwpath, MAX_PATH); //ask for name and path of this executable
    char dllpath[MAX_PATH]{};
    GetModuleFileName(gl_hThisInstance, dllpath, MAX_PATH); //ask for name and path of this dll
    const auto exe = std::filesystem::path(gwpath).parent_path();
    const auto dll = std::filesystem::path(dllpath).parent_path();
    for (const auto& path : {exe, dll}) {
        const auto modlist = path / "modlist.txt";
        if (std::filesystem::exists(modlist)) {
            Message("Initialize: found %s\n", modlist.string().c_str());
            LoadModsFromFile(modlist.string().c_str());
        }
    }

    Message("Initialize: end\n");
}
