#ifndef CHAOS_IL2CPP_CODEGEN_CODE_GENERATOR_H_
#define CHAOS_IL2CPP_CODEGEN_CODE_GENERATOR_H_

// ── x64 native code generator ──────────────────────────────────────────────
//
// Translates a RegisterMethod (register-allocated IR) into x64 machine code.
// The generated code is a self-contained native function that reads arguments
// from args_buf, executes the method body, and writes the return value to
// ret_buf.
//
// Generated function signature:
//   void native_entry(void* args_buf, void* ret_buf);
//
// Phase 3b implementation: stack-slot register file with scratch-register
// computation.  Each virtual register has a fixed stack slot.  Arithmetic
// operations load from stack, compute via scratch registers, store to stack.

#include "native_method.h"
#include "../interpreter/ir_reg_alloc.h"  // RegisterMethod, RegisterInstruction

#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::codegen {

/// Configuration for native code generation.
struct CodeGenConfig {
    // If true, insert GC safepoint polls at loop back edges.
    bool enable_safepoint_polls = true;

    // If true, enable basic register caching (map hot GPRs to x64 regs).
    bool enable_register_caching = true;

    // If true, record deoptimization metadata at call sites.
    bool enable_deopt = true;

    // If true, perform register liveness analysis to refine GC/deopt metadata.
    bool enable_liveness = false;

    // Max instructions allowed before we emit a safepoint poll
    // (as a fallback when no loop back edges are detected).
    uint32_t safepoint_interval = 64;

    // Function pointer to SafepointPoll (runtime_core::threading::SafepointPoll).
    // Set by the caller (entry_direct.cpp) before calling GenerateNativeCode.
    // When non-null, EmitSafepointPoll emits a call through this pointer.
    void* safepoint_fn = nullptr;

    // Absolute address of the current thread's suspend_seq field (atomic<uint32_t>).
    // When non-null, EmitSafepointPoll emits an inline check before the full call:
    //   cmp dword [ptr], 0  → je .skip  (fast path, ~3 cycles)
    // Set by entry_direct.cpp during T3→T4 promotion.
    void* thread_suspend_seq_ptr = nullptr;

    // PIC dispatch data (PatchMethod::pic_dispatch_data) for CallVirt.
    // Set by entry_direct.cpp during T3→T4 promotion.
    void* pic_dispatch_data = nullptr;

    // ── TLAB inline allocation (V3.5) ────────────────────────────────────

    // Absolute address of the thread-local tls_tlab.current (char* bump pointer).
    // Set by entry_direct.cpp before calling GenerateNativeCode.
    // When both tlab_current_loc and tlab_end_loc are non-null, NewObj/Box
    // emit an inline TLAB bump fast path instead of calling GcAllocate.
    void* tlab_current_loc = nullptr;

    // Absolute address of the thread-local tls_tlab.end (char* end bound).
    void* tlab_end_loc = nullptr;

    // If true, use graph coloring register allocation for optimal register use.
    bool enable_graph_coloring = false;
};

/// Generate native x64 code from a RegisterMethod.
///
/// @param rm       The register-allocated method to compile.
/// @param config   Code generation configuration.
///
/// @return NativeMethod containing the generated code + metadata,
///         or nullptr on failure (unsupported opcodes, allocation error).
NativeMethod* GenerateNativeCode(
    const interpreter::RegisterMethod& rm,
    const CodeGenConfig& config = CodeGenConfig()) noexcept;

/// Check if GenerateNativeCode can handle this RegisterMethod.
/// Returns false if the method contains unsupported opcodes.
bool CanGenerateNativeCode(
    const interpreter::RegisterMethod& rm) noexcept;

}  // namespace chaos::il2cpp::codegen

#endif  // CHAOS_IL2CPP_CODEGEN_CODE_GENERATOR_H_
