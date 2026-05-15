// eeclass.cpp — EEClass lazy-fill logic
//
// Implements EnsureEEClass and the Ensure*Filled helpers.  For AOT types
// the EEClass is a domain-allocated container whose LazyArray pointers
// reference the constexpr ReflectionQuery*Descriptor data emitted by the
// codegen.  For dynamic HotUpdate types the data is filled from the patch
// metadata cache (or allocated empty if not yet available).
//
// Design rationale:
//   - AOT fast path (already-resolved descriptor): zero allocations,
//     pointers alias constexpr arrays directly.
//   - Dynamic path: domain heap allocation, filled lazily on first
//     reflection access.  The cost is paid once per type per domain.

#include "eeclass.h"
#include "module_registry.h"
#include "memory_domain.h"

namespace chaos::il2cpp::runtime_core {

// ── Internal: reverse-lookup MethodTable* → ReflectionQueryTypeDescriptor ──
// Scans all registered modules and their type_info_ptrs arrays for a
// pointer match.  This is O(total_types) but only called on the first
// EEClass access for AOT types (cold path, acceptable).
static const ReflectionQueryTypeDescriptor* FindDescriptorByMethodTable(const MethodTable* mt) noexcept {
    if (mt == nullptr) return nullptr;

    const uint32_t count = GetModuleCount();
    for (uint32_t i = 0; i < count; i++) {
        const auto* mod = GetModuleByIndex(i);
        if (mod == nullptr || mod->type_info_ptrs == nullptr || mod->image == nullptr) continue;
        for (uint32_t j = 0; j < mod->type_count; j++) {
            if (reinterpret_cast<const MethodTable*>(mod->type_info_ptrs[j]) == mt) {
                return mod->image->types[j];
            }
        }
    }
    return nullptr;
}

// ── EnsureEEClass ──────────────────────────────────────────────────────
// Attaches an EEClass to the given MethodTable.  For AOT types the EEClass
// is domain-allocated and points into constexpr descriptor data.  For
// dynamic types it is allocated and left empty (filled on demand by the
// Ensure*Filled helpers).
bool EnsureEEClass(MethodTable* mt) noexcept {
    if (mt == nullptr) return false;
    if (mt->cold_delta != 0) return true;  // already attached

    // ── AOT path: reverse-lookup through module registry ──────────
    const auto* desc = FindDescriptorByMethodTable(mt);
    if (desc != nullptr) {
        auto* ee = CHAOS_IL2CPP_DOMAIN_CURRENT_NEW(EEClass);
        if (ee == nullptr) return false;

        ee->name_utf8      = desc->name_utf8;
        ee->namespace_utf8 = desc->namespace_name_utf8;
        ee->mt             = mt;

        // Reference constexpr descriptor arrays directly (zero copy).
        if (desc->methods != nullptr && desc->method_count > 0) {
            ee->methods.data   = reinterpret_cast<const ReflectionMethodEntry*>(desc->methods);
            ee->methods.count  = desc->method_count;
            ee->methods.filled = true;
        }
        if (desc->fields != nullptr && desc->field_count > 0) {
            ee->fields.data   = reinterpret_cast<const ReflectionFieldEntry*>(desc->fields);
            ee->fields.count  = desc->field_count;
            ee->fields.filled = true;
        }
        if (desc->properties != nullptr && desc->property_count > 0) {
            ee->properties.data   = reinterpret_cast<const ReflectionPropertyEntry*>(desc->properties);
            ee->properties.count  = desc->property_count;
            ee->properties.filled = true;
        }

        // Generic metadata
        if (desc->generic_type_definition != nullptr) {
            // Convert generic_type_definition from descriptor to MethodTable*.
            // In AOT, the constexpr ReflectionQueryTypeDescriptor IS the MethodTable
            // (first 32B are TypeInfoHot compatible), so a direct reinterpret_cast
            // is valid.
            ee->generic_type_def = const_cast<MethodTable*>(
                reinterpret_cast<const MethodTable*>(desc->generic_type_definition));
        }

        // Attach via cold_delta byte offset from MethodTable base.
        mt->cold_delta = static_cast<uint32_t>(
            reinterpret_cast<uint8_t*>(ee) - reinterpret_cast<uint8_t*>(mt));
        return true;
    }

    // ── Dynamic type path: allocate empty EEClass ──────────────────
    // Metadata will be filled on demand by Ensure*Filled() from the
    // PatchMetadataCache once the HotUpdate pipeline provides it.
    auto* ee = CHAOS_IL2CPP_DOMAIN_CURRENT_NEW(EEClass);
    if (ee == nullptr) return false;

    ee->mt = mt;
    // name/namespace are not available until the dynamic type descriptor
    // is registered (Phase 1+ enhancement).
    mt->cold_delta = static_cast<uint32_t>(
        reinterpret_cast<uint8_t*>(ee) - reinterpret_cast<uint8_t*>(mt));
    return true;
}

// ── Ensure*Filled helpers ─────────────────────────────────────────────
// For AOT types these are effectively no-ops (data was linked in
// EnsureEEClass).  For dynamic types these populate from patch metadata.

void EnsureMethodsFilled(EEClass* ee) noexcept {
    if (ee == nullptr || ee->methods.filled) return;

    // Dynamic type path: fill from PatchMetadataCache.
    // Phase 1+ implementation: look up the method descriptors by
    // (mt->module_id, mt->token) and allocate+copy into domain heap.
    //
    // For now, leave empty (methods will return 0 gracefully).
    ee->methods.filled = true;
}

void EnsureFieldsFilled(EEClass* ee) noexcept {
    if (ee == nullptr || ee->fields.filled) return;
    ee->fields.filled = true;
}

void EnsurePropertiesFilled(EEClass* ee) noexcept {
    if (ee == nullptr || ee->properties.filled) return;
    ee->properties.filled = true;
}

void EnsureEventsFilled(EEClass* ee) noexcept {
    if (ee == nullptr || ee->events.filled) return;
    ee->events.filled = true;
}

}  // namespace chaos::il2cpp::runtime_core