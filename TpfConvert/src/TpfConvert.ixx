#include <corecrt_wstdio.h>
#include <Windows.h>
#include <d3dx9.h>
#include <d3d9.h>

import std;
import ModfileLoader;
import <libzippp.h>;

struct TexEntry;

namespace {
    IDirect3D9* pD3D = nullptr;
    IDirect3DDevice9* pDevice = nullptr;

    bool InitializeDirect3D()
    {
        pD3D = Direct3DCreate9(D3D_SDK_VERSION);
        if (pD3D == nullptr) {
            std::print(stderr, "Failed to create Direct3D9 object\n");
            return false;
        }

        D3DPRESENT_PARAMETERS d3dpp = {};
        d3dpp.Windowed = TRUE;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = GetDesktopWindow();

        if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetDesktopWindow(),
            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice))) {
            std::print(stderr, "Failed to create Direct3D9 device\n");
            pD3D->Release();
            return false;
        }

        return true;
    }

    void CleanupDirect3D()
    {
        if (pDevice) pDevice->Release();
        if (pD3D) pD3D->Release();
    }

    std::pair<std::string, std::vector<uint8_t>> SaveAsDDS(const TexEntry& entry)
    {
        IDirect3DTexture9* pTexture = nullptr;
        ID3DXBuffer* pBuffer = nullptr;

        HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(pDevice,
                                                         entry.data.data(), entry.data.size(),
                                                         D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
                                                         D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE,
                                                         D3DX_FILTER_NONE, 0, NULL, NULL, &pTexture);

        if (FAILED(hr)) {
            std::print(stderr, "Failed to create texture from memory\n");
            return {};
        }

        hr = D3DXSaveTextureToFileInMemory(&pBuffer, D3DXIFF_DDS, pTexture, NULL);
        if (FAILED(hr)) {
            std::print(stderr, "Failed to save texture to DDS memory buffer\n");
            pTexture->Release();
            return {};
        }

        std::string dds_filename = std::format("0x{:x}.dds", entry.crc_hash);
        std::vector<uint8_t> dds_data(pBuffer->GetBufferSize());
        std::memcpy(dds_data.data(), pBuffer->GetBufferPointer(), pBuffer->GetBufferSize());

        pTexture->Release();
        pBuffer->Release();

        return {dds_filename, dds_data};
    }

    std::filesystem::path GetExecutablePath()
    {
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        return std::filesystem::path(buffer).parent_path();
    }
}

int main(int argc, char* argv[])
{
    const std::filesystem::path path = argc > 1 ? argv[1] : GetExecutablePath() / "plugins";

    if (!InitializeDirect3D()) {
        return -1;
    }

    if (!std::filesystem::exists(path)) {
        return 1;
    }

    for (const auto& modfile : std::filesystem::directory_iterator(path)) {
        if (modfile.is_regular_file() && (modfile.path().extension() == ".tpf" || modfile.path().extension() == ".zip")) {
            const auto mod_path = modfile.path();
            if (mod_path.extension() == ".zip" && mod_path.stem().string().ends_with("_")) {
                std::print("Skipping previous TpfConvert output: {}\n", mod_path.filename().string());
                continue;
            }
            else {
                std::print("Processing: {}\n", mod_path.filename().string());
            }

            const auto zip_filename = mod_path.parent_path() / (mod_path.stem().string() + "_.zip");
            if (std::filesystem::exists(zip_filename)) {
                std::print("{} was already processed\n", mod_path.filename().string());
                continue;
            }
            libzippp::ZipArchive zip_archive(zip_filename.string());

            ModfileLoader loader(mod_path);
            std::vector<std::pair<std::string, std::vector<uint8_t>>> data_entries;
            const auto entries = loader.GetContents();

            zip_archive.open(libzippp::ZipArchive::Write);

            for (const auto& entry : entries) {
                auto [dds_filename, dds_data] = SaveAsDDS(entry);
                if (!dds_filename.empty()) {
                    data_entries.emplace_back(dds_filename, std::move(dds_data));
                }
            }

            for (const auto& [filename, data] : data_entries) {
                const auto success = zip_archive.addData(filename, data.data(), data.size());
                if (!success) {
                    std::print(stderr, "Failed to add data to ZIP: {}\n", filename);
                }
            }

            zip_archive.close();
            std::print("Saved to ZIP: {}\n", zip_filename.string());
        }
    }

    CleanupDirect3D();

    return 0;
}
