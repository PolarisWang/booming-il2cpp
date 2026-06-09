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
    { 0, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Add:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Add", True, "fact" },
    { 1, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AndNot:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AndNot", True, "fact" },
    { 2, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::As:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "As", True, "fact" },
    { 3, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsByte:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsByte", True, "fact" },
    { 4, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsDouble:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsDouble", True, "fact" },
    { 5, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsInt16:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsInt16", True, "fact" },
    { 6, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsInt32:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsInt32", True, "fact" },
    { 7, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsInt64:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsInt64", True, "fact" },
    { 8, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsNInt:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsNInt", True, "fact" },
    { 9, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsNUInt:System.Runtime.Intrinsics.Vector128<System.UIntPtr>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsNUInt", True, "fact" },
    { 10, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsSByte:System.Runtime.Intrinsics.Vector128<System.SByte>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsSByte", True, "fact" },
    { 11, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsSingle:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsSingle", True, "fact" },
    { 12, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsUInt16:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsUInt16", True, "fact" },
    { 13, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsUInt32:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsUInt32", True, "fact" },
    { 14, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsUInt64:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsUInt64", True, "fact" },
    { 15, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsVector128:System.Runtime.Intrinsics.Vector128<System.Single>(System.Numerics.Vector2)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsVector128", True, "fact" },
    { 16, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsVector128:System.Runtime.Intrinsics.Vector128<System.Single>(System.Numerics.Vector3)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsVector128", True, "fact" },
    { 17, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsVector128:System.Runtime.Intrinsics.Vector128<System.Single>(System.Numerics.Vector4)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsVector128", True, "fact" },
    { 18, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsVector128:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Numerics.Vector<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsVector128", True, "fact" },
    { 19, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsVector128Unsafe:System.Runtime.Intrinsics.Vector128<System.Single>(System.Numerics.Vector2)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsVector128Unsafe", True, "fact" },
    { 20, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::AsVector128Unsafe:System.Runtime.Intrinsics.Vector128<System.Single>(System.Numerics.Vector3)", "codegen", "System.Runtime.Intrinsics.Vector128", "AsVector128Unsafe", True, "fact" },
    { 21, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::BitwiseAnd:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "BitwiseAnd", True, "fact" },
    { 22, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::BitwiseOr:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "BitwiseOr", True, "fact" },
    { 23, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Ceiling:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Ceiling", True, "fact" },
    { 24, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Ceiling:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Ceiling", True, "fact" },
    { 25, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ClampNative:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ClampNative", True, "fact" },
    { 26, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConditionalSelect:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConditionalSelect", True, "fact" },
    { 27, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToDouble:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToDouble", True, "fact" },
    { 28, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToDouble:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToDouble", True, "fact" },
    { 29, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToInt32:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToInt32", True, "fact" },
    { 30, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToInt32Native:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToInt32Native", True, "fact" },
    { 31, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToInt64:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToInt64", True, "fact" },
    { 32, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToInt64Native:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToInt64Native", True, "fact" },
    { 33, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToSingle:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToSingle", True, "fact" },
    { 34, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToSingle:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToSingle", True, "fact" },
    { 35, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToUInt32:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToUInt32", True, "fact" },
    { 36, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToUInt32Native:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToUInt32Native", True, "fact" },
    { 37, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToUInt64:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToUInt64", True, "fact" },
    { 38, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ConvertToUInt64Native:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ConvertToUInt64Native", True, "fact" },
    { 39, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CopySign:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "CopySign", True, "fact" },
    { 40, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CopyTo:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector128", "CopyTo", True, "fact" },
    { 41, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CopyTo:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector128", "CopyTo", True, "fact" },
    { 42, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CopyTo:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector128", "CopyTo", True, "fact" },
    { 43, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CopyTo:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CopyTo", True, "fact" },
    { 44, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CopyTo:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CopyTo", True, "fact" },
    { 45, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CopyTo:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CopyTo", True, "fact" },
    { 46, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CopyTo:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CopyTo", True, "fact" },
    { 47, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Cos:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Cos", True, "fact" },
    { 48, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Cos:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Cos", True, "fact" },
    { 49, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 50, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 51, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 52, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 53, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 54, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 55, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 56, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 57, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 58, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 59, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 60, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 61, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 62, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 63, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 64, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 65, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 66, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 67, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 68, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 69, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 70, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 71, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 72, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 73, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 74, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalar:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalar", True, "fact" },
    { 75, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 76, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 77, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 78, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 79, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 80, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 81, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 82, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 83, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 84, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 85, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 86, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 87, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 88, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 89, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 90, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 91, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 92, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 93, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 94, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 95, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 96, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 97, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 98, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 99, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 100, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateScalarUnsafe", True, "fact" },
    { 101, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateSequence:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateSequence", True, "fact" },
    { 102, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateSequence:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateSequence", True, "fact" },
    { 103, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::CreateSequence:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "CreateSequence", True, "fact" },
    { 104, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::DegreesToRadians:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "DegreesToRadians", True, "fact" },
    { 105, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::DegreesToRadians:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "DegreesToRadians", True, "fact" },
    { 106, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Divide:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Divide", True, "fact" },
    { 107, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Divide:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "Divide", True, "fact" },
    { 108, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Divide:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "Divide", True, "fact" },
    { 109, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::EqualsAll:System.Boolean(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "EqualsAll", True, "fact" },
    { 110, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::EqualsAny:System.Boolean(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "EqualsAny", True, "fact" },
    { 111, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Exp:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Exp", True, "fact" },
    { 112, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Exp:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Exp", True, "fact" },
    { 113, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ExtractMostSignificantBits:System.UInt32(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ExtractMostSignificantBits", True, "fact" },
    { 114, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Floor:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Floor", True, "fact" },
    { 115, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Floor:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Floor", True, "fact" },
    { 116, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::FusedMultiplyAdd:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>,System.Runtime.Intrinsics.Vector128<System.Double>,System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "FusedMultiplyAdd", True, "fact" },
    { 117, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::FusedMultiplyAdd:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>,System.Runtime.Intrinsics.Vector128<System.Single>,System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "FusedMultiplyAdd", True, "fact" },
    { 118, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::GetElement:System.Int32(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "GetElement", True, "fact" },
    { 119, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::GetElement:System.Int32(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "GetElement", True, "fact" },
    { 120, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::GetLower:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "GetLower", True, "fact" },
    { 121, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::GetUpper:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "GetUpper", True, "fact" },
    { 122, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::GreaterThan:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "GreaterThan", True, "fact" },
    { 123, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::GreaterThanAll:System.Boolean(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "GreaterThanAll", True, "fact" },
    { 124, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::GreaterThanAny:System.Boolean(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "GreaterThanAny", True, "fact" },
    { 125, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::GreaterThanOrEqual:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "GreaterThanOrEqual", True, "fact" },
    { 126, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::GreaterThanOrEqualAll:System.Boolean(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "GreaterThanOrEqualAll", True, "fact" },
    { 127, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::GreaterThanOrEqualAny:System.Boolean(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "GreaterThanOrEqualAny", True, "fact" },
    { 128, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Hypot:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>,System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Hypot", True, "fact" },
    { 129, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Hypot:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>,System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Hypot", True, "fact" },
    { 130, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::IsNaN:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "IsNaN", True, "fact" },
    { 131, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::IsNegative:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "IsNegative", True, "fact" },
    { 132, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::IsPositive:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "IsPositive", True, "fact" },
    { 133, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::IsPositiveInfinity:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "IsPositiveInfinity", True, "fact" },
    { 134, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::IsZero:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "IsZero", True, "fact" },
    { 135, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LessThan:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "LessThan", True, "fact" },
    { 136, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LessThanAll:System.Boolean(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "LessThanAll", True, "fact" },
    { 137, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LessThanAny:System.Boolean(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "LessThanAny", True, "fact" },
    { 138, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LessThanOrEqual:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "LessThanOrEqual", True, "fact" },
    { 139, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LessThanOrEqualAll:System.Boolean(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "LessThanOrEqualAll", True, "fact" },
    { 140, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LessThanOrEqualAny:System.Boolean(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "LessThanOrEqualAny", True, "fact" },
    { 141, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Load:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector128", "Load", True, "fact" },
    { 142, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LoadAligned:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector128", "LoadAligned", True, "fact" },
    { 143, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LoadAlignedNonTemporal:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector128", "LoadAlignedNonTemporal", True, "fact" },
    { 144, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LoadUnsafe:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "LoadUnsafe", True, "fact" },
    { 145, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LoadUnsafe:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "LoadUnsafe", True, "fact" },
    { 146, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LoadUnsafe:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "LoadUnsafe", True, "fact" },
    { 147, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LoadUnsafe:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "LoadUnsafe", True, "fact" },
    { 148, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::LoadUnsafe:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "LoadUnsafe", True, "fact" },
    { 149, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::MaxMagnitude:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "MaxMagnitude", True, "fact" },
    { 150, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::MaxMagnitudeNumber:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "MaxMagnitudeNumber", True, "fact" },
    { 151, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::MaxNative:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "MaxNative", True, "fact" },
    { 152, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::MaxNumber:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "MaxNumber", True, "fact" },
    { 153, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::MinMagnitude:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "MinMagnitude", True, "fact" },
    { 154, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::MinMagnitudeNumber:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "MinMagnitudeNumber", True, "fact" },
    { 155, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::MinNative:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "MinNative", True, "fact" },
    { 156, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::MinNumber:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "MinNumber", True, "fact" },
    { 157, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Multiply:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Multiply", True, "fact" },
    { 158, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Multiply:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "Multiply", True, "fact" },
    { 159, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Multiply:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "Multiply", True, "fact" },
    { 160, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Multiply:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Multiply", True, "fact" },
    { 161, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Multiply:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Int32,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Multiply", True, "fact" },
    { 162, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Narrow:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Double>,System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Narrow", True, "fact" },
    { 163, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Narrow:System.Runtime.Intrinsics.Vector128<System.SByte>(System.Runtime.Intrinsics.Vector128<System.Int16>,System.Runtime.Intrinsics.Vector128<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Narrow", True, "fact" },
    { 164, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Narrow:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Narrow", True, "fact" },
    { 165, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Narrow:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int64>,System.Runtime.Intrinsics.Vector128<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Narrow", True, "fact" },
    { 166, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Narrow:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Runtime.Intrinsics.Vector128<System.UInt16>,System.Runtime.Intrinsics.Vector128<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Narrow", True, "fact" },
    { 167, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Narrow:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.Runtime.Intrinsics.Vector128<System.UInt32>,System.Runtime.Intrinsics.Vector128<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Narrow", True, "fact" },
    { 168, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Narrow:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.Runtime.Intrinsics.Vector128<System.UInt64>,System.Runtime.Intrinsics.Vector128<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Narrow", True, "fact" },
    { 169, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Negate:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Negate", True, "fact" },
    { 170, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::OnesComplement:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "OnesComplement", True, "fact" },
    { 171, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::RadiansToDegrees:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "RadiansToDegrees", True, "fact" },
    { 172, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::RadiansToDegrees:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "RadiansToDegrees", True, "fact" },
    { 173, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Round:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Round", True, "fact" },
    { 174, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Round:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Round", True, "fact" },
    { 175, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Round:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector128", "Round", True, "fact" },
    { 176, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Round:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector128", "Round", True, "fact" },
    { 177, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Round:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector128", "Round", True, "fact" },
    { 178, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Round:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector128", "Round", True, "fact" },
    { 179, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Runtime.Intrinsics.Vector128<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 180, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Runtime.Intrinsics.Vector128<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 181, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Runtime.Intrinsics.Vector128<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 182, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Runtime.Intrinsics.Vector128<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 183, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 184, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 185, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Runtime.Intrinsics.Vector128<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 186, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Runtime.Intrinsics.Vector128<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 187, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.Runtime.Intrinsics.Vector128<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 188, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.Runtime.Intrinsics.Vector128<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 189, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.UIntPtr>(System.Runtime.Intrinsics.Vector128<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 190, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.UIntPtr>(System.Runtime.Intrinsics.Vector128<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 191, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.SByte>(System.Runtime.Intrinsics.Vector128<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 192, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.SByte>(System.Runtime.Intrinsics.Vector128<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 193, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.Runtime.Intrinsics.Vector128<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 194, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.Runtime.Intrinsics.Vector128<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 195, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.Runtime.Intrinsics.Vector128<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 196, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.Runtime.Intrinsics.Vector128<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 197, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.Runtime.Intrinsics.Vector128<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 198, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftLeft:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.Runtime.Intrinsics.Vector128<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftLeft", True, "fact" },
    { 199, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Runtime.Intrinsics.Vector128<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightArithmetic", True, "fact" },
    { 200, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Runtime.Intrinsics.Vector128<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightArithmetic", True, "fact" },
    { 201, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightArithmetic", True, "fact" },
    { 202, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightArithmetic", True, "fact" },
    { 203, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Runtime.Intrinsics.Vector128<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightArithmetic", True, "fact" },
    { 204, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Runtime.Intrinsics.Vector128<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightArithmetic", True, "fact" },
    { 205, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.Runtime.Intrinsics.Vector128<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightArithmetic", True, "fact" },
    { 206, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.Runtime.Intrinsics.Vector128<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightArithmetic", True, "fact" },
    { 207, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector128<System.SByte>(System.Runtime.Intrinsics.Vector128<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightArithmetic", True, "fact" },
    { 208, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector128<System.SByte>(System.Runtime.Intrinsics.Vector128<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightArithmetic", True, "fact" },
    { 209, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Runtime.Intrinsics.Vector128<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 210, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Runtime.Intrinsics.Vector128<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 211, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Runtime.Intrinsics.Vector128<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 212, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Runtime.Intrinsics.Vector128<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 213, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 214, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 215, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Runtime.Intrinsics.Vector128<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 216, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Runtime.Intrinsics.Vector128<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 217, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.Runtime.Intrinsics.Vector128<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 218, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.IntPtr>(System.Runtime.Intrinsics.Vector128<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 219, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.UIntPtr>(System.Runtime.Intrinsics.Vector128<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 220, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.UIntPtr>(System.Runtime.Intrinsics.Vector128<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 221, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.SByte>(System.Runtime.Intrinsics.Vector128<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 222, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.SByte>(System.Runtime.Intrinsics.Vector128<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 223, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.Runtime.Intrinsics.Vector128<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 224, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.Runtime.Intrinsics.Vector128<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 225, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.Runtime.Intrinsics.Vector128<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 226, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.Runtime.Intrinsics.Vector128<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 227, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.Runtime.Intrinsics.Vector128<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 228, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ShiftRightLogical:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.Runtime.Intrinsics.Vector128<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "ShiftRightLogical", True, "fact" },
    { 229, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Shuffle:System.Runtime.Intrinsics.Vector128<System.Byte>(System.Runtime.Intrinsics.Vector128<System.Byte>,System.Runtime.Intrinsics.Vector128<System.Byte>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Shuffle", True, "fact" },
    { 230, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Shuffle:System.Runtime.Intrinsics.Vector128<System.SByte>(System.Runtime.Intrinsics.Vector128<System.SByte>,System.Runtime.Intrinsics.Vector128<System.SByte>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Shuffle", True, "fact" },
    { 231, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Shuffle:System.Runtime.Intrinsics.Vector128<System.Int16>(System.Runtime.Intrinsics.Vector128<System.Int16>,System.Runtime.Intrinsics.Vector128<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Shuffle", True, "fact" },
    { 232, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Shuffle:System.Runtime.Intrinsics.Vector128<System.UInt16>(System.Runtime.Intrinsics.Vector128<System.UInt16>,System.Runtime.Intrinsics.Vector128<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Shuffle", True, "fact" },
    { 233, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Shuffle:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Shuffle", True, "fact" },
    { 234, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Shuffle:System.Runtime.Intrinsics.Vector128<System.UInt32>(System.Runtime.Intrinsics.Vector128<System.UInt32>,System.Runtime.Intrinsics.Vector128<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Shuffle", True, "fact" },
    { 235, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Shuffle:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Shuffle", True, "fact" },
    { 236, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Shuffle:System.Runtime.Intrinsics.Vector128<System.Int64>(System.Runtime.Intrinsics.Vector128<System.Int64>,System.Runtime.Intrinsics.Vector128<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Shuffle", True, "fact" },
    { 237, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Shuffle:System.Runtime.Intrinsics.Vector128<System.UInt64>(System.Runtime.Intrinsics.Vector128<System.UInt64>,System.Runtime.Intrinsics.Vector128<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Shuffle", True, "fact" },
    { 238, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Shuffle:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>,System.Runtime.Intrinsics.Vector128<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Shuffle", True, "fact" },
    { 239, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Sin:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Sin", True, "fact" },
    { 240, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Sin:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Sin", True, "fact" },
    { 241, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::SinCos:System.ValueTuple<System.Runtime.Intrinsics.Vector128<System.Double>,System.Runtime.Intrinsics.Vector128<System.Double>>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "SinCos", True, "fact" },
    { 242, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::SinCos:System.ValueTuple<System.Runtime.Intrinsics.Vector128<System.Single>,System.Runtime.Intrinsics.Vector128<System.Single>>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "SinCos", True, "fact" },
    { 243, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Sqrt:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Sqrt", True, "fact" },
    { 244, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Store:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector128", "Store", True, "fact" },
    { 245, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::StoreAligned:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector128", "StoreAligned", True, "fact" },
    { 246, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::StoreAlignedNonTemporal:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector128", "StoreAlignedNonTemporal", True, "fact" },
    { 247, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "StoreUnsafe", True, "fact" },
    { 248, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "StoreUnsafe", True, "fact" },
    { 249, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "StoreUnsafe", True, "fact" },
    { 250, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "StoreUnsafe", True, "fact" },
    { 251, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector128", "StoreUnsafe", True, "fact" },
    { 252, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Subtract:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Subtract", True, "fact" },
    { 253, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ToScalar:System.Int32(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ToScalar", True, "fact" },
    { 254, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ToVector256:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ToVector256", True, "fact" },
    { 255, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::ToVector256Unsafe:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "ToVector256Unsafe", True, "fact" },
    { 256, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Truncate:System.Runtime.Intrinsics.Vector128<System.Double>(System.Runtime.Intrinsics.Vector128<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Truncate", True, "fact" },
    { 257, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Truncate:System.Runtime.Intrinsics.Vector128<System.Single>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Truncate", True, "fact" },
    { 258, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector128<System.UInt16>,System.Runtime.Intrinsics.Vector128<System.UInt16>>(System.Runtime.Intrinsics.Vector128<System.Byte>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Widen", True, "fact" },
    { 259, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>>(System.Runtime.Intrinsics.Vector128<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Widen", True, "fact" },
    { 260, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector128<System.Int64>,System.Runtime.Intrinsics.Vector128<System.Int64>>(System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Widen", True, "fact" },
    { 261, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector128<System.Int16>,System.Runtime.Intrinsics.Vector128<System.Int16>>(System.Runtime.Intrinsics.Vector128<System.SByte>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Widen", True, "fact" },
    { 262, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector128<System.Double>,System.Runtime.Intrinsics.Vector128<System.Double>>(System.Runtime.Intrinsics.Vector128<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Widen", True, "fact" },
    { 263, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector128<System.UInt32>,System.Runtime.Intrinsics.Vector128<System.UInt32>>(System.Runtime.Intrinsics.Vector128<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Widen", True, "fact" },
    { 264, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector128<System.UInt64>,System.Runtime.Intrinsics.Vector128<System.UInt64>>(System.Runtime.Intrinsics.Vector128<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Widen", True, "fact" },
    { 265, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::WithElement:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "WithElement", True, "fact" },
    { 266, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::WithElement:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "WithElement", True, "fact" },
    { 267, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::WithElement:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector128", "WithElement", True, "fact" },
    { 268, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::WithLower:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "WithLower", True, "fact" },
    { 269, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::WithUpper:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "WithUpper", True, "fact" },
    { 270, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector128::Xor:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector128<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector128", "Xor", True, "fact" },
    { 271, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Add:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Add", True, "fact" },
    { 272, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AndNot:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AndNot", True, "fact" },
    { 273, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::As:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "As", True, "fact" },
    { 274, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsByte:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsByte", True, "fact" },
    { 275, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsDouble:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsDouble", True, "fact" },
    { 276, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsInt16:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsInt16", True, "fact" },
    { 277, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsInt32:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsInt32", True, "fact" },
    { 278, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsInt64:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsInt64", True, "fact" },
    { 279, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsNInt:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsNInt", True, "fact" },
    { 280, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsNUInt:System.Runtime.Intrinsics.Vector256<System.UIntPtr>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsNUInt", True, "fact" },
    { 281, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsSByte:System.Runtime.Intrinsics.Vector256<System.SByte>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsSByte", True, "fact" },
    { 282, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsSingle:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsSingle", True, "fact" },
    { 283, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsUInt16:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsUInt16", True, "fact" },
    { 284, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsUInt32:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsUInt32", True, "fact" },
    { 285, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsUInt64:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsUInt64", True, "fact" },
    { 286, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::AsVector256:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Numerics.Vector<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "AsVector256", True, "fact" },
    { 287, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::BitwiseAnd:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "BitwiseAnd", True, "fact" },
    { 288, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::BitwiseOr:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "BitwiseOr", True, "fact" },
    { 289, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Ceiling:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Ceiling", True, "fact" },
    { 290, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Ceiling:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Ceiling", True, "fact" },
    { 291, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ClampNative:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ClampNative", True, "fact" },
    { 292, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConditionalSelect:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConditionalSelect", True, "fact" },
    { 293, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToDouble:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToDouble", True, "fact" },
    { 294, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToDouble:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToDouble", True, "fact" },
    { 295, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToInt32:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToInt32", True, "fact" },
    { 296, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToInt32Native:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToInt32Native", True, "fact" },
    { 297, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToInt64:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToInt64", True, "fact" },
    { 298, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToInt64Native:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToInt64Native", True, "fact" },
    { 299, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToSingle:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToSingle", True, "fact" },
    { 300, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToSingle:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToSingle", True, "fact" },
    { 301, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToUInt32:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToUInt32", True, "fact" },
    { 302, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToUInt32Native:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToUInt32Native", True, "fact" },
    { 303, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToUInt64:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToUInt64", True, "fact" },
    { 304, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ConvertToUInt64Native:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ConvertToUInt64Native", True, "fact" },
    { 305, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CopySign:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "CopySign", True, "fact" },
    { 306, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CopyTo:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector256", "CopyTo", True, "fact" },
    { 307, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CopyTo:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector256", "CopyTo", True, "fact" },
    { 308, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CopyTo:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector256", "CopyTo", True, "fact" },
    { 309, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CopyTo:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CopyTo", True, "fact" },
    { 310, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CopyTo:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CopyTo", True, "fact" },
    { 311, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CopyTo:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CopyTo", True, "fact" },
    { 312, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CopyTo:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CopyTo", True, "fact" },
    { 313, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Cos:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Cos", True, "fact" },
    { 314, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Cos:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Cos", True, "fact" },
    { 315, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 316, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 317, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 318, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 319, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 320, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 321, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 322, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 323, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 324, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 325, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 326, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 327, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 328, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 329, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 330, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 331, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 332, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 333, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 334, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 335, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 336, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 337, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 338, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 339, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 340, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalar:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalar", True, "fact" },
    { 341, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 342, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 343, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 344, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 345, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 346, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 347, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 348, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 349, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 350, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 351, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 352, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 353, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 354, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 355, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 356, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 357, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 358, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 359, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 360, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 361, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 362, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 363, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 364, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 365, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 366, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateScalarUnsafe", True, "fact" },
    { 367, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateSequence:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateSequence", True, "fact" },
    { 368, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateSequence:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateSequence", True, "fact" },
    { 369, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::CreateSequence:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "CreateSequence", True, "fact" },
    { 370, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::DegreesToRadians:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "DegreesToRadians", True, "fact" },
    { 371, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::DegreesToRadians:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "DegreesToRadians", True, "fact" },
    { 372, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Divide:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Divide", True, "fact" },
    { 373, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Divide:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "Divide", True, "fact" },
    { 374, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Divide:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "Divide", True, "fact" },
    { 375, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::EqualsAll:System.Boolean(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "EqualsAll", True, "fact" },
    { 376, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::EqualsAny:System.Boolean(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "EqualsAny", True, "fact" },
    { 377, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Exp:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Exp", True, "fact" },
    { 378, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Exp:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Exp", True, "fact" },
    { 379, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ExtractMostSignificantBits:System.UInt32(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ExtractMostSignificantBits", True, "fact" },
    { 380, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Floor:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Floor", True, "fact" },
    { 381, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Floor:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Floor", True, "fact" },
    { 382, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::FusedMultiplyAdd:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>,System.Runtime.Intrinsics.Vector256<System.Double>,System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "FusedMultiplyAdd", True, "fact" },
    { 383, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::FusedMultiplyAdd:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>,System.Runtime.Intrinsics.Vector256<System.Single>,System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "FusedMultiplyAdd", True, "fact" },
    { 384, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::GetElement:System.Int32(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "GetElement", True, "fact" },
    { 385, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::GetElement:System.Int32(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "GetElement", True, "fact" },
    { 386, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::GetLower:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "GetLower", True, "fact" },
    { 387, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::GetUpper:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "GetUpper", True, "fact" },
    { 388, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::GreaterThan:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "GreaterThan", True, "fact" },
    { 389, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::GreaterThanAll:System.Boolean(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "GreaterThanAll", True, "fact" },
    { 390, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::GreaterThanAny:System.Boolean(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "GreaterThanAny", True, "fact" },
    { 391, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::GreaterThanOrEqual:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "GreaterThanOrEqual", True, "fact" },
    { 392, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::GreaterThanOrEqualAll:System.Boolean(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "GreaterThanOrEqualAll", True, "fact" },
    { 393, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::GreaterThanOrEqualAny:System.Boolean(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "GreaterThanOrEqualAny", True, "fact" },
    { 394, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Hypot:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>,System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Hypot", True, "fact" },
    { 395, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Hypot:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>,System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Hypot", True, "fact" },
    { 396, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::IsNaN:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "IsNaN", True, "fact" },
    { 397, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::IsNegative:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "IsNegative", True, "fact" },
    { 398, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::IsPositive:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "IsPositive", True, "fact" },
    { 399, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::IsPositiveInfinity:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "IsPositiveInfinity", True, "fact" },
    { 400, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::IsZero:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "IsZero", True, "fact" },
    { 401, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LessThan:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "LessThan", True, "fact" },
    { 402, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LessThanAll:System.Boolean(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "LessThanAll", True, "fact" },
    { 403, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LessThanAny:System.Boolean(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "LessThanAny", True, "fact" },
    { 404, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LessThanOrEqual:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "LessThanOrEqual", True, "fact" },
    { 405, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LessThanOrEqualAll:System.Boolean(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "LessThanOrEqualAll", True, "fact" },
    { 406, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LessThanOrEqualAny:System.Boolean(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "LessThanOrEqualAny", True, "fact" },
    { 407, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Load:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector256", "Load", True, "fact" },
    { 408, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LoadAligned:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector256", "LoadAligned", True, "fact" },
    { 409, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LoadAlignedNonTemporal:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector256", "LoadAlignedNonTemporal", True, "fact" },
    { 410, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LoadUnsafe:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "LoadUnsafe", True, "fact" },
    { 411, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LoadUnsafe:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "LoadUnsafe", True, "fact" },
    { 412, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LoadUnsafe:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "LoadUnsafe", True, "fact" },
    { 413, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LoadUnsafe:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "LoadUnsafe", True, "fact" },
    { 414, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::LoadUnsafe:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "LoadUnsafe", True, "fact" },
    { 415, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::MaxMagnitude:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "MaxMagnitude", True, "fact" },
    { 416, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::MaxMagnitudeNumber:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "MaxMagnitudeNumber", True, "fact" },
    { 417, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::MaxNative:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "MaxNative", True, "fact" },
    { 418, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::MaxNumber:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "MaxNumber", True, "fact" },
    { 419, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::MinMagnitude:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "MinMagnitude", True, "fact" },
    { 420, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::MinMagnitudeNumber:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "MinMagnitudeNumber", True, "fact" },
    { 421, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::MinNative:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "MinNative", True, "fact" },
    { 422, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::MinNumber:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "MinNumber", True, "fact" },
    { 423, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Multiply:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Multiply", True, "fact" },
    { 424, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Multiply:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "Multiply", True, "fact" },
    { 425, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Multiply:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "Multiply", True, "fact" },
    { 426, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Multiply:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Multiply", True, "fact" },
    { 427, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Multiply:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Int32,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Multiply", True, "fact" },
    { 428, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Narrow:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Double>,System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Narrow", True, "fact" },
    { 429, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Narrow:System.Runtime.Intrinsics.Vector256<System.SByte>(System.Runtime.Intrinsics.Vector256<System.Int16>,System.Runtime.Intrinsics.Vector256<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Narrow", True, "fact" },
    { 430, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Narrow:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Narrow", True, "fact" },
    { 431, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Narrow:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int64>,System.Runtime.Intrinsics.Vector256<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Narrow", True, "fact" },
    { 432, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Narrow:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Runtime.Intrinsics.Vector256<System.UInt16>,System.Runtime.Intrinsics.Vector256<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Narrow", True, "fact" },
    { 433, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Narrow:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.Runtime.Intrinsics.Vector256<System.UInt32>,System.Runtime.Intrinsics.Vector256<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Narrow", True, "fact" },
    { 434, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Narrow:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.Runtime.Intrinsics.Vector256<System.UInt64>,System.Runtime.Intrinsics.Vector256<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Narrow", True, "fact" },
    { 435, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Negate:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Negate", True, "fact" },
    { 436, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::OnesComplement:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "OnesComplement", True, "fact" },
    { 437, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::RadiansToDegrees:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "RadiansToDegrees", True, "fact" },
    { 438, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::RadiansToDegrees:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "RadiansToDegrees", True, "fact" },
    { 439, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Round:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Round", True, "fact" },
    { 440, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Round:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Round", True, "fact" },
    { 441, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Round:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector256", "Round", True, "fact" },
    { 442, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Round:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector256", "Round", True, "fact" },
    { 443, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Round:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector256", "Round", True, "fact" },
    { 444, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Round:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector256", "Round", True, "fact" },
    { 445, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Runtime.Intrinsics.Vector256<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 446, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Runtime.Intrinsics.Vector256<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 447, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Runtime.Intrinsics.Vector256<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 448, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Runtime.Intrinsics.Vector256<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 449, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 450, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 451, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Runtime.Intrinsics.Vector256<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 452, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Runtime.Intrinsics.Vector256<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 453, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.Runtime.Intrinsics.Vector256<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 454, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.Runtime.Intrinsics.Vector256<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 455, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.UIntPtr>(System.Runtime.Intrinsics.Vector256<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 456, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.UIntPtr>(System.Runtime.Intrinsics.Vector256<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 457, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.SByte>(System.Runtime.Intrinsics.Vector256<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 458, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.SByte>(System.Runtime.Intrinsics.Vector256<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 459, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.Runtime.Intrinsics.Vector256<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 460, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.Runtime.Intrinsics.Vector256<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 461, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.Runtime.Intrinsics.Vector256<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 462, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.Runtime.Intrinsics.Vector256<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 463, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.Runtime.Intrinsics.Vector256<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 464, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftLeft:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.Runtime.Intrinsics.Vector256<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftLeft", True, "fact" },
    { 465, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Runtime.Intrinsics.Vector256<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightArithmetic", True, "fact" },
    { 466, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Runtime.Intrinsics.Vector256<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightArithmetic", True, "fact" },
    { 467, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightArithmetic", True, "fact" },
    { 468, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightArithmetic", True, "fact" },
    { 469, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Runtime.Intrinsics.Vector256<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightArithmetic", True, "fact" },
    { 470, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Runtime.Intrinsics.Vector256<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightArithmetic", True, "fact" },
    { 471, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.Runtime.Intrinsics.Vector256<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightArithmetic", True, "fact" },
    { 472, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.Runtime.Intrinsics.Vector256<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightArithmetic", True, "fact" },
    { 473, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector256<System.SByte>(System.Runtime.Intrinsics.Vector256<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightArithmetic", True, "fact" },
    { 474, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector256<System.SByte>(System.Runtime.Intrinsics.Vector256<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightArithmetic", True, "fact" },
    { 475, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Runtime.Intrinsics.Vector256<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 476, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Runtime.Intrinsics.Vector256<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 477, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Runtime.Intrinsics.Vector256<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 478, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Runtime.Intrinsics.Vector256<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 479, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 480, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 481, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Runtime.Intrinsics.Vector256<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 482, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Runtime.Intrinsics.Vector256<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 483, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.Runtime.Intrinsics.Vector256<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 484, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.IntPtr>(System.Runtime.Intrinsics.Vector256<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 485, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.UIntPtr>(System.Runtime.Intrinsics.Vector256<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 486, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.UIntPtr>(System.Runtime.Intrinsics.Vector256<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 487, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.SByte>(System.Runtime.Intrinsics.Vector256<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 488, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.SByte>(System.Runtime.Intrinsics.Vector256<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 489, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.Runtime.Intrinsics.Vector256<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 490, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.Runtime.Intrinsics.Vector256<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 491, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.Runtime.Intrinsics.Vector256<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 492, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.Runtime.Intrinsics.Vector256<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 493, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.Runtime.Intrinsics.Vector256<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 494, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ShiftRightLogical:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.Runtime.Intrinsics.Vector256<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "ShiftRightLogical", True, "fact" },
    { 495, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Shuffle:System.Runtime.Intrinsics.Vector256<System.Byte>(System.Runtime.Intrinsics.Vector256<System.Byte>,System.Runtime.Intrinsics.Vector256<System.Byte>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Shuffle", True, "fact" },
    { 496, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Shuffle:System.Runtime.Intrinsics.Vector256<System.SByte>(System.Runtime.Intrinsics.Vector256<System.SByte>,System.Runtime.Intrinsics.Vector256<System.SByte>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Shuffle", True, "fact" },
    { 497, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Shuffle:System.Runtime.Intrinsics.Vector256<System.Int16>(System.Runtime.Intrinsics.Vector256<System.Int16>,System.Runtime.Intrinsics.Vector256<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Shuffle", True, "fact" },
    { 498, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Shuffle:System.Runtime.Intrinsics.Vector256<System.UInt16>(System.Runtime.Intrinsics.Vector256<System.UInt16>,System.Runtime.Intrinsics.Vector256<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Shuffle", True, "fact" },
    { 499, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Shuffle:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Shuffle", True, "fact" },
    { 500, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Shuffle:System.Runtime.Intrinsics.Vector256<System.UInt32>(System.Runtime.Intrinsics.Vector256<System.UInt32>,System.Runtime.Intrinsics.Vector256<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Shuffle", True, "fact" },
    { 501, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Shuffle:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Shuffle", True, "fact" },
    { 502, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Shuffle:System.Runtime.Intrinsics.Vector256<System.Int64>(System.Runtime.Intrinsics.Vector256<System.Int64>,System.Runtime.Intrinsics.Vector256<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Shuffle", True, "fact" },
    { 503, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Shuffle:System.Runtime.Intrinsics.Vector256<System.UInt64>(System.Runtime.Intrinsics.Vector256<System.UInt64>,System.Runtime.Intrinsics.Vector256<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Shuffle", True, "fact" },
    { 504, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Shuffle:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>,System.Runtime.Intrinsics.Vector256<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Shuffle", True, "fact" },
    { 505, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Sin:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Sin", True, "fact" },
    { 506, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Sin:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Sin", True, "fact" },
    { 507, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::SinCos:System.ValueTuple<System.Runtime.Intrinsics.Vector256<System.Double>,System.Runtime.Intrinsics.Vector256<System.Double>>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "SinCos", True, "fact" },
    { 508, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::SinCos:System.ValueTuple<System.Runtime.Intrinsics.Vector256<System.Single>,System.Runtime.Intrinsics.Vector256<System.Single>>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "SinCos", True, "fact" },
    { 509, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Sqrt:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Sqrt", True, "fact" },
    { 510, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Store:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector256", "Store", True, "fact" },
    { 511, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::StoreAligned:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector256", "StoreAligned", True, "fact" },
    { 512, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::StoreAlignedNonTemporal:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector256", "StoreAlignedNonTemporal", True, "fact" },
    { 513, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "StoreUnsafe", True, "fact" },
    { 514, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "StoreUnsafe", True, "fact" },
    { 515, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "StoreUnsafe", True, "fact" },
    { 516, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "StoreUnsafe", True, "fact" },
    { 517, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector256", "StoreUnsafe", True, "fact" },
    { 518, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Subtract:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Subtract", True, "fact" },
    { 519, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ToScalar:System.Int32(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ToScalar", True, "fact" },
    { 520, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ToVector512:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ToVector512", True, "fact" },
    { 521, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::ToVector512Unsafe:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "ToVector512Unsafe", True, "fact" },
    { 522, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Truncate:System.Runtime.Intrinsics.Vector256<System.Double>(System.Runtime.Intrinsics.Vector256<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Truncate", True, "fact" },
    { 523, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Truncate:System.Runtime.Intrinsics.Vector256<System.Single>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Truncate", True, "fact" },
    { 524, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector256<System.UInt16>,System.Runtime.Intrinsics.Vector256<System.UInt16>>(System.Runtime.Intrinsics.Vector256<System.Byte>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Widen", True, "fact" },
    { 525, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>>(System.Runtime.Intrinsics.Vector256<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Widen", True, "fact" },
    { 526, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector256<System.Int64>,System.Runtime.Intrinsics.Vector256<System.Int64>>(System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Widen", True, "fact" },
    { 527, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector256<System.Int16>,System.Runtime.Intrinsics.Vector256<System.Int16>>(System.Runtime.Intrinsics.Vector256<System.SByte>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Widen", True, "fact" },
    { 528, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector256<System.Double>,System.Runtime.Intrinsics.Vector256<System.Double>>(System.Runtime.Intrinsics.Vector256<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Widen", True, "fact" },
    { 529, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector256<System.UInt32>,System.Runtime.Intrinsics.Vector256<System.UInt32>>(System.Runtime.Intrinsics.Vector256<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Widen", True, "fact" },
    { 530, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector256<System.UInt64>,System.Runtime.Intrinsics.Vector256<System.UInt64>>(System.Runtime.Intrinsics.Vector256<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Widen", True, "fact" },
    { 531, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::WithElement:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "WithElement", True, "fact" },
    { 532, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::WithElement:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "WithElement", True, "fact" },
    { 533, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::WithElement:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector256", "WithElement", True, "fact" },
    { 534, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::WithLower:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "WithLower", True, "fact" },
    { 535, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::WithUpper:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector128<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "WithUpper", True, "fact" },
    { 536, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector256::Xor:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector256<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector256", "Xor", True, "fact" },
    { 537, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Add:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Add", True, "fact" },
    { 538, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AndNot:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AndNot", True, "fact" },
    { 539, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::As:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "As", True, "fact" },
    { 540, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsByte:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsByte", True, "fact" },
    { 541, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsDouble:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsDouble", True, "fact" },
    { 542, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsInt16:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsInt16", True, "fact" },
    { 543, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsInt32:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsInt32", True, "fact" },
    { 544, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsInt64:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsInt64", True, "fact" },
    { 545, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsNInt:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsNInt", True, "fact" },
    { 546, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsNUInt:System.Runtime.Intrinsics.Vector512<System.UIntPtr>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsNUInt", True, "fact" },
    { 547, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsSByte:System.Runtime.Intrinsics.Vector512<System.SByte>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsSByte", True, "fact" },
    { 548, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsSingle:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsSingle", True, "fact" },
    { 549, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsUInt16:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsUInt16", True, "fact" },
    { 550, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsUInt32:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsUInt32", True, "fact" },
    { 551, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsUInt64:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsUInt64", True, "fact" },
    { 552, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::AsVector512:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Numerics.Vector<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "AsVector512", True, "fact" },
    { 553, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::BitwiseAnd:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "BitwiseAnd", True, "fact" },
    { 554, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::BitwiseOr:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "BitwiseOr", True, "fact" },
    { 555, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Ceiling:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Ceiling", True, "fact" },
    { 556, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Ceiling:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Ceiling", True, "fact" },
    { 557, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ClampNative:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ClampNative", True, "fact" },
    { 558, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConditionalSelect:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConditionalSelect", True, "fact" },
    { 559, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToDouble:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToDouble", True, "fact" },
    { 560, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToDouble:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToDouble", True, "fact" },
    { 561, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToInt32:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToInt32", True, "fact" },
    { 562, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToInt32Native:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToInt32Native", True, "fact" },
    { 563, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToInt64:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToInt64", True, "fact" },
    { 564, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToInt64Native:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToInt64Native", True, "fact" },
    { 565, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToSingle:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToSingle", True, "fact" },
    { 566, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToSingle:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToSingle", True, "fact" },
    { 567, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToUInt32:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToUInt32", True, "fact" },
    { 568, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToUInt32Native:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToUInt32Native", True, "fact" },
    { 569, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToUInt64:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToUInt64", True, "fact" },
    { 570, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ConvertToUInt64Native:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ConvertToUInt64Native", True, "fact" },
    { 571, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CopySign:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "CopySign", True, "fact" },
    { 572, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CopyTo:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector512", "CopyTo", True, "fact" },
    { 573, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CopyTo:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector512", "CopyTo", True, "fact" },
    { 574, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CopyTo:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector512", "CopyTo", True, "fact" },
    { 575, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CopyTo:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CopyTo", True, "fact" },
    { 576, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CopyTo:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CopyTo", True, "fact" },
    { 577, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CopyTo:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CopyTo", True, "fact" },
    { 578, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CopyTo:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CopyTo", True, "fact" },
    { 579, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Cos:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Cos", True, "fact" },
    { 580, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Cos:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Cos", True, "fact" },
    { 581, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 582, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 583, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 584, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 585, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 586, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 587, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 588, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 589, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 590, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 591, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 592, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 593, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 594, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 595, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 596, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 597, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 598, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 599, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 600, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 601, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 602, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 603, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 604, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 605, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 606, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalar:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalar", True, "fact" },
    { 607, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 608, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 609, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 610, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 611, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 612, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 613, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 614, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 615, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 616, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 617, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 618, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 619, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 620, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 621, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 622, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 623, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 624, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 625, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 626, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 627, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 628, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 629, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 630, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 631, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 632, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateScalarUnsafe", True, "fact" },
    { 633, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateSequence:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateSequence", True, "fact" },
    { 634, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateSequence:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateSequence", True, "fact" },
    { 635, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::CreateSequence:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "CreateSequence", True, "fact" },
    { 636, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::DegreesToRadians:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "DegreesToRadians", True, "fact" },
    { 637, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::DegreesToRadians:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "DegreesToRadians", True, "fact" },
    { 638, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Divide:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Divide", True, "fact" },
    { 639, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Divide:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "Divide", True, "fact" },
    { 640, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Divide:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "Divide", True, "fact" },
    { 641, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::EqualsAll:System.Boolean(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "EqualsAll", True, "fact" },
    { 642, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::EqualsAny:System.Boolean(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "EqualsAny", True, "fact" },
    { 643, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Exp:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Exp", True, "fact" },
    { 644, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Exp:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Exp", True, "fact" },
    { 645, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ExtractMostSignificantBits:System.UInt64(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ExtractMostSignificantBits", True, "fact" },
    { 646, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Floor:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Floor", True, "fact" },
    { 647, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Floor:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Floor", True, "fact" },
    { 648, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::FusedMultiplyAdd:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>,System.Runtime.Intrinsics.Vector512<System.Double>,System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "FusedMultiplyAdd", True, "fact" },
    { 649, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::FusedMultiplyAdd:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>,System.Runtime.Intrinsics.Vector512<System.Single>,System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "FusedMultiplyAdd", True, "fact" },
    { 650, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::GetElement:System.Int32(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "GetElement", True, "fact" },
    { 651, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::GetElement:System.Int32(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "GetElement", True, "fact" },
    { 652, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::GetLower:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "GetLower", True, "fact" },
    { 653, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::GetUpper:System.Runtime.Intrinsics.Vector256<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "GetUpper", True, "fact" },
    { 654, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::GreaterThan:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "GreaterThan", True, "fact" },
    { 655, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::GreaterThanAll:System.Boolean(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "GreaterThanAll", True, "fact" },
    { 656, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::GreaterThanAny:System.Boolean(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "GreaterThanAny", True, "fact" },
    { 657, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::GreaterThanOrEqual:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "GreaterThanOrEqual", True, "fact" },
    { 658, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::GreaterThanOrEqualAll:System.Boolean(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "GreaterThanOrEqualAll", True, "fact" },
    { 659, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::GreaterThanOrEqualAny:System.Boolean(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "GreaterThanOrEqualAny", True, "fact" },
    { 660, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Hypot:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>,System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Hypot", True, "fact" },
    { 661, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Hypot:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>,System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Hypot", True, "fact" },
    { 662, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::IsNaN:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "IsNaN", True, "fact" },
    { 663, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::IsNegative:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "IsNegative", True, "fact" },
    { 664, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::IsPositive:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "IsPositive", True, "fact" },
    { 665, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::IsPositiveInfinity:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "IsPositiveInfinity", True, "fact" },
    { 666, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::IsZero:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "IsZero", True, "fact" },
    { 667, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LessThan:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "LessThan", True, "fact" },
    { 668, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LessThanAll:System.Boolean(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "LessThanAll", True, "fact" },
    { 669, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LessThanAny:System.Boolean(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "LessThanAny", True, "fact" },
    { 670, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LessThanOrEqual:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "LessThanOrEqual", True, "fact" },
    { 671, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LessThanOrEqualAll:System.Boolean(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "LessThanOrEqualAll", True, "fact" },
    { 672, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LessThanOrEqualAny:System.Boolean(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "LessThanOrEqualAny", True, "fact" },
    { 673, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Load:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector512", "Load", True, "fact" },
    { 674, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LoadAligned:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector512", "LoadAligned", True, "fact" },
    { 675, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LoadAlignedNonTemporal:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector512", "LoadAlignedNonTemporal", True, "fact" },
    { 676, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LoadUnsafe:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "LoadUnsafe", True, "fact" },
    { 677, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LoadUnsafe:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "LoadUnsafe", True, "fact" },
    { 678, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LoadUnsafe:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "LoadUnsafe", True, "fact" },
    { 679, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LoadUnsafe:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "LoadUnsafe", True, "fact" },
    { 680, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::LoadUnsafe:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "LoadUnsafe", True, "fact" },
    { 681, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::MaxMagnitude:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "MaxMagnitude", True, "fact" },
    { 682, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::MaxMagnitudeNumber:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "MaxMagnitudeNumber", True, "fact" },
    { 683, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::MaxNative:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "MaxNative", True, "fact" },
    { 684, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::MaxNumber:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "MaxNumber", True, "fact" },
    { 685, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::MinMagnitude:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "MinMagnitude", True, "fact" },
    { 686, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::MinMagnitudeNumber:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "MinMagnitudeNumber", True, "fact" },
    { 687, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::MinNative:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "MinNative", True, "fact" },
    { 688, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::MinNumber:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "MinNumber", True, "fact" },
    { 689, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Multiply:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Multiply", True, "fact" },
    { 690, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Multiply:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "Multiply", True, "fact" },
    { 691, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Multiply:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "Multiply", True, "fact" },
    { 692, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Multiply:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Multiply", True, "fact" },
    { 693, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Multiply:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Int32,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Multiply", True, "fact" },
    { 694, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Narrow:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Double>,System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Narrow", True, "fact" },
    { 695, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Narrow:System.Runtime.Intrinsics.Vector512<System.SByte>(System.Runtime.Intrinsics.Vector512<System.Int16>,System.Runtime.Intrinsics.Vector512<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Narrow", True, "fact" },
    { 696, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Narrow:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Narrow", True, "fact" },
    { 697, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Narrow:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int64>,System.Runtime.Intrinsics.Vector512<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Narrow", True, "fact" },
    { 698, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Narrow:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Runtime.Intrinsics.Vector512<System.UInt16>,System.Runtime.Intrinsics.Vector512<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Narrow", True, "fact" },
    { 699, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Narrow:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.Runtime.Intrinsics.Vector512<System.UInt32>,System.Runtime.Intrinsics.Vector512<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Narrow", True, "fact" },
    { 700, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Narrow:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.Runtime.Intrinsics.Vector512<System.UInt64>,System.Runtime.Intrinsics.Vector512<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Narrow", True, "fact" },
    { 701, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Negate:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Negate", True, "fact" },
    { 702, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::OnesComplement:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "OnesComplement", True, "fact" },
    { 703, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::RadiansToDegrees:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "RadiansToDegrees", True, "fact" },
    { 704, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::RadiansToDegrees:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "RadiansToDegrees", True, "fact" },
    { 705, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Round:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Round", True, "fact" },
    { 706, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Round:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Round", True, "fact" },
    { 707, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Round:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector512", "Round", True, "fact" },
    { 708, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Round:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector512", "Round", True, "fact" },
    { 709, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Round:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector512", "Round", True, "fact" },
    { 710, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Round:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector512", "Round", True, "fact" },
    { 711, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Runtime.Intrinsics.Vector512<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 712, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Runtime.Intrinsics.Vector512<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 713, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Runtime.Intrinsics.Vector512<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 714, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Runtime.Intrinsics.Vector512<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 715, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 716, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 717, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Runtime.Intrinsics.Vector512<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 718, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Runtime.Intrinsics.Vector512<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 719, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.Runtime.Intrinsics.Vector512<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 720, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.Runtime.Intrinsics.Vector512<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 721, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.UIntPtr>(System.Runtime.Intrinsics.Vector512<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 722, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.UIntPtr>(System.Runtime.Intrinsics.Vector512<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 723, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.SByte>(System.Runtime.Intrinsics.Vector512<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 724, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.SByte>(System.Runtime.Intrinsics.Vector512<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 725, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.Runtime.Intrinsics.Vector512<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 726, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.Runtime.Intrinsics.Vector512<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 727, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.Runtime.Intrinsics.Vector512<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 728, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.Runtime.Intrinsics.Vector512<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 729, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.Runtime.Intrinsics.Vector512<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 730, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftLeft:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.Runtime.Intrinsics.Vector512<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftLeft", True, "fact" },
    { 731, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Runtime.Intrinsics.Vector512<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightArithmetic", True, "fact" },
    { 732, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Runtime.Intrinsics.Vector512<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightArithmetic", True, "fact" },
    { 733, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightArithmetic", True, "fact" },
    { 734, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightArithmetic", True, "fact" },
    { 735, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Runtime.Intrinsics.Vector512<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightArithmetic", True, "fact" },
    { 736, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Runtime.Intrinsics.Vector512<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightArithmetic", True, "fact" },
    { 737, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.Runtime.Intrinsics.Vector512<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightArithmetic", True, "fact" },
    { 738, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.Runtime.Intrinsics.Vector512<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightArithmetic", True, "fact" },
    { 739, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector512<System.SByte>(System.Runtime.Intrinsics.Vector512<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightArithmetic", True, "fact" },
    { 740, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector512<System.SByte>(System.Runtime.Intrinsics.Vector512<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightArithmetic", True, "fact" },
    { 741, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Runtime.Intrinsics.Vector512<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 742, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Runtime.Intrinsics.Vector512<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 743, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Runtime.Intrinsics.Vector512<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 744, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Runtime.Intrinsics.Vector512<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 745, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 746, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 747, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Runtime.Intrinsics.Vector512<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 748, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Runtime.Intrinsics.Vector512<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 749, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.Runtime.Intrinsics.Vector512<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 750, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.IntPtr>(System.Runtime.Intrinsics.Vector512<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 751, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.UIntPtr>(System.Runtime.Intrinsics.Vector512<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 752, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.UIntPtr>(System.Runtime.Intrinsics.Vector512<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 753, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.SByte>(System.Runtime.Intrinsics.Vector512<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 754, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.SByte>(System.Runtime.Intrinsics.Vector512<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 755, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.Runtime.Intrinsics.Vector512<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 756, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.Runtime.Intrinsics.Vector512<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 757, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.Runtime.Intrinsics.Vector512<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 758, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.Runtime.Intrinsics.Vector512<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 759, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.Runtime.Intrinsics.Vector512<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 760, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ShiftRightLogical:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.Runtime.Intrinsics.Vector512<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "ShiftRightLogical", True, "fact" },
    { 761, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Shuffle:System.Runtime.Intrinsics.Vector512<System.Byte>(System.Runtime.Intrinsics.Vector512<System.Byte>,System.Runtime.Intrinsics.Vector512<System.Byte>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Shuffle", True, "fact" },
    { 762, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Shuffle:System.Runtime.Intrinsics.Vector512<System.SByte>(System.Runtime.Intrinsics.Vector512<System.SByte>,System.Runtime.Intrinsics.Vector512<System.SByte>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Shuffle", True, "fact" },
    { 763, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Shuffle:System.Runtime.Intrinsics.Vector512<System.Int16>(System.Runtime.Intrinsics.Vector512<System.Int16>,System.Runtime.Intrinsics.Vector512<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Shuffle", True, "fact" },
    { 764, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Shuffle:System.Runtime.Intrinsics.Vector512<System.UInt16>(System.Runtime.Intrinsics.Vector512<System.UInt16>,System.Runtime.Intrinsics.Vector512<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Shuffle", True, "fact" },
    { 765, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Shuffle:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Shuffle", True, "fact" },
    { 766, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Shuffle:System.Runtime.Intrinsics.Vector512<System.UInt32>(System.Runtime.Intrinsics.Vector512<System.UInt32>,System.Runtime.Intrinsics.Vector512<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Shuffle", True, "fact" },
    { 767, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Shuffle:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Shuffle", True, "fact" },
    { 768, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Shuffle:System.Runtime.Intrinsics.Vector512<System.Int64>(System.Runtime.Intrinsics.Vector512<System.Int64>,System.Runtime.Intrinsics.Vector512<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Shuffle", True, "fact" },
    { 769, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Shuffle:System.Runtime.Intrinsics.Vector512<System.UInt64>(System.Runtime.Intrinsics.Vector512<System.UInt64>,System.Runtime.Intrinsics.Vector512<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Shuffle", True, "fact" },
    { 770, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Shuffle:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>,System.Runtime.Intrinsics.Vector512<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Shuffle", True, "fact" },
    { 771, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Sin:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Sin", True, "fact" },
    { 772, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Sin:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Sin", True, "fact" },
    { 773, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::SinCos:System.ValueTuple<System.Runtime.Intrinsics.Vector512<System.Double>,System.Runtime.Intrinsics.Vector512<System.Double>>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "SinCos", True, "fact" },
    { 774, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::SinCos:System.ValueTuple<System.Runtime.Intrinsics.Vector512<System.Single>,System.Runtime.Intrinsics.Vector512<System.Single>>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "SinCos", True, "fact" },
    { 775, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Sqrt:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Sqrt", True, "fact" },
    { 776, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Store:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector512", "Store", True, "fact" },
    { 777, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::StoreAligned:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector512", "StoreAligned", True, "fact" },
    { 778, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::StoreAlignedNonTemporal:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector512", "StoreAlignedNonTemporal", True, "fact" },
    { 779, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "StoreUnsafe", True, "fact" },
    { 780, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "StoreUnsafe", True, "fact" },
    { 781, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "StoreUnsafe", True, "fact" },
    { 782, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "StoreUnsafe", True, "fact" },
    { 783, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector512", "StoreUnsafe", True, "fact" },
    { 784, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Subtract:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Subtract", True, "fact" },
    { 785, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::ToScalar:System.Int32(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "ToScalar", True, "fact" },
    { 786, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Truncate:System.Runtime.Intrinsics.Vector512<System.Double>(System.Runtime.Intrinsics.Vector512<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Truncate", True, "fact" },
    { 787, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Truncate:System.Runtime.Intrinsics.Vector512<System.Single>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Truncate", True, "fact" },
    { 788, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector512<System.UInt16>,System.Runtime.Intrinsics.Vector512<System.UInt16>>(System.Runtime.Intrinsics.Vector512<System.Byte>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Widen", True, "fact" },
    { 789, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>>(System.Runtime.Intrinsics.Vector512<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Widen", True, "fact" },
    { 790, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector512<System.Int64>,System.Runtime.Intrinsics.Vector512<System.Int64>>(System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Widen", True, "fact" },
    { 791, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector512<System.Int16>,System.Runtime.Intrinsics.Vector512<System.Int16>>(System.Runtime.Intrinsics.Vector512<System.SByte>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Widen", True, "fact" },
    { 792, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector512<System.Double>,System.Runtime.Intrinsics.Vector512<System.Double>>(System.Runtime.Intrinsics.Vector512<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Widen", True, "fact" },
    { 793, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector512<System.UInt32>,System.Runtime.Intrinsics.Vector512<System.UInt32>>(System.Runtime.Intrinsics.Vector512<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Widen", True, "fact" },
    { 794, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector512<System.UInt64>,System.Runtime.Intrinsics.Vector512<System.UInt64>>(System.Runtime.Intrinsics.Vector512<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Widen", True, "fact" },
    { 795, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::WithElement:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "WithElement", True, "fact" },
    { 796, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::WithElement:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "WithElement", True, "fact" },
    { 797, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::WithElement:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector512", "WithElement", True, "fact" },
    { 798, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::WithLower:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "WithLower", True, "fact" },
    { 799, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::WithUpper:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector256<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "WithUpper", True, "fact" },
    { 800, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector512::Xor:System.Runtime.Intrinsics.Vector512<System.Int32>(System.Runtime.Intrinsics.Vector512<System.Int32>,System.Runtime.Intrinsics.Vector512<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector512", "Xor", True, "fact" },
    { 801, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Add:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Add", True, "fact" },
    { 802, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AndNot:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AndNot", True, "fact" },
    { 803, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::As:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "As", True, "fact" },
    { 804, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsByte:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsByte", True, "fact" },
    { 805, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsDouble:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsDouble", True, "fact" },
    { 806, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsInt16:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsInt16", True, "fact" },
    { 807, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsInt32:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsInt32", True, "fact" },
    { 808, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsInt64:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsInt64", True, "fact" },
    { 809, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsNInt:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsNInt", True, "fact" },
    { 810, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsNUInt:System.Runtime.Intrinsics.Vector64<System.UIntPtr>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsNUInt", True, "fact" },
    { 811, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsSByte:System.Runtime.Intrinsics.Vector64<System.SByte>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsSByte", True, "fact" },
    { 812, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsSingle:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsSingle", True, "fact" },
    { 813, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsUInt16:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsUInt16", True, "fact" },
    { 814, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsUInt32:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsUInt32", True, "fact" },
    { 815, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::AsUInt64:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "AsUInt64", True, "fact" },
    { 816, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::BitwiseAnd:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "BitwiseAnd", True, "fact" },
    { 817, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::BitwiseOr:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "BitwiseOr", True, "fact" },
    { 818, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Ceiling:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Ceiling", True, "fact" },
    { 819, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Ceiling:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Ceiling", True, "fact" },
    { 820, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ClampNative:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ClampNative", True, "fact" },
    { 821, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConditionalSelect:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConditionalSelect", True, "fact" },
    { 822, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToDouble:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToDouble", True, "fact" },
    { 823, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToDouble:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToDouble", True, "fact" },
    { 824, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToInt32:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToInt32", True, "fact" },
    { 825, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToInt32Native:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToInt32Native", True, "fact" },
    { 826, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToInt64:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToInt64", True, "fact" },
    { 827, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToInt64Native:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToInt64Native", True, "fact" },
    { 828, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToSingle:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToSingle", True, "fact" },
    { 829, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToSingle:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToSingle", True, "fact" },
    { 830, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToUInt32:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToUInt32", True, "fact" },
    { 831, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToUInt32Native:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToUInt32Native", True, "fact" },
    { 832, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToUInt64:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToUInt64", True, "fact" },
    { 833, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ConvertToUInt64Native:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ConvertToUInt64Native", True, "fact" },
    { 834, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CopySign:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "CopySign", True, "fact" },
    { 835, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CopyTo:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector64", "CopyTo", True, "fact" },
    { 836, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CopyTo:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector64", "CopyTo", True, "fact" },
    { 837, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CopyTo:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32[])", "codegen", "System.Runtime.Intrinsics.Vector64", "CopyTo", True, "fact" },
    { 838, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CopyTo:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CopyTo", True, "fact" },
    { 839, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CopyTo:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CopyTo", True, "fact" },
    { 840, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CopyTo:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CopyTo", True, "fact" },
    { 841, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CopyTo:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32[],System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CopyTo", True, "fact" },
    { 842, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Cos:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Cos", True, "fact" },
    { 843, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Cos:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Cos", True, "fact" },
    { 844, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 845, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 846, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 847, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 848, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 849, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 850, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 851, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 852, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 853, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 854, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 855, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 856, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 857, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 858, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 859, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 860, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 861, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 862, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 863, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 864, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 865, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 866, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 867, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 868, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 869, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalar:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalar", True, "fact" },
    { 870, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 871, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 872, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 873, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Byte)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 874, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 875, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Double>(System.Double)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 876, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 877, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Int16)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 878, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 879, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 880, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 881, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Int64)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 882, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 883, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.IntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 884, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 885, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.UIntPtr>(System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 886, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 887, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.SByte>(System.SByte)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 888, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 889, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.Single>(System.Single)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 890, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 891, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.UInt16)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 892, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 893, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.UInt32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 894, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 895, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateScalarUnsafe:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.UInt64)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateScalarUnsafe", True, "fact" },
    { 896, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateSequence:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateSequence", True, "fact" },
    { 897, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateSequence:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateSequence", True, "fact" },
    { 898, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::CreateSequence:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "CreateSequence", True, "fact" },
    { 899, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::DegreesToRadians:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "DegreesToRadians", True, "fact" },
    { 900, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::DegreesToRadians:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "DegreesToRadians", True, "fact" },
    { 901, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Divide:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Divide", True, "fact" },
    { 902, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Divide:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "Divide", True, "fact" },
    { 903, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Divide:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "Divide", True, "fact" },
    { 904, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::EqualsAll:System.Boolean(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "EqualsAll", True, "fact" },
    { 905, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::EqualsAny:System.Boolean(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "EqualsAny", True, "fact" },
    { 906, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Exp:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Exp", True, "fact" },
    { 907, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Exp:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Exp", True, "fact" },
    { 908, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ExtractMostSignificantBits:System.UInt32(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ExtractMostSignificantBits", True, "fact" },
    { 909, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Floor:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Floor", True, "fact" },
    { 910, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Floor:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Floor", True, "fact" },
    { 911, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::FusedMultiplyAdd:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>,System.Runtime.Intrinsics.Vector64<System.Double>,System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "FusedMultiplyAdd", True, "fact" },
    { 912, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::FusedMultiplyAdd:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>,System.Runtime.Intrinsics.Vector64<System.Single>,System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "FusedMultiplyAdd", True, "fact" },
    { 913, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::GetElement:System.Int32(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "GetElement", True, "fact" },
    { 914, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::GetElement:System.Int32(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "GetElement", True, "fact" },
    { 915, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::GreaterThan:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "GreaterThan", True, "fact" },
    { 916, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::GreaterThanAll:System.Boolean(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "GreaterThanAll", True, "fact" },
    { 917, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::GreaterThanAny:System.Boolean(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "GreaterThanAny", True, "fact" },
    { 918, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::GreaterThanOrEqual:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "GreaterThanOrEqual", True, "fact" },
    { 919, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::GreaterThanOrEqualAll:System.Boolean(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "GreaterThanOrEqualAll", True, "fact" },
    { 920, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::GreaterThanOrEqualAny:System.Boolean(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "GreaterThanOrEqualAny", True, "fact" },
    { 921, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Hypot:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>,System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Hypot", True, "fact" },
    { 922, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Hypot:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>,System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Hypot", True, "fact" },
    { 923, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::IsNaN:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "IsNaN", True, "fact" },
    { 924, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::IsNegative:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "IsNegative", True, "fact" },
    { 925, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::IsPositive:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "IsPositive", True, "fact" },
    { 926, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::IsPositiveInfinity:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "IsPositiveInfinity", True, "fact" },
    { 927, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::IsZero:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "IsZero", True, "fact" },
    { 928, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LessThan:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "LessThan", True, "fact" },
    { 929, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LessThanAll:System.Boolean(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "LessThanAll", True, "fact" },
    { 930, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LessThanAny:System.Boolean(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "LessThanAny", True, "fact" },
    { 931, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LessThanOrEqual:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "LessThanOrEqual", True, "fact" },
    { 932, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LessThanOrEqualAll:System.Boolean(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "LessThanOrEqualAll", True, "fact" },
    { 933, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LessThanOrEqualAny:System.Boolean(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "LessThanOrEqualAny", True, "fact" },
    { 934, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Load:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector64", "Load", True, "fact" },
    { 935, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LoadAligned:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector64", "LoadAligned", True, "fact" },
    { 936, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LoadAlignedNonTemporal:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector64", "LoadAlignedNonTemporal", True, "fact" },
    { 937, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LoadUnsafe:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "LoadUnsafe", True, "fact" },
    { 938, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LoadUnsafe:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "LoadUnsafe", True, "fact" },
    { 939, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LoadUnsafe:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "LoadUnsafe", True, "fact" },
    { 940, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LoadUnsafe:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "LoadUnsafe", True, "fact" },
    { 941, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::LoadUnsafe:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "LoadUnsafe", True, "fact" },
    { 942, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::MaxMagnitude:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "MaxMagnitude", True, "fact" },
    { 943, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::MaxMagnitudeNumber:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "MaxMagnitudeNumber", True, "fact" },
    { 944, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::MaxNative:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "MaxNative", True, "fact" },
    { 945, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::MaxNumber:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "MaxNumber", True, "fact" },
    { 946, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::MinMagnitude:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "MinMagnitude", True, "fact" },
    { 947, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::MinMagnitudeNumber:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "MinMagnitudeNumber", True, "fact" },
    { 948, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::MinNative:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "MinNative", True, "fact" },
    { 949, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::MinNumber:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "MinNumber", True, "fact" },
    { 950, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Multiply:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Multiply", True, "fact" },
    { 951, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Multiply:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "Multiply", True, "fact" },
    { 952, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Multiply:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "Multiply", True, "fact" },
    { 953, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Multiply:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Multiply", True, "fact" },
    { 954, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Multiply:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Int32,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Multiply", True, "fact" },
    { 955, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Narrow:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Double>,System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Narrow", True, "fact" },
    { 956, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Narrow:System.Runtime.Intrinsics.Vector64<System.SByte>(System.Runtime.Intrinsics.Vector64<System.Int16>,System.Runtime.Intrinsics.Vector64<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Narrow", True, "fact" },
    { 957, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Narrow:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Narrow", True, "fact" },
    { 958, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Narrow:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int64>,System.Runtime.Intrinsics.Vector64<System.Int64>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Narrow", True, "fact" },
    { 959, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Narrow:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Runtime.Intrinsics.Vector64<System.UInt16>,System.Runtime.Intrinsics.Vector64<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Narrow", True, "fact" },
    { 960, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Narrow:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.Runtime.Intrinsics.Vector64<System.UInt32>,System.Runtime.Intrinsics.Vector64<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Narrow", True, "fact" },
    { 961, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Narrow:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.Runtime.Intrinsics.Vector64<System.UInt64>,System.Runtime.Intrinsics.Vector64<System.UInt64>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Narrow", True, "fact" },
    { 962, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Negate:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Negate", True, "fact" },
    { 963, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::OnesComplement:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "OnesComplement", True, "fact" },
    { 964, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::RadiansToDegrees:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "RadiansToDegrees", True, "fact" },
    { 965, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::RadiansToDegrees:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "RadiansToDegrees", True, "fact" },
    { 966, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Round:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Round", True, "fact" },
    { 967, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Round:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Round", True, "fact" },
    { 968, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Round:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector64", "Round", True, "fact" },
    { 969, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Round:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector64", "Round", True, "fact" },
    { 970, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Round:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector64", "Round", True, "fact" },
    { 971, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Round:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>,System.MidpointRounding)", "codegen", "System.Runtime.Intrinsics.Vector64", "Round", True, "fact" },
    { 972, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Runtime.Intrinsics.Vector64<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 973, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Runtime.Intrinsics.Vector64<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 974, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Runtime.Intrinsics.Vector64<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 975, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Runtime.Intrinsics.Vector64<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 976, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 977, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 978, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Runtime.Intrinsics.Vector64<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 979, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Runtime.Intrinsics.Vector64<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 980, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.Runtime.Intrinsics.Vector64<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 981, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.Runtime.Intrinsics.Vector64<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 982, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.UIntPtr>(System.Runtime.Intrinsics.Vector64<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 983, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.UIntPtr>(System.Runtime.Intrinsics.Vector64<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 984, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.SByte>(System.Runtime.Intrinsics.Vector64<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 985, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.SByte>(System.Runtime.Intrinsics.Vector64<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 986, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.Runtime.Intrinsics.Vector64<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 987, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.Runtime.Intrinsics.Vector64<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 988, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.Runtime.Intrinsics.Vector64<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 989, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.Runtime.Intrinsics.Vector64<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 990, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.Runtime.Intrinsics.Vector64<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 991, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftLeft:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.Runtime.Intrinsics.Vector64<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftLeft", True, "fact" },
    { 992, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Runtime.Intrinsics.Vector64<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightArithmetic", True, "fact" },
    { 993, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Runtime.Intrinsics.Vector64<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightArithmetic", True, "fact" },
    { 994, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightArithmetic", True, "fact" },
    { 995, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightArithmetic", True, "fact" },
    { 996, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Runtime.Intrinsics.Vector64<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightArithmetic", True, "fact" },
    { 997, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Runtime.Intrinsics.Vector64<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightArithmetic", True, "fact" },
    { 998, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.Runtime.Intrinsics.Vector64<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightArithmetic", True, "fact" },
    { 999, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.Runtime.Intrinsics.Vector64<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightArithmetic", True, "fact" },
    { 1000, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector64<System.SByte>(System.Runtime.Intrinsics.Vector64<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightArithmetic", True, "fact" },
    { 1001, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightArithmetic:System.Runtime.Intrinsics.Vector64<System.SByte>(System.Runtime.Intrinsics.Vector64<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightArithmetic", True, "fact" },
    { 1002, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Runtime.Intrinsics.Vector64<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1003, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Runtime.Intrinsics.Vector64<System.Byte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1004, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Runtime.Intrinsics.Vector64<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1005, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Runtime.Intrinsics.Vector64<System.Int16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1006, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1007, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1008, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Runtime.Intrinsics.Vector64<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1009, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.Int64>(System.Runtime.Intrinsics.Vector64<System.Int64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1010, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.Runtime.Intrinsics.Vector64<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1011, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.IntPtr>(System.Runtime.Intrinsics.Vector64<System.IntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1012, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.UIntPtr>(System.Runtime.Intrinsics.Vector64<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1013, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.UIntPtr>(System.Runtime.Intrinsics.Vector64<System.UIntPtr>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1014, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.SByte>(System.Runtime.Intrinsics.Vector64<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1015, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.SByte>(System.Runtime.Intrinsics.Vector64<System.SByte>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1016, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.Runtime.Intrinsics.Vector64<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1017, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.Runtime.Intrinsics.Vector64<System.UInt16>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1018, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.Runtime.Intrinsics.Vector64<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1019, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.Runtime.Intrinsics.Vector64<System.UInt32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1020, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.Runtime.Intrinsics.Vector64<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1021, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ShiftRightLogical:System.Runtime.Intrinsics.Vector64<System.UInt64>(System.Runtime.Intrinsics.Vector64<System.UInt64>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "ShiftRightLogical", True, "fact" },
    { 1022, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Shuffle:System.Runtime.Intrinsics.Vector64<System.Byte>(System.Runtime.Intrinsics.Vector64<System.Byte>,System.Runtime.Intrinsics.Vector64<System.Byte>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Shuffle", True, "fact" },
    { 1023, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Shuffle:System.Runtime.Intrinsics.Vector64<System.SByte>(System.Runtime.Intrinsics.Vector64<System.SByte>,System.Runtime.Intrinsics.Vector64<System.SByte>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Shuffle", True, "fact" },
    { 1024, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Shuffle:System.Runtime.Intrinsics.Vector64<System.Int16>(System.Runtime.Intrinsics.Vector64<System.Int16>,System.Runtime.Intrinsics.Vector64<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Shuffle", True, "fact" },
    { 1025, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Shuffle:System.Runtime.Intrinsics.Vector64<System.UInt16>(System.Runtime.Intrinsics.Vector64<System.UInt16>,System.Runtime.Intrinsics.Vector64<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Shuffle", True, "fact" },
    { 1026, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Shuffle:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Shuffle", True, "fact" },
    { 1027, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Shuffle:System.Runtime.Intrinsics.Vector64<System.UInt32>(System.Runtime.Intrinsics.Vector64<System.UInt32>,System.Runtime.Intrinsics.Vector64<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Shuffle", True, "fact" },
    { 1028, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Shuffle:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Shuffle", True, "fact" },
    { 1029, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Sin:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Sin", True, "fact" },
    { 1030, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Sin:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Sin", True, "fact" },
    { 1031, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::SinCos:System.ValueTuple<System.Runtime.Intrinsics.Vector64<System.Double>,System.Runtime.Intrinsics.Vector64<System.Double>>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "SinCos", True, "fact" },
    { 1032, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::SinCos:System.ValueTuple<System.Runtime.Intrinsics.Vector64<System.Single>,System.Runtime.Intrinsics.Vector64<System.Single>>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "SinCos", True, "fact" },
    { 1033, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Sqrt:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Sqrt", True, "fact" },
    { 1034, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Store:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector64", "Store", True, "fact" },
    { 1035, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::StoreAligned:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector64", "StoreAligned", True, "fact" },
    { 1036, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::StoreAlignedNonTemporal:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32*)", "codegen", "System.Runtime.Intrinsics.Vector64", "StoreAlignedNonTemporal", True, "fact" },
    { 1037, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "StoreUnsafe", True, "fact" },
    { 1038, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "StoreUnsafe", True, "fact" },
    { 1039, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "StoreUnsafe", True, "fact" },
    { 1040, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "StoreUnsafe", True, "fact" },
    { 1041, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::StoreUnsafe:System.Void(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32,System.UIntPtr)", "codegen", "System.Runtime.Intrinsics.Vector64", "StoreUnsafe", True, "fact" },
    { 1042, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Subtract:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Subtract", True, "fact" },
    { 1043, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ToScalar:System.Int32(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ToScalar", True, "fact" },
    { 1044, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ToVector128:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ToVector128", True, "fact" },
    { 1045, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::ToVector128Unsafe:System.Runtime.Intrinsics.Vector128<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "ToVector128Unsafe", True, "fact" },
    { 1046, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Truncate:System.Runtime.Intrinsics.Vector64<System.Double>(System.Runtime.Intrinsics.Vector64<System.Double>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Truncate", True, "fact" },
    { 1047, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Truncate:System.Runtime.Intrinsics.Vector64<System.Single>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Truncate", True, "fact" },
    { 1048, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector64<System.UInt16>,System.Runtime.Intrinsics.Vector64<System.UInt16>>(System.Runtime.Intrinsics.Vector64<System.Byte>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Widen", True, "fact" },
    { 1049, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>>(System.Runtime.Intrinsics.Vector64<System.Int16>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Widen", True, "fact" },
    { 1050, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector64<System.Int64>,System.Runtime.Intrinsics.Vector64<System.Int64>>(System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Widen", True, "fact" },
    { 1051, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector64<System.Int16>,System.Runtime.Intrinsics.Vector64<System.Int16>>(System.Runtime.Intrinsics.Vector64<System.SByte>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Widen", True, "fact" },
    { 1052, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector64<System.Double>,System.Runtime.Intrinsics.Vector64<System.Double>>(System.Runtime.Intrinsics.Vector64<System.Single>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Widen", True, "fact" },
    { 1053, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector64<System.UInt32>,System.Runtime.Intrinsics.Vector64<System.UInt32>>(System.Runtime.Intrinsics.Vector64<System.UInt16>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Widen", True, "fact" },
    { 1054, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Widen:System.ValueTuple<System.Runtime.Intrinsics.Vector64<System.UInt64>,System.Runtime.Intrinsics.Vector64<System.UInt64>>(System.Runtime.Intrinsics.Vector64<System.UInt32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Widen", True, "fact" },
    { 1055, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::WithElement:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "WithElement", True, "fact" },
    { 1056, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::WithElement:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "WithElement", True, "fact" },
    { 1057, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::WithElement:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Int32,System.Int32)", "codegen", "System.Runtime.Intrinsics.Vector64", "WithElement", True, "fact" },
    { 1058, "System.Private.CoreLib/System.Runtime.Intrinsics.Vector64::Xor:System.Runtime.Intrinsics.Vector64<System.Int32>(System.Runtime.Intrinsics.Vector64<System.Int32>,System.Runtime.Intrinsics.Vector64<System.Int32>)", "codegen", "System.Runtime.Intrinsics.Vector64", "Xor", True, "fact" },

};

static const int kSubjectEntryCount_Generated = 1059;

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


// ── --profile: per-method GC/allocation/code-size profile ───────────
static int RunProfileMode() {
    const int kCount = kSubjectEntryCount;
    chaos::il2cpp::runtime_core::ProfileStoreInit(kCount);
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
{% if is_jit %}
        try {
            int64_t heap_before = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_before = heap_before;
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            int64_t heap_after = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_after = heap_after;
            chaos::il2cpp::runtime_core::FlushThreadProfileData(i);
        } catch(...) { }
{% else %}
#if defined(_WIN32)
        __try {
            int64_t heap_before = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_before = heap_before;
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            int64_t heap_after = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_after = heap_after;
            chaos::il2cpp::runtime_core::FlushThreadProfileData(i);
        } __except(EXCEPTION_EXECUTE_HANDLER) {
            // SEH caught — continue profiling
        }
#else
        CHAOS_EH_TRY
            int64_t heap_before = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_before = heap_before;
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            int64_t heap_after = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
            chaos::il2cpp::runtime_core::GetThreadProfileData().heap_after = heap_after;
            chaos::il2cpp::runtime_core::FlushThreadProfileData(i);
        CHAOS_EH_CATCH_BEGIN
        CHAOS_EH_END
#endif
{% endif %}
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