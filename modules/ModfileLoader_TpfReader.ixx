export module ModfileLoader.TpfReader;

import std;

export class TpfReader {
public:
    TpfReader(const std::filesystem::path& path)
    {
        file_stream = std::ifstream(path, std::ios::binary);
        if (!file_stream.seekg(0, std::ios::end).good() || !file_stream.seekg(0, std::ios::beg).good()) {
            throw std::invalid_argument("Provided stream needs to have SEEK set to True");
        }
    }

    ~TpfReader()
    {
        file_stream.close();
    }

    std::vector<char> ReadToEnd()
    {
        file_stream.seekg(0, std::ios::end);
        line_length = file_stream.tellg();
        file_stream.seekg(0, std::ios::beg); // Go to the beginning of the stream

        std::vector<char> data(line_length);
        file_stream.read(data.data(), line_length);
        for (auto i = 0; i < data.size(); i++) {
            data[i] = XOR(data[i], i);
        }

        for (int i = data.size() - 1; i > 0 && data[i] != 0; i--) {
            data[i] = 0;
        }

        // in the other zip libraries, these had to be cut off, with libzip we need to zero them out
        // cutting them off makes the archive invalid
        //data.resize(last_zero);

        return data;
    }

private:
    std::ifstream file_stream;
    long line_length;

    [[nodiscard]] char XOR(const char b, const long position) const
    {
        if (position > line_length - 4) {
            return b ^ TPF_XOREven;
        }

        return position % 2 == 0 ? b ^ TPF_XOREven : b ^ TPF_XOROdd;
    }

    static constexpr char TPF_XOROdd = 0x3F;
    static constexpr char TPF_XOREven = static_cast<char>(0xA4);
};
