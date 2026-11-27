#pragma once

#include <string>
#include <vector>

namespace utils {
    // The whole file as bytes, or empty if it cannot be opened. Uses the C runtime, not iostreams, to keep the DLL small.
    inline std::string ReadWholeFile(const std::filesystem::path& path)
    {
        std::string data;
        FILE* file = _wfopen(path.c_str(), L"rb");
        if (!file) return data;
        char buffer[4096];
        size_t count = 0;
        while ((count = std::fread(buffer, 1, sizeof(buffer), file)) > 0) {
            data.append(buffer, count);
        }
        std::fclose(file);
        return data;
    }

    // The lines of text as repeated std::getline calls would return them, without the iostream machinery.
    inline std::vector<std::string> SplitLines(const std::string& text)
    {
        std::vector<std::string> lines;
        size_t start = 0;
        while (start < text.size()) {
            const auto end = text.find('\n', start);
            if (end == std::string::npos) {
                lines.push_back(text.substr(start));
                break;
            }
            lines.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        return lines;
    }

    template <typename T>
    void erase_first(std::vector<T>& vec, const T& elem)
    {
        const auto found = std::ranges::find(vec, elem);
        if (found != std::ranges::end(vec)) {
            vec.erase(found);
        }
    }

    inline std::wstring utf8_to_wstring(const std::string& utf8str)
    {
        if (utf8str.empty()) return {};

        // Calculate the number of wide characters needed for the conversion
        const int count = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, nullptr, 0);
        if (count == 0) throw std::runtime_error("Failed to convert UTF-8 to UTF-16");

        std::wstring wstr(count, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, wstr.data(), count);

        return wstr;
    }
}
