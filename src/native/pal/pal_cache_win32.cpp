#include <chaos/pal/pal_cache.h>

#define NOMINMAX
#include <windows.h>

namespace chaos::il2cpp::pal {

void PalFlushInstructionCache(void* addr, size_t size) noexcept {
    ::FlushInstructionCache(::GetCurrentProcess(), addr, size);
}

}  // namespace chaos::il2cpp::pal
