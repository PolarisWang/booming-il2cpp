#include <cstdlib>
#include <cstring>
#include <chaos/log.h>
#include <chaos/native_types.h>
#include <chaos/type_info.h>
#include <runtime_abi.h>
#include "generated_code_compat.h"
#include "com_ccw.h"
#include "com_platform.h"
#include "com_typelib.h"
#include "memory_domain.h"

namespace chaos::il2cpp::com_ccw {
namespace {

// All-zero IUnknown GUID.
static const CHAOS_IL2CPP_UINT8 kIidIUnknown[16] = {0};

}  // anonymous namespace

// ── IUnknown method implementations (external linkage for generated code) ──

/// Recover ComCcw* from either a direct CCW pointer or an interface identity pointer.
/// Identity pointers are &ComCcwInterfaceEntry::vtable (the address of the vtable field).
/// Uses the back-pointer stored in entry->ccw_ptr at registration time.
inline ComCcw* ResolveCcw(void* self) noexcept {
    if (self == nullptr) return nullptr;

    // Two distinct `self` forms reach the CCW IUnknown methods:
    //   1. Direct ComCcw*  — the COM identity (IUnknown) pointer.  QI for IUnknown
    //      returns the CCW object itself (com_ccw.cpp `*ppv = ccw`), so self IS the
    //      struct base.
    //   2. Interface identity — `&ComCcwInterfaceEntry::vtable` for a registered
    //      non-IUnknown interface.  Its owner is entry->ccw_ptr (at self + 8).
    //
    // The legacy code ALWAYS treated self as form #2, doing
    // `self - offsetof(vtable)` then reading entry->ccw_ptr (= *(self + 8)).
    // For a form-#1 direct pointer, self + 8 is the CCW's `refcount` field (a
    // small counter), which was mis-read as an owner ComCcw* and dereferenced →
    // 0xC0000005.  We must distinguish forms safely, WITHOUT dereferencing an
    // unvalidated `entry->ccw_ptr`.
    //
    // Safe discriminator: read self + 8 as a raw integer (in-bounds for a direct
    // ComCcw* — it is refcount; in-bounds for an interface entry — it is ccw_ptr).
    // A real owner back-pointer is a 8-aligned Domain pointer well above the small
    // counter range that refcount exposes.  Only if it looks pointer-like do we
    // treat self as an interface identity and return it.
    const uintptr_t back = *reinterpret_cast<const uintptr_t*>(
        static_cast<char*>(self) - offsetof(ComCcwInterfaceEntry, vtable) +
        offsetof(ComCcwInterfaceEntry, ccw_ptr));
    constexpr uintptr_t kPtrMask = sizeof(void*) - 1;
    if (back != 0 && (back & kPtrMask) == 0 && back > 0x10000u) {
        return static_cast<ComCcw*>(reinterpret_cast<void*>(back));
    }
    // Direct CCW pointer: self IS the ComCcw.
    return static_cast<ComCcw*>(self);
}


CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwQueryInterface(
    void* self, const void* iid, void** ppv) noexcept {
    if (self == nullptr || ppv == nullptr) return kE_POINTER;
    auto* ccw = ResolveCcw(self);
    *ppv = nullptr;

    if (iid == nullptr) return kE_POINTER;

    // Aggregation: delegate all QI to the outer controlling IUnknown.
    if (ccw->is_aggregated && ccw->outer_unknown != nullptr) {
        // COM identity rule: QI for IUnknown returns the outer.
        // Non-IUnknown QI is delegated entirely — the outer controls identity.
        return runtime_core::com_platform::PlatformQueryInterface(ccw->outer_unknown, iid, ppv);
    }

    // Check for ITypeInfo/ITypeLib QI on IDispatch CCWs.
    if (ccw->typelib_data != nullptr) {
        if (CHAOS_IL2CPP_MEMCMP(iid, com_abi::kIidITypeInfo, 16) == 0) {
            auto* type_info = runtime_core::GetComTypeInfoForCcw(ccw);
            if (type_info != nullptr) {
                *ppv = type_info;
                ccw->refcount.fetch_add(1, std::memory_order_relaxed);
                return kS_OK;
            }
        }
        if (CHAOS_IL2CPP_MEMCMP(iid, com_abi::kIidITypeLib, 16) == 0) {
            auto* type_lib = runtime_core::GetComTypeLibForCcw(ccw);
            if (type_lib != nullptr) {
                *ppv = type_lib;
                ccw->refcount.fetch_add(1, std::memory_order_relaxed);
                return kS_OK;
            }
        }
    }

    // Non-aggregated: scan registered interfaces (entry 0 is always IUnknown).
    for (CHAOS_IL2CPP_SIZE i = 0; i < ccw->interface_count; ++i) {
        if (CHAOS_IL2CPP_MEMCMP(iid, ccw->interfaces[i].guid, 16) == 0) {
            if (i == 0) {
                // IUnknown identity: return self (the CCW object pointer).
                *ppv = ccw;
            } else {
                // Per-interface identity: return &entry.vtable so the caller's
                // first field dereference yields the interface vtable array.
                *ppv = &ccw->interfaces[i].vtable;
            }
            ccw->refcount.fetch_add(1, std::memory_order_relaxed);
            return kS_OK;
        }
    }

    return kE_NOINTERFACE;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL CcwAddRef(void* self) noexcept {
    if (self == nullptr) return 0;
    auto* ccw = ResolveCcw(self);
    if (ccw->is_aggregated && ccw->outer_unknown != nullptr) {
        return runtime_core::com_platform::PlatformAddRef(ccw->outer_unknown);
    }
    return ccw->refcount.fetch_add(1, std::memory_order_relaxed) + 1;
}

CHAOS_IL2CPP_UINT32 CHAOS_RUNTIME_ABI_CALL CcwRelease(void* self) noexcept {
    if (self == nullptr) return 0;
    auto* ccw = ResolveCcw(self);
    if (ccw->is_aggregated && ccw->outer_unknown != nullptr) {
        return runtime_core::com_platform::PlatformRelease(ccw->outer_unknown);
    }
    CHAOS_IL2CPP_UINT32 remaining = ccw->refcount.fetch_sub(1, std::memory_order_acq_rel) - 1;
    if (remaining == 0) {
        CHAOS_IL2CPP_LOG_DEBUG_M("COM", "CCW {0} refcount reached 0, freeing", static_cast<void*>(ccw));

        // Release the GCHandle so the GC can collect the managed object.
        if (ccw->gc_handle != static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID)) {
            const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
            if (abi != nullptr && abi->gc_handle_free != nullptr) {
                abi->gc_handle_free(
                    static_cast<RuntimeState*>(ccw->runtime_state),
                    static_cast<GCHandle>(ccw->gc_handle));
            }
        }

        // Free dynamic interface storage if allocated.
        if (ccw->interfaces != ccw->inline_interfaces) {
            memory_domain::DomainFreeTagged(ccw->interfaces);
        }

        memory_domain::DomainFreeTagged(ccw);
    }
    return remaining;
}

// ── IDispatch helper implementations ──

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwGetTypeInfoCount(void* self, CHAOS_IL2CPP_UINT32* pctinfo) noexcept {
    if (pctinfo == nullptr) return kE_POINTER;
    auto* ccw = ResolveCcw(self);
    // Return 1 if this CCW has TypeLib data (IDispatch interface), else 0.
    *pctinfo = (ccw != nullptr && ccw->typelib_data != nullptr) ? 1u : 0u;
    return kS_OK;
}

CHAOS_IL2CPP_INT32 CHAOS_RUNTIME_ABI_CALL CcwGetTypeInfo(void* self, CHAOS_IL2CPP_UINT32 iTInfo, CHAOS_IL2CPP_UINT32 lcid, void** ppTInfo) noexcept {
    if (ppTInfo == nullptr) return kE_POINTER;
    *ppTInfo = nullptr;
    auto* ccw = ResolveCcw(self);
    if (ccw == nullptr || ccw->typelib_data == nullptr) return kE_NOTIMPL;
    if (iTInfo != 0) return kE_NOTIMPL;  // Only one TypeLib (index 0).
    (void)lcid;  // Locale ignored for V2.

    auto* type_info = runtime_core::GetComTypeInfoForCcw(ccw);
    if (type_info == nullptr) return kE_NOTIMPL;
    *ppTInfo = type_info;
    ccw->refcount.fetch_add(1, std::memory_order_relaxed);
    return kS_OK;
}

// ── Static vtable ──────────────────────────────────────────────────

namespace {
ComCcwVtbl s_ccw_vtbl = {
    &CcwQueryInterface,
    &CcwAddRef,
    &CcwRelease,
};
}  // anonymous namespace

CHAOS_IL2CPP_INTPTR CreateCcw(void* managed_object, void* runtime_state) noexcept {
    if (managed_object == nullptr) return 0;

    auto* ccw = static_cast<ComCcw*>(memory_domain::DomainCurrentAllocateTagged(sizeof(ComCcw)));
    if (ccw == nullptr) return 0;

    // Allocate a GCHandle to root the managed object.
    CHAOS_IL2CPP_UINT64 gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID);
    const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
    if (abi != nullptr && abi->gc_handle_new != nullptr) {
        gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(
            abi->gc_handle_new(
                static_cast<RuntimeState*>(runtime_state),
                managed_object,
                false));  // not pinned
    }

