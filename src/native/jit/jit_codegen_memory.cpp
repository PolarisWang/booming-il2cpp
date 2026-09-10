// jit_codegen_memory.cpp — GPR/FPR register load/store + register-cache
// selection/spill + call-with-spill for NativeCodeGenerator (T2.4 module
// split of jit_engine.cpp).
//
// Pure move: LoadGpr/StoreGpr/LoadFpr/StoreFpr/SelectCacheableRegs/
// SpillCachedRegs/SpillGcRefCachedRegs/EmitCallWithSpill/
// EmitRuntimeHelperCallImpl were relocated verbatim; no logic changed.

#include "jit_engine.h"
#include "ArchTraits.h"
#include "jit_helpers.h"
#include "code_buffer.h"
#include "IEncoder.h"
#include "jit_reg_alloc.h"
#include "../interpreter/ir_reg_alloc.h" // interpreter::kGPRegisters
#include <instantiation_engine.h>
#if defined(__aarch64__)
#include "arm64_encoder.h"
#include "Arm64Encoder.h"
#else
#include "X64Encoder.h"
#endif

namespace chaos::il2cpp::jit {

void NativeCodeGenerator::LoadGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::LoadGpr");
    if (vreg >= interpreter::kGPRegisters) {
        if (collect_stats_) RecordGprAccess(true, true, false, current_opc_);
        enc_.EmitMovRM(x64_reg, AT::kStackReg, static_cast<int32_t>(GprOff(vreg)));
        return;
    }

    // Graph coloring V2: colored vreg → direct reg-to-reg move
    if (has_graph_coloring_) {
        uint8_t colored_x64 = gcr_.gpr_color[vreg];
        if (colored_x64 != 0xFF) {
            if (collect_stats_) RecordGprAccess(true, false, false, current_opc_);
            if (x64_reg != colored_x64)
                enc_.EmitMovRR(x64_reg, colored_x64);
            return;
        }
    }
    // V1 cache hit
    if (config_.enable_register_caching) {
        uint8_t cached = cached_x64_for_vreg_[vreg];
        if (cached != kNotCached) {
            if (collect_stats_) RecordGprAccess(true, false, false, current_opc_);
            if (x64_reg != cached)
                enc_.EmitMovRR(x64_reg, cached);
            return;
        }
    }
    // Load from stack
    if (collect_stats_) RecordGprAccess(true, true, false, current_opc_);
    enc_.EmitMovRM(x64_reg, AT::kStackReg, static_cast<int32_t>(GprOff(vreg)));
}

void NativeCodeGenerator::StoreGpr(uint8_t x64_reg, uint32_t vreg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StoreGpr");
    if (vreg >= interpreter::kGPRegisters) {
        if (collect_stats_) RecordGprAccess(false, true, false, current_opc_);
        enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vreg)), x64_reg);
        return;
    }

    // Graph coloring V2: colored vreg → direct reg-to-reg move (no stack write)
    if (has_graph_coloring_) {
        uint8_t colored_x64 = gcr_.gpr_color[vreg];
        if (colored_x64 != 0xFF) {
            if (x64_reg != colored_x64)
                enc_.EmitMovRR(colored_x64, x64_reg);
            // Caller-colored vregs: write through to stack so the stack slot
            // holds the correct value even if argument setup clobbers the
            // colored register before EmitCallWithSpill's pre-call spill.
            // T2.1 A1 / per-vreg refinement: only a caller-colored vreg whose live
            // range crosses a call/safepoint (cross_call_mask_) needs the write —
            // it's live across the call and will be reloaded.  A vreg live only in
            // call-free code stays purely register-resident (no stack write).
            // 省写穿 (Phase 1): additionally, ONLY an arg-register-colored caller
            // vreg (RCX/RDX/R8/R9) keeps the write-through — argument setup writes
            // those registers BEFORE the pre-call spill, so their stack slot must be
            // current from def time.  Non-arg caller-saved vregs (RAX/R10/R11) are
            // now preserved by SpillLiveColoredForCall at the call site and stay
            // purely register-resident between calls (no write-through store).
            bool write_through = (caller_colored_mask_ & (1ULL << vreg)) &&
                                 (cross_call_mask_ & (1ULL << vreg)) &&
                                 IsGprArgReg(gcr_.gpr_color[vreg]);
            if (collect_stats_)
                RecordGprAccess(false, write_through, write_through, current_opc_);
            if (write_through)
                enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vreg)), colored_x64);
            return;
        }
    }
    // V1 cache
    if (config_.enable_register_caching) {
        uint8_t cached = cached_x64_for_vreg_[vreg];
        if (cached != kNotCached && num_cache_regs_ > 0) {
            if (collect_stats_) RecordGprAccess(false, false, false, current_opc_);
            if (cached == x64_reg) {
                uint32_t slot = 0;
                for (; slot < kMaxCacheRegs; ++slot) {
                    if (kCacheableRegs[slot] == cached)
                        break;
                }
                if (slot < kMaxCacheRegs)
                    cached_dirty_mask_ |= (1u << slot);
                return;
            }
            enc_.EmitMovRR(cached, x64_reg);
            uint32_t slot = 0;
            for (; slot < kMaxCacheRegs; ++slot) {
                if (kCacheableRegs[slot] == cached)
                    break;
            }
            if (slot < kMaxCacheRegs)
                cached_dirty_mask_ |= (1u << slot);
            return;
        }
    }
    // Not cached/spilled: write through to stack
    if (collect_stats_) RecordGprAccess(false, true, false, current_opc_);
    enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vreg)), x64_reg);
}

