#include <Main.h>
#include <regex>
#include <algorithm>
#include <filesystem>
#include "FileLoader.h"
#include "TpfReader.h"

FileLoader::FileLoader(const std::string& fileName)
{
    file_name = std::filesystem::absolute(fileName).string();
}

std::vector<TexEntry> FileLoader::GetContents()
{
    try {
        return file_name.ends_with(".tpf") ? GetTpfContents() : GetFileContents();
    }
    catch (const std::exception&) {
        Warning("Failed to open mod file: %s\n", file_name.c_str());
    }
    return {};
}

std::vector<TexEntry> FileLoader::GetTpfContents()
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

std::vector<TexEntry> FileLoader::GetFileContents()
{
    std::vector<TexEntry> entries;

    libzippp::ZipArchive zip_archive(file_name);
    zip_archive.open();
    LoadEntries(zip_archive, entries);
    zip_archive.close();

    return entries;
}

void ParseSimpleArchive(const libzippp::ZipArchive& archive, std::vector<TexEntry>& entries)
{
    for (const auto& entry : archive.getEntries()) {
        if (entry.isFile()) {
            //TODO: #6 - Implement regex search
            auto name = entry.getName();


            const static std::regex re(R"(0x[0-9a-f]{8})", std::regex::optimize | std::regex::icase);
            std::smatch match;
            if (!std::regex_match(name, match, re)) {
                continue;
            }

            uint32_t crc_hash;
            try {
                crc_hash = std::stoul(match.str(), nullptr, 16);
            }
            catch (const std::invalid_argument& e) {
                Warning("Failed to parse %s as a hash", name.c_str());
                continue;
            }
            catch (const std::out_of_range& e) {
                Message("Out of range while parsing %s as a hash", name.c_str());
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
}

void ParseTexmodArchive(std::vector<std::string>& lines, libzippp::ZipArchive& archive, std::vector<TexEntry>& entries)
{
    for (const auto& line : lines) {
        // 0xC57D73F7|GW.EXE_0xC57D73F7.tga\r\n
        // match[1] | match[2]
        const static auto address_file_regex = std::regex(R"(^[\\/.]*([^|]+)\|([^\r\n]+))", std::regex::optimize);
        std::smatch match;
        std::regex_search(line, match, address_file_regex);
        if (match.size() != 3u)
            continue;
        const auto address_string = match[1].str();
        const auto file_path = match[2].str();

        const auto entry = archive.getEntry(file_path);
        if (entry.isNull() || !entry.isFile()) {
            continue;
        }

        uint32_t crc_hash{};
        try {
            crc_hash = std::stoul(address_string, nullptr, 16);
        }
        catch (const std::invalid_argument& e) {
            Warning("Failed to parse %s as a hash", address_string.c_str());
            continue;
        }
        catch (const std::out_of_range& e) {
            Warning("Out of range while parsing %s as a hash", address_string.c_str());
            continue;
        }

        const auto data_ptr = static_cast<BYTE*>(entry.readAsBinary());
        const auto size = static_cast<size_t>(entry.getSize());
        std::vector vec(data_ptr, data_ptr + size);
        const auto tex_name = std::filesystem::path(entry.getName());
        entries.emplace_back(std::move(vec), crc_hash, tex_name.extension().string());
        delete[] data_ptr;
    }
}

void FileLoader::LoadEntries(libzippp::ZipArchive& archive, std::vector<TexEntry>& entries)
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
