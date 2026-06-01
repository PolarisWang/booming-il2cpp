// osr_trigger.cpp — On-Stack Replacement (OSR) trigger management
//
// Implements TryOsrPromotion: promotes hot interpreted methods from
// RegisterExecute to T4 native code mid-execution.
//
// Separated from ir_reg_alloc.cpp for clearer architecture.

#include <osr_trigger.h>

#include "ir_reg_alloc.h"
#include "patch_loader.h"
#include "jit_engine.h"
#include "jit_seh.h"

#include <chaos/log.h>

namespace chaos::il2cpp::interpreter {

void TryOsrPromotion(RegisterFrame& frame,
                     const RegisterInstruction* instrs,
                     uint32_t instr_count) noexcept {
    if (frame.patch_method == nullptr) return;
    auto* pm = static_cast<chaos::il2cpp::runtime_core::PatchMethod*>(frame.patch_method);
    using PM = chaos::il2cpp::runtime_core::PatchMethod;

    // Don't re-promote if already at T4 (another thread got there first).
    if (pm->tier_state.load(std::memory_order_acquire) >= PM::kJitted) {
        // If the method already has a cached JitMethod with an OSR entry,
        // re-enter T4 via OSR directly.  This handles the deopt→T4
        // re-promotion loop: after deoptimization, the tier_state is still
        // kJitted and the cached JitMethod is still valid.
        auto* existing_nm = static_cast<chaos::il2cpp::jit::JitMethod*>(
            pm->cached_native_method);
        if (existing_nm != nullptr && existing_nm->osr_entry_offset != 0) {
            // Set OSR resume PC to loop header (frame.pc is the backward branch
            // target after the branch handler executed).
            chaos::il2cpp::jit::g_jit_deopt_state.osr_resume_pc = frame.pc;

            using OsrEntry = void (*)(void*, void*);
            auto osr_entry = reinterpret_cast<OsrEntry>(
                static_cast<uint8_t*>(existing_nm->code) + existing_nm->osr_entry_offset);
            uint64_t osr_ret_buf[2] = {};
            CHAOS_IL2CPP_LOG_DEBUG_M("osr", "TryOsrPromotion: re-promote osr_entry at offset={}", existing_nm->osr_entry_offset);
            osr_entry(&frame.regs, osr_ret_buf);
            CHAOS_IL2CPP_LOG_DEBUG_M("osr", "TryOsrPromotion: re-promote osr_entry returned, ret_val=0x{:x}", osr_ret_buf[0]);
            frame.ret_val = osr_ret_buf[0];
            frame.has_ret = true;
            frame.pc = 0xFFffFFffu;
        }
        return;
    }

    // RegisterMethod should already be cached from T3 lowering.
    auto* rm = static_cast<RegisterMethod*>(pm->cached_reg_method);
    if (rm == nullptr) return;

#ifdef CHAOS_IL2CPP_JIT_MODE
    // Generate native code with full deopt support.
    chaos::il2cpp::jit::CompileConfig cfg;
    cfg.enable_deopt = true;
    cfg.enable_liveness = true;
    cfg.safepoint_fn = nullptr;

    auto* nm = chaos::il2cpp::jit::Compile(*rm, cfg);
    if (nm == nullptr) return;

    // OSR V2: if the generated code has an OSR entry, transfer execution
    // to native code mid-stream with the current register file.
    if (nm->osr_entry_offset != 0) {
        // Set tier state BEFORE calling OSR entry so future calls also hit T4.
        pm->cached_native_method = nm;
        pm->tier_state.store(PM::kJitted, std::memory_order_release);
        chaos::il2cpp::jit::RegisterNativeCodeSection(nm->code, nm->code_size, nm);

        // Set OSR resume PC to loop header before calling OSR entry.
        chaos::il2cpp::jit::g_jit_deopt_state.osr_resume_pc = frame.pc;

        using OsrEntry = void (*)(void*, void*);
        auto osr_entry = reinterpret_cast<OsrEntry>(
            static_cast<uint8_t*>(nm->code) + nm->osr_entry_offset);
        uint64_t osr_ret_buf[2] = {};
        CHAOS_IL2CPP_LOG_DEBUG_M("osr", "TryOsrPromotion: calling osr_entry at offset={}", nm->osr_entry_offset);
        osr_entry(&frame.regs, osr_ret_buf);
        CHAOS_IL2CPP_LOG_DEBUG_M("osr", "TryOsrPromotion: osr_entry returned, ret_val=0x{:x}", osr_ret_buf[0]);

        // Native code completed — capture return value and exit RegisterExecute
        frame.ret_val = osr_ret_buf[0];
        frame.has_ret = true;
        frame.pc = 0xFFffFFffu;
        return;
    }

    // V1 fallback: cache for re-entry on next call
    pm->cached_native_method = nm;
    pm->tier_state.store(PM::kJitted, std::memory_order_release);
    chaos::il2cpp::jit::RegisterNativeCodeSection(nm->code, nm->code_size, nm);
#else
    // JIT disabled — no native compilation in AOT-only builds.
    return;
#endif
}

}  // namespace chaos::il2cpp::interpreter
