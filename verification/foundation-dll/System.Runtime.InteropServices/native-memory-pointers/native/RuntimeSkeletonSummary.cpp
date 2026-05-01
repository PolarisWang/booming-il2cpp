// family-runtime-skeleton
// native-reference runtime skeleton for family/System.Runtime.InteropServices/native-memory-pointers
#include "bootstrap.h"
#include "codegen_bridge.h"
#include "memory_domain.h"
#include <chaos/native_types.h>
#include <chaos/native_types.h>
#include <cstring>

namespace chaos::il2cpp::generated
{
namespace runtime_skeleton_native_memory_pointers
{
constexpr const char* kAssemblyName = "System.Runtime.InteropServices";
constexpr const char* kPlanKind = "family-runtime-skeleton";
constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";
constexpr CHAOS_IL2CPP_SIZE translation_unit_method_count = 335;
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_size = 256;
constexpr CHAOS_IL2CPP_SIZE translation_unit_page_count = 2;

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

int32_t CHAOS_RUNTIME_ABI_CALL DispatchRuntimeSkeletonPage0002(
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
    { 0, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::UnmanagedMemoryAccessor::Void()", DispatchRuntimeSkeletonPage0001, 0 },
    { 1, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::UnmanagedMemoryAccessor::Void(System.Object, System.Object, System.Object)", DispatchRuntimeSkeletonPage0001, 1 },
    { 2, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::UnmanagedMemoryAccessor::Void(System.Object, System.Object, System.Object, System.Object)", DispatchRuntimeSkeletonPage0001, 2 },
    { 3, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Dispose:System.Void()", DispatchRuntimeSkeletonPage0001, 3 },
    { 4, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Dispose:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 4 },
    { 5, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Initialize:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 5 },
    { 6, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadArray`1:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 6 },
    { 7, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadBoolean:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 7 },
    { 8, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadByte:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 8 },
    { 9, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadChar:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 9 },
    { 10, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDecimal:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 10 },
    { 11, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDouble:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 11 },
    { 12, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt16:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 12 },
    { 13, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt32:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 13 },
    { 14, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt64:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 14 },
    { 15, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSByte:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 15 },
    { 16, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSingle:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 16 },
    { 17, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt16:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 17 },
    { 18, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt32:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 18 },
    { 19, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt64:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 19 },
    { 20, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Read`1:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 20 },
    { 21, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 21 },
    { 22, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::WriteArray`1:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 22 },
    { 23, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write`1:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 23 },
    { 24, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanRead:System.Void()", DispatchRuntimeSkeletonPage0001, 24 },
    { 25, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanWrite:System.Void()", DispatchRuntimeSkeletonPage0001, 25 },
    { 26, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_Capacity:System.Void()", DispatchRuntimeSkeletonPage0001, 26 },
    { 27, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_IsOpen:System.Void()", DispatchRuntimeSkeletonPage0001, 27 },
    { 28, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::set_CanRead:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 28 },
    { 29, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::set_CanWrite:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 29 },
    { 30, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::set_Capacity:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 30 },
    { 31, "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::set_IsOpen:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 31 },
    { 32, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AddRef:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 32 },
    { 33, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocCoTaskMem:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 33 },
    { 34, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 34 },
    { 35, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AreComObjectsAvailableForCleanup:System.Void()", DispatchRuntimeSkeletonPage0001, 35 },
    { 36, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::BindToMoniker:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 36 },
    { 37, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ChangeWrapperHandleStrength:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 37 },
    { 38, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CleanupUnusedObjectsInCurrentContext:System.Void()", DispatchRuntimeSkeletonPage0001, 38 },
    { 39, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0001, 39 },
    { 40, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 40 },
    { 41, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject`1:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 41 },
    { 42, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 42 },
    { 43, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType`2:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 43 },
    { 44, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 44 },
    { 45, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure`1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 45 },
    { 46, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FinalReleaseComObject:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 46 },
    { 47, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeBSTR:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 47 },
    { 48, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeCoTaskMem:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 48 },
    { 49, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeHGlobal:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 49 },
    { 50, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateGuidForType:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 50 },
    { 51, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateProgIdForType:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 51 },
    { 52, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 52 },
    { 53, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 53 },
    { 54, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject`2:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 54 },
    { 55, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComObjectData:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 55 },
    { 56, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 56 },
    { 57, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer`1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 57 },
    { 58, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetEndComSlot:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 58 },
    { 59, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionCode:System.Void()", DispatchRuntimeSkeletonPage0001, 59 },
    { 60, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 60 },
    { 61, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 61 },
    { 62, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 62 },
    { 63, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionPointers:System.Void()", DispatchRuntimeSkeletonPage0001, 63 },
    { 64, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 64 },
    { 65, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate`1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 65 },
    { 66, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHINSTANCE:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 66 },
    { 67, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForException:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 67 },
    { 68, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForLastWin32Error:System.Void()", DispatchRuntimeSkeletonPage0001, 68 },
    { 69, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIDispatchForObject:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 69 },
    { 70, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIUnknownForObject:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 70 },
    { 71, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Void()", DispatchRuntimeSkeletonPage0001, 71 },
    { 72, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeErrorMessage:System.Void()", DispatchRuntimeSkeletonPage0001, 72 },
    { 73, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastSystemError:System.Void()", DispatchRuntimeSkeletonPage0001, 73 },
    { 74, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastWin32Error:System.Void()", DispatchRuntimeSkeletonPage0001, 74 },
    { 75, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 75 },
    { 76, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject`1:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 76 },
    { 77, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForIUnknown:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 77 },
    { 78, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 78 },
    { 79, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant`1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 79 },
    { 80, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 80 },
    { 81, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants`1:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 81 },
    { 82, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetPInvokeErrorMessage:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 82 },
    { 83, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetStartComSlot:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 83 },
    { 84, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeFromCLSID:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 84 },
    { 85, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeInfoName:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 85 },
    { 86, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypedObjectForIUnknown:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 86 },
    { 87, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetUniqueObjectForIUnknown:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 87 },
    { 88, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::InitHandle:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 88 },
    { 89, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsComObject:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 89 },
    { 90, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsTypeVisibleFromCom:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 90 },
    { 91, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 91 },
    { 92, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf`1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 92 },
    { 93, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Prelink:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 93 },
    { 94, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PrelinkAll:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 94 },
    { 95, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 95 },
    { 96, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 96 },
    { 97, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 97 },
    { 98, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 98 },
    { 99, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringBSTR:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 99 },
    { 100, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 100 },
    { 101, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 101 },
    { 102, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 102 },
    { 103, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 103 },
    { 104, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 104 },
    { 105, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure`1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 105 },
    { 106, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure`1:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 106 },
    { 107, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::QueryInterface:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 107 },
    { 108, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocCoTaskMem:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 108 },
    { 109, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocHGlobal:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 109 },
    { 110, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 110 },
    { 111, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 111 },
    { 112, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 112 },
    { 113, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 113 },
    { 114, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 114 },
    { 115, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 115 },
    { 116, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 116 },
    { 117, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 117 },
    { 118, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 118 },
    { 119, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 119 },
    { 120, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Release:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 120 },
    { 121, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReleaseComObject:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 121 },
    { 122, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToBSTR:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 122 },
    { 123, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemAnsi:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 123 },
    { 124, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemUnicode:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 124 },
    { 125, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocAnsi:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 125 },
    { 126, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocUnicode:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 126 },
    { 127, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetComObjectData:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 127 },
    { 128, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastPInvokeError:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 128 },
    { 129, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastSystemError:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 129 },
    { 130, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 130 },
    { 131, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf`1:System.Void()", DispatchRuntimeSkeletonPage0001, 131 },
    { 132, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf`1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 132 },
    { 133, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToBSTR:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 133 },
    { 134, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAnsi:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 134 },
    { 135, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAuto:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 135 },
    { 136, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 136 },
    { 137, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUni:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 137 },
    { 138, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAnsi:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 138 },
    { 139, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAuto:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 139 },
    { 140, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalUni:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 140 },
    { 141, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 141 },
    { 142, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr`1:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 142 },
    { 143, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 143 },
    { 144, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 144 },
    { 145, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 145 },
    { 146, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 146 },
    { 147, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement`1:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 147 },
    { 148, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 148 },
    { 149, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 149 },
    { 150, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 150 },
    { 151, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 151 },
    { 152, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 152 },
    { 153, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 153 },
    { 154, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 154 },
    { 155, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 155 },
    { 156, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 156 },
    { 157, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 157 },
    { 158, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeBSTR:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 158 },
    { 159, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemAnsi:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 159 },
    { 160, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUTF8:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 160 },
    { 161, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUnicode:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 161 },
    { 162, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocAnsi:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 162 },
    { 163, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocUnicode:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 163 },
    { 164, "System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::MarshalAsAttribute::Void(System.Object)", DispatchRuntimeSkeletonPage0001, 164 },
    { 165, "System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::get_Value:System.Void()", DispatchRuntimeSkeletonPage0001, 165 },
    { 166, "System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::set_Value:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 166 },
    { 167, "System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalDirectiveException::MarshalDirectiveException::Void()", DispatchRuntimeSkeletonPage0001, 167 },
    { 168, "System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalDirectiveException::MarshalDirectiveException::Void(System.Object)", DispatchRuntimeSkeletonPage0001, 168 },
    { 169, "System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalDirectiveException::MarshalDirectiveException::Void(System.Object, System.Object)", DispatchRuntimeSkeletonPage0001, 169 },
    { 170, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller::ConvertToManaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 170 },
    { 171, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 171 },
    { 172, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller::Free:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 172 },
    { 173, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::Free:System.Void()", DispatchRuntimeSkeletonPage0001, 173 },
    { 174, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 174 },
    { 175, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::ToUnmanaged:System.Void()", DispatchRuntimeSkeletonPage0001, 175 },
    { 176, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::get_BufferSize:System.Void()", DispatchRuntimeSkeletonPage0001, 176 },
    { 177, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 177 },
    { 178, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::AllocateContainerForManagedElements:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 178 },
    { 179, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::AllocateContainerForUnmanagedElements:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 179 },
    { 180, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::Free:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 180 },
    { 181, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetManagedValuesDestination:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 181 },
    { 182, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetManagedValuesSource:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 182 },
    { 183, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetUnmanagedValuesDestination:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 183 },
    { 184, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetUnmanagedValuesSource:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 184 },
    { 185, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::Free:System.Void()", DispatchRuntimeSkeletonPage0001, 185 },
    { 186, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 186 },
    { 187, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetManagedValuesSource:System.Void()", DispatchRuntimeSkeletonPage0001, 187 },
    { 188, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void()", DispatchRuntimeSkeletonPage0001, 188 },
    { 189, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 189 },
    { 190, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetUnmanagedValuesDestination:System.Void()", DispatchRuntimeSkeletonPage0001, 190 },
    { 191, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::ToUnmanaged:System.Void()", DispatchRuntimeSkeletonPage0001, 191 },
    { 192, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::get_BufferSize:System.Void()", DispatchRuntimeSkeletonPage0001, 192 },
    { 193, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 193 },
    { 194, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller::ConvertToManaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 194 },
    { 195, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 195 },
    { 196, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller::Free:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 196 },
    { 197, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::Free:System.Void()", DispatchRuntimeSkeletonPage0001, 197 },
    { 198, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 198 },
    { 199, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::ToUnmanaged:System.Void()", DispatchRuntimeSkeletonPage0001, 199 },
    { 200, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::get_BufferSize:System.Void()", DispatchRuntimeSkeletonPage0001, 200 },
    { 201, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 201 },
    { 202, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::ComExposedClassAttribute`1::Void()", DispatchRuntimeSkeletonPage0001, 202 },
    { 203, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::GetComInterfaceEntries:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 203 },
    { 204, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToManaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 204 },
    { 205, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 205 },
    { 206, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::Free:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 206 },
    { 207, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::FinalRelease:System.Void()", DispatchRuntimeSkeletonPage0001, 207 },
    { 208, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::Finalize:System.Void()", DispatchRuntimeSkeletonPage0001, 208 },
    { 209, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::As`1:System.Void()", DispatchRuntimeSkeletonPage0001, 209 },
    { 210, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::CreateRaw`1:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 210 },
    { 211, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::Create`1:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 211 },
    { 212, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::Dispose:System.Void()", DispatchRuntimeSkeletonPage0001, 212 },
    { 213, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::GetRawDataRef`1:System.Void()", DispatchRuntimeSkeletonPage0001, 213 },
    { 214, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::get_Null:System.Void()", DispatchRuntimeSkeletonPage0001, 214 },
    { 215, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::get_VarType:System.Void()", DispatchRuntimeSkeletonPage0001, 215 },
    { 216, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::set_Null:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 216 },
    { 217, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::set_VarType:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 217 },
    { 218, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToManaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 218 },
    { 219, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 219 },
    { 220, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::Free:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 220 },
    { 221, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::Free:System.Void()", DispatchRuntimeSkeletonPage0001, 221 },
    { 222, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromManaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 222 },
    { 223, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 223 },
    { 224, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToManaged:System.Void()", DispatchRuntimeSkeletonPage0001, 224 },
    { 225, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToUnmanaged:System.Void()", DispatchRuntimeSkeletonPage0001, 225 },
    { 226, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsDefaultMarshaller`1::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 226 },
    { 227, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsHResultMarshaller`1::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 227 },
    { 228, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsNaNMarshaller`1::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 228 },
    { 229, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 229 },
    { 230, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComClassAttribute::GeneratedComClassAttribute::Void()", DispatchRuntimeSkeletonPage0001, 230 },
    { 231, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::GeneratedComInterfaceAttribute::Void()", DispatchRuntimeSkeletonPage0001, 231 },
    { 232, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_ExceptionToUnmanagedMarshaller:System.Void()", DispatchRuntimeSkeletonPage0001, 232 },
    { 233, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_Options:System.Void()", DispatchRuntimeSkeletonPage0001, 233 },
    { 234, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshalling:System.Void()", DispatchRuntimeSkeletonPage0001, 234 },
    { 235, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshallingCustomType:System.Void()", DispatchRuntimeSkeletonPage0001, 235 },
    { 236, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_ExceptionToUnmanagedMarshaller:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 236 },
    { 237, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_Options:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 237 },
    { 238, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshalling:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 238 },
    { 239, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshallingCustomType:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 239 },
    { 240, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedClass::GetComInterfaceEntries:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 240 },
    { 241, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedDetails::GetComInterfaceEntries:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 241 },
    { 242, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::Clear:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 242 },
    { 243, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::ConstructTableInfo:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0001, 243 },
    { 244, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TryGetTableInfo:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 244 },
    { 245, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TrySetTableInfo:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0001, 245 },
    { 246, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ManagedType:System.Void()", DispatchRuntimeSkeletonPage0001, 246 },
    { 247, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_Table:System.Void()", DispatchRuntimeSkeletonPage0001, 247 },
    { 248, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ThisPtr:System.Void()", DispatchRuntimeSkeletonPage0001, 248 },
    { 249, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ManagedType:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 249 },
    { 250, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_Table:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 250 },
    { 251, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ThisPtr:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 251 },
    { 252, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Iid:System.Void()", DispatchRuntimeSkeletonPage0001, 252 },
    { 253, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Implementation:System.Void()", DispatchRuntimeSkeletonPage0001, 253 },
    { 254, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_ManagedVirtualMethodTable:System.Void()", DispatchRuntimeSkeletonPage0001, 254 },
    { 255, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::set_Iid:System.Void(P0)", DispatchRuntimeSkeletonPage0001, 255 },
    { 256, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::set_Implementation:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 0 },
    { 257, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::set_ManagedVirtualMethodTable:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 1 },
    { 258, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetComExposedTypeDetails:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 2 },
    { 259, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetIUnknownDerivedDetails:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 3 },
    { 260, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_Iid:System.Void()", DispatchRuntimeSkeletonPage0002, 4 },
    { 261, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_ManagedVirtualMethodTable:System.Void()", DispatchRuntimeSkeletonPage0002, 5 },
    { 262, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::set_Iid:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 6 },
    { 263, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::set_ManagedVirtualMethodTable:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 7 },
    { 264, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::CreateInstancePointer:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 8 },
    { 265, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::QueryInterface:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0002, 9 },
    { 266, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::Release:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 10 },
    { 267, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::IUnknownDerivedAttribute`2::Void()", DispatchRuntimeSkeletonPage0002, 11 },
    { 268, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Iid:System.Void()", DispatchRuntimeSkeletonPage0002, 12 },
    { 269, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Implementation:System.Void()", DispatchRuntimeSkeletonPage0002, 13 },
    { 270, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_ManagedVirtualMethodTable:System.Void()", DispatchRuntimeSkeletonPage0002, 14 },
    { 271, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::set_Iid:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 15 },
    { 272, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::set_Implementation:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 16 },
    { 273, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::set_ManagedVirtualMethodTable:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 17 },
    { 274, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnmanagedVirtualMethodTableProvider::GetVirtualMethodTableInfoForKey:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 18 },
    { 275, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::MarshalUsingAttribute::Void()", DispatchRuntimeSkeletonPage0002, 19 },
    { 276, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::MarshalUsingAttribute::Void(System.Object)", DispatchRuntimeSkeletonPage0002, 20 },
    { 277, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_ConstantElementCount:System.Void()", DispatchRuntimeSkeletonPage0002, 21 },
    { 278, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_CountElementName:System.Void()", DispatchRuntimeSkeletonPage0002, 22 },
    { 279, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_ElementIndirectionDepth:System.Void()", DispatchRuntimeSkeletonPage0002, 23 },
    { 280, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_NativeType:System.Void()", DispatchRuntimeSkeletonPage0002, 24 },
    { 281, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_ConstantElementCount:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 25 },
    { 282, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_CountElementName:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 26 },
    { 283, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_ElementIndirectionDepth:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 27 },
    { 284, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_NativeType:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 28 },
    { 285, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::AllocateContainerForManagedElements:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0002, 29 },
    { 286, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::AllocateContainerForUnmanagedElements:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0002, 30 },
    { 287, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::Free:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 31 },
    { 288, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetManagedValuesDestination:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 32 },
    { 289, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetManagedValuesSource:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 33 },
    { 290, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetUnmanagedValuesDestination:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0002, 34 },
    { 291, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetUnmanagedValuesSource:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0002, 35 },
    { 292, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::Free:System.Void()", DispatchRuntimeSkeletonPage0002, 36 },
    { 293, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0002, 37 },
    { 294, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetManagedValuesSource:System.Void()", DispatchRuntimeSkeletonPage0002, 38 },
    { 295, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void()", DispatchRuntimeSkeletonPage0002, 39 },
    { 296, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 40 },
    { 297, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetUnmanagedValuesDestination:System.Void()", DispatchRuntimeSkeletonPage0002, 41 },
    { 298, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::ToUnmanaged:System.Void()", DispatchRuntimeSkeletonPage0002, 42 },
    { 299, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::get_BufferSize:System.Void()", DispatchRuntimeSkeletonPage0002, 43 },
    { 300, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 44 },
    { 301, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::StrategyBasedComWrappers::Void()", DispatchRuntimeSkeletonPage0002, 45 },
    { 302, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::ComputeVtables:System.Void(P0, P1, P2)", DispatchRuntimeSkeletonPage0002, 46 },
    { 303, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateCacheStrategy:System.Void()", DispatchRuntimeSkeletonPage0002, 47 },
    { 304, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateDefaultCacheStrategy:System.Void()", DispatchRuntimeSkeletonPage0002, 48 },
    { 305, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateObject:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0002, 49 },
    { 306, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateObject:System.Void(P0, P1, P2, P3)", DispatchRuntimeSkeletonPage0002, 50 },
    { 307, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::GetOrCreateIUnknownStrategy:System.Void()", DispatchRuntimeSkeletonPage0002, 51 },
    { 308, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::GetOrCreateInterfaceDetailsStrategy:System.Void()", DispatchRuntimeSkeletonPage0002, 52 },
    { 309, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::ReleaseObjects:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 53 },
    { 310, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownInterfaceDetailsStrategy:System.Void()", DispatchRuntimeSkeletonPage0002, 54 },
    { 311, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownStrategy:System.Void()", DispatchRuntimeSkeletonPage0002, 55 },
    { 312, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::set_DefaultIUnknownInterfaceDetailsStrategy:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 56 },
    { 313, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::set_DefaultIUnknownStrategy:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 57 },
    { 314, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToManaged:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 58 },
    { 315, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 59 },
    { 316, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::Free:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 60 },
    { 317, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::ConvertToManaged:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 61 },
    { 318, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 62 },
    { 319, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::Free:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 63 },
    { 320, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::GetPinnableReference:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 64 },
    { 321, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller::ConvertToManaged:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 65 },
    { 322, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller::ConvertToUnmanaged:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 66 },
    { 323, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller::Free:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 67 },
    { 324, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::Free:System.Void()", DispatchRuntimeSkeletonPage0002, 68 },
    { 325, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0002, 69 },
    { 326, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::ToUnmanaged:System.Void()", DispatchRuntimeSkeletonPage0002, 70 },
    { 327, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::get_BufferSize:System.Void()", DispatchRuntimeSkeletonPage0002, 71 },
    { 328, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 72 },
    { 329, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::VirtualMethodTableInfo::Void(System.Object, System.Object)", DispatchRuntimeSkeletonPage0002, 73 },
    { 330, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::Deconstruct:System.Void(P0, P1)", DispatchRuntimeSkeletonPage0002, 74 },
    { 331, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_ThisPointer:System.Void()", DispatchRuntimeSkeletonPage0002, 75 },
    { 332, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_VirtualMethodTable:System.Void()", DispatchRuntimeSkeletonPage0002, 76 },
    { 333, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::set_ThisPointer:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 77 },
    { 334, "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::set_VirtualMethodTable:System.Void(P0)", DispatchRuntimeSkeletonPage0002, 78 },
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

}  // namespace runtime_skeleton_native_memory_pointers

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReferenceAssembly_native_memory_pointers(
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
    auto* domain = md::FindDomainByName(runtime_skeleton_native_memory_pointers::kAssemblyName);
    if (domain == nullptr) { domain = md::FindDomainById(chaos::il2cpp::bootstrap::GetAotDomainId()); }
    md::DomainScope domain_scope(domain);

    CodeRegistrationV0 local_code_registration = *code_registration;
    const BridgeStatus register_status = bridge->register_codegen(&local_code_registration, metadata_registration, options);
    if (register_status != CHAOS_BRIDGE_STATUS_OK) { return register_status; }
    const BridgeStatus bootstrap_status = bridge->bootstrap_runtime();
    if (bootstrap_status != CHAOS_BRIDGE_STATUS_OK) { return bootstrap_status; }
    if (managed_args == nullptr) { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }
    const auto* request = static_cast<const runtime_skeleton_native_memory_pointers::NativeReferenceAssemblyDispatchRequest*>(managed_args);
    if (request->method_id != 0) {
        return runtime_skeleton_native_memory_pointers::DispatchAssemblyMethodId(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->method_id, request->managed_args);
    }
    return runtime_skeleton_native_memory_pointers::DispatchAssemblySubject(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->subject_id, request->managed_args);
}

int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReference_native_memory_pointers(
    const CodegenBridgeV0* bridge,
    const CodeRegistrationV0* code_registration,
    const MetadataRegistrationV0* metadata_registration,
    const CodegenRegistrationOptionsV0* options,
    RuntimeState* runtime,
    ThreadState* thread,
    void* managed_args) {
    return RunNativeReferenceAssembly_native_memory_pointers(bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);
}
}  // namespace chaos::il2cpp::generated
