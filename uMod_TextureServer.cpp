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

uMod_TextureServer::uMod_TextureServer(wchar_t *game)
{
  Message("uMod_TextureServer(void): %lu\n", this);

  Mutex = CreateMutex(NULL, false, NULL);

  Clients = NULL;
  NumberOfClients = 0;
  LenghtOfClients = 0;
  BoolSaveAllTextures = false;
  BoolSaveSingleTexture = false;
  SavePath[0] = 0;

  int len=0;
  int path_pos=0;
  int dot_pos=0;
  for (len=0; len<MAX_PATH && (game[len]); len++)
  {
    if (game[len]==L'\\' || game[len]==L'/') path_pos=len+1;
    else if (game[len]==L'.') dot_pos=len;
  }

  if (dot_pos>path_pos) len = dot_pos - path_pos;
  else len -= path_pos;

  for (int i=0; i<len; i++) GameName[i] = game[i+path_pos];

  if (len<MAX_PATH) GameName[len] = 0;
  else GameName[0] = 0;

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

  if (Pipe.In != INVALID_HANDLE_VALUE ) CloseHandle(Pipe.In);
  Pipe.In = INVALID_HANDLE_VALUE;
  if (Pipe.Out != INVALID_HANDLE_VALUE) CloseHandle(Pipe.Out);
  Pipe.Out = INVALID_HANDLE_VALUE;
}

int uMod_TextureServer::AddClient(uMod_TextureClient *client, TextureFileStruct** update, int* number) // called from a client
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

  if (FontColour>0u)
  {
    DWORD r = (FontColour>>16)&0xFF;
    DWORD g = (FontColour>>8)&0xFF;
    DWORD b = (FontColour)&0xFF;
    client->SetFontColour( r, g, b);
  }
  if (TextureColour>0u)
  {
    DWORD r = (TextureColour>>16)&0xFF;
    DWORD g = (TextureColour>>8)&0xFF;
    DWORD b = (TextureColour)&0xFF;
    client->SetTextureColour( r, g, b);
  }


  if (int ret = PrepareUpdate( update, number)) return (ret); // get a copy of all texture to be modded


  if (NumberOfClients == LenghtOfClients) //allocate more memory
  {
    uMod_TextureClient** temp = NULL;
    try {temp = new uMod_TextureClient*[LenghtOfClients + 10];}
    catch (...)
    {
      gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_SERVER;
      if (int ret = UnlockMutex()) return (ret);
      return (RETURN_NO_MEMORY);
    }
    for (int i=0; i<LenghtOfClients; i++) temp[i] = Clients[i];
    if (Clients!=NULL) delete [] Clients;
    Clients = temp;
    LenghtOfClients += 10;
  }
  Clients[NumberOfClients++] = client;

  return (UnlockMutex());
}

int uMod_TextureServer::RemoveClient(uMod_TextureClient *client) // called from a client
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

