#include <mutex>
#include <chaos/native_types.h>
#include "com_factory.h"

namespace chaos::il2cpp::runtime_core {
namespace {

// Simple static registry using a fixed-size array to avoid any
// static initialization ordering issues with complex containers.
constexpr CHAOS_IL2CPP_SIZE kMaxFactoryEntries = 64;

struct FactoryEntry {
    CHAOS_IL2CPP_UINT64 stable_id;
    CcwFactoryFn factory;
};

FactoryEntry s_factory_table[kMaxFactoryEntries] = {};
CHAOS_IL2CPP_SIZE s_factory_count = 0;
CHAOS_IL2CPP_MUTEX s_factory_mutex;

}  // anonymous namespace

void RegisterCcwFactory(CHAOS_IL2CPP_UINT64 stable_id, CcwFactoryFn factory) noexcept {
    if (factory == nullptr) return;
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_factory_mutex);

    // Replace existing or add new.
    for (CHAOS_IL2CPP_SIZE i = 0; i < s_factory_count; ++i) {
        if (s_factory_table[i].stable_id == stable_id) {
            s_factory_table[i].factory = factory;
            return;
        }
    }

    // Add new entry if space available.
    if (s_factory_count < kMaxFactoryEntries) {
        s_factory_table[s_factory_count].stable_id = stable_id;
        s_factory_table[s_factory_count].factory = factory;
        ++s_factory_count;
    }
}

void UnregisterCcwFactory(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_factory_mutex);
    for (CHAOS_IL2CPP_SIZE i = 0; i < s_factory_count; ++i) {
        if (s_factory_table[i].stable_id == stable_id) {
            // Swap with last and decrement.
            s_factory_table[i] = s_factory_table[s_factory_count - 1];
            --s_factory_count;
            return;
        }
    }
}

CcwFactoryFn FindCcwFactory(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    std::lock_guard<CHAOS_IL2CPP_MUTEX> lock(s_factory_mutex);
    for (CHAOS_IL2CPP_SIZE i = 0; i < s_factory_count; ++i) {
        if (s_factory_table[i].stable_id == stable_id) {
            return s_factory_table[i].factory;
        }
    }
    return nullptr;
}

CHAOS_IL2CPP_INTPTR CreateCcwForStableId(CHAOS_IL2CPP_UINT64 stable_id,
                                           void* managed_object,
                                           void* runtime_state) noexcept {
    auto factory = FindCcwFactory(stable_id);
    if (factory == nullptr) {
        return 0;
    }
    return factory(managed_object, runtime_state);
}

}  // namespace chaos::il2cpp::runtime_core
