#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <ranges>

#include <libzippp.h>

struct TpfEntry {
    std::string Name;
    std::string Entry; // Assuming ZipEntry is a string in your original code
    uint32_t CrcHash;
};

class gMod_FileLoader {
    const std::string _fileName;
    std::ifstream _stream;
    const std::vector<uint8_t> _tpfPassword{
        0x73, 0x2A, 0x63, 0x7D, 0x5F, 0x0A, 0xA6, 0xBD,
        0x7D, 0x65, 0x7E, 0x67, 0x61, 0x2A, 0x7F, 0x7F,
        0x74, 0x61, 0x67, 0x5B, 0x60, 0x70, 0x45, 0x74,
        0x5C, 0x22, 0x74, 0x5D, 0x6E, 0x6A, 0x73, 0x41,
        0x77, 0x6E, 0x46, 0x47, 0x77, 0x49, 0x0C, 0x4B,
        0x46, 0x6F
    };
    std::vector<TpfEntry> entryCache;
    bool loaded;

public:
    gMod_FileLoader(const std::string& fileName)
        : _fileName(std::filesystem::absolute(fileName).string()), _stream(_fileName, std::ios::binary), loaded(false) {
        if (!_stream) {
            throw std::runtime_error("Failed to open file: " + _fileName);
        }
    }

    std::vector<TpfEntry> Load() {
        if (!loaded) {
            entryCache = GetContents();
            loaded = true;
        }

        return entryCache;
    }

private:
    std::vector<TpfEntry> GetContents() {
        if (_fileName.ends_with(".tpf")) {
            return GetTpfContents();
        } else {
            return GetFileContents();
        }
    }

    std::vector<TpfEntry> GetTpfContents() {
        std::vector<TpfEntry> entries;

        try {
            // Open the zip file using libzippp
            libzippp::ZipArchive zipArchive(_fileName);

            // Iterate over the files in the zip archive
            for (auto entry : zipArchive) {
                if (entry.isFile()) {
                    entries.push_back({ entry.getName(), entry.getName(), 0 }); // Assuming ZipEntry is a string
                }
            }
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to open zip file: " + _fileName);
        }

        return entries;
    }

    std::vector<TpfEntry> GetFileContents() {
        std::vector<TpfEntry> entries;

        try {
            // Open the zip file using libzippp
            libzippp::ZipArchive zipArchive(_fileName);

            // Iterate over the files in the zip archive
            for (auto entry : zipArchive) {
                if (entry.isFile()) {
                    entries.push_back({ entry.getName(), entry.getName(), 0 }); // Assuming ZipEntry is a string
                }
            }
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to open zip file: " + _fileName);
        }

        return entries;
    }
};
