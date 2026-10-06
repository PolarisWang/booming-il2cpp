// net_impl_posix.cpp -- Layer D transport for POSIX platforms (NT-11).
//
// Port of net_impl_win32.cpp semantics onto POSIX sockets, keeping the exact
// NetSocket* API surface declared in net_api.h:
//
//   * Linux   -> level-triggered epoll + eventfd waker
//   * other   -> poll() + pipe waker (MSYS2/Cygwin/Android/macOS/...)
//
// (epoll is Linux-only; everything else falls back to poll per
//  docs/dev/in-progress/net-cpp-architecture.md §7.1 / pal_wakeable pattern.)
//
// Async completions follow the same contract as the IOCP driver:
// PosixOpContext* (op->platform) is registered per-fd, a single worker loop
// performs the non-blocking recv/send when the descriptor is ready, and the
// op completes through async::PostCompletion with a heap Completion that
// OnNetCompletion frees (async_op.h).  Cancel/close claim the fd under the
// registry mutex and force-complete immediately, mirroring CancelIoEx.
// One op per socket (Layer C) makes the fd the unique registry key.
//
// All synchronous functions are noexcept and return NetError; the
// fine-grained SocketError of the last failure is available via
// NetSocketLastError() (thread-local), matching abi.md / NetSocketLastError
// semantics of the win32 driver.
#if defined(_WIN32)
#error "net_impl_posix.cpp must only be built on a POSIX platform"
#endif

// epoll_create1 / eventfd / EPOLL_CLOEXEC / EFD_CLOEXEC are glibc GNU
// extensions; declare _GNU_SOURCE up-front so the Linux backend also
// compiles under strict -std=c++NN (no default feature-test macros).
#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif

#include <chaos/net/net_api.h>

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

#include <arpa/inet.h>
#include <sys/time.h>    // htons / ntohs
#include <fcntl.h>        // fcntl O_NONBLOCK
#include <netinet/in.h>
#include <netinet/tcp.h>  // TCP_NODELAY
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#if defined(__linux__)
#include <sys/epoll.h>
#include <sys/eventfd.h>
#endif

namespace chaos::net {
namespace {

using async::AsyncOp;
using async::Completion;

// ── error plumbing (thread-local, same shape as the win32 driver) ───────────
thread_local CHAOS_IL2CPP_INT32 tls_last_native = 0;

void SetLastNative(CHAOS_IL2CPP_INT32 code) noexcept { tls_last_native = code; }

// Coarse per-op classification of the native error in tls_last_native.
// if-chain (not switch) so platforms where errno macros share values
// (EAGAIN==EWOULDBLOCK, EOPNOTSUPP==ENOTSUP) still compile.
NetError CoarseError() noexcept
{
    const auto e = tls_last_native;
    if (e == ENOTCONN)                return NetError::NotConnected;
    if (e == ETIMEDOUT)               return NetError::TimedOut;
    if (e == EAGAIN || e == EWOULDBLOCK) return NetError::WouldBlock;
    if (e == ECONNRESET || e == ECONNABORTED) return NetError::ConnectionReset;
    return NetError::Unsupported;
}

// ── address conversion (IPv4+IPv6, mirrors win32 ToSockAddr/FillNetAddress) ─
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
        std::memset(out->addr, 0, sizeof(out->addr));
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
            std::memset(out->addr, 0, sizeof(out->addr));
            std::memcpy(out->addr, p + 12, 4);
            return;
        }
        out->family = kAddressFamilyInet6;
        out->port = ntohs(sa6->sin6_port);
        std::memset(out->addr, 0, sizeof(out->addr));
        std::memcpy(out->addr, p, 16);
        return;
    }
    out->family = 0;
    out->port = 0;
    std::memset(out->addr, 0, sizeof(out->addr));
}

bool SetNonBlocking(int fd) noexcept
{
    const int flags = ::fcntl(fd, F_GETFL, 0);
    if (flags < 0) return false;
    return ::fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}

