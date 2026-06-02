// pal_eh_posix.cpp — POSIX native call wrapper (no SEH, passthrough)

#include <chaos/pal/pal_eh.h>

namespace chaos::il2cpp::pal {

bool PalTryCallNoExcept(uint64_t (*fn)(uint64_t, uint64_t, uint64_t, uint64_t,
                                        uint64_t, uint64_t, uint64_t, uint64_t),
                         uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
                         uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7,
                         uint64_t& out_result) noexcept
{
    out_result = fn(a0, a1, a2, a3, a4, a5, a6, a7);
    return false;
}

}  // namespace chaos::il2cpp::pal