    ccw->vtable = &s_ccw_vtbl;
    ccw->refcount = 1;
    ccw->gc_handle = gc_handle;
    ccw->runtime_state = runtime_state;

    // SmallVector: inline storage by default.
    ccw->interfaces = ccw->inline_interfaces;
    ccw->interface_capacity = kInlineCcwInterfaces;

    // Initialise interface table: slot 0 = IUnknown.
    ccw->interface_count = 1;
    ccw->interfaces[0].guid = kIidIUnknown;
    ccw->interfaces[0].vtable = &s_ccw_vtbl;
    ccw->interfaces[0].ccw_ptr = ccw;

    // TypeLib data initialised to nullptr (set by codegen if IDispatch).
    ccw->typelib_data = nullptr;
    ccw->cp_container = nullptr;

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "Created CCW {0} for managed object {1} (gc_handle={2})",
                              static_cast<void*>(ccw), managed_object, gc_handle);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw);
}

CHAOS_IL2CPP_INTPTR CreateCcwAggregated(void* managed_object, void* runtime_state,
                                         void* outer_unknown) noexcept {
    if (managed_object == nullptr || outer_unknown == nullptr) return 0;

    auto* ccw = static_cast<ComCcw*>(memory_domain::DomainCurrentAllocateTagged(sizeof(ComCcw)));
    if (ccw == nullptr) return 0;

    // Allocate a GCHandle to root the managed object.
    CHAOS_IL2CPP_UINT64 gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID);
    const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
    if (abi != nullptr && abi->gc_handle_new != nullptr) {
        gc_handle = static_cast<CHAOS_IL2CPP_UINT64>(
            abi->gc_handle_new(
                static_cast<RuntimeState*>(runtime_state),
                managed_object,
                false));  // not pinned
    }

    ccw->vtable = &s_ccw_vtbl;
    ccw->refcount = 1;                      // Inner refcount (not controlling)
    ccw->gc_handle = gc_handle;
    ccw->runtime_state = runtime_state;

    // SmallVector: inline storage by default.
    ccw->interfaces = ccw->inline_interfaces;
    ccw->interface_capacity = kInlineCcwInterfaces;

    ccw->interface_count = 1;
    ccw->outer_unknown = outer_unknown;
    ccw->is_aggregated = true;

    // interfaces[0] = IUnknown slot (populated for non-QI access).
    ccw->interfaces[0].guid = kIidIUnknown;
    ccw->interfaces[0].vtable = &s_ccw_vtbl;
    ccw->interfaces[0].ccw_ptr = ccw;

    for (CHAOS_IL2CPP_SIZE i = 1; i < kInlineCcwInterfaces; ++i) {
        ccw->inline_interfaces[i].guid = nullptr;
        ccw->inline_interfaces[i].vtable = nullptr;
        ccw->inline_interfaces[i].ccw_ptr = nullptr;
    }

    ccw->typelib_data = nullptr;
    ccw->cp_container = nullptr;

    // Hold a reference on the outer to keep it alive.
    runtime_core::com_platform::PlatformAddRef(outer_unknown);

    CHAOS_IL2CPP_LOG_DEBUG_M("COM",
        "Created aggregated CCW {0} for managed object {1}, outer={2}",
        static_cast<void*>(ccw), managed_object, outer_unknown);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(ccw);
}

