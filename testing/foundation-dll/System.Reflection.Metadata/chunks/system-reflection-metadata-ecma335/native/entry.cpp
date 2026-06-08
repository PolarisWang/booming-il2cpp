// entry.cpp — Auto-generated
// CLI entry point for native-AOT verification dispatch.
//
// Modes:
//   (no args)            — fact: run all subject entries, print Passed: N/M
//   --benchmark N I      — benchmark method N for I iterations
//   --benchmark-all I    — benchmark ALL methods for I iterations, aggregated JSON
//   --hotupdate          — hotpatch fact: baseline + apply + semantic-check + revert
//   --hotupdate-and-benchmark N I — post-patch benchmark
//   --microbench         — interpreter microbenchmarks
//   --list               — list all subjects with their indices
//   --fact-json          — per-method JSON output for value-level verification
//   --profile             — profile mode: GC stats, alloc volume, heap delta, code size

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <cstdint>
#include <cinttypes>
#include <memory>

#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#endif

namespace chaos { namespace il2cpp { namespace common { namespace log_internal {
bool g_log_use_stderr = false;
}}}}

#include <chaos/config.h>
#include <chaos/native_types.h>
#include <chaos/profile.h>
#include <runtime_core.h>

#include "chaos_runtime_host.h"

#include <chaos/hotpatch_dispatch.h>
#include <patch_loader.h>
#include <profile_stats.h>

extern "C" const int kAotMethodCount;
extern "C" const int kSubjectEntryCount;
extern "C" const int kSubjectSlotMap[];

extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept;

extern "C" const CodeRegistrationV0 chaos_codegen_code_registration;
extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration;
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options;
extern "C" void ChaosRegisterGcLayouts();

#define CHAOS_USE_DEFAULT_THUNKS nullptr

struct BenchmarkResult {
    double elapsed_ms;
    int64_t allocated_bytes;
};

extern "C" CHAOS_IL2CPP_INT32 RunFactAll();
extern "C" BenchmarkResult RunBenchmark(int entry_index, int iterations);
extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll();
extern "C" BenchmarkResult RunHotpatchBenchmark(int entry_index, int iterations);

extern const uint8_t kPatchData[];
extern const size_t kPatchDataSize;
extern const char* const kPatchDataHostClassName;

extern "C" void RunMicrobench();

// Subject metadata table for --list / --fact-json modes
struct SubjectEntry {
    int index;
    const char* subjectId;
    const char* assemblyName;
    const char* typeName;
    const char* methodName;
    bool isStatic;
    const char* kind;
};

