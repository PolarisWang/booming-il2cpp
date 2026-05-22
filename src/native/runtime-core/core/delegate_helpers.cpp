#include "delegate_helpers.h"

#include "gc/gc_old_gen.h"

namespace chaos::il2cpp::runtime_core {
namespace {

DelegateObject* RequireDelegate(CHAOS_IL2CPP_INTPTR value) {
    if (value == 0) {
        CHAOS_IL2CPP_FAIL();
    }
    return reinterpret_cast<DelegateObject*>(value);
}

const std::vector<CHAOS_IL2CPP_INTPTR>* TryGetInvocationList(
    const DelegateObject* d) noexcept {
    if (d == nullptr ||
        d->chaos_delegate_invocation_list == 0 ||
        d->chaos_delegate_invocation_count <= 0) {
        return nullptr;
    }
    return reinterpret_cast<const std::vector<CHAOS_IL2CPP_INTPTR>*>(
        d->chaos_delegate_invocation_list);
}

}  // anonymous namespace

bool DelegateSingleEntryEquals(CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) {
    if (left == right) return true;
    if (left == 0 || right == 0) return false;

    const auto* l = reinterpret_cast<const DelegateObject*>(left);
    const auto* r = reinterpret_cast<const DelegateObject*>(right);
    return l->type_info == r->type_info &&
           l->chaos_delegate_target == r->chaos_delegate_target &&
           l->chaos_delegate_method_ptr == r->chaos_delegate_method_ptr;
}

void DelegateAppendFlattenedEntries(
    std::vector<CHAOS_IL2CPP_INTPTR>& entries,
    CHAOS_IL2CPP_INTPTR delegate_value) {
    if (delegate_value == 0) return;

    auto* d = RequireDelegate(delegate_value);
    const auto* invocation_list = TryGetInvocationList(d);
    if (invocation_list == nullptr) {
        if (d->chaos_delegate_method_ptr == 0) {
            CHAOS_IL2CPP_FAIL();
        }
        entries.push_back(delegate_value);
        return;
    }

    if (static_cast<CHAOS_IL2CPP_INTPTR>(invocation_list->size()) !=
        d->chaos_delegate_invocation_count) {
        CHAOS_IL2CPP_FAIL();
    }

    for (auto entry_value : *invocation_list) {
        DelegateAppendFlattenedEntries(entries, entry_value);
    }
}

void DelegateValidateEntryTypes(const std::vector<CHAOS_IL2CPP_INTPTR>& entries) {
    if (entries.empty()) return;

    const auto* first = RequireDelegate(entries.front());
    for (auto entry_value : entries) {
        const auto* entry = RequireDelegate(entry_value);
        if (entry->type_info != first->type_info) {
            CHAOS_IL2CPP_FAIL();
        }
    }
}

CHAOS_IL2CPP_INTPTR DelegateCreateMulticast(
    CHAOS_IL2CPP_INTPTR template_delegate_value,
    const std::vector<CHAOS_IL2CPP_INTPTR>& entries) {
    if (entries.empty()) return 0;
    if (entries.size() == 1) return entries.front();

    const auto* template_delegate = RequireDelegate(template_delegate_value);

    // Allocate DelegateObject via old-gen (conservative scan covers all pointer fields).
    auto* delegate = static_cast<DelegateObject*>(
        g_old_gen.Allocate(sizeof(DelegateObject), true));
    delegate->type_info = template_delegate->type_info;
    delegate->sync_state = 0;
    delegate->chaos_delegate_target = 0;
    delegate->chaos_delegate_method_ptr = 0;

    // Allocate invocation list in GC heap, placement-new vector copy.
    using InvocationList = std::vector<CHAOS_IL2CPP_INTPTR>;
    auto* vec = static_cast<InvocationList*>(
        g_old_gen.Allocate(sizeof(InvocationList), true));
    vec = new (vec) InvocationList(entries);

    delegate->chaos_delegate_invocation_list = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(vec);
    delegate->chaos_delegate_invocation_count =
        static_cast<CHAOS_IL2CPP_INTPTR>(vec->size());
    delegate->chaos_delegate_method_token = 0;
    delegate->_pad = 0;

    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(delegate);
}

CHAOS_IL2CPP_INTPTR DelegateCombine(
    CHAOS_IL2CPP_INTPTR left, CHAOS_IL2CPP_INTPTR right) {
    if (left == 0) return right;
    if (right == 0) return left;

    std::vector<CHAOS_IL2CPP_INTPTR> entries;
    DelegateAppendFlattenedEntries(entries, left);
    DelegateAppendFlattenedEntries(entries, right);
    DelegateValidateEntryTypes(entries);
    return DelegateCreateMulticast(left, entries);
}

CHAOS_IL2CPP_INTPTR DelegateRemove(
    CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value_to_remove) {
    if (source == 0) return 0;
    if (value_to_remove == 0) return source;

    std::vector<CHAOS_IL2CPP_INTPTR> source_entries;
    std::vector<CHAOS_IL2CPP_INTPTR> remove_entries;
    DelegateAppendFlattenedEntries(source_entries, source);
    DelegateAppendFlattenedEntries(remove_entries, value_to_remove);

    if (remove_entries.empty() ||
        source_entries.size() < remove_entries.size()) {
        return source;
    }

    for (CHAOS_IL2CPP_INTPTR start = static_cast<CHAOS_IL2CPP_INTPTR>(
             source_entries.size() - remove_entries.size());
         start >= 0; --start) {
        bool matches = true;
        for (CHAOS_IL2CPP_SIZE i = 0; i < remove_entries.size(); ++i) {
            if (!DelegateSingleEntryEquals(
                    source_entries[static_cast<CHAOS_IL2CPP_SIZE>(start) + i],
                    remove_entries[i])) {
                matches = false;
                break;
            }
        }
        if (!matches) continue;

        // Erase the matching range.
        source_entries.erase(
            source_entries.begin() + static_cast<CHAOS_IL2CPP_SIZE>(start),
            source_entries.begin() + static_cast<CHAOS_IL2CPP_SIZE>(start) +
                static_cast<CHAOS_IL2CPP_SIZE>(remove_entries.size()));

        if (source_entries.empty()) return 0;
        if (source_entries.size() == 1) return source_entries.front();
        return DelegateCreateMulticast(source, source_entries);
    }

    return source;
}

// ── Invoke a single delegate entry's thunk ────────────────────────────
// Dispatches to the codegen-generated thunk with the correct calling
// convention based on arg_count. Handles closed (target != 0) vs open
// (target == 0) delegates.
namespace {
static void InvokeSingleEntry(void* method_ptr, CHAOS_IL2CPP_INTPTR target,
                               CHAOS_IL2CPP_INTPTR* args_buf,
                               CHAOS_IL2CPP_INTPTR* ret_buf,
                               CHAOS_IL2CPP_UINT32 arg_count) noexcept {
    switch (arg_count) {
        case 0:
            fprintf(stderr, "[TRACE] InvokeSingleEntry(0): calling thunk 0x%llx target=0x%llx\n",
                    (unsigned long long)method_ptr, (unsigned long long)target);
            if (target) {
                auto fn = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(method_ptr);
                auto result = fn(target);
                fprintf(stderr, "[TRACE] InvokeSingleEntry(0): closed returned\n");
                if (ret_buf) *ret_buf = result;
            } else {
                auto fn = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(method_ptr);
                auto result = fn();
                if (ret_buf) *ret_buf = result;
            }
            break;
        case 1:
            if (target) {
                auto fn = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(method_ptr);
                auto result = fn(target, args_buf[0]);
                if (ret_buf) *ret_buf = result;
            } else {
                auto fn = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(method_ptr);
                auto result = fn(args_buf[0]);
                if (ret_buf) *ret_buf = result;
            }
            break;
        case 2:
            if (target) {
                auto fn = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(method_ptr);
                auto result = fn(target, args_buf[0], args_buf[1]);
                if (ret_buf) *ret_buf = result;
            } else {
                auto fn = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(method_ptr);
                auto result = fn(args_buf[0], args_buf[1]);
                if (ret_buf) *ret_buf = result;
            }
            break;
        case 3:
            if (target) {
                auto fn = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(method_ptr);
                auto result = fn(target, args_buf[0], args_buf[1], args_buf[2]);
                if (ret_buf) *ret_buf = result;
            } else {
                auto fn = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(method_ptr);
                auto result = fn(args_buf[0], args_buf[1], args_buf[2]);
                if (ret_buf) *ret_buf = result;
            }
            break;
        case 4:
            if (target) {
                auto fn = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(method_ptr);
                auto result = fn(target, args_buf[0], args_buf[1], args_buf[2], args_buf[3]);
                if (ret_buf) *ret_buf = result;
            } else {
                auto fn = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(method_ptr);
                auto result = fn(args_buf[0], args_buf[1], args_buf[2], args_buf[3]);
                if (ret_buf) *ret_buf = result;
            }
            break;
        default:
            CHAOS_IL2CPP_FAIL(); // unsupported arg count
            break;
    }
}
}  // anonymous namespace

extern "C" void chaos_delegate_object_invoke(
    CHAOS_IL2CPP_INTPTR delegate_ptr,
    CHAOS_IL2CPP_INTPTR* args_buf,
    CHAOS_IL2CPP_INTPTR* ret_buf,
    CHAOS_IL2CPP_UINT32 arg_count) noexcept
{
    fprintf(stderr, "[TRACE] chaos_delegate_object_invoke: delegate_ptr=0x%llx arg_count=%u\n",
            (unsigned long long)delegate_ptr, (unsigned)arg_count);
    if (delegate_ptr == 0 || arg_count > 4) {
        fprintf(stderr, "[TRACE] chaos_delegate_object_invoke: EARLY RETURN (ptr=0 count=%u)\n",
                (unsigned)arg_count);
        return;
    }

    auto* del = reinterpret_cast<DelegateObject*>(delegate_ptr);

    // ── Multicast path ────────────────────────────────────────────────
    if (del->chaos_delegate_invocation_count > 0) {
        fprintf(stderr, "[TRACE] chaos_delegate_object_invoke: multicast count=%lld\n",
                (long long)del->chaos_delegate_invocation_count);
        auto* list = reinterpret_cast<const std::vector<CHAOS_IL2CPP_INTPTR>*>(
            del->chaos_delegate_invocation_list);
        if (list != nullptr) {
            for (auto entry_value : *list) {
                chaos_delegate_object_invoke(entry_value, args_buf, ret_buf, arg_count);
            }
        }
        return;
    }

    // ── Single delegate path ──────────────────────────────────────────
    auto* method_ptr = reinterpret_cast<void*>(del->chaos_delegate_method_ptr);
    fprintf(stderr, "[TRACE] chaos_delegate_object_invoke: single, target=0x%llx method_ptr=0x%llx token=%u\n",
            (unsigned long long)del->chaos_delegate_target,
            (unsigned long long)del->chaos_delegate_method_ptr,
            (unsigned)del->chaos_delegate_method_token);
    if (method_ptr == nullptr) {
        fprintf(stderr, "[TRACE] chaos_delegate_object_invoke: method_ptr is NULL!\n");
        return;
    }

    // Hotpatch checkpoint: if the method has been hotpatched, route through
    // InterpreterEntryDirect.
    if (del->chaos_delegate_method_token != 0) {
        if (DelegateHotpatchCheckpoint(
                del->chaos_delegate_method_token,
                reinterpret_cast<uint64_t*>(args_buf),
                reinterpret_cast<uint64_t*>(ret_buf),
                arg_count)) {
            return;
        }
    }

    // Direct thunk call: thunk(target, args...) for closed delegates,
    // thunk(args...) for open/static delegates.
    InvokeSingleEntry(method_ptr, del->chaos_delegate_target,
                       args_buf, ret_buf, arg_count);
}

}  // namespace chaos::il2cpp::runtime_core
