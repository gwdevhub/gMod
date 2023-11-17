#include "uMod_Main.h"
#include "gMod_FileLoader.h"

long long loadedSize = 0;

uMod_TextureServer::uMod_TextureServer(char* game, char* uModName)
{
    Message("uMod_TextureServer(): %p\n", this);

    Mutex = CreateMutex(nullptr, false, nullptr);

    Clients = nullptr;
    NumberOfClients = 0;
    LenghtOfClients = 0;
    BoolSaveAllTextures = false;
    BoolSaveSingleTexture = false;
    SavePath[0] = 0;

    int len = 0;
    int path_pos = 0;
    int dot_pos = 0;
    for (len = 0; len < MAX_PATH && (game[len]); len++) {
        if (game[len] == '\\' || game[len] == '/') {
            path_pos = len + 1;
        }
        else if (game[len] == '.') {
            dot_pos = len;
        }
    }

    if (dot_pos > path_pos) {
        len = dot_pos - path_pos;
    }
    else {
        len -= path_pos;
    }

    for (int i = 0; i < len; i++) {
        GameName[i] = game[i + path_pos];
    }

    if (len < MAX_PATH) {
        GameName[len] = 0;
    }
    else {
        GameName[0] = 0;
    }

    for (len = 0; len < MAX_PATH; len++) {
        UModName[len] = uModName[len];
    }

    KeyBack = 0;
    KeySave = 0;
    KeyNext = 0;

    FontColour = 0u;
    TextureColour = 0u;

    Pipe.In = INVALID_HANDLE_VALUE;
    Pipe.Out = INVALID_HANDLE_VALUE;
}

uMod_TextureServer::~uMod_TextureServer()
{
    Message("~uMod_TextureServer(): %p\n", this);
    if (Mutex != nullptr) {
        CloseHandle(Mutex);
    }

    //delete the files in memory
    int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++) {
        delete[] CurrentMod[i]->pData; //delete the file content of the texture
    }

    num = OldMod.GetNumber();
    for (int i = 0; i < num; i++) {
        delete[] OldMod[i]->pData; //delete the file content of the texture
    }

    if (Pipe.In != INVALID_HANDLE_VALUE) {
        CloseHandle(Pipe.In);
    }
    Pipe.In = INVALID_HANDLE_VALUE;
    if (Pipe.Out != INVALID_HANDLE_VALUE) {
        CloseHandle(Pipe.Out);
    }
    Pipe.Out = INVALID_HANDLE_VALUE;
}

int uMod_TextureServer::AddClient(uMod_TextureClient* client, TextureFileStruct** update, int* number) // called from a client
{
    Message("AddClient(%p): %p\n", client, this);
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return ret;
    }

    // the following functions must not change the original uMod_IDirect3DDevice9 object
    // somehow on game start some uMod_IDirect3DDevice9 object are created, which must rest unchanged!!
    // these objects are released and are not used for rendering
    client->SetGameName(GameName);
    client->SaveAllTextures(BoolSaveAllTextures);
    client->SaveSingleTexture(BoolSaveSingleTexture);
    client->SetSaveDirectory(SavePath);
    if (KeyBack > 0) {
        client->SetKeyBack(KeyBack);
    }
    if (KeySave > 0) {
        client->SetKeySave(KeySave);
    }
    if (KeyNext > 0) {
        client->SetKeyNext(KeyNext);
    }

    if (FontColour > 0u) {
        const DWORD r = FontColour >> 16 & 0xFF;
        const DWORD g = FontColour >> 8 & 0xFF;
        const DWORD b = FontColour & 0xFF;
        client->SetFontColour(r, g, b);
    }
    if (TextureColour > 0u) {
        const DWORD r = TextureColour >> 16 & 0xFF;
        const DWORD g = TextureColour >> 8 & 0xFF;
        const DWORD b = TextureColour & 0xFF;
        client->SetTextureColour(r, g, b);
    }


    if (const int ret = PrepareUpdate(update, number)) {
        return ret; // get a copy of all texture to be modded
    }


    if (NumberOfClients == LenghtOfClients) //allocate more memory
    {
        uMod_TextureClient** temp = nullptr;
        try { temp = new uMod_TextureClient*[LenghtOfClients + 10]; }
        catch (...) {
            gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_SERVER;
            if (const int ret = UnlockMutex()) {
                return ret;
            }
            return RETURN_NO_MEMORY;
        }
        for (int i = 0; i < LenghtOfClients; i++) {
            temp[i] = Clients[i];
        }

        delete[] Clients;

        Clients = temp;
        LenghtOfClients += 10;
    }
    Clients[NumberOfClients++] = client;

    return UnlockMutex();
}

