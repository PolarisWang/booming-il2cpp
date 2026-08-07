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
// Deferred enum metadata registration — GC must be initialized first.
extern "C" void ChaosRegisterCombinedSubjectsEnumMetadata();

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
    { 0, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "fact" },
    { 1, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "fact" },
    { 2, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "fact" },
    { 3, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "fact" },
    { 4, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "fact" },
    { 5, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "fact" },
    { 6, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "fact" },
    { 7, "System.Linq/System.Linq.Enumerable::Aggregate:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Aggregate", True, "fact" },
    { 8, "System.Linq/System.Linq.Enumerable::Any:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Any", True, "fact" },
    { 9, "System.Linq/System.Linq.Enumerable::Any:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Any", True, "fact" },
    { 10, "System.Linq/System.Linq.Enumerable::Any:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Any", True, "fact" },
    { 11, "System.Linq/System.Linq.Enumerable::Any:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Any", True, "fact" },
    { 12, "System.Linq/System.Linq.Enumerable::All:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "All", True, "fact" },
    { 13, "System.Linq/System.Linq.Enumerable::All:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "All", True, "fact" },
    { 14, "System.Linq/System.Linq.Enumerable::Append:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Append", True, "fact" },
    { 15, "System.Linq/System.Linq.Enumerable::Append:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Append", True, "fact" },
    { 16, "System.Linq/System.Linq.Enumerable::Append:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Append", True, "fact" },
    { 17, "System.Linq/System.Linq.Enumerable::Prepend:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Prepend", True, "fact" },
    { 18, "System.Linq/System.Linq.Enumerable::Prepend:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Prepend", True, "fact" },
    { 19, "System.Linq/System.Linq.Enumerable::Prepend:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Prepend", True, "fact" },
    { 20, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 21, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 22, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int64>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 23, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int64>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 24, "System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable<System.Single>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 25, "System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable<System.Single>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 26, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Double>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 27, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Double>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 28, "System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable<System.Decimal>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 29, "System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable<System.Decimal>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 30, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 31, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 32, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int64>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 33, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Int64>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 34, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Nullable<System.Single>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 35, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Nullable<System.Single>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 36, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Double>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 37, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Nullable<System.Double>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 38, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Nullable<System.Decimal>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 39, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Nullable<System.Decimal>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 40, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 41, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 42, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int64>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 43, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int64>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 44, "System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Single>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 45, "System.Linq/System.Linq.Enumerable::Average:System.Single(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Single>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 46, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Double>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 47, "System.Linq/System.Linq.Enumerable::Average:System.Double(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Double>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 48, "System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Decimal>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 49, "System.Linq/System.Linq.Enumerable::Average:System.Decimal(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Decimal>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 50, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 51, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 52, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int64>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 53, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Int64>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 54, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Single>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 55, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Single>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Single>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 56, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Double>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 57, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Double>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Double>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 58, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Decimal>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 59, "System.Linq/System.Linq.Enumerable::Average:System.Nullable<System.Decimal>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Nullable<System.Decimal>>)", "codegen", "System.Linq.Enumerable", "Average", True, "fact" },
    { 60, "System.Linq/System.Linq.Enumerable::Cast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.IEnumerable)", "codegen", "System.Linq.Enumerable", "Cast", True, "fact" },
    { 61, "System.Linq/System.Linq.Enumerable::Cast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.IEnumerable)", "codegen", "System.Linq.Enumerable", "Cast", True, "fact" },
    { 62, "System.Linq/System.Linq.Enumerable::Chunk:System.Collections.Generic.IEnumerable<System.Int32[]>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Chunk", True, "fact" },
    { 63, "System.Linq/System.Linq.Enumerable::Chunk:System.Collections.Generic.IEnumerable<System.Int32[]>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Chunk", True, "fact" },
    { 64, "System.Linq/System.Linq.Enumerable::Chunk:System.Collections.Generic.IEnumerable<System.Int32[]>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Chunk", True, "fact" },
    { 65, "System.Linq/System.Linq.Enumerable::Concat:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Concat", True, "fact" },
    { 66, "System.Linq/System.Linq.Enumerable::Concat:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Concat", True, "fact" },
    { 67, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Contains", True, "fact" },
    { 68, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Contains", True, "fact" },
    { 69, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Contains", True, "fact" },
    { 70, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Contains", True, "fact" },
    { 71, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Contains", True, "fact" },
    { 72, "System.Linq/System.Linq.Enumerable::Contains:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Contains", True, "fact" },
    { 73, "System.Linq/System.Linq.Enumerable::AggregateBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "AggregateBy", True, "fact" },
    { 74, "System.Linq/System.Linq.Enumerable::AggregateBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "AggregateBy", True, "fact" },
    { 75, "System.Linq/System.Linq.Enumerable::AggregateBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Int32,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "AggregateBy", True, "fact" },
    { 76, "System.Linq/System.Linq.Enumerable::AggregateBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "AggregateBy", True, "fact" },
    { 77, "System.Linq/System.Linq.Enumerable::AggregateBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "AggregateBy", True, "fact" },
    { 78, "System.Linq/System.Linq.Enumerable::CountBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "CountBy", True, "fact" },
    { 79, "System.Linq/System.Linq.Enumerable::CountBy:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "CountBy", True, "fact" },
    { 80, "System.Linq/System.Linq.Enumerable::Count:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Count", True, "fact" },
    { 81, "System.Linq/System.Linq.Enumerable::Count:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Count", True, "fact" },
    { 82, "System.Linq/System.Linq.Enumerable::Count:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Count", True, "fact" },
    { 83, "System.Linq/System.Linq.Enumerable::Count:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Count", True, "fact" },
    { 84, "System.Linq/System.Linq.Enumerable::TryGetNonEnumeratedCount:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TryGetNonEnumeratedCount", True, "fact" },
    { 85, "System.Linq/System.Linq.Enumerable::TryGetNonEnumeratedCount:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TryGetNonEnumeratedCount", True, "fact" },
    { 86, "System.Linq/System.Linq.Enumerable::TryGetNonEnumeratedCount:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TryGetNonEnumeratedCount", True, "fact" },
    { 87, "System.Linq/System.Linq.Enumerable::LongCount:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "LongCount", True, "fact" },
    { 88, "System.Linq/System.Linq.Enumerable::LongCount:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "LongCount", True, "fact" },
    { 89, "System.Linq/System.Linq.Enumerable::LongCount:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "LongCount", True, "fact" },
    { 90, "System.Linq/System.Linq.Enumerable::LongCount:System.Int64(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "LongCount", True, "fact" },
    { 91, "System.Linq/System.Linq.Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "DefaultIfEmpty", True, "fact" },
    { 92, "System.Linq/System.Linq.Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "DefaultIfEmpty", True, "fact" },
    { 93, "System.Linq/System.Linq.Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "DefaultIfEmpty", True, "fact" },
    { 94, "System.Linq/System.Linq.Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "DefaultIfEmpty", True, "fact" },
    { 95, "System.Linq/System.Linq.Enumerable::DefaultIfEmpty:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "DefaultIfEmpty", True, "fact" },
    { 96, "System.Linq/System.Linq.Enumerable::Distinct:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Distinct", True, "fact" },
    { 97, "System.Linq/System.Linq.Enumerable::Distinct:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Distinct", True, "fact" },
    { 98, "System.Linq/System.Linq.Enumerable::Distinct:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Distinct", True, "fact" },
    { 99, "System.Linq/System.Linq.Enumerable::Distinct:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Distinct", True, "fact" },
    { 100, "System.Linq/System.Linq.Enumerable::DistinctBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "DistinctBy", True, "fact" },
    { 101, "System.Linq/System.Linq.Enumerable::DistinctBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "DistinctBy", True, "fact" },
    { 102, "System.Linq/System.Linq.Enumerable::DistinctBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "DistinctBy", True, "fact" },
    { 103, "System.Linq/System.Linq.Enumerable::DistinctBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "DistinctBy", True, "fact" },
    { 104, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "fact" },
    { 105, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "fact" },
    { 106, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "fact" },
    { 107, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "fact" },
    { 108, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "fact" },
    { 109, "System.Linq/System.Linq.Enumerable::ElementAt:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAt", True, "fact" },
    { 110, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "fact" },
    { 111, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "fact" },
    { 112, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "fact" },
    { 113, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "fact" },
    { 114, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "fact" },
    { 115, "System.Linq/System.Linq.Enumerable::ElementAtOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Index)", "codegen", "System.Linq.Enumerable", "ElementAtOrDefault", True, "fact" },
    { 116, "System.Linq/System.Linq.Enumerable::AsEnumerable:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "AsEnumerable", True, "fact" },
    { 117, "System.Linq/System.Linq.Enumerable::AsEnumerable:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "AsEnumerable", True, "fact" },
    { 118, "System.Linq/System.Linq.Enumerable::Empty:System.Collections.Generic.IEnumerable<System.Int32>()", "codegen", "System.Linq.Enumerable", "Empty", True, "fact" },
    { 119, "System.Linq/System.Linq.Enumerable::Except:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Except", True, "fact" },
    { 120, "System.Linq/System.Linq.Enumerable::Except:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Except", True, "fact" },
    { 121, "System.Linq/System.Linq.Enumerable::Except:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Except", True, "fact" },
    { 122, "System.Linq/System.Linq.Enumerable::Except:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Except", True, "fact" },
    { 123, "System.Linq/System.Linq.Enumerable::ExceptBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ExceptBy", True, "fact" },
    { 124, "System.Linq/System.Linq.Enumerable::ExceptBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ExceptBy", True, "fact" },
    { 125, "System.Linq/System.Linq.Enumerable::ExceptBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ExceptBy", True, "fact" },
    { 126, "System.Linq/System.Linq.Enumerable::ExceptBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ExceptBy", True, "fact" },
    { 127, "System.Linq/System.Linq.Enumerable::First:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "First", True, "fact" },
    { 128, "System.Linq/System.Linq.Enumerable::First:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "First", True, "fact" },
    { 129, "System.Linq/System.Linq.Enumerable::First:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "First", True, "fact" },
    { 130, "System.Linq/System.Linq.Enumerable::First:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "First", True, "fact" },
    { 131, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "fact" },
    { 132, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "fact" },
    { 133, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "fact" },
    { 134, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "fact" },
    { 135, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "fact" },
    { 136, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "fact" },
    { 137, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "fact" },
    { 138, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "fact" },
    { 139, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "fact" },
    { 140, "System.Linq/System.Linq.Enumerable::FirstOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "FirstOrDefault", True, "fact" },
    { 141, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 142, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 143, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 144, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 145, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 146, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 147, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 148, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Linq.IGrouping<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 149, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 150, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 151, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 152, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 153, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 154, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 155, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 156, "System.Linq/System.Linq.Enumerable::GroupBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupBy", True, "fact" },
    { 157, "System.Linq/System.Linq.Enumerable::GroupJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupJoin", True, "fact" },
    { 158, "System.Linq/System.Linq.Enumerable::GroupJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupJoin", True, "fact" },
    { 159, "System.Linq/System.Linq.Enumerable::GroupJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupJoin", True, "fact" },
    { 160, "System.Linq/System.Linq.Enumerable::GroupJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "GroupJoin", True, "fact" },
    { 161, "System.Linq/System.Linq.Enumerable::Index:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Index", True, "fact" },
    { 162, "System.Linq/System.Linq.Enumerable::Index:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Index", True, "fact" },
    { 163, "System.Linq/System.Linq.Enumerable::Intersect:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Intersect", True, "fact" },
    { 164, "System.Linq/System.Linq.Enumerable::Intersect:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Intersect", True, "fact" },
    { 165, "System.Linq/System.Linq.Enumerable::Intersect:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Intersect", True, "fact" },
    { 166, "System.Linq/System.Linq.Enumerable::Intersect:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Intersect", True, "fact" },
    { 167, "System.Linq/System.Linq.Enumerable::IntersectBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "IntersectBy", True, "fact" },
    { 168, "System.Linq/System.Linq.Enumerable::IntersectBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "IntersectBy", True, "fact" },
    { 169, "System.Linq/System.Linq.Enumerable::IntersectBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "IntersectBy", True, "fact" },
    { 170, "System.Linq/System.Linq.Enumerable::IntersectBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "IntersectBy", True, "fact" },
    { 171, "System.Linq/System.Linq.Enumerable::Join:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Join", True, "fact" },
    { 172, "System.Linq/System.Linq.Enumerable::Join:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Join", True, "fact" },
    { 173, "System.Linq/System.Linq.Enumerable::Join:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Join", True, "fact" },
    { 174, "System.Linq/System.Linq.Enumerable::Join:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Join", True, "fact" },
    { 175, "System.Linq/System.Linq.Enumerable::Last:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Last", True, "fact" },
    { 176, "System.Linq/System.Linq.Enumerable::Last:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Last", True, "fact" },
    { 177, "System.Linq/System.Linq.Enumerable::Last:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Last", True, "fact" },
    { 178, "System.Linq/System.Linq.Enumerable::Last:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Last", True, "fact" },
    { 179, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "fact" },
    { 180, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "fact" },
    { 181, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "fact" },
    { 182, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "fact" },
    { 183, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "fact" },
    { 184, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "fact" },
    { 185, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "fact" },
    { 186, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "fact" },
    { 187, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "fact" },
    { 188, "System.Linq/System.Linq.Enumerable::LastOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "LastOrDefault", True, "fact" },
    { 189, "System.Linq/System.Linq.Enumerable::LeftJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "LeftJoin", True, "fact" },
    { 190, "System.Linq/System.Linq.Enumerable::LeftJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "LeftJoin", True, "fact" },
    { 191, "System.Linq/System.Linq.Enumerable::LeftJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "LeftJoin", True, "fact" },
    { 192, "System.Linq/System.Linq.Enumerable::LeftJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "LeftJoin", True, "fact" },
    { 193, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "fact" },
    { 194, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "fact" },
    { 195, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "fact" },
    { 196, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "fact" },
    { 197, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "fact" },
    { 198, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "fact" },
    { 199, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "fact" },
    { 200, "System.Linq/System.Linq.Enumerable::ToLookup:System.Linq.ILookup<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToLookup", True, "fact" },
    { 201, "System.Linq/System.Linq.Enumerable::MaxBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "MaxBy", True, "fact" },
    { 202, "System.Linq/System.Linq.Enumerable::MaxBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "MaxBy", True, "fact" },
    { 203, "System.Linq/System.Linq.Enumerable::MaxBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "MaxBy", True, "fact" },
    { 204, "System.Linq/System.Linq.Enumerable::MaxBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "MaxBy", True, "fact" },
    { 205, "System.Linq/System.Linq.Enumerable::MinBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "MinBy", True, "fact" },
    { 206, "System.Linq/System.Linq.Enumerable::MinBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "MinBy", True, "fact" },
    { 207, "System.Linq/System.Linq.Enumerable::MinBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "MinBy", True, "fact" },
    { 208, "System.Linq/System.Linq.Enumerable::MinBy:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "MinBy", True, "fact" },
    { 209, "System.Linq/System.Linq.Enumerable::OfType:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.IEnumerable)", "codegen", "System.Linq.Enumerable", "OfType", True, "fact" },
    { 210, "System.Linq/System.Linq.Enumerable::OfType:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.IEnumerable)", "codegen", "System.Linq.Enumerable", "OfType", True, "fact" },
    { 211, "System.Linq/System.Linq.Enumerable::Order:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Order", True, "fact" },
    { 212, "System.Linq/System.Linq.Enumerable::Order:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Order", True, "fact" },
    { 213, "System.Linq/System.Linq.Enumerable::Order:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Order", True, "fact" },
    { 214, "System.Linq/System.Linq.Enumerable::Order:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Order", True, "fact" },
    { 215, "System.Linq/System.Linq.Enumerable::OrderBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderBy", True, "fact" },
    { 216, "System.Linq/System.Linq.Enumerable::OrderBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderBy", True, "fact" },
    { 217, "System.Linq/System.Linq.Enumerable::OrderBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderBy", True, "fact" },
    { 218, "System.Linq/System.Linq.Enumerable::OrderBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderBy", True, "fact" },
    { 219, "System.Linq/System.Linq.Enumerable::OrderDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderDescending", True, "fact" },
    { 220, "System.Linq/System.Linq.Enumerable::OrderDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderDescending", True, "fact" },
    { 221, "System.Linq/System.Linq.Enumerable::OrderDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderDescending", True, "fact" },
    { 222, "System.Linq/System.Linq.Enumerable::OrderDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderDescending", True, "fact" },
    { 223, "System.Linq/System.Linq.Enumerable::OrderByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderByDescending", True, "fact" },
    { 224, "System.Linq/System.Linq.Enumerable::OrderByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderByDescending", True, "fact" },
    { 225, "System.Linq/System.Linq.Enumerable::OrderByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderByDescending", True, "fact" },
    { 226, "System.Linq/System.Linq.Enumerable::OrderByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "OrderByDescending", True, "fact" },
    { 227, "System.Linq/System.Linq.Enumerable::ThenBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenBy", True, "fact" },
    { 228, "System.Linq/System.Linq.Enumerable::ThenBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenBy", True, "fact" },
    { 229, "System.Linq/System.Linq.Enumerable::ThenBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenBy", True, "fact" },
    { 230, "System.Linq/System.Linq.Enumerable::ThenBy:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenBy", True, "fact" },
    { 231, "System.Linq/System.Linq.Enumerable::ThenByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenByDescending", True, "fact" },
    { 232, "System.Linq/System.Linq.Enumerable::ThenByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenByDescending", True, "fact" },
    { 233, "System.Linq/System.Linq.Enumerable::ThenByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenByDescending", True, "fact" },
    { 234, "System.Linq/System.Linq.Enumerable::ThenByDescending:System.Linq.IOrderedEnumerable<System.Int32>(System.Linq.IOrderedEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ThenByDescending", True, "fact" },
    { 235, "System.Linq/System.Linq.Enumerable::Range:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Range", True, "fact" },
    { 236, "System.Linq/System.Linq.Enumerable::Range:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Range", True, "fact" },
    { 237, "System.Linq/System.Linq.Enumerable::Range:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Range", True, "fact" },
    { 238, "System.Linq/System.Linq.Enumerable::Repeat:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Repeat", True, "fact" },
    { 239, "System.Linq/System.Linq.Enumerable::Repeat:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Repeat", True, "fact" },
    { 240, "System.Linq/System.Linq.Enumerable::Repeat:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Repeat", True, "fact" },
    { 241, "System.Linq/System.Linq.Enumerable::Shuffle:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Shuffle", True, "fact" },
    { 242, "System.Linq/System.Linq.Enumerable::Shuffle:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Shuffle", True, "fact" },
    { 243, "System.Linq/System.Linq.Enumerable::Reverse:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Reverse", True, "fact" },
    { 244, "System.Linq/System.Linq.Enumerable::Reverse:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Reverse", True, "fact" },
    { 245, "System.Linq/System.Linq.Enumerable::Reverse:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32[])", "codegen", "System.Linq.Enumerable", "Reverse", True, "fact" },
    { 246, "System.Linq/System.Linq.Enumerable::Reverse:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32[])", "codegen", "System.Linq.Enumerable", "Reverse", True, "fact" },
    { 247, "System.Linq/System.Linq.Enumerable::Reverse:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32[])", "codegen", "System.Linq.Enumerable", "Reverse", True, "fact" },
    { 248, "System.Linq/System.Linq.Enumerable::RightJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "RightJoin", True, "fact" },
    { 249, "System.Linq/System.Linq.Enumerable::RightJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "RightJoin", True, "fact" },
    { 250, "System.Linq/System.Linq.Enumerable::RightJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "RightJoin", True, "fact" },
    { 251, "System.Linq/System.Linq.Enumerable::RightJoin:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "RightJoin", True, "fact" },
    { 252, "System.Linq/System.Linq.Enumerable::Select:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Select", True, "fact" },
    { 253, "System.Linq/System.Linq.Enumerable::Select:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Select", True, "fact" },
    { 254, "System.Linq/System.Linq.Enumerable::Select:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Select", True, "fact" },
    { 255, "System.Linq/System.Linq.Enumerable::Select:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Select", True, "fact" },
    { 256, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "fact" },
    { 257, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "fact" },
    { 258, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "fact" },
    { 259, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "fact" },
    { 260, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "fact" },
    { 261, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "fact" },
    { 262, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "fact" },
    { 263, "System.Linq/System.Linq.Enumerable::SelectMany:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "SelectMany", True, "fact" },
    { 264, "System.Linq/System.Linq.Enumerable::Sequence:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Sequence", True, "fact" },
    { 265, "System.Linq/System.Linq.Enumerable::Sequence:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Sequence", True, "fact" },
    { 266, "System.Linq/System.Linq.Enumerable::Sequence:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Sequence", True, "fact" },
    { 267, "System.Linq/System.Linq.Enumerable::Sequence:System.Collections.Generic.IEnumerable<System.Int32>(System.Int32,System.Int32,System.Int32)", "codegen", "System.Linq.Enumerable", "Sequence", True, "fact" },
    { 268, "System.Linq/System.Linq.Enumerable::SequenceEqual:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "SequenceEqual", True, "fact" },
    { 269, "System.Linq/System.Linq.Enumerable::SequenceEqual:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "SequenceEqual", True, "fact" },
    { 270, "System.Linq/System.Linq.Enumerable::SequenceEqual:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "SequenceEqual", True, "fact" },
    { 271, "System.Linq/System.Linq.Enumerable::SequenceEqual:System.Boolean(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "SequenceEqual", True, "fact" },
    { 272, "System.Linq/System.Linq.Enumerable::Single:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Single", True, "fact" },
    { 273, "System.Linq/System.Linq.Enumerable::Single:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Single", True, "fact" },
    { 274, "System.Linq/System.Linq.Enumerable::Single:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Single", True, "fact" },
    { 275, "System.Linq/System.Linq.Enumerable::Single:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Single", True, "fact" },
    { 276, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "fact" },
    { 277, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "fact" },
    { 278, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "fact" },
    { 279, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "fact" },
    { 280, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "fact" },
    { 281, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "fact" },
    { 282, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "fact" },
    { 283, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "fact" },
    { 284, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "fact" },
    { 285, "System.Linq/System.Linq.Enumerable::SingleOrDefault:System.Int32(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>,System.Int32)", "codegen", "System.Linq.Enumerable", "SingleOrDefault", True, "fact" },
    { 286, "System.Linq/System.Linq.Enumerable::Skip:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Skip", True, "fact" },
    { 287, "System.Linq/System.Linq.Enumerable::Skip:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Skip", True, "fact" },
    { 288, "System.Linq/System.Linq.Enumerable::Skip:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Skip", True, "fact" },
    { 289, "System.Linq/System.Linq.Enumerable::SkipWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SkipWhile", True, "fact" },
    { 290, "System.Linq/System.Linq.Enumerable::SkipWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SkipWhile", True, "fact" },
    { 291, "System.Linq/System.Linq.Enumerable::SkipWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SkipWhile", True, "fact" },
    { 292, "System.Linq/System.Linq.Enumerable::SkipWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "SkipWhile", True, "fact" },
    { 293, "System.Linq/System.Linq.Enumerable::SkipLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SkipLast", True, "fact" },
    { 294, "System.Linq/System.Linq.Enumerable::SkipLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SkipLast", True, "fact" },
    { 295, "System.Linq/System.Linq.Enumerable::SkipLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "SkipLast", True, "fact" },
    { 296, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Take", True, "fact" },
    { 297, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Take", True, "fact" },
    { 298, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "Take", True, "fact" },
    { 299, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Range)", "codegen", "System.Linq.Enumerable", "Take", True, "fact" },
    { 300, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Range)", "codegen", "System.Linq.Enumerable", "Take", True, "fact" },
    { 301, "System.Linq/System.Linq.Enumerable::Take:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Range)", "codegen", "System.Linq.Enumerable", "Take", True, "fact" },
    { 302, "System.Linq/System.Linq.Enumerable::TakeWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "TakeWhile", True, "fact" },
    { 303, "System.Linq/System.Linq.Enumerable::TakeWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "TakeWhile", True, "fact" },
    { 304, "System.Linq/System.Linq.Enumerable::TakeWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "TakeWhile", True, "fact" },
    { 305, "System.Linq/System.Linq.Enumerable::TakeWhile:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "TakeWhile", True, "fact" },
    { 306, "System.Linq/System.Linq.Enumerable::TakeLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TakeLast", True, "fact" },
    { 307, "System.Linq/System.Linq.Enumerable::TakeLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TakeLast", True, "fact" },
    { 308, "System.Linq/System.Linq.Enumerable::TakeLast:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Int32)", "codegen", "System.Linq.Enumerable", "TakeLast", True, "fact" },
    { 309, "System.Linq/System.Linq.Enumerable::ToArray:System.Int32[](System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToArray", True, "fact" },
    { 310, "System.Linq/System.Linq.Enumerable::ToArray:System.Int32[](System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToArray", True, "fact" },
    { 311, "System.Linq/System.Linq.Enumerable::ToList:System.Collections.Generic.List<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToList", True, "fact" },
    { 312, "System.Linq/System.Linq.Enumerable::ToList:System.Collections.Generic.List<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToList", True, "fact" },
    { 313, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 314, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 315, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 316, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.Int32,System.Int32>>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 317, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 318, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 319, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 320, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 321, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 322, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 323, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 324, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 325, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 326, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 327, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 328, "System.Linq/System.Linq.Enumerable::ToDictionary:System.Collections.Generic.Dictionary<System.Int32,System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToDictionary", True, "fact" },
    { 329, "System.Linq/System.Linq.Enumerable::ToHashSet:System.Collections.Generic.HashSet<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToHashSet", True, "fact" },
    { 330, "System.Linq/System.Linq.Enumerable::ToHashSet:System.Collections.Generic.HashSet<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToHashSet", True, "fact" },
    { 331, "System.Linq/System.Linq.Enumerable::ToHashSet:System.Collections.Generic.HashSet<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToHashSet", True, "fact" },
    { 332, "System.Linq/System.Linq.Enumerable::ToHashSet:System.Collections.Generic.HashSet<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "ToHashSet", True, "fact" },
    { 333, "System.Linq/System.Linq.Enumerable::Union:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Union", True, "fact" },
    { 334, "System.Linq/System.Linq.Enumerable::Union:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Union", True, "fact" },
    { 335, "System.Linq/System.Linq.Enumerable::Union:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Union", True, "fact" },
    { 336, "System.Linq/System.Linq.Enumerable::Union:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "Union", True, "fact" },
    { 337, "System.Linq/System.Linq.Enumerable::UnionBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "UnionBy", True, "fact" },
    { 338, "System.Linq/System.Linq.Enumerable::UnionBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "UnionBy", True, "fact" },
    { 339, "System.Linq/System.Linq.Enumerable::UnionBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "UnionBy", True, "fact" },
    { 340, "System.Linq/System.Linq.Enumerable::UnionBy:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IEqualityComparer<System.Int32>)", "codegen", "System.Linq.Enumerable", "UnionBy", True, "fact" },
    { 341, "System.Linq/System.Linq.Enumerable::Where:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Where", True, "fact" },
    { 342, "System.Linq/System.Linq.Enumerable::Where:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Where", True, "fact" },
    { 343, "System.Linq/System.Linq.Enumerable::Where:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Where", True, "fact" },
    { 344, "System.Linq/System.Linq.Enumerable::Where:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Boolean>)", "codegen", "System.Linq.Enumerable", "Where", True, "fact" },
    { 345, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "fact" },
    { 346, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.Int32>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Func<System.Int32,System.Int32,System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "fact" },
    { 347, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "fact" },
    { 348, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "fact" },
    { 349, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "fact" },
    { 350, "System.Linq/System.Linq.Enumerable::Zip:System.Collections.Generic.IEnumerable<System.ValueTuple<System.Int32,System.Int32,System.Int32>>(System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>,System.Collections.Generic.IEnumerable<System.Int32>)", "codegen", "System.Linq.Enumerable", "Zip", True, "fact" },
    { 351, "System.Linq/System.Linq.Lookup<System.Int32,System.Int32>::Contains:System.Boolean(System.Int32)", "codegen", "System.Linq.Lookup<System.Int32,System.Int32>", "Contains", True, "fact" },
    { 352, "System.Linq/System.Linq.Lookup<System.Int32,System.Int32>::Contains:System.Boolean(System.Int32)", "codegen", "System.Linq.Lookup<System.Int32,System.Int32>", "Contains", True, "fact" },
    { 353, "System.Linq/System.Linq.Lookup<System.Int32,System.Int32>::GetEnumerator:System.Collections.Generic.IEnumerator<System.Linq.IGrouping<System.Int32,System.Int32>>()", "codegen", "System.Linq.Lookup<System.Int32,System.Int32>", "GetEnumerator", True, "fact" },
    { 354, "System.Linq/System.Linq.Lookup<System.Int32,System.Int32>::ApplyResultSelector:System.Collections.Generic.IEnumerable<System.Int32>(System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Lookup<System.Int32,System.Int32>", "ApplyResultSelector", True, "fact" },
    { 355, "System.Linq/System.Linq.Lookup<System.Int32,System.Int32>::ApplyResultSelector:System.Collections.Generic.IEnumerable<System.Int32>(System.Func<System.Int32,System.Collections.Generic.IEnumerable<System.Int32>,System.Int32>)", "codegen", "System.Linq.Lookup<System.Int32,System.Int32>", "ApplyResultSelector", True, "fact" },

};

static const int kSubjectEntryCount_Generated = 356;

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
    // Deferred enum registration — GC heap must be initialized first.
    ChaosRegisterCombinedSubjectsEnumMetadata();

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