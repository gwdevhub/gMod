#ifndef OTM_FILE_H_
#define OTM_FILE_H_


class AddTextureClass
{
public:
  AddTextureClass(void);
  void SetSize(int num);
  ~AddTextureClass(void);

  unsigned int Num;
  char **Textures;
  unsigned int *Size;
  unsigned long *Hash;
  bool *Add;
  bool *Force;

  unsigned int Len;
};
#include "OTM_Main.h"
#include "unzip.h"


class OTM_File
{
public:
  OTM_File(OTM_Language &lang);
  OTM_File(OTM_Language &lang, const wxString &file);
  ~OTM_File(void);

  int GetComment( wxString &tool_tip);
  int GetContent( AddTextureClass *tex, bool add, bool force);

  int SetFile(const wxString &file) {FileName=file;Loaded=false; return 0;}
  wxString GetFile(void) {return FileName;}


  wxString LastError;

private:
  int ReadFile(void);

  int UnXOR(void);
  int GetCommentZip( wxString &tool_tip);
  int GetCommentTpf( wxString &tool_tip);

  int AddFile( AddTextureClass *tex, bool add, bool force);
  int AddZip( AddTextureClass *tex, bool add, bool force, bool tpf);
  int AddContent( const char* pw, AddTextureClass *tex, bool add, bool force);

  OTM_Language &Language;
  wxString FileName;
  bool Loaded;
  bool XORed;
  char *FileInMemory;
  unsigned int MemoryLength;
  unsigned int FileLen;
};


#endif /* OTM_FILE_H_ */
