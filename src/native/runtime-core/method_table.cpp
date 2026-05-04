#include "method_table.h"
#include "module_registry.h"
#include "abi_manifest.h"

#include <cstring>

namespace chaos::il2cpp::method_table {

// ── Global table definitions ──────────────────────────────────────────
// Zero-initialized (.bss) at process load time.
MethodTableEntry   g_method_table[kMethodTableSize] = {};
MethodTableOrigin  g_method_origins[kMethodTableSize] = {};

// ── API ────────────────────────────────────────────────────────────────

void InitializeMethodTable() {
    // Table is already zero-initialized by the loader (.bss).
    // This function exists as a hook for test harnesses that need to
    // reset the table without a full process reload.
    std::memset(g_method_table, 0, sizeof(g_method_table));
    // Origins use kInvalidModuleId as the unset sentinel (not zero, since
    // module_id 0 is a valid CoreLib module).  In production the table is
    // .bss, so Origins get zeroed, but InitializeMethodTable (test hook)
    // must mark every slot as unset.
    for (uint32_t i = 0; i < kMethodTableSize; i++) {
        g_method_origins[i].module_id = runtime_core::kInvalidModuleId;
        g_method_origins[i].manifest_method_index = 0;
    }
}

bool WriteMethodTable(uint32_t index, void* fn_ptr, uint32_t module_gen) {
    if (index >= kMethodTableSize) {
        return false;
    }

    g_method_table[index].fn_ptr     = fn_ptr;
    g_method_table[index].module_gen = module_gen;
    return true;
}

void* ResolveMethodTable(uint32_t index) {
    if (index >= kMethodTableSize) {
        return nullptr;
    }

    return g_method_table[index].fn_ptr;
}

void ClearMethodTableByGeneration(uint32_t module_gen) {
    if (module_gen == kInvalidGeneration) {
        return;
    }

    for (uint32_t i = 0; i < kMethodTableSize; i++) {
        if (g_method_table[i].module_gen == module_gen) {
            g_method_table[i].fn_ptr    = nullptr;
            g_method_table[i].module_gen = kInvalidGeneration;
        }
    }
}

// ── Origin tracking ────────────────────────────────────────────────────

void SetMethodOrigin(uint32_t index, uint32_t module_id, uint32_t manifest_method_index) {
    if (index >= kMethodTableSize) return;
    g_method_origins[index].module_id            = module_id;
    g_method_origins[index].manifest_method_index = manifest_method_index;
}

MethodTableOrigin GetMethodOrigin(uint32_t index) {
    if (index >= kMethodTableSize) {
        return {runtime_core::kInvalidModuleId, 0};
    }
    return g_method_origins[index];
}

// ── Cross-DLL ABI-validated resolution ─────────────────────────────────

void* ResolveMethodTableWithAbiCheck(
    uint32_t index,
    uint8_t expected_return_carrier,
    const uint8_t* expected_param_carriers,
    uint8_t expected_param_count)
{
    if (index >= kMethodTableSize) {
        return nullptr;
    }

    void* fn_ptr = g_method_table[index].fn_ptr;
    if (fn_ptr == nullptr) {
        return nullptr;
    }

    // If no origin is registered for this slot, we cannot validate —
    // return the pointer as-is (defensive: caller must verify).
    MethodTableOrigin origin = g_method_origins[index];
    if (origin.module_id == runtime_core::kInvalidModuleId) {
        return fn_ptr;
    }

    const auto* manifest = runtime_core::LookupModuleAbiManifest(origin.module_id);
    if (manifest == nullptr) {
        // No ABI manifest registered for the owning module — skip validation.
        return fn_ptr;
    }

    ChaosAbiManifestResult result = ChaosAbiManifestCheckMethodSignature(
        manifest,
        origin.manifest_method_index,
        static_cast<ChaosAbiCarrierCode>(expected_return_carrier),
        reinterpret_cast<const ChaosAbiCarrierCode*>(expected_param_carriers),
        expected_param_count);

    return (result == CHAOS_ABI_MANIFEST_OK) ? fn_ptr : nullptr;
}

}  // namespace chaos::il2cpp::method_table
