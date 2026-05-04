// runtime_stubs.cpp — Pre-existing runtime helper stubs
//
// These functions are referenced by generated C++ code but have no real
// runtime implementation yet. All stubs return 0 / nullptr / no-op so
// that native compilation succeeds. Replace with real implementations
// as the corresponding managed features are brought online.
//
// See generated_code_compat.h for the extern "C" declarations.

#include <chaos/common.h>
#include <cstring>

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

// ─── Reflection helpers ───────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionIsDefined(CHAOS_IL2CPP_INTPTR /*assembly*/, CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionIsSubclassOf(CHAOS_IL2CPP_INTPTR /*type*/, CHAOS_IL2CPP_INTPTR /*base*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsGenericType(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsValueType(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetNamespace(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetReflectedType(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionGetCallingConvention(CHAOS_IL2CPP_INTPTR /*method*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsPublic(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

// ─── Module reflection ────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetType(CHAOS_IL2CPP_INTPTR /*module*/, CHAOS_IL2CPP_INTPTR /*name*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionModuleGetTypes(CHAOS_IL2CPP_INTPTR /*module*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleAssembly(CHAOS_IL2CPP_INTPTR /*module*/) noexcept
{
    return 0;
}

// ─── Parameter reflection ─────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionGetParamAttributes(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDefaultValue(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
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

// ─── Method handle operations ─────────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosRuntimemethodhandleGetHashCode(CHAOS_IL2CPP_INT64 /*handle*/) noexcept
{
    return 0;
}

// ─── Runtime-wrapped exception ────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosRuntimewrappedGetWrappedException(CHAOS_IL2CPP_INTPTR /*exc*/) noexcept
{
    return 0;
}

// ─── Reflection assignable ─────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionIsAssignableFrom(CHAOS_IL2CPP_INTPTR /*target*/, CHAOS_IL2CPP_INTPTR /*source*/) noexcept
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

// ─── Reflection generic ────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericParamConstraints(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

// ─── Reflection member info ────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsVirtual(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseDefinition(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

// ─── Reflection field handle ───────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosRuntimefieldhandleGetHashCode(CHAOS_IL2CPP_INTPTR /*handle*/) noexcept
{
    return 0;
}

// ─── Reflection module ─────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleName(CHAOS_IL2CPP_INTPTR /*module*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetModuleNameOnly(CHAOS_IL2CPP_INTPTR /*module*/) noexcept
{
    return 0;
}

// ─── Reflection parameter helpers ──────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionHasDefaultValue(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetParameterType(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetParamPosition(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

// ─── Reflection type handle ────────────────────────────────────

CHAOS_IL2CPP_INT32 ChaosRuntimetypehandleGetHashCode(CHAOS_IL2CPP_INTPTR /*handle*/) noexcept
{
    return 0;
}

// ─── Reflection constructors ───────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionGetConstructorsDefault(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}


// ─── Reflection assignable (supplementary) ──────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionIsAssignableTo(CHAOS_IL2CPP_INTPTR /*target*/, CHAOS_IL2CPP_INTPTR /*source*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionIsInstanceOfType(CHAOS_IL2CPP_INTPTR /*obj*/, CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

// ─── Assembly reflection ────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionGetImageRuntimeVersion(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsConstructedGeneric(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsInterface(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsArray(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyFullName(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetGenericParamPos(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsEnum(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsAbstract(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyQualifiedName(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetTypes(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetCallingAssembly(void) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetEntryAssembly(void) noexcept
{
    return 0;
}

// Reflection supplementary
CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsSealed(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsGenericTypeDef(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFromAssemblyBool(CHAOS_IL2CPP_INTPTR /*assembly*/, CHAOS_IL2CPP_INTPTR /*name*/, CHAOS_IL2CPP_INT32 /*throw_on_error*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetExecutingAssembly(void) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetAssemblyLocation(CHAOS_IL2CPP_INTPTR /*assembly*/) noexcept
{
    return 0;
}

// ─── Generic type reflection ────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionMakeGenericType(CHAOS_IL2CPP_INTPTR /*def*/, CHAOS_IL2CPP_INTPTR /*args*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetContainsGenericParams(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

// ─── Parameter default value ────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRawDefaultValue(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

// ─── Type reflection ────────────────────────────────────────────

CHAOS_IL2CPP_INTPTR ChaosReflectionGetFieldsBindingflags(CHAOS_IL2CPP_INTPTR /*type*/, CHAOS_IL2CPP_INT32 /*flags*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetMethodsBindingflags(CHAOS_IL2CPP_INTPTR /*type*/, CHAOS_IL2CPP_INT32 /*flags*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseType(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetTypeFullName(CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
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