static const SubjectEntry kSubjectEntries[] = {
    { 0, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ArrayShapeEncoder::Shape:System.Void(System.Int32,System.Collections.Immutable.ImmutableArray<System.Int32>,System.Collections.Immutable.ImmutableArray<System.Int32>)", "codegen", "System.Reflection.Metadata.Ecma335.ArrayShapeEncoder", "Shape", True, "benchmark" },
    { 1, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ArrayShapeEncoder::Shape:System.Void(System.Int32,System.Collections.Immutable.ImmutableArray<System.Int32>,System.Collections.Immutable.ImmutableArray<System.Int32>)", "codegen", "System.Reflection.Metadata.Ecma335.ArrayShapeEncoder", "Shape", True, "benchmark" },
    { 2, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::Field:System.Reflection.Metadata.Ecma335.FieldTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "Field", True, "benchmark" },
    { 3, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::FieldSignature:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "FieldSignature", True, "benchmark" },
    { 4, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::MethodSpecificationSignature:System.Reflection.Metadata.Ecma335.GenericTypeArgumentsEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "MethodSpecificationSignature", True, "benchmark" },
    { 5, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::MethodSpecificationSignature:System.Reflection.Metadata.Ecma335.GenericTypeArgumentsEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "MethodSpecificationSignature", True, "benchmark" },
    { 6, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::MethodSignature:System.Reflection.Metadata.Ecma335.MethodSignatureEncoder(System.Reflection.Metadata.SignatureCallingConvention,System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "MethodSignature", True, "benchmark" },
    { 7, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::MethodSignature:System.Reflection.Metadata.Ecma335.MethodSignatureEncoder(System.Reflection.Metadata.SignatureCallingConvention,System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "MethodSignature", True, "benchmark" },
    { 8, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::MethodSignature:System.Reflection.Metadata.Ecma335.MethodSignatureEncoder(System.Reflection.Metadata.SignatureCallingConvention,System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "MethodSignature", True, "benchmark" },
    { 9, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::PropertySignature:System.Reflection.Metadata.Ecma335.MethodSignatureEncoder(System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "PropertySignature", True, "benchmark" },
    { 10, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::PropertySignature:System.Reflection.Metadata.Ecma335.MethodSignatureEncoder(System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "PropertySignature", True, "benchmark" },
    { 11, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::CustomAttributeSignature:System.Void(System.Reflection.Metadata.Ecma335.FixedArgumentsEncoder,System.Reflection.Metadata.Ecma335.CustomAttributeNamedArgumentsEncoder)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "CustomAttributeSignature", True, "benchmark" },
    { 12, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::CustomAttributeSignature:System.Void(System.Action<System.Reflection.Metadata.Ecma335.FixedArgumentsEncoder>,System.Action<System.Reflection.Metadata.Ecma335.CustomAttributeNamedArgumentsEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "CustomAttributeSignature", True, "benchmark" },
    { 13, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::CustomAttributeSignature:System.Void(System.Action<System.Reflection.Metadata.Ecma335.FixedArgumentsEncoder>,System.Action<System.Reflection.Metadata.Ecma335.CustomAttributeNamedArgumentsEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "CustomAttributeSignature", True, "benchmark" },
    { 14, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::LocalVariableSignature:System.Reflection.Metadata.Ecma335.LocalVariablesEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "LocalVariableSignature", True, "benchmark" },
    { 15, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::LocalVariableSignature:System.Reflection.Metadata.Ecma335.LocalVariablesEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "LocalVariableSignature", True, "benchmark" },
    { 16, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::TypeSpecificationSignature:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "TypeSpecificationSignature", True, "benchmark" },
    { 17, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::PermissionSetBlob:System.Reflection.Metadata.Ecma335.PermissionSetEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "PermissionSetBlob", True, "benchmark" },
    { 18, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::PermissionSetBlob:System.Reflection.Metadata.Ecma335.PermissionSetEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "PermissionSetBlob", True, "benchmark" },
    { 19, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::PermissionSetArguments:System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "PermissionSetArguments", True, "benchmark" },
    { 20, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.BlobEncoder::PermissionSetArguments:System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.BlobEncoder", "PermissionSetArguments", True, "benchmark" },
    { 21, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::HasCustomAttribute:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "HasCustomAttribute", True, "benchmark" },
    { 22, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::HasConstant:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "HasConstant", True, "benchmark" },
    { 23, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::CustomAttributeType:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "CustomAttributeType", True, "benchmark" },
    { 24, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::HasDeclSecurity:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "HasDeclSecurity", True, "benchmark" },
    { 25, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::HasFieldMarshal:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "HasFieldMarshal", True, "benchmark" },
    { 26, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::HasSemantics:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "HasSemantics", True, "benchmark" },
    { 27, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::Implementation:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "Implementation", True, "benchmark" },
    { 28, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::MemberForwarded:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "MemberForwarded", True, "benchmark" },
    { 29, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::MemberRefParent:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "MemberRefParent", True, "benchmark" },
    { 30, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::MethodDefOrRef:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "MethodDefOrRef", True, "benchmark" },
    { 31, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::ResolutionScope:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "ResolutionScope", True, "benchmark" },
    { 32, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::TypeDefOrRef:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "TypeDefOrRef", True, "benchmark" },
    { 33, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::TypeDefOrRefOrSpec:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "TypeDefOrRefOrSpec", True, "benchmark" },
    { 34, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::TypeOrMethodDef:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "TypeOrMethodDef", True, "benchmark" },
    { 35, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CodedIndex::HasCustomDebugInformation:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.CodedIndex", "HasCustomDebugInformation", True, "benchmark" },
    { 36, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ControlFlowBuilder::Clear:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.ControlFlowBuilder", "Clear", True, "benchmark" },
    { 37, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ControlFlowBuilder::AddFinallyRegion:System.Void(System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle)", "codegen", "System.Reflection.Metadata.Ecma335.ControlFlowBuilder", "AddFinallyRegion", True, "benchmark" },
    { 38, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ControlFlowBuilder::AddFaultRegion:System.Void(System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle)", "codegen", "System.Reflection.Metadata.Ecma335.ControlFlowBuilder", "AddFaultRegion", True, "benchmark" },
    { 39, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ControlFlowBuilder::AddCatchRegion:System.Void(System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.ControlFlowBuilder", "AddCatchRegion", True, "benchmark" },
    { 40, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ControlFlowBuilder::AddFilterRegion:System.Void(System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle,System.Reflection.Metadata.Ecma335.LabelHandle)", "codegen", "System.Reflection.Metadata.Ecma335.ControlFlowBuilder", "AddFilterRegion", True, "benchmark" },
    { 41, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeArrayTypeEncoder::ObjectArray:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeArrayTypeEncoder", "ObjectArray", True, "benchmark" },
    { 42, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeArrayTypeEncoder::ElementType:System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeArrayTypeEncoder", "ElementType", True, "benchmark" },
    { 43, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::Boolean:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "Boolean", True, "benchmark" },
    { 44, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::Char:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "Char", True, "benchmark" },
    { 45, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::SByte:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "SByte", True, "benchmark" },
    { 46, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::Byte:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "Byte", True, "benchmark" },
    { 47, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::Int16:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "Int16", True, "benchmark" },
    { 48, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::UInt16:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "UInt16", True, "benchmark" },
    { 49, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::Int32:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "Int32", True, "benchmark" },
    { 50, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::UInt32:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "UInt32", True, "benchmark" },
    { 51, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::Int64:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "Int64", True, "benchmark" },
    { 52, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::UInt64:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "UInt64", True, "benchmark" },
    { 53, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::Single:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "Single", True, "benchmark" },
    { 54, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::Double:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "Double", True, "benchmark" },
    { 55, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::String:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "String", True, "benchmark" },
    { 56, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::PrimitiveType:System.Void(System.Reflection.Metadata.PrimitiveSerializationTypeCode)", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "PrimitiveType", True, "benchmark" },
    { 57, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::SystemType:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "SystemType", True, "benchmark" },
    { 58, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::Enum:System.Void(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "Enum", True, "benchmark" },
    { 59, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder::Enum:System.Void(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder", "Enum", True, "benchmark" },
    { 60, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeNamedArgumentsEncoder::Count:System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeNamedArgumentsEncoder", "Count", True, "benchmark" },
    { 61, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomAttributeNamedArgumentsEncoder::Count:System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.CustomAttributeNamedArgumentsEncoder", "Count", True, "benchmark" },
    { 62, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomModifiersEncoder::AddModifier:System.Reflection.Metadata.Ecma335.CustomModifiersEncoder(System.Reflection.Metadata.EntityHandle,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.CustomModifiersEncoder", "AddModifier", True, "benchmark" },
    { 63, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.CustomModifiersEncoder::AddModifier:System.Reflection.Metadata.Ecma335.CustomModifiersEncoder(System.Reflection.Metadata.EntityHandle,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.CustomModifiersEncoder", "AddModifier", True, "benchmark" },
    { 64, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::IsSmallRegionCount:System.Boolean(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "IsSmallRegionCount", True, "benchmark" },
    { 65, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::IsSmallRegionCount:System.Boolean(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "IsSmallRegionCount", True, "benchmark" },
    { 66, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::IsSmallExceptionRegion:System.Boolean(System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "IsSmallExceptionRegion", True, "benchmark" },
    { 67, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::IsSmallExceptionRegion:System.Boolean(System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "IsSmallExceptionRegion", True, "benchmark" },
    { 68, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::IsSmallExceptionRegion:System.Boolean(System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "IsSmallExceptionRegion", True, "benchmark" },
    { 69, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFinally:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFinally", True, "benchmark" },
    { 70, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFinally:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFinally", True, "benchmark" },
    { 71, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFinally:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFinally", True, "benchmark" },
    { 72, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFinally:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFinally", True, "benchmark" },
    { 73, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFault:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFault", True, "benchmark" },
    { 74, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFault:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFault", True, "benchmark" },
    { 75, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFault:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFault", True, "benchmark" },
    { 76, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFault:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFault", True, "benchmark" },
    { 77, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddCatch:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddCatch", True, "benchmark" },
    { 78, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddCatch:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddCatch", True, "benchmark" },
    { 79, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddCatch:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddCatch", True, "benchmark" },
    { 80, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddCatch:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddCatch", True, "benchmark" },
    { 81, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFilter:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFilter", True, "benchmark" },
    { 82, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFilter:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFilter", True, "benchmark" },
    { 83, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFilter:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFilter", True, "benchmark" },
    { 84, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::AddFilter:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "AddFilter", True, "benchmark" },
    { 85, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::Add:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Reflection.Metadata.ExceptionRegionKind,System.Int32,System.Int32,System.Int32,System.Int32,System.Reflection.Metadata.EntityHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "Add", True, "benchmark" },
    { 86, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::Add:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Reflection.Metadata.ExceptionRegionKind,System.Int32,System.Int32,System.Int32,System.Int32,System.Reflection.Metadata.EntityHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "Add", True, "benchmark" },
    { 87, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder::Add:System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder(System.Reflection.Metadata.ExceptionRegionKind,System.Int32,System.Int32,System.Int32,System.Int32,System.Reflection.Metadata.EntityHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.ExceptionRegionEncoder", "Add", True, "benchmark" },
    { 88, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ExportedTypeExtensions::GetTypeDefinitionId:System.Int32(System.Reflection.Metadata.ExportedType)", "codegen", "System.Reflection.Metadata.Ecma335.ExportedTypeExtensions", "GetTypeDefinitionId", True, "benchmark" },
    { 89, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.FieldTypeEncoder::CustomModifiers:System.Reflection.Metadata.Ecma335.CustomModifiersEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.FieldTypeEncoder", "CustomModifiers", True, "benchmark" },
    { 90, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.FieldTypeEncoder::Type:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder(System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.FieldTypeEncoder", "Type", True, "benchmark" },
    { 91, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.FieldTypeEncoder::Type:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder(System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.FieldTypeEncoder", "Type", True, "benchmark" },
    { 92, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.FieldTypeEncoder::TypedReference:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.FieldTypeEncoder", "TypedReference", True, "benchmark" },
    { 93, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.FixedArgumentsEncoder::AddArgument:System.Reflection.Metadata.Ecma335.LiteralEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.FixedArgumentsEncoder", "AddArgument", True, "benchmark" },
    { 94, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.GenericTypeArgumentsEncoder::AddArgument:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.GenericTypeArgumentsEncoder", "AddArgument", True, "benchmark" },
    { 95, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::OpCode:System.Void(System.Reflection.Metadata.ILOpCode)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "OpCode", True, "benchmark" },
    { 96, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::Token:System.Void(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "Token", True, "benchmark" },
    { 97, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::Token:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "Token", True, "benchmark" },
    { 98, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::Token:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "Token", True, "benchmark" },
    { 99, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadString:System.Void(System.Reflection.Metadata.UserStringHandle)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadString", True, "benchmark" },
    { 100, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::Call:System.Void(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "Call", True, "benchmark" },
    { 101, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::Call:System.Void(System.Reflection.Metadata.MethodDefinitionHandle)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "Call", True, "benchmark" },
    { 102, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::Call:System.Void(System.Reflection.Metadata.MethodSpecificationHandle)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "Call", True, "benchmark" },
    { 103, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::Call:System.Void(System.Reflection.Metadata.MemberReferenceHandle)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "Call", True, "benchmark" },
    { 104, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::CallIndirect:System.Void(System.Reflection.Metadata.StandaloneSignatureHandle)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "CallIndirect", True, "benchmark" },
    { 105, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadConstantI4:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadConstantI4", True, "benchmark" },
    { 106, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadConstantI4:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadConstantI4", True, "benchmark" },
    { 107, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadConstantI8:System.Void(System.Int64)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadConstantI8", True, "benchmark" },
    { 108, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadConstantI8:System.Void(System.Int64)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadConstantI8", True, "benchmark" },
    { 109, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadConstantR4:System.Void(System.Single)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadConstantR4", True, "benchmark" },
    { 110, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadConstantR4:System.Void(System.Single)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadConstantR4", True, "benchmark" },
    { 111, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadConstantR8:System.Void(System.Double)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadConstantR8", True, "benchmark" },
    { 112, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadConstantR8:System.Void(System.Double)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadConstantR8", True, "benchmark" },
    { 113, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadLocal:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadLocal", True, "benchmark" },
    { 114, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadLocal:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadLocal", True, "benchmark" },
    { 115, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::StoreLocal:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "StoreLocal", True, "benchmark" },
    { 116, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::StoreLocal:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "StoreLocal", True, "benchmark" },
    { 117, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadLocalAddress:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadLocalAddress", True, "benchmark" },
    { 118, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadLocalAddress:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadLocalAddress", True, "benchmark" },
    { 119, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadArgument:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadArgument", True, "benchmark" },
    { 120, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadArgument:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadArgument", True, "benchmark" },
    { 121, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadArgumentAddress:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadArgumentAddress", True, "benchmark" },
    { 122, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::LoadArgumentAddress:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "LoadArgumentAddress", True, "benchmark" },
    { 123, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::StoreArgument:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "StoreArgument", True, "benchmark" },
    { 124, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::StoreArgument:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "StoreArgument", True, "benchmark" },
    { 125, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::DefineLabel:System.Reflection.Metadata.Ecma335.LabelHandle()", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "DefineLabel", True, "benchmark" },
    { 126, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::Branch:System.Void(System.Reflection.Metadata.ILOpCode,System.Reflection.Metadata.Ecma335.LabelHandle)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "Branch", True, "benchmark" },
    { 127, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::Switch:System.Reflection.Metadata.Ecma335.SwitchInstructionEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "Switch", True, "benchmark" },
    { 128, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::Switch:System.Reflection.Metadata.Ecma335.SwitchInstructionEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "Switch", True, "benchmark" },
    { 129, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.InstructionEncoder::MarkLabel:System.Void(System.Reflection.Metadata.Ecma335.LabelHandle)", "codegen", "System.Reflection.Metadata.Ecma335.InstructionEncoder", "MarkLabel", True, "benchmark" },
    { 130, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LiteralEncoder::Vector:System.Reflection.Metadata.Ecma335.VectorEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.LiteralEncoder", "Vector", True, "benchmark" },
    { 131, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LiteralEncoder::TaggedVector:System.Void(System.Reflection.Metadata.Ecma335.CustomAttributeArrayTypeEncoder,System.Reflection.Metadata.Ecma335.VectorEncoder)", "codegen", "System.Reflection.Metadata.Ecma335.LiteralEncoder", "TaggedVector", True, "benchmark" },
    { 132, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LiteralEncoder::TaggedVector:System.Void(System.Action<System.Reflection.Metadata.Ecma335.CustomAttributeArrayTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.VectorEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.LiteralEncoder", "TaggedVector", True, "benchmark" },
    { 133, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LiteralEncoder::TaggedVector:System.Void(System.Action<System.Reflection.Metadata.Ecma335.CustomAttributeArrayTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.VectorEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.LiteralEncoder", "TaggedVector", True, "benchmark" },
    { 134, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LiteralEncoder::Scalar:System.Reflection.Metadata.Ecma335.ScalarEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.LiteralEncoder", "Scalar", True, "benchmark" },
    { 135, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LiteralEncoder::TaggedScalar:System.Void(System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder,System.Reflection.Metadata.Ecma335.ScalarEncoder)", "codegen", "System.Reflection.Metadata.Ecma335.LiteralEncoder", "TaggedScalar", True, "benchmark" },
    { 136, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LiteralEncoder::TaggedScalar:System.Void(System.Action<System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.ScalarEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.LiteralEncoder", "TaggedScalar", True, "benchmark" },
    { 137, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LiteralEncoder::TaggedScalar:System.Void(System.Action<System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.ScalarEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.LiteralEncoder", "TaggedScalar", True, "benchmark" },
    { 138, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LiteralsEncoder::AddLiteral:System.Reflection.Metadata.Ecma335.LiteralEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.LiteralsEncoder", "AddLiteral", True, "benchmark" },
    { 139, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LocalVariablesEncoder::AddVariable:System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.LocalVariablesEncoder", "AddVariable", True, "benchmark" },
    { 140, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder::CustomModifiers:System.Reflection.Metadata.Ecma335.CustomModifiersEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder", "CustomModifiers", True, "benchmark" },
    { 141, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder::Type:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder(System.Boolean,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder", "Type", True, "benchmark" },
    { 142, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder::Type:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder(System.Boolean,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder", "Type", True, "benchmark" },
    { 143, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder::Type:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder(System.Boolean,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder", "Type", True, "benchmark" },
    { 144, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder::TypedReference:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.LocalVariableTypeEncoder", "TypedReference", True, "benchmark" },
    { 145, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataAggregator::GetGenerationHandle:System.Reflection.Metadata.Handle(System.Reflection.Metadata.Handle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataAggregator", "GetGenerationHandle", True, "benchmark" },
    { 146, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataAggregator::GetGenerationHandle:System.Reflection.Metadata.Handle(System.Reflection.Metadata.Handle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataAggregator", "GetGenerationHandle", True, "benchmark" },
    { 147, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::SetCapacity:System.Void(System.Reflection.Metadata.Ecma335.TableIndex,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "SetCapacity", True, "benchmark" },
    { 148, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::SetCapacity:System.Void(System.Reflection.Metadata.Ecma335.TableIndex,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "SetCapacity", True, "benchmark" },
    { 149, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetRowCount:System.Int32(System.Reflection.Metadata.Ecma335.TableIndex)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetRowCount", True, "benchmark" },
    { 150, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetRowCounts:System.Collections.Immutable.ImmutableArray<System.Int32>()", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetRowCounts", True, "benchmark" },
    { 151, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddModule:System.Reflection.Metadata.ModuleDefinitionHandle(System.Int32,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.GuidHandle,System.Reflection.Metadata.GuidHandle,System.Reflection.Metadata.GuidHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddModule", True, "benchmark" },
    { 152, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddModule:System.Reflection.Metadata.ModuleDefinitionHandle(System.Int32,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.GuidHandle,System.Reflection.Metadata.GuidHandle,System.Reflection.Metadata.GuidHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddModule", True, "benchmark" },
    { 153, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddAssembly:System.Reflection.Metadata.AssemblyDefinitionHandle(System.Reflection.Metadata.StringHandle,System.Version,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle,System.Reflection.AssemblyFlags,System.Reflection.AssemblyHashAlgorithm)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddAssembly", True, "benchmark" },
    { 154, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddAssemblyReference:System.Reflection.Metadata.AssemblyReferenceHandle(System.Reflection.Metadata.StringHandle,System.Version,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle,System.Reflection.AssemblyFlags,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddAssemblyReference", True, "benchmark" },
    { 155, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddTypeDefinition:System.Reflection.Metadata.TypeDefinitionHandle(System.Reflection.TypeAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.FieldDefinitionHandle,System.Reflection.Metadata.MethodDefinitionHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddTypeDefinition", True, "benchmark" },
    { 156, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddTypeDefinition:System.Reflection.Metadata.TypeDefinitionHandle(System.Reflection.TypeAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.FieldDefinitionHandle,System.Reflection.Metadata.MethodDefinitionHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddTypeDefinition", True, "benchmark" },
    { 157, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddTypeLayout:System.Void(System.Reflection.Metadata.TypeDefinitionHandle,System.UInt16,System.UInt32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddTypeLayout", True, "benchmark" },
    { 158, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddTypeLayout:System.Void(System.Reflection.Metadata.TypeDefinitionHandle,System.UInt16,System.UInt32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddTypeLayout", True, "benchmark" },
    { 159, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddTypeLayout:System.Void(System.Reflection.Metadata.TypeDefinitionHandle,System.UInt16,System.UInt32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddTypeLayout", True, "benchmark" },
    { 160, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddInterfaceImplementation:System.Reflection.Metadata.InterfaceImplementationHandle(System.Reflection.Metadata.TypeDefinitionHandle,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddInterfaceImplementation", True, "benchmark" },
    { 161, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddNestedType:System.Void(System.Reflection.Metadata.TypeDefinitionHandle,System.Reflection.Metadata.TypeDefinitionHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddNestedType", True, "benchmark" },
    { 162, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddTypeReference:System.Reflection.Metadata.TypeReferenceHandle(System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.StringHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddTypeReference", True, "benchmark" },
    { 163, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddTypeSpecification:System.Reflection.Metadata.TypeSpecificationHandle(System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddTypeSpecification", True, "benchmark" },
    { 164, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddStandaloneSignature:System.Reflection.Metadata.StandaloneSignatureHandle(System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddStandaloneSignature", True, "benchmark" },
    { 165, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddProperty:System.Reflection.Metadata.PropertyDefinitionHandle(System.Reflection.PropertyAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddProperty", True, "benchmark" },
    { 166, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddProperty:System.Reflection.Metadata.PropertyDefinitionHandle(System.Reflection.PropertyAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddProperty", True, "benchmark" },
    { 167, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddPropertyMap:System.Void(System.Reflection.Metadata.TypeDefinitionHandle,System.Reflection.Metadata.PropertyDefinitionHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddPropertyMap", True, "benchmark" },
    { 168, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddEvent:System.Reflection.Metadata.EventDefinitionHandle(System.Reflection.EventAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddEvent", True, "benchmark" },
    { 169, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddEvent:System.Reflection.Metadata.EventDefinitionHandle(System.Reflection.EventAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddEvent", True, "benchmark" },
    { 170, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddEventMap:System.Void(System.Reflection.Metadata.TypeDefinitionHandle,System.Reflection.Metadata.EventDefinitionHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddEventMap", True, "benchmark" },
    { 171, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddConstant:System.Reflection.Metadata.ConstantHandle(System.Reflection.Metadata.EntityHandle,System.Object)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddConstant", True, "benchmark" },
    { 172, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMethodSemantics:System.Void(System.Reflection.Metadata.EntityHandle,System.Reflection.MethodSemanticsAttributes,System.Reflection.Metadata.MethodDefinitionHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMethodSemantics", True, "benchmark" },
    { 173, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMethodSemantics:System.Void(System.Reflection.Metadata.EntityHandle,System.Reflection.MethodSemanticsAttributes,System.Reflection.Metadata.MethodDefinitionHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMethodSemantics", True, "benchmark" },
    { 174, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddCustomAttribute:System.Reflection.Metadata.CustomAttributeHandle(System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddCustomAttribute", True, "benchmark" },
    { 175, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMethodSpecification:System.Reflection.Metadata.MethodSpecificationHandle(System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMethodSpecification", True, "benchmark" },
    { 176, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddModuleReference:System.Reflection.Metadata.ModuleReferenceHandle(System.Reflection.Metadata.StringHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddModuleReference", True, "benchmark" },
    { 177, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddParameter:System.Reflection.Metadata.ParameterHandle(System.Reflection.ParameterAttributes,System.Reflection.Metadata.StringHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddParameter", True, "benchmark" },
    { 178, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddParameter:System.Reflection.Metadata.ParameterHandle(System.Reflection.ParameterAttributes,System.Reflection.Metadata.StringHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddParameter", True, "benchmark" },
    { 179, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddParameter:System.Reflection.Metadata.ParameterHandle(System.Reflection.ParameterAttributes,System.Reflection.Metadata.StringHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddParameter", True, "benchmark" },
    { 180, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddGenericParameter:System.Reflection.Metadata.GenericParameterHandle(System.Reflection.Metadata.EntityHandle,System.Reflection.GenericParameterAttributes,System.Reflection.Metadata.StringHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddGenericParameter", True, "benchmark" },
    { 181, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddGenericParameter:System.Reflection.Metadata.GenericParameterHandle(System.Reflection.Metadata.EntityHandle,System.Reflection.GenericParameterAttributes,System.Reflection.Metadata.StringHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddGenericParameter", True, "benchmark" },
    { 182, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddGenericParameterConstraint:System.Reflection.Metadata.GenericParameterConstraintHandle(System.Reflection.Metadata.GenericParameterHandle,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddGenericParameterConstraint", True, "benchmark" },
    { 183, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddFieldDefinition:System.Reflection.Metadata.FieldDefinitionHandle(System.Reflection.FieldAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddFieldDefinition", True, "benchmark" },
    { 184, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddFieldDefinition:System.Reflection.Metadata.FieldDefinitionHandle(System.Reflection.FieldAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddFieldDefinition", True, "benchmark" },
    { 185, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddFieldLayout:System.Void(System.Reflection.Metadata.FieldDefinitionHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddFieldLayout", True, "benchmark" },
    { 186, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddFieldLayout:System.Void(System.Reflection.Metadata.FieldDefinitionHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddFieldLayout", True, "benchmark" },
    { 187, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMarshallingDescriptor:System.Void(System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMarshallingDescriptor", True, "benchmark" },
    { 188, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddFieldRelativeVirtualAddress:System.Void(System.Reflection.Metadata.FieldDefinitionHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddFieldRelativeVirtualAddress", True, "benchmark" },
    { 189, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddFieldRelativeVirtualAddress:System.Void(System.Reflection.Metadata.FieldDefinitionHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddFieldRelativeVirtualAddress", True, "benchmark" },
    { 190, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMethodDefinition:System.Reflection.Metadata.MethodDefinitionHandle(System.Reflection.MethodAttributes,System.Reflection.MethodImplAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle,System.Int32,System.Reflection.Metadata.ParameterHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMethodDefinition", True, "benchmark" },
    { 191, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMethodDefinition:System.Reflection.Metadata.MethodDefinitionHandle(System.Reflection.MethodAttributes,System.Reflection.MethodImplAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle,System.Int32,System.Reflection.Metadata.ParameterHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMethodDefinition", True, "benchmark" },
    { 192, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMethodDefinition:System.Reflection.Metadata.MethodDefinitionHandle(System.Reflection.MethodAttributes,System.Reflection.MethodImplAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle,System.Int32,System.Reflection.Metadata.ParameterHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMethodDefinition", True, "benchmark" },
    { 193, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMethodImport:System.Void(System.Reflection.Metadata.MethodDefinitionHandle,System.Reflection.MethodImportAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.ModuleReferenceHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMethodImport", True, "benchmark" },
    { 194, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMethodImplementation:System.Reflection.Metadata.MethodImplementationHandle(System.Reflection.Metadata.TypeDefinitionHandle,System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMethodImplementation", True, "benchmark" },
    { 195, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMemberReference:System.Reflection.Metadata.MemberReferenceHandle(System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMemberReference", True, "benchmark" },
    { 196, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddManifestResource:System.Reflection.Metadata.ManifestResourceHandle(System.Reflection.ManifestResourceAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.EntityHandle,System.UInt32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddManifestResource", True, "benchmark" },
    { 197, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddAssemblyFile:System.Reflection.Metadata.AssemblyFileHandle(System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddAssemblyFile", True, "benchmark" },
    { 198, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddAssemblyFile:System.Reflection.Metadata.AssemblyFileHandle(System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddAssemblyFile", True, "benchmark" },
    { 199, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddExportedType:System.Reflection.Metadata.ExportedTypeHandle(System.Reflection.TypeAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.EntityHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddExportedType", True, "benchmark" },
    { 200, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddExportedType:System.Reflection.Metadata.ExportedTypeHandle(System.Reflection.TypeAttributes,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.EntityHandle,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddExportedType", True, "benchmark" },
    { 201, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddDeclarativeSecurityAttribute:System.Reflection.Metadata.DeclarativeSecurityAttributeHandle(System.Reflection.Metadata.EntityHandle,System.Reflection.DeclarativeSecurityAction,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddDeclarativeSecurityAttribute", True, "benchmark" },
    { 202, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddEncLogEntry:System.Void(System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.Ecma335.EditAndContinueOperation)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddEncLogEntry", True, "benchmark" },
    { 203, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddEncMapEntry:System.Void(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddEncMapEntry", True, "benchmark" },
    { 204, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddDocument:System.Reflection.Metadata.DocumentHandle(System.Reflection.Metadata.BlobHandle,System.Reflection.Metadata.GuidHandle,System.Reflection.Metadata.BlobHandle,System.Reflection.Metadata.GuidHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddDocument", True, "benchmark" },
    { 205, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddMethodDebugInformation:System.Reflection.Metadata.MethodDebugInformationHandle(System.Reflection.Metadata.DocumentHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddMethodDebugInformation", True, "benchmark" },
    { 206, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddLocalScope:System.Reflection.Metadata.LocalScopeHandle(System.Reflection.Metadata.MethodDefinitionHandle,System.Reflection.Metadata.ImportScopeHandle,System.Reflection.Metadata.LocalVariableHandle,System.Reflection.Metadata.LocalConstantHandle,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddLocalScope", True, "benchmark" },
    { 207, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddLocalVariable:System.Reflection.Metadata.LocalVariableHandle(System.Reflection.Metadata.LocalVariableAttributes,System.Int32,System.Reflection.Metadata.StringHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddLocalVariable", True, "benchmark" },
    { 208, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddLocalVariable:System.Reflection.Metadata.LocalVariableHandle(System.Reflection.Metadata.LocalVariableAttributes,System.Int32,System.Reflection.Metadata.StringHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddLocalVariable", True, "benchmark" },
    { 209, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddLocalConstant:System.Reflection.Metadata.LocalConstantHandle(System.Reflection.Metadata.StringHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddLocalConstant", True, "benchmark" },
    { 210, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddImportScope:System.Reflection.Metadata.ImportScopeHandle(System.Reflection.Metadata.ImportScopeHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddImportScope", True, "benchmark" },
    { 211, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddStateMachineMethod:System.Void(System.Reflection.Metadata.MethodDefinitionHandle,System.Reflection.Metadata.MethodDefinitionHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddStateMachineMethod", True, "benchmark" },
    { 212, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::AddCustomDebugInformation:System.Reflection.Metadata.CustomDebugInformationHandle(System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.GuidHandle,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "AddCustomDebugInformation", True, "benchmark" },
    { 213, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::SetCapacity:System.Void(System.Reflection.Metadata.Ecma335.HeapIndex,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "SetCapacity", True, "benchmark" },
    { 214, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::SetCapacity:System.Void(System.Reflection.Metadata.Ecma335.HeapIndex,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "SetCapacity", True, "benchmark" },
    { 215, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddBlob:System.Reflection.Metadata.BlobHandle(System.Reflection.Metadata.BlobBuilder)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddBlob", True, "benchmark" },
    { 216, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddBlob:System.Reflection.Metadata.BlobHandle(System.Byte[])", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddBlob", True, "benchmark" },
    { 217, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddBlob:System.Reflection.Metadata.BlobHandle(System.Byte[])", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddBlob", True, "benchmark" },
    { 218, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddBlob:System.Reflection.Metadata.BlobHandle(System.Byte[])", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddBlob", True, "benchmark" },
    { 219, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddBlob:System.Reflection.Metadata.BlobHandle(System.Collections.Immutable.ImmutableArray<System.Byte>)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddBlob", True, "benchmark" },
    { 220, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddConstantBlob:System.Reflection.Metadata.BlobHandle(System.Object)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddConstantBlob", True, "benchmark" },
    { 221, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddBlobUTF16:System.Reflection.Metadata.BlobHandle(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddBlobUTF16", True, "benchmark" },
    { 222, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddBlobUTF16:System.Reflection.Metadata.BlobHandle(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddBlobUTF16", True, "benchmark" },
    { 223, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddBlobUTF8:System.Reflection.Metadata.BlobHandle(System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddBlobUTF8", True, "benchmark" },
    { 224, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddBlobUTF8:System.Reflection.Metadata.BlobHandle(System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddBlobUTF8", True, "benchmark" },
    { 225, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddBlobUTF8:System.Reflection.Metadata.BlobHandle(System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddBlobUTF8", True, "benchmark" },
    { 226, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddDocumentName:System.Reflection.Metadata.BlobHandle(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddDocumentName", True, "benchmark" },
    { 227, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddDocumentName:System.Reflection.Metadata.BlobHandle(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddDocumentName", True, "benchmark" },
    { 228, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddGuid:System.Reflection.Metadata.GuidHandle(System.Guid)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddGuid", True, "benchmark" },
    { 229, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::ReserveGuid:System.Reflection.Metadata.ReservedBlob<System.Reflection.Metadata.GuidHandle>()", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "ReserveGuid", True, "benchmark" },
    { 230, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddString:System.Reflection.Metadata.StringHandle(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddString", True, "benchmark" },
    { 231, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddString:System.Reflection.Metadata.StringHandle(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddString", True, "benchmark" },
    { 232, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::ReserveUserString:System.Reflection.Metadata.ReservedBlob<System.Reflection.Metadata.UserStringHandle>(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "ReserveUserString", True, "benchmark" },
    { 233, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::ReserveUserString:System.Reflection.Metadata.ReservedBlob<System.Reflection.Metadata.UserStringHandle>(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "ReserveUserString", True, "benchmark" },
    { 234, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddUserString:System.Reflection.Metadata.UserStringHandle(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddUserString", True, "benchmark" },
    { 235, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataBuilder::GetOrAddUserString:System.Reflection.Metadata.UserStringHandle(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataBuilder", "GetOrAddUserString", True, "benchmark" },
    { 236, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetTableRowCount:System.Int32(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.Ecma335.TableIndex)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetTableRowCount", True, "benchmark" },
    { 237, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetTableRowSize:System.Int32(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.Ecma335.TableIndex)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetTableRowSize", True, "benchmark" },
    { 238, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetTableMetadataOffset:System.Int32(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.Ecma335.TableIndex)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetTableMetadataOffset", True, "benchmark" },
    { 239, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetHeapSize:System.Int32(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.Ecma335.HeapIndex)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetHeapSize", True, "benchmark" },
    { 240, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetHeapMetadataOffset:System.Int32(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.Ecma335.HeapIndex)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetHeapMetadataOffset", True, "benchmark" },
    { 241, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetNextHandle:System.Reflection.Metadata.UserStringHandle(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.UserStringHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetNextHandle", True, "benchmark" },
    { 242, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetNextHandle:System.Reflection.Metadata.BlobHandle(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetNextHandle", True, "benchmark" },
    { 243, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetNextHandle:System.Reflection.Metadata.StringHandle(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.StringHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetNextHandle", True, "benchmark" },
    { 244, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetEditAndContinueLogEntries:System.Collections.Generic.IEnumerable<System.Reflection.Metadata.Ecma335.EditAndContinueLogEntry>(System.Reflection.Metadata.MetadataReader)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetEditAndContinueLogEntries", True, "benchmark" },
    { 245, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetEditAndContinueMapEntries:System.Collections.Generic.IEnumerable<System.Reflection.Metadata.EntityHandle>(System.Reflection.Metadata.MetadataReader)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetEditAndContinueMapEntries", True, "benchmark" },
    { 246, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetTypesWithProperties:System.Collections.Generic.IEnumerable<System.Reflection.Metadata.TypeDefinitionHandle>(System.Reflection.Metadata.MetadataReader)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetTypesWithProperties", True, "benchmark" },
    { 247, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::GetTypesWithEvents:System.Collections.Generic.IEnumerable<System.Reflection.Metadata.TypeDefinitionHandle>(System.Reflection.Metadata.MetadataReader)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "GetTypesWithEvents", True, "benchmark" },
    { 248, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::ResolveSignatureTypeKind:System.Reflection.Metadata.SignatureTypeKind(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.EntityHandle,System.Byte)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "ResolveSignatureTypeKind", True, "benchmark" },
    { 249, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataReaderExtensions::ResolveSignatureTypeKind:System.Reflection.Metadata.SignatureTypeKind(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.EntityHandle,System.Byte)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataReaderExtensions", "ResolveSignatureTypeKind", True, "benchmark" },
    { 250, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataRootBuilder::Serialize:System.Void(System.Reflection.Metadata.BlobBuilder,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataRootBuilder", "Serialize", True, "benchmark" },
    { 251, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataRootBuilder::Serialize:System.Void(System.Reflection.Metadata.BlobBuilder,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataRootBuilder", "Serialize", True, "benchmark" },
    { 252, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataRootBuilder::Serialize:System.Void(System.Reflection.Metadata.BlobBuilder,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataRootBuilder", "Serialize", True, "benchmark" },
    { 253, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataSizes::GetAlignedHeapSize:System.Int32(System.Reflection.Metadata.Ecma335.HeapIndex)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataSizes", "GetAlignedHeapSize", True, "benchmark" },
    { 254, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetRowNumber:System.Int32(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetRowNumber", True, "benchmark" },
    { 255, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetHeapOffset:System.Int32(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.Handle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetHeapOffset", True, "benchmark" },
    { 256, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetToken:System.Int32(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetToken", True, "benchmark" },
    { 257, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetToken:System.Int32(System.Reflection.Metadata.MetadataReader,System.Reflection.Metadata.Handle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetToken", True, "benchmark" },
    { 258, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetRowNumber:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetRowNumber", True, "benchmark" },
    { 259, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetHeapOffset:System.Int32(System.Reflection.Metadata.Handle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetHeapOffset", True, "benchmark" },
    { 260, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetHeapOffset:System.Int32(System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetHeapOffset", True, "benchmark" },
    { 261, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetHeapOffset:System.Int32(System.Reflection.Metadata.GuidHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetHeapOffset", True, "benchmark" },
    { 262, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetHeapOffset:System.Int32(System.Reflection.Metadata.UserStringHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetHeapOffset", True, "benchmark" },
    { 263, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetHeapOffset:System.Int32(System.Reflection.Metadata.StringHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetHeapOffset", True, "benchmark" },
    { 264, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetToken:System.Int32(System.Reflection.Metadata.Handle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetToken", True, "benchmark" },
    { 265, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GetToken:System.Int32(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GetToken", True, "benchmark" },
    { 266, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::TryGetTableIndex:System.Boolean(System.Reflection.Metadata.HandleKind,System.Reflection.Metadata.Ecma335.TableIndex)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "TryGetTableIndex", True, "benchmark" },
    { 267, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::TryGetHeapIndex:System.Boolean(System.Reflection.Metadata.HandleKind,System.Reflection.Metadata.Ecma335.HeapIndex)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "TryGetHeapIndex", True, "benchmark" },
    { 268, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::Handle:System.Reflection.Metadata.Handle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "Handle", True, "benchmark" },
    { 269, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::Handle:System.Reflection.Metadata.Handle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "Handle", True, "benchmark" },
    { 270, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::EntityHandle:System.Reflection.Metadata.EntityHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "EntityHandle", True, "benchmark" },
    { 271, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::EntityHandle:System.Reflection.Metadata.EntityHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "EntityHandle", True, "benchmark" },
    { 272, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::EntityHandle:System.Reflection.Metadata.EntityHandle(System.Reflection.Metadata.Ecma335.TableIndex,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "EntityHandle", True, "benchmark" },
    { 273, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::EntityHandle:System.Reflection.Metadata.EntityHandle(System.Reflection.Metadata.Ecma335.TableIndex,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "EntityHandle", True, "benchmark" },
    { 274, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::Handle:System.Reflection.Metadata.EntityHandle(System.Reflection.Metadata.Ecma335.TableIndex,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "Handle", True, "benchmark" },
    { 275, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::Handle:System.Reflection.Metadata.EntityHandle(System.Reflection.Metadata.Ecma335.TableIndex,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "Handle", True, "benchmark" },
    { 276, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::MethodDefinitionHandle:System.Reflection.Metadata.MethodDefinitionHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "MethodDefinitionHandle", True, "benchmark" },
    { 277, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::MethodDefinitionHandle:System.Reflection.Metadata.MethodDefinitionHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "MethodDefinitionHandle", True, "benchmark" },
    { 278, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::MethodImplementationHandle:System.Reflection.Metadata.MethodImplementationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "MethodImplementationHandle", True, "benchmark" },
    { 279, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::MethodImplementationHandle:System.Reflection.Metadata.MethodImplementationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "MethodImplementationHandle", True, "benchmark" },
    { 280, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::MethodSpecificationHandle:System.Reflection.Metadata.MethodSpecificationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "MethodSpecificationHandle", True, "benchmark" },
    { 281, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::MethodSpecificationHandle:System.Reflection.Metadata.MethodSpecificationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "MethodSpecificationHandle", True, "benchmark" },
    { 282, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::TypeDefinitionHandle:System.Reflection.Metadata.TypeDefinitionHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "TypeDefinitionHandle", True, "benchmark" },
    { 283, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::TypeDefinitionHandle:System.Reflection.Metadata.TypeDefinitionHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "TypeDefinitionHandle", True, "benchmark" },
    { 284, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ExportedTypeHandle:System.Reflection.Metadata.ExportedTypeHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ExportedTypeHandle", True, "benchmark" },
    { 285, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ExportedTypeHandle:System.Reflection.Metadata.ExportedTypeHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ExportedTypeHandle", True, "benchmark" },
    { 286, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::TypeReferenceHandle:System.Reflection.Metadata.TypeReferenceHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "TypeReferenceHandle", True, "benchmark" },
    { 287, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::TypeReferenceHandle:System.Reflection.Metadata.TypeReferenceHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "TypeReferenceHandle", True, "benchmark" },
    { 288, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::TypeSpecificationHandle:System.Reflection.Metadata.TypeSpecificationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "TypeSpecificationHandle", True, "benchmark" },
    { 289, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::TypeSpecificationHandle:System.Reflection.Metadata.TypeSpecificationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "TypeSpecificationHandle", True, "benchmark" },
    { 290, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::InterfaceImplementationHandle:System.Reflection.Metadata.InterfaceImplementationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "InterfaceImplementationHandle", True, "benchmark" },
    { 291, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::InterfaceImplementationHandle:System.Reflection.Metadata.InterfaceImplementationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "InterfaceImplementationHandle", True, "benchmark" },
    { 292, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::MemberReferenceHandle:System.Reflection.Metadata.MemberReferenceHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "MemberReferenceHandle", True, "benchmark" },
    { 293, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::MemberReferenceHandle:System.Reflection.Metadata.MemberReferenceHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "MemberReferenceHandle", True, "benchmark" },
    { 294, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::FieldDefinitionHandle:System.Reflection.Metadata.FieldDefinitionHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "FieldDefinitionHandle", True, "benchmark" },
    { 295, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::FieldDefinitionHandle:System.Reflection.Metadata.FieldDefinitionHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "FieldDefinitionHandle", True, "benchmark" },
    { 296, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::EventDefinitionHandle:System.Reflection.Metadata.EventDefinitionHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "EventDefinitionHandle", True, "benchmark" },
    { 297, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::EventDefinitionHandle:System.Reflection.Metadata.EventDefinitionHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "EventDefinitionHandle", True, "benchmark" },
    { 298, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::PropertyDefinitionHandle:System.Reflection.Metadata.PropertyDefinitionHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "PropertyDefinitionHandle", True, "benchmark" },
    { 299, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::PropertyDefinitionHandle:System.Reflection.Metadata.PropertyDefinitionHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "PropertyDefinitionHandle", True, "benchmark" },
    { 300, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::StandaloneSignatureHandle:System.Reflection.Metadata.StandaloneSignatureHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "StandaloneSignatureHandle", True, "benchmark" },
    { 301, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::StandaloneSignatureHandle:System.Reflection.Metadata.StandaloneSignatureHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "StandaloneSignatureHandle", True, "benchmark" },
    { 302, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ParameterHandle:System.Reflection.Metadata.ParameterHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ParameterHandle", True, "benchmark" },
    { 303, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ParameterHandle:System.Reflection.Metadata.ParameterHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ParameterHandle", True, "benchmark" },
    { 304, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GenericParameterHandle:System.Reflection.Metadata.GenericParameterHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GenericParameterHandle", True, "benchmark" },
    { 305, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GenericParameterHandle:System.Reflection.Metadata.GenericParameterHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GenericParameterHandle", True, "benchmark" },
    { 306, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GenericParameterConstraintHandle:System.Reflection.Metadata.GenericParameterConstraintHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GenericParameterConstraintHandle", True, "benchmark" },
    { 307, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GenericParameterConstraintHandle:System.Reflection.Metadata.GenericParameterConstraintHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GenericParameterConstraintHandle", True, "benchmark" },
    { 308, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ModuleReferenceHandle:System.Reflection.Metadata.ModuleReferenceHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ModuleReferenceHandle", True, "benchmark" },
    { 309, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ModuleReferenceHandle:System.Reflection.Metadata.ModuleReferenceHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ModuleReferenceHandle", True, "benchmark" },
    { 310, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::AssemblyReferenceHandle:System.Reflection.Metadata.AssemblyReferenceHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "AssemblyReferenceHandle", True, "benchmark" },
    { 311, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::AssemblyReferenceHandle:System.Reflection.Metadata.AssemblyReferenceHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "AssemblyReferenceHandle", True, "benchmark" },
    { 312, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::CustomAttributeHandle:System.Reflection.Metadata.CustomAttributeHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "CustomAttributeHandle", True, "benchmark" },
    { 313, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::CustomAttributeHandle:System.Reflection.Metadata.CustomAttributeHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "CustomAttributeHandle", True, "benchmark" },
    { 314, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::DeclarativeSecurityAttributeHandle:System.Reflection.Metadata.DeclarativeSecurityAttributeHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "DeclarativeSecurityAttributeHandle", True, "benchmark" },
    { 315, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::DeclarativeSecurityAttributeHandle:System.Reflection.Metadata.DeclarativeSecurityAttributeHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "DeclarativeSecurityAttributeHandle", True, "benchmark" },
    { 316, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ConstantHandle:System.Reflection.Metadata.ConstantHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ConstantHandle", True, "benchmark" },
    { 317, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ConstantHandle:System.Reflection.Metadata.ConstantHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ConstantHandle", True, "benchmark" },
    { 318, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ManifestResourceHandle:System.Reflection.Metadata.ManifestResourceHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ManifestResourceHandle", True, "benchmark" },
    { 319, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ManifestResourceHandle:System.Reflection.Metadata.ManifestResourceHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ManifestResourceHandle", True, "benchmark" },
    { 320, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::AssemblyFileHandle:System.Reflection.Metadata.AssemblyFileHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "AssemblyFileHandle", True, "benchmark" },
    { 321, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::AssemblyFileHandle:System.Reflection.Metadata.AssemblyFileHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "AssemblyFileHandle", True, "benchmark" },
    { 322, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::DocumentHandle:System.Reflection.Metadata.DocumentHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "DocumentHandle", True, "benchmark" },
    { 323, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::DocumentHandle:System.Reflection.Metadata.DocumentHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "DocumentHandle", True, "benchmark" },
    { 324, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::MethodDebugInformationHandle:System.Reflection.Metadata.MethodDebugInformationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "MethodDebugInformationHandle", True, "benchmark" },
    { 325, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::MethodDebugInformationHandle:System.Reflection.Metadata.MethodDebugInformationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "MethodDebugInformationHandle", True, "benchmark" },
    { 326, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::LocalScopeHandle:System.Reflection.Metadata.LocalScopeHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "LocalScopeHandle", True, "benchmark" },
    { 327, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::LocalScopeHandle:System.Reflection.Metadata.LocalScopeHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "LocalScopeHandle", True, "benchmark" },
    { 328, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::LocalVariableHandle:System.Reflection.Metadata.LocalVariableHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "LocalVariableHandle", True, "benchmark" },
    { 329, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::LocalVariableHandle:System.Reflection.Metadata.LocalVariableHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "LocalVariableHandle", True, "benchmark" },
    { 330, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::LocalConstantHandle:System.Reflection.Metadata.LocalConstantHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "LocalConstantHandle", True, "benchmark" },
    { 331, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::LocalConstantHandle:System.Reflection.Metadata.LocalConstantHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "LocalConstantHandle", True, "benchmark" },
    { 332, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ImportScopeHandle:System.Reflection.Metadata.ImportScopeHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ImportScopeHandle", True, "benchmark" },
    { 333, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::ImportScopeHandle:System.Reflection.Metadata.ImportScopeHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "ImportScopeHandle", True, "benchmark" },
    { 334, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::CustomDebugInformationHandle:System.Reflection.Metadata.CustomDebugInformationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "CustomDebugInformationHandle", True, "benchmark" },
    { 335, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::CustomDebugInformationHandle:System.Reflection.Metadata.CustomDebugInformationHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "CustomDebugInformationHandle", True, "benchmark" },
    { 336, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::UserStringHandle:System.Reflection.Metadata.UserStringHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "UserStringHandle", True, "benchmark" },
    { 337, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::UserStringHandle:System.Reflection.Metadata.UserStringHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "UserStringHandle", True, "benchmark" },
    { 338, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::StringHandle:System.Reflection.Metadata.StringHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "StringHandle", True, "benchmark" },
    { 339, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::StringHandle:System.Reflection.Metadata.StringHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "StringHandle", True, "benchmark" },
    { 340, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::BlobHandle:System.Reflection.Metadata.BlobHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "BlobHandle", True, "benchmark" },
    { 341, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::BlobHandle:System.Reflection.Metadata.BlobHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "BlobHandle", True, "benchmark" },
    { 342, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GuidHandle:System.Reflection.Metadata.GuidHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GuidHandle", True, "benchmark" },
    { 343, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::GuidHandle:System.Reflection.Metadata.GuidHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "GuidHandle", True, "benchmark" },
    { 344, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::DocumentNameBlobHandle:System.Reflection.Metadata.DocumentNameBlobHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "DocumentNameBlobHandle", True, "benchmark" },
    { 345, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MetadataTokens::DocumentNameBlobHandle:System.Reflection.Metadata.DocumentNameBlobHandle(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.MetadataTokens", "DocumentNameBlobHandle", True, "benchmark" },
    { 346, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MethodBodyStreamEncoder::AddMethodBody:System.Int32(System.Reflection.Metadata.Ecma335.InstructionEncoder,System.Int32,System.Reflection.Metadata.StandaloneSignatureHandle,System.Reflection.Metadata.Ecma335.MethodBodyAttributes)", "codegen", "System.Reflection.Metadata.Ecma335.MethodBodyStreamEncoder", "AddMethodBody", True, "benchmark" },
    { 347, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MethodBodyStreamEncoder::AddMethodBody:System.Int32(System.Reflection.Metadata.Ecma335.InstructionEncoder,System.Int32,System.Reflection.Metadata.StandaloneSignatureHandle,System.Reflection.Metadata.Ecma335.MethodBodyAttributes)", "codegen", "System.Reflection.Metadata.Ecma335.MethodBodyStreamEncoder", "AddMethodBody", True, "benchmark" },
    { 348, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MethodBodyStreamEncoder::AddMethodBody:System.Int32(System.Reflection.Metadata.Ecma335.InstructionEncoder,System.Int32,System.Reflection.Metadata.StandaloneSignatureHandle,System.Reflection.Metadata.Ecma335.MethodBodyAttributes,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.MethodBodyStreamEncoder", "AddMethodBody", True, "benchmark" },
    { 349, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MethodBodyStreamEncoder::AddMethodBody:System.Int32(System.Reflection.Metadata.Ecma335.InstructionEncoder,System.Int32,System.Reflection.Metadata.StandaloneSignatureHandle,System.Reflection.Metadata.Ecma335.MethodBodyAttributes,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.MethodBodyStreamEncoder", "AddMethodBody", True, "benchmark" },
    { 350, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MethodSignatureEncoder::Parameters:System.Void(System.Int32,System.Reflection.Metadata.Ecma335.ReturnTypeEncoder,System.Reflection.Metadata.Ecma335.ParametersEncoder)", "codegen", "System.Reflection.Metadata.Ecma335.MethodSignatureEncoder", "Parameters", True, "benchmark" },
    { 351, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MethodSignatureEncoder::Parameters:System.Void(System.Int32,System.Reflection.Metadata.Ecma335.ReturnTypeEncoder,System.Reflection.Metadata.Ecma335.ParametersEncoder)", "codegen", "System.Reflection.Metadata.Ecma335.MethodSignatureEncoder", "Parameters", True, "benchmark" },
    { 352, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MethodSignatureEncoder::Parameters:System.Void(System.Int32,System.Action<System.Reflection.Metadata.Ecma335.ReturnTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.ParametersEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.MethodSignatureEncoder", "Parameters", True, "benchmark" },
    { 353, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MethodSignatureEncoder::Parameters:System.Void(System.Int32,System.Action<System.Reflection.Metadata.Ecma335.ReturnTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.ParametersEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.MethodSignatureEncoder", "Parameters", True, "benchmark" },
    { 354, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.MethodSignatureEncoder::Parameters:System.Void(System.Int32,System.Action<System.Reflection.Metadata.Ecma335.ReturnTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.ParametersEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.MethodSignatureEncoder", "Parameters", True, "benchmark" },
    { 355, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder::AddArgument:System.Void(System.Boolean,System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder,System.Reflection.Metadata.Ecma335.NameEncoder,System.Reflection.Metadata.Ecma335.LiteralEncoder)", "codegen", "System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder", "AddArgument", True, "benchmark" },
    { 356, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder::AddArgument:System.Void(System.Boolean,System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder,System.Reflection.Metadata.Ecma335.NameEncoder,System.Reflection.Metadata.Ecma335.LiteralEncoder)", "codegen", "System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder", "AddArgument", True, "benchmark" },
    { 357, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder::AddArgument:System.Void(System.Boolean,System.Action<System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.NameEncoder>,System.Action<System.Reflection.Metadata.Ecma335.LiteralEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder", "AddArgument", True, "benchmark" },
    { 358, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder::AddArgument:System.Void(System.Boolean,System.Action<System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.NameEncoder>,System.Action<System.Reflection.Metadata.Ecma335.LiteralEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder", "AddArgument", True, "benchmark" },
    { 359, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder::AddArgument:System.Void(System.Boolean,System.Action<System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.NameEncoder>,System.Action<System.Reflection.Metadata.Ecma335.LiteralEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.NamedArgumentsEncoder", "AddArgument", True, "benchmark" },
    { 360, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder::ScalarType:System.Reflection.Metadata.Ecma335.CustomAttributeElementTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder", "ScalarType", True, "benchmark" },
    { 361, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder::Object:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder", "Object", True, "benchmark" },
    { 362, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder::SZArray:System.Reflection.Metadata.Ecma335.CustomAttributeArrayTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.NamedArgumentTypeEncoder", "SZArray", True, "benchmark" },
    { 363, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.NameEncoder::Name:System.Void(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.NameEncoder", "Name", True, "benchmark" },
    { 364, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.NameEncoder::Name:System.Void(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.NameEncoder", "Name", True, "benchmark" },
    { 365, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ParametersEncoder::AddParameter:System.Reflection.Metadata.Ecma335.ParameterTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.ParametersEncoder", "AddParameter", True, "benchmark" },
    { 366, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ParametersEncoder::StartVarArgs:System.Reflection.Metadata.Ecma335.ParametersEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.ParametersEncoder", "StartVarArgs", True, "benchmark" },
    { 367, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ParameterTypeEncoder::CustomModifiers:System.Reflection.Metadata.Ecma335.CustomModifiersEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.ParameterTypeEncoder", "CustomModifiers", True, "benchmark" },
    { 368, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ParameterTypeEncoder::Type:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder(System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.ParameterTypeEncoder", "Type", True, "benchmark" },
    { 369, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ParameterTypeEncoder::Type:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder(System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.ParameterTypeEncoder", "Type", True, "benchmark" },
    { 370, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ParameterTypeEncoder::TypedReference:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.ParameterTypeEncoder", "TypedReference", True, "benchmark" },
    { 371, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.PermissionSetEncoder::AddPermission:System.Reflection.Metadata.Ecma335.PermissionSetEncoder(System.String,System.Collections.Immutable.ImmutableArray<System.Byte>)", "codegen", "System.Reflection.Metadata.Ecma335.PermissionSetEncoder", "AddPermission", True, "benchmark" },
    { 372, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.PermissionSetEncoder::AddPermission:System.Reflection.Metadata.Ecma335.PermissionSetEncoder(System.String,System.Collections.Immutable.ImmutableArray<System.Byte>)", "codegen", "System.Reflection.Metadata.Ecma335.PermissionSetEncoder", "AddPermission", True, "benchmark" },
    { 373, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.PermissionSetEncoder::AddPermission:System.Reflection.Metadata.Ecma335.PermissionSetEncoder(System.String,System.Reflection.Metadata.BlobBuilder)", "codegen", "System.Reflection.Metadata.Ecma335.PermissionSetEncoder", "AddPermission", True, "benchmark" },
    { 374, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.PermissionSetEncoder::AddPermission:System.Reflection.Metadata.Ecma335.PermissionSetEncoder(System.String,System.Reflection.Metadata.BlobBuilder)", "codegen", "System.Reflection.Metadata.Ecma335.PermissionSetEncoder", "AddPermission", True, "benchmark" },
    { 375, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.PortablePdbBuilder::Serialize:System.Reflection.Metadata.BlobContentId(System.Reflection.Metadata.BlobBuilder)", "codegen", "System.Reflection.Metadata.Ecma335.PortablePdbBuilder", "Serialize", True, "benchmark" },
    { 376, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ReturnTypeEncoder::CustomModifiers:System.Reflection.Metadata.Ecma335.CustomModifiersEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.ReturnTypeEncoder", "CustomModifiers", True, "benchmark" },
    { 377, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ReturnTypeEncoder::Type:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder(System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.ReturnTypeEncoder", "Type", True, "benchmark" },
    { 378, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ReturnTypeEncoder::Type:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder(System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.ReturnTypeEncoder", "Type", True, "benchmark" },
    { 379, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ReturnTypeEncoder::TypedReference:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.ReturnTypeEncoder", "TypedReference", True, "benchmark" },
    { 380, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ReturnTypeEncoder::Void:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.ReturnTypeEncoder", "Void", True, "benchmark" },
    { 381, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ScalarEncoder::NullArray:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.ScalarEncoder", "NullArray", True, "benchmark" },
    { 382, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ScalarEncoder::Constant:System.Void(System.Object)", "codegen", "System.Reflection.Metadata.Ecma335.ScalarEncoder", "Constant", True, "benchmark" },
    { 383, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ScalarEncoder::SystemType:System.Void(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.ScalarEncoder", "SystemType", True, "benchmark" },
    { 384, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.ScalarEncoder::SystemType:System.Void(System.String)", "codegen", "System.Reflection.Metadata.Ecma335.ScalarEncoder", "SystemType", True, "benchmark" },
    { 385, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>::DecodeType:System.Int32(System.Reflection.Metadata.BlobReader,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>", "DecodeType", True, "benchmark" },
    { 386, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>::DecodeType:System.Int32(System.Reflection.Metadata.BlobReader,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>", "DecodeType", True, "benchmark" },
    { 387, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>::DecodeMethodSignature:System.Reflection.Metadata.MethodSignature<System.Int32>(System.Reflection.Metadata.BlobReader)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>", "DecodeMethodSignature", True, "benchmark" },
    { 388, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>::DecodeMethodSpecificationSignature:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Reflection.Metadata.BlobReader)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>", "DecodeMethodSpecificationSignature", True, "benchmark" },
    { 389, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>::DecodeLocalSignature:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Reflection.Metadata.BlobReader)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>", "DecodeLocalSignature", True, "benchmark" },
    { 390, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>::DecodeFieldSignature:System.Int32(System.Reflection.Metadata.BlobReader)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureDecoder<System.Int32,System.Int32>", "DecodeFieldSignature", True, "benchmark" },
    { 391, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Boolean:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Boolean", True, "benchmark" },
    { 392, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Char:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Char", True, "benchmark" },
    { 393, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::SByte:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "SByte", True, "benchmark" },
    { 394, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Byte:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Byte", True, "benchmark" },
    { 395, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Int16:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Int16", True, "benchmark" },
    { 396, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::UInt16:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "UInt16", True, "benchmark" },
    { 397, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Int32:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Int32", True, "benchmark" },
    { 398, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::UInt32:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "UInt32", True, "benchmark" },
    { 399, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Int64:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Int64", True, "benchmark" },
    { 400, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::UInt64:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "UInt64", True, "benchmark" },
    { 401, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Single:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Single", True, "benchmark" },
    { 402, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Double:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Double", True, "benchmark" },
    { 403, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::String:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "String", True, "benchmark" },
    { 404, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::TypedReference:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "TypedReference", True, "benchmark" },
    { 405, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::IntPtr:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "IntPtr", True, "benchmark" },
    { 406, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::UIntPtr:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "UIntPtr", True, "benchmark" },
    { 407, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Object:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Object", True, "benchmark" },
    { 408, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::PrimitiveType:System.Void(System.Reflection.Metadata.PrimitiveTypeCode)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "PrimitiveType", True, "benchmark" },
    { 409, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Array:System.Void(System.Reflection.Metadata.Ecma335.SignatureTypeEncoder,System.Reflection.Metadata.Ecma335.ArrayShapeEncoder)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Array", True, "benchmark" },
    { 410, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Array:System.Void(System.Action<System.Reflection.Metadata.Ecma335.SignatureTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.ArrayShapeEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Array", True, "benchmark" },
    { 411, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Array:System.Void(System.Action<System.Reflection.Metadata.Ecma335.SignatureTypeEncoder>,System.Action<System.Reflection.Metadata.Ecma335.ArrayShapeEncoder>)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Array", True, "benchmark" },
    { 412, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Type:System.Void(System.Reflection.Metadata.EntityHandle,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Type", True, "benchmark" },
    { 413, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Type:System.Void(System.Reflection.Metadata.EntityHandle,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Type", True, "benchmark" },
    { 414, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::FunctionPointer:System.Reflection.Metadata.Ecma335.MethodSignatureEncoder(System.Reflection.Metadata.SignatureCallingConvention,System.Reflection.Metadata.Ecma335.FunctionPointerAttributes,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "FunctionPointer", True, "benchmark" },
    { 415, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::FunctionPointer:System.Reflection.Metadata.Ecma335.MethodSignatureEncoder(System.Reflection.Metadata.SignatureCallingConvention,System.Reflection.Metadata.Ecma335.FunctionPointerAttributes,System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "FunctionPointer", True, "benchmark" },
    { 416, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::GenericInstantiation:System.Reflection.Metadata.Ecma335.GenericTypeArgumentsEncoder(System.Reflection.Metadata.EntityHandle,System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "GenericInstantiation", True, "benchmark" },
    { 417, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::GenericInstantiation:System.Reflection.Metadata.Ecma335.GenericTypeArgumentsEncoder(System.Reflection.Metadata.EntityHandle,System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "GenericInstantiation", True, "benchmark" },
    { 418, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::GenericInstantiation:System.Reflection.Metadata.Ecma335.GenericTypeArgumentsEncoder(System.Reflection.Metadata.EntityHandle,System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "GenericInstantiation", True, "benchmark" },
    { 419, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::GenericMethodTypeParameter:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "GenericMethodTypeParameter", True, "benchmark" },
    { 420, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::GenericMethodTypeParameter:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "GenericMethodTypeParameter", True, "benchmark" },
    { 421, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::GenericTypeParameter:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "GenericTypeParameter", True, "benchmark" },
    { 422, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::GenericTypeParameter:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "GenericTypeParameter", True, "benchmark" },
    { 423, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::Pointer:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "Pointer", True, "benchmark" },
    { 424, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::VoidPointer:System.Void()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "VoidPointer", True, "benchmark" },
    { 425, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::SZArray:System.Reflection.Metadata.Ecma335.SignatureTypeEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "SZArray", True, "benchmark" },
    { 426, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SignatureTypeEncoder::CustomModifiers:System.Reflection.Metadata.Ecma335.CustomModifiersEncoder()", "codegen", "System.Reflection.Metadata.Ecma335.SignatureTypeEncoder", "CustomModifiers", True, "benchmark" },
    { 427, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.SwitchInstructionEncoder::Branch:System.Void(System.Reflection.Metadata.Ecma335.LabelHandle)", "codegen", "System.Reflection.Metadata.Ecma335.SwitchInstructionEncoder", "Branch", True, "benchmark" },
    { 428, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.VectorEncoder::Count:System.Reflection.Metadata.Ecma335.LiteralsEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.VectorEncoder", "Count", True, "benchmark" },
    { 429, "System.Reflection.Metadata/System.Reflection.Metadata.Ecma335.VectorEncoder::Count:System.Reflection.Metadata.Ecma335.LiteralsEncoder(System.Int32)", "codegen", "System.Reflection.Metadata.Ecma335.VectorEncoder", "Count", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 430;

static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchIfAvailable() {
    if (kPatchDataSize > 0u) {
        auto* patch_ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
            kPatchData, kPatchDataSize, kPatchDataHostClassName);
        if (patch_ctx == nullptr) {
            std::fprintf(stderr, "WARN: ApplyPatchFromMemory returned null (no patches applied)\n");
        } else {
            std::fprintf(stderr, "INFO: Applied patches to %u methods\n",
                         static_cast<unsigned>(patch_ctx->method_count));
        }
        return patch_ctx;
    }
    return nullptr;
}

static int RunFactMode() {
    const int kCount = kSubjectEntryCount;
    int passed_count = 0;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
{% if is_jit %}
        try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) {
            // caught — skip increment
        }
{% else %}
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            ++passed_count;
        CHAOS_EH_CATCH_BEGIN
            // caught — skip increment
        CHAOS_EH_END
{% endif %}
    }
    int failed_count = kCount - passed_count;
    printf("Passed: %d/%d\n", passed_count, kSubjectEntryCount);
    std::fflush(stdout);
    return failed_count;
}

static int RunFactJsonMode() {
    const int kCount = kSubjectEntryCount;
    printf("{\"factResults\":[");
    bool first = true;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t result = 0;
        bool caught = false;
{% if is_jit %}
        try {
            result = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) {
            caught = true;
        }
{% else %}
        CHAOS_EH_TRY
            result = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
            caught = true;
        CHAOS_EH_END
{% end %}
        if (!first) printf(",");
        printf("{\"si\":%d,\"methodIndex\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, i, caught ? "false" : "true", caught ? -1 : result);
        first = false;
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

static int RunListMode() {
    printf("{\"subjects\":[");
    for (int i = 0; i < kSubjectEntryCount_Generated; i++) {
        if (i > 0) printf(",");
        printf("{\"index\":%d,\"subjectId\":\"%s\",\"assemblyName\":\"%s\","
               "\"typeName\":\"%s\",\"methodName\":\"%s\",\"isStatic\":%s,\"kind\":\"%s\"}",
               kSubjectEntries[i].index,
               kSubjectEntries[i].subjectId,
               kSubjectEntries[i].assemblyName,
               kSubjectEntries[i].typeName,
               kSubjectEntries[i].methodName,
               kSubjectEntries[i].isStatic ? "true" : "false",
               kSubjectEntries[i].kind);
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

static int RunBenchmarkMode(int entry_index, int iterations) {
    CHAOS_IL2CPP_PROFILE_SCOPE("RunBenchmarkMode");
    auto result = RunBenchmark(entry_index, iterations);
    if (result.elapsed_ms < 0.0) {
        printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}\n");
        return 1;
    }
    double ns_per_op = (result.elapsed_ms * 1e6) / iterations;
    double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
    double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
    printf(
        "{\"elapsedMilliseconds\":%.3f,\"calibratedMs\":%.3f,"
        "\"opsPerSecond\":%.0f,\"iterations\":%d,"
        "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}\n",
        result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
        result.allocated_bytes, alloc_per_op);
    std::fflush(stdout);
    return 0;
}

// ── --benchmark-all: benchmark ALL subject methods ─────────────────
static int RunBenchmarkAllMode(int iterations) {
    const int kCount = kSubjectEntryCount;
    printf("{\"benchmarkAll\":[");
    bool first = true;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        auto result = RunBenchmark(i, iterations);
        if (!first) printf(",");
        first = false;
        if (result.elapsed_ms < 0.0) {
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}", i);
        } else {
            double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
            double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
            printf(
                "{\"methodIndex\":%d,\"elapsedMilliseconds\":%.3f,"
                "\"opsPerSecond\":%.0f,\"iterations\":%d,"
                "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}",
                i, result.elapsed_ms, ops_per_sec, iterations,
                result.allocated_bytes, alloc_per_op);
        }
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

// ── --benchmark-range: benchmark a contiguous range of method table indices ─
// First argument: start index (inclusive), Second argument: end index (exclusive)
static int RunBenchmarkRangeMode(int iterations, int start_idx, int end_idx) {
    if (start_idx < 0) start_idx = 0;
    if (end_idx > kAotMethodCount) end_idx = kAotMethodCount;
    printf("{\"benchmarkAll\":[");
    bool first = true;
    for (int i = start_idx; i < end_idx; i++) {
        auto result = RunBenchmark(i, iterations);
        if (!first) printf(",");
        first = false;
        if (result.elapsed_ms < 0.0) {
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}", i);
        } else {
            double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
            double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
            printf(
                "{\"methodIndex\":%d,\"elapsedMilliseconds\":%.3f,"
                "\"opsPerSecond\":%.0f,\"iterations\":%d,"
                "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}",
                i, result.elapsed_ms, ops_per_sec, iterations,
                result.allocated_bytes, alloc_per_op);
        }
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

// Load patch data from an external file (used by the verification pipeline).
// Returns nullptr if path is nullptr or file cannot be opened.
static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchFromFile(const char* path) {
    if (path == nullptr) return nullptr;
    FILE* f = fopen(path, "rb");
    if (!f) { std::fprintf(stderr, "WARN: cannot open patch data: %s\n", path); return nullptr; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    uint8_t* data = (uint8_t*)std::malloc(size > 0 ? size : 1);
    if (!data) { fclose(f); return nullptr; }
    size_t read_size = fread(data, 1, size, f);
    fclose(f);
    if (read_size == 0) { std::free(data); return nullptr; }
    auto* ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
        data, read_size, nullptr);
    std::free(data);
    if (ctx == nullptr) {
        std::fprintf(stderr, "WARN: ApplyPatchFromMemory returned null (no patches applied)\n");
    } else {
        std::fprintf(stderr, "INFO: Applied patches to %u methods from %s\n",
                     static_cast<unsigned>(ctx->method_count), path);
    }
    return ctx;
}

static int RunHotupdateMode(const char* patchDataPath = nullptr) {
    const int kCount = kSubjectEntryCount;
    // Dynamic allocation to support arbitrary method counts
    auto baseline_ok = std::make_unique<bool[]>(kCount);
    auto baseline_caught = std::make_unique<bool[]>(kCount);
    auto baseline_value = std::make_unique<int64_t[]>(kCount);
    for (int si = 0; si < kCount; si++) {
        baseline_ok[si] = false;
        baseline_caught[si] = false;
        baseline_value[si] = 0;
    }

    printf("{\n");
    printf("\"baselineFact\":[");
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t bv = 0;
        bool caught = false;
{% if is_jit %}
        try {
            bv = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            baseline_ok[si] = true;
        } catch(...) {
            caught = true;
        }
{% else %}
        CHAOS_EH_TRY
            bv = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            baseline_ok[si] = true;
        CHAOS_EH_CATCH_BEGIN
            caught = true;
        CHAOS_EH_END
{% end %}
        baseline_caught[si] = caught;
        baseline_value[si] = bv;
        if (si > 0) printf(",");
        printf("{\"si\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, caught ? "false" : "true", bv);
    }
    printf("],");
    auto* patch_ctx = patchDataPath ? ApplyHotpatchFromFile(patchDataPath) : ApplyHotpatchIfAvailable();
    int semantic_passed = 0;
    int semantic_changed_count = 0;
    printf("\"patchedFact\":[");
    for (int si = 0; si < kCount; si++) {
        if (!baseline_ok[si]) { continue; }
        int i = kSubjectSlotMap[si];
        int64_t patched_value = 0;
        bool patched_caught = false;
{% if is_jit %}
        try {
            patched_value = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) {
            patched_caught = true;
        }
{% else %}
        CHAOS_EH_TRY
            patched_value = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
            patched_caught = true;
        CHAOS_EH_END
{% end %}
        if (semantic_passed > 0) printf(",");
        printf("{\"si\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, patched_caught ? "false" : "true",
               patched_caught ? 0 : patched_value);
        semantic_passed++;
        if (baseline_caught[si] != patched_caught ||
            baseline_value[si] != patched_value) {
            semantic_changed_count++;
        }
    }
    printf("],");
    if (patch_ctx != nullptr) {
        chaos::il2cpp::runtime_core::Unpatch(patch_ctx);
    }
    bool all_revert = true;
    int revert_passed = 0;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
{% if is_jit %}
        try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            revert_passed++;
        } catch(...) {
            all_revert = false;
        }
{% else %}
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            revert_passed++;
        CHAOS_EH_CATCH_BEGIN
            all_revert = false;
        CHAOS_EH_END
{% end %}
    }
    bool all_semantic = (semantic_passed > 0 && semantic_changed_count > 0);
    printf(
        "\"passedMethods\":%d,\"failedMethods\":0,"
        "\"totalMethods\":%d,\"allSemantic\":%s,\"allRevert\":%s,"
        "\"semanticChangedCount\":%d}\n",
        semantic_passed, kCount, all_semantic ? "true" : "false",
        all_revert ? "true" : "false", semantic_changed_count);
    std::fflush(stdout);
    return 0;
}

static int RunMicrobenchMode() {
    RunMicrobench();
    return 0;
}

static int RunHotupdateBenchmarkMode(int entry_index, int iterations) {
    auto* patch_ctx = ApplyHotpatchIfAvailable();
    auto result = RunHotpatchBenchmark(entry_index, iterations);
    if (result.elapsed_ms < 0.0) {
        printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}\n");
        return 1;
    }
    double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
    double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
    printf(
        "{\"elapsedMilliseconds\":%.3f,\"calibratedMs\":%.3f,"
        "\"opsPerSecond\":%.0f,\"iterations\":%d,"
        "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}\n",
        result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
        result.allocated_bytes, alloc_per_op);
    std::fflush(stdout);
    return 0;
}

// ── --profile: per-method GC/allocation/code-size profile ───────────
static int RunProfileMode() {
    const int kCount = kSubjectEntryCount;
    chaos::il2cpp::runtime_core::ProfileStoreInit(kCount);
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t heap_before = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
        chaos::il2cpp::runtime_core::GetThreadProfileData().heap_before = heap_before;

{% if is_jit %}
        try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) { }
{% else %}
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
        CHAOS_EH_END
{% endif %}

        int64_t heap_after = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
        chaos::il2cpp::runtime_core::GetThreadProfileData().heap_after = heap_after;
        chaos::il2cpp::runtime_core::FlushThreadProfileData(i);
    }
    chaos::il2cpp::runtime_core::ProfileStoreFinalize();
    chaos::il2cpp::runtime_core::ProfileEmitJson();
    return 0;
}

int main(int argc, char* argv[]) {
    chaos::il2cpp::common::log_internal::g_log_use_stderr = true;
    chaos::il2cpp::runtime_core::g_bgc_enabled = false;

    auto* chaos_host = new ChaosRuntimeHost();
    if (!chaos_host->Initialize("verification-entry")) {
        std::fprintf(stderr, "FATAL: ChaosRuntimeHost::Initialize failed\n");
        return 1;
    }
    if (!chaos_host->RegisterModule(
            &chaos_codegen_code_registration,
            &chaos_codegen_metadata_registration,
            &chaos_codegen_options)) {
        std::fprintf(stderr, "FATAL: RegisterModule failed\n");
        return 1;
    }
    ChaosRegisterGcLayouts();

    int ret = 0;
    if (argc < 2) { ret = RunFactMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--fact-json") == 0) { ret = RunFactJsonMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--list") == 0) { ret = RunListMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--benchmark") == 0) {
        if (argc < 4) { printf("Usage: entry.exe --benchmark <index> <iterations>\n"); return 1; }
        ret = RunBenchmarkMode(std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--benchmark-all") == 0) {
        if (argc < 3) { printf("Usage: entry.exe --benchmark-all <iterations>\n"); return 1; }
        ret = RunBenchmarkAllMode(std::atoi(argv[2]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--benchmark-range") == 0) {
        if (argc < 5) { printf("Usage: entry.exe --benchmark-range <start> <end> <iterations>\n"); return 1; }
        ret = RunBenchmarkRangeMode(std::atoi(argv[4]), std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--hotupdate") == 0) {
        const char* patchDataPath = nullptr;
        if (argc >= 4 && std::strcmp(argv[2], "--patch-data") == 0) {
            patchDataPath = argv[3];
        }
        ret = RunHotupdateMode(patchDataPath);
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--hotupdate-and-benchmark") == 0) {
        if (argc < 4) { printf("Usage: entry.exe --hotupdate-and-benchmark <index> <iterations>\n"); return 1; }
        ret = RunHotupdateBenchmarkMode(std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--microbench") == 0) { ret = RunMicrobenchMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--profile") == 0) { ret = RunProfileMode(); goto shutdown; }

    printf("Unknown flag: %s\n", argv[1]);
    ret = 1;

shutdown:
    std::fflush(stdout);
{% if is_windows %}
    TerminateProcess(GetCurrentProcess(), static_cast<DWORD>(ret));
{% else %}
    _exit(ret);
{% end %}
}