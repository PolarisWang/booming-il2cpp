#ifndef CHAOS_IL2CPP_NUMERICS_CARRIERS_H_
#define CHAOS_IL2CPP_NUMERICS_CARRIERS_H_

#include <chaos/native_types.h>

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
};

struct RuntimeIntrinsicVector128Carrier {
    CHAOS_IL2CPP_UINT8 bytes[16];
};

struct RuntimeIntrinsicVector256Carrier {
    CHAOS_IL2CPP_UINT8 bytes[32];
};

struct RuntimeIntrinsicVector512Carrier {
    CHAOS_IL2CPP_UINT8 bytes[64];
};

}  // namespace chaos::il2cpp::numerics_carriers

#endif  // CHAOS_IL2CPP_NUMERICS_CARRIERS_H_
