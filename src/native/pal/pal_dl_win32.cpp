// pal_dl_win32.cpp — Win32 LoadLibrary/GetProcAddress/FreeLibrary implementation

#include <chaos/pal/pal_dl.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <cstring>

namespace chaos::il2cpp::pal {

PalLibraryPtr PalDlOpen(const char* path) noexcept {
    if (path == nullptr || path[0] == '\0') return nullptr;

    // If the path doesn't contain an extension, append ".dll".
    // This makes "kernel32" → "kernel32.dll" work automatically.
    const char* dot = std::strrchr(path, '.');
    if (dot == nullptr || std::strchr(dot, '\\') != nullptr || std::strchr(dot, '/') != nullptr) {
        // No extension found — try appending .dll.
        // Use a local buffer; note that LoadLibraryA also searches the
        // system search path, so even a bare name like "kernel32" works.
        std::string search(path);
        search += ".dll";
        return reinterpret_cast<PalLibraryPtr>(::LoadLibraryA(search.c_str()));
    }
    return reinterpret_cast<PalLibraryPtr>(::LoadLibraryA(path));
}

PalProcAddress PalDlSym(PalLibraryPtr handle, const char* symbol) noexcept {
    if (handle == nullptr || symbol == nullptr) return nullptr;
    return reinterpret_cast<PalProcAddress>(
        ::GetProcAddress(static_cast<HMODULE>(handle), symbol));
}

bool PalDlClose(PalLibraryPtr handle) noexcept {
    if (handle == nullptr) return false;
    return ::FreeLibrary(static_cast<HMODULE>(handle)) != FALSE;
}

const char* PalDlError() noexcept {
    static thread_local char buf[256];
    DWORD err = ::GetLastError();
    if (err == 0) return nullptr;
    ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                     nullptr, err, 0, buf, sizeof(buf), nullptr);
    // Strip trailing \r\n
    size_t len = std::strlen(buf);
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
        buf[--len] = '\0';
    }
    return buf;
}

const char* PalDlNormalisePath(const char* path) noexcept {
    if (path == nullptr) return nullptr;
    static thread_local std::string buf;
    buf.clear();
    buf.reserve(std::strlen(path));
    for (const char* p = path; *p != '\0'; ++p) {
        char c = *p;
        if (c == '/') c = '\\';           // Normalise to Windows backslash
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a'); // Fold case
        buf.push_back(c);
    }
    return buf.c_str();
}

}  // namespace chaos::il2cpp::pal
