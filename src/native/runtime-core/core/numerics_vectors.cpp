#include <chaos/native_types.h>
#include <cmath>

namespace chaos::il2cpp::runtime_core {

static RuntimeNumericsVector2Carrier MakeVector2(float x, float y) { return RuntimeNumericsVector2Carrier{ x, y }; }
static RuntimeNumericsVector3Carrier MakeVector3(float x, float y, float z) { return RuntimeNumericsVector3Carrier{ x, y, z }; }
static RuntimeNumericsVector4Carrier MakeVector4(float x, float y, float z, float w) { return RuntimeNumericsVector4Carrier{ x, y, z, w }; }

template <typename TCarrier>
static bool VectorTryCopyToRaw(TCarrier value, float* destination, CHAOS_IL2CPP_SIZE destination_length, CHAOS_IL2CPP_SIZE start_index) {
    constexpr CHAOS_IL2CPP_SIZE lane_count = sizeof(TCarrier) / sizeof(float);
    if (start_index > destination_length || destination == nullptr) return false;
    const CHAOS_IL2CPP_SIZE remaining_length = destination_length - start_index;
    if (remaining_length < lane_count) return false;
    CHAOS_IL2CPP_MEMCPY(destination + start_index, &value, sizeof(TCarrier));
    return true;
}

static RuntimeNumericsVector2Carrier TransformVector2ByMatrix3x2(
    RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix3x2Carrier matrix, bool include_translation) {
    const float tx = include_translation ? matrix.m31 : 0.0f;
    const float ty = include_translation ? matrix.m32 : 0.0f;
    return MakeVector2(
        (value.x * matrix.m11) + (value.y * matrix.m21) + tx,
        (value.x * matrix.m12) + (value.y * matrix.m22) + ty);
}

static RuntimeNumericsVector4Carrier TransformVector2ByMatrix4x4(
    RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix) {
    return MakeVector4(
        (value.x * matrix.m11) + (value.y * matrix.m21) + matrix.m41,
        (value.x * matrix.m12) + (value.y * matrix.m22) + matrix.m42,
        (value.x * matrix.m13) + (value.y * matrix.m23) + matrix.m43,
        (value.x * matrix.m14) + (value.y * matrix.m24) + matrix.m44);
}

static RuntimeNumericsVector3Carrier TransformVector3ByMatrix4x4(
    RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix, bool include_translation) {
    const float tx = include_translation ? matrix.m41 : 0.0f;
    const float ty = include_translation ? matrix.m42 : 0.0f;
    const float tz = include_translation ? matrix.m43 : 0.0f;
    return MakeVector3(
        (value.x * matrix.m11) + (value.y * matrix.m21) + (value.z * matrix.m31) + tx,
        (value.x * matrix.m12) + (value.y * matrix.m22) + (value.z * matrix.m32) + ty,
        (value.x * matrix.m13) + (value.y * matrix.m23) + (value.z * matrix.m33) + tz);
}

static RuntimeNumericsVector3Carrier RotateVector3ByQuaternion(
    RuntimeNumericsVector3Carrier value, RuntimeNumericsQuaternionCarrier rotation) {
    const float x2 = rotation.x + rotation.x, y2 = rotation.y + rotation.y, z2 = rotation.z + rotation.z;
    const float wx2 = rotation.w * x2, wy2 = rotation.w * y2, wz2 = rotation.w * z2;
    const float xx2 = rotation.x * x2, xy2 = rotation.x * y2, xz2 = rotation.x * z2;
    const float yy2 = rotation.y * y2, yz2 = rotation.y * z2, zz2 = rotation.z * z2;

    return MakeVector3(
        (value.x * (1.0f - yy2 - zz2)) + (value.y * (xy2 - wz2)) + (value.z * (xz2 + wy2)),
        (value.x * (xy2 + wz2)) + (value.y * (1.0f - xx2 - zz2)) + (value.z * (yz2 - wx2)),
        (value.x * (xz2 - wy2)) + (value.y * (yz2 + wx2)) + (value.z * (1.0f - xx2 - yy2)));
}

// Vector2
RuntimeNumericsVector2Carrier Vector2Zero() { return MakeVector2(0.0f, 0.0f); }
RuntimeNumericsVector2Carrier Vector2One() { return MakeVector2(1.0f, 1.0f); }
RuntimeNumericsVector2Carrier Vector2UnitX() { return MakeVector2(1.0f, 0.0f); }
RuntimeNumericsVector2Carrier Vector2UnitY() { return MakeVector2(0.0f, 1.0f); }

RuntimeNumericsVector2Carrier Vector2Add(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return MakeVector2(l.x + r.x, l.y + r.y); }
RuntimeNumericsVector2Carrier Vector2Subtract(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return MakeVector2(l.x - r.x, l.y - r.y); }
RuntimeNumericsVector2Carrier Vector2Multiply(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return MakeVector2(l.x * r.x, l.y * r.y); }
RuntimeNumericsVector2Carrier Vector2MultiplyScalar(RuntimeNumericsVector2Carrier v, float s) { return MakeVector2(v.x * s, v.y * s); }
RuntimeNumericsVector2Carrier Vector2Divide(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return MakeVector2(l.x / r.x, l.y / r.y); }
RuntimeNumericsVector2Carrier Vector2DivideScalar(RuntimeNumericsVector2Carrier v, float s) { return MakeVector2(v.x / s, v.y / s); }
RuntimeNumericsVector2Carrier Vector2Abs(RuntimeNumericsVector2Carrier v) { return MakeVector2(CHAOS_IL2CPP_FABS(v.x), CHAOS_IL2CPP_FABS(v.y)); }
RuntimeNumericsVector2Carrier Vector2Min(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return MakeVector2(VectorMinElement(l.x, r.x), VectorMinElement(l.y, r.y)); }
RuntimeNumericsVector2Carrier Vector2Max(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return MakeVector2(VectorMaxElement(l.x, r.x), VectorMaxElement(l.y, r.y)); }
RuntimeNumericsVector2Carrier Vector2Clamp(RuntimeNumericsVector2Carrier v, RuntimeNumericsVector2Carrier mn, RuntimeNumericsVector2Carrier mx) { return Vector2Min(Vector2Max(v, mn), mx); }
RuntimeNumericsVector2Carrier Vector2SquareRoot(RuntimeNumericsVector2Carrier v) { return MakeVector2(CHAOS_IL2CPP_SQRT(v.x), CHAOS_IL2CPP_SQRT(v.y)); }
RuntimeNumericsVector2Carrier Vector2Normalize(RuntimeNumericsVector2Carrier v) { return Vector2DivideScalar(v, Vector2Length(v)); }
RuntimeNumericsVector2Carrier Vector2Lerp(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r, float a) { return Vector2Add(l, Vector2MultiplyScalar(Vector2Subtract(r, l), a)); }
RuntimeNumericsVector2Carrier Vector2Reflect(RuntimeNumericsVector2Carrier v, RuntimeNumericsVector2Carrier n) { return Vector2Subtract(v, Vector2MultiplyScalar(n, 2.0f * Vector2Dot(v, n))); }
RuntimeNumericsVector2Carrier Vector2Negate(RuntimeNumericsVector2Carrier v) { return MakeVector2(-v.x, -v.y); }
bool Vector2Equals(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return l.x == r.x && l.y == r.y; }
// A2-1: Vector2 lane-wise compare reducer — all-lanes satisfy predicate.
// GreaterThanAll/LessThanAll/... are the SIMD _All operators: true iff every
// component of l strictly satisfies the relation against the corresponding
// component of r.  Bool is returned as CHAOS_IL2CPP_INT32 (1/0) matching the
// fact-contract bool→Int64 subject encoding.
CHAOS_IL2CPP_INT32 Vector2GreaterThanAll(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return (l.x > r.x) && (l.y > r.y) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector2GreaterThanOrEqualAll(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return (l.x >= r.x) && (l.y >= r.y) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector2LessThanAll(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return (l.x < r.x) && (l.y < r.y) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector2LessThanOrEqualAll(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return (l.x <= r.x) && (l.y <= r.y) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector2EqualsAll(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return (l.x == r.x) && (l.y == r.y) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector2EqualsAny(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return (l.x == r.x) || (l.y == r.y) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector2GetHashCode(RuntimeNumericsVector2Carrier v) { return HashCodeCombine2(SingleGetHashCodeValue(v.x), SingleGetHashCodeValue(v.y)); }
float Vector2Length(RuntimeNumericsVector2Carrier v) { return CHAOS_IL2CPP_SQRT(Vector2LengthSquared(v)); }
float Vector2Dot(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return (l.x * r.x) + (l.y * r.y); }
float Vector2LengthSquared(RuntimeNumericsVector2Carrier v) { return Vector2Dot(v, v); }
float Vector2DistanceSquared(RuntimeNumericsVector2Carrier l, RuntimeNumericsVector2Carrier r) { return Vector2LengthSquared(Vector2Subtract(l, r)); }

float Vector2GetElement(RuntimeNumericsVector2Carrier v, CHAOS_IL2CPP_INT32 i) {
    switch (i) { case 0: return v.x; case 1: return v.y; default: return 0.0f; }
}

RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier v, RuntimeNumericsMatrix3x2Carrier m) { return TransformVector2ByMatrix3x2(v, m, true); }
RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier v, RuntimeNumericsMatrix4x4Carrier m) { auto t = TransformVector2ByMatrix4x4(v, m); return MakeVector2(t.x, t.y); }
RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier v, RuntimeNumericsQuaternionCarrier q) { auto t = RotateVector3ByQuaternion(MakeVector3(v.x, v.y, 0.0f), q); return MakeVector2(t.x, t.y); }
RuntimeNumericsVector2Carrier Vector2TransformNormal(RuntimeNumericsVector2Carrier v, RuntimeNumericsMatrix3x2Carrier m) { return TransformVector2ByMatrix3x2(v, m, false); }
RuntimeNumericsVector2Carrier Vector2TransformNormal(RuntimeNumericsVector2Carrier v, RuntimeNumericsMatrix4x4Carrier m) { return MakeVector2((v.x * m.m11) + (v.y * m.m21), (v.x * m.m12) + (v.y * m.m22)); }
bool Vector2TryCopyTo(RuntimeNumericsVector2Carrier v, float* d, CHAOS_IL2CPP_SIZE dl, CHAOS_IL2CPP_SIZE si) { return VectorTryCopyToRaw(v, d, dl, si); }

// Vector3
RuntimeNumericsVector3Carrier Vector3Zero() { return MakeVector3(0.0f, 0.0f, 0.0f); }
RuntimeNumericsVector3Carrier Vector3One() { return MakeVector3(1.0f, 1.0f, 1.0f); }
RuntimeNumericsVector3Carrier Vector3UnitX() { return MakeVector3(1.0f, 0.0f, 0.0f); }
RuntimeNumericsVector3Carrier Vector3UnitY() { return MakeVector3(0.0f, 1.0f, 0.0f); }
RuntimeNumericsVector3Carrier Vector3UnitZ() { return MakeVector3(0.0f, 0.0f, 1.0f); }

RuntimeNumericsVector3Carrier Vector3Add(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return MakeVector3(l.x + r.x, l.y + r.y, l.z + r.z); }
RuntimeNumericsVector3Carrier Vector3Subtract(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return MakeVector3(l.x - r.x, l.y - r.y, l.z - r.z); }
RuntimeNumericsVector3Carrier Vector3Multiply(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return MakeVector3(l.x * r.x, l.y * r.y, l.z * r.z); }
RuntimeNumericsVector3Carrier Vector3MultiplyScalar(RuntimeNumericsVector3Carrier v, float s) { return MakeVector3(v.x * s, v.y * s, v.z * s); }
RuntimeNumericsVector3Carrier Vector3Divide(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return MakeVector3(l.x / r.x, l.y / r.y, l.z / r.z); }
RuntimeNumericsVector3Carrier Vector3DivideScalar(RuntimeNumericsVector3Carrier v, float s) { return MakeVector3(v.x / s, v.y / s, v.z / s); }
RuntimeNumericsVector3Carrier Vector3Abs(RuntimeNumericsVector3Carrier v) { return MakeVector3(CHAOS_IL2CPP_FABS(v.x), CHAOS_IL2CPP_FABS(v.y), CHAOS_IL2CPP_FABS(v.z)); }
RuntimeNumericsVector3Carrier Vector3Min(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return MakeVector3(VectorMinElement(l.x, r.x), VectorMinElement(l.y, r.y), VectorMinElement(l.z, r.z)); }
RuntimeNumericsVector3Carrier Vector3Max(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return MakeVector3(VectorMaxElement(l.x, r.x), VectorMaxElement(l.y, r.y), VectorMaxElement(l.z, r.z)); }
RuntimeNumericsVector3Carrier Vector3Clamp(RuntimeNumericsVector3Carrier v, RuntimeNumericsVector3Carrier mn, RuntimeNumericsVector3Carrier mx) { return Vector3Min(Vector3Max(v, mn), mx); }
RuntimeNumericsVector3Carrier Vector3SquareRoot(RuntimeNumericsVector3Carrier v) { return MakeVector3(CHAOS_IL2CPP_SQRT(v.x), CHAOS_IL2CPP_SQRT(v.y), CHAOS_IL2CPP_SQRT(v.z)); }
RuntimeNumericsVector3Carrier Vector3Normalize(RuntimeNumericsVector3Carrier v) { return Vector3DivideScalar(v, Vector3Length(v)); }
RuntimeNumericsVector3Carrier Vector3Lerp(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r, float a) { return Vector3Add(l, Vector3MultiplyScalar(Vector3Subtract(r, l), a)); }
RuntimeNumericsVector3Carrier Vector3Reflect(RuntimeNumericsVector3Carrier v, RuntimeNumericsVector3Carrier n) { return Vector3Subtract(v, Vector3MultiplyScalar(n, 2.0f * Vector3Dot(v, n))); }
RuntimeNumericsVector3Carrier Vector3Cross(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return MakeVector3((l.y * r.z) - (l.z * r.y), (l.z * r.x) - (l.x * r.z), (l.x * r.y) - (l.y * r.x)); }
RuntimeNumericsVector3Carrier Vector3Negate(RuntimeNumericsVector3Carrier v) { return MakeVector3(-v.x, -v.y, -v.z); }
bool Vector3Equals(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return l.x == r.x && l.y == r.y && l.z == r.z; }
CHAOS_IL2CPP_INT32 Vector3GreaterThanAll(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return (l.x > r.x) && (l.y > r.y) && (l.z > r.z) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector3GreaterThanOrEqualAll(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return (l.x >= r.x) && (l.y >= r.y) && (l.z >= r.z) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector3LessThanAll(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return (l.x < r.x) && (l.y < r.y) && (l.z < r.z) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector3LessThanOrEqualAll(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return (l.x <= r.x) && (l.y <= r.y) && (l.z <= r.z) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector3EqualsAll(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return (l.x == r.x) && (l.y == r.y) && (l.z == r.z) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector3EqualsAny(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return (l.x == r.x) || (l.y == r.y) || (l.z == r.z) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector3GetHashCode(RuntimeNumericsVector3Carrier v) { return HashCodeCombine3(SingleGetHashCodeValue(v.x), SingleGetHashCodeValue(v.y), SingleGetHashCodeValue(v.z)); }
float Vector3Length(RuntimeNumericsVector3Carrier v) { return CHAOS_IL2CPP_SQRT(Vector3LengthSquared(v)); }
float Vector3Dot(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return (l.x * r.x) + (l.y * r.y) + (l.z * r.z); }
float Vector3LengthSquared(RuntimeNumericsVector3Carrier v) { return Vector3Dot(v, v); }
float Vector3DistanceSquared(RuntimeNumericsVector3Carrier l, RuntimeNumericsVector3Carrier r) { return Vector3LengthSquared(Vector3Subtract(l, r)); }

float Vector3GetElement(RuntimeNumericsVector3Carrier v, CHAOS_IL2CPP_INT32 i) {
    switch (i) { case 0: return v.x; case 1: return v.y; case 2: return v.z; default: return 0.0f; }
}

RuntimeNumericsVector3Carrier Vector3Transform(RuntimeNumericsVector3Carrier v, RuntimeNumericsMatrix4x4Carrier m) { return TransformVector3ByMatrix4x4(v, m, true); }
RuntimeNumericsVector3Carrier Vector3Transform(RuntimeNumericsVector3Carrier v, RuntimeNumericsQuaternionCarrier q) { return RotateVector3ByQuaternion(v, q); }
RuntimeNumericsVector3Carrier Vector3TransformNormal(RuntimeNumericsVector3Carrier v, RuntimeNumericsMatrix4x4Carrier m) { return TransformVector3ByMatrix4x4(v, m, false); }
bool Vector3TryCopyTo(RuntimeNumericsVector3Carrier v, float* d, CHAOS_IL2CPP_SIZE dl, CHAOS_IL2CPP_SIZE si) { return VectorTryCopyToRaw(v, d, dl, si); }

// Vector4
RuntimeNumericsVector4Carrier Vector4Zero() { return MakeVector4(0.0f, 0.0f, 0.0f, 0.0f); }
RuntimeNumericsVector4Carrier Vector4One() { return MakeVector4(1.0f, 1.0f, 1.0f, 1.0f); }
RuntimeNumericsVector4Carrier Vector4UnitX() { return MakeVector4(1.0f, 0.0f, 0.0f, 0.0f); }
RuntimeNumericsVector4Carrier Vector4UnitY() { return MakeVector4(0.0f, 1.0f, 0.0f, 0.0f); }
RuntimeNumericsVector4Carrier Vector4UnitZ() { return MakeVector4(0.0f, 0.0f, 1.0f, 0.0f); }
RuntimeNumericsVector4Carrier Vector4UnitW() { return MakeVector4(0.0f, 0.0f, 0.0f, 1.0f); }

RuntimeNumericsVector4Carrier Vector4Add(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return MakeVector4(l.x + r.x, l.y + r.y, l.z + r.z, l.w + r.w); }
RuntimeNumericsVector4Carrier Vector4Subtract(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return MakeVector4(l.x - r.x, l.y - r.y, l.z - r.z, l.w - r.w); }
RuntimeNumericsVector4Carrier Vector4Multiply(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return MakeVector4(l.x * r.x, l.y * r.y, l.z * r.z, l.w * r.w); }
RuntimeNumericsVector4Carrier Vector4MultiplyScalar(RuntimeNumericsVector4Carrier v, float s) { return MakeVector4(v.x * s, v.y * s, v.z * s, v.w * s); }
RuntimeNumericsVector4Carrier Vector4Divide(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return MakeVector4(l.x / r.x, l.y / r.y, l.z / r.z, l.w / r.w); }
RuntimeNumericsVector4Carrier Vector4DivideScalar(RuntimeNumericsVector4Carrier v, float s) { return MakeVector4(v.x / s, v.y / s, v.z / s, v.w / s); }
RuntimeNumericsVector4Carrier Vector4Abs(RuntimeNumericsVector4Carrier v) { return MakeVector4(CHAOS_IL2CPP_FABS(v.x), CHAOS_IL2CPP_FABS(v.y), CHAOS_IL2CPP_FABS(v.z), CHAOS_IL2CPP_FABS(v.w)); }
RuntimeNumericsVector4Carrier Vector4Min(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return MakeVector4(VectorMinElement(l.x, r.x), VectorMinElement(l.y, r.y), VectorMinElement(l.z, r.z), VectorMinElement(l.w, r.w)); }
RuntimeNumericsVector4Carrier Vector4Max(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return MakeVector4(VectorMaxElement(l.x, r.x), VectorMaxElement(l.y, r.y), VectorMaxElement(l.z, r.z), VectorMaxElement(l.w, r.w)); }
RuntimeNumericsVector4Carrier Vector4Clamp(RuntimeNumericsVector4Carrier v, RuntimeNumericsVector4Carrier mn, RuntimeNumericsVector4Carrier mx) { return Vector4Min(Vector4Max(v, mn), mx); }
RuntimeNumericsVector4Carrier Vector4SquareRoot(RuntimeNumericsVector4Carrier v) { return MakeVector4(CHAOS_IL2CPP_SQRT(v.x), CHAOS_IL2CPP_SQRT(v.y), CHAOS_IL2CPP_SQRT(v.z), CHAOS_IL2CPP_SQRT(v.w)); }
RuntimeNumericsVector4Carrier Vector4Normalize(RuntimeNumericsVector4Carrier v) { return Vector4DivideScalar(v, Vector4Length(v)); }
RuntimeNumericsVector4Carrier Vector4Lerp(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r, float a) { return Vector4Add(l, Vector4MultiplyScalar(Vector4Subtract(r, l), a)); }
RuntimeNumericsVector4Carrier Vector4Negate(RuntimeNumericsVector4Carrier v) { return MakeVector4(-v.x, -v.y, -v.z, -v.w); }
bool Vector4Equals(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return l.x == r.x && l.y == r.y && l.z == r.z && l.w == r.w; }
CHAOS_IL2CPP_INT32 Vector4GreaterThanAll(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return (l.x > r.x) && (l.y > r.y) && (l.z > r.z) && (l.w > r.w) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector4GreaterThanOrEqualAll(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return (l.x >= r.x) && (l.y >= r.y) && (l.z >= r.z) && (l.w >= r.w) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector4LessThanAll(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return (l.x < r.x) && (l.y < r.y) && (l.z < r.z) && (l.w < r.w) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector4LessThanOrEqualAll(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return (l.x <= r.x) && (l.y <= r.y) && (l.z <= r.z) && (l.w <= r.w) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector4EqualsAll(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return (l.x == r.x) && (l.y == r.y) && (l.z == r.z) && (l.w == r.w) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector4EqualsAny(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return (l.x == r.x) || (l.y == r.y) || (l.z == r.z) || (l.w == r.w) ? 1 : 0; }
CHAOS_IL2CPP_INT32 Vector4GetHashCode(RuntimeNumericsVector4Carrier v) { return HashCodeCombine4(SingleGetHashCodeValue(v.x), SingleGetHashCodeValue(v.y), SingleGetHashCodeValue(v.z), SingleGetHashCodeValue(v.w)); }
float Vector4Length(RuntimeNumericsVector4Carrier v) { return CHAOS_IL2CPP_SQRT(Vector4LengthSquared(v)); }
float Vector4Dot(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return (l.x * r.x) + (l.y * r.y) + (l.z * r.z) + (l.w * r.w); }
float Vector4LengthSquared(RuntimeNumericsVector4Carrier v) { return Vector4Dot(v, v); }
float Vector4DistanceSquared(RuntimeNumericsVector4Carrier l, RuntimeNumericsVector4Carrier r) { return Vector4LengthSquared(Vector4Subtract(l, r)); }

float Vector4GetElement(RuntimeNumericsVector4Carrier v, CHAOS_IL2CPP_INT32 i) {
    switch (i) { case 0: return v.x; case 1: return v.y; case 2: return v.z; case 3: return v.w; default: return 0.0f; }
}

RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector2Carrier v, RuntimeNumericsMatrix4x4Carrier m) { return TransformVector2ByMatrix4x4(v, m); }
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector2Carrier v, RuntimeNumericsQuaternionCarrier q) { auto t = RotateVector3ByQuaternion(MakeVector3(v.x, v.y, 0.0f), q); return MakeVector4(t.x, t.y, t.z, 1.0f); }
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector3Carrier v, RuntimeNumericsMatrix4x4Carrier m) { return MakeVector4((v.x*m.m11)+(v.y*m.m21)+(v.z*m.m31)+m.m41, (v.x*m.m12)+(v.y*m.m22)+(v.z*m.m32)+m.m42, (v.x*m.m13)+(v.y*m.m23)+(v.z*m.m33)+m.m43, (v.x*m.m14)+(v.y*m.m24)+(v.z*m.m34)+m.m44); }
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector3Carrier v, RuntimeNumericsQuaternionCarrier q) { auto t = RotateVector3ByQuaternion(v, q); return MakeVector4(t.x, t.y, t.z, 1.0f); }
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector4Carrier v, RuntimeNumericsMatrix4x4Carrier m) { return MakeVector4((v.x*m.m11)+(v.y*m.m21)+(v.z*m.m31)+(v.w*m.m41), (v.x*m.m12)+(v.y*m.m22)+(v.z*m.m32)+(v.w*m.m42), (v.x*m.m13)+(v.y*m.m23)+(v.z*m.m33)+(v.w*m.m43), (v.x*m.m14)+(v.y*m.m24)+(v.z*m.m34)+(v.w*m.m44)); }
RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector4Carrier v, RuntimeNumericsQuaternionCarrier q) { auto t = RotateVector3ByQuaternion(MakeVector3(v.x, v.y, v.z), q); return MakeVector4(t.x, t.y, t.z, v.w); }
bool Vector4TryCopyTo(RuntimeNumericsVector4Carrier v, float* d, CHAOS_IL2CPP_SIZE dl, CHAOS_IL2CPP_SIZE si) { return VectorTryCopyToRaw(v, d, dl, si); }

// Vector64/128/256/512 — declared in runtime_core.h, require external linkage.

RuntimeIntrinsicVector64Carrier Vector64Reinterpret(RuntimeIntrinsicVector64Carrier v) { return v; }
RuntimeIntrinsicVector128Carrier Vector128Reinterpret(RuntimeIntrinsicVector128Carrier v) { return v; }
RuntimeIntrinsicVector256Carrier Vector256Reinterpret(RuntimeIntrinsicVector256Carrier v) { return v; }
RuntimeIntrinsicVector512Carrier Vector512Reinterpret(RuntimeIntrinsicVector512Carrier v) { return v; }

CHAOS_IL2CPP_INT32 Vector64Count() { return 8 / 4; }
CHAOS_IL2CPP_INT32 Vector128Count() { return 16 / 4; }
CHAOS_IL2CPP_INT32 Vector256Count() { return 32 / 4; }
CHAOS_IL2CPP_INT32 Vector512Count() { return 64 / 4; }

CHAOS_IL2CPP_INT32 VectorPlatformByteWidth() { return 32; }

bool Vector64IsSupported() { return true; }
bool Vector128IsSupported() { return true; }
bool Vector256IsSupported() { return true; }
bool Vector512IsSupported() { return true; }

}  // namespace chaos::il2cpp::runtime_core