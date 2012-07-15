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
#include "unzip.h"


#ifdef __CDT_PARSER__
#define FindZipItem(...) 0
#define UnzipItem(...) 0
#define CloseZip(...) 0
#define GetZipItem(...) 0
#endif


uMod_File::uMod_File(void)
{
  Loaded=false;
  XORed=false;
  FileInMemory=(char*)0;
  MemoryLength=0u;
  FileLen=0u;
}

uMod_File::uMod_File(const wxString &file)
{
  Loaded=false;
  XORed=false;
  FileInMemory=(char*)0;
  MemoryLength=0u;
  FileLen=0u;
  SetFile(file);
}


uMod_File::~uMod_File(void)
{
  if (FileInMemory!=(char*)0) delete [] FileInMemory;
}


bool uMod_File::FileSupported(void)
{
  wxString file_type = FileName.AfterLast( '.');
  if (file_type == L"zip") return true;
  else if (file_type == L"tpf") return true;
  else if (file_type == L"dds") return true;

  return false;
}


bool uMod_File::SingleFile(void)
{
  wxString file_type = FileName.AfterLast( '.');
  if (file_type == L"dds") return true;
  return false;
}

/*
int uMod_File::GetComment( wxString &tool_tip)
{
  wxString file_type = FileName.AfterLast( '.');
  if (file_type == L"zip")
  {
    if (int ret = GetCommentZip( tool_tip)) return ret;
  }
  else if (file_type == L"tpf")
  {
    if (int ret = GetCommentTpf( tool_tip)) return ret;
  }
  else if (file_type == L"dds")
  {
    tool_tip = Language->NoComment;
    return -1;
  }
  return 0;
}

int uMod_File::GetContent( AddTextureClass &tex, bool add)
{
  wxString file_type = FileName.AfterLast( '.');
  if (file_type == L"zip")
  {
    AddZip( tex, add, false);
  }
  else if (file_type == L"tpf")
  {
    AddZip( tex, add, true);
  }
  else if (file_type == L"dds")
  {
    AddFile( tex, add);
  }
  else
  {
    LastError << Language->Error_FileNotSupported;
    LastError << "\n" << FileName;
  }
  if (LastError.Len()>0) return -1;
  else
  {
    if (add) tex.Loaded = true;
    return 0;
  }
}
*/


int uMod_File::GetContent( uMod_TreeViewNode* node)
{
  wxString file_type = FileName.AfterLast( '.');
  if (file_type == L"zip")
  {
    AddZip( node);
  }
  else if (file_type == L"tpf")
  {
    AddTpf( node);
  }
  else if (file_type == L"dds")
  {
    AddFile( node);
  }
  else
  {
    LastError << Language->Error_FileNotSupported;
    LastError << "\n" << FileName;
  }
  if (LastError.Len()>0) return -1;
  return 0;
}

int uMod_File::ReadFile(void)
{
  if (Loaded) return 0;
  XORed=false;

  wxFile dat;
  if (!dat.Access(FileName, wxFile::read)) {LastError << Language->Error_FileOpen <<"\n" << FileName; return -1;}
  dat.Open(FileName, wxFile::read);
  if (!dat.IsOpened()) {LastError << Language->Error_FileOpen <<"\n" << FileName; return -1;}
  FileLen = dat.Length();
  if (FileLen==0) {LastError << Language->Error_FileOpen <<"\n" << FileName; return -1;}

  if (FileLen>=MemoryLength)
  {
    if (FileInMemory!=(char*)0) delete [] FileInMemory;
    try {FileInMemory = new char [FileLen+1];}
    catch (...) {FileInMemory=(char*)0; MemoryLength=0; FileLen=0; LastError << Language->Error_Memory; return -1;}
    MemoryLength = FileLen+1;
  }
  unsigned int result = dat.Read( FileInMemory, FileLen);
  dat.Close();

  if (result != FileLen) {FileLen=0; LastError << Language->Error_FileRead<<"\n" << FileName; return -1;}
  FileInMemory[FileLen]=0;

  Loaded = true;
  return 0;
}



int uMod_File::UnXOR(void)
{
  if (XORed) return 0;
  /*
   *
   * BIG THANKS TO Tonttu
   * (TPFcreate 1.5)
   *
   */
  unsigned int *buff = (unsigned int*) FileInMemory;
  unsigned int TPF_XOR = 0x3FA43FA4u;
  unsigned int size = FileLen/4u;
  for (unsigned int i=0; i<size; i++) buff[i] ^= TPF_XOR;
  for (unsigned int i=size*4u; i<size*4u+FileLen%4u; i++) ((unsigned char*) FileInMemory)[i]^=(unsigned char) TPF_XOR;


  unsigned int pos = FileLen-1;
  while (pos>0u && FileInMemory[pos]) pos--;
  if (pos>0u && pos<FileLen-1) FileLen = pos+1;
  XORed = true;

  /*
   * original code by Tonttu
   * The last bytes are not revealed correctly
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
  */
  return 0;
}



