/// bootstrap_delegates.cpp — Delegate system (CreateDelegate, CombineDelegate,
/// RemoveDelegate, DelegateInvoke).
///
/// Extracted from bootstrap.cpp during industrialization (Phase D).
/// Uses CRAG old-gen (GC-scanned) allocation for DelegateInstance nodes.

#include "bootstrap_internal.h"

#include "gc/gc_old_gen.h"
#include "runtime_core.h"

#include <cstdint>

namespace chaos::il2cpp::bootstrap {

// Forward-declare public bootstrap API used by CreateDelegate.
// (Defined in bootstrap.cpp, declared in bootstrap.h)
void* FindMethodPointerByToken(CHAOS_IL2CPP_UINT32 method_token);

// ── AllocateDelegateNode ────────────────────────────────────────────────

static DelegateInstance* AllocateDelegateNode(
    CHAOS_IL2CPP_UINT32 method_token,
    void* method_pointer,
    void* target_instance,
    DelegateInstance* next) {
    // Use CRAG old-gen (GC-scanned) allocation since DelegateInstance
    // nodes contain managed object pointers (target_instance, next) that
    // must be traced by the GC for reachability.
    auto* node = static_cast<DelegateInstance*>(
        chaos::il2cpp::runtime_core::g_old_gen.Allocate(sizeof(DelegateInstance), true));
    if (node == nullptr) return nullptr;

    node->method_token = method_token;
    node->method_pointer = method_pointer;
    node->target_instance = target_instance;
    node->next = next;
    return node;
}

// ── ClonePrefixTo ───────────────────────────────────────────────────────

static DelegateInstance* ClonePrefixTo(const DelegateInstance* head, const DelegateInstance* exclusive_end) {
    if (head == nullptr || head == exclusive_end) {
        return nullptr;
    }

    auto* clone_head = AllocateDelegateNode(head->method_token, head->method_pointer, head->target_instance, nullptr);
    if (clone_head == nullptr) return nullptr;

    DelegateInstance* tail = clone_head;
    const DelegateInstance* cur = head->next;
    while (cur != nullptr && cur != exclusive_end) {
        auto* clone = AllocateDelegateNode(cur->method_token, cur->method_pointer, cur->target_instance, nullptr);
        if (clone == nullptr) break;
        tail->next = clone;
        tail = clone;
        cur = cur->next;
    }
    return clone_head;
}

// ── CreateDelegate ──────────────────────────────────────────────────────

void* CHAOS_RUNTIME_ABI_CALL CreateDelegate(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    MethodInfoHandle method,
    void* target_instance) {
    (void)runtime_state;
    (void)thread_state;

    if (method == 0) {
        return nullptr;
    }

    const CHAOS_IL2CPP_UINT32 method_token = DecodeOpaqueToken(method);
    void* const method_pointer = FindMethodPointerByToken(method_token);
    if (method_pointer == nullptr) {
        return nullptr;
    }

    return AllocateDelegateNode(method_token, method_pointer, target_instance, nullptr);
}

// ── DelegateNodesEqual ───────────────────────────────────────────────────

/// Returns true if two delegate nodes refer to the same (method, target) pair.
static bool DelegateNodesEqual(const DelegateInstance* a, const DelegateInstance* b) {
    return a != nullptr && b != nullptr
        && a->method_token  == b->method_token
        && a->target_instance == b->target_instance;
}

// ── FindLastMatch ────────────────────────────────────────────────────────

/// Finds the last node matching `tgt` in the chain starting at `head`.
/// Returns { prev, match } pointers (prev = nullptr if match is head).
static void FindLastMatch(
    const DelegateInstance* head,
    const DelegateInstance* tgt,
    const DelegateInstance** out_prev,
    const DelegateInstance** out_match) {
    *out_match = nullptr;
    *out_prev = nullptr;
    const DelegateInstance* prev = nullptr;
    for (auto* cur = head; cur != nullptr; prev = cur, cur = cur->next) {
        if (DelegateNodesEqual(cur, tgt)) {
            *out_match = cur;
            *out_prev = prev;
        }
    }
}

// ── CombineDelegate ─────────────────────────────────────────────────────

void* CHAOS_RUNTIME_ABI_CALL CombineDelegate(
    void* left_delegate,
    void* right_delegate) {
    if (left_delegate == nullptr)  return right_delegate;
    if (right_delegate == nullptr) return left_delegate;

    // Build: clone(left) -> share(right as suffix)
    // The right chain is shared by reference (immutable nodes), so this is
    // O(|left|) — we only copy the left prefix, not the right chain.
    auto* left = static_cast<DelegateInstance*>(left_delegate);
    auto* right = static_cast<DelegateInstance*>(right_delegate);

    auto* clone_head = ClonePrefixTo(left, nullptr);
    if (clone_head == nullptr) {
        return right;  // left was empty
    }

    // Attach shared right suffix to the tail of the cloned left.
    DelegateInstance* tail = clone_head;
    while (tail->next != nullptr) {
        tail = tail->next;
    }
    tail->next = right;
    return clone_head;
}

// ── RemoveDelegate ───────────────────────────────────────────────────────

void* CHAOS_RUNTIME_ABI_CALL RemoveDelegate(
    void* source_delegate,
    void* target_delegate) {
    if (source_delegate == nullptr || target_delegate == nullptr) return source_delegate;

    auto* src = static_cast<DelegateInstance*>(source_delegate);
    auto* tgt = static_cast<DelegateInstance*>(target_delegate);

    // 1. Find the last matching node in the original chain (without cloning).
    const DelegateInstance* match = nullptr;
    const DelegateInstance* match_prev = nullptr;
    FindLastMatch(src, tgt, &match_prev, &match);

    if (match == nullptr) {
        return source_delegate;
    }

    // 2. Clone the prefix up to (but not including) the match node.
    if (match_prev == nullptr) {
        // match is the head — no prefix to clone, skip directly to match->next
        return match->next;
    }

    auto* clone_head = ClonePrefixTo(src, match);
    // Attach shared suffix (match->next) to the tail of the cloned prefix.
    DelegateInstance* tail = clone_head;
    while (tail->next != nullptr) {
        tail = tail->next;
    }
    tail->next = match->next;
    return clone_head;
}

// ── DfnDispatch (template helper) ──────────────────────────────────────

/// Template helper: dispatch delegate call with `has_target` (closed/static)
/// and `argc` (0-4) to the correct function pointer signature.
template <bool has_target>
static void DfnDispatch(
    void** return_slot,
    void* method_pointer,
    CHAOS_IL2CPP_UINT32 argc,
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* target_instance,
    void* const* argv) {
    if constexpr (has_target) {
        using Dfn0 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*);
        using Dfn1 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*);
        using Dfn2 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*);
        using Dfn3 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*, void*);
        using Dfn4 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*, void*, void*);
        switch (argc) {
            case 0u: *return_slot = reinterpret_cast<Dfn0>(method_pointer)(runtime_state, thread_state, target_instance); break;
            case 1u: *return_slot = reinterpret_cast<Dfn1>(method_pointer)(runtime_state, thread_state, target_instance, argv[0]); break;
            case 2u: *return_slot = reinterpret_cast<Dfn2>(method_pointer)(runtime_state, thread_state, target_instance, argv[0], argv[1]); break;
            case 3u: *return_slot = reinterpret_cast<Dfn3>(method_pointer)(runtime_state, thread_state, target_instance, argv[0], argv[1], argv[2]); break;
            case 4u: *return_slot = reinterpret_cast<Dfn4>(method_pointer)(runtime_state, thread_state, target_instance, argv[0], argv[1], argv[2], argv[3]); break;
        }
    } else {
        using Dfn0 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*);
        using Dfn1 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*);
        using Dfn2 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*);
        using Dfn3 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*);
        using Dfn4 = void* (CHAOS_RUNTIME_ABI_CALL*)(RuntimeState*, ThreadState*, void*, void*, void*, void*);
        switch (argc) {
            case 0u: *return_slot = reinterpret_cast<Dfn0>(method_pointer)(runtime_state, thread_state); break;
            case 1u: *return_slot = reinterpret_cast<Dfn1>(method_pointer)(runtime_state, thread_state, argv[0]); break;
            case 2u: *return_slot = reinterpret_cast<Dfn2>(method_pointer)(runtime_state, thread_state, argv[0], argv[1]); break;
            case 3u: *return_slot = reinterpret_cast<Dfn3>(method_pointer)(runtime_state, thread_state, argv[0], argv[1], argv[2]); break;
            case 4u: *return_slot = reinterpret_cast<Dfn4>(method_pointer)(runtime_state, thread_state, argv[0], argv[1], argv[2], argv[3]); break;
        }
    }
}

