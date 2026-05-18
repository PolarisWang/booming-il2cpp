#include "deopt_runtime.h"
#include "codegen_helpers.h"  // for t_deopt_state
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
    const DeoptValue* values,
    uint64_t  codegen_rsp) noexcept {

    if (gpr_file == nullptr || fpr_file == nullptr || values == nullptr) return;

    for (uint32_t i = 0; i < entry.num_values; ++i) {
        const auto& v = values[i];
        if (v.reg_index < 64) {
            // GPR value
            if (v.is_spilled && v.spill_offset != 0) {
                gpr_file[v.reg_index] =
                    *reinterpret_cast<const uint64_t*>(codegen_rsp + v.spill_offset);
            } else if (v.reg_index < 16) {
                gpr_file[v.reg_index] = ctx.gpr[v.reg_index];
            }
        } else {
            // FPR value
            uint32_t fpr_idx = v.reg_index - 64;
            if (v.is_spilled && v.spill_offset != 0) {
                fpr_file[fpr_idx] =
                    *reinterpret_cast<const double*>(codegen_rsp + v.spill_offset);
            } else if (fpr_idx < 16) {
                fpr_file[fpr_idx] = ctx.fpr[fpr_idx];
            }
        }
    }
}

void DeoptRuntime::DeoptTrap(
    NativeMethod* nm,
    uint32_t      return_address,
    NativeContext ctx,
    uint64_t      codegen_rsp) noexcept {

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

    // 2. Reconstruct the register file from the saved context and stack frame.
    uint64_t gpr_file[64] = {};
    double   fpr_file[32] = {};
    ReconstructRegisterFile(gpr_file, fpr_file, ctx, *entry,
                            nm->deopt_values, codegen_rsp);

    // 3. Publish reconstructed state to TLS for InterpreterEntryDirect.
    t_deopt_state.instr_pc = entry->instr_pc;
    std::memcpy(t_deopt_state.gpr_file, gpr_file, sizeof(gpr_file));
    std::memcpy(t_deopt_state.fpr_file, fpr_file, sizeof(fpr_file));
    t_deopt_state.deopt_happened = true;

    CHAOS_IL2CPP_LOG_DEBUG_M("deopt",
        "DeoptTrap: deoptimized at pc=%d, %u values restored",
        entry->instr_pc, entry->num_values);
}

}  // namespace chaos::il2cpp::codegen
