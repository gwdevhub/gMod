#pragma once

#include <fstream>
#include <vector>

class XorStreamReader {
public:
    XorStreamReader(const std::string& path)
    {
        file_stream = std::ifstream(path, std::ios::binary);
        if (!file_stream.seekg(0, std::ios::end).good() || !file_stream.seekg(0, std::ios::beg).good()) {
            throw std::invalid_argument("Provided stream needs to have SEEK set to True");
        }
    }

    ~XorStreamReader()
    {
        file_stream.close();
    }

    std::vector<char> ReadToEnd()
    {
        file_stream.seekg(0, std::ios::end);
        originalStreamLength = file_stream.tellg();
        file_stream.seekg(0);
        file_stream.clear(); // Clear any error state of the stream
        file_stream.seekg(0, std::ios::beg); // Go to the beginning of the stream

        std::vector<char> data(originalStreamLength);
        file_stream.read(data.data(), originalStreamLength);
        const auto read = file_stream.gcount();
        Message("Read %d bytes\n", read);
        for (auto i = 0; i < data.size(); i++) {
            data[i] = XOR(data[i], i);
        }

        return data;
    }

private:
    std::ifstream file_stream;
    long originalStreamLength;
    long length;

    char XOR(char b, long position) const
    {
        if (position > originalStreamLength - 4) {
            return b ^ TPF_XOREven;
        }

        return position % 2 == 0 ? b ^ TPF_XOREven : b ^ TPF_XOROdd;
    }

    static const char TPF_XOROdd = 0x3F;
    static const char TPF_XOREven = 0xA4;
};
