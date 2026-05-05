// runtime_stubs.cpp — Pre-existing runtime helper stubs
//
// These functions are referenced by generated C++ code but have no real
// runtime implementation yet. All stubs return 0 / nullptr / no-op so
// that native compilation succeeds. Replace with real implementations
// as the corresponding managed features are brought online.
//
// See generated_code_compat.h for the extern "C" declarations.

#include <chaos/native_types.h>
#include <chaos/trace.h>
#include <cstring>

#include "runtime_instantiation.h"
#include "runtime_abi.h"

namespace chaos::il2cpp::runtime_core {
extern "C"
{

// ── Array operations ───────────────────────────────────────────────

void ChaosArrayClear(CHAOS_IL2CPP_INTPTR /*array*/, CHAOS_IL2CPP_INT32 /*index*/, CHAOS_IL2CPP_INT32 /*count*/) noexcept
{
}

CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR /*array*/, CHAOS_IL2CPP_INT32 /*dimension*/) noexcept
{
    return 0;
}

// ── Type marshalling helpers ───────────────────────────────────────
// The eval stack stores values as CHAOS_IL2CPP_INTPTR. These helpers
// pack/unpack 64-bit integer and double values. On x64, CHAOS_IL2CPP_INTPTR
// is already 64 bits so store/load_int64 are identity operations.

CHAOS_IL2CPP_INTPTR ChaosStoreInt64(CHAOS_IL2CPP_INT64 value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INTPTR>(value);
}

CHAOS_IL2CPP_INTPTR ChaosStoreFloat64(CHAOS_IL2CPP_FLOAT64 value) noexcept
{
    CHAOS_IL2CPP_INT64 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}

CHAOS_IL2CPP_INT64 ChaosLoadInt64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return static_cast<CHAOS_IL2CPP_INT64>(value);
}

CHAOS_IL2CPP_FLOAT64 ChaosLoadFloat64(CHAOS_IL2CPP_INTPTR value) noexcept
{
    CHAOS_IL2CPP_INT64 bits = static_cast<CHAOS_IL2CPP_INT64>(value);
    CHAOS_IL2CPP_FLOAT64 result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

// ─── Buffer operations ─────────────────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR /*array*/) noexcept
{
    return 0;
}

// ─── DateTime operations ──────────────────────────────────────────

CHAOS_IL2CPP_INT64 ChaosDatetimeGetUtcNow(void) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosDatetimeGetHashCode(CHAOS_IL2CPP_INTPTR /*datetime*/) noexcept
{
    return 0;
}

// ─── Math operations ──────────────────────────────────────────────

CHAOS_IL2CPP_INT64 ChaosMathSqrt(CHAOS_IL2CPP_INT64 value) noexcept
{
    double d;
    CHAOS_IL2CPP_MEMCPY(&d, &value, sizeof(d));
    d = CHAOS_IL2CPP_SQRT(d);
    CHAOS_IL2CPP_INT64 result;
    CHAOS_IL2CPP_MEMCPY(&result, &d, sizeof(result));
    return result;
}

// ─── Interlocked / threading ──────────────────────────────────────

void ChaosInterlockedMemoryBarrier(void) noexcept
{
}

// ─── Exception helpers ────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosExceptionGetBaseException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    return exc;
}

CHAOS_IL2CPP_INTPTR ChaosExceptionGetInnerException(CHAOS_IL2CPP_INTPTR exc) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosExceptionGetHresult(CHAOS_IL2CPP_INTPTR /*exc*/) noexcept
{
    return 0;
}

// ─── Object helpers ───────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosObjectEqualsStatic(CHAOS_IL2CPP_INTPTR /*left*/, CHAOS_IL2CPP_INTPTR /*right*/) noexcept
{
    return 0;
}

void ChaosObjectCtor(CHAOS_IL2CPP_INTPTR /*obj*/) noexcept
{
}

// ─── GUID / Random ────────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosGuidNewGuid(void) noexcept
{
    return 0;
}

void ChaosRandomNextBytes(CHAOS_IL2CPP_INTPTR /*rng*/, CHAOS_IL2CPP_INTPTR /*buffer*/) noexcept
{
}

CHAOS_IL2CPP_INT64 ChaosRandomNextDouble(CHAOS_IL2CPP_INTPTR /*rng*/) noexcept
{
    return 0;
}

// ─── Culture helpers ──────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosCultureGetCurrent(void) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetInvariant(void) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetCompareInfo(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetDateTimeFormat(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetDisplayName(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetName(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosCultureGetNumberFormat(CHAOS_IL2CPP_INTPTR /*culture*/) noexcept
{
    return 0;
}

// ─── String helpers ───────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosStringContains(CHAOS_IL2CPP_INTPTR /*str*/, CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosStringJoinSs(CHAOS_IL2CPP_INTPTR /*separator*/, CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosStringStartsWith(CHAOS_IL2CPP_INTPTR /*str*/, CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    return 0;
}

// ─── Reflection helpers (deferred — needs ECMA metadata) ──────────

CHAOS_IL2CPP_INTPTR ChaosReflectionIsDefined(CHAOS_IL2CPP_INTPTR /*assembly*/, CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDefaultValue(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionHasDefaultValue(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRawDefaultValue(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

// ─── Reflection deferred stubs ──────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericParamConstraints(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsVirtual(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseDefinition(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

// ─── Runtime helpers ──────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosFormattablestringFactoryCreate(CHAOS_IL2CPP_INTPTR /*format*/, CHAOS_IL2CPP_INTPTR /*args*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersEquals(CHAOS_IL2CPP_INTPTR /*left*/, CHAOS_IL2CPP_INTPTR /*right*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosRuntimeHelpersGetHashCode(CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeHelpersGetObjectValue(CHAOS_IL2CPP_INTPTR /*value*/) noexcept
{
    return 0;
}

// ─── Runtime-wrapped exception ────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosRuntimewrappedGetWrappedException(CHAOS_IL2CPP_INTPTR /*exc*/) noexcept
{
    return 0;
}

// ─── Float32 marshalling ───────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosStoreFloat32(CHAOS_IL2CPP_FLOAT32 value) noexcept
{
    CHAOS_IL2CPP_INT32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}

// ─── Volatile operations ──────────────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosVolatileRead(CHAOS_IL2CPP_INTPTR ptr) noexcept
{
    return *reinterpret_cast<volatile CHAOS_IL2CPP_INT32*>(ptr);
}

// -- Generic registration callback (defined by generated code via static init) --
extern "C" void (*g_chaos_populate_generic_registration)(void) = nullptr;

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
