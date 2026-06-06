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
    { 0, "System.Reflection.Metadata/System.Reflection.Metadata.AssemblyDefinition::GetAssemblyName:System.Reflection.AssemblyName()", "codegen", "System.Reflection.Metadata.AssemblyDefinition", "GetAssemblyName", True, "benchmark" },
    { 1, "System.Reflection.Metadata/System.Reflection.Metadata.AssemblyDefinition::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.AssemblyDefinition", "GetCustomAttributes", True, "benchmark" },
    { 2, "System.Reflection.Metadata/System.Reflection.Metadata.AssemblyDefinition::GetDeclarativeSecurityAttributes:System.Reflection.Metadata.DeclarativeSecurityAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.AssemblyDefinition", "GetDeclarativeSecurityAttributes", True, "benchmark" },
    { 3, "System.Reflection.Metadata/System.Reflection.Metadata.AssemblyFile::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.AssemblyFile", "GetCustomAttributes", True, "benchmark" },
    { 4, "System.Reflection.Metadata/System.Reflection.Metadata.AssemblyNameInfo::ToAssemblyName:System.Reflection.AssemblyName()", "codegen", "System.Reflection.Metadata.AssemblyNameInfo", "ToAssemblyName", True, "benchmark" },
    { 5, "System.Reflection.Metadata/System.Reflection.Metadata.AssemblyReference::GetAssemblyName:System.Reflection.AssemblyName()", "codegen", "System.Reflection.Metadata.AssemblyReference", "GetAssemblyName", True, "benchmark" },
    { 6, "System.Reflection.Metadata/System.Reflection.Metadata.AssemblyReference::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.AssemblyReference", "GetCustomAttributes", True, "benchmark" },
    { 7, "System.Reflection.Metadata/System.Reflection.Metadata.Blob::GetBytes:System.ArraySegment<System.Byte>()", "codegen", "System.Reflection.Metadata.Blob", "GetBytes", True, "benchmark" },
    { 8, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::Clear:System.Void()", "codegen", "System.Reflection.Metadata.BlobBuilder", "Clear", True, "benchmark" },
    { 9, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ContentEquals:System.Boolean(System.Reflection.Metadata.BlobBuilder)", "codegen", "System.Reflection.Metadata.BlobBuilder", "ContentEquals", True, "benchmark" },
    { 10, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ToArray:System.Byte[]()", "codegen", "System.Reflection.Metadata.BlobBuilder", "ToArray", True, "benchmark" },
    { 11, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ToArray:System.Byte[](System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "ToArray", True, "benchmark" },
    { 12, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ToArray:System.Byte[](System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "ToArray", True, "benchmark" },
    { 13, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ToArray:System.Byte[](System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "ToArray", True, "benchmark" },
    { 14, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ToImmutableArray:System.Collections.Immutable.ImmutableArray<System.Byte>()", "codegen", "System.Reflection.Metadata.BlobBuilder", "ToImmutableArray", True, "benchmark" },
    { 15, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ToImmutableArray:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "ToImmutableArray", True, "benchmark" },
    { 16, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ToImmutableArray:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "ToImmutableArray", True, "benchmark" },
    { 17, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ToImmutableArray:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "ToImmutableArray", True, "benchmark" },
    { 18, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteContentTo:System.Void(System.IO.Stream)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteContentTo", True, "benchmark" },
    { 19, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteContentTo:System.Void(System.IO.Stream)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteContentTo", True, "benchmark" },
    { 20, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteContentTo:System.Void(System.Reflection.Metadata.BlobWriter)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteContentTo", True, "benchmark" },
    { 21, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteContentTo:System.Void(System.Reflection.Metadata.BlobBuilder)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteContentTo", True, "benchmark" },
    { 22, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::LinkPrefix:System.Void(System.Reflection.Metadata.BlobBuilder)", "codegen", "System.Reflection.Metadata.BlobBuilder", "LinkPrefix", True, "benchmark" },
    { 23, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::LinkSuffix:System.Void(System.Reflection.Metadata.BlobBuilder)", "codegen", "System.Reflection.Metadata.BlobBuilder", "LinkSuffix", True, "benchmark" },
    { 24, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ReserveBytes:System.Reflection.Metadata.Blob(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "ReserveBytes", True, "benchmark" },
    { 25, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::ReserveBytes:System.Reflection.Metadata.Blob(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "ReserveBytes", True, "benchmark" },
    { 26, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 27, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 28, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 29, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte*,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 30, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte*,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 31, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::TryWriteBytes:System.Int32(System.IO.Stream,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "TryWriteBytes", True, "benchmark" },
    { 32, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::TryWriteBytes:System.Int32(System.IO.Stream,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "TryWriteBytes", True, "benchmark" },
    { 33, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::TryWriteBytes:System.Int32(System.IO.Stream,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "TryWriteBytes", True, "benchmark" },
    { 34, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Collections.Immutable.ImmutableArray<System.Byte>)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 35, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Collections.Immutable.ImmutableArray<System.Byte>,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 36, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Collections.Immutable.ImmutableArray<System.Byte>,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 37, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Collections.Immutable.ImmutableArray<System.Byte>,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 38, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte[])", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 39, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte[])", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 40, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte[])", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 41, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 42, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 43, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 44, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 45, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBytes", True, "benchmark" },
    { 46, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::PadTo:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "PadTo", True, "benchmark" },
    { 47, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::PadTo:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "PadTo", True, "benchmark" },
    { 48, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::Align:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "Align", True, "benchmark" },
    { 49, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::Align:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "Align", True, "benchmark" },
    { 50, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBoolean:System.Void(System.Boolean)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBoolean", True, "benchmark" },
    { 51, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteBoolean:System.Void(System.Boolean)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteBoolean", True, "benchmark" },
    { 52, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteByte:System.Void(System.Byte)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteByte", True, "benchmark" },
    { 53, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteByte:System.Void(System.Byte)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteByte", True, "benchmark" },
    { 54, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteSByte:System.Void(System.SByte)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteSByte", True, "benchmark" },
    { 55, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteSByte:System.Void(System.SByte)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteSByte", True, "benchmark" },
    { 56, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteDouble:System.Void(System.Double)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteDouble", True, "benchmark" },
    { 57, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteDouble:System.Void(System.Double)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteDouble", True, "benchmark" },
    { 58, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteSingle:System.Void(System.Single)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteSingle", True, "benchmark" },
    { 59, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteSingle:System.Void(System.Single)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteSingle", True, "benchmark" },
    { 60, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUInt16:System.Void(System.UInt16)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUInt16", True, "benchmark" },
    { 61, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUInt16:System.Void(System.UInt16)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUInt16", True, "benchmark" },
    { 62, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteInt16BE:System.Void(System.Int16)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteInt16BE", True, "benchmark" },
    { 63, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteInt16BE:System.Void(System.Int16)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteInt16BE", True, "benchmark" },
    { 64, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUInt16BE:System.Void(System.UInt16)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUInt16BE", True, "benchmark" },
    { 65, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUInt16BE:System.Void(System.UInt16)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUInt16BE", True, "benchmark" },
    { 66, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteInt32BE:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteInt32BE", True, "benchmark" },
    { 67, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteInt32BE:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteInt32BE", True, "benchmark" },
    { 68, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUInt32BE:System.Void(System.UInt32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUInt32BE", True, "benchmark" },
    { 69, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUInt32BE:System.Void(System.UInt32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUInt32BE", True, "benchmark" },
    { 70, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteInt32:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteInt32", True, "benchmark" },
    { 71, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteInt32:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteInt32", True, "benchmark" },
    { 72, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUInt32:System.Void(System.UInt32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUInt32", True, "benchmark" },
    { 73, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUInt32:System.Void(System.UInt32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUInt32", True, "benchmark" },
    { 74, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteInt64:System.Void(System.Int64)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteInt64", True, "benchmark" },
    { 75, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteInt64:System.Void(System.Int64)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteInt64", True, "benchmark" },
    { 76, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUInt64:System.Void(System.UInt64)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUInt64", True, "benchmark" },
    { 77, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUInt64:System.Void(System.UInt64)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUInt64", True, "benchmark" },
    { 78, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteDecimal:System.Void(System.Decimal)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteDecimal", True, "benchmark" },
    { 79, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteDecimal:System.Void(System.Decimal)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteDecimal", True, "benchmark" },
    { 80, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteGuid:System.Void(System.Guid)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteGuid", True, "benchmark" },
    { 81, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteDateTime:System.Void(System.DateTime)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteDateTime", True, "benchmark" },
    { 82, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteReference:System.Void(System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteReference", True, "benchmark" },
    { 83, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteReference:System.Void(System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteReference", True, "benchmark" },
    { 84, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteReference:System.Void(System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteReference", True, "benchmark" },
    { 85, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUTF16:System.Void(System.Char[])", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUTF16", True, "benchmark" },
    { 86, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUTF16:System.Void(System.Char[])", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUTF16", True, "benchmark" },
    { 87, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUTF16:System.Void(System.Char[])", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUTF16", True, "benchmark" },
    { 88, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUTF16:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUTF16", True, "benchmark" },
    { 89, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUTF16:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUTF16", True, "benchmark" },
    { 90, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteSerializedString:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteSerializedString", True, "benchmark" },
    { 91, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteSerializedString:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteSerializedString", True, "benchmark" },
    { 92, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUserString:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUserString", True, "benchmark" },
    { 93, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUserString:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUserString", True, "benchmark" },
    { 94, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUTF8:System.Void(System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUTF8", True, "benchmark" },
    { 95, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUTF8:System.Void(System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUTF8", True, "benchmark" },
    { 96, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteUTF8:System.Void(System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteUTF8", True, "benchmark" },
    { 97, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteCompressedSignedInteger:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteCompressedSignedInteger", True, "benchmark" },
    { 98, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteCompressedSignedInteger:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteCompressedSignedInteger", True, "benchmark" },
    { 99, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteCompressedInteger:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteCompressedInteger", True, "benchmark" },
    { 100, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteCompressedInteger:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteCompressedInteger", True, "benchmark" },
    { 101, "System.Reflection.Metadata/System.Reflection.Metadata.BlobBuilder::WriteConstant:System.Void(System.Object)", "codegen", "System.Reflection.Metadata.BlobBuilder", "WriteConstant", True, "benchmark" },
    { 102, "System.Reflection.Metadata/System.Reflection.Metadata.BlobContentId::FromHash:System.Reflection.Metadata.BlobContentId(System.Collections.Immutable.ImmutableArray<System.Byte>)", "codegen", "System.Reflection.Metadata.BlobContentId", "FromHash", True, "benchmark" },
    { 103, "System.Reflection.Metadata/System.Reflection.Metadata.BlobContentId::FromHash:System.Reflection.Metadata.BlobContentId(System.Byte[])", "codegen", "System.Reflection.Metadata.BlobContentId", "FromHash", True, "benchmark" },
    { 104, "System.Reflection.Metadata/System.Reflection.Metadata.BlobContentId::FromHash:System.Reflection.Metadata.BlobContentId(System.Byte[])", "codegen", "System.Reflection.Metadata.BlobContentId", "FromHash", True, "benchmark" },
    { 105, "System.Reflection.Metadata/System.Reflection.Metadata.BlobContentId::FromHash:System.Reflection.Metadata.BlobContentId(System.Byte[])", "codegen", "System.Reflection.Metadata.BlobContentId", "FromHash", True, "benchmark" },
    { 106, "System.Reflection.Metadata/System.Reflection.Metadata.BlobContentId::GetTimeBasedProvider:System.Func<System.Collections.Generic.IEnumerable<System.Reflection.Metadata.Blob>,System.Reflection.Metadata.BlobContentId>()", "codegen", "System.Reflection.Metadata.BlobContentId", "GetTimeBasedProvider", True, "benchmark" },
    { 107, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::Align:System.Void(System.Byte)", "codegen", "System.Reflection.Metadata.BlobReader", "Align", True, "benchmark" },
    { 108, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::Align:System.Void(System.Byte)", "codegen", "System.Reflection.Metadata.BlobReader", "Align", True, "benchmark" },
    { 109, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadBoolean:System.Boolean()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadBoolean", True, "benchmark" },
    { 110, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadSByte:System.SByte()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadSByte", True, "benchmark" },
    { 111, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadByte:System.Byte()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadByte", True, "benchmark" },
    { 112, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadChar:System.Char()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadChar", True, "benchmark" },
    { 113, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadInt16:System.Int16()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadInt16", True, "benchmark" },
    { 114, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadUInt16:System.UInt16()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadUInt16", True, "benchmark" },
    { 115, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadInt32:System.Int32()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadInt32", True, "benchmark" },
    { 116, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadUInt32:System.UInt32()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadUInt32", True, "benchmark" },
    { 117, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadInt64:System.Int64()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadInt64", True, "benchmark" },
    { 118, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadUInt64:System.UInt64()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadUInt64", True, "benchmark" },
    { 119, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadSingle:System.Single()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadSingle", True, "benchmark" },
    { 120, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadDouble:System.Double()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadDouble", True, "benchmark" },
    { 121, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadGuid:System.Guid()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadGuid", True, "benchmark" },
    { 122, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadDecimal:System.Decimal()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadDecimal", True, "benchmark" },
    { 123, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadDateTime:System.DateTime()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadDateTime", True, "benchmark" },
    { 124, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadSignatureHeader:System.Reflection.Metadata.SignatureHeader()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadSignatureHeader", True, "benchmark" },
    { 125, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::IndexOf:System.Int32(System.Byte)", "codegen", "System.Reflection.Metadata.BlobReader", "IndexOf", True, "benchmark" },
    { 126, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::IndexOf:System.Int32(System.Byte)", "codegen", "System.Reflection.Metadata.BlobReader", "IndexOf", True, "benchmark" },
    { 127, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadUTF8:System.String(System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadUTF8", True, "benchmark" },
    { 128, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadUTF8:System.String(System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadUTF8", True, "benchmark" },
    { 129, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadUTF16:System.String(System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadUTF16", True, "benchmark" },
    { 130, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadUTF16:System.String(System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadUTF16", True, "benchmark" },
    { 131, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadBytes:System.Byte[](System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadBytes", True, "benchmark" },
    { 132, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadBytes:System.Byte[](System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadBytes", True, "benchmark" },
    { 133, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadBytes:System.Void(System.Int32,System.Byte[],System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadBytes", True, "benchmark" },
    { 134, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadBytes:System.Void(System.Int32,System.Byte[],System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadBytes", True, "benchmark" },
    { 135, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadBytes:System.Void(System.Int32,System.Byte[],System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadBytes", True, "benchmark" },
    { 136, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadBytes:System.Void(System.Int32,System.Byte[],System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadBytes", True, "benchmark" },
    { 137, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadBytes:System.Void(System.Int32,System.Byte[],System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadBytes", True, "benchmark" },
    { 138, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::TryReadCompressedInteger:System.Boolean(System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "TryReadCompressedInteger", True, "benchmark" },
    { 139, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::TryReadCompressedInteger:System.Boolean(System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "TryReadCompressedInteger", True, "benchmark" },
    { 140, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadCompressedInteger:System.Int32()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadCompressedInteger", True, "benchmark" },
    { 141, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::TryReadCompressedSignedInteger:System.Boolean(System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "TryReadCompressedSignedInteger", True, "benchmark" },
    { 142, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::TryReadCompressedSignedInteger:System.Boolean(System.Int32)", "codegen", "System.Reflection.Metadata.BlobReader", "TryReadCompressedSignedInteger", True, "benchmark" },
    { 143, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadCompressedSignedInteger:System.Int32()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadCompressedSignedInteger", True, "benchmark" },
    { 144, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadSerializationTypeCode:System.Reflection.Metadata.SerializationTypeCode()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadSerializationTypeCode", True, "benchmark" },
    { 145, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadSignatureTypeCode:System.Reflection.Metadata.SignatureTypeCode()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadSignatureTypeCode", True, "benchmark" },
    { 146, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadSerializedString:System.String()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadSerializedString", True, "benchmark" },
    { 147, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadTypeHandle:System.Reflection.Metadata.EntityHandle()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadTypeHandle", True, "benchmark" },
    { 148, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadBlobHandle:System.Reflection.Metadata.BlobHandle()", "codegen", "System.Reflection.Metadata.BlobReader", "ReadBlobHandle", True, "benchmark" },
    { 149, "System.Reflection.Metadata/System.Reflection.Metadata.BlobReader::ReadConstant:System.Object(System.Reflection.Metadata.ConstantTypeCode)", "codegen", "System.Reflection.Metadata.BlobReader", "ReadConstant", True, "benchmark" },
    { 150, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::ContentEquals:System.Boolean(System.Reflection.Metadata.BlobWriter)", "codegen", "System.Reflection.Metadata.BlobWriter", "ContentEquals", True, "benchmark" },
    { 151, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::ToArray:System.Byte[]()", "codegen", "System.Reflection.Metadata.BlobWriter", "ToArray", True, "benchmark" },
    { 152, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::ToArray:System.Byte[](System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "ToArray", True, "benchmark" },
    { 153, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::ToArray:System.Byte[](System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "ToArray", True, "benchmark" },
    { 154, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::ToArray:System.Byte[](System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "ToArray", True, "benchmark" },
    { 155, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::ToImmutableArray:System.Collections.Immutable.ImmutableArray<System.Byte>()", "codegen", "System.Reflection.Metadata.BlobWriter", "ToImmutableArray", True, "benchmark" },
    { 156, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::ToImmutableArray:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "ToImmutableArray", True, "benchmark" },
    { 157, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::ToImmutableArray:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "ToImmutableArray", True, "benchmark" },
    { 158, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::ToImmutableArray:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "ToImmutableArray", True, "benchmark" },
    { 159, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 160, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 161, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 162, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte*,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 163, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte*,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 164, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Reflection.Metadata.BlobBuilder)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 165, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Int32(System.IO.Stream,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 166, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Int32(System.IO.Stream,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 167, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Int32(System.IO.Stream,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 168, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Collections.Immutable.ImmutableArray<System.Byte>)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 169, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Collections.Immutable.ImmutableArray<System.Byte>,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 170, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Collections.Immutable.ImmutableArray<System.Byte>,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 171, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Collections.Immutable.ImmutableArray<System.Byte>,System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 172, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte[])", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 173, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte[])", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 174, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte[])", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 175, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 176, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 177, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 178, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 179, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBytes:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBytes", True, "benchmark" },
    { 180, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::PadTo:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "PadTo", True, "benchmark" },
    { 181, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::PadTo:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "PadTo", True, "benchmark" },
    { 182, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::Align:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "Align", True, "benchmark" },
    { 183, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::Align:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "Align", True, "benchmark" },
    { 184, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBoolean:System.Void(System.Boolean)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBoolean", True, "benchmark" },
    { 185, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteBoolean:System.Void(System.Boolean)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteBoolean", True, "benchmark" },
    { 186, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteByte:System.Void(System.Byte)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteByte", True, "benchmark" },
    { 187, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteByte:System.Void(System.Byte)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteByte", True, "benchmark" },
    { 188, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteSByte:System.Void(System.SByte)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteSByte", True, "benchmark" },
    { 189, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteSByte:System.Void(System.SByte)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteSByte", True, "benchmark" },
    { 190, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteDouble:System.Void(System.Double)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteDouble", True, "benchmark" },
    { 191, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteDouble:System.Void(System.Double)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteDouble", True, "benchmark" },
    { 192, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteSingle:System.Void(System.Single)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteSingle", True, "benchmark" },
    { 193, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteSingle:System.Void(System.Single)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteSingle", True, "benchmark" },
    { 194, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUInt16:System.Void(System.UInt16)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUInt16", True, "benchmark" },
    { 195, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUInt16:System.Void(System.UInt16)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUInt16", True, "benchmark" },
    { 196, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteInt16BE:System.Void(System.Int16)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteInt16BE", True, "benchmark" },
    { 197, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteInt16BE:System.Void(System.Int16)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteInt16BE", True, "benchmark" },
    { 198, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUInt16BE:System.Void(System.UInt16)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUInt16BE", True, "benchmark" },
    { 199, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUInt16BE:System.Void(System.UInt16)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUInt16BE", True, "benchmark" },
    { 200, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteInt32BE:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteInt32BE", True, "benchmark" },
    { 201, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteInt32BE:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteInt32BE", True, "benchmark" },
    { 202, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUInt32BE:System.Void(System.UInt32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUInt32BE", True, "benchmark" },
    { 203, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUInt32BE:System.Void(System.UInt32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUInt32BE", True, "benchmark" },
    { 204, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteInt32:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteInt32", True, "benchmark" },
    { 205, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteInt32:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteInt32", True, "benchmark" },
    { 206, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUInt32:System.Void(System.UInt32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUInt32", True, "benchmark" },
    { 207, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUInt32:System.Void(System.UInt32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUInt32", True, "benchmark" },
    { 208, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteInt64:System.Void(System.Int64)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteInt64", True, "benchmark" },
    { 209, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteInt64:System.Void(System.Int64)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteInt64", True, "benchmark" },
    { 210, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUInt64:System.Void(System.UInt64)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUInt64", True, "benchmark" },
    { 211, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUInt64:System.Void(System.UInt64)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUInt64", True, "benchmark" },
    { 212, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteDecimal:System.Void(System.Decimal)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteDecimal", True, "benchmark" },
    { 213, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteDecimal:System.Void(System.Decimal)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteDecimal", True, "benchmark" },
    { 214, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteGuid:System.Void(System.Guid)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteGuid", True, "benchmark" },
    { 215, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteDateTime:System.Void(System.DateTime)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteDateTime", True, "benchmark" },
    { 216, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteReference:System.Void(System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteReference", True, "benchmark" },
    { 217, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteReference:System.Void(System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteReference", True, "benchmark" },
    { 218, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteReference:System.Void(System.Int32,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteReference", True, "benchmark" },
    { 219, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUTF16:System.Void(System.Char[])", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUTF16", True, "benchmark" },
    { 220, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUTF16:System.Void(System.Char[])", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUTF16", True, "benchmark" },
    { 221, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUTF16:System.Void(System.Char[])", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUTF16", True, "benchmark" },
    { 222, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUTF16:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUTF16", True, "benchmark" },
    { 223, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUTF16:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUTF16", True, "benchmark" },
    { 224, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteSerializedString:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteSerializedString", True, "benchmark" },
    { 225, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteSerializedString:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteSerializedString", True, "benchmark" },
    { 226, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUserString:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUserString", True, "benchmark" },
    { 227, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUserString:System.Void(System.String)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUserString", True, "benchmark" },
    { 228, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUTF8:System.Void(System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUTF8", True, "benchmark" },
    { 229, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUTF8:System.Void(System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUTF8", True, "benchmark" },
    { 230, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteUTF8:System.Void(System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteUTF8", True, "benchmark" },
    { 231, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteCompressedSignedInteger:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteCompressedSignedInteger", True, "benchmark" },
    { 232, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteCompressedSignedInteger:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteCompressedSignedInteger", True, "benchmark" },
    { 233, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteCompressedInteger:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteCompressedInteger", True, "benchmark" },
    { 234, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteCompressedInteger:System.Void(System.Int32)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteCompressedInteger", True, "benchmark" },
    { 235, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::WriteConstant:System.Void(System.Object)", "codegen", "System.Reflection.Metadata.BlobWriter", "WriteConstant", True, "benchmark" },
    { 236, "System.Reflection.Metadata/System.Reflection.Metadata.BlobWriter::Clear:System.Void()", "codegen", "System.Reflection.Metadata.BlobWriter", "Clear", True, "benchmark" },
    { 237, "System.Reflection.Metadata/System.Reflection.Metadata.CustomAttribute::DecodeValue:System.Reflection.Metadata.CustomAttributeValue<System.Int32>(System.Reflection.Metadata.ICustomAttributeTypeProvider<System.Int32>)", "codegen", "System.Reflection.Metadata.CustomAttribute", "DecodeValue", True, "benchmark" },
    { 238, "System.Reflection.Metadata/System.Reflection.Metadata.EventDefinition::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.EventDefinition", "GetCustomAttributes", True, "benchmark" },
    { 239, "System.Reflection.Metadata/System.Reflection.Metadata.EventDefinition::GetAccessors:System.Reflection.Metadata.EventAccessors()", "codegen", "System.Reflection.Metadata.EventDefinition", "GetAccessors", True, "benchmark" },
    { 240, "System.Reflection.Metadata/System.Reflection.Metadata.ExportedType::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.ExportedType", "GetCustomAttributes", True, "benchmark" },
    { 241, "System.Reflection.Metadata/System.Reflection.Metadata.FieldDefinition::DecodeSignature:System.Int32(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.FieldDefinition", "DecodeSignature", True, "benchmark" },
    { 242, "System.Reflection.Metadata/System.Reflection.Metadata.FieldDefinition::DecodeSignature:System.Int32(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.FieldDefinition", "DecodeSignature", True, "benchmark" },
    { 243, "System.Reflection.Metadata/System.Reflection.Metadata.FieldDefinition::GetDeclaringType:System.Reflection.Metadata.TypeDefinitionHandle()", "codegen", "System.Reflection.Metadata.FieldDefinition", "GetDeclaringType", True, "benchmark" },
    { 244, "System.Reflection.Metadata/System.Reflection.Metadata.FieldDefinition::GetDefaultValue:System.Reflection.Metadata.ConstantHandle()", "codegen", "System.Reflection.Metadata.FieldDefinition", "GetDefaultValue", True, "benchmark" },
    { 245, "System.Reflection.Metadata/System.Reflection.Metadata.FieldDefinition::GetRelativeVirtualAddress:System.Int32()", "codegen", "System.Reflection.Metadata.FieldDefinition", "GetRelativeVirtualAddress", True, "benchmark" },
    { 246, "System.Reflection.Metadata/System.Reflection.Metadata.FieldDefinition::GetOffset:System.Int32()", "codegen", "System.Reflection.Metadata.FieldDefinition", "GetOffset", True, "benchmark" },
    { 247, "System.Reflection.Metadata/System.Reflection.Metadata.FieldDefinition::GetMarshallingDescriptor:System.Reflection.Metadata.BlobHandle()", "codegen", "System.Reflection.Metadata.FieldDefinition", "GetMarshallingDescriptor", True, "benchmark" },
    { 248, "System.Reflection.Metadata/System.Reflection.Metadata.FieldDefinition::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.FieldDefinition", "GetCustomAttributes", True, "benchmark" },
    { 249, "System.Reflection.Metadata/System.Reflection.Metadata.GenericParameter::GetConstraints:System.Reflection.Metadata.GenericParameterConstraintHandleCollection()", "codegen", "System.Reflection.Metadata.GenericParameter", "GetConstraints", True, "benchmark" },
    { 250, "System.Reflection.Metadata/System.Reflection.Metadata.GenericParameter::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.GenericParameter", "GetCustomAttributes", True, "benchmark" },
    { 251, "System.Reflection.Metadata/System.Reflection.Metadata.GenericParameterConstraint::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.GenericParameterConstraint", "GetCustomAttributes", True, "benchmark" },
    { 252, "System.Reflection.Metadata/System.Reflection.Metadata.HandleComparer::Compare:System.Int32(System.Reflection.Metadata.Handle,System.Reflection.Metadata.Handle)", "codegen", "System.Reflection.Metadata.HandleComparer", "Compare", True, "benchmark" },
    { 253, "System.Reflection.Metadata/System.Reflection.Metadata.HandleComparer::Compare:System.Int32(System.Reflection.Metadata.EntityHandle,System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.HandleComparer", "Compare", True, "benchmark" },
    { 254, "System.Reflection.Metadata/System.Reflection.Metadata.ILOpCodeExtensions::IsBranch:System.Boolean(System.Reflection.Metadata.ILOpCode)", "codegen", "System.Reflection.Metadata.ILOpCodeExtensions", "IsBranch", True, "benchmark" },
    { 255, "System.Reflection.Metadata/System.Reflection.Metadata.ILOpCodeExtensions::GetBranchOperandSize:System.Int32(System.Reflection.Metadata.ILOpCode)", "codegen", "System.Reflection.Metadata.ILOpCodeExtensions", "GetBranchOperandSize", True, "benchmark" },
    { 256, "System.Reflection.Metadata/System.Reflection.Metadata.ILOpCodeExtensions::GetShortBranch:System.Reflection.Metadata.ILOpCode(System.Reflection.Metadata.ILOpCode)", "codegen", "System.Reflection.Metadata.ILOpCodeExtensions", "GetShortBranch", True, "benchmark" },
    { 257, "System.Reflection.Metadata/System.Reflection.Metadata.ILOpCodeExtensions::GetLongBranch:System.Reflection.Metadata.ILOpCode(System.Reflection.Metadata.ILOpCode)", "codegen", "System.Reflection.Metadata.ILOpCodeExtensions", "GetLongBranch", True, "benchmark" },
    { 258, "System.Reflection.Metadata/System.Reflection.Metadata.ImportScope::GetImports:System.Reflection.Metadata.ImportDefinitionCollection()", "codegen", "System.Reflection.Metadata.ImportScope", "GetImports", True, "benchmark" },
    { 259, "System.Reflection.Metadata/System.Reflection.Metadata.InterfaceImplementation::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.InterfaceImplementation", "GetCustomAttributes", True, "benchmark" },
    { 260, "System.Reflection.Metadata/System.Reflection.Metadata.LocalScope::GetLocalVariables:System.Reflection.Metadata.LocalVariableHandleCollection()", "codegen", "System.Reflection.Metadata.LocalScope", "GetLocalVariables", True, "benchmark" },
    { 261, "System.Reflection.Metadata/System.Reflection.Metadata.LocalScope::GetLocalConstants:System.Reflection.Metadata.LocalConstantHandleCollection()", "codegen", "System.Reflection.Metadata.LocalScope", "GetLocalConstants", True, "benchmark" },
    { 262, "System.Reflection.Metadata/System.Reflection.Metadata.ManifestResource::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.ManifestResource", "GetCustomAttributes", True, "benchmark" },
    { 263, "System.Reflection.Metadata/System.Reflection.Metadata.MemberReference::DecodeFieldSignature:System.Int32(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.MemberReference", "DecodeFieldSignature", True, "benchmark" },
    { 264, "System.Reflection.Metadata/System.Reflection.Metadata.MemberReference::DecodeFieldSignature:System.Int32(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.MemberReference", "DecodeFieldSignature", True, "benchmark" },
    { 265, "System.Reflection.Metadata/System.Reflection.Metadata.MemberReference::DecodeMethodSignature:System.Reflection.Metadata.MethodSignature<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.MemberReference", "DecodeMethodSignature", True, "benchmark" },
    { 266, "System.Reflection.Metadata/System.Reflection.Metadata.MemberReference::DecodeMethodSignature:System.Reflection.Metadata.MethodSignature<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.MemberReference", "DecodeMethodSignature", True, "benchmark" },
    { 267, "System.Reflection.Metadata/System.Reflection.Metadata.MemberReference::GetKind:System.Reflection.Metadata.MemberReferenceKind()", "codegen", "System.Reflection.Metadata.MemberReference", "GetKind", True, "benchmark" },
    { 268, "System.Reflection.Metadata/System.Reflection.Metadata.MemberReference::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.MemberReference", "GetCustomAttributes", True, "benchmark" },
    { 269, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetAssemblyName:System.Reflection.AssemblyName(System.String)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetAssemblyName", True, "benchmark" },
    { 270, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetAssemblyName:System.Reflection.AssemblyName(System.String)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetAssemblyName", True, "benchmark" },
    { 271, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetAssemblyDefinition:System.Reflection.Metadata.AssemblyDefinition()", "codegen", "System.Reflection.Metadata.MetadataReader", "GetAssemblyDefinition", True, "benchmark" },
    { 272, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetString:System.String(System.Reflection.Metadata.StringHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetString", True, "benchmark" },
    { 273, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetString:System.String(System.Reflection.Metadata.NamespaceDefinitionHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetString", True, "benchmark" },
    { 274, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetBlobBytes:System.Byte[](System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetBlobBytes", True, "benchmark" },
    { 275, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetBlobContent:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetBlobContent", True, "benchmark" },
    { 276, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetBlobReader:System.Reflection.Metadata.BlobReader(System.Reflection.Metadata.BlobHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetBlobReader", True, "benchmark" },
    { 277, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetBlobReader:System.Reflection.Metadata.BlobReader(System.Reflection.Metadata.StringHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetBlobReader", True, "benchmark" },
    { 278, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetUserString:System.String(System.Reflection.Metadata.UserStringHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetUserString", True, "benchmark" },
    { 279, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetGuid:System.Guid(System.Reflection.Metadata.GuidHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetGuid", True, "benchmark" },
    { 280, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetModuleDefinition:System.Reflection.Metadata.ModuleDefinition()", "codegen", "System.Reflection.Metadata.MetadataReader", "GetModuleDefinition", True, "benchmark" },
    { 281, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetAssemblyReference:System.Reflection.Metadata.AssemblyReference(System.Reflection.Metadata.AssemblyReferenceHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetAssemblyReference", True, "benchmark" },
    { 282, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetTypeDefinition:System.Reflection.Metadata.TypeDefinition(System.Reflection.Metadata.TypeDefinitionHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetTypeDefinition", True, "benchmark" },
    { 283, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetNamespaceDefinitionRoot:System.Reflection.Metadata.NamespaceDefinition()", "codegen", "System.Reflection.Metadata.MetadataReader", "GetNamespaceDefinitionRoot", True, "benchmark" },
    { 284, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetNamespaceDefinition:System.Reflection.Metadata.NamespaceDefinition(System.Reflection.Metadata.NamespaceDefinitionHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetNamespaceDefinition", True, "benchmark" },
    { 285, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetTypeReference:System.Reflection.Metadata.TypeReference(System.Reflection.Metadata.TypeReferenceHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetTypeReference", True, "benchmark" },
    { 286, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetExportedType:System.Reflection.Metadata.ExportedType(System.Reflection.Metadata.ExportedTypeHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetExportedType", True, "benchmark" },
    { 287, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetCustomAttributes", True, "benchmark" },
    { 288, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetCustomAttribute:System.Reflection.Metadata.CustomAttribute(System.Reflection.Metadata.CustomAttributeHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetCustomAttribute", True, "benchmark" },
    { 289, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetDeclarativeSecurityAttribute:System.Reflection.Metadata.DeclarativeSecurityAttribute(System.Reflection.Metadata.DeclarativeSecurityAttributeHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetDeclarativeSecurityAttribute", True, "benchmark" },
    { 290, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetConstant:System.Reflection.Metadata.Constant(System.Reflection.Metadata.ConstantHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetConstant", True, "benchmark" },
    { 291, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetMethodDefinition:System.Reflection.Metadata.MethodDefinition(System.Reflection.Metadata.MethodDefinitionHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetMethodDefinition", True, "benchmark" },
    { 292, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetFieldDefinition:System.Reflection.Metadata.FieldDefinition(System.Reflection.Metadata.FieldDefinitionHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetFieldDefinition", True, "benchmark" },
    { 293, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetPropertyDefinition:System.Reflection.Metadata.PropertyDefinition(System.Reflection.Metadata.PropertyDefinitionHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetPropertyDefinition", True, "benchmark" },
    { 294, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetEventDefinition:System.Reflection.Metadata.EventDefinition(System.Reflection.Metadata.EventDefinitionHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetEventDefinition", True, "benchmark" },
    { 295, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetMethodImplementation:System.Reflection.Metadata.MethodImplementation(System.Reflection.Metadata.MethodImplementationHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetMethodImplementation", True, "benchmark" },
    { 296, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetMemberReference:System.Reflection.Metadata.MemberReference(System.Reflection.Metadata.MemberReferenceHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetMemberReference", True, "benchmark" },
    { 297, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetMethodSpecification:System.Reflection.Metadata.MethodSpecification(System.Reflection.Metadata.MethodSpecificationHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetMethodSpecification", True, "benchmark" },
    { 298, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetParameter:System.Reflection.Metadata.Parameter(System.Reflection.Metadata.ParameterHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetParameter", True, "benchmark" },
    { 299, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetGenericParameter:System.Reflection.Metadata.GenericParameter(System.Reflection.Metadata.GenericParameterHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetGenericParameter", True, "benchmark" },
    { 300, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetGenericParameterConstraint:System.Reflection.Metadata.GenericParameterConstraint(System.Reflection.Metadata.GenericParameterConstraintHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetGenericParameterConstraint", True, "benchmark" },
    { 301, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetManifestResource:System.Reflection.Metadata.ManifestResource(System.Reflection.Metadata.ManifestResourceHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetManifestResource", True, "benchmark" },
    { 302, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetAssemblyFile:System.Reflection.Metadata.AssemblyFile(System.Reflection.Metadata.AssemblyFileHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetAssemblyFile", True, "benchmark" },
    { 303, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetStandaloneSignature:System.Reflection.Metadata.StandaloneSignature(System.Reflection.Metadata.StandaloneSignatureHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetStandaloneSignature", True, "benchmark" },
    { 304, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetTypeSpecification:System.Reflection.Metadata.TypeSpecification(System.Reflection.Metadata.TypeSpecificationHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetTypeSpecification", True, "benchmark" },
    { 305, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetModuleReference:System.Reflection.Metadata.ModuleReference(System.Reflection.Metadata.ModuleReferenceHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetModuleReference", True, "benchmark" },
    { 306, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetInterfaceImplementation:System.Reflection.Metadata.InterfaceImplementation(System.Reflection.Metadata.InterfaceImplementationHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetInterfaceImplementation", True, "benchmark" },
    { 307, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetString:System.String(System.Reflection.Metadata.DocumentNameBlobHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetString", True, "benchmark" },
    { 308, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetDocument:System.Reflection.Metadata.Document(System.Reflection.Metadata.DocumentHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetDocument", True, "benchmark" },
    { 309, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetMethodDebugInformation:System.Reflection.Metadata.MethodDebugInformation(System.Reflection.Metadata.MethodDebugInformationHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetMethodDebugInformation", True, "benchmark" },
    { 310, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetMethodDebugInformation:System.Reflection.Metadata.MethodDebugInformation(System.Reflection.Metadata.MethodDefinitionHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetMethodDebugInformation", True, "benchmark" },
    { 311, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetLocalScope:System.Reflection.Metadata.LocalScope(System.Reflection.Metadata.LocalScopeHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetLocalScope", True, "benchmark" },
    { 312, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetLocalVariable:System.Reflection.Metadata.LocalVariable(System.Reflection.Metadata.LocalVariableHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetLocalVariable", True, "benchmark" },
    { 313, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetLocalConstant:System.Reflection.Metadata.LocalConstant(System.Reflection.Metadata.LocalConstantHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetLocalConstant", True, "benchmark" },
    { 314, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetImportScope:System.Reflection.Metadata.ImportScope(System.Reflection.Metadata.ImportScopeHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetImportScope", True, "benchmark" },
    { 315, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetCustomDebugInformation:System.Reflection.Metadata.CustomDebugInformation(System.Reflection.Metadata.CustomDebugInformationHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetCustomDebugInformation", True, "benchmark" },
    { 316, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetCustomDebugInformation:System.Reflection.Metadata.CustomDebugInformationHandleCollection(System.Reflection.Metadata.EntityHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetCustomDebugInformation", True, "benchmark" },
    { 317, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetLocalScopes:System.Reflection.Metadata.LocalScopeHandleCollection(System.Reflection.Metadata.MethodDefinitionHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetLocalScopes", True, "benchmark" },
    { 318, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReader::GetLocalScopes:System.Reflection.Metadata.LocalScopeHandleCollection(System.Reflection.Metadata.MethodDebugInformationHandle)", "codegen", "System.Reflection.Metadata.MetadataReader", "GetLocalScopes", True, "benchmark" },
    { 319, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromPortablePdbImage:System.Reflection.Metadata.MetadataReaderProvider(System.Byte*,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromPortablePdbImage", True, "benchmark" },
    { 320, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromPortablePdbImage:System.Reflection.Metadata.MetadataReaderProvider(System.Byte*,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromPortablePdbImage", True, "benchmark" },
    { 321, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromMetadataImage:System.Reflection.Metadata.MetadataReaderProvider(System.Byte*,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromMetadataImage", True, "benchmark" },
    { 322, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromMetadataImage:System.Reflection.Metadata.MetadataReaderProvider(System.Byte*,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromMetadataImage", True, "benchmark" },
    { 323, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromPortablePdbImage:System.Reflection.Metadata.MetadataReaderProvider(System.Collections.Immutable.ImmutableArray<System.Byte>)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromPortablePdbImage", True, "benchmark" },
    { 324, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromMetadataImage:System.Reflection.Metadata.MetadataReaderProvider(System.Collections.Immutable.ImmutableArray<System.Byte>)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromMetadataImage", True, "benchmark" },
    { 325, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromPortablePdbStream:System.Reflection.Metadata.MetadataReaderProvider(System.IO.Stream,System.Reflection.Metadata.MetadataStreamOptions,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromPortablePdbStream", True, "benchmark" },
    { 326, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromPortablePdbStream:System.Reflection.Metadata.MetadataReaderProvider(System.IO.Stream,System.Reflection.Metadata.MetadataStreamOptions,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromPortablePdbStream", True, "benchmark" },
    { 327, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromPortablePdbStream:System.Reflection.Metadata.MetadataReaderProvider(System.IO.Stream,System.Reflection.Metadata.MetadataStreamOptions,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromPortablePdbStream", True, "benchmark" },
    { 328, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromMetadataStream:System.Reflection.Metadata.MetadataReaderProvider(System.IO.Stream,System.Reflection.Metadata.MetadataStreamOptions,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromMetadataStream", True, "benchmark" },
    { 329, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromMetadataStream:System.Reflection.Metadata.MetadataReaderProvider(System.IO.Stream,System.Reflection.Metadata.MetadataStreamOptions,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromMetadataStream", True, "benchmark" },
    { 330, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::FromMetadataStream:System.Reflection.Metadata.MetadataReaderProvider(System.IO.Stream,System.Reflection.Metadata.MetadataStreamOptions,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "FromMetadataStream", True, "benchmark" },
    { 331, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::Dispose:System.Void()", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "Dispose", True, "benchmark" },
    { 332, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataReaderProvider::GetMetadataReader:System.Reflection.Metadata.MetadataReader(System.Reflection.Metadata.MetadataReaderOptions,System.Reflection.Metadata.MetadataStringDecoder)", "codegen", "System.Reflection.Metadata.MetadataReaderProvider", "GetMetadataReader", True, "benchmark" },
    { 333, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataStringComparer::StartsWith:System.Boolean(System.Reflection.Metadata.StringHandle,System.String)", "codegen", "System.Reflection.Metadata.MetadataStringComparer", "StartsWith", True, "benchmark" },
    { 334, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataStringComparer::StartsWith:System.Boolean(System.Reflection.Metadata.StringHandle,System.String)", "codegen", "System.Reflection.Metadata.MetadataStringComparer", "StartsWith", True, "benchmark" },
    { 335, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataStringComparer::StartsWith:System.Boolean(System.Reflection.Metadata.StringHandle,System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.MetadataStringComparer", "StartsWith", True, "benchmark" },
    { 336, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataStringComparer::StartsWith:System.Boolean(System.Reflection.Metadata.StringHandle,System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.MetadataStringComparer", "StartsWith", True, "benchmark" },
    { 337, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataStringComparer::StartsWith:System.Boolean(System.Reflection.Metadata.StringHandle,System.String,System.Boolean)", "codegen", "System.Reflection.Metadata.MetadataStringComparer", "StartsWith", True, "benchmark" },
    { 338, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataStringDecoder::GetString:System.String(System.Byte*,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataStringDecoder", "GetString", True, "benchmark" },
    { 339, "System.Reflection.Metadata/System.Reflection.Metadata.MetadataStringDecoder::GetString:System.String(System.Byte*,System.Int32)", "codegen", "System.Reflection.Metadata.MetadataStringDecoder", "GetString", True, "benchmark" },
    { 340, "System.Reflection.Metadata/System.Reflection.Metadata.MethodBodyBlock::GetILBytes:System.Byte[]()", "codegen", "System.Reflection.Metadata.MethodBodyBlock", "GetILBytes", True, "benchmark" },
    { 341, "System.Reflection.Metadata/System.Reflection.Metadata.MethodBodyBlock::GetILContent:System.Collections.Immutable.ImmutableArray<System.Byte>()", "codegen", "System.Reflection.Metadata.MethodBodyBlock", "GetILContent", True, "benchmark" },
    { 342, "System.Reflection.Metadata/System.Reflection.Metadata.MethodBodyBlock::GetILReader:System.Reflection.Metadata.BlobReader()", "codegen", "System.Reflection.Metadata.MethodBodyBlock", "GetILReader", True, "benchmark" },
    { 343, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDebugInformation::GetSequencePoints:System.Reflection.Metadata.SequencePointCollection()", "codegen", "System.Reflection.Metadata.MethodDebugInformation", "GetSequencePoints", True, "benchmark" },
    { 344, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDebugInformation::GetStateMachineKickoffMethod:System.Reflection.Metadata.MethodDefinitionHandle()", "codegen", "System.Reflection.Metadata.MethodDebugInformation", "GetStateMachineKickoffMethod", True, "benchmark" },
    { 345, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDebugInformationHandle::ToDefinitionHandle:System.Reflection.Metadata.MethodDefinitionHandle()", "codegen", "System.Reflection.Metadata.MethodDebugInformationHandle", "ToDefinitionHandle", True, "benchmark" },
    { 346, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDefinition::DecodeSignature:System.Reflection.Metadata.MethodSignature<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.MethodDefinition", "DecodeSignature", True, "benchmark" },
    { 347, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDefinition::DecodeSignature:System.Reflection.Metadata.MethodSignature<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.MethodDefinition", "DecodeSignature", True, "benchmark" },
    { 348, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDefinition::GetDeclaringType:System.Reflection.Metadata.TypeDefinitionHandle()", "codegen", "System.Reflection.Metadata.MethodDefinition", "GetDeclaringType", True, "benchmark" },
    { 349, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDefinition::GetParameters:System.Reflection.Metadata.ParameterHandleCollection()", "codegen", "System.Reflection.Metadata.MethodDefinition", "GetParameters", True, "benchmark" },
    { 350, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDefinition::GetGenericParameters:System.Reflection.Metadata.GenericParameterHandleCollection()", "codegen", "System.Reflection.Metadata.MethodDefinition", "GetGenericParameters", True, "benchmark" },
    { 351, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDefinition::GetImport:System.Reflection.Metadata.MethodImport()", "codegen", "System.Reflection.Metadata.MethodDefinition", "GetImport", True, "benchmark" },
    { 352, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDefinition::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.MethodDefinition", "GetCustomAttributes", True, "benchmark" },
    { 353, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDefinition::GetDeclarativeSecurityAttributes:System.Reflection.Metadata.DeclarativeSecurityAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.MethodDefinition", "GetDeclarativeSecurityAttributes", True, "benchmark" },
    { 354, "System.Reflection.Metadata/System.Reflection.Metadata.MethodDefinitionHandle::ToDebugInformationHandle:System.Reflection.Metadata.MethodDebugInformationHandle()", "codegen", "System.Reflection.Metadata.MethodDefinitionHandle", "ToDebugInformationHandle", True, "benchmark" },
    { 355, "System.Reflection.Metadata/System.Reflection.Metadata.MethodImplementation::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.MethodImplementation", "GetCustomAttributes", True, "benchmark" },
    { 356, "System.Reflection.Metadata/System.Reflection.Metadata.MethodSpecification::DecodeSignature:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.MethodSpecification", "DecodeSignature", True, "benchmark" },
    { 357, "System.Reflection.Metadata/System.Reflection.Metadata.MethodSpecification::DecodeSignature:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.MethodSpecification", "DecodeSignature", True, "benchmark" },
    { 358, "System.Reflection.Metadata/System.Reflection.Metadata.MethodSpecification::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.MethodSpecification", "GetCustomAttributes", True, "benchmark" },
    { 359, "System.Reflection.Metadata/System.Reflection.Metadata.ModuleDefinition::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.ModuleDefinition", "GetCustomAttributes", True, "benchmark" },
    { 360, "System.Reflection.Metadata/System.Reflection.Metadata.ModuleReference::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.ModuleReference", "GetCustomAttributes", True, "benchmark" },
    { 361, "System.Reflection.Metadata/System.Reflection.Metadata.Parameter::GetDefaultValue:System.Reflection.Metadata.ConstantHandle()", "codegen", "System.Reflection.Metadata.Parameter", "GetDefaultValue", True, "benchmark" },
    { 362, "System.Reflection.Metadata/System.Reflection.Metadata.Parameter::GetMarshallingDescriptor:System.Reflection.Metadata.BlobHandle()", "codegen", "System.Reflection.Metadata.Parameter", "GetMarshallingDescriptor", True, "benchmark" },
    { 363, "System.Reflection.Metadata/System.Reflection.Metadata.Parameter::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.Parameter", "GetCustomAttributes", True, "benchmark" },
    { 364, "System.Reflection.Metadata/System.Reflection.Metadata.PEReaderExtensions::GetMethodBody:System.Reflection.Metadata.MethodBodyBlock(System.Reflection.PortableExecutable.PEReader,System.Int32)", "codegen", "System.Reflection.Metadata.PEReaderExtensions", "GetMethodBody", True, "benchmark" },
    { 365, "System.Reflection.Metadata/System.Reflection.Metadata.PEReaderExtensions::GetMethodBody:System.Reflection.Metadata.MethodBodyBlock(System.Reflection.PortableExecutable.PEReader,System.Int32)", "codegen", "System.Reflection.Metadata.PEReaderExtensions", "GetMethodBody", True, "benchmark" },
    { 366, "System.Reflection.Metadata/System.Reflection.Metadata.PEReaderExtensions::GetMetadataReader:System.Reflection.Metadata.MetadataReader(System.Reflection.PortableExecutable.PEReader)", "codegen", "System.Reflection.Metadata.PEReaderExtensions", "GetMetadataReader", True, "benchmark" },
    { 367, "System.Reflection.Metadata/System.Reflection.Metadata.PEReaderExtensions::GetMetadataReader:System.Reflection.Metadata.MetadataReader(System.Reflection.PortableExecutable.PEReader,System.Reflection.Metadata.MetadataReaderOptions)", "codegen", "System.Reflection.Metadata.PEReaderExtensions", "GetMetadataReader", True, "benchmark" },
    { 368, "System.Reflection.Metadata/System.Reflection.Metadata.PEReaderExtensions::GetMetadataReader:System.Reflection.Metadata.MetadataReader(System.Reflection.PortableExecutable.PEReader,System.Reflection.Metadata.MetadataReaderOptions,System.Reflection.Metadata.MetadataStringDecoder)", "codegen", "System.Reflection.Metadata.PEReaderExtensions", "GetMetadataReader", True, "benchmark" },
    { 369, "System.Reflection.Metadata/System.Reflection.Metadata.PropertyDefinition::DecodeSignature:System.Reflection.Metadata.MethodSignature<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.PropertyDefinition", "DecodeSignature", True, "benchmark" },
    { 370, "System.Reflection.Metadata/System.Reflection.Metadata.PropertyDefinition::DecodeSignature:System.Reflection.Metadata.MethodSignature<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.PropertyDefinition", "DecodeSignature", True, "benchmark" },
    { 371, "System.Reflection.Metadata/System.Reflection.Metadata.PropertyDefinition::GetDefaultValue:System.Reflection.Metadata.ConstantHandle()", "codegen", "System.Reflection.Metadata.PropertyDefinition", "GetDefaultValue", True, "benchmark" },
    { 372, "System.Reflection.Metadata/System.Reflection.Metadata.PropertyDefinition::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.PropertyDefinition", "GetCustomAttributes", True, "benchmark" },
    { 373, "System.Reflection.Metadata/System.Reflection.Metadata.PropertyDefinition::GetAccessors:System.Reflection.Metadata.PropertyAccessors()", "codegen", "System.Reflection.Metadata.PropertyDefinition", "GetAccessors", True, "benchmark" },
    { 374, "System.Reflection.Metadata/System.Reflection.Metadata.ReservedBlob<System.Int32>::CreateWriter:System.Reflection.Metadata.BlobWriter()", "codegen", "System.Reflection.Metadata.ReservedBlob<System.Int32>", "CreateWriter", True, "benchmark" },
    { 375, "System.Reflection.Metadata/System.Reflection.Metadata.StandaloneSignature::DecodeMethodSignature:System.Reflection.Metadata.MethodSignature<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.StandaloneSignature", "DecodeMethodSignature", True, "benchmark" },
    { 376, "System.Reflection.Metadata/System.Reflection.Metadata.StandaloneSignature::DecodeMethodSignature:System.Reflection.Metadata.MethodSignature<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.StandaloneSignature", "DecodeMethodSignature", True, "benchmark" },
    { 377, "System.Reflection.Metadata/System.Reflection.Metadata.StandaloneSignature::DecodeLocalSignature:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.StandaloneSignature", "DecodeLocalSignature", True, "benchmark" },
    { 378, "System.Reflection.Metadata/System.Reflection.Metadata.StandaloneSignature::DecodeLocalSignature:System.Collections.Immutable.ImmutableArray<System.Int32>(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.StandaloneSignature", "DecodeLocalSignature", True, "benchmark" },
    { 379, "System.Reflection.Metadata/System.Reflection.Metadata.StandaloneSignature::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.StandaloneSignature", "GetCustomAttributes", True, "benchmark" },
    { 380, "System.Reflection.Metadata/System.Reflection.Metadata.StandaloneSignature::GetKind:System.Reflection.Metadata.StandaloneSignatureKind()", "codegen", "System.Reflection.Metadata.StandaloneSignature", "GetKind", True, "benchmark" },
    { 381, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetLayout:System.Reflection.Metadata.TypeLayout()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetLayout", True, "benchmark" },
    { 382, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetDeclaringType:System.Reflection.Metadata.TypeDefinitionHandle()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetDeclaringType", True, "benchmark" },
    { 383, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetGenericParameters:System.Reflection.Metadata.GenericParameterHandleCollection()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetGenericParameters", True, "benchmark" },
    { 384, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetMethods:System.Reflection.Metadata.MethodDefinitionHandleCollection()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetMethods", True, "benchmark" },
    { 385, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetFields:System.Reflection.Metadata.FieldDefinitionHandleCollection()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetFields", True, "benchmark" },
    { 386, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetProperties:System.Reflection.Metadata.PropertyDefinitionHandleCollection()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetProperties", True, "benchmark" },
    { 387, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetEvents:System.Reflection.Metadata.EventDefinitionHandleCollection()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetEvents", True, "benchmark" },
    { 388, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetNestedTypes:System.Collections.Immutable.ImmutableArray<System.Reflection.Metadata.TypeDefinitionHandle>()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetNestedTypes", True, "benchmark" },
    { 389, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetMethodImplementations:System.Reflection.Metadata.MethodImplementationHandleCollection()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetMethodImplementations", True, "benchmark" },
    { 390, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetInterfaceImplementations:System.Reflection.Metadata.InterfaceImplementationHandleCollection()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetInterfaceImplementations", True, "benchmark" },
    { 391, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetCustomAttributes", True, "benchmark" },
    { 392, "System.Reflection.Metadata/System.Reflection.Metadata.TypeDefinition::GetDeclarativeSecurityAttributes:System.Reflection.Metadata.DeclarativeSecurityAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.TypeDefinition", "GetDeclarativeSecurityAttributes", True, "benchmark" },
    { 393, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::GetNodeCount:System.Int32()", "codegen", "System.Reflection.Metadata.TypeName", "GetNodeCount", True, "benchmark" },
    { 394, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::GetElementType:System.Reflection.Metadata.TypeName()", "codegen", "System.Reflection.Metadata.TypeName", "GetElementType", True, "benchmark" },
    { 395, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::GetGenericTypeDefinition:System.Reflection.Metadata.TypeName()", "codegen", "System.Reflection.Metadata.TypeName", "GetGenericTypeDefinition", True, "benchmark" },
    { 396, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::GetArrayRank:System.Int32()", "codegen", "System.Reflection.Metadata.TypeName", "GetArrayRank", True, "benchmark" },
    { 397, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::GetGenericArguments:System.Collections.Immutable.ImmutableArray<System.Reflection.Metadata.TypeName>()", "codegen", "System.Reflection.Metadata.TypeName", "GetGenericArguments", True, "benchmark" },
    { 398, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::WithAssemblyName:System.Reflection.Metadata.TypeName(System.Reflection.Metadata.AssemblyNameInfo)", "codegen", "System.Reflection.Metadata.TypeName", "WithAssemblyName", True, "benchmark" },
    { 399, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::MakeSZArrayTypeName:System.Reflection.Metadata.TypeName()", "codegen", "System.Reflection.Metadata.TypeName", "MakeSZArrayTypeName", True, "benchmark" },
    { 400, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::MakeArrayTypeName:System.Reflection.Metadata.TypeName(System.Int32)", "codegen", "System.Reflection.Metadata.TypeName", "MakeArrayTypeName", True, "benchmark" },
    { 401, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::MakeArrayTypeName:System.Reflection.Metadata.TypeName(System.Int32)", "codegen", "System.Reflection.Metadata.TypeName", "MakeArrayTypeName", True, "benchmark" },
    { 402, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::MakePointerTypeName:System.Reflection.Metadata.TypeName()", "codegen", "System.Reflection.Metadata.TypeName", "MakePointerTypeName", True, "benchmark" },
    { 403, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::MakeByRefTypeName:System.Reflection.Metadata.TypeName()", "codegen", "System.Reflection.Metadata.TypeName", "MakeByRefTypeName", True, "benchmark" },
    { 404, "System.Reflection.Metadata/System.Reflection.Metadata.TypeName::MakeGenericTypeName:System.Reflection.Metadata.TypeName(System.Collections.Immutable.ImmutableArray<System.Reflection.Metadata.TypeName>)", "codegen", "System.Reflection.Metadata.TypeName", "MakeGenericTypeName", True, "benchmark" },
    { 405, "System.Reflection.Metadata/System.Reflection.Metadata.TypeSpecification::DecodeSignature:System.Int32(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.TypeSpecification", "DecodeSignature", True, "benchmark" },
    { 406, "System.Reflection.Metadata/System.Reflection.Metadata.TypeSpecification::DecodeSignature:System.Int32(System.Reflection.Metadata.ISignatureTypeProvider<System.Int32,System.Int32>,System.Int32)", "codegen", "System.Reflection.Metadata.TypeSpecification", "DecodeSignature", True, "benchmark" },
    { 407, "System.Reflection.Metadata/System.Reflection.Metadata.TypeSpecification::GetCustomAttributes:System.Reflection.Metadata.CustomAttributeHandleCollection()", "codegen", "System.Reflection.Metadata.TypeSpecification", "GetCustomAttributes", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 408;

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