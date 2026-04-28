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
    CHAOS_IL2CPP_MUTEX mutex;
    CHAOS_IL2CPP_UNIQUE_PTR(CHAOS_IL2CPP_THREAD) worker;
    CHAOS_IL2CPP_INTPTR thread_start_delegate = 0;
    CHAOS_IL2CPP_INTPTR name = 0;
    CHAOS_IL2CPP_INT32 managed_thread_id = 0;
};

inline CHAOS_IL2CPP_MUTEX& thread_runtime_table_mutex()
{
    static CHAOS_IL2CPP_MUTEX m;
    return m;
}

inline CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_UNIQUE_PTR(ThreadRuntimeEntry))& thread_runtime_table()
{
    static CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_UNIQUE_PTR(ThreadRuntimeEntry)) table;
    return table;
}

inline CHAOS_IL2CPP_INT32 allocate_managed_thread_id()
{
    static CHAOS_IL2CPP_MUTEX id_mutex;
    static CHAOS_IL2CPP_INT32 next_id = 2;
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(id_mutex);
    return next_id++;
}

inline thread_local CHAOS_IL2CPP_INTPTR current_thread_object = 0;
inline thread_local CHAOS_IL2CPP_INT32 current_managed_thread_id = 1;

inline ThreadRuntimeEntry* try_get_thread_runtime_entry(CHAOS_IL2CPP_INTPTR thread_object_value)
{
    if (thread_object_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        return nullptr;
    }
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(thread_runtime_table_mutex());
    const auto it = thread_runtime_table().find(thread_object_value);
    if (it == thread_runtime_table().end())
    {
        return nullptr;
    }
    return it->second.get();
}

inline ThreadRuntimeEntry& require_thread_runtime_entry(CHAOS_IL2CPP_INTPTR thread_object_value)
{
    if (thread_object_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(thread_runtime_table_mutex());
    auto& entry = thread_runtime_table()[thread_object_value];
    if (!entry)
    {
        entry = CHAOS_IL2CPP_MAKE_UNIQUE(ThreadRuntimeEntry)();
    }
    return *entry;
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_THREAD_H_
