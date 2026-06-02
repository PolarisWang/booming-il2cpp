#include <chaos/pal/pal_cache.h>

namespace chaos::il2cpp::pal {

void PalFlushInstructionCache(void* addr, size_t size) noexcept {
    // __builtin___clear_cache is available on GCC/Clang for all architectures.
    // On x86 it's a compile-time no-op (hardware I-cache coherency).
    // On ARM64 it emits the required ISB + DSB instructions.
    // On ARM64 Apple silicon it's also handled correctly by the builtin.
    __builtin___clear_cache(
        static_cast<char*>(addr),
        static_cast<char*>(addr) + size);
}

}  // namespace chaos::il2cpp::pal
