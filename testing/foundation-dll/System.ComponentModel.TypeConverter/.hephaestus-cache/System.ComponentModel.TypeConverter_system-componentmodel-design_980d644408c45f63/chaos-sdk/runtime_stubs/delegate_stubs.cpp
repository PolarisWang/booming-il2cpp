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
