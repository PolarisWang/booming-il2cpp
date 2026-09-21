// delegate_stubs.cpp — Delegate combine/remove stubs for test builds.
// These provide C++ definitions for chaos_delegate_combine and
// chaos_delegate_remove, declared in misc_stubs.h. The implementations
// forward to runtime_core::DelegateCombine/DelegateRemove which handle
// the DelegateObject layout and invocation-list model.
//
// NOTE: These stubs are NOT suitable for production use — they link
// against the full runtime_core library and rely on its GC integration.

#include <chaos/native_types.h>
#include <core/delegate_helpers.h>

// These declarations appear in misc_stubs.h inside extern "C" (via stubs.h)
// at global scope. The definitions here must match with extern "C" to
// produce the correct C-linkage (unmangled) symbol.

CHAOS_IL2CPP_INTPTR chaos_delegate_combine(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) noexcept
{
    return chaos::il2cpp::runtime_core::DelegateCombine(left, right);
}

CHAOS_IL2CPP_INTPTR chaos_delegate_remove(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value) noexcept
{
    return chaos::il2cpp::runtime_core::DelegateRemove(source, value);
}

// ── Delegate constructor (Phase 6 / E1) ──
// System.Func<T>/System.Action<T>::.ctor(object, IntPtr) pattern.
// The runtime represents a delegate as a DelegateObject handle; the ctor
// binds exactly (target, method-ptr) — a store, not an interpretation.
// Without this, every delegate construction in generated code falls through
// to the interpreter at ~20us/call.
CHAOS_IL2CPP_INTPTR chaos_delegate_ctor(
    CHAOS_IL2CPP_INTPTR this_ref,
    CHAOS_IL2CPP_INTPTR target,
    CHAOS_IL2CPP_INTPTR method_ptr) noexcept
{
    // The DelegateObject representation:
    //   chaos_delegate_target = target (the object the closure captures)
    //   chaos_delegate_method_ptr = the native function pointer to call
    if (this_ref == 0) return 0;
    auto* del = reinterpret_cast<chaos::il2cpp::runtime_core::DelegateObject*>(this_ref);
    del->chaos_delegate_target = target;
    del->chaos_delegate_method_ptr = method_ptr;
    del->chaos_delegate_invocation_list = 0;
    del->chaos_delegate_invocation_count = 0;
    del->chaos_delegate_method_token = 0;
    // type_info must be set by the managed side before calling .ctor.
    return this_ref;
}
