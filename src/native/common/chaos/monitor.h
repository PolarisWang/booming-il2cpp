#ifndef CHAOS_IL2CPP_COMMON_MONITOR_H_
#define CHAOS_IL2CPP_COMMON_MONITOR_H_

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace ChaosIl2cpp::Common {

struct MonitorRuntimeEntry
{
    CHAOS_IL2CPP_RECURSIVE_MUTEX mutex;
};

inline MonitorRuntimeEntry& require_monitor_runtime_entry(CHAOS_IL2CPP_INTPTR object_value)
{
    if (object_value == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }
    static CHAOS_IL2CPP_MUTEX table_mutex;
    static std::unordered_map<CHAOS_IL2CPP_INTPTR, std::unique_ptr<MonitorRuntimeEntry>> table;
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) guard(table_mutex);
    auto& entry = table[object_value];
    if (!entry)
    {
        entry = std::make_unique<MonitorRuntimeEntry>();
    }
    return *entry;
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_MONITOR_H_
