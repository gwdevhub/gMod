#include "uMod_Main.h"
#include "unzip.h"
#include "uMod_File.h"
#include "utils.h"
#include "uMod_Texture.h"
#include <fstream>
#include <sstream>
#include <vector>


#ifdef __CDT_PARSER__
#define FindZipItem(...) 0
#define UnzipItem(...) 0
#define CloseZip(...) 0
#define GetZipItem(...) 0
#endif


uMod_File::uMod_File()
{
    Loaded = false;
    XORed = false;
    FileInMemory = static_cast<char*>(nullptr);
    MemoryLength = 0u;
    FileLen = 0u;
}

uMod_File::uMod_File(const std::string file)
{
    Loaded = false;
    XORed = false;
    FileInMemory = static_cast<char*>(nullptr);
    MemoryLength = 0u;
    FileLen = 0u;
    SetFile(file);
}


uMod_File::~uMod_File()
{
    if (FileInMemory != static_cast<char*>(nullptr)) {
        delete[] FileInMemory;
    }
}


bool uMod_File::FileSupported()
{
    const std::string file_type = GetFileExtension(FileName);
    if (file_type == "zip") {
        return true;
    }
    if (file_type == "tpf") {
        return true;
    }
    if (file_type == "bmp") {
        return true;
    }
    if (file_type == "jpg") {
        return true;
    }
    if (file_type == "tga") {
        return true;
    }
    if (file_type == "png") {
        return true;
    }
    if (file_type == "dds") {
        return true;
    }
    if (file_type == "ppm") {
        return true;
    }

    return false;
}


bool uMod_File::PackageFile()
{
    const std::string file_type = GetFileExtension(FileName);
    if (file_type == "zip") {
        return true;
    }
    if (file_type == "tpf") {
        return true;
    }
    return false;
}

bool uMod_File::SingleFile()
{
    const std::string file_type = GetFileExtension(FileName);
    if (file_type == "bmp") {
        return true;
    }
    if (file_type == "jpg") {
        return true;
    }
    if (file_type == "tga") {
        return true;
    }
    if (file_type == "png") {
        return true;
    }
    if (file_type == "dds") {
        return true;
    }
    if (file_type == "ppm") {
        return true;
    }
    return false;
}


int uMod_File::GetContent()
{
    const std::string file_type = GetFileExtension(FileName);
    if (file_type == "zip") {
        AddZip();
    }
    else if (file_type == "tpf") {
        AddTpf();
    }
    else if (SingleFile()) {
        AddFile();
    }
    else {
        printf(FileName.c_str());
        printf(" Not supported\n");
        return -1;
    }

    return 0;
}

int uMod_File::ReadFile()
{
    if (Loaded) {
        return 0;
    }
    XORed = false;

    const auto name = FileName;
    std::ifstream inputFile(name, std::ios::binary);
    if (!inputFile ||
        !inputFile.is_open()) {
        printf(name.c_str());
        printf(" Could not open\n");
        return -1;
    }

    const std::vector<char> buffer(std::istreambuf_iterator<char>(inputFile), {});
    if (FileInMemory) {
        delete[] FileInMemory;
    }

    FileInMemory = new char[buffer.size()];
    for (auto i = 0; i < buffer.size(); i++) {
        FileInMemory[i] = buffer[i];
    }

    FileLen = buffer.size();
    inputFile.close();
    FileInMemory[FileLen] = 0;
    printf(name.c_str());
    printf("%d\n", FileLen);

    Loaded = true;
    return 0;
}



int uMod_File::UnXOR()
{
    if (XORed) {
        return 0;
    }
    /*
     *
     * BIG THANKS TO Tonttu
     * (TPFcreate 1.5)
     *
     */
    const auto buff = (unsigned int*)FileInMemory;
    const unsigned int TPF_XOR = 0x3FA43FA4u;
    const unsigned int size = FileLen / 4u;
    for (unsigned int i = 0; i < size; i++) {
        buff[i] ^= TPF_XOR;
    }
    for (unsigned int i = size * 4u; i < size * 4u + FileLen % 4u; i++) {
        ((unsigned char*)FileInMemory)[i] ^= static_cast<unsigned char>(TPF_XOR);
    }


    unsigned int pos = FileLen - 1;
    while (pos > 0u && FileInMemory[pos]) {
        pos--;
    }
    if (pos > 0u && pos < FileLen - 1) {
        FileLen = pos + 1;
    }
    XORed = true;

    /*
     * original code by Tonttu
     * The last bytes are not revealed correctly
    unsigned int j=0;
    while ( j <= result - 4 )
    {
      *( unsigned int* )( &buffer[j] ) ^= TPF_XOR;
      j += 4;
    }

    while ( j < result )
    {
      buffer[j] ^= (unsigned char )( TPF_XOR >> 24 );
      TPF_XOR <<= 4;
      j++;
    }
    */
    return 0;
}


