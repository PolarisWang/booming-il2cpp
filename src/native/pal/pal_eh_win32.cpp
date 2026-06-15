// pal_eh_win32.cpp — SEH-safe native call wrapper (Windows)

#include <chaos/pal/pal_eh.h>

#define NOMINMAX
#include <windows.h>

// TLS state variables — declared extern in pal_eh.h, used on POSIX for
// sigsetjmp/siglongjmp recovery. Defined here on Windows (as empty stubs)
// so that the PalTryResetState() inline function from pal_eh.h compiles
// and links correctly in all translation units.
namespace chaos::il2cpp::pal {
thread_local bool g_pal_try_active = false;
thread_local int g_pal_try_reentry = 0;
}

namespace chaos::il2cpp::pal {

bool PalTryCallNoExcept(uint64_t (*fn)(uint64_t, uint64_t, uint64_t, uint64_t,
                                        uint64_t, uint64_t, uint64_t, uint64_t),
                         uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
                         uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7,
                         uint64_t& out_result) noexcept
{
    __try {
        out_result = fn(a0, a1, a2, a3, a4, a5, a6, a7);
        return false;
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        return true;
    }
}

}  // namespace chaos::il2cpp::pal
