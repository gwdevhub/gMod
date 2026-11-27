module;

#include <cstdio>

export module ModfileLoader.TpfReader;

import std;

export class TpfReader {
public:
    // Reads the whole file up front through the C runtime (iostreams would pull in the locale machinery).
    // Check IsOpen() before ReadToEnd(): an unreadable file leaves the reader empty.
    TpfReader(const std::filesystem::path& path)
    {
        FILE* file = _wfopen(path.c_str(), L"rb");
        if (!file) {
            return;
        }
        char buffer[4096];
        size_t count = 0;
        while ((count = std::fread(buffer, 1, sizeof(buffer), file)) > 0) {
            contents.insert(contents.end(), buffer, buffer + count);
        }
        std::fclose(file);
        line_length = static_cast<long>(contents.size());
        opened = true;
    }

    [[nodiscard]] bool IsOpen() const
    {
        return opened;
    }

    std::vector<char> ReadToEnd()
    {
        std::vector<char> data = contents;
        for (auto i = 0u; i < data.size(); i++) {
            data[i] = XOR(data[i], i);
        }

        for (auto i = data.size() - 1; i > 0 && data[i] != 0; i--) {
            data[i] = 0;
        }

        // in the other zip libraries, these had to be cut off, with libzip we need to zero them out
        // cutting them off makes the archive invalid
        // data.resize(last_zero);

        return data;
    }

private:
    std::vector<char> contents;
    long line_length = 0;
    bool opened = false;

    [[nodiscard]] char XOR(const char b, const long position) const
    {
        if (position >= (line_length / 4) * 4) {
            return b ^ TPF_XOREven;
        }

        return position % 2 == 0 ? b ^ TPF_XOREven : b ^ TPF_XOROdd;
    }

    static constexpr char TPF_XOROdd = 0x3F;
    static constexpr char TPF_XOREven = static_cast<char>(0xA4);
};