int uMod_TextureServer::RemoveClient(uMod_TextureClient* client) // called from a client
{
    Message("RemoveClient(): %p\n", client);
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return ret;
    }

    for (int i = 0; i < NumberOfClients; i++) {
        if (client == Clients[i]) {
            NumberOfClients--;
            Clients[i] = Clients[NumberOfClients];
            break;
        }
    }
    return UnlockMutex();
}

int uMod_TextureServer::AddFile(char* dataPtr, unsigned int size, MyTypeHash hash, bool force) // called from Mainloop()
{
    Message("uMod_TextureServer::AddFile( %p %p, %#lX, %d): %p\n", dataPtr, size, hash, force, this);

    TextureFileStruct* temp = nullptr;

    int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++) {
        if (CurrentMod[i]->Hash == hash) //look through all current textures
        {
            if (force) {
                temp = CurrentMod[i];
                break;
            } // we need to reload it


            return RETURN_EXISTS; // we still have added this texture
        }
    }
    if (temp == nullptr) // if not found, look through all old textures
    {
        num = OldMod.GetNumber();
        for (int i = 0; i < num; i++) {
            if (OldMod[i]->Hash == hash) {
                temp = OldMod[i];
                OldMod.Remove(temp);
                CurrentMod.Add(temp);
                if (force) {
                    break; // we must reload it
                }

                return RETURN_EXISTS; // we should not reload it
            }
        }
    }

    bool new_file = true;
    if (temp != nullptr) //if it was found, we delete the old file content
    {
        new_file = false;

        delete[] temp->pData;

        temp->pData = nullptr;
    }
    else //if it was not found, we need to create a new object
    {
        new_file = true;
        temp = new TextureFileStruct;
        temp->Reference = -1;
    }

    temp->pData = dataPtr;
    temp->Size = size;
    temp->NumberOfTextures = 0;
    temp->Textures = nullptr;
    temp->Hash = hash;

    //if (new_file) temp->ForceReload = false; // no need to force a load of the texture
    //else
    temp->ForceReload = force;

    Message("End AddFile(%#lX)\n", hash);
    if (new_file) {
        return CurrentMod.Add(temp); // new files must be added to the list of the CurrentMod
    }
    return RETURN_OK;
}

int uMod_TextureServer::RemoveFile(MyTypeHash hash) // called from Mainloop()
{
    Message("RemoveFile( %p): %p\n", hash, this);

    const int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++) {
        if (CurrentMod[i]->Hash == hash) {
            TextureFileStruct* temp = CurrentMod[i];
            CurrentMod.Remove(temp);
            return OldMod.Add(temp);
        }
    }
    return RETURN_OK;
}

int uMod_TextureServer::SaveAllTextures(bool val) // called from Mainloop()
{
    if (BoolSaveAllTextures == val) {
        return RETURN_OK;
    }
    BoolSaveAllTextures = val;

    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return ret;
    }
    for (int i = 0; i < NumberOfClients; i++) {
        Clients[i]->SaveAllTextures(BoolSaveAllTextures);
    }
    return UnlockMutex();
}

int uMod_TextureServer::SaveSingleTexture(bool val) // called from Mainloop()
{
    if (BoolSaveSingleTexture == val) {
        return RETURN_OK;
    }
    BoolSaveSingleTexture = val;

    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return ret;
    }
    for (int i = 0; i < NumberOfClients; i++) {
        Clients[i]->SaveSingleTexture(BoolSaveSingleTexture);
    }
    return UnlockMutex();
}

