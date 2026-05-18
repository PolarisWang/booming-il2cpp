// ── Exception delivery for generated C++ code ──────────────────────────
//
// Three-path architecture:
//
// CHAOS_IL2CPP_EH_CPP_THROW (macOS/Linux):
//   chaos_raise_exception(obj) → throw chaos_managed_exception{obj}
//   Normal C++ exception propagation with full RAII unwind.
//   Requires LSDA/.ARM.extab tables.
//
// CHAOS_IL2CPP_EH_SETJMP (iOS/Android):
//   chaos_raise_exception(obj) → TLS save + longjmp
//   Zero LSDA/.ARM.extab tables, ~5x faster throw.
//   Codegen emits manual finally dispatch instead of RAII.
//
// CHAOS_IL2CPP_EH_WIN32_SEH (Windows):
//   chaos_raise_exception(obj) → TLS save + RaiseException
//   Uses __try/__except/__finally — avoids C++ EH table walk overhead.
//
// TLS exception object (SETJMP and WIN32_SEH modes):
//   The exception pointer is stored in g_chaos_exception_obj before
//   delivery to avoid 64-bit truncation.
//
// Thread safety: all TLS — no contention.

#ifndef CHAOS_IL2CPP_EXCEPTION_JMP_H_
#define CHAOS_IL2CPP_EXCEPTION_JMP_H_

#include <chaos/config.h>
#include <chaos/native_types.h>

#include <cstdlib>

#if defined(CHAOS_IL2CPP_EH_SETJMP)
#  include <csetjmp>
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#endif

// ── SETJMP mode: TLS jmp_buf stack + longjmp delivery ────────────────────
#if defined(CHAOS_IL2CPP_EH_SETJMP)

namespace chaos::il2cpp::runtime_core {

static constexpr int kMaxNestedTry = 16;

// TLS jmp_buf stack. push_exception_jmp_buf/pop_exception_jmp_buf manage it.
extern thread_local jmp_buf g_chaos_exception_jmp_stack[kMaxNestedTry];
extern thread_local int g_chaos_exception_jmp_depth;

// TLS current exception object pointer.
// Must be volatile because longjmp may clobber register values.
extern thread_local void* volatile g_chaos_exception_obj;

inline jmp_buf* push_exception_jmp_buf() noexcept {
    if (g_chaos_exception_jmp_depth >= kMaxNestedTry) {
        std::abort();
    }
    return &g_chaos_exception_jmp_stack[g_chaos_exception_jmp_depth++];
}

inline void pop_exception_jmp_buf() noexcept {
    if (g_chaos_exception_jmp_depth > 0) {
        g_chaos_exception_jmp_depth--;
    }
}

[[noreturn]] inline void chaos_raise_exception(CHAOS_IL2CPP_INTPTR obj) noexcept {
    g_chaos_exception_obj = reinterpret_cast<void*>(obj);
    if (g_chaos_exception_jmp_depth > 0) {
        longjmp(g_chaos_exception_jmp_stack[g_chaos_exception_jmp_depth - 1], 1);
    }
    std::abort();
}

}  // namespace chaos::il2cpp::runtime_core

// ── WIN32_SEH mode: RaiseException + __except filter delivery ──────────────
#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)

namespace chaos::il2cpp::runtime_core {

// TLS current exception object pointer (shared with SETJMP mode).
extern thread_local void* volatile g_chaos_exception_obj;

static constexpr DWORD kChaosManagedExceptionCode = 0xE0000001;

// ── SEH filter helpers ────────────────────────────────────
// NOTE: GetExceptionInformation() is an intrinsic valid ONLY within an
// __except(filter-expression) at compile time.  MSVC traces the call
// chain and rejects intermediate function calls.  Use a macro to
// expand directly in filter context.
//
// For typed catches, the codegen emits per-catch SEH filter lambdas
// directly in the generated .cpp file where chaos_is_type_compatible()
// is already defined.  This header provides only the catch-all macro.
#define CHAOS_SEH_FILTER_ALL()                                              \
    (GetExceptionCode() == kChaosManagedExceptionCode ?                     \
        (g_chaos_exception_obj = reinterpret_cast<void*>(                   \
            GetExceptionInformation()->ExceptionRecord->ExceptionInformation[0]), \
         EXCEPTION_EXECUTE_HANDLER) :                                       \
        EXCEPTION_CONTINUE_SEARCH)

// ── raise ─────────────────────────────────────────────────
[[noreturn]] inline void chaos_raise_exception(CHAOS_IL2CPP_INTPTR obj) noexcept {
    g_chaos_exception_obj = reinterpret_cast<void*>(obj);
    RaiseException(kChaosManagedExceptionCode, 0, 1,
        reinterpret_cast<const ULONG_PTR*>(&obj));
    std::abort();
}

}  // namespace chaos::il2cpp::runtime_core

// ── CPP_THROW mode: C++ throw delivery ───────────────────────────────────
#elif defined(CHAOS_IL2CPP_EH_CPP_THROW)

#  include "generated_code_compat.h"  // for chaos_managed_exception

namespace chaos::il2cpp::runtime_core {

// CPP_THROW mode: throw is not noexcept, but we are [[noreturn]]
[[noreturn]] inline void chaos_raise_exception(CHAOS_IL2CPP_INTPTR obj) {
    throw chaos_managed_exception{obj};
}

}  // namespace chaos::il2cpp::runtime_core

// ── Default (C++ try/catch) mode: same C++ throw delivery ─────────────
// Used when no CHAOS_IL2CPP_EH_* macro is defined (the codegen's first
// preprocessor branch: !SETJMP && !WIN32_SEH).
#else

#  include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {

[[noreturn]] inline void chaos_raise_exception(CHAOS_IL2CPP_INTPTR obj) {
    throw chaos_managed_exception{obj};
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_EH_SETJMP / CHAOS_IL2CPP_EH_WIN32_SEH / CHAOS_IL2CPP_EH_CPP_THROW / default

#endif  // CHAOS_IL2CPP_EXCEPTION_JMP_H_