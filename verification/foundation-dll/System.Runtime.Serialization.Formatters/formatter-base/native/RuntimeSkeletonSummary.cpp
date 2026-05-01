// family-runtime-skeleton
// native-reference runtime skeleton for family/System.Runtime.Serialization.Formatters/formatter-base
#include "bootstrap.h"
#include "codegen_bridge.h"
#include "memory_domain.h"
#include <chaos/native_types.h>
#include <chaos/native_types.h>
#include <cstring>

namespace chaos::il2cpp::generated
{
namespace runtime_skeleton_formatter_base
{
constexpr const char* kAssemblyName = "System.Runtime.Serialization.Formatters";
constexpr const char* kPlanKind = "family-runtime-skeleton";
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr CHAOS_IL2CPP_SIZE translation_unit_method_count = 84;
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_size = 84;
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
    { 0, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Formatter::Void()", DispatchRuntimeSkeletonPage0001, 0 },
    { 1, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Deserialize:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 1 },
    { 2, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::GetNext:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 2 },
    { 3, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Schedule:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 3 },
    { 4, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::Serialize:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 4 },
    { 5, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteArray:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 5 },
    { 6, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteBoolean:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 6 },
    { 7, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteByte:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 7 },
    { 8, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteChar:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 8 },
    { 9, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteDateTime:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 9 },
    { 10, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteDecimal:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 10 },
    { 11, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteDouble:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 11 },
    { 12, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteInt16:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 12 },
    { 13, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteInt32:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 13 },
    { 14, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteInt64:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 14 },
    { 15, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteMember:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 15 },
    { 16, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteObjectRef:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 16 },
    { 17, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteSByte:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 17 },
    { 18, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteSingle:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 18 },
    { 19, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteTimeSpan:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 19 },
    { 20, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteUInt16:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 20 },
    { 21, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteUInt32:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 21 },
    { 22, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteUInt64:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 22 },
    { 23, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::WriteValueType:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 23 },
    { 24, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Binder:System.Void()", DispatchRuntimeSkeletonPage0001, 24 },
    { 25, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_Context:System.Void()", DispatchRuntimeSkeletonPage0001, 25 },
    { 26, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::get_SurrogateSelector:System.Void()", DispatchRuntimeSkeletonPage0001, 26 },
    { 27, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Binder:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 27 },
    { 28, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_Context:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 28 },
    { 29, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatter::set_SurrogateSelector:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 29 },
    { 30, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::FormatterConverter::Void()", DispatchRuntimeSkeletonPage0001, 30 },
    { 31, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 31 },
    { 32, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToBoolean:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 32 },
    { 33, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToByte:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 33 },
    { 34, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToChar:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 34 },
    { 35, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDateTime:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 35 },
    { 36, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDecimal:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 36 },
    { 37, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToDouble:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 37 },
    { 38, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt16:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 38 },
    { 39, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt32:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 39 },
    { 40, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToInt64:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 40 },
    { 41, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToSByte:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 41 },
    { 42, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToSingle:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 42 },
    { 43, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToString:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 43 },
    { 44, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt16:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 44 },
    { 45, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt32:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 45 },
    { 46, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::ToUInt64:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 46 },
    { 47, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::CheckTypeSecurity:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 47 },
    { 48, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetObjectData:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 48 },
    { 49, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSafeUninitializedObject:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 49 },
    { 50, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSerializableMembers:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 50 },
    { 51, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSerializableMembers:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 51 },
    { 52, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetSurrogateForCyclicalReference:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 52 },
    { 53, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetTypeFromAssembly:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 53 },
    { 54, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::GetUninitializedObject:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 54 },
    { 55, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterServices::PopulateObjectMembers:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 55 },
    { 56, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void()", DispatchRuntimeSkeletonPage0001, 56 },
    { 57, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void(System.Object, System.Object)", DispatchRuntimeSkeletonPage0001, 57 },
    { 58, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Deserialize:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 58 },
    { 59, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Serialize:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 59 },
    { 60, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_AssemblyFormat:System.Void()", DispatchRuntimeSkeletonPage0001, 60 },
    { 61, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Binder:System.Void()", DispatchRuntimeSkeletonPage0001, 61 },
    { 62, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Context:System.Void()", DispatchRuntimeSkeletonPage0001, 62 },
    { 63, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_FilterLevel:System.Void()", DispatchRuntimeSkeletonPage0001, 63 },
    { 64, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_SurrogateSelector:System.Void()", DispatchRuntimeSkeletonPage0001, 64 },
    { 65, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_TypeFormat:System.Void()", DispatchRuntimeSkeletonPage0001, 65 },
    { 66, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_AssemblyFormat:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 66 },
    { 67, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Binder:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 67 },
    { 68, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Context:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 68 },
    { 69, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_FilterLevel:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 69 },
    { 70, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_SurrogateSelector:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 70 },
    { 71, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_TypeFormat:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 71 },
    { 72, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldNames:System.Void()", DispatchRuntimeSkeletonPage0001, 72 },
    { 73, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldTypes:System.Void()", DispatchRuntimeSkeletonPage0001, 73 },
    { 74, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldNames:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 74 },
    { 75, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldTypes:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 75 },
    { 76, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Deserialize:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 76 },
    { 77, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::Serialize:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 77 },
    { 78, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Binder:System.Void()", DispatchRuntimeSkeletonPage0001, 78 },
    { 79, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_Context:System.Void()", DispatchRuntimeSkeletonPage0001, 79 },
    { 80, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::get_SurrogateSelector:System.Void()", DispatchRuntimeSkeletonPage0001, 80 },
    { 81, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Binder:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 81 },
    { 82, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_Context:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 82 },
    { 83, "System.Runtime.Serialization.Formatters/System.Runtime.Serialization.IFormatter::set_SurrogateSelector:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 83 },
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

}  // namespace runtime_skeleton_formatter_base

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReferenceAssembly_formatter_base(
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
    auto* domain = md::FindDomainByName(runtime_skeleton_formatter_base::kAssemblyName);
    if (domain == nullptr) { domain = md::FindDomainById(chaos::il2cpp::bootstrap::GetAotDomainId()); }
    md::DomainScope domain_scope(domain);

    CodeRegistrationV0 local_code_registration = *code_registration;
    const BridgeStatus register_status = bridge->register_codegen(&local_code_registration, metadata_registration, options);
    if (register_status != CHAOS_BRIDGE_STATUS_OK) { return register_status; }
    const BridgeStatus bootstrap_status = bridge->bootstrap_runtime();
    if (bootstrap_status != CHAOS_BRIDGE_STATUS_OK) { return bootstrap_status; }
    if (managed_args == nullptr) { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }
    const auto* request = static_cast<const runtime_skeleton_formatter_base::NativeReferenceAssemblyDispatchRequest*>(managed_args);
    if (request->method_id != 0) {
        return runtime_skeleton_formatter_base::DispatchAssemblyMethodId(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->method_id, request->managed_args);
    }
    return runtime_skeleton_formatter_base::DispatchAssemblySubject(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->subject_id, request->managed_args);
}

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReference_formatter_base(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args) {
    return RunNativeReferenceAssembly_formatter_base(bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);
}
}  // namespace chaos::il2cpp::generated
