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
    std::vector<TValue> items;
};

template <typename TValue>
ListRuntimeStorage<TValue>* require_list_runtime_storage(std::intptr_t handle)
{
    if (handle == static_cast<std::intptr_t>(0))
    {
        std::abort();
    }
    static std::unordered_map<std::intptr_t, std::unique_ptr<ListRuntimeStorage<TValue>>> storage_by_handle;
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
    std::vector<std::pair<TKey, TValue>> entries;
};

template <typename TKey, typename TValue>
DictionaryRuntimeStorage<TKey, TValue>* require_dictionary_runtime_storage(std::intptr_t handle)
{
    if (handle == static_cast<std::intptr_t>(0))
    {
        std::abort();
    }
    static std::unordered_map<std::intptr_t, std::unique_ptr<DictionaryRuntimeStorage<TKey, TValue>>> storage_by_handle;
    auto& storage = storage_by_handle[handle];
    if (storage == nullptr)
    {
        storage = std::make_unique<DictionaryRuntimeStorage<TKey, TValue>>();
    }
    return storage.get();
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_COLLECTION_H_
