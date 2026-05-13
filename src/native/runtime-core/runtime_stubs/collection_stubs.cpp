// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// ── Collection stub implementations ──────────────────────────
#include "collection_stubs.h"

#include <algorithm>
#include <cstring>
#include <chaos/collection.h>
#include "gc_helpers.h"
#include "runtime_stubs/stub_common.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ═══════════════════════════════════════════════════════════════
// List<T> helpers
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INT32 CollectionListGetCount(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    return static_cast<CHAOS_IL2CPP_INT32>(storage->items.size());
}

CHAOS_IL2CPP_INTPTR CollectionListGetItem(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 index) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    auto idx = static_cast<CHAOS_IL2CPP_SIZE>(index);
    if (index < 0 || idx >= storage->items.size()) return 0;
    return storage->items[idx];
}

void CollectionListSetItem(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (handle == 0) return;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    auto idx = static_cast<CHAOS_IL2CPP_SIZE>(index);
    if (index >= 0 && idx < storage->items.size())
        storage->items[idx] = value;
}

void CollectionListAdd(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (handle == 0) return;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    storage->items.push_back(value);
}

void CollectionListInsert(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (handle == 0) return;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    auto idx = static_cast<CHAOS_IL2CPP_SIZE>(index);
    if (index >= 0 && idx <= storage->items.size())
        storage->items.insert(storage->items.begin() + idx, value);
}

void CollectionListRemoveAt(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 index) noexcept
{
    if (handle == 0) return;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    auto idx = static_cast<CHAOS_IL2CPP_SIZE>(index);
    if (index >= 0 && idx < storage->items.size())
        storage->items.erase(storage->items.begin() + idx);
}

CHAOS_IL2CPP_INT32 CollectionListIndexOf(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (handle == 0) return -1;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    for (CHAOS_IL2CPP_SIZE i = 0; i < storage->items.size(); ++i) {
        if (storage->items[i] == value)
            return static_cast<CHAOS_IL2CPP_INT32>(i);
    }
    return -1;
}

CHAOS_IL2CPP_INT32 CollectionListContains(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept
{
    return CollectionListIndexOf(handle, value) >= 0 ? 1 : 0;
}

CHAOS_IL2CPP_INT32 CollectionListRemove(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    auto& items = storage->items;
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (*it == value) {
            items.erase(it);
            return 1;
        }
    }
    return 0;
}

CHAOS_IL2CPP_INTPTR CollectionListToArray(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    auto count = storage->items.size();

    auto* arr = static_cast<StubArrayHeader*>(
        GcAllocateAtomic(sizeof(StubArrayHeader) + count * sizeof(CHAOS_IL2CPP_INTPTR)));
    if (arr == nullptr) return 0;
    arr->element_type = 0;
    arr->length = static_cast<CHAOS_IL2CPP_UINTPTR>(count);
    if (count > 0) {
        std::memcpy(arr + 1, storage->items.data(), count * sizeof(CHAOS_IL2CPP_INTPTR));
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

void CollectionListClear(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    storage->items.clear();
}

void CollectionListSort(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    std::sort(storage->items.begin(), storage->items.end());
}

// ═══════════════════════════════════════════════════════════════
// Dictionary<K,V> helpers
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INT32 CollectionDictionaryGetCount(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_dictionary_runtime_storage<CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR>(handle);
    return static_cast<CHAOS_IL2CPP_INT32>(storage->entries.size());
}

CHAOS_IL2CPP_INTPTR CollectionDictionaryGetItem(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_dictionary_runtime_storage<CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR>(handle);
    for (auto& entry : storage->entries) {
        if (entry.first == key) return entry.second;
    }
    return 0;
}

void CollectionDictionarySetItem(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (handle == 0) return;
    auto* storage = chaos::il2cpp::common::require_dictionary_runtime_storage<CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR>(handle);
    for (auto& entry : storage->entries) {
        if (entry.first == key) { entry.second = value; return; }
    }
    storage->entries.push_back({key, value});
}

CHAOS_IL2CPP_INT32 CollectionDictionaryTryGetValue(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR* out_value) noexcept
{
    if (handle == 0 || out_value == nullptr) return 0;
    auto* storage = chaos::il2cpp::common::require_dictionary_runtime_storage<CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR>(handle);
    for (auto& entry : storage->entries) {
        if (entry.first == key) { *out_value = entry.second; return 1; }
    }
    return 0;
}

CHAOS_IL2CPP_INT32 CollectionDictionaryContainsKey(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_dictionary_runtime_storage<CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR>(handle);
    for (auto& entry : storage->entries) {
        if (entry.first == key) return 1;
    }
    return 0;
}

CHAOS_IL2CPP_INT32 CollectionDictionaryRemove(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_dictionary_runtime_storage<CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR>(handle);
    auto& entries = storage->entries;
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (it->first == key) {
            entries.erase(it);
            return 1;
        }
    }
    return 0;
}

void CollectionDictionaryAdd(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (handle == 0) return;
    auto* storage = chaos::il2cpp::common::require_dictionary_runtime_storage<CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR>(handle);
    storage->entries.push_back({key, value});
}

// ═══════════════════════════════════════════════════════════════
// HashSet<T> helpers
// ═══════════════════════════════════════════════════════════════

CHAOS_IL2CPP_INT32 CollectionHashSetGetCount(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    return static_cast<CHAOS_IL2CPP_INT32>(storage->items.size());
}

CHAOS_IL2CPP_INT32 CollectionHashSetAdd(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    for (auto& item : storage->items) {
        if (item == value) return 0;
    }
    storage->items.push_back(value);
    return 1;
}

CHAOS_IL2CPP_INT32 CollectionHashSetContains(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    for (auto& item : storage->items) {
        if (item == value) return 1;
    }
    return 0;
}

CHAOS_IL2CPP_INT32 CollectionHashSetRemove(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept
{
    if (handle == 0) return 0;
    auto* storage = chaos::il2cpp::common::require_list_runtime_storage<CHAOS_IL2CPP_INTPTR>(handle);
    auto& items = storage->items;
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (*it == value) {
            items.erase(it);
            return 1;
        }
    }
    return 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core