// ── DelegateInvoke ─────────────────────────────────────────────────────

BridgeStatus CHAOS_RUNTIME_ABI_CALL DelegateInvoke(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* delegate_instance,
    void* const* argv,
    CHAOS_IL2CPP_UINT32 argc,
    void* out_return_value,
    CHAOS_IL2CPP_SIZE out_return_value_size,
    ExceptionHandle* out_exception) {
    if (runtime_state == nullptr
        || thread_state == nullptr
        || delegate_instance == nullptr
        || argv == nullptr
        || argc > 4u
        || out_return_value == nullptr
        || out_return_value_size != sizeof(void*)) {
        return CHAOS_BRIDGE_STATUS_NOT_SUPPORTED;
    }

    if (out_exception != nullptr) {
        *out_exception = nullptr;
    }

    auto* delegate_handle = static_cast<DelegateInstance*>(delegate_instance);
    auto* const return_slot = static_cast<void**>(out_return_value);
    *return_slot = nullptr;

    // Walk the multicast chain; last return value wins (C# semantics).
    try {
        while (delegate_handle != nullptr) {
            if (delegate_handle->method_pointer == nullptr) {
                return CHAOS_BRIDGE_STATUS_NOT_FOUND;
            }

            if (delegate_handle->target_instance != nullptr) {
                DfnDispatch<true>(return_slot, delegate_handle->method_pointer, argc,
                    runtime_state, thread_state, delegate_handle->target_instance, argv);
            } else {
                DfnDispatch<false>(return_slot, delegate_handle->method_pointer, argc,
                    runtime_state, thread_state, nullptr, argv);
            }

            delegate_handle = delegate_handle->next;
        }
        return CHAOS_BRIDGE_STATUS_OK;
    } catch (const chaos_managed_exception& carrier) {
        if (out_exception != nullptr) {
            *out_exception = reinterpret_cast<ExceptionHandle>(carrier.object_value);
        }
        return CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION;
    }
}

}  // namespace chaos::il2cpp::bootstrap
