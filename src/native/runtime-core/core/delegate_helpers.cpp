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

}  // namespace chaos::il2cpp::runtime_core
