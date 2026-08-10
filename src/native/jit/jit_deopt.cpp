#include "jit_deopt.h"
#include "jit_method.h"
#include "../interpreter/ir_reg_alloc.h"
#include "../interpreter/osr_state.h"

#include <chaos/log.h>

#include <cstring>

#if defined(_MSC_VER)
#include <intrin.h> // _AddressOfReturnAddress()
#else
#include <cstdint>
// GCC/Clang: _AddressOfReturnAddress() via __builtin_return_address
#define _AddressOfReturnAddress() __builtin_return_address(0)
#endif

namespace chaos::il2cpp::jit {

uint64_t DeoptRuntime::ReadSpillSlot(uint64_t codegen_rsp, int16_t spill_offset) noexcept {
    auto* addr = reinterpret_cast<const volatile uint64_t*>(codegen_rsp + spill_offset);
    return *addr;
}

const DeoptEntry* DeoptRuntime::FindEntry(const JitMethod* nm, uint32_t native_offset) noexcept {
    if (nm == nullptr || nm->deopt_entries == nullptr || nm->deopt_entry_count == 0) {
        return nullptr;
    }

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

    if (hi >= 0) {
        return &nm->deopt_entries[hi];
    }
    return nullptr;
}

void DeoptRuntime::ReconstructRegisterFile(uint64_t* out_regs, double* fpr_file, uint8_t* gpr_tags, uint8_t* fpr_tags,
                                           const NativeContext& ctx, const DeoptEntry& entry,
                                           const DeoptValue* values) noexcept {
    if (out_regs == nullptr || fpr_file == nullptr || values == nullptr)
        return;

    for (uint32_t i = 0; i < entry.num_values; ++i) {
        const auto& v = values[entry.values_offset + i];
        if (v.reg_index < 64) {
            if (v.reg_index < 16) {
                if (v.is_spilled) {
                    out_regs[v.reg_index] = ctx.gpr[v.reg_index];
                } else {
                    out_regs[v.reg_index] = ctx.gpr[v.reg_index];
                }
                if (gpr_tags != nullptr) {
                    gpr_tags[v.reg_index] = v.value_tag;
                }
            }
        } else {
            uint32_t fpr_idx = v.reg_index - 64;
            if (fpr_idx < 16) {
                fpr_file[fpr_idx] = ctx.fpr[fpr_idx];
                if (fpr_tags != nullptr) {
                    fpr_tags[fpr_idx] = v.value_tag;
                }
            }
        }
    }
}

void DeoptRuntime::DeoptTrap(JitMethod* nm, uint32_t return_address, NativeContext ctx, uint64_t codegen_rsp,
                             uint64_t* out_gpr_file, double* out_fpr_file, uint8_t* out_gpr_tags,
                             uint8_t* out_fpr_tags) noexcept {
    CHAOS_IL2CPP_LOG_DEBUG_M("deopt", "DeoptTrap entered: nm=%p, ret_addr=0x%x", static_cast<void*>(nm),
                             return_address);

    if (codegen_rsp == 0) {
        void* ret_addr_loc = _AddressOfReturnAddress();
        codegen_rsp = reinterpret_cast<uint64_t>(ret_addr_loc) + 8;
    }

    if (out_gpr_file != nullptr) {
        for (uint32_t vr = 0; vr < 64; ++vr) {
            out_gpr_file[vr] = ReadSpillSlot(codegen_rsp, static_cast<int16_t>(32 + static_cast<int32_t>(vr * 8)));
        }
    }

    if (out_fpr_file != nullptr) {
        for (uint32_t vr = 0; vr < 32; ++vr) {
            out_fpr_file[vr] = *reinterpret_cast<const double*>(codegen_rsp + 544 + vr * 8);
        }
    }

    auto* entry = FindEntry(nm, return_address);
    if (entry == nullptr) {
        CHAOS_IL2CPP_LOG_ERROR_M("deopt", "DeoptTrap: no deopt entry for ret_addr=0x%x, entry_count=%u", return_address,
                                 nm ? nm->deopt_entry_count : 0);
        return;
    }

    ReconstructRegisterFile(out_gpr_file ? out_gpr_file : nullptr, out_fpr_file ? out_fpr_file : nullptr, out_gpr_tags,
                            out_fpr_tags, ctx, *entry, nm->deopt_values);

    CHAOS_IL2CPP_LOG_DEBUG_M("deopt", "DeoptTrap: reconstructed register file at pc=%d, %u values", entry->instr_pc,
                             entry->num_values);

    CHAOS_IL2CPP_LOG_DEBUG_M("deopt", "DeoptTrap: deoptimization at pc=%d complete, returning to interpreter",
                             entry->instr_pc);
}

} // namespace chaos::il2cpp::jit