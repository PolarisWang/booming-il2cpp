// family-runtime-skeleton
// native-reference runtime skeleton for family/System.Runtime.InteropServices/com-types
#include "bootstrap.h"
#include "codegen_bridge.h"
#include "memory_domain.h"
#include <chaos/native_types.h>
#include <chaos/native_types.h>
#include <cstring>

namespace chaos::il2cpp::generated
{
namespace runtime_skeleton_com_types
{
constexpr const char* kAssemblyName = "System.Runtime.InteropServices";
constexpr const char* kPlanKind = "family-runtime-skeleton";
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr CHAOS_IL2CPP_SIZE translation_unit_method_count = 182;
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_size = 182;
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
    { 0, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnClose:System.Void()", DispatchRuntimeSkeletonPage0001, 0 },
    { 1, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnDataChange:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 1 },
    { 2, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnRename:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 2 },
    { 3, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnSave:System.Void()", DispatchRuntimeSkeletonPage0001, 3 },
    { 4, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IAdviseSink::OnViewChange:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 4 },
    { 5, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::EnumObjectParam:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 5 },
    { 6, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::GetBindOptions:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 6 },
    { 7, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::GetObjectParam:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 7 },
    { 8, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::GetRunningObjectTable:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 8 },
    { 9, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RegisterObjectBound:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 9 },
    { 10, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RegisterObjectParam:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 10 },
    { 11, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::ReleaseBoundObjects:System.Void()", DispatchRuntimeSkeletonPage0001, 11 },
    { 12, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RevokeObjectBound:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 12 },
    { 13, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::RevokeObjectParam:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 13 },
    { 14, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IBindCtx::SetBindOptions:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 14 },
    { 15, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::Advise:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 15 },
    { 16, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::EnumConnections:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 16 },
    { 17, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::GetConnectionInterface:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 17 },
    { 18, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::GetConnectionPointContainer:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 18 },
    { 19, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPoint::Unadvise:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 19 },
    { 20, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPointContainer::EnumConnectionPoints:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 20 },
    { 21, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IConnectionPointContainer::FindConnectionPoint:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 21 },
    { 22, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DAdvise:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 22 },
    { 23, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::DUnadvise:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 23 },
    { 24, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumDAdvise:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 24 },
    { 25, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::EnumFormatEtc:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 25 },
    { 26, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetCanonicalFormatEtc:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 26 },
    { 27, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetData:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 27 },
    { 28, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::GetDataHere:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 28 },
    { 29, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::QueryGetData:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 29 },
    { 30, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IDataObject::SetData:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 30 },
    { 31, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Clone:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 31 },
    { 32, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Next:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 32 },
    { 33, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Reset:System.Void()", DispatchRuntimeSkeletonPage0001, 33 },
    { 34, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnectionPoints::Skip:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 34 },
    { 35, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Clone:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 35 },
    { 36, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Next:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 36 },
    { 37, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Reset:System.Void()", DispatchRuntimeSkeletonPage0001, 37 },
    { 38, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumConnections::Skip:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 38 },
    { 39, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Clone:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 39 },
    { 40, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Next:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 40 },
    { 41, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Reset:System.Void()", DispatchRuntimeSkeletonPage0001, 41 },
    { 42, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumFORMATETC::Skip:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 42 },
    { 43, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Clone:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 43 },
    { 44, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Next:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 44 },
    { 45, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Reset:System.Void()", DispatchRuntimeSkeletonPage0001, 45 },
    { 46, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumMoniker::Skip:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 46 },
    { 47, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Clone:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 47 },
    { 48, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Next:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 48 },
    { 49, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Reset:System.Void()", DispatchRuntimeSkeletonPage0001, 49 },
    { 50, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumSTATDATA::Skip:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 50 },
    { 51, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Clone:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 51 },
    { 52, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Next:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 52 },
    { 53, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Reset:System.Void()", DispatchRuntimeSkeletonPage0001, 53 },
    { 54, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumString::Skip:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 54 },
    { 55, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Clone:System.Void()", DispatchRuntimeSkeletonPage0001, 55 },
    { 56, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Next:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 56 },
    { 57, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Reset:System.Void()", DispatchRuntimeSkeletonPage0001, 57 },
    { 58, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IEnumVARIANT::Skip:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 58 },
    { 59, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::BindToObject:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 59 },
    { 60, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::BindToStorage:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 60 },
    { 61, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::CommonPrefixWith:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 61 },
    { 62, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::ComposeWith:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 62 },
    { 63, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Enum:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 63 },
    { 64, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetClassID:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 64 },
    { 65, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetDisplayName:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 65 },
    { 66, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetSizeMax:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 66 },
    { 67, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::GetTimeOfLastChange:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 67 },
    { 68, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Hash:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 68 },
    { 69, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Inverse:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 69 },
    { 70, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsDirty:System.Void()", DispatchRuntimeSkeletonPage0001, 70 },
    { 71, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsEqual:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 71 },
    { 72, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsRunning:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 72 },
    { 73, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::IsSystemMoniker:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 73 },
    { 74, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Load:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 74 },
    { 75, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::ParseDisplayName:System.Void(P0, P1, P2, P3, P4)", DispatchRuntimeSkeletonPage0001, 75 },
    { 76, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Reduce:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 76 },
    { 77, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::RelativePathTo:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 77 },
    { 78, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IMoniker::Save:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 78 },
    { 79, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::GetClassID:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 79 },
    { 80, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::GetCurFile:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 80 },
    { 81, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::IsDirty:System.Void()", DispatchRuntimeSkeletonPage0001, 81 },
    { 82, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::Load:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 82 },
    { 83, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::Save:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 83 },
    { 84, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IPersistFile::SaveCompleted:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 84 },
    { 85, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::EnumRunning:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 85 },
    { 86, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::GetObject:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 86 },
    { 87, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::GetTimeOfLastChange:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 87 },
    { 88, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::IsRunning:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 88 },
    { 89, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::NoteChangeTime:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 89 },
    { 90, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::Register:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 90 },
    { 91, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IRunningObjectTable::Revoke:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 91 },
    { 92, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Clone:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 92 },
    { 93, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Commit:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 93 },
    { 94, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::CopyTo:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 94 },
    { 95, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::LockRegion:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 95 },
    { 96, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Read:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 96 },
    { 97, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Revert:System.Void()", DispatchRuntimeSkeletonPage0001, 97 },
    { 98, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Seek:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 98 },
    { 99, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::SetSize:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 99 },
    { 100, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Stat:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 100 },
    { 101, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::UnlockRegion:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 101 },
    { 102, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.IStream::Write:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 102 },
    { 103, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeComp::Bind:System.Void(P0, P1, P2, P3, P4, P5)", DispatchRuntimeSkeletonPage0001, 103 },
    { 104, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeComp::BindType:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 104 },
    { 105, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::AddressOfMember:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 105 },
    { 106, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::CreateInstance:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 106 },
    { 107, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetContainingTypeLib:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 107 },
    { 108, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetDllEntry:System.Void(P0, P1, P2, P3, P4)", DispatchRuntimeSkeletonPage0001, 108 },
    { 109, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetDocumentation:System.Void(P0, P1, P2, P3, P4)", DispatchRuntimeSkeletonPage0001, 109 },
    { 110, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetFuncDesc:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 110 },
    { 111, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetIDsOfNames:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 111 },
    { 112, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetImplTypeFlags:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 112 },
    { 113, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetMops:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 113 },
    { 114, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetNames:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 114 },
    { 115, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetRefTypeInfo:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 115 },
    { 116, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetRefTypeOfImplType:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 116 },
    { 117, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetTypeAttr:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 117 },
    { 118, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetTypeComp:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 118 },
    { 119, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::GetVarDesc:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 119 },
    { 120, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::Invoke:System.Void(P0, P1, P2, P3, P4, P5, P6)", DispatchRuntimeSkeletonPage0001, 120 },
    { 121, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::ReleaseFuncDesc:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 121 },
    { 122, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::ReleaseTypeAttr:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 122 },
    { 123, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo::ReleaseVarDesc:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 123 },
    { 124, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::AddressOfMember:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 124 },
    { 125, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::CreateInstance:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 125 },
    { 126, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllCustData:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 126 },
    { 127, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllFuncCustData:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 127 },
    { 128, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllImplTypeCustData:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 128 },
    { 129, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllParamCustData:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 129 },
    { 130, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetAllVarCustData:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 130 },
    { 131, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetContainingTypeLib:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 131 },
    { 132, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetCustData:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 132 },
    { 133, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetDllEntry:System.Void(P0, P1, P2, P3, P4)", DispatchRuntimeSkeletonPage0001, 133 },
    { 134, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetDocumentation:System.Void(P0, P1, P2, P3, P4)", DispatchRuntimeSkeletonPage0001, 134 },
    { 135, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetDocumentation2:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 135 },
    { 136, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetFuncCustData:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 136 },
    { 137, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetFuncDesc:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 137 },
    { 138, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetFuncIndexOfMemId:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 138 },
    { 139, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetIDsOfNames:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 139 },
    { 140, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetImplTypeCustData:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 140 },
    { 141, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetImplTypeFlags:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 141 },
    { 142, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetMops:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 142 },
    { 143, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetNames:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 143 },
    { 144, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetParamCustData:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 144 },
    { 145, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetRefTypeInfo:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 145 },
    { 146, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetRefTypeOfImplType:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 146 },
    { 147, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeAttr:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 147 },
    { 148, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeComp:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 148 },
    { 149, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeFlags:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 149 },
    { 150, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetTypeKind:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 150 },
    { 151, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetVarCustData:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 151 },
    { 152, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetVarDesc:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 152 },
    { 153, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::GetVarIndexOfMemId:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 153 },
    { 154, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::Invoke:System.Void(P0, P1, P2, P3, P4, P5, P6)", DispatchRuntimeSkeletonPage0001, 154 },
    { 155, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::ReleaseFuncDesc:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 155 },
    { 156, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::ReleaseTypeAttr:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 156 },
    { 157, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeInfo2::ReleaseVarDesc:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 157 },
    { 158, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::FindName:System.Void(P0, P1, P2, P3, P4)", DispatchRuntimeSkeletonPage0001, 158 },
    { 159, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetDocumentation:System.Void(P0, P1, P2, P3, P4)", DispatchRuntimeSkeletonPage0001, 159 },
    { 160, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetLibAttr:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 160 },
    { 161, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeComp:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 161 },
    { 162, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfo:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 162 },
    { 163, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfoCount:System.Void()", DispatchRuntimeSkeletonPage0001, 163 },
    { 164, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfoOfGuid:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 164 },
    { 165, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::GetTypeInfoType:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 165 },
    { 166, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::IsName:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 166 },
    { 167, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib::ReleaseTLibAttr:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 167 },
    { 168, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::FindName:System.Void(P0, P1, P2, P3, P4)", DispatchRuntimeSkeletonPage0001, 168 },
    { 169, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetAllCustData:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 169 },
    { 170, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetCustData:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 170 },
    { 171, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetDocumentation:System.Void(P0, P1, P2, P3, P4)", DispatchRuntimeSkeletonPage0001, 171 },
    { 172, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetDocumentation2:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 172 },
    { 173, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetLibAttr:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 173 },
    { 174, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetLibStatistics:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 174 },
    { 175, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeComp:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 175 },
    { 176, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfo:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 176 },
    { 177, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfoCount:System.Void()", DispatchRuntimeSkeletonPage0001, 177 },
    { 178, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfoOfGuid:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 178 },
    { 179, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::GetTypeInfoType:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 179 },
    { 180, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::IsName:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 180 },
    { 181, "System.Runtime.InteropServices/System.Runtime.InteropServices.ComTypes.ITypeLib2::ReleaseTLibAttr:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 181 },
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

}  // namespace runtime_skeleton_com_types

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReferenceAssembly_com_types(
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
    auto* domain = md::FindDomainByName(runtime_skeleton_com_types::kAssemblyName);
    if (domain == nullptr) { domain = md::FindDomainById(chaos::il2cpp::bootstrap::GetAotDomainId()); }
    md::DomainScope domain_scope(domain);

    CodeRegistrationV0 local_code_registration = *code_registration;
    const BridgeStatus register_status = bridge->register_codegen(&local_code_registration, metadata_registration, options);
    if (register_status != CHAOS_BRIDGE_STATUS_OK) { return register_status; }
    const BridgeStatus bootstrap_status = bridge->bootstrap_runtime();
    if (bootstrap_status != CHAOS_BRIDGE_STATUS_OK) { return bootstrap_status; }
    if (managed_args == nullptr) { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }
    const auto* request = static_cast<const runtime_skeleton_com_types::NativeReferenceAssemblyDispatchRequest*>(managed_args);
    if (request->method_id != 0) {
        return runtime_skeleton_com_types::DispatchAssemblyMethodId(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->method_id, request->managed_args);
    }
    return runtime_skeleton_com_types::DispatchAssemblySubject(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->subject_id, request->managed_args);
}

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReference_com_types(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args) {
    return RunNativeReferenceAssembly_com_types(bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);
}
}  // namespace chaos::il2cpp::generated
