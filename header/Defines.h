#pragma once

//using HashType = DWORD32;
using HashType = DWORD64;

struct HashTuple {
    HashType crc32;
    HashType crc64;

    bool operator==(const HashTuple& other) const noexcept
    {
        return (other.crc32 == crc32) || (other.crc64 == crc64);
    }

    explicit operator bool() const noexcept
    {
        return crc32 != 0 || crc64 != 0;
    }

    explicit operator HashType() const noexcept
    {
        return crc32 ? crc32 : crc64;
    }
};

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
    const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    [[maybe_unused]] const auto success = SetConsoleTextAttribute(hConsole, 0); // white
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#endif
}

inline void Warning(const char* format, ...)
{
#ifdef _DEBUG
    const HANDLE hConsole = GetStdHandle(STD_ERROR_HANDLE);
    [[maybe_unused]] const auto success = SetConsoleTextAttribute(hConsole, 6); // yellow
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
#endif
}