// Polls fd for the given events with the win32 PollWait semantics:
//   timeout_ms <= 0 -> infinite, rc==0 -> 0 (timeout), rc<0 -> -1 (errno set).
// Returns revents on readiness (possibly including POLLERR/POLLHUP/POLLNVAL).
CHAOS_IL2CPP_INT32 PollWait(int fd, short events, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = events;
    pfd.revents = 0;
    const int timeout = timeout_ms <= 0 ? -1 : timeout_ms;
    int rc;
    do { rc = ::poll(&pfd, 1, timeout); } while (rc < 0 && errno == EINTR);
    if (rc < 0) { SetLastNative(errno); return -1; }
    if (rc == 0) return 0;
    return pfd.revents;
}

// ── NT-7 async completion pipeline ─────────────────────────────────────────
// Registry: one PosixOpContext per pending op (fd == registry key, Layer C
// enforces single-op-per-socket).  The worker loop performs the non-blocking
// recv/send while HOLDING the registry mutex, so cancel/close cannot close an
// fd the loop is touching, and the loop cannot re-arm a closed fd.
struct PosixOpContext {
    int fd = -1;
    bool write = false;      // true: EPOLLOUT/send ; false: EPOLLIN/recv
    AsyncOp* op = nullptr;
};

std::mutex g_ops_mutex;
std::vector<PosixOpContext*> g_ops;
std::atomic<bool> g_loop_stop{false};
std::atomic<bool> g_loop_up{false};
std::thread g_loop_thread;

#if defined(__linux__)
int g_epoll_fd = -1;
int g_wake_fd = -1;          // eventfd
void WakeLoop() noexcept { if (g_wake_fd >= 0) { uint64_t one = 1; (void)::write(g_wake_fd, &one, sizeof(one)); } }
bool RegisterBackend(PosixOpContext* ctx) noexcept
{
    struct epoll_event ev;
    ev.events = ctx->write ? EPOLLOUT : EPOLLIN;
    // fd carried as an integer, never dereferenced, so a stale event for a
    // closed/reused fd cannot touch freed memory.
    ev.data.ptr = reinterpret_cast<void*>(static_cast<intptr_t>(ctx->fd) + 1);  // (fd+1): fd 0 != waker sentinel (nullptr)
    return ::epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, ctx->fd, &ev) == 0;
}
void UnregisterBackend(PosixOpContext* ctx) noexcept
{
    if (g_epoll_fd >= 0 && ctx->fd >= 0)
        (void)::epoll_ctl(g_epoll_fd, EPOLL_CTL_DEL, ctx->fd, nullptr);
}
#else
int g_wake_pipe[2] = {-1, -1};
void WakeLoop() noexcept { if (g_wake_pipe[1] >= 0) { char b = 1; (void)::write(g_wake_pipe[1], &b, 1); } }
bool RegisterBackend(PosixOpContext*) noexcept { return true; }
void UnregisterBackend(PosixOpContext*) noexcept {}
#endif

// Completes the op: clears op->platform, frees the ctx and posts the heap
// Completion to Layer G.  Never called while holding the registry mutex
// (PostCompletion may run on_done synchronously, which can re-enter).
void Finish(PosixOpContext* ctx, CHAOS_IL2CPP_INT32 bytes, NetError err) noexcept
{
    AsyncOp* op = ctx->op;
    ctx->op = nullptr;
    if (op != nullptr) op->platform = nullptr;
    delete ctx;
    async::PostCompletion(new Completion{op, static_cast<CHAOS_IL2CPP_UINT32>(bytes), err});
}

