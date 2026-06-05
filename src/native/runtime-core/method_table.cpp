#include "method_table.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "hotpatch_table.h"

#include <chaos/trace.h>
#include <chaos/profile.h>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <vector>

namespace chaos::il2cpp::method_table {

// ── Global table definitions ──────────────────────────────────────────
// Zero-initialized (.bss) at process load time.
MethodTableEntry   g_method_table[kMethodTableSize] = {};
MethodTableOrigin  g_method_origins[kMethodTableSize] = {};

// ── API ────────────────────────────────────────────────────────────────

void InitializeMethodTable() noexcept {
    // Table is already zero-initialized by the loader (.bss).
    // This function exists as a hook for test harnesses that need to
    // reset the table without a full process reload.
    for (uint32_t i = 0; i < kMethodTableSize; i++) {
        g_method_table[i].fn_ptr.store(nullptr, std::memory_order_relaxed);
        g_method_table[i].module_gen.store(0, std::memory_order_relaxed);
        g_method_table[i].reserved = 0;
    }
    // Origins use kInvalidModuleId as the unset sentinel (not zero, since
    // module_id 0 is a valid CoreLib module).  In production the table is
    // .bss, so Origins get zeroed, but InitializeMethodTable (test hook)
    // must mark every slot as unset.
    for (uint32_t i = 0; i < kMethodTableSize; i++) {
        g_method_origins[i].module_id.store(runtime_core::kInvalidModuleId, std::memory_order_relaxed);
        g_method_origins[i].manifest_method_index = 0;
    }
}

bool WriteMethodTable(uint32_t index, void* fn_ptr, uint32_t module_gen) noexcept {
    if (index >= kMethodTableSize) {
        return false;
    }

    // Release ordering: fn_ptr store must be visible before any reader
    // observes the matching module_gen, so a reader that finds a matching
    // generation can safely dereference fn_ptr.
    g_method_table[index].fn_ptr.store(fn_ptr, std::memory_order_release);
    g_method_table[index].module_gen.store(module_gen, std::memory_order_release);
    return true;
}

void* ResolveMethodTable(uint32_t index) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("ResolveMethodTable");
    if (index >= kMethodTableSize) {
        return nullptr;
    }

    return g_method_table[index].fn_ptr.load(std::memory_order_acquire);
}

void ClearMethodTableByGeneration(uint32_t module_gen) noexcept {
    if (module_gen == kInvalidGeneration) {
        return;
    }

    for (uint32_t i = 0; i < kMethodTableSize; i++) {
        if (g_method_table[i].module_gen.load(std::memory_order_acquire) == module_gen) {
            // Release on fn_ptr ensures the null store is visible to any
            // thread that subsequently loads fn_ptr with acquire, and prevents
            // reordering such that module_gen (below) becomes visible first.
            // Without this, a reader could see kInvalidGeneration yet still
            // observe a non-null (now-dangling) fn_ptr.
            g_method_table[i].fn_ptr.store(nullptr, std::memory_order_release);
            g_method_table[i].module_gen.store(kInvalidGeneration, std::memory_order_release);
        }
    }
}

// ── Origin tracking ────────────────────────────────────────────────────

void SetMethodOrigin(uint32_t index, uint32_t module_id, uint32_t manifest_method_index) noexcept {
    if (index >= kMethodTableSize) return;
    // Relaxed ordering: origins are published before the owning module is
    // made visible, so readers will only observe them after the module is
    // registered.  module_id store acts as the visible marker.
    g_method_origins[index].module_id.store(module_id, std::memory_order_relaxed);
    g_method_origins[index].manifest_method_index = manifest_method_index;
}

MethodTableOrigin GetMethodOrigin(uint32_t index) noexcept {
    if (index >= kMethodTableSize) {
        return {runtime_core::kInvalidModuleId, 0};
    }
    // Load module_id atomically (relaxed: used for best-effort ABI validation).
    uint32_t mid = g_method_origins[index].module_id.load(std::memory_order_relaxed);
    uint32_t idx = g_method_origins[index].manifest_method_index;
    return {mid, idx};
}

// ── Cross-DLL ABI-validated resolution ─────────────────────────────────

