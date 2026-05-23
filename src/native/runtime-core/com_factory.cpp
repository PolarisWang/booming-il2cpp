#include <mutex>
#include <chaos/native_types.h>
#include <chaos/unordered_dense.h>
#include <chaos/log.h>
#include "com_factory.h"

namespace chaos::il2cpp::runtime_core {
namespace {

CHAOS_IL2CPP_MUTEX& GetFactoryMutex() {
    static CHAOS_IL2CPP_MUTEX s_mutex;
    return s_mutex;
}

CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(CHAOS_IL2CPP_UINT64, CcwFactoryFn)& GetFactoryTable() {
    static CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(CHAOS_IL2CPP_UINT64, CcwFactoryFn) s_table;
    return s_table;
}

}  // anonymous namespace

void RegisterCcwFactory(CHAOS_IL2CPP_UINT64 stable_id, CcwFactoryFn factory) noexcept {
    if (factory == nullptr) return;
    auto& table = GetFactoryTable();
    auto& mtx = GetFactoryMutex();
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(mtx);
    table[stable_id] = factory;
    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "Registered CCW factory for stable_id=0x{0:X16}",
                              static_cast<CHAOS_IL2CPP_UINT64>(stable_id));
}

void UnregisterCcwFactory(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    auto& table = GetFactoryTable();
    auto& mtx = GetFactoryMutex();
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(mtx);
    table.erase(stable_id);
    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "Unregistered CCW factory for stable_id=0x{0:X16}",
                              static_cast<CHAOS_IL2CPP_UINT64>(stable_id));
}

CcwFactoryFn FindCcwFactory(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    auto& table = GetFactoryTable();
    auto& mtx = GetFactoryMutex();
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(mtx);
    auto it = table.find(stable_id);
    if (it != table.end()) {
        return it->second;
    }
    return nullptr;
}

CHAOS_IL2CPP_INTPTR CreateCcwForStableId(CHAOS_IL2CPP_UINT64 stable_id,
                                           void* managed_object,
                                           void* runtime_state) noexcept {
    auto& table = GetFactoryTable();
    auto& mtx = GetFactoryMutex();
    if (factory == nullptr) {
        CHAOS_IL2CPP_LOG_DEBUG_M("COM", "No CCW factory found for stable_id=0x{0:X16}",
                                  static_cast<CHAOS_IL2CPP_UINT64>(stable_id));
        return 0;
    }
    return factory(managed_object, runtime_state);
}

}  // namespace chaos::il2cpp::runtime_core
