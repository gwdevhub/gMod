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


uMod_Sender::uMod_Sender(PipeStruct &pipe) : Pipe(pipe)
{
  try {Buffer = new char[BIG_BUFSIZE];}
  catch (...) {Buffer=(char*)0;}
  OldTextures = (uMod_TextureElement_SortedArrayPtr*)0;
}

uMod_Sender::~uMod_Sender(void)
{
  if (Buffer!=(char*)0) delete [] Buffer;
  if (OldTextures != (uMod_TextureElement_SortedArrayPtr*)0)
  {
    for (unsigned int t=0; t<OldTextures->GetCount(); t++)
    {
      OldTextures->Item(t)->Release();
    }
    delete OldTextures;
  }
}


int uMod_Sender::Send( const uMod_GameInfo &game, const uMod_GameInfo &game_old, uMod_TextureElement_SortedArrayPtr* current_textures, bool force)
{
  LastError.Empty();

  int key = game.GetKeyBack();
  if (key>=0 && key!=game_old.GetKeyBack())
    SendKey( key, CONTROL_KEY_BACK);

  key = game.GetKeySave();
  if (key>=0 && key!=game_old.GetKeySave())
    SendKey( key, CONTROL_KEY_SAVE);

  key = game.GetKeyNext();
  if (key>=0 && key!=game_old.GetKeyNext())
    SendKey( key, CONTROL_KEY_NEXT);

  unsigned char colour[4], colour_old[4];
  game.GetFontColour( colour);
  game_old.GetFontColour( colour_old);
  for (int i=0; i<4; i++) if (colour[i]!=colour_old[i])
  {
    SendColour( colour, CONTROL_FONT_COLOUR);
    break;
  }

  game.GetTextureColour( colour);
  game_old.GetTextureColour( colour_old);
  for (int i=0; i<4; i++) if (colour[i]!=colour_old[i])
  {
    SendColour( colour, CONTROL_TEXTURE_COLOUR);
    break;
  }

  SendDWORD64( game.FileFormat(), CONTROL_SAVE_FORMAT);

  if (game.UseFormatFilter())
    SendDWORD64( game.FormatFilter(), CONTROL_FORMAT_FILTER);
  else
    SendDWORD64( 0u, CONTROL_FORMAT_FILTER);

  if (game.UseSizeFilter())
  {
    SendMinMax( game.WidthMin(), game.WidthMax(), CONTROL_WIDTH_FILTER);
    SendMinMax( game.HeightMin(), game.HeightMax(), CONTROL_HEIGHT_FILTER);
    SendMinMax( game.DepthMin(), game.DepthMax(), CONTROL_DEPTH_FILTER);
  }
  else
  {
    SendMinMax( 0, 0, CONTROL_HEIGHT_FILTER);
    SendMinMax( 0, 0, CONTROL_HEIGHT_FILTER);
    SendMinMax( 0, 0, CONTROL_HEIGHT_FILTER);
  }

  SendBool( game.ShowSingleTextureString(), CONTROL_SHOW_STRING);
  SendBool( game.ShowSingleTexture(), CONTROL_SHOW_TEXTURE);
  SendBool( game.GetSaveSingleTexture(), CONTROL_SAVE_SINGLE);
  SendBool( game.GetSaveAllTextures(), CONTROL_SAVE_ALL);

  SendBool(game.SupportTPF(), CONTROL_SUPPORT_TPF);

  SendPath(game.GetSavePath());
  /*
  wxString path;
  path = game.GetSavePath();
  if (path!=game_old.GetSavePath()) SendPath(path);
*/



  if ( OldTextures== (uMod_TextureElement_SortedArrayPtr*)0) OldTextures = new uMod_TextureElement_SortedArrayPtr( Compare_uMod_TextureElement );

  //uMod_TextureElement_SortedArrayPtr* current_textures = new uMod_TextureElement_SortedArrayPtr( Compare_uMod_TextureElement );

  SendTextures( *OldTextures, *current_textures,  force);

  for (unsigned int t=0; t<OldTextures->GetCount(); t++)
  {
    OldTextures->Item(t)->Release();
  }
  delete OldTextures;
  OldTextures = current_textures;

  if (LastError.Len()>0) return 1;
  else return 0;
}



