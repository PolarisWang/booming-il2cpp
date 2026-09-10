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
// Holds the EXCEPTION_POINTERS captured in the __except filter (where
// GetExceptionInformation() is legal) for use in the handler body (Step 4.3).
thread_local const EXCEPTION_POINTERS* g_win_exc_info = nullptr;
}

namespace chaos::il2cpp::pal {

bool PalTryCallNoExcept(uint64_t (*fn)(uint64_t, uint64_t, uint64_t, uint64_t,
                                        uint64_t, uint64_t, uint64_t, uint64_t),
                         uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3,
                         uint64_t a4, uint64_t a5, uint64_t a6, uint64_t a7,
                         uint64_t& out_result,
                         uint64_t* out_exception_object) noexcept
{
    __try {
        out_result = fn(a0, a1, a2, a3, a4, a5, a6, a7);
        return false;
    } __except(
        (g_win_exc_info = GetExceptionInformation(),
         EXCEPTION_EXECUTE_HANDLER)) {
        // 4.3: a C++ chaos_managed_exception (0xE06D7363) thrown by the callee
        // carries its managed exception object.  On MSVC x64, ExceptionInformation[1]
        // is the address of the thrown C++ object; chaos_managed_exception is a
        // single-field struct, so object_value sits at offset 0.  Surface it to the
        // caller so interpreter fault semantics (Message/Stack) survive the boundary.
        if (out_exception_object != nullptr &&
            GetExceptionCode() == 0xE06D7363 &&
            g_win_exc_info != nullptr &&
            g_win_exc_info->ExceptionRecord->NumberParameters >= 2 &&
            g_win_exc_info->ExceptionRecord->ExceptionInformation[1] != 0) {
            *out_exception_object =
                *reinterpret_cast<const uint64_t*>(
                    g_win_exc_info->ExceptionRecord->ExceptionInformation[1]);
        }
        return true;
    }
}

}  // namespace chaos::il2cpp::pal
