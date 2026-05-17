#ifndef CHAOS_IL2CPP_ENGINE_LIFECYCLE_H_
#define CHAOS_IL2CPP_ENGINE_LIFECYCLE_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>

namespace chaos::il2cpp::runtime_core {

// Lifecycle callback type used by engine lifecycle registrations.
using EngineLifecycleCallback = CHAOS_IL2CPP_INT32(CHAOS_RUNTIME_ABI_CALL*)(
    const char* phase_utf8, void* user_data);

struct EngineLifecycleRegistration {
    CHAOS_IL2CPP_STRING phase;
    EngineLifecycleCallback callback;
    void* user_data;
};

// GC handle table entry (defined in engine_lifecycle.cpp, referenced by
// abi_reflection.cpp and GC scan code at runtime_core namespace scope).
struct GcHandleEntry {
    void* object_instance;
    bool pinned;
    bool weak;
};

// ABI constants at runtime_core namespace scope (shared across core files).
extern const char* kEngineObservePrefix;

// Engine-global state.
extern CHAOS_IL2CPP_MUTEX g_engine_binding_mutex;
extern CHAOS_IL2CPP_UINTPTR g_next_engine_handle;
extern CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINTPTR, void*) g_engine_handles;
extern CHAOS_IL2CPP_VECTOR(EngineLifecycleRegistration) g_engine_lifecycle_registrations;
extern const CHAOS_IL2CPP_THREAD::id g_main_thread_id;

// GC handle state (referenced from abi_reflection.cpp in unity build).
extern CHAOS_IL2CPP_MUTEX s_gc_handle_mutex;
extern CHAOS_IL2CPP_ATOMIC(CHAOS_IL2CPP_UINT64) s_next_gc_handle;
extern CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT64, GcHandleEntry) s_gc_handle_table;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_ENGINE_LIFECYCLE_H_
