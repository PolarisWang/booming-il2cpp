// ABI exports: extern "C" linkage for managed/NativeAOT callability.

// hashcode_stubs.cpp — HashCode helper implementations (real)
#include <chaos/native_types.h>
#include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

static CHAOS_IL2CPP_UINT32 ChaosHashCodeRotateLeft32(CHAOS_IL2CPP_UINT32 value, int offset) noexcept {
    return (value << offset) | (value >> (32 - offset));
}

static CHAOS_IL2CPP_UINT32 ChaosHashCodeMixFinal(CHAOS_IL2CPP_UINT32 hash) noexcept {
    constexpr CHAOS_IL2CPP_UINT32 kPrime2 = 2246822519u;
    constexpr CHAOS_IL2CPP_UINT32 kPrime3 = 3266489917u;
    hash ^= hash >> 15;
    hash *= kPrime2;
    hash ^= hash >> 13;
    hash *= kPrime3;
    hash ^= hash >> 16;
    return hash;
}

static CHAOS_IL2CPP_UINT32 ChaosHashCodeQueueRound(CHAOS_IL2CPP_UINT32 hash, CHAOS_IL2CPP_UINT32 queued_value) noexcept {
    constexpr CHAOS_IL2CPP_UINT32 kPrime3 = 3266489917u;
    constexpr CHAOS_IL2CPP_UINT32 kPrime4 = 668265263u;
    return ChaosHashCodeRotateLeft32(hash + queued_value * kPrime3, 17) * kPrime4;
}

CHAOS_IL2CPP_INT32 ChaosHashCodeToHashCode(CHAOS_IL2CPP_INTPTR state) noexcept
{
    if (state == 0) return 0;
    auto* acc = reinterpret_cast<CHAOS_IL2CPP_UINT32*>(state);
    constexpr CHAOS_IL2CPP_UINT32 kPrime5 = 374761393u;
    return static_cast<CHAOS_IL2CPP_INT32>(ChaosHashCodeMixFinal(*acc + kPrime5));
}

CHAOS_IL2CPP_INT32 ChaosHashCodeCombine2(CHAOS_IL2CPP_INT32 hc1, CHAOS_IL2CPP_INT32 hc2) noexcept
{
    constexpr CHAOS_IL2CPP_UINT32 kPrime5 = 374761393u;
    constexpr CHAOS_IL2CPP_UINT32 kSeed = 0u;
    CHAOS_IL2CPP_UINT32 hash = kSeed + kPrime5;
    hash += 8u;
    hash = ChaosHashCodeQueueRound(hash, static_cast<CHAOS_IL2CPP_UINT32>(hc1));
    hash = ChaosHashCodeQueueRound(hash, static_cast<CHAOS_IL2CPP_UINT32>(hc2));
    return static_cast<CHAOS_IL2CPP_INT32>(ChaosHashCodeMixFinal(hash));
}

void ChaosHashCodeAdd(CHAOS_IL2CPP_INTPTR state, CHAOS_IL2CPP_INT32 value) noexcept
{
    if (state == 0) return;
    auto* acc = reinterpret_cast<CHAOS_IL2CPP_UINT32*>(state);
    constexpr CHAOS_IL2CPP_UINT32 kPrime3 = 3266489917u;
    constexpr CHAOS_IL2CPP_UINT32 kPrime4 = 668265263u;
    *acc = ChaosHashCodeRotateLeft32(*acc + static_cast<CHAOS_IL2CPP_UINT32>(value) * kPrime3, 17) * kPrime4;
}

CHAOS_IL2CPP_INT32 ChaosInt32GetHashCode(CHAOS_IL2CPP_INTPTR value_ptr) noexcept {
    if (value_ptr == 0) return 0;
    return *reinterpret_cast<CHAOS_IL2CPP_INT32*>(value_ptr);
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core