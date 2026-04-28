// family-runtime-skeleton
// native-reference runtime skeleton for family/System.Private.CoreLib/time/date-time-timespan
#include "bootstrap.h"
#include "codegen_bridge.h"
#include "memory_domain.h"
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace chaos::il2cpp::generated
{
namespace runtime_skeleton_time_date_time_timespan
{
constexpr const char* kAssemblyName = "System.Private.CoreLib";
constexpr const char* kPlanKind = "family-runtime-skeleton";
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr std::size_t translation_unit_method_count = 16;
constexpr std::size_t translation_unit_page_size = 16;
constexpr std::size_t translation_unit_page_count = 1;

using NativeReferenceAssemblyPageDispatchFn = int32_t (CHAOS_RUNTIME_ABI_CALL*)(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    std::uint32_t method_slot,
    void* managed_args);

struct NativeReferenceAssemblyDispatchRequest
{
    const char* subject_id;
    void* managed_args;
    std::uint32_t method_id;
};

struct RuntimeSkeletonMethodDispatchCatalogEntry
{
    std::uint32_t method_id;
    const char* subject_id;
    NativeReferenceAssemblyPageDispatchFn dispatch;
    std::uint32_t method_slot;
};

// Page dispatch declarations
int32_t CHAOS_RUNTIME_ABI_CALL DispatchRuntimeSkeletonPage0001(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    std::uint32_t method_slot,
    void* managed_args);

constexpr RuntimeSkeletonMethodDispatchCatalogEntry kMethodDispatchCatalog[] =
{
    { 0, "System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)", DispatchRuntimeSkeletonPage0001, 0 },
    { 1, "System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)", DispatchRuntimeSkeletonPage0001, 1 },
    { 2, "System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)", DispatchRuntimeSkeletonPage0001, 2 },
    { 3, "System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)", DispatchRuntimeSkeletonPage0001, 3 },
    { 4, "System.Private.CoreLib/System.DateTime::ToString:System.String()", DispatchRuntimeSkeletonPage0001, 4 },
    { 5, "System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)", DispatchRuntimeSkeletonPage0001, 5 },
    { 6, "System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)", DispatchRuntimeSkeletonPage0001, 6 },
    { 7, "System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)", DispatchRuntimeSkeletonPage0001, 7 },
    { 8, "System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)", DispatchRuntimeSkeletonPage0001, 8 },
    { 9, "System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)", DispatchRuntimeSkeletonPage0001, 9 },
    { 10, "System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)", DispatchRuntimeSkeletonPage0001, 10 },
    { 11, "System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)", DispatchRuntimeSkeletonPage0001, 11 },
    { 12, "System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)", DispatchRuntimeSkeletonPage0001, 12 },
    { 13, "System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)", DispatchRuntimeSkeletonPage0001, 13 },
    { 14, "System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)", DispatchRuntimeSkeletonPage0001, 14 },
    { 15, "System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)", DispatchRuntimeSkeletonPage0001, 15 },
};
constexpr std::size_t kMethodDispatchCatalogCount = sizeof(kMethodDispatchCatalog) / sizeof(kMethodDispatchCatalog[0]);

const RuntimeSkeletonMethodDispatchCatalogEntry* FindMethodDispatchCatalogEntry(const char* subject_id)
{
    if (subject_id == nullptr || subject_id[0] == '\0') { return nullptr; }
    if (kMethodDispatchCatalogCount == 0) { return nullptr; }
    std::size_t left = 0;
    std::size_t right = kMethodDispatchCatalogCount;
    while (left < right) {
        const std::size_t mid = left + ((right - left) / 2);
        const auto& entry = kMethodDispatchCatalog[mid];
        const int compare = std::strcmp(entry.subject_id, subject_id);
        if (compare < 0) { left = mid + 1; continue; }
        if (compare > 0) { right = mid; continue; }
        return &entry;
    }
    return nullptr;
}

const RuntimeSkeletonMethodDispatchCatalogEntry* FindMethodDispatchCatalogEntryByMethodId(std::uint32_t method_id)
{
    if (method_id == 0) { return nullptr; }
    if (kMethodDispatchCatalogCount == 0) { return nullptr; }
    std::size_t left = 0;
    std::size_t right = kMethodDispatchCatalogCount;
    while (left < right) {
        const std::size_t mid = left + ((right - left) / 2);
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
    std::uint32_t method_id,
    void* managed_args)
{
    const auto* method = FindMethodDispatchCatalogEntryByMethodId(method_id);
    if (method == nullptr) { return CHAOS_BRIDGE_STATUS_NOT_FOUND; }
    return method->dispatch(bridge, code_registration, metadata_registration, options, runtime, thread, method->method_slot, managed_args);
}

}  // namespace runtime_skeleton_time_date_time_timespan

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReferenceAssembly_time_date_time_timespan(
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
    auto* domain = md::FindDomainByName(runtime_skeleton_time_date_time_timespan::kAssemblyName);
    if (domain == nullptr) { domain = md::FindDomainById(chaos::il2cpp::bootstrap::GetAotDomainId()); }
    md::DomainScope domain_scope(domain);

    CodeRegistrationV0 local_code_registration = *code_registration;
    const BridgeStatus register_status = bridge->register_codegen(&local_code_registration, metadata_registration, options);
    if (register_status != CHAOS_BRIDGE_STATUS_OK) { return register_status; }
    const BridgeStatus bootstrap_status = bridge->bootstrap_runtime();
    if (bootstrap_status != CHAOS_BRIDGE_STATUS_OK) { return bootstrap_status; }
    if (managed_args == nullptr) { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }
    const auto* request = static_cast<const runtime_skeleton_time_date_time_timespan::NativeReferenceAssemblyDispatchRequest*>(managed_args);
    if (request->method_id != 0) {
        return runtime_skeleton_time_date_time_timespan::DispatchAssemblyMethodId(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->method_id, request->managed_args);
    }
    return runtime_skeleton_time_date_time_timespan::DispatchAssemblySubject(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->subject_id, request->managed_args);
}

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReference_time_date_time_timespan(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args) {
    return RunNativeReferenceAssembly_time_date_time_timespan(bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);
}
}  // namespace chaos::il2cpp::generated
