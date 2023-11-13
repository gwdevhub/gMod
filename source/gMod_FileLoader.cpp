#include "../header/gMod_FileLoader.h"
#include <filesystem>

gMod_FileLoader::gMod_FileLoader(const std::string& fileName) {
    _fileName = std::filesystem::absolute(fileName).string();
    _stream = std::ifstream(_fileName, std::ios::binary);
    loaded = false;
    if (!_stream) {
        throw std::runtime_error("Failed to open file: " + _fileName);
    }
}

std::vector<TpfEntry> gMod_FileLoader::Load() {
    if (!loaded) {
        entryCache = GetContents();
        loaded = true;
    }

    return entryCache;
}

std::vector<TpfEntry> gMod_FileLoader::GetContents() {
    if (_fileName.ends_with(".tpf")) {
        return GetTpfContents();
    }
    else {
        return GetFileContents();
    }
}

std::vector<TpfEntry> gMod_FileLoader::GetTpfContents() {
    std::vector<TpfEntry> entries;
    try {
        // Open the zip file using libzippp
        libzippp::ZipArchive zipArchive(_fileName, encodedPassword);
        zipArchive.open(); //ReadOnly and no consistency checks
        LoadEntries(zipArchive, entries);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to open zip file: " + _fileName);
    }

    return entries;
}

std::vector<TpfEntry> gMod_FileLoader::GetFileContents() {
    std::vector<TpfEntry> entries;

    try {
        // Open the zip file using libzippp
        libzippp::ZipArchive zipArchive(_fileName);
        zipArchive.open();
        LoadEntries(zipArchive, entries);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to open zip file: " + _fileName);
    }

    return entries;
}

void gMod_FileLoader::LoadEntries(libzippp::ZipArchive& archive, std::vector<TpfEntry>& entries) {
    // Iterate over the files in the zip archive
    for (const auto& entry : archive.getEntries()) {
        if (entry.isFile()) {
            const auto dataPtr = entry.readAsBinary();
            const auto size = entry.getSize();
            entries.push_back({ entry.getName(), entry.getName(), 0, dataPtr, size }); // Assuming ZipEntry is a string
        }
    }
}
