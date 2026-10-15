#pragma once

#include <vector>

namespace utils {
    template <typename T>
    void erase_first(std::vector<T>& vec, const T& elem)
    {
        const auto found = std::ranges::find(vec, elem);
        if (found != std::ranges::end(vec)) {
            vec.erase(found);
        }
    }

    inline std::wstring utf8_to_wstring(const std::string& utf8str)
    {
        if (utf8str.empty()) return {};

        // Calculate the number of wide characters needed for the conversion
        const int count = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, nullptr, 0);
        if (count == 0) throw std::runtime_error("Failed to convert UTF-8 to UTF-16");

        std::wstring wstr(count, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, wstr.data(), count);

        return wstr;
    }

    // True when this x86/x64 process is running under an ARM64 emulator (Windows-on-ARM's
    // xtajit, or Wine on FEX-Emu/box64 on ARM64 Linux). Patching a method on a device the
    // game is already calling races that emulator's self-modifying-code handling, so callers
    // must not hot-attach to an existing device in that case.
    inline bool IsRunningUnderArm64Emulation()
    {
        using IsWow64Process2_t = BOOL(WINAPI*)(HANDLE, USHORT*, USHORT*);
        const auto IsWow64Process2_fn = reinterpret_cast<IsWow64Process2_t>(
            GetProcAddress(GetModuleHandleA("kernel32.dll"), "IsWow64Process2"));
        if (!IsWow64Process2_fn) return false;

        USHORT process_machine = IMAGE_FILE_MACHINE_UNKNOWN;
        USHORT native_machine = IMAGE_FILE_MACHINE_UNKNOWN;
        if (!IsWow64Process2_fn(GetCurrentProcess(), &process_machine, &native_machine))
            return false;

        return native_machine == IMAGE_FILE_MACHINE_ARM64 && process_machine != IMAGE_FILE_MACHINE_UNKNOWN;
    }
}
