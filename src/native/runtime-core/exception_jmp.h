// ── Exception delivery for generated C++ code ──────────────────────────
//
// Dual-path architecture:
//
// CHAOS_IL2CPP_EH_CPP_THROW (default on desktop):
//   chaos_raise_exception(obj) → throw chaos_managed_exception{obj}
//   Normal C++ exception propagation with full RAII unwind.
//   Requires LSDA/.ARM.extab tables.
//
// CHAOS_IL2CPP_EH_SETJMP (default on iOS/Android):
//   chaos_raise_exception(obj) → TLS save + longjmp
//   Zero LSDA/.ARM.extab tables, ~5x faster throw.
//   Codegen emits manual finally dispatch instead of RAII.
//
// Both modes share the same API: codegen and runtime helpers always call
// chaos_raise_exception(obj) to raise a managed exception.
//
// TLS exception object (SETJMP mode only):
//   The exception pointer is stored in g_chaos_exception_obj before longjmp
//   to avoid 64-bit truncation (longjmp's int return value is insufficient).
//
// Thread safety: all TLS — no contention.

#ifndef CHAOS_IL2CPP_EXCEPTION_JMP_H_
#define CHAOS_IL2CPP_EXCEPTION_JMP_H_

#include <chaos/config.h>
#include <chaos/native_types.h>

#include <cstdlib>

#if defined(CHAOS_IL2CPP_EH_SETJMP)
#  include <csetjmp>
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

// ── CPP_THROW mode: C++ throw delivery ───────────────────────────────────
#else

#  include "generated_code_compat.h"  // for chaos_managed_exception

namespace chaos::il2cpp::runtime_core {

// CPP_THROW mode: throw is not noexcept, but we are [[noreturn]]
[[noreturn]] inline void chaos_raise_exception(CHAOS_IL2CPP_INTPTR obj) {
    throw chaos_managed_exception{obj};
}

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_EH_CPP_THROW

#endif  // CHAOS_IL2CPP_EXCEPTION_JMP_H_