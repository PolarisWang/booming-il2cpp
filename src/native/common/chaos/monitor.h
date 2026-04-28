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
    std::recursive_timed_mutex mutex;
};

inline MonitorRuntimeEntry& require_monitor_runtime_entry(std::intptr_t object_value)
{
    if (object_value == static_cast<std::intptr_t>(0))
    {
        std::abort();
    }
    static std::mutex table_mutex;
    static std::unordered_map<std::intptr_t, std::unique_ptr<MonitorRuntimeEntry>> table;
    std::lock_guard<std::mutex> guard(table_mutex);
    auto& entry = table[object_value];
    if (!entry)
    {
        entry = std::make_unique<MonitorRuntimeEntry>();
    }
    return *entry;
}

} // namespace ChaosIl2cpp::Common

#endif // CHAOS_IL2CPP_COMMON_MONITOR_H_
