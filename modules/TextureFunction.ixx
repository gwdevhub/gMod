module;

#include "Main.h"
#include <d3d9types.h>
#include <DirectXTex/DirectXTex.h>

export module TextureFunction;

export template <typename T>
concept uModTexturePtr = requires(T a)
{
    std::same_as<uMod_IDirect3DTexture9*, T> ||
    std::same_as<uMod_IDirect3DVolumeTexture9*, T> ||
    std::same_as<uMod_IDirect3DCubeTexture9*, T>;
};

export template <typename T>
concept uModTexturePtrPtr = uModTexturePtr<std::remove_pointer_t<T>>;

export template <typename T> requires uModTexturePtr<T>
void UnswitchTextures(T pTexture)
{
    decltype(pTexture) CrossRef = pTexture->CrossRef_D3Dtex;
    if (CrossRef != nullptr) {
        std::swap(pTexture->m_D3Dtex, CrossRef->m_D3Dtex);

        // cancel the link
        CrossRef->CrossRef_D3Dtex = nullptr;
        pTexture->CrossRef_D3Dtex = nullptr;
    }
}

export template <typename T> requires uModTexturePtr<T>
int SwitchTextures(T pTexture1, T pTexture2)
{
    if (pTexture1->m_D3Ddev == pTexture2->m_D3Ddev && pTexture1->CrossRef_D3Dtex == nullptr && pTexture2->CrossRef_D3Dtex == nullptr) {
        // make cross reference
        pTexture1->CrossRef_D3Dtex = pTexture2;
        pTexture2->CrossRef_D3Dtex = pTexture1;

        // switch textures
        std::swap(pTexture1->m_D3Dtex, pTexture2->m_D3Dtex);
        return RETURN_OK;
    }
    return RETURN_TEXTURE_NOT_SWITCHED;
}

