// Layer D -- winsock2 transport implementation for net_api.h.
//
// NT-6: real non-blocking sockets behind the platform-neutral net_api
// surface.  Every socket is created non-blocking (FIONBIO); synchronous
// semantics (waits, timeouts) are implemented here with WSAPoll so Layer B
// never observes WSAEWOULDBLOCK.  Failure mapping: each failed op stores the
// last native (WSA) code in a thread-local; NetSocketLastError() exposes the
// fine-grained SocketError for the managed error slot (abi.md / error.cpp).
#include <chaos/net/net_api.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstring>
#include <thread>
#include <atomic>

namespace chaos::net {
namespace {

thread_local CHAOS_IL2CPP_INT32 tls_last_native = 0;

void SetLastNative(CHAOS_IL2CPP_INT32 native) noexcept { tls_last_native = native; }

// Coarse NetError from the current tls_last_native.  The fine-grained code
// stays available via NetSocketLastError().
NetError CoarseError() noexcept
{
    switch (tls_last_native) {
        case WSAENOTCONN:  return NetError::NotConnected;
        case WSAETIMEDOUT: return NetError::TimedOut;
        case WSAEWOULDBLOCK: return NetError::WouldBlock;
        case WSAECONNRESET:
        case WSAECONNABORTED: return NetError::ConnectionReset;
        default: return NetError::Unsupported;
    }
}

// Fills sockaddr_storage (sockaddr_in for IPv4, sockaddr_in6 for IPv6);
// returns the sockaddr length (16 or 28) or 0 when the family is unsupported.
int ToSockAddr(const NetAddress& in, sockaddr_storage* out) noexcept
{
    if (in.family == kAddressFamilyInet) {
        sockaddr_in* sa = reinterpret_cast<sockaddr_in*>(out);
        std::memset(sa, 0, sizeof(*sa));
        sa->sin_family = AF_INET;
        sa->sin_port = htons(in.port);
        std::memcpy(&sa->sin_addr.s_addr, in.addr, 4);
        return static_cast<int>(sizeof(sockaddr_in));
    }
    if (in.family == kAddressFamilyInet6) {
        sockaddr_in6* sa6 = reinterpret_cast<sockaddr_in6*>(out);
        std::memset(sa6, 0, sizeof(*sa6));
        sa6->sin6_family = AF_INET6;
        sa6->sin6_port = htons(in.port);
        std::memcpy(&sa6->sin6_addr, in.addr, 16);
        return static_cast<int>(sizeof(sockaddr_in6));
    }
    return 0;
}

// Fills NetAddress from a sockaddr_in / sockaddr_in6 (length-aware).
// IPv4-mapped IPv6 (::ffff:a.b.c.d) is unmapped back to kAddressFamilyInet.
void FillNetAddress(const sockaddr* base, int length, NetAddress* out) noexcept
{
    if (base->sa_family == AF_INET && length >= static_cast<int>(sizeof(sockaddr_in))) {
        const sockaddr_in* sa = reinterpret_cast<const sockaddr_in*>(base);
        out->family = kAddressFamilyInet;
        out->port = ntohs(sa->sin_port);
        std::memset(out->addr, 0, 16);
        std::memcpy(out->addr, &sa->sin_addr.s_addr, 4);
        return;
    }
    if (base->sa_family == AF_INET6 && length >= static_cast<int>(sizeof(sockaddr_in6))) {
        const sockaddr_in6* sa6 = reinterpret_cast<const sockaddr_in6*>(base);
        const CHAOS_IL2CPP_UINT8* p = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(&sa6->sin6_addr);
        static const CHAOS_IL2CPP_UINT8 kMapped[12] = {0,0,0,0,0,0,0,0,0,0,0xFF,0xFF};
        if (std::memcmp(p, kMapped, 12) == 0) {
            out->family = kAddressFamilyInet;  // ::ffff:a.b.c.d -> IPv4
            out->port = ntohs(sa6->sin6_port);
            std::memset(out->addr, 0, 16);
            std::memcpy(out->addr, p + 12, 4);
            return;
        }
        out->family = kAddressFamilyInet6;
        out->port = ntohs(sa6->sin6_port);
        std::memset(out->addr, 0, 16);
        std::memcpy(out->addr, p, 16);
        return;
    }
    out->family = 0;
    out->port = 0;
    std::memset(out->addr, 0, 16);
}

// Wait up to timeout_ms (<= 0 == infinite) for events on the socket.
// Returns the poll result: >0 ready, 0 timed out (last=TimedOut), -1 error.
CHAOS_IL2CPP_INT32 PollWait(SOCKET s, short events, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    WSAPOLLFD pfd{};
    pfd.fd = s;
    pfd.events = events;
    const int rc = WSAPoll(&pfd, 1, timeout_ms <= 0 ? -1 : timeout_ms);
    if (rc == SOCKET_ERROR) {
        SetLastNative(WSAGetLastError());
        return -1;
    }
    if (rc == 0) {
        SetLastNative(WSAETIMEDOUT);
        return 0;
    }
    return pfd.revents;
}

bool SetNonBlocking(SOCKET s) noexcept
{
    u_long mode = 1;
    return ioctlsocket(s, FIONBIO, &mode) == 0;
}

}  // namespace

// ── NT-7: IOCP async transport ────────────────────────────────────────
// One completion port shared by every socket (CreateIoCompletionPort
// association happens in NetSocketCreate).  A single worker dequeues
// completions and routes them through Layer G (async::PostCompletion),
// so on_done never runs on Layer C's submission thread.  The completion
// port and worker live for the lifetime of NetStartup..NetCleanup.
namespace {

// OVERLAPPED must be the first member so the worker can CONTAINING_RECORD
// back to the owning AsyncOp.
struct IocpOpContext {
    OVERLAPPED overlapped{};
    async::AsyncOp* op{nullptr};
};

HANDLE g_iocp = nullptr;
std::thread g_iocp_worker;
std::atomic<bool> g_iocp_stop{false};

// Finalizes an op on the worker thread: maps the completion status, frees
// the transport context, and hands a heap Completion to Layer G.  For
// cancels (ERROR_OPERATION_ABORTED) the reason was recorded on op->error
// by NetAsyncCancel before CancelIoEx, so it is preserved verbatim.
void FinalizeOp(IocpOpContext* ctx, DWORD bytes, BOOL ok, DWORD winerr) noexcept
{
    async::AsyncOp* op = ctx->op;
    NetError err = NetError::None;
    if (!ok) {
        if (winerr == ERROR_OPERATION_ABORTED) {
            err = op->error;  // cancel/timeout reason set by Layer C
        } else {
            tls_last_native = static_cast<CHAOS_IL2CPP_INT32>(winerr);
            err = CoarseError();
        }
        bytes = 0;
    }
    op->platform = nullptr;
    delete ctx;
    auto* c = new async::Completion{op, static_cast<CHAOS_IL2CPP_UINT32>(bytes), err};
    async::PostCompletion(c);
}

void IocpWorkerLoop() noexcept
{
    for (;;) {
        DWORD bytes = 0;
        ULONG_PTR key = 0;
        OVERLAPPED* ov = nullptr;
        const BOOL ok = GetQueuedCompletionStatus(g_iocp, &bytes, &key, &ov, INFINITE);
        const DWORD winerr = ok ? 0 : GetLastError();
        if (ov == nullptr) {
            if (g_iocp_stop.load(std::memory_order_relaxed)) {
                break;  // sentinel posted by StopIocp
            }
            std::this_thread::yield();
            continue;
        }
        auto* ctx = CONTAINING_RECORD(ov, IocpOpContext, overlapped);
        FinalizeOp(ctx, bytes, ok, winerr);
    }
}

bool StartIocp() noexcept
{
    if (g_iocp != nullptr) {
        return true;
    }
    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
    if (g_iocp == nullptr) {
        SetLastNative(WSAGetLastError());
        return false;
    }
    g_iocp_stop.store(false, std::memory_order_relaxed);
    g_iocp_worker = std::thread(IocpWorkerLoop);
    return true;
}

void StopIocp() noexcept
{
    if (g_iocp == nullptr) {
        return;
    }
    g_iocp_stop.store(true, std::memory_order_relaxed);
    PostQueuedCompletionStatus(g_iocp, 0, 0, nullptr);  // wake the worker
    if (g_iocp_worker.joinable()) {
        g_iocp_worker.join();
    }
    CloseHandle(g_iocp);
    g_iocp = nullptr;
}

}  // namespace

int g_net_impl_refcount = 0;  // shared with Layer A entry.cpp via NetStartup/Cleanup

NetError NetStartup() noexcept
{
    if (g_net_impl_refcount == 0) {
        WSADATA data{};
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
            SetLastNative(WSAGetLastError());
            return CoarseError();
        }
        if (!StartIocp()) {
            WSACleanup();
            return CoarseError();
        }
    }
    ++g_net_impl_refcount;
    SetLastNative(0);
    return NetError::None;
}