// Attempts one non-blocking recv/send for a registered fd, called with the
// registry mutex already held (the fd is guaranteed open at this point).
// Returns true when the op completed (caller must Finish outside the lock).
bool ProcessTargetLocked(PosixOpContext* ctx, CHAOS_IL2CPP_INT32& bytes_out,
                         NetError& err_out) noexcept
{
    const size_t len = ctx->op != nullptr && ctx->op->len > 0
                           ? static_cast<size_t>(ctx->op->len) : 0u;
    const ssize_t rc = ctx->write
                           ? ::send(ctx->fd, ctx->op != nullptr ? ctx->op->buf : nullptr, len, 0)
                           : ::recv(ctx->fd, ctx->op != nullptr ? ctx->op->buf : nullptr, len, 0);
    if (rc >= 0) { bytes_out = static_cast<CHAOS_IL2CPP_INT32>(rc); err_out = NetError::None; return true; }
    if (errno == EINTR) return false;               // stays registered
    if (errno == EAGAIN || errno == EWOULDBLOCK) return false;  // stays registered
    SetLastNative(errno);
    bytes_out = 0;
    err_out = CoarseError();
    return true;
}

// Claims + processes a fd (mutex held for the whole syscall).
void TryProcessSocket(int fd) noexcept
{
    PosixOpContext* ctx = nullptr;
    CHAOS_IL2CPP_INT32 bytes = 0;
    NetError err = NetError::None;
    bool completed = false;
    {
        std::lock_guard<std::mutex> lk(g_ops_mutex);
        auto it = std::find_if(g_ops.begin(), g_ops.end(),
                               [fd](PosixOpContext* c) { return c->fd == fd; });
        if (it == g_ops.end()) return;              // cancelled/closed meanwhile
        ctx = *it;
        if (ProcessTargetLocked(ctx, bytes, err)) {
            g_ops.erase(it);
            completed = true;
            UnregisterBackend(ctx);                 // re-ADD on next Begin*
        }
    }
    if (completed) Finish(ctx, bytes, err);
}

// Forces completion of the pending op on fd (cancel / close path; errno
// semantics: op->error carries the recorded reason, like ERROR_OPERATION_ABORTED).
PosixOpContext* ClaimAndErase(int fd) noexcept
{
    PosixOpContext* ctx = nullptr;
    {
        std::lock_guard<std::mutex> lk(g_ops_mutex);
        auto it = std::find_if(g_ops.begin(), g_ops.end(),
                               [fd](PosixOpContext* c) { return c->fd == fd; });
        if (it != g_ops.end()) { ctx = *it; g_ops.erase(it); }
    }
    return ctx;
}

void DrainRemaining() noexcept
{
    std::vector<PosixOpContext*> remaining;
    {
        std::lock_guard<std::mutex> lk(g_ops_mutex);
        remaining.swap(g_ops);
    }
    for (PosixOpContext* ctx : remaining) {
        UnregisterBackend(ctx);
        Finish(ctx, 0, NetError::Unsupported);
    }
}

#if defined(__linux__)
void LoopMain() noexcept
{
    struct epoll_event events[64];
    while (!g_loop_stop.load(std::memory_order_relaxed)) {
        const int n = ::epoll_wait(g_epoll_fd, events, 64, 100);
        if (n < 0) { if (errno == EINTR) continue; break; }
        for (int i = 0; i < n; ++i) {
            if (events[i].data.ptr == nullptr) {   // waker (eventfd)
                uint64_t v;
                while (::read(g_wake_fd, &v, sizeof(v)) < 0 && errno == EINTR) {}
                continue;
            }
            const int fd = static_cast<int>(reinterpret_cast<intptr_t>(events[i].data.ptr)) - 1;
            TryProcessSocket(fd);
        }
    }
    DrainRemaining();
}
#else
void LoopMain() noexcept
{
    std::vector<struct pollfd> pfds;
    while (!g_loop_stop.load(std::memory_order_relaxed)) {
        pfds.clear();
        {
            std::lock_guard<std::mutex> lk(g_ops_mutex);
            pfds.reserve(g_ops.size() + 1);
            struct pollfd w;
            w.fd = g_wake_pipe[0]; w.events = POLLIN; w.revents = 0;
            pfds.push_back(w);
            for (PosixOpContext* ctx : g_ops) {
                struct pollfd p;
                p.fd = ctx->fd;
                p.events = ctx->write ? POLLOUT : POLLIN;
                p.revents = 0;
                pfds.push_back(p);
            }
        }
        const int rc = ::poll(pfds.data(), static_cast<nfds_t>(pfds.size()), 100);
        if (rc < 0) { if (errno == EINTR) continue; break; }
        if (rc == 0) continue;
        if (pfds[0].revents & POLLIN) {             // waker (pipe): drain
            char buf[64];
            while (::read(g_wake_pipe[0], buf, sizeof(buf)) > 0) {}
        }
        for (size_t i = 1; i < pfds.size(); ++i)
            if (pfds[i].revents != 0) TryProcessSocket(pfds[i].fd);
    }
    DrainRemaining();
}
#endif

