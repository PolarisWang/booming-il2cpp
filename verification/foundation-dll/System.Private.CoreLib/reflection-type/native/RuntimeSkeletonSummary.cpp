// family-runtime-skeleton
// native-reference runtime skeleton for family/System.Private.CoreLib/reflection/type
#include "bootstrap.h"
#include "codegen_bridge.h"
#include "memory_domain.h"
#include <chaos/native_types.h>
#include <chaos/native_types.h>
#include <cstring>

namespace chaos::il2cpp::generated
{
namespace runtime_skeleton_reflection_type
{{
constexpr const char* kAssemblyName = "System.Private.CoreLib";
constexpr const char* kPlanKind = "family-runtime-skeleton";
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr CHAOS_IL2CPP_SIZE translation_unit_method_count = 43;
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_size = 43;
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_count = 1;

using NativeReferenceAssemblyPageDispatchFn = CHAOS_IL2CPP_INT32 (CHAOS_RUNTIME_ABI_CALL*)(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    CHAOS_IL2CPP_UINT32 method_slot,
    void* managed_args);

struct NativeReferenceAssemblyDispatchRequest
{
    const char* subject_id;
    void* managed_args;
    CHAOS_IL2CPP_UINT32 method_id;
};

struct RuntimeSkeletonMethodDispatchCatalogEntry
{
    CHAOS_IL2CPP_UINT32 method_id;
    const char* subject_id;
    NativeReferenceAssemblyPageDispatchFn dispatch;
    CHAOS_IL2CPP_UINT32 method_slot;
};

// Page dispatch declarations
int32_t CHAOS_RUNTIME_ABI_CALL DispatchRuntimeSkeletonPage0001(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    CHAOS_IL2CPP_UINT32 method_slot,
    void* managed_args);

constexpr RuntimeSkeletonMethodDispatchCatalogEntry kMethodDispatchCatalog[] =
{
    { 0, "System.Private.CoreLib/System.Type::GetType:System.Type(System.String)", DispatchRuntimeSkeletonPage0001, 0 },
    { 1, "System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean)", DispatchRuntimeSkeletonPage0001, 1 },
    { 2, "System.Private.CoreLib/System.Type::GetType:System.Type(System.String,System.Boolean,System.Boolean)", DispatchRuntimeSkeletonPage0001, 2 },
    { 3, "System.Private.CoreLib/System.Type::GetTypeFromHandle:System.Type(System.RuntimeTypeHandle)", DispatchRuntimeSkeletonPage0001, 3 },
    { 4, "System.Private.CoreLib/System.Type::get_Name:System.String()", DispatchRuntimeSkeletonPage0001, 4 },
    { 5, "System.Private.CoreLib/System.Type::get_FullName:System.String()", DispatchRuntimeSkeletonPage0001, 5 },
    { 6, "System.Private.CoreLib/System.Type::get_Namespace:System.String()", DispatchRuntimeSkeletonPage0001, 6 },
    { 7, "System.Private.CoreLib/System.Type::get_AssemblyQualifiedName:System.String()", DispatchRuntimeSkeletonPage0001, 7 },
    { 8, "System.Private.CoreLib/System.Type::get_TypeHandle:System.RuntimeTypeHandle()", DispatchRuntimeSkeletonPage0001, 8 },
    { 9, "System.Private.CoreLib/System.Type::get_IsValueType:System.Boolean()", DispatchRuntimeSkeletonPage0001, 9 },
    { 10, "System.Private.CoreLib/System.Type::get_IsEnum:System.Boolean()", DispatchRuntimeSkeletonPage0001, 10 },
    { 11, "System.Private.CoreLib/System.Type::get_IsAbstract:System.Boolean()", DispatchRuntimeSkeletonPage0001, 11 },
    { 12, "System.Private.CoreLib/System.Type::get_IsSealed:System.Boolean()", DispatchRuntimeSkeletonPage0001, 12 },
    { 13, "System.Private.CoreLib/System.Type::get_IsInterface:System.Boolean()", DispatchRuntimeSkeletonPage0001, 13 },
    { 14, "System.Private.CoreLib/System.Type::get_IsArray:System.Boolean()", DispatchRuntimeSkeletonPage0001, 14 },
    { 15, "System.Private.CoreLib/System.Type::get_IsByRef:System.Boolean()", DispatchRuntimeSkeletonPage0001, 15 },
    { 16, "System.Private.CoreLib/System.Type::get_IsPointer:System.Boolean()", DispatchRuntimeSkeletonPage0001, 16 },
    { 17, "System.Private.CoreLib/System.Type::get_IsGenericType:System.Boolean()", DispatchRuntimeSkeletonPage0001, 17 },
    { 18, "System.Private.CoreLib/System.Type::get_IsConstructedGenericType:System.Boolean()", DispatchRuntimeSkeletonPage0001, 18 },
    { 19, "System.Private.CoreLib/System.Type::get_IsNested:System.Boolean()", DispatchRuntimeSkeletonPage0001, 19 },
    { 20, "System.Private.CoreLib/System.Type::get_IsPublic:System.Boolean()", DispatchRuntimeSkeletonPage0001, 20 },
    { 21, "System.Private.CoreLib/System.Type::get_IsNotPublic:System.Boolean()", DispatchRuntimeSkeletonPage0001, 21 },
    { 22, "System.Private.CoreLib/System.Type::get_IsNestedPublic:System.Boolean()", DispatchRuntimeSkeletonPage0001, 22 },
    { 23, "System.Private.CoreLib/System.Type::get_IsNestedPrivate:System.Boolean()", DispatchRuntimeSkeletonPage0001, 23 },
    { 24, "System.Private.CoreLib/System.Type::get_BaseType:System.Type()", DispatchRuntimeSkeletonPage0001, 24 },
    { 25, "System.Private.CoreLib/System.Type::get_DeclaringType:System.Type()", DispatchRuntimeSkeletonPage0001, 25 },
    { 26, "System.Private.CoreLib/System.Type::get_ReflectedType:System.Type()", DispatchRuntimeSkeletonPage0001, 26 },
    { 27, "System.Private.CoreLib/System.Type::get_UnderlyingSystemType:System.Type()", DispatchRuntimeSkeletonPage0001, 27 },
    { 28, "System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[]()", DispatchRuntimeSkeletonPage0001, 28 },
    { 29, "System.Private.CoreLib/System.Type::GetMethods:System.Reflection.MethodInfo[](System.Reflection.BindingFlags)", DispatchRuntimeSkeletonPage0001, 29 },
    { 30, "System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[]()", DispatchRuntimeSkeletonPage0001, 30 },
    { 31, "System.Private.CoreLib/System.Type::GetFields:System.Reflection.FieldInfo[](System.Reflection.BindingFlags)", DispatchRuntimeSkeletonPage0001, 31 },
    { 32, "System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[]()", DispatchRuntimeSkeletonPage0001, 32 },
    { 33, "System.Private.CoreLib/System.Type::GetProperties:System.Reflection.PropertyInfo[](System.Reflection.BindingFlags)", DispatchRuntimeSkeletonPage0001, 33 },
    { 34, "System.Private.CoreLib/System.Type::GetEvents:System.Reflection.EventInfo[]()", DispatchRuntimeSkeletonPage0001, 34 },
    { 35, "System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[]()", DispatchRuntimeSkeletonPage0001, 35 },
    { 36, "System.Private.CoreLib/System.Type::GetConstructors:System.Reflection.ConstructorInfo[](System.Reflection.BindingFlags)", DispatchRuntimeSkeletonPage0001, 36 },
    { 37, "System.Private.CoreLib/System.Type::GetMembers:System.Reflection.MemberInfo[]()", DispatchRuntimeSkeletonPage0001, 37 },
    { 38, "System.Private.CoreLib/System.Type::GetInterfaces:System.Type[]()", DispatchRuntimeSkeletonPage0001, 38 },
    { 39, "System.Private.CoreLib/System.Type::GetElementType:System.Type()", DispatchRuntimeSkeletonPage0001, 39 },
    { 40, "System.Private.CoreLib/System.Type::GetNestedTypes:System.Type[]()", DispatchRuntimeSkeletonPage0001, 40 },
    { 41, "System.Private.CoreLib/System.Type::GetNestedType:System.Type(System.String)", DispatchRuntimeSkeletonPage0001, 41 },
    { 42, "System.Private.CoreLib/System.Type::get_Assembly:System.Reflection.Assembly()", DispatchRuntimeSkeletonPage0001, 42 },
};
constexpr CHAOS_IL2CPP_SIZE kMethodDispatchCatalogCount = sizeof(kMethodDispatchCatalog) / sizeof(kMethodDispatchCatalog[0]);

const RuntimeSkeletonMethodDispatchCatalogEntry* FindMethodDispatchCatalogEntry(const char* subject_id)
{
    if (subject_id == nullptr || subject_id[0] == '\0') { return nullptr; }
    if (kMethodDispatchCatalogCount == 0) { return nullptr; }
    CHAOS_IL2CPP_SIZE left = 0;
    CHAOS_IL2CPP_SIZE right = kMethodDispatchCatalogCount;
    while (left < right) {
        const CHAOS_IL2CPP_SIZE mid = left + ((right - left) / 2);
        const auto& entry = kMethodDispatchCatalog[mid];
        const int compare = CHAOS_IL2CPP_STRCMP(entry.subject_id, subject_id);
        if (compare < 0) { left = mid + 1; continue; }
        if (compare > 0) { right = mid; continue; }
        return &entry;
    }
    return nullptr;
}

constexpr RuntimeSkeletonMethodDispatchCatalogEntry* FindMethodDispatchCatalogEntryByMethodId(CHAOS_IL2CPP_UINT32 method_id)
{
    if (method_id == 0) { return nullptr; }
    if (kMethodDispatchCatalogCount == 0) { return nullptr; }
    CHAOS_IL2CPP_SIZE left = 0;
    CHAOS_IL2CPP_SIZE right = kMethodDispatchCatalogCount;
    while (left < right) {
        const CHAOS_IL2CPP_SIZE mid = left + ((right - left) / 2);
        const auto& entry = kMethodDispatchCatalog[mid];
        if (entry.method_id < method_id) { left = mid + 1; continue; }
        if (entry.method_id > method_id) { right = mid; continue; }
        return &entry;
    }
    return nullptr;
}

int32_t DispatchAssemblySubject(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    const char* subject_id,
    void* managed_args)
{
    if (subject_id == nullptr || subject_id[0] == '\0') { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }
    const auto* method = FindMethodDispatchCatalogEntry(subject_id);
    if (method == nullptr) { return CHAOS_BRIDGE_STATUS_NOT_FOUND; }
    return method->dispatch(bridge, code_registration, metadata_registration, options, runtime, thread, method->method_slot, managed_args);
}

int32_t DispatchAssemblyMethodId(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    CHAOS_IL2CPP_UINT32 method_id,
    void* managed_args)
{
    const auto* method = FindMethodDispatchCatalogEntryByMethodId(method_id);
    if (method == nullptr) { return CHAOS_BRIDGE_STATUS_NOT_FOUND; }
    return method->dispatch(bridge, code_registration, metadata_registration, options, runtime, thread, method->method_slot, managed_args);
}

}  // namespace runtime_skeleton_reflection_type

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReferenceAssembly_reflection_type(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args) {
    (void)managed_args;
    if (bridge == nullptr || code_registration == nullptr || metadata_registration == nullptr || options == nullptr || runtime == nullptr || thread == nullptr) { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }

    namespace md = chaos::il2cpp::memory_domain;
    auto* domain = md::FindDomainByName(runtime_skeleton_reflection_type::kAssemblyName);
    if (domain == nullptr) { domain = md::FindDomainById(chaos::il2cpp::bootstrap::GetAotDomainId()); }
    md::DomainScope domain_scope(domain);

    CodeRegistrationV0 local_code_registration = *code_registration;
    const BridgeStatus register_status = bridge->register_codegen(&local_code_registration, metadata_registration, options);
    if (register_status != CHAOS_BRIDGE_STATUS_OK) { return register_status; }
    const BridgeStatus bootstrap_status = bridge->bootstrap_runtime();
    if (bootstrap_status != CHAOS_BRIDGE_STATUS_OK) { return bootstrap_status; }
    if (managed_args == nullptr) { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }
    const auto* request = static_cast<const runtime_skeleton_reflection_type::NativeReferenceAssemblyDispatchRequest*>(managed_args);
    if (request->method_id != 0) {
        return runtime_skeleton_reflection_type::DispatchAssemblyMethodId(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->method_id, request->managed_args);
    }
    return runtime_skeleton_reflection_type::DispatchAssemblySubject(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->subject_id, request->managed_args);
}

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReference_reflection_type(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args) {
    return RunNativeReferenceAssembly_reflection_type(bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);
}
}  // namespace chaos::il2cpp::generated
