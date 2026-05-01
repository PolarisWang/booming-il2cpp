// family-runtime-skeleton
// native-reference runtime skeleton for family/System.Linq/filtering
#include "bootstrap.h"
#include "codegen_bridge.h"
#include "memory_domain.h"
#include <chaos/native_types.h>
#include <chaos/native_types.h>
#include <cstring>

namespace chaos::il2cpp::generated
{
namespace runtime_skeleton_filtering
{
constexpr const char* kAssemblyName = "System.Linq";
constexpr const char* kPlanKind = "family-runtime-skeleton";
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr CHAOS_IL2CPP_SIZE translation_unit_method_count = 65;
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_size = 65;
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
    { 0, "System.Linq/System.Linq.Enumerable::Enumerable::Void(System.Object)", DispatchRuntimeSkeletonPage0001, 0 },
    { 1, "System.Linq/System.Linq.Enumerable::get_Property:System.Void()", DispatchRuntimeSkeletonPage0001, 1 },
    { 2, "System.Linq/System.Linq.Enumerable::Method1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 2 },
    { 3, "System.Linq/System.Linq.Enumerable::Method2:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 3 },
    { 4, "System.Linq/System.Linq.Enumerable::Validate:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 4 },
    { 5, "System.Linq/System.Linq.IGrouping::IGrouping::Void(System.Object)", DispatchRuntimeSkeletonPage0001, 5 },
    { 6, "System.Linq/System.Linq.IGrouping::get_Property:System.Void()", DispatchRuntimeSkeletonPage0001, 6 },
    { 7, "System.Linq/System.Linq.IGrouping::Method1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 7 },
    { 8, "System.Linq/System.Linq.IGrouping::Method2:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 8 },
    { 9, "System.Linq/System.Linq.IGrouping::Validate:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 9 },
    { 10, "System.Linq/System.Linq.ILookup::ILookup::Void(System.Object)", DispatchRuntimeSkeletonPage0001, 10 },
    { 11, "System.Linq/System.Linq.ILookup::get_Property:System.Void()", DispatchRuntimeSkeletonPage0001, 11 },
    { 12, "System.Linq/System.Linq.ILookup::Method1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 12 },
    { 13, "System.Linq/System.Linq.ILookup::Method2:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 13 },
    { 14, "System.Linq/System.Linq.ILookup::Validate:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 14 },
    { 15, "System.Linq/System.Linq.IOrderedEnumerable::IOrderedEnumerable::Void(System.Object)", DispatchRuntimeSkeletonPage0001, 15 },
    { 16, "System.Linq/System.Linq.IOrderedEnumerable::get_Property:System.Void()", DispatchRuntimeSkeletonPage0001, 16 },
    { 17, "System.Linq/System.Linq.IOrderedEnumerable::Method1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 17 },
    { 18, "System.Linq/System.Linq.IOrderedEnumerable::Method2:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 18 },
    { 19, "System.Linq/System.Linq.IOrderedEnumerable::Validate:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 19 },
    { 20, "System.Linq/System.Linq.Lookup::Lookup::Void(System.Object)", DispatchRuntimeSkeletonPage0001, 20 },
    { 21, "System.Linq/System.Linq.Lookup::get_Property:System.Void()", DispatchRuntimeSkeletonPage0001, 21 },
    { 22, "System.Linq/System.Linq.Lookup::Method1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 22 },
    { 23, "System.Linq/System.Linq.Lookup::Method2:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 23 },
    { 24, "System.Linq/System.Linq.Lookup::Validate:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 24 },
    { 25, "System.Linq/System.Linq.Enumerable::get_Value:System.Void()", DispatchRuntimeSkeletonPage0001, 25 },
    { 26, "System.Linq/System.Linq.Enumerable::set_Value:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 26 },
    { 27, "System.Linq/System.Linq.Enumerable::get_Count:System.Void()", DispatchRuntimeSkeletonPage0001, 27 },
    { 28, "System.Linq/System.Linq.Enumerable::set_Count:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 28 },
    { 29, "System.Linq/System.Linq.Enumerable::get_IsEmpty:System.Void()", DispatchRuntimeSkeletonPage0001, 29 },
    { 30, "System.Linq/System.Linq.Enumerable::set_IsEmpty:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 30 },
    { 31, "System.Linq/System.Linq.Enumerable::get_Key:System.Void()", DispatchRuntimeSkeletonPage0001, 31 },
    { 32, "System.Linq/System.Linq.Enumerable::set_Key:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 32 },
    { 33, "System.Linq/System.Linq.IGrouping::get_Value:System.Void()", DispatchRuntimeSkeletonPage0001, 33 },
    { 34, "System.Linq/System.Linq.IGrouping::set_Value:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 34 },
    { 35, "System.Linq/System.Linq.IGrouping::get_Count:System.Void()", DispatchRuntimeSkeletonPage0001, 35 },
    { 36, "System.Linq/System.Linq.IGrouping::set_Count:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 36 },
    { 37, "System.Linq/System.Linq.IGrouping::get_IsEmpty:System.Void()", DispatchRuntimeSkeletonPage0001, 37 },
    { 38, "System.Linq/System.Linq.IGrouping::set_IsEmpty:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 38 },
    { 39, "System.Linq/System.Linq.IGrouping::get_Key:System.Void()", DispatchRuntimeSkeletonPage0001, 39 },
    { 40, "System.Linq/System.Linq.IGrouping::set_Key:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 40 },
    { 41, "System.Linq/System.Linq.ILookup::get_Value:System.Void()", DispatchRuntimeSkeletonPage0001, 41 },
    { 42, "System.Linq/System.Linq.ILookup::set_Value:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 42 },
    { 43, "System.Linq/System.Linq.ILookup::get_Count:System.Void()", DispatchRuntimeSkeletonPage0001, 43 },
    { 44, "System.Linq/System.Linq.ILookup::set_Count:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 44 },
    { 45, "System.Linq/System.Linq.ILookup::get_IsEmpty:System.Void()", DispatchRuntimeSkeletonPage0001, 45 },
    { 46, "System.Linq/System.Linq.ILookup::set_IsEmpty:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 46 },
    { 47, "System.Linq/System.Linq.ILookup::get_Key:System.Void()", DispatchRuntimeSkeletonPage0001, 47 },
    { 48, "System.Linq/System.Linq.ILookup::set_Key:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 48 },
    { 49, "System.Linq/System.Linq.IOrderedEnumerable::get_Value:System.Void()", DispatchRuntimeSkeletonPage0001, 49 },
    { 50, "System.Linq/System.Linq.IOrderedEnumerable::set_Value:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 50 },
    { 51, "System.Linq/System.Linq.IOrderedEnumerable::get_Count:System.Void()", DispatchRuntimeSkeletonPage0001, 51 },
    { 52, "System.Linq/System.Linq.IOrderedEnumerable::set_Count:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 52 },
    { 53, "System.Linq/System.Linq.IOrderedEnumerable::get_IsEmpty:System.Void()", DispatchRuntimeSkeletonPage0001, 53 },
    { 54, "System.Linq/System.Linq.IOrderedEnumerable::set_IsEmpty:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 54 },
    { 55, "System.Linq/System.Linq.IOrderedEnumerable::get_Key:System.Void()", DispatchRuntimeSkeletonPage0001, 55 },
    { 56, "System.Linq/System.Linq.IOrderedEnumerable::set_Key:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 56 },
    { 57, "System.Linq/System.Linq.Lookup::get_Value:System.Void()", DispatchRuntimeSkeletonPage0001, 57 },
    { 58, "System.Linq/System.Linq.Lookup::set_Value:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 58 },
    { 59, "System.Linq/System.Linq.Lookup::get_Count:System.Void()", DispatchRuntimeSkeletonPage0001, 59 },
    { 60, "System.Linq/System.Linq.Lookup::set_Count:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 60 },
    { 61, "System.Linq/System.Linq.Lookup::get_IsEmpty:System.Void()", DispatchRuntimeSkeletonPage0001, 61 },
    { 62, "System.Linq/System.Linq.Lookup::set_IsEmpty:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 62 },
    { 63, "System.Linq/System.Linq.Lookup::get_Key:System.Void()", DispatchRuntimeSkeletonPage0001, 63 },
    { 64, "System.Linq/System.Linq.Lookup::set_Key:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 64 },
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

}  // namespace runtime_skeleton_filtering

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReferenceAssembly_filtering(
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
    auto* domain = md::FindDomainByName(runtime_skeleton_filtering::kAssemblyName);
    if (domain == nullptr) { domain = md::FindDomainById(chaos::il2cpp::bootstrap::GetAotDomainId()); }
    md::DomainScope domain_scope(domain);

    CodeRegistrationV0 local_code_registration = *code_registration;
    const BridgeStatus register_status = bridge->register_codegen(&local_code_registration, metadata_registration, options);
    if (register_status != CHAOS_BRIDGE_STATUS_OK) { return register_status; }
    const BridgeStatus bootstrap_status = bridge->bootstrap_runtime();
    if (bootstrap_status != CHAOS_BRIDGE_STATUS_OK) { return bootstrap_status; }
    if (managed_args == nullptr) { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }
    const auto* request = static_cast<const runtime_skeleton_filtering::NativeReferenceAssemblyDispatchRequest*>(managed_args);
    if (request->method_id != 0) {
        return runtime_skeleton_filtering::DispatchAssemblyMethodId(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->method_id, request->managed_args);
    }
    return runtime_skeleton_filtering::DispatchAssemblySubject(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->subject_id, request->managed_args);
}

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReference_filtering(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args) {
    return RunNativeReferenceAssembly_filtering(bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);
}
}  // namespace chaos::il2cpp::generated
