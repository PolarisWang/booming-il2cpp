// ── JIT Data Loader — implementation ─────────────────────────────────────
// Loads .jdata binary file into memory via platform-native file mapping.
//
// ABI export: extern "C" for C-language linkage from managed/NativeAot code

#include <chaos/log.h>
#include "jit_data_loader.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace {

// Registry for the last .jdata mapping, used by ChaosJitDataUnload on POSIX.
// Only supports one active mapping at a time, which is sufficient for the
// single .jdata file per process use case.
#if !defined(_WIN32)
struct JitDataMapping {
    void* ptr = nullptr;
    size_t size = 0;
};
JitDataMapping g_last_mapping;
#endif

} // anonymous namespace

extern "C" void* ChaosJitDataLoad(const char* filepath,
                                  uint64_t* out_size) noexcept {
    if (!filepath || !filepath[0]) {
        CHAOS_IL2CPP_LOG_ERROR_M("jit_data",
            "ChaosJitDataLoad: null or empty filepath");
        return nullptr;
    }

#if defined(_WIN32)
    HANDLE hFile = CreateFileA(
        filepath,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataLoad: could not open file '{}' (error {})",
            filepath, GetLastError());
        return nullptr;
    }

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataLoad: GetFileSizeEx failed for '{}'", filepath);
        return nullptr;
    }

    if (fileSize.QuadPart == 0) {
        CloseHandle(hFile);
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataLoad: file '{}' is empty", filepath);
        return nullptr;
    }

    HANDLE hMapping = CreateFileMappingW(
        hFile,
        nullptr,
        PAGE_READONLY,
        0, 0,
        nullptr);
    if (!hMapping) {
        CloseHandle(hFile);
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataLoad: CreateFileMapping failed for '{}' (error {})",
            filepath, GetLastError());
        return nullptr;
    }

    void* data = MapViewOfFile(
        hMapping,
        FILE_MAP_READ,
        0, 0, 0);
    if (!data) {
        CloseHandle(hMapping);
        CloseHandle(hFile);
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataLoad: MapViewOfFile failed for '{}' (error {})",
            filepath, GetLastError());
        return nullptr;
    }

    // MapViewOfFile keeps the mapping alive even after CloseHandle.
    CloseHandle(hMapping);
    CloseHandle(hFile);

    if (out_size) {
        *out_size = static_cast<uint64_t>(fileSize.QuadPart);
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("jit_data",
        "ChaosJitDataLoad: mapped '{}' ({} bytes) at {}",
        filepath, fileSize.QuadPart, static_cast<void*>(data));

    return data;

#else  // POSIX (Linux, macOS)

    int fd = ::open(filepath, O_RDONLY);
    if (fd < 0) {
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataLoad: could not open file '{}'", filepath);
        return nullptr;
    }

    struct stat st;
    if (::fstat(fd, &st) < 0) {
        ::close(fd);
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataLoad: fstat failed for '{}'", filepath);
        return nullptr;
    }

    if (st.st_size == 0) {
        ::close(fd);
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataLoad: file '{}' is empty", filepath);
        return nullptr;
    }

    void* data = ::mmap(nullptr, static_cast<size_t>(st.st_size),
                        PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        ::close(fd);
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataLoad: mmap failed for '{}'", filepath);
        return nullptr;
    }

    ::close(fd);  // fd no longer needed after mmap

    // Store mapping info for ChaosJitDataUnload.
    g_last_mapping.ptr = data;
    g_last_mapping.size = static_cast<size_t>(st.st_size);

    if (out_size) {
        *out_size = static_cast<uint64_t>(st.st_size);
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("jit_data",
        "ChaosJitDataLoad: mmap'd '{}' ({} bytes) at {}",
        filepath, st.st_size, data);

    return data;
#endif
}

extern "C" void ChaosJitDataUnload(void* data) noexcept {
    if (!data) return;

#if defined(_WIN32)
    if (!UnmapViewOfFile(data)) {
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataUnload: UnmapViewOfFile failed (error {})",
            GetLastError());
    }
#else
    if (data == g_last_mapping.ptr && g_last_mapping.size > 0) {
        ::munmap(data, g_last_mapping.size);
        g_last_mapping.ptr = nullptr;
        g_last_mapping.size = 0;
    } else {
        CHAOS_IL2CPP_LOG_WARN_M("jit_data",
            "ChaosJitDataUnload: pointer {} not recognized (or already unmapped)",
            data);
    }
#endif
}
