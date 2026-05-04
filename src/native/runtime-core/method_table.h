#ifndef CHAOS_IL2CPP_METHOD_TABLE_H_
#define CHAOS_IL2CPP_METHOD_TABLE_H_

#include <cstdint>

namespace chaos::il2cpp::method_table {

// ── Constants ──────────────────────────────────────────────────────────

/// Maximum number of method table entries.
/// Chosen to accommodate all AOT-compiled methods across all foundation DLLs.
/// At 16 bytes per entry, this consumes 1 MB of .data / .bss.
constexpr uint32_t kMethodTableSize = 65536;

/// Sentinel value meaning "slot not yet initialized."
constexpr uint32_t kInvalidGeneration = 0;

// ── MethodTableEntry ───────────────────────────────────────────────────

/// A single entry in the global method table.
/// Each entry stores a function pointer and a generation stamp for hotupdate
/// version tracking.
struct MethodTableEntry {
    void*    fn_ptr;       ///< Function pointer (nullptr = uninitialized)
    uint32_t module_gen;   ///< Module generation for hotupdate tracking (0 = uninitialized)
    uint32_t reserved;     ///< Reserved for future use (alignment / flags)
};

// ── Global table declaration ───────────────────────────────────────────

/// The global method table, pre-allocated at link time.
/// Generated AOT code reads from this table via g_method_table[index].fn_ptr.
/// Hotupdate code writes to this table via WriteMethodTable().
extern MethodTableEntry g_method_table[kMethodTableSize];

// ── API ────────────────────────────────────────────────────────────────

/// Initialize the entire method table to zero (fn_ptr = nullptr, gen = 0).
/// Called once during runtime bootstrap.
void InitializeMethodTable();

/// Write a single method table entry.
/// Used by hotupdate modules to register replacement functions.
/// @param index       Slot index (0 <= index < kMethodTableSize)
/// @param fn_ptr      Function pointer to install
/// @param module_gen  Module generation stamp (0 = invalid)
/// @return true if the write was accepted, false on invalid index.
bool WriteMethodTable(uint32_t index, void* fn_ptr, uint32_t module_gen);

/// Read a function pointer from the method table, or nullptr if uninitialized.
void* ResolveMethodTable(uint32_t index);

/// Clear all entries associated with a given module generation.
/// Used during hotupdate domain unload.
void ClearMethodTableByGeneration(uint32_t module_gen);

}  // namespace chaos::il2cpp::method_table

#endif  // CHAOS_IL2CPP_METHOD_TABLE_H_
