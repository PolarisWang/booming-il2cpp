#pragma once
#include <chaos/net/net.h>
#include <chaos/net/socket_handle.h>
#include <chaos/net/error.h>
#include <chaos/net/async_op.h>

// Layer D -- platform-neutral transport API (net_impl_win32 / net_impl_posix).
//
// Every function operates on a SocketHandle carrying a real native
// descriptor (handle.fd).  Sockets are created NON-BLOCKING; synchronous
// semantics (waits, timeouts) are implemented inside the impl so Layer B
// and the managed wrappers never see WSAEWOULDBLOCK.  All functions are
// noexcept and return NetError; on failure the fine-grained SocketError
// of the last operation is available via NetSocketLastError() (thread-local
// in the impl), which Layer B writes into the managed error slot per
// abi.md (synchronous semantics).
//
// Option values are plain INT32: boolean options are 0/1, timeout options
// are milliseconds, Linger is seconds (0 == disabled).
namespace chaos::net {

// Platform-neutral address families (winsock/posix share these numbers).
inline constexpr CHAOS_IL2CPP_INT32 kAddressFamilyInet   = 2;   // AF_INET
inline constexpr CHAOS_IL2CPP_INT32 kAddressFamilyInet6  = 23;  // AF_INET6
// Neutral socket types / protocols.
inline constexpr CHAOS_IL2CPP_INT32 kSocketTypeStream    = 1;   // SOCK_STREAM
inline constexpr CHAOS_IL2CPP_INT32 kSocketTypeDgram     = 2;   // SOCK_DGRAM
inline constexpr CHAOS_IL2CPP_INT32 kProtocolTcp         = 6;   // IPPROTO_TCP
inline constexpr CHAOS_IL2CPP_INT32 kProtocolUdp         = 17;  // IPPROTO_UDP

enum class NetOption : CHAOS_IL2CPP_INT32 {
    NoDelay = 0,         // TCP_NODELAY (stream)
    ReuseAddress,        // SO_REUSEADDR
    KeepAlive,           // SO_KEEPALIVE
    IPv6Only,            // IPV6_V6ONLY (v6 only)
    ReceiveTimeout,      // SO_RCVTIMEO (ms)
    SendTimeout,         // SO_SNDTIMEO (ms)
    EnableBroadcast,     // SO_BROADCAST (dgram)
    Linger,              // SO_LINGER (seconds; 0 == disabled)
};

// Address carrier: IPv4 (kAddressFamilyInet, first 4 addr bytes) or IPv6
// (kAddressFamilyInet6, full 16 addr bytes).  Port is always host order.
struct NetAddress {
    CHAOS_IL2CPP_INT32 family;    // kAddressFamilyInet
    CHAOS_IL2CPP_UINT16 port;     // host byte order
    CHAOS_IL2CPP_UINT8 addr[16];  // first 4 bytes = IPv4 (127.0.0.1)
};

inline NetAddress LoopbackV4(CHAOS_IL2CPP_UINT16 port) noexcept
{
    NetAddress a{};
    a.family = kAddressFamilyInet;
    a.port = port;
    a.addr[0] = 127; a.addr[1] = 0; a.addr[2] = 0; a.addr[3] = 1;
    return a;
}

// IPv6 loopback ::1 (NT-17 multi-address-family).
inline NetAddress LoopbackV6(CHAOS_IL2CPP_UINT16 port) noexcept
{
    NetAddress a{};
    a.family = kAddressFamilyInet6;
    a.port = port;
    a.addr[15] = 1;  // ::1
    return a;
}

// Brings the transport up (WSAStartup refcount on Windows, no-op posix).
NetError NetStartup() noexcept;
void NetCleanup() noexcept;

// Creates a non-blocking transport socket and binds it to handle->fd.
// Leaves handle state at Created (state transitions belong to Layer B).
NetError NetSocketCreate(CHAOS_IL2CPP_INT32 family, CHAOS_IL2CPP_INT32 type,
                         CHAOS_IL2CPP_INT32 protocol, SocketHandle* handle) noexcept;
NetError NetSocketClose(SocketHandle* handle) noexcept;

NetError NetSocketBind(SocketHandle* handle, const NetAddress& addr) noexcept;
NetError NetSocketListen(SocketHandle* handle, CHAOS_IL2CPP_INT32 backlog) noexcept;
// Accepts one connection into peer (a fresh handle the caller owns); fills
// peer_addr when non-null.  timeout_ms <= 0 waits indefinitely.
NetError NetSocketAccept(SocketHandle* handle, SocketHandle* peer,
                         NetAddress* peer_addr, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;
// Connects; timeout_ms <= 0 waits indefinitely.
NetError NetSocketConnect(SocketHandle* handle, const NetAddress& addr,
                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept;


// ── NT-23: Happy-Eyeballs multi-address connect (RFC 8305 simplified) ──
// Ordered connect list: tries addrs[0..count) in order.  Each attempt
// creates a fresh stream socket of the address family and connects with
// a per-attempt timeout; the first success wins and is returned through
// *out (a Connected socket the caller owns and closes).  When
// useHappyEyeballs is set the per-attempt timeout is clamped to
// kHappyEyeballAttemptMs so a dead first family falls through quickly;
// when clear only addrs[0] is tried (legacy single-address behavior) with
// the full connectTimeoutMs budget.  connectTimeoutMs <= 0 means 10000.
struct NetConnectOptions {
    CHAOS_IL2CPP_INT32 connectTimeoutMs;   // per-attempt; <=0 → 10000
    CHAOS_IL2CPP_INT32 useHappyEyeballs;   // 1: clamp + fall through
};
inline constexpr CHAOS_IL2CPP_INT32 kHappyEyeballAttemptMs = 250;

// Tries addrs[0..count) per NetConnectOptions.  On total failure returns
// the last per-attempt error and leaves *out.fd == -1; count <= 0 or
// null out/addrs is Unsupported.
NetError NetSocketConnectList(SocketHandle* out, const NetAddress* addrs,
                              CHAOS_IL2CPP_INT32 count,
                              const NetConnectOptions& opts) noexcept;
// Send/recv the given buffer (len bytes). *sent/*got receives the byte
// count (recv: 0 means peer closed).  timeout_ms <= 0 waits indefinitely.
NetError NetSocketSend(SocketHandle* handle, const CHAOS_IL2CPP_UINT8* data,
                       CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                       CHAOS_IL2CPP_INT32* sent, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;
NetError NetSocketRecv(SocketHandle* handle, CHAOS_IL2CPP_UINT8* data,
                       CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                       CHAOS_IL2CPP_INT32* got, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

NetError NetSocketSetOption(SocketHandle* handle, NetOption opt,
                            CHAOS_IL2CPP_INT32 value) noexcept;
NetError NetSocketGetOption(SocketHandle* handle, NetOption opt,
                            CHAOS_IL2CPP_INT32* value) noexcept;

// Getsockname wrapper (needed to learn the ephemeral port after bind(0)).
NetError NetSocketGetLocalAddress(SocketHandle* handle, NetAddress* addr) noexcept;

// Fine-grained SocketError of the last failed op (thread-local in impl).
SocketError NetSocketLastError() noexcept;

// ── NT-7: async (overlapped/IOCP) submission ─────────────────────────
// Socket must have been created with kSocketTypeStream/kSocketTypeDgram.
// The AsyncOp stays owned by the caller and completes through on_done
// (Layer G).  The buffer must remain valid until completion.  Single op
// per socket is enforced by Layer C, not here.  On failure of the actual
// submission the op still completes through on_done with an error state.
NetError NetSocketBeginSend(SocketHandle* handle, const CHAOS_IL2CPP_UINT8* data,
                            CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                            async::AsyncOp* op) noexcept;
NetError NetSocketBeginRecv(SocketHandle* handle, CHAOS_IL2CPP_UINT8* data,
                            CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                            async::AsyncOp* op) noexcept;
// Cancels the overlapped operation backing op (CancelIoEx).  The op still
// completes through on_done with the recorded reason as error.
NetError NetSocketCancelOp(SocketHandle* handle, async::AsyncOp* op,
                           NetError reason) noexcept;

}  // namespace chaos::net