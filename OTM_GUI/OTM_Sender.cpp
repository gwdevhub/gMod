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

#include "unzip.h"

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










OTM_Sender::OTM_Sender(PipeStruct &pipe, OTM_Language &lang) : Pipe(pipe), Language(lang)
{
  try {Buffer = new char[BIG_BUFSIZE];}
  catch (...) {Buffer=NULL;}
}

OTM_Sender::~OTM_Sender(void)
{
  if (Buffer!=NULL) delete [] Buffer;
}


int OTM_Sender::Send( const OTM_GameInfo &game, const OTM_GameInfo &game_old)
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
  if (game.GetNumberOfTextures()<=0)
  {
    if (LastError.Len()>0) return 1;
    else return 0;
  }

  wxArrayString textures;

  game.GetTextures( textures);
  int num = textures.GetCount();
  bool *checked = new bool [num];
  game.GetChecked( checked, num);

  AddTextureClass *tex = new AddTextureClass[num];

  wxString file_type;
  unsigned long temp_hash;
  for (int i=0; i<num; i++)
  {
    file_type = textures[i];
    file_type = file_type.AfterLast( '.');


    if (file_type == L"zip")
    {
      AddZip( &tex[i], textures[i], checked[i], true, false);
    }
    else if (file_type == L"tpf")
    {
      AddZip( &tex[i], textures[i], checked[i], true, true);
    }
    else if (file_type == L"dds")
    {
      AddFile( &tex[i], textures[i], checked[i], true);
    }
    else
    {
      wxString msg = Language.FileNotSupported;
      msg << textures[i];
      wxMessageBox(msg, "ERROR", wxOK);
    }
  }
  SendTextures( num, tex);

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
  for (unsigned int i=0u; i<num; i++) count+=tex->Num;
  unsigned long *hash = new unsigned long[count];
  count = 0;

  //wxMessageBox("OTM_Sender::SendTextures", "start", wxOK);
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
    msg = (MsgStruct*) &Buffer[pos];
    msg->Hash = temp_hash;
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


int OTM_Sender::AddFile( AddTextureClass *tex, wxString file, bool add, bool force)
{
  tex->SetSize(1);
  unsigned long temp_hash;

  wxString name = file.AfterLast( '_');
  name = name.BeforeLast( '.');
  if (!name.ToULong( &temp_hash, 16)) {LastError << Language.Error_Hash <<"\n" << file; return -1;} // return if hash could not be extracted

  tex->Add[0] = add;
  if (add)
  {
    wxFile dat;
    if (!dat.Access(name, wxFile::read)) {LastError << Language.Error_FileOpen <<"\n" << file; return -1;}
    dat.Open(file, wxFile::read);
    if (!dat.IsOpened()) {LastError << Language.Error_FileOpen <<"\n" << file; return -1;}
    unsigned len = file.Length();

    try {tex->Textures[0] = new char [len];}
    catch (...) {tex->Textures[0] = NULL; LastError << Language.Error_Memory; return -1;}

    unsigned int result = dat.Read( (void*) tex->Textures[0], len);
    dat.Close();

    if (result != len) {LastError << Language.Error_FileRead <<"\n" << file; return -1;}
    tex->Size[0] = len;
  }
  else {tex->Size[0] = 0; tex->Textures[0] = NULL;}

  tex->Num = 1;
  tex->Force[0] = true;
  tex->Hash[0] = temp_hash;
  return 0;
}

int OTM_Sender::AddZip( AddTextureClass *tex, wxString file, bool add, bool force, bool tpf)
{
  wxFile dat;
  if (!dat.Access(file, wxFile::read)) {LastError << Language.Error_FileOpen <<"\n" << file; return -1;}
  dat.Open(file, wxFile::read);
  if (!dat.IsOpened()) {LastError << Language.Error_FileOpen <<"\n" << file; return -1;}
  unsigned len = dat.Length();

  unsigned char* buffer;
  try {buffer = new unsigned char [len];}
  catch (...) {LastError << Language.Error_Memory; return -1;}

  unsigned int result = dat.Read( buffer, len);
  dat.Close();

  if (result != len) {LastError << Language.Error_FileRead<<"\n" << file; return -1;}

  if (tpf)
  {
    /*
     *
     * BIG THANKS TO Tonttu
     * (TPFcreate 1.5)
     *
     */
    unsigned int TPF_XOR = 0x3FA43FA4u;
    const char pw[] = {0x73, 0x2A, 0x63, 0x7D, 0x5F, 0x0A, 0xA6, 0xBD,
          0x7D, 0x65, 0x7E, 0x67, 0x61, 0x2A, 0x7F, 0x7F,
          0x74, 0x61, 0x67, 0x5B, 0x60, 0x70, 0x45, 0x74,
          0x5C, 0x22, 0x74, 0x5D, 0x6E, 0x6A, 0x73, 0x41,
          0x77, 0x6E, 0x46, 0x47, 0x77, 0x49, 0x0C, 0x4B,
          0x46, 0x6F, '\0'};



    unsigned int j=0;
    while ( j <= result - 4 )
    {
      *( unsigned int* )( &buffer[j] ) ^= TPF_XOR;
      j += 4;
    }
    while ( j < result )
    {
      buffer[j] ^= (unsigned char )( TPF_XOR >> 24 );
      TPF_XOR <<= 4;
      j++;
    }

    AddContent( (char*) buffer, len, pw, tex, add,  force);
  }
  else
  {
    AddContent( (char*) buffer, len, NULL, tex, add,  force);
  }

  delete [] buffer;

  return 0;
}


