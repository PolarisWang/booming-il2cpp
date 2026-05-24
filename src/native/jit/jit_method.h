#ifndef CHAOS_IL2CPP_CODEGEN_NATIVE_METHOD_H_
#define CHAOS_IL2CPP_CODEGEN_NATIVE_METHOD_H_

// ── JitMethod: metadata for a generated native code method ──────────────
//
// Produced by Compile() and stored in PatchMethod for dispatch
// via InterpreterEntryDirect when tier_state reaches T4_ready.
//
// Layout:
//   code     = entry point (RX, managed by CodeBuffer)
//   metadata = GC slot map, deopt table, call site info

#include <cstdint>
#include <cstddef>
#include <cstddef>
#include <atomic>

// Forward declaration: GcSlotMapV0 is defined in codegen_bridge.h
// We use a pointer member only, so no full definition is needed here.
struct GcSlotMapV0;

namespace chaos::il2cpp::jit {

/// A single call site in the generated code — used by deoptimization
/// to map a native return address back to the RegisterInstruction pc.
struct CallSiteInfo {
    uint32_t    instr_pc;           // RegisterInstruction index
    uint32_t    native_offset;      // byte offset from code entry
    uint32_t    method_token = 0;   // AOT metadata token of the called method
    uint32_t    module_id    = 0;   // module owning the called method
};

/// Debug info header — appended to the end of the code buffer.
/// External debugger tooling (e.g., a WinDbg/natvis plugin) can locate this
/// by scanning backwards for the magic number.
struct JitDebugInfo {
    static constexpr uint32_t kMagic = 0x544A4442u;  // "BJDT" (Booming JIT Debug)
    static constexpr uint32_t kVersion = 2u;         // v2: added method_name_off/len

    uint32_t magic;                  // kMagic — identity marker
    uint32_t version;                // kVersion
    uint32_t code_size;              // total code buffer size (bytes)
    uint32_t instr_offset_count;     // number of instr_offsets entries
    uint32_t instr_offsets_off;      // offset from code start to instr_offsets array
                                     // (0 = not embedded, use JitMethod::instr_offsets)
    uint32_t method_name_off;        // offset from code start to UTF-8 method name (0 = none)
    uint32_t method_name_len;        // length of method name in bytes (excluding null)
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
    // Slot map entries for precise root scanning (GcSlotMapV0).
};

/// Deoptimization entry — maps a native safepoint back to RegisterInstruction
/// state so the deoptimization runtime can reconstruct OsrState.
struct DeoptEntry {
    uint32_t   native_offset;       // byte offset from code entry (safepoint)
    uint32_t   instr_pc;            // RegisterInstruction index
    uint32_t   osr_resume_pc = 0;   // OSR resume PC (loop header). 0 = instruction 0.
    uint32_t   num_values;          // number of active values at this point
    uint32_t   values_offset;       // index into DeoptValue[] array for this entry's values
};

struct DeoptValue {
    uint32_t   reg_index;           // RegisterFile register index
    uint8_t    value_tag;           // ValueTag (Int32, Int64, etc.)
    bool       is_spilled;          // true = value is in a stack spill slot
    int16_t    spill_offset;        // stack spill offset from RBP (if is_spilled)
};

/// Complete metadata for a generated native method.
struct JitMethod {
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

    // SEH clause table offset (relative to code start).
    // The table is appended to the code buffer after Generate() completes.
    // 0 means no SEH table.
    uint32_t      seh_table_offset = 0;

#if defined(_WIN64)
    // Win64 RUNTIME_FUNCTION for .pdata/.xdata unwind info.
    // Allocated by Compile, freed by ~JitMethod.
    void* runtime_function = nullptr;
#else
    // Linux x64 .eh_frame DWARF CFI offset from code start.
    // 0 = no .eh_frame emitted (no prologue).
    uint32_t eh_frame_offset = 0;
#endif

    // Precise GC slot map for root scanning during collection.
    // Allocated by Compile, freed by ~JitMethod.
    // When non-null, the GC can use this instead of conservative scanning.
    GcSlotMapV0* gc_slot_map = nullptr;

    // GcSlotMapV0: precise GC root map for stack scanning
    void*         slot_map_data   = nullptr;   // serialized GcSlotMapV0 blob
    uint32_t      slot_map_size   = 0;

    // OSR entry point: byte offset from code start for the OSR entry stub.
    // The OSR entry copies a RegisterFile to the stack frame and jumps to
    // instruction 0, enabling true mid-execution on-stack replacement.
    // 0 = no OSR entry (no loop headers in this method).
    uint32_t      osr_entry_offset = 0;

    // JitDebugInfo byte offset from code start. 0 = not emitted.
    // SOS extension uses this to locate the debug info footer.
    uint32_t      debug_info_offset = 0;

    // Instruction offset table: instr_offsets[pc] = native byte offset.
    // Maps RegisterInstruction index → byte offset from code entry.
    // Always populated by Compile() for all JIT-compiled methods.
    // Used by deoptimization (OsrResolveLoopHeader), SEH clause emission,
    // branch resolution, and debugger line-number mapping.
    // The last entry is a sentinel whose offset equals code_size (useful
    // for range checks in SEH clause dispatch).
    uint32_t*     instr_offsets      = nullptr;
    uint32_t      instr_offset_count = 0;

    // Offset from RBP (native frame pointer) to RSP (stack frame base).
    // Used by GC stack walker to compute frame_ptr from the native RBP
    // for T4 frames.  Varies per-method with register caching.
    uint32_t      rbp_to_rsp_offset = 0;

    // ── Call-site slot table (for hotpatch-safe indirect calls) ──────────
    // Each entry is a void* in the RX code buffer, accessed via call [rip+off].
    // Updated by ReverseSlotMap::UpdateAll() during hotpatch.
    void**        call_site_slots      = nullptr;  // points into the RX code buffer
    uint32_t      call_site_slot_count = 0;
    uint32_t      call_site_capacity   = 0;

    // Destructor: frees all allocations.
    ~JitMethod() noexcept;

    JitMethod() = default;
    JitMethod(const JitMethod&) = delete;
    JitMethod& operator=(const JitMethod&) = delete;
    JitMethod(JitMethod&& other) noexcept;
    JitMethod& operator=(JitMethod&& other) noexcept;

    // ── Inline tracking (for hotupdate-aware inliner) ──────────────────────
    // Records callee tokens and their version snapshots so the runtime can
    // detect stale inline caches after hotpatch and trigger recompilation.
    struct InlinedCallee {
        uint32_t callee_token;
        uint32_t snapshot_version;
    };
    InlinedCallee* inlined_callees  = nullptr;
    uint32_t       inlined_callee_count = 0;

    /// Atomic stale flag: set by InlineReverseMap::InvalidateCallers when a
    /// callee is hotpatched after it was inlined into this method.  Checked
    /// in JitStubDispatchImpl — when true, triggers recompilation.
    std::atomic<bool> stale{false};
};

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_CODEGEN_NATIVE_METHOD_H_