void NetCleanup() noexcept
{
    if (g_net_impl_refcount > 0) {
        --g_net_impl_refcount;
        if (g_net_impl_refcount == 0) {
            StopIocp();
            WSACleanup();
        }
    }
}

NetError NetSocketCreate(CHAOS_IL2CPP_INT32 family, CHAOS_IL2CPP_INT32 type,
                         CHAOS_IL2CPP_INT32 protocol, SocketHandle* handle) noexcept
{
    SetLastNative(0);
    if (handle == nullptr) {
        SetLastNative(WSAEINVAL);
        return NetError::Unsupported;
    }
    SOCKET s = ::socket(family, type, protocol);
    if (s == INVALID_SOCKET) {
        SetLastNative(WSAGetLastError());
        return CoarseError();
    }
    if (!SetNonBlocking(s)) {
        SetLastNative(WSAGetLastError());
        ::closesocket(s);
        return CoarseError();
    }
    if (g_iocp != nullptr) {
        // Ignore failure: sync IO still works; async Begin* rejects with
        // NotSupported when the socket is not port-associated.
        CreateIoCompletionPort(reinterpret_cast<HANDLE>(s), g_iocp,
                               reinterpret_cast<ULONG_PTR>(handle), 0);
    }
    handle->fd = static_cast<CHAOS_IL2CPP_INTPTR>(s);
    handle->family = family;
    handle->type = type;
    handle->protocol = protocol;
    handle->state = SocketState::Created;
    return NetError::None;
}

