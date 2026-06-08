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
    { 0, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)", "codegen", "System.Net.Sockets.NetworkStream", "Seek", True, "benchmark" },
    { 1, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)", "codegen", "System.Net.Sockets.NetworkStream", "Seek", True, "benchmark" },
    { 2, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)", "codegen", "System.Net.Sockets.NetworkStream", "Seek", True, "benchmark" },
    { 3, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Read", True, "benchmark" },
    { 4, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Read", True, "benchmark" },
    { 5, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Read", True, "benchmark" },
    { 6, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Read", True, "benchmark" },
    { 7, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Read", True, "benchmark" },
    { 8, "System.Net.Sockets/System.Net.Sockets.NetworkStream::ReadByte:System.Int32()", "codegen", "System.Net.Sockets.NetworkStream", "ReadByte", True, "benchmark" },
    { 9, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Write", True, "benchmark" },
    { 10, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Write", True, "benchmark" },
    { 11, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Write", True, "benchmark" },
    { 12, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Write", True, "benchmark" },
    { 13, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Write:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Write", True, "benchmark" },
    { 14, "System.Net.Sockets/System.Net.Sockets.NetworkStream::WriteByte:System.Void(System.Byte)", "codegen", "System.Net.Sockets.NetworkStream", "WriteByte", True, "benchmark" },
    { 15, "System.Net.Sockets/System.Net.Sockets.NetworkStream::WriteByte:System.Void(System.Byte)", "codegen", "System.Net.Sockets.NetworkStream", "WriteByte", True, "benchmark" },
    { 16, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Close:System.Void(System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Close", True, "benchmark" },
    { 17, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Close:System.Void(System.Int32)", "codegen", "System.Net.Sockets.NetworkStream", "Close", True, "benchmark" },
    { 18, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Close:System.Void(System.TimeSpan)", "codegen", "System.Net.Sockets.NetworkStream", "Close", True, "benchmark" },
    { 19, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Close:System.Void(System.TimeSpan)", "codegen", "System.Net.Sockets.NetworkStream", "Close", True, "benchmark" },
    { 20, "System.Net.Sockets/System.Net.Sockets.NetworkStream::BeginRead:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.NetworkStream", "BeginRead", True, "benchmark" },
    { 21, "System.Net.Sockets/System.Net.Sockets.NetworkStream::BeginRead:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.NetworkStream", "BeginRead", True, "benchmark" },
    { 22, "System.Net.Sockets/System.Net.Sockets.NetworkStream::BeginRead:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.NetworkStream", "BeginRead", True, "benchmark" },
    { 23, "System.Net.Sockets/System.Net.Sockets.NetworkStream::BeginRead:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.NetworkStream", "BeginRead", True, "benchmark" },
    { 24, "System.Net.Sockets/System.Net.Sockets.NetworkStream::BeginRead:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.NetworkStream", "BeginRead", True, "benchmark" },
    { 25, "System.Net.Sockets/System.Net.Sockets.NetworkStream::EndRead:System.Int32(System.IAsyncResult)", "codegen", "System.Net.Sockets.NetworkStream", "EndRead", True, "benchmark" },
    { 26, "System.Net.Sockets/System.Net.Sockets.NetworkStream::BeginWrite:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.NetworkStream", "BeginWrite", True, "benchmark" },
    { 27, "System.Net.Sockets/System.Net.Sockets.NetworkStream::BeginWrite:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.NetworkStream", "BeginWrite", True, "benchmark" },
    { 28, "System.Net.Sockets/System.Net.Sockets.NetworkStream::BeginWrite:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.NetworkStream", "BeginWrite", True, "benchmark" },
    { 29, "System.Net.Sockets/System.Net.Sockets.NetworkStream::BeginWrite:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.NetworkStream", "BeginWrite", True, "benchmark" },
    { 30, "System.Net.Sockets/System.Net.Sockets.NetworkStream::BeginWrite:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.NetworkStream", "BeginWrite", True, "benchmark" },
    { 31, "System.Net.Sockets/System.Net.Sockets.NetworkStream::EndWrite:System.Void(System.IAsyncResult)", "codegen", "System.Net.Sockets.NetworkStream", "EndWrite", True, "benchmark" },
    { 32, "System.Net.Sockets/System.Net.Sockets.NetworkStream::ReadAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "ReadAsync", True, "benchmark" },
    { 33, "System.Net.Sockets/System.Net.Sockets.NetworkStream::ReadAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "ReadAsync", True, "benchmark" },
    { 34, "System.Net.Sockets/System.Net.Sockets.NetworkStream::ReadAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "ReadAsync", True, "benchmark" },
    { 35, "System.Net.Sockets/System.Net.Sockets.NetworkStream::ReadAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "ReadAsync", True, "benchmark" },
    { 36, "System.Net.Sockets/System.Net.Sockets.NetworkStream::ReadAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "ReadAsync", True, "benchmark" },
    { 37, "System.Net.Sockets/System.Net.Sockets.NetworkStream::ReadAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "ReadAsync", True, "benchmark" },
    { 38, "System.Net.Sockets/System.Net.Sockets.NetworkStream::WriteAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "WriteAsync", True, "benchmark" },
    { 39, "System.Net.Sockets/System.Net.Sockets.NetworkStream::WriteAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "WriteAsync", True, "benchmark" },
    { 40, "System.Net.Sockets/System.Net.Sockets.NetworkStream::WriteAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "WriteAsync", True, "benchmark" },
    { 41, "System.Net.Sockets/System.Net.Sockets.NetworkStream::WriteAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "WriteAsync", True, "benchmark" },
    { 42, "System.Net.Sockets/System.Net.Sockets.NetworkStream::WriteAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "WriteAsync", True, "benchmark" },
    { 43, "System.Net.Sockets/System.Net.Sockets.NetworkStream::WriteAsync:System.Threading.Tasks.ValueTask(System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "WriteAsync", True, "benchmark" },
    { 44, "System.Net.Sockets/System.Net.Sockets.NetworkStream::Flush:System.Void()", "codegen", "System.Net.Sockets.NetworkStream", "Flush", True, "benchmark" },
    { 45, "System.Net.Sockets/System.Net.Sockets.NetworkStream::FlushAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.NetworkStream", "FlushAsync", True, "benchmark" },
    { 46, "System.Net.Sockets/System.Net.Sockets.NetworkStream::SetLength:System.Void(System.Int64)", "codegen", "System.Net.Sockets.NetworkStream", "SetLength", True, "benchmark" },
    { 47, "System.Net.Sockets/System.Net.Sockets.NetworkStream::SetLength:System.Void(System.Int64)", "codegen", "System.Net.Sockets.NetworkStream", "SetLength", True, "benchmark" },
    { 48, "System.Net.Sockets/System.Net.Sockets.Socket::Bind:System.Void(System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "Bind", True, "benchmark" },
    { 49, "System.Net.Sockets/System.Net.Sockets.Socket::Connect:System.Void(System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "Connect", True, "benchmark" },
    { 50, "System.Net.Sockets/System.Net.Sockets.Socket::Connect:System.Void(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.Socket", "Connect", True, "benchmark" },
    { 51, "System.Net.Sockets/System.Net.Sockets.Socket::Connect:System.Void(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.Socket", "Connect", True, "benchmark" },
    { 52, "System.Net.Sockets/System.Net.Sockets.Socket::Connect:System.Void(System.String,System.Int32)", "codegen", "System.Net.Sockets.Socket", "Connect", True, "benchmark" },
    { 53, "System.Net.Sockets/System.Net.Sockets.Socket::Connect:System.Void(System.String,System.Int32)", "codegen", "System.Net.Sockets.Socket", "Connect", True, "benchmark" },
    { 54, "System.Net.Sockets/System.Net.Sockets.Socket::Connect:System.Void(System.String,System.Int32)", "codegen", "System.Net.Sockets.Socket", "Connect", True, "benchmark" },
    { 55, "System.Net.Sockets/System.Net.Sockets.Socket::Connect:System.Void(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.Socket", "Connect", True, "benchmark" },
    { 56, "System.Net.Sockets/System.Net.Sockets.Socket::Connect:System.Void(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.Socket", "Connect", True, "benchmark" },
    { 57, "System.Net.Sockets/System.Net.Sockets.Socket::Connect:System.Void(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.Socket", "Connect", True, "benchmark" },
    { 58, "System.Net.Sockets/System.Net.Sockets.Socket::Connect:System.Void(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.Socket", "Connect", True, "benchmark" },
    { 59, "System.Net.Sockets/System.Net.Sockets.Socket::Close:System.Void()", "codegen", "System.Net.Sockets.Socket", "Close", True, "benchmark" },
    { 60, "System.Net.Sockets/System.Net.Sockets.Socket::Close:System.Void(System.Int32)", "codegen", "System.Net.Sockets.Socket", "Close", True, "benchmark" },
    { 61, "System.Net.Sockets/System.Net.Sockets.Socket::Close:System.Void(System.Int32)", "codegen", "System.Net.Sockets.Socket", "Close", True, "benchmark" },
    { 62, "System.Net.Sockets/System.Net.Sockets.Socket::Listen:System.Void()", "codegen", "System.Net.Sockets.Socket", "Listen", True, "benchmark" },
    { 63, "System.Net.Sockets/System.Net.Sockets.Socket::Listen:System.Void(System.Int32)", "codegen", "System.Net.Sockets.Socket", "Listen", True, "benchmark" },
    { 64, "System.Net.Sockets/System.Net.Sockets.Socket::Listen:System.Void(System.Int32)", "codegen", "System.Net.Sockets.Socket", "Listen", True, "benchmark" },
    { 65, "System.Net.Sockets/System.Net.Sockets.Socket::Accept:System.Net.Sockets.Socket()", "codegen", "System.Net.Sockets.Socket", "Accept", True, "benchmark" },
    { 66, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 67, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 68, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 69, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 70, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 71, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 72, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 73, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[])", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 74, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[])", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 75, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[])", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 76, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 77, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 78, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 79, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 80, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 81, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 82, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 83, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 84, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 85, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 86, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 87, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 88, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 89, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 90, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 91, "System.Net.Sockets/System.Net.Sockets.Socket::Send:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Send", True, "benchmark" },
    { 92, "System.Net.Sockets/System.Net.Sockets.Socket::SendFile:System.Void(System.String)", "codegen", "System.Net.Sockets.Socket", "SendFile", True, "benchmark" },
    { 93, "System.Net.Sockets/System.Net.Sockets.Socket::SendFile:System.Void(System.String)", "codegen", "System.Net.Sockets.Socket", "SendFile", True, "benchmark" },
    { 94, "System.Net.Sockets/System.Net.Sockets.Socket::SendFile:System.Void(System.String,System.Byte[],System.Byte[],System.Net.Sockets.TransmitFileOptions)", "codegen", "System.Net.Sockets.Socket", "SendFile", True, "benchmark" },
    { 95, "System.Net.Sockets/System.Net.Sockets.Socket::SendFile:System.Void(System.String,System.Byte[],System.Byte[],System.Net.Sockets.TransmitFileOptions)", "codegen", "System.Net.Sockets.Socket", "SendFile", True, "benchmark" },
    { 96, "System.Net.Sockets/System.Net.Sockets.Socket::SendFile:System.Void(System.String,System.Byte[],System.Byte[],System.Net.Sockets.TransmitFileOptions)", "codegen", "System.Net.Sockets.Socket", "SendFile", True, "benchmark" },
    { 97, "System.Net.Sockets/System.Net.Sockets.Socket::SendFile:System.Void(System.String,System.Byte[],System.Byte[],System.Net.Sockets.TransmitFileOptions)", "codegen", "System.Net.Sockets.Socket", "SendFile", True, "benchmark" },
    { 98, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 99, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 100, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 101, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 102, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 103, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 104, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 105, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 106, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 107, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 108, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 109, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 110, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 111, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 112, "System.Net.Sockets/System.Net.Sockets.Socket::SendTo:System.Int32(System.Byte[],System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendTo", True, "benchmark" },
    { 113, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 114, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 115, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 116, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 117, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 118, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 119, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 120, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[])", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 121, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[])", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 122, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[])", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 123, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 124, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 125, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 126, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 127, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 128, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 129, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 130, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 131, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 132, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 133, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 134, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 135, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 136, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 137, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 138, "System.Net.Sockets/System.Net.Sockets.Socket::Receive:System.Int32(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "Receive", True, "benchmark" },
    { 139, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveMessageFrom:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.Net.Sockets.IPPacketInformation)", "codegen", "System.Net.Sockets.Socket", "ReceiveMessageFrom", True, "benchmark" },
    { 140, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveMessageFrom:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.Net.Sockets.IPPacketInformation)", "codegen", "System.Net.Sockets.Socket", "ReceiveMessageFrom", True, "benchmark" },
    { 141, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveMessageFrom:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.Net.Sockets.IPPacketInformation)", "codegen", "System.Net.Sockets.Socket", "ReceiveMessageFrom", True, "benchmark" },
    { 142, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveMessageFrom:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.Net.Sockets.IPPacketInformation)", "codegen", "System.Net.Sockets.Socket", "ReceiveMessageFrom", True, "benchmark" },
    { 143, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveMessageFrom:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.Net.Sockets.IPPacketInformation)", "codegen", "System.Net.Sockets.Socket", "ReceiveMessageFrom", True, "benchmark" },
    { 144, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 145, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 146, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 147, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 148, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 149, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 150, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 151, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 152, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 153, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 154, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 155, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 156, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 157, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 158, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFrom:System.Int32(System.Byte[],System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFrom", True, "benchmark" },
    { 159, "System.Net.Sockets/System.Net.Sockets.Socket::IOControl:System.Int32(System.Int32,System.Byte[],System.Byte[])", "codegen", "System.Net.Sockets.Socket", "IOControl", True, "benchmark" },
    { 160, "System.Net.Sockets/System.Net.Sockets.Socket::IOControl:System.Int32(System.Int32,System.Byte[],System.Byte[])", "codegen", "System.Net.Sockets.Socket", "IOControl", True, "benchmark" },
    { 161, "System.Net.Sockets/System.Net.Sockets.Socket::IOControl:System.Int32(System.Int32,System.Byte[],System.Byte[])", "codegen", "System.Net.Sockets.Socket", "IOControl", True, "benchmark" },
    { 162, "System.Net.Sockets/System.Net.Sockets.Socket::IOControl:System.Int32(System.Int32,System.Byte[],System.Byte[])", "codegen", "System.Net.Sockets.Socket", "IOControl", True, "benchmark" },
    { 163, "System.Net.Sockets/System.Net.Sockets.Socket::IOControl:System.Int32(System.Net.Sockets.IOControlCode,System.Byte[],System.Byte[])", "codegen", "System.Net.Sockets.Socket", "IOControl", True, "benchmark" },
    { 164, "System.Net.Sockets/System.Net.Sockets.Socket::IOControl:System.Int32(System.Net.Sockets.IOControlCode,System.Byte[],System.Byte[])", "codegen", "System.Net.Sockets.Socket", "IOControl", True, "benchmark" },
    { 165, "System.Net.Sockets/System.Net.Sockets.Socket::IOControl:System.Int32(System.Net.Sockets.IOControlCode,System.Byte[],System.Byte[])", "codegen", "System.Net.Sockets.Socket", "IOControl", True, "benchmark" },
    { 166, "System.Net.Sockets/System.Net.Sockets.Socket::SetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Int32)", "codegen", "System.Net.Sockets.Socket", "SetSocketOption", True, "benchmark" },
    { 167, "System.Net.Sockets/System.Net.Sockets.Socket::SetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Int32)", "codegen", "System.Net.Sockets.Socket", "SetSocketOption", True, "benchmark" },
    { 168, "System.Net.Sockets/System.Net.Sockets.Socket::SetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Byte[])", "codegen", "System.Net.Sockets.Socket", "SetSocketOption", True, "benchmark" },
    { 169, "System.Net.Sockets/System.Net.Sockets.Socket::SetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Byte[])", "codegen", "System.Net.Sockets.Socket", "SetSocketOption", True, "benchmark" },
    { 170, "System.Net.Sockets/System.Net.Sockets.Socket::SetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Byte[])", "codegen", "System.Net.Sockets.Socket", "SetSocketOption", True, "benchmark" },
    { 171, "System.Net.Sockets/System.Net.Sockets.Socket::SetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Boolean)", "codegen", "System.Net.Sockets.Socket", "SetSocketOption", True, "benchmark" },
    { 172, "System.Net.Sockets/System.Net.Sockets.Socket::SetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Boolean)", "codegen", "System.Net.Sockets.Socket", "SetSocketOption", True, "benchmark" },
    { 173, "System.Net.Sockets/System.Net.Sockets.Socket::SetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Object)", "codegen", "System.Net.Sockets.Socket", "SetSocketOption", True, "benchmark" },
    { 174, "System.Net.Sockets/System.Net.Sockets.Socket::GetSocketOption:System.Object(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName)", "codegen", "System.Net.Sockets.Socket", "GetSocketOption", True, "benchmark" },
    { 175, "System.Net.Sockets/System.Net.Sockets.Socket::GetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Byte[])", "codegen", "System.Net.Sockets.Socket", "GetSocketOption", True, "benchmark" },
    { 176, "System.Net.Sockets/System.Net.Sockets.Socket::GetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Byte[])", "codegen", "System.Net.Sockets.Socket", "GetSocketOption", True, "benchmark" },
    { 177, "System.Net.Sockets/System.Net.Sockets.Socket::GetSocketOption:System.Void(System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Byte[])", "codegen", "System.Net.Sockets.Socket", "GetSocketOption", True, "benchmark" },
    { 178, "System.Net.Sockets/System.Net.Sockets.Socket::GetSocketOption:System.Byte[](System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Int32)", "codegen", "System.Net.Sockets.Socket", "GetSocketOption", True, "benchmark" },
    { 179, "System.Net.Sockets/System.Net.Sockets.Socket::GetSocketOption:System.Byte[](System.Net.Sockets.SocketOptionLevel,System.Net.Sockets.SocketOptionName,System.Int32)", "codegen", "System.Net.Sockets.Socket", "GetSocketOption", True, "benchmark" },
    { 180, "System.Net.Sockets/System.Net.Sockets.Socket::SetIPProtectionLevel:System.Void(System.Net.Sockets.IPProtectionLevel)", "codegen", "System.Net.Sockets.Socket", "SetIPProtectionLevel", True, "benchmark" },
    { 181, "System.Net.Sockets/System.Net.Sockets.Socket::Poll:System.Boolean(System.Int32,System.Net.Sockets.SelectMode)", "codegen", "System.Net.Sockets.Socket", "Poll", True, "benchmark" },
    { 182, "System.Net.Sockets/System.Net.Sockets.Socket::Poll:System.Boolean(System.Int32,System.Net.Sockets.SelectMode)", "codegen", "System.Net.Sockets.Socket", "Poll", True, "benchmark" },
    { 183, "System.Net.Sockets/System.Net.Sockets.Socket::Poll:System.Boolean(System.TimeSpan,System.Net.Sockets.SelectMode)", "codegen", "System.Net.Sockets.Socket", "Poll", True, "benchmark" },
    { 184, "System.Net.Sockets/System.Net.Sockets.Socket::Poll:System.Boolean(System.TimeSpan,System.Net.Sockets.SelectMode)", "codegen", "System.Net.Sockets.Socket", "Poll", True, "benchmark" },
    { 185, "System.Net.Sockets/System.Net.Sockets.Socket::Select:System.Void(System.Collections.IList,System.Collections.IList,System.Collections.IList,System.Int32)", "codegen", "System.Net.Sockets.Socket", "Select", True, "benchmark" },
    { 186, "System.Net.Sockets/System.Net.Sockets.Socket::Select:System.Void(System.Collections.IList,System.Collections.IList,System.Collections.IList,System.Int32)", "codegen", "System.Net.Sockets.Socket", "Select", True, "benchmark" },
    { 187, "System.Net.Sockets/System.Net.Sockets.Socket::Select:System.Void(System.Collections.IList,System.Collections.IList,System.Collections.IList,System.TimeSpan)", "codegen", "System.Net.Sockets.Socket", "Select", True, "benchmark" },
    { 188, "System.Net.Sockets/System.Net.Sockets.Socket::Select:System.Void(System.Collections.IList,System.Collections.IList,System.Collections.IList,System.TimeSpan)", "codegen", "System.Net.Sockets.Socket", "Select", True, "benchmark" },
    { 189, "System.Net.Sockets/System.Net.Sockets.Socket::BeginConnect:System.IAsyncResult(System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginConnect", True, "benchmark" },
    { 190, "System.Net.Sockets/System.Net.Sockets.Socket::BeginConnect:System.IAsyncResult(System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginConnect", True, "benchmark" },
    { 191, "System.Net.Sockets/System.Net.Sockets.Socket::BeginConnect:System.IAsyncResult(System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginConnect", True, "benchmark" },
    { 192, "System.Net.Sockets/System.Net.Sockets.Socket::BeginConnect:System.IAsyncResult(System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginConnect", True, "benchmark" },
    { 193, "System.Net.Sockets/System.Net.Sockets.Socket::BeginConnect:System.IAsyncResult(System.Net.IPAddress,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginConnect", True, "benchmark" },
    { 194, "System.Net.Sockets/System.Net.Sockets.Socket::BeginConnect:System.IAsyncResult(System.Net.IPAddress,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginConnect", True, "benchmark" },
    { 195, "System.Net.Sockets/System.Net.Sockets.Socket::BeginConnect:System.IAsyncResult(System.Net.IPAddress[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginConnect", True, "benchmark" },
    { 196, "System.Net.Sockets/System.Net.Sockets.Socket::BeginConnect:System.IAsyncResult(System.Net.IPAddress[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginConnect", True, "benchmark" },
    { 197, "System.Net.Sockets/System.Net.Sockets.Socket::BeginConnect:System.IAsyncResult(System.Net.IPAddress[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginConnect", True, "benchmark" },
    { 198, "System.Net.Sockets/System.Net.Sockets.Socket::BeginConnect:System.IAsyncResult(System.Net.IPAddress[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginConnect", True, "benchmark" },
    { 199, "System.Net.Sockets/System.Net.Sockets.Socket::EndConnect:System.Void(System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndConnect", True, "benchmark" },
    { 200, "System.Net.Sockets/System.Net.Sockets.Socket::BeginDisconnect:System.IAsyncResult(System.Boolean,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginDisconnect", True, "benchmark" },
    { 201, "System.Net.Sockets/System.Net.Sockets.Socket::BeginDisconnect:System.IAsyncResult(System.Boolean,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginDisconnect", True, "benchmark" },
    { 202, "System.Net.Sockets/System.Net.Sockets.Socket::Disconnect:System.Void(System.Boolean)", "codegen", "System.Net.Sockets.Socket", "Disconnect", True, "benchmark" },
    { 203, "System.Net.Sockets/System.Net.Sockets.Socket::Disconnect:System.Void(System.Boolean)", "codegen", "System.Net.Sockets.Socket", "Disconnect", True, "benchmark" },
    { 204, "System.Net.Sockets/System.Net.Sockets.Socket::EndDisconnect:System.Void(System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndDisconnect", True, "benchmark" },
    { 205, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 206, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 207, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 208, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 209, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 210, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 211, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 212, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 213, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 214, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 215, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 216, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 217, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 218, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSend:System.IAsyncResult(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSend", True, "benchmark" },
    { 219, "System.Net.Sockets/System.Net.Sockets.Socket::EndSend:System.Int32(System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndSend", True, "benchmark" },
    { 220, "System.Net.Sockets/System.Net.Sockets.Socket::EndSend:System.Int32(System.IAsyncResult,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "EndSend", True, "benchmark" },
    { 221, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendFile:System.IAsyncResult(System.String,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendFile", True, "benchmark" },
    { 222, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendFile:System.IAsyncResult(System.String,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendFile", True, "benchmark" },
    { 223, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendFile:System.IAsyncResult(System.String,System.Byte[],System.Byte[],System.Net.Sockets.TransmitFileOptions,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendFile", True, "benchmark" },
    { 224, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendFile:System.IAsyncResult(System.String,System.Byte[],System.Byte[],System.Net.Sockets.TransmitFileOptions,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendFile", True, "benchmark" },
    { 225, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendFile:System.IAsyncResult(System.String,System.Byte[],System.Byte[],System.Net.Sockets.TransmitFileOptions,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendFile", True, "benchmark" },
    { 226, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendFile:System.IAsyncResult(System.String,System.Byte[],System.Byte[],System.Net.Sockets.TransmitFileOptions,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendFile", True, "benchmark" },
    { 227, "System.Net.Sockets/System.Net.Sockets.Socket::EndSendFile:System.Void(System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndSendFile", True, "benchmark" },
    { 228, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendTo:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendTo", True, "benchmark" },
    { 229, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendTo:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendTo", True, "benchmark" },
    { 230, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendTo:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendTo", True, "benchmark" },
    { 231, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendTo:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendTo", True, "benchmark" },
    { 232, "System.Net.Sockets/System.Net.Sockets.Socket::BeginSendTo:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginSendTo", True, "benchmark" },
    { 233, "System.Net.Sockets/System.Net.Sockets.Socket::EndSendTo:System.Int32(System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndSendTo", True, "benchmark" },
    { 234, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 235, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 236, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 237, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 238, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 239, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 240, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 241, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 242, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 243, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 244, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 245, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 246, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 247, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceive:System.IAsyncResult(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags,System.Net.Sockets.SocketError,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceive", True, "benchmark" },
    { 248, "System.Net.Sockets/System.Net.Sockets.Socket::EndReceive:System.Int32(System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndReceive", True, "benchmark" },
    { 249, "System.Net.Sockets/System.Net.Sockets.Socket::EndReceive:System.Int32(System.IAsyncResult,System.Net.Sockets.SocketError)", "codegen", "System.Net.Sockets.Socket", "EndReceive", True, "benchmark" },
    { 250, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceiveMessageFrom:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceiveMessageFrom", True, "benchmark" },
    { 251, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceiveMessageFrom:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceiveMessageFrom", True, "benchmark" },
    { 252, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceiveMessageFrom:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceiveMessageFrom", True, "benchmark" },
    { 253, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceiveMessageFrom:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceiveMessageFrom", True, "benchmark" },
    { 254, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceiveMessageFrom:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceiveMessageFrom", True, "benchmark" },
    { 255, "System.Net.Sockets/System.Net.Sockets.Socket::EndReceiveMessageFrom:System.Int32(System.IAsyncResult,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.Net.Sockets.IPPacketInformation)", "codegen", "System.Net.Sockets.Socket", "EndReceiveMessageFrom", True, "benchmark" },
    { 256, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceiveFrom:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceiveFrom", True, "benchmark" },
    { 257, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceiveFrom:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceiveFrom", True, "benchmark" },
    { 258, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceiveFrom:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceiveFrom", True, "benchmark" },
    { 259, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceiveFrom:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceiveFrom", True, "benchmark" },
    { 260, "System.Net.Sockets/System.Net.Sockets.Socket::BeginReceiveFrom:System.IAsyncResult(System.Byte[],System.Int32,System.Int32,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginReceiveFrom", True, "benchmark" },
    { 261, "System.Net.Sockets/System.Net.Sockets.Socket::EndReceiveFrom:System.Int32(System.IAsyncResult,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "EndReceiveFrom", True, "benchmark" },
    { 262, "System.Net.Sockets/System.Net.Sockets.Socket::BeginAccept:System.IAsyncResult(System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginAccept", True, "benchmark" },
    { 263, "System.Net.Sockets/System.Net.Sockets.Socket::EndAccept:System.Net.Sockets.Socket(System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndAccept", True, "benchmark" },
    { 264, "System.Net.Sockets/System.Net.Sockets.Socket::BeginAccept:System.IAsyncResult(System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginAccept", True, "benchmark" },
    { 265, "System.Net.Sockets/System.Net.Sockets.Socket::BeginAccept:System.IAsyncResult(System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginAccept", True, "benchmark" },
    { 266, "System.Net.Sockets/System.Net.Sockets.Socket::BeginAccept:System.IAsyncResult(System.Net.Sockets.Socket,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginAccept", True, "benchmark" },
    { 267, "System.Net.Sockets/System.Net.Sockets.Socket::BeginAccept:System.IAsyncResult(System.Net.Sockets.Socket,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.Socket", "BeginAccept", True, "benchmark" },
    { 268, "System.Net.Sockets/System.Net.Sockets.Socket::EndAccept:System.Net.Sockets.Socket(System.Byte[],System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndAccept", True, "benchmark" },
    { 269, "System.Net.Sockets/System.Net.Sockets.Socket::EndAccept:System.Net.Sockets.Socket(System.Byte[],System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndAccept", True, "benchmark" },
    { 270, "System.Net.Sockets/System.Net.Sockets.Socket::EndAccept:System.Net.Sockets.Socket(System.Byte[],System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndAccept", True, "benchmark" },
    { 271, "System.Net.Sockets/System.Net.Sockets.Socket::EndAccept:System.Net.Sockets.Socket(System.Byte[],System.Int32,System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndAccept", True, "benchmark" },
    { 272, "System.Net.Sockets/System.Net.Sockets.Socket::EndAccept:System.Net.Sockets.Socket(System.Byte[],System.Int32,System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndAccept", True, "benchmark" },
    { 273, "System.Net.Sockets/System.Net.Sockets.Socket::EndAccept:System.Net.Sockets.Socket(System.Byte[],System.Int32,System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndAccept", True, "benchmark" },
    { 274, "System.Net.Sockets/System.Net.Sockets.Socket::EndAccept:System.Net.Sockets.Socket(System.Byte[],System.Int32,System.IAsyncResult)", "codegen", "System.Net.Sockets.Socket", "EndAccept", True, "benchmark" },
    { 275, "System.Net.Sockets/System.Net.Sockets.Socket::Shutdown:System.Void(System.Net.Sockets.SocketShutdown)", "codegen", "System.Net.Sockets.Socket", "Shutdown", True, "benchmark" },
    { 276, "System.Net.Sockets/System.Net.Sockets.Socket::AcceptAsync:System.Boolean(System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "AcceptAsync", True, "benchmark" },
    { 277, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Boolean(System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 278, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Boolean(System.Net.Sockets.SocketType,System.Net.Sockets.ProtocolType,System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 279, "System.Net.Sockets/System.Net.Sockets.Socket::CancelConnectAsync:System.Void(System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "CancelConnectAsync", True, "benchmark" },
    { 280, "System.Net.Sockets/System.Net.Sockets.Socket::DisconnectAsync:System.Boolean(System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "DisconnectAsync", True, "benchmark" },
    { 281, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveAsync:System.Boolean(System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "ReceiveAsync", True, "benchmark" },
    { 282, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFromAsync:System.Boolean(System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "ReceiveFromAsync", True, "benchmark" },
    { 283, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveMessageFromAsync:System.Boolean(System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "ReceiveMessageFromAsync", True, "benchmark" },
    { 284, "System.Net.Sockets/System.Net.Sockets.Socket::SendAsync:System.Boolean(System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "SendAsync", True, "benchmark" },
    { 285, "System.Net.Sockets/System.Net.Sockets.Socket::SendPacketsAsync:System.Boolean(System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "SendPacketsAsync", True, "benchmark" },
    { 286, "System.Net.Sockets/System.Net.Sockets.Socket::SendToAsync:System.Boolean(System.Net.Sockets.SocketAsyncEventArgs)", "codegen", "System.Net.Sockets.Socket", "SendToAsync", True, "benchmark" },
    { 287, "System.Net.Sockets/System.Net.Sockets.Socket::Dispose:System.Void()", "codegen", "System.Net.Sockets.Socket", "Dispose", True, "benchmark" },
    { 288, "System.Net.Sockets/System.Net.Sockets.Socket::AcceptAsync:System.Threading.Tasks.Task<System.Net.Sockets.Socket>()", "codegen", "System.Net.Sockets.Socket", "AcceptAsync", True, "benchmark" },
    { 289, "System.Net.Sockets/System.Net.Sockets.Socket::AcceptAsync:System.Threading.Tasks.ValueTask<System.Net.Sockets.Socket>(System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "AcceptAsync", True, "benchmark" },
    { 290, "System.Net.Sockets/System.Net.Sockets.Socket::AcceptAsync:System.Threading.Tasks.Task<System.Net.Sockets.Socket>(System.Net.Sockets.Socket)", "codegen", "System.Net.Sockets.Socket", "AcceptAsync", True, "benchmark" },
    { 291, "System.Net.Sockets/System.Net.Sockets.Socket::AcceptAsync:System.Threading.Tasks.ValueTask<System.Net.Sockets.Socket>(System.Net.Sockets.Socket,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "AcceptAsync", True, "benchmark" },
    { 292, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.Task(System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 293, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.EndPoint,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 294, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 295, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 296, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 297, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 298, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 299, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 300, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 301, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 302, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 303, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 304, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 305, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 306, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.Task(System.String,System.Int32)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 307, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.Task(System.String,System.Int32)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 308, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.Task(System.String,System.Int32)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 309, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.ValueTask(System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 310, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.ValueTask(System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 311, "System.Net.Sockets/System.Net.Sockets.Socket::ConnectAsync:System.Threading.Tasks.ValueTask(System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ConnectAsync", True, "benchmark" },
    { 312, "System.Net.Sockets/System.Net.Sockets.Socket::DisconnectAsync:System.Threading.Tasks.ValueTask(System.Boolean,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "DisconnectAsync", True, "benchmark" },
    { 313, "System.Net.Sockets/System.Net.Sockets.Socket::DisconnectAsync:System.Threading.Tasks.ValueTask(System.Boolean,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "DisconnectAsync", True, "benchmark" },
    { 314, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveAsync:System.Threading.Tasks.Task<System.Int32>(System.ArraySegment<System.Byte>)", "codegen", "System.Net.Sockets.Socket", "ReceiveAsync", True, "benchmark" },
    { 315, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveAsync:System.Threading.Tasks.Task<System.Int32>(System.ArraySegment<System.Byte>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "ReceiveAsync", True, "benchmark" },
    { 316, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Memory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ReceiveAsync", True, "benchmark" },
    { 317, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Memory<System.Byte>,System.Net.Sockets.SocketFlags,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ReceiveAsync", True, "benchmark" },
    { 318, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveAsync:System.Threading.Tasks.Task<System.Int32>(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>)", "codegen", "System.Net.Sockets.Socket", "ReceiveAsync", True, "benchmark" },
    { 319, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveAsync:System.Threading.Tasks.Task<System.Int32>(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>)", "codegen", "System.Net.Sockets.Socket", "ReceiveAsync", True, "benchmark" },
    { 320, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveAsync:System.Threading.Tasks.Task<System.Int32>(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "ReceiveAsync", True, "benchmark" },
    { 321, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveAsync:System.Threading.Tasks.Task<System.Int32>(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "ReceiveAsync", True, "benchmark" },
    { 322, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFromAsync:System.Threading.Tasks.Task<System.Net.Sockets.SocketReceiveFromResult>(System.ArraySegment<System.Byte>,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFromAsync", True, "benchmark" },
    { 323, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFromAsync:System.Threading.Tasks.Task<System.Net.Sockets.SocketReceiveFromResult>(System.ArraySegment<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveFromAsync", True, "benchmark" },
    { 324, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFromAsync:System.Threading.Tasks.ValueTask<System.Net.Sockets.SocketReceiveFromResult>(System.Memory<System.Byte>,System.Net.EndPoint,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ReceiveFromAsync", True, "benchmark" },
    { 325, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFromAsync:System.Threading.Tasks.ValueTask<System.Net.Sockets.SocketReceiveFromResult>(System.Memory<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ReceiveFromAsync", True, "benchmark" },
    { 326, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveFromAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Memory<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.SocketAddress,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ReceiveFromAsync", True, "benchmark" },
    { 327, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveMessageFromAsync:System.Threading.Tasks.Task<System.Net.Sockets.SocketReceiveMessageFromResult>(System.ArraySegment<System.Byte>,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveMessageFromAsync", True, "benchmark" },
    { 328, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveMessageFromAsync:System.Threading.Tasks.Task<System.Net.Sockets.SocketReceiveMessageFromResult>(System.ArraySegment<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "ReceiveMessageFromAsync", True, "benchmark" },
    { 329, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveMessageFromAsync:System.Threading.Tasks.ValueTask<System.Net.Sockets.SocketReceiveMessageFromResult>(System.Memory<System.Byte>,System.Net.EndPoint,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ReceiveMessageFromAsync", True, "benchmark" },
    { 330, "System.Net.Sockets/System.Net.Sockets.Socket::ReceiveMessageFromAsync:System.Threading.Tasks.ValueTask<System.Net.Sockets.SocketReceiveMessageFromResult>(System.Memory<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "ReceiveMessageFromAsync", True, "benchmark" },
    { 331, "System.Net.Sockets/System.Net.Sockets.Socket::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.ArraySegment<System.Byte>)", "codegen", "System.Net.Sockets.Socket", "SendAsync", True, "benchmark" },
    { 332, "System.Net.Sockets/System.Net.Sockets.Socket::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.ArraySegment<System.Byte>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "SendAsync", True, "benchmark" },
    { 333, "System.Net.Sockets/System.Net.Sockets.Socket::SendAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "SendAsync", True, "benchmark" },
    { 334, "System.Net.Sockets/System.Net.Sockets.Socket::SendAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.Net.Sockets.SocketFlags,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "SendAsync", True, "benchmark" },
    { 335, "System.Net.Sockets/System.Net.Sockets.Socket::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>)", "codegen", "System.Net.Sockets.Socket", "SendAsync", True, "benchmark" },
    { 336, "System.Net.Sockets/System.Net.Sockets.Socket::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>)", "codegen", "System.Net.Sockets.Socket", "SendAsync", True, "benchmark" },
    { 337, "System.Net.Sockets/System.Net.Sockets.Socket::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "SendAsync", True, "benchmark" },
    { 338, "System.Net.Sockets/System.Net.Sockets.Socket::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.Socket", "SendAsync", True, "benchmark" },
    { 339, "System.Net.Sockets/System.Net.Sockets.Socket::SendToAsync:System.Threading.Tasks.Task<System.Int32>(System.ArraySegment<System.Byte>,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendToAsync", True, "benchmark" },
    { 340, "System.Net.Sockets/System.Net.Sockets.Socket::SendToAsync:System.Threading.Tasks.Task<System.Int32>(System.ArraySegment<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.Socket", "SendToAsync", True, "benchmark" },
    { 341, "System.Net.Sockets/System.Net.Sockets.Socket::SendToAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.Net.EndPoint,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "SendToAsync", True, "benchmark" },
    { 342, "System.Net.Sockets/System.Net.Sockets.Socket::SendToAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.EndPoint,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "SendToAsync", True, "benchmark" },
    { 343, "System.Net.Sockets/System.Net.Sockets.Socket::SendToAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.SocketAddress,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "SendToAsync", True, "benchmark" },
    { 344, "System.Net.Sockets/System.Net.Sockets.Socket::SendFileAsync:System.Threading.Tasks.ValueTask(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "SendFileAsync", True, "benchmark" },
    { 345, "System.Net.Sockets/System.Net.Sockets.Socket::SendFileAsync:System.Threading.Tasks.ValueTask(System.String,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "SendFileAsync", True, "benchmark" },
    { 346, "System.Net.Sockets/System.Net.Sockets.Socket::SendFileAsync:System.Threading.Tasks.ValueTask(System.String,System.ReadOnlyMemory<System.Byte>,System.ReadOnlyMemory<System.Byte>,System.Net.Sockets.TransmitFileOptions,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "SendFileAsync", True, "benchmark" },
    { 347, "System.Net.Sockets/System.Net.Sockets.Socket::SendFileAsync:System.Threading.Tasks.ValueTask(System.String,System.ReadOnlyMemory<System.Byte>,System.ReadOnlyMemory<System.Byte>,System.Net.Sockets.TransmitFileOptions,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.Socket", "SendFileAsync", True, "benchmark" },
    { 348, "System.Net.Sockets/System.Net.Sockets.Socket::DuplicateAndClose:System.Net.Sockets.SocketInformation(System.Int32)", "codegen", "System.Net.Sockets.Socket", "DuplicateAndClose", True, "benchmark" },
    { 349, "System.Net.Sockets/System.Net.Sockets.Socket::DuplicateAndClose:System.Net.Sockets.SocketInformation(System.Int32)", "codegen", "System.Net.Sockets.Socket", "DuplicateAndClose", True, "benchmark" },
    { 350, "System.Net.Sockets/System.Net.Sockets.SocketAsyncEventArgs::SetBuffer:System.Void(System.Int32,System.Int32)", "codegen", "System.Net.Sockets.SocketAsyncEventArgs", "SetBuffer", True, "benchmark" },
    { 351, "System.Net.Sockets/System.Net.Sockets.SocketAsyncEventArgs::SetBuffer:System.Void(System.Int32,System.Int32)", "codegen", "System.Net.Sockets.SocketAsyncEventArgs", "SetBuffer", True, "benchmark" },
    { 352, "System.Net.Sockets/System.Net.Sockets.SocketAsyncEventArgs::SetBuffer:System.Void(System.Int32,System.Int32)", "codegen", "System.Net.Sockets.SocketAsyncEventArgs", "SetBuffer", True, "benchmark" },
    { 353, "System.Net.Sockets/System.Net.Sockets.SocketAsyncEventArgs::SetBuffer:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.SocketAsyncEventArgs", "SetBuffer", True, "benchmark" },
    { 354, "System.Net.Sockets/System.Net.Sockets.SocketAsyncEventArgs::SetBuffer:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.SocketAsyncEventArgs", "SetBuffer", True, "benchmark" },
    { 355, "System.Net.Sockets/System.Net.Sockets.SocketAsyncEventArgs::SetBuffer:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.SocketAsyncEventArgs", "SetBuffer", True, "benchmark" },
    { 356, "System.Net.Sockets/System.Net.Sockets.SocketAsyncEventArgs::SetBuffer:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.SocketAsyncEventArgs", "SetBuffer", True, "benchmark" },
    { 357, "System.Net.Sockets/System.Net.Sockets.SocketAsyncEventArgs::SetBuffer:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Net.Sockets.SocketAsyncEventArgs", "SetBuffer", True, "benchmark" },
    { 358, "System.Net.Sockets/System.Net.Sockets.SocketAsyncEventArgs::SetBuffer:System.Void(System.Memory<System.Byte>)", "codegen", "System.Net.Sockets.SocketAsyncEventArgs", "SetBuffer", True, "benchmark" },
    { 359, "System.Net.Sockets/System.Net.Sockets.SocketAsyncEventArgs::Dispose:System.Void()", "codegen", "System.Net.Sockets.SocketAsyncEventArgs", "Dispose", True, "benchmark" },
    { 360, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::AcceptAsync:System.Threading.Tasks.Task<System.Net.Sockets.Socket>(System.Net.Sockets.Socket)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "AcceptAsync", True, "benchmark" },
    { 361, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::AcceptAsync:System.Threading.Tasks.Task<System.Net.Sockets.Socket>(System.Net.Sockets.Socket,System.Net.Sockets.Socket)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "AcceptAsync", True, "benchmark" },
    { 362, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.Task(System.Net.Sockets.Socket,System.Net.EndPoint)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 363, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.Sockets.Socket,System.Net.EndPoint,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 364, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.Task(System.Net.Sockets.Socket,System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 365, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.Task(System.Net.Sockets.Socket,System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 366, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.Sockets.Socket,System.Net.IPAddress,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 367, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.Sockets.Socket,System.Net.IPAddress,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 368, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.Task(System.Net.Sockets.Socket,System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 369, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.Task(System.Net.Sockets.Socket,System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 370, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.Task(System.Net.Sockets.Socket,System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 371, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.Task(System.Net.Sockets.Socket,System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 372, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.Sockets.Socket,System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 373, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.Sockets.Socket,System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 374, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.Sockets.Socket,System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 375, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.Sockets.Socket,System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 376, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.Task(System.Net.Sockets.Socket,System.String,System.Int32)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 377, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.Task(System.Net.Sockets.Socket,System.String,System.Int32)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 378, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.Task(System.Net.Sockets.Socket,System.String,System.Int32)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 379, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.Sockets.Socket,System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 380, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.Sockets.Socket,System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 381, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.Sockets.Socket,System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ConnectAsync", True, "benchmark" },
    { 382, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ReceiveAsync:System.Threading.Tasks.Task<System.Int32>(System.Net.Sockets.Socket,System.ArraySegment<System.Byte>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ReceiveAsync", True, "benchmark" },
    { 383, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ReceiveAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Net.Sockets.Socket,System.Memory<System.Byte>,System.Net.Sockets.SocketFlags,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ReceiveAsync", True, "benchmark" },
    { 384, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ReceiveAsync:System.Threading.Tasks.Task<System.Int32>(System.Net.Sockets.Socket,System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ReceiveAsync", True, "benchmark" },
    { 385, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ReceiveAsync:System.Threading.Tasks.Task<System.Int32>(System.Net.Sockets.Socket,System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ReceiveAsync", True, "benchmark" },
    { 386, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ReceiveFromAsync:System.Threading.Tasks.Task<System.Net.Sockets.SocketReceiveFromResult>(System.Net.Sockets.Socket,System.ArraySegment<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ReceiveFromAsync", True, "benchmark" },
    { 387, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::ReceiveMessageFromAsync:System.Threading.Tasks.Task<System.Net.Sockets.SocketReceiveMessageFromResult>(System.Net.Sockets.Socket,System.ArraySegment<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "ReceiveMessageFromAsync", True, "benchmark" },
    { 388, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Net.Sockets.Socket,System.ArraySegment<System.Byte>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "SendAsync", True, "benchmark" },
    { 389, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::SendAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.Net.Sockets.Socket,System.ReadOnlyMemory<System.Byte>,System.Net.Sockets.SocketFlags,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "SendAsync", True, "benchmark" },
    { 390, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Net.Sockets.Socket,System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "SendAsync", True, "benchmark" },
    { 391, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Net.Sockets.Socket,System.Collections.Generic.IList<System.ArraySegment<System.Byte>>,System.Net.Sockets.SocketFlags)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "SendAsync", True, "benchmark" },
    { 392, "System.Net.Sockets/System.Net.Sockets.SocketTaskExtensions::SendToAsync:System.Threading.Tasks.Task<System.Int32>(System.Net.Sockets.Socket,System.ArraySegment<System.Byte>,System.Net.Sockets.SocketFlags,System.Net.EndPoint)", "codegen", "System.Net.Sockets.SocketTaskExtensions", "SendToAsync", True, "benchmark" },
    { 393, "System.Net.Sockets/System.Net.Sockets.TcpClient::Connect:System.Void(System.String,System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "Connect", True, "benchmark" },
    { 394, "System.Net.Sockets/System.Net.Sockets.TcpClient::Connect:System.Void(System.String,System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "Connect", True, "benchmark" },
    { 395, "System.Net.Sockets/System.Net.Sockets.TcpClient::Connect:System.Void(System.String,System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "Connect", True, "benchmark" },
    { 396, "System.Net.Sockets/System.Net.Sockets.TcpClient::Connect:System.Void(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "Connect", True, "benchmark" },
    { 397, "System.Net.Sockets/System.Net.Sockets.TcpClient::Connect:System.Void(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "Connect", True, "benchmark" },
    { 398, "System.Net.Sockets/System.Net.Sockets.TcpClient::Connect:System.Void(System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.TcpClient", "Connect", True, "benchmark" },
    { 399, "System.Net.Sockets/System.Net.Sockets.TcpClient::Connect:System.Void(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "Connect", True, "benchmark" },
    { 400, "System.Net.Sockets/System.Net.Sockets.TcpClient::Connect:System.Void(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "Connect", True, "benchmark" },
    { 401, "System.Net.Sockets/System.Net.Sockets.TcpClient::Connect:System.Void(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "Connect", True, "benchmark" },
    { 402, "System.Net.Sockets/System.Net.Sockets.TcpClient::Connect:System.Void(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "Connect", True, "benchmark" },
    { 403, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 404, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 405, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.Task(System.String,System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 406, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.Task(System.String,System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 407, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.Task(System.String,System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 408, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 409, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 410, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 411, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPAddress[],System.Int32)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 412, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.Task(System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 413, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 414, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 415, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.ValueTask(System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 416, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.ValueTask(System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 417, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.ValueTask(System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 418, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 419, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 420, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 421, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPAddress[],System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 422, "System.Net.Sockets/System.Net.Sockets.TcpClient::ConnectAsync:System.Threading.Tasks.ValueTask(System.Net.IPEndPoint,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpClient", "ConnectAsync", True, "benchmark" },
    { 423, "System.Net.Sockets/System.Net.Sockets.TcpClient::BeginConnect:System.IAsyncResult(System.Net.IPAddress,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpClient", "BeginConnect", True, "benchmark" },
    { 424, "System.Net.Sockets/System.Net.Sockets.TcpClient::BeginConnect:System.IAsyncResult(System.Net.IPAddress,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpClient", "BeginConnect", True, "benchmark" },
    { 425, "System.Net.Sockets/System.Net.Sockets.TcpClient::BeginConnect:System.IAsyncResult(System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpClient", "BeginConnect", True, "benchmark" },
    { 426, "System.Net.Sockets/System.Net.Sockets.TcpClient::BeginConnect:System.IAsyncResult(System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpClient", "BeginConnect", True, "benchmark" },
    { 427, "System.Net.Sockets/System.Net.Sockets.TcpClient::BeginConnect:System.IAsyncResult(System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpClient", "BeginConnect", True, "benchmark" },
    { 428, "System.Net.Sockets/System.Net.Sockets.TcpClient::BeginConnect:System.IAsyncResult(System.Net.IPAddress[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpClient", "BeginConnect", True, "benchmark" },
    { 429, "System.Net.Sockets/System.Net.Sockets.TcpClient::BeginConnect:System.IAsyncResult(System.Net.IPAddress[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpClient", "BeginConnect", True, "benchmark" },
    { 430, "System.Net.Sockets/System.Net.Sockets.TcpClient::BeginConnect:System.IAsyncResult(System.Net.IPAddress[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpClient", "BeginConnect", True, "benchmark" },
    { 431, "System.Net.Sockets/System.Net.Sockets.TcpClient::BeginConnect:System.IAsyncResult(System.Net.IPAddress[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpClient", "BeginConnect", True, "benchmark" },
    { 432, "System.Net.Sockets/System.Net.Sockets.TcpClient::EndConnect:System.Void(System.IAsyncResult)", "codegen", "System.Net.Sockets.TcpClient", "EndConnect", True, "benchmark" },
    { 433, "System.Net.Sockets/System.Net.Sockets.TcpClient::GetStream:System.Net.Sockets.NetworkStream()", "codegen", "System.Net.Sockets.TcpClient", "GetStream", True, "benchmark" },
    { 434, "System.Net.Sockets/System.Net.Sockets.TcpClient::Close:System.Void()", "codegen", "System.Net.Sockets.TcpClient", "Close", True, "benchmark" },
    { 435, "System.Net.Sockets/System.Net.Sockets.TcpClient::Dispose:System.Void()", "codegen", "System.Net.Sockets.TcpClient", "Dispose", True, "benchmark" },
    { 436, "System.Net.Sockets/System.Net.Sockets.TcpListener::AllowNatTraversal:System.Void(System.Boolean)", "codegen", "System.Net.Sockets.TcpListener", "AllowNatTraversal", True, "benchmark" },
    { 437, "System.Net.Sockets/System.Net.Sockets.TcpListener::AllowNatTraversal:System.Void(System.Boolean)", "codegen", "System.Net.Sockets.TcpListener", "AllowNatTraversal", True, "benchmark" },
    { 438, "System.Net.Sockets/System.Net.Sockets.TcpListener::Stop:System.Void()", "codegen", "System.Net.Sockets.TcpListener", "Stop", True, "benchmark" },
    { 439, "System.Net.Sockets/System.Net.Sockets.TcpListener::Dispose:System.Void()", "codegen", "System.Net.Sockets.TcpListener", "Dispose", True, "benchmark" },
    { 440, "System.Net.Sockets/System.Net.Sockets.TcpListener::Pending:System.Boolean()", "codegen", "System.Net.Sockets.TcpListener", "Pending", True, "benchmark" },
    { 441, "System.Net.Sockets/System.Net.Sockets.TcpListener::AcceptSocket:System.Net.Sockets.Socket()", "codegen", "System.Net.Sockets.TcpListener", "AcceptSocket", True, "benchmark" },
    { 442, "System.Net.Sockets/System.Net.Sockets.TcpListener::AcceptTcpClient:System.Net.Sockets.TcpClient()", "codegen", "System.Net.Sockets.TcpListener", "AcceptTcpClient", True, "benchmark" },
    { 443, "System.Net.Sockets/System.Net.Sockets.TcpListener::BeginAcceptSocket:System.IAsyncResult(System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpListener", "BeginAcceptSocket", True, "benchmark" },
    { 444, "System.Net.Sockets/System.Net.Sockets.TcpListener::EndAcceptSocket:System.Net.Sockets.Socket(System.IAsyncResult)", "codegen", "System.Net.Sockets.TcpListener", "EndAcceptSocket", True, "benchmark" },
    { 445, "System.Net.Sockets/System.Net.Sockets.TcpListener::BeginAcceptTcpClient:System.IAsyncResult(System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.TcpListener", "BeginAcceptTcpClient", True, "benchmark" },
    { 446, "System.Net.Sockets/System.Net.Sockets.TcpListener::EndAcceptTcpClient:System.Net.Sockets.TcpClient(System.IAsyncResult)", "codegen", "System.Net.Sockets.TcpListener", "EndAcceptTcpClient", True, "benchmark" },
    { 447, "System.Net.Sockets/System.Net.Sockets.TcpListener::AcceptSocketAsync:System.Threading.Tasks.Task<System.Net.Sockets.Socket>()", "codegen", "System.Net.Sockets.TcpListener", "AcceptSocketAsync", True, "benchmark" },
    { 448, "System.Net.Sockets/System.Net.Sockets.TcpListener::AcceptSocketAsync:System.Threading.Tasks.ValueTask<System.Net.Sockets.Socket>(System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpListener", "AcceptSocketAsync", True, "benchmark" },
    { 449, "System.Net.Sockets/System.Net.Sockets.TcpListener::AcceptTcpClientAsync:System.Threading.Tasks.Task<System.Net.Sockets.TcpClient>()", "codegen", "System.Net.Sockets.TcpListener", "AcceptTcpClientAsync", True, "benchmark" },
    { 450, "System.Net.Sockets/System.Net.Sockets.TcpListener::AcceptTcpClientAsync:System.Threading.Tasks.ValueTask<System.Net.Sockets.TcpClient>(System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.TcpListener", "AcceptTcpClientAsync", True, "benchmark" },
    { 451, "System.Net.Sockets/System.Net.Sockets.UdpClient::AllowNatTraversal:System.Void(System.Boolean)", "codegen", "System.Net.Sockets.UdpClient", "AllowNatTraversal", True, "benchmark" },
    { 452, "System.Net.Sockets/System.Net.Sockets.UdpClient::AllowNatTraversal:System.Void(System.Boolean)", "codegen", "System.Net.Sockets.UdpClient", "AllowNatTraversal", True, "benchmark" },
    { 453, "System.Net.Sockets/System.Net.Sockets.UdpClient::Dispose:System.Void()", "codegen", "System.Net.Sockets.UdpClient", "Dispose", True, "benchmark" },
    { 454, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 455, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 456, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 457, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 458, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 459, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 460, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 461, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 462, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.String,System.Int32,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 463, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Net.IPEndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 464, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Net.IPEndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 465, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Net.IPEndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 466, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginSend:System.IAsyncResult(System.Byte[],System.Int32,System.Net.IPEndPoint,System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginSend", True, "benchmark" },
    { 467, "System.Net.Sockets/System.Net.Sockets.UdpClient::EndSend:System.Int32(System.IAsyncResult)", "codegen", "System.Net.Sockets.UdpClient", "EndSend", True, "benchmark" },
    { 468, "System.Net.Sockets/System.Net.Sockets.UdpClient::BeginReceive:System.IAsyncResult(System.AsyncCallback,System.Object)", "codegen", "System.Net.Sockets.UdpClient", "BeginReceive", True, "benchmark" },
    { 469, "System.Net.Sockets/System.Net.Sockets.UdpClient::EndReceive:System.Byte[](System.IAsyncResult,System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "EndReceive", True, "benchmark" },
    { 470, "System.Net.Sockets/System.Net.Sockets.UdpClient::JoinMulticastGroup:System.Void(System.Net.IPAddress)", "codegen", "System.Net.Sockets.UdpClient", "JoinMulticastGroup", True, "benchmark" },
    { 471, "System.Net.Sockets/System.Net.Sockets.UdpClient::JoinMulticastGroup:System.Void(System.Net.IPAddress,System.Net.IPAddress)", "codegen", "System.Net.Sockets.UdpClient", "JoinMulticastGroup", True, "benchmark" },
    { 472, "System.Net.Sockets/System.Net.Sockets.UdpClient::JoinMulticastGroup:System.Void(System.Int32,System.Net.IPAddress)", "codegen", "System.Net.Sockets.UdpClient", "JoinMulticastGroup", True, "benchmark" },
    { 473, "System.Net.Sockets/System.Net.Sockets.UdpClient::JoinMulticastGroup:System.Void(System.Int32,System.Net.IPAddress)", "codegen", "System.Net.Sockets.UdpClient", "JoinMulticastGroup", True, "benchmark" },
    { 474, "System.Net.Sockets/System.Net.Sockets.UdpClient::JoinMulticastGroup:System.Void(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "JoinMulticastGroup", True, "benchmark" },
    { 475, "System.Net.Sockets/System.Net.Sockets.UdpClient::JoinMulticastGroup:System.Void(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "JoinMulticastGroup", True, "benchmark" },
    { 476, "System.Net.Sockets/System.Net.Sockets.UdpClient::DropMulticastGroup:System.Void(System.Net.IPAddress)", "codegen", "System.Net.Sockets.UdpClient", "DropMulticastGroup", True, "benchmark" },
    { 477, "System.Net.Sockets/System.Net.Sockets.UdpClient::DropMulticastGroup:System.Void(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "DropMulticastGroup", True, "benchmark" },
    { 478, "System.Net.Sockets/System.Net.Sockets.UdpClient::DropMulticastGroup:System.Void(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "DropMulticastGroup", True, "benchmark" },
    { 479, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 480, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 481, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 482, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 483, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 484, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 485, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 486, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 487, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 488, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 489, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 490, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 491, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.String,System.Int32,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 492, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 493, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 494, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 495, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 496, "System.Net.Sockets/System.Net.Sockets.UdpClient::SendAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.ReadOnlyMemory<System.Byte>,System.Net.IPEndPoint,System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.UdpClient", "SendAsync", True, "benchmark" },
    { 497, "System.Net.Sockets/System.Net.Sockets.UdpClient::ReceiveAsync:System.Threading.Tasks.Task<System.Net.Sockets.UdpReceiveResult>()", "codegen", "System.Net.Sockets.UdpClient", "ReceiveAsync", True, "benchmark" },
    { 498, "System.Net.Sockets/System.Net.Sockets.UdpClient::ReceiveAsync:System.Threading.Tasks.ValueTask<System.Net.Sockets.UdpReceiveResult>(System.Threading.CancellationToken)", "codegen", "System.Net.Sockets.UdpClient", "ReceiveAsync", True, "benchmark" },
    { 499, "System.Net.Sockets/System.Net.Sockets.UdpClient::Close:System.Void()", "codegen", "System.Net.Sockets.UdpClient", "Close", True, "benchmark" },
    { 500, "System.Net.Sockets/System.Net.Sockets.UdpClient::Connect:System.Void(System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Connect", True, "benchmark" },
    { 501, "System.Net.Sockets/System.Net.Sockets.UdpClient::Connect:System.Void(System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Connect", True, "benchmark" },
    { 502, "System.Net.Sockets/System.Net.Sockets.UdpClient::Connect:System.Void(System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Connect", True, "benchmark" },
    { 503, "System.Net.Sockets/System.Net.Sockets.UdpClient::Connect:System.Void(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Connect", True, "benchmark" },
    { 504, "System.Net.Sockets/System.Net.Sockets.UdpClient::Connect:System.Void(System.Net.IPAddress,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Connect", True, "benchmark" },
    { 505, "System.Net.Sockets/System.Net.Sockets.UdpClient::Connect:System.Void(System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "Connect", True, "benchmark" },
    { 506, "System.Net.Sockets/System.Net.Sockets.UdpClient::Receive:System.Byte[](System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "Receive", True, "benchmark" },
    { 507, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32,System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 508, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32,System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 509, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32,System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 510, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32,System.Net.IPEndPoint)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 511, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32,System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 512, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32,System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 513, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32,System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 514, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32,System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 515, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32,System.String,System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 516, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 517, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 518, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 519, "System.Net.Sockets/System.Net.Sockets.UdpClient::Send:System.Int32(System.Byte[],System.Int32)", "codegen", "System.Net.Sockets.UdpClient", "Send", True, "benchmark" },
    { 520, "System.Net.Sockets/System.Net.Sockets.UnixDomainSocketEndPoint::Serialize:System.Net.SocketAddress()", "codegen", "System.Net.Sockets.UnixDomainSocketEndPoint", "Serialize", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 521;

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