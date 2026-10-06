// chaos_net_eyeballs_test.cpp -- NT-23: Happy-Eyeballs connect (RFC 8305
// simplified) fault-tolerance tests.
//
// Verifies the ordered multi-address connect path NetSocketConnectList:
//   1) the first live address is used immediately (no artificial wait);
//   2) an IPv4-only listener hint falls through when the v4 endpoint is
//      refused and the IPv6 candidate succeeds (and vice versa);
//   3) a non-responding first candidate is bounded by the short per-attempt
//      time out (kHappyEyeballAttemptMs clamp) and the next one is tried;
//   4) useHappyEyeballs=0 is legacy single-address behavior (no fallback);
//   5) when every candidate fails the last error is returned and the out
//      handle is invalid.  Every successful connect round-trips a ping/
//      pong over the socket to prove the connection is usable.
//
// Exit: RC 0 and "[NET-EYEBALLS-TEST] ALL PASS" when every Check passed.
#include <chaos/net/net_api.h>
#include <chaos/net/socket_handle.h>
#include <chaos/net/net.h>
#include <chaos/net/error.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>

using namespace chaos::net;

namespace {

int g_failures = 0;

void Check(bool ok, const char* what) noexcept
{
    std::printf("[NET-EYEBALLS-TEST] %s: %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) { ++g_failures; }
}

// Echo server: binds loopback on the requested family (optionally
// IPv6-only), publishes the ephemeral port, accepts ONE connection and
// echoes back whatever it receives, then closes the listener.
void RunEchoServer(bool v6, bool v6Only, std::atomic<int>* portOut) noexcept
{
    SocketHandle lsn{};
    NetError e = NetSocketCreate(v6 ? kAddressFamilyInet6 : kAddressFamilyInet,
                                 kSocketTypeStream, kProtocolTcp, &lsn);
    if (e != NetError::None) { Check(false, "server create"); return; }
    if (v6Only) { NetSocketSetOption(&lsn, NetOption::IPv6Only, 1); }
    const NetAddress bindAddr = v6 ? LoopbackV6(0) : LoopbackV4(0);
    e = NetSocketBind(&lsn, bindAddr);
    if (e != NetError::None) { Check(false, "server bind"); NetSocketClose(&lsn); return; }
    if (NetSocketListen(&lsn, 16) != NetError::None) {
        Check(false, "server listen"); NetSocketClose(&lsn); return;
    }
    NetAddress got{};
    if (NetSocketGetLocalAddress(&lsn, &got) == NetError::None && portOut != nullptr) {
        portOut->store(static_cast<int>(got.port));
    }
    SocketHandle peer{};
    if (NetSocketAccept(&lsn, &peer, nullptr, 10000) == NetError::None) {
        CHAOS_IL2CPP_UINT8 buf[8] = {};
        CHAOS_IL2CPP_INT32 gotN = 0;
        if (NetSocketRecv(&peer, buf, 8, 0, &gotN, 5000) == NetError::None && gotN > 0) {
            CHAOS_IL2CPP_INT32 sent = 0;
            NetSocketSend(&peer, buf, gotN, 0, &sent, 5000);
        }
        NetSocketClose(&peer);
    }
    NetSocketClose(&lsn);
}

// Connect through NetSocketConnectList; returns elapsed ms and the error.
long long ConnectList(const NetAddress* addrs, CHAOS_IL2CPP_INT32 count,
                      const NetConnectOptions& opts, NetError* errOut,
                      SocketHandle* sockOut) noexcept
{
    using namespace std::chrono;
    const auto t0 = steady_clock::now();
    *errOut = NetSocketConnectList(sockOut, addrs, count, opts);
    const auto t1 = steady_clock::now();
    return duration_cast<milliseconds>(t1 - t0).count();
}

// Echos 4 bytes over a connected socket; true when the round-trip works.
bool PingPong(SocketHandle* s) noexcept
{
    static const CHAOS_IL2CPP_UINT8 kPing[4] = {'p', 'i', 'n', 'g'};
    CHAOS_IL2CPP_UINT8 out[4] = {};
    CHAOS_IL2CPP_INT32 sent = 0, got = 0;
    if (NetSocketSend(s, kPing, 4, 0, &sent, 5000) != NetError::None || sent != 4) {
        return false;
    }
    if (NetSocketRecv(s, out, 4, 0, &got, 5000) != NetError::None || got != 4) {
        return false;
    }
    return std::memcmp(out, kPing, 4) == 0;
}

// Test 1: first address is live -> immediate success, no fallback wait.
void TestFirstAddressSucceeds() noexcept
{
    std::atomic<int> port{0};
    std::thread th(RunEchoServer, false, false, &port);
    for (int i = 0; i < 200 && port.load() == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    if (port.load() == 0) { Check(false, "t1: server port"); th.join(); return; }
    const CHAOS_IL2CPP_UINT16 p = static_cast<CHAOS_IL2CPP_UINT16>(port.load());
    NetAddress addrs[2];
    addrs[0] = LoopbackV4(p);
    addrs[1] = LoopbackV6(p);
    NetConnectOptions opts{30000, 1};
    SocketHandle sock{};
    NetError err = NetError::Unsupported;
    const long long ms = ConnectList(addrs, 2, opts, &err, &sock);
    Check(err == NetError::None, "t1: connect ok");
    Check(sock.fd >= 0 && PingPong(&sock), "t1: ping/pong ok");
    Check(ms < 1000, "t1: first-address success did not wait");
    if (sock.fd >= 0) { NetSocketClose(&sock); }
    th.join();
}

// Test 2: v4 refused (v6-only ::1 listener) -> falls through to v6.
void TestV4RefusedFallsToV6() noexcept
{
    std::atomic<int> port{0};
    std::thread th(RunEchoServer, true, true, &port);
    for (int i = 0; i < 200 && port.load() == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    if (port.load() == 0) { Check(false, "t2: server port"); th.join(); return; }
    const CHAOS_IL2CPP_UINT16 p = static_cast<CHAOS_IL2CPP_UINT16>(port.load());
    NetAddress addrs[2];
    addrs[0] = LoopbackV4(p);   // refused: nothing listens on IPv4
    addrs[1] = LoopbackV6(p);   // this one is live
    NetConnectOptions opts{30000, 1};
    SocketHandle sock{};
    NetError err = NetError::Unsupported;
    const long long ms = ConnectList(addrs, 2, opts, &err, &sock);
    Check(err == NetError::None, "t2: v4-refused fell through to v6");
    Check(sock.fd >= 0 && PingPong(&sock), "t2: ping/pong ok");
    Check(ms < 3000, "t2: refused fallback was quick");
    if (sock.fd >= 0) { NetSocketClose(&sock); }
    th.join();
}

// Test 3: v6 refused (v4-only listener) -> falls through to v4.
void TestV6RefusedFallsToV4() noexcept
{
    std::atomic<int> port{0};
    std::thread th(RunEchoServer, false, false, &port);
    for (int i = 0; i < 200 && port.load() == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    if (port.load() == 0) { Check(false, "t3: server port"); th.join(); return; }
    const CHAOS_IL2CPP_UINT16 p = static_cast<CHAOS_IL2CPP_UINT16>(port.load());
    NetAddress addrs[2];
    addrs[0] = LoopbackV6(p);   // refused: nothing listens on ::1
    addrs[1] = LoopbackV4(p);   // live
    NetConnectOptions opts{30000, 1};
    SocketHandle sock{};
    NetError err = NetError::Unsupported;
    const long long ms = ConnectList(addrs, 2, opts, &err, &sock);
    Check(err == NetError::None, "t3: v6-refused fell through to v4");
    Check(sock.fd >= 0 && PingPong(&sock), "t3: ping/pong ok");
    Check(ms < 3000, "t3: refused fallback was quick");
    if (sock.fd >= 0) { NetSocketClose(&sock); }
    th.join();
}

// Test 4: non-responding first candidate is bounded by the short per-attempt
// timeout (connectTimeoutMs is huge; the clamp must kick in) and the second
// candidate connects.
void TestTimeoutFallback() noexcept
{
    // TEST-NET-1 (RFC 5737) 192.0.2.1: no host responds; on hosts without a
    // route the connect may fail fast (ENETUNREACH), with a route it burns
    // the 250ms per-attempt clamp.  Either way the fallback must land on the
    // live loopback candidate within the bounded budget.
    NetAddress dead{};
    dead.family = kAddressFamilyInet;
    dead.port = 9;
    dead.addr[0] = 192; dead.addr[1] = 0; dead.addr[2] = 2; dead.addr[3] = 1;
    std::atomic<int> port{0};
    std::thread th(RunEchoServer, false, false, &port);
    for (int i = 0; i < 200 && port.load() == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    if (port.load() == 0) { Check(false, "t4: server port"); th.join(); return; }
    const CHAOS_IL2CPP_UINT16 p = static_cast<CHAOS_IL2CPP_UINT16>(port.load());
    NetAddress addrs[2];
    addrs[0] = dead;
    addrs[1] = LoopbackV4(p);
    NetConnectOptions opts{30000, 1};   // huge budget; clamp must bound attempt 1
    SocketHandle sock{};
    NetError err = NetError::Unsupported;
    const long long ms = ConnectList(addrs, 2, opts, &err, &sock);
    Check(err == NetError::None, "t4: timeout fallback connected");
    Check(sock.fd >= 0 && PingPong(&sock), "t4: ping/pong ok");
    Check(ms < 5000, "t4: per-attempt clamp bounded the dead candidate");
    if (sock.fd >= 0) { NetSocketClose(&sock); }
    th.join();
}

// Test 5: useHappyEyeballs=0 is legacy single-address connect: only the
// FIRST candidate is tried, so a refused first address fails even though
// the second would have succeeded.
void TestLegacyNoFallback() noexcept
{
    std::atomic<int> port{0};
    std::thread th(RunEchoServer, true, true, &port);   // v6-only ::1 listener
    for (int i = 0; i < 200 && port.load() == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    if (port.load() == 0) { Check(false, "t5: server port"); th.join(); return; }
    const CHAOS_IL2CPP_UINT16 p = static_cast<CHAOS_IL2CPP_UINT16>(port.load());
    NetAddress addrs[2];
    addrs[0] = LoopbackV4(p);   // deterministically refused (v6-only listener)
    addrs[1] = LoopbackV6(p);   // live, but legacy mode must NOT try it
    NetConnectOptions opts{3000, 0};   // legacy: no fallback
    SocketHandle sock{};
    NetError err = NetError::None;
    ConnectList(addrs, 2, opts, &err, &sock);
    Check(err != NetError::None, "t5: legacy single-address did not fall back");
    Check(sock.fd < 0, "t5: out handle invalid after total failure");
    if (sock.fd >= 0) { NetSocketClose(&sock); }
    th.join();
}

// Test 6: every candidate fails -> last error, invalid out handle; and
// degenerate inputs (null/count<=0) are Unsupported.
void TestAllFailAndInvalidArgs() noexcept
{
    NetAddress dead1{}, dead2{};
    dead1.family = kAddressFamilyInet; dead1.port = 9;
    dead1.addr[0] = 192; dead1.addr[1] = 0; dead1.addr[2] = 2; dead1.addr[3] = 1;
    dead2 = dead1;
    dead2.port = 10;
    NetAddress addrs[2] = {dead1, dead2};
    NetConnectOptions opts{2000, 1};
    SocketHandle sock{};
    NetError err = NetError::None;
    ConnectList(addrs, 2, opts, &err, &sock);
    Check(err != NetError::None, "t6: all candidates failed");
    Check(sock.fd < 0, "t6: out handle invalid");
    if (sock.fd >= 0) { NetSocketClose(&sock); }
    SocketHandle s2{};
    NetError e0 = NetSocketConnectList(nullptr, addrs, 2, opts);
    NetError e1 = NetSocketConnectList(&s2, nullptr, 2, opts);
    NetError e2 = NetSocketConnectList(&s2, addrs, 0, opts);
    Check(e0 == NetError::Unsupported && e1 == NetError::Unsupported &&
          e2 == NetError::Unsupported, "t6: invalid args rejected");
}

}  // namespace

int main()
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::printf("[NET-EYEBALLS-TEST] starting\n");
    if (NetStartup() != NetError::None) {
        Check(false, "NetStartup");
    } else {
        TestFirstAddressSucceeds();
        TestV4RefusedFallsToV6();
        TestV6RefusedFallsToV4();
        TestTimeoutFallback();
        TestLegacyNoFallback();
        TestAllFailAndInvalidArgs();
        NetCleanup();
    }
    const bool pass = (g_failures == 0);
    std::printf("[NET-EYEBALLS-TEST] %s (%d failures)\n",
                pass ? "ALL PASS" : "FAILED", g_failures);
    return pass ? 0 : 1;
}
