#pragma once

#include <fstream>
#include <vector>

class XorStreamReader {
public:
    XorStreamReader(const std::string& path)
    {
        innerStream = std::ifstream(path, std::ios::binary);
        if (!innerStream.seekg(0, std::ios::end).good() || !innerStream.seekg(0, std::ios::beg).good()) {
            throw std::invalid_argument("Provided stream needs to have SEEK set to True");
        }

        innerStream.seekg(0, std::ios::end);
        const auto file_size = innerStream.tellg();
        innerStream.seekg(0, std::ios::beg);
        originalStreamLength = file_size;
        innerStream.seekg(originalStreamLength - 1);

        while (innerStream.tellg() > 0) {
            const auto readByte = ReadByte();
            if (readByte == 0) {
                break;
            }

            innerStream.seekg(-2, std::ios::cur);
        }

        const auto lastZeroPosition = innerStream.tellg() - std::fpos<int>(1);
        innerStream.seekg(0);
        length = lastZeroPosition;
    }

    ~XorStreamReader()
    {
        innerStream.close();
    }

    std::vector<char> ReadToEnd()
    {
        std::vector<char> data;
        data.reserve(length); // Reserve memory to avoid frequent reallocations

        innerStream.clear(); // Clear any error state of the stream
        innerStream.seekg(0, std::ios::beg); // Go to the beginning of the stream

        innerStream.read(data.data(), length);
        for (auto i = 0u; i < data.size(); i++) {
            data[i] = XOR(data[i], i);
        }

        return data;
    }

private:
    std::ifstream innerStream;
    long originalStreamLength;
    long length;

    char XOR(char b, long position) const
    {
        if (position > originalStreamLength - 4) {
            return b ^ TPF_XOREven;
        }

        return position % 2 == 0 ? b ^ TPF_XOREven : b ^ TPF_XOROdd;
    }

    char ReadByte()
    {
        char byte;
        const auto position = innerStream.tellg();
        if (innerStream.get(byte)) {
            return XOR(byte, position);
        }

        return 0;
    }

    static const char TPF_XOROdd = 0x3F;
    static const char TPF_XOREven = 0xA4;
};
