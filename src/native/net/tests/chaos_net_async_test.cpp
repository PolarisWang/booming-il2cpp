// chaos_net_async_test -- NT-7: IOCP async completion pipeline.
//
// Covers the async finish-chain on loopback:
//   1. async send / async recv byte roundtrip (client -> server -> client),
//      with on_done observed to run OFF the main thread (IOCP worker ->
//      dispatcher -> listener thread).
//   2. cancel of a pending recv: op completes with the recorded reason.
//   3. timeout watchdog: pending recv completes with NetError::TimedOut.
// Build: cmake -S src/native/net/tests -B build/net-tests -A x64
//        cmake --build build/net-tests --config RelWithDebInfo
#include <chaos/net/net_api.h>
#include <chaos/net/async_op.h>
#include <cstdio>
#include <cstring>
#include <atomic>
#include <thread>
#include <chrono>

namespace async = chaos::net::async;
using chaos::net::SocketHandle;
using chaos::net::SocketState;

static int g_failures = 0;
static std::atomic<unsigned> g_main_thread_id{0};
static std::atomic<int> g_done_count{0};
static std::atomic<unsigned> g_done_thread_id{0};

void MarkDone() noexcept
{
    g_done_thread_id = (unsigned)std::hash<std::thread::id>{}(std::this_thread::get_id());
    ++g_done_count;
}

#define CHECK(cond, ...)                                                     \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::printf("[NET-ASYNC] FAIL %s:%d: ", __FILE__, __LINE__);     \
            std::printf(__VA_ARGS__);                                        \
            std::printf("\n");                                               \
            ++g_failures;                                                    \
        }                                                                    \
    } while (0)

// Dispatcher that runs completions on a fresh thread -- proves on_done is
// invoked off the submission thread (runtime binary installs the real
// ThreadPoolQueueUserWorkItem here instead).
void SpawnThreadDispatcher(void (*fn)(void*), void* arg) noexcept
{
    std::thread runner([fn, arg]() { fn(arg); });
    runner.detach();
}

