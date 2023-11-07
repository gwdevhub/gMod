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

std::wstring MakeWString(const std::string& str) {
    if (str.empty()) return std::wstring();

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

uMod_TextureServer::uMod_TextureServer(wchar_t* game)
{
    Message("uMod_TextureServer(void): %p\n", this);

    Mutex = CreateMutex(NULL, false, NULL);

    Clients = NULL;
    NumberOfClients = 0;
    LenghtOfClients = 0;
    BoolSaveAllTextures = false;
    BoolSaveSingleTexture = false;
    BoolShowTextureString = false;
    BoolShowSingleTexture = false;
    BoolSupportTPF = false;

    SavePath[0] = 0;

    int len = 0;
    int path_pos = 0;
    int dot_pos = 0;
    for (len = 0; len < MAX_PATH && (game[len]); len++)
    {
        if (game[len] == L'\\' || game[len] == L'/') path_pos = len + 1;
        else if (game[len] == L'.') dot_pos = len;
    }

    if (dot_pos > path_pos) len = dot_pos - path_pos;
    else len -= path_pos;

    for (int i = 0; i < len; i++) GameName[i] = game[i + path_pos];

    if (len < MAX_PATH) GameName[len] = 0;
    else GameName[0] = 0;

    KeyBack = 0;
    KeySave = 0;
    KeyNext = 0;

    WidthFilter = 0u;
    HeightFilter = 0u;
    DepthFilter = 0u;

    FormatFilter = 0u;

    FileFormat = uMod_D3DXIFF_DDS;

    FontColour = D3DCOLOR_ARGB(255, 255, 0, 0);
    TextureColour = D3DCOLOR_ARGB(255, 0, 255, 0);

    Pipe.In = INVALID_HANDLE_VALUE;
    Pipe.Out = INVALID_HANDLE_VALUE;
}

uMod_TextureServer::~uMod_TextureServer(void)
{
    Message("~uMod_TextureServer(void): %p\n", this);
    if (Mutex != NULL) CloseHandle(Mutex);

    //delete the files in memory
    int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++)
    {
        delete[] CurrentMod[i]->pData; //delete the file content of the texture
        delete CurrentMod[i]; //delete the structure
    }

    num = OldMod.GetNumber();
    for (int i = 0; i < num; i++)
    {
        delete[] OldMod[i]->pData; //delete the file content of the texture
        delete OldMod[i]; //delete the structure
    }

    if (Clients != NULL) delete[] Clients;

    if (Pipe.In != INVALID_HANDLE_VALUE) CloseHandle(Pipe.In);
    Pipe.In = INVALID_HANDLE_VALUE;
    if (Pipe.Out != INVALID_HANDLE_VALUE) CloseHandle(Pipe.Out);
    Pipe.Out = INVALID_HANDLE_VALUE;
}

int uMod_TextureServer::AddClient(uMod_TextureClient* client, TextureFileStruct*& update, int& number, const int version) // called from a client
{
    Message("AddClient(%p): %p\n", client, this);
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
    client->ShowTextureString(BoolShowTextureString);
    client->ShowSingleTexture(BoolShowSingleTexture);
    client->SupportTPF(BoolSupportTPF);
    client->SetSaveDirectory(SavePath);

    client->SetKeyBack(KeyBack);
    client->SetKeySave(KeySave);
    client->SetKeyNext(KeyNext);

    client->SetFontColour(FontColour);
    client->SetTextureColour(TextureColour);

    client->SetWidthFilter(WidthFilter);
    client->SetHeightFilter(HeightFilter);
    client->SetDepthFilter(DepthFilter);
    client->SetFormatFilter(FormatFilter);


    client->SetFileFormat(FileFormat);

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

    if (int ret = UnlockMutex())  return (ret);

    if (Pipe.Out != INVALID_HANDLE_VALUE)
    {
        MsgStruct msg;
        msg.Control = CONTROL_ADD_CLIENT;
        msg.Value = version;

        unsigned long num;
        bool ret2 = WriteFile(Pipe.Out, (const void*)&msg, sizeof(msg), &num, NULL);
        if (!ret2 || sizeof(msg) != num) { return (RETURN_PIPE_ERROR); }
        if (!FlushFileBuffers(Pipe.Out)) { return (RETURN_PIPE_ERROR); }
    }
    return (RETURN_OK);
}