void NativeCodeGenerator::LoadFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    // Graph coloring V2: colored FPR → direct reg-to-reg move
    if (has_graph_coloring_ && vreg >= kGprCount) {
        uint32_t fi = vreg - kGprCount;
        if (fi < 32) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (collect_stats_) RecordFprAccess(true, false, false, current_opc_);
                if (xmm_reg != colored_xmm)
                    enc_.EmitMovdqaRR(xmm_reg, colored_xmm);
                return;
            }
        }
    }
    // Fallback: load 128-bit from stack via movdqa
    if (collect_stats_) RecordFprAccess(true, true, false, current_opc_);
    enc_.EmitMovdqaRM(xmm_reg, AT::kStackReg, static_cast<int32_t>(FprOff(vreg)));
}

void NativeCodeGenerator::StoreFpr(uint8_t xmm_reg, uint32_t vreg) noexcept {
    // Graph coloring V2: colored FPR → direct reg-to-reg move (128-bit movdqa, no stack write)
    if (has_graph_coloring_ && vreg >= kGprCount) {
        uint32_t fi = vreg - kGprCount;
        if (fi < 32) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (xmm_reg != colored_xmm)
                    enc_.EmitMovdqaRR(colored_xmm, xmm_reg);
                // Caller-colored FPRs: write through to stack so the stack slot
                // holds the correct value even if argument setup clobbers the
                // colored register before EmitCallWithSpill's pre-call spill.
                // T2.1 A1: skip on a call-free method (no caller-clobber sites).
                bool write_through = (caller_fpr_colored_mask_ & (1ULL << fi)) && has_caller_clobber_;
                if (collect_stats_)
                    RecordFprAccess(false, write_through, write_through, current_opc_);
                if (write_through)
                    enc_.EmitMovdqaMR(AT::kStackReg, static_cast<int32_t>(FprOff(vreg)), colored_xmm);
                return;
            }
        }
    }
    // Fallback: write 128-bit to stack via movdqa
    if (collect_stats_) RecordFprAccess(false, true, false, current_opc_);
    enc_.EmitMovdqaMR(AT::kStackReg, static_cast<int32_t>(FprOff(vreg)), xmm_reg);
}

void NativeCodeGenerator::SelectCacheableRegs() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::SelectCacheableRegs");
    if (!config_.enable_register_caching)
        return;

    // Count access frequency for each virtual register across all instructions.
    // Weight: dst counts 2, src counts 1 — dst access is more critical since it
    // implies a subsequent read by another instruction.
    uint32_t freq[interpreter::kGPRegisters] = {};
    for (const auto& ri : rm_.instructions) {
        if (ri.has_dst() && ri.dst_reg() < interpreter::kGPRegisters)
            freq[ri.dst_reg()] += 2;
        if (ri.has_src1() && ri.src1_reg() < interpreter::kGPRegisters)
            freq[ri.src1_reg()] += 1;
        if (ri.has_src2() && ri.src2_reg() < interpreter::kGPRegisters)
            freq[ri.src2_reg()] += 1;
    }

    // Initialize all mappings to "not cached"
    std::memset(cached_x64_for_vreg_, kNotCached, sizeof(cached_x64_for_vreg_));
    std::memset(phys_to_cached_vreg_, kNotCached, sizeof(phys_to_cached_vreg_));
    cached_slots_used_ = 0;
    cached_dirty_mask_ = 0;
    num_cache_regs_ = 0;

    // Select top-k vregs by frequency. Since kMaxCacheRegs is small (4), a
    // simple linear selection is sufficient — no need for partial sort.
    for (uint32_t slot = 0; slot < kMaxCacheRegs; ++slot) {
        uint32_t best_vreg = kNotCached;
        uint32_t best_freq = 0;
        for (uint32_t vr = 0; vr < interpreter::kGPRegisters; ++vr) {
            if (freq[vr] > best_freq && cached_x64_for_vreg_[vr] == kNotCached) {
                best_freq = freq[vr];
                best_vreg = vr;
            }
        }
        if (best_vreg == kNotCached || best_freq == 0)
            break;

        uint8_t x64r = kCacheableRegs[slot];
        cached_x64_for_vreg_[best_vreg] = x64r;
        phys_to_cached_vreg_[x64r] = static_cast<uint8_t>(best_vreg);
        cached_slots_used_ |= (1u << slot);
        ++num_cache_regs_;
    }
}

