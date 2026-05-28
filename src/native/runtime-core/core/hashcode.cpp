#include <chaos/native_types.h>
#include <cstring>

namespace chaos::il2cpp::runtime_core {

static CHAOS_IL2CPP_UINT32 BitCastSingleToUInt32(float value) {
    CHAOS_IL2CPP_UINT32 bits = 0u;
    CHAOS_IL2CPP_MEMCPY(&bits, &value, sizeof(bits));
    return bits;
}

static CHAOS_IL2CPP_UINT64 BitCastDoubleToUInt64(double value) {
    CHAOS_IL2CPP_UINT64 bits = 0u;
    CHAOS_IL2CPP_MEMCPY(&bits, &value, sizeof(bits));
    return bits;
}

constexpr CHAOS_IL2CPP_UINT32 kHashCodePrime1 = 2654435761u;
constexpr CHAOS_IL2CPP_UINT32 kHashCodePrime2 = 2246822519u;
constexpr CHAOS_IL2CPP_UINT32 kHashCodePrime3 = 3266489917u;
constexpr CHAOS_IL2CPP_UINT32 kHashCodePrime4 = 668265263u;
constexpr CHAOS_IL2CPP_UINT32 kHashCodePrime5 = 374761393u;
constexpr CHAOS_IL2CPP_UINT32 kHashCodeSeed = 0u;

static CHAOS_IL2CPP_UINT32 RotateLeft32(CHAOS_IL2CPP_UINT32 value, int offset) {
    return (value << offset) | (value >> (32 - offset));
}

static CHAOS_IL2CPP_UINT32 HashCodeRound(CHAOS_IL2CPP_UINT32 hash, CHAOS_IL2CPP_UINT32 input) {
    return RotateLeft32(hash + input * kHashCodePrime2, 13) * kHashCodePrime1;
}

static CHAOS_IL2CPP_UINT32 HashCodeQueueRound(CHAOS_IL2CPP_UINT32 hash, CHAOS_IL2CPP_UINT32 queued_value) {
    return RotateLeft32(hash + queued_value * kHashCodePrime3, 17) * kHashCodePrime4;
}

static CHAOS_IL2CPP_UINT32 HashCodeMixState(CHAOS_IL2CPP_UINT32 v1, CHAOS_IL2CPP_UINT32 v2, CHAOS_IL2CPP_UINT32 v3, CHAOS_IL2CPP_UINT32 v4) {
    return RotateLeft32(v1, 1) + RotateLeft32(v2, 7) + RotateLeft32(v3, 12) + RotateLeft32(v4, 18);
}

static CHAOS_IL2CPP_UINT32 HashCodeMixEmptyState() { return kHashCodeSeed + kHashCodePrime5; }

static CHAOS_IL2CPP_UINT32 HashCodeMixFinal(CHAOS_IL2CPP_UINT32 hash) {
    hash ^= hash >> 15;
    hash *= kHashCodePrime2;
    hash ^= hash >> 13;
    hash *= kHashCodePrime3;
    hash ^= hash >> 16;
    return hash;
}

static CHAOS_IL2CPP_UINT32 SingleGetHashCodeValue(float value) {
    CHAOS_IL2CPP_UINT32 bits = BitCastSingleToUInt32(value);
    if (SingleIsNaNOrZero(value)) bits &= 0x7F800000u;
    return bits;
}

static CHAOS_IL2CPP_INT32 HashCodeCombine2(CHAOS_IL2CPP_UINT32 hc1, CHAOS_IL2CPP_UINT32 hc2) {
    CHAOS_IL2CPP_UINT32 hash = HashCodeMixEmptyState();
    hash += 8u;
    hash = HashCodeQueueRound(hash, hc1);
    hash = HashCodeQueueRound(hash, hc2);
    return static_cast<CHAOS_IL2CPP_INT32>(HashCodeMixFinal(hash));
}

static CHAOS_IL2CPP_INT32 HashCodeCombine3(CHAOS_IL2CPP_UINT32 hc1, CHAOS_IL2CPP_UINT32 hc2, CHAOS_IL2CPP_UINT32 hc3) {
    CHAOS_IL2CPP_UINT32 hash = HashCodeMixEmptyState();
    hash += 12u;
    hash = HashCodeQueueRound(hash, hc1);
    hash = HashCodeQueueRound(hash, hc2);
    hash = HashCodeQueueRound(hash, hc3);
    return static_cast<CHAOS_IL2CPP_INT32>(HashCodeMixFinal(hash));
}

static CHAOS_IL2CPP_INT32 HashCodeCombine4(CHAOS_IL2CPP_UINT32 hc1, CHAOS_IL2CPP_UINT32 hc2, CHAOS_IL2CPP_UINT32 hc3, CHAOS_IL2CPP_UINT32 hc4) {
    CHAOS_IL2CPP_UINT32 v1 = kHashCodeSeed + kHashCodePrime1 + kHashCodePrime2;
    CHAOS_IL2CPP_UINT32 v2 = kHashCodeSeed + kHashCodePrime2;
    CHAOS_IL2CPP_UINT32 v3 = kHashCodeSeed;
    CHAOS_IL2CPP_UINT32 v4 = kHashCodeSeed - kHashCodePrime1;

    v1 = HashCodeRound(v1, hc1);
    v2 = HashCodeRound(v2, hc2);
    v3 = HashCodeRound(v3, hc3);
    v4 = HashCodeRound(v4, hc4);

    CHAOS_IL2CPP_UINT32 hash = HashCodeMixState(v1, v2, v3, v4);
    hash += 16u;
    return static_cast<CHAOS_IL2CPP_INT32>(HashCodeMixFinal(hash));
}

}  // namespace chaos::il2cpp::runtime_core