// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// random_stubs.cpp — Random stub implementations (real xorshift32 PRNG)
#include <chaos/native_types.h>
#include <cstring>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

void ChaosRandomCtor(CHAOS_IL2CPP_INTPTR instance) noexcept { (void)instance; }

void ChaosRandomNextBytes(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INTPTR buffer) noexcept
{
    (void)rng;
    if (buffer == 0) return;
    auto* hdr = reinterpret_cast<StubArrayHeader*>(buffer);
    auto* bytes = reinterpret_cast<CHAOS_IL2CPP_UINT8*>(hdr + 1);
    for (CHAOS_IL2CPP_UINTPTR i = 0; i < hdr->length; ++i) {
        bytes[i] = static_cast<CHAOS_IL2CPP_UINT8>(stub_xorshift32());
    }
}

CHAOS_IL2CPP_INT64 ChaosRandomNextDouble(CHAOS_IL2CPP_INTPTR rng) noexcept
{
    (void)rng;
    uint64_t bits = (static_cast<uint64_t>(stub_xorshift32()) << 21)
                  | (static_cast<uint64_t>(stub_xorshift32()) >> 11);
    bits = (0x3FFULL << 52) | (bits & 0x000FFFFFFFFFFFFFULL);
    double d;
    std::memcpy(&d, &bits, sizeof(d));
    d -= 1.0;
    CHAOS_IL2CPP_INT64 result;
    std::memcpy(&result, &d, sizeof(result));
    return result;
}

CHAOS_IL2CPP_INT32 ChaosRandomNext(CHAOS_IL2CPP_INTPTR rng) noexcept
{
    (void)rng;
    return static_cast<CHAOS_IL2CPP_INT32>(stub_xorshift32());
}

CHAOS_IL2CPP_INT32 ChaosRandomNextMax(CHAOS_IL2CPP_INTPTR rng, CHAOS_IL2CPP_INT32 maxValue) noexcept
{
    (void)rng;
    if (maxValue <= 0) return 0;
    return static_cast<CHAOS_IL2CPP_INT32>(stub_xorshift32() % static_cast<uint32_t>(maxValue));
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core