int uMod_File::AddFile()
{
    DWORD64 temp_hash;

    std::string name = AfterLast(FileName, '_');
    name = BeforeLast(name, '.');

    try {
        // Convert hexadecimal string to unsigned long long
        temp_hash = std::stoull(name, nullptr, 16);
    }
    catch (const std::invalid_argument& e) {
        printf("Encountered error");
        printf(e.what());
    }
    catch (const std::out_of_range& e) {
        printf("Encountered error");
        printf(e.what());
    }

    if (const int ret = ReadFile()) {
        return ret;
    }

    FileHash = temp_hash;
    return 0;
}

int uMod_File::AddZip()
{
    if (const int ret = ReadFile()) {
        return ret;
    }
    return AddContent(nullptr);
}

int uMod_File::AddTpf()
{
    if (const int ret = ReadFile()) {
        return ret;
    }

    UnXOR();

    constexpr char pw[] = {
        0x73, 0x2A, 0x63, 0x7D, 0x5F, 0x0A, static_cast<char>(0xA6), static_cast<char>(0xBD),
        0x7D, 0x65, 0x7E, 0x67, 0x61, 0x2A, 0x7F, 0x7F,
        0x74, 0x61, 0x67, 0x5B, 0x60, 0x70, 0x45, 0x74,
        0x5C, 0x22, 0x74, 0x5D, 0x6E, 0x6A, 0x73, 0x41,
        0x77, 0x6E, 0x46, 0x47, 0x77, 0x49, 0x0C, 0x4B,
        0x46, 0x6F, '\0'
    };

    return AddContent(pw);
}


