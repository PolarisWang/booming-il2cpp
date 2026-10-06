// Layer B -- SocketHandle state machine + synchronous semantics.
//
// Platform-independent: all transport work goes through net_api.h (Layer D).
// Layer B owns the handle table and the state transitions; Layer D owns the
// native descriptor and winsock/epoll specifics (net_impl_win32.cpp).
#include <chaos/net/socket_handle.h>
#include <chaos/net/error.h>
#include <chaos/net/net_api.h>
#include <cstddef>
#include <deque>

namespace chaos::net {
namespace {

// Growable handle table (GetOrCreate lazy binding).  The managed Socket
// has no native descriptor until the first operation binds one (ctor is
// not registered -- instances come from test fixtures), so the first Send
// on an instance creates a Created-state handle and the unconnected
// contract (10057) drives it.
//
// The table is unbounded: slots are kept for the lifetime of the process
// (there is no managed-object finalizer hook to reclaim them), and
// verification runs (foundation-dll benchmark / hotupdate) create a fresh
// managed Socket instance per dispatch, which can reach tens of thousands
// of every-slot-eaten addresses across phases.  A deque preserves element
// references across push_back, so handles handed out to callers stay
// valid as the table grows.
struct Entry {
    CHAOS_IL2CPP_INTPTR managed_handle;  // 0 == unbound entry
    SocketHandle handle;
};

std::deque<Entry> g_table;

}  // namespace

SocketHandle* SocketForInstance(CHAOS_IL2CPP_INTPTR managed_handle) noexcept
{
    if (managed_handle == 0) {
        return nullptr;
    }
    // Existing binding?
    for (Entry& e : g_table) {
        if (e.managed_handle == managed_handle) {
            return &e.handle;
        }
    }
    // GetOrCreate: append a fresh Created-state entry.
    {
        Entry& e = g_table.emplace_back();
        e.managed_handle = managed_handle;
        SocketHandle& h = e.handle;
        h.managed_handle = managed_handle;
        h.fd = static_cast<CHAOS_IL2CPP_INTPTR>(-1);
        h.state = SocketState::Created;
        h.family = 0;
        h.type = 0;
        h.protocol = 0;
        h.opts.timeout_ms = 0;
        h.opts.send_timeout_ms = 0;
        h.opts.receive_timeout_ms = 0;
        h.opts.no_delay = 0;
        h.opts.reuse_addr = 0;
        h.opts.keep_alive = 0;
        h.opts.ipv6_only = 0;
        h.opts.enable_broadcast = 0;
        h.opts.linger_sec = 0;
        h.opts.linger_enabled = 0;
        return &h;
    }
}

CHAOS_IL2CPP_INT32 SocketSendBytes(SocketHandle* handle,
                                   const CHAOS_IL2CPP_UINT8* bytes,
                                   CHAOS_IL2CPP_INTPTR byteLength,
                                   CHAOS_IL2CPP_INT32 offset,
                                   CHAOS_IL2CPP_INT32 size,
                                   CHAOS_IL2CPP_INT32 flags,
                                   CHAOS_IL2CPP_INT32* error) noexcept
{
    if (error != nullptr) {
        *error = ToInt(SocketError::Success);
    }
    if (handle == nullptr || handle->state == SocketState::Closed ||
        handle->state == SocketState::Aborted) {
        if (error != nullptr) {
            *error = ToInt(SocketError::NotSocket);
        }
        return 0;
    }
    // Validate the caller window against the buffer extents.  The wrapper
    // already unpacked the managed byte[] to (bytes, byteLength); offset/size
    // are the managed-visible window into it.
    if (offset < 0 || size < 0 ||
        byteLength < 0 ||
        (byteLength > 0 && (bytes == nullptr)) ||
        static_cast<CHAOS_IL2CPP_INTPTR>(offset) > byteLength ||
        static_cast<CHAOS_IL2CPP_INTPTR>(size) > byteLength - offset) {
        if (error != nullptr) {
            *error = ToInt(SocketError::MessageSize);
        }
        return 0;
    }
    if (handle->state != SocketState::Connected || handle->fd < 0) {
        if (error != nullptr) {
            *error = ToInt(SocketError::NotConnected);
        }
        return 0;
    }
    // Layer D: synchronous send (waits internally on non-blocking fd).
    const CHAOS_IL2CPP_INT32 send_timeout =
        handle->opts.send_timeout_ms != 0 ? handle->opts.send_timeout_ms : handle->opts.timeout_ms;
    CHAOS_IL2CPP_INT32 sent = 0;
    const NetError err = NetSocketSend(handle,
                                       size > 0 ? bytes + offset : nullptr,
                                       size, flags, &sent,
                                       send_timeout);
    if (err != NetError::None) {
        if (error != nullptr) {
            *error = ToInt(NetSocketLastError());
        }
        return 0;
    }
    return sent;
}

CHAOS_IL2CPP_INT32 SocketReceiveBytes(SocketHandle* handle,
                                      CHAOS_IL2CPP_UINT8* bytes,
                                      CHAOS_IL2CPP_INTPTR byteLength,
                                      CHAOS_IL2CPP_INT32 offset,
                                      CHAOS_IL2CPP_INT32 size,
                                      CHAOS_IL2CPP_INT32 flags,
                                      CHAOS_IL2CPP_INT32* error) noexcept
{
    if (error != nullptr) {
        *error = ToInt(SocketError::Success);
    }
    if (handle == nullptr || handle->state == SocketState::Closed ||
        handle->state == SocketState::Aborted) {
        if (error != nullptr) {
            *error = ToInt(SocketError::NotSocket);
        }
        return 0;
    }
    // Validate the caller window against the buffer extents (mirror send).
    if (offset < 0 || size < 0 ||
        byteLength < 0 ||
        (byteLength > 0 && (bytes == nullptr)) ||
        static_cast<CHAOS_IL2CPP_INTPTR>(offset) > byteLength ||
        static_cast<CHAOS_IL2CPP_INTPTR>(size) > byteLength - offset) {
        if (error != nullptr) {
            *error = ToInt(SocketError::MessageSize);
        }
        return 0;
    }
    // NT-4 unconnected contract: receive on an unconnected socket reports
    // NotConnected (10057) and returns 0 (matches .NET / WSAENOTCONN).
    if (handle->state != SocketState::Connected || handle->fd < 0) {
        if (error != nullptr) {
            *error = ToInt(SocketError::NotConnected);
        }
        return 0;
    }
    // Layer D: synchronous receive (waits internally on non-blocking fd).
    const CHAOS_IL2CPP_INT32 recv_timeout =
        handle->opts.receive_timeout_ms != 0 ? handle->opts.receive_timeout_ms : handle->opts.timeout_ms;
    CHAOS_IL2CPP_INT32 got = 0;
    const NetError err = NetSocketRecv(handle,
                                       size > 0 ? bytes + offset : nullptr,
                                       size, flags, &got,
                                       recv_timeout);
    if (err != NetError::None) {
        if (error != nullptr) {
            *error = ToInt(NetSocketLastError());
        }
        return 0;
    }
    return got;
}

}  // namespace chaos::net
