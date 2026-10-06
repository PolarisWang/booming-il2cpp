// Layer C/G -- async operation records + completion dispatch (NT-7).
//
// Design follows docs/dev/in-progress/net-cpp-architecture.md §7:
//   Layer C async_ops.cpp -- platform-neutral orchestration (single op per
//                            socket, begin/cancel/timeout semantics)
//   Layer G completion.cpp -- Completion -> dispatcher -> on_done
//
// AsyncOp.buf is caller-owned memory that must stay alive until on_done
// fires (v1 copy semantics; zero-copy rental is a later upgrade point).
// Completion objects are heap-allocated by the transport layer and freed
// by OnNetCompletion after on_done returns.
#pragma once
#include <atomic>
#include <chaos/net/net.h>
#include <chaos/net/socket_handle.h>
#include <chaos/net/error.h>
#include <chaos/native_types.h>

namespace chaos::net::async {

// .NET SocketAsyncEventArgs / Begin-* op kinds.
enum class OpKind : CHAOS_IL2CPP_UINT8 {
    Accept = 0,
    Connect = 1,
    Receive = 2,
    Send = 3,
    RecvFrom = 4,
    SendTo = 5,
};

enum class OpState : CHAOS_IL2CPP_UINT8 {
    Idle = 0,
    Pending = 1,
    Completed = 2,
    Cancelled = 3,
    TimedOut = 4,
};

struct AsyncOp {
    OpKind kind{OpKind::Accept};
    SocketHandle* owner{nullptr};            // borrowed; must outlive the op
    CHAOS_IL2CPP_UINT8* buf{nullptr};        // caller-owned native buffer
    CHAOS_IL2CPP_INT32 len{0};
    CHAOS_IL2CPP_UINT32 transferred{0};
    NetError error{NetError::None};          // coarse async result
    SocketError socket_error{SocketError::Success};  // fine-grained
    CHAOS_IL2CPP_INTPTR completion_slot{0};  // SAEA / Task completion slot (short-lived)
    void (*on_done)(AsyncOp*) noexcept{nullptr};  // Layer G dispatch target
    std::atomic<OpState> state{OpState::Idle};
    void* platform{nullptr};                 // transport-owned context (win: IocpOpContext*)
};

// Heap-allocated completion record flowing from the transport driver to
// Layer G; freed by OnNetCompletion after on_done returns.
struct Completion {
    AsyncOp* op{nullptr};
    CHAOS_IL2CPP_UINT32 bytes{0};
    NetError error{NetError::None};
};

// ── Layer C orchestration (async_ops.cpp) ─────────────────────────────
// Submits an async send/recv on the socket.  Fails with NetError::WouldBlock
// when another op is already pending on the same socket (single-op-per-socket
// semantics matching .NET Socket).  The op completes asynchronously through
// on_done with op->state completed/cancelled/timed-out.
NetError NetAsyncBeginSend(SocketHandle* h, const CHAOS_IL2CPP_UINT8* data,
                           CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                           AsyncOp* op) noexcept;
NetError NetAsyncBeginRecv(SocketHandle* h, CHAOS_IL2CPP_UINT8* data,
                           CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                           AsyncOp* op) noexcept;
// Cancels a pending op.  The op still completes through on_done, with
// state Cancelled and reason as its error.  No-op when already completed.
NetError NetAsyncCancel(AsyncOp* op, NetError reason) noexcept;
// Arms a fire-and-forget watchdog that cancels the op with NetError::TimedOut
// once timeout_ms elapses and the op is still pending.  Safe to call only
// once per op (spike: std::thread; runtime: timer_queue swap point).
NetError NetAsyncStartTimeout(AsyncOp* op, CHAOS_IL2CPP_UINT32 timeout_ms) noexcept;

// ── Layer G (completion.cpp) ──────────────────────────────────────────
// Test binaries install a dispatcher that runs completions off the worker
// thread; the runtime binary installs ThreadPoolQueueUserWorkItem (the
// HillClimbing pool entry).  Default = direct synchronous invocation.
using CompletionDispatcher = void (*)(void (*)(void*), void*);
void SetCompletionDispatcher(CompletionDispatcher d) noexcept;
void PostCompletion(Completion* c) noexcept;

}  // namespace chaos::net::async
