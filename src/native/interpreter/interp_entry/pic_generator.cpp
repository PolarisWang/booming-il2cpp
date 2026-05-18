// pic_generator.cpp — Profile collection + PIC generation (Phase 2)
//
// Two responsibilities:
//   1. During T2 execution: sample receiver type_tokens at callvirt sites
//      (every kProfileSampleInterval = 64 calls, to keep T2 overhead low).
//   2. During T2→T3 promotion: analyze profiles and generate PicDispatchChain[]
//      for polymorphic inline caching in RegisterExecute.
//
// Data flow:
//   T2 execution → SampleCallVirtProfile() accumulates receiver types
//   T2→T3 CAS     → GeneratePICData() reads profiles → creates PicDispatchChain[]
//   T3 execution  → Reg_CallVirt checks PIC chain before InterpreterDispatchRaw

#include "patch_loader.h"
#include "reflection_query_model.h"
#include "vtable_registry.h"
#include "instantiation_engine.h"

#include "ir_reg_alloc.h"

#include <chaos/log.h>

#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

// ── Count callvirt instructions in a RegisterMethod ───────────────────────
// Used to pre-allocate CallSiteProfile[] and PicDispatchChain[] arrays
// indexed by instruction position (frame.pc).
static uint32_t CountCallVirtInstructions(
    const interpreter::RegisterMethod& rm) noexcept {
    uint32_t count = 0;
    for (const auto& instr : rm.instructions) {
        if (instr.op_code() == interpreter::IROpCode::CallVirt ||
            instr.op_code() == interpreter::IROpCode::CallVirtConstrained) {
            ++count;
        }
    }
    return count;
}

// ── Ensure call site profiles are allocated ───────────────────────────────
// Called from entry_direct.cpp when entering T2 execution for the first time.
// Allocates CallSiteProfile[callvirt_count] and stores on PatchMethod.
// Returns true if profiles were allocated (or already exist).
bool EnsureCallSiteProfiles(PatchMethod* pm) noexcept {
    if (pm == nullptr) return false;
    if (pm->call_site_profiles != nullptr) return true;  // already allocated

    auto* rm = static_cast<interpreter::RegisterMethod*>(pm->cached_optimized_reg_method);
    if (rm == nullptr) return false;

    uint32_t callvirt_count = CountCallVirtInstructions(*rm);
    if (callvirt_count == 0) return false;  // no callvirt sites to profile

    auto* profiles = static_cast<CallSiteProfile*>(
        std::calloc(callvirt_count, sizeof(CallSiteProfile)));
    if (profiles == nullptr) return false;

    // Map instruction index → profile index: store the instruction index
    // for each profile entry so sampling can match frame.pc to profile.
    uint32_t prof_idx = 0;
    for (uint32_t i = 0; i < static_cast<uint32_t>(rm->instructions.size()); ++i) {
        if (rm->instructions[i].op_code() == interpreter::IROpCode::CallVirt ||
            rm->instructions[i].op_code() == interpreter::IROpCode::CallVirtConstrained) {
            if (prof_idx < callvirt_count) {
                profiles[prof_idx].instruction_idx = i;
                ++prof_idx;
            }
        }
    }

    pm->call_site_profiles = profiles;
    pm->call_site_profile_count = callvirt_count;
    return true;
}

// ── Profile sampling ──────────────────────────────────────────────────────
// Called from RegisterExecute's Reg_CallVirt for each callvirt dispatch.
// Samples receiver type_token every kProfileSampleInterval calls
// (checked by caller: only call when count & 63 == 0).
//
// Updates the running type distribution in the CallSiteProfile for this
// instruction index. Uses lightweight decay: when a new type appears and
// all 4 slots are full, evict the least-frequent type.
void SampleCallVirtProfile(
    PatchMethod* pm,
    uint32_t instruction_idx,
    uint64_t receiver_type_token) noexcept {

    if (pm == nullptr || pm->call_site_profiles == nullptr) return;

    auto* profiles = static_cast<CallSiteProfile*>(pm->call_site_profiles);
    uint32_t count = pm->call_site_profile_count;

    // Linear scan to find the profile entry matching this instruction index.
    // N is small (≤ callvirt_count, typically < 20).
    for (uint32_t i = 0; i < count; ++i) {
        if (profiles[i].instruction_idx != instruction_idx) continue;

        auto& prof = profiles[i];
        ++prof.sample_count;

        // Look for existing type entry
        for (uint32_t j = 0; j < prof.type_count; ++j) {
            if (prof.type_tokens[j] == receiver_type_token) {
                ++prof.type_hit_counts[j];
                return;
            }
        }

        // New type: add if space, or replace least-frequent
        if (prof.type_count < 4) {
            prof.type_tokens[prof.type_count] = receiver_type_token;
            prof.type_hit_counts[prof.type_count] = 1;
            ++prof.type_count;
        } else {
            // Replace least-frequent entry (decay old types)
            uint32_t min_idx = 0;
            for (uint32_t j = 1; j < 4; ++j) {
                if (prof.type_hit_counts[j] < prof.type_hit_counts[min_idx]) {
                    min_idx = j;
                }
            }
            prof.type_tokens[min_idx] = receiver_type_token;
            prof.type_hit_counts[min_idx] = 1;
        }
        return;
    }
}