/*
int uMod_File::AddFile( uMod_TreeViewNode* node)
{
  DWORD64 temp_hash;

  wxString name = FileName.AfterLast( '_');
  name = name.BeforeLast( '.');
  if (!name.ToULongLong( &temp_hash, 16)) {LastError << Language->Error_Hash <<"\n" << FileName << "\n"; return -1;} // return if hash could not be extracted

  if (int ret = ReadFile()) return ret;

  uMod_TextureElement *texture = new uMod_TextureElement();

  if (texture->Content().SetSize(FileLen))
  {
    LastError << Language->Error_Memory;
    texture->Release();
    return -1;
  }
  char *data = texture->Content().Data();

  for (unsigned int i=0; i<FileLen; i++) data[i] = FileInMemory[i];

  texture->Hash() = temp_hash;
  texture->Title() = FileName;
  texture->Status() = uMod_TextureElement::Activate;

  uMod_ModElement *element = new uMod_ModElement(uMod_ModElement::Group, uMod_ModElement::NONE);
  element->Title() = FileName;
  node = new uMod_TreeViewNode( (uMod_TreeViewNode*)0, element);
  uMod_TreeViewNode *texture_node = new uMod_TreeViewNode( node, texture);
  node->Append(texture_node);

  texture->Release();
  element->Release();
  return 0;
}
*/

int uMod_File::AddFile( uMod_TreeViewNode* node)
{
  DWORD64 temp_hash;

  wxString name = FileName.AfterLast( '_');
  name = name.BeforeLast( '.');
  if (!name.ToULongLong( &temp_hash, 16)) {LastError << Language->Error_Hash <<"\n" << FileName << "\n"; return -1;} // return if hash could not be extracted

  if (int ret = ReadFile()) return ret;

  uMod_TextureElement *texture = new uMod_TextureElement();

  if (texture->Content().SetSize(FileLen))
  {
    LastError << Language->Error_Memory;
    texture->Release();
    return -1;
  }
  char *data = texture->Content().Data();

  for (unsigned int i=0; i<FileLen; i++) data[i] = FileInMemory[i];

  texture->Hash() = temp_hash;
  texture->File() = FileName;
  texture->Status() = uMod_TextureElement::Activate;


  uMod_TreeViewNode *texture_node = new uMod_TreeViewNode( node, texture);
  node->Append(texture_node);

  texture->Release();
  return 0;
}


int uMod_File::GetContentTemplate(const wxString &content, uMod_TreeViewNode* node)
{
  uMod_TreeViewNode_ArrayPtr list_node;

  if (uMod_TreeViewModel::String2Node( content, node, &list_node))
  {
    LastError << uMod_TreeViewModel::LastError << "\n" << FileName << "\n";
    uMod_TreeViewModel::LastError.Empty();
    return -1;
  }

  if (node->myElement->File().IsEmpty())
    return GetContentTemplate_SF( list_node, node);
  else
    return GetContentTemplate_ZIP( list_node, node);
}