int uMod_TextureServer::AddFile( char* buffer, unsigned int size,  MyTypeHash hash, bool force) // called from Mainloop()
{
  Message("uMod_TextureServer::AddFile( %lu %lu, %#lX, %d): %lu\n", buffer, size, hash, force, this);

  TextureFileStruct* temp = NULL;

  int num = CurrentMod.GetNumber();
  for (int i=0; i<num; i++) if (CurrentMod[i]->Hash == hash) //look through all current textures
  {
    if (force) {temp = CurrentMod[i]; break;} // we need to reload it
    else return (RETURN_OK); // we still have added this texture
  }
  if (temp==NULL) // if not found, look through all old textures
  {
    num = OldMod.GetNumber();
    for (int i=0; i<num; i++) if (OldMod[i]->Hash == hash)
    {
      temp = OldMod[i];
      OldMod.Remove(temp);
      CurrentMod.Add(temp);
      if (force) break; // we must reload it
      else return (RETURN_OK); // we should not reload it
    }
  }

  bool new_file = true;
  if (temp!=NULL) //if it was found, we delete the old file content
  {
    new_file = false;
    if (temp->pData!=NULL) delete [] temp->pData;
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
    if (!new_file) CurrentMod.Remove( temp); // if this is a not a new file it is in the list of the CurrentMod
    delete temp;
    gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_SERVER;
    return (RETURN_NO_MEMORY);
  }

  for (unsigned int i=0; i<size; i++) temp->pData[i] = buffer[i];

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
    if (force) {temp = CurrentMod[i]; break;}
    else return (RETURN_OK);
  }
  if (temp==NULL)
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

  fseek (file, 0, SEEK_END);
  unsigned int size = ftell(file);
  fseek (file, 0, SEEK_SET);

  bool new_file = true;
  if (temp!=NULL)
  {
    new_file = false;
    if (temp->pData!=NULL) delete [] temp->pData;
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
    if (!new_file) CurrentMod.Remove( temp);
    delete temp;
    gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_SERVER;
    return (RETURN_NO_MEMORY);
  }
  int result = fread(temp->pData, 1, size, file);
  fclose(file);
  if (result != size)
  {
    delete[] temp->pData;
    if (!new_file) CurrentMod.Remove( temp);
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

int uMod_TextureServer::SetSaveDirectory(wchar_t *dir) // called from Mainloop()
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
  if (colour==0u) return (RETURN_OK);
  if (int ret = LockMutex())
  {
    gl_ErrorState |= uMod_ERROR_SERVER;
    return (ret);
  }
  FontColour = colour;
  DWORD r = (FontColour>>16)&0xFF;
  DWORD g = (FontColour>>8)&0xFF;
  DWORD b = (FontColour)&0xFF;
  Message("uMod_TextureServer::SetFontColour( %u %u %u): %lu\n", r ,g ,b, this);
  for (int i = 0; i < NumberOfClients; i++)
  {
    Clients[i]->SetFontColour( r, g, b);
  }
  return (UnlockMutex());
}

