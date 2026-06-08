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
    { 0, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetStatus:System.Diagnostics.Activity(System.Diagnostics.ActivityStatusCode,System.String)", "codegen", "System.Diagnostics.Activity", "SetStatus", True, "benchmark" },
    { 1, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetStatus:System.Diagnostics.Activity(System.Diagnostics.ActivityStatusCode,System.String)", "codegen", "System.Diagnostics.Activity", "SetStatus", True, "benchmark" },
    { 2, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::GetBaggageItem:System.String(System.String)", "codegen", "System.Diagnostics.Activity", "GetBaggageItem", True, "benchmark" },
    { 3, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::GetBaggageItem:System.String(System.String)", "codegen", "System.Diagnostics.Activity", "GetBaggageItem", True, "benchmark" },
    { 4, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::GetTagItem:System.Object(System.String)", "codegen", "System.Diagnostics.Activity", "GetTagItem", True, "benchmark" },
    { 5, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::GetTagItem:System.Object(System.String)", "codegen", "System.Diagnostics.Activity", "GetTagItem", True, "benchmark" },
    { 6, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddTag:System.Diagnostics.Activity(System.String,System.String)", "codegen", "System.Diagnostics.Activity", "AddTag", True, "benchmark" },
    { 7, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddTag:System.Diagnostics.Activity(System.String,System.String)", "codegen", "System.Diagnostics.Activity", "AddTag", True, "benchmark" },
    { 8, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddTag:System.Diagnostics.Activity(System.String,System.String)", "codegen", "System.Diagnostics.Activity", "AddTag", True, "benchmark" },
    { 9, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddTag:System.Diagnostics.Activity(System.String,System.Object)", "codegen", "System.Diagnostics.Activity", "AddTag", True, "benchmark" },
    { 10, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddTag:System.Diagnostics.Activity(System.String,System.Object)", "codegen", "System.Diagnostics.Activity", "AddTag", True, "benchmark" },
    { 11, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetTag:System.Diagnostics.Activity(System.String,System.Object)", "codegen", "System.Diagnostics.Activity", "SetTag", True, "benchmark" },
    { 12, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetTag:System.Diagnostics.Activity(System.String,System.Object)", "codegen", "System.Diagnostics.Activity", "SetTag", True, "benchmark" },
    { 13, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddEvent:System.Diagnostics.Activity(System.Diagnostics.ActivityEvent)", "codegen", "System.Diagnostics.Activity", "AddEvent", True, "benchmark" },
    { 14, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddException:System.Diagnostics.Activity(System.Exception,System.Diagnostics.TagList,System.DateTimeOffset)", "codegen", "System.Diagnostics.Activity", "AddException", True, "benchmark" },
    { 15, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddLink:System.Diagnostics.Activity(System.Diagnostics.ActivityLink)", "codegen", "System.Diagnostics.Activity", "AddLink", True, "benchmark" },
    { 16, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddBaggage:System.Diagnostics.Activity(System.String,System.String)", "codegen", "System.Diagnostics.Activity", "AddBaggage", True, "benchmark" },
    { 17, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddBaggage:System.Diagnostics.Activity(System.String,System.String)", "codegen", "System.Diagnostics.Activity", "AddBaggage", True, "benchmark" },
    { 18, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::AddBaggage:System.Diagnostics.Activity(System.String,System.String)", "codegen", "System.Diagnostics.Activity", "AddBaggage", True, "benchmark" },
    { 19, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetBaggage:System.Diagnostics.Activity(System.String,System.String)", "codegen", "System.Diagnostics.Activity", "SetBaggage", True, "benchmark" },
    { 20, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetBaggage:System.Diagnostics.Activity(System.String,System.String)", "codegen", "System.Diagnostics.Activity", "SetBaggage", True, "benchmark" },
    { 21, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetBaggage:System.Diagnostics.Activity(System.String,System.String)", "codegen", "System.Diagnostics.Activity", "SetBaggage", True, "benchmark" },
    { 22, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetParentId:System.Diagnostics.Activity(System.String)", "codegen", "System.Diagnostics.Activity", "SetParentId", True, "benchmark" },
    { 23, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetParentId:System.Diagnostics.Activity(System.String)", "codegen", "System.Diagnostics.Activity", "SetParentId", True, "benchmark" },
    { 24, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetParentId:System.Diagnostics.Activity(System.Diagnostics.ActivityTraceId,System.Diagnostics.ActivitySpanId,System.Diagnostics.ActivityTraceFlags)", "codegen", "System.Diagnostics.Activity", "SetParentId", True, "benchmark" },
    { 25, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetStartTime:System.Diagnostics.Activity(System.DateTime)", "codegen", "System.Diagnostics.Activity", "SetStartTime", True, "benchmark" },
    { 26, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetEndTime:System.Diagnostics.Activity(System.DateTime)", "codegen", "System.Diagnostics.Activity", "SetEndTime", True, "benchmark" },
    { 27, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::Stop:System.Void()", "codegen", "System.Diagnostics.Activity", "Stop", True, "benchmark" },
    { 28, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetIdFormat:System.Diagnostics.Activity(System.Diagnostics.ActivityIdFormat)", "codegen", "System.Diagnostics.Activity", "SetIdFormat", True, "benchmark" },
    { 29, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetCustomProperty:System.Void(System.String,System.Object)", "codegen", "System.Diagnostics.Activity", "SetCustomProperty", True, "benchmark" },
    { 30, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::SetCustomProperty:System.Void(System.String,System.Object)", "codegen", "System.Diagnostics.Activity", "SetCustomProperty", True, "benchmark" },
    { 31, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::GetCustomProperty:System.Object(System.String)", "codegen", "System.Diagnostics.Activity", "GetCustomProperty", True, "benchmark" },
    { 32, "System.Diagnostics.DiagnosticSource/System.Diagnostics.Activity::GetCustomProperty:System.Object(System.String)", "codegen", "System.Diagnostics.Activity", "GetCustomProperty", True, "benchmark" },
    { 33, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityContext::TryParse:System.Boolean(System.String,System.String,System.Boolean,System.Diagnostics.ActivityContext)", "codegen", "System.Diagnostics.ActivityContext", "TryParse", True, "benchmark" },
    { 34, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityContext::TryParse:System.Boolean(System.String,System.String,System.Boolean,System.Diagnostics.ActivityContext)", "codegen", "System.Diagnostics.ActivityContext", "TryParse", True, "benchmark" },
    { 35, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityContext::TryParse:System.Boolean(System.String,System.String,System.Boolean,System.Diagnostics.ActivityContext)", "codegen", "System.Diagnostics.ActivityContext", "TryParse", True, "benchmark" },
    { 36, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityContext::TryParse:System.Boolean(System.String,System.String,System.Boolean,System.Diagnostics.ActivityContext)", "codegen", "System.Diagnostics.ActivityContext", "TryParse", True, "benchmark" },
    { 37, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityContext::TryParse:System.Boolean(System.String,System.String,System.Diagnostics.ActivityContext)", "codegen", "System.Diagnostics.ActivityContext", "TryParse", True, "benchmark" },
    { 38, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityContext::TryParse:System.Boolean(System.String,System.String,System.Diagnostics.ActivityContext)", "codegen", "System.Diagnostics.ActivityContext", "TryParse", True, "benchmark" },
    { 39, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityContext::TryParse:System.Boolean(System.String,System.String,System.Diagnostics.ActivityContext)", "codegen", "System.Diagnostics.ActivityContext", "TryParse", True, "benchmark" },
    { 40, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityContext::Parse:System.Diagnostics.ActivityContext(System.String,System.String)", "codegen", "System.Diagnostics.ActivityContext", "Parse", True, "benchmark" },
    { 41, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityContext::Parse:System.Diagnostics.ActivityContext(System.String,System.String)", "codegen", "System.Diagnostics.ActivityContext", "Parse", True, "benchmark" },
    { 42, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityContext::Parse:System.Diagnostics.ActivityContext(System.String,System.String)", "codegen", "System.Diagnostics.ActivityContext", "Parse", True, "benchmark" },
    { 43, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityListener::Dispose:System.Void()", "codegen", "System.Diagnostics.ActivityListener", "Dispose", True, "benchmark" },
    { 44, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::HasListeners:System.Boolean()", "codegen", "System.Diagnostics.ActivitySource", "HasListeners", True, "benchmark" },
    { 45, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::CreateActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind)", "codegen", "System.Diagnostics.ActivitySource", "CreateActivity", True, "benchmark" },
    { 46, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::CreateActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind)", "codegen", "System.Diagnostics.ActivitySource", "CreateActivity", True, "benchmark" },
    { 47, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::CreateActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.Diagnostics.ActivityContext,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.Diagnostics.ActivityIdFormat)", "codegen", "System.Diagnostics.ActivitySource", "CreateActivity", True, "benchmark" },
    { 48, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::CreateActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.Diagnostics.ActivityContext,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.Diagnostics.ActivityIdFormat)", "codegen", "System.Diagnostics.ActivitySource", "CreateActivity", True, "benchmark" },
    { 49, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::CreateActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.Diagnostics.ActivityContext,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.Diagnostics.ActivityIdFormat)", "codegen", "System.Diagnostics.ActivitySource", "CreateActivity", True, "benchmark" },
    { 50, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::CreateActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.String,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.Diagnostics.ActivityIdFormat)", "codegen", "System.Diagnostics.ActivitySource", "CreateActivity", True, "benchmark" },
    { 51, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::CreateActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.String,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.Diagnostics.ActivityIdFormat)", "codegen", "System.Diagnostics.ActivitySource", "CreateActivity", True, "benchmark" },
    { 52, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::CreateActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.String,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.Diagnostics.ActivityIdFormat)", "codegen", "System.Diagnostics.ActivitySource", "CreateActivity", True, "benchmark" },
    { 53, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::CreateActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.String,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.Diagnostics.ActivityIdFormat)", "codegen", "System.Diagnostics.ActivitySource", "CreateActivity", True, "benchmark" },
    { 54, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 55, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 56, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.Diagnostics.ActivityContext,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.DateTimeOffset)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 57, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.Diagnostics.ActivityContext,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.DateTimeOffset)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 58, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.Diagnostics.ActivityContext,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.DateTimeOffset)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 59, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.String,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.DateTimeOffset)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 60, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.String,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.DateTimeOffset)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 61, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.String,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.DateTimeOffset)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 62, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.String,System.Diagnostics.ActivityKind,System.String,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.DateTimeOffset)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 63, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.Diagnostics.ActivityKind,System.Diagnostics.ActivityContext,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.DateTimeOffset,System.String)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 64, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::StartActivity:System.Diagnostics.Activity(System.Diagnostics.ActivityKind,System.Diagnostics.ActivityContext,System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Collections.Generic.IEnumerable<System.Diagnostics.ActivityLink>,System.DateTimeOffset,System.String)", "codegen", "System.Diagnostics.ActivitySource", "StartActivity", True, "benchmark" },
    { 65, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::Dispose:System.Void()", "codegen", "System.Diagnostics.ActivitySource", "Dispose", True, "benchmark" },
    { 66, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySource::AddActivityListener:System.Void(System.Diagnostics.ActivityListener)", "codegen", "System.Diagnostics.ActivitySource", "AddActivityListener", True, "benchmark" },
    { 67, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySpanId::CreateRandom:System.Diagnostics.ActivitySpanId()", "codegen", "System.Diagnostics.ActivitySpanId", "CreateRandom", True, "benchmark" },
    { 68, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivitySpanId::ToHexString:System.String()", "codegen", "System.Diagnostics.ActivitySpanId", "ToHexString", True, "benchmark" },
    { 69, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::Add:System.Void(System.String,System.Object)", "codegen", "System.Diagnostics.ActivityTagsCollection", "Add", True, "benchmark" },
    { 70, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::Add:System.Void(System.String,System.Object)", "codegen", "System.Diagnostics.ActivityTagsCollection", "Add", True, "benchmark" },
    { 71, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::Add:System.Void(System.Collections.Generic.KeyValuePair<System.String,System.Object>)", "codegen", "System.Diagnostics.ActivityTagsCollection", "Add", True, "benchmark" },
    { 72, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::Clear:System.Void()", "codegen", "System.Diagnostics.ActivityTagsCollection", "Clear", True, "benchmark" },
    { 73, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::Contains:System.Boolean(System.Collections.Generic.KeyValuePair<System.String,System.Object>)", "codegen", "System.Diagnostics.ActivityTagsCollection", "Contains", True, "benchmark" },
    { 74, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::ContainsKey:System.Boolean(System.String)", "codegen", "System.Diagnostics.ActivityTagsCollection", "ContainsKey", True, "benchmark" },
    { 75, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::ContainsKey:System.Boolean(System.String)", "codegen", "System.Diagnostics.ActivityTagsCollection", "ContainsKey", True, "benchmark" },
    { 76, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::CopyTo:System.Void(System.Collections.Generic.KeyValuePair<System.String,System.Object>[],System.Int32)", "codegen", "System.Diagnostics.ActivityTagsCollection", "CopyTo", True, "benchmark" },
    { 77, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::CopyTo:System.Void(System.Collections.Generic.KeyValuePair<System.String,System.Object>[],System.Int32)", "codegen", "System.Diagnostics.ActivityTagsCollection", "CopyTo", True, "benchmark" },
    { 78, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::CopyTo:System.Void(System.Collections.Generic.KeyValuePair<System.String,System.Object>[],System.Int32)", "codegen", "System.Diagnostics.ActivityTagsCollection", "CopyTo", True, "benchmark" },
    { 79, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::CopyTo:System.Void(System.Collections.Generic.KeyValuePair<System.String,System.Object>[],System.Int32)", "codegen", "System.Diagnostics.ActivityTagsCollection", "CopyTo", True, "benchmark" },
    { 80, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::Remove:System.Boolean(System.String)", "codegen", "System.Diagnostics.ActivityTagsCollection", "Remove", True, "benchmark" },
    { 81, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::Remove:System.Boolean(System.String)", "codegen", "System.Diagnostics.ActivityTagsCollection", "Remove", True, "benchmark" },
    { 82, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::Remove:System.Boolean(System.Collections.Generic.KeyValuePair<System.String,System.Object>)", "codegen", "System.Diagnostics.ActivityTagsCollection", "Remove", True, "benchmark" },
    { 83, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::TryGetValue:System.Boolean(System.String,System.Object)", "codegen", "System.Diagnostics.ActivityTagsCollection", "TryGetValue", True, "benchmark" },
    { 84, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTagsCollection::TryGetValue:System.Boolean(System.String,System.Object)", "codegen", "System.Diagnostics.ActivityTagsCollection", "TryGetValue", True, "benchmark" },
    { 85, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTraceId::CreateRandom:System.Diagnostics.ActivityTraceId()", "codegen", "System.Diagnostics.ActivityTraceId", "CreateRandom", True, "benchmark" },
    { 86, "System.Diagnostics.DiagnosticSource/System.Diagnostics.ActivityTraceId::ToHexString:System.String()", "codegen", "System.Diagnostics.ActivityTraceId", "ToHexString", True, "benchmark" },
    { 87, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::OnActivityImport:System.Void(System.Diagnostics.Activity,System.Object)", "codegen", "System.Diagnostics.DiagnosticListener", "OnActivityImport", True, "benchmark" },
    { 88, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::OnActivityExport:System.Void(System.Diagnostics.Activity,System.Object)", "codegen", "System.Diagnostics.DiagnosticListener", "OnActivityExport", True, "benchmark" },
    { 89, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::Subscribe:System.IDisposable(System.IObserver<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Func<System.String,System.Object,System.Object,System.Boolean>,System.Action<System.Diagnostics.Activity,System.Object>,System.Action<System.Diagnostics.Activity,System.Object>)", "codegen", "System.Diagnostics.DiagnosticListener", "Subscribe", True, "benchmark" },
    { 90, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::Subscribe:System.IDisposable(System.IObserver<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Func<System.String,System.Object,System.Object,System.Boolean>,System.Action<System.Diagnostics.Activity,System.Object>,System.Action<System.Diagnostics.Activity,System.Object>)", "codegen", "System.Diagnostics.DiagnosticListener", "Subscribe", True, "benchmark" },
    { 91, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::Subscribe:System.IDisposable(System.IObserver<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Predicate<System.String>)", "codegen", "System.Diagnostics.DiagnosticListener", "Subscribe", True, "benchmark" },
    { 92, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::Subscribe:System.IDisposable(System.IObserver<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Predicate<System.String>)", "codegen", "System.Diagnostics.DiagnosticListener", "Subscribe", True, "benchmark" },
    { 93, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::Subscribe:System.IDisposable(System.IObserver<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Func<System.String,System.Object,System.Object,System.Boolean>)", "codegen", "System.Diagnostics.DiagnosticListener", "Subscribe", True, "benchmark" },
    { 94, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::Subscribe:System.IDisposable(System.IObserver<System.Collections.Generic.KeyValuePair<System.String,System.Object>>,System.Func<System.String,System.Object,System.Object,System.Boolean>)", "codegen", "System.Diagnostics.DiagnosticListener", "Subscribe", True, "benchmark" },
    { 95, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::Subscribe:System.IDisposable(System.IObserver<System.Collections.Generic.KeyValuePair<System.String,System.Object>>)", "codegen", "System.Diagnostics.DiagnosticListener", "Subscribe", True, "benchmark" },
    { 96, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::Dispose:System.Void()", "codegen", "System.Diagnostics.DiagnosticListener", "Dispose", True, "benchmark" },
    { 97, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::IsEnabled:System.Boolean()", "codegen", "System.Diagnostics.DiagnosticListener", "IsEnabled", True, "benchmark" },
    { 98, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::IsEnabled:System.Boolean(System.String)", "codegen", "System.Diagnostics.DiagnosticListener", "IsEnabled", True, "benchmark" },
    { 99, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::IsEnabled:System.Boolean(System.String)", "codegen", "System.Diagnostics.DiagnosticListener", "IsEnabled", True, "benchmark" },
    { 100, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::IsEnabled:System.Boolean(System.String,System.Object,System.Object)", "codegen", "System.Diagnostics.DiagnosticListener", "IsEnabled", True, "benchmark" },
    { 101, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::IsEnabled:System.Boolean(System.String,System.Object,System.Object)", "codegen", "System.Diagnostics.DiagnosticListener", "IsEnabled", True, "benchmark" },
    { 102, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::Write:System.Void(System.String,System.Object)", "codegen", "System.Diagnostics.DiagnosticListener", "Write", True, "benchmark" },
    { 103, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticListener::Write:System.Void(System.String,System.Object)", "codegen", "System.Diagnostics.DiagnosticListener", "Write", True, "benchmark" },
    { 104, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::Write:System.Void(System.String,System.Object)", "codegen", "System.Diagnostics.DiagnosticSource", "Write", True, "benchmark" },
    { 105, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::Write:System.Void(System.String,System.Object)", "codegen", "System.Diagnostics.DiagnosticSource", "Write", True, "benchmark" },
    { 106, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::Write:System.Void(System.String,System.Int32)", "codegen", "System.Diagnostics.DiagnosticSource", "Write", True, "benchmark" },
    { 107, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::Write:System.Void(System.String,System.Int32)", "codegen", "System.Diagnostics.DiagnosticSource", "Write", True, "benchmark" },
    { 108, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::Write:System.Void(System.String,System.Int32)", "codegen", "System.Diagnostics.DiagnosticSource", "Write", True, "benchmark" },
    { 109, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::IsEnabled:System.Boolean(System.String)", "codegen", "System.Diagnostics.DiagnosticSource", "IsEnabled", True, "benchmark" },
    { 110, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::IsEnabled:System.Boolean(System.String)", "codegen", "System.Diagnostics.DiagnosticSource", "IsEnabled", True, "benchmark" },
    { 111, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::IsEnabled:System.Boolean(System.String,System.Object,System.Object)", "codegen", "System.Diagnostics.DiagnosticSource", "IsEnabled", True, "benchmark" },
    { 112, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::IsEnabled:System.Boolean(System.String,System.Object,System.Object)", "codegen", "System.Diagnostics.DiagnosticSource", "IsEnabled", True, "benchmark" },
    { 113, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::StartActivity:System.Diagnostics.Activity(System.Diagnostics.Activity,System.Object)", "codegen", "System.Diagnostics.DiagnosticSource", "StartActivity", True, "benchmark" },
    { 114, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::StartActivity:System.Diagnostics.Activity(System.Diagnostics.Activity,System.Int32)", "codegen", "System.Diagnostics.DiagnosticSource", "StartActivity", True, "benchmark" },
    { 115, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::StartActivity:System.Diagnostics.Activity(System.Diagnostics.Activity,System.Int32)", "codegen", "System.Diagnostics.DiagnosticSource", "StartActivity", True, "benchmark" },
    { 116, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::StopActivity:System.Void(System.Diagnostics.Activity,System.Object)", "codegen", "System.Diagnostics.DiagnosticSource", "StopActivity", True, "benchmark" },
    { 117, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::StopActivity:System.Void(System.Diagnostics.Activity,System.Int32)", "codegen", "System.Diagnostics.DiagnosticSource", "StopActivity", True, "benchmark" },
    { 118, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::StopActivity:System.Void(System.Diagnostics.Activity,System.Int32)", "codegen", "System.Diagnostics.DiagnosticSource", "StopActivity", True, "benchmark" },
    { 119, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::OnActivityImport:System.Void(System.Diagnostics.Activity,System.Object)", "codegen", "System.Diagnostics.DiagnosticSource", "OnActivityImport", True, "benchmark" },
    { 120, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DiagnosticSource::OnActivityExport:System.Void(System.Diagnostics.Activity,System.Object)", "codegen", "System.Diagnostics.DiagnosticSource", "OnActivityExport", True, "benchmark" },
    { 121, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DistributedContextPropagator::Inject:System.Void(System.Diagnostics.Activity,System.Object,DistributedContextPropagator.PropagatorSetterCallback)", "codegen", "System.Diagnostics.DistributedContextPropagator", "Inject", True, "benchmark" },
    { 122, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DistributedContextPropagator::ExtractTraceIdAndState:System.Void(System.Object,DistributedContextPropagator.PropagatorGetterCallback,System.String,System.String)", "codegen", "System.Diagnostics.DistributedContextPropagator", "ExtractTraceIdAndState", True, "benchmark" },
    { 123, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DistributedContextPropagator::ExtractTraceIdAndState:System.Void(System.Object,DistributedContextPropagator.PropagatorGetterCallback,System.String,System.String)", "codegen", "System.Diagnostics.DistributedContextPropagator", "ExtractTraceIdAndState", True, "benchmark" },
    { 124, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DistributedContextPropagator::ExtractBaggage:System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<System.String,System.String>>(System.Object,DistributedContextPropagator.PropagatorGetterCallback)", "codegen", "System.Diagnostics.DistributedContextPropagator", "ExtractBaggage", True, "benchmark" },
    { 125, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DistributedContextPropagator::CreateDefaultPropagator:System.Diagnostics.DistributedContextPropagator()", "codegen", "System.Diagnostics.DistributedContextPropagator", "CreateDefaultPropagator", True, "benchmark" },
    { 126, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DistributedContextPropagator::CreatePassThroughPropagator:System.Diagnostics.DistributedContextPropagator()", "codegen", "System.Diagnostics.DistributedContextPropagator", "CreatePassThroughPropagator", True, "benchmark" },
    { 127, "System.Diagnostics.DiagnosticSource/System.Diagnostics.DistributedContextPropagator::CreateNoOutputPropagator:System.Diagnostics.DistributedContextPropagator()", "codegen", "System.Diagnostics.DistributedContextPropagator", "CreateNoOutputPropagator", True, "benchmark" },
    { 128, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::Add:System.Void(System.String,System.Object)", "codegen", "System.Diagnostics.TagList", "Add", True, "benchmark" },
    { 129, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::Add:System.Void(System.String,System.Object)", "codegen", "System.Diagnostics.TagList", "Add", True, "benchmark" },
    { 130, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::Add:System.Void(System.Collections.Generic.KeyValuePair<System.String,System.Object>)", "codegen", "System.Diagnostics.TagList", "Add", True, "benchmark" },
    { 131, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::CopyTo:System.Void(System.Collections.Generic.KeyValuePair<System.String,System.Object>[],System.Int32)", "codegen", "System.Diagnostics.TagList", "CopyTo", True, "benchmark" },
    { 132, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::CopyTo:System.Void(System.Collections.Generic.KeyValuePair<System.String,System.Object>[],System.Int32)", "codegen", "System.Diagnostics.TagList", "CopyTo", True, "benchmark" },
    { 133, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::CopyTo:System.Void(System.Collections.Generic.KeyValuePair<System.String,System.Object>[],System.Int32)", "codegen", "System.Diagnostics.TagList", "CopyTo", True, "benchmark" },
    { 134, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::CopyTo:System.Void(System.Collections.Generic.KeyValuePair<System.String,System.Object>[],System.Int32)", "codegen", "System.Diagnostics.TagList", "CopyTo", True, "benchmark" },
    { 135, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::Insert:System.Void(System.Int32,System.Collections.Generic.KeyValuePair<System.String,System.Object>)", "codegen", "System.Diagnostics.TagList", "Insert", True, "benchmark" },
    { 136, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::Insert:System.Void(System.Int32,System.Collections.Generic.KeyValuePair<System.String,System.Object>)", "codegen", "System.Diagnostics.TagList", "Insert", True, "benchmark" },
    { 137, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::RemoveAt:System.Void(System.Int32)", "codegen", "System.Diagnostics.TagList", "RemoveAt", True, "benchmark" },
    { 138, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::RemoveAt:System.Void(System.Int32)", "codegen", "System.Diagnostics.TagList", "RemoveAt", True, "benchmark" },
    { 139, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::Clear:System.Void()", "codegen", "System.Diagnostics.TagList", "Clear", True, "benchmark" },
    { 140, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::Contains:System.Boolean(System.Collections.Generic.KeyValuePair<System.String,System.Object>)", "codegen", "System.Diagnostics.TagList", "Contains", True, "benchmark" },
    { 141, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::Remove:System.Boolean(System.Collections.Generic.KeyValuePair<System.String,System.Object>)", "codegen", "System.Diagnostics.TagList", "Remove", True, "benchmark" },
    { 142, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::GetEnumerator:System.Collections.Generic.IEnumerator<System.Collections.Generic.KeyValuePair<System.String,System.Object>>()", "codegen", "System.Diagnostics.TagList", "GetEnumerator", True, "benchmark" },
    { 143, "System.Diagnostics.DiagnosticSource/System.Diagnostics.TagList::IndexOf:System.Int32(System.Collections.Generic.KeyValuePair<System.String,System.Object>)", "codegen", "System.Diagnostics.TagList", "IndexOf", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 144;

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