constexpr auto crctab64 = std::to_array({
  0x0000000000000000ULL, 0x7ad870c830358979ULL, 0xf5b0e190606b12f2ULL,
  0x8f689158505e9b8bULL, 0xc038e5739841b68fULL, 0xbae095bba8743ff6ULL,
  0x358804e3f82aa47dULL, 0x4f50742bc81f2d04ULL, 0xab28ecb46814fe75ULL,
  0xd1f09c7c5821770cULL, 0x5e980d24087fec87ULL, 0x24407dec384a65feULL,
  0x6b1009c7f05548faULL, 0x11c8790fc060c183ULL, 0x9ea0e857903e5a08ULL,
  0xe478989fa00bd371ULL, 0x7d08ff3b88be6f81ULL, 0x07d08ff3b88be6f8ULL,
  0x88b81eabe8d57d73ULL, 0xf2606e63d8e0f40aULL, 0xbd301a4810ffd90eULL,
  0xc7e86a8020ca5077ULL, 0x4880fbd87094cbfcULL, 0x32588b1040a14285ULL,
  0xd620138fe0aa91f4ULL, 0xacf86347d09f188dULL, 0x2390f21f80c18306ULL,
  0x594882d7b0f40a7fULL, 0x1618f6fc78eb277bULL, 0x6cc0863448deae02ULL,
  0xe3a8176c18803589ULL, 0x997067a428b5bcf0ULL, 0xfa11fe77117cdf02ULL,
  0x80c98ebf2149567bULL, 0x0fa11fe77117cdf0ULL, 0x75796f2f41224489ULL,
  0x3a291b04893d698dULL, 0x40f16bccb908e0f4ULL, 0xcf99fa94e9567b7fULL,
  0xb5418a5cd963f206ULL, 0x513912c379682177ULL, 0x2be1620b495da80eULL,
  0xa489f35319033385ULL, 0xde51839b2936bafcULL, 0x9101f7b0e12997f8ULL,
  0xebd98778d11c1e81ULL, 0x64b116208142850aULL, 0x1e6966e8b1770c73ULL,
  0x8719014c99c2b083ULL, 0xfdc17184a9f739faULL, 0x72a9e0dcf9a9a271ULL,
  0x08719014c99c2b08ULL, 0x4721e43f0183060cULL, 0x3df994f731b68f75ULL,
  0xb29105af61e814feULL, 0xc849756751dd9d87ULL, 0x2c31edf8f1d64ef6ULL,
  0x56e99d30c1e3c78fULL, 0xd9810c6891bd5c04ULL, 0xa3597ca0a188d57dULL,
  0xec09088b6997f879ULL, 0x96d1784359a27100ULL, 0x19b9e91b09fcea8bULL,
  0x636199d339c963f2ULL, 0xdf7adabd7a6e2d6fULL, 0xa5a2aa754a5ba416ULL,
  0x2aca3b2d1a053f9dULL, 0x50124be52a30b6e4ULL, 0x1f423fcee22f9be0ULL,
  0x659a4f06d21a1299ULL, 0xeaf2de5e82448912ULL, 0x902aae96b271006bULL,
  0x74523609127ad31aULL, 0x0e8a46c1224f5a63ULL, 0x81e2d7997211c1e8ULL,
  0xfb3aa75142244891ULL, 0xb46ad37a8a3b6595ULL, 0xceb2a3b2ba0eececULL,
  0x41da32eaea507767ULL, 0x3b024222da65fe1eULL, 0xa2722586f2d042eeULL,
  0xd8aa554ec2e5cb97ULL, 0x57c2c41692bb501cULL, 0x2d1ab4dea28ed965ULL,
  0x624ac0f56a91f461ULL, 0x1892b03d5aa47d18ULL, 0x97fa21650afae693ULL,
  0xed2251ad3acf6feaULL, 0x095ac9329ac4bc9bULL, 0x7382b9faaaf135e2ULL,
  0xfcea28a2faafae69ULL, 0x8632586aca9a2710ULL, 0xc9622c4102850a14ULL,
  0xb3ba5c8932b0836dULL, 0x3cd2cdd162ee18e6ULL, 0x460abd1952db919fULL,
  0x256b24ca6b12f26dULL, 0x5fb354025b277b14ULL, 0xd0dbc55a0b79e09fULL,
  0xaa03b5923b4c69e6ULL, 0xe553c1b9f35344e2ULL, 0x9f8bb171c366cd9bULL,
  0x10e3202993385610ULL, 0x6a3b50e1a30ddf69ULL, 0x8e43c87e03060c18ULL,
  0xf49bb8b633338561ULL, 0x7bf329ee636d1eeaULL, 0x012b592653589793ULL,
  0x4e7b2d0d9b47ba97ULL, 0x34a35dc5ab7233eeULL, 0xbbcbcc9dfb2ca865ULL,
  0xc113bc55cb19211cULL, 0x5863dbf1e3ac9decULL, 0x22bbab39d3991495ULL,
  0xadd33a6183c78f1eULL, 0xd70b4aa9b3f20667ULL, 0x985b3e827bed2b63ULL,
  0xe2834e4a4bd8a21aULL, 0x6debdf121b863991ULL, 0x1733afda2bb3b0e8ULL,
  0xf34b37458bb86399ULL, 0x8993478dbb8deae0ULL, 0x06fbd6d5ebd3716bULL,
  0x7c23a61ddbe6f812ULL, 0x3373d23613f9d516ULL, 0x49aba2fe23cc5c6fULL,
  0xc6c333a67392c7e4ULL, 0xbc1b436e43a74e9dULL, 0x95ac9329ac4bc9b5ULL,
  0xef74e3e19c7e40ccULL, 0x601c72b9cc20db47ULL, 0x1ac40271fc15523eULL,
  0x5594765a340a7f3aULL, 0x2f4c0692043ff643ULL, 0xa02497ca54616dc8ULL,
  0xdafce7026454e4b1ULL, 0x3e847f9dc45f37c0ULL, 0x445c0f55f46abeb9ULL,
  0xcb349e0da4342532ULL, 0xb1eceec59401ac4bULL, 0xfebc9aee5c1e814fULL,
  0x8464ea266c2b0836ULL, 0x0b0c7b7e3c7593bdULL, 0x71d40bb60c401ac4ULL,
  0xe8a46c1224f5a634ULL, 0x927c1cda14c02f4dULL, 0x1d148d82449eb4c6ULL,
  0x67ccfd4a74ab3dbfULL, 0x289c8961bcb410bbULL, 0x5244f9a98c8199c2ULL,
  0xdd2c68f1dcdf0249ULL, 0xa7f41839ecea8b30ULL, 0x438c80a64ce15841ULL,
  0x3954f06e7cd4d138ULL, 0xb63c61362c8a4ab3ULL, 0xcce411fe1cbfc3caULL,
  0x83b465d5d4a0eeceULL, 0xf96c151de49567b7ULL, 0x76048445b4cbfc3cULL,
  0x0cdcf48d84fe7545ULL, 0x6fbd6d5ebd3716b7ULL, 0x15651d968d029fceULL,
  0x9a0d8ccedd5c0445ULL, 0xe0d5fc06ed698d3cULL, 0xaf85882d2576a038ULL,
  0xd55df8e515432941ULL, 0x5a3569bd451db2caULL, 0x20ed197575283bb3ULL,
  0xc49581ead523e8c2ULL, 0xbe4df122e51661bbULL, 0x3125607ab548fa30ULL,
  0x4bfd10b2857d7349ULL, 0x04ad64994d625e4dULL, 0x7e7514517d57d734ULL,
  0xf11d85092d094cbfULL, 0x8bc5f5c11d3cc5c6ULL, 0x12b5926535897936ULL,
  0x686de2ad05bcf04fULL, 0xe70573f555e26bc4ULL, 0x9ddd033d65d7e2bdULL,
  0xd28d7716adc8cfb9ULL, 0xa85507de9dfd46c0ULL, 0x273d9686cda3dd4bULL,
  0x5de5e64efd965432ULL, 0xb99d7ed15d9d8743ULL, 0xc3450e196da80e3aULL,
  0x4c2d9f413df695b1ULL, 0x36f5ef890dc31cc8ULL, 0x79a59ba2c5dc31ccULL,
  0x037deb6af5e9b8b5ULL, 0x8c157a32a5b7233eULL, 0xf6cd0afa9582aa47ULL,
  0x4ad64994d625e4daULL, 0x300e395ce6106da3ULL, 0xbf66a804b64ef628ULL,
  0xc5bed8cc867b7f51ULL, 0x8aeeace74e645255ULL, 0xf036dc2f7e51db2cULL,
  0x7f5e4d772e0f40a7ULL, 0x05863dbf1e3ac9deULL, 0xe1fea520be311aafULL,
  0x9b26d5e88e0493d6ULL, 0x144e44b0de5a085dULL, 0x6e963478ee6f8124ULL,
  0x21c640532670ac20ULL, 0x5b1e309b16452559ULL, 0xd476a1c3461bbed2ULL,
  0xaeaed10b762e37abULL, 0x37deb6af5e9b8b5bULL, 0x4d06c6676eae0222ULL,
  0xc26e573f3ef099a9ULL, 0xb8b627f70ec510d0ULL, 0xf7e653dcc6da3dd4ULL,
  0x8d3e2314f6efb4adULL, 0x0256b24ca6b12f26ULL, 0x788ec2849684a65fULL,
  0x9cf65a1b368f752eULL, 0xe62e2ad306bafc57ULL, 0x6946bb8b56e467dcULL,
  0x139ecb4366d1eea5ULL, 0x5ccebf68aecec3a1ULL, 0x2616cfa09efb4ad8ULL,
  0xa97e5ef8cea5d153ULL, 0xd3a62e30fe90582aULL, 0xb0c7b7e3c7593bd8ULL,
  0xca1fc72bf76cb2a1ULL, 0x45775673a732292aULL, 0x3faf26bb9707a053ULL,
  0x70ff52905f188d57ULL, 0x0a2722586f2d042eULL, 0x854fb3003f739fa5ULL,
  0xff97c3c80f4616dcULL, 0x1bef5b57af4dc5adULL, 0x61372b9f9f784cd4ULL,
  0xee5fbac7cf26d75fULL, 0x9487ca0fff135e26ULL, 0xdbd7be24370c7322ULL,
  0xa10fceec0739fa5bULL, 0x2e675fb4576761d0ULL, 0x54bf2f7c6752e8a9ULL,
  0xcdcf48d84fe75459ULL, 0xb71738107fd2dd20ULL, 0x387fa9482f8c46abULL,
  0x42a7d9801fb9cfd2ULL, 0x0df7adabd7a6e2d6ULL, 0x772fdd63e7936bafULL,
  0xf8474c3bb7cdf024ULL, 0x829f3cf387f8795dULL, 0x66e7a46c27f3aa2cULL,
  0x1c3fd4a417c62355ULL, 0x935745fc4798b8deULL, 0xe98f353477ad31a7ULL,
  0xa6df411fbfb21ca3ULL, 0xdc0731d78f8795daULL, 0x536fa08fdfd90e51ULL,
  0x29b7d047efec8728ULL
});