int uMod_File::AddContent(const char* pw)
{
    // Thats is really nasty code, but atm I am happy that it works. I should try an other unzip api,
    // This one seems to behave very strange.
    // I know, that also a bug in my code could be reason for the crashes, but UnzipItem( ... ) unzipes wrong files
    // and GetZipItem( ... ) returns garbage as file names, and the next call of GetZipItem( ... ) blows up the program.
    //
    // I have commented line 3519 in unzip.cpp.
    // It was stated this is a bug, but it did not solve my problems.
    //
    // closing and reopen the zip handle did the trick.
    //

    const auto& name = FileName;
    HZIP ZIP_Handle = OpenZip(FileInMemory, FileLen, pw);
    if (ZIP_Handle == static_cast<HZIP>(nullptr)) {
        printf(name.c_str());
        printf(" Failed to unzip file\n");
        return -1;
    }
    ZIPENTRY ze;
    int index;
    FindZipItem(ZIP_Handle, "texmod.def", false, &index, &ze);
    if (index >= 0) //if texmod.def is present in the zip file
    {
        printf(name.c_str());
        printf(" Unzipping based on texmod.def of size %d\n", ze.unc_size);
        char* def;
        int len = ze.unc_size;
        try { def = new char[len + 1]; }
        catch (...) {
            printf(name.c_str());
            printf(" Memory error\n");
            return -1;
        }
        ZRESULT zr = UnzipItem(ZIP_Handle, index, def, len);

        if (zr != ZR_OK && zr != ZR_MORE) {
            delete[] def;
            return -1;
        }
        def[len] = 0;

        std::stringstream tokenStream(def);
        std::string token;
        DWORD64 temp_hash;
        std::string entry;
        std::string file;

        while (std::getline(tokenStream, token)) {
            entry = token;
            printf(name.c_str());
            printf(" Parsing token %s\n", token.c_str());
            file = BeforeFirst(token, '|');
            try {
                temp_hash = std::stoull(file, nullptr, 16);
                // Successful conversion; can continue processing...
            }
            catch (const std::invalid_argument&) {
                // Handle invalid argument
                printf(name.c_str());
                printf(" Invalid hash\n");
                continue; // Skip the rest of the current loop iteration
            }
            catch (const std::out_of_range&) {
                // Handle out of range
                printf(name.c_str());
                printf(" Invalid hash\n");
                continue; // Skip the rest of the current loop iteration
            }

            file = AfterFirst(entry, '|');
            ReplaceAll(file, "\r", "");

            while ((!file.empty() && file[0] == '.' && (file.size() > 1 && (file[1] == '/' || file[1] == '\\')))
                   || (!file.empty() && (file[0] == '/' || file[0] == '\\'))) {
                file.erase(0, 1);
            }

            FindZipItem(ZIP_Handle, file.c_str(), false, &index, &ze); // look for texture
            if (index >= 0) {
                std::vector<char> data;
                UModTexture texture;
                data.resize(ze.unc_size);

                ZRESULT rz = UnzipItem(ZIP_Handle, index, data.data(), ze.unc_size);
                if (rz != ZR_OK && rz != ZR_MORE) {
                    printf(name.c_str());
                    printf(" Unzip error\n");
                }
                else {
                    texture.hash = temp_hash;
                    texture.data = data;
                    texture.name = file;
                    Textures.push_back(texture);
                    printf(name.c_str());
                    printf(" Added texture of size %d %s\n", data.size(), file.c_str());
                }
            }
            else {
                printf(name.c_str());
                printf(" Unzip error\n");
                CloseZip(ZIP_Handle); //somehow we need to close and to reopen the zip handle, otherwise the program crashes
                ZIP_Handle = OpenZip(FileInMemory, FileLen, pw);
            }
        }
        delete[] def;

        CloseZip(ZIP_Handle);
        if (Textures.size() == 0) {
            printf(name.c_str());
            printf(" No textures parsed\n");
            return -1;
        }

        return 0;
    }

    //we load each dds file
    {
        printf(name.c_str());
        printf(" Unzipping without texmode.def\n");
        CloseZip(ZIP_Handle); //somehow we need to close and to reopen the zip handle, otherwise the program crashes
        ZIP_Handle = OpenZip(FileInMemory, FileLen, pw);
        if (ZIP_Handle == static_cast<HZIP>(nullptr)) {
            printf(name.c_str());
            printf(" Failed to unzip file");
            return -1;
        }
        std::string file;
        GetZipItem(ZIP_Handle, -1, &ze); //ask for number of entries
        int num = ze.index;
        DWORD64 temp_hash;
        for (int i = 0; i < num; i++) {
            if (GetZipItem(ZIP_Handle, i, &ze) != ZR_OK) {
                continue; //ask for name and size
            }
            int len = ze.unc_size;

            printf(name.c_str());
            printf(" Parsing token %s\n", ze.name);
            std::vector<char> data;
            UModTexture texture;
            data.resize(ze.unc_size);

            ZRESULT rz = UnzipItem(ZIP_Handle, i, data.data(), len);
            if (rz != ZR_OK && rz != ZR_MORE) {
                printf(name.c_str());
                printf(" Unzip error\n");
                continue;
            }

            file = ze.name;
            if (file.size() == 0) {
                continue;
            }

            if (file == "Comment.txt") // skip comment
            {
                continue;
            }

            auto entryName = AfterLast(file, '.');
            if (entryName != "dds") {
                continue;
            }

            entryName = AfterLast(file, L'_');
            entryName = BeforeLast(entryName, L'.');

            try {
                temp_hash = std::stoull(entryName, nullptr, 16); // Convert hex string to number
            }
            catch (const std::invalid_argument& e) {
                printf(name.c_str());
                printf(" Invalid hash\n");
                continue;
            }
            catch (const std::out_of_range& e) {
                printf(name.c_str());
                printf(" Invalid hash\n");
                continue;
            }

            texture.hash = temp_hash;
            texture.name = entryName;
            texture.data = data;
            Textures.push_back(texture);
            printf(name.c_str());
            printf(" Added texture of size %d %s\n", data.size(), entryName.c_str());
        }

        CloseZip(ZIP_Handle);
        if (Textures.size() == 0) {
            printf(name.c_str());
            printf(" No textures parsed\n");
            return -1;
        }
        return 0;
    }
    return 0;
}
