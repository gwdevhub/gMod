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

OTM_Sender::OTM_Sender(PipeStruct &pipe) : Pipe(pipe)
{
}

OTM_Sender::~OTM_Sender(void)
{
}

int OTM_Sender::SendSaveAllTextures(bool val)
{
  MsgStruct msg;
  msg.Control = CONTROL_SAVE_ALL;
  if (val) msg.Value = 1;
  else msg.Value = 0;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}

int OTM_Sender::SendSaveSingleTexture(bool val)
{
  MsgStruct msg;
  msg.Control = CONTROL_SAVE_SINGLE;
  if (val) msg.Value = 1;
  else msg.Value = 0;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}


int OTM_Sender::SendTextures(const wxArrayString &textures, unsigned long *hash, bool *add, bool *force)
{
  char buffer[BUFSIZE];
  int num = textures.GetCount();
  MsgStruct *msg;
  int pos = 0;
  int old_pos = 0;
  for (int i=0; i<num; i++)
  {
    msg = (MsgStruct*) &buffer[pos];
    msg->Hash = hash[i];
    if (add[i])
    {
      if (force[i]) msg->Control = CONTROL_FORCE_RELOAD_TEXTURE;
      else  msg->Control = CONTROL_ADD_TEXTURE;
    }
    else msg->Control = CONTROL_REMOVE_TEXTURE;
    msg->Value = 0;
    pos += sizeof(MsgStruct);

    if (add[i] && pos<BUFSIZE)
    {
      const wchar_t *file = textures[i].wc_str();
      wchar_t *buff_file = (wchar_t*) &buffer[pos];
      int len = 0;
      while (file[len] && (pos+len*sizeof(wchar_t))<BUFSIZE) {buff_file[len] = file[len]; len++;};
      if ((pos+len*sizeof(wchar_t))<BUFSIZE) buff_file[len] = 0;
      len++;
      pos+=len*sizeof(wchar_t);
    }
    if (pos>=BUFSIZE)
    {
      if (int ret = SendToGame( buffer, old_pos)) return ret;
      pos = 0; old_pos = 0; i--;
    }
    else old_pos = pos;
  }
  if (old_pos) if (int ret = SendToGame( buffer, old_pos)) return ret;
  return 0;
}


int OTM_Sender::SendKeyBack(int key)
{
  MsgStruct msg;
  msg.Control = CONTROL_KEY_BACK;
  msg.Value = key;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}

int OTM_Sender::SendKeySave(int key)
{
  MsgStruct msg;
  msg.Control = CONTROL_KEY_SAVE;
  msg.Value = key;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}

int OTM_Sender::SendKeyNext(int key)
{
  MsgStruct msg;
  msg.Control = CONTROL_KEY_NEXT;
  msg.Value = key;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}



int OTM_Sender::SendPath( const wxString &path)
{
  char buffer[BUFSIZE];
  MsgStruct *msg = (MsgStruct*) buffer;

  msg->Hash = 0u;
  msg->Control = CONTROL_SET_DIR;
  msg->Value = 0;

  const wchar_t *file = path.wc_str();
  wchar_t *buff_file = (wchar_t*) &buffer[sizeof(MsgStruct)];
  int len = 0;
  while (file[len] && (sizeof(MsgStruct)+len*sizeof(wchar_t))<BUFSIZE) {buff_file[len] = file[len]; len++;};
  if ((sizeof(MsgStruct)+len*sizeof(wchar_t))<BUFSIZE) buff_file[len] = 0;
  len++;

  return SendToGame( buffer, sizeof(MsgStruct)+len*sizeof(wchar_t));
}


int OTM_Sender::SendToGame( void *msg, unsigned long len)
{
  unsigned long num;
  if (Pipe.Out==INVALID_HANDLE_VALUE) return -1;
  bool ret = WriteFile( Pipe.Out, (const void*) msg, len, &num, NULL);
  if (!ret || len!=num) return -1;
  return 0;
}
