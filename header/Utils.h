#pragma once

#include <vector>

namespace utils
{
    template<typename T>
    void erase_first(std::vector<T>& vec, const T& elem)
    {
        const auto found = std::ranges::find(vec, elem);
        if (found != std::ranges::end(vec)) {
            vec.erase(found);
        }
    }

    inline std::wstring utf8_to_wstring(const std::string& utf8str) {
        if (utf8str.empty()) return {};

        // Calculate the number of wide characters needed for the conversion
        const int count = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, nullptr, 0);
        if (count == 0) throw std::runtime_error("Failed to convert UTF-8 to UTF-16");

        std::wstring wstr(count, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, wstr.data(), count);

        return wstr;
    }
}
