#pragma once
#include <string>
#include <vector>
#include <Windows.h>

struct UModTexture {
    std::vector<char> data;
    std::string name;
    DWORD64 hash;
};
