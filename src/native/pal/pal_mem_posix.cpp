// pal_mem_posix.cpp — POSIX mmap/munmap/mprotect implementation

#include <chaos/pal/pal_mem.h>

#include <sys/mman.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace chaos::il2cpp::pal {

void* PalVirtualAlloc(size_t size) noexcept {
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (ptr == MAP_FAILED) ? nullptr : ptr;
}

bool PalVirtualFree(void* ptr, size_t size) noexcept {
    return ::munmap(ptr, size) == 0;
}

bool PalVirtualProtect(void* ptr, size_t size, PalMemoryProtection prot) noexcept {
    int posix_prot = PROT_NONE;
    if (prot & kPalMemRead)  posix_prot |= PROT_READ;
    if (prot & kPalMemWrite) posix_prot |= PROT_WRITE;
    if (prot & kPalMemExec)  posix_prot |= PROT_EXEC;
    return ::mprotect(ptr, size, posix_prot) == 0;
}

bool PalVirtualCommit(void* /*ptr*/, size_t /*size*/) noexcept {
    // POSIX mmap already commits on allocation; this is a no-op.
    return true;
}

bool PalVirtualDecommit(void* ptr, size_t size) noexcept {
    // On POSIX we can madvise to hint that pages are not needed.
    // This is advisory only; the mapping remains valid.
    return ::madvise(ptr, size, MADV_DONTNEED) == 0;
}

size_t PalGetPageSize() noexcept {
    return static_cast<size_t>(::sysconf(_SC_PAGESIZE));
}

size_t PalGetLargePageSize() noexcept {
    // Linux: try /proc/meminfo for Hugepagesize, or return 2MB default.
    // macOS: no transparent hugepages.
    // Return 2MB as a reasonable default; actual availability depends on
    // kernel configuration and MAP_HUGETLB support.
    return 2 * 1024 * 1024;  // 2MB default
}

void* PalVirtualAllocLarge(size_t size) noexcept {
#ifdef MAP_HUGETLB
    void* ptr = ::mmap(nullptr, size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (ptr != MAP_FAILED) return ptr;
#endif
    return PalVirtualAlloc(size);
}

void PalGetMemoryStatus(PalMemoryStatus& out) noexcept {
    out.total_phys = -1;  // -1 indicates parsing failure / unknown
    out.avail_phys = -1;

    FILE* f = ::fopen("/proc/meminfo", "r");
    if (!f) return;

    char line[256];
    while (::fgets(line, sizeof(line), f)) {
        if (line[0] == 'M' && line[1] == 'e' && line[2] == 'm') {
            if (line[3] == 'T' && line[4] == 'o' && line[5] == 't' && line[6] == 'a' && line[7] == 'l') {
                // "MemTotal:      12345678 kB"
                char* val = line;
                while (*val && *val != ':') ++val;
                if (*val == ':') {
                    long kb = std::atol(val + 1);
                    out.total_phys = static_cast<int64_t>(kb) * 1024;
                }
            } else if (line[3] == 'A' && line[4] == 'v' && line[5] == 'a' && line[6] == 'i' && line[7] == 'l') {
                // "MemAvailable:  12345678 kB"
                char* val = line;
                while (*val && *val != ':') ++val;
                if (*val == ':') {
                    long kb = std::atol(val + 1);
                    out.avail_phys = static_cast<int64_t>(kb) * 1024;
                }
            }
        }
    }
    ::fclose(f);
}

size_t PalVirtualQuery(const void* /*addr*/, void* /*out_info*/, size_t /*info_size*/) noexcept {
    return 0;  // Not supported on POSIX — mmap'd regions are always valid.
}

bool PalVirtualAllocIsValid(const void* /*ptr*/) noexcept {
    return true;  // POSIX: munmap is safe on any mapped address.
}

bool PalCanJit() noexcept {
#if defined(CHAOS_IL2CPP_TARGET_PLATFORM_IOS)
    return false;  // iOS W^X enforcement: no executable memory allocation.
#else
    return true;
#endif
}

}  // namespace chaos::il2cpp::pal
