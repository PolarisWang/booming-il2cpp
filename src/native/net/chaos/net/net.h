#pragma once
#include <chaos/native_types.h>

// chaos_net top-level lifecycle and NetError codes.
//
// Layer model (see docs/dev/in-progress/net-cpp-architecture.md):
//   Layer A  entry.cpp        -- all ChaosNet* entry points (shape dispatch)
//   Layer B  socket_ops.cpp   -- SocketHandle lifecycle + sync semantics
//   Layer C  async_ops.cpp    -- NT-6/7 (IOCP/epoll completions)
//   Layer D  net_impl_win32/posix -- winsock2 / sockets + epoll

namespace chaos::net {

enum class NetError : CHAOS_IL2CPP_INT32 {
    None = 0,
    NotInitialized = 1,
    InvalidSocketHandle = 2,
    NotConnected = 3,     // maps to SocketError::NotConnected (10057)
    WouldBlock = 4,
    TimedOut = 5,
    ConnectionReset = 6,
    Unsupported = 7,
    DnsFailure = 8,    // NT-9: DNS resolution failed (no such host)
};

// Brings the native net layer up (WSAStartup on Windows in later stages).
// Reference-counted; every ChaosNetInit must pair with ChaosNetShutdown.
bool ChaosNetInit() noexcept;
void ChaosNetShutdown() noexcept;

}  // namespace chaos::net
