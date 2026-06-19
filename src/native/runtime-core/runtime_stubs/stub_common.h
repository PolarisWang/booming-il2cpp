#ifndef CHAOS_IL2CPP_STUB_COMMON_H_
#define CHAOS_IL2CPP_STUB_COMMON_H_
#pragma once

#include <chaos/native_types.h>
#include <chaos/log.h>
#include <ctime>
#include <cstring>

#include "gc_helpers.h"

struct StubArrayHeader {
    CHAOS_IL2CPP_INTPTR element_type;
    CHAOS_IL2CPP_UINTPTR length;
};

// ManagedArrayAccessor mirrors chaos_managed_array (ThinLockableHeader 8B + fields).
// NOTE: elements pointer was removed — element data is contiguous after the header.
// Stubs must NOT reinterpret_cast<StubArrayHeader> on codegen arrays.
struct ManagedArrayAccessor {
    CHAOS_IL2CPP_UINT8  header_data[8];
    CHAOS_IL2CPP_UINT8  element_type_shape;
    CHAOS_IL2CPP_UINT8  padding[7];
    const void*         element_type_info;
    CHAOS_IL2CPP_INTPTR length;
    // elements pointer removed — contiguous after header.
};
static_assert(sizeof(ManagedArrayAccessor) == 32,
    "ManagedArrayAccessor must be 32 bytes");

inline const ManagedArrayAccessor* get_managed_array(CHAOS_IL2CPP_INTPTR handle) noexcept {
    return reinterpret_cast<const ManagedArrayAccessor*>(handle);
}

inline ManagedArrayAccessor* get_managed_array_mut(CHAOS_IL2CPP_INTPTR handle) noexcept {
    return reinterpret_cast<ManagedArrayAccessor*>(handle);
}

// Access inline element storage (contiguous after the fixed header).
inline CHAOS_IL2CPP_INTPTR* accessor_get_elements(ManagedArrayAccessor* arr) noexcept {
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
        reinterpret_cast<uint8_t*>(arr) + sizeof(ManagedArrayAccessor));
}
inline const CHAOS_IL2CPP_INTPTR* accessor_get_elements(const ManagedArrayAccessor* arr) noexcept {
    return reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
        reinterpret_cast<const uint8_t*>(arr) + sizeof(ManagedArrayAccessor));
}

struct StubStringHeader {
    CHAOS_IL2CPP_INTPTR type;
    CHAOS_IL2CPP_UINTPTR byte_count;
};

inline const char* stub_string_data(const void* str) noexcept {
    return reinterpret_cast<const char*>(static_cast<const StubStringHeader*>(str) + 1);
}

inline uint32_t stub_xorshift32() noexcept {
    thread_local uint32_t state = 0;
    if (state == 0) {
        state = static_cast<uint32_t>(
            reinterpret_cast<uintptr_t>(&state) ^
            static_cast<uint32_t>(std::time(nullptr)));
        if (state == 0) state = 1;
    }
    uint32_t x = state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state = x;
    return x;
}
#endif  // CHAOS_IL2CPP_STUB_COMMON_H_
