// reflection_stubs.cpp — Reflection deferred stub implementations
#include <chaos/native_types.h>
#include "generated_code_compat.h"
#include "reflection_query_model.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR ChaosReflectionIsDefined(CHAOS_IL2CPP_INTPTR /*assembly*/, CHAOS_IL2CPP_INTPTR /*type*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRequiredCustomModifiers(CHAOS_IL2CPP_INTPTR /*param*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionHasDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept
{
    if (param == 0) return 0;
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(param));
    return (p->default_value_blob != 0 && p->default_value_size > 0) ? 1 : 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept
{
    if (param == 0) return 0;
    auto* p = reinterpret_cast<const ReflectionQueryParameterDescriptor*>(
        static_cast<CHAOS_IL2CPP_INTPTR>(param));
    return p->default_value_blob;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetRawDefaultValue(CHAOS_IL2CPP_INTPTR param) noexcept
{
    return ChaosReflectionGetDefaultValue(param);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetIsVirtual(CHAOS_IL2CPP_INTPTR /*member*/) noexcept
{
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionGetBaseDefinition(CHAOS_IL2CPP_INTPTR member_handle) noexcept
{
    return member_handle;
}

CHAOS_IL2CPP_INTPTR ChaosRuntimeTypeFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    (void)handle;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetTypeFromHandle(CHAOS_IL2CPP_INTPTR handle) noexcept
{
    (void)handle;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeEquals(CHAOS_IL2CPP_INTPTR type_a, CHAOS_IL2CPP_INTPTR type_b) noexcept
{
    (void)type_a; (void)type_b;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetTypeInfo(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

// Type stub stubs (SimpleForward replacements for GenericShapeDescriptors)
CHAOS_IL2CPP_INTPTR ChaosTypeGetFieldBindingFlags(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INTPTR bindingFlags) noexcept
{
    (void)type; (void)name; (void)bindingFlags;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetEvent(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name) noexcept
{
    (void)type; (void)name;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetProperty(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name) noexcept
{
    (void)type; (void)name;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsByRef(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeMakeArrayType(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeMakeByRefType(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsPointer(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsNested(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsNestedPrivate(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsNestedPublic(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsNotPublic(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosTypeGetIsPublic(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetUnderlyingSystemType(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetElementType(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetEvents(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetInterfaces(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetNestedTypes(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeMakePointerType(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetNestedType(CHAOS_IL2CPP_INTPTR type, CHAOS_IL2CPP_INTPTR name) noexcept
{
    (void)type; (void)name;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetMembers(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosTypeGetProperties(CHAOS_IL2CPP_INTPTR type) noexcept
{
    (void)type;
    return 0;
}

// ── Assembly stubs ──
CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetExportedTypes(CHAOS_IL2CPP_INTPTR assembly) noexcept
{
    (void)assembly;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetForwardedTypes(CHAOS_IL2CPP_INTPTR assembly) noexcept
{
    (void)assembly;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetManifestResourceNames(CHAOS_IL2CPP_INTPTR assembly) noexcept
{
    (void)assembly;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetModules(CHAOS_IL2CPP_INTPTR assembly) noexcept
{
    (void)assembly;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyGetEntryPoint(CHAOS_IL2CPP_INTPTR assembly) noexcept
{
    (void)assembly;
    return 0;
}

// ── ParameterInfo stubs ──
CHAOS_IL2CPP_INTPTR ChaosReflectionGetOptionalCustomModifiers(CHAOS_IL2CPP_INTPTR param) noexcept
{
    (void)param;
    return 0;
}

// ── FieldInfo stubs ──
CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsInitOnly(CHAOS_IL2CPP_INTPTR field) noexcept
{
    (void)field;
    return 0;
}

CHAOS_IL2CPP_INT64 ChaosReflectionFieldGetFieldHandle(CHAOS_IL2CPP_INTPTR field) noexcept
{
    (void)field;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionFieldGetIsLiteral(CHAOS_IL2CPP_INTPTR field) noexcept
{
    (void)field;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionPropertyGetIndexParameters(CHAOS_IL2CPP_INTPTR prop) noexcept
{
    (void)prop;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetCanRead(CHAOS_IL2CPP_INTPTR prop) noexcept
{
    (void)prop;
    return 0;
}

CHAOS_IL2CPP_INT32 ChaosReflectionPropertyGetCanWrite(CHAOS_IL2CPP_INTPTR prop) noexcept
{
    (void)prop;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetCultureInfo(CHAOS_IL2CPP_INTPTR name) noexcept
{
    (void)name;
    return 0;
}

CHAOS_IL2CPP_INTPTR ChaosReflectionAssemblyNameGetVersion(CHAOS_IL2CPP_INTPTR name) noexcept
{
    (void)name;
    return 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core