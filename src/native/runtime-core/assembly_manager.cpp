// assembly_manager.cpp — AssemblyLoadContext lifecycle for hot-update packages
//
// Implements per-assembly static field isolation, load/unload lifecycle,
// dispatch entry registration via hotpatch table integration, and
// ALC-level tombstone unloading via domain_unloader.

#include "assembly_manager.h"

#include <interpreter_vm.h>       // interpreter::InterpreterValue
#include "memory_domain.h"        // memory_domain::{RegisterMemoryDomain, DomainScope, ...}
#include "metadata_interface.h"   // ModuleLifecycleManager

#include "gc_static_roots.h"
#include "static_var_store.h"

#include <mutex>
#include <shared_mutex>

namespace chaos::il2cpp::runtime_core {

// ── Internal helpers ──────────────────────────────────────────────────

namespace {

/// Reader-writer lock guarding assemblies_ table and loaded_count_.
/// Read paths (FindAssembly, FindByModuleId, GetStaticField) acquire shared_lock.
/// Write paths (LoadAssembly, UnloadAssembly) acquire unique_lock.
/// Function-local static to avoid cross-TU static init ordering fiasco.
static std::shared_mutex& s_asm_mutex() {
    static std::shared_mutex mutex;
    return mutex;
}

/// Default number of static fields to pre-allocate when the assembly
/// doesn't specify a count. Grown on demand via GetStaticField.
constexpr uint32_t kDefaultStaticFieldCount = 64;

/// Maximum static field count per assembly to prevent unbounded growth.
constexpr uint32_t kMaxStaticFieldCount = 1024 * 1024;

/// Allocate an uninitialized InterpreterValue[] buffer on the given domain heap.
/// Returns nullptr on failure. The caller owns the returned pointer and must
/// DomainFreeTagged it or let Heap::Destroy bulk-free it.
static void* AllocateStaticFieldStorage(memory_domain::MemoryDomain* domain,
                                         uint32_t count) noexcept {
    if (domain == nullptr || count == 0) return nullptr;

    memory_domain::DomainScope scope(domain);
    size_t bytes = sizeof(interpreter::InterpreterValue) * count;
    auto* ptr = static_cast<interpreter::InterpreterValue*>(
        memory_domain::DomainCurrentAllocateTagged(bytes));
    if (ptr == nullptr) return nullptr;

    // Placement-new default-construct each element.
    for (uint32_t i = 0; i < count; ++i) {
        ::new (&ptr[i]) interpreter::InterpreterValue();
    }
    return ptr;
}

/// Grow an existing static field array to @a new_count on the same domain.
/// Returns the new pointer on success, or nullptr on failure (original
/// pointer remains valid).
static void* GrowStaticFieldStorage(void* old_ptr, uint32_t old_count,
                                     uint32_t new_count) noexcept {
    if (new_count <= old_count) return old_ptr;
    if (old_ptr == nullptr) return nullptr;

    size_t new_bytes = sizeof(interpreter::InterpreterValue) * new_count;
    auto* new_ptr = static_cast<interpreter::InterpreterValue*>(
        memory_domain::DomainCurrentReallocateTagged(old_ptr, new_bytes));
    if (new_ptr == nullptr) return old_ptr;  // original unchanged

    // Placement-new default-construct the newly added elements.
    for (uint32_t i = old_count; i < new_count; ++i) {
        ::new (&new_ptr[i]) interpreter::InterpreterValue();
    }
    return new_ptr;
}

}  // anonymous namespace

// ── AssemblyManager singleton ──────────────────────────────────────────

AssemblyManager* AssemblyManager::Get() noexcept {
    static AssemblyManager instance;
    return &instance;
}

// ── LoadAssembly ──────────────────────────────────────────────────────

AssemblyLoadContext* AssemblyManager::LoadAssembly(
    const void* patch_data,
    size_t patch_size,
    const char* assembly_name,
    const char* host_type_name) noexcept
{
    if (patch_data == nullptr || patch_size == 0 || assembly_name == nullptr) {
        return nullptr;
    }

    // 1. Create a memory domain for per-assembly allocations BEFORE applying
    //    the patch so that PatchContext/metadata (CreatePatchContext) uses
    //    domain-tagged allocation (R10: MD/IL metadata → MemoryDomain).
    memory_domain::DomainId domain_id = memory_domain::RegisterMemoryDomain(
        {assembly_name, 1 /* HotUpdate */, 0 /* unlimited */, nullptr});
    if (domain_id == memory_domain::kDomainIdInvalid) {
        return nullptr;
    }

    // 2. Apply patch from memory within the domain scope — PatchContext and
    //    all metadata allocations land on the domain heap.
    auto* domain = memory_domain::FindDomainById(domain_id);
    memory_domain::DomainScope scope(domain);

    auto* ctx = ApplyPatchFromMemory(patch_data, patch_size, host_type_name);
    if (ctx == nullptr || ctx->method_count == 0) {
        if (ctx != nullptr) {
            Unpatch(ctx);  // DomainFreeTagged — safe while domain heap lives
        }
        memory_domain::UnregisterMemoryDomain(domain_id);
        return nullptr;
    }

    // 3. Determine module_id from the first patched method.
    //    All methods in one .patchdata belong to the same module.
    uint32_t module_id = ctx->methods[0].module_id;

    // Validate module_id — the AOT-side module must be registered before
    // a hotpatch assembly can reference it.  A missing tombstone check
    // here is intentional: MarkModuleTombstone runs under the same
    // unique_lock acquired below, so there's no race between validation
    // and the slot assignment.
    if (LookupModule(module_id) == nullptr) {
        memory_domain::UnregisterMemoryDomain(domain_id);
        Unpatch(ctx);
        return nullptr;
    }

    // 4. Pre-allocate static field storage on the assembly domain.
    void* static_fields = AllocateStaticFieldStorage(domain, kDefaultStaticFieldCount);
    if (static_fields == nullptr) {
        memory_domain::UnregisterMemoryDomain(domain_id);
        Unpatch(ctx);
        return nullptr;
    }

    // Register static field range as a GC root.
    {
        size_t field_bytes = sizeof(interpreter::InterpreterValue) * kDefaultStaticFieldCount;
        GcRegisterStaticRootRange(static_fields, field_bytes, domain_id);
    }

    // 5. Register generic instantiations — the generic context data
    //    is embedded in the .patchdata's metadata and was already
    //    processed by ApplyPatchFromMemory.  ModuleLifecycleManager
    //    chains RegisterModuleGenerics.
    //
    //    For hotpatch assemblies, the AOT-side module is already
    //    registered.  Generics that involve hotpatch types are tracked
    //    separately via the generic_context subsystem.  The PatchContext
    //    metadata_cache holds token-resolution state but does not
    //    duplicate ModuleGenericRegistrationV0 — that is codegen-emitted
    //    and registered during AOT bootstrap.
    //
    //    Future: a ModuleGenericRegistrationV0 can be passed explicitly
    //    to ModuleLifecycleManager::RegisterModule if the .patchdata
    //    carries generic instantiation tables.

    // 6. Find a free slot and register the assembly.
    uint32_t slot = static_cast<uint32_t>(assemblies_.size());
    {
        // Thread-safe: use shared_mutex for table writes.
        std::unique_lock<std::shared_mutex> lock(s_asm_mutex());

        for (uint32_t i = 0; i < assemblies_.size(); ++i) {
            if (!assemblies_[i].is_loaded) {
                slot = i;
                break;
            }
        }

        if (slot >= assemblies_.size()) {
            // Table full — grow the vector (double capacity, min 64).
            size_t old_size = assemblies_.size();
            size_t new_size = old_size + (old_size < 64 ? 64 : old_size);
            assemblies_.resize(new_size);
            slot = static_cast<uint32_t>(old_size);
        }

        auto& desc = assemblies_[slot];
        desc.alc_id                 = AssemblyManager::NextAlcId();
        desc.module_id              = module_id;
        desc.name                   = assembly_name;
        desc.patch_context          = ctx;
        desc.static_field_ptr       = static_fields;
        desc.static_field_count     = kDefaultStaticFieldCount;
        desc.domain_id              = domain_id;
        desc.generics_registered    = false;
        desc.is_loaded              = true;
        desc.is_unloading.store(false, std::memory_order_relaxed);

        ++loaded_count_;
        {
            uint32_t hash = ComputeAssemblyHash(desc.name.c_str());
            static_var_store_register(hash, desc.module_id, desc.domain_id);
        }
        return &desc;
    }
}

// ── UnloadAssembly (ALC unload) ──────────────────────────────────────────

bool AssemblyManager::UnloadAssembly(AssemblyLoadContext* alc) noexcept {
    if (alc == nullptr || !alc->is_loaded) {
        return false;
    }

    // Set unloading flag — prevents new type lookups during teardown.
    alc->is_unloading.store(true, std::memory_order_release);

    // Acquire exclusive lock for the entire unload sequence.
    std::unique_lock<std::shared_mutex> lock(s_asm_mutex());

    uint32_t module_id = alc->module_id;

    // 1. Mark module tombstone — prevents future handle resolution.
    //    LookupModule(module_id) still returns a valid pointer but
    //    type_flags/image/type_info_ptrs are nulled for safety.
    MarkModuleTombstone(module_id);

    // 1a. Evict ICustomMarshaler cache — marshaler instances from the unloaded
    //     module are now stale.  Re-resolution happens lazily on next P/Invoke.
    ClearMarshalerCache();

    // 2. Unpatch — clear kHotpatchActive flags on all dispatch entries.
    if (alc->patch_context != nullptr) {
        Unpatch(alc->patch_context);
        alc->patch_context = nullptr;
    }

    // 3. Unregister generic instantiations for this module.
    if (alc->generics_registered) {
        generic_context::UnregisterModuleGenerics(module_id);
        alc->generics_registered = false;
    }

    // 4. Unregister static root ranges before destroying domain memory.
    if (alc->domain_id != 0) {
        GcUnregisterDomainStaticRoots(alc->domain_id);
        static_var_store_unregister_domain(alc->domain_id);
    }

    // 4a. Clean up field-level store for the AOT codegen indirect access path.
    {
        uint32_t hash = ComputeAssemblyHash(alc->name.c_str());
        static_var_store_unregister_assembly(hash);
    }

    // 5. Destroy the memory domain (safe stop-the-world teardown).
    //    UnloadDomain handles:
    //      - Safepoint request
    //      - Cross-domain reference scan + clear
    //      - Domain memory release
    //      - Memory domain unregistration
    //    The InterpreterValue[] static field storage is bulk-freed here.
    if (alc->domain_id != 0) {
        auto result = UnloadDomain(alc->domain_id);
        (void)result;  // Phase 2+: log cross_domain_refs_found/cleared
        alc->domain_id = 0;
    }

    // 6. Clear the descriptor.
    alc->alc_id                 = 0;
    alc->module_id              = 0;
    alc->name.clear();
    alc->patch_context          = nullptr;
    alc->static_field_ptr       = nullptr;
    alc->static_field_count     = 0;
    alc->generics_registered    = false;
    alc->is_loaded              = false;
    alc->is_unloading.store(false, std::memory_order_relaxed);

    // 6. Update count.
    if (loaded_count_ > 0) {
        --loaded_count_;
    }

    return true;
}

// ── FindAssembly ─────────────────────────────────────────────────────

AssemblyLoadContext* AssemblyManager::FindAssembly(const char* name) noexcept {
    if (name == nullptr || name[0] == '\0') return nullptr;

    std::shared_lock<std::shared_mutex> lock(s_asm_mutex());
    for (uint32_t i = 0; i < assemblies_.size(); ++i) {
        auto& desc = assemblies_[i];
        if (desc.is_loaded && desc.name == name) {
            return &desc;
        }
    }
    return nullptr;
}

// ── FindByModuleId ───────────────────────────────────────────────────

AssemblyLoadContext* AssemblyManager::FindByModuleId(uint32_t module_id) noexcept {
    if (module_id == 0) return nullptr;

    std::shared_lock<std::shared_mutex> lock(s_asm_mutex());
    for (uint32_t i = 0; i < assemblies_.size(); ++i) {
        auto& desc = assemblies_[i];
        if (desc.is_loaded && desc.module_id == module_id) {
            return &desc;
        }
    }
    return nullptr;
}

// ── GetStaticField ───────────────────────────────────────────────────

void* AssemblyManager::GetStaticField(uint32_t module_id,
                                       uint32_t field_offset) noexcept {
    // Reject clearly invalid offsets before lookup.
    if (field_offset >= kMaxStaticFieldCount) return nullptr;

    // Find the ALC that owns this module_id (read lock).
    AssemblyLoadContext* alc = nullptr;
    {
        std::shared_lock<std::shared_mutex> lock(s_asm_mutex());
        for (uint32_t i = 0; i < assemblies_.size(); ++i) {
            if (assemblies_[i].is_loaded && assemblies_[i].module_id == module_id) {
                alc = &assemblies_[i];
                break;
            }
        }
    }
    if (alc == nullptr) return nullptr;

    // Grow the static field array if needed (realloc on the domain heap).
    if (field_offset >= alc->static_field_count) {
        // Prevent near-UINT32_MAX field_offset from wrapping to 0.
        // kMaxStaticFieldCount check above already rejects near-wraparound values
        // (1M << UINT32_MAX), but keep the explicit guard for defense-in-depth.
        if (field_offset >= UINT32_MAX - 1) {
            return nullptr;
        }

        auto* domain = memory_domain::FindDomainById(alc->domain_id);
        if (domain == nullptr) return nullptr;

        memory_domain::DomainScope scope(domain);
        uint32_t new_count = field_offset + 1u;
        if (new_count < alc->static_field_count * 2) {
            new_count = alc->static_field_count * 2;
        }

        // Cap growth to kMaxStaticFieldCount.
        if (new_count > kMaxStaticFieldCount) {
            new_count = kMaxStaticFieldCount;
        }
        if (new_count <= alc->static_field_count) {
            return nullptr;  // already at cap
        }

        void* grown = GrowStaticFieldStorage(
            alc->static_field_ptr,
            alc->static_field_count,
            new_count);
        if (grown == nullptr) {
            return nullptr;
        }

        alc->static_field_ptr   = grown;
        alc->static_field_count = new_count;
    }

    auto* fields = static_cast<interpreter::InterpreterValue*>(
        alc->static_field_ptr);
    return &fields[field_offset];
}

}  // namespace chaos::il2cpp::runtime_core
