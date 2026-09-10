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

#include "jit_method.h"

#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::jit {

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
    /// Read a uint64_t from the codegen stack frame at a given RSP-relative
    /// spill offset.  'codegen_rsp' is the RSP value at function entry
    /// (after prologue sub rsp, frame_size).
    static uint64_t ReadSpillSlot(uint64_t codegen_rsp, int16_t spill_offset) noexcept;

    /// Find the DeoptEntry for a given native return address.
    /// Binary search over entries sorted by native_offset.
    /// Returns nullptr if no matching entry is found.
    static const DeoptEntry* FindEntry(
        const JitMethod* nm,
        uint32_t native_offset) noexcept;

    /// Reconstruct the register file from a NativeContext + DeoptEntry.
    /// Reads values from the context's GPR/FPR arrays and writes to gpr_file[64]
    /// and fpr_file[32] according to the DeoptValue descriptors.
    /// Also writes per-register value tags to gpr_tags[64] and fpr_tags[32].
    static void ReconstructRegisterFile(
        uint64_t* gpr_file,          // output[64]
        double*   fpr_file,          // output[32]
        uint8_t*  gpr_tags,          // output[64] — per-register ValueTag
        uint8_t*  fpr_tags,          // output[32] — per-register ValueTag
        const NativeContext& ctx,
        const DeoptEntry& entry,
        const DeoptValue* values) noexcept;

    /// Deoptimization trap: called from generated code when it cannot continue.
    /// Saves all registers and dispatches back through the interpreter.
    /// This is the entry point that generated code calls via a thunk.
    ///
    /// Parameters:
    ///   nm            — JitMethod for the generated code
    ///   return_address — native offset within the generated code
    ///   ctx           — saved NativeContext (16 GPRs + 16 FPRs)
    ///   codegen_rsp   — RSP at function entry (after prologue), or 0 to
    ///                    use _AddressOfReturnAddress() auto-detection
    ///   out_gpr_file  — if non-null, batch-copy all 64 GPRs from stack frame
    ///   out_fpr_file  — if non-null, batch-copy all 32 FPRs from stack frame
    ///   out_gpr_tags  — if non-null, filled with per-register ValueTag (size 64)
    ///   out_fpr_tags  — if non-null, filled with per-register ValueTag (size 32)
    static void DeoptTrap(
        JitMethod* nm,
        uint32_t      return_address,
        NativeContext ctx,
        uint64_t      codegen_rsp = 0,
        uint64_t*     out_gpr_file = nullptr,
        double*       out_fpr_file = nullptr,
        uint8_t*      out_gpr_tags = nullptr,
        uint8_t*      out_fpr_tags = nullptr) noexcept;
};

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_CODEGEN_DEOPT_RUNTIME_H_