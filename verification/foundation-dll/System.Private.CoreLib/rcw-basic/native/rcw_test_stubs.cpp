// rcw-basic test stubs — provides extern "C" wrappers for runtime functions
// that are referenced as DllImport("__Internal") by the managed test code,
// and forward declarations for managed method symbols referenced by the
// COM vtable array but not directly reachable by the codegen.
//
// Also provides stub implementations for external runtime functions
// (GCHandle, Marshal) that can't be resolved through the hotpatch name
// registry in per-family verification builds.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "runtime_core.h"
#include "engine_binding.h"

// Local definition of managed array layout (mirrors chaos::il2cpp::codegen::chaos_managed_array
// from ChaosGeneratedRuntimePrelude.h, which is in a namespace not visible at global scope).
struct StubManagedArray
{
    void* header;
    unsigned char element_type_shape;
    void* element_type_info;
    CHAOS_IL2CPP_INTPTR length;
    CHAOS_IL2CPP_INTPTR* elements;
};

// ── Runtime helper wrappers (extern "C" → namespace-scoped) ──────────
// The managed test code uses [DllImport("__Internal")] which causes the
// codegen to emit extern "C" P/Invoke stubs.  The actual implementations
// live in chaos::il2cpp::runtime_core so we need C-linkage trampolines.

extern "C" {

CHAOS_IL2CPP_INTPTR MarshalCreateCcw(
    CHAOS_IL2CPP_INTPTR managed_object,
    CHAOS_IL2CPP_INTPTR runtime_state_ptr)
{
    return chaos::il2cpp::runtime_core::MarshalCreateCcw(
        managed_object, runtime_state_ptr);
}

CHAOS_IL2CPP_INTPTR MarshalCreateRcw(CHAOS_IL2CPP_INTPTR unknown_ptr)
{
    return chaos::il2cpp::runtime_core::MarshalCreateRcw(unknown_ptr);
}

void MarshalReleaseRcw(CHAOS_IL2CPP_INTPTR rcw_native_ptr)
{
    chaos::il2cpp::runtime_core::MarshalReleaseRcw(rcw_native_ptr);
}

CHAOS_IL2CPP_INTPTR MarshalGetRcwUnknown(CHAOS_IL2CPP_INTPTR rcw_native_ptr)
{
    return chaos::il2cpp::runtime_core::MarshalGetRcwUnknown(rcw_native_ptr);
}

CHAOS_IL2CPP_INTPTR MarshalRcwQueryInterface(
    CHAOS_IL2CPP_INTPTR rcw_native_ptr,
    const CHAOS_IL2CPP_UINT8* iid_bytes)
{
    return chaos::il2cpp::runtime_core::MarshalRcwQueryInterface(
        rcw_native_ptr, iid_bytes);
}

	// ── Managed method stubs (COM interface methods not directly reachable) ──
// SimpleMath::Add and SimpleMath::Multiply are referenced by the COM vtable
// array but the codegen doesn't emit native function bodies for them because
// no reachable managed method calls them directly (dispatch is through COM
// vtable via MarshalCallComMethod).  These stubs use the interpreter for
// correct execution.

CHAOS_IL2CPP_INT32 RcwBasicSubjects_SimpleMath_Add(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INT32 a,
    CHAOS_IL2CPP_INT32 b)
{
    // Fall back to interpreter dispatch for SimpleMath::Add (slot 0x00000016u)
    // The interpreter handles the actual managed method body.
    extern void InterpreterEntryDirect(
        CHAOS_IL2CPP_UINTPTR method_key,
        void* args_buf,
        void* ret_buf) noexcept;
    CHAOS_IL2CPP_UINTPTR key = static_cast<CHAOS_IL2CPP_UINTPTR>(0x00000016u);
    CHAOS_IL2CPP_INTPTR args[3] = { this_ptr, a, b };
    CHAOS_IL2CPP_INT32 result;
    InterpreterEntryDirect(key, args, &result);
    return result;
}

CHAOS_IL2CPP_INT32 RcwBasicSubjects_SimpleMath_Multiply(
    CHAOS_IL2CPP_INTPTR this_ptr,
    CHAOS_IL2CPP_INT32 a,
    CHAOS_IL2CPP_INT32 b)
{
    extern void InterpreterEntryDirect(
        CHAOS_IL2CPP_UINTPTR method_key,
        void* args_buf,
        void* ret_buf) noexcept;
    CHAOS_IL2CPP_UINTPTR key = static_cast<CHAOS_IL2CPP_UINTPTR>(0x00000017u);
    CHAOS_IL2CPP_INTPTR args[3] = { this_ptr, a, b };
    CHAOS_IL2CPP_INT32 result;
    InterpreterEntryDirect(key, args, &result);
    return result;
}

// ── External runtime function stubs for per-family verification builds ──
// These functions are referenced by kChaosExternalRuntimeFnTable entries
// that can't be resolved through the hotpatch name registry (they're
// System.Private.CoreLib internals not present in per-family AOT modules).
//
// Declared here and assigned to the table by PatchExternalRuntimeFnTable().

// GCHandle::Alloc(object) → GCHandle
// Returns the raw object pointer as a handle (no actual GC handle in per-family builds).
CHAOS_IL2CPP_INTPTR StubGCHandleAlloc(CHAOS_IL2CPP_INTPTR obj_ptr)
{
    return obj_ptr;
}

// GCHandle::ToIntPtr(GCHandle) → IntPtr
// Identity: GCHandle and IntPtr have the same representation.
CHAOS_IL2CPP_INTPTR StubGCHandleToIntPtr(CHAOS_IL2CPP_INTPTR handle)
{
    return handle;
}

// GCHandle::FromIntPtr(IntPtr) → GCHandle
// Identity (inverse of ToIntPtr).
CHAOS_IL2CPP_INTPTR StubGCHandleFromIntPtr(CHAOS_IL2CPP_INTPTR ptr)
{
    return ptr;
}

// GCHandle::Free(GCHandle)
// No-op: no actual GC handle was created by the Alloc stub above.
void StubGCHandleFree(CHAOS_IL2CPP_INTPTR)
{
}

// Marshal::AllocHGlobal(Int32) → IntPtr
CHAOS_IL2CPP_INTPTR StubMarshalAllocHGlobal(CHAOS_IL2CPP_INTPTR size)
{
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
        std::malloc(static_cast<size_t>(size)));
}

