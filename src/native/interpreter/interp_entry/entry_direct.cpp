// entry_direct.cpp — InterpreterEntryDirect hot path
#include <cstdio>
#include <tier_manager.h>

#include "vtable_registry.h"

#include <jit_engine.h>
#include <jit_method.h>
#include <jit_helpers.h>
#include <jit_seh.h>

#include <codegen_bridge.h>
#include <gc_root_scanner.h>
#include <gc_transition.h>
#include <patch_loader.h>

#include <method_replacement.h>
#include <osr_state.h>
#include "register_vm_profiler.h"

#include <chaos/log.h>
#include <chaos/pal/pal_mem.h>
#include <chaos/runtime/execution_config.h>

#include <stdexcept>
#include <vector>

namespace chaos::il2cpp::runtime_core {

// Forward declaration of the interpreter frame scanner registration.
void RegisterInterpFrameScanner() noexcept;

// ── Tier hit counters (profile instrumentation) ─────────────────────────
struct TierCounters {
    std::atomic<uint64_t> step_1c       = 0;
    std::atomic<uint64_t> step_reg      = 0;
    std::atomic<uint64_t> step_fast     = 0;
    std::atomic<uint64_t> step_vm       = 0;
    std::atomic<uint64_t> step_native   = 0;
    std::atomic<uint64_t> deopt_t4      = 0;
};

static TierCounters& GetTierCounters() {
    static TierCounters counters;
    return counters;
}

// ── TypedCatchCheck: parent-chain walk for typed catch matching ───────────
// Reads TypeInfoHot* from exc_obj at offset 0, resolves class_token to
// TypeInfoHot* via module registry scan, walks parent chain.
static bool TypedCatchCheck(void* exc_obj, uint32_t class_token) noexcept {
    using namespace chaos::il2cpp::runtime_core;
    if (exc_obj == nullptr) return false;
    const TypeInfoHot* exc_type = *static_cast<const TypeInfoHot* const*>(exc_obj);
    if (exc_type == nullptr) return false;

    // Scan all registered modules for the target type by class_token.
    const TypeInfoHot* target_type = nullptr;
    uint32_t raw_index = (class_token & 0x00FFFFFFu);
    uint32_t type_index = raw_index > 0 ? raw_index - 1 : 0;
    uint32_t module_count = GetModuleCount();
    for (uint32_t mi = 0; mi < module_count; ++mi) {
        const auto* mod = GetModuleByIndex(mi);
        if (mod == nullptr || mod->type_info_ptrs == nullptr || mod->image == nullptr) continue;
        if (type_index < mod->type_count) {
            const auto* desc = mod->image->types[type_index];
            if (desc != nullptr && desc->metadata_token == class_token) {
                target_type = mod->type_info_ptrs[type_index];
                break;
            }
        }
    }
    if (target_type == nullptr) return false;

    // Walk parent chain.
    const TypeInfoHot* current = exc_type;
    while (current != nullptr) {
        if (current == target_type) return true;
        current = current->parent;
    }
    return false;
}

// ── InterpreterEntryDirect ──────────────────────────────────────────────

static void CacheSignature(PatchMethod* patch_method) noexcept;
static interpreter::InterpreterValue ReadTypedArg(
    ArgBuffer& reader, interpreter::ValueTag tag) noexcept;
static void WriteTypedRet(void* ret_buf, const interpreter::ExecutionResult& result,
                           interpreter::ValueTag ret_tag) noexcept;

void InlineLeafCallees(
    interpreter::IRMethod& ir,
    PatchMethod& patch_method) noexcept;

// ── Tier 2 IR re-optimization helpers ─────────────────────────────────────

static interpreter::IRMethod CloneIRMethod(const interpreter::IRMethod& src) noexcept {
    interpreter::IRMethod dst;
    dst.instructions = src.instructions;
    dst.seh_clauses  = src.seh_clauses;
    return dst;
}

struct FusionStats {
    uint32_t dead_pops_removed    = 0;
    uint32_t dup_pop_cancelled    = 0;
    uint32_t redundant_locals     = 0;
    uint32_t ldnull_stloc_fused   = 0;
    uint32_t ldc_add_fused        = 0;
};

static FusionStats FusePass(interpreter::IRMethod& ir) noexcept {
    FusionStats stats;
    auto& instrs = ir.instructions;
    if (instrs.size() < 2) return stats;

    std::vector<interpreter::IRInstruction> fused;
    fused.reserve(instrs.size());

    for (size_t i = 0; i < instrs.size(); ++i) {
        const auto& op = instrs[i];

        if (op.op_code == interpreter::IROpCode::Pop) {
            size_t j = i;
            while (j < instrs.size() &&
                   instrs[j].op_code == interpreter::IROpCode::Pop) ++j;
            if (j < instrs.size() &&
                instrs[j].op_code == interpreter::IROpCode::Ret) {
                stats.dead_pops_removed += static_cast<uint32_t>(j - i);
                i = j - 1; continue;
            }
            fused.push_back(op); continue;
        }

        if (op.op_code == interpreter::IROpCode::Dup &&
            i + 1 < instrs.size() &&
            instrs[i + 1].op_code == interpreter::IROpCode::Pop) {
            ++stats.dup_pop_cancelled; ++i; continue;
        }

        if (op.op_code == interpreter::IROpCode::LdLoc &&
            i + 1 < instrs.size() &&
            instrs[i + 1].op_code == interpreter::IROpCode::StLoc &&
            op.operand_index == instrs[i + 1].operand_index) {
            ++stats.redundant_locals; ++i; continue;
        }

        fused.push_back(op);
    }

    if (fused.size() < instrs.size()) instrs.swap(fused);
    return stats;
}

static void FuseInstructions(interpreter::IRMethod& ir) noexcept {
    FusionStats total;
    for (int pass = 0; pass < 4; ++pass) {
        auto stats = FusePass(ir);
        total.dead_pops_removed  += stats.dead_pops_removed;
        total.dup_pop_cancelled  += stats.dup_pop_cancelled;
        total.redundant_locals   += stats.redundant_locals;
        total.ldnull_stloc_fused += stats.ldnull_stloc_fused;
        if (stats.dead_pops_removed == 0 &&
            stats.dup_pop_cancelled == 0 &&
            stats.redundant_locals == 0) break;
    }
    CHAOS_IL2CPP_LOG_DEBUG_M("tier", "FuseInstructions: removed {} dead Pops, "
                           "{} Dup+Pop pairs, {} redundant LdLoc+StLoc",
                           total.dead_pops_removed, total.dup_pop_cancelled, total.redundant_locals);
}

bool OptimizeToTier2(PatchMethod* pm) noexcept {
    if (pm == nullptr) return false;
    auto* orig_ir = static_cast<interpreter::IRMethod*>(pm->cached_ir);
    if (orig_ir == nullptr) return false;
    // SEH-containing methods are now accepted — AllocateRegisters has seh_clauses
    // support (ir_reg_alloc.h:291-295) and RegisterExecute executes flat without
    // SEH dispatch, which is correct when all call targets resolve via the
    // interp dispatch loop.  InterpreterVM fallback (Step 4) still has full SEH
    // handling for any edge cases.
    if (orig_ir->instructions.size() <= 2) return false;

    auto cloned_ir = CloneIRMethod(*orig_ir);
    InlineLeafCallees(cloned_ir, *pm);
    FuseInstructions(cloned_ir);
    auto optimized_rm = interpreter::AllocateRegisters(cloned_ir);
    if (optimized_rm.instructions.empty()) return false;

    auto* storage = static_cast<interpreter::RegisterMethod*>(
        CHAOS_IL2CPP_MALLOC(sizeof(interpreter::RegisterMethod)));
    if (storage == nullptr) return false;
    ::new (storage) interpreter::RegisterMethod(std::move(optimized_rm));
    pm->cached_optimized_reg_method = storage;
    return true;
}

// ── PIC profile + generation functions ─────────────────────────────────────

static uint32_t CountCallVirtInstructions(const interpreter::RegisterMethod& rm) noexcept {
    uint32_t count = 0;
    for (const auto& instr : rm.instructions) {
        if (instr.op_code() == interpreter::IROpCode::CallVirt ||
            instr.op_code() == interpreter::IROpCode::CallVirtConstrained) ++count;
    }
    return count;
}

bool EnsureCallSiteProfiles(PatchMethod* pm) noexcept {
    if (pm == nullptr) return false;
    if (pm->call_site_profiles != nullptr) return true;
    auto* rm = static_cast<interpreter::RegisterMethod*>(pm->cached_optimized_reg_method);
    if (rm == nullptr) return false;
    uint32_t callvirt_count = CountCallVirtInstructions(*rm);
    if (callvirt_count == 0) return false;
    auto* profiles = static_cast<CallSiteProfile*>(std::calloc(callvirt_count, sizeof(CallSiteProfile)));
    if (profiles == nullptr) return false;
    uint32_t prof_idx = 0;
    for (uint32_t i = 0; i < static_cast<uint32_t>(rm->instructions.size()); ++i) {
        if (rm->instructions[i].op_code() == interpreter::IROpCode::CallVirt ||
            rm->instructions[i].op_code() == interpreter::IROpCode::CallVirtConstrained) {
            if (prof_idx < callvirt_count) { profiles[prof_idx].instruction_idx = i; ++prof_idx; }
        }
    }
    pm->call_site_profiles = profiles;
    pm->call_site_profile_count = callvirt_count;
    return true;
}

void SampleCallVirtProfile(PatchMethod* pm, uint32_t instruction_idx, uint64_t receiver_type_token) noexcept {
    if (pm == nullptr || pm->call_site_profiles == nullptr) return;
    auto* profiles = static_cast<CallSiteProfile*>(pm->call_site_profiles);
    uint32_t count = pm->call_site_profile_count;
    for (uint32_t i = 0; i < count; ++i) {
        if (profiles[i].instruction_idx != instruction_idx) continue;
        auto& prof = profiles[i];
        ++prof.sample_count;
        for (uint32_t j = 0; j < prof.type_count; ++j) {
            if (prof.type_tokens[j] == receiver_type_token) { ++prof.type_hit_counts[j]; return; }
        }
        if (prof.type_count < 4) {
            prof.type_tokens[prof.type_count] = receiver_type_token;
            prof.type_hit_counts[prof.type_count] = 1;
            ++prof.type_count;
        } else {
            uint32_t min_idx = 0;
            for (uint32_t j = 1; j < 4; ++j) {
                if (prof.type_hit_counts[j] < prof.type_hit_counts[min_idx]) min_idx = j;
            }
            prof.type_tokens[min_idx] = receiver_type_token;
            prof.type_hit_counts[min_idx] = 1;
        }
        return;
    }
}

bool GeneratePICData(PatchMethod* pm) noexcept {
    if (pm == nullptr || pm->call_site_profiles == nullptr) return false;
    if (pm->pic_dispatch_data != nullptr) return true;
    auto* rm = static_cast<interpreter::RegisterMethod*>(pm->cached_optimized_reg_method);
    if (rm == nullptr) return false;
    auto* profiles = static_cast<CallSiteProfile*>(pm->call_site_profiles);
    uint32_t count = pm->call_site_profile_count;
    if (count == 0) return false;
    uint8_t* alloc_base = static_cast<uint8_t*>(std::calloc(1, sizeof(uint32_t) + count * sizeof(PicDispatchChain)));
    if (alloc_base == nullptr) return false;
    *reinterpret_cast<uint32_t*>(alloc_base) = count;
    auto* chains = reinterpret_cast<PicDispatchChain*>(alloc_base + sizeof(uint32_t));
    uint32_t instruct_count = static_cast<uint32_t>(rm->instructions.size());
    CHAOS_IL2CPP_VECTOR(uint32_t) inst_method_tokens(instruct_count, 0);
    for (uint32_t ii = 0; ii < instruct_count; ++ii) {
        const auto& ri = rm->instructions[ii];
        if (ri.op_code() == interpreter::IROpCode::CallVirt ||
            ri.op_code() == interpreter::IROpCode::CallVirtConstrained) {
            const auto* desc = TryDecodeReflectionQueryMethodHandle(
                static_cast<MethodInfoHandle>(reinterpret_cast<uintptr_t>(ri.imm.ptr)));
            inst_method_tokens[ii] = (desc != nullptr) ? desc->metadata_token : 0;
        }
    }
    uint32_t generated_count = 0;
    uint64_t current_gen = g_patch_generation.load(std::memory_order_relaxed);
    for (uint32_t i = 0; i < count; ++i) {
        const auto& prof = profiles[i];
        if (prof.sample_count < 4) continue;
        uint32_t total = 0;
        for (uint32_t j = 0; j < prof.type_count; ++j) total += prof.type_hit_counts[j];
        if (total == 0) continue;
        uint64_t sorted_tokens[4] = {};
        uint32_t sorted_hits[4] = {};
        for (uint32_t j = 0; j < prof.type_count; ++j) { sorted_tokens[j] = prof.type_tokens[j]; sorted_hits[j] = prof.type_hit_counts[j]; }
        for (uint32_t a = 0; a < prof.type_count; ++a)
            for (uint32_t b = a + 1; b < prof.type_count; ++b)
                if (sorted_hits[b] > sorted_hits[a]) { std::swap(sorted_tokens[a], sorted_tokens[b]); std::swap(sorted_hits[a], sorted_hits[b]); }
        auto& chain = chains[i];
        chain.generation = static_cast<uint32_t>(current_gen);
        chain.instruction_idx = prof.instruction_idx;
        chain.method_token = (prof.instruction_idx < instruct_count) ? inst_method_tokens[prof.instruction_idx] : 0;
        float top1_ratio = static_cast<float>(sorted_hits[0]) / static_cast<float>(total);
        uint32_t slots_to_fill = 0;
        if (top1_ratio > 0.90f) slots_to_fill = 1;
        else {
            uint32_t cumulative = sorted_hits[0];
            for (uint32_t s = 1; s < prof.type_count && s < 3; ++s) {
                cumulative += sorted_hits[s];
                if (static_cast<float>(cumulative) / static_cast<float>(total) > 0.95f) { slots_to_fill = s + 1; break; }
            }
        }
        for (uint32_t s = 0; s < slots_to_fill && s < 3; ++s) {
            chain.slots[s].type_token = sorted_tokens[s];
            if (chain.method_token != 0 && sorted_tokens[s] != 0)
                chain.slots[s].direct_fn = vtable_registry::ResolveVirtualMethodPointer(
                    static_cast<uint32_t>(sorted_tokens[s]), chain.method_token);
        }
        ++generated_count;
    }
    if (generated_count == 0) { std::free(alloc_base); return false; }
    pm->pic_dispatch_data = alloc_base;
    return true;
}

void RebuildCallCacheForT3(PatchMethod* pm) noexcept {
    if (pm == nullptr) return;
    auto* rm = static_cast<interpreter::RegisterMethod*>(pm->cached_optimized_reg_method);
    if (rm == nullptr) return;
    uint32_t instr_count = static_cast<uint32_t>(rm->instructions.size());
    if (instr_count == 0) return;
    auto* cc = static_cast<runtime_instantiation::CachedCallInfo*>(
        CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(instr_count * sizeof(runtime_instantiation::CachedCallInfo)));
    if (cc == nullptr) return;
    for (uint32_t i = 0; i < instr_count; ++i) {
        const auto& ri = rm->instructions[i];
        auto op_int = static_cast<int>(ri.op_code());
        switch (op_int) {
        case static_cast<int>(interpreter::IROpCode::Call):
        case static_cast<int>(interpreter::IROpCode::Calli):
        case static_cast<int>(interpreter::IROpCode::CallVirt):
        case static_cast<int>(interpreter::IROpCode::CallBridge):
        case static_cast<int>(interpreter::IROpCode::CallVirtConstrained):
            cc[i] = (ri.imm.ptr != nullptr) ? runtime_instantiation::PrecacheCallTarget(ri.imm.ptr) : runtime_instantiation::CachedCallInfo{};
            if (ri.imm.ptr == nullptr) cc[i].ret_tag = 0xFF;
            break;
        default: cc[i].ret_tag = 0xFF; break;
        }
    }
    pm->call_cache = cc;
    // Release fence: ensure call_cache pointer is visible to readers (FastExecute,
    // RegisterExecute) that may load it concurrently.  Domain heap allocation
    // prevents UAF, but without the fence a reader may see stale call_cache data
    // on weak memory models.
    std::atomic_thread_fence(std::memory_order_release);
}

static void FreeCallSiteProfiles(PatchMethod* pm) noexcept {
    if (pm == nullptr) return;
    std::free(pm->call_site_profiles); pm->call_site_profiles = nullptr; pm->call_site_profile_count = 0;
}
static void FreePICData(PatchMethod* pm) noexcept {
    if (pm == nullptr) return;
    std::free(pm->pic_dispatch_data); pm->pic_dispatch_data = nullptr;
}

void PromoteToTier3(PatchMethod* pm) noexcept {
    if (pm == nullptr) return;
    if (GeneratePICData(pm)) { }
    FreeCallSiteProfiles(pm);
}

static std::once_flag g_tier3_cb_once;
static void RegisterTier3CallbackFn() noexcept {
    SetTier3PromotionCallback([](PatchMethod* pm) noexcept {
        PromoteToTier3(pm);
        RebuildCallCacheForT3(pm);
    });
}

// ── CompileAndCacheEntry (entry_direct) ─────────────────────────────────────
// Compile a T3→T4 native method and cache it on the PatchMethod.
// Called when EvaluateTierPromotion returns kCompileToNative.
// Entry_direct-specific: populates PIC data, dispatch_ctx, call_cache,
// arg_type_tags in the CompileConfig, and sets dispatch_entry->direct_ptr
// for the Step A0/QuickJit dispatch path.
static void CompileAndCacheEntry(
    PatchMethod* pm,
    runtime_instantiation::InterpreterDispatchContext* dispatch_ctx) noexcept {

    auto* reg_m = static_cast<interpreter::RegisterMethod*>(pm->cached_optimized_reg_method);
    if (reg_m == nullptr) reg_m = static_cast<interpreter::RegisterMethod*>(pm->cached_reg_method);
    if (reg_m == nullptr || !jit::CanCompile(*reg_m)) {
        ++pm->codegen_fail_count;
        if (pm->codegen_fail_count >= PatchMethod::kMaxCodegenFailures) {
            pm->tier_state.store(PatchMethod::kJitSkip, std::memory_order_release);
        } else {
            pm->tier_state.store(PatchMethod::kOptimizedRegister, std::memory_order_release);
        }
        return;
    }

    jit::CompileConfig cfg;
    cfg.enable_safepoint_polls = true;
    cfg.enable_liveness = true;
    cfg.safepoint_fn = reinterpret_cast<void*>(&threading::SafepointPoll);
    cfg.cooperative_fn = reinterpret_cast<void*>(&threading::EnterCooperativeMode);
    cfg.preemptive_fn = reinterpret_cast<void*>(&threading::EnterPreemptiveMode);
    cfg.pic_dispatch_data = pm->pic_dispatch_data;
    cfg.per_instr_pic_count = pm->reg_ir_instr_count;
    if (cfg.pic_dispatch_data != nullptr && cfg.per_instr_pic_count > 0) {
        const auto* data_ptr = static_cast<const uint8_t*>(cfg.pic_dispatch_data);
        uint32_t chain_count = *reinterpret_cast<const uint32_t*>(data_ptr);
        const auto* chains = reinterpret_cast<const PicDispatchChain*>(
            data_ptr + sizeof(uint32_t));
        auto* pic_arr = static_cast<PerInstrPicData*>(
            std::calloc(cfg.per_instr_pic_count, sizeof(PerInstrPicData)));
        for (uint32_t ci = 0; ci < chain_count; ++ci) {
            uint32_t ii = chains[ci].instruction_idx;
            if (ii >= cfg.per_instr_pic_count) continue;
            auto& pd = pic_arr[ii];
            for (uint32_t s = 0; s < 3; ++s) {
                if (chains[ci].slots[s].type_token != 0 &&
                    chains[ci].slots[s].direct_fn != nullptr) {
                    pd.expected_type_tokens[s] = static_cast<uint32_t>(
                        chains[ci].slots[s].type_token);
                    pd.direct_fns[s] = chains[ci].slots[s].direct_fn;
                    pd.slot_count = s + 1;
                }
            }
        }
        cfg.per_instr_pic = pic_arr;
    }
    cfg.dispatch_ctx = dispatch_ctx;
    cfg.call_cache = pm->call_cache;
    cfg.call_cache_count = pm->reg_ir_instr_count;
    cfg.arg_type_tags = pm->cached_sig_valid ? pm->cached_arg_types : nullptr;
    cfg.arg_type_count = pm->cached_arg_count;
    cfg.method_token = pm->token;
    cfg.method_module_id = pm->module_id;
    auto* nm = jit::Compile(*reg_m, cfg);
    if (cfg.per_instr_pic != nullptr) {
        std::free(const_cast<PerInstrPicData*>(cfg.per_instr_pic));
    }
    auto* old_nm = pm->cached_native_method;
    pm->cached_native_method = nm;
    if (nm != nullptr) {
        // Unregister old GC slot map from previous compilation (e.g. Quick JIT
        // promotion to Full JIT).  Without this, GC sees two slot maps for the
        // same method — the old one may reference stale code regions.
        if (old_nm != nullptr && old_nm->slot_map_data != nullptr) {
            GcUnregisterSlotMap(old_nm->code);
        }
        if (nm->slot_map_data != nullptr) {
            GcRegisterSlotMap(nm->code,
                static_cast<const GcSlotMapV0*>(nm->slot_map_data));
        }
        ::chaos::il2cpp::jit::RegisterNativeCodeSection(nm->code, nm->code_size, nm, pm->token);
        // Sync to dispatch entry for Step A0/QuickJit direct dispatch.
        if (auto* entry = static_cast<HotpatchEntryV0*>(pm->dispatch_entry); entry != nullptr) {
            entry->direct_ptr = nm->code;
            // Release fence: ensure direct_ptr write is visible to any thread that
            // load-acquires tier_state == kJitted (set in EvaluateTierPromotion
            // before this function was called).  Without this fence, a reader may
            // see kJitted but stale direct_ptr on ARM64 weak memory model.
            std::atomic_thread_fence(std::memory_order_release);
        }
    } else {
        ++pm->codegen_fail_count;
        if (pm->codegen_fail_count >= PatchMethod::kMaxCodegenFailures) {
            pm->tier_state.store(PatchMethod::kJitSkip, std::memory_order_release);
        } else {
            pm->tier_state.store(PatchMethod::kOptimizedRegister, std::memory_order_release);
        }
    }
}

// ── QuickJitAndCacheEntry — Quick JIT compilation (no optimizer) ───────────
// Called from TryTierUpgrade when EvaluateTierPromotion returns kQuickJit.
// Compiles with enable_optimizer=false for fast warmup.  The resulting
// JitMethod is cached in cached_native_method and the entry point is set
// as dispatch_entry->direct_ptr so subsequent calls go directly to native code.
// Thread-safety: caller already won the CAS for kQuickJitted.
static void QuickJitAndCacheEntry(
    PatchMethod* pm,
    runtime_instantiation::InterpreterDispatchContext* dispatch_ctx) noexcept {

    auto* reg_m = static_cast<interpreter::RegisterMethod*>(pm->cached_reg_method);
    if (reg_m == nullptr || reg_m->instructions.empty()) return;

    jit::CompileConfig cfg;
    cfg.enable_safepoint_polls = true;
    cfg.enable_liveness = true;
    cfg.safepoint_fn = reinterpret_cast<void*>(&threading::SafepointPoll);
    cfg.cooperative_fn = reinterpret_cast<void*>(&threading::EnterCooperativeMode);
    cfg.preemptive_fn = reinterpret_cast<void*>(&threading::EnterPreemptiveMode);
    cfg.dispatch_ctx = dispatch_ctx;
    cfg.call_cache = pm->call_cache;
    cfg.call_cache_count = pm->reg_ir_instr_count;
    cfg.arg_type_tags = pm->cached_sig_valid ? pm->cached_arg_types : nullptr;
    cfg.arg_type_count = pm->cached_arg_count;
    cfg.method_token = pm->token;
    cfg.method_module_id = pm->module_id;
    cfg.enable_pgo = true;          // PGO for Quick JIT -> Tier 1 upgrade
    if (reg_m->seh_clauses.empty()) {
        cfg.compile_tier = jit::CompileTier::kQuick;  // True Quick JIT: stack-only, no regalloc/liveness/deopt/SEH/unwind
    } else {
        cfg.enable_optimizer = false;  // SEH: full tier, skip optimizer
    }

    auto* jit = jit::Compile(*reg_m, cfg);
    pm->cached_native_method = jit;
    if (jit != nullptr) {
        if (jit->slot_map_data != nullptr) {
            GcRegisterSlotMap(jit->code,
                static_cast<const GcSlotMapV0*>(jit->slot_map_data));
        }
        ::chaos::il2cpp::jit::RegisterNativeCodeSection(jit->code, jit->code_size, jit, pm->token);

        // Atomically patch direct_ptr so subsequent dispatch goes straight to native.
        // (Only if the dispatch entry still points to us — entry may have been unpatched.)
        auto* entry = static_cast<HotpatchEntryV0*>(pm->dispatch_entry);
        if (entry == nullptr) {
            // Fallback: resolve via registry (method not from SetPatchedBySlot).
            entry = GetHotpatchNameRegistry()
                .GetDispatchEntryBySlot(pm->module_id,
                    GetHotpatchNameRegistry().TokenToSlot(pm->module_id, pm->token));
        }
        if (entry != nullptr) {
            entry->direct_ptr = jit->code;
            // Release fence: ensure direct_ptr write is visible to any thread that
            // load-acquires tier_state == kQuickJitted (set in EvaluateTierPromotion
            // before this function was called).  Without this fence, a reader may
            // see kQuickJitted but stale direct_ptr on ARM64 weak memory model.
            std::atomic_thread_fence(std::memory_order_release);
        }
    } else {
        ++pm->codegen_fail_count;
        // Quick JIT failed — fall back to stack-interpreted, let normal tiering try later.
        pm->tier_state.store(PatchMethod::kStackInterpreted, std::memory_order_release);
    }
}
// Called once per InterpreterEntryDirect invocation, before Step B/C branch.
// Delegates all CAS + threshold decisions to EvaluateTierPromotion,
// executes the returned action.
static void TryTierUpgrade(PatchMethod* patch_method, uint32_t call_count,
                           runtime_instantiation::InterpreterDispatchContext* dispatch_ctx) noexcept {
    auto action = TierManager::EvaluateTierPromotion(patch_method, call_count);

    switch (action) {
    case TierManager::PromotionAction::kQuickJit:
#if CHAOS_IL2CPP_ENABLE_JIT
        if (::chaos::il2cpp::pal::PalCanJit()) {
            QuickJitAndCacheEntry(patch_method, dispatch_ctx);
            break;
        }
#endif
        // JIT not available — stay at kStackInterpreted
        break;

    case TierManager::PromotionAction::kPromoteToTier2:
        OptimizeToTier2(patch_method);
        patch_method->tier_state.store(PatchMethod::kRegisterMapped, std::memory_order_release);
        break;

    case TierManager::PromotionAction::kPromoteToTier3:
        if (!TierManager::Get().EnqueueOptimization(patch_method)) {
            patch_method->tier_state.store(PatchMethod::kRegisterMapped, std::memory_order_release);
        }
        break;

    case TierManager::PromotionAction::kCompileToNative:
#if CHAOS_IL2CPP_ENABLE_JIT
        if (::chaos::il2cpp::pal::PalCanJit()) {
            CompileAndCacheEntry(patch_method, dispatch_ctx);
            break;
        }
#endif
        // JIT not available — permanently skip so we never retry
        patch_method->tier_state.store(PatchMethod::kJitSkip, std::memory_order_release);
        break;

    default:
        break;
    }
}

void InterpreterEntryDirect(
    uintptr_t method_key,
    void*     args_buf,
    void*     ret_buf) noexcept {

    CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect");
    std::call_once(g_tier3_cb_once, RegisterTier3CallbackFn);
    // Register the interpreter frame scanner for precise GC root scanning.
    static std::once_flag g_interp_scanner_once;
    std::call_once(g_interp_scanner_once, RegisterInterpFrameScanner);
    if (method_key == 0) {
        std::fprintf(stderr, "[INTERP-SILENT] null method_key\n");
        std::fflush(stderr);
        return;
    }
    {
        auto* pm = reinterpret_cast<PatchMethod*>(method_key);
        // DBG-PM (removed)
    }

    CHAOS_IL2CPP_ASSERT(threading::tls_this_thread != nullptr
        && "InterpreterEntryDirect: thread TLS not attached");
    CHAOS_IL2CPP_ASSERT(threading::tls_this_thread_id > 0
        && "InterpreterEntryDirect: invalid thread ID");
    CHAOS_IL2CPP_ASSERT(threading::tls_this_thread->gc_mode.load(std::memory_order_relaxed) == kGcModeCooperative
        && "InterpreterEntryDirect: thread not in cooperative mode");

    auto* patch_method = reinterpret_cast<PatchMethod*>(method_key);
    CHAOS_IL2CPP_LOG_INFO("interpreter", "InterpreterEntryDirect entered");

    { CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.Step1_LowerIR");
      PatchMethodLowerIR(method_key); }

    auto* ir = static_cast<interpreter::IRMethod*>(patch_method->cached_ir);
    if (ir == nullptr) {
        std::fprintf(stderr, "[INTERP-SILENT] null IR after lowering for method_key=%p\n", reinterpret_cast<void*>(method_key));
        std::fflush(stderr);
        return;
    }
    if (ir->instructions.empty()) {
        std::fprintf(stderr, "[INTERP-SILENT] empty IR instructions for method_key=%p\n", reinterpret_cast<void*>(method_key));
        std::fflush(stderr);
        return;
    }
    const auto instr_count = ir->instructions.size();
    // ── Binary IR fallback: placeholder cached_ir ─────────────────────
    // When the patch method uses pre-allocated register IR (v2+ binary
    // path), cached_ir contains a 1-instr placeholder while the real
    // body lives in cached_reg_method.  Route to tier upgrade and then
    // RegisterExecute (Step B) instead of returning — ensures real
    // method body is executed, ret_buf is written, and call_count is
    // incremented for tier promotion.
    //
    // Genuine 1-instr methods (e.g. empty void stubs with no reg_method)
    // still return fast at this point.
    if (instr_count == 1) {
        auto* reg_m = static_cast<interpreter::RegisterMethod*>(
            patch_method->cached_reg_method);
        if (reg_m == nullptr || reg_m->instructions.empty()) {
            return;
        }
        // Fall through to tier upgrade + RegisterExecute below.
    }

    // ── Step 1c: 2-instr fast path ───────────────────────────────────
    if (instr_count == 2 && ir->seh_clauses.empty()) {
        CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.Step1c_2InstrFastPath");
        const auto& op0 = ir->instructions[0];
        const auto& op1 = ir->instructions[1];
        if (op1.op_code == interpreter::IROpCode::Ret) {
            GetTierCounters().step_1c.fetch_add(1, std::memory_order_relaxed);
            if (!patch_method->cached_sig_valid) CacheSignature(patch_method);
            // [Full 2-instr fast path follows; kept minimal here for clarity]
            if (op0.op_code == interpreter::IROpCode::LdArg) {
                if (ret_buf != nullptr) {
                    ArgBuffer args(args_buf); ArgBuffer ret(ret_buf);
                    if (patch_method->cached_sig_valid) {
                        auto ret_tag = static_cast<interpreter::ValueTag>(patch_method->cached_ret_tag);
                        switch (ret_tag) {
                        case interpreter::ValueTag::Int32: ret.WriteI32(args.ReadI32()); return;
                        case interpreter::ValueTag::Int64: ret.WriteI64(args.ReadI64()); return;
                        case interpreter::ValueTag::Float32: ret.WriteF32(args.ReadF32()); return;
                        case interpreter::ValueTag::Float64: ret.WriteF64(args.ReadF64()); return;
                        default: ret.WritePtr(args.ReadPtr()); return;
                        }
                    }
                    ret.WritePtr(args.ReadPtr());
                }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::LdcI4) {
                if (ret_buf != nullptr) { ArgBuffer ret(ret_buf); ret.WriteI32(op0.immediate_i4); }
                return;
            }
            if (op0.op_code == interpreter::IROpCode::LdNull) {
                if (ret_buf != nullptr) { ArgBuffer ret(ret_buf); ret.WritePtr(nullptr); }
                return;
            }
        }
    }

    // ── Unified tier upgrade check (Phase 1.3) ─────────────────────────
    // Single call_count fetch + tier promotion check, done once per invocation
    // before the Step B/C branch, eliminating duplicated checks.
    //
    // Skip when no further promotion is possible:
    //   - JIT enabled:  terminal tier = kJitted (state 7)
    //   - JIT disabled: terminal tier = kOptimizedRegister (state 4, T3)
    //     since T4 (kCompileToNative) requires JIT.
    // Deopt-demoted methods (kJitSkip) and QuickJIT-native methods also skip.
#if CHAOS_IL2CPP_ENABLE_JIT
    constexpr uint32_t kTerminalTier = PatchMethod::kJitted;
#else
    constexpr uint32_t kTerminalTier = PatchMethod::kOptimizedRegister;
#endif
    if (patch_method->tier_state.load(std::memory_order_acquire) < kTerminalTier) {
        auto tier_call_count = patch_method->call_count.fetch_add(1, std::memory_order_relaxed) + 1;
        auto* rs = GetCurrentRuntimeState();
        auto* ts = GetCurrentThreadState();
        runtime_instantiation::InterpreterDispatchContext tier_dispatch_ctx;
        tier_dispatch_ctx.runtime_state = rs;
        tier_dispatch_ctx.thread_state = ts;
        TryTierUpgrade(patch_method, tier_call_count, &tier_dispatch_ctx);
    }

    CHAOS_IL2CPP_LOG_DEBUG("diag", "method replacement check");
    // ── Check for method replacement BEFORE T4 path ────
    {
        void* replacement = method_replacement::Resolve(patch_method->token);
        if (replacement != nullptr) {
            CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.MethodReplace");
            ::chaos::il2cpp::interpreter::VmProfileScope profiler_scope(
                static_cast<uintptr_t>(patch_method->token));
            using ReplacementFn = void (*)(void*, void*);
            auto repl_fn = reinterpret_cast<ReplacementFn>(replacement);
            repl_fn(args_buf, ret_buf);
            return;
        }
    }



    // ── Native dispatch (consolidated, Phase D) ──────────────────────────
    // Single tier_state load feeds the entire if-else chain, eliminating the
    // cascade where each step re-reads tier_state and may mismatch across
    // Step A / Step A-QuickJit / Step A0.
    struct GcTransitionGuard {
        ~GcTransitionGuard() noexcept { GC_TRANSITION_TO_COOPERATIVE(); }
    };

#if CHAOS_IL2CPP_ENABLE_JIT || CHAOS_IL2CPP_ENABLE_AOT
    auto t4_tier = patch_method->tier_state.load(std::memory_order_acquire);
#endif

#if CHAOS_IL2CPP_ENABLE_JIT
    if (t4_tier >= PatchMethod::kJitted) {
        auto* nm = patch_method->cached_native_method;
        if (nm != nullptr && nm->code != nullptr) {
            GetTierCounters().step_native.fetch_add(1, std::memory_order_relaxed);
            using NativeEntry = void (*)(void*, void*);
            auto native_entry = reinterpret_cast<NativeEntry>(nm->code);
            GC_TRANSITION_TO_PREEMPTIVE();
            GcTransitionGuard gc_guard;
            native_entry(args_buf, ret_buf);
            GC_TRANSITION_TO_COOPERATIVE();

            if (::chaos::il2cpp::jit::g_jit_deopt_state.deopt_happened) {
                GetTierCounters().deopt_t4.fetch_add(1, std::memory_order_relaxed);
                ++patch_method->deopt_count;

                if (patch_method->deopt_count > PatchMethod::kMaxDeoptBeforeDemote) {
                    patch_method->tier_state.store(PatchMethod::kJitSkip, std::memory_order_release);
                    auto* dnm = patch_method->cached_native_method;
                    if (dnm != nullptr) {
                        ::chaos::il2cpp::jit::UnregisterNativeCodeSection(dnm->code);
                        GcUnregisterSlotMap(dnm->code);
                        patch_method->cached_native_method = nullptr;
                    }
                    // Restore dispatch entry to original AOT code so Step A0
                    // falls back to correct AOT code instead of demoted JIT code.
                    if (auto* entry = static_cast<HotpatchEntryV0*>(patch_method->dispatch_entry); entry != nullptr) {
                        entry->direct_ptr = patch_method->original_aot_ptr;
                    }
                    patch_method->deopt_count = 0;
                }
                // Fall through to interpreter (deopt recovery)
            } else {
                if (patch_method->deopt_count > 0) --patch_method->deopt_count;
                return;
            }
        }
    }

    if (t4_tier == PatchMethod::kQuickJitted) {
        auto* qj_entry = static_cast<HotpatchEntryV0*>(patch_method->dispatch_entry);
        if (qj_entry != nullptr && qj_entry->direct_ptr != nullptr) {
            GetTierCounters().step_native.fetch_add(1, std::memory_order_relaxed);
            using NativeEntry = void (*)(void*, void*);
            auto native_entry = reinterpret_cast<NativeEntry>(qj_entry->direct_ptr);
            native_entry(args_buf, ret_buf);
            return;
        }
    }
#endif

#if CHAOS_IL2CPP_ENABLE_AOT
    // Step A0: AOT native code path — no tiering, no deopt needed.
    // No GC_TRANSITION wrapper (AOT code follows the GC contract).
    // Guard: skip if method has JIT code (cached_native_method != nullptr)
    // since JIT code requires GC_TRANSITION from Step A.
    if (patch_method->cached_native_method == nullptr) {
        auto* a0_entry = static_cast<HotpatchEntryV0*>(patch_method->dispatch_entry);
        if (a0_entry != nullptr && a0_entry->direct_ptr != nullptr) {
            GetTierCounters().step_native.fetch_add(1, std::memory_order_relaxed);
            using NativeEntry = void (*)(void*, void*);
            auto native_entry = reinterpret_cast<NativeEntry>(a0_entry->direct_ptr);
            native_entry(args_buf, ret_buf);
            return;
        }
    }
#endif
    {
        auto* reg_m = static_cast<interpreter::RegisterMethod*>(
            patch_method->cached_reg_method);
        if (reg_m != nullptr &&
            ::chaos::il2cpp::jit::g_jit_deopt_state.instr_pc <
                static_cast<uint32_t>(reg_m->stack_map.entries.size()))
        {
            auto& stack_entry =
                reg_m->stack_map.entries[::chaos::il2cpp::jit::g_jit_deopt_state.instr_pc];

            interpreter::OsrState osr;
            interpreter::CaptureNativeFrame(osr,
                ::chaos::il2cpp::jit::g_jit_deopt_state.gpr_file,
                ::chaos::il2cpp::jit::g_jit_deopt_state.fpr_file,
                stack_entry,
                patch_method->cached_arg_count,
                interpreter::OsrState::kMaxLocals,
                ::chaos::il2cpp::jit::g_jit_deopt_state.gpr_tags);
            osr.pc = ::chaos::il2cpp::jit::g_jit_deopt_state.instr_pc;

            FastFrame* ff = tls_frame_pool.Acquire();
            FastFrame ff_fallback;
            bool using_pool = (ff != nullptr);
            if (!using_pool) { ff = &ff_fallback; memset(ff, 0, sizeof(*ff)); }

            void* a5_prev_frame = threading::GetCurrentInterpFrame();
            ff->prev_frame = a5_prev_frame;

            interpreter::RestoreOsrToFastFrame(osr, *ff);
            {
                auto* rs = GetCurrentRuntimeState();
                auto* ts = GetCurrentThreadState();
                runtime_instantiation::InterpreterDispatchContext a5_dispatch_ctx;
                a5_dispatch_ctx.runtime_state = rs;
                a5_dispatch_ctx.thread_state = ts;
                SetupFastFrame(ff, patch_method, args_buf, ir,
                    reinterpret_cast<void*>(runtime_instantiation::InterpreterDispatch),
                    &a5_dispatch_ctx);
            }

            threading::SetCurrentInterpFrame(ff);
            bool ok = FastExecute(*ff,
                ir->instructions.data(),
                static_cast<uint32_t>(ir->instructions.size()));

            if (ok && ff->has_ret && ret_buf != nullptr) {
                auto ret_tag = static_cast<interpreter::ValueTag>(ff->ret_tag);
                ArgBuffer ret_writer(ret_buf);
                switch (ret_tag) {
                case interpreter::ValueTag::Int32:
                    ret_writer.WriteI32(static_cast<int32_t>(ff->ret_val));
                    if (using_pool) tls_frame_pool.Release(ff);
                    ::chaos::il2cpp::jit::g_jit_deopt_state = {};
                    threading::SetCurrentInterpFrame(a5_prev_frame);
                    return;
                case interpreter::ValueTag::Int64:
                    ret_writer.WriteI64(static_cast<int64_t>(ff->ret_val));
                    if (using_pool) tls_frame_pool.Release(ff);
                    ::chaos::il2cpp::jit::g_jit_deopt_state = {};
                    threading::SetCurrentInterpFrame(a5_prev_frame);
                    return;
                case interpreter::ValueTag::Float32: {
                    float v;
                    std::memcpy(&v, &ff->ret_val, sizeof(float));
                    ret_writer.WriteF32(v);
                    if (using_pool) tls_frame_pool.Release(ff);
                    ::chaos::il2cpp::jit::g_jit_deopt_state = {};
                    threading::SetCurrentInterpFrame(a5_prev_frame);
                    return;
                }
                case interpreter::ValueTag::Float64: {
                    double v;
                    std::memcpy(&v, &ff->ret_val, sizeof(double));
                    ret_writer.WriteF64(v);
                    if (using_pool) tls_frame_pool.Release(ff);
                    ::chaos::il2cpp::jit::g_jit_deopt_state = {};
                    threading::SetCurrentInterpFrame(a5_prev_frame);
                    return;
                }
                default:
                    ret_writer.WritePtr(reinterpret_cast<void*>(ff->ret_val));
                    if (using_pool) tls_frame_pool.Release(ff);
                    ::chaos::il2cpp::jit::g_jit_deopt_state = {};
                    threading::SetCurrentInterpFrame(a5_prev_frame);
                    return;
                }
            }

            if (using_pool) tls_frame_pool.Release(ff);
            threading::SetCurrentInterpFrame(a5_prev_frame);
            // Fall through to RegisterExecute (t_deopt_state preserved for Step B)
        }
    }


    CHAOS_IL2CPP_LOG_DEBUG("diag", "Step B (RegisterExecute) entering");
    // ── Step B: RegisterExecute path (Layer R) ─────────────────────────
    auto* reg_method = static_cast<interpreter::RegisterMethod*>(
        patch_method->cached_reg_method);
    bool can_reg = reg_method != nullptr && !reg_method->instructions.empty();
    if (can_reg) {
        GetTierCounters().step_reg.fetch_add(1, std::memory_order_relaxed);
        CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.RegisterExecute");
        if (!patch_method->cached_sig_valid) CacheSignature(patch_method);

        interpreter::RegisterFrame rf = {};
        rf.seh_clauses = ir->seh_clauses.data();
        rf.seh_clause_count = static_cast<uint32_t>(ir->seh_clauses.size());
        rf.catch_handler_entries = reg_method->catch_handler_entries.data();
        rf.catch_handler_count = static_cast<uint32_t>(reg_method->catch_handler_entries.size());
        rf.typed_catch_check = TypedCatchCheck;
        auto* runtime_state = GetCurrentRuntimeState();
        auto* thread_state  = GetCurrentThreadState();
        rf.args = args_buf; rf.arg_count = patch_method->cached_sig_valid ? patch_method->cached_arg_count : 0;
        rf.dispatch_fn = reinterpret_cast<void*>(runtime_instantiation::InterpreterDispatch);
        runtime_instantiation::InterpreterDispatchContext reg_dispatch_ctx;
        reg_dispatch_ctx.runtime_state = runtime_state; reg_dispatch_ctx.thread_state = thread_state;
        rf.dispatch_ctx = &reg_dispatch_ctx;
        rf.call_cache = patch_method->call_cache;
        rf.call_count = static_cast<uint32_t>(instr_count);
        rf.patch_method = patch_method;
        rf.prev_frame = threading::GetCurrentInterpFrame();
        threading::SetCurrentInterpFrame(&rf);

        const interpreter::RegisterInstruction* exec_instrs = reg_method->instructions.data();
        uint32_t exec_instr_count = static_cast<uint32_t>(reg_method->instructions.size());

        // T3 optimized instruction swap (when available from background promotion)
        {   auto tier = patch_method->tier_state.load(std::memory_order_acquire);
            if (tier >= PatchMethod::kRegisterMapped && patch_method->cached_optimized_reg_method != nullptr) {
                auto* opt = static_cast<interpreter::RegisterMethod*>(patch_method->cached_optimized_reg_method);
                exec_instrs = opt->instructions.data();
                exec_instr_count = static_cast<uint32_t>(opt->instructions.size());
                rf.call_cache = patch_method->call_cache;
            }
        }

        // Deoptimization-aware RegisterFrame setup
        bool t4_deopt_happened = ::chaos::il2cpp::jit::g_jit_deopt_state.deopt_happened;
        if (t4_deopt_happened) {
            std::memcpy(rf.regs.gpr, ::chaos::il2cpp::jit::g_jit_deopt_state.gpr_file, 64 * sizeof(uint64_t));
            std::memcpy(rf.regs.fpr, ::chaos::il2cpp::jit::g_jit_deopt_state.fpr_file, 32 * sizeof(double));
            std::memcpy(rf.regs.gpr_tags, ::chaos::il2cpp::jit::g_jit_deopt_state.gpr_tags, 64);
            std::memcpy(rf.regs.fpr_tags, ::chaos::il2cpp::jit::g_jit_deopt_state.fpr_tags, 32);
            // H3: bound-check deopt PC before assignment to prevent out-of-range
            // access in RegisterExecute.  On mismatch, restart from PC 0.
            auto deopt_pc = ::chaos::il2cpp::jit::g_jit_deopt_state.instr_pc;
            if (CHAOS_IL2CPP_UNLIKELY(deopt_pc >= exec_instr_count)) {
                CHAOS_IL2CPP_LOG_WARN_M("jit", "Step B: deopt PC %u out of bounds "
                    "(max %u), restarting from PC 0", deopt_pc, exec_instr_count);
                rf.pc = 0;
            } else {
                rf.pc = deopt_pc;
            }
            rf.osr_reenable = true;  // trigger immediate OSR on first backedge
            ::chaos::il2cpp::jit::g_jit_deopt_state = {};
        }

        CHAOS_IL2CPP_LOG_DEBUG("diag", "Step-B: before RegisterExecute");
        bool ok = interpreter::RegisterExecute(rf, reg_method->instructions.data(),
            static_cast<uint32_t>(reg_method->instructions.size()));
        CHAOS_IL2CPP_LOG_DEBUG_M("diag", "Step-B: RegisterExecute ok=%d", ok);
        if (ok) {
            if (rf.has_ret && ret_buf != nullptr) {
                auto ret_tag = static_cast<interpreter::ValueTag>(rf.ret_tag);
                ArgBuffer ret_writer(ret_buf);
                switch (ret_tag) {
                case interpreter::ValueTag::Int32: ret_writer.WriteI32(static_cast<int32_t>(rf.ret_val)); threading::SetCurrentInterpFrame(rf.prev_frame); return;
                case interpreter::ValueTag::Int64: ret_writer.WriteI64(static_cast<int64_t>(rf.ret_val)); threading::SetCurrentInterpFrame(rf.prev_frame); return;
                case interpreter::ValueTag::Float32: { float v; std::memcpy(&v, &rf.ret_val, sizeof(float)); ret_writer.WriteF32(v); threading::SetCurrentInterpFrame(rf.prev_frame); return; }
                case interpreter::ValueTag::Float64: { double v; std::memcpy(&v, &rf.ret_val, sizeof(double)); ret_writer.WriteF64(v); threading::SetCurrentInterpFrame(rf.prev_frame); return; }
                default: ret_writer.WritePtr(reinterpret_cast<void*>(rf.ret_val)); threading::SetCurrentInterpFrame(rf.prev_frame); return;
                }
            }
            threading::SetCurrentInterpFrame(rf.prev_frame);
            return;
        }
    }

    // ── Step C: FastExecute path (SEH fully supported) ──────────────
    // Step C: only available for v1 JSON path (cached_ir has real instructions).
    // v2 binary path's cached_ir is a 1-instr placeholder; skip.
    if (patch_method->reg_ir_data == nullptr && !ir->instructions.empty()) {
        GetTierCounters().step_fast.fetch_add(1, std::memory_order_relaxed);
        CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.FastExecute");
        if (!patch_method->cached_sig_valid) CacheSignature(patch_method);

        FastFrame* ff = tls_frame_pool.Acquire();
        FastFrame ff_fallback;
        bool using_pool = true;
        if (ff == nullptr) { ff = &ff_fallback; memset(ff, 0, sizeof(*ff)); using_pool = false; }
        ff->prev_frame = threading::GetCurrentInterpFrame();
        ff->ir_instrs = ir->instructions.data();
        ff->instr_count = static_cast<uint32_t>(ir->instructions.size());
        threading::SetCurrentInterpFrame(ff);
        void* step_c_prev_frame = ff->prev_frame;

        { CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.SetupFrame");
          auto* runtime_state = GetCurrentRuntimeState(); auto* thread_state = GetCurrentThreadState();
          runtime_instantiation::InterpreterDispatchContext dispatch_ctx;
          dispatch_ctx.runtime_state = runtime_state; dispatch_ctx.thread_state = thread_state;
          SetupFastFrame(ff, patch_method, args_buf, ir,
                         reinterpret_cast<void*>(runtime_instantiation::InterpreterDispatch), &dispatch_ctx); }

        bool ok;
        { CHAOS_IL2CPP_PROFILE_SCOPE("InterpreterEntryDirect.FastExecuteCall");
          ok = FastExecute(*ff, ir->instructions.data(), static_cast<uint32_t>(ir->instructions.size())); }
        if (ok) {
            if (ff->has_ret && ret_buf != nullptr) {
                auto ret_tag = static_cast<interpreter::ValueTag>(ff->ret_tag);
                ArgBuffer ret_writer(ret_buf);
                switch (ret_tag) {
                case interpreter::ValueTag::Int32: ret_writer.WriteI32(static_cast<int32_t>(ff->ret_val)); if (using_pool) tls_frame_pool.Release(ff); threading::SetCurrentInterpFrame(step_c_prev_frame); return;
                case interpreter::ValueTag::Int64: ret_writer.WriteI64(static_cast<int64_t>(ff->ret_val)); if (using_pool) tls_frame_pool.Release(ff); threading::SetCurrentInterpFrame(step_c_prev_frame); return;
                case interpreter::ValueTag::Float32: { float v; std::memcpy(&v, &ff->ret_val, sizeof(float)); ret_writer.WriteF32(v); if (using_pool) tls_frame_pool.Release(ff); threading::SetCurrentInterpFrame(step_c_prev_frame); return; }
                case interpreter::ValueTag::Float64: { double v; std::memcpy(&v, &ff->ret_val, sizeof(double)); ret_writer.WriteF64(v); if (using_pool) tls_frame_pool.Release(ff); threading::SetCurrentInterpFrame(step_c_prev_frame); return; }
                default: ret_writer.WritePtr(reinterpret_cast<void*>(ff->ret_val)); if (using_pool) tls_frame_pool.Release(ff); threading::SetCurrentInterpFrame(step_c_prev_frame); return;
                }
            }
            if (using_pool) tls_frame_pool.Release(ff);
            threading::SetCurrentInterpFrame(step_c_prev_frame);
            return;
        }
        if (using_pool) tls_frame_pool.Release(ff);
        threading::SetCurrentInterpFrame(step_c_prev_frame);
    }

    CHAOS_IL2CPP_LOG_DEBUG("diag", "Step D (InterpreterVM) entering");
    // ── Step D: InterpreterVM (slow path) ──────────────────────────────
    CHAOS_IL2CPP_UINT32 arg_count = 0;
    bool type_aware_args = false;
    if (!patch_method->cached_sig_valid) CacheSignature(patch_method);
    if (patch_method->cached_sig_valid) {
        arg_count = patch_method->cached_arg_count;
        type_aware_args = true;
    }
    interpreter::ExecutionFrame frame;
    ArgBuffer arg_reader(args_buf);
    frame.arguments.reserve(arg_count);
    if (type_aware_args) {
        for (CHAOS_IL2CPP_UINT32 i = 0; i < arg_count; ++i) {
            auto tag = (i < patch_method->cached_arg_capacity)
                ? static_cast<interpreter::ValueTag>(patch_method->cached_arg_types[i])
                : interpreter::ValueTag::ObjectRef;
            frame.arguments.push_back(ReadTypedArg(arg_reader, tag));
        }
    } else {
        for (CHAOS_IL2CPP_UINT32 i = 0; i < arg_count; ++i)
            frame.arguments.push_back(interpreter::InterpreterValue::from_obj(arg_reader.ReadPtr()));
    }
    // Pre-reserve evaluation stack and locals to avoid reallocation during execution.
    // Stack depth varies by method; 64 is a reasonable default that covers >90% of methods.
    frame.stack.reserve(64);
    frame.locals.reserve(8);
    auto* runtime_state = GetCurrentRuntimeState();
    auto* thread_state  = GetCurrentThreadState();
    runtime_instantiation::InterpreterDispatchContext dispatch_ctx;
    dispatch_ctx.runtime_state = runtime_state; dispatch_ctx.thread_state = thread_state;
    frame.dispatch_fn = runtime_instantiation::InterpreterDispatch;
    frame.dispatch_context = &dispatch_ctx;
    GetTierCounters().step_vm.fetch_add(1, std::memory_order_relaxed);
    interpreter::ExecutionResult result;
    CHAOS_IL2CPP_LOG_INFO("interpreter", "InterpreterVM::Execute entering");
    try {
        interpreter::InterpreterVM vm; result = vm.Execute(*ir, &frame);
    } catch (...) {
        CHAOS_IL2CPP_LOG_ERROR("interpreter", "InterpreterVM::Execute threw (unknown exception)");
        throw;
    }
    if (result.threw_exception) {
        CHAOS_IL2CPP_INTPTR exc_val = 0;
        if (result.exception_value.tag == interpreter::ValueTag::ObjectRef)
            exc_val = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result.exception_value.obj);
        throw chaos_managed_exception{exc_val};
    }
    if (ret_buf != nullptr && result.has_return_value) {
        auto ret_tag = (type_aware_args && patch_method->cached_sig_valid)
            ? static_cast<interpreter::ValueTag>(patch_method->cached_ret_tag) : interpreter::ValueTag::Void;
        WriteTypedRet(ret_buf, result, ret_tag);
    }
}

void InterpreterEntryDirectFast(uintptr_t method_key) noexcept {
    uint64_t __chaos_args[4];
    uint64_t __chaos_ret[2];
    InterpreterEntryDirect(method_key, __chaos_args, __chaos_ret);
}

}  // namespace chaos::il2cpp::runtime_core