int uMod_File::GetContentTemplate_ZIP( const uMod_TreeViewNode_ArrayPtr &list_node, uMod_TreeViewNode* node)
{
  SetFile(node->myElement->File());
  if (!FileSupported())
  {
    LastError << Language->Error_FileNotSupported <<"\n" << FileName;
    return -1;
  }
  if (int ret = ReadFile()) return ret;
  wxString file_type = FileName.AfterLast( '.');
  bool tpf = false;
  if (file_type == L"tpf")
  {
    tpf = true;
    UnXOR();
  }

  const char pw[] = {0x73, 0x2A, 0x63, 0x7D, 0x5F, 0x0A, 0xA6, 0xBD,
     0x7D, 0x65, 0x7E, 0x67, 0x61, 0x2A, 0x7F, 0x7F,
     0x74, 0x61, 0x67, 0x5B, 0x60, 0x70, 0x45, 0x74,
     0x5C, 0x22, 0x74, 0x5D, 0x6E, 0x6A, 0x73, 0x41,
     0x77, 0x6E, 0x46, 0x47, 0x77, 0x49, 0x0C, 0x4B,
     0x46, 0x6F, '\0'};

  ZIPENTRY ze;
  int index = -1;
  HZIP ZIP_Handle;
  if (tpf) ZIP_Handle= OpenZip( FileInMemory, FileLen, pw);
  else ZIP_Handle= OpenZip( FileInMemory, FileLen, (const char*)0);

  if (ZIP_Handle==(HZIP)0)
  {
    LastError << Language->Error_Unzip <<"\n" << FileName;
    return -1;
  }

  for (unsigned int i=0; i<list_node.GetCount(); i++)
  {
    if (list_node[i]->myElement->Type() == uMod_ModElement::Texture)
    {
      uMod_TextureElement *texture = (uMod_TextureElement*) list_node[i]->myElement;

      wxString file = texture->File();
      while ( (file[0]=='.' && (file[1]=='/' || file[1]=='\\')) || file[0]=='/' || file[0]=='\\') file.Remove(0,1);

      FindZipItem( ZIP_Handle, file.wc_str(), false, &index, &ze); // look for texture
      if (index>=0)
      {
        if (texture->Content().SetSize(ze.unc_size))
        {
          texture->Comment() = Language->Error_Memory;
          continue;
        }
        char *data = texture->Content().Data();

        ZRESULT rz = UnzipItem( ZIP_Handle, index, data, ze.unc_size);
        if (rz!=ZR_OK && rz!=ZR_MORE)
        {
          texture->Content().SetSize(0);
          texture->Comment() = Language->Error_Unzip;
          texture->Comment() <<" : " << FileName << " ==>> " << file;
        }
      }
      else
      {
        texture->Content().SetSize(0);
        texture->Comment() = Language->Error_Unzip;
        texture->Comment() << " : " << FileName << " -> " << file;
        CloseZip(ZIP_Handle); //somehow we need to close and to reopen the zip handle, otherwise the program crashes
        if (tpf) ZIP_Handle= OpenZip( FileInMemory, FileLen, pw);
        else ZIP_Handle= OpenZip( FileInMemory, FileLen, (const char*)0);
      }
    }
  }

  CloseZip(ZIP_Handle);
  if (node->myChildren.GetCount()==0)
  {
    LastError <<  Language->Error_FileIsEmpty << FileName << "\n";
    return -1;
  }
  return 0;
}


int uMod_File::GetContentTemplate_SF( const uMod_TreeViewNode_ArrayPtr &list_node, uMod_TreeViewNode* node)
{
  DWORD64 temp_hash;
  wxString name;

  for (unsigned int i=0; i<list_node.GetCount(); i++)
  {
    if (list_node[i]->myElement->Type() != uMod_ModElement::Texture) continue;

    uMod_TextureElement *texture = (uMod_TextureElement*) list_node[i]->myElement;

    SetFile( texture->File());
    if (int ret = ReadFile())
    {
      texture->Comment() = Language->Error_FileRead;
      texture->Comment() << " : " << texture->File();
      continue;
    }

    if (texture->Content().SetSize(FileLen))
    {
      texture->Comment() = Language->Error_Memory;
      texture->Comment() << " : " << texture->File();
      continue;
    }
    char *data = texture->Content().Data();

    for (unsigned int i=0; i<FileLen; i++) data[i] = FileInMemory[i];

    texture->Hash() = temp_hash;
    texture->Status() = uMod_TextureElement::Activate;
  }

  return 0;
}



int uMod_File::AddZip( uMod_TreeViewNode* node)
{
  if (int ret = ReadFile()) return ret;
  return AddContent( (char*)0, node);
}

int uMod_File::AddTpf( uMod_TreeViewNode* node)
{
  if (int ret = ReadFile()) return ret;

  UnXOR();

  const char pw[] = {0x73, 0x2A, 0x63, 0x7D, 0x5F, 0x0A, 0xA6, 0xBD,
     0x7D, 0x65, 0x7E, 0x67, 0x61, 0x2A, 0x7F, 0x7F,
     0x74, 0x61, 0x67, 0x5B, 0x60, 0x70, 0x45, 0x74,
     0x5C, 0x22, 0x74, 0x5D, 0x6E, 0x6A, 0x73, 0x41,
     0x77, 0x6E, 0x46, 0x47, 0x77, 0x49, 0x0C, 0x4B,
     0x46, 0x6F, '\0'};

  return AddContent( pw, node);
}