int uMod_Sender::SendSaveAllTextures(bool val)
{
  MsgStruct msg;
  msg.Control = CONTROL_SAVE_ALL;
  if (val) msg.Value = 1;
  else msg.Value = 0;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}

int uMod_Sender::SendSaveSingleTexture(bool val)
{
  MsgStruct msg;
  msg.Control = CONTROL_SAVE_SINGLE;
  if (val) msg.Value = 1;
  else msg.Value = 0;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}


int uMod_Sender::SendTextures(const uMod_TextureElement_SortedArrayPtr &old_tex, const uMod_TextureElement_SortedArrayPtr &new_tex, bool force)
{
  if (Buffer==(char*)0) return (RETURN_NO_MEMORY);

  unsigned int old_pos=0;
  unsigned int new_pos=0;
  MsgStruct *msg;
  int buffer_pos = 0;

  // both arrays are sorted
  // we go through both arrays in parallel
  while (old_pos<old_tex.GetCount() && new_pos<new_tex.GetCount())
  {
    if (old_tex[old_pos]->Hash() < new_tex[new_pos]->Hash())
    {
      RemoveTexture(old_tex[old_pos], buffer_pos);
      old_pos++;
    }
    else if (old_tex[old_pos]->Hash() < new_tex[new_pos]->Hash())
    {
      AddTexture(new_tex[new_pos], buffer_pos);
      new_pos++;
    }
    else
    {
      //add if forced or if pointer are not the same, which means that two different textures modify the same target texture
      if (force || old_tex[old_pos]!=new_tex[new_pos]) AddTexture( new_tex[new_pos], buffer_pos);
      new_pos++;
      old_pos++;
    }
  }

  // add remaining texture in new_tex
  while (new_pos<new_tex.GetCount())
  {
    AddTexture(new_tex[new_pos], buffer_pos);
    new_pos++;
  }


  // delete remaining texture in old_tex
  while (old_pos<old_tex.GetCount())
  {
    RemoveTexture(old_tex[old_pos], buffer_pos);
    old_pos++;
  }


  //empty the buffer if it can't hold 1 MsgStruct anymore
  if ((sizeof(MsgStruct) + buffer_pos) >=BIG_BUFSIZE) //the buffer is full
  {
    if (int ret = SendToGame( Buffer, buffer_pos)) return ret;
    buffer_pos = 0;
  }

  msg = (MsgStruct*) &Buffer[buffer_pos];
  msg->Control = CONTROL_END_TEXTURES; //End of texture sending
  msg->Hash = 0u;
  msg->Value = 0;
  buffer_pos += sizeof(MsgStruct);
  if (int ret = SendToGame( Buffer, buffer_pos)) return ret;

  if (LastError.Len()>0) return 1;
  else return 0;
}

int uMod_Sender::AddTexture( uMod_TextureElement *tex, int &buffer_pos)
{
  MsgStruct *msg;
  unsigned int size = tex->Content().Len();
  if ((sizeof(MsgStruct) + buffer_pos) >=BIG_BUFSIZE) //the buffer is full
  {
    if (int ret = SendToGame( Buffer, buffer_pos)) return ret;
    buffer_pos = 0;
  }

  msg = (MsgStruct*) &Buffer[buffer_pos];
  msg->Hash = tex->Hash();
  msg->Value = size;
  msg->Control = CONTROL_FORCE_RELOAD_TEXTURE_DATA;


  buffer_pos += sizeof(MsgStruct);

  const char* temp = tex->Content().Data();
  if (temp!=(char*)0)
  {
    unsigned int l = 0u;
    while (l<size)
    {
      while (buffer_pos<BIG_BUFSIZE && l<size) Buffer[buffer_pos++] = temp[l++];
      if (buffer_pos==BIG_BUFSIZE)
      {
        if (int ret = SendToGame( Buffer, buffer_pos)) return ret;
        buffer_pos = 0;
      }
    }
  }

  if (LastError.Len()>0) return 1;
  else return 0;
}

