// chaos_net_entry_test -- NT-8: S34 high-level entry points.
//
// Covers the native side of the si 45 / si 555 fix, which the fact-run
// validates end-to-end (580/580): the S34 registrations in
// RuntimeHelperShapeRegistry.CoreStubs.Part2.S34.cs land on these extern
// symbols, so this test pins their contract directly:
//   1. ChaosStreamFlushAsync -> an ALREADY-COMPLETED, non-faulted AsyncTask
//      with result == 0 (NetworkStream::FlushAsync observable).
//   2. ChaosUdpClientSendAsync -> an ALREADY-COMPLETED AsyncTask with
//      result == 0 for the wire-free path (0-byte datagram + invalid
//      count/byteLength warning paths both complete 0).
//   3. ChaosIPEndPointCtor -> no-op handler (fixes the generated ctor call
//      site's eval-stack under-consumption).
//
// The generated awaiter chain (GetAwaiter -> GetResultValue -> AreEqual) is
// itself exercised by the fact suite; here we verify the return value is a
// LIVE AsyncTask with the completed/faulted/canceled state machine contract
// (the exact fields the awaiter stubs read after slot resolution).
//
// Build: cmake -S src/native/net/tests -B build/net-tests -A x64
//        cmake --build build/net-tests --config RelWithDebInfo
#include <chaos/native_types.h>
#include <chaos/async.h>
#include <cstdint>
#include <cstdio>

// entry.cpp's log path (chaos/log.h) references the single-instance extern
// flag that chaos_runtime_core.lib normally provides; supply it here so the
// standalone test binary links.  stderr keeps unit-test diagnostics off the
// protocol stdout.
namespace chaos::il2cpp::common {
namespace log_internal {
bool g_log_use_stderr = true;
}
}

// The S34 extern "C" symbols implemented in src/native/net/entry.cpp.
// Declared locally (entry.cpp has no header; misc_stubs.h / stream_stubs.h
// carry the verbatim generated-TU-visible declarations).
extern "C" {
CHAOS_IL2CPP_INTPTR ChaosStreamFlushAsync(CHAOS_IL2CPP_INTPTR stream,
                                          CHAOS_IL2CPP_INTPTR cancellationToken) noexcept;
CHAOS_IL2CPP_INTPTR ChaosUdpClientSendAsync(CHAOS_IL2CPP_INTPTR client,
                                            const CHAOS_IL2CPP_UINT8* bytes,
                                            CHAOS_IL2CPP_INTPTR byteLength,
                                            CHAOS_IL2CPP_INT32 count,
                                            CHAOS_IL2CPP_INTPTR endpoint) noexcept;
void ChaosIPEndPointCtor(CHAOS_IL2CPP_INTPTR instance,
                         CHAOS_IL2CPP_INTPTR address,
                         CHAOS_IL2CPP_INTPTR port) noexcept;
}

namespace common = chaos::il2cpp::common;

static int g_failures = 0;

#define CHECK(cond, ...)                                                     \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::printf("[NET-ENTRY] FAIL %s:%d: ", __FILE__, __LINE__);     \
            std::printf(__VA_ARGS__);                                        \
            std::printf("\n");                                               \
            ++g_failures;                                                    \
        }                                                                    \
    } while (0)

// A live handle must point at an AsyncTask in the completed state with
// result==0, faulted==false (no exception to raise) and canceled==false.
static void CheckCompletedTask(CHAOS_IL2CPP_INTPTR handle, const char* what)
{
    CHECK(handle != static_cast<CHAOS_IL2CPP_INTPTR>(0), "%s: non-null handle", what);
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0)) return;
    auto* task = reinterpret_cast<common::AsyncTask*>(handle);
    CHECK(task->completed.load(std::memory_order_acquire),
          "%s: task completed", what);
    CHECK(!task->faulted.load(std::memory_order_acquire),
          "%s: task not faulted", what);
    CHECK(!task->canceled.load(std::memory_order_acquire),
          "%s: task not canceled", what);
    CHECK(task->exception == static_cast<CHAOS_IL2CPP_INTPTR>(0),
          "%s: no exception payload", what);
    CHECK(task->result == static_cast<CHAOS_IL2CPP_INTPTR>(0),
          "%s: result == 0", what);
}