// ── PIC generation ────────────────────────────────────────────────────────
// Analyzes CallSiteProfile[] and generates PicDispatchChain[].
// For each callvirt site:
//   - If 1 type dominates (>90%): monomorphic PIC (1 slot).
//   - If 2-3 types dominate (>95%): polymorphic chain.
//   - Otherwise: no PIC (falls back to MIC in InterpreterDispatchRaw).
//
// The generated chains are stored in pm->pic_dispatch_data as a flat
// PicDispatchChain[callvirt_count], indexed by profile index (matching
// the CallSiteProfile order).
//
// Returns true if any PIC data was generated.
bool GeneratePICData(PatchMethod* pm) noexcept {
    if (pm == nullptr || pm->call_site_profiles == nullptr) return false;
    if (pm->pic_dispatch_data != nullptr) return true;  // already generated

    auto* rm = static_cast<interpreter::RegisterMethod*>(pm->cached_optimized_reg_method);
    if (rm == nullptr) return false;

    auto* profiles = static_cast<CallSiteProfile*>(pm->call_site_profiles);
    uint32_t count = pm->call_site_profile_count;
    if (count == 0) return false;

    // Allocate count header + chains array as a single block.
    // Layout: [uint32_t count][PicDispatchChain[count]]
    // The count header enables O(1) bounds for runtime PIC chain scanning.
    uint8_t* alloc_base = static_cast<uint8_t*>(
        std::calloc(1, sizeof(uint32_t) + count * sizeof(PicDispatchChain)));
    if (alloc_base == nullptr) return false;
    *reinterpret_cast<uint32_t*>(alloc_base) = count;
    auto* chains = reinterpret_cast<PicDispatchChain*>(alloc_base + sizeof(uint32_t));

    // Pre-extract method_token for each profile entry's instruction_idx.
    // Map instruction_idx → method_token from the RegisterMethod.
    uint32_t instruct_count = static_cast<uint32_t>(rm->instructions.size());
    CHAOS_IL2CPP_VECTOR(uint32_t) inst_method_tokens(instruct_count, 0);
    for (uint32_t ii = 0; ii < instruct_count; ++ii) {
        const auto& ri = rm->instructions[ii];
        if (ri.op_code() == interpreter::IROpCode::CallVirt ||
            ri.op_code() == interpreter::IROpCode::CallVirtConstrained) {
            // Extract method_token from call_target (MethodInfoHandle → descriptor)
            MethodInfoHandle handle = static_cast<MethodInfoHandle>(
                reinterpret_cast<uintptr_t>(ri.imm.ptr));
            const auto* desc = TryDecodeReflectionQueryMethodHandle(handle);
            inst_method_tokens[ii] = (desc != nullptr) ? desc->metadata_token : 0;
        }
    }

    uint32_t generated_count = 0;
    uint64_t current_gen = g_patch_generation.load(std::memory_order_relaxed);

    for (uint32_t i = 0; i < count; ++i) {
        const auto& prof = profiles[i];
        if (prof.sample_count < 4) continue;  // insufficient samples

        // Calculate dominance
        uint32_t total = 0;
        for (uint32_t j = 0; j < prof.type_count; ++j) {
            total += prof.type_hit_counts[j];
        }
        if (total == 0) continue;

        // Sort types by hit count descending (simple bubble sort for ≤4)
        uint64_t sorted_tokens[4] = {};
        uint32_t sorted_hits[4] = {};
        for (uint32_t j = 0; j < prof.type_count; ++j) {
            sorted_tokens[j] = prof.type_tokens[j];
            sorted_hits[j] = prof.type_hit_counts[j];
        }
        for (uint32_t a = 0; a < prof.type_count; ++a) {
            for (uint32_t b = a + 1; b < prof.type_count; ++b) {
                if (sorted_hits[b] > sorted_hits[a]) {
                    uint64_t tt = sorted_tokens[a]; sorted_tokens[a] = sorted_tokens[b]; sorted_tokens[b] = tt;
                    uint32_t th = sorted_hits[a]; sorted_hits[a] = sorted_hits[b]; sorted_hits[b] = th;
                }
            }
        }

        auto& chain = chains[i];
        chain.generation = static_cast<uint32_t>(current_gen);
        chain.instruction_idx = prof.instruction_idx;
        chain.method_token = (prof.instruction_idx < instruct_count)
            ? inst_method_tokens[prof.instruction_idx] : 0;

        // Determine how many slots to populate
        float top1_ratio = static_cast<float>(sorted_hits[0]) / static_cast<float>(total);
        uint32_t slots_to_fill = 0;

        if (top1_ratio > 0.90f) {
            slots_to_fill = 1;  // monomorphic
        } else {
            // Check top 2, top 3
            uint32_t cumulative = sorted_hits[0];
            for (uint32_t s = 1; s < prof.type_count && s < 3; ++s) {
                cumulative += sorted_hits[s];
                float ratio = static_cast<float>(cumulative) / static_cast<float>(total);
                if (ratio > 0.95f) {
                    slots_to_fill = s + 1;
                    break;
                }
            }
        }

        // Pre-resolve AOT function pointers for each PIC slot
        for (uint32_t s = 0; s < slots_to_fill && s < 3; ++s) {
            chain.slots[s].type_token = sorted_tokens[s];
            if (chain.method_token != 0 && sorted_tokens[s] != 0) {
                chain.slots[s].direct_fn = vtable_registry::ResolveVirtualMethodPointer(
                    static_cast<uint32_t>(sorted_tokens[s]),
                    chain.method_token);
            }
        }

        ++generated_count;
    }

    if (generated_count == 0) {
        std::free(alloc_base);
        return false;
    }

    pm->pic_dispatch_data = alloc_base;
    return true;
}