int OTM_Sender::AddContent( char* buffer, unsigned int len, const char* pw, AddTextureClass *tex, bool add, bool force)
{
  HZIP hz = OpenZip( (void*) buffer, len, pw);

  ZIPENTRY ze;
  int index;
  FindZipItem( hz, L"texmod.def", false, &index, &ze);
  if (index>=0) //if texmod.def is present in the zip file
  {
    char* def;
    int len = ze.unc_size;
    try {def=new char[len+1];}
    catch(...) {CloseZip(hz); LastError << Language.Error_Memory; return -1;}
    ZRESULT zr = UnzipItem( hz,index, def, len);

    if (zr!=ZR_OK && zr!=ZR_MORE) {delete [] def; CloseZip(hz); return -1;}
    def[len]=0;

    wxStringTokenizer token( def, "\n");

    int num = token.CountTokens();


    tex->SetSize(num);

    unsigned long temp_hash;
    int pos = 0;
    int count = 0;
    wxString entry;
    wxString file;

    for (int i=0; i<num; i++)
    {
      entry = token.GetNextToken();
      file = entry.BeforeFirst( '|');
      if (!file.ToULong( &temp_hash, 16)) {LastError << Language.Error_Hash <<"\nTPF:" << file; continue;}

      file = entry.AfterFirst( '|');
      file.Replace( "\r", "");

      if (add)
      {
        FindZipItem( hz, file.wc_str(), false, &index, &ze); // look for texture
        if (index>=0)
        {
          try {tex->Textures[count] = new char[ze.unc_size];}
          catch(...)
          {
            tex->Textures[count] = NULL;
            LastError << Language.Error_Memory;
            continue;
          }

          if (ZR_OK!=UnzipItem( hz, index, tex->Textures[count], ze.unc_size))
          {
            delete [] tex->Textures[count];
            LastError << Language.Error_Unzip <<"\nTPF:" << file;
            tex->Textures[count] = NULL;
          }
          else
          {
            tex->Hash[count] = temp_hash;
            tex->Add[count] = true;
            tex->Size[count] = ze.unc_size;
            tex->Force[count] = force;
            count++;
          }
        }
      }
      else
      {
        tex->Hash[count] = temp_hash;
        tex->Add[count] = false;
        tex->Size[count] = 0;
        tex->Force[count] = force;
        count++;
      }
    }
    tex->Num = count;
  }
  else // texmod.def is not present in the zip file
  {
    wxString name;
    wxString file;
    GetZipItem( hz, -1, &ze); //ask for number of entries
    index = ze.index;

    tex->SetSize(index);
    int count = 0;
    unsigned long temp_hash;
    for (int i=0; i<index; i++)
    {
      if (GetZipItem( hz, i, &ze)!=ZR_OK) continue; //ask for name and size
      file = ze.name;

      name = file.AfterLast( '.');
      if (name!="dds") {LastError << Language.Error_FileformatNotSupported <<"\nZIP: " << file; continue;} //if this is not texture file, continue

      name = file.AfterLast( '_');
      name = name.BeforeLast( '.');
      if (!name.ToULong( &temp_hash, 16)) {LastError << Language.Error_Hash <<"\nZIP:" << file; continue;} //if hash couldt not be extracted

      if (add)
      {
        try {tex->Textures[count] = new char[ze.unc_size];}
        catch(...)
        {
          tex->Textures[count] = NULL;
          LastError << Language.Error_Memory;
          continue;
        }

        ZRESULT rz = UnzipItem( hz, i, tex->Textures[count], ze.unc_size);
        if (ZR_OK!=rz)
        {
          delete [] tex->Textures[count];
          LastError << Language.Error_Unzip <<"\nZIP:" << file;
          tex->Textures[count] = NULL;
        }
        else
        {
          tex->Hash[count] = temp_hash;
          tex->Add[count] = true;
          tex->Size[count] = ze.unc_size;
          tex->Force[count] = force;
          count++;
        }
      }
      else
      {
        tex->Hash[count] = temp_hash;
        tex->Add[count] = false;
        tex->Size[count] = 0;
        tex->Force[count] = force;
        count++;
      }
    }
    tex->Num = count;
  }
  CloseZip(hz);
  return 0;
}
