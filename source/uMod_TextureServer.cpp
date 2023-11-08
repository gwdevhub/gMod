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
#include "../header/Server.h"
#include "../header/uMod_File.h"

uMod_TextureServer::uMod_TextureServer(char* game, char* uModName)
{
    Message("uMod_TextureServer(void): %lu\n", this);

    Mutex = CreateMutex(NULL, false, NULL);

    Clients = NULL;
    NumberOfClients = 0;
    LenghtOfClients = 0;
    BoolSaveAllTextures = false;
    BoolSaveSingleTexture = false;
    SavePath[0] = 0;

    int len = 0;
    int path_pos = 0;
    int dot_pos = 0;
    for (len = 0; len < MAX_PATH && (game[len]); len++)
    {
        if (game[len] == '\\' || game[len] == '/') path_pos = len + 1;
        else if (game[len] == '.') dot_pos = len;
    }

    if (dot_pos > path_pos) len = dot_pos - path_pos;
    else len -= path_pos;

    for (int i = 0; i < len; i++) GameName[i] = game[i + path_pos];

    if (len < MAX_PATH) GameName[len] = 0;
    else GameName[0] = 0;

    for (len = 0; len < MAX_PATH; len++)
    {
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

uMod_TextureServer::~uMod_TextureServer(void)
{
    Message("~uMod_TextureServer(void): %lu\n", this);
    if (Mutex != NULL) CloseHandle(Mutex);

    //delete the files in memory
    int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++) delete[] CurrentMod[i]->pData; //delete the file content of the texture

    num = OldMod.GetNumber();
    for (int i = 0; i < num; i++) delete[] OldMod[i]->pData; //delete the file content of the texture

    if (Pipe.In != INVALID_HANDLE_VALUE) CloseHandle(Pipe.In);
    Pipe.In = INVALID_HANDLE_VALUE;
    if (Pipe.Out != INVALID_HANDLE_VALUE) CloseHandle(Pipe.Out);
    Pipe.Out = INVALID_HANDLE_VALUE;
}

int uMod_TextureServer::AddClient(uMod_TextureClient* client, TextureFileStruct** update, int* number) // called from a client
{
    Message("AddClient(%lu): %lu\n", client, this);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }

    // the following functions must not change the original uMod_IDirect3DDevice9 object
    // somehow on game start some uMod_IDirect3DDevice9 object are created, which must rest unchanged!!
    // these objects are released and are not used for rendering
    client->SetGameName(GameName);
    client->SaveAllTextures(BoolSaveAllTextures);
    client->SaveSingleTexture(BoolSaveSingleTexture);
    client->SetSaveDirectory(SavePath);
    if (KeyBack > 0) client->SetKeyBack(KeyBack);
    if (KeySave > 0) client->SetKeySave(KeySave);
    if (KeyNext > 0) client->SetKeyNext(KeyNext);

    if (FontColour > 0u)
    {
        DWORD r = (FontColour >> 16) & 0xFF;
        DWORD g = (FontColour >> 8) & 0xFF;
        DWORD b = (FontColour) & 0xFF;
        client->SetFontColour(r, g, b);
    }
    if (TextureColour > 0u)
    {
        DWORD r = (TextureColour >> 16) & 0xFF;
        DWORD g = (TextureColour >> 8) & 0xFF;
        DWORD b = (TextureColour) & 0xFF;
        client->SetTextureColour(r, g, b);
    }


    if (int ret = PrepareUpdate(update, number)) return (ret); // get a copy of all texture to be modded


    if (NumberOfClients == LenghtOfClients) //allocate more memory
    {
        uMod_TextureClient** temp = NULL;
        try { temp = new uMod_TextureClient * [LenghtOfClients + 10]; }
        catch (...)
        {
            gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_SERVER;
            if (int ret = UnlockMutex()) return (ret);
            return (RETURN_NO_MEMORY);
        }
        for (int i = 0; i < LenghtOfClients; i++) temp[i] = Clients[i];
        if (Clients != NULL) delete[] Clients;
        Clients = temp;
        LenghtOfClients += 10;
    }
    Clients[NumberOfClients++] = client;

    return (UnlockMutex());
}

int uMod_TextureServer::RemoveClient(uMod_TextureClient* client) // called from a client
{
    Message("RemoveClient(%lu): %lu\n", client);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }

    for (int i = 0; i < NumberOfClients; i++) if (client == Clients[i])
    {
        NumberOfClients--;
        Clients[i] = Clients[NumberOfClients];
        break;
    }
    return (UnlockMutex());
}

