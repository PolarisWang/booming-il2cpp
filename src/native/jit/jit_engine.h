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

#include "IEncoder.h"
#include "ISehHandler.h"
#include "jit_method.h"
#include "jit_helpers.h"
#include "../interpreter/ir_reg_alloc.h"  // RegisterMethod, RegisterInstruction

#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::jit {

/// Compilation tier for JIT code generation.
/// Tier 0 produces code quickly with no optimizations (stack-only register access,
/// no liveness, no deopt metadata, no SEH).  Tier 1 is the full pipeline with
/// graph coloring, optimizer, liveness analysis, deopt, SEH, and OSR.
enum class CompileTier : uint8_t {
    kQuick = 0,  // Quick JIT (<50µs target): stack-only, no optimizer/liveness/deopt/SEH
    kFull = 1,  // Standard JIT: full pipeline with graph coloring + optimizations
};

/// Configuration for native code generation.
struct CompileConfig {
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
    // Set by the caller (entry_direct.cpp) before calling Compile.
    // When non-null, EmitSafepointPoll emits a call through this pointer.
    void* safepoint_fn = nullptr;

    // PIC dispatch data from PatchMethod (for CallVirt fast path in T4 code).
    // Set by entry_direct.cpp before Compile.  When non-null,
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
    // Used by RegisterNativeCodeSection to associate the generated code with its token
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

    // ── Precise GC: field type tags by field token (indexed by token value) ─
    // When non-null, LdFld/LdSFld type inference uses the per-field tag
    // instead of conservative ObjectRef, enabling precise GC slot maps.
    const uint8_t* field_type_tags = nullptr;
    uint32_t       field_type_count = 0;

    // ── Precise GC: method return type tags by instruction index ──────────
    // When non-null, Call/CallVirt/CallBridge/Calli type inference uses the
    // per-instruction return tag as a fallback when call_cache ret_tag is
    // unavailable.  Indexed by current_instr_index_.
    const uint8_t* method_ret_tags = nullptr;
    uint32_t       method_ret_tag_count = 0;

    // ── Per-instruction PIC data for inline monomorphic cache ──────────
    // Populated by entry_direct.cpp from the first PIC chain slot for each
    // CallVirt instruction.  When non-null and the entry for the current
    // instruction has a valid direct_fn, the code generator emits an inline
    // monomorphic check + direct call instead of going through CodegenCallVirt.
    // Indexed by current_instr_index_.
    const PerInstrPicData* per_instr_pic = nullptr;
    uint32_t               per_instr_pic_count = 0;

    /// Compilation tier.  kFull (full pipeline) by default.
    CompileTier compile_tier = CompileTier::kFull;

    /// If true, enable PGO (Profile-Guided Optimization).  When enabled, Tier 0
    /// compilation does NOT patch direct_ptr — calls continue through the dispatch
    /// function which counts calls and triggers background Tier 1 recompilation
    /// when the call count exceeds kPgoTier1Threshold.
    bool enable_pgo = false;
};

/// Generate native x64 code from a RegisterMethod.
///
/// @param rm       The register-allocated method to compile.
/// @param config   Code generation configuration.
///
/// @return JitMethod containing the generated code + metadata,
///         or nullptr on failure (unsupported opcodes, allocation error).
JitMethod* Compile(
    const interpreter::RegisterMethod& rm,
    const CompileConfig& config = CompileConfig()) noexcept;

/// Check if Compile can handle this RegisterMethod.
/// Returns false if the method contains unsupported opcodes.
bool CanCompile(
    const interpreter::RegisterMethod& rm) noexcept;

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_CODEGEN_CODE_GENERATOR_H_
