// assembly_manager.h — AssemblyLoadContext (ALC) abstraction
//
// Provides AssemblyLoadContext (per-assembly state with ALC isolation)
// and AssemblyManager (lifecycle management for loaded assemblies).
// Key responsibilities:
//   - Per-assembly static field storage isolation
//   - Assembly load/unload lifecycle with domain_unloader integration
//   - ALC-level type identity and isolation
//   - Dispatch entry registration via HotpatchNameRegistry
//
// This is the runtime-side counterpart to HotUpdateAssemblyLoader (C#).
// Phase 2 CoreCLR alignment: AssemblyDescriptor → AssemblyLoadContext.

#ifndef CHAOS_IL2CPP_ASSEMBLY_MANAGER_H_
#define CHAOS_IL2CPP_ASSEMBLY_MANAGER_H_

#include "hotpatch_table.h"
#include "module_registry.h"
#include "patch_loader.h"
#include "generic_context.h"
#include "domain_unloader.h"

#include <cstdint>
#include <vector>
#include <string>
#include <atomic>

namespace chaos::il2cpp::runtime_core {

// ── AssemblyLoadContext (was AssemblyDescriptor) ───────────────────────
// Holds all per-assembly state loaded from a .patchdata binary.
// Extended with ALC fields: alc_id, is_shared, is_unloading + tombstone.
struct AssemblyLoadContext {
    // ── ALC identity ─────────────────────────────────────────────────
    uint32_t        alc_id          = 0;       // 0 = SharedContext / CoreLib
    std::string     name;                      // e.g. "SharedContext" | "MyAssembly"
    bool            is_shared       = false;   // true = never unloaded (SharedContext)

    // ── Module binding ───────────────────────────────────────────────
    uint32_t        module_id       = 0;       // module_registry index
    PatchContext*   patch_context   = nullptr; // .patchdata context (methods + metadata)

    // ── Per-assembly static field storage ────────────────────────────
    // Isolated from other ALCs so that UnloadAssembly can bulk-free
    // without affecting other assemblies.
    void*           static_field_ptr    = nullptr; // domain-allocated InterpreterValue[]
    uint32_t        static_field_count  = 0;        // number of static fields

    // ── Memory domain ────────────────────────────────────────────────
    // Domain for static field storage and per-assembly allocations.
    uint32_t        domain_id       = 0;

    // ── Generic context ──────────────────────────────────────────────
    bool            generics_registered = false; // bulk-unregistered during unload

    // ── Lifecycle state ──────────────────────────────────────────────
    bool            is_loaded           = false;
    std::atomic<bool> is_unloading{false};       // true during active unload

    // ── Type tracking ────────────────────────────────────────────────
    uint32_t        type_count      = 0;         // number of types in this ALC
};

// ── AssemblyManager ──────────────────────────────────────────────────
// Manages the lifecycle of all loaded assemblies / ALCs.
// Provides thread-safe load/unload operations.
class AssemblyManager {
public:
    /// Global singleton.
    static AssemblyManager* Get() noexcept;

    /// Load an assembly into a new ALC from a .patchdata binary blob.
    /// Performs:
    ///   1. ApplyPatchFromMemory (validates + patches dispatch entries)
    ///   2. Register module + generics
    ///   3. Allocate per-assembly static field storage
    ///   4. Register in internal assembly table
    ///
    /// @return AssemblyLoadContext*, or nullptr on failure.
    AssemblyLoadContext* LoadAssembly(
        const void* patch_data,
        size_t patch_size,
        const char* assembly_name,
        const char* host_type_name = nullptr) noexcept;

    /// Unload an ALC: tombstone module, clear generics, destroy domain.
    /// Integrates domain_unloader::UnloadDomain for safe stop-the-world
    /// domain teardown with cross-domain ref scanning.
    ///
    /// After this call, the AssemblyLoadContext pointer is invalidated.
    bool UnloadAssembly(AssemblyLoadContext* alc) noexcept;

    /// Find an ALC by name.
    AssemblyLoadContext* FindAssembly(const char* name) noexcept;

    /// Find an ALC by module_id.
    AssemblyLoadContext* FindByModuleId(uint32_t module_id) noexcept;

    /// Get the static field storage for a given (module_id, field_offset).
    /// Returns a pointer to the InterpreterValue, or nullptr if not found.
    void* GetStaticField(uint32_t module_id, uint32_t field_offset) noexcept;

    /// Number of loaded assemblies.
    uint32_t LoadedCount() const noexcept { return loaded_count_; }

    /// Maximum number of concurrent assemblies.
    static constexpr uint32_t kMaxAssemblies = 256;

    /// Next ALC ID (monotonically increasing, 0 = SharedContext reserved).
    static uint32_t NextAlcId() noexcept {
        static uint32_t s_next_alc_id = 1;
        return s_next_alc_id++;
    }

private:
    AssemblyManager() = default;
    ~AssemblyManager() = default;

    AssemblyManager(const AssemblyManager&) = delete;
    AssemblyManager& operator=(const AssemblyManager&) = delete;

    AssemblyLoadContext assemblies_[kMaxAssemblies]{};
    uint32_t loaded_count_ = 0;
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_ASSEMBLY_MANAGER_H_