void NativeCodeGenerator::SpillCachedRegs() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::SpillCachedRegs");
    if (!config_.enable_register_caching || cached_slots_used_ == 0)
        return;
    if (cached_dirty_mask_ == 0)
        return;

    // For each dirty slot, write the cached x64 register to the vreg's stack slot
    uint32_t dirty = cached_dirty_mask_;
    for (uint32_t slot = 0; slot < kMaxCacheRegs && dirty; ++slot) {
        if (!(dirty & (1u << slot)))
            continue;
        dirty &= ~(1u << slot);
        uint8_t x64r = static_cast<uint8_t>(kCacheableRegs[slot]);
        uint32_t vreg = phys_to_cached_vreg_[x64r];
        if (vreg != kNotCached) {
            enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vreg)), x64r);
        }
    }
    cached_dirty_mask_ = 0;
}

void NativeCodeGenerator::SpillGcRefCachedRegs() noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::SpillGcRefCachedRegs");
    if (!config_.enable_register_caching || cached_slots_used_ == 0)
        return;
    if (cached_dirty_mask_ == 0)
        return;

    // Only spill cache slots holding GC object references. Non-GC ref values
    // (int32, int64, float, etc.) stay in registers across safepoints since
    // the GC only needs to scan object references on the stack.
    uint32_t dirty = cached_dirty_mask_;
    for (uint32_t slot = 0; slot < kMaxCacheRegs && dirty; ++slot) {
        if (!(dirty & (1u << slot)))
            continue;
        dirty &= ~(1u << slot);
        uint8_t x64r = static_cast<uint8_t>(kCacheableRegs[slot]);
        uint32_t vreg = phys_to_cached_vreg_[x64r];
        if (vreg != kNotCached && vreg < vreg_types_.size() && vreg_types_[vreg] == kTypeObjectRef) {
            enc_.EmitMovMR(AT::kStackReg, static_cast<int32_t>(GprOff(vreg)), x64r);
            cached_dirty_mask_ &= ~(1u << slot);
        }
    }
}

void NativeCodeGenerator::EmitCallWithSpill(uint8_t reg) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::EmitCallWithSpill");
    if (config_.enable_register_caching && cached_slots_used_)
        SpillCachedRegs();
    // 省写穿 (Phase 1): spill the caller-colored GPRs that are live at this call
    // point to their fixed stack slots (in addition to the callee-colored ones),
    // so their stack slots are current for the call.  Arg-register-colored vregs
    // are excluded (kept current by write-through — see SpillLiveColoredForCall),
    // because at a helper call site argument setup has already written RCX/RDX/
    // R8/R9 before this spill runs.  FPRs stay conservative: spill all
    // non-caller-colored FPRs, keep caller-colored FPRs on write-through.
    if (has_graph_coloring_)
        SpillLiveColoredForCall(false);
    if (has_graph_coloring_) {
        // Spill colored FPRs (same logic: StoreFpr with graph coloring skips
        // the stack write).  Caller-colored FPRs are excluded — StoreFpr
        // already does write-through for them.
        for (uint32_t fi = 0; fi < 32; ++fi) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (caller_fpr_colored_mask_ & (1ULL << fi))
                    continue;
                enc_.EmitMovdqaMR(AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)), colored_xmm);
            }
        }
    }
    enc_.EmitCallReg(reg);
    // Post-call reload: restore caller-saved colored vregs (R8-R11, colors 1-4)
    // that were clobbered by the call.  Only reload caller-colored vregs that are
    // live across a call (cross_call_mask_) — those are the only ones kept current
    // via write-through.  Vregs not in cross_call_mask_ have call-free live ranges
    // and must NOT be reloaded (their stack slots are stale); their registers stay
    // valid because no call overlaps their def→use.
    if (has_graph_coloring_) {
        uint64_t mask = caller_colored_mask_ & cross_call_mask_;
        for (uint32_t vr = 0; mask; ++vr) {
            if (mask & 1) {
                uint8_t colored_x64 = gcr_.gpr_color[vr];
                enc_.EmitMovRM(colored_x64, AT::kStackReg, static_cast<int32_t>(GprOff(vr)));
            }
            mask >>= 1;
        }
    }
    // Post-call reload: restore caller-saved colored FPRs.
    if (has_graph_coloring_ && caller_fpr_colored_mask_) {
        uint64_t mask = caller_fpr_colored_mask_;
        for (uint32_t fi = 0; mask; ++fi) {
            if (mask & 1) {
                uint8_t colored_xmm = gcr_.fpr_color[fi];
                enc_.EmitMovdqaRM(colored_xmm, AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)));
            }
            mask >>= 1;
        }
    }
}

