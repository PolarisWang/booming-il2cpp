#ifndef CHAOS_IL2CPP_LOAD_STORE_CHAOS_BRIDGE_H_
#define CHAOS_IL2CPP_LOAD_STORE_CHAOS_BRIDGE_H_

// ── Bridge: chaos_store_* / chaos_load_* → chaos::il2cpp::common::store_* / load_* ──
//
// Generated C++ code (native-aot.generated.cpp) calls chaos_store_float32(),
// chaos_load_float64(), chaos_store_int64(), etc. as bare identifiers.
// The actual implementations live in chaos::il2cpp::common namespace
// (load_store.h included via <chaos/common.h>).
//
// These thin inline wrappers bridge the naming gap so that no changes
// to the emitter are needed for older generated files.

#include <chaos/load_store.h>

// ── Wrappers exposed inside runtime_core namespace ──
// The emitter references these as bare identifiers (e.g. ChaosStoreFloat32).
// After wrapping in the namespace, global-scope using declarations make
// them visible to generated code (which lives in anonymous namespaces).

namespace chaos::il2cpp::runtime_core {

inline CHAOS_IL2CPP_INTPTR chaos_store_float32(float value) noexcept
{
    return chaos::il2cpp::common::store_float32(value);
}

inline float chaos_load_float32(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return chaos::il2cpp::common::load_float32(value);
}

inline CHAOS_IL2CPP_INTPTR chaos_store_float64(double value) noexcept
{
    return chaos::il2cpp::common::store_float64(value);
}

inline double chaos_load_float64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return chaos::il2cpp::common::load_float64(value);
}

inline CHAOS_IL2CPP_INTPTR chaos_store_int64(CHAOS_IL2CPP_INT64 value) noexcept
{
    return chaos::il2cpp::common::store_int64(value);
}

inline CHAOS_IL2CPP_INT64 chaos_load_int64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return chaos::il2cpp::common::load_int64(value);
}

inline CHAOS_IL2CPP_INTPTR chaos_store_uint64(CHAOS_IL2CPP_UINT64 value) noexcept
{
    return chaos::il2cpp::common::store_uint64(value);
}

inline CHAOS_IL2CPP_UINT64 chaos_load_uint64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return chaos::il2cpp::common::load_uint64(value);
}

inline CHAOS_IL2CPP_INT32 chaos_shift_left_int32(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 shift) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>(value) << (shift & 31));
}

inline CHAOS_IL2CPP_INT32 chaos_shift_right_int32(CHAOS_IL2CPP_INT32 value, CHAOS_IL2CPP_INT32 shift) noexcept
{
    return value >> (shift & 31);
}

}  // namespace chaos::il2cpp::runtime_core

// Expose at global scope so generated code (inside anonymous namespaces)
// can call these as bare identifiers.
using chaos::il2cpp::runtime_core::chaos_store_float32;
using chaos::il2cpp::runtime_core::chaos_load_float32;
using chaos::il2cpp::runtime_core::chaos_store_float64;
using chaos::il2cpp::runtime_core::chaos_load_float64;
using chaos::il2cpp::runtime_core::chaos_store_int64;
using chaos::il2cpp::runtime_core::chaos_load_int64;
using chaos::il2cpp::runtime_core::chaos_store_uint64;
using chaos::il2cpp::runtime_core::chaos_load_uint64;
using chaos::il2cpp::runtime_core::chaos_shift_left_int32;
using chaos::il2cpp::runtime_core::chaos_shift_right_int32;

#endif  // CHAOS_IL2CPP_LOAD_STORE_CHAOS_BRIDGE_H_
