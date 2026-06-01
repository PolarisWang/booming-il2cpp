// pal_dl_posix.cpp — POSIX dlopen/dlsym/dlclose implementation

#include <chaos/pal/pal_dl.h>

#include <dlfcn.h>
#include <string>
#include <cstring>

namespace chaos::il2cpp::pal {

PalLibraryPtr PalDlOpen(const char* path) noexcept {
    if (path == nullptr || path[0] == '\0') return nullptr;
    return reinterpret_cast<PalLibraryPtr>(::dlopen(path, RTLD_LAZY | RTLD_LOCAL));
}

PalProcAddress PalDlSym(PalLibraryPtr handle, const char* symbol) noexcept {
    if (handle == nullptr || symbol == nullptr) return nullptr;
    return ::dlsym(handle, symbol);
}

bool PalDlClose(PalLibraryPtr handle) noexcept {
    if (handle == nullptr) return false;
    return ::dlclose(handle) == 0;
}

const char* PalDlError() noexcept {
    return ::dlerror();
}

const char* PalDlNormalisePath(const char* path) noexcept {
    if (path == nullptr) return nullptr;
    static thread_local std::string buf;
    buf.clear();
    buf.reserve(std::strlen(path));
    for (const char* p = path; *p != '\0'; ++p) {
        char c = *p;
        if (c == '\\') c = '/';           // Normalise to POSIX forward slash
        buf.push_back(c);
    }
    return buf.c_str();
}

}  // namespace chaos::il2cpp::pal
