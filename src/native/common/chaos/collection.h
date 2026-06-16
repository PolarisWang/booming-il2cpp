#ifndef CHAOS_IL2CPP_COMMON_COLLECTION_H_
#define CHAOS_IL2CPP_COMMON_COLLECTION_H_

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

#include <chaos/native_types.h>
#include <chaos/unordered_dense.h>

namespace chaos::il2cpp::common {

// Offset within managed GC objects where collection stubs store their
// native runtime storage pointer (right after ThinLockableHeader).
// Must match the codegen field emission in ObjectModelEmission.cs.
inline constexpr CHAOS_IL2CPP_SIZE kNativeStorageSlotOffset = 16;

// ── List<T> ─────────────────────────────────────────────────────
// List<T> no longer uses native runtime storage. It uses inline
// fields (items_array, size, version) in the managed GC object.
// See chaos_list_fields in generated_code_compat.h.
// ListRuntimeStorage and require_list_runtime_storage have been
// removed — List<T> operations are emitted as inline C++ code.

// ── Dictionary<K,V> — O(1) hash table via unordered_dense ───────
template <typename TKey, typename TValue>
struct DictionaryRuntimeStorage
{
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(TKey, TValue) entries;
};

template <typename TKey, typename TValue>
DictionaryRuntimeStorage<TKey, TValue>* require_dictionary_runtime_storage(CHAOS_IL2CPP_INTPTR handle)
{
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }
    // Embedded pointer at known offset — no global map lookup
    auto* slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
        reinterpret_cast<char*>(handle) + kNativeStorageSlotOffset);
    if (*slot != 0)
    {
        return reinterpret_cast<DictionaryRuntimeStorage<TKey, TValue>*>(*slot);
    }
    auto* storage = new DictionaryRuntimeStorage<TKey, TValue>();
    *slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);
    return storage;
}

/// Const-qualified accessor for read-only dictionary operations (lookup, count).
/// Returns nullptr if no storage has been allocated yet — callers must check.
template <typename TKey, typename TValue>
const DictionaryRuntimeStorage<TKey, TValue>*
get_dictionary_runtime_storage(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return nullptr;
    }
    auto* slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
        reinterpret_cast<char*>(handle) + kNativeStorageSlotOffset);
    if (*slot == 0)
    {
        return nullptr;
    }
    return reinterpret_cast<const DictionaryRuntimeStorage<TKey, TValue>*>(*slot);
}

// ── HashSet<T> ──────────────────────────────────────────────────
// HashSet<T> reuses ListRuntimeStorage (vector of values with linear scan).
// For HashSet<T>, the GC object also embeds the native storage pointer.

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_COLLECTION_H_
