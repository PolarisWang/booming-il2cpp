// marshal_com.cpp -- COM apartment, RCW, CCW, Variant marshalling.
//
// Unity-file sub-module: #included by runtime_core.cpp (or compiled standalone).
// When compiled as part of the Unity build, runtime_core.h and its transitive
// includes (including the namespace and using declarations) are already in scope.
// For standalone TU compilation, include the necessary headers below.

#include <atomic>
#include <cstring>

// CHAOS_SPIN_HINT is defined in runtime_core.cpp when compiled as Unity build.
// Provide a local fallback for standalone TU compilation.
#include "com_platform.h"

#include <chaos/pal/pal_error.h>
#include <chaos/pal/pal_encoding.h>

#ifndef CHAOS_SPIN_HINT
#define CHAOS_SPIN_HINT() CHAOS_IL2CPP_PAUSE_HINT()
#endif

#include "marshal_internal.h"
#include "runtime_core.h"
#include "struct_marshal.h"

namespace chaos::il2cpp::runtime_core {
using namespace chaos::il2cpp::marshal_abi;

// TLS: HRESULT stored by ChaosThrowComExceptionForHR so the managed-side
// COMException constructor can retrieve it via ChaosGetComFailureHR().
thread_local CHAOS_IL2CPP_INT32 tls_com_failure_hr = 0;

// ── COM apartment management ────────────────────────────────────────

CHAOS_IL2CPP_INTPTR CoCreateComInstance(
    const CHAOS_IL2CPP_UINT8* clsid_bytes,
    const CHAOS_IL2CPP_UINT8* iid_bytes) noexcept {
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        com_platform::PlatformCoCreateInstance(
            clsid_bytes, iid_bytes));
}

CHAOS_IL2CPP_INTPTR CoCreateComInstanceAggregated(
    const CHAOS_IL2CPP_UINT8* clsid_bytes,
    const CHAOS_IL2CPP_UINT8* iid_bytes,
    CHAOS_IL2CPP_INTPTR outer_unknown) noexcept {
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        com_platform::PlatformCoCreateInstanceAggregated(
            clsid_bytes, iid_bytes, reinterpret_cast<void*>(outer_unknown)));
}

// ── RCW (Runtime Callable Wrapper) ─────────────────────────────────

CHAOS_IL2CPP_INTPTR MarshalCreateRcw(CHAOS_IL2CPP_INTPTR unknown_ptr) noexcept {
    if (unknown_ptr == 0) return 0;
    auto* rcw = com_rcw::FindOrCreateRcw(reinterpret_cast<void*>(unknown_ptr));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(rcw);
}

void MarshalReleaseRcw(CHAOS_IL2CPP_INTPTR rcw_native_ptr) noexcept {
    if (rcw_native_ptr == 0) return;
    com_rcw::ReleaseRcw(static_cast<com_rcw::ComRcwNative*>(
        reinterpret_cast<void*>(rcw_native_ptr)));
}

CHAOS_IL2CPP_INTPTR MarshalGetRcwUnknown(CHAOS_IL2CPP_INTPTR rcw_native_ptr) noexcept {
    if (rcw_native_ptr == 0) return 0;
    auto* rcw = static_cast<com_rcw::ComRcwNative*>(
        reinterpret_cast<void*>(rcw_native_ptr));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(rcw->identity_unknown);
}

