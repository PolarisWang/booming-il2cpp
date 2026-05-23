/// bootstrap_internal.h — Internal declarations shared across bootstrap split units.
///
/// These declarations bridge the bootstrap core, delegate subsystem, and icall
/// resolver, all of which are referenced from the process-wide kCodegenBridgeV0.
///
/// NOTE: This header is NOT part of the public API. It is internal to the
/// chaos_bootstrap library.

#ifndef CHAOS_IL2CPP_BOOTSTRAP_INTERNAL_H_
#define CHAOS_IL2CPP_BOOTSTRAP_INTERNAL_H_

#include "bootstrap.h"
#include "codegen_bridge.h"

namespace chaos::il2cpp::bootstrap {

// ── Shared utilities ──
// Decodes an opaque metadata handle (MethodInfoHandle / TypeInfoHandle / FieldInfoHandle)
// to its raw 32-bit metadata token.  Handles are either opaque token values or
// reflection-query handles; this function extracts the token portion.

CHAOS_IL2CPP_UINT32 DecodeOpaqueToken(CHAOS_IL2CPP_UINTPTR handle);

// ── DelegateInstance (shared between bootstrap.cpp and bootstrap_delegates.cpp) ──
// GC-allocated immutable linked-list node for delegate multicast chains.
// Must match the layout expected by the runtime's GC scanner.
struct DelegateInstance {
    CHAOS_IL2CPP_UINT32 method_token;
    void* method_pointer;
    void* target_instance;
    DelegateInstance* next;  // multicast chain (nullptr = last in chain)
};

// ── Delegate system entry points (implemented in bootstrap_delegates.cpp) ──
// These are CHAOS_RUNTIME_ABI_CALL functions registered in the CodegenBridgeV0
// or reachable via icall resolution.

void* CHAOS_RUNTIME_ABI_CALL CreateDelegate(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    MethodInfoHandle method,
    void* target_instance);

void* CHAOS_RUNTIME_ABI_CALL CombineDelegate(
    void* left_delegate,
    void* right_delegate);

void* CHAOS_RUNTIME_ABI_CALL RemoveDelegate(
    void* source_delegate,
    void* target_delegate);

BridgeStatus CHAOS_RUNTIME_ABI_CALL DelegateInvoke(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* delegate_instance,
    void* const* argv,
    CHAOS_IL2CPP_UINT32 argc,
    void* out_return_value,
    CHAOS_IL2CPP_SIZE out_return_value_size,
    ExceptionHandle* out_exception);

// ── Icall resolution (implemented in bootstrap_icall.cpp) ──

void* CHAOS_RUNTIME_ABI_CALL ResolveIcall(const char* icall_name_utf8);

// ── Bootstrap state query (defined in bootstrap.cpp) ──
// Used by bootstrap_icall.cpp to check is_bootstrapped gate.

const BootstrapState* PeekBootstrapState();

}  // namespace chaos::il2cpp::bootstrap

#endif  // CHAOS_IL2CPP_BOOTSTRAP_INTERNAL_H_
