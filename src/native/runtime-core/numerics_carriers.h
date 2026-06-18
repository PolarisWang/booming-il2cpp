#ifndef CHAOS_IL2CPP_NUMERICS_CARRIERS_H_
#define CHAOS_IL2CPP_NUMERICS_CARRIERS_H_

#include <chaos/native_types.h>

#if defined(__x86_64__) || defined(_M_AMD64)
#include <immintrin.h>
#endif

namespace chaos::il2cpp::numerics_carriers {

struct RuntimeNumericsVector2Carrier {
    float x;
    float y;
};

struct RuntimeNumericsVector3Carrier {
    float x;
    float y;
    float z;
};

struct RuntimeNumericsVector4Carrier {
    float x;
    float y;
    float z;
    float w;
};

struct RuntimeNumericsMatrix3x2Carrier {
    float m11;
    float m12;
    float m21;
    float m22;
    float m31;
    float m32;
};

struct RuntimeNumericsMatrix4x4Carrier {
    float m11;
    float m12;
    float m13;
    float m14;
    float m21;
    float m22;
    float m23;
    float m24;
    float m31;
    float m32;
    float m33;
    float m34;
    float m41;
    float m42;
    float m43;
    float m44;
};

struct RuntimeNumericsQuaternionCarrier {
    float x;
    float y;
    float z;
    float w;
};

struct RuntimeIntrinsicVector64Carrier {
    CHAOS_IL2CPP_UINT8 bytes[8];
    CHAOS_IL2CPP_UINT8& operator[](CHAOS_IL2CPP_SIZE i) noexcept { return bytes[i]; }
    const CHAOS_IL2CPP_UINT8& operator[](CHAOS_IL2CPP_SIZE i) const noexcept { return bytes[i]; }
};

#if defined(__x86_64__) || defined(_M_AMD64)
struct alignas(16) RuntimeIntrinsicVector128Carrier {
    __m128i value;
    CHAOS_IL2CPP_UINT8& operator[](CHAOS_IL2CPP_SIZE i) noexcept {
        return reinterpret_cast<CHAOS_IL2CPP_UINT8*>(&value)[i];
    }
    const CHAOS_IL2CPP_UINT8& operator[](CHAOS_IL2CPP_SIZE i) const noexcept {
        return reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(&value)[i];
    }
};

struct alignas(32) RuntimeIntrinsicVector256Carrier {
    __m256i value;
    CHAOS_IL2CPP_UINT8& operator[](CHAOS_IL2CPP_SIZE i) noexcept {
        return reinterpret_cast<CHAOS_IL2CPP_UINT8*>(&value)[i];
    }
    const CHAOS_IL2CPP_UINT8& operator[](CHAOS_IL2CPP_SIZE i) const noexcept {
        return reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(&value)[i];
    }
};
#else
struct alignas(16) RuntimeIntrinsicVector128Carrier {
    CHAOS_IL2CPP_UINT8 bytes[16];
    CHAOS_IL2CPP_UINT8& operator[](CHAOS_IL2CPP_SIZE i) noexcept { return bytes[i]; }
    const CHAOS_IL2CPP_UINT8& operator[](CHAOS_IL2CPP_SIZE i) const noexcept { return bytes[i]; }
};

struct alignas(32) RuntimeIntrinsicVector256Carrier {
    CHAOS_IL2CPP_UINT8 bytes[32];
    CHAOS_IL2CPP_UINT8& operator[](CHAOS_IL2CPP_SIZE i) noexcept { return bytes[i]; }
    const CHAOS_IL2CPP_UINT8& operator[](CHAOS_IL2CPP_SIZE i) const noexcept { return bytes[i]; }
};
#endif

struct RuntimeIntrinsicVector512Carrier {
    CHAOS_IL2CPP_UINT8 bytes[64];
};

}  // namespace chaos::il2cpp::numerics_carriers

#endif  // CHAOS_IL2CPP_NUMERICS_CARRIERS_H_
