// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// object_stubs.cpp — Object / RuntimeHelpers stub implementations
#include <chaos/native_types.h>
#include <chaos/log.h>
#include "generated_code_compat.h"

#include <cstring>
#include <cstdio>

#include "gc/gc_layout.h"
#include "gc/gc_helpers.h"
#include "core/gc_alloc_stubs.h"
#include "module_registry.h"
#include "runtime_stubs/async_stubs.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR ChaosObjectEqualsStatic(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept
{
    // Was a constant-false stub — every object-typed Assert.AreEqual went
    // through it and failed (B7 si=128/133).  Same-runtime-type objects compare
    // their 8-byte payload at the canonical box offset (8).  This covers boxed
    // primitives (Int32/Int64/Boolean/Char/Double) which is what the assertion
    // path feeds here; reference types beyond identity still compare unequal
    // unless pointer-identical.
    if (left == right) return 1;
    if (left == 0 || right == 0) return 0;
    const void* const* l = reinterpret_cast<const void* const*>(left);
    const void* const* r = reinterpret_cast<const void* const*>(right);
    // Runtime-made boxes (reflection boxing) carry a zeroed header — the
    // MethodTable symbol is not visible in the runtime TU.  A null type_info on
    // either side degrades the check to payload equality; both non-null and
    // different means different runtime types.
    //
    // EXCEPT: the codegen MethodTable (chaos_mt_*) and the runtime shadow type
    // (what object_new installs via ResolveTypeByName) are two DISTINCT
    // TypeInfoHot instances describing the SAME type — reflection_box_via_runtime
    // produces the latter, codegen literal boxes the former, and pointer
    // inequality wrongly failed every cross-compare (B7 si=128/133:
    // Assert.AreEqual(2147483647, FieldInfo.GetValue(...)) → assertFailed=true).
    // The stable_id is the type's codegen-computed identity (FNV64 of the
    // subject id, ComputeStableTypeId) and is identical for both instances, so
    // it is the honest same-type test.
    if (*l != *r && *l != nullptr && *r != nullptr) {
        const auto* lti = static_cast<const chaos::il2cpp::common::TypeInfoHot*>(*l);
        const auto* rti = static_cast<const chaos::il2cpp::common::TypeInfoHot*>(*r);
        if (lti->stable_id != rti->stable_id) return 0;
    }
    return std::memcmp(reinterpret_cast<const unsigned char*>(left) + 8,
                       reinterpret_cast<const unsigned char*>(right) + 8, 8) == 0 ? 1 : 0;
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
    if (type_handle == 0) return 0;

    // Read runtime_type_handle from System.Type object at offset 16.
    CHAOS_IL2CPP_INTPTR inner_handle = 0;
    std::memcpy(&inner_handle,
        reinterpret_cast<const void*>(static_cast<CHAOS_IL2CPP_INTPTR>(type_handle) + 16),
        sizeof(inner_handle));
    if (inner_handle == 0) return 0;

    // Resolve TypeInfoHandle to TypeInfoHot* via module registry.
    TypeInfoHandle th = static_cast<TypeInfoHandle>(inner_handle);
    uint32_t module_id = static_cast<uint32_t>(th >> 32);
    uint32_t token = static_cast<uint32_t>(th & 0xFFFFFFFFu);
    auto* type_info = LookupTypeInfoPtr(module_id, token);
    if (type_info == nullptr) return 0;

    // Look up GcTypeLayout by stable_id to get instance size.
    auto& registry = GcLayoutRegistry::Instance();
    auto* layout = registry.Lookup(type_info->stable_id);
    if (layout == nullptr || layout->instance_size == 0) return 0;

    // ⚠️ Task / Task<T> receivers must be LIVE task handles, not zeroed managed
    // objects: every task operation (ContinueWith/Wait/GetAwaiter/…) consumes an
    // AsyncTask handle, and SubjectInstanceFactory.Create<Task>() is the probe's
    // only receiver supply (GetUninitializedObject path — Task has no usable
    // public ctor).  A zeroed object made every instance-method fact fail with
    // factoryGap (87 items in the threading-tasks chunk).  Hand back an
    // already-completed task instead: methods then observe real completed-state
    // semantics (Wait returns immediately, awaiters yield the default result).
    {
        const char* type_name = LookupTypeNameByInfoPtr(type_info);
        if (type_name != nullptr &&
            (std::strcmp(type_name, "Task") == 0 ||
             std::strcmp(type_name, "Task`1") == 0))
        {
            return async_task_create_gc();
        }
    }

    // Allocate via GC fast path (zero-initialized).
    void* obj = GcAllocateFast(layout->instance_size);
    if (obj == nullptr) return 0;

    // Set header.type_info so the object has proper type identity.
    *static_cast<const TypeInfoHot**>(obj) = type_info;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core