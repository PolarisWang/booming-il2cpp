// Layer G -- completion dispatch (NT-7).
//
// Transport driver (Layer D, IOCP worker) hands a heap-allocated Completion
// to PostCompletion; the configured dispatcher decides where on_done runs:
//   * runtime binary -> ThreadPoolQueueUserWorkItem(OnNetCompletion, c)
//   * tests           -> std::thread spawner (prove non-main-thread runs)
//   * default         -> direct synchronous invocation (simplest)
// OnNetCompletion: copies completion results into the AsyncOp, advances its
// state, clears the socket's pending_op slot, runs on_done, frees the
// Completion.  Because the runner thread may be a pool thread, no call in
// this file blocks.
#include <chaos/net/async_op.h>

namespace chaos::net::async {
namespace {

void DirectDispatcher(void (*fn)(void*), void* arg) noexcept
{
    fn(arg);
}

CompletionDispatcher g_dispatcher = DirectDispatcher;

}  // namespace

void SetCompletionDispatcher(CompletionDispatcher d) noexcept
{
    g_dispatcher = d != nullptr ? d : DirectDispatcher;
}

void OnNetCompletion(void* arg) noexcept
{
    auto* c = static_cast<Completion*>(arg);
    AsyncOp* op = c->op;
    op->transferred = c->bytes;
    op->error = c->error;
    switch (c->error) {
        case NetError::None:      op->state.store(OpState::Completed, std::memory_order_relaxed); break;
        case NetError::TimedOut:  op->state.store(OpState::TimedOut, std::memory_order_relaxed); break;
        default:                  op->state.store(OpState::Cancelled, std::memory_order_relaxed); break;
    }
    if (op->owner != nullptr) {
        op->owner->pending_op = nullptr;
    }
    if (op->on_done != nullptr) {
        op->on_done(op);
    }
    delete c;
}

void PostCompletion(Completion* c) noexcept
{
    g_dispatcher(OnNetCompletion, c);
}

}  // namespace chaos::net::async