bool StartLoop() noexcept
{
    if (g_loop_up.load(std::memory_order_acquire)) return true;
#if defined(__linux__)
    g_epoll_fd = ::epoll_create1(EPOLL_CLOEXEC);
    if (g_epoll_fd < 0) return false;
    g_wake_fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if (g_wake_fd < 0) { ::close(g_epoll_fd); g_epoll_fd = -1; return false; }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = nullptr;
    if (::epoll_ctl(g_epoll_fd, EPOLL_CTL_ADD, g_wake_fd, &ev) != 0) {
        ::close(g_wake_fd); ::close(g_epoll_fd); g_wake_fd = -1; g_epoll_fd = -1;
        return false;
    }
#else
    if (::pipe(g_wake_pipe) != 0) return false;
    (void)SetNonBlocking(g_wake_pipe[0]);
    (void)SetNonBlocking(g_wake_pipe[1]);
#endif
    g_loop_stop.store(false, std::memory_order_relaxed);
    g_loop_thread = std::thread(LoopMain);
    g_loop_up.store(true, std::memory_order_release);
    return true;
}

void StopLoop() noexcept
{
    if (!g_loop_up.load(std::memory_order_acquire)) return;
    g_loop_stop.store(true, std::memory_order_relaxed);
    WakeLoop();
    if (g_loop_thread.joinable()) g_loop_thread.join();
    g_loop_up.store(false, std::memory_order_release);
#if defined(__linux__)
    if (g_wake_fd >= 0) { ::close(g_wake_fd); g_wake_fd = -1; }
    if (g_epoll_fd >= 0) { ::close(g_epoll_fd); g_epoll_fd = -1; }
#else
    if (g_wake_pipe[0] >= 0) { ::close(g_wake_pipe[0]); g_wake_pipe[0] = -1; }
    if (g_wake_pipe[1] >= 0) { ::close(g_wake_pipe[1]); g_wake_pipe[1] = -1; }
#endif
    // Registry is empty after DrainRemaining; nothing to clean.
    std::vector<PosixOpContext*> leftover;
    {
        std::lock_guard<std::mutex> lk(g_ops_mutex);
        leftover.swap(g_ops);
    }
    for (PosixOpContext* ctx : leftover) { delete ctx; }
}

int g_refcount = 0;

}  // namespace

// ── synchronous transport API (mirrors net_impl_win32.cpp) ─────────────────
NetError NetSocketCreate(CHAOS_IL2CPP_INT32 family, CHAOS_IL2CPP_INT32 type,
                         CHAOS_IL2CPP_INT32 protocol, SocketHandle* handle) noexcept
{
    SetLastNative(0);
    if (handle == nullptr) { SetLastNative(EINVAL); return NetError::Unsupported; }
    const int fd = ::socket(family, type, protocol);
    if (fd < 0) { SetLastNative(errno); return CoarseError(); }
    if (!SetNonBlocking(fd)) {
        const int e = errno;
        ::close(fd);
        SetLastNative(e);
        return CoarseError();
    }
    handle->fd = fd;
    handle->family = family;
    handle->type = type;
    handle->protocol = protocol;
    handle->state = SocketState::Created;
    return NetError::None;
}