export namespace TextureFunction {

    uint64_t get_crc64(const char* data, unsigned int length) {
        uint64_t crc = 0xFFFFFFFFFFFFFFFFULL;

        while (length--) {
            crc = crctab64[(crc ^ *data++) & 0xFF] ^ (crc >> 8);
        }

        return crc;
    }

    uint32_t get_crc32(const char* data_ptr, const unsigned int length)
    {
        constexpr static auto crc32_poly = 0xEDB88320u;
        constexpr static auto ul_crc_in = 0xffffffff;
        unsigned int crc = ul_crc_in;
        for (unsigned int idx = 0u; idx < length; idx++) {
            unsigned int data = *data_ptr++;
            for (unsigned int bit = 0u; bit < 8u; bit++, data >>= 1) {
                crc = crc >> 1 ^ ((crc ^ data) & 1 ? crc32_poly : 0);
            }
        }
        return crc;
    }

    int GetBitsFromFormat(D3DFORMAT format)
    {
        switch (format) //switch trough the formats to calculate the size of the raw data
        {
            case D3DFMT_A1: // 1-bit monochrome.
            {
                return 1;
            }

            case D3DFMT_R3G3B2: // 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue.
            case D3DFMT_A8: // 8-bit alpha only.
            case D3DFMT_A8P8: // 8-bit color indexed with 8 bits of alpha.
            case D3DFMT_P8: // 8-bit color indexed.
            case D3DFMT_L8: // 8-bit luminance only.
            case D3DFMT_A4L4: // 8-bit using 4 bits each for alpha and luminance.
            case D3DFMT_FORCE_DWORD:
            case D3DFMT_S8_LOCKABLE: // A lockable 8-bit stencil buffer.
            {
                return 8;
            }

            case D3DFMT_D16_LOCKABLE: //16-bit z-buffer bit depth.
            case D3DFMT_D15S1: // 16-bit z-buffer bit depth where 15 bits are reserved for the depth channel and 1 bit is reserved for the stencil channel.
            case D3DFMT_L6V5U5: // 16-bit bump-map format with luminance using 6 bits for luminance, and 5 bits each for v and u.
            case D3DFMT_V8U8: // 16-bit bump-map format using 8 bits each for u and v data.
            case D3DFMT_CxV8U8: // 16-bit normal compression format. The texture sampler computes the C channel from: C = sqrt(1 - U2 - V2).
            case D3DFMT_R5G6B5: // 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue.
            case D3DFMT_X1R5G5B5: // 16-bit pixel format where 5 bits are reserved for each color.
            case D3DFMT_A1R5G5B5: // 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha.
            case D3DFMT_A4R4G4B4: // 16-bit ARGB pixel format with 4 bits for each channel.
            case D3DFMT_A8R3G3B2: // 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue.
            case D3DFMT_X4R4G4B4: // 16-bit RGB pixel format using 4 bits for each color.
            case D3DFMT_L16: // 16-bit luminance only.
            case D3DFMT_R16F: // 16-bit float format using 16 bits for the red channel.
            case D3DFMT_A8L8: // 16-bit using 8 bits each for alpha and luminance.
            case D3DFMT_D16: // 16-bit z-buffer bit depth.
            case D3DFMT_INDEX16: // 16-bit index buffer bit depth.
            case D3DFMT_G8R8_G8B8: // ??
            case D3DFMT_R8G8_B8G8: // ??
            case D3DFMT_UYVY: // ??
            case D3DFMT_YUY2: // ??
            {
                return 16;
            }


            case D3DFMT_R8G8B8: //24-bit RGB pixel format with 8 bits per channel.
            {
                return 24;
            }

            case D3DFMT_R32F: // 32-bit float format using 32 bits for the red channel.
            case D3DFMT_X8L8V8U8: // 32-bit bump-map format with luminance using 8 bits for each channel.
            case D3DFMT_A2W10V10U10: // 32-bit bump-map format using 2 bits for alpha and 10 bits each for w, v, and u.
            case D3DFMT_Q8W8V8U8: // 32-bit bump-map format using 8 bits for each channel.
            case D3DFMT_V16U16: // 32-bit bump-map format using 16 bits for each channel.
            case D3DFMT_A8R8G8B8: // 32-bit ARGB pixel format with alpha, using 8 bits per channel.
            case D3DFMT_X8R8G8B8: // 32-bit RGB pixel format, where 8 bits are reserved for each color.
            case D3DFMT_A2B10G10R10: // 32-bit pixel format using 10 bits for each color and 2 bits for alpha.
            case D3DFMT_A8B8G8R8: // 32-bit ARGB pixel format with alpha, using 8 bits per channel.
            case D3DFMT_X8B8G8R8: // 32-bit RGB pixel format, where 8 bits are reserved for each color.
            case D3DFMT_G16R16: // 32-bit pixel format using 16 bits each for green and red.
            case D3DFMT_G16R16F: // 32-bit float format using 16 bits for the red channel and 16 bits for the green channel.
            case D3DFMT_A2R10G10B10: // 32-bit pixel format using 10 bits each for red, green, and blue, and 2 bits for alpha.
            case D3DFMT_D32: // 32-bit z-buffer bit depth.
            case D3DFMT_D24S8: // 32-bit z-buffer bit depth using 24 bits for the depth channel and 8 bits for the stencil channel.
            case D3DFMT_D24X8: //32-bit z-buffer bit depth using 24 bits for the depth channel.
            case D3DFMT_D24X4S4: // 32-bit z-buffer bit depth using 24 bits for the depth channel and 4 bits for the stencil channel.
            case D3DFMT_D32F_LOCKABLE: // A lockable format where the depth value is represented as a standard IEEE floating-point number.
            case D3DFMT_D24FS8: // A non-lockable format that contains 24 bits of depth (in a 24-bit floating point format - 20e4) and 8 bits of stencil.
            case D3DFMT_D32_LOCKABLE: // A lockable 32-bit depth buffer.
            case D3DFMT_INDEX32: // 32-bit index buffer bit depth.
            {
                return 32;
            }

            case D3DFMT_G32R32F: // 64-bit float format using 32 bits for the red channel and 32 bits for the green channel.
            case D3DFMT_Q16W16V16U16: // 64-bit bump-map format using 16 bits for each component.
            case D3DFMT_A16B16G16R16: // 64-bit pixel format using 16 bits for each component.
            case D3DFMT_A16B16G16R16F: // 64-bit float format using 16 bits for the each channel (alpha, blue, green, red).
            {
                return 64;
            }

            case D3DFMT_A32B32G32R32F: // 128-bit float format using 32 bits for the each channel (alpha, blue, green, red).
            {
                return 128;
            }
            case D3DFMT_DXT2:
            case D3DFMT_DXT3:
            case D3DFMT_DXT4:
            case D3DFMT_DXT5: {
                return 8;
            }
            case D3DFMT_DXT1: {
                return 4;
            }
            default: //compressed formats
            {
                return 4;
            }
        }
    }

