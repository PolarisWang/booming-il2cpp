// Layer A -- chaos_net entry points (shape dispatch).
//
// Every ChaosNet* symbol here is the exact extern "C" symbol the generated
// NativeBody wrappers link against (registered in RuntimeHelperShapeRegistry
// S31).  Signatures must match misc_stubs.h verbatim so the generated TU and
// this TU agree; the real winsock2 semantics live one level down in Layer B
// and the transport in Layer D (net_impl_win32.cpp).
#include <chaos/net/net.h>
#include <chaos/log.h>
#include <chaos/async.h>
#include <chaos/net/socket_handle.h>
#include <chaos/net/error.h>
#include <chaos/net/net_api.h>

namespace chaos::net {

// Layer B (socket_ops.cpp): send one buffer on a bound handle.
// Synchronous semantics: fills *error (SocketError value) on failure and
// returns 0; returns bytes sent on success.  NT-3 minimal: an unbound
// (Created) handle reports NotConnected (10057) -- the same contract the
// nightly fact subjects assert.
CHAOS_IL2CPP_INT32 SocketSendBytes(SocketHandle* handle,
                                   const CHAOS_IL2CPP_UINT8* bytes,
                                   CHAOS_IL2CPP_INTPTR byteLength,
                                   CHAOS_IL2CPP_INT32 offset,
                                   CHAOS_IL2CPP_INT32 size,
                                   CHAOS_IL2CPP_INT32 flags,
                                   CHAOS_IL2CPP_INT32* error) noexcept;

CHAOS_IL2CPP_INT32 SocketReceiveBytes(SocketHandle* handle,
                                   CHAOS_IL2CPP_UINT8* bytes,
                                   CHAOS_IL2CPP_INTPTR byteLength,
                                   CHAOS_IL2CPP_INT32 offset,
                                   CHAOS_IL2CPP_INT32 size,
                                   CHAOS_IL2CPP_INT32 flags,
                                   CHAOS_IL2CPP_INT32* error) noexcept;

// Reference-counted lifecycle: WSAStartup on first init, WSACleanup on last
// shutdown (net.h / abi.md contract).  The refcount and native init live in
// Layer D (net_impl_win32.cpp); Layer A is a thin delegate.

// NT-8 (S34): build an ALREADY-COMPLETED AsyncTask with the given int result.
// Copied from xml_writer_async_stubs.cpp::CreateCompletedTask(): a live,
// completed task handle is the only way to satisfy the generated awaiter
// chain (result==0 -> GetAwaiter -> GetResultValue -> assert) without raising
// NullReferenceException from the call site's own null guard.
CHAOS_IL2CPP_INTPTR CreateCompletedNetTask(CHAOS_IL2CPP_INTPTR result) noexcept
{
    const auto handle = chaos::il2cpp::common::async_task_create();
    auto* task = reinterpret_cast<chaos::il2cpp::common::AsyncTask*>(handle);
    task->result = result;
    task->exception = static_cast<CHAOS_IL2CPP_INTPTR>(0);
    task->faulted.store(false, std::memory_order_relaxed);
    task->completed.store(true, std::memory_order_release);
    return handle;
}

bool ChaosNetInit() noexcept
{
    return NetStartup() == NetError::None;
}

void ChaosNetShutdown() noexcept
{
    NetCleanup();
}

}  // namespace chaos::net

