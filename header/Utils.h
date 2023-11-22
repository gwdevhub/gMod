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
}