    DirectX::ScratchImage ImageConvertToBGRA(DirectX::ScratchImage& image, const TexEntry& entry)
    {
        if (image.GetMetadata().format == DXGI_FORMAT_B8G8R8A8_UNORM ||
            image.GetMetadata().format == DXGI_FORMAT_BC1_UNORM ||
            image.GetMetadata().format == DXGI_FORMAT_BC2_UNORM ||
            image.GetMetadata().format == DXGI_FORMAT_BC3_UNORM ||
            image.GetMetadata().format == DXGI_FORMAT_BC4_UNORM ||
            image.GetMetadata().format == DXGI_FORMAT_BC5_UNORM) {
            return std::move(image);
        }
        DirectX::ScratchImage bgra_image;
        const HRESULT hr = DirectX::Convert(
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            DXGI_FORMAT_B8G8R8A8_UNORM,
            DirectX::TEX_FILTER_DEFAULT,
            DirectX::TEX_THRESHOLD_DEFAULT,
            bgra_image);
        if (FAILED(hr)) {
            Warning("ImageConvertToBGRA (%#lX%s): FAILED\n", entry.crc_hash, entry.ext.c_str());
            bgra_image = std::move(image);
        }
        image.Release();
        return bgra_image;
    }

    DirectX::ScratchImage ImageGenerateMipMaps(DirectX::ScratchImage& image, const TexEntry& entry)
    {
        if (entry.ext == ".dds") {
            return std::move(image);
        }
        DirectX::ScratchImage mipmapped_image;
        const auto hr = DirectX::GenerateMipMaps(
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            DirectX::TEX_FILTER_DEFAULT,
            0,
            mipmapped_image);
        if (FAILED(hr)) {
            Warning("GenerateMipMaps (%#lX%s): FAILED\n", entry.crc_hash, entry.ext.c_str());
            mipmapped_image = std::move(image);
        }
        image.Release();
        return mipmapped_image;
    }

