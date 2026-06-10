#include "Main.h"
#include "D3D9Hooks.h"

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>

import TextureClient;
import TextureFunction;
import ModfileLoader; // HashCheck::Use64BitCrc

// Hooked vtable slot indices (frozen D3D9 ABI). Device9Ex shares Device9's prefix.

namespace {

    // --- IDirect3D9 / IDirect3D9Ex --------------------------------------
    constexpr int kIDirect3D9_CreateDevice = 16;
    constexpr int kIDirect3D9Ex_CreateDeviceEx = 20;

    // --- IDirect3DDevice9 -----------------------------------------------
    constexpr int kDevice_Release = 2;
    constexpr int kDevice_CreateTexture = 23;
    constexpr int kDevice_CreateVolumeTexture = 24;
    constexpr int kDevice_CreateCubeTexture = 25;
    constexpr int kDevice_UpdateTexture = 31;
    constexpr int kDevice_BeginScene = 41;
    constexpr int kDevice_GetTexture = 64;
    constexpr int kDevice_SetTexture = 65;

    // --- IDirect3D*Texture9 (IUnknown layout) ---------------------------
    constexpr int kResource_Release = 2;

    void** GetVTable(void* com_object)
    {
        return *static_cast<void***>(com_object);
    }

    // ---- trampolines to the real implementations -----------------------
    using CreateDevice_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3D9*, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice9**);
    using CreateDeviceEx_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3D9Ex*, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, D3DDISPLAYMODEEX*, IDirect3DDevice9Ex**);
    using DeviceRelease_t = ULONG(STDMETHODCALLTYPE*)(IDirect3DDevice9*);
    using CreateTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9**, HANDLE*);
    using CreateVolumeTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, UINT, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DVolumeTexture9**, HANDLE*);
    using CreateCubeTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DCubeTexture9**, HANDLE*);
    using UpdateTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, IDirect3DBaseTexture9*, IDirect3DBaseTexture9*);
    using BeginScene_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*);
    using SetTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, DWORD, IDirect3DBaseTexture9*);
    using GetTexture_t = HRESULT(STDMETHODCALLTYPE*)(IDirect3DDevice9*, DWORD, IDirect3DBaseTexture9**);
    using ResourceRelease_t = ULONG(STDMETHODCALLTYPE*)(IUnknown*);

    CreateDevice_t o_CreateDevice = nullptr;
    CreateDeviceEx_t o_CreateDeviceEx = nullptr;
    DeviceRelease_t o_DeviceRelease = nullptr;
    CreateTexture_t o_CreateTexture = nullptr;
    CreateVolumeTexture_t o_CreateVolumeTexture = nullptr;
    CreateCubeTexture_t o_CreateCubeTexture = nullptr;
    UpdateTexture_t o_UpdateTexture = nullptr;
    BeginScene_t o_BeginScene = nullptr;
    SetTexture_t o_SetTexture = nullptr;
    GetTexture_t o_GetTexture = nullptr;
    ResourceRelease_t o_Tex2DRelease = nullptr;
    ResourceRelease_t o_VolumeRelease = nullptr;
    ResourceRelease_t o_CubeRelease = nullptr;

    // A vtable is shared by every instance of its class, so each is hooked once.
    bool g_d3d9_hooks_installed = false;
    bool g_device_hooks_installed = false;
    bool g_tex2d_release_installed = false;
    bool g_volume_release_installed = false;
    bool g_cube_release_installed = false;

    // One overwritten vtable entry, remembered so teardown can put the original back.
    struct SwappedSlot
    {
        void** slot;    // address of the vtable entry we overwrote
        void* detour;   // value we wrote (our hook); lets us tell if we're still on top
        void* original; // value that was there before (real func, or a lower hook's detour)
    };
    std::vector<SwappedSlot> g_swapped_slots;

    // Set when teardown begins; a detour still reached after this just calls the original.
    std::atomic<bool> g_unhooked{false};

    // device -> owning TextureClient
    std::mutex g_devices_mutex;
    std::unordered_map<IDirect3DDevice9*, TextureClient*> g_devices;

    TextureClient* ClientFor(IDirect3DDevice9* device)
    {
        std::lock_guard lk(g_devices_mutex);
        const auto it = g_devices.find(device);
        return it != g_devices.end() ? it->second : nullptr;
    }

    // Overwrite one vtable entry with our detour, saving the original for restore. This
    // writes a data slot (the vtable lives in .rdata), never the function's code, so it is
    // safe under the x86-on-ARM64 emulator (no self-modifying-code trap) and needs no
    // thread suspension: an aligned pointer store is atomic for any thread reading the slot.
    bool SwapSlotRaw(void** vtable, int index, void* detour, void** original)
    {
        if (vtable == nullptr) return false;
        void** slot = &vtable[index];
        DWORD old_protect = 0;
        if (!VirtualProtect(slot, sizeof(void*), PAGE_READWRITE, &old_protect)) {
            Warning("D3D9Hooks: VirtualProtect failed for slot %p\n", slot);
            return false;
        }
        *original = *slot; // remember the real function (or a lower engine's detour)
        *slot = detour;
        VirtualProtect(slot, sizeof(void*), old_protect, &old_protect);
        g_swapped_slots.push_back({slot, detour, *original});
        return true;
    }

    // Funnels the fn-ptr <-> void* casts through one spot so call sites stay /WX-clean.
    template <typename TDetour>
    bool Hook(void** vtable, int index, TDetour detour, void** original)
    {
        return SwapSlotRaw(vtable, index, reinterpret_cast<void*>(detour), original);
    }

    HRESULT STDMETHODCALLTYPE h_CreateDevice(IDirect3D9*, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice9**);
    HRESULT STDMETHODCALLTYPE h_CreateDeviceEx(IDirect3D9Ex*, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, D3DDISPLAYMODEEX*, IDirect3DDevice9Ex**);
    ULONG STDMETHODCALLTYPE h_DeviceRelease(IDirect3DDevice9*);
    HRESULT STDMETHODCALLTYPE h_CreateTexture(IDirect3DDevice9*, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DTexture9**, HANDLE*);
    HRESULT STDMETHODCALLTYPE h_CreateVolumeTexture(IDirect3DDevice9*, UINT, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DVolumeTexture9**, HANDLE*);
    HRESULT STDMETHODCALLTYPE h_CreateCubeTexture(IDirect3DDevice9*, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DCubeTexture9**, HANDLE*);
    HRESULT STDMETHODCALLTYPE h_UpdateTexture(IDirect3DDevice9*, IDirect3DBaseTexture9*, IDirect3DBaseTexture9*);
    HRESULT STDMETHODCALLTYPE h_BeginScene(IDirect3DDevice9*);
    HRESULT STDMETHODCALLTYPE h_SetTexture(IDirect3DDevice9*, DWORD, IDirect3DBaseTexture9*);
    HRESULT STDMETHODCALLTYPE h_GetTexture(IDirect3DDevice9*, DWORD, IDirect3DBaseTexture9**);
    ULONG STDMETHODCALLTYPE h_Tex2DRelease(IUnknown*);
    ULONG STDMETHODCALLTYPE h_VolumeRelease(IUnknown*);
    ULONG STDMETHODCALLTYPE h_CubeRelease(IUnknown*);

    void InstallDeviceHooks(IDirect3DDevice9* device)
    {
        if (g_device_hooks_installed) return;
        void** vt = GetVTable(device);
        Hook(vt, kDevice_Release, &h_DeviceRelease, reinterpret_cast<void**>(&o_DeviceRelease));
        Hook(vt, kDevice_CreateTexture, &h_CreateTexture, reinterpret_cast<void**>(&o_CreateTexture));
        Hook(vt, kDevice_CreateVolumeTexture, &h_CreateVolumeTexture, reinterpret_cast<void**>(&o_CreateVolumeTexture));
        Hook(vt, kDevice_CreateCubeTexture, &h_CreateCubeTexture, reinterpret_cast<void**>(&o_CreateCubeTexture));
        Hook(vt, kDevice_UpdateTexture, &h_UpdateTexture, reinterpret_cast<void**>(&o_UpdateTexture));
        Hook(vt, kDevice_BeginScene, &h_BeginScene, reinterpret_cast<void**>(&o_BeginScene));
        Hook(vt, kDevice_GetTexture, &h_GetTexture, reinterpret_cast<void**>(&o_GetTexture));
        Hook(vt, kDevice_SetTexture, &h_SetTexture, reinterpret_cast<void**>(&o_SetTexture));
        g_device_hooks_installed = true;
    }

    // A texture vtable is reachable only via an instance, so hook Release lazily per kind.
    void InstallTextureReleaseHook(void* sample_texture, TexType type)
    {
        void** vt = GetVTable(sample_texture);
        switch (type) {
            case TexType::Tex2D:
                if (!g_tex2d_release_installed && Hook(vt, kResource_Release, &h_Tex2DRelease, reinterpret_cast<void**>(&o_Tex2DRelease)))
                    g_tex2d_release_installed = true;
                break;
            case TexType::Volume:
                if (!g_volume_release_installed && Hook(vt, kResource_Release, &h_VolumeRelease, reinterpret_cast<void**>(&o_VolumeRelease)))
                    g_volume_release_installed = true;
                break;
            case TexType::Cube:
                if (!g_cube_release_installed && Hook(vt, kResource_Release, &h_CubeRelease, reinterpret_cast<void**>(&o_CubeRelease)))
                    g_cube_release_installed = true;
                break;
        }
    }

    void OnDeviceCreated(IDirect3DDevice9* device)
    {
        if (device == nullptr) return;

        TextureClient* client = nullptr;
        {
            // Lock spans check+install+create so racing callers can't double-register.
            std::lock_guard lk(g_devices_mutex);
            if (g_devices.contains(device)) return;
            InstallDeviceHooks(device);
            client = new TextureClient(device);
            g_devices.emplace(device, client);
        }
        client->Initialize();
    }

    HRESULT STDMETHODCALLTYPE h_CreateDevice(IDirect3D9* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                             D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice9** ppReturnedDeviceInterface)
    {
        const HRESULT hr = o_CreateDevice(self, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
        if (g_unhooked) return hr;
        if (SUCCEEDED(hr) && ppReturnedDeviceInterface && *ppReturnedDeviceInterface) {
            OnDeviceCreated(*ppReturnedDeviceInterface);
        }
        return hr;
    }

    HRESULT STDMETHODCALLTYPE h_CreateDeviceEx(IDirect3D9Ex* self, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
                                               D3DPRESENT_PARAMETERS* pPresentationParameters, D3DDISPLAYMODEEX* pFullscreenDisplayMode, IDirect3DDevice9Ex** ppReturnedDeviceInterface)
    {
        const HRESULT hr = o_CreateDeviceEx(self, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
        if (g_unhooked) return hr;
        if (SUCCEEDED(hr) && ppReturnedDeviceInterface && *ppReturnedDeviceInterface) {
            OnDeviceCreated(*ppReturnedDeviceInterface);
        }
        return hr;
    }

    ULONG STDMETHODCALLTYPE h_DeviceRelease(IDirect3DDevice9* self)
    {
        const ULONG count = o_DeviceRelease(self);
        if (g_unhooked) return count;
        if (count == 0) {
            TextureClient* client = nullptr;
            {
                std::lock_guard lk(g_devices_mutex);
                const auto it = g_devices.find(self);
                if (it != g_devices.end()) {
                    client = it->second;
                    g_devices.erase(it);
                }
            }
            delete client; // ~TextureClient drops all side-state
        }
        return count;
    }

    HRESULT STDMETHODCALLTYPE h_CreateTexture(IDirect3DDevice9* self, UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                              IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
    {
        const HRESULT hr = o_CreateTexture(self, Width, Height, Levels, Usage, Format, Pool, ppTexture, pSharedHandle);
        if (g_unhooked) return hr;
        if (SUCCEEDED(hr) && ppTexture && *ppTexture) {
            InstallTextureReleaseHook(*ppTexture, TexType::Tex2D);
            if (auto* client = ClientFor(self))
                client->OnCreateTexture(static_cast<IDirect3DBaseTexture9*>(*ppTexture), TexType::Tex2D);
        }
        return hr;
    }

    HRESULT STDMETHODCALLTYPE h_CreateVolumeTexture(IDirect3DDevice9* self, UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                                    IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
    {
        const HRESULT hr = o_CreateVolumeTexture(self, Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, pSharedHandle);
        if (g_unhooked) return hr;
        if (SUCCEEDED(hr) && ppVolumeTexture && *ppVolumeTexture) {
            InstallTextureReleaseHook(*ppVolumeTexture, TexType::Volume);
            if (auto* client = ClientFor(self))
                client->OnCreateTexture(static_cast<IDirect3DBaseTexture9*>(*ppVolumeTexture), TexType::Volume);
        }
        return hr;
    }

    HRESULT STDMETHODCALLTYPE h_CreateCubeTexture(IDirect3DDevice9* self, UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
                                                  IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle)
    {
        const HRESULT hr = o_CreateCubeTexture(self, EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, pSharedHandle);
        if (g_unhooked) return hr;
        if (SUCCEEDED(hr) && ppCubeTexture && *ppCubeTexture) {
            InstallTextureReleaseHook(*ppCubeTexture, TexType::Cube);
            if (auto* client = ClientFor(self))
                client->OnCreateTexture(static_cast<IDirect3DBaseTexture9*>(*ppCubeTexture), TexType::Cube);
        }
        return hr;
    }

    HRESULT STDMETHODCALLTYPE h_UpdateTexture(IDirect3DDevice9* self, IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture)
    {
        // Pass the real textures through, then re-evaluate mods on the changed destination.
        const HRESULT hr = o_UpdateTexture(self, pSourceTexture, pDestinationTexture);
        if (g_unhooked) return hr;
        if (SUCCEEDED(hr)) {
            if (auto* client = ClientFor(self))
                client->OnUpdateTexture(pSourceTexture, pDestinationTexture);
        }
        return hr;
    }

    HRESULT STDMETHODCALLTYPE h_BeginScene(IDirect3DDevice9* self)
    {
        if (g_unhooked) return o_BeginScene(self);
        if (auto* client = ClientFor(self))
            client->OnBeginScene();
        return o_BeginScene(self);
    }

    HRESULT STDMETHODCALLTYPE h_SetTexture(IDirect3DDevice9* self, DWORD Stage, IDirect3DBaseTexture9* pTexture)
    {
        if (g_unhooked) return o_SetTexture(self, Stage, pTexture);
        IDirect3DBaseTexture9* bind = pTexture;
        if (auto* client = ClientFor(self))
            bind = client->ResolveBinding(pTexture); // substitute the fake when modded
        return o_SetTexture(self, Stage, bind);
    }

    HRESULT STDMETHODCALLTYPE h_GetTexture(IDirect3DDevice9* self, DWORD Stage, IDirect3DBaseTexture9** ppTexture)
    {
        const HRESULT hr = o_GetTexture(self, Stage, ppTexture);
        if (g_unhooked) return hr;
        if (SUCCEEDED(hr) && ppTexture && *ppTexture) {
            if (auto* client = ClientFor(self)) {
                if (auto* original = client->ResolveOriginalFromFake(*ppTexture)) {
                    (*ppTexture)->Release(); // drop the fake reference the runtime handed back
                    original->AddRef();      // return the original the game set, with a matching ref
                    *ppTexture = original;
                }
            }
        }
        return hr;
    }

    void ReleaseTextureCleanup(IUnknown* self, ULONG count)
    {
        if (count != 0) return;
        if (auto* client = TextureClient::CurrentClient())
            client->OnReleaseTexture(reinterpret_cast<IDirect3DBaseTexture9*>(self));
    }

    ULONG STDMETHODCALLTYPE h_Tex2DRelease(IUnknown* self)
    {
        const ULONG count = o_Tex2DRelease(self);
        if (g_unhooked) return count;
        ReleaseTextureCleanup(self, count);
        return count;
    }

    ULONG STDMETHODCALLTYPE h_VolumeRelease(IUnknown* self)
    {
        const ULONG count = o_VolumeRelease(self);
        if (g_unhooked) return count;
        ReleaseTextureCleanup(self, count);
        return count;
    }

    ULONG STDMETHODCALLTYPE h_CubeRelease(IUnknown* self)
    {
        const ULONG count = o_CubeRelease(self);
        if (g_unhooked) return count;
        ReleaseTextureCleanup(self, count);
        return count;
    }
}

void InstallD3D9Hooks(IDirect3D9* d3d9, const bool is_ex)
{
    if (d3d9 == nullptr || g_d3d9_hooks_installed) return;
    g_unhooked = false; // re-arm in case a prior teardown left the flag set
    void** vt = GetVTable(d3d9);
    Hook(vt, kIDirect3D9_CreateDevice, &h_CreateDevice, reinterpret_cast<void**>(&o_CreateDevice));
    if (is_ex) {
        Hook(vt, kIDirect3D9Ex_CreateDeviceEx, &h_CreateDeviceEx, reinterpret_cast<void**>(&o_CreateDeviceEx));
    }
    g_d3d9_hooks_installed = true;
}

bool RegisterExistingDevice(IDirect3DDevice9* device)
{
    if (device == nullptr) return false;
    {
        std::lock_guard lk(g_devices_mutex);
        if (g_devices.contains(device)) return false; // already known
        if (!g_devices.empty()) return false;         // gMod supports a single device at a time
    }
    g_unhooked = false;      // re-arm in case a prior teardown left the flag set
    OnDeviceCreated(device); // installs device hooks + TextureClient (idempotent)
    return ClientFor(device) != nullptr;
}

void RemoveAllD3D9Hooks()
{
    // Signal first: a detour still reached before its slot is restored falls through.
    g_unhooked = true;

    // Restore in reverse install order. Only put the original back if our detour is still
    // the live entry, so we don't clobber a hook another engine layered on top of ours.
    for (auto it = g_swapped_slots.rbegin(); it != g_swapped_slots.rend(); ++it) {
        DWORD old_protect = 0;
        if (!VirtualProtect(it->slot, sizeof(void*), PAGE_READWRITE, &old_protect)) continue;
        if (*it->slot == it->detour) *it->slot = it->original;
        VirtualProtect(it->slot, sizeof(void*), old_protect, &old_protect);
    }
    g_swapped_slots.clear();

    g_d3d9_hooks_installed = false;
    g_device_hooks_installed = false;
    g_tex2d_release_installed = false;
    g_volume_release_installed = false;
    g_cube_release_installed = false;

    // Leave the o_* trampoline pointers intact so a surviving detour can still call through.
}

void DestroyAllTextureClients()
{
    // Collect under the lock, delete outside it (~TextureClient takes its own locks).
    std::vector<TextureClient*> clients;
    {
        std::lock_guard lk(g_devices_mutex);
        for (const auto& entry : g_devices) {
            clients.push_back(entry.second);
        }
        g_devices.clear();
    }
    for (auto* client : clients) {
        delete client;
    }
}

// All three read state->real directly; gMod never swaps the underlying resource.

namespace {

    HashTuple HashBits(const void* bits, const int size)
    {
        const auto data = static_cast<const char*>(bits);
        const auto crc32 = TextureFunction::get_crc32(data, size);
        const auto crc64 = HashCheck::Use64BitCrc() ? TextureFunction::get_crc64(data, size) : 0;
        return {crc32, crc64};
    }

    HashTuple HashTexture2D(const TexState* state)
    {
        const auto pTexture = static_cast<IDirect3DTexture9*>(state->real);
        IDirect3DDevice9* device = state->device;

        IDirect3DSurface9* pOffscreenSurface = nullptr;
        IDirect3DSurface9* pResolvedSurface = nullptr;
        D3DLOCKED_RECT d3dlr;
        D3DSURFACE_DESC desc;

        if (pTexture->GetLevelDesc(0, &desc) != D3D_OK) {
            Warning("GetTextureHash(2D) Failed: GetLevelDesc\n");
            return {};
        }

        if (desc.Pool == D3DPOOL_DEFAULT) {
            IDirect3DSurface9* pSurfaceLevel_orig = nullptr;
            if (pTexture->GetSurfaceLevel(0, &pSurfaceLevel_orig) != D3D_OK) {
                Warning("GetTextureHash(2D) Failed: GetSurfaceLevel 1 (D3DPOOL_DEFAULT)\n");
                return {};
            }

            // The surface GetRenderTargetData reads from: either the level itself, or a
            // non-multisampled resolve of it. Non-owning; pSurfaceLevel_orig and
            // pResolvedSurface each own a ref and are released exactly once below.
            IDirect3DSurface9* pCopySource = pSurfaceLevel_orig;

            if (desc.MultiSampleType != D3DMULTISAMPLE_NONE) {
                if (D3D_OK != device->CreateRenderTarget(desc.Width, desc.Height, desc.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &pResolvedSurface, nullptr)) {
                    pSurfaceLevel_orig->Release();
                    Warning("GetTextureHash(2D) Failed: CreateRenderTarget (D3DPOOL_DEFAULT)\n");
                    return {};
                }
                if (D3D_OK != device->StretchRect(pSurfaceLevel_orig, nullptr, pResolvedSurface, nullptr, D3DTEXF_NONE)) {
                    pResolvedSurface->Release();
                    pResolvedSurface = nullptr;
                    pSurfaceLevel_orig->Release();
                    Warning("GetTextureHash(2D) Failed: StretchRect (D3DPOOL_DEFAULT)\n");
                    return {};
                }
                pCopySource = pResolvedSurface;
            }

            if (D3D_OK != device->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &pOffscreenSurface, nullptr)) {
                if (pResolvedSurface != nullptr) pResolvedSurface->Release();
                pSurfaceLevel_orig->Release();
                Warning("GetTextureHash(2D) Failed: CreateOffscreenPlainSurface (D3DPOOL_DEFAULT)\n");
                return {};
            }

            if (D3D_OK != device->GetRenderTargetData(pCopySource, pOffscreenSurface)) {
                pOffscreenSurface->Release();
                if (pResolvedSurface != nullptr) pResolvedSurface->Release();
                pSurfaceLevel_orig->Release();
                Warning("GetTextureHash(2D) Failed: GetRenderTargetData (D3DPOOL_DEFAULT)\n");
                return {};
            }

            // Copy is done; both source surfaces are finished with. Null pResolvedSurface
            // so the shared cleanup below treats this as the pOffscreenSurface case.
            if (pResolvedSurface != nullptr) {
                pResolvedSurface->Release();
                pResolvedSurface = nullptr;
            }
            pSurfaceLevel_orig->Release();

            if (pOffscreenSurface->LockRect(&d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
                pOffscreenSurface->Release();
                Warning("GetTextureHash(2D) Failed: LockRect (D3DPOOL_DEFAULT)\n");
                return {};
            }
        }
        else if (pTexture->LockRect(0, &d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
            Warning("GetTextureHash(2D) Failed: LockRect 1\n");
            if (pTexture->GetSurfaceLevel(0, &pResolvedSurface) != D3D_OK) {
                Warning("GetTextureHash(2D) Failed: GetSurfaceLevel\n");
                return {};
            }
            if (pResolvedSurface->LockRect(&d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
                pResolvedSurface->Release();
                Warning("GetTextureHash(2D) Failed: LockRect 2\n");
                return {};
            }
        }

        const int size = (TextureFunction::GetBitsFromFormat(desc.Format) * desc.Width * desc.Height) / 8;
        const auto hash = HashBits(d3dlr.pBits, size);

        if (pOffscreenSurface != nullptr) {
            pOffscreenSurface->UnlockRect();
            pOffscreenSurface->Release();
        }
        else if (pResolvedSurface != nullptr) {
            pResolvedSurface->UnlockRect();
            pResolvedSurface->Release();
        }
        else {
            pTexture->UnlockRect(0);
        }

        return hash;
    }

    HashTuple HashVolume(const TexState* state)
    {
        const auto pTexture = static_cast<IDirect3DVolumeTexture9*>(state->real);

        IDirect3DVolume9* pResolvedSurface = nullptr;
        D3DLOCKED_BOX d3dlr;
        D3DVOLUME_DESC desc;

        if (pTexture->GetLevelDesc(0, &desc) != D3D_OK) {
            Warning("GetTextureHash(Volume) Failed: GetLevelDesc\n");
            return {};
        }

        if (pTexture->LockBox(0, &d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
            if (pTexture->GetVolumeLevel(0, &pResolvedSurface) != D3D_OK) {
                Warning("GetTextureHash(Volume) Failed: GetVolumeLevel\n");
                return {};
            }
            if (pResolvedSurface->LockBox(&d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
                pResolvedSurface->Release();
                Warning("GetTextureHash(Volume) Failed: LockBox\n");
                return {};
            }
        }

        const int size = (TextureFunction::GetBitsFromFormat(desc.Format) * desc.Width * desc.Height * desc.Depth) / 8;
        const auto hash = HashBits(d3dlr.pBits, size);

        if (pResolvedSurface != nullptr) {
            pResolvedSurface->UnlockBox();
            pResolvedSurface->Release();
        }
        else {
            pTexture->UnlockBox(0);
        }

        return hash;
    }

    HashTuple HashCube(const TexState* state)
    {
        const auto pTexture = static_cast<IDirect3DCubeTexture9*>(state->real);

        IDirect3DSurface9* pResolvedSurface = nullptr;
        D3DLOCKED_RECT d3dlr;
        D3DSURFACE_DESC desc;

        if (pTexture->GetLevelDesc(0, &desc) != D3D_OK) {
            Warning("GetTextureHash(Cube) Failed: GetLevelDesc\n");
            return {};
        }

        if (pTexture->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0, &d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
            if (pTexture->GetCubeMapSurface(D3DCUBEMAP_FACE_POSITIVE_X, 0, &pResolvedSurface) != D3D_OK) {
                Warning("GetTextureHash(Cube) Failed: GetCubeMapSurface\n");
                return {};
            }
            if (pResolvedSurface->LockRect(&d3dlr, nullptr, D3DLOCK_READONLY) != D3D_OK) {
                pResolvedSurface->Release();
                Warning("GetTextureHash(Cube) Failed: LockRect\n");
                return {};
            }
        }

        const int size = (TextureFunction::GetBitsFromFormat(desc.Format) * desc.Width * desc.Height) / 8;
        const auto hash = HashBits(d3dlr.pBits, size);

        if (pResolvedSurface != nullptr) {
            pResolvedSurface->UnlockRect();
            pResolvedSurface->Release();
        }
        else {
            pTexture->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, 0);
        }

        return hash;
    }
}

HashTuple GetTextureHash(const TexState* state)
{
    if (state == nullptr || state->real == nullptr || state->isFake) return {};
    switch (state->type) {
        case TexType::Tex2D: return HashTexture2D(state);
        case TexType::Volume: return HashVolume(state);
        case TexType::Cube: return HashCube(state);
    }
    return {};
}
