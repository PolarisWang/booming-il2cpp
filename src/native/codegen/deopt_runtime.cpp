#include "deopt_runtime.h"
#include "native_method.h"
#include "../interpreter/ir_reg_alloc.h"
#include "../interpreter/osr_state.h"

#include <chaos/log.h>

#include <cstring>

namespace chaos::il2cpp::codegen {

const DeoptEntry* DeoptRuntime::FindEntry(
    const NativeMethod* nm,
    uint32_t native_offset) noexcept {

    if (nm == nullptr || nm->deopt_entries == nullptr || nm->deopt_entry_count == 0) {
        return nullptr;
    }

    // Binary search: deopt_entries are sorted by native_offset.
    int32_t lo = 0;
    int32_t hi = static_cast<int32_t>(nm->deopt_entry_count) - 1;
    while (lo <= hi) {
        int32_t mid = lo + (hi - lo) / 2;
        auto& e = nm->deopt_entries[mid];
        if (e.native_offset < native_offset) {
            lo = mid + 1;
        } else if (e.native_offset > native_offset) {
            hi = mid - 1;
        } else {
            return &e;
        }
    }

    // Exact match not found: return the nearest entry with
    // native_offset < target.  For call instructions, the return
    // address is after the call, so the preceding entry applies.
    if (hi >= 0) {
        return &nm->deopt_entries[hi];
    }
    return nullptr;
}

void DeoptRuntime::ReconstructRegisterFile(
    uint64_t* gpr_file,
    double*   fpr_file,
    const NativeContext& ctx,
    const DeoptEntry& entry,
    const DeoptValue* values) noexcept {

    if (gpr_file == nullptr || fpr_file == nullptr || values == nullptr) return;

    for (uint32_t i = 0; i < entry.num_values; ++i) {
        const auto& v = values[i];
        if (v.reg_index < 64) {
            // GPR value
            if (v.is_spilled) {
                // All values are spilled to the stack frame in Phase 3c.
                // The NativeContext captures only the 16 architectural GPRs,
                // not the 64 virtual register stack slots.  For the initial
                // implementation, we read from the saved context registers
                // where possible, and leave others as zero (they will be
                // reconstructed by RegisterExecute from the RegisterMethod).
                if (v.reg_index < 16) {
                    gpr_file[v.reg_index] = ctx.gpr[v.reg_index];
                }
                // Virtual registers >= 16 must be recovered from the stack
                // frame.  Phase 3c+ will add a full stack snapshot.
            } else {
                // Value is in a register — read directly from context.
                if (v.reg_index < 16) {
                    gpr_file[v.reg_index] = ctx.gpr[v.reg_index];
                }
            }
        } else {
            // FPR value
            uint32_t fpr_idx = v.reg_index - 64;
            if (fpr_idx < 16) {
                fpr_file[fpr_idx] = ctx.fpr[fpr_idx];
            }
        }
    }
}

void DeoptRuntime::DeoptTrap(
    NativeMethod* nm,
    uint32_t      return_address,
    NativeContext ctx) noexcept {

    CHAOS_IL2CPP_LOG_DEBUG_M("deopt", "DeoptTrap entered: nm=%p, ret_addr=0x%x",
                              static_cast<void*>(nm), return_address);

    // 1. Find the DeoptEntry matching this return address.
    auto* entry = FindEntry(nm, return_address);
    if (entry == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR_M("deopt",
            "DeoptTrap: no deopt entry for ret_addr=0x%x, entry_count=%u",
            return_address, nm ? nm->deopt_entry_count : 0);
        return;
    }

    // 2. Reconstruct the register file from the saved context.
    uint64_t gpr_file[64] = {};
    double   fpr_file[32] = {};
    ReconstructRegisterFile(gpr_file, fpr_file, ctx, *entry,
                            nm->deopt_values);

    CHAOS_IL2CPP_LOG_DEBUG_M("deopt",
        "DeoptTrap: reconstructed register file at pc=%d, %u values",
        entry->instr_pc, entry->num_values);

    // 3. Phase 3c simplified deoptimization:
    //    We do not perform full OsrState reconstruction here because that
    //    requires RegStackMap data (from the RegisterMethod) and a target
    //    frame format.  Instead, the native code caller (InterpreterEntryDirect)
    //    detects that native execution failed and falls back to RegisterExecute
    //    from the beginning (pc=0).
    //
    //    Future phases will implement OSR-based deoptimization:
    //    a) Capture full virtual register file from the stack frame
    //    b) Use RegStackMap to map register values to evaluation stack
    //    c) Restore OsrState into RegisterFrame at the exact pc
    //    d) Resume RegisterExecute from the deopt point
    //
    //    For now, DeoptTrap serves as a hook for debugging and logging.
    CHAOS_IL2CPP_LOG_DEBUG_M("deopt",
        "DeoptTrap: deoptimization at pc=%d complete, returning to interpreter",
        entry->instr_pc);
}

}  // namespace chaos::il2cpp::codegen
