#include <..\header\Main.h>
#include <filesystem>
#include "FileLoader.h"
#include "TpfReader.h"

FileLoader::FileLoader(const std::string& fileName)
{
    file_name = std::filesystem::absolute(fileName).string();
}

std::vector<TpfEntry> FileLoader::GetContents()
{
    try {
        return file_name.ends_with(".tpf") ? GetTpfContents() : GetFileContents();
    }
    catch (const std::exception&) {
        Message("Failed to open mod file: %s\n", file_name.c_str());
    }
    return {};
}

std::vector<TpfEntry> FileLoader::GetTpfContents()
{
    std::vector<TpfEntry> entries;
    auto tpf_reader = TpfReader(file_name);
    const auto buffer = tpf_reader.ReadToEnd();
    const auto zip_archive = libzippp::ZipArchive::fromBuffer(buffer.data(), buffer.size(), false, TPF_PASSWORD);
    if (!zip_archive) {
        Message("Failed to open tpf file: %s - %u bytes!", file_name.c_str(), buffer.size());
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

std::vector<TpfEntry> FileLoader::GetFileContents()
{
    std::vector<TpfEntry> entries;

    libzippp::ZipArchive zip_archive(file_name);
    zip_archive.open();
    LoadEntries(zip_archive, entries);
    zip_archive.close();

    return entries;
}

void ParseSimpleArchive(const libzippp::ZipArchive& archive, std::vector<TpfEntry>& entries)
{
    for (const auto& entry : archive.getEntries()) {
        if (entry.isFile()) {
            //TODO: #6 - Implement regex search
            auto name = entry.getName();

            // Remove the part before the last underscore (if any)
            size_t firstIndex = name.find_last_of('_');
            while (firstIndex != std::string::npos) {
                if (firstIndex >= entry.getName().length() - 1) {
                    name = entry.getName();
                }
                else {
                    name = entry.getName().substr(firstIndex + 1);
                }

                firstIndex = name.find_last_of('_');
            }

            // Remove the file extension (if any)
            size_t lastIndex = name.find_last_of('.');
            if (lastIndex != std::string::npos) {
                name = name.substr(0, lastIndex);
            }

            uint32_t crc_hash;
            try {
                crc_hash = std::stoul(name, nullptr, 16);
            }
            catch (const std::invalid_argument& e) {
                Message("Failed to parse %s as a hash", name.c_str());
                continue;
            }
            catch (const std::out_of_range& e) {
                Message("Out of range while parsing %s as a hash", name.c_str());
                continue;
            }

            const auto data_ptr = static_cast<char*>(entry.readAsBinary());
            const auto size = entry.getSize();
            std::vector<char> vec;
            vec.assign(data_ptr, data_ptr + size);
            entries.emplace_back(name, entry.getName(), crc_hash, std::move(vec));
        }
    }
}

void ParseTexmodArchive(std::vector<std::string>& lines, libzippp::ZipArchive& archive, std::vector<TpfEntry>& entries)
{
    for (const auto& line : lines) {
        std::istringstream iss(line);
        std::string part;
        std::vector<std::string> splits;

        // Split the line by '|'
        while (std::getline(iss, part, '|')) {
            splits.push_back(part);
        }

        if (splits.size() != 2) {
            continue;
        }

        std::string addrstr = splits[0];
        std::string path = splits[1];

        // Remove unwanted characters from the beginning of the path
        while (!path.empty() && (path[0] == '.' && (path[1] == '/' || path[1] == '\\')) || path[0] == '/' || path[0] == '\\') {
            path.erase(0, 1);
        }

        // Remove trailing newline and carriage return characters
        if (const auto end_pos = path.find_last_not_of("\r\n"); end_pos != std::string::npos) {
            path.erase(end_pos + 1);
        }
        else if (!path.empty()) {
            path.clear();
        }

        const auto entry = archive.getEntry(path);
        if (entry.isNull()) {
            continue;
        }

        if (!entry.isFile()) {
            continue;
        }

        uint32_t crc_hash{};
        try {
            crc_hash = std::stoul(addrstr, nullptr, 16);
        }
        catch (const std::invalid_argument& e) {
            Message("Failed to parse %s as a hash", addrstr.c_str());
            continue;
        }
        catch (const std::out_of_range& e) {
            Message("Out of range while parsing %s as a hash", addrstr.c_str());
            continue;
        }

        const auto data_ptr = static_cast<char*>(entry.readAsBinary());
        const auto size = static_cast<size_t>(entry.getSize());
        std::vector vec(data_ptr, data_ptr + size);
        entries.emplace_back(addrstr, entry.getName(), crc_hash, std::move(vec));
        delete[] data_ptr;
    }
}

void FileLoader::LoadEntries(libzippp::ZipArchive& archive, std::vector<TpfEntry>& entries)
{
    // Iterate over the files in the zip archive
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