    DirectX::ScratchImage ImageCompress(DirectX::ScratchImage& image, const TexEntry& entry)
    {
        if (image.GetMetadata().format == DXGI_FORMAT_BC1_UNORM ||
            image.GetMetadata().format == DXGI_FORMAT_BC2_UNORM ||
            image.GetMetadata().format == DXGI_FORMAT_BC3_UNORM ||
            image.GetMetadata().format == DXGI_FORMAT_BC4_UNORM ||
            image.GetMetadata().format == DXGI_FORMAT_BC5_UNORM) {
            return std::move(image);
        }
        DirectX::ScratchImage compressed_image;
        const auto hr = DirectX::Compress(
            image.GetImages(),
            image.GetImageCount(),
            image.GetMetadata(),
            DXGI_FORMAT_BC3_UNORM,
            DirectX::TEX_COMPRESS_DEFAULT,
            DirectX::TEX_THRESHOLD_DEFAULT,
            compressed_image);
        if (FAILED(hr)) {
            Warning("ImageCompress (%#lX%s): FAILED\n", entry.crc_hash, entry.ext.c_str());
            compressed_image = std::move(image);
        }
        image.Release();
        return compressed_image;
    }

    void ImageSave(const DirectX::ScratchImage& image, const TexEntry& entry, const std::filesystem::path& dll_path)
    {
        const auto file_name = std::format("0x{:x}.dds", entry.crc_hash);
        const auto file_out = dll_path / "textures" / file_name;
        try {
            if (std::filesystem::exists(file_out)) {
                return;
            }
            if (!std::filesystem::exists(file_out.parent_path())) {
                std::filesystem::create_directory(file_out.parent_path());
            }
            const auto hr = DirectX::SaveToDDSFile(
                image.GetImages(),
                image.GetImageCount(),
                image.GetMetadata(),
                DirectX::DDS_FLAGS_NONE,
                file_out.c_str());
            if (FAILED(hr)) {
                Warning("SaveDDSImageToDisk (%#lX%s): FAILED\n", entry.crc_hash, entry.ext.c_str());
            }
        }
        catch (const std::exception& e) {
            Warning("SaveDDSImageToDisk (%#lX%s): %s\n", entry.crc_hash, entry.ext.c_str(), e.what());
            return;
        }
    }