// ── Accessor: get PIC chain count and array from the stored data ───────────
inline const PicDispatchChain* GetPICChains(void* pic_data, uint32_t& out_count) noexcept {
    if (pic_data == nullptr) { out_count = 0; return nullptr; }
    auto* base = static_cast<const uint8_t*>(pic_data);
    out_count = *reinterpret_cast<const uint32_t*>(base);
    return reinterpret_cast<const PicDispatchChain*>(base + sizeof(uint32_t));
}

// ── Rebuild call_cache for T3 execution ──────────────────────────────────
// Creates a new call_cache array for the optimized RegisterMethod.
// For each call instruction, precaches the ret tag and metadata.
// This replaces the null call_cache used during T2, enabling correct
// return-type handling in RegisterExecute.
void RebuildCallCacheForT3(PatchMethod* pm) noexcept {
    if (pm == nullptr) return;

    auto* rm = static_cast<interpreter::RegisterMethod*>(pm->cached_optimized_reg_method);
    if (rm == nullptr) return;

    uint32_t instr_count = static_cast<uint32_t>(rm->instructions.size());
    if (instr_count == 0) return;

    auto* cc = static_cast<runtime_instantiation::CachedCallInfo*>(
        CHAOS_IL2CPP_DOMAIN_CURRENT_ALLOCATE(
            instr_count * sizeof(runtime_instantiation::CachedCallInfo)));
    if (cc == nullptr) return;

    for (uint32_t i = 0; i < instr_count; ++i) {
        const auto& ri = rm->instructions[i];
        auto op = ri.op_code();
        if (op == interpreter::IROpCode::Call ||
            op == interpreter::IROpCode::Calli ||
            op == interpreter::IROpCode::CallVirt ||
            op == interpreter::IROpCode::CallBridge ||
            op == interpreter::IROpCode::CallVirtConstrained) {
            if (ri.imm.ptr != nullptr) {
                cc[i] = runtime_instantiation::PrecacheCallTarget(ri.imm.ptr);
            } else {
                cc[i].ret_tag = 0xFF;
            }
        } else {
            cc[i].ret_tag = 0xFF;
        }
    }

    pm->call_cache = cc;
}

// ── Cleanup ───────────────────────────────────────────────────────────────
void FreeCallSiteProfiles(PatchMethod* pm) noexcept {
    if (pm == nullptr) return;
    std::free(pm->call_site_profiles);
    pm->call_site_profiles = nullptr;
    pm->call_site_profile_count = 0;
}

void FreePICData(PatchMethod* pm) noexcept {
    if (pm == nullptr) return;
    std::free(pm->pic_dispatch_data);
    pm->pic_dispatch_data = nullptr;
}

// ── T2→T3 promotion helper ───────────────────────────────────────────────
// Called from entry_direct.cpp when the CAS winner promotes to T3.
// 1. Frees existing profiles (no longer needed after PIC generation).
// 2. Generates PIC data from profiles.
// 3. Frees profiles after successful PIC generation.
void PromoteToTier3(PatchMethod* pm) noexcept {
    if (pm == nullptr) return;

    CHAOS_IL2CPP_LOG_DEBUG_M("tier", "T2->T3 promotion: token={}, profiles={}",
        pm->token, pm->call_site_profile_count);

    // Generate PIC data from collected call-site profiles.
    if (GeneratePICData(pm)) {
        CHAOS_IL2CPP_LOG_DEBUG_M("tier", "T3: PIC data generated for token={}", pm->token);
    } else {
        CHAOS_IL2CPP_LOG_DEBUG_M("tier", "T3: no profitable PIC for token={} (insufficient samples)", pm->token);
    }

    // Free profiles regardless — they are only needed for PIC generation.
    FreeCallSiteProfiles(pm);
}

}  // namespace chaos::il2cpp::runtime_core
