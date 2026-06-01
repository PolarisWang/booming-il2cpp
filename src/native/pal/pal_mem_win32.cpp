// pal_mem_win32.cpp — Win32 VirtualAlloc/VirtualFree/VirtualProtect implementation

#include <chaos/pal/pal_mem.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace chaos::il2cpp::pal {

void* PalVirtualAlloc(size_t size) noexcept {
    return ::VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

bool PalVirtualFree(void* ptr, size_t /*size*/) noexcept {
    return ::VirtualFree(ptr, 0, MEM_RELEASE) != FALSE;
}

bool PalVirtualProtect(void* ptr, size_t size, PalMemoryProtection prot) noexcept {
    DWORD win_prot = PAGE_NOACCESS;
    if (prot == kPalMemReadWrite)        win_prot = PAGE_READWRITE;
    else if (prot == kPalMemReadExec)    win_prot = PAGE_EXECUTE_READ;
    else if (prot == kPalMemReadWriteExec) win_prot = PAGE_EXECUTE_READWRITE;
    else if (prot == kPalMemRead)        win_prot = PAGE_READONLY;
    DWORD old;
    return ::VirtualProtect(ptr, size, win_prot, &old) != FALSE;
}

bool PalVirtualCommit(void* ptr, size_t size) noexcept {
    return ::VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != nullptr;
}

bool PalVirtualDecommit(void* ptr, size_t size) noexcept {
    return ::VirtualFree(ptr, size, MEM_DECOMMIT) != FALSE;
}

size_t PalGetPageSize() noexcept {
    SYSTEM_INFO si;
    ::GetSystemInfo(&si);
    return si.dwPageSize;
}

size_t PalGetLargePageSize() noexcept {
    return ::GetLargePageMinimum();
}

void* PalVirtualAllocLarge(size_t size) noexcept {
    return ::VirtualAlloc(nullptr, size,
                          MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES,
                          PAGE_READWRITE);
}

void PalGetMemoryStatus(PalMemoryStatus& out) noexcept {
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(ms);
    if (::GlobalMemoryStatusEx(&ms)) {
        out.total_phys = static_cast<int64_t>(ms.ullTotalPhys);
        out.avail_phys = static_cast<int64_t>(ms.ullAvailPhys);
    } else {
        out.total_phys = 0;
        out.avail_phys = 0;
    }
}

}  // namespace chaos::il2cpp::pal
