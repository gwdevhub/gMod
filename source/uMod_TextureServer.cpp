#include <algorithm>
#include <filesystem>
#include <fstream>

#include "uMod_Main.h"
#include "gMod_FileLoader.h"

unsigned long loadedSize = 0;

uMod_TextureServer::uMod_TextureServer(char* game, char* uModName)
{
    Message("uMod_TextureServer(): %p\n", this);

    Client = nullptr;

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

    strcpy(UModName, uModName);
}

uMod_TextureServer::~uMod_TextureServer()
{
    Message("~uMod_TextureServer(): %p\n", this);
    //delete the files in memory
    int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++) {
        delete[] CurrentMod[i]->pData; //delete the file content of the texture
    }

    num = OldMod.GetNumber();
    for (int i = 0; i < num; i++) {
        delete[] OldMod[i]->pData; //delete the file content of the texture
    }
}

int uMod_TextureServer::AddClient(uMod_TextureClient* client, TextureFileStruct** update, int* number) // called from a client
{
    Message("AddClient(%p): %p\n", client, this);
    // the following functions must not change the original uMod_IDirect3DDevice9 object
    // somehow on game start some uMod_IDirect3DDevice9 object are created, which must rest unchanged!!
    // these objects are released and are not used for rendering

    if (const int ret = PrepareUpdate(update, number)) {
        return ret; // get a copy of all texture to be modded
    }

    Client = client;

    return RETURN_OK;
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

int uMod_TextureServer::PropagateUpdate(uMod_TextureClient* client) // called from Mainloop(), send the update to all clients
{
    Message("PropagateUpdate(%p): %p\n", client, this);
    if (client != nullptr) {
        TextureFileStruct* update;
        int number;
        if (const int ret = PrepareUpdate(&update, &number)) {
            return ret;
        }
        client->AddUpdate(update, number);
    }
    else {
        TextureFileStruct* update;
        int number;
        if (const int ret = PrepareUpdate(&update, &number)) {
            return ret;
        }
        Client->AddUpdate(update, number);
    }
    return RETURN_OK;
}

void cpy_file_struct(TextureFileStruct& a,TextureFileStruct& b)
{
    a.ForceReload = b.ForceReload;
    a.pData = b.pData;
    a.Size = b.Size;
    a.NumberOfTextures = b.NumberOfTextures;
    a.Reference = b.Reference;
    a.Textures = b.Textures;
    a.Hash = b.Hash;
}

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

void uMod_TextureServer::LoadModsFromFile(const char* source)
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
            if (loadedSize > 1'500'000'000) {
                Message("LoadModsFromFile: Loaded %d bytes, aborting!!!", loadedSize);
                return;
            }
            if (!entries.empty()) {
                Message("Initialize: Texture count %zu %s\n", entries.size(), line.c_str());
                for (const auto& tpf_entry : entries) {
                    loadedSize += tpf_entry.size;
                    if (AddFile(static_cast<char*>(tpf_entry.data), static_cast<unsigned int>(tpf_entry.size), tpf_entry.crc_hash, false) == RETURN_EXISTS) {
                        //Texture is already loaded, so we need to clear our data
                        loadedSize -= tpf_entry.size;
                    }

                    Message("LoadModsFromFile: Loaded %d bytes", loadedSize);
                }

                PropagateUpdate(nullptr);
            }
            else {
                Message("Initialize: Failed to load any textures for %s\n", line.c_str());
            }
        }
        Message("Finished loading mods: Loaded %u bytes (%u mb)", loadedSize, loadedSize / 1024 / 1024);
    }
}

int uMod_TextureServer::Initialize()
{
    Message("Initialize: begin\n");
    Message("Initialize: searching for modlist.txt\n");
    char gwpath[MAX_PATH];
    GetModuleFileName(GetModuleHandle(nullptr), gwpath, MAX_PATH); //ask for name and path of this executable
    const auto exe = std::filesystem::path(gwpath).parent_path();
    const auto dll = std::filesystem::path(UModName).parent_path();
    for (const auto& path : {exe, dll}) {
        const auto modlist = path / "modlist.txt";
        if (std::filesystem::exists(modlist)) {
            LoadModsFromFile(modlist.string().c_str());
        }
    }

    Message("Initialize: end\n");

    return RETURN_OK;
}
