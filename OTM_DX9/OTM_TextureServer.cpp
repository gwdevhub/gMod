/*
This file is part of OpenTexMod.


OpenTexMod is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OpenTexMod is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FooOpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "OTM_Main.h"

OTM_TextureServer::OTM_TextureServer(wchar_t *game)
{
  Message("OTM_TextureServer(void): %lu\n", this);
  Message("sizeof(unsigned long)=%lu\n", sizeof(unsigned long));
  Message("sizeof(DWORDLONG)=%lu\n", sizeof(DWORDLONG));
  Mutex = CreateMutex(NULL, false, NULL);

  Clients = NULL;
  NumberOfClients = 0;
  LenghtOfClients = 0;
  BoolSaveAllTextures = false;
  BoolSaveSingleTexture = false;
  SavePath[0] = 0;
  int i=0;
  for (i=0; i<MAX_PATH && (game[i]) && (game[i]!='.'); i++) GameName[i] = game[i];
  if (i<MAX_PATH) GameName[i] = 0;
  else GameName[0] = 0;

  KeyBack = 0;
  KeySave = 0;
  KeyNext = 0;

  FontColour = 0u;
  TextureColour = 0u;

  Pipe.In = INVALID_HANDLE_VALUE;
  Pipe.Out = INVALID_HANDLE_VALUE;
}

OTM_TextureServer::~OTM_TextureServer(void)
{
  Message("~OTM_TextureServer(void): %lu\n", this);
  if (Mutex != NULL) CloseHandle(Mutex);

  //delete the files in memory
  int num = CurrentMod.GetNumber();
  for (int i = 0; i < num; i++) delete[] CurrentMod[i]->pData;

  num = OldMod.GetNumber();
  for (int i = 0; i < num; i++) delete[] OldMod[i]->pData;

  if (Pipe.In != INVALID_HANDLE_VALUE ) CloseHandle(Pipe.In);
  Pipe.In = INVALID_HANDLE_VALUE;
  if (Pipe.Out != INVALID_HANDLE_VALUE) CloseHandle(Pipe.Out);
  Pipe.Out = INVALID_HANDLE_VALUE;
}

int OTM_TextureServer::AddClient(OTM_TextureClient *client, TextureFileStruct** update, int* number) // called from a client
{
  Message("AddClient(%lu): %lu\n", client, this);
  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_SERVER;
    return (ret);
  }

  // the following functions must not change the original OTM_IDirect3DDevice9 object
  // somehow on game start some OTM_IDirect3DDevice9 object are created, which must rest unchanged!!
  // these objects are released and are not uses for rendering
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


  if (int ret = PrepareUpdate( update, number)) return (ret);

  OTM_TextureClient** temp = NULL;

  if (NumberOfClients == LenghtOfClients)
  {
    try {temp = new OTM_TextureClient*[LenghtOfClients + 10];}
    catch (...)
    {
      gl_ErrorState |= OTM_ERROR_MEMORY | OTM_ERROR_SERVER;
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

int OTM_TextureServer::RemoveClient(OTM_TextureClient *client) // called from a client
{
  Message("RemoveClient(%lu): %lu\n", client);
  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_SERVER;
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

int OTM_TextureServer::AddFile( char* buffer, unsigned int size,  MyTypeHash hash, bool force) // called from the server
{
  Message("OTM_TextureServer::AddFile( %lu %lu, %#lX, %d): %lu\n", buffer, size, hash, force, this);

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
    gl_ErrorState |= OTM_ERROR_MEMORY | OTM_ERROR_SERVER;
    return (RETURN_NO_MEMORY);
  }

  for (unsigned int i=0; i<size; i++) temp->pData[i] = buffer[i];

  temp->Checked = false;
  temp->Size = size;
  temp->pTexture = NULL;
  temp->Hash = hash;

  if (new_file) temp->ForceReload = false;
  else temp->ForceReload = force;

  Message("End AddFile(%#lX)\n", hash);
  if (new_file) return (CurrentMod.Add(temp));
  else return (RETURN_OK);
}

int OTM_TextureServer::AddFile(wchar_t* file_name, MyTypeHash hash, bool force) // called from the server
{
  Message("OTM_TextureServer::AddFile( %ls, %#lX, %d): %lu\n", file_name, hash, force, this);

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
    gl_ErrorState |= OTM_ERROR_MEMORY | OTM_ERROR_SERVER;
    return (RETURN_NO_MEMORY);
  }
  //Message("Read AddFile( %lu)\n", hash, this);
  int result = fread(temp->pData, 1, size, file);
  fclose(file);
  if (result != size)
  {
    //Message("Read Failed AddFile( %lu): %lu\n", hash, this);
    delete[] temp->pData;
    if (!new_file) CurrentMod.Remove( temp);
    delete temp;
    return (RETURN_FILE_NOT_LOADED);
  }

  temp->Checked = false;
  temp->Size = size;
  temp->pTexture = NULL;
  temp->Hash = hash;

  if (new_file) temp->ForceReload = false;
  else temp->ForceReload = force;

  Message("End AddFile(%#lX)\n", hash);
  if (new_file) return (CurrentMod.Add(temp));
  else return (RETURN_OK);
}

int OTM_TextureServer::RemoveFile(MyTypeHash hash) // called from the server
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

int OTM_TextureServer::SaveAllTextures(bool val) // called from the server
{
  if (BoolSaveAllTextures == val) return (RETURN_OK);
  BoolSaveAllTextures = val;

  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_SERVER;
    return (ret);
  }
  for (int i = 0; i < NumberOfClients; i++)
  {
    Clients[i]->SaveAllTextures(BoolSaveAllTextures);
  }
  return (UnlockMutex());
}

int OTM_TextureServer::SaveSingleTexture(bool val) // called from the server
{
  if (BoolSaveSingleTexture == val) return (RETURN_OK);
  BoolSaveSingleTexture = val;

  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_SERVER;
    return (ret);
  }
  for (int i = 0; i < NumberOfClients; i++)
  {
    Clients[i]->SaveSingleTexture(BoolSaveSingleTexture);
  }
  return (UnlockMutex());
}

int OTM_TextureServer::SetSaveDirectory(wchar_t *dir) // called from the server
{
  Message("OTM_TextureServer::SetSaveDirectory( %ls): %lu\n", dir, this);
  int i = 0;
  for (i = 0; i < MAX_PATH && (dir[i]); i++) SavePath[i] = dir[i];
  if (i == MAX_PATH)
  {
    SavePath[0] = 0;
    return (RETURN_BAD_ARGUMENT);
  }
  SavePath[i] = 0;

  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_SERVER;
    return (ret);
  }
  for (int i = 0; i < NumberOfClients; i++)
  {
    Clients[i]->SetSaveDirectory(SavePath);
  }
  return (UnlockMutex());
}

int OTM_TextureServer::SetKeyBack(int key) // called from the server
{
  if (KeyBack == key || KeySave == key || KeyNext == key) return (RETURN_OK);
  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_SERVER;
    return (ret);
  }
  KeyBack = key;
  for (int i = 0; i < NumberOfClients; i++)
  {
    Clients[i]->SetKeyBack(key);
  }
  return (UnlockMutex());
}

int OTM_TextureServer::SetKeySave(int key) // called from the server
{
  if (KeyBack == key || KeySave == key || KeyNext == key) return (RETURN_OK);
  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_SERVER;
    return (ret);
  }
  KeySave = key;
  for (int i = 0; i < NumberOfClients; i++)
  {
    Clients[i]->SetKeySave(key);
  }
  return (UnlockMutex());
}

int OTM_TextureServer::SetKeyNext(int key) // called from the server
{
  if (KeyBack == key || KeySave == key || KeyNext == key) return (RETURN_OK);
  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_SERVER;
    return (ret);
  }
  KeyNext = key;
  for (int i = 0; i < NumberOfClients; i++)
  {
    Clients[i]->SetKeyNext(key);
  }
  return (UnlockMutex());
}

int OTM_TextureServer::SetFontColour(DWORD colour) // called from the server
{
  if (colour==0u) return (RETURN_OK);
  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_SERVER;
    return (ret);
  }
  FontColour = colour;
  DWORD r = (FontColour>>16)&0xFF;
  DWORD g = (FontColour>>8)&0xFF;
  DWORD b = (FontColour)&0xFF;
  Message("OTM_TextureServer::SetFontColour( %u %u %u): %lu\n", r ,g ,b, this);
  for (int i = 0; i < NumberOfClients; i++)
  {
    Clients[i]->SetFontColour( r, g, b);
  }
  return (UnlockMutex());
}

int OTM_TextureServer::SetTextureColour(DWORD colour) // called from the server
{
  if (colour==0u) return (RETURN_OK);
  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_SERVER;
    return (ret);
  }
  TextureColour = colour;
  DWORD r = (TextureColour>>16)&0xFF;
  DWORD g = (TextureColour>>8)&0xFF;
  DWORD b = (TextureColour)&0xFF;
  Message("OTM_TextureServer::SetTextureColour( %u %u %u): %lu\n", r ,g ,b, this);
  for (int i = 0; i < NumberOfClients; i++)
  {
    Clients[i]->SetTextureColour( r, g, b);
  }
  return (UnlockMutex());
}

int OTM_TextureServer::PropagateUpdate(OTM_TextureClient* client) // called from the server, send the update to all clients
{
  Message("PropagateUpdate(%lu): %lu\n", client, this);
  if (int ret = LockMutex())
  {
    gl_ErrorState |= OTM_ERROR_TEXTURE;
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
    for (int i = 0; i < NumberOfClients; i++)
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
{ a.Checked = b.Checked; \
  a.ForceReload = b.ForceReload; \
  a.pData = b.pData; \
  a.Size = b.Size; \
  a.Reference = b.Reference; \
  a.pTexture = b.pTexture; \
  a.Hash = b.Hash; }

int OTM_TextureServer::PrepareUpdate(TextureFileStruct** update, int* number) // called from the server, prepare an update for one client. the allocated memory must deleted by the client
{
  Message("PrepareUpdate(%lu, %d): %lu\n", update, number, this);
  TextureFileStruct* temp = NULL;

  int num = CurrentMod.GetNumber();
  if (num>0)
  {
    try {temp = new TextureFileStruct[num];}
    catch (...)
    {
      gl_ErrorState |= OTM_ERROR_MEMORY | OTM_ERROR_SERVER;
      return (RETURN_NO_MEMORY);
    }
  }

  for (int i = 0; i < num; i++) cpy_file_struct(temp[i], (*(CurrentMod[i])));

  *update = temp;
  *number = num;
  return (RETURN_OK);
}
#undef cpy_file_struct

int OTM_TextureServer::LockMutex(void)
{
  //if (( gl_ErrorState & (OTM_ERROR_FATAL | OTM_ERROR_MUTEX) )) return (RETURN_NO_MUTEX);
  //if (WAIT_OBJECT_0!=WaitForSingleObject( Mutex, 100)) return (RETURN_MUTEX_LOCK); //waiting 100ms, to wait infinite pass INFINITE
  return (RETURN_OK);
}

int OTM_TextureServer::UnlockMutex(void)
{
  //if (ReleaseMutex(Mutex)==0) return (RETURN_MUTEX_UNLOCK);
  return (RETURN_OK);
}

int OTM_TextureServer::MainLoop(void) // run as a separated thread !!
{
  Message("MainLoop: begin\n");
  if (Pipe.In == INVALID_HANDLE_VALUE) return (RETURN_PIPE_NOT_OPENED);
  char *buffer;
  try {buffer = new char[BIG_BUFSIZE];}
  catch (...) {return (RETURN_NO_MEMORY);}

  unsigned long num;

  Message("MainLoop: started\n");
  while (1)
  {
    Message("MainLoop: run\n");
    bool ret = ReadFile(Pipe.In, // pipe handle
        buffer, // buffer to receive reply
        BIG_BUFSIZE, // size of buffer
        &num, // number of bytes read
        NULL); // not overlapped

    Message("MainLoop: read something (%lu)\n", num);
    //for (int i=0; i<size; i++) {Message("%d\n",buffer[i]);}
    if (ret || GetLastError() == ERROR_MORE_DATA)
    {
      unsigned int pos = 0;
      MsgStruct *commands;
      //wchar_t *file;
      bool update_textures = false;
      while (pos <= num - sizeof(MsgStruct))
      {
        commands = (MsgStruct*) &buffer[pos];
        unsigned int size = 0u;
        bool force = false;

        switch (commands->Control)
        {
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
          Message("MainLoop: CONTROL_ADD_TEXTURE_DATA (%#lX  %u,  %u %u): %lu\n", commands->Hash, size, sizeof(MsgStruct), sizeof(char), this);
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
      if (update_textures) PropagateUpdate();
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

int OTM_TextureServer::OpenPipe(void) // called from InitInstance()
{
  Message("OpenPipe: Out\n")
  // open first outgoing pipe !!
  Pipe.Out = CreateFileW(PIPE_Game2OTM, // pipe name
      GENERIC_WRITE, // write access
      0, // no sharing
      NULL, // default security attributes
      OPEN_EXISTING, // opens existing pipe
      0, // default attributes
      NULL); // no template file

  // Exit if an error other than ERROR_PIPE_BUSY occurs.
  if (Pipe.Out == INVALID_HANDLE_VALUE) return (RETURN_PIPE_NOT_OPENED);

  unsigned int len = 0u;
  while (GameName[len]) len++;
  len++; //to send also the zero
  unsigned long num;
  //send name of this game to OTM_GUI
  WriteFile(Pipe.Out, (const void*) GameName, len * sizeof(wchar_t), &num, NULL);

  Message("OpenPipe: In\n");
  Pipe.In = CreateFileW(PIPE_OTM2Game, // pipe name
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

int OTM_TextureServer::ClosePipe(void) //called from ExitInstance, this must be done, otherwise the Mainloop will wait endless on the ReadFile()
{
  Message("ClosePipe:\n");

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