static bool WaitForOp(async::AsyncOp* op, int timeout_ms) noexcept
{
    const auto deadline = std::chrono::steady_clock::now() +
                          std::chrono::milliseconds(timeout_ms);
    while (std::chrono::steady_clock::now() < deadline) {
        const auto st = op->state.load(std::memory_order_relaxed);
        if (st != async::OpState::Pending) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return false;
}

static bool TestAsyncRoundtrip() noexcept
{
    std::printf("[NET-ASYNC] async roundtrip...\n");

    SocketHandle listener{};
    listener.managed_handle = 0;
    listener.fd = -1;
    listener.state = SocketState::Created;
    SocketHandle server{};
    server.managed_handle = 0;
    server.fd = -1;
    server.state = SocketState::Created;
    SocketHandle client{};
    client.managed_handle = 0;
    client.fd = -1;
    client.state = SocketState::Created;

    if (chaos::net::NetSocketCreate(chaos::net::kAddressFamilyInet,
                                    chaos::net::kSocketTypeStream,
                                    chaos::net::kProtocolTcp, &listener) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL listener create\n");
        return false;
    }
    if (chaos::net::NetSocketBind(&listener, chaos::net::LoopbackV4(0)) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL listener bind\n");
        return false;
    }
    if (chaos::net::NetSocketListen(&listener, 8) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL listener listen\n");
        return false;
    }
    chaos::net::NetAddress addr{};
    if (chaos::net::NetSocketGetLocalAddress(&listener, &addr) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL get local addr\n");
        return false;
    }

    // Client connects synchronously.
    if (chaos::net::NetSocketCreate(chaos::net::kAddressFamilyInet,
                                    chaos::net::kSocketTypeStream,
                                    chaos::net::kProtocolTcp, &client) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL client create\n");
        return false;
    }
    if (chaos::net::NetSocketConnect(&client, addr, 5000) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL client connect\n");
        return false;
    }
    if (chaos::net::NetSocketAccept(&listener, &server, nullptr, 5000) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL accept\n");
        return false;
    }

    // Async client -> server send; async server recv.
    static constexpr char kPing[] = "ping";
    static constexpr char kPong[] = "pong";
    unsigned char send_buf[8]{};
    unsigned char recv_buf[8]{};
    std::memcpy(send_buf, kPing, 4);

    async::AsyncOp send_op{};
    send_op.on_done = [](async::AsyncOp* op) noexcept { MarkDone(); };
    send_op.kind = async::OpKind::Send;

    async::AsyncOp recv_op{};
    recv_op.on_done = [](async::AsyncOp* op) noexcept { MarkDone(); };
    recv_op.kind = async::OpKind::Receive;

    // Setup: server recv, then client send.
    auto err = async::NetAsyncBeginRecv(&server, recv_buf, 4, 0, &recv_op);
    if (err != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL begin recv: %d\n", (int)err);
        return false;
    }
    err = async::NetAsyncBeginSend(&client, send_buf, 4, 0, &send_op);
    if (err != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL begin send: %d\n", (int)err);
        return false;
    }

    const bool send_done = WaitForOp(&send_op, 3000);
    const bool recv_done = WaitForOp(&recv_op, 3000);
    CHECK(send_done && send_op.state.load() == async::OpState::Completed,
          "send op completed (done=%d state=%d)", send_done, (int)send_op.state.load());
    CHECK(send_op.transferred == 4, "send transferred=%u want 4", send_op.transferred);
    CHECK(recv_done && recv_op.state.load() == async::OpState::Completed,
          "recv op completed (done=%d state=%d)", recv_done, (int)recv_op.state.load());
    CHECK(recv_op.transferred == 4, "recv transferred=%u want 4", recv_op.transferred);
    CHECK(std::memcmp(recv_buf, kPing, 4) == 0, "server got 'ping'");
    CHECK(g_done_count.load() == 2, "both on_done fired (n=%d)", g_done_count.load());
    CHECK(g_done_thread_id.load() != g_main_thread_id.load(),
          "completions ran off main thread (done=%u main=%u)",
          g_done_thread_id.load(), g_main_thread_id.load());

    // Echo back server -> client.
    std::memcpy(send_buf, kPong, 4);
    async::AsyncOp echo_send{};
    echo_send.kind = async::OpKind::Send;
    echo_send.on_done = [](async::AsyncOp* op) noexcept { MarkDone(); };
    async::AsyncOp echo_recv{};
    echo_recv.kind = async::OpKind::Receive;
    echo_recv.on_done = [](async::AsyncOp* op) noexcept { MarkDone(); };

    err = async::NetAsyncBeginRecv(&client, recv_buf, 4, 0, &echo_recv);
    CHECK(err == chaos::net::NetError::None, "echo begin recv err=%d", (int)err);
    err = async::NetAsyncBeginSend(&server, send_buf, 4, 0, &echo_send);
    CHECK(err == chaos::net::NetError::None, "echo begin send err=%d", (int)err);
    CHECK(WaitForOp(&echo_send, 3000), "echo send completed");
    CHECK(WaitForOp(&echo_recv, 3000), "echo recv completed");
    CHECK(echo_send.transferred == 4 && echo_recv.transferred == 4,
          "echo transferred %u/%u", echo_send.transferred, echo_recv.transferred);
    CHECK(std::memcmp(recv_buf, kPong, 4) == 0, "client got 'pong'");

    chaos::net::NetSocketClose(&client);
    chaos::net::NetSocketClose(&server);
    chaos::net::NetSocketClose(&listener);
    return true;
}

static bool TestAsyncCancel() noexcept
{
    std::printf("[NET-ASYNC] async cancel...\n");
    SocketHandle a{};
    SocketHandle b{};
    a.managed_handle = 0; a.fd = -1; a.state = SocketState::Created;
    b.managed_handle = 0; b.fd = -1; b.state = SocketState::Created;

    if (chaos::net::NetSocketCreate(chaos::net::kAddressFamilyInet,
                                    chaos::net::kSocketTypeStream,
                                    chaos::net::kProtocolTcp, &a) != chaos::net::NetError::None) return false;
    if (chaos::net::NetSocketBind(&a, chaos::net::LoopbackV4(0)) != chaos::net::NetError::None) return false;
    if (chaos::net::NetSocketListen(&a, 8) != chaos::net::NetError::None) return false;
    chaos::net::NetAddress addr{};
    if (chaos::net::NetSocketGetLocalAddress(&a, &addr) != chaos::net::NetError::None) return false;
    if (chaos::net::NetSocketCreate(chaos::net::kAddressFamilyInet,
                                    chaos::net::kSocketTypeStream,
                                    chaos::net::kProtocolTcp, &b) != chaos::net::NetError::None) return false;
    if (chaos::net::NetSocketConnect(&b, addr, 5000) != chaos::net::NetError::None) return false;
    SocketHandle peer{};
    peer.managed_handle = 0; peer.fd = -1; peer.state = SocketState::Created;
    if (chaos::net::NetSocketAccept(&a, &peer, nullptr, 5000) != chaos::net::NetError::None) return false;

    unsigned char buf[4]{};
    async::AsyncOp recv_op{};
    recv_op.kind = async::OpKind::Receive;
    recv_op.on_done = [](async::AsyncOp* op) noexcept {};

    if (async::NetAsyncBeginRecv(&peer, buf, 4, 0, &recv_op) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL cancel: begin recv\n");
        return false;
    }
    // No data arrives; cancel with an explicit reason.
    if (async::NetAsyncCancel(&recv_op, chaos::net::NetError::ConnectionReset) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL cancel: NetAsyncCancel\n");
        return false;
    }
    CHECK(WaitForOp(&recv_op, 3000), "cancelled op completed");
    CHECK(recv_op.state.load() == async::OpState::Cancelled,
          "cancel state=%d want Cancelled", (int)recv_op.state.load());
    CHECK(recv_op.error == chaos::net::NetError::ConnectionReset,
          "cancel error=%d want ConnectionReset", (int)recv_op.error);
    CHECK(peer.pending_op == nullptr, "pending_op cleared after cancel");

    chaos::net::NetSocketClose(&b);
    chaos::net::NetSocketClose(&peer);
    chaos::net::NetSocketClose(&a);
    return true;
}

static bool TestAsyncTimeout() noexcept
{
    std::printf("[NET-ASYNC] async timeout...\n");
    SocketHandle a{};
    SocketHandle b{};
    a.managed_handle = 0; a.fd = -1; a.state = SocketState::Created;
    b.managed_handle = 0; b.fd = -1; b.state = SocketState::Created;

    if (chaos::net::NetSocketCreate(chaos::net::kAddressFamilyInet,
                                    chaos::net::kSocketTypeStream,
                                    chaos::net::kProtocolTcp, &a) != chaos::net::NetError::None) return false;
    if (chaos::net::NetSocketBind(&a, chaos::net::LoopbackV4(0)) != chaos::net::NetError::None) return false;
    if (chaos::net::NetSocketListen(&a, 8) != chaos::net::NetError::None) return false;
    chaos::net::NetAddress addr{};
    if (chaos::net::NetSocketGetLocalAddress(&a, &addr) != chaos::net::NetError::None) return false;
    if (chaos::net::NetSocketCreate(chaos::net::kAddressFamilyInet,
                                    chaos::net::kSocketTypeStream,
                                    chaos::net::kProtocolTcp, &b) != chaos::net::NetError::None) return false;
    if (chaos::net::NetSocketConnect(&b, addr, 5000) != chaos::net::NetError::None) return false;
    SocketHandle peer{};
    peer.managed_handle = 0; peer.fd = -1; peer.state = SocketState::Created;
    if (chaos::net::NetSocketAccept(&a, &peer, nullptr, 5000) != chaos::net::NetError::None) return false;

    unsigned char buf[4]{};
    async::AsyncOp recv_op{};
    recv_op.kind = async::OpKind::Receive;
    recv_op.on_done = [](async::AsyncOp* op) noexcept {};

    if (async::NetAsyncBeginRecv(&peer, buf, 4, 0, &recv_op) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL timeout: begin recv\n");
        return false;
    }
    if (async::NetAsyncStartTimeout(&recv_op, 150) != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL timeout: arm watchdog\n");
        return false;
    }
    CHECK(WaitForOp(&recv_op, 3000), "timed-out op completed");
    CHECK(recv_op.state.load() == async::OpState::TimedOut,
          "timeout state=%d want TimedOut", (int)recv_op.state.load());
    CHECK(recv_op.error == chaos::net::NetError::TimedOut,
          "timeout error=%d want TimedOut", (int)recv_op.error);

    chaos::net::NetSocketClose(&b);
    chaos::net::NetSocketClose(&peer);
    chaos::net::NetSocketClose(&a);
    return true;
}

int main()
{
    std::printf("[NET-ASYNC] chaos_net async tests starting\n");
    g_main_thread_id = (unsigned)std::hash<std::thread::id>{}(std::this_thread::get_id());
    if (chaos::net::NetStartup() != chaos::net::NetError::None) {
        std::printf("[NET-ASYNC] FAIL NetStartup\n");
        return 1;
    }
    async::SetCompletionDispatcher(SpawnThreadDispatcher);

    if (TestAsyncRoundtrip()) std::printf("[NET-ASYNC] PASS: async roundtrip\n");
    else std::printf("[NET-ASYNC] FAIL: async roundtrip\n");
    if (TestAsyncCancel()) std::printf("[NET-ASYNC] PASS: async cancel\n");
    else std::printf("[NET-ASYNC] FAIL: async cancel\n");
    if (TestAsyncTimeout()) std::printf("[NET-ASYNC] PASS: async timeout\n");
    else std::printf("[NET-ASYNC] FAIL: async timeout\n");

    chaos::net::NetCleanup();
    if (g_failures == 0) {
        std::printf("[NET-ASYNC] ALL PASS\n");
        return 0;
    }
    std::printf("[NET-ASYNC] FAILURES: %d\n", g_failures);
    return 1;
}
