#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <ranges>
#include "zip.h"

struct TpfEntry {
    std::string Name;
    std::string Entry; // Assuming ZipEntry is a string in your original code
    uint32_t CrcHash;
};

class ZipLoader {
private:
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
    ZipLoader(const std::string& fileName)
        : _fileName(fs::absolute(fileName).string()), _stream(_fileName, std::ios::binary), loaded(false) {
        if (!_stream) {
            throw std::runtime_error("Failed to open file: " + _fileName);
        }
    }

    std::vector<TpfEntry> Load() {
        std::lock_guard lock(lockObject);

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

        zip_t* archive = zip_open(_fileName.c_str(), ZIP_RDONLY, nullptr);
        if (archive == nullptr) {
            throw std::runtime_error("Failed to open zip file: " + _fileName);
        }

        zip_source* source = zip_source_function(archive, nullptr, nullptr);
        zip_password_set(source, reinterpret_cast<const char*>(_tpfPassword.data()));
        zip_set_source(archive, source, 0);

        zip_int64_t numEntries = zip_get_num_entries(archive, 0);
        for (zip_int64_t i = 0; i < numEntries; ++i) {
            struct zip_stat stats;
            zip_stat_init(&stats);
            zip_stat_index(archive, i, 0, &stats);
            entries.push_back({ stats.name, stats.name, 0 }); // Assuming ZipEntry is a string
        }

        zip_close(archive);
        return entries;
    }

    std::vector<TpfEntry> GetFileContents() {
        std::vector<TpfEntry> entries;

        zip_t* archive = zip_open(_fileName.c_str(), ZIP_RDONLY, nullptr);
        if (archive == nullptr) {
            throw std::runtime_error("Failed to open zip file: " + _fileName);
        }

        zip_int64_t numEntries = zip_get_num_entries(archive, 0);
        for (zip_int64_t i = 0; i < numEntries; ++i) {
            struct zip_stat stats;
            zip_stat_init(&stats);
            zip_stat_index(archive, i, 0, &stats);
            entries.push_back({ stats.name, stats.name, 0 }); // Assuming ZipEntry is a string
        }

        zip_close(archive);
        return entries;
    }
};