    DirectX::Blob ConvertToCompressedDDS(TexEntry& entry, const bool compress, [[maybe_unused]] const std::filesystem::path& dll_path)
    {
        DirectX::ScratchImage image;
        HRESULT hr = 0;

        if (entry.ext == ".dds") {
            hr = DirectX::LoadFromDDSMemory(entry.data.data(), entry.data.size(), DirectX::DDS_FLAGS_NONE, nullptr, image);
        }
        else if (entry.ext == ".tga") {
            hr = DirectX::LoadFromTGAMemory(entry.data.data(), entry.data.size(), DirectX::TGA_FLAGS_BGR, nullptr, image);
        }
        else if (entry.ext == ".hdr") {
            hr = DirectX::LoadFromHDRMemory(entry.data.data(), entry.data.size(), nullptr, image);
        }
        else {
            hr = DirectX::LoadFromWICMemory(entry.data.data(), entry.data.size(), DirectX::WIC_FLAGS_NONE, nullptr, image);
            if (image.GetMetadata().format == DXGI_FORMAT_B8G8R8X8_UNORM) {
                image.OverrideFormat(DXGI_FORMAT_B8G8R8A8_UNORM);
            }
        }
        entry.data.clear();
        if (FAILED(hr)) {
            Warning("LoadImageFromMemory (%#lX%s): FAILED\n", entry.crc_hash, entry.ext.c_str());
            return {};
        }

        auto bgra_image = ImageConvertToBGRA(image, entry);
        auto mipmapped_image = ImageGenerateMipMaps(bgra_image, entry);
        const auto compressed_image = compress ? ImageCompress(mipmapped_image, entry) : std::move(mipmapped_image);

        DirectX::Blob dds_blob;
        hr = DirectX::SaveToDDSMemory(
            compressed_image.GetImages(),
            compressed_image.GetImageCount(),
            compressed_image.GetMetadata(),
            DirectX::DDS_FLAGS_NONE,
            dds_blob);
        if (FAILED(hr)) {
            Warning("SaveDDSImageToMemory (%#lX%s): FAILED\n", entry.crc_hash, entry.ext.c_str());
            return {};
        }

    #ifdef _DEBUG
        ImageSave(compressed_image, entry, dll_path);
    #endif
        return dds_blob;
    }
}
