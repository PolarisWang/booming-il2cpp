// static_var_store.h — Indirect static field access for hot-update assemblies
//
// Provides a C-linkage API that routes static field reads/writes through
// AssemblyManager::GetStaticField() instead of direct global variable access.
//
// The codegen emits assembly_hash + field_offset pairs for each hot-update
// static field access. At runtime, the assembly loader registers the mapping
// from assembly_hash to module_id so that static_var_store_read/write can
// resolve the correct per-assembly storage.
//
// Thread safety: internal mutex for registration map; read/write paths
// perform a shared-mutex-protected lookup then delegate to GetStaticField
// (which has its own synchronization).

#ifndef CHAOS_IL2CPP_STATIC_VAR_STORE_H_
#define CHAOS_IL2CPP_STATIC_VAR_STORE_H_

#include <cstdint>

// ── FNV-1a 32-bit helper ───────────────────────────────────────────────
// Matches CreatePseudoMetadataHandle() in NativeAotLoweringPlanner.
// Computes a stable 24-bit hash from an ASCII/UTF-8 assembly name.
inline uint32_t ComputeAssemblyHash(const char* name) noexcept {
    uint32_t hash = 2166136261u;
    for (const char* p = name; *p; ++p) {
        hash ^= static_cast<unsigned char>(*p);
        hash *= 16777619u;
    }
    uint32_t result = hash & 0x00FFFFFFu;
    return result != 0 ? result : 1u;
}

extern "C" {

/// Read a pointer to the static field value for (assembly_hash, field_offset).
void* static_var_store_read(uint32_t assembly_hash, uint32_t field_offset);

/// Write @a value into the static field for (assembly_hash, field_offset).
/// The value is stored as a pointer-sized integer; callers cast as needed.
void static_var_store_write(uint32_t assembly_hash, uint32_t field_offset, uintptr_t value);

/// Register the mapping from assembly_hash to module_id/domain_id.
/// Called by AssemblyManager::LoadAssembly.
void static_var_store_register(uint32_t assembly_hash, uint32_t module_id, uint32_t domain_id);

/// Unregister all entries for the given domain_id.
/// Called by AssemblyManager::UnloadAssembly.
void static_var_store_unregister_domain(uint32_t domain_id);

/// Read a pointer to the static field value for (assembly_hash, field_hash).
/// Lazily allocates a storage slot on first access. Returns nullptr on lookup failure.
/// Used by AOT codegen for hot-update assembly static fields.
void* static_var_store_read_field(uint32_t assembly_hash, uint32_t field_hash);

/// Write @a value into the static field for (assembly_hash, field_hash).
/// The value is stored as a pointer-sized integer; callers cast as needed.
void static_var_store_write_field(uint32_t assembly_hash, uint32_t field_hash, uintptr_t value);

/// Unregister all field entries for the given assembly_hash.
/// Called by AssemblyManager::UnloadAssembly to clean up field-level storage.
void static_var_store_unregister_assembly(uint32_t assembly_hash);

}  // extern "C"

#endif  // CHAOS_IL2CPP_STATIC_VAR_STORE_H_
