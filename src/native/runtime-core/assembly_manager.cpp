// assembly_manager.cpp — Assembly-level lifecycle for hot-update packages
//
// Implements per-assembly static field isolation, load/unload lifecycle,
// and dispatch entry registration via hotpatch table integration.

#include "assembly_manager.h"

#include <interpreter_vm.h>       // interpreter::InterpreterValue
#include "memory_domain.h"        // memory_domain::{RegisterMemoryDomain, DomainScope, ...}
#include "metadata_interface.h"   // ModuleLifecycleManager

#include <mutex>

namespace chaos::il2cpp::runtime_core {

// ── Internal helpers ──────────────────────────────────────────────────

namespace {

/// Default number of static fields to pre-allocate when the assembly
/// doesn't specify a count. Grown on demand via GetStaticField.
constexpr uint32_t kDefaultStaticFieldCount = 64;

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

AssemblyDescriptor* AssemblyManager::LoadAssembly(
    const void* patch_data,
    size_t patch_size,
    const char* assembly_name,
    const char* host_type_name) noexcept
{
    if (patch_data == nullptr || patch_size == 0 || assembly_name == nullptr) {
        return nullptr;
    }

    // 1. Apply patch from memory — validates, resolves methods via
    //    HotpatchNameRegistry, marks dispatch entries as kHotpatchActive.
    auto* ctx = ApplyPatchFromMemory(patch_data, patch_size, host_type_name);
    if (ctx == nullptr || ctx->method_count == 0) {
        if (ctx != nullptr) {
            Unpatch(ctx);
        }
        return nullptr;
    }

    // 2. Determine module_id from the first patched method.
    //    All methods in one .patchdata belong to the same module.
    uint32_t module_id = ctx->methods[0].module_id;

    // 3. Create a memory domain for per-assembly allocations.
    memory_domain::DomainId domain_id = memory_domain::RegisterMemoryDomain(
        {assembly_name, 1 /* HotUpdate */, 0 /* unlimited */, nullptr});
    if (domain_id == memory_domain::kDomainIdInvalid) {
        Unpatch(ctx);
        return nullptr;
    }

    // 4. Pre-allocate static field storage on the assembly domain.
    auto* domain = memory_domain::FindDomainById(domain_id);
    void* static_fields = AllocateStaticFieldStorage(domain, kDefaultStaticFieldCount);
    if (static_fields == nullptr) {
        memory_domain::UnregisterMemoryDomain(domain_id);
        Unpatch(ctx);
        return nullptr;
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
    uint32_t slot = kMaxAssemblies;
    {
        // Thread-safe: only one load/unload at a time for the table.
        static std::mutex table_mutex;
        std::lock_guard<std::mutex> lock(table_mutex);

        for (uint32_t i = 0; i < kMaxAssemblies; ++i) {
            if (!assemblies_[i].is_loaded) {
                slot = i;
                break;
            }
        }

        if (slot >= kMaxAssemblies) {
            // Table full — roll back.
            memory_domain::UnregisterMemoryDomain(domain_id);
            Unpatch(ctx);
            return nullptr;
        }

        auto& desc = assemblies_[slot];
        desc.module_id              = module_id;
        desc.name                   = assembly_name;
        desc.patch_context          = ctx;
        desc.static_field_ptr       = static_fields;
        desc.static_field_count     = kDefaultStaticFieldCount;
        desc.domain_id              = domain_id;
        desc.generics_registered    = false;
        desc.is_loaded              = true;

        ++loaded_count_;
        return &desc;
    }
}

// ── UnloadAssembly ────────────────────────────────────────────────────

bool AssemblyManager::UnloadAssembly(AssemblyDescriptor* asm_desc) noexcept {
    if (asm_desc == nullptr || !asm_desc->is_loaded) {
        return false;
    }

    uint32_t module_id = asm_desc->module_id;

    // 1. Unpatch — clear kHotpatchActive flags on all dispatch entries.
    if (asm_desc->patch_context != nullptr) {
        Unpatch(asm_desc->patch_context);
        asm_desc->patch_context = nullptr;
    }

    // 2. Unregister generic instantiations for this module.
    if (asm_desc->generics_registered) {
        generic_context::UnregisterModuleGenerics(module_id);
        asm_desc->generics_registered = false;
    }

    // 3. Destroy the memory domain — bulk-frees static field storage
    //    and any per-assembly allocations.  The domain heap's Destroy()
    //    releases all memory without individual InterpreterValue dtors.
    if (asm_desc->domain_id != 0) {
        memory_domain::UnregisterMemoryDomain(asm_desc->domain_id);
        asm_desc->domain_id = 0;
    }

    // 4. Clear the descriptor.
    asm_desc->module_id            = 0;
    asm_desc->name.clear();
    asm_desc->patch_context        = nullptr;
    asm_desc->static_field_ptr     = nullptr;
    asm_desc->static_field_count   = 0;
    asm_desc->generics_registered  = false;
    asm_desc->is_loaded            = false;

    // 5. Update count (thread-safe increment is fine since we already
    //    hold the conceptual "slot" — a full table scan for decrement
    //    is not needed; just an atomic-ish decrement).
    if (loaded_count_ > 0) {
        --loaded_count_;
    }

    return true;
}

// ── FindAssembly ─────────────────────────────────────────────────────

AssemblyDescriptor* AssemblyManager::FindAssembly(const char* name) noexcept {
    if (name == nullptr || name[0] == '\0') return nullptr;

    for (uint32_t i = 0; i < kMaxAssemblies; ++i) {
        auto& desc = assemblies_[i];
        if (desc.is_loaded && desc.name == name) {
            return &desc;
        }
    }
    return nullptr;
}

// ── GetStaticField ───────────────────────────────────────────────────

void* AssemblyManager::GetStaticField(uint32_t module_id,
                                       uint32_t field_offset) noexcept {
    // Find the assembly that owns this module_id.
    AssemblyDescriptor* asm_desc = nullptr;
    for (uint32_t i = 0; i < kMaxAssemblies; ++i) {
        if (assemblies_[i].is_loaded && assemblies_[i].module_id == module_id) {
            asm_desc = &assemblies_[i];
            break;
        }
    }
    if (asm_desc == nullptr) return nullptr;

    // Grow the static field array if needed (realloc on the domain heap).
    if (field_offset >= asm_desc->static_field_count) {
        auto* domain = memory_domain::FindDomainById(asm_desc->domain_id);
        if (domain == nullptr) return nullptr;

        memory_domain::DomainScope scope(domain);
        uint32_t new_count = field_offset + 1u;
        // Round up to next power-of-2-ish boundary to amortize reallocs.
        if (new_count < asm_desc->static_field_count * 2) {
            new_count = asm_desc->static_field_count * 2;
        }

        void* grown = GrowStaticFieldStorage(
            asm_desc->static_field_ptr,
            asm_desc->static_field_count,
            new_count);
        if (grown == nullptr) {
            // Grow failed — can still access up to current count.
            return nullptr;
        }

        asm_desc->static_field_ptr   = grown;
        asm_desc->static_field_count = new_count;
    }

    // Return pointer to the InterpreterValue at field_offset.
    auto* fields = static_cast<interpreter::InterpreterValue*>(
        asm_desc->static_field_ptr);
    return &fields[field_offset];
}

}  // namespace chaos::il2cpp::runtime_core