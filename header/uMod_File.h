#pragma once

#include "uMod_Main.h"
#include <string>
#include <vector>
#include "uMod_Texture.h"

class uMod_File {
public:
    uMod_File();
    uMod_File(std::string file);
    ~uMod_File();

    bool FileSupported();
    bool PackageFile();
    bool SingleFile();
    //int AddSingleFileToNode( uMod_TreeViewNode* node);
    int GetContentTemplate(const std::string& content);

    /*
    int GetComment( wxString &tool_tip);
    int GetContent( AddTextureClass &tex, bool add);
  */
    int GetContent();

    int SetFile(const std::string file)
    {
        FileName = file;
        Loaded = false;
        return 0;
    }

    std::string GetFile() { return FileName; }
    std::vector<UModTexture> Textures;

private:
    int ReadFile();

    int UnXOR();
    /*
    int GetCommentZip( wxString &tool_tip);
    int GetCommentTpf( wxString &tool_tip);
  */
    int AddFile();
    int AddZip();
    int AddTpf();
    int AddContent(const char* pw);

    std::string FileName;
    bool Loaded;
    bool XORed;
    char* FileInMemory;
    unsigned int MemoryLength;
    unsigned int FileLen;
    unsigned long long FileHash;
};
