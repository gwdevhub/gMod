#pragma once

#include <iostream>
#include <fstream>
#include <vector>

class gMod_XorStream : public std::streambuf {
public:
    gMod_XorStream(std::ifstream& stream) : innerStream(stream) {
        if (!innerStream.seekg(0, std::ios::end).good() || !innerStream.seekg(0, std::ios::beg).good()) {
            throw std::invalid_argument("Provided stream needs to have SEEK set to True");
        }

        innerStream.seekg(0, std::ios::end);
        std::streampos file_size = innerStream.tellg();
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
        setg(nullptr, nullptr, nullptr);
    }

    int_type underflow() override {
        char_type buffer[2];
        const auto bytesRead = innerStream.readsome(buffer, 2);

        const auto startPosition = innerStream.tellg() - bytesRead;
        for (int i = 0; i < bytesRead; i++) {
            buffer[i] = XOR(buffer[i], i + startPosition);
        }

        auto bytesOverLength = startPosition + bytesRead - std::fpos<int>(length);
        for (int i = 0; i < bytesOverLength; i++) {
            if (bytesRead - i - 1 >= 0) {
                buffer[bytesRead - i - 1] = 0;
            }
        }

        if (bytesRead > 0) {
            setg(buffer, buffer, buffer + bytesRead);
            return traits_type::to_int_type(buffer[0]);
        }

        return traits_type::eof();
    }

    std::vector<char> ReadToEnd() {
        const size_t bufferSize = 1024;
        char buffer[bufferSize];
        std::vector<char> streamData;
        streamData.reserve(length); // Reserve memory to avoid frequent reallocations

        innerStream.clear(); // Clear any error state of the stream
        innerStream.seekg(0, std::ios::beg); // Go to the beginning of the stream

        while(true) {
            innerStream.read(buffer, bufferSize);
            auto readBytes = innerStream.gcount();
            if (readBytes == 0) {
                break;
            }

            const auto readBytesSize = static_cast<size_t>(readBytes);
            for (auto i = 0; i < readBytes; i++) {
                buffer[i] = XOR(buffer[i], streamData.size() + i);
            }

            streamData.insert(streamData.end(), buffer, buffer + readBytes);
        }

        return streamData;
    }

private:
    std::ifstream& innerStream;
    long originalStreamLength;
    long length;

    char XOR(char b, long position) const {
        if (position > originalStreamLength - 4) {
            return b ^ TPF_XOREven;
        }

        return position % 2 == 0 ? b ^ TPF_XOREven : b ^ TPF_XOROdd;
    }

    char_type ReadByte() {
        char_type byte;
        auto position = innerStream.tellg();
        if (innerStream.get(byte)) {
            return XOR(byte, position);
        }

        return traits_type::eof();
    }

    static const char TPF_XOROdd = 0x3F;
    static const char TPF_XOREven = 0xA4;
};