NetError NetSocketClose(SocketHandle* handle) noexcept
{
    if (handle == nullptr || handle->fd < 0) return NetError::None;
    const int fd = static_cast<int>(handle->fd);
    PosixOpContext* ctx = nullptr;
    {
        // Close under the registry mutex so the worker loop can never touch
        // (or re-arm) a closed descriptor.
        std::lock_guard<std::mutex> lk(g_ops_mutex);
        auto it = std::find_if(g_ops.begin(), g_ops.end(),
                               [fd](PosixOpContext* c) { return c->fd == fd; });
        if (it != g_ops.end()) { ctx = *it; g_ops.erase(it); }
        ::close(fd);
        handle->fd = -1;
    }
    if (ctx != nullptr) {
        UnregisterBackend(ctx);
        Finish(ctx, 0, ctx->op != nullptr ? ctx->op->error : NetError::None);
    }
    handle->state = SocketState::Closed;
    return NetError::None;
}

NetError NetSocketBind(SocketHandle* handle, const NetAddress& addr) noexcept
{
    if (handle == nullptr || handle->fd < 0) { SetLastNative(EINVAL); return NetError::Unsupported; }
    sockaddr_storage ss;
    const int salen = ToSockAddr(addr, &ss);
    if (salen == 0) { SetLastNative(EAFNOSUPPORT); return NetError::Unsupported; }
    if (::bind(static_cast<int>(handle->fd), reinterpret_cast<sockaddr*>(&ss), salen) != 0) {
        SetLastNative(errno);
        return CoarseError();
    }
    return NetError::None;
}

NetError NetSocketListen(SocketHandle* handle, CHAOS_IL2CPP_INT32 backlog) noexcept
{
    if (handle == nullptr || handle->fd < 0) { SetLastNative(EINVAL); return NetError::Unsupported; }
    if (::listen(static_cast<int>(handle->fd), backlog) != 0) {
        SetLastNative(errno);
        return CoarseError();
    }
    handle->state = SocketState::Listening;
    return NetError::None;
}

NetError NetSocketAccept(SocketHandle* handle, SocketHandle* peer,
                         NetAddress* peer_addr, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (handle == nullptr || peer == nullptr || handle->fd < 0) {
        SetLastNative(EINVAL);
        return NetError::Unsupported;
    }
    const int fd = static_cast<int>(handle->fd);
    for (;;) {
        sockaddr_storage client;
        std::memset(&client, 0, sizeof(client));
        socklen_t len = sizeof(client);
        const int c = ::accept(fd, reinterpret_cast<sockaddr*>(&client), &len);
        if (c >= 0) {
            if (!SetNonBlocking(c)) {
                const int e = errno;
                ::close(c);
                SetLastNative(e);
                return CoarseError();
            }
            peer->fd = c;
            peer->state = SocketState::Connected;
            peer->family = handle->family;
            peer->type = handle->type;
            peer->protocol = handle->protocol;
            if (peer_addr != nullptr) FillNetAddress(reinterpret_cast<sockaddr*>(&client), static_cast<int>(len), peer_addr);
            return NetError::None;
        }
        if (errno == EINTR) continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            const CHAOS_IL2CPP_INT32 rev = PollWait(fd, POLLIN, timeout_ms);
            if (rev < 0) return CoarseError();
            if (rev == 0) return NetError::TimedOut;
            continue;
        }
        SetLastNative(errno);
        return CoarseError();
    }
}

