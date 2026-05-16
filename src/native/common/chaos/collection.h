#ifndef CHAOS_IL2CPP_COMMON_COLLECTION_H_
#define CHAOS_IL2CPP_COMMON_COLLECTION_H_

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <utility>
#include <vector>

// Compile-time toggle: define CHAOS_IL2CPP_USE_DENSE_MAP before including
// collection.h to replace internal std::unordered_map with unordered_dense
// for runtime storage lookups.  See <chaos/unordered_dense.h> for limitations.
#if defined(CHAOS_IL2CPP_USE_DENSE_MAP)
#  include <chaos/unordered_dense.h>
#  define CHAOS_IL2CPP_COLLECTION_MAP(K, V) CHAOS_IL2CPP_UNORDERED_DENSE_MAP(K, V)
#else
#  include <unordered_map>
#  define CHAOS_IL2CPP_COLLECTION_MAP(K, V) std::unordered_map<K, V>
#endif

namespace chaos::il2cpp::common {

template <typename TValue>
struct ListRuntimeStorage
{
    CHAOS_IL2CPP_VECTOR(TValue) items;
};

template <typename TValue>
ListRuntimeStorage<TValue>* require_list_runtime_storage(CHAOS_IL2CPP_INTPTR handle)
{
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }
    using StorageMap = CHAOS_IL2CPP_COLLECTION_MAP(CHAOS_IL2CPP_INTPTR, std::unique_ptr<ListRuntimeStorage<TValue>>);
    static StorageMap storage_by_handle;
    auto& storage = storage_by_handle[handle];
    if (storage == nullptr)
    {
        storage = std::make_unique<ListRuntimeStorage<TValue>>();
    }
    return storage.get();
}

template <typename TKey, typename TValue>
struct DictionaryRuntimeStorage
{
    CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_PAIR(TKey, TValue)) entries;
};

template <typename TKey, typename TValue>
DictionaryRuntimeStorage<TKey, TValue>* require_dictionary_runtime_storage(CHAOS_IL2CPP_INTPTR handle)
{
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }
    using MapValueT = std::unique_ptr<DictionaryRuntimeStorage<TKey, TValue>>;
    using StorageMap = CHAOS_IL2CPP_COLLECTION_MAP(CHAOS_IL2CPP_INTPTR, MapValueT);
    static StorageMap storage_by_handle;
    auto& storage = storage_by_handle[handle];
    if (storage == nullptr)
    {
        storage = std::make_unique<DictionaryRuntimeStorage<TKey, TValue>>();
    }
    return storage.get();
}

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_COLLECTION_H_
