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
along with OpenTexMod.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "OTM_Main.h"

OTM_Sender::OTM_Sender(PipeStruct &pipe, OTM_Language &lang) : Pipe(pipe), Language(lang)
{
  try {Buffer = new char[BIG_BUFSIZE];}
  catch (...) {Buffer=NULL;}
}

OTM_Sender::~OTM_Sender(void)
{
  if (Buffer!=NULL) delete [] Buffer;
}


int OTM_Sender::Send( const OTM_GameInfo &game, const OTM_GameInfo &game_old, wxArrayString *comments)
{
  LastError.Empty();
  int key = game.GetKeyBack();
  if (key>=0 && key!=game_old.GetKeyBack())
  {
    key = Language.KeyValues[key];
    SendKey( key, CONTROL_KEY_BACK);
  }
  key = game.GetKeySave();
  if (key>=0 && key!=game_old.GetKeySave())
  {
    key = Language.KeyValues[key];
    SendKey( key, CONTROL_KEY_SAVE);
  }
  key = game.GetKeyNext();
  if (key>=0 && key!=game_old.GetKeyNext())
  {
    key = Language.KeyValues[key];
    SendKey( key, CONTROL_KEY_NEXT);
  }

  int colour[3], colour_old[3];
  game.GetFontColour( colour);
  game_old.GetFontColour( colour_old);
  for (int i=0; i<3; i++) if (colour[i]!=colour_old[i])
  {
    SendColour( colour, CONTROL_FONT_COLOUR);
    break;
  }

  game.GetTextureColour( colour);
  game_old.GetTextureColour( colour_old);
  for (int i=0; i<3; i++) if (colour[i]!=colour_old[i])
  {
    SendColour( colour, CONTROL_TEXTURE_COLOUR);
    break;
  }


  if ( game.GetSaveSingleTexture() != game_old.GetSaveSingleTexture() ) SendSaveSingleTexture( game.GetSaveSingleTexture());
  if ( game.GetSaveAllTextures() != game_old.GetSaveAllTextures() ) SendSaveAllTextures(game.GetSaveAllTextures());

  wxString path;
  path = game.GetSavePath();
  if (path!=game_old.GetSavePath()) SendPath(path);


  // the rest of this function is not optimized !!
  if (game.GetNumberOfFiles()<=0)
  {
    if (LastError.Len()>0) return 1;
    else return 0;
  }

  wxArrayString files;

  game.GetFiles( files);
  int num = files.GetCount();
  if (comments!=NULL)
  {
    comments->Empty();
    comments->Alloc(num);
  }
  bool *checked = new bool [num];
  game.GetChecked( checked, num);

  AddTextureClass *tex = new AddTextureClass[num];
  wxString comment;
  for (int i=0; i<num; i++)
  {
    OTM_File file( Language, files[i]);
    if (comments!=NULL)
    {
      file.GetComment(comment);
      comments->Add(comment);
    }
    if (file.GetContent( &tex[i], checked[i], true))
    {
      LastError << file.LastError;
      file.LastError.Empty();
    }
  }
  SendTextures( num, tex);

  delete [] checked;
  delete [] tex;

  if (LastError.Len()>0) return 1;
  else return 0;
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

  int count = 0;
  for (unsigned int i=0u; i<num; i++) count+=tex[i].Num;
  unsigned long *hash = new unsigned long[count];
  count = 0;

  MsgStruct *msg;
  int pos = 0;
  for (unsigned int i=0u; i<num; i++) for (unsigned int j=0u; j<tex[i].Num; j++)
  {
    bool hit = false;
    unsigned long temp_hash = tex[i].Hash[j];
    for (int h=0; h<count; h++) if (temp_hash == hash[h]) {hit= true; break;}
    if (hit) continue;

    if (tex[i].Size[j]+sizeof(MsgStruct)+pos>BIG_BUFSIZE)
    {
      if (int ret = SendToGame( Buffer, pos)) return ret;
      pos = 0;
    }

    hash[count++] = temp_hash;
    unsigned int size = tex[i].Size[j];
    msg = (MsgStruct*) &Buffer[pos];
    msg->Hash = temp_hash;
    msg->Value = size;
    pos += sizeof(MsgStruct);

    if (tex[i].Add[j])
    {
      if (tex[i].Force[j]) msg->Control = CONTROL_FORCE_RELOAD_TEXTURE_DATA;
      else  msg->Control = CONTROL_ADD_TEXTURE_DATA;

      char* temp = tex[i].Textures[j];
      if (temp!=NULL)
      {
        for (unsigned int l=0; l<size; l++) Buffer[pos+l] = temp[l];
        pos+=size;
      }
    }
    else msg->Control = CONTROL_REMOVE_TEXTURE;
  }
  delete [] hash;

  if (pos) if (int ret = SendToGame( Buffer, pos)) return ret;

  if (LastError.Len()>0) return 1;
  else return 0;
}


int OTM_Sender::SendKey(int key, int ctr)
{
  MsgStruct msg;
  msg.Control = ctr;
  msg.Value = key;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}

#define D3DCOLOR_ARGB(a,r,g,b) ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

int OTM_Sender::SendColour( int* colour, int ctr)
{
  MsgStruct msg;
  msg.Control = ctr;
  msg.Value = D3DCOLOR_ARGB( 255, colour[0], colour[1], colour[2]);
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}
#undef D3DCOLOR_ARGB


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

  if (Pipe.Out==INVALID_HANDLE_VALUE) {LastError << Language.Error_NoPipe; return -1;}
  bool ret = WriteFile( Pipe.Out, (const void*) msg, len, &num, NULL);
  if (!ret || len!=num) {LastError << Language.Error_WritePipe; return -1;}
  if (!FlushFileBuffers(Pipe.Out)) {LastError << Language.Error_FlushPipe; return -1;}
  return 0;
}