int uMod_TextureServer::SetSaveDirectory(wchar_t* dir) // called from Mainloop()
{
    Message("uMod_TextureServer::SetSaveDirectory( %ls): %p\n", dir, this);
    int i = 0;
    for (i = 0; i < MAX_PATH && dir[i]; i++) {
        SavePath[i] = dir[i];
    }
    if (i == MAX_PATH) {
        SavePath[0] = 0;
        return RETURN_BAD_ARGUMENT;
    }
    SavePath[i] = 0;

    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return ret;
    }
    for (int i = 0; i < NumberOfClients; i++) {
        Clients[i]->SetSaveDirectory(SavePath);
    }
    return UnlockMutex();
}

int uMod_TextureServer::SetKeyBack(int key) // called from Mainloop()
{
    if (KeyBack == key || KeySave == key || KeyNext == key) {
        return RETURN_OK;
    }
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return ret;
    }
    KeyBack = key;
    for (int i = 0; i < NumberOfClients; i++) {
        Clients[i]->SetKeyBack(key);
    }
    return UnlockMutex();
}

int uMod_TextureServer::SetKeySave(int key) // called from Mainloop()
{
    if (KeyBack == key || KeySave == key || KeyNext == key) {
        return RETURN_OK;
    }
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return ret;
    }
    KeySave = key;
    for (int i = 0; i < NumberOfClients; i++) {
        Clients[i]->SetKeySave(key);
    }
    return UnlockMutex();
}

int uMod_TextureServer::SetKeyNext(int key) // called from Mainloop()
{
    if (KeyBack == key || KeySave == key || KeyNext == key) {
        return RETURN_OK;
    }
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return ret;
    }
    KeyNext = key;
    for (int i = 0; i < NumberOfClients; i++) {
        Clients[i]->SetKeyNext(key);
    }
    return UnlockMutex();
}

int uMod_TextureServer::SetFontColour(DWORD colour) // called from Mainloop()
{
    if (colour == 0u) {
        return RETURN_OK;
    }
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return ret;
    }
    FontColour = colour;
    const DWORD r = (FontColour >> 16) & 0xFF;
    const DWORD g = (FontColour >> 8) & 0xFF;
    const DWORD b = (FontColour) & 0xFF;
    Message("uMod_TextureServer::SetFontColour( %u %u %u): %p\n", r, g, b, this);
    for (int i = 0; i < NumberOfClients; i++) {
        Clients[i]->SetFontColour(r, g, b);
    }
    return UnlockMutex();
}

int uMod_TextureServer::SetTextureColour(DWORD colour) // called from Mainloop()
{
    if (colour == 0u) {
        return RETURN_OK;
    }
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return ret;
    }
    TextureColour = colour;
    const DWORD r = (TextureColour >> 16) & 0xFF;
    const DWORD g = (TextureColour >> 8) & 0xFF;
    const DWORD b = (TextureColour) & 0xFF;
    Message("uMod_TextureServer::SetTextureColour( %u %u %u): %p\n", r, g, b, this);
    for (int i = 0; i < NumberOfClients; i++) {
        Clients[i]->SetTextureColour(r, g, b);
    }
    return UnlockMutex();
}

int uMod_TextureServer::PropagateUpdate(uMod_TextureClient* client) // called from Mainloop(), send the update to all clients
{
    Message("PropagateUpdate(%p): %p\n", client, this);
    if (const int ret = LockMutex()) {
        gl_ErrorState |= uMod_ERROR_TEXTURE;
        return ret;
    }
    if (client != nullptr) {
        TextureFileStruct* update;
        int number;
        if (const int ret = PrepareUpdate(&update, &number)) {
            return ret;
        }
        client->AddUpdate(update, number);
    }
    else {
        for (int i = 0; i < NumberOfClients; i++) {
            TextureFileStruct* update;
            int number;
            if (const int ret = PrepareUpdate(&update, &number)) {
                return ret;
            }
            Clients[i]->AddUpdate(update, number);
        }
    }
    return UnlockMutex();
}