void DestroyCcw(void* ccw_ptr) noexcept {
    if (ccw_ptr == nullptr) return;
    auto* ccw = static_cast<ComCcw*>(ccw_ptr);

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "Destroying CCW {0}", ccw_ptr);

    // Release the outer_unknown reference held since creation.
    if (ccw->is_aggregated && ccw->outer_unknown != nullptr) {
        runtime_core::com_platform::PlatformRelease(ccw->outer_unknown);
    }

    // Release the GCHandle so the GC can collect the managed object.
    if (ccw->gc_handle != static_cast<CHAOS_IL2CPP_UINT64>(CHAOS_GC_HANDLE_INVALID)) {
        const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
        if (abi != nullptr && abi->gc_handle_free != nullptr) {
            abi->gc_handle_free(
                static_cast<RuntimeState*>(ccw->runtime_state),
                static_cast<GCHandle>(ccw->gc_handle));
        }
    }

    // Free dynamic interface storage if allocated.
    if (ccw->interfaces != ccw->inline_interfaces) {
        memory_domain::DomainFreeTagged(ccw->interfaces);
    }

    memory_domain::DomainFreeTagged(ccw);
}

bool RegisterCcwInterface(void* ccw_ptr, const CHAOS_IL2CPP_UINT8* guid, void* vtable) noexcept {
    if (ccw_ptr == nullptr || guid == nullptr || vtable == nullptr) return false;
    auto* ccw = static_cast<ComCcw*>(ccw_ptr);

    if (ccw->interface_count >= kMaxCcwInterfaces) return false;

    // SmallVector: grow dynamically when inline storage is exhausted.
    if (ccw->interface_count >= ccw->interface_capacity) {
        CHAOS_IL2CPP_SIZE new_capacity = ccw->interface_capacity * 2;
        if (new_capacity > kMaxCcwInterfaces) new_capacity = kMaxCcwInterfaces;
        if (new_capacity <= ccw->interface_count) return false;

        auto* new_block = static_cast<ComCcwInterfaceEntry*>(
            memory_domain::DomainCurrentAllocateTagged(
                new_capacity * sizeof(ComCcwInterfaceEntry)));
        if (new_block == nullptr) return false;

        CHAOS_IL2CPP_MEMCPY(new_block, ccw->interfaces,
                             ccw->interface_count * sizeof(ComCcwInterfaceEntry));

        // Update ccw_ptr back-pointers in the new block.
        for (CHAOS_IL2CPP_SIZE i = 0; i < ccw->interface_count; ++i) {
            new_block[i].ccw_ptr = ccw;
        }

        // Free old dynamic block (but not inline storage).
        if (ccw->interfaces != ccw->inline_interfaces) {
            memory_domain::DomainFreeTagged(ccw->interfaces);
        }

        ccw->interfaces = new_block;
        ccw->interface_capacity = new_capacity;
    }

    CHAOS_IL2CPP_SIZE slot = ccw->interface_count++;
    ccw->interfaces[slot].guid = guid;
    ccw->interfaces[slot].vtable = vtable;
    ccw->interfaces[slot].ccw_ptr = ccw;

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "CCW {0}: registered interface at slot {1}",
                              ccw_ptr, slot);
    return true;
}