int uMod_TextureServer::SetTextureColour(DWORD colour) // called from Mainloop()
{
  if (colour==0u) return (RETURN_OK);
  if (int ret = LockMutex())
  {
    gl_ErrorState |= uMod_ERROR_SERVER;
    return (ret);
  }
  TextureColour = colour;
  DWORD r = (TextureColour>>16)&0xFF;
  DWORD g = (TextureColour>>8)&0xFF;
  DWORD b = (TextureColour)&0xFF;
  Message("uMod_TextureServer::SetTextureColour( %u %u %u): %lu\n", r ,g ,b, this);
  for (int i = 0; i < NumberOfClients; i++)
  {
    Clients[i]->SetTextureColour( r, g, b);
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
    if (int ret = PrepareUpdate( &update, &number)) return (ret);
    client->AddUpdate(update, number);
  }
  else
  {
    for (int i=0; i<NumberOfClients; i++)
    {
      TextureFileStruct* update;
      int number;
      if (int ret = PrepareUpdate( &update, &number)) return (ret);
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

int TextureFileStruct_Compare( const void * elem1, const void * elem2 )
{
  TextureFileStruct *tex1 = (TextureFileStruct*)elem1;
  TextureFileStruct *tex2 = (TextureFileStruct*)elem2;
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
  if (num>0)
  {
    try {temp = new TextureFileStruct[num];}
    catch (...)
    {
      gl_ErrorState |= uMod_ERROR_MEMORY | uMod_ERROR_SERVER;
      return (RETURN_NO_MEMORY);
    }

    for (int i=0; i<num; i++) cpy_file_struct(temp[i], (*(CurrentMod[i])));
    qsort( temp, num, sizeof(TextureFileStruct), TextureFileStruct_Compare);
  }


  *update = temp;
  *number = num;
  return (RETURN_OK);
}
#undef cpy_file_struct

int uMod_TextureServer::LockMutex(void)
{
  if (( gl_ErrorState & (uMod_ERROR_FATAL | uMod_ERROR_MUTEX) )) return (RETURN_NO_MUTEX);
  if (WAIT_OBJECT_0!=WaitForSingleObject( Mutex, 100)) return (RETURN_MUTEX_LOCK); //waiting 100ms, to wait infinite pass INFINITE
  return (RETURN_OK);
}

int uMod_TextureServer::UnlockMutex(void)
{
  if (ReleaseMutex( Mutex) == 0) return (RETURN_MUTEX_UNLOCK);
  return (RETURN_OK);
}

int uMod_TextureServer::MainLoop(void) // run as a separated thread
{
  Message("MainLoop: begin\n");
  if (Pipe.In == INVALID_HANDLE_VALUE) return (RETURN_PIPE_NOT_OPENED);
  char *buffer;
  try {buffer = new char[BIG_BUFSIZE];}
  catch (...) {return (RETURN_NO_MEMORY);}

  unsigned long num;

  Message("MainLoop: started\n");
  bool update_textures = false;
  bool more_textures = false;
  while (1)
  {
    Message("MainLoop: run\n");
    bool ret = ReadFile(Pipe.In, // pipe handle
        buffer, // buffer to receive reply
        BIG_BUFSIZE, // size of buffer
        &num, // number of bytes read
        NULL); // not overlapped

    Message("MainLoop: read something (%lu)\n", num);
    if (ret || GetLastError() == ERROR_MORE_DATA)
    {
      unsigned int pos = 0;
      MsgStruct *commands;
      more_textures = false;
      while (pos <= num - sizeof(MsgStruct))
      {
        commands = (MsgStruct*) &buffer[pos];
        unsigned int size = 0u;
        bool force = false;

        switch (commands->Control)
        {
        case CONTROL_MORE_TEXTURES:
        {
          more_textures=true;
          break;
        }
        case CONTROL_FORCE_RELOAD_TEXTURE: force=true;
        case CONTROL_ADD_TEXTURE:
        {
          size = commands->Value;
          Message("MainLoop: CONTROL_ADD_TEXTURE (%#lX  %u,  %u %u): %lu\n", commands->Hash, size, sizeof(MsgStruct), sizeof(char), this);
          if (pos + sizeof(MsgStruct) + size <= num) AddFile( (wchar_t*) &buffer[pos + sizeof(MsgStruct)], commands->Hash, force);
          update_textures = true;
          force = false;
          break;
        }
        case CONTROL_FORCE_RELOAD_TEXTURE_DATA: force=true;
        case CONTROL_ADD_TEXTURE_DATA:
        {
          size = commands->Value;
          Message("MainLoop: CONTROL_FORCE_RELOAD_TEXTURE_DATA (%#lX  %u,  %u %u): %lu\n", commands->Hash, size, sizeof(MsgStruct), sizeof(char), this);
          if (pos + sizeof(MsgStruct) + size <= num) AddFile( &buffer[pos + sizeof(MsgStruct)], size, commands->Hash, force);
          update_textures = true;
          force = false;
          break;
        }

        case CONTROL_REMOVE_TEXTURE:
        {
          Message("MainLoop: CONTROL_REMOVE_TEXTURE (%#lX): %lu\n", commands->Hash, this);
          RemoveFile(commands->Hash);
          update_textures = true;
          break;
        }

        case CONTROL_SAVE_SINGLE:
        {
          Message("MainLoop: CONTROL_SAVE_SINGLE (%d): %lu\n", commands->Value, this);
          if (commands->Value == 0) SaveSingleTexture(false);
          else SaveSingleTexture(true);
          break;
        }
        case CONTROL_SAVE_ALL:
        {
          Message("MainLoop: CONTROL_SAVE_ALL (%d): %lu\n", commands->Value, this);
          if (commands->Value == 0) SaveAllTextures(false);
          else SaveAllTextures(true);
          break;
        }
        case CONTROL_SET_DIR:
        {
          size = commands->Value;
          if (pos + sizeof(MsgStruct) +size <= num) SetSaveDirectory( (wchar_t*) &buffer[pos + sizeof(MsgStruct)]);
          break;
        }

        case CONTROL_KEY_BACK:
        {
          Message("MainLoop: CONTROL_KEY_BACK (%#X): %lu\n", commands->Value, this);
          SetKeyBack(commands->Value);
          break;
        }
        case CONTROL_KEY_SAVE:
        {
          Message("MainLoop: CONTROL_KEY_SAVE (%#X): %lu\n", commands->Value, this);
          SetKeySave(commands->Value);
          break;
        }
        case CONTROL_KEY_NEXT:
        {
          Message("MainLoop: CONTROL_KEY_NEXT (%#X): %lu\n", commands->Value, this);
          SetKeyNext(commands->Value);
          break;
        }
        case CONTROL_FONT_COLOUR:
        {
          Message("MainLoop: CONTROL_FONT_COLOUR (%#X): %lu\n", commands->Value, this);
          SetFontColour(commands->Value);
          break;
        }
        case CONTROL_TEXTURE_COLOUR:
        {
          Message("MainLoop: CONTROL_TEXTURE_COLOUR (%#X): %lu\n", commands->Value, this);
          SetTextureColour(commands->Value);
          break;
        }
        default:
        {
          Message("MainLoop: DEFAULT: %lu  %lu  %#lX\n", commands->Control, commands->Value, commands->Hash, this);
          break;
        }
        }
        pos += sizeof(MsgStruct) + size;
      }
      if (!more_textures && update_textures) {PropagateUpdate(); update_textures=false;}
    }
    else
    {
      Message("MainLoop: error in ReadFile()\n");
      delete [] buffer;
      ClosePipe();
      return (RETURN_OK);
    }
  }

  delete [] buffer;
  return (RETURN_OK);
}

int uMod_TextureServer::OpenPipe(wchar_t *game) // called from InitInstance()
{
  Message("OpenPipe: Out\n")
  // open first outgoing pipe !!
  Pipe.Out = CreateFileW(PIPE_Game2uMod, // pipe name
      GENERIC_WRITE, // write access
      0, // no sharing
      NULL, // default security attributes
      OPEN_EXISTING, // opens existing pipe
      0, // default attributes
      NULL); // no template file

  // Exit if an error other than ERROR_PIPE_BUSY occurs.
  if (Pipe.Out == INVALID_HANDLE_VALUE) return (RETURN_PIPE_NOT_OPENED);

  unsigned int len = 0u;
  while (game[len]) len++;
  len++; //to send also the zero
  unsigned long num;
  //send name of this game to uMod_GUI
  WriteFile(Pipe.Out, (const void*) game, len * sizeof(wchar_t), &num, NULL);

  // now we can open the pipe for reading
  Message("OpenPipe: In\n");
  Pipe.In = CreateFileW(PIPE_uMod2Game, // pipe name
      GENERIC_READ, // read access
      0, // no sharing
      NULL, // default security attributes
      OPEN_EXISTING, // opens existing pipe
      0, // default attributes
      NULL); // no template file

  if (Pipe.In == INVALID_HANDLE_VALUE)
  {
    CloseHandle(Pipe.In);
    Pipe.In = INVALID_HANDLE_VALUE;
    return (RETURN_PIPE_NOT_OPENED);
  }

  Message("OpenPipe: Done\n");
  return (RETURN_OK);
}

int uMod_TextureServer::ClosePipe(void) //called from ExitInstance, this must be done, otherwise the Mainloop will wait endless on the ReadFile()
{
  Message("ClosePipe:\n");

  // We close the outgoing pipe first.
  // The GUI will notice that the opposite side of it incoming pipe is closed
  // and closes it outgoing (our incoming) pipe and thus cancel the ReadFile() in the Mainloop()

  if (Pipe.Out != INVALID_HANDLE_VALUE)
  {
    DisconnectNamedPipe(Pipe.Out);
    CloseHandle(Pipe.Out);
    Pipe.Out = INVALID_HANDLE_VALUE;
  }

  if (Pipe.In != INVALID_HANDLE_VALUE)
  {
    DisconnectNamedPipe(Pipe.In);
    CloseHandle(Pipe.In);
    Pipe.In = INVALID_HANDLE_VALUE;
  }

  return (RETURN_OK);
}
