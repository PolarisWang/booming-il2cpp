// chaos_net_udp_test.cpp - NT-24: UDP datagram layer tests.
//
// Scenarios (all loopback, dual-platform):
//   t1 udp-echo-simple   single client/server bidirectional ping/pong over
//                        pre-registered addresses (datagram integrity).
//   t2 udp-multi-client  one server, N clients, distinct payloads all
//                        delivered; each recv returns exactly one datagram.
//   t3 udp-broadcast     SO_BROADCAST send to 255.255.255.255 received by a
//                        socket bound to INADDR_ANY on the same host.
//   t4 udp-option        EnableBroadcast set/get roundtrip + ReceiveTimeout
//                        short wait returns TimedOut (option plumbing).
//
// The transport layer sends with ::send / receives with ::recv, so a UDP
// socket must Connect() first to fix its default peer; recv on an unbound
// connectionless socket delivers any inbound datagram regardless of peer.
#include <chaos/net/net_api.h>
#include <chaos/net/net.h>
#include <chaos/net/error.h>
#include <chaos/net/socket_handle.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

using namespace chaos::net;

namespace {

int g_failures = 0;

void Check(bool ok, const char* what) {
    std::printf("[NET-UDP-TEST] %s: %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) ++g_failures;
}

void WaitPort(const std::atomic<int>& port) {
    for (int i = 0; i < 5000 && port.load() == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

bool SendAll(SocketHandle* s, const char* data, int len, int tmo) {
    CHAOS_IL2CPP_INT32 sent = 0;
    NetError e = NetSocketSend(s, reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(data),
                              len, 0, &sent, tmo);
    return e == NetError::None && sent == len;
}

bool MakeUdpBound(NetAddress* addr, SocketHandle* s) {
    if (NetSocketCreate(kAddressFamilyInet, kSocketTypeDgram, kProtocolUdp, s)
        != NetError::None)
        return false;
    if (NetSocketBind(s, LoopbackV4(0)) != NetError::None) return false;
    return NetSocketGetLocalAddress(s, addr) == NetError::None;
}

void TestUdpEcho() {
    std::atomic<int> serverPort{0};
    std::atomic<int> clientPort{0};
    std::atomic<bool> serverOk{false};
    std::thread server([&] {
        SocketHandle s{};
        NetAddress srv{};
        if (!MakeUdpBound(&srv, &s)) { Check(false, "echo server bind"); return; }
        serverPort.store((int)srv.port);
        char buf[64] = {0};
        CHAOS_IL2CPP_INT32 got = 0;
        NetError e = NetSocketRecv(&s, reinterpret_cast<CHAOS_IL2CPP_UINT8*>(buf),
                                  (CHAOS_IL2CPP_INT32)sizeof(buf), 0, &got, 5000);
        bool ok = e == NetError::None && got == 9 &&
                  std::memcmp(buf, "ping-echo", 9) == 0;
        Check(ok, "t1 server recv ping (9B exact datagram)");
        WaitPort(clientPort);
        NetAddress cli = LoopbackV4((CHAOS_IL2CPP_UINT16)clientPort.load());
        NetError ce = NetSocketConnect(&s, cli, 5000);
        bool ok2 = ce == NetError::None &&
                   SendAll(&s, "pong-echo", 10, 5000);
        Check(ok2, "t1 server connect client + send pong (10B)");
        serverOk.store(ok && ok2);
    });
    SocketHandle c{};
    NetAddress cli{};
    if (!MakeUdpBound(&cli, &c)) {
        Check(false, "t1 client bind");
    } else {
        clientPort.store((int)cli.port);
        WaitPort(serverPort);
        NetError ce = NetSocketConnect(&c, LoopbackV4((CHAOS_IL2CPP_UINT16)serverPort.load()), 5000);
        Check(ce == NetError::None, "t1 client connect server");
        Check(SendAll(&c, "ping-echo", 9, 5000), "t1 client send ping (9B)");
        char buf[64] = {0};
        CHAOS_IL2CPP_INT32 got = 0;
        NetError e = NetSocketRecv(&c, reinterpret_cast<CHAOS_IL2CPP_UINT8*>(buf),
                                  (CHAOS_IL2CPP_INT32)sizeof(buf), 0, &got, 5000);
        bool ok = e == NetError::None && got == 10 &&
                  std::memcmp(buf, "pong-echo", 10) == 0;
        Check(ok, "t1 client recv pong (10B exact)");
    }
    server.join();
    Check(serverOk.load(), "t1 server aggregate ok");
    NetSocketClose(&c);
}

void TestUdpMultiClient() {
    const int kClients = 8;
    std::atomic<int> serverPort{0};
    std::atomic<int> received{0};
    bool ok = true;
    std::thread server([&] {
        SocketHandle s{};
        NetAddress srv{};
        if (!MakeUdpBound(&srv, &s)) { Check(false, "t2 server bind"); return; }
        serverPort.store((int)srv.port);
        std::vector<std::string> seen;
        for (int i = 0; i < kClients; ++i) {
            char buf[256] = {0};
            CHAOS_IL2CPP_INT32 got = 0;
            NetError e = NetSocketRecv(&s, reinterpret_cast<CHAOS_IL2CPP_UINT8*>(buf),
                                      (CHAOS_IL2CPP_INT32)sizeof(buf), 0, &got, 8000);
            if (e != NetError::None || got <= 0) { ok = false; break; }
            seen.emplace_back(buf, (std::size_t)got);
            received.fetch_add(1);
        }
        for (int i = 0; ok && i < kClients; ++i) {
            std::string want = "client-" + std::to_string(i) + "-payload";
            int hits = 0;
            for (const auto& s2 : seen) if (s2 == want) ++hits;
            if (hits != 1) ok = false;
        }
        NetSocketClose(&s);
    });
    WaitPort(serverPort);
    std::vector<std::thread> clients;
    for (int i = 0; i < kClients; ++i) {
        clients.emplace_back([&, i] {
            SocketHandle c{};
            NetAddress cli{};
            if (!MakeUdpBound(&cli, &c)) { Check(false, "t2 client bind"); return; }
            if (NetSocketConnect(&c, LoopbackV4((CHAOS_IL2CPP_UINT16)serverPort.load()), 5000)
                != NetError::None) { Check(false, "t2 client connect"); NetSocketClose(&c); return; }
            std::string msg = "client-" + std::to_string(i) + "-payload";
            Check(SendAll(&c, msg.c_str(), (int)msg.size(), 5000),
                  "t2 client send datagram");
            NetSocketClose(&c);
        });
    }
    for (auto& t : clients) t.join();
    server.join();
    Check(ok, "t2 server received all distinct payloads");
    Check(received.load() == kClients, "t2 exact datagram count");
}

void TestUdpBroadcast() {
    std::atomic<int> recvPort{0};
    std::atomic<bool> recvOk{false};
    std::thread receiver([&] {
        SocketHandle r{};
        if (NetSocketCreate(kAddressFamilyInet, kSocketTypeDgram, kProtocolUdp, &r)
            != NetError::None) { Check(false, "t3 recv create"); return; }
        NetAddress any{};
        any.family = kAddressFamilyInet;
        any.port = 0;
        if (NetSocketBind(&r, any) != NetError::None) { Check(false, "t3 recv bind"); NetSocketClose(&r); return; }
        NetAddress got{};
        if (NetSocketGetLocalAddress(&r, &got) != NetError::None) { Check(false, "t3 recv local"); NetSocketClose(&r); return; }
        recvPort.store((int)got.port);
        char buf[32] = {0};
        CHAOS_IL2CPP_INT32 n = 0;
        NetError e = NetSocketRecv(&r, reinterpret_cast<CHAOS_IL2CPP_UINT8*>(buf),
                                  (CHAOS_IL2CPP_INT32)sizeof(buf), 0, &n, 8000);
        bool ok = e == NetError::None && n == 3 && std::memcmp(buf, "bcd", 3) == 0;
        recvOk.store(ok);
        NetSocketClose(&r);
    });
    WaitPort(recvPort);
    SocketHandle s{};
    bool ok = false;
    do {
        if (NetSocketCreate(kAddressFamilyInet, kSocketTypeDgram, kProtocolUdp, &s)
            != NetError::None) break;
        if (NetSocketSetOption(&s, NetOption::EnableBroadcast, 1) != NetError::None) break;
        NetAddress bc{};
        bc.family = kAddressFamilyInet;
        bc.port = (CHAOS_IL2CPP_UINT16)recvPort.load();
        bc.addr[0] = 255; bc.addr[1] = 255; bc.addr[2] = 255; bc.addr[3] = 255;
        if (NetSocketConnect(&s, bc, 5000) != NetError::None) break;
        ok = SendAll(&s, "bcd", 3, 5000);
    } while (false);
    Check(ok, "t3 sender SO_BROADCAST + send to 255.255.255.255");
    receiver.join();
    Check(recvOk.load(), "t3 receiver got broadcast datagram");
    NetSocketClose(&s);
}

void TestUdpOptions() {
    SocketHandle s{};
    bool ok = false;
    do {
        if (NetSocketCreate(kAddressFamilyInet, kSocketTypeDgram, kProtocolUdp, &s)
            != NetError::None) break;
        // EnableBroadcast set/get roundtrip
        if (NetSocketSetOption(&s, NetOption::EnableBroadcast, 1) != NetError::None) break;
        CHAOS_IL2CPP_INT32 v = 0;
        if (NetSocketGetOption(&s, NetOption::EnableBroadcast, &v) != NetError::None) break;
        if (v != 1) break;
        // Bind first: recv on an UNBOUND dgram socket is WSAEINVAL on Windows
        // but WOULDBLOCK on POSIX, so the timeout path must be tested on a
        // bound socket (bind(0) keeps the ephemeral port).
        if (NetSocketBind(&s, LoopbackV4(0)) != NetError::None) break;
        // ReceiveTimeout: short wait with no peer must time out
        if (NetSocketSetOption(&s, NetOption::ReceiveTimeout, 200) != NetError::None) break;
        char b[4];
        CHAOS_IL2CPP_INT32 got = 0;
        NetError e = NetSocketRecv(&s, reinterpret_cast<CHAOS_IL2CPP_UINT8*>(b), 4, 0, &got, 1500);
        ok = e == NetError::TimedOut;
        NetSocketClose(&s);
    } while (false);
    Check(ok, "t4 broadcast option roundtrip + recv timeout");
}

}  // namespace

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::printf("[NET-UDP-TEST] starting\n");
    if (NetStartup() != NetError::None) {
        Check(false, "NetStartup");
    } else {
        TestUdpEcho();
        TestUdpMultiClient();
        TestUdpBroadcast();
        TestUdpOptions();
        NetCleanup();
    }
    if (g_failures == 0) {
        std::printf("[NET-UDP-TEST] ALL PASS\n");
        return 0;
    }
    std::printf("[NET-UDP-TEST] FAILED (%d)\n", g_failures);
    return 1;
}