CHAOS_IL2CPP_INTPTR MarshalRcwQueryInterface(
    CHAOS_IL2CPP_INTPTR rcw_native_ptr,
    const CHAOS_IL2CPP_UINT8* iid_bytes) noexcept {
    if (rcw_native_ptr == 0 || iid_bytes == nullptr) return 0;
    auto* rcw = static_cast<com_rcw::ComRcwNative*>(
        reinterpret_cast<void*>(rcw_native_ptr));
    void* result = com_rcw::QueryInterfaceCached(rcw, iid_bytes);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

bool MarshalIsRcwHandle(CHAOS_IL2CPP_INTPTR ptr) noexcept {
    return com_rcw::IsComRcwHandle(ptr);
}

// ── ComVtable dispatch helpers (test/verification) ──────────────────
// These mirror the codegen's RCW-handle-aware vtable dispatch logic.
// They are used by P/Invoke test helpers in per-family verification builds.

/// RCW-aware ComVtable dispatch: if ptr is an RCW handle, extract
/// identity_unknown; then call vtable[slot](com_ptr, a, b).
/// Returns the int32 result from the COM method.
CHAOS_IL2CPP_INT32 MarshalCallComMethod(
    CHAOS_IL2CPP_INTPTR ptr,
    CHAOS_IL2CPP_INT32 slot,
    CHAOS_IL2CPP_INT32 a,
    CHAOS_IL2CPP_INT32 b) noexcept
{
    if (ptr == 0) return 0;
    void* com_ptr = reinterpret_cast<void*>(ptr);

    // If ptr is an RCW handle, extract the identity IUnknown*.
    if (com_rcw::IsComRcwHandle(ptr)) {
        auto* rcw = static_cast<com_rcw::ComRcwNative*>(com_ptr);
        com_ptr = rcw->identity_unknown;
        if (com_ptr == nullptr) return 0;
    }

    // COM vtable dispatch: first read the vtable pointer, then call slot.
    using ComMethod = CHAOS_IL2CPP_INT32 (*)(void*, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32);
    void** vtable = *static_cast<void***>(com_ptr);
    auto method = reinterpret_cast<ComMethod>(vtable[slot]);
    return method(com_ptr, a, b);
}

/// Direct ComVtable dispatch (no RCW check): treat ptr as raw COM pointer
/// and call vtable[slot](com_ptr, a, b).
CHAOS_IL2CPP_INT32 MarshalCallDirectComMethod(
    CHAOS_IL2CPP_INTPTR com_ptr,
    CHAOS_IL2CPP_INT32 slot,
    CHAOS_IL2CPP_INT32 a,
    CHAOS_IL2CPP_INT32 b) noexcept
{
    if (com_ptr == 0) return 0;
    void* ptr = reinterpret_cast<void*>(com_ptr);

    using ComMethod = CHAOS_IL2CPP_INT32 (*)(void*, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32);
    void** vtable = *static_cast<void***>(ptr);
    auto method = reinterpret_cast<ComMethod>(vtable[slot]);
    return method(ptr, a, b);
}

void ChaosThrowComExceptionForHR(CHAOS_IL2CPP_INT32 hr) noexcept {
    // Sentinel-based COMException delivery.  The kManagedExceptionComFailure
    // sentinel propagates through generated AOT catch blocks (which re-throw
    // sentinels) and the interpreter SEH layer up to the managed exception
    // dispatch, which creates a managed COMException from the HRESULT.
    // Store the HR in TLS so the managed-side COMException constructor can
    // retrieve it via ChaosGetComFailureHR().
    CHAOS_IL2CPP_LOG_WARN_M("COM", "ChaosThrowComExceptionForHR HRESULT 0x{0:08x} - throwing COMException sentinel",
                             static_cast<unsigned int>(hr));
    tls_com_failure_hr = hr;
    throw chaos_managed_exception{kManagedExceptionComFailure};
}

CHAOS_IL2CPP_INT32 ChaosGetComFailureHR() noexcept {
    return tls_com_failure_hr;
}

// ── CCW (COM Callable Wrapper) ─────────────────────────────────────

CHAOS_IL2CPP_INTPTR MarshalCreateCcw(
    CHAOS_IL2CPP_INTPTR managed_object,
    CHAOS_IL2CPP_INTPTR runtime_state) noexcept {
    return com_ccw::CreateCcw(
        reinterpret_cast<void*>(managed_object),
        reinterpret_cast<void*>(runtime_state));
}

CHAOS_IL2CPP_INTPTR MarshalCreateCcwAggregated(
    CHAOS_IL2CPP_INTPTR managed_object,
    CHAOS_IL2CPP_INTPTR runtime_state,
    CHAOS_IL2CPP_INTPTR outer_unknown) noexcept {
    return com_ccw::CreateCcwAggregated(
        reinterpret_cast<void*>(managed_object),
        reinterpret_cast<void*>(runtime_state),
        reinterpret_cast<void*>(outer_unknown));
}

// ── Variant support ─────────────────────────────────────────────────

// Minimal VARIANT vt-field values (Win32 COM ABI).  When <oleauto.h> is
// available these match the system definitions; defined here to avoid a
// hard dependency on oleauto.h in the build.
#ifndef CHAOS_IL2CPP_VT_EMPTY
#define CHAOS_IL2CPP_VT_EMPTY    0
#define CHAOS_IL2CPP_VT_NULL     1
#define CHAOS_IL2CPP_VT_I2       2
#define CHAOS_IL2CPP_VT_I4       3
#define CHAOS_IL2CPP_VT_R4       4
#define CHAOS_IL2CPP_VT_R8       5
#define CHAOS_IL2CPP_VT_BOOL     11
#define CHAOS_IL2CPP_VT_BSTR     8
#define CHAOS_IL2CPP_VT_UNKNOWN  13
#define CHAOS_IL2CPP_VT_BYREF    0x4000
#endif

// Minimal VARIANT layout (16 bytes on x64, 16 bytes on x86).
struct alignas(CHAOS_IL2CPP_SIZE) VariantLayout {
    CHAOS_IL2CPP_UINT16 vt;
    CHAOS_IL2CPP_UINT16 reserved1;
    CHAOS_IL2CPP_UINT16 reserved2;
    CHAOS_IL2CPP_UINT16 reserved3;
    union {
        CHAOS_IL2CPP_INT16   iVal;
        CHAOS_IL2CPP_INT32   lVal;
        float                fltVal;
        double               dblVal;
        CHAOS_IL2CPP_INT32   boolVal;
        void*                bstrVal;
        void*                punkVal;
    } data;
};

// ICALL: Marshal.GetObjectForNativeVariant(IntPtr) → Object
void* CHAOS_RUNTIME_ABI_CALL ChaosGetObjectForNativeVariant(CHAOS_IL2CPP_INTPTR variant_ptr) noexcept {
#if defined(_WIN32)
    if (variant_ptr == 0) return nullptr;
    const auto* v = static_cast<const VariantLayout*>(
        reinterpret_cast<const void*>(variant_ptr));

    auto* ts = GetCurrentThreadState();
    auto* rs = ts ? ts->runtime_state : nullptr;

    auto vt = static_cast<CHAOS_IL2CPP_UINT16>(v->vt & ~CHAOS_IL2CPP_VT_BYREF);
    switch (vt) {
    case CHAOS_IL2CPP_VT_EMPTY:
    case CHAOS_IL2CPP_VT_NULL:
        return nullptr;
    case CHAOS_IL2CPP_VT_I2:
        if (rs == nullptr || ts == nullptr) return nullptr;
        {
            auto type_handle = ResolveTypeByName("System.Int16");
            if (type_handle == 0) return nullptr;
            return BoxValueObject(rs, ts, type_handle, &v->data.iVal, sizeof(v->data.iVal));
        }
    case CHAOS_IL2CPP_VT_I4:
        if (rs == nullptr || ts == nullptr) return nullptr;
        {
            auto type_handle = ResolveTypeByName("System.Int32");
            if (type_handle == 0) return nullptr;
            return BoxValueObject(rs, ts, type_handle, &v->data.lVal, sizeof(v->data.lVal));
        }
    case CHAOS_IL2CPP_VT_R4:
        if (rs == nullptr || ts == nullptr) return nullptr;
        {
            auto type_handle = ResolveTypeByName("System.Single");
            if (type_handle == 0) return nullptr;
            return BoxValueObject(rs, ts, type_handle, &v->data.fltVal, sizeof(v->data.fltVal));
        }
    case CHAOS_IL2CPP_VT_R8:
        if (rs == nullptr || ts == nullptr) return nullptr;
        {
            auto type_handle = ResolveTypeByName("System.Double");
            if (type_handle == 0) return nullptr;
            return BoxValueObject(rs, ts, type_handle, &v->data.dblVal, sizeof(v->data.dblVal));
        }
    case CHAOS_IL2CPP_VT_BOOL:
        if (rs == nullptr || ts == nullptr) return nullptr;
        {
            auto type_handle = ResolveTypeByName("System.Boolean");
            if (type_handle == 0) return nullptr;
            CHAOS_IL2CPP_INT32 bool_val = (v->data.boolVal != 0) ? 1 : 0;
            return BoxValueObject(rs, ts, type_handle, &bool_val, sizeof(bool_val));
        }
    case CHAOS_IL2CPP_VT_BSTR:
        return MarshalPtrToStringBSTR(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(v->data.bstrVal));
    case CHAOS_IL2CPP_VT_UNKNOWN:
        return v->data.punkVal != nullptr
            ? reinterpret_cast<void*>(MarshalCreateRcw(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(v->data.punkVal)))
            : nullptr;
    default:
        return nullptr;
    }
#else
    (void)variant_ptr;
    return nullptr;
#endif
}

// ICALL: Marshal.GetNativeVariantForObject(Object, IntPtr, IntPtr) → void
void CHAOS_RUNTIME_ABI_CALL ChaosGetNativeVariantForObject(
    void* obj, CHAOS_IL2CPP_INTPTR variant_ptr, CHAOS_IL2CPP_INTPTR destroy_old) noexcept {
#if defined(_WIN32)
    if (variant_ptr == 0) return;
    auto* v = static_cast<VariantLayout*>(reinterpret_cast<void*>(variant_ptr));
    if (destroy_old != 0 && v->vt != CHAOS_IL2CPP_VT_EMPTY) {
        CHAOS_IL2CPP_MEMSET(v, 0, sizeof(VariantLayout));
    }
    if (obj == nullptr) {
        v->vt = CHAOS_IL2CPP_VT_NULL;
        return;
    }

    // Inspect the managed object's type to determine the correct VT.
    // Boxed value-type layout: TypeInfoHot* (8B) [+ sync (8B if ThinLockable)] + value.
    using ::chaos::il2cpp::common::kTypeInfoHeaderKindMask;
    using ::chaos::il2cpp::common::kTypeInfoHeaderKindPure;
    const auto* obj_type = *static_cast<const TypeInfoHot* const*>(obj);
    const CHAOS_IL2CPP_SIZE hdr = (obj_type->flags & kTypeInfoHeaderKindMask) == kTypeInfoHeaderKindPure ? 8u : 16u;
    const void* val_ptr = static_cast<const uint8_t*>(obj) + hdr;

    // Resolve primitive type handles and compare against the object's type.
    auto th = ResolveTypeByName("System.Int32");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I4;
        v->data.lVal = *static_cast<const CHAOS_IL2CPP_INT32*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.Int16");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I2;
        v->data.iVal = *static_cast<const CHAOS_IL2CPP_INT16*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.Byte");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I4;
        v->data.lVal = *static_cast<const CHAOS_IL2CPP_UINT8*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.SByte");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I4;
        v->data.lVal = *static_cast<const CHAOS_IL2CPP_INT8*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.UInt16");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I4;
        v->data.lVal = *static_cast<const CHAOS_IL2CPP_UINT16*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.UInt32");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I4;
        v->data.lVal = static_cast<CHAOS_IL2CPP_INT32>(*static_cast<const CHAOS_IL2CPP_UINT32*>(val_ptr));
        return;
    }
    th = ResolveTypeByName("System.Single");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_R4;
        v->data.fltVal = *static_cast<const float*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.Double");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_R8;
        v->data.dblVal = *static_cast<const double*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.Boolean");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_BOOL;
        // VARIANT_TRUE = -1 (0xFFFF), VARIANT_FALSE = 0
        const CHAOS_IL2CPP_UINT8 b = *static_cast<const CHAOS_IL2CPP_UINT8*>(val_ptr);
        v->data.boolVal = (b != 0) ? -1 : 0;
        return;
    }

    // Fallback: marshal as VT_UNKNOWN.
    v->vt = CHAOS_IL2CPP_VT_UNKNOWN;
    v->data.punkVal = obj;
#else
    (void)obj;
    (void)variant_ptr;
    (void)destroy_old;
#endif
}

}  // namespace chaos::il2cpp::runtime_core