int uMod_Sender::RemoveTexture( uMod_TextureElement *tex, int &buffer_pos)
{
  MsgStruct *msg;
  //empty the buffer if it can't hold 1 MsgStruct anymore
  if ((sizeof(MsgStruct) + buffer_pos) >=BIG_BUFSIZE) //the buffer is full
  {
    if (int ret = SendToGame( Buffer, buffer_pos)) return ret;
    buffer_pos = 0;
  }

  msg = (MsgStruct*) &Buffer[buffer_pos];
  msg->Hash = tex->Hash();
  msg->Control = CONTROL_REMOVE_TEXTURE;
  buffer_pos += sizeof(MsgStruct);


  if (LastError.Len()>0) return 1;
  else return 0;
}

int uMod_Sender::SendKey(int key, int ctr)
{
  if (key<0) key=0;
  MsgStruct msg;
  msg.Control = ctr;
  msg.Value = key;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}

#define D3DCOLOR_ARGB(a,r,g,b) ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

int uMod_Sender::SendColour( unsigned char* colour, int ctr)
{
  MsgStruct msg;
  msg.Control = ctr;
  msg.Value = D3DCOLOR_ARGB( colour[3], colour[0], colour[1], colour[2]);
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}
#undef D3DCOLOR_ARGB


int uMod_Sender::SendPath( const wxString &path)
{
  MsgStruct *msg = (MsgStruct*) Buffer;

  msg->Hash = 0u;
  msg->Control = CONTROL_SET_DIR;
  int len = path.Len();
  if ((len+1)*sizeof(wchar_t) + sizeof(MsgStruct) >= BIG_BUFSIZE) return -1;

  const wchar_t *file = path.wc_str();
  wchar_t *buff_file = (wchar_t*) &Buffer[sizeof(MsgStruct)];
  for (int i=0; i<len; i++) {buff_file[i] = file[i];}
  buff_file[len] = 0;

  msg->Value = (len+1)*sizeof(wchar_t);
  return SendToGame( Buffer, sizeof(MsgStruct)+(len+1)*sizeof(wchar_t));
}


int uMod_Sender::SendBool( bool val, int ctr)
{
  MsgStruct msg;
  msg.Control = ctr;
  if (val) msg.Value = 1;
  else msg.Value = 0;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}
int uMod_Sender::SendDWORD64( DWORD64 val, int ctr)
{
  MsgStruct msg;
  msg.Control = ctr;
  msg.Value = val;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}

int uMod_Sender::SendMinMax( unsigned long min, unsigned long  max, int ctr)
{
  DWORD64 u_min = min, u_max = max;
  MsgStruct msg;
  msg.Control = ctr;
  msg.Value = u_min<<32 | u_max;
  msg.Hash = 0u;

  return SendToGame( (void*)  &msg, sizeof(MsgStruct));
}



int uMod_Sender::SendToGame( void *msg, unsigned long len)
{
  if (len==0) return (RETURN_BAD_ARGUMENT);
  unsigned long num;

  if (Pipe.Out==INVALID_HANDLE_VALUE) {LastError << Language->Error_NoPipe; return -1;}
  bool ret = WriteFile( Pipe.Out, (const void*) msg, len, &num, NULL);
  if (!ret || len!=num) {LastError << Language->Error_WritePipe; return -1;}
  if (!FlushFileBuffers(Pipe.Out)) {LastError << Language->Error_FlushPipe; return -1;}
  return 0;
}