void* ResolveMethodTableWithAbiCheck(
    uint32_t index,
    uint8_t expected_return_carrier,
    const uint8_t* expected_param_carriers,
    uint8_t expected_param_count) noexcept
{
    if (index >= kMethodTableSize) {
        return nullptr;
    }

    void* fn_ptr = g_method_table[index].fn_ptr.load(std::memory_order_acquire);
    if (fn_ptr == nullptr) {
        return nullptr;
    }

    // If no origin is registered for this slot, we cannot validate —
    // return the pointer as-is (defensive: caller must verify).
    uint32_t origin_module_id = g_method_origins[index].module_id.load(std::memory_order_relaxed);
    if (origin_module_id == runtime_core::kInvalidModuleId) {
        return fn_ptr;
    }

    const auto* manifest = runtime_core::LookupModuleAbiManifest(origin_module_id);
    if (manifest == nullptr) {
        // No ABI manifest registered for the owning module — skip validation.
        return fn_ptr;
    }

    ChaosAbiManifestResult result = ChaosAbiManifestCheckMethodSignature(
        manifest,
        g_method_origins[index].manifest_method_index,
        static_cast<ChaosAbiCarrierCode>(expected_return_carrier),
        reinterpret_cast<const ChaosAbiCarrierCode*>(expected_param_carriers),
        expected_param_count);

    return (result == CHAOS_ABI_MANIFEST_OK) ? fn_ptr : nullptr;
}

// ── P1-B: Hotpatch-backed method table population ─────────────────────────

/// Entry in the (module_id, slot) → method_table_index sorted mapping.
/// Sorted by (module_id, slot) for O(log n) binary search.
struct MtSlotMapping {
    uint32_t module_id;
    uint32_t slot;
    uint32_t mt_index;  // index into g_method_table[]
};

/// Atomic counter for sequential method table index assignment.
static std::atomic<uint32_t> g_next_mt_index{0};

/// Sorted mapping: (module_id, slot) → mt_index.
/// Populated during PopulateMethodTableFromHotpatch, read-only thereafter.
static std::vector<MtSlotMapping> g_mt_slot_map;

void PopulateMethodTableFromHotpatch() noexcept {
    auto& registry = runtime_core::GetHotpatchNameRegistry();
    size_t module_count = registry.ModuleCount();
    if (module_count == 0) return;

    // First pass: count total dispatch entries across all modules.
    size_t total_entries = 0;
    for (size_t mi = 0; mi < module_count; ++mi) {
        const auto* mod = registry.GetModuleByIndex(mi);
        if (mod != nullptr) {
            total_entries += mod->entry_table_size;
        }
    }
    if (total_entries == 0) return;

    g_mt_slot_map.reserve(total_entries);

    uint32_t gen = 0;  // module_gen = 0 for AOT root

    // Second pass: assign sequential indices, write to method table,
    // and build the reverse mapping.
    for (size_t mi = 0; mi < module_count; ++mi) {
        const auto* mod = registry.GetModuleByIndex(mi);
        if (mod == nullptr || mod->entry_table == nullptr) continue;

        for (uint32_t slot = 0; slot < mod->entry_table_size; ++slot) {
            uint32_t mt_idx = g_next_mt_index.fetch_add(1, std::memory_order_relaxed);
            void* fn_ptr = mod->entry_table[slot].direct_ptr;

            WriteMethodTable(mt_idx, fn_ptr, gen);
            g_mt_slot_map.push_back({static_cast<uint32_t>(mi), slot, mt_idx});
        }
    }

    // Sort by (module_id, slot) for binary search.
    std::sort(g_mt_slot_map.begin(), g_mt_slot_map.end(),
        [](const MtSlotMapping& a, const MtSlotMapping& b) noexcept {
            if (a.module_id != b.module_id) return a.module_id < b.module_id;
            return a.slot < b.slot;
        });

    // PopulateMethodTableFromHotpatch: n modules, m entries populated.
    // Logging skipped: method_table.cpp doesn't include <chaos/log.h>.
}

void* ResolveMethodTableByModuleSlot(uint32_t module_id, uint32_t slot) noexcept {
    if (g_mt_slot_map.empty()) return nullptr;

    // Binary search for (module_id, slot).
    const MtSlotMapping key{module_id, slot, 0};
    auto it = std::lower_bound(g_mt_slot_map.begin(), g_mt_slot_map.end(), key,
        [](const MtSlotMapping& a, const MtSlotMapping& b) noexcept {
            if (a.module_id != b.module_id) return a.module_id < b.module_id;
            return a.slot < b.slot;
        });

    if (it != g_mt_slot_map.end() &&
        it->module_id == module_id &&
        it->slot == slot) {
        return ResolveMethodTable(it->mt_index);
    }
    return nullptr;
}

}  // namespace chaos::il2cpp::method_table