uint32_t NativeCodeGenerator::EmitRuntimeHelperCallImpl(void* target_fn) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::RuntimeHelperCall");
    if (config_.enable_register_caching && cached_slots_used_)
        SpillCachedRegs();

    // On Linux (System V ABI), convert args from Win64 positions (RCX, RDX, R8, R9)
    // to SysV positions (RDI, RSI, RDX, RCX, R8, R9). The JIT engine always sets up
    // args in Win64 positions internally.
#if !defined(_WIN32) && !defined(_WIN64) && defined(__x86_64__)
    // EmitMovRR(dst, src) — dst and src are x86-64 register numbers
    enc_.EmitMovRR(7, 1); // RDI = RCX  (arg1 fixup — RCX→RDI)
    enc_.EmitMovRR(6, 2); // RSI = RDX  (arg2 fixup — RDX→RSI)
    enc_.EmitMovRR(2, 8); // RDX = R8   (arg3 fixup — R8→RDX)
    // R9 stays in R9 (SysV arg6 = R9, same as Win64 arg4 — no 4-arg helpers currently)
#endif

    // 省写穿 (Phase 1): spill caller-colored GPRs live at this helper call to
    // their fixed stack slots (not write-through on every def).  Arg-register-
    // colored vregs (RCX/RDX/R8/R9) are excluded — argument setup has already
    // written them before this spill; they keep write-through (see
    // SpillLiveColoredForCall).  The SysV arg fixup above moves args into
    // RDI/RSI/RDX; on Win64 (our test path) those aren't Win64 arg regs so this
    // is safe there.  FPRs stay conservative (spill all non-caller-colored).
    if (has_graph_coloring_)
        SpillLiveColoredForCall(false);
    if (has_graph_coloring_) {
        // Spill colored FPRs (same logic: StoreFpr skips stack write).
        for (uint32_t fi = 0; fi < 32; ++fi) {
            uint8_t colored_xmm = gcr_.fpr_color[fi];
            if (colored_xmm != 0xFF) {
                if (caller_fpr_colored_mask_ & (1ULL << fi))
                    continue;
                enc_.EmitMovdqaMR(AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)), colored_xmm);
            }
        }
    }
    uint32_t call_start = buf_.pos();
    enc_.EmitCallRipRel(0);
    slot_patches_.push_back({call_start + kSlotPatchDispOff, UINT32_MAX, target_fn});
    slot_count_used_++;
    // Post-call reload: restore caller-saved colored GPRs that are live across a
    // call (cross_call_mask_), matching EmitCallWithSpill / Call.  Both
    // arg-register (kept current by write-through) and non-arg (spilled by
    // SpillLiveColoredForCall above) caller-colored vregs lived at this site
    // have a current slot to reload from.
    if (has_graph_coloring_ && (caller_colored_mask_ & cross_call_mask_)) {
        uint64_t mask = caller_colored_mask_ & cross_call_mask_;
        for (uint32_t vr = 0; mask; ++vr) {
            if (mask & 1) {
                uint8_t colored_x64 = gcr_.gpr_color[vr];
                enc_.EmitMovRM(colored_x64, AT::kStackReg, static_cast<int32_t>(GprOff(vr)));
            }
            mask >>= 1;
        }
    }
    // Post-call reload: restore caller-saved colored FPRs.
    if (has_graph_coloring_ && caller_fpr_colored_mask_) {
        uint64_t mask = caller_fpr_colored_mask_;
        for (uint32_t fi = 0; mask; ++fi) {
            if (mask & 1) {
                uint8_t colored_xmm = gcr_.fpr_color[fi];
                enc_.EmitMovdqaRM(colored_xmm, AT::kStackReg, static_cast<int32_t>(FprOff(kGprCount + fi)));
            }
            mask >>= 1;
        }
    }
    return call_start;
}

} // namespace chaos::il2cpp::jit
