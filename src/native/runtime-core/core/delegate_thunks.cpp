#include <chaos/native_types.h>
#include <runtime_abi.h>
#include "../gc/gc_old_gen.h"
#include <hotpatch_table.h>
#include <interpreter_entry.h>
#include <cstring>
#include <mutex>
#include <shared_mutex>

#include "gc_heap.h"
namespace chaos::il2cpp::runtime_core {
namespace {

struct DelegateThunkEntry {
    const char* type_id;
    void* thunk_fn;
    CHAOS_IL2CPP_INTPTR* target_slot;
    uint8_t param_count;        // 0-4: arity of delegate invoke
};

// P0.3: Dynamic vector — no fixed cap.
static std::vector<DelegateThunkEntry> g_delegate_thunks;
static std::shared_mutex g_delegate_thunks_mutex;

}  // anonymous namespace

// ── Native function dispatch thunks for GetDelegateForFunctionPointer ──
//
// When managed code calls a delegate created via GetDelegateForFunctionPointer,
// the DelegateInvoke bridge routes through DfnDispatch<has_target=true> with
// target_instance = native_fn_ptr. These thunks receive the managed calling
// convention (RuntimeState*, ThreadState*, target, argv...) and forward to
// the native function with pointer-sized arguments.
//
// Limitation V1: All arguments are treated as CHAOS_IL2CPP_INTPTR (pointer-sized).

extern "C" void* NativeDfnThunkArity0(RuntimeState* rs, ThreadState* ts, void* native_fn_ptr) {
    (void)rs; (void)ts;
    using Fn = CHAOS_IL2CPP_INTPTR(CHAOS_RUNTIME_ABI_CALL*)();
    return reinterpret_cast<void*>(reinterpret_cast<Fn>(native_fn_ptr)());
}

extern "C" void* NativeDfnThunkArity1(RuntimeState* rs, ThreadState* ts, void* native_fn_ptr, void* a0) {
    (void)rs; (void)ts;
    using Fn = CHAOS_IL2CPP_INTPTR(CHAOS_RUNTIME_ABI_CALL*)(CHAOS_IL2CPP_INTPTR);
    return reinterpret_cast<void*>(reinterpret_cast<Fn>(native_fn_ptr)(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(a0)));
}

extern "C" void* NativeDfnThunkArity2(RuntimeState* rs, ThreadState* ts, void* native_fn_ptr, void* a0, void* a1) {
    (void)rs; (void)ts;
    using Fn = CHAOS_IL2CPP_INTPTR(CHAOS_RUNTIME_ABI_CALL*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR);
    return reinterpret_cast<void*>(reinterpret_cast<Fn>(native_fn_ptr)(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(a0),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(a1)));
}

extern "C" void* NativeDfnThunkArity3(RuntimeState* rs, ThreadState* ts, void* native_fn_ptr, void* a0, void* a1, void* a2) {
    (void)rs; (void)ts;
    using Fn = CHAOS_IL2CPP_INTPTR(CHAOS_RUNTIME_ABI_CALL*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR);
    return reinterpret_cast<void*>(reinterpret_cast<Fn>(native_fn_ptr)(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(a0),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(a1),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(a2)));
}

extern "C" void* NativeDfnThunkArity4(RuntimeState* rs, ThreadState* ts, void* native_fn_ptr, void* a0, void* a1, void* a2, void* a3) {
    (void)rs; (void)ts;
    using Fn = CHAOS_IL2CPP_INTPTR(CHAOS_RUNTIME_ABI_CALL*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR);
    return reinterpret_cast<void*>(reinterpret_cast<Fn>(native_fn_ptr)(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(a0),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(a1),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(a2),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(a3)));
}

static void* const kNativeDfnThunks[5] = {
    reinterpret_cast<void*>(&NativeDfnThunkArity0),
    reinterpret_cast<void*>(&NativeDfnThunkArity1),
    reinterpret_cast<void*>(&NativeDfnThunkArity2),
    reinterpret_cast<void*>(&NativeDfnThunkArity3),
    reinterpret_cast<void*>(&NativeDfnThunkArity4),
};

namespace {  // reopen anonymous for NativeFunctionDelegate

// Layout-compatible with bootstrap::DelegateInstance.
struct NativeFunctionDelegate {
    CHAOS_IL2CPP_UINT32 method_token;   // 0 = native function sentinel
    void* method_pointer;               // dispatch thunk
    void* target_instance;              // native function pointer
    NativeFunctionDelegate* next;
};

}  // anonymous namespace (NativeFunctionDelegate)

// ── Exported functions (declared in runtime_core.h) ──

void RegisterDelegateThunk(const char* type_id, void* thunk_fn,
                           CHAOS_IL2CPP_INTPTR* target_slot,
                           uint8_t param_count) {
    if (type_id == nullptr || thunk_fn == nullptr || target_slot == nullptr) return;

    std::unique_lock lock(g_delegate_thunks_mutex);
    for (size_t i = 0; i < g_delegate_thunks.size(); i++) {
        if (std::strcmp(g_delegate_thunks[i].type_id, type_id) == 0) {
            g_delegate_thunks[i].thunk_fn = thunk_fn;
            g_delegate_thunks[i].target_slot = target_slot;
            g_delegate_thunks[i].param_count = param_count;
            return;
        }
    }

    g_delegate_thunks.push_back({type_id, thunk_fn, target_slot, param_count});
}

void* FindDelegateThunk(const char* type_id) {
    if (type_id == nullptr) return nullptr;
    std::shared_lock lock(g_delegate_thunks_mutex);
    for (size_t i = 0; i < g_delegate_thunks.size(); i++) {
        if (std::strcmp(g_delegate_thunks[i].type_id, type_id) == 0)
            return g_delegate_thunks[i].thunk_fn;
    }
    return nullptr;
}

void* MarshalGetFunctionPointerForDelegateImpl(
    RuntimeState* runtime_state, ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR delegate_obj, const char* delegate_type_id) {
    (void)thread_state;
    if (runtime_state == nullptr || delegate_obj == static_cast<CHAOS_IL2CPP_INTPTR>(0) || delegate_type_id == nullptr)
        return nullptr;

    std::shared_lock lock(g_delegate_thunks_mutex);
    for (size_t i = 0; i < g_delegate_thunks.size(); i++) {
        if (std::strcmp(g_delegate_thunks[i].type_id, delegate_type_id) == 0) {
            if (g_delegate_thunks[i].target_slot != nullptr)
                *g_delegate_thunks[i].target_slot = delegate_obj;
            return g_delegate_thunks[i].thunk_fn;
        }
    }
    return nullptr;
}

void* MarshalGetDelegateForFunctionPointerImpl(
    RuntimeState* runtime_state, ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR native_fn_ptr, const char* delegate_type_id) {
    (void)thread_state;
    if (runtime_state == nullptr || native_fn_ptr == 0 || delegate_type_id == nullptr)
        return nullptr;

    // Look up the delegate thunk by type_id.
    size_t thunk_idx = ~size_t(0);
    uint8_t param_count = 0;
    {
        std::shared_lock lock(g_delegate_thunks_mutex);
        for (size_t i = 0; i < g_delegate_thunks.size(); i++) {
            if (std::strcmp(g_delegate_thunks[i].type_id, delegate_type_id) == 0) {
                thunk_idx = i;
                param_count = g_delegate_thunks[i].param_count;
                break;
            }
        }
    }
    if (thunk_idx == ~size_t(0))
        return nullptr;  // Unknown delegate type

    if (param_count > 4) param_count = 4;  // Cap at arity 4 (DelegateInvoke limit)

    void* dispatch_thunk = kNativeDfnThunks[param_count];

    // Allocate via old-gen GC so the GC scans the node for managed object refs.
    // target_instance stores the native_fn_ptr; DfnDispatch<true> passes it as
    // the third argument to the dispatch thunk.
    auto* node = static_cast<NativeFunctionDelegate*>(
        G_OldGen().Allocate(sizeof(NativeFunctionDelegate), true));
    node->method_token = 0;
    node->method_pointer = dispatch_thunk;
    node->target_instance = reinterpret_cast<void*>(native_fn_ptr);
    node->next = nullptr;

    return node;
}

// ── DelegateHotpatchCheckpoint ─────────────────────────────────────────
// Hotpatch checkpoint for delegate invocations.
// Checks if the method identified by method_token has been hotpatched.
// If active, routes through InterpreterEntryDirect and returns true.
// If no hotpatch, returns false (caller should call native method_ptr).

bool DelegateHotpatchCheckpoint(
    CHAOS_IL2CPP_UINT32 method_token,
    uint64_t* args_buf,
    uint64_t* ret_buf,
    uint32_t arg_count)
{
    if (method_token == 0u)
        return false;  // No token → can't check, call native

    if (args_buf == nullptr || ret_buf == nullptr)
        return false;

    // Find which module owns this token (binary search across all modules).
    auto& registry = GetHotpatchNameRegistry();
    uint64_t composite = registry.FindToken(method_token);
    if (composite == 0ull)
        return false;  // Token not found in any module → not a hotpatchable method

    uint32_t module_id = ExtractModuleId(composite);
    uint32_t token = ExtractToken(composite);

    // Get the dispatch entry for this (module_id, token).
    auto* entry = registry.GetDispatchEntry(module_id, token);
    if (entry == nullptr)
        return false;  // No dispatch entry → not managed by hotpatch system

    // Check if hotpatch is active and we should route to interpreter.
    if (!HotpatchIsActive(*entry) || HotpatchShouldKeepNative(*entry))
        return false;  // No active hotpatch → call native

    // Route through interpreter. The interpreter reads arg_count from the
    // method's IR signature and parses args_buf according to parameter types.
    InterpreterEntryDirect(entry->method_key,
                           reinterpret_cast<void*>(args_buf),
                           reinterpret_cast<void*>(ret_buf));
    return true;
}

}  // namespace chaos::il2cpp::runtime_core
