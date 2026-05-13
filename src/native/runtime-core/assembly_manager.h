// assembly_manager.h — Assembly-level abstraction for hot-update packages
//
// Provides AssemblyDescriptor (per-assembly state) and AssemblyManager
// (lifecycle management for loaded assemblies).  Key responsibilities:
//   - Per-assembly static field storage isolation
//   - Assembly load/unload lifecycle
//   - Dispatch entry registration via HotpatchNameRegistry
//
// This is the runtime-side counterpart to HotUpdateAssemblyLoader (C#).

#ifndef CHAOS_IL2CPP_ASSEMBLY_MANAGER_H_
#define CHAOS_IL2CPP_ASSEMBLY_MANAGER_H_

#include "hotpatch_table.h"
#include "module_registry.h"
#include "patch_loader.h"
#include "generic_context.h"

#include <cstdint>
#include <vector>
#include <string>

namespace chaos::il2cpp::runtime_core {

// ── AssemblyDescriptor ──────────────────────────────────────────────
// Holds all per-assembly state loaded from a .patchdata binary.
struct AssemblyDescriptor {
    uint32_t        module_id       = 0;       // module_registry index
    std::string     name;                      // assembly name (e.g. "MyAssembly")
    PatchContext*   patch_context   = nullptr; // .patchdata context (methods + metadata)

    // Per-assembly static field storage.
    // Isolated from the global g_static_fields vector so that
    // AssemblyUnload can bulk-free without affecting other assemblies.
    // Fields are stored as InterpreterValue (interpreter/interpreter_vm.h).
    // Allocated on domain heap; freed by AssemblyUnload.
    void*           static_field_ptr    = nullptr; // domain-allocated InterpreterValue[]
    uint32_t        static_field_count  = 0;        // number of static fields

    // Memory domain for static field storage and per-assembly allocations.
    uint32_t        domain_id           = 0;

    // Generic context for this assembly's closed instantiations.
    // Registered during load, bulk-unregistered during unload.
    bool            generics_registered = false;

    // Assembly load state.
    bool            is_loaded           = false;
};

// ── AssemblyManager ─────────────────────────────────────────────────
// Manages the lifecycle of all loaded hot-update assemblies.
// Provides thread-safe load/unload operations.
class AssemblyManager {
public:
    /// Global singleton.
    static AssemblyManager* Get() noexcept;

    /// Load an assembly from a .patchdata binary blob.
    /// Performs:
    ///   1. ApplyPatchFromMemory (validates + patches dispatch entries)
    ///   2. Register module + generics
    ///   3. Allocate per-assembly static field storage
    ///   4. Register in internal assembly table
    ///
    /// @param patch_data     Pointer to .patchdata binary
    /// @param patch_size     Size of .patchdata binary
    /// @param assembly_name  Assembly name (e.g. "MyAssembly")
    /// @param host_type_name Optional type name override (see ApplyPatchFromMemory)
    ///
    /// @return AssemblyDescriptor*, or nullptr on failure.
    AssemblyDescriptor* LoadAssembly(
        const void* patch_data,
        size_t patch_size,
        const char* assembly_name,
        const char* host_type_name = nullptr) noexcept;

    /// Unload an assembly: clear patches, free static fields, unregister generics.
    /// After this call, the AssemblyDescriptor pointer is invalidated.
    bool UnloadAssembly(AssemblyDescriptor* asm_desc) noexcept;

    /// Find an assembly by name.
    AssemblyDescriptor* FindAssembly(const char* name) noexcept;

    /// Get the static field storage for a given (module_id, field_offset).
    /// Returns a pointer to the InterpreterValue, or nullptr if not found.
    /// This is called by Handle_LdSFld / Handle_StSFld in the interpreter.
    void* GetStaticField(uint32_t module_id, uint32_t field_offset) noexcept;

    /// Number of loaded assemblies.
    uint32_t LoadedCount() const noexcept { return loaded_count_; }

    /// Maximum number of concurrent assemblies.
    static constexpr uint32_t kMaxAssemblies = 64;

private:
    AssemblyManager() = default;
    ~AssemblyManager() = default;

    AssemblyManager(const AssemblyManager&) = delete;
    AssemblyManager& operator=(const AssemblyManager&) = delete;

    AssemblyDescriptor assemblies_[kMaxAssemblies]{};
    uint32_t loaded_count_ = 0;
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_ASSEMBLY_MANAGER_H_