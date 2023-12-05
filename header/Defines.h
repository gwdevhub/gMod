#pragma once

//#define HashType DWORD64
using HashType = DWORD32;

struct TexEntry {
    std::vector<BYTE> data{};
    HashType crc_hash = 0; // hash value
    std::string ext{};
};

struct TextureFileStruct {
    std::vector<BYTE> data{};
    HashType crc_hash = 0; // hash value
};

inline void Message(const char* format, ...)
{
#ifdef _DEBUG
#if 0
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#endif
#endif
}

inline void Info(const char* format, ...)
{
#ifdef _DEBUG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#endif
}

inline void Warning(const char* format, ...)
{
#ifdef _DEBUG
    static HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
    [[maybe_unused]] static auto success = SetConsoleTextAttribute(hConsole, 6);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
#endif
}

