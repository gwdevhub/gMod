#pragma once

#include "uMod_GlobalDefines.h"
#include "uMod_ArrayHandler.h"


/*
 *  An object of this class is created only once.
 *  The Mainloop functions is executed by a server thread,
 *  which listen on a pipe.
 *
 *  Functions called by the Client are called from the a thread instance of the game itself.
 *  Nearly all other functions are called from the server thread instance.
 */


class uMod_TextureClient;

class uMod_TextureServer {
public:
    uMod_TextureServer(char* name, char* uModName);
    ~uMod_TextureServer();

    int AddClient(uMod_TextureClient* client, TextureFileStruct** update, int* number); // called from a Client
    int Initialize(); // is executed in a server thread


    // following functions are only public for testing purpose !!
    // they should be private and only be called from the Mainloop

    int AddFile(char* buffer, unsigned int size, HashType hash, bool force); // called from Mainloop(), if the content of the texture is sent

private:
    wchar_t GameName[MAX_PATH];
    char UModName[MAX_PATH];

    void LoadModsFromFile(const char* source);
    int PropagateUpdate(uMod_TextureClient* client = nullptr); // called from Mainloop() if texture are loaded or removed
    int PrepareUpdate(TextureFileStruct** update, int* number); // called from PropagateUpdate() and AddClient()
    // generate a copy of the current texture to be modded
    // the file content of the textures are not copied, the clients get the pointer to the file content
    // but the arrays allocate by this function, must be deleted by the client

    uMod_TextureClient* Client;
    uMod_FileHandler CurrentMod;  // hold the file content of texture
    uMod_FileHandler OldMod; // hold the file content of texture which were added previously but are not needed any more
    // this is needed, because a texture clients might not have merged the last update and thus hold pointers to the file content of old textures
};