int uMod_File::AddContent( const char* pw, uMod_TreeViewNode* node)
{
  // Thats is really nasty code, but atm I am happy that it works. I should try an other unzip api,
  // This one seems to behave very strange.
  // I know, that also a bug in my code could be reason for the crashes, but UnzipItem( ... ) unzipes wrong files
  // and GetZipItem( ... ) returns garbage as file names, and the next call of GetZipItem( ... ) blows up the program.
  //
  // I have commented line 3519 in unzip.cpp.
  // It was stated this is a bug, but it did not solve my problems.
  //
  // closing and reopen the zip handle did the trick.
  //

  HZIP ZIP_Handle = OpenZip( FileInMemory, FileLen, pw);
  if (ZIP_Handle==(HZIP)0)  {LastError << Language->Error_Unzip; return -1;}

  node->myElement->File() = FileName;

  ZIPENTRY ze;
  int index;
  FindZipItem( ZIP_Handle, L"texmod.def", false, &index, &ze);
  if (index>=0) //if texmod.def is present in the zip file
  {
    char* def;
    int len = ze.unc_size;
    try {def=new char[len+1];}
    catch(...) {LastError << Language->Error_Memory; return -1;}
    ZRESULT zr = UnzipItem( ZIP_Handle, index, def, len);

    if (zr!=ZR_OK && zr!=ZR_MORE) {delete [] def; return -1;}
    def[len]=0;

    wxStringTokenizer token( def, "\n");

    int num = token.CountTokens();

    DWORD64 temp_hash;
    wxString entry;
    wxString file;

    for (int i=0; i<num; i++)
    {
      entry = token.GetNextToken();
      file = entry.BeforeFirst( '|');
      if (!file.ToULongLong( &temp_hash, 16)) {LastError << Language->Error_Hash <<"\nTPF:" << entry << "\n"; continue;}

      file = entry.AfterFirst( '|');
      file.Replace( "\r", "");

      while ( (file[0]=='.' && (file[1]=='/' || file[1]=='\\')) || file[0]=='/' || file[0]=='\\') file.Remove(0,1);

      FindZipItem( ZIP_Handle, file.wc_str(), false, &index, &ze); // look for texture
      if (index>=0)
      {
        uMod_TextureElement *texture = new uMod_TextureElement();

        if (texture->Content().SetSize(ze.unc_size))
        {
          LastError << Language->Error_Memory;
          texture->Release();
          continue;
        }
        char *data = texture->Content().Data();

        ZRESULT rz = UnzipItem( ZIP_Handle, index, data, ze.unc_size);
        if (rz!=ZR_OK && rz!=ZR_MORE)
        {
          texture->Release();
          LastError << Language->Error_Unzip <<"\nTPF:" << file << "\n";
        }
        else
        {
          texture->Hash() = temp_hash;
          texture->Title() = file;
          texture->File() = file;
          texture->Status() = uMod_TextureElement::Activate;

          uMod_TreeViewNode *node_texture = new uMod_TreeViewNode( node, texture);
          node->Append(node_texture);
          texture->Release();
        }
      }
      else
      {
        LastError << Language->Error_Unzip <<"\nTPF:" << file << "\n";
        CloseZip(ZIP_Handle); //somehow we need to close and to reopen the zip handle, otherwise the program crashes
        ZIP_Handle = OpenZip( FileInMemory, FileLen, pw);
      }
    }
    delete [] def;

    wxString temp = &FileInMemory[FileLen];
    node->myElement->Author() = temp.BeforeFirst('\n');
    node->myElement->Author().Replace("\r", "", true);
    node->myElement->Author().Replace("\n", "", true);

    node->myElement->Comment() = temp.AfterFirst('\n');
    node->myElement->Comment().Replace("\r\n", " ; ", true);
    node->myElement->Comment().Replace("\n", " ; ", true);

    CloseZip(ZIP_Handle);

    if (node->myChildren.GetCount()==0)
    {
      LastError <<  Language->Error_FileIsEmpty << FileName << "\n";
      return -1;
    }
    return 0;
  }





  CloseZip(ZIP_Handle); //somehow we need to close and to reopen the zip handle, otherwise the program crashes
  ZIP_Handle = OpenZip( FileInMemory, FileLen, pw);
  if (ZIP_Handle==(HZIP)0)  {LastError << Language->Error_Unzip; return -1;}
  FindZipItem( ZIP_Handle, L"content.txt", false, &index, &ze);




  if (index>=0) //if content.txt is present in the zip file
  {
    char* def;
    int len = ze.unc_size;
    try {def=new char[len+1];}
    catch(...) {LastError << Language->Error_Memory; return -1;}
    ZRESULT zr = UnzipItem( ZIP_Handle, index, def, len);

    if (zr!=ZR_OK && zr!=ZR_MORE) {delete [] def; return -1;}
    def[len]=0;

    wxString content = def;

    uMod_TreeViewNode_ArrayPtr list_node;

    if (uMod_TreeViewModel::String2Node( content, node, &list_node))
    {
      LastError << uMod_TreeViewModel::LastError << "\n" << FileName << "\n";
      uMod_TreeViewModel::LastError.Empty();
      return -1;
    }

    // overwrite file information
    node->myElement->File() = FileName;

    for (unsigned int i=0; i< list_node.GetCount(); i++)
    {
      if (list_node[i]->myElement->Type() == uMod_ModElement::Texture)
      {
        uMod_TextureElement *texture = (uMod_TextureElement*) list_node[i]->myElement;

        wxString file = texture->File();
        while ( (file[0]=='.' && (file[1]=='/' || file[1]=='\\')) || file[0]=='/' || file[0]=='\\') file.Remove(0,1);

        FindZipItem( ZIP_Handle, file.wc_str(), false, &index, &ze); // look for texture
        if (index>=0)
        {
          if (texture->Content().SetSize(ze.unc_size))
          {
            texture->Comment() = Language->Error_Memory;
            continue;
          }
          char *data = texture->Content().Data();

          ZRESULT rz = UnzipItem( ZIP_Handle, index, data, ze.unc_size);
          if (rz!=ZR_OK && rz!=ZR_MORE)
          {
            texture->Content().SetSize(0);
            texture->Comment() = Language->Error_Unzip;
          }
        }
        else
        {
          texture->Content().SetSize(0);
          texture->Comment() =  Language->Error_Unzip;
          CloseZip(ZIP_Handle); //somehow we need to close and to reopen the zip handle, otherwise the program crashes
          ZIP_Handle = OpenZip( FileInMemory, FileLen, pw);
        }
      }
    }


    CloseZip(ZIP_Handle);
    if (node->myChildren.GetCount()==0)
    {
      LastError <<  Language->Error_FileIsEmpty << FileName << "\n";
      return -1;
    }
    return 0;
  }


  //we load each dds file
  {
    CloseZip(ZIP_Handle); //somehow we need to close and to reopen the zip handle, otherwise the program crashes
    ZIP_Handle = OpenZip( FileInMemory, FileLen, pw);
    if (ZIP_Handle==(HZIP)0)  {LastError << Language->Error_Unzip; return -1;}
    wxString name;
    wxString file;
    GetZipItem( ZIP_Handle, -1, &ze); //ask for number of entries
    int num = ze.index;

    DWORD64 temp_hash;
    for (int i=0; i<num; i++)
    {
      if (GetZipItem( ZIP_Handle, i, &ze)!=ZR_OK) continue; //ask for name and size
      int len = ze.unc_size;

      uMod_TextureElement *texture = new uMod_TextureElement();

      if (texture->Content().SetSize(ze.unc_size))
      {
        LastError << Language->Error_Memory;
        texture->Release();
        continue;
      }
      char *data = texture->Content().Data();

      ZRESULT rz = UnzipItem( ZIP_Handle, i, data, len);
      if (rz!=ZR_OK && rz!=ZR_MORE)
      {
        texture->Release();
        LastError << Language->Error_Unzip <<"\nZIP:" << FileName << "\n";
        continue;
      }

      file = ze.name;
      if (file.Len()==0)
      {
        texture->Release();
        continue;
      }

      if (file == "Comment.txt") // load comment
      {
        char *buffer;
        try {buffer = new char [len+1];}
        catch (...)
        {
          LastError << Language->Error_Memory;
          texture->Release();
          continue;
        }
        for (int i=0; i<len; i++) buffer[i] = data[i];
        buffer[len]=0;

        node->myElement->Comment() = buffer;

        node->myElement->Comment().Replace("\r\n", " ; ", true);
        node->myElement->Comment().Replace("\n", " ; ", true);

        delete [] buffer;
        texture->Release();
        continue;
      }

      name = file.AfterLast( '.');
      if (name!="dds")
      {
        texture->Release();
        continue;
      }

      name = file.AfterLast( '_');
      name = name.BeforeLast( '.');

      if (!name.ToULongLong( &temp_hash, 16))
      {
        texture->Release();
        LastError << Language->Error_Hash <<"\nZIP:" << file << "\n";
        continue;
      } //if hash could not be extracted


      texture->Hash() = temp_hash;
      texture->Title() = ze.name;
      texture->File() = ze.name;
      texture->Status() = uMod_TextureElement::Activate;

      uMod_TreeViewNode *node_texture = new uMod_TreeViewNode( node, texture);
      node->Append(node_texture);
      texture->Release();
    }

    CloseZip(ZIP_Handle);

    if (node->myChildren.GetCount()==0)
    {
      LastError <<  Language->Error_FileIsEmpty << FileName << "\n";
      return -1;
    }
    return 0;
  }
  return 0;
}