NetError NetSocketClose(SocketHandle* handle) noexcept
{
    SetLastNative(0);
    if (handle == nullptr || handle->fd < 0) {
        return NetError::None;  // already closed; idempotent
    }
    if (handle->pending_op != nullptr) {
        // Abort the in-flight overlapped op; its completion arrives on the
        // worker with ERROR_OPERATION_ABORTED -> the recorded reason.
        CancelIoEx(reinterpret_cast<HANDLE>(handle->fd), nullptr);
    }
    ::closesocket(static_cast<SOCKET>(handle->fd));
    handle->fd = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
    handle->state = SocketState::Closed;
    return NetError::None;
}

NetError NetSocketBind(SocketHandle* handle, const NetAddress& addr) noexcept
{
    if (handle == nullptr || handle->fd < 0) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    sockaddr_storage ss{};
    const int salen = ToSockAddr(addr, &ss);
    if (salen == 0) {
        SetLastNative(WSAEAFNOSUPPORT);
        return CoarseError();
    }
    if (::bind(static_cast<SOCKET>(handle->fd), reinterpret_cast<sockaddr*>(&ss),
               salen) == SOCKET_ERROR) {
        SetLastNative(WSAGetLastError());
        return CoarseError();
    }
    return NetError::None;
}

NetError NetSocketListen(SocketHandle* handle, CHAOS_IL2CPP_INT32 backlog) noexcept
{
    if (handle == nullptr || handle->fd < 0) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    if (::listen(static_cast<SOCKET>(handle->fd), backlog) == SOCKET_ERROR) {
        SetLastNative(WSAGetLastError());
        return CoarseError();
    }
    handle->state = SocketState::Listening;
    return NetError::None;
}

