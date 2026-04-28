#ifndef CHAOS_IL2CPP_COMMON_THREAD_H_
#define CHAOS_IL2CPP_COMMON_THREAD_H_

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace ChaosIl2cpp::Common {

struct ThreadRuntimeEntry
{
    std::mutex mutex;
    std::unique_ptr<std::thread> worker;
    std::intptr_t thread_start_delegate = 0;
    std::intptr_t name = 0;
    std::int32_t managed_thread_id = 0;
};

inline std::mutex& thread_runtime_table_mutex()
{
    static std::mutex m;
    return m;
}

inline std::unordered_map<std::intptr_t, std::unique_ptr<ThreadRuntimeEntry>>& thread_runtime_table()
{
    static std::unordered_map<std::intptr_t, std::unique_ptr<ThreadRuntimeEntry>> table;
    return table;
}

inline std::int32_t allocate_managed_thread_id()
{
    static std::mutex id_mutex;
    static std::int32_t next_id = 2;
    std::lock_guard<std::mutex> guard(id_mutex);
    return next_id++;
}

inline thread_local std::intptr_t current_thread_object = 0;
inline thread_local std::int32_t current_managed_thread_id = 1;

inline ThreadRuntimeEntry* try_get_thread_runtime_entry(std::intptr_t thread_object_value)
{
    if (thread_object_value == static_cast<std::intptr_t>(0))
    {
        return nullptr;
    }
    std::lock_guard<std::mutex> guard(thread_runtime_table_mutex());
    const auto it = thread_runtime_table().find(thread_object_value);
    if (it == thread_runtime_table().end())
    {
        return nullptr;
    }
    return it->second.get();
}

inline ThreadRuntimeEntry& require_thread_runtime_entry(std::intptr_t thread_object_value)
{
    if (thread_object_value == static_cast<std::intptr_t>(0))
    {
        std::abort();
    }
    std::lock_guard<std::mutex> guard(thread_runtime_table_mutex());
    auto& entry = thread_runtime_table()[thread_object_value];
    if (!entry)
    {
        entry = std::make_unique<ThreadRuntimeEntry>();
    }
    return *entry;
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_THREAD_H_
