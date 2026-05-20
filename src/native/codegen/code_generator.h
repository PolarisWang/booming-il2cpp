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

    // If true, run the IR optimizer (constant folding + DCE) before codegen.
    bool enable_optimizer = true;

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

    // PIC dispatch data from PatchMethod (for CallVirt fast path in T4 code).
    // Set by entry_direct.cpp before GenerateNativeCode.  When non-null,
    // CallVirt instructions use PIC chain lookup for direct dispatch.
    const void* pic_dispatch_data = nullptr;

    // Dispatch context pointer for InterpreterDispatchRaw fallback from
    // CodegenCallVirt.  Points to InterpreterDispatchContext on the caller's
    // stack, valid for the lifetime of the current RegisterExecute session.
    void* dispatch_ctx = nullptr;

    // CachedCallInfo[] from PatchMethod (one per instruction).
    // Used by EmitCall to populate CallSiteInfo.method_token for T4 call-site
    // hotpatch tracking.  Indexed by current_instr_index_.
    // nullptr = no call cache available (T4 test paths without PatchMethod).
    const void* call_cache = nullptr;
    uint32_t    call_cache_count = 0;

    // This method's own AOT metadata token.
    // Used by RegisterT4Code to associate the generated code with its token
    // so method_replacement::Register can find and demote matching T4 entries.
    uint32_t method_token = 0;
    uint32_t method_module_id = 0;

    // ── Argument type tags for LdArg precision ──────────────────────────
    // Cached argument type tags from PatchMethod (ValueTag values).
    // When non-null, LdArg type inference uses these instead of conservative
    // ObjectRef, enabling precise GC slot maps for method arguments.
    // Indexed by `RegisterInstruction::imm::operand_index`.
    const uint8_t* arg_type_tags = nullptr;
    uint32_t       arg_type_count = 0;
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
