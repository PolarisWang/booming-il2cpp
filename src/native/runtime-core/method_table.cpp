#include "method_table.h"

#include <cstring>

namespace chaos::il2cpp::method_table {

// ── Global table definition ────────────────────────────────────────────
// Zero-initialized (.bss) at process load time.
MethodTableEntry g_method_table[kMethodTableSize] = {};

// ── API ────────────────────────────────────────────────────────────────

void InitializeMethodTable() {
    // Table is already zero-initialized by the loader (.bss).
    // This function exists as a hook for test harnesses that need to
    // reset the table without a full process reload.
    std::memset(g_method_table, 0, sizeof(g_method_table));
}

bool WriteMethodTable(uint32_t index, void* fn_ptr, uint32_t module_gen) {
    if (index >= kMethodTableSize) {
        return false;
    }

    g_method_table[index].fn_ptr    = fn_ptr;
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

}  // namespace chaos::il2cpp::method_table