NetError NetSocketConnect(SocketHandle* handle, const NetAddress& addr,
                          CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (handle == nullptr || handle->fd < 0) { SetLastNative(EINVAL); return NetError::Unsupported; }
    sockaddr_storage ss;
    const int salen = ToSockAddr(addr, &ss);
    if (salen == 0) { SetLastNative(EAFNOSUPPORT); return NetError::Unsupported; }
    const int fd = static_cast<int>(handle->fd);
    if (::connect(fd, reinterpret_cast<sockaddr*>(&ss), salen) == 0) {
        handle->state = SocketState::Connected;
        return NetError::None;
    }
    if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK) {
        const CHAOS_IL2CPP_INT32 rev = PollWait(fd, POLLOUT, timeout_ms);
        if (rev < 0) return CoarseError();
        if (rev == 0) return NetError::TimedOut;
        int soerr = 0;
        socklen_t soerr_len = sizeof(soerr);
        if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &soerr, &soerr_len) != 0) {
            SetLastNative(errno);
            return CoarseError();
        }
        if (soerr != 0) { SetLastNative(soerr); return CoarseError(); }
        handle->state = SocketState::Connected;
        return NetError::None;
    }
    SetLastNative(errno);
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
    if (sent != nullptr) *sent = 0;
    if (handle == nullptr || handle->fd < 0 || (len > 0 && data == nullptr)) {
        SetLastNative(EINVAL);
        return NetError::Unsupported;
    }
    const int fd = static_cast<int>(handle->fd);
    const size_t length = len > 0 ? static_cast<size_t>(len) : 0u;
    for (;;) {
        const ssize_t rc = ::send(fd, data, length, flags);
        if (rc >= 0) { if (sent != nullptr) *sent = static_cast<CHAOS_IL2CPP_INT32>(rc); return NetError::None; }
        if (errno == EINTR) continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            const CHAOS_IL2CPP_INT32 rev = PollWait(fd, POLLOUT, timeout_ms);
            if (rev < 0) return CoarseError();
            if (rev == 0) return NetError::TimedOut;
            continue;
        }
        SetLastNative(errno);
        return CoarseError();
    }
}

NetError NetSocketRecv(SocketHandle* handle, CHAOS_IL2CPP_UINT8* data,
                       CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                       CHAOS_IL2CPP_INT32* got, CHAOS_IL2CPP_INT32 timeout_ms) noexcept
{
    if (got != nullptr) *got = 0;
    if (handle == nullptr || handle->fd < 0 || (len > 0 && data == nullptr)) {
        SetLastNative(EINVAL);
        return NetError::Unsupported;
    }
    const int fd = static_cast<int>(handle->fd);
    const size_t length = len > 0 ? static_cast<size_t>(len) : 0u;
    for (;;) {
        const ssize_t rc = ::recv(fd, data, length, flags);
        if (rc >= 0) { if (got != nullptr) *got = static_cast<CHAOS_IL2CPP_INT32>(rc); return NetError::None; }
        if (errno == EINTR) continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            const CHAOS_IL2CPP_INT32 rev = PollWait(fd, POLLIN, timeout_ms);
            if (rev < 0) return CoarseError();
            if (rev == 0) return NetError::TimedOut;
            continue;
        }
        SetLastNative(errno);
        return CoarseError();
    }
}

