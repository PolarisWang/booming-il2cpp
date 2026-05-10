#pragma once

#include <chaos/native_types.h>
#include <ctime>

// ── Stub helper structs for Array and String layouts ──────────────
// These are used by multiple stub files. They mirror the managed
// object layout so that stubs can read length/data from raw pointers.

struct StubArrayHeader {
    CHAOS_IL2CPP_INTPTR element_type;
    CHAOS_IL2CPP_UINTPTR length;
};

struct StubStringHeader {
    CHAOS_IL2CPP_INTPTR type;
    CHAOS_IL2CPP_UINTPTR byte_count;
};

// Get UTF-8 data pointer from a managed string.
inline const char* stub_string_data(const void* str) noexcept {
    return reinterpret_cast<const char*>(static_cast<const StubStringHeader*>(str) + 1);
}

// Thread-local xorshift32 PRNG used by both Guid and Random stubs.
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