#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <ranges>
#include <streambuf>
#include <libzippp.h>

struct TpfEntry {
    std::string Name;
    std::string Entry; // Assuming ZipEntry is a string in your original code
    uint32_t CrcHash;
    void* Data;
    unsigned long long Size;
};

class gMod_FileLoader {
    std::string _fileName;
    std::ifstream _stream;
    const std::vector<uint8_t> _tpfPassword{
        0x73, 0x2A, 0x63, 0x7D, 0x5F, 0x0A, 0xA6, 0xBD,
        0x7D, 0x65, 0x7E, 0x67, 0x61, 0x2A, 0x7F, 0x7F,
        0x74, 0x61, 0x67, 0x5B, 0x60, 0x70, 0x45, 0x74,
        0x5C, 0x22, 0x74, 0x5D, 0x6E, 0x6A, 0x73, 0x41,
        0x77, 0x6E, 0x46, 0x47, 0x77, 0x49, 0x0C, 0x4B,
        0x46, 0x6F
    };
    const std::string encodedPassword = std::string(_tpfPassword.begin(), _tpfPassword.end());
    std::vector<TpfEntry> entryCache;
    bool loaded;

public:
    gMod_FileLoader(const std::string& fileName);

    std::vector<TpfEntry> Load();

private:
    std::vector<TpfEntry> GetContents();

    std::vector<TpfEntry> GetTpfContents();

    std::vector<TpfEntry> GetFileContents();

    void LoadEntries(libzippp::ZipArchive& archive, std::vector<TpfEntry>& entries);
};