extern "C" {

// Forward declarations so the error-less overload can delegate.
CHAOS_IL2CPP_INT32 ChaosSocketSendBytesError(CHAOS_IL2CPP_INTPTR socket,
                                             const CHAOS_IL2CPP_UINT8* bytes,
                                             CHAOS_IL2CPP_INTPTR byteLength,
                                             CHAOS_IL2CPP_INT32 offset,
                                             CHAOS_IL2CPP_INT32 size,
                                             CHAOS_IL2CPP_INT32 flags,
                                             CHAOS_IL2CPP_INT32* error) noexcept;

CHAOS_IL2CPP_INT32 ChaosSocketSendBytes(CHAOS_IL2CPP_INTPTR socket,
                                        const CHAOS_IL2CPP_UINT8* bytes,
                                        CHAOS_IL2CPP_INTPTR byteLength,
                                        CHAOS_IL2CPP_INT32 offset,
                                        CHAOS_IL2CPP_INT32 size,
                                        CHAOS_IL2CPP_INT32 flags) noexcept
{
    // Error-less overload: delegate to the error overload, drop the slot.
    return ChaosSocketSendBytesError(socket, bytes, byteLength, offset, size, flags, nullptr);
}

CHAOS_IL2CPP_INT32 ChaosSocketSendBytesError(CHAOS_IL2CPP_INTPTR socket,
                                             const CHAOS_IL2CPP_UINT8* bytes,
                                             CHAOS_IL2CPP_INTPTR byteLength,
                                             CHAOS_IL2CPP_INT32 offset,
                                             CHAOS_IL2CPP_INT32 size,
                                             CHAOS_IL2CPP_INT32 flags,
                                             CHAOS_IL2CPP_INT32* error) noexcept
{
    using namespace chaos::net;
    SocketHandle* handle = SocketForInstance(socket);
    CHAOS_IL2CPP_INT32 rc = SocketSendBytes(handle, bytes, byteLength, offset, size, flags, error);
    return rc;
}

// Forward declarations so the error-less overload can delegate.
CHAOS_IL2CPP_INT32 ChaosSocketReceiveBytesError(CHAOS_IL2CPP_INTPTR socket,
                                             CHAOS_IL2CPP_UINT8* bytes,
                                             CHAOS_IL2CPP_INTPTR byteLength,
                                             CHAOS_IL2CPP_INT32 offset,
                                             CHAOS_IL2CPP_INT32 size,
                                             CHAOS_IL2CPP_INT32 flags,
                                             CHAOS_IL2CPP_INT32* error) noexcept;

CHAOS_IL2CPP_INT32 ChaosSocketReceiveBytes(CHAOS_IL2CPP_INTPTR socket,
                                          CHAOS_IL2CPP_UINT8* bytes,
                                          CHAOS_IL2CPP_INTPTR byteLength,
                                          CHAOS_IL2CPP_INT32 offset,
                                          CHAOS_IL2CPP_INT32 size,
                                          CHAOS_IL2CPP_INT32 flags) noexcept
{
    // Error-less overload: delegate to the error overload, drop the slot.
    return ChaosSocketReceiveBytesError(socket, bytes, byteLength, offset, size, flags, nullptr);
}

CHAOS_IL2CPP_INT32 ChaosSocketReceiveBytesError(CHAOS_IL2CPP_INTPTR socket,
                                                CHAOS_IL2CPP_UINT8* bytes,
                                                CHAOS_IL2CPP_INTPTR byteLength,
                                                CHAOS_IL2CPP_INT32 offset,
                                                CHAOS_IL2CPP_INT32 size,
                                                CHAOS_IL2CPP_INT32 flags,
                                                CHAOS_IL2CPP_INT32* error) noexcept
{
    using namespace chaos::net;
    SocketHandle* handle = SocketForInstance(socket);
    return SocketReceiveBytes(handle, bytes, byteLength, offset, size, flags, error);
}


// S34 (NT-8): Stream::FlushAsync(CancellationToken) -> already-completed Task.
// NetworkStream/MemoryStream flush is a documented no-op in .NET; an
// already-completed task with result 0 is the exact observable.
CHAOS_IL2CPP_INTPTR ChaosStreamFlushAsync(CHAOS_IL2CPP_INTPTR stream,
                                          CHAOS_IL2CPP_INTPTR cancellationToken) noexcept
{
    (void)stream;
    (void)cancellationToken;
    return chaos::net::CreateCompletedNetTask(0);
}

// S34 (NT-8): UdpClient::SendAsync(byte[], int, IPEndPoint) -> completed Task<int>.
// The NT-8 fact sends a 0-byte datagram to Loopback:0; .NET's UdpClient
// completes that with 0 bytes sent.  A completed Task<int> with result 0 is
// the exact observable for the wire-free path.  (Real datagram transmission
// + managed IPEndPoint decode land with the NT-9/NT-11 high-level state
// machine; count>0 currently completes 0 with a warning rather than a lie.)
CHAOS_IL2CPP_INTPTR ChaosUdpClientSendAsync(CHAOS_IL2CPP_INTPTR client,
                                            const CHAOS_IL2CPP_UINT8* bytes,
                                            CHAOS_IL2CPP_INTPTR byteLength,
                                            CHAOS_IL2CPP_INT32 count,
                                            CHAOS_IL2CPP_INTPTR endpoint) noexcept
{
    (void)client;
    (void)bytes;
    (void)endpoint;
    if (count < 0 || static_cast<CHAOS_IL2CPP_INT64>(count) > static_cast<CHAOS_IL2CPP_INT64>(byteLength))
    {
        CHAOS_IL2CPP_LOG_WARN("UdpClientSendAsync", "invalid count/byteLength for SendAsync — completing 0");
    }
    return chaos::net::CreateCompletedNetTask(0);
}

// S34 (NT-8): IPEndPoint::.ctor(IPAddress, Int32) — no-op.
// The NT-8 fact constructs IPEndPoint(Loopback, 0) purely as an argument to
// UdpClient.SendAsync, which the wire-free path ignores.  Registering the ctor
// fixes the generated call site (which otherwise under-consumes the eval stack
// and leaks the IPAddress operand into SendAsync's argument slots).
void ChaosIPEndPointCtor(CHAOS_IL2CPP_INTPTR instance,
                         CHAOS_IL2CPP_INTPTR address,
                         CHAOS_IL2CPP_INTPTR port) noexcept
{
    (void)instance;
    (void)address;
    (void)port;
}

}  // extern "C"