int uMod_TextureServer::AddFile(char* buffer, unsigned int size, MyTypeHash hash, bool force) // called from Mainloop()
{
    Message("uMod_TextureServer::AddFile( %lu %lu, %#lX, %d): %lu\n", buffer, size, hash, force, this);

    TextureFileStruct* temp = NULL;

    int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++) if (CurrentMod[i]->Hash == hash) //look through all current textures
    {
        if (force) { temp = CurrentMod[i]; break; } // we need to reload it
        else return (RETURN_OK); // we still have added this texture
    }
    if (temp == NULL) // if not found, look through all old textures
    {
        num = OldMod.GetNumber();
        for (int i = 0; i < num; i++) if (OldMod[i]->Hash == hash)
        {
            temp = OldMod[i];
            OldMod.Remove(temp);
            CurrentMod.Add(temp);
            if (force) break; // we must reload it
            else return (RETURN_OK); // we should not reload it
        }
    }

    bool new_file = true;
    if (temp != NULL) //if it was found, we delete the old file content
    {
        new_file = false;
        if (temp->pData != NULL) delete[] temp->pData;
        temp->pData = NULL;
    }
    else //if it was not found, we need to create a new object
    {
        new_file = true;
        temp = new TextureFileStruct;
        temp->Reference = -1;
    }

    try
    {
        temp->pData = new char[size];
    }
    catch (...)
    {
        if (!new_file) CurrentMod.Remove(temp); // if this is a not a new file it is in the list of the CurrentMod
        delete temp;
        gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_SERVER;
        return (RETURN_NO_MEMORY);
    }

    for (unsigned int i = 0; i < size; i++) temp->pData[i] = buffer[i];

    temp->Size = size;
    temp->NumberOfTextures = 0;
    temp->Textures = NULL;
    temp->Hash = hash;

    //if (new_file) temp->ForceReload = false; // no need to force a load of the texture
    //else
    temp->ForceReload = force;

    Message("End AddFile(%#lX)\n", hash);
    if (new_file) return (CurrentMod.Add(temp)); // new files must be added to the list of the CurrentMod
    else return (RETURN_OK);
}

int uMod_TextureServer::AddFile(wchar_t* file_name, MyTypeHash hash, bool force) // called from Mainloop
// this functions does the same, but loads the file content from disk
{
    Message("uMod_TextureServer::AddFile( %ls, %#lX, %d): %lu\n", file_name, hash, force, this);

    TextureFileStruct* temp = NULL;

    int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++) if (CurrentMod[i]->Hash == hash)
    {
        if (force) { temp = CurrentMod[i]; break; }
        else return (RETURN_OK);
    }
    if (temp == NULL)
    {
        num = OldMod.GetNumber();
        for (int i = 0; i < num; i++) if (OldMod[i]->Hash == hash)
        {
            temp = OldMod[i];
            OldMod.Remove(temp);
            CurrentMod.Add(temp);
            if (force) break;
            else return (RETURN_OK);
        }
    }

    FILE* file;
    if (_wfopen_s(&file, file_name, L"rb") != 0)
    {
        Message("AddFile( ): file not found\n");
        return (RETURN_FILE_NOT_LOADED);
    }

    fseek(file, 0, SEEK_END);
    unsigned int size = ftell(file);
    fseek(file, 0, SEEK_SET);

    bool new_file = true;
    if (temp != NULL)
    {
        new_file = false;
        if (temp->pData != NULL) delete[] temp->pData;
        temp->pData = NULL;
    }
    else
    {
        new_file = true;
        temp = new TextureFileStruct;
        temp->Reference = -1;
    }

    try
    {
        temp->pData = new char[size];
    }
    catch (...)
    {
        if (!new_file) CurrentMod.Remove(temp);
        delete temp;
        gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_SERVER;
        return (RETURN_NO_MEMORY);
    }
    int result = fread(temp->pData, 1, size, file);
    fclose(file);
    if (result != size)
    {
        delete[] temp->pData;
        if (!new_file) CurrentMod.Remove(temp);
        delete temp;
        return (RETURN_FILE_NOT_LOADED);
    }

    temp->Size = size;
    temp->NumberOfTextures = 0;
    temp->Textures = NULL;
    temp->Hash = hash;

    if (new_file) temp->ForceReload = false;
    else temp->ForceReload = force;

    Message("End AddFile(%#lX)\n", hash);
    if (new_file) return (CurrentMod.Add(temp));
    else return (RETURN_OK);
}

