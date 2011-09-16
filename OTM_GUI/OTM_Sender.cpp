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


AddTextureClass::AddTextureClass(void)
{
  Num = 0;
  Textures = NULL;
  Size = NULL;
  Hash = NULL;
  Force = NULL;
  Add = NULL;
  Len=0;
}
void AddTextureClass::SetSize(int num)
{
  Num = 0;
  Force = new bool[num];
  Add = new bool[num];
  Size = new unsigned int[num];
  for (int i=0; i<num; i++) Size[i] = 0;
  Hash = new unsigned long[num];
  Textures = new char*[num];
  for (int i=0; i<num; i++) Textures[i] = NULL;

  Len = num;
}

AddTextureClass::~AddTextureClass(void)
{
  if (Force!=NULL) delete [] Force;
  if (Add!=NULL) delete [] Add;
  if (Size!=NULL) delete [] Size;
  if (Hash!=NULL) delete [] Hash;

  if (Textures!=NULL)
  {
    for (unsigned int j=0; j<Len; j++) if (Textures[j]!=NULL) delete [] Textures[j];
    delete [] Textures;
  }
}

OTM_Sender::OTM_Sender(PipeStruct &pipe) : Pipe(pipe)
{
  try {Buffer = new char[BIG_BUFSIZE];}
  catch (...) {Buffer=NULL;}
}

OTM_Sender::~OTM_Sender(void)
{
  if (Buffer!=NULL) delete [] Buffer;
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



int OTM_Sender::SendTextures(unsigned int num, AddTextureClass *tex)
{
  if (Buffer==NULL) return (RETURN_NO_MEMORY);

  //wxMessageBox("OTM_Sender::SendTextures", "start", wxOK);
  MsgStruct *msg;
  int pos = 0;
  for (unsigned int i=0u; i<num; i++) for (unsigned int j=0u; j<tex[i].Num; j++)
  {
    if (tex[i].Size[j]+sizeof(MsgStruct)+pos>BIG_BUFSIZE)
    {
      if (int ret = SendToGame( Buffer, pos)) return ret;
      pos = 0;
    }
    msg = (MsgStruct*) &Buffer[pos];
    msg->Hash = tex[i].Hash[j];
    msg->Value = tex[i].Size[j];
    pos += sizeof(MsgStruct);

    if (tex[i].Add[j])
    {
      if (tex[i].Force[j]) msg->Control = CONTROL_FORCE_RELOAD_TEXTURE_DATA;
      else  msg->Control = CONTROL_ADD_TEXTURE_DATA;

      char* temp = tex[i].Textures[j];
      for (unsigned int l=0; l<tex[i].Size[j]; l++) Buffer[pos+l] = temp[l];
      pos+=tex[i].Size[j];
    }
    else msg->Control = CONTROL_REMOVE_TEXTURE;
  }
  if (pos) if (int ret = SendToGame( Buffer, pos)) return ret;

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
  MsgStruct *msg = (MsgStruct*) Buffer;

  msg->Hash = 0u;
  msg->Control = CONTROL_SET_DIR;

  const wchar_t *file = path.wc_str();
  wchar_t *buff_file = (wchar_t*) &Buffer[sizeof(MsgStruct)];
  int len = 0;
  while (file[len] && (sizeof(MsgStruct)+len*sizeof(wchar_t))<BIG_BUFSIZE) {buff_file[len] = file[len]; len++;};
  if ((sizeof(MsgStruct)+len*sizeof(wchar_t))<BIG_BUFSIZE) buff_file[len] = 0;
  len++;

  msg->Value = len*sizeof(wchar_t);
  return SendToGame( Buffer, sizeof(MsgStruct)+len*sizeof(wchar_t));
}


int OTM_Sender::SendToGame( void *msg, unsigned long len)
{
  if (len==0) return (RETURN_BAD_ARGUMENT);
  unsigned long num;
  if (Pipe.Out==INVALID_HANDLE_VALUE) return -1;
  bool ret = WriteFile( Pipe.Out, (const void*) msg, len, &num, NULL);
  if (!ret || len!=num) return -1;
  if (!FlushFileBuffers(Pipe.Out)) return -1;
  return 0;
}