NetError NetSocketAccept(SocketHandle* handle, SocketHandle* peer,
                         NetAddress* peer_addr, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (handle == nullptr || handle->fd < 0 || peer == nullptr) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    for (;;) {
        sockaddr_storage ca{};
        int clen = static_cast<int>(sizeof(ca));
        SOCKET c = ::accept(static_cast<SOCKET>(handle->fd),
                            reinterpret_cast<sockaddr*>(&ca), &clen);
        if (c != INVALID_SOCKET) {
            if (!SetNonBlocking(c)) {
                SetLastNative(WSAGetLastError());
                ::closesocket(c);
                return CoarseError();
            }
            if (g_iocp != nullptr) {
                // Accepted sockets are not created via NetSocketCreate, so
                // associate explicitly or overlapped completions are lost.
                CreateIoCompletionPort(reinterpret_cast<HANDLE>(c), g_iocp,
                                       reinterpret_cast<ULONG_PTR>(peer), 0);
            }
            peer->fd = static_cast<CHAOS_IL2CPP_INTPTR>(c);
            peer->state = SocketState::Connected;
            peer->family = handle->family;
            peer->type = handle->type;
            peer->protocol = handle->protocol;
            if (peer_addr != nullptr) {
                FillNetAddress(reinterpret_cast<sockaddr*>(&ca), clen, peer_addr);
            }
            SetLastNative(0);
            return NetError::None;
        }
        const int e = WSAGetLastError();
        if (e == WSAEWOULDBLOCK) {
            const CHAOS_IL2CPP_INT32 rev = PollWait(static_cast<SOCKET>(handle->fd), POLLIN, timeout_ms);
            if (rev <= 0) {
                return rev < 0 ? CoarseError() : NetError::TimedOut;
            }
            continue;  // retry accept
        }
        SetLastNative(e);
        return CoarseError();
    }
}

NetError NetSocketConnect(SocketHandle* handle, const NetAddress& addr,
                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (handle == nullptr || handle->fd < 0) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    sockaddr_storage ss{};
    const int salen = ToSockAddr(addr, &ss);
    if (salen == 0) {
        SetLastNative(WSAEAFNOSUPPORT);
        return CoarseError();
    }
    int rc = ::connect(static_cast<SOCKET>(handle->fd), reinterpret_cast<sockaddr*>(&ss),
                       salen);
    if (rc == 0) {
        handle->state = SocketState::Connected;
        SetLastNative(0);
        return NetError::None;
    }
    const int e = WSAGetLastError();
    if (e == WSAEWOULDBLOCK || e == WSAEINPROGRESS) {
        const CHAOS_IL2CPP_INT32 rev = PollWait(static_cast<SOCKET>(handle->fd), POLLOUT, timeout_ms);
        if (rev <= 0) {
            return rev < 0 ? CoarseError() : NetError::TimedOut;
        }
        // Connection result via SO_ERROR (accepts both POLLOUT and POLLERR).
        int soerr = 0;
        int soerr_len = static_cast<int>(sizeof(soerr));
        if (getsockopt(static_cast<SOCKET>(handle->fd), SOL_SOCKET, SO_ERROR,
                       reinterpret_cast<char*>(&soerr), &soerr_len) == SOCKET_ERROR) {
            SetLastNative(WSAGetLastError());
            return CoarseError();
        }
        if (soerr != 0) {
            SetLastNative(soerr);
            return CoarseError();
        }
        handle->state = SocketState::Connected;
        SetLastNative(0);
        return NetError::None;
    }
    SetLastNative(e);
    return CoarseError();
}


