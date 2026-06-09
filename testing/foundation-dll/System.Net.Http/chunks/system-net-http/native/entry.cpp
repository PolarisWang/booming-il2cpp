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
    { 0, "System.Net.Http/System.Net.Http.HttpClient::GetStringAsync:System.Threading.Tasks.Task<System.String>(System.String)", "codegen", "System.Net.Http.HttpClient", "GetStringAsync", True, "benchmark" },
    { 1, "System.Net.Http/System.Net.Http.HttpClient::GetStringAsync:System.Threading.Tasks.Task<System.String>(System.String)", "codegen", "System.Net.Http.HttpClient", "GetStringAsync", True, "benchmark" },
    { 2, "System.Net.Http/System.Net.Http.HttpClient::GetStringAsync:System.Threading.Tasks.Task<System.String>(System.Uri)", "codegen", "System.Net.Http.HttpClient", "GetStringAsync", True, "benchmark" },
    { 3, "System.Net.Http/System.Net.Http.HttpClient::GetStringAsync:System.Threading.Tasks.Task<System.String>(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetStringAsync", True, "benchmark" },
    { 4, "System.Net.Http/System.Net.Http.HttpClient::GetStringAsync:System.Threading.Tasks.Task<System.String>(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetStringAsync", True, "benchmark" },
    { 5, "System.Net.Http/System.Net.Http.HttpClient::GetStringAsync:System.Threading.Tasks.Task<System.String>(System.Uri,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetStringAsync", True, "benchmark" },
    { 6, "System.Net.Http/System.Net.Http.HttpClient::GetByteArrayAsync:System.Threading.Tasks.Task<System.Byte[]>(System.String)", "codegen", "System.Net.Http.HttpClient", "GetByteArrayAsync", True, "benchmark" },
    { 7, "System.Net.Http/System.Net.Http.HttpClient::GetByteArrayAsync:System.Threading.Tasks.Task<System.Byte[]>(System.String)", "codegen", "System.Net.Http.HttpClient", "GetByteArrayAsync", True, "benchmark" },
    { 8, "System.Net.Http/System.Net.Http.HttpClient::GetByteArrayAsync:System.Threading.Tasks.Task<System.Byte[]>(System.Uri)", "codegen", "System.Net.Http.HttpClient", "GetByteArrayAsync", True, "benchmark" },
    { 9, "System.Net.Http/System.Net.Http.HttpClient::GetByteArrayAsync:System.Threading.Tasks.Task<System.Byte[]>(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetByteArrayAsync", True, "benchmark" },
    { 10, "System.Net.Http/System.Net.Http.HttpClient::GetByteArrayAsync:System.Threading.Tasks.Task<System.Byte[]>(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetByteArrayAsync", True, "benchmark" },
    { 11, "System.Net.Http/System.Net.Http.HttpClient::GetByteArrayAsync:System.Threading.Tasks.Task<System.Byte[]>(System.Uri,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetByteArrayAsync", True, "benchmark" },
    { 12, "System.Net.Http/System.Net.Http.HttpClient::GetStreamAsync:System.Threading.Tasks.Task<System.IO.Stream>(System.String)", "codegen", "System.Net.Http.HttpClient", "GetStreamAsync", True, "benchmark" },
    { 13, "System.Net.Http/System.Net.Http.HttpClient::GetStreamAsync:System.Threading.Tasks.Task<System.IO.Stream>(System.String)", "codegen", "System.Net.Http.HttpClient", "GetStreamAsync", True, "benchmark" },
    { 14, "System.Net.Http/System.Net.Http.HttpClient::GetStreamAsync:System.Threading.Tasks.Task<System.IO.Stream>(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetStreamAsync", True, "benchmark" },
    { 15, "System.Net.Http/System.Net.Http.HttpClient::GetStreamAsync:System.Threading.Tasks.Task<System.IO.Stream>(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetStreamAsync", True, "benchmark" },
    { 16, "System.Net.Http/System.Net.Http.HttpClient::GetStreamAsync:System.Threading.Tasks.Task<System.IO.Stream>(System.Uri)", "codegen", "System.Net.Http.HttpClient", "GetStreamAsync", True, "benchmark" },
    { 17, "System.Net.Http/System.Net.Http.HttpClient::GetStreamAsync:System.Threading.Tasks.Task<System.IO.Stream>(System.Uri,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetStreamAsync", True, "benchmark" },
    { 18, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 19, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 20, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 21, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpCompletionOption)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 22, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpCompletionOption)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 23, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri,System.Net.Http.HttpCompletionOption)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 24, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 25, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 26, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 27, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpCompletionOption,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 28, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpCompletionOption,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 29, "System.Net.Http/System.Net.Http.HttpClient::GetAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri,System.Net.Http.HttpCompletionOption,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "GetAsync", True, "benchmark" },
    { 30, "System.Net.Http/System.Net.Http.HttpClient::PostAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent)", "codegen", "System.Net.Http.HttpClient", "PostAsync", True, "benchmark" },
    { 31, "System.Net.Http/System.Net.Http.HttpClient::PostAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent)", "codegen", "System.Net.Http.HttpClient", "PostAsync", True, "benchmark" },
    { 32, "System.Net.Http/System.Net.Http.HttpClient::PostAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri,System.Net.Http.HttpContent)", "codegen", "System.Net.Http.HttpClient", "PostAsync", True, "benchmark" },
    { 33, "System.Net.Http/System.Net.Http.HttpClient::PostAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "PostAsync", True, "benchmark" },
    { 34, "System.Net.Http/System.Net.Http.HttpClient::PostAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "PostAsync", True, "benchmark" },
    { 35, "System.Net.Http/System.Net.Http.HttpClient::PostAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri,System.Net.Http.HttpContent,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "PostAsync", True, "benchmark" },
    { 36, "System.Net.Http/System.Net.Http.HttpClient::PutAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent)", "codegen", "System.Net.Http.HttpClient", "PutAsync", True, "benchmark" },
    { 37, "System.Net.Http/System.Net.Http.HttpClient::PutAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent)", "codegen", "System.Net.Http.HttpClient", "PutAsync", True, "benchmark" },
    { 38, "System.Net.Http/System.Net.Http.HttpClient::PutAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri,System.Net.Http.HttpContent)", "codegen", "System.Net.Http.HttpClient", "PutAsync", True, "benchmark" },
    { 39, "System.Net.Http/System.Net.Http.HttpClient::PutAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "PutAsync", True, "benchmark" },
    { 40, "System.Net.Http/System.Net.Http.HttpClient::PutAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "PutAsync", True, "benchmark" },
    { 41, "System.Net.Http/System.Net.Http.HttpClient::PutAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri,System.Net.Http.HttpContent,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "PutAsync", True, "benchmark" },
    { 42, "System.Net.Http/System.Net.Http.HttpClient::PatchAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent)", "codegen", "System.Net.Http.HttpClient", "PatchAsync", True, "benchmark" },
    { 43, "System.Net.Http/System.Net.Http.HttpClient::PatchAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent)", "codegen", "System.Net.Http.HttpClient", "PatchAsync", True, "benchmark" },
    { 44, "System.Net.Http/System.Net.Http.HttpClient::PatchAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri,System.Net.Http.HttpContent)", "codegen", "System.Net.Http.HttpClient", "PatchAsync", True, "benchmark" },
    { 45, "System.Net.Http/System.Net.Http.HttpClient::PatchAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "PatchAsync", True, "benchmark" },
    { 46, "System.Net.Http/System.Net.Http.HttpClient::PatchAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Net.Http.HttpContent,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "PatchAsync", True, "benchmark" },
    { 47, "System.Net.Http/System.Net.Http.HttpClient::PatchAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri,System.Net.Http.HttpContent,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "PatchAsync", True, "benchmark" },
    { 48, "System.Net.Http/System.Net.Http.HttpClient::DeleteAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String)", "codegen", "System.Net.Http.HttpClient", "DeleteAsync", True, "benchmark" },
    { 49, "System.Net.Http/System.Net.Http.HttpClient::DeleteAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String)", "codegen", "System.Net.Http.HttpClient", "DeleteAsync", True, "benchmark" },
    { 50, "System.Net.Http/System.Net.Http.HttpClient::DeleteAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri)", "codegen", "System.Net.Http.HttpClient", "DeleteAsync", True, "benchmark" },
    { 51, "System.Net.Http/System.Net.Http.HttpClient::DeleteAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "DeleteAsync", True, "benchmark" },
    { 52, "System.Net.Http/System.Net.Http.HttpClient::DeleteAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "DeleteAsync", True, "benchmark" },
    { 53, "System.Net.Http/System.Net.Http.HttpClient::DeleteAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Uri,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "DeleteAsync", True, "benchmark" },
    { 54, "System.Net.Http/System.Net.Http.HttpClient::Send:System.Net.Http.HttpResponseMessage(System.Net.Http.HttpRequestMessage)", "codegen", "System.Net.Http.HttpClient", "Send", True, "benchmark" },
    { 55, "System.Net.Http/System.Net.Http.HttpClient::Send:System.Net.Http.HttpResponseMessage(System.Net.Http.HttpRequestMessage,System.Net.Http.HttpCompletionOption)", "codegen", "System.Net.Http.HttpClient", "Send", True, "benchmark" },
    { 56, "System.Net.Http/System.Net.Http.HttpClient::Send:System.Net.Http.HttpResponseMessage(System.Net.Http.HttpRequestMessage,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "Send", True, "benchmark" },
    { 57, "System.Net.Http/System.Net.Http.HttpClient::Send:System.Net.Http.HttpResponseMessage(System.Net.Http.HttpRequestMessage,System.Net.Http.HttpCompletionOption,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "Send", True, "benchmark" },
    { 58, "System.Net.Http/System.Net.Http.HttpClient::SendAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Net.Http.HttpRequestMessage)", "codegen", "System.Net.Http.HttpClient", "SendAsync", True, "benchmark" },
    { 59, "System.Net.Http/System.Net.Http.HttpClient::SendAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Net.Http.HttpRequestMessage,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "SendAsync", True, "benchmark" },
    { 60, "System.Net.Http/System.Net.Http.HttpClient::SendAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Net.Http.HttpRequestMessage,System.Net.Http.HttpCompletionOption)", "codegen", "System.Net.Http.HttpClient", "SendAsync", True, "benchmark" },
    { 61, "System.Net.Http/System.Net.Http.HttpClient::SendAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Net.Http.HttpRequestMessage,System.Net.Http.HttpCompletionOption,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpClient", "SendAsync", True, "benchmark" },
    { 62, "System.Net.Http/System.Net.Http.HttpClient::CancelPendingRequests:System.Void()", "codegen", "System.Net.Http.HttpClient", "CancelPendingRequests", True, "benchmark" },
    { 63, "System.Net.Http/System.Net.Http.HttpContent::ReadAsStringAsync:System.Threading.Tasks.Task<System.String>()", "codegen", "System.Net.Http.HttpContent", "ReadAsStringAsync", True, "benchmark" },
    { 64, "System.Net.Http/System.Net.Http.HttpContent::ReadAsStringAsync:System.Threading.Tasks.Task<System.String>(System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "ReadAsStringAsync", True, "benchmark" },
    { 65, "System.Net.Http/System.Net.Http.HttpContent::ReadAsByteArrayAsync:System.Threading.Tasks.Task<System.Byte[]>()", "codegen", "System.Net.Http.HttpContent", "ReadAsByteArrayAsync", True, "benchmark" },
    { 66, "System.Net.Http/System.Net.Http.HttpContent::ReadAsByteArrayAsync:System.Threading.Tasks.Task<System.Byte[]>(System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "ReadAsByteArrayAsync", True, "benchmark" },
    { 67, "System.Net.Http/System.Net.Http.HttpContent::ReadAsStream:System.IO.Stream()", "codegen", "System.Net.Http.HttpContent", "ReadAsStream", True, "benchmark" },
    { 68, "System.Net.Http/System.Net.Http.HttpContent::ReadAsStream:System.IO.Stream(System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "ReadAsStream", True, "benchmark" },
    { 69, "System.Net.Http/System.Net.Http.HttpContent::ReadAsStreamAsync:System.Threading.Tasks.Task<System.IO.Stream>()", "codegen", "System.Net.Http.HttpContent", "ReadAsStreamAsync", True, "benchmark" },
    { 70, "System.Net.Http/System.Net.Http.HttpContent::ReadAsStreamAsync:System.Threading.Tasks.Task<System.IO.Stream>(System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "ReadAsStreamAsync", True, "benchmark" },
    { 71, "System.Net.Http/System.Net.Http.HttpContent::CopyTo:System.Void(System.IO.Stream,System.Net.TransportContext,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "CopyTo", True, "benchmark" },
    { 72, "System.Net.Http/System.Net.Http.HttpContent::CopyTo:System.Void(System.IO.Stream,System.Net.TransportContext,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "CopyTo", True, "benchmark" },
    { 73, "System.Net.Http/System.Net.Http.HttpContent::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream)", "codegen", "System.Net.Http.HttpContent", "CopyToAsync", True, "benchmark" },
    { 74, "System.Net.Http/System.Net.Http.HttpContent::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream)", "codegen", "System.Net.Http.HttpContent", "CopyToAsync", True, "benchmark" },
    { 75, "System.Net.Http/System.Net.Http.HttpContent::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "CopyToAsync", True, "benchmark" },
    { 76, "System.Net.Http/System.Net.Http.HttpContent::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "CopyToAsync", True, "benchmark" },
    { 77, "System.Net.Http/System.Net.Http.HttpContent::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Net.TransportContext)", "codegen", "System.Net.Http.HttpContent", "CopyToAsync", True, "benchmark" },
    { 78, "System.Net.Http/System.Net.Http.HttpContent::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Net.TransportContext)", "codegen", "System.Net.Http.HttpContent", "CopyToAsync", True, "benchmark" },
    { 79, "System.Net.Http/System.Net.Http.HttpContent::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Net.TransportContext,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "CopyToAsync", True, "benchmark" },
    { 80, "System.Net.Http/System.Net.Http.HttpContent::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Net.TransportContext,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "CopyToAsync", True, "benchmark" },
    { 81, "System.Net.Http/System.Net.Http.HttpContent::LoadIntoBufferAsync:System.Threading.Tasks.Task()", "codegen", "System.Net.Http.HttpContent", "LoadIntoBufferAsync", True, "benchmark" },
    { 82, "System.Net.Http/System.Net.Http.HttpContent::LoadIntoBufferAsync:System.Threading.Tasks.Task(System.Int64)", "codegen", "System.Net.Http.HttpContent", "LoadIntoBufferAsync", True, "benchmark" },
    { 83, "System.Net.Http/System.Net.Http.HttpContent::LoadIntoBufferAsync:System.Threading.Tasks.Task(System.Int64)", "codegen", "System.Net.Http.HttpContent", "LoadIntoBufferAsync", True, "benchmark" },
    { 84, "System.Net.Http/System.Net.Http.HttpContent::LoadIntoBufferAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "LoadIntoBufferAsync", True, "benchmark" },
    { 85, "System.Net.Http/System.Net.Http.HttpContent::LoadIntoBufferAsync:System.Threading.Tasks.Task(System.Int64,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "LoadIntoBufferAsync", True, "benchmark" },
    { 86, "System.Net.Http/System.Net.Http.HttpContent::LoadIntoBufferAsync:System.Threading.Tasks.Task(System.Int64,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpContent", "LoadIntoBufferAsync", True, "benchmark" },
    { 87, "System.Net.Http/System.Net.Http.HttpContent::Dispose:System.Void()", "codegen", "System.Net.Http.HttpContent", "Dispose", True, "benchmark" },
    { 88, "System.Net.Http/System.Net.Http.HttpMessageHandler::Dispose:System.Void()", "codegen", "System.Net.Http.HttpMessageHandler", "Dispose", True, "benchmark" },
    { 89, "System.Net.Http/System.Net.Http.HttpMessageInvoker::Send:System.Net.Http.HttpResponseMessage(System.Net.Http.HttpRequestMessage,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpMessageInvoker", "Send", True, "benchmark" },
    { 90, "System.Net.Http/System.Net.Http.HttpMessageInvoker::SendAsync:System.Threading.Tasks.Task<System.Net.Http.HttpResponseMessage>(System.Net.Http.HttpRequestMessage,System.Threading.CancellationToken)", "codegen", "System.Net.Http.HttpMessageInvoker", "SendAsync", True, "benchmark" },
    { 91, "System.Net.Http/System.Net.Http.HttpMessageInvoker::Dispose:System.Void()", "codegen", "System.Net.Http.HttpMessageInvoker", "Dispose", True, "benchmark" },
    { 92, "System.Net.Http/System.Net.Http.HttpRequestMessage::Dispose:System.Void()", "codegen", "System.Net.Http.HttpRequestMessage", "Dispose", True, "benchmark" },
    { 93, "System.Net.Http/System.Net.Http.HttpRequestOptions::TryGetValue:System.Boolean(System.Net.Http.HttpRequestOptionsKey<System.Int32>,System.Int32)", "codegen", "System.Net.Http.HttpRequestOptions", "TryGetValue", True, "benchmark" },
    { 94, "System.Net.Http/System.Net.Http.HttpRequestOptions::TryGetValue:System.Boolean(System.Net.Http.HttpRequestOptionsKey<System.Int32>,System.Int32)", "codegen", "System.Net.Http.HttpRequestOptions", "TryGetValue", True, "benchmark" },
    { 95, "System.Net.Http/System.Net.Http.HttpRequestOptions::Set:System.Void(System.Net.Http.HttpRequestOptionsKey<System.Int32>,System.Int32)", "codegen", "System.Net.Http.HttpRequestOptions", "Set", True, "benchmark" },
    { 96, "System.Net.Http/System.Net.Http.HttpRequestOptions::Set:System.Void(System.Net.Http.HttpRequestOptionsKey<System.Int32>,System.Int32)", "codegen", "System.Net.Http.HttpRequestOptions", "Set", True, "benchmark" },
    { 97, "System.Net.Http/System.Net.Http.HttpResponseMessage::EnsureSuccessStatusCode:System.Net.Http.HttpResponseMessage()", "codegen", "System.Net.Http.HttpResponseMessage", "EnsureSuccessStatusCode", True, "benchmark" },
    { 98, "System.Net.Http/System.Net.Http.HttpResponseMessage::Dispose:System.Void()", "codegen", "System.Net.Http.HttpResponseMessage", "Dispose", True, "benchmark" },
    { 99, "System.Net.Http/System.Net.Http.MultipartContent::Add:System.Void(System.Net.Http.HttpContent)", "codegen", "System.Net.Http.MultipartContent", "Add", True, "benchmark" },
    { 100, "System.Net.Http/System.Net.Http.MultipartContent::GetEnumerator:System.Collections.Generic.IEnumerator<System.Net.Http.HttpContent>()", "codegen", "System.Net.Http.MultipartContent", "GetEnumerator", True, "benchmark" },
    { 101, "System.Net.Http/System.Net.Http.MultipartFormDataContent::Add:System.Void(System.Net.Http.HttpContent)", "codegen", "System.Net.Http.MultipartFormDataContent", "Add", True, "benchmark" },
    { 102, "System.Net.Http/System.Net.Http.MultipartFormDataContent::Add:System.Void(System.Net.Http.HttpContent,System.String)", "codegen", "System.Net.Http.MultipartFormDataContent", "Add", True, "benchmark" },
    { 103, "System.Net.Http/System.Net.Http.MultipartFormDataContent::Add:System.Void(System.Net.Http.HttpContent,System.String)", "codegen", "System.Net.Http.MultipartFormDataContent", "Add", True, "benchmark" },
    { 104, "System.Net.Http/System.Net.Http.MultipartFormDataContent::Add:System.Void(System.Net.Http.HttpContent,System.String,System.String)", "codegen", "System.Net.Http.MultipartFormDataContent", "Add", True, "benchmark" },
    { 105, "System.Net.Http/System.Net.Http.MultipartFormDataContent::Add:System.Void(System.Net.Http.HttpContent,System.String,System.String)", "codegen", "System.Net.Http.MultipartFormDataContent", "Add", True, "benchmark" },
    { 106, "System.Net.Http/System.Net.Http.MultipartFormDataContent::Add:System.Void(System.Net.Http.HttpContent,System.String,System.String)", "codegen", "System.Net.Http.MultipartFormDataContent", "Add", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 107;

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