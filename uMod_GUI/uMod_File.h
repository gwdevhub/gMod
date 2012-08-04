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

#ifndef uMod_FILE_H_
#define uMod_FILE_H_

#include "uMod_Main.h"

class uMod_File
{
public:
  uMod_File(void);
  uMod_File(const wxString &file);
  ~uMod_File(void);

  bool FileSupported(void);
  bool PackageFile(void);
  bool SingleFile(void);
  //int AddSingleFileToNode( uMod_TreeViewNode* node);
  int GetContentTemplate(const wxString &content, uMod_TreeViewNode* node);

  /*
  int GetComment( wxString &tool_tip);
  int GetContent( AddTextureClass &tex, bool add);
*/
  int GetContent( uMod_TreeViewNode* node);

  int SetFile(const wxString &file) {FileName=file;Loaded=false; return 0;}
  wxString GetFile(void) {return FileName;}


  wxString LastError;

private:
  int ReadFile(void);

  int UnXOR(void);
  /*
  int GetCommentZip( wxString &tool_tip);
  int GetCommentTpf( wxString &tool_tip);
*/
  int AddFile( uMod_TreeViewNode* node);
  int AddZip( uMod_TreeViewNode* node);
  int AddTpf( uMod_TreeViewNode* node);

  int AddContent( const char* pw, uMod_TreeViewNode* node);

  int GetContentTemplate_ZIP( const uMod_TreeViewNode_ArrayPtr &list_node, uMod_TreeViewNode* node);
  int GetContentTemplate_SF( const uMod_TreeViewNode_ArrayPtr &list_node, uMod_TreeViewNode* node);

  wxString FileName;
  bool Loaded;
  bool XORed;
  char *FileInMemory;
  unsigned int MemoryLength;
  unsigned int FileLen;
};


#endif /* uMod_FILE_H_ */
