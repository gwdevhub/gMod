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
#pragma once

#include "uMod_Main.h"
#include <string>
#include <vector>
#include "uMod_Texture.h"

class uMod_File
{
public:
    uMod_File(void);
    uMod_File(const std::wstring file);
    ~uMod_File(void);

    bool FileSupported(void);
    bool PackageFile(void);
    bool SingleFile(void);
    //int AddSingleFileToNode( uMod_TreeViewNode* node);
    int GetContentTemplate(const std::wstring& content);

    /*
    int GetComment( wxString &tool_tip);
    int GetContent( AddTextureClass &tex, bool add);
  */
    int GetContent();

    int SetFile(const std::wstring file) { FileName = file; Loaded = false; return 0; }
    std::wstring GetFile(void) { return FileName; }
    std::vector<UModTexture> Textures;

private:
    int ReadFile(void);

    int UnXOR(void);
    /*
    int GetCommentZip( wxString &tool_tip);
    int GetCommentTpf( wxString &tool_tip);
  */
    int AddFile();
    int AddZip();
    int AddTpf();
    int AddContent(const char* pw);

    std::wstring FileName;
    bool Loaded;
    bool XORed;
    char* FileInMemory;
    unsigned int MemoryLength;
    unsigned int FileLen;
    unsigned long long FileHash;
};