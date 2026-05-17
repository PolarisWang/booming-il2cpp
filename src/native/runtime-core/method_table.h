#ifndef CHAOS_IL2CPP_METHOD_TABLE_H_
#define CHAOS_IL2CPP_METHOD_TABLE_H_

#include <atomic>
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
    std::atomic<void*>   fn_ptr;       ///< Function pointer (nullptr = uninitialized)
    std::atomic<uint32_t> module_gen;   ///< Module generation for hotupdate tracking (0 = uninitialized)
    uint32_t             reserved;     ///< Reserved for future use (alignment / flags)

    MethodTableEntry() noexcept : fn_ptr(nullptr), module_gen(0), reserved(0) {}
};

// ── MethodTableOrigin ──────────────────────────────────────────────────

/// Origin tracking for cross-DLL ABI validation.
/// Records which module owns this method table slot and its index within
/// that module's ABI manifest.
/// module_id is atomic for lock-free concurrent read from resolve paths.
struct alignas(8) MethodTableOrigin {
    std::atomic<uint32_t> module_id;            ///< Module that owns this method (kInvalidModuleId = unknown)
    uint32_t              manifest_method_index; ///< Index into the module's ABI manifest

    MethodTableOrigin() noexcept : module_id(0), manifest_method_index(0) {}
    MethodTableOrigin(uint32_t mid, uint32_t idx) noexcept : module_id(mid), manifest_method_index(idx) {}

    // std::atomic deletes implicit copy/move assignment, so provide explicit one.
    MethodTableOrigin& operator=(const MethodTableOrigin& other) noexcept {
        if (this != &other) {
            module_id.store(other.module_id.load(std::memory_order_relaxed), std::memory_order_relaxed);
            manifest_method_index = other.manifest_method_index;
        }
        return *this;
    }
};

// ── Global table declarations ──────────────────────────────────────────

/// The global method table, pre-allocated at link time.
extern MethodTableEntry g_method_table[kMethodTableSize];

/// Parallel origin array (one entry per method table slot).
extern MethodTableOrigin g_method_origins[kMethodTableSize];

// ── API ────────────────────────────────────────────────────────────────

/// Initialize the entire method table and origins to zero.
void InitializeMethodTable();

/// Write a single method table entry.
bool WriteMethodTable(uint32_t index, void* fn_ptr, uint32_t module_gen);

/// Read a function pointer, or nullptr if uninitialized.
void* ResolveMethodTable(uint32_t index);

/// Clear all entries associated with a given module generation.
void ClearMethodTableByGeneration(uint32_t module_gen);

/// Record the ABI origin for a method table slot.
/// Must be called after WriteMethodTable() for cross-DLL ABI validation to work.
void SetMethodOrigin(uint32_t index, uint32_t module_id, uint32_t manifest_method_index);

/// Read the origin for a method table slot.
MethodTableOrigin GetMethodOrigin(uint32_t index);

/// Resolve a method table entry AND validate the method's ABI signature against
/// the owning module's manifest.  Returns the function pointer if valid, or
/// nullptr on mismatch (or if the slot is uninitialized).
void* ResolveMethodTableWithAbiCheck(
    uint32_t index,
    uint8_t expected_return_carrier,
    const uint8_t* expected_param_carriers,
    uint8_t expected_param_count);

}  // namespace chaos::il2cpp::method_table

#endif  // CHAOS_IL2CPP_METHOD_TABLE_H_