NetError NetSocketSetOption(SocketHandle* handle, NetOption opt,
                            CHAOS_IL2CPP_INT32 value) noexcept
{
    if (handle == nullptr || handle->fd < 0) { SetLastNative(EINVAL); return NetError::Unsupported; }
    const int fd = static_cast<int>(handle->fd);
    const int ival = value;
    int rc = -1;
    switch (opt) {
        case NetOption::NoDelay:      rc = ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &ival, sizeof(ival)); break;
        case NetOption::ReuseAddress: rc = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &ival, sizeof(ival)); break;
        case NetOption::KeepAlive:    rc = ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &ival, sizeof(ival)); break;
        case NetOption::IPv6Only:     rc = ::setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &ival, sizeof(ival)); break;
        case NetOption::EnableBroadcast: rc = ::setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &ival, sizeof(ival)); break;
        case NetOption::ReceiveTimeout:
        case NetOption::SendTimeout: {
            const struct timeval tv{ value / 1000, static_cast<suseconds_t>((value % 1000) * 1000) };
            rc = ::setsockopt(fd, SOL_SOCKET,
                              opt == NetOption::ReceiveTimeout ? SO_RCVTIMEO : SO_SNDTIMEO,
                              &tv, sizeof(tv));
            break;
        }
        case NetOption::Linger: {
            const struct linger lg{ static_cast<unsigned short>(value > 0 ? 1 : 0),
                                    static_cast<unsigned short>(value > 0 ? value : 0) };
            rc = ::setsockopt(fd, SOL_SOCKET, SO_LINGER, &lg, sizeof(lg));
            break;
        }
        default: SetLastNative(EOPNOTSUPP); return CoarseError();
    }
    if (rc != 0) { SetLastNative(errno); return CoarseError(); }
    switch (opt) {
        case NetOption::NoDelay:      handle->opts.no_delay = value != 0; break;
        case NetOption::ReuseAddress: handle->opts.reuse_addr = value != 0; break;
        case NetOption::KeepAlive:    handle->opts.keep_alive = value != 0; break;
        case NetOption::IPv6Only:     handle->opts.ipv6_only = value != 0; break;
        case NetOption::ReceiveTimeout: handle->opts.receive_timeout_ms = value; break;
        case NetOption::SendTimeout:    handle->opts.send_timeout_ms = value; break;
        case NetOption::EnableBroadcast: handle->opts.enable_broadcast = value != 0; break;
        case NetOption::Linger:       handle->opts.linger_enabled = value > 0; handle->opts.linger_sec = value; break;
        default: break;
    }
    return NetError::None;
}

NetError NetSocketGetOption(SocketHandle* handle, NetOption opt,
                            CHAOS_IL2CPP_INT32* value) noexcept
{
    if (handle == nullptr || value == nullptr || handle->fd < 0) {
        SetLastNative(EINVAL);
        return NetError::Unsupported;
    }
    const int fd = static_cast<int>(handle->fd);
    int rc = -1;
    switch (opt) {
        case NetOption::NoDelay:
        case NetOption::ReuseAddress:
        case NetOption::KeepAlive:
        case NetOption::IPv6Only:
        case NetOption::EnableBroadcast: {
            int ival = 0;
            socklen_t len = sizeof(ival);
            int level = SOL_SOCKET, name = 0;
            if (opt == NetOption::NoDelay) { level = IPPROTO_TCP; name = TCP_NODELAY; }
            else if (opt == NetOption::ReuseAddress) name = SO_REUSEADDR;
            else if (opt == NetOption::KeepAlive) name = SO_KEEPALIVE;
            else if (opt == NetOption::IPv6Only) { level = IPPROTO_IPV6; name = IPV6_V6ONLY; }
            else name = SO_BROADCAST;
            rc = ::getsockopt(fd, level, name, &ival, &len);
            if (rc == 0) *value = ival;
            break;
        }
        case NetOption::ReceiveTimeout:
        case NetOption::SendTimeout: {
            struct timeval tv;
            std::memset(&tv, 0, sizeof(tv));
            socklen_t len = sizeof(tv);
            rc = ::getsockopt(fd, SOL_SOCKET,
                              opt == NetOption::ReceiveTimeout ? SO_RCVTIMEO : SO_SNDTIMEO,
                              &tv, &len);
            if (rc == 0) *value = static_cast<CHAOS_IL2CPP_INT32>(tv.tv_sec * 1000 + tv.tv_usec / 1000);
            break;
        }
        case NetOption::Linger: {
            struct linger lg;
            std::memset(&lg, 0, sizeof(lg));
            socklen_t len = sizeof(lg);
            rc = ::getsockopt(fd, SOL_SOCKET, SO_LINGER, &lg, &len);
            if (rc == 0) *value = lg.l_onoff != 0 ? static_cast<CHAOS_IL2CPP_INT32>(lg.l_linger) : 0;
            break;
        }
        default: SetLastNative(EOPNOTSUPP); return CoarseError();
    }
    if (rc != 0) { SetLastNative(errno); return CoarseError(); }
    return NetError::None;
}

