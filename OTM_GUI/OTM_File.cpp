

#include "OTM_File.h"


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
  for (int i=0; i<num; i++) Size[i] = 0u;
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
    for (unsigned int i=0; i<Num && i<Len; i++) if (Textures[i]!=NULL) delete [] Textures[i];
    delete [] Textures;
  }
}






OTM_File::OTM_File(OTM_Language &lang) : Language(lang)
{
  Loaded=false;
  XORed=false;
  FileInMemory=NULL;
  MemoryLength=0u;
  FileLen=0u;
}

OTM_File::OTM_File(OTM_Language &lang, const wxString &file) : Language(lang)
{
  Loaded=false;
  XORed=false;
  FileInMemory=NULL;
  MemoryLength=0u;
  FileLen=0u;
  SetFile(file);
}


OTM_File::~OTM_File(void)
{
  if (FileInMemory!=NULL) delete [] FileInMemory;
}


int OTM_File::GetComment( wxString &tool_tip)
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
    tool_tip = Language.NoComment;
    return -1;
  }
  return 0;
}

int OTM_File::GetContent( AddTextureClass *tex, bool add, bool force)
{
  wxString file_type = FileName.AfterLast( '.');
  if (file_type == L"zip")
  {
    AddZip( tex, add, force, false);
  }
  else if (file_type == L"tpf")
  {
    AddZip( tex, add, force, true);
  }
  else if (file_type == L"dds")
  {
    AddFile( tex, add, force);
  }
  else
  {
    LastError << Language.Error_FileNotSupported;
    LastError << "\n" << FileName;
  }
  if (LastError.Len()>0) return -1;
  else return 0;
}

int OTM_File::ReadFile(void)
{
  if (Loaded) return 0;
  XORed=false;

  wxFile dat;
  if (!dat.Access(FileName, wxFile::read)) {LastError << Language.Error_FileOpen <<"\n" << FileName; return -1;}
  dat.Open(FileName, wxFile::read);
  if (!dat.IsOpened()) {LastError << Language.Error_FileOpen <<"\n" << FileName; return -1;}
  FileLen = dat.Length();
  if (FileLen==0) {LastError << Language.Error_FileOpen <<"\n" << FileName; return -1;}

  if (FileLen>=MemoryLength)
  {
    if (FileInMemory!=NULL) delete [] FileInMemory;
    try {FileInMemory = new char [FileLen+1];}
    catch (...) {FileInMemory=NULL; MemoryLength=0; FileLen=0; LastError << Language.Error_Memory; return -1;}
    MemoryLength = FileLen+1;
  }
  unsigned int result = dat.Read( FileInMemory, FileLen);
  dat.Close();

  if (result != FileLen) {FileLen=0; LastError << Language.Error_FileRead<<"\n" << FileName; return -1;}
  FileInMemory[FileLen]=0;

  Loaded = true;
  return 0;
}



int OTM_File::UnXOR(void)
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



int OTM_File::GetCommentZip( wxString &tool_tip)
{
  if (int ret = ReadFile()) return ret;

  HZIP ZIP_Handle = OpenZip( FileInMemory, FileLen, NULL);
  if (ZIP_Handle==NULL)  {tool_tip = Language.NoComment; LastError << Language.Error_Unzip; return -1;}

  ZIPENTRY ze;
  int index;
  FindZipItem( ZIP_Handle, L"Comment.txt", false, &index, &ze);
  if (index>=0) //if Comment.txt is present in the zip file
  {
    char* comment;
    int len = ze.unc_size;
    try {comment=new char[len+1];}
    catch(...) {tool_tip = Language.NoComment; LastError << Language.Error_Memory; return -1;}
    ZRESULT zr = UnzipItem( ZIP_Handle, index, comment, len);

    if (zr!=ZR_OK && zr!=ZR_MORE) {delete [] comment; tool_tip = Language.NoComment;  LastError << Language.Error_Unzip <<"\nZIP:" << L"Comment.txt"; return -1;}
    comment[len]=0;
    tool_tip = comment;
    delete [] comment;
  }
  else tool_tip = Language.NoComment;

  CloseZip(ZIP_Handle);
  return 0;
}

int OTM_File::GetCommentTpf( wxString &tool_tip)
{
  if (int ret = ReadFile()) return ret;

  UnXOR();
  tool_tip = &FileInMemory[FileLen];
  tool_tip.Prepend( Language.Author);
  return 0;
}









int OTM_File::AddFile( AddTextureClass *tex, bool add, bool force)
{
  tex->SetSize(1);
  unsigned long temp_hash;

  wxString name = FileName.AfterLast( '_');
  name = name.BeforeLast( '.');
  if (!name.ToULong( &temp_hash, 16)) {LastError << Language.Error_Hash <<"\n" << FileName << "\n"; return -1;} // return if hash could not be extracted

  if (add)
  {
    if (int ret = ReadFile()) return ret;

    try {tex->Textures[0] = new char[FileLen];}
    catch (...) {tex->Textures[0]=NULL; LastError << Language.Error_Memory; return -1;}
    for (unsigned int i=0; i<FileLen; i++) tex->Textures[0][i] = FileInMemory[i];
    tex->Size[0] = FileLen;
  }
  else {tex->Size[0] = 0; tex->Textures[0] = NULL;}

  tex->Num = 1;
  tex->Add[0] = add;
  tex->Force[0] = force;
  tex->Hash[0] = temp_hash;
  return 0;
}