// NT-23: Happy-Eyeballs connect (RFC 8305 simplified).  Tries addrs[0..count)
// in order; a fresh non-blocking socket is created per attempt (its family),
// connected with a per-attempt timeout (clamped to kHappyEyeballAttemptMs
// when useHappyEyeballs is set), and closed on failure.  The first address
// that connects wins; *out receives the Connected socket the caller owns and
// closes.  connectTimeoutMs <= 0 → 10000.  With useHappyEyeballs == 0 only
// addrs[0] is attempted (legacy single-address).  null out/addrs or count
// <= 0 → Unsupported.
NetError NetSocketConnectList(SocketHandle* out, const NetAddress* addrs,
                              CHAOS_IL2CPP_INT32 count,
                              const NetConnectOptions& opts) noexcept
{
    if (out == nullptr || addrs == nullptr || count <= 0) {
        return NetError::Unsupported;
    }
    out->fd = -1;
    out->state = SocketState::Closed;
    const CHAOS_IL2CPP_INT32 budget = opts.connectTimeoutMs > 0 ? opts.connectTimeoutMs : 10000;
    const CHAOS_IL2CPP_INT32 per_attempt =
        opts.useHappyEyeballs ? (budget < kHappyEyeballAttemptMs ? budget : kHappyEyeballAttemptMs)
                              : budget;
    const CHAOS_IL2CPP_INT32 attempts = opts.useHappyEyeballs ? count : 1;
    NetError last = NetError::Unsupported;
    for (CHAOS_IL2CPP_INT32 i = 0; i < attempts; ++i) {
        SocketHandle h{};
        NetError e = NetSocketCreate(addrs[i].family, kSocketTypeStream, kProtocolTcp, &h);
        if (e != NetError::None) { last = e; continue; }
        NetSocketSetOption(&h, NetOption::NoDelay, 1);
        e = NetSocketConnect(&h, addrs[i], per_attempt);
        if (e == NetError::None) { *out = h; return NetError::None; }
        last = e;
        NetSocketClose(&h);
    }
    return last;
}
NetError NetSocketSend(SocketHandle* handle, const CHAOS_IL2CPP_UINT8* data,
                       CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                       CHAOS_IL2CPP_INT32* sent, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (sent != nullptr) {
        *sent = 0;
    }
    if (handle == nullptr || handle->fd < 0) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    if (len > 0 && data == nullptr) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    for (;;) {
        const int rc = ::send(static_cast<SOCKET>(handle->fd),
                              reinterpret_cast<const char*>(len > 0 ? data : nullptr),
                              len, flags);
        if (rc != SOCKET_ERROR) {
            if (sent != nullptr) {
                *sent = rc;
            }
            SetLastNative(0);
            return NetError::None;
        }
        const int e = WSAGetLastError();
        if (e == WSAEWOULDBLOCK) {
            const CHAOS_IL2CPP_INT32 rev = PollWait(static_cast<SOCKET>(handle->fd), POLLOUT, timeout_ms);
            if (rev < 0) {
                return CoarseError();
            }
            if (rev == 0) {
                return NetError::TimedOut;
            }
            continue;  // writable again; retry send
        }
        SetLastNative(e);
        return CoarseError();
    }
}

NetError NetSocketRecv(SocketHandle* handle, CHAOS_IL2CPP_UINT8* data,
                       CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                       CHAOS_IL2CPP_INT32* got, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (got != nullptr) {
        *got = 0;
    }
    if (handle == nullptr || handle->fd < 0) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    if (len > 0 && data == nullptr) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    for (;;) {
        const int rc = ::recv(static_cast<SOCKET>(handle->fd),
                              reinterpret_cast<char*>(len > 0 ? data : nullptr),
                              len, flags);
        if (rc != SOCKET_ERROR) {
            if (got != nullptr) {
                *got = rc;  // 0 == peer closed
            }
            SetLastNative(0);
            return NetError::None;
        }
        const int e = WSAGetLastError();
        if (e == WSAEWOULDBLOCK) {
            const CHAOS_IL2CPP_INT32 rev = PollWait(static_cast<SOCKET>(handle->fd), POLLIN, timeout_ms);
            if (rev < 0) {
                return CoarseError();
            }
            if (rev == 0) {
                return NetError::TimedOut;
            }
            continue;  // readable again; retry recv
        }
        SetLastNative(e);
        return CoarseError();
    }
}

namespace {
bool SetOptRaw(SOCKET s, int level, int optname, const void* value, int len) noexcept
{
    if (setsockopt(s, level, optname, static_cast<const char*>(value), len) == SOCKET_ERROR) {
        SetLastNative(WSAGetLastError());
        return false;
    }
    return true;
}
bool GetOptRaw(SOCKET s, int level, int optname, void* value, int* len) noexcept
{
    if (getsockopt(s, level, optname, static_cast<char*>(value), len) == SOCKET_ERROR) {
        SetLastNative(WSAGetLastError());
        return false;
    }
    return true;
}
}  // namespace

