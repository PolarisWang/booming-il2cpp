// Layer C -- async op orchestration (NT-7).
//
// Platform-neutral: single outstanding op per socket (.NET Socket semantic),
// begin/cancel/timeout entry points, completion routing to Layer G.
// The actual overlapped submission lives in Layer D (net_impl_win32.cpp).
#include <chaos/net/async_op.h>
#include <chaos/net/net_api.h>

#include <thread>
#include <chrono>

namespace chaos::net::async {
namespace {

bool CanSubmit(const SocketHandle* h) noexcept
{
    return h != nullptr && h->fd >= 0 && h->pending_op == nullptr;
}

NetError SubmitOp(SocketHandle* h, AsyncOp* op, OpKind kind,
                  CHAOS_IL2CPP_UINT8* buf, CHAOS_IL2CPP_INT32 len,
                  CHAOS_IL2CPP_INT32 flags, bool is_send) noexcept
{
    if (!CanSubmit(h) || op == nullptr) {
        return NetError::InvalidSocketHandle;
    }
    op->kind = kind;
    op->owner = h;
    op->buf = buf;
    op->len = len;
    op->transferred = 0;
    op->error = NetError::None;
    op->socket_error = SocketError::Success;
    op->state.store(OpState::Pending, std::memory_order_relaxed);
    h->pending_op = op;
    const NetError err = is_send ? NetSocketBeginSend(h, buf, len, flags, op)
                                 : NetSocketBeginRecv(h, buf, len, flags, op);
    if (err != NetError::None) {
        h->pending_op = nullptr;
        op->state.store(OpState::Cancelled, std::memory_order_relaxed);
        op->error = err;
    }
    return err;
}

}  // namespace

NetError NetAsyncBeginSend(SocketHandle* h, const CHAOS_IL2CPP_UINT8* data,
                           CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                           AsyncOp* op) noexcept
{
    // Caller-owned buffer; submitted as-is (v1 copy semantics live in the
    // managed wrapper layer, see architecture doc §7.4).
    return SubmitOp(h, op, OpKind::Send, const_cast<CHAOS_IL2CPP_UINT8*>(data),
                    len, flags, /*is_send=*/true);
}

NetError NetAsyncBeginRecv(SocketHandle* h, CHAOS_IL2CPP_UINT8* data,
                           CHAOS_IL2CPP_INT32 len, CHAOS_IL2CPP_INT32 flags,
                           AsyncOp* op) noexcept
{
    return SubmitOp(h, op, OpKind::Receive, data, len, flags, /*is_send=*/false);
}

NetError NetAsyncCancel(AsyncOp* op, NetError reason) noexcept
{
    if (op == nullptr || op->owner == nullptr) {
        return NetError::InvalidSocketHandle;
    }
    if (op->state.load(std::memory_order_relaxed) != OpState::Pending) {
        return NetError::None;  // already completed; nothing to cancel
    }
    op->error = reason;
    // Layer D performs the actual Cancellation (CancelIoEx).  On completion
    // the worker maps the aborted status onto op->error and dispatches.
    return NetSocketCancelOp(op->owner, op, reason);
}

NetError NetAsyncStartTimeout(AsyncOp* op, CHAOS_IL2CPP_UINT32 timeout_ms) noexcept
{
    if (op == nullptr) {
        return NetError::InvalidSocketHandle;
    }
    // Spike: fire-and-forget watchdog thread.  Runtime swap point: register a
    // timer_queue entry instead (architecture doc §7.1: pending op 登记
    // timer_queue → 到期投 TimedOut 完成).
    std::thread watchdog([op, timeout_ms]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
        if (op->state.load(std::memory_order_relaxed) == OpState::Pending) {
            NetAsyncCancel(op, NetError::TimedOut);
        }
    });
    watchdog.detach();
    return NetError::None;
}

}  // namespace chaos::net::async