int OTM_File::AddZip( AddTextureClass *tex, bool add, bool force, bool tpf)
{
  if (int ret = ReadFile()) return ret;

  if (tpf)
  {
    UnXOR();

    const char pw[] = {0x73, 0x2A, 0x63, 0x7D, 0x5F, 0x0A, 0xA6, 0xBD,
       0x7D, 0x65, 0x7E, 0x67, 0x61, 0x2A, 0x7F, 0x7F,
       0x74, 0x61, 0x67, 0x5B, 0x60, 0x70, 0x45, 0x74,
       0x5C, 0x22, 0x74, 0x5D, 0x6E, 0x6A, 0x73, 0x41,
       0x77, 0x6E, 0x46, 0x47, 0x77, 0x49, 0x0C, 0x4B,
       0x46, 0x6F, '\0'};

    return AddContent( pw, tex, add,  force);
  }
  else
  {
    return AddContent( NULL, tex, add,  force);
  }
}


int OTM_File::AddContent( const char* pw, AddTextureClass *tex, bool add, bool force)
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
  if (ZIP_Handle==NULL)  {LastError << Language.Error_Unzip; return -1;}

  ZIPENTRY ze;
  int index;
  FindZipItem( ZIP_Handle, L"texmod.def", false, &index, &ze);
  if (index>=0) //if texmod.def is present in the zip file
  {
    char* def;
    int len = ze.unc_size;
    try {def=new char[len+1];}
    catch(...) {LastError << Language.Error_Memory; return -1;}
    ZRESULT zr = UnzipItem( ZIP_Handle, index, def, len);

    if (zr!=ZR_OK && zr!=ZR_MORE) {delete [] def; return -1;}
    def[len]=0;

    wxStringTokenizer token( def, "\n");

    int num = token.CountTokens();


    tex->SetSize(num);

    unsigned long temp_hash;
    int count = 0;
    wxString entry;
    wxString file;

    for (int i=0; i<num; i++)
    {
      entry = token.GetNextToken();
      file = entry.BeforeFirst( '|');
      if (!file.ToULong( &temp_hash, 16)) {LastError << Language.Error_Hash <<"\nTPF:" << entry << "\n"; continue;}

      file = entry.AfterFirst( '|');
      file.Replace( "\r", "");

      while ( (file[0]=='.' && (file[1]=='/' || file[1]=='\\')) || file[0]=='/' || file[0]=='\\') file.Remove(0,1);

      if (add)
      {
        FindZipItem( ZIP_Handle, file.wc_str(), false, &index, &ze); // look for texture
        if (index>=0)
        {
          try {tex->Textures[count] = new char[ze.unc_size];}
          catch(...)
          {
            tex->Textures[count] = NULL;
            LastError << Language.Error_Memory;
            continue;
          }

          ZRESULT rz = UnzipItem( ZIP_Handle, index, tex->Textures[count], ze.unc_size);
          if (rz!=ZR_OK && rz!=ZR_MORE)
          {
            delete [] tex->Textures[count];
            LastError << Language.Error_Unzip <<"\nTPF:" << file << "\n";
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
          LastError << Language.Error_Unzip <<"\nTPF:" << file << "\n";
          CloseZip(ZIP_Handle); //somehow we need to close and to reopen the zip handle, otherwise the program crashes
          ZIP_Handle = OpenZip( FileInMemory, FileLen, pw);
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
    delete [] def;
    tex->Num = count;
  }
  else // texmod.def is not present in the zip file
  {
    CloseZip(ZIP_Handle); //somehow we need to close and to reopen the zip handle, otherwise the program crashes
    ZIP_Handle = OpenZip( FileInMemory, FileLen, pw);
    if (ZIP_Handle==NULL)  {LastError << Language.Error_Unzip; return -1;}
    wxString name;
    wxString file;
    GetZipItem( ZIP_Handle, -1, &ze); //ask for number of entries
    int num = ze.index;

    tex->SetSize(num);
    int count = 0;
    unsigned long temp_hash;
    for (int i=0; i<num; i++)
    {
      if (GetZipItem( ZIP_Handle, i, &ze)!=ZR_OK) continue; //ask for name and size
      int len = ze.unc_size;

      char *buffer;
      try {buffer = new char[len+1];}
      catch(...)
      {
        continue;
      }


      ZRESULT rz = UnzipItem( ZIP_Handle, i, buffer, len);
      if (rz!=ZR_OK && rz!=ZR_MORE)
      {
        delete [] buffer;
        buffer = NULL;
        LastError << Language.Error_Unzip <<"\nZIP:" << file << "\n";
      }

      file = ze.name;
      if (file.Len()==0) continue;

      name = file.AfterLast( '.');
      if (name!="dds") continue; //if this is not texture file, continue

      name = file.AfterLast( '_');
      name = name.BeforeLast( '.');

      if (!name.ToULong( &temp_hash, 16)) {LastError << Language.Error_Hash <<"\nZIP:" << file << "\n"; continue;} //if hash could not be extracted

      if (add)
      {
        if (buffer==NULL) continue;

        tex->Textures[count] = buffer;
        tex->Hash[count] = temp_hash;
        tex->Add[count] = true;
        tex->Size[count] = len;//ze.unc_size;
        tex->Force[count] = force;
        count++;
      }
      else
      {
        if (buffer!=NULL) delete [] buffer;
        tex->Hash[count] = temp_hash;
        tex->Add[count] = false;
        tex->Size[count] = 0;
        tex->Force[count] = force;
        count++;
      }
    }
    tex->Num = count;
  }

  CloseZip(ZIP_Handle);
  return 0;
}
