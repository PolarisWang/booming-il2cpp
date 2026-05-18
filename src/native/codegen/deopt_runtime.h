#ifndef CHAOS_IL2CPP_CODEGEN_DEOPT_RUNTIME_H_
#define CHAOS_IL2CPP_CODEGEN_DEOPT_RUNTIME_H_

// ── Deoptimization runtime for generated native code ──────────────────────
//
// When generated native code encounters a situation it cannot handle (e.g., a
// call that throws, or an unsupported operation), the deoptimization runtime
// captures the native register context, maps the return address back to a
// RegisterInstruction pc, and reconstructs the RegisterFrame state so the
// method can continue execution in a lower tier (RegisterExecute).
//
// Phase 3c initial implementation: DeoptTrap saves all registers and attempts
// to reconstruct the register file from spilled frame slots.  Full OsrState
// reconstruction with RegStackMap is deferred to later.

#include "native_method.h"

#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::codegen {

/// Saved native register context at a deoptimization point.
/// Captures all caller-saved registers that might hold live values.
struct NativeContext {
    uint64_t gpr[16] = {};   // RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI, R8-R15
    double   fpr[16] = {};   // XMM0-XMM15
};

/// Deoptimization runtime: captures native frame, looks up DeoptEntry,
/// and reconstructs interpreter state for re-execution in a lower tier.
class DeoptRuntime {
public:
    /// Find the DeoptEntry for a given native return address.
    /// Binary search over entries sorted by native_offset.
    /// Returns nullptr if no matching entry is found.
    static const DeoptEntry* FindEntry(
        const NativeMethod* nm,
        uint32_t native_offset) noexcept;

    /// Reconstruct the register file from a NativeContext + DeoptEntry.
    /// Reads values from the context's GPR/FPR arrays and writes to gpr_file[64]
    /// and fpr_file[32] according to the DeoptValue descriptors.
    static void ReconstructRegisterFile(
        uint64_t* gpr_file,          // output[64]
        double*   fpr_file,          // output[32]
        const NativeContext& ctx,
        const DeoptEntry& entry,
        const DeoptValue* values) noexcept;

    /// Deoptimization trap: called from generated code when it cannot continue.
    /// Saves all registers and dispatches back through the interpreter.
    /// This is the entry point that generated code calls via a thunk.
    ///
    /// Phase 3c simplified strategy:
    ///   1. Capture current register state into NativeContext
    ///   2. Look up DeoptEntry from the return address
    ///   3. Reconstruct register file values
    ///   4. Fall through to RegisterExecute for re-execution
    ///
    /// Since generated code runs in cooperative GC mode, this function must
    /// complete quickly and not trigger a GC safepoint.
    static void DeoptTrap(
        NativeMethod* nm,
        uint32_t      return_address,
        NativeContext ctx) noexcept;
};

}  // namespace chaos::il2cpp::codegen

#endif  // CHAOS_IL2CPP_CODEGEN_DEOPT_RUNTIME_H_
