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

#ifndef OTM_FILE_H_
#define OTM_FILE_H_

#include "OTM_Main.h"

class OTM_File
{
public:
  OTM_File(void);
  OTM_File(const wxString &file);
  ~OTM_File(void);

  bool FileSupported(void);

  int GetComment( wxString &tool_tip);
  int GetContent( AddTextureClass &tex, bool add);

  int SetFile(const wxString &file) {FileName=file;Loaded=false; return 0;}
  wxString GetFile(void) {return FileName;}


  wxString LastError;

private:
  int ReadFile(void);

  int UnXOR(void);
  int GetCommentZip( wxString &tool_tip);
  int GetCommentTpf( wxString &tool_tip);

  int AddFile( AddTextureClass &tex, bool add);
  int AddZip( AddTextureClass &tex, bool add, bool tpf);
  int AddContent( const char* pw, AddTextureClass &tex, bool add);

  wxString FileName;
  bool Loaded;
  bool XORed;
  char *FileInMemory;
  unsigned int MemoryLength;
  unsigned int FileLen;
};


#endif /* OTM_FILE_H_ */
