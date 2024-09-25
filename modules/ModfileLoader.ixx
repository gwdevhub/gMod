module;

#include "Main.h"
#include "Defines.h"

export module ModfileLoader;

import std;
import <libzippp.h>;
import ModfileLoader.TpfReader;
import TextureFunction;

namespace {
    uint32_t GetCrcFromFilename(const std::string& filename) {
        const static std::regex re(R"(0x[0-9a-f]{4,16})", std::regex::optimize | std::regex::icase);
        std::smatch match;
        if (!std::regex_search(filename, match, re)) {
            return 0;
        }

        uint64_t crc64_hash = 0;
        const auto number_str = match.str();
        try {
            crc64_hash = std::stoull(number_str, nullptr, 16);
        }
        catch (const std::invalid_argument&) {
            Warning("Failed to parse %s as a hash\n", filename.c_str());
            return 0;
        }
        catch (const std::out_of_range&) {
            Warning("Out of range while parsing %s as a hash\n", filename.c_str());
            return 0;
        }
        // TODO: @3vcloud go at this when you can, truncating the higher bits doesn't work
        // Truncate the higher 32-bits
        if (crc64_hash > 0xFFFFFFFF) {
            Warning("Truncating hash %s to 0x%08x\n", number_str.c_str(), static_cast<uint32_t>(crc64_hash & 0xFFFFFFFF));
            return 0;
        }
        else {
            Warning("Non truncated hash %s to 0x%8x\n", number_str.c_str(), crc64_hash);
        }
        return static_cast<uint32_t>(crc64_hash & 0xFFFFFFFF);
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
        0x46, 0x6F
    };

public:
    ModfileLoader(const std::filesystem::path& fileName);

    std::vector<TexEntry> GetContents();

private:

    std::vector<TexEntry> GetTpfContents();

    std::vector<TexEntry> GetFileContents();

    void LoadEntries(libzippp::ZipArchive& archive, std::vector<TexEntry>& entries);
};

ModfileLoader::ModfileLoader(const std::filesystem::path& fileName)
{
    file_name = std::filesystem::absolute(fileName);
}

std::vector<TexEntry> ModfileLoader::GetContents()
{
    try {
        return file_name.wstring().ends_with(L".tpf") ? GetTpfContents() : GetFileContents();
    }
    catch (const std::exception&) {
        Warning("Failed to open mod file: %s\n", file_name.c_str());
    }
    return {};
}

std::vector<TexEntry> ModfileLoader::GetTpfContents()
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

std::vector<TexEntry> ModfileLoader::GetFileContents()
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
        const auto crc_hash = GetCrcFromFilename(entry.getName());
        if (!crc_hash)
            continue;
        const auto data_ptr = static_cast<BYTE*>(entry.readAsBinary());
        const auto size = entry.getSize();
        std::vector vec(data_ptr, data_ptr + size);
        std::filesystem::path tex_name(entry.getName());
        entries.emplace_back(std::move(vec), crc_hash, tex_name.extension().string());
        delete[] data_ptr;
    }
}

void ParseTexmodArchive(std::vector<std::string>& lines, libzippp::ZipArchive& archive, std::vector<TexEntry>& entries)
{
    for (const auto& line : lines) {
        // 0xC57D73F7|GW.EXE_0xC57D73F7.tga\r\n
        // match[1] | match[2]
        const static auto address_file_regex = std::regex(R"(^[\\/.]*([^|]+)\|([^\r\n]+))", std::regex::optimize);
        std::smatch match;
        if (!std::regex_search(line, match, address_file_regex))
            continue;
        const auto address_string = match[1].str();
        const auto file_path = match[2].str();

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
        const auto def = def_file.readAsText();
        std::istringstream iss(def);
        std::vector<std::string> lines;
        std::string line;

        while (std::getline(iss, line)) {
            lines.push_back(line);
        }

        ParseTexmodArchive(lines, archive, entries);
    }
}
