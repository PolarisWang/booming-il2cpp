// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// object_stubs.cpp — Object / RuntimeHelpers stub implementations
#include <chaos/native_types.h>
#include <chaos/log.h>
#include "generated_code_compat.h"

#include <cstring>

#include "gc/gc_layout.h"
#include "gc/gc_helpers.h"
#include "core/gc_alloc_stubs.h"
#include "module_registry.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR ChaosObjectEqualsStatic(CHAOS_IL2CPP_INTPTR /*left*/, CHAOS_IL2CPP_INTPTR /*right*/) noexcept
{
    CHAOS_IL2CPP_LOG_WARN("Stub", "object_stubs called");
    return 0;
}

void ChaosObjectCtor(CHAOS_IL2CPP_INTPTR /*obj*/) noexcept
{
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersEquals(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept
{
    return (left == right) ? 1 : 0;
}

CHAOS_IL2CPP_INT32 ChaosRuntimeHelpersGetHashCode(CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (value == 0) return 0;
    auto p = static_cast<CHAOS_IL2CPP_UINTPTR>(value);
    return static_cast<CHAOS_IL2CPP_INT32>(p ^ (p >> 32));
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersGetObjectValue(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return value;
}

CHAOS_IL2CPP_INT32 ChaosObjectGetHashCode(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    if (obj == 0) return 0;
    auto p = static_cast<CHAOS_IL2CPP_UINTPTR>(obj);
    return static_cast<CHAOS_IL2CPP_INT32>(p ^ (p >> 32));
}
CHAOS_IL2CPP_INTPTR ChaosObjectToString(CHAOS_IL2CPP_INTPTR obj) noexcept {
    (void)obj;
    static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}
CHAOS_IL2CPP_INTPTR ChaosObjectGetType(CHAOS_IL2CPP_INTPTR obj) noexcept {
    (void)obj;
    static CHAOS_IL2CPP_UINT8 s_sentinel = 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&s_sentinel);
}

// ── MemberwiseClone ──────────────────────────────────────────────────
// Returns a shallow copy of the managed object at @a obj.
// Uses GcLayoutRegistry to determine the object size, allocates a new GC
// object via GcAllocate (scanned), and memcpy the instance data.
//
// Phase-1 limitation: GcLayout must be registered for the source type.
// Without it, returns the source pointer as identity (no crash but no clone).
// Full GcLayoutRegistry coverage deferred to Phase 1b.
CHAOS_IL2CPP_INTPTR ChaosObjectMemberwiseClone(CHAOS_IL2CPP_INTPTR obj) noexcept
{
    if (obj == 0) return 0;

    auto* src = reinterpret_cast<void*>(obj);
    auto* ti = chaos_object_get_type_info(src);
    if (ti == nullptr) return 0;

    auto& registry = GcLayoutRegistry::Instance();
    auto* layout = registry.Lookup(ti->stable_id);
    if (layout == nullptr) {
        // No GcLayout registered — fall back to identity (Phase 1b).
        return obj;
    }

    auto* clone = GcAllocateFast(layout->instance_size);
    if (clone == nullptr) return 0;

    std::memcpy(clone, src, layout->instance_size);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(clone);
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersGetUninitializedObject(CHAOS_IL2CPP_INTPTR type_handle) noexcept
{
    if (type_handle == 0) { fprintf(stderr, "[getui] FAIL: null handle\n"); return 0; }

    // Step 1: Read runtime_type_handle from System.Type object at offset 16.
    CHAOS_IL2CPP_INTPTR inner_handle = 0;
    std::memcpy(&inner_handle,
        reinterpret_cast<const void*>(static_cast<CHAOS_IL2CPP_INTPTR>(type_handle) + 16),
        sizeof(inner_handle));
    if (inner_handle == 0) { fprintf(stderr, "[getui] FAIL: inner_handle=0\n"); return 0; }
    fprintf(stderr, "[getui] inner_handle=0x%llx\n", (unsigned long long)inner_handle);

    // Step 2: Resolve TypeInfoHandle to TypeInfoHot* via module registry.
    TypeInfoHandle th = static_cast<TypeInfoHandle>(inner_handle);
    uint32_t module_id = static_cast<uint32_t>(th >> 32);
    uint32_t token = static_cast<uint32_t>(th & 0xFFFFFFFFu);
    fprintf(stderr, "[getui] module_id=%u token=0x%x\n", module_id, token);
    auto* type_info = LookupTypeInfoPtr(module_id, token);
    if (type_info == nullptr) { fprintf(stderr, "[getui] FAIL: type_info null\n"); return 0; }
    fprintf(stderr, "[getui] type_info=%p stable_id=0x%llx\n", (void*)type_info, (unsigned long long)type_info->stable_id);

    // Step 3: Look up GcTypeLayout by stable_id to get instance size.
    auto& registry = GcLayoutRegistry::Instance();
    auto* layout = registry.Lookup(type_info->stable_id);
    if (layout == nullptr) { fprintf(stderr, "[getui] FAIL: no GcLayout for stable_id\n"); return 0; }
    if (layout->instance_size == 0) { fprintf(stderr, "[getui] FAIL: instance_size=0\n"); return 0; }
    fprintf(stderr, "[getui] layout=%p instance_size=%u\n", (void*)layout, layout->instance_size);

    // Step 4: Allocate via ChaOS GC fast path (zero-initialized).
    void* obj = GcAllocateFast(layout->instance_size);
    if (obj == nullptr) { fprintf(stderr, "[getui] FAIL: GcAllocateFast returned null\n"); return 0; }

    // Step 5: Set header.type_info so the object has proper type identity.
    *static_cast<const TypeInfoHot**>(obj) = type_info;

    fprintf(stderr, "[getui] OK obj=%p\n", obj);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core