int uMod_TextureServer::RemoveFile(MyTypeHash hash) // called from Mainloop()
{
    Message("RemoveFile( %lu): %lu\n", hash, this);

    int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++) if (CurrentMod[i]->Hash == hash)
    {
        TextureFileStruct* temp = CurrentMod[i];
        CurrentMod.Remove(temp);
        return (OldMod.Add(temp));
    }
    return (RETURN_OK);
}

int uMod_TextureServer::SaveAllTextures(bool val) // called from Mainloop()
{
    if (BoolSaveAllTextures == val) return (RETURN_OK);
    BoolSaveAllTextures = val;

    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SaveAllTextures(BoolSaveAllTextures);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SaveSingleTexture(bool val) // called from Mainloop()
{
    if (BoolSaveSingleTexture == val) return (RETURN_OK);
    BoolSaveSingleTexture = val;

    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SaveSingleTexture(BoolSaveSingleTexture);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetSaveDirectory(wchar_t* dir) // called from Mainloop()
{
    Message("uMod_TextureServer::SetSaveDirectory( %ls): %lu\n", dir, this);
    int i = 0;
    for (i = 0; i < MAX_PATH && (dir[i]); i++) SavePath[i] = dir[i];
    if (i == MAX_PATH)
    {
        SavePath[0] = 0;
        return (RETURN_BAD_ARGUMENT);
    }
    else SavePath[i] = 0;

    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetSaveDirectory(SavePath);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetKeyBack(int key) // called from Mainloop()
{
    if (KeyBack == key || KeySave == key || KeyNext == key) return (RETURN_OK);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    KeyBack = key;
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetKeyBack(key);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetKeySave(int key) // called from Mainloop()
{
    if (KeyBack == key || KeySave == key || KeyNext == key) return (RETURN_OK);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    KeySave = key;
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetKeySave(key);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetKeyNext(int key) // called from Mainloop()
{
    if (KeyBack == key || KeySave == key || KeyNext == key) return (RETURN_OK);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    KeyNext = key;
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetKeyNext(key);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetFontColour(DWORD colour) // called from Mainloop()
{
    if (colour == 0u) return (RETURN_OK);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    FontColour = colour;
    DWORD r = (FontColour >> 16) & 0xFF;
    DWORD g = (FontColour >> 8) & 0xFF;
    DWORD b = (FontColour) & 0xFF;
    Message("uMod_TextureServer::SetFontColour( %u %u %u): %lu\n", r, g, b, this);
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetFontColour(r, g, b);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetTextureColour(DWORD colour) // called from Mainloop()
{
    if (colour == 0u) return (RETURN_OK);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    TextureColour = colour;
    DWORD r = (TextureColour >> 16) & 0xFF;
    DWORD g = (TextureColour >> 8) & 0xFF;
    DWORD b = (TextureColour) & 0xFF;
    Message("uMod_TextureServer::SetTextureColour( %u %u %u): %lu\n", r, g, b, this);
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetTextureColour(r, g, b);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::PropagateUpdate(uMod_TextureClient* client) // called from Mainloop(), send the update to all clients
{
    Message("PropagateUpdate(%lu): %lu\n", client, this);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_TEXTURE;
        return (ret);
    }
    if (client != NULL)
    {
        TextureFileStruct* update;
        int number;
        if (int ret = PrepareUpdate(&update, &number)) return (ret);
        client->AddUpdate(update, number);
    }
    else
    {
        for (int i = 0; i < NumberOfClients; i++)
        {
            TextureFileStruct* update;
            int number;
            if (int ret = PrepareUpdate(&update, &number)) return (ret);
            Clients[i]->AddUpdate(update, number);
        }
    }
    return (UnlockMutex());
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
    TextureFileStruct* tex1 = (TextureFileStruct*)elem1;
    TextureFileStruct* tex2 = (TextureFileStruct*)elem2;
    if (tex1->Hash < tex2->Hash) return (-1);
    if (tex1->Hash > tex2->Hash) return (+1);
    return (0);
}

int uMod_TextureServer::PrepareUpdate(TextureFileStruct** update, int* number) // called from the PropagateUpdate() and AddClient.
// Prepare an update for one client. The allocated memory must deleted by the client.
{
    Message("PrepareUpdate(%lu, %d): %lu\n", update, number, this);

    TextureFileStruct* temp = NULL;
    int num = CurrentMod.GetNumber();
    if (num > 0)
    {
        try { temp = new TextureFileStruct[num]; }
        catch (...)
        {
            gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_SERVER;
            return (RETURN_NO_MEMORY);
        }

        for (int i = 0; i < num; i++) cpy_file_struct(temp[i], (*(CurrentMod[i])));
        qsort(temp, num, sizeof(TextureFileStruct), TextureFileStruct_Compare);
    }


    *update = temp;
    *number = num;
    return (RETURN_OK);
}
#undef cpy_file_struct

int uMod_TextureServer::LockMutex(void)
{
    if ((gl_ErrorState & (uMod_ERROR_FATAL | uMod_ERROR_MUTEX))) return (RETURN_NO_MUTEX);
    if (WAIT_OBJECT_0 != WaitForSingleObject(Mutex, 100)) return (RETURN_MUTEX_LOCK); //waiting 100ms, to wait infinite pass INFINITE
    return (RETURN_OK);
}

int uMod_TextureServer::UnlockMutex(void)
{
    if (ReleaseMutex(Mutex) == 0) return (RETURN_MUTEX_UNLOCK);
    return (RETURN_OK);
}

void uMod_TextureServer::LoadModsFromFile(char* source)
{
    Message("MainLoop: searching in %s\n", source);
    // Attempt to open the file
    FILE* file = fopen(source, "r");
    if (file) {
        Message("MainLoop: found modlist.txt. Reading\n");
        // Read each line from the file
        char line[MAX_PATH];
        while (fgets(line, sizeof(line), file) != NULL) {
            Message("MainLoop: loading file %s\n", line);
            for (auto i = 0; i < MAX_PATH; i++) {
                if (line[i] == '\n') {
                    line[i] = 0;
                }
            }

            auto file = new uMod_File(line);
            auto result = file->GetContent();
            if (file->Textures.size() > 0) {
                if (!result) {
                    Message("MainLoop: WARNING! GetContent returned failure, but some textures have been loaded for %s\n", line);
                }

                Message("MainLoop: Texture count %d %s\n", file->Textures.size(), line);
                for (auto& texture : file->Textures) {
                    AddFile(texture.data.data(), static_cast<DWORD64>(texture.data.size()), texture.hash, true);
                }

                PropagateUpdate(NULL);
            }
            else {
                Message("MainLoop: Failed to load any textures for %s\n", line);
            }
        }
    }
}

int uMod_TextureServer::MainLoop(void) // run as a separated thread
{
    Message("MainLoop: searching for modlist.txt\n");
    char gwpath[MAX_PATH];
    GetModuleFileName(GetModuleHandle(NULL), gwpath, MAX_PATH); //ask for name and path of this executable
    char* last_backslash = strrchr(gwpath, '\\');
    if (last_backslash != NULL) {
        // Terminate the string at the last backslash to remove the executable name
        *last_backslash = '\0';
    }

    strcat(gwpath, "\\modlist.txt");
    LoadModsFromFile(gwpath);

    char umodpath[MAX_PATH];
    strcpy(umodpath, UModName);

    last_backslash = strrchr(umodpath, '\\');
    if (last_backslash != NULL) {
        // Terminate the string at the last backslash to remove the executable name
        *last_backslash = '\0';
    }

    strcat(umodpath, "\\modlist.txt");
    LoadModsFromFile(umodpath);

    Message("MainLoop: begin\n");
    http::server::Get("/id", [](const httplib::Request&, httplib::Response& response) {
        auto processId = GetCurrentProcessId();
        response.set_content(std::to_string(processId), "text/plain");
        });
    http::server::Get("/add", [this](const httplib::Request& req, httplib::Response& response) {
        auto file_it = req.params.find("name");
        if (file_it == req.params.end()) {
            response.status = 400;
            response.set_content("Missing id parameter", "text/plain");
            return;
        }

        const auto& fileName = file_it->second;
        Message("MainLoop: Loading file %s", fileName.c_str());
        auto file = new uMod_File(fileName);
        auto result = file->GetContent();
        if (file->Textures.size() > 0) {
            if (!result) {
                Message("MainLoop: WARNING! GetContent returned failure, but some textures have been loaded", fileName.c_str());
            }

            Message("MainLoop: Texture count %d %s", file->Textures.size(), fileName.c_str());
            for (auto& texture : file->Textures) {
                AddFile(texture.data.data(), static_cast<DWORD64>(texture.data.size()), texture.hash, true);
            }

            PropagateUpdate(NULL);
        }
        else {
            Message("MainLoop: Failed to load any textures for %s", fileName.c_str());
            response.status = 400;
            response.set_content("Failed to load any textures", "text/plain");
        }
        });
    http::server::StartServer();
    return (RETURN_OK);
}
