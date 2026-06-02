// pal_encoding_win32.cpp — Win32 string encoding implementation

#include <chaos/pal/pal_encoding.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace chaos::il2cpp::pal {

int32_t PalUtf8ToUtf16(const uint8_t* utf8, int32_t utf8_len,
                       uint16_t* utf16, int32_t utf16_capacity) noexcept {
    int needed = ::MultiByteToWideChar(CP_UTF8, 0,
        reinterpret_cast<const char*>(utf8), utf8_len,
        nullptr, 0);
    if (needed <= 0) return -1;
    if (utf16 == nullptr || utf16_capacity <= 0) return needed;
    int written = ::MultiByteToWideChar(CP_UTF8, 0,
        reinterpret_cast<const char*>(utf8), utf8_len,
        reinterpret_cast<wchar_t*>(utf16), utf16_capacity);
    return written > 0 ? written : -1;
}

int32_t PalUtf16ToUtf8(const uint16_t* utf16, int32_t utf16_len,
                       uint8_t* utf8, int32_t utf8_capacity) noexcept {
    int needed = ::WideCharToMultiByte(CP_UTF8, 0,
        reinterpret_cast<const wchar_t*>(utf16), utf16_len,
        nullptr, 0, nullptr, nullptr);
    if (needed <= 0) return -1;
    if (utf8 == nullptr || utf8_capacity <= 0) return needed;
    int written = ::WideCharToMultiByte(CP_UTF8, 0,
        reinterpret_cast<const wchar_t*>(utf16), utf16_len,
        reinterpret_cast<char*>(utf8), utf8_capacity,
        nullptr, nullptr);
    return written > 0 ? written : -1;
}

bool PalCloseHandle(intptr_t handle) noexcept {
    if (handle == 0) return true;
    return ::CloseHandle(reinterpret_cast<HANDLE>(handle)) != FALSE;
}

}  // namespace chaos::il2cpp::pal