// Marshal::Copy(byte[], int, IntPtr, int)
// Copies bytes from a managed byte array to native memory.
// The managed byte array stores each element in a pointer-width slot.
void StubMarshalCopy(
    CHAOS_IL2CPP_INTPTR src_array,
    CHAOS_IL2CPP_INTPTR start_index,
    CHAOS_IL2CPP_INTPTR dest_ptr,
    CHAOS_IL2CPP_INTPTR length)
{
    auto* array = reinterpret_cast<StubManagedArray*>(
        reinterpret_cast<void*>(src_array));
    auto* dest = reinterpret_cast<uint8_t*>(dest_ptr);
    for (CHAOS_IL2CPP_INTPTR i = 0; i < length; i++) {
        dest[i] = static_cast<uint8_t>(
            array->elements[start_index + i] & 0xFF);
    }
}

// Marshal::FreeHGlobal(IntPtr)
void StubMarshalFreeHGlobal(CHAOS_IL2CPP_INTPTR ptr)
{
    std::free(reinterpret_cast<void*>(ptr));
}

// ── External runtime function table patcher ─────────────────────────
// Called from runtime-entry.cpp after bootstrap_runtime() to populate
// unresolved entries in the codegen-emitted external function table.
extern "C" void* kChaosExternalRuntimeFnTable[];

void PatchExternalRuntimeFnTable()
{
    kChaosExternalRuntimeFnTable[2]  = reinterpret_cast<void*>(&StubGCHandleAlloc);
    kChaosExternalRuntimeFnTable[3]  = reinterpret_cast<void*>(&StubGCHandleToIntPtr);
    kChaosExternalRuntimeFnTable[5]  = reinterpret_cast<void*>(&StubGCHandleFromIntPtr);
    kChaosExternalRuntimeFnTable[6]  = reinterpret_cast<void*>(&StubGCHandleFree);
    kChaosExternalRuntimeFnTable[10] = reinterpret_cast<void*>(&StubMarshalAllocHGlobal);
    kChaosExternalRuntimeFnTable[11] = reinterpret_cast<void*>(&StubMarshalCopy);
    kChaosExternalRuntimeFnTable[12] = reinterpret_cast<void*>(&StubMarshalFreeHGlobal);
}

// ── ComVtable dispatch for P/Invoke test helpers ────────────────
// These wrap runtime_core implementations with extern "C" linkage
// so managed [DllImport("__Internal")] declarations can reach them.

CHAOS_IL2CPP_INT32 MarshalCallComMethod(
    CHAOS_IL2CPP_INTPTR ptr,
    CHAOS_IL2CPP_INT32 slot,
    CHAOS_IL2CPP_INT32 a,
    CHAOS_IL2CPP_INT32 b)
{
    return chaos::il2cpp::runtime_core::MarshalCallComMethod(ptr, slot, a, b);
}

CHAOS_IL2CPP_INT32 MarshalCallDirectComMethod(
    CHAOS_IL2CPP_INTPTR com_ptr,
    CHAOS_IL2CPP_INT32 slot,
    CHAOS_IL2CPP_INT32 a,
    CHAOS_IL2CPP_INT32 b)
{
    return chaos::il2cpp::runtime_core::MarshalCallDirectComMethod(com_ptr, slot, a, b);
}

}  // extern "C"