#define cpy_file_struct( a, b) \
{  \
  a.ForceReload = b.ForceReload; \
  a.pData = b.pData; \
  a.Size = b.Size; \
  a.NumberOfTextures = b.NumberOfTextures; \
  a.Reference = b.Reference; \
  a.Textures = b.Textures; \
  a.Hash = b.Hash; }

int TextureFileStruct_Compare(const void* elem1, const void* elem2)
{
    const auto tex1 = (TextureFileStruct*)elem1;
    const auto tex2 = (TextureFileStruct*)elem2;
    if (tex1->Hash < tex2->Hash) {
        return -1;
    }
    if (tex1->Hash > tex2->Hash) {
        return +1;
    }
    return 0;
}

int uMod_TextureServer::PrepareUpdate(TextureFileStruct** update, int* number) // called from the PropagateUpdate() and AddClient.
// Prepare an update for one client. The allocated memory must deleted by the client.
{
    Message("PrepareUpdate(%p, %d): %p\n", update, number, this);

    TextureFileStruct* temp = nullptr;
    const int num = CurrentMod.GetNumber();
    if (num > 0) {
        try { temp = new TextureFileStruct[num]; }
        catch (...) {
            gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_SERVER;
            return RETURN_NO_MEMORY;
        }

        for (int i = 0; i < num; i++) cpy_file_struct(temp[i], (*(CurrentMod[i])));
        qsort(temp, num, sizeof(TextureFileStruct), TextureFileStruct_Compare);
    }


    *update = temp;
    *number = num;
    return RETURN_OK;
}
#undef cpy_file_struct

int uMod_TextureServer::LockMutex()
{
    if ((gl_ErrorState & (uMod_ERROR_FATAL | uMod_ERROR_MUTEX))) {
        return RETURN_NO_MUTEX;
    }
    if (WAIT_OBJECT_0 != WaitForSingleObject(Mutex, 100)) {
        return RETURN_MUTEX_LOCK; //waiting 100ms, to wait infinite pass INFINITE
    }
    return RETURN_OK;
}

int uMod_TextureServer::UnlockMutex()
{
    if (ReleaseMutex(Mutex) == 0) {
        return RETURN_MUTEX_UNLOCK;
    }
    return RETURN_OK;
}

void uMod_TextureServer::LoadModsFromFile(char* source)
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
            const auto entries = file_loader.Load();
            if (!entries.empty()) {
                Message("Initialize: Texture count %zu %s\n", entries.size(), line.c_str());
                for (const auto& tpf_entry : entries) {
                    loadedSize += tpf_entry.size;
                    if (AddFile(static_cast<char*>(tpf_entry.data), static_cast<unsigned int>(tpf_entry.size), tpf_entry.crc_hash, false) == RETURN_EXISTS) {
                        //Texture is already loaded, so we need to clear our data
                        loadedSize -= tpf_entry.size;
                        delete[](tpf_entry.data);
                    }

                    Message("LoadModsFromFile: Loaded %d bytes", loadedSize);
                }

                PropagateUpdate(nullptr);
            }
            else {
                Message("Initialize: Failed to load any textures for %s\n", line.c_str());
            }
        }
    }
}

int uMod_TextureServer::Initialize()
{
    Message("Initialize: begin\n");
    Message("Initialize: searching for modlist.txt\n");
    char gwpath[MAX_PATH];
    GetModuleFileName(GetModuleHandle(nullptr), gwpath, MAX_PATH); //ask for name and path of this executable
    char* last_backslash = strrchr(gwpath, '\\');
    if (last_backslash != nullptr) {
        // Terminate the string at the last backslash to remove the executable name
        *last_backslash = '\0';
    }

    strcat(gwpath, "\\modlist.txt");
    LoadModsFromFile(gwpath);

    char umodpath[MAX_PATH];
    strcpy(umodpath, UModName);

    last_backslash = strrchr(umodpath, '\\');
    if (last_backslash != nullptr) {
        // Terminate the string at the last backslash to remove the executable name
        *last_backslash = '\0';
    }

    strcat(umodpath, "\\modlist.txt");
    LoadModsFromFile(umodpath);

    Message("Initialize: end\n");

    return RETURN_OK;
}