int uMod_TextureServer::RemoveClient(uMod_TextureClient* client, const int version) // called from a client
{
    Message("RemoveClient(%p): %p\n", client);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }

    bool hit = false;
    for (int i = 0; i < NumberOfClients; i++) if (client == Clients[i])
    {
        hit = true;
        NumberOfClients--;
        Clients[i] = Clients[NumberOfClients];
        break;
    }

    int ret = UnlockMutex();
    if (!hit) return ret;
    if (ret != RETURN_OK) return (ret);


    if (Pipe.Out != INVALID_HANDLE_VALUE)
    {
        MsgStruct msg;
        msg.Control = CONTROL_REMOVE_CLIENT;
        msg.Value = version;

        unsigned long num;
        bool ret2 = WriteFile(Pipe.Out, (const void*)&msg, sizeof(msg), &num, NULL);
        if (!ret2 || sizeof(msg) != num) { return (RETURN_PIPE_ERROR); }
        if (!FlushFileBuffers(Pipe.Out)) { return (RETURN_PIPE_ERROR); }
    }

    return (RETURN_OK);
}

int uMod_TextureServer::AddFile(char* buffer, DWORD64 size, DWORD64 hash, bool force) // called from Mainloop()
{
    Message("uMod_TextureServer::AddFile( %p %llu, %#llX, %d): %p\n", buffer, size, hash, force, this);

    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    TextureFileStruct* temp = NULL;

    int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++) if (CurrentMod[i]->Hash == hash) //look through all current textures
    {
        if (force) { temp = CurrentMod[i]; break; } // we need to reload it
        else
        {
            if (buffer != NULL) delete[] buffer;
            return (RETURN_OK); // we still have added this texture
        }
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
            else
            {
                if (buffer != NULL) delete[] buffer;
                return (RETURN_OK); // we should not reload it
            }
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

    temp->pData = buffer;

    for (unsigned int i = 0; i < size; i++) temp->pData[i] = buffer[i];

    temp->Size = (unsigned int)size;
    temp->NumberOfTextures = 0;
    temp->Textures = NULL;
    temp->Hash = hash;

    //if (new_file) temp->ForceReload = false; // no need to force a load of the texture
    //else
    temp->ForceReload = force;

    Message("uMod_TextureServer::End AddFile(%#llX)\n", hash);
    if (new_file) CurrentMod.Add(temp); // new files must be added to the list of the CurrentMod+

    return (UnlockMutex());
}


