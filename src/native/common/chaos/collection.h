#ifndef CHAOS_IL2CPP_COMMON_COLLECTION_H_
#define CHAOS_IL2CPP_COMMON_COLLECTION_H_

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ChaosIl2cpp::Common {

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
    using StorageMap = std::unordered_map<CHAOS_IL2CPP_INTPTR, std::unique_ptr<ListRuntimeStorage<TValue>>>;
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
    using StorageMap = std::unordered_map<CHAOS_IL2CPP_INTPTR, std::unique_ptr<DictionaryRuntimeStorage<TKey, TValue>>>;
    static StorageMap storage_by_handle;
    auto& storage = storage_by_handle[handle];
    if (storage == nullptr)
    {
        storage = std::make_unique<DictionaryRuntimeStorage<TKey, TValue>>();
    }
    return storage.get();
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_COLLECTION_H_