NetError NetSocketSetOption(SocketHandle* handle, NetOption opt,
                            CHAOS_IL2CPP_INT32 value) noexcept
{
    if (handle == nullptr || handle->fd < 0) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    const SOCKET s = static_cast<SOCKET>(handle->fd);
    switch (opt) {
        case NetOption::NoDelay: {
            const int v = value;
            if (!SetOptRaw(s, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v))) return CoarseError();
            handle->opts.no_delay = value != 0 ? 1 : 0;
            return NetError::None;
        }
        case NetOption::ReuseAddress: {
            const int v = value;
            if (!SetOptRaw(s, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v))) return CoarseError();
            handle->opts.reuse_addr = value != 0 ? 1 : 0;
            return NetError::None;
        }
        case NetOption::KeepAlive: {
            const int v = value;
            if (!SetOptRaw(s, SOL_SOCKET, SO_KEEPALIVE, &v, sizeof(v))) return CoarseError();
            handle->opts.keep_alive = value != 0 ? 1 : 0;
            return NetError::None;
        }
        case NetOption::IPv6Only: {
            const int v = value;
            if (!SetOptRaw(s, IPPROTO_IPV6, IPV6_V6ONLY, &v, sizeof(v))) return CoarseError();
            handle->opts.ipv6_only = value != 0 ? 1 : 0;
            return NetError::None;
        }
        case NetOption::ReceiveTimeout: {
            const int v = value;  // ms (DWORD on winsock; same size)
            if (!SetOptRaw(s, SOL_SOCKET, SO_RCVTIMEO, &v, sizeof(v))) return CoarseError();
            handle->opts.receive_timeout_ms = value;
            return NetError::None;
        }
        case NetOption::SendTimeout: {
            const int v = value;
            if (!SetOptRaw(s, SOL_SOCKET, SO_SNDTIMEO, &v, sizeof(v))) return CoarseError();
            handle->opts.send_timeout_ms = value;
            return NetError::None;
        }
        case NetOption::EnableBroadcast: {
            const int v = value;
            if (!SetOptRaw(s, SOL_SOCKET, SO_BROADCAST, &v, sizeof(v))) return CoarseError();
            handle->opts.enable_broadcast = value != 0 ? 1 : 0;
            return NetError::None;
        }
        case NetOption::Linger: {
            linger lg{};
            lg.l_onoff = value > 0 ? 1 : 0;
            lg.l_linger = static_cast<u_short>(value > 0 ? value : 0);
            if (!SetOptRaw(s, SOL_SOCKET, SO_LINGER, &lg, sizeof(lg))) return CoarseError();
            handle->opts.linger_enabled = lg.l_onoff != 0 ? 1 : 0;
            handle->opts.linger_sec = lg.l_linger;
            return NetError::None;
        }
        default:
            SetLastNative(WSAEOPNOTSUPP);
            return CoarseError();
    }
}

NetError NetSocketGetOption(SocketHandle* handle, NetOption opt,
                            CHAOS_IL2CPP_INT32* value) noexcept
{
    if (handle == nullptr || handle->fd < 0 || value == nullptr) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    const SOCKET s = static_cast<SOCKET>(handle->fd);
    switch (opt) {
        case NetOption::NoDelay: {
            int v = 0; int l = sizeof(v);
            if (!GetOptRaw(s, IPPROTO_TCP, TCP_NODELAY, &v, &l)) return CoarseError();
            *value = v;
            return NetError::None;
        }
        case NetOption::ReuseAddress: {
            int v = 0; int l = sizeof(v);
            if (!GetOptRaw(s, SOL_SOCKET, SO_REUSEADDR, &v, &l)) return CoarseError();
            *value = v;
            return NetError::None;
        }
        case NetOption::KeepAlive: {
            int v = 0; int l = sizeof(v);
            if (!GetOptRaw(s, SOL_SOCKET, SO_KEEPALIVE, &v, &l)) return CoarseError();
            *value = v;
            return NetError::None;
        }
        case NetOption::IPv6Only: {
            int v = 0; int l = sizeof(v);
            if (!GetOptRaw(s, IPPROTO_IPV6, IPV6_V6ONLY, &v, &l)) return CoarseError();
            *value = v;
            return NetError::None;
        }
        case NetOption::ReceiveTimeout: {
            int v = 0; int l = sizeof(v);
            if (!GetOptRaw(s, SOL_SOCKET, SO_RCVTIMEO, &v, &l)) return CoarseError();
            *value = v;
            return NetError::None;
        }
        case NetOption::SendTimeout: {
            int v = 0; int l = sizeof(v);
            if (!GetOptRaw(s, SOL_SOCKET, SO_SNDTIMEO, &v, &l)) return CoarseError();
            *value = v;
            return NetError::None;
        }
        case NetOption::EnableBroadcast: {
            int v = 0; int l = sizeof(v);
            if (!GetOptRaw(s, SOL_SOCKET, SO_BROADCAST, &v, &l)) return CoarseError();
            *value = v;
            return NetError::None;
        }
        case NetOption::Linger: {
            linger lg{};
            int l = sizeof(lg);
            if (!GetOptRaw(s, SOL_SOCKET, SO_LINGER, &lg, &l)) return CoarseError();
            *value = lg.l_onoff != 0 ? static_cast<CHAOS_IL2CPP_INT32>(lg.l_linger) : 0;
            return NetError::None;
        }
        default:
            SetLastNative(WSAEOPNOTSUPP);
            return CoarseError();
    }
}