int uMod_TextureServer::RemoveFile(DWORD64 hash) // called from Mainloop()
{
    Message("RemoveFile( %#llx): %p\n", hash, this);

    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    int num = CurrentMod.GetNumber();
    for (int i = 0; i < num; i++) if (CurrentMod[i]->Hash == hash)
    {
        TextureFileStruct* temp = CurrentMod[i];
        CurrentMod.Remove(temp);
        return (OldMod.Add(temp));
    }
    return (UnlockMutex());
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

int uMod_TextureServer::ShowTextureString(bool val) // called from Mainloop()
{
    if (BoolShowTextureString == val) return (RETURN_OK);
    BoolShowTextureString = val;

    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->ShowTextureString(BoolShowTextureString);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::ShowSingleTexture(bool val) // called from Mainloop()
{
    if (BoolShowSingleTexture == val) return (RETURN_OK);
    BoolShowSingleTexture = val;

    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->ShowSingleTexture(BoolShowSingleTexture);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SupportTPF(bool val) // called from Mainloop()
{
    if (BoolSupportTPF == val) return (RETURN_OK);
    BoolSupportTPF = val;

    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SupportTPF(BoolSupportTPF);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetSaveDirectory(wchar_t* dir) // called from Mainloop()
{
    Message("uMod_TextureServer::SetSaveDirectory( %ls): %p\n", dir, this);
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

int uMod_TextureServer::SetFontColour(DWORD64 colour) // called from Mainloop()
{
    if (colour == 0u) return (RETURN_OK);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    FontColour = colour;
    Message("uMod_TextureServer::SetFontColour( %#llX): %p\n", colour, this);
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetFontColour(colour);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetTextureColour(DWORD64 colour) // called from Mainloop()
{
    if (colour == 0u) return (RETURN_OK);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    TextureColour = colour;
    Message("uMod_TextureServer::SetTextureColour( %#llX): %p\n", colour, this);
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetTextureColour(colour);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetFileFormat(DWORD64 format) // called from Mainloop()
{
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    FileFormat = format;
    Message("uMod_TextureServer::SetFileFormat( %#llX): %p\n", format, this);
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetFileFormat(format);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetFormatFilter(DWORD64 format) // called from Mainloop()
{
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    FormatFilter = format;
    Message("uMod_TextureServer::SetFormatFilter( %#llX): %p\n", format, this);
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetFormatFilter(format);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetWidthFilter(DWORD64 size) // called from Mainloop()
{
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    WidthFilter = size;
    Message("uMod_TextureServer::SetWidthFilter( %#llX): %p\n", size, this);
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetWidthFilter(size);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetHeightFilter(DWORD64 size) // called from Mainloop()
{
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    HeightFilter = size;
    Message("uMod_TextureServer::SetHeightFilter( %#llX): %p\n", size, this);
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetHeightFilter(size);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::SetDepthFilter(DWORD64 size) // called from Mainloop()
{
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_SERVER;
        return (ret);
    }
    DepthFilter = size;
    Message("uMod_TextureServer::SetDepthFilter( %#llX): %p\n", size, this);
    for (int i = 0; i < NumberOfClients; i++)
    {
        Clients[i]->SetDepthFilter(size);
    }
    return (UnlockMutex());
}

int uMod_TextureServer::PropagateUpdate(uMod_TextureClient* client) // called from Mainloop(), send the update to all clients
{
    Message("PropagateUpdate(%p): %p\n", client, this);
    if (int ret = LockMutex())
    {
        gl_ErrorState |= uMod_ERROR_TEXTURE;
        return (ret);
    }
    if (client != NULL)
    {
        TextureFileStruct* update;
        int number;
        if (int ret = PrepareUpdate(update, number)) return (ret);
        client->AddUpdate(update, number);
    }
    else
    {
        for (int i = 0; i < NumberOfClients; i++)
        {
            TextureFileStruct* update;
            int number;
            if (int ret = PrepareUpdate(update, number)) return (ret);
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

int uMod_TextureServer::PrepareUpdate(TextureFileStruct*& update, int& number) // called from the PropagateUpdate() and AddClient.
// Prepare an update for one client. The allocated memory must deleted by the client.
{
    update = NULL;
    number = 0;

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

    update = temp;
    number = num;

    Message("PrepareUpdate(%p, %d): %p\n", update, number, this);
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

int uMod_TextureServer::MainLoop(void) // run as a separated thread
{
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
        std::cout << fileName << " Loading file " << std::endl;
        auto file = new uMod_File(MakeWString(fileName));
        auto result = file->GetContent();
        if (file->Textures.size() > 0) {
            if (!result) {
                std::cout << fileName << " WARNING! GetContent returned failure, but some textures have been loaded" << std::endl;
            }

            std::cout << fileName << " Texture count " << file->Textures.size() << std::endl;
            for (auto& texture : file->Textures) {
                AddFile(texture.data.data(), static_cast<DWORD64>(texture.data.size()), texture.hash, true);
            }
        }
        else {
            std::cout << fileName << " Failed to load any textures" << std::endl;
            response.status = 400;
            response.set_content("Failed to load any textures", "text/plain");
        }
        });
    http::server::StartServer();
    return (RETURN_OK);
}
