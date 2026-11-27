module;

#include "Main.h"
#include "Defines.h"
#include <libzippp.h>

export module ModfileLoader;

import std;
import ModfileLoader.TpfReader;
import TextureFunction;

namespace {
    bool use_64_bit_crc = false;

    // Finds the first "0x" or "0X" followed by 4 to 16 hex digits (the match a regex of 0[xX][0-9a-fA-F]{4,16} would
    // give). Sixteen hex digits always fit a uint64_t, so the parse cannot fail or overflow.
    HashType GetCrcFromFilename(const std::string& filename)
    {
        auto is_hex = [](const char c) {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        };
        for (size_t i = 0; i + 1 < filename.size(); ++i) {
            if (filename[i] != '0' || (filename[i + 1] != 'x' && filename[i + 1] != 'X')) {
                continue;
            }
            size_t digits = 0;
            while (digits < 16 && i + 2 + digits < filename.size() && is_hex(filename[i + 2 + digits])) {
                ++digits;
            }
            if (digits < 4) {
                continue;
            }
            uint64_t crc64_hash = 0;
            for (size_t d = 0; d < digits; ++d) {
                const char c = filename[i + 2 + d];
                const uint64_t value = (c >= 'a') ? (c - 'a' + 10) : (c >= 'A') ? (c - 'A' + 10) : (c - '0');
                crc64_hash = crc64_hash * 16 + value;
            }
            use_64_bit_crc = use_64_bit_crc || crc64_hash > 0xFFFFFFFF;
            return crc64_hash;
        }
        return 0;
    }
}

namespace HashCheck {
    export bool Use64BitCrc()
    {
        return use_64_bit_crc;
    }
}

export class ModfileLoader {
    std::filesystem::path file_name;
    const std::string TPF_PASSWORD{
        0x73, 0x2A, 0x63, 0x7D, 0x5F, 0x0A, static_cast<char>(0xA6), static_cast<char>(0xBD),
        0x7D, 0x65, 0x7E, 0x67, 0x61, 0x2A, 0x7F, 0x7F,
        0x74, 0x61, 0x67, 0x5B, 0x60, 0x70, 0x45, 0x74,
        0x5C, 0x22, 0x74, 0x5D, 0x6E, 0x6A, 0x73, 0x41,
        0x77, 0x6E, 0x46, 0x47, 0x77, 0x49, 0x0C, 0x4B,
        0x46, 0x6F};

public:
    ModfileLoader(const std::filesystem::path& fileName);

    std::vector<TexEntry> GetContents() const;

private:
    std::vector<TexEntry> GetTpfContents() const;

    std::vector<TexEntry> GetFileContents() const;

    static void LoadEntries(libzippp::ZipArchive& archive, std::vector<TexEntry>& entries);
};

ModfileLoader::ModfileLoader(const std::filesystem::path& fileName)
{
    file_name = std::filesystem::absolute(fileName);
}

std::vector<TexEntry> ModfileLoader::GetContents() const
{
    try {
        return file_name.wstring().ends_with(L".tpf") ? GetTpfContents() : GetFileContents();
    }
    catch (const std::exception&) {
        Warning("Failed to open mod file: %s\n", file_name.c_str());
    }
    return {};
}

std::vector<TexEntry> ModfileLoader::GetTpfContents() const
{
    std::vector<TexEntry> entries;
    auto tpf_reader = TpfReader(file_name);
    const auto buffer = tpf_reader.ReadToEnd();
    const auto zip_archive = libzippp::ZipArchive::fromBuffer(buffer.data(), buffer.size(), false, TPF_PASSWORD);
    if (!zip_archive) {
        Warning("Failed to open tpf file: %s - %u bytes!", file_name.c_str(), buffer.size());
        return {};
    }
    zip_archive->setErrorHandlerCallback(
        [](const std::string& message, const std::string& strerror, int zip_error_code, int system_error_code) -> void {
            Message("GetTpfContents: %s %s %d %d\n", message.c_str(), strerror.c_str(), zip_error_code, system_error_code);
        });
    zip_archive->open();
    LoadEntries(*zip_archive, entries);
    zip_archive->close();
    libzippp::ZipArchive::free(zip_archive);

    return entries;
}