NetError NetSocketGetLocalAddress(SocketHandle* handle, NetAddress* addr) noexcept
{
    if (handle == nullptr || handle->fd < 0 || addr == nullptr) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    sockaddr_storage ss{};
    int l = static_cast<int>(sizeof(ss));
    if (getsockname(static_cast<SOCKET>(handle->fd), reinterpret_cast<sockaddr*>(&ss), &l) == SOCKET_ERROR) {
        SetLastNative(WSAGetLastError());
        return CoarseError();
    }
    FillNetAddress(reinterpret_cast<sockaddr*>(&ss), l, addr);
    return NetError::None;
}

SocketError NetSocketLastError() noexcept
{
    return NativeCodeToSocketError(tls_last_native);
}

NetError NetSocketBeginSend(SocketHandle* handle, const CHAOS_IL2CPP_UINT8* data,
                            CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                            async::AsyncOp* op) noexcept
{
    SetLastNative(0);
    if (handle == nullptr || handle->fd < 0 || op == nullptr || g_iocp == nullptr) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    auto* ctx = new IocpOpContext();
    ctx->op = op;
    op->platform = ctx;
    WSABUF wb{};
    wb.len = static_cast<ULONG>(len < 0 ? 0 : len);
    wb.buf = const_cast<CHAR*>(reinterpret_cast<const CHAR*>(data));
    DWORD sent = 0;
    const int rc = WSASend(static_cast<SOCKET>(handle->fd), &wb, 1, &sent, flags,
                           &ctx->overlapped, nullptr);
    if (rc == 0) {
        // Immediate completion: the completion packet was already queued to
        // the IOCP; the worker finalizes it.  Nothing else to do.
        return NetError::None;
    }
    const int e = WSAGetLastError();
    if (e == WSA_IO_PENDING) {
        return NetError::None;  // in flight; completion arrives via worker
    }
    tls_last_native = e;
    FinalizeOp(ctx, 0, FALSE, static_cast<DWORD>(e));  // hard failure
    return NetError::None;
}

NetError NetSocketBeginRecv(SocketHandle* handle, CHAOS_IL2CPP_UINT8* data,
                            CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                            async::AsyncOp* op) noexcept
{
    SetLastNative(0);
    if (handle == nullptr || handle->fd < 0 || op == nullptr || g_iocp == nullptr) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    auto* ctx = new IocpOpContext();
    ctx->op = op;
    op->platform = ctx;
    WSABUF wb{};
    wb.len = static_cast<ULONG>(len < 0 ? 0 : len);
    wb.buf = reinterpret_cast<CHAR*>(data);
    DWORD got = 0;
    DWORD recv_flags = static_cast<DWORD>(flags);
    const int rc = WSARecv(static_cast<SOCKET>(handle->fd), &wb, 1, &got, &recv_flags,
                           &ctx->overlapped, nullptr);
    if (rc == 0) {
        return NetError::None;  // completion packet already queued
    }
    const int e = WSAGetLastError();
    if (e == WSA_IO_PENDING) {
        return NetError::None;
    }
    tls_last_native = e;
    FinalizeOp(ctx, 0, FALSE, static_cast<DWORD>(e));
    return NetError::None;
}

NetError NetSocketCancelOp(SocketHandle* handle, async::AsyncOp* op,
                           NetError reason) noexcept
{
    if (handle == nullptr || handle->fd < 0 || op == nullptr) {
        SetLastNative(WSAEINVAL);
        return CoarseError();
    }
    (void)reason;  // Layer C already recorded the reason on op->error
    auto* ctx = static_cast<IocpOpContext*>(op->platform);
    if (ctx == nullptr) {
        return NetError::None;  // nothing submitted
    }
    if (!CancelIoEx(reinterpret_cast<HANDLE>(handle->fd), &ctx->overlapped)) {
        const DWORD e = GetLastError();
        if (e == ERROR_NOT_FOUND) {
            return NetError::None;  // op already completed
        }
        SetLastNative(static_cast<CHAOS_IL2CPP_INT32>(e));
        return CoarseError();
    }
    return NetError::None;
}

}  // namespace chaos::net