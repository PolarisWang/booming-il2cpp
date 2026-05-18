#ifndef CHAOS_IL2CPP_CODEGEN_NATIVE_METHOD_H_
#define CHAOS_IL2CPP_CODEGEN_NATIVE_METHOD_H_

// ── NativeMethod: metadata for a generated native code method ──────────────
//
// Produced by GenerateNativeCode() and stored in PatchMethod for dispatch
// via InterpreterEntryDirect when tier_state reaches T4_ready.
//
// Layout:
//   code     = entry point (RX, managed by CodeBuffer)
//   metadata = GC slot map, deopt table, call site info

#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::codegen {

/// A single call site in the generated code — used by deoptimization
/// to map a native return address back to the RegisterInstruction pc.
struct CallSiteInfo {
    uint32_t    instr_pc;           // RegisterInstruction index
    uint32_t    native_offset;      // byte offset from code entry
};

/// GC slot descriptor for a single stack/register location at a safepoint.
enum class GcSlotKind : uint8_t {
    Gpr    = 0,   // General-purpose register (index in native register file)
    Stack  = 1,   // Stack spill slot (offset from RBP/RSP)
};

struct GcSlot {
    GcSlotKind kind;
    uint16_t   index;    // for Gpr: x64 register number; for Stack: offset / 8
};

/// GC point: a native address where GC may be active.
/// At this point, certain GPRs and stack slots hold managed references.
struct GcPoint {
    uint32_t   native_offset;       // byte offset from code entry
    uint32_t   slot_count;          // number of GcSlots at this point
    GcSlot*    slots;               // array of slot_count GcSlot entries
    // TODO(Phase 3d): precise slot map; for Phase 3b, use conservative scanning.
};

/// Deoptimization entry — maps a native safepoint back to RegisterInstruction
/// state so the deoptimization runtime can reconstruct OsrState.
struct DeoptEntry {
    uint32_t   native_offset;       // byte offset from code entry (safepoint)
    uint32_t   instr_pc;            // RegisterInstruction index
    uint32_t   num_values;          // number of active values at this point
    // values[] not stored inline — store separately in deopt table.
};

struct DeoptValue {
    uint32_t   reg_index;           // RegisterFile register index
    uint8_t    value_tag;           // ValueTag (Int32, Int64, etc.)
    bool       is_spilled;          // true = value is in a stack spill slot
    int16_t    spill_offset;        // stack spill offset from RBP (if is_spilled)
};

/// Complete metadata for a generated native method.
struct NativeMethod {
    void*      code         = nullptr;  // Entry point (executable RX memory)
    uint32_t   code_size    = 0;         // Size of generated code in bytes
    uint32_t   instr_count  = 0;         // Number of RegisterInstructions

    // Call sites (for deopt: map native address → RegisterInstruction pc)
    CallSiteInfo* call_sites     = nullptr;
    uint32_t      call_site_count = 0;

    // Deoptimization entries
    DeoptEntry*   deopt_entries    = nullptr;
    DeoptValue*   deopt_values     = nullptr;
    uint32_t      deopt_entry_count = 0;

    // GC points (for precise root scanning)
    GcPoint*      gc_points    = nullptr;
    uint32_t      gc_point_count = 0;

    // Destructor: frees all allocations.
    ~NativeMethod() noexcept;

    NativeMethod() = default;
    NativeMethod(const NativeMethod&) = delete;
    NativeMethod& operator=(const NativeMethod&) = delete;
    NativeMethod(NativeMethod&& other) noexcept;
    NativeMethod& operator=(NativeMethod&& other) noexcept;
};

}  // namespace chaos::il2cpp::codegen

#endif  // CHAOS_IL2CPP_CODEGEN_NATIVE_METHOD_H_