std::vector<TexEntry> ModfileLoader::GetFileContents() const
{
    std::vector<TexEntry> entries;

    libzippp::ZipArchive zip_archive(file_name.string());
    zip_archive.open();
    LoadEntries(zip_archive, entries);
    zip_archive.close();

    return entries;
}

void ParseSimpleArchive(const libzippp::ZipArchive& archive, std::vector<TexEntry>& entries)
{
    for (const auto& entry : archive.getEntries()) {
        if (!entry.isFile())
            continue;
        const auto entry_name = entry.getName();
        const auto crc_hash = GetCrcFromFilename(entry.getName());
        if (!crc_hash) {
            Warning("Entry with name %s could not be parsed", entry_name.c_str());
            continue;
        }
        const auto data_ptr = static_cast<BYTE*>(entry.readAsBinary());
        const auto size = entry.getSize();
        std::vector vec(data_ptr, data_ptr + size);
        std::filesystem::path tex_name(entry.getName());
        entries.emplace_back(std::move(vec), crc_hash, tex_name.extension().string());
        delete[] data_ptr;
    }
}

// Equivalent to matching ^[\\/.]*([^|]+)\|([^\r\n]+) against the line. Returns false when that would not match.
bool ParseAddressLine(const std::string& line, std::string& address, std::string& file_path)
    {
        const auto is_prefix = [](const char c) { return c == '\\' || c == '/' || c == '.'; };
        const auto pipe = line.find('|');
        if (pipe == std::string::npos || pipe == 0) {
            return false;
        }
        // Leading \ / . are skipped, but the address must keep at least one character before the pipe.
        size_t start = 0;
        while (start < pipe - 1 && is_prefix(line[start])) {
            ++start;
        }
        const auto file_end = line.find_first_of("\r\n", pipe + 1);
        const auto file_length = (file_end == std::string::npos ? line.size() : file_end) - (pipe + 1);
        if (file_length == 0) {
            return false;
        }
        address = line.substr(start, pipe - start);
        file_path = line.substr(pipe + 1, file_length);
        return true;
    }

void ParseTexmodArchive(std::vector<std::string>& lines, libzippp::ZipArchive& archive, std::vector<TexEntry>& entries)
{
    for (const auto& line : lines) {
        // 0xC57D73F7|GW.EXE_0xC57D73F7.tga\r\n
        // address | file_path
        std::string address_string;
        std::string file_path;
        if (!ParseAddressLine(line, address_string, file_path)) {
            Warning("Failed to parse texmod.def archive line: %s - %s", line.c_str(), line.c_str());
            continue;
        }

        const auto crc_hash = GetCrcFromFilename(address_string);
        if (!crc_hash)
            continue;

        const auto entry = archive.getEntry(file_path);
        if (entry.isNull() || !entry.isFile())
            continue;

        const auto data_ptr = static_cast<BYTE*>(entry.readAsBinary());
        const auto size = static_cast<size_t>(entry.getSize());
        std::vector vec(data_ptr, data_ptr + size);
        const auto tex_name = std::filesystem::path(entry.getName());
        entries.emplace_back(std::move(vec), crc_hash, tex_name.extension().string());
        delete[] data_ptr;
    }
}

void ModfileLoader::LoadEntries(libzippp::ZipArchive& archive, std::vector<TexEntry>& entries)
{
    const auto def_file = archive.getEntry("texmod.def");
    if (def_file.isNull() || !def_file.isFile()) {
        ParseSimpleArchive(archive, entries);
    }
    else {
        auto lines = utils::SplitLines(def_file.readAsText());
        ParseTexmodArchive(lines, archive, entries);
    }
}