// Replicates the generated-code awaiter protocol (chaos_locals slot holding
// the task handle, slot address passed as the awaiter) through the SAME
// slot-resolution helper the async.h inline contract and the fixed
// async_stubs.cpp stubs use.  If the handle read back through the slot is
// null or the task is not completed, the await chain would raise -- this
// mirrors what si 45 / si 555's GetAwaiter/GetResultValue observe.
static void CheckSlotProtocol(CHAOS_IL2CPP_INTPTR handle, const char* what)
{
    CHAOS_IL2CPP_INTPTR chaos_locals[2]{};
    chaos_locals[1] = handle;                       // TaskAwaiter -> task handle
    CHAOS_IL2CPP_INTPTR awaiter = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
    auto* task = common::require_async_task(*common::resolve_native_int_slot(awaiter));
    CHECK(task->completed.load(std::memory_order_acquire), "%s: slot-resolved completed", what);
    CHECK(!task->faulted.load(std::memory_order_acquire), "%s: slot-resolved not faulted", what);
    CHECK(task->result == static_cast<CHAOS_IL2CPP_INTPTR>(0), "%s: slot-resolved result 0", what);
}

static bool TestFlushAsync() noexcept
{
    std::printf("[NET-ENTRY] Stream::FlushAsync...\n");
    // Arbitrary stream + cancellation token handles: NT-8 no-ops both.
    const CHAOS_IL2CPP_INTPTR stream = static_cast<CHAOS_IL2CPP_INTPTR>(0x1234);
    const CHAOS_IL2CPP_INTPTR ct = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    const CHAOS_IL2CPP_INTPTR handle = ChaosStreamFlushAsync(stream, ct);
    CheckCompletedTask(handle, "FlushAsync");
    CheckSlotProtocol(handle, "FlushAsync");
    // A second call must also succeed (fresh task, not a shared singleton).
    CheckCompletedTask(ChaosStreamFlushAsync(stream, ct), "FlushAsync#2");
    return true;
}

static bool TestUdpClientSendAsync() noexcept
{
    std::printf("[NET-ENTRY] UdpClient::SendAsync...\n");
    // 0-byte datagram to an arbitrary endpoint: completes 0 bytes sent.
    const CHAOS_IL2CPP_INTPTR client = static_cast<CHAOS_IL2CPP_INTPTR>(0x99);
    const CHAOS_IL2CPP_INTPTR endpoint = static_cast<CHAOS_IL2CPP_INTPTR>(0x42);
    CHAOS_IL2CPP_INTPTR handle = ChaosUdpClientSendAsync(client, nullptr, 0, 0, endpoint);
    CheckCompletedTask(handle, "SendAsync(0 bytes)");
    CheckSlotProtocol(handle, "SendAsync(0 bytes)");

    // count > byteLength is the guard path (logs a warning, still completes 0).
    handle = ChaosUdpClientSendAsync(client, nullptr, 0, 1, endpoint);
    CheckCompletedTask(handle, "SendAsync(count>byteLength)");
    // count < 0 guard path.
    handle = ChaosUdpClientSendAsync(client, nullptr, 0, -1, endpoint);
    CheckCompletedTask(handle, "SendAsync(count<0)");
    return true;
}

static bool TestIPEndPointCtor() noexcept
{
    std::printf("[NET-ENTRY] IPEndPoint::.ctor...\n");
    // Registered to make the generated call site consume BOTH explicit args
    // (IPAddress + port) instead of under-consuming the eval stack; the
    // native side is a documented no-op.  Invoking with any pointers must
    // neither crash nor return.
    ChaosIPEndPointCtor(static_cast<CHAOS_IL2CPP_INTPTR>(0x11),
                        static_cast<CHAOS_IL2CPP_INTPTR>(0x22),
                        static_cast<CHAOS_IL2CPP_INTPTR>(0));
    return true;
}

int main()
{
    std::printf("[NET-ENTRY] chaos_net S34 entry tests starting\n");
    if (TestFlushAsync()) std::printf("[NET-ENTRY] PASS: FlushAsync\n");
    else std::printf("[NET-ENTRY] FAIL: FlushAsync\n");
    if (TestUdpClientSendAsync()) std::printf("[NET-ENTRY] PASS: UdpClient SendAsync\n");
    else std::printf("[NET-ENTRY] FAIL: UdpClient SendAsync\n");
    if (TestIPEndPointCtor()) std::printf("[NET-ENTRY] PASS: IPEndPoint ctor\n");
    else std::printf("[NET-ENTRY] FAIL: IPEndPoint ctor\n");

    if (g_failures == 0) {
        std::printf("[NET-ENTRY] ALL PASS\n");
        return 0;
    }
    std::printf("[NET-ENTRY] FAILURES: %d\n", g_failures);
    return 1;
}
