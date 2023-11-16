#include <uMod_Main.h>
#include <filesystem>
#include "gMod_FileLoader.h"
#include "gMod_XorStream.h"

gMod_FileLoader::gMod_FileLoader(const std::string& fileName)
{
    file_name = std::filesystem::absolute(fileName).string();
    stream = std::ifstream(file_name, std::ios::binary);
    if (!stream) {
        throw std::runtime_error("Failed to open file: " + file_name);
    }
}

std::vector<TpfEntry> gMod_FileLoader::Load()
{
    if (!loaded) {
        entry_cache = GetContents();
        loaded = true;
    }

    return entry_cache;
}

std::vector<TpfEntry> gMod_FileLoader::GetContents()
{
    return file_name.ends_with(".tpf") ? GetTpfContents() : GetFileContents();
}

std::vector<TpfEntry> gMod_FileLoader::GetTpfContents()
{
    std::vector<TpfEntry> entries;
    try {
        auto istream = std::ifstream(file_name, std::ios::binary);
        auto xorstream = gMod_XorStream(istream);

        const std::string password(TPF_PASSWORD.begin(), TPF_PASSWORD.end());
        libzippp::ZipArchive zipArchive(file_name, password);
        zipArchive.setErrorHandlerCallback([](const std::string& message, const std::string& strerror, int zip_error_code, int system_error_code) -> void {
            Message("GetTpfContents: %s %s %d %d\n", message.c_str(), strerror.c_str(), zip_error_code, system_error_code);
        });
        zipArchive.open();
        LoadEntries(zipArchive, entries);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to open zip file: " + file_name + "\n" + e.what());
    }

    return entries;
}

std::vector<TpfEntry> gMod_FileLoader::GetFileContents()
{
    std::vector<TpfEntry> entries;

    try {
        libzippp::ZipArchive zipArchive(file_name);
        zipArchive.open();
        LoadEntries(zipArchive, entries);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to open zip file: " + file_name);
    }

    return entries;
}

void gMod_FileLoader::LoadEntries(libzippp::ZipArchive& archive, std::vector<TpfEntry>& entries)
{
    // Iterate over the files in the zip archive
    for (const auto& entry : archive.getEntries()) {
        if (entry.isFile()) {
            const auto dataPtr = entry.readAsBinary();
            const auto size = entry.getSize();
            entries.push_back({entry.getName(), entry.getName(), 0, dataPtr, size}); // Assuming ZipEntry is a string
        }
    }
}
