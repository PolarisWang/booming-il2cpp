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
    { 0, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "benchmark" },
    { 1, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "benchmark" },
    { 2, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "benchmark" },
    { 3, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "benchmark" },
    { 4, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "benchmark" },
    { 5, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "benchmark" },
    { 6, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "benchmark" },
    { 7, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "benchmark" },
    { 8, "System.Linq/System.Linq.Enumerable::Any:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Any", True, "benchmark" },
    { 9, "System.Linq/System.Linq.Enumerable::Any:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Any", True, "benchmark" },
    { 10, "System.Linq/System.Linq.Enumerable::Any:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Any", True, "benchmark" },
    { 11, "System.Linq/System.Linq.Enumerable::Any:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Any", True, "benchmark" },
    { 12, "System.Linq/System.Linq.Enumerable::All:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "All", True, "benchmark" },
    { 13, "System.Linq/System.Linq.Enumerable::All:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "All", True, "benchmark" },
    { 14, "System.Linq/System.Linq.Enumerable::Append:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Append", True, "benchmark" },
    { 15, "System.Linq/System.Linq.Enumerable::Append:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Append", True, "benchmark" },
    { 16, "System.Linq/System.Linq.Enumerable::Append:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Append", True, "benchmark" },
    { 17, "System.Linq/System.Linq.Enumerable::Prepend:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Prepend", True, "benchmark" },
    { 18, "System.Linq/System.Linq.Enumerable::Prepend:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Prepend", True, "benchmark" },
    { 19, "System.Linq/System.Linq.Enumerable::Prepend:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Prepend", True, "benchmark" },
    { 20, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 21, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 22, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int64>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 23, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int64>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 24, "System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable<System.Single>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 25, "System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable<System.Single>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 26, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Double>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 27, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Double>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 28, "System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable<System.Decimal>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 29, "System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable<System.Decimal>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 30, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 31, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 32, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int64>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 33, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int64>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 34, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Nullable<System.Single>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 35, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Nullable<System.Single>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 36, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Double>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 37, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Double>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 38, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Nullable<System.Decimal>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 39, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Nullable<System.Decimal>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 40, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 41, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 42, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int64>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 43, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int64>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 44, "System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Single>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 45, "System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Single>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 46, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Double>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 47, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Double>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 48, "System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Decimal>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 49, "System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Decimal>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 50, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 51, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 52, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int64>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 53, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int64>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 54, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Single>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 55, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Single>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 56, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Double>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 57, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Double>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 58, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Decimal>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 59, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Decimal>>)", "codegen", "System.Linq.Enumerable", "Average", True, "benchmark" },
    { 60, "System.Linq/System.Linq.Enumerable::Cast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.IEnumerable)", "codegen", "System.Linq.Enumerable", "Cast", True, "benchmark" },
    { 61, "System.Linq/System.Linq.Enumerable::Cast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.IEnumerable)", "codegen", "System.Linq.Enumerable", "Cast", True, "benchmark" },
    { 62, "System.Linq/System.Linq.Enumerable::Chunk:System.Collections.Generic.IEnumerable<System.Int32[]>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Chunk", True, "benchmark" },
    { 63, "System.Linq/System.Linq.Enumerable::Chunk:System.Collections.Generic.IEnumerable<System.Int32[]>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Chunk", True, "benchmark" },
    { 64, "System.Linq/System.Linq.Enumerable::Chunk:System.Collections.Generic.IEnumerable<System.Int32[]>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Chunk", True, "benchmark" },
    { 65, "System.Linq/System.Linq.Enumerable::Concat:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Concat", True, "benchmark" },
    { 66, "System.Linq/System.Linq.Enumerable::Concat:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Concat", True, "benchmark" },
    { 67, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Contains", True, "benchmark" },
    { 68, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Contains", True, "benchmark" },
    { 69, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Contains", True, "benchmark" },
    { 70, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Contains", True, "benchmark" },
    { 71, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Contains", True, "benchmark" },
    { 72, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Contains", True, "benchmark" },
    { 73, "System.Linq/System.Linq.Enumerable::AggregateBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "AggregateBy", True, "benchmark" },
    { 74, "System.Linq/System.Linq.Enumerable::AggregateBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "AggregateBy", True, "benchmark" },
    { 75, "System.Linq/System.Linq.Enumerable::AggregateBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "AggregateBy", True, "benchmark" },
    { 76, "System.Linq/System.Linq.Enumerable::AggregateBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "AggregateBy", True, "benchmark" },
    { 77, "System.Linq/System.Linq.Enumerable::AggregateBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "AggregateBy", True, "benchmark" },
    { 78, "System.Linq/System.Linq.Enumerable::CountBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "CountBy", True, "benchmark" },
    { 79, "System.Linq/System.Linq.Enumerable::CountBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "CountBy", True, "benchmark" },
    { 80, "System.Linq/System.Linq.Enumerable::Count:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Count", True, "benchmark" },
    { 81, "System.Linq/System.Linq.Enumerable::Count:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Count", True, "benchmark" },
    { 82, "System.Linq/System.Linq.Enumerable::Count:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Count", True, "benchmark" },
    { 83, "System.Linq/System.Linq.Enumerable::Count:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Count", True, "benchmark" },
    { 84, "System.Linq/System.Linq.Enumerable::TryGetNonEnumeratedCount:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TryGetNonEnumeratedCount", True, "benchmark" },
    { 85, "System.Linq/System.Linq.Enumerable::TryGetNonEnumeratedCount:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TryGetNonEnumeratedCount", True, "benchmark" },
    { 86, "System.Linq/System.Linq.Enumerable::TryGetNonEnumeratedCount:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TryGetNonEnumeratedCount", True, "benchmark" },
    { 87, "System.Linq/System.Linq.Enumerable::LongCount:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "LongCount", True, "benchmark" },
    { 88, "System.Linq/System.Linq.Enumerable::LongCount:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "LongCount", True, "benchmark" },
    { 89, "System.Linq/System.Linq.Enumerable::LongCount:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "LongCount", True, "benchmark" },
    { 90, "System.Linq/System.Linq.Enumerable::LongCount:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "LongCount", True, "benchmark" },
    { 91, "System.Linq/System.Linq.Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "DefaultIfEmpty", True, "benchmark" },
    { 92, "System.Linq/System.Linq.Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "DefaultIfEmpty", True, "benchmark" },
    { 93, "System.Linq/System.Linq.Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "DefaultIfEmpty", True, "benchmark" },
    { 94, "System.Linq/System.Linq.Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "DefaultIfEmpty", True, "benchmark" },
    { 95, "System.Linq/System.Linq.Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "DefaultIfEmpty", True, "benchmark" },
    { 96, "System.Linq/System.Linq.Enumerable::Distinct:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Distinct", True, "benchmark" },
    { 97, "System.Linq/System.Linq.Enumerable::Distinct:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Distinct", True, "benchmark" },
    { 98, "System.Linq/System.Linq.Enumerable::Distinct:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Distinct", True, "benchmark" },
    { 99, "System.Linq/System.Linq.Enumerable::Distinct:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Distinct", True, "benchmark" },
    { 100, "System.Linq/System.Linq.Enumerable::DistinctBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "DistinctBy", True, "benchmark" },
    { 101, "System.Linq/System.Linq.Enumerable::DistinctBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "DistinctBy", True, "benchmark" },
    { 102, "System.Linq/System.Linq.Enumerable::DistinctBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "DistinctBy", True, "benchmark" },
    { 103, "System.Linq/System.Linq.Enumerable::DistinctBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "DistinctBy", True, "benchmark" },
    { 104, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "benchmark" },
    { 105, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "benchmark" },
    { 106, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "benchmark" },
    { 107, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "benchmark" },
    { 108, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "benchmark" },
    { 109, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "benchmark" },
    { 110, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "benchmark" },
    { 111, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "benchmark" },
    { 112, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "benchmark" },
    { 113, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "benchmark" },
    { 114, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "benchmark" },
    { 115, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "benchmark" },
    { 116, "System.Linq/System.Linq.Enumerable::AsEnumerable:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "AsEnumerable", True, "benchmark" },
    { 117, "System.Linq/System.Linq.Enumerable::AsEnumerable:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "AsEnumerable", True, "benchmark" },
    { 118, "System.Linq/System.Linq.Enumerable::Empty:System.Collections.Generic.IEnumerable<System.Int32>()", "codegen", "System.Linq.Enumerable", "Empty", True, "benchmark" },
    { 119, "System.Linq/System.Linq.Enumerable::Except:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Except", True, "benchmark" },
    { 120, "System.Linq/System.Linq.Enumerable::Except:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Except", True, "benchmark" },
    { 121, "System.Linq/System.Linq.Enumerable::Except:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Except", True, "benchmark" },
    { 122, "System.Linq/System.Linq.Enumerable::Except:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Except", True, "benchmark" },
    { 123, "System.Linq/System.Linq.Enumerable::ExceptBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ExceptBy", True, "benchmark" },
    { 124, "System.Linq/System.Linq.Enumerable::ExceptBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ExceptBy", True, "benchmark" },
    { 125, "System.Linq/System.Linq.Enumerable::ExceptBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ExceptBy", True, "benchmark" },
    { 126, "System.Linq/System.Linq.Enumerable::ExceptBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ExceptBy", True, "benchmark" },
    { 127, "System.Linq/System.Linq.Enumerable::First:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "First", True, "benchmark" },
    { 128, "System.Linq/System.Linq.Enumerable::First:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "First", True, "benchmark" },
    { 129, "System.Linq/System.Linq.Enumerable::First:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "First", True, "benchmark" },
    { 130, "System.Linq/System.Linq.Enumerable::First:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "First", True, "benchmark" },
    { 131, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "benchmark" },
    { 132, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "benchmark" },
    { 133, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "benchmark" },
    { 134, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "benchmark" },
    { 135, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "benchmark" },
    { 136, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "benchmark" },
    { 137, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "benchmark" },
    { 138, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "benchmark" },
    { 139, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "benchmark" },
    { 140, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "benchmark" },
    { 141, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 142, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 143, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 144, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 145, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 146, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 147, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 148, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 149, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 150, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 151, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 152, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 153, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 154, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 155, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 156, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "benchmark" },
    { 157, "System.Linq/System.Linq.Enumerable::GroupJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupJoin", True, "benchmark" },
    { 158, "System.Linq/System.Linq.Enumerable::GroupJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupJoin", True, "benchmark" },
    { 159, "System.Linq/System.Linq.Enumerable::GroupJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupJoin", True, "benchmark" },
    { 160, "System.Linq/System.Linq.Enumerable::GroupJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupJoin", True, "benchmark" },
    { 161, "System.Linq/System.Linq.Enumerable::Index:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Index", True, "benchmark" },
    { 162, "System.Linq/System.Linq.Enumerable::Index:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Index", True, "benchmark" },
    { 163, "System.Linq/System.Linq.Enumerable::Intersect:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Intersect", True, "benchmark" },
    { 164, "System.Linq/System.Linq.Enumerable::Intersect:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Intersect", True, "benchmark" },
    { 165, "System.Linq/System.Linq.Enumerable::Intersect:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Intersect", True, "benchmark" },
    { 166, "System.Linq/System.Linq.Enumerable::Intersect:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Intersect", True, "benchmark" },
    { 167, "System.Linq/System.Linq.Enumerable::IntersectBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "IntersectBy", True, "benchmark" },
    { 168, "System.Linq/System.Linq.Enumerable::IntersectBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "IntersectBy", True, "benchmark" },
    { 169, "System.Linq/System.Linq.Enumerable::IntersectBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "IntersectBy", True, "benchmark" },
    { 170, "System.Linq/System.Linq.Enumerable::IntersectBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "IntersectBy", True, "benchmark" },
    { 171, "System.Linq/System.Linq.Enumerable::Join:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Join", True, "benchmark" },
    { 172, "System.Linq/System.Linq.Enumerable::Join:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Join", True, "benchmark" },
    { 173, "System.Linq/System.Linq.Enumerable::Join:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Join", True, "benchmark" },
    { 174, "System.Linq/System.Linq.Enumerable::Join:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Join", True, "benchmark" },
    { 175, "System.Linq/System.Linq.Enumerable::Last:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Last", True, "benchmark" },
    { 176, "System.Linq/System.Linq.Enumerable::Last:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Last", True, "benchmark" },
    { 177, "System.Linq/System.Linq.Enumerable::Last:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Last", True, "benchmark" },
    { 178, "System.Linq/System.Linq.Enumerable::Last:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Last", True, "benchmark" },
    { 179, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "benchmark" },
    { 180, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "benchmark" },
    { 181, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "benchmark" },
    { 182, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "benchmark" },
    { 183, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "benchmark" },
    { 184, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "benchmark" },
    { 185, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "benchmark" },
    { 186, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "benchmark" },
    { 187, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "benchmark" },
    { 188, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "benchmark" },
    { 189, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "benchmark" },
    { 190, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "benchmark" },
    { 191, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "benchmark" },
    { 192, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "benchmark" },
    { 193, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "benchmark" },
    { 194, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "benchmark" },
    { 195, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "benchmark" },
    { 196, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "benchmark" },
    { 197, "System.Linq/System.Linq.Enumerable::MaxBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "MaxBy", True, "benchmark" },
    { 198, "System.Linq/System.Linq.Enumerable::MaxBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "MaxBy", True, "benchmark" },
    { 199, "System.Linq/System.Linq.Enumerable::MaxBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "MaxBy", True, "benchmark" },
    { 200, "System.Linq/System.Linq.Enumerable::MaxBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "MaxBy", True, "benchmark" },
    { 201, "System.Linq/System.Linq.Enumerable::MinBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "MinBy", True, "benchmark" },
    { 202, "System.Linq/System.Linq.Enumerable::MinBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "MinBy", True, "benchmark" },
    { 203, "System.Linq/System.Linq.Enumerable::MinBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "MinBy", True, "benchmark" },
    { 204, "System.Linq/System.Linq.Enumerable::MinBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "MinBy", True, "benchmark" },
    { 205, "System.Linq/System.Linq.Enumerable::OfType:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.IEnumerable)", "codegen", "System.Linq.Enumerable", "OfType", True, "benchmark" },
    { 206, "System.Linq/System.Linq.Enumerable::OfType:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.IEnumerable)", "codegen", "System.Linq.Enumerable", "OfType", True, "benchmark" },
    { 207, "System.Linq/System.Linq.Enumerable::Order:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Order", True, "benchmark" },
    { 208, "System.Linq/System.Linq.Enumerable::Order:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Order", True, "benchmark" },
    { 209, "System.Linq/System.Linq.Enumerable::Order:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Order", True, "benchmark" },
    { 210, "System.Linq/System.Linq.Enumerable::Order:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Order", True, "benchmark" },
    { 211, "System.Linq/System.Linq.Enumerable::OrderBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderBy", True, "benchmark" },
    { 212, "System.Linq/System.Linq.Enumerable::OrderBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderBy", True, "benchmark" },
    { 213, "System.Linq/System.Linq.Enumerable::OrderBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderBy", True, "benchmark" },
    { 214, "System.Linq/System.Linq.Enumerable::OrderBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderBy", True, "benchmark" },
    { 215, "System.Linq/System.Linq.Enumerable::OrderDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderDescending", True, "benchmark" },
    { 216, "System.Linq/System.Linq.Enumerable::OrderDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderDescending", True, "benchmark" },
    { 217, "System.Linq/System.Linq.Enumerable::OrderDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderDescending", True, "benchmark" },
    { 218, "System.Linq/System.Linq.Enumerable::OrderDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderDescending", True, "benchmark" },
    { 219, "System.Linq/System.Linq.Enumerable::OrderByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderByDescending", True, "benchmark" },
    { 220, "System.Linq/System.Linq.Enumerable::OrderByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderByDescending", True, "benchmark" },
    { 221, "System.Linq/System.Linq.Enumerable::OrderByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderByDescending", True, "benchmark" },
    { 222, "System.Linq/System.Linq.Enumerable::OrderByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderByDescending", True, "benchmark" },
    { 223, "System.Linq/System.Linq.Enumerable::ThenBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenBy", True, "benchmark" },
    { 224, "System.Linq/System.Linq.Enumerable::ThenBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenBy", True, "benchmark" },
    { 225, "System.Linq/System.Linq.Enumerable::ThenBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenBy", True, "benchmark" },
    { 226, "System.Linq/System.Linq.Enumerable::ThenBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenBy", True, "benchmark" },
    { 227, "System.Linq/System.Linq.Enumerable::ThenByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenByDescending", True, "benchmark" },
    { 228, "System.Linq/System.Linq.Enumerable::ThenByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenByDescending", True, "benchmark" },
    { 229, "System.Linq/System.Linq.Enumerable::ThenByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenByDescending", True, "benchmark" },
    { 230, "System.Linq/System.Linq.Enumerable::ThenByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenByDescending", True, "benchmark" },
    { 231, "System.Linq/System.Linq.Enumerable::Range:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Range", True, "benchmark" },
    { 232, "System.Linq/System.Linq.Enumerable::Range:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Range", True, "benchmark" },
    { 233, "System.Linq/System.Linq.Enumerable::Range:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Range", True, "benchmark" },
    { 234, "System.Linq/System.Linq.Enumerable::Repeat:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Repeat", True, "benchmark" },
    { 235, "System.Linq/System.Linq.Enumerable::Repeat:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Repeat", True, "benchmark" },
    { 236, "System.Linq/System.Linq.Enumerable::Repeat:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Repeat", True, "benchmark" },
    { 237, "System.Linq/System.Linq.Enumerable::Reverse:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Reverse", True, "benchmark" },
    { 238, "System.Linq/System.Linq.Enumerable::Reverse:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Reverse", True, "benchmark" },
    { 239, "System.Linq/System.Linq.Enumerable::Select:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Select", True, "benchmark" },
    { 240, "System.Linq/System.Linq.Enumerable::Select:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Select", True, "benchmark" },
    { 241, "System.Linq/System.Linq.Enumerable::Select:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Select", True, "benchmark" },
    { 242, "System.Linq/System.Linq.Enumerable::Select:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Select", True, "benchmark" },
    { 243, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "benchmark" },
    { 244, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "benchmark" },
    { 245, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "benchmark" },
    { 246, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "benchmark" },
    { 247, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "benchmark" },
    { 248, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "benchmark" },
    { 249, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "benchmark" },
    { 250, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "benchmark" },
    { 251, "System.Linq/System.Linq.Enumerable::SequenceEqual:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "SequenceEqual", True, "benchmark" },
    { 252, "System.Linq/System.Linq.Enumerable::SequenceEqual:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "SequenceEqual", True, "benchmark" },
    { 253, "System.Linq/System.Linq.Enumerable::SequenceEqual:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "SequenceEqual", True, "benchmark" },
    { 254, "System.Linq/System.Linq.Enumerable::SequenceEqual:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "SequenceEqual", True, "benchmark" },
    { 255, "System.Linq/System.Linq.Enumerable::Single:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Single", True, "benchmark" },
    { 256, "System.Linq/System.Linq.Enumerable::Single:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Single", True, "benchmark" },
    { 257, "System.Linq/System.Linq.Enumerable::Single:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Single", True, "benchmark" },
    { 258, "System.Linq/System.Linq.Enumerable::Single:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Single", True, "benchmark" },
    { 259, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "benchmark" },
    { 260, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "benchmark" },
    { 261, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "benchmark" },
    { 262, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "benchmark" },
    { 263, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "benchmark" },
    { 264, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "benchmark" },
    { 265, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "benchmark" },
    { 266, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "benchmark" },
    { 267, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "benchmark" },
    { 268, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "benchmark" },
    { 269, "System.Linq/System.Linq.Enumerable::Skip:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Skip", True, "benchmark" },
    { 270, "System.Linq/System.Linq.Enumerable::Skip:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Skip", True, "benchmark" },
    { 271, "System.Linq/System.Linq.Enumerable::Skip:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Skip", True, "benchmark" },
    { 272, "System.Linq/System.Linq.Enumerable::SkipWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SkipWhile", True, "benchmark" },
    { 273, "System.Linq/System.Linq.Enumerable::SkipWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SkipWhile", True, "benchmark" },
    { 274, "System.Linq/System.Linq.Enumerable::SkipWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SkipWhile", True, "benchmark" },
    { 275, "System.Linq/System.Linq.Enumerable::SkipWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SkipWhile", True, "benchmark" },
    { 276, "System.Linq/System.Linq.Enumerable::SkipLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SkipLast", True, "benchmark" },
    { 277, "System.Linq/System.Linq.Enumerable::SkipLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SkipLast", True, "benchmark" },
    { 278, "System.Linq/System.Linq.Enumerable::SkipLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SkipLast", True, "benchmark" },
    { 279, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Take", True, "benchmark" },
    { 280, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Take", True, "benchmark" },
    { 281, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Take", True, "benchmark" },
    { 282, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Range)", "codegen", "System.Linq.Enumerable", "Take", True, "benchmark" },
    { 283, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Range)", "codegen", "System.Linq.Enumerable", "Take", True, "benchmark" },
    { 284, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Range)", "codegen", "System.Linq.Enumerable", "Take", True, "benchmark" },
    { 285, "System.Linq/System.Linq.Enumerable::TakeWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "TakeWhile", True, "benchmark" },
    { 286, "System.Linq/System.Linq.Enumerable::TakeWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "TakeWhile", True, "benchmark" },
    { 287, "System.Linq/System.Linq.Enumerable::TakeWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "TakeWhile", True, "benchmark" },
    { 288, "System.Linq/System.Linq.Enumerable::TakeWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "TakeWhile", True, "benchmark" },
    { 289, "System.Linq/System.Linq.Enumerable::TakeLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TakeLast", True, "benchmark" },
    { 290, "System.Linq/System.Linq.Enumerable::TakeLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TakeLast", True, "benchmark" },
    { 291, "System.Linq/System.Linq.Enumerable::TakeLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TakeLast", True, "benchmark" },
    { 292, "System.Linq/System.Linq.Enumerable::ToArray:System.Int32[](System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToArray", True, "benchmark" },
    { 293, "System.Linq/System.Linq.Enumerable::ToArray:System.Int32[](System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToArray", True, "benchmark" },
    { 294, "System.Linq/System.Linq.Enumerable::ToList:System.Collections.Generic.List<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToList", True, "benchmark" },
    { 295, "System.Linq/System.Linq.Enumerable::ToList:System.Collections.Generic.List<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToList", True, "benchmark" },
    { 296, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 297, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 298, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 299, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 300, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 301, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 302, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 303, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 304, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 305, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 306, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 307, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 308, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 309, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 310, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 311, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "benchmark" },
    { 312, "System.Linq/System.Linq.Enumerable::ToHashSet:System.Collections.Generic.HashSet<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToHashSet", True, "benchmark" },
    { 313, "System.Linq/System.Linq.Enumerable::ToHashSet:System.Collections.Generic.HashSet<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToHashSet", True, "benchmark" },
    { 314, "System.Linq/System.Linq.Enumerable::ToHashSet:System.Collections.Generic.HashSet<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToHashSet", True, "benchmark" },
    { 315, "System.Linq/System.Linq.Enumerable::ToHashSet:System.Collections.Generic.HashSet<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToHashSet", True, "benchmark" },
    { 316, "System.Linq/System.Linq.Enumerable::Union:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Union", True, "benchmark" },
    { 317, "System.Linq/System.Linq.Enumerable::Union:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Union", True, "benchmark" },
    { 318, "System.Linq/System.Linq.Enumerable::Union:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Union", True, "benchmark" },
    { 319, "System.Linq/System.Linq.Enumerable::Union:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Union", True, "benchmark" },
    { 320, "System.Linq/System.Linq.Enumerable::UnionBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "UnionBy", True, "benchmark" },
    { 321, "System.Linq/System.Linq.Enumerable::UnionBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "UnionBy", True, "benchmark" },
    { 322, "System.Linq/System.Linq.Enumerable::UnionBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "UnionBy", True, "benchmark" },
    { 323, "System.Linq/System.Linq.Enumerable::UnionBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "UnionBy", True, "benchmark" },
    { 324, "System.Linq/System.Linq.Enumerable::Where:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Where", True, "benchmark" },
    { 325, "System.Linq/System.Linq.Enumerable::Where:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Where", True, "benchmark" },
    { 326, "System.Linq/System.Linq.Enumerable::Where:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Where", True, "benchmark" },
    { 327, "System.Linq/System.Linq.Enumerable::Where:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Where", True, "benchmark" },
    { 328, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "benchmark" },
    { 329, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "benchmark" },
    { 330, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "benchmark" },
    { 331, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "benchmark" },
    { 332, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "benchmark" },
    { 333, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "benchmark" },
    { 334, "System.Linq/System.Linq.Lookup<System.Int32,System.Int32>::Contains:System.Boolean(System.Int32)", "codegen", "System.Linq.Lookup<System.Int32,System.Int32>", "Contains", True, "benchmark" },
    { 335, "System.Linq/System.Linq.Lookup<System.Int32,System.Int32>::Contains:System.Boolean(System.Int32)", "codegen", "System.Linq.Lookup<System.Int32,System.Int32>", "Contains", True, "benchmark" },
    { 336, "System.Linq/System.Linq.Lookup<System.Int32,System.Int32>::GetEnumerator:System.Collections.Generic.IEnumerator<System.Linq.IGrouping<System.Int32,System.Int32>>()", "codegen", "System.Linq.Lookup<System.Int32,System.Int32>", "GetEnumerator", True, "benchmark" },
    { 337, "System.Linq/System.Linq.Lookup<System.Int32,System.Int32>::ApplyResultSelector:System.Collections.Generic.IEnumerable<System.Int32>(System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Lookup<System.Int32,System.Int32>", "ApplyResultSelector", True, "benchmark" },
    { 338, "System.Linq/System.Linq.Lookup<System.Int32,System.Int32>::ApplyResultSelector:System.Collections.Generic.IEnumerable<System.Int32>(System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Lookup<System.Int32,System.Int32>", "ApplyResultSelector", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 339;

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