void CcwDispatchMethod(void* ccw_ptr, CHAOS_IL2CPP_UINT64 iface_stable_id, CHAOS_IL2CPP_UINT32 method_index) noexcept {
    if (ccw_ptr == nullptr) return;
    auto* ccw = static_cast<ComCcw*>(ccw_ptr);

    // Get the managed object from the GCHandle.
    const auto* abi = static_cast<const RuntimeAbiV0*>(chaos_runtime_get_abi_v0());
    if (abi == nullptr || abi->gc_handle_get == nullptr) return;
    void* obj = abi->gc_handle_get(
        static_cast<RuntimeState*>(ccw->runtime_state),
        static_cast<GCHandle>(ccw->gc_handle));
    if (obj == nullptr) return;

    // Get the type info for the managed object.
    const auto* type_info = chaos_object_get_type_info(obj);
    if (type_info == nullptr) return;

    // Get the warm section for interface map access.
    const auto* warm = reinterpret_cast<const TypeInfoWarm*>(
        reinterpret_cast<const char*>(type_info) + type_info->warm_delta);
    if (warm == nullptr || warm->iface_map == nullptr) return;

    // Walk the interface map to find the target interface.
    for (uint32_t i = 0; i < warm->iface_count; ++i) {
        if (warm->iface_map[i].iface_stable_id == iface_stable_id) {
            if (method_index >= warm->iface_map[i].method_count) return;
            uint32_t vtable_slot = warm->iface_map[i].vtable_offset + method_index;
            if (vtable_slot >= type_info->vtable_length) return;
            auto fn = reinterpret_cast<void (*)(void*)>(
                const_cast<void*>(type_info->vtable_array[vtable_slot]));
            if (fn != nullptr) {
                fn(obj);
            }
            return;
        }
    }
}

}  // namespace chaos::il2cpp::com_ccw
