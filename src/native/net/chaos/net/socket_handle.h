#pragma once
#include <chaos/net/abi.h>

// Layer B socket state and handle table (GetOrCreate lazy binding).
namespace chaos::net {

enum class SocketState : CHAOS_IL2CPP_UINT8 {
    Created = 0,
    Bound,
    Connected,
    Listening,
    Closed,
    Aborted,
};

struct SocketHandle {
    CHAOS_IL2CPP_INTPTR managed_handle;  // managed Socket instance (wrapper slot 0)
    CHAOS_IL2CPP_INTPTR fd;              // native descriptor; -1 while unbound
    SocketState state;
    CHAOS_IL2CPP_INT32 family;
    CHAOS_IL2CPP_INT32 type;
    CHAOS_IL2CPP_INT32 protocol;

    struct Options {
        // Generic/default timeout (connect, and fallback for IO when the
        // per-direction timeouts are 0).  0 == infinite wait.
        CHAOS_IL2CPP_INT32 timeout_ms;
        CHAOS_IL2CPP_INT32 send_timeout_ms;    // NetOption::SendTimeout
        CHAOS_IL2CPP_INT32 receive_timeout_ms; // NetOption::ReceiveTimeout
        CHAOS_IL2CPP_UINT8 no_delay;
        CHAOS_IL2CPP_UINT8 reuse_addr;
        CHAOS_IL2CPP_UINT8 keep_alive;
        CHAOS_IL2CPP_UINT8 ipv6_only;
        CHAOS_IL2CPP_UINT8 enable_broadcast;   // NetOption::EnableBroadcast
        CHAOS_IL2CPP_INT32 linger_sec;
        CHAOS_IL2CPP_UINT8 linger_enabled;
    } opts;
    // Single outstanding async op (Layer C, NT-7).  .NET Socket enforces
    // one async op per socket at a time; nullptr when idle.  Points at
    // chaos::net::async::AsyncOp (async_op.h includes this header).
    void* pending_op{nullptr};
};

// GetOrCreate lazy binding for a managed Socket instance.
// Returns nullptr only on capacity exhaustion.
SocketHandle* SocketForInstance(CHAOS_IL2CPP_INTPTR managed_handle) noexcept;

}  // namespace chaos::net
