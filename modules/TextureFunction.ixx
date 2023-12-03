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

export namespace TextureFunction {

    unsigned int GetCRC32(char* pcDatabuf, unsigned int ulDatalen)
    {
        constexpr static auto crc32_poly = 0xEDB88320u;
        constexpr static auto ul_crc_in = 0xffffffff;
        unsigned int crc = ul_crc_in;
        for (unsigned int idx = 0u; idx < ulDatalen; idx++) {
            unsigned int data = *pcDatabuf++;
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
        DirectX::ScratchImage bgra_image;
        if (image.GetMetadata().format != DXGI_FORMAT_B8G8R8A8_UNORM) {
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
        }
        else {
            bgra_image = std::move(image);
        }
        image.Release();
        return bgra_image;
    }

    DirectX::ScratchImage ImageGenerateMipMaps(DirectX::ScratchImage& image, const TexEntry& entry)
    {
        DirectX::ScratchImage mipmapped_image;
        if (entry.ext != ".dds") {
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
        }
        else {
            mipmapped_image = std::move(image);
        }
        image.Release();
        return mipmapped_image;
    }

    DirectX::ScratchImage ImageCompress(DirectX::ScratchImage& image, const TexEntry& entry)
    {
        DirectX::ScratchImage compressed_image;
        if (image.GetMetadata().format != DXGI_FORMAT_BC3_UNORM) {
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
        }
        else {
            compressed_image = std::move(image);
        }
        image.Release();
        return compressed_image;
    }

    void ImageSave(const DirectX::ScratchImage& image, const TexEntry& entry, const std::filesystem::path& dll_path)
    {
        const auto file_name = std::format("0x{:x}.dds", entry.crc_hash);
        const auto file_out = dll_path / "textures" / file_name;
        std::filesystem::create_directory(file_out.parent_path());
        if (!std::filesystem::exists(file_out)) {
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
    }

    DirectX::Blob ConvertToCompressedDDS(TexEntry& entry, const std::filesystem::path& dll_path)
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
                // todo: this is undefined behaviour, but we must force them to be interpreted as BGRA instead of BGRX
                const_cast<DXGI_FORMAT&>(image.GetMetadata().format) = DXGI_FORMAT_B8G8R8A8_UNORM;
                const auto images = image.GetImages();
                for (int i = 0; i < image.GetImageCount(); ++i) {
                    const_cast<DXGI_FORMAT&>(images[i].format) = DXGI_FORMAT_B8G8R8A8_UNORM;
                }
            }
        }
        entry.data.clear();
        if (FAILED(hr)) {
            Warning("LoadImageFromMemory (%#lX%s): FAILED\n", entry.crc_hash, entry.ext.c_str());
            return {};
        }

        auto bgra_image = ImageConvertToBGRA(image, entry);
        auto mipmapped_image = ImageGenerateMipMaps(bgra_image, entry);
        const auto compressed_image = ImageCompress(mipmapped_image, entry);

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