NetError NetSocketGetLocalAddress(SocketHandle* handle, NetAddress* addr) noexcept
{
    if (handle == nullptr || addr == nullptr || handle->fd < 0) {
        SetLastNative(EINVAL);
        return NetError::Unsupported;
    }
    sockaddr_storage ss;
    std::memset(&ss, 0, sizeof(ss));
    socklen_t len = sizeof(ss);
    if (::getsockname(static_cast<int>(handle->fd), reinterpret_cast<sockaddr*>(&ss), &len) != 0) {
        SetLastNative(errno);
        return CoarseError();
    }
    FillNetAddress(reinterpret_cast<sockaddr*>(&ss), static_cast<int>(len), addr);
    return NetError::None;
}

SocketError NetSocketLastError() noexcept
{
    return NativeCodeToSocketError(tls_last_native);
}

// ── async submission (mirrors WSASend/WSARecv + FinalizeOp) ────────────────
NetError BeginIo(SocketHandle* handle, const CHAOS_IL2CPP_UINT8* data,
                 CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                 async::AsyncOp* op, bool write) noexcept
{
    (void)flags;
    if (handle->fd < 0 || op == nullptr || (len > 0 && data == nullptr)) {
        SetLastNative(EINVAL);
        return CoarseError();
    }
    if (len <= 0) {   // win32 WSABUF 0-length: completes immediately
        async::PostCompletion(new Completion{op, 0, NetError::None});
        return NetError::None;
    }
    auto* ctx = new PosixOpContext;
    ctx->fd = static_cast<int>(handle->fd);
    ctx->write = write;
    ctx->op = op;
    op->platform = ctx;
    {
        std::lock_guard<std::mutex> lk(g_ops_mutex);
        g_ops.push_back(ctx);
    }
    if (!RegisterBackend(ctx)) {
        SetLastNative(errno);
        {
            std::lock_guard<std::mutex> lk2(g_ops_mutex);
            auto it = std::find(g_ops.begin(), g_ops.end(), ctx);
            if (it != g_ops.end()) g_ops.erase(it);
        }
        Finish(ctx, 0, CoarseError());
        return NetError::None;
    }
    WakeLoop();
    return NetError::None;
}

NetError NetSocketBeginSend(SocketHandle* handle, const CHAOS_IL2CPP_UINT8* data,
                            CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                            async::AsyncOp* op) noexcept
{
    if (!g_loop_up.load(std::memory_order_acquire) || handle == nullptr) {
        SetLastNative(EINVAL);
        return CoarseError();
    }
    return BeginIo(handle, data, len, flags, op, true);
}

NetError NetSocketBeginRecv(SocketHandle* handle, CHAOS_IL2CPP_UINT8* data,
                            CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                            async::AsyncOp* op) noexcept
{
    if (!g_loop_up.load(std::memory_order_acquire) || handle == nullptr) {
        SetLastNative(EINVAL);
        return CoarseError();
    }
    return BeginIo(handle, data, len, flags, op, false);
}

NetError NetSocketCancelOp(SocketHandle* handle, async::AsyncOp* op,
                           NetError reason) noexcept
{
    (void)reason;
    if (op == nullptr || op->owner == nullptr || op->owner->fd < 0) return NetError::None;
    PosixOpContext* ctx = ClaimAndErase(static_cast<int>(op->owner->fd));
    if (ctx == nullptr) return NetError::None;   // loop already completing it
    UnregisterBackend(ctx);
    Finish(ctx, 0, op->error);                   // reason recorded by Layer C
    return NetError::None;
}

// ── exported lifecycle (NetStartup/NetCleanup, shared with Layer A) ────────
NetError NetStartup() noexcept
{
    if (g_refcount == 0 && !StartLoop()) {
        SetLastNative(errno);
        return CoarseError();
    }
    ++g_refcount;
    SetLastNative(0);
    return NetError::None;
}

void NetCleanup() noexcept
{
    if (g_refcount > 0 && --g_refcount == 0) StopLoop();
}

}  // namespace chaos::net