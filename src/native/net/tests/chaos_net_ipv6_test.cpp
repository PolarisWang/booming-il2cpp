// chaos_net_ipv6_test -- IPv6 multi-address-family coverage (NT-17).
// Covers: IPv4 echo regression, IPv6 (::1) loopback echo, IPV6_V6ONLY
// option set/get roundtrip, dual-stack in6addr_any bind + IPv4-mapped
// incoming unmapping, and v6only rejection of IPv4 connects.
// Pure native, no managed code; run standalone (exit 0 == all PASS).
#include <chaos/net/net_api.h>
#include <chaos/net/net.h>
#include <chaos/net/error.h>
#include <chaos/net/socket_handle.h>

#include <cstdio>
#include <cstring>

using namespace chaos::net;

namespace {

int g_failures = 0;

void Check(bool ok, const char* what)
{
    std::printf("[NET-IPV6-TEST] %s: %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) {
        ++g_failures;
    }
}

bool BytesEq(const CHAOS_IL2CPP_UINT8* a, const char* b, int n)
{
    return std::memcmp(a, b, static_cast<size_t>(n)) == 0;
}

// IPv4 loopback echo (regression guard: NT-6 semantics must not break).
void TestV4Echo()
{
    SocketHandle listener{};
    if (NetSocketCreate(kAddressFamilyInet, kSocketTypeStream, kProtocolTcp, &listener) != NetError::None) {
        Check(false, "v4: create listener");
        return;
    }
    NetAddress bind_addr = LoopbackV4(0);
    Check(NetSocketBind(&listener, bind_addr) == NetError::None, "v4: bind 127.0.0.1:0");
    Check(NetSocketListen(&listener, 8) == NetError::None, "v4: listen backlog=8");

    NetAddress local{};
    Check(NetSocketGetLocalAddress(&listener, &local) == NetError::None, "v4: get local address");
    Check(local.port != 0, "v4: ephemeral port != 0");

    SocketHandle client{};
    Check(NetSocketCreate(kAddressFamilyInet, kSocketTypeStream, kProtocolTcp, &client) == NetError::None,
          "v4: create client");
    Check(NetSocketConnect(&client, local, 5000) == NetError::None, "v4: client connect");

    SocketHandle peer{};
    NetAddress peer_addr{};
    Check(NetSocketAccept(&listener, &peer, &peer_addr, 5000) == NetError::None, "v4: server accept");
    Check(peer_addr.family == kAddressFamilyInet, "v4: peer addr family inet");
    Check(peer_addr.addr[0] == 127 && peer_addr.addr[1] == 0 &&
              peer_addr.addr[2] == 0 && peer_addr.addr[3] == 1,
          "v4: peer addr is 127.0.0.1");

    const char ping[] = "ping";
    CHAOS_IL2CPP_INT32 sent = 0;
    Check(NetSocketSend(&client, reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(ping), 4, 0, &sent, 5000) == NetError::None &&
              sent == 4,
          "v4: client send 4 bytes");
    CHAOS_IL2CPP_UINT8 rx[8]{};
    CHAOS_IL2CPP_INT32 got = 0;
    Check(NetSocketRecv(&peer, rx, 4, 0, &got, 5000) == NetError::None && got == 4 &&
              BytesEq(rx, ping, 4),
          "v4: server recv 'ping'");
    Check(NetSocketClose(&peer) == NetError::None, "v4: close server peer");
    Check(NetSocketClose(&client) == NetError::None, "v4: close client");
    Check(NetSocketClose(&listener) == NetError::None, "v4: close listener");
}

// IPv6 ::1 loopback end-to-end echo (NT-17 core).
void TestV6Echo()
{
    SocketHandle listener{};
    if (NetSocketCreate(kAddressFamilyInet6, kSocketTypeStream, kProtocolTcp, &listener) != NetError::None) {
        Check(false, "v6: create listener");
        return;
    }
    NetAddress bind_addr = LoopbackV6(0);
    Check(NetSocketBind(&listener, bind_addr) == NetError::None, "v6: bind ::1:0");
    Check(NetSocketListen(&listener, 8) == NetError::None, "v6: listen backlog=8");

    NetAddress local{};
    Check(NetSocketGetLocalAddress(&listener, &local) == NetError::None, "v6: get local address");
    Check(local.family == kAddressFamilyInet6, "v6: local family inet6");
    Check(local.port != 0, "v6: ephemeral port != 0");
    Check(local.addr[15] == 1, "v6: local addr is ::1");

    SocketHandle client{};
    Check(NetSocketCreate(kAddressFamilyInet6, kSocketTypeStream, kProtocolTcp, &client) == NetError::None,
          "v6: create client");
    Check(NetSocketConnect(&client, local, 5000) == NetError::None, "v6: client connect");

    SocketHandle peer{};
    NetAddress peer_addr{};
    Check(NetSocketAccept(&listener, &peer, &peer_addr, 5000) == NetError::None, "v6: server accept");
    Check(peer_addr.family == kAddressFamilyInet6, "v6: peer addr family inet6");
    Check(peer_addr.port != 0, "v6: peer addr ephemeral port != 0");
    {
        bool is_loop = peer_addr.addr[15] == 1;
        for (int i = 0; i < 15 && is_loop; ++i) {
            is_loop = peer_addr.addr[i] == 0;
        }
        Check(is_loop, "v6: peer addr is ::1");
    }

    const char ping[] = "ping";
    CHAOS_IL2CPP_INT32 sent = 0;
    Check(NetSocketSend(&client, reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(ping), 4, 0, &sent, 5000) == NetError::None &&
              sent == 4,
          "v6: client send 4 bytes");
    CHAOS_IL2CPP_UINT8 rx[8]{};
    CHAOS_IL2CPP_INT32 got = 0;
    Check(NetSocketRecv(&peer, rx, 4, 0, &got, 5000) == NetError::None && got == 4 &&
              BytesEq(rx, ping, 4),
          "v6: server recv 'ping'");
    const char pong[] = "pong";
    sent = 0;
    Check(NetSocketSend(&peer, reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(pong), 4, 0, &sent, 5000) == NetError::None &&
              sent == 4,
          "v6: server send 4 bytes");
    got = 0;
    Check(NetSocketRecv(&client, rx, 4, 0, &got, 5000) == NetError::None && got == 4 &&
              BytesEq(rx, pong, 4),
          "v6: client recv 'pong'");
    Check(NetSocketClose(&peer) == NetError::None, "v6: close server peer");
    Check(NetSocketClose(&client) == NetError::None, "v6: close client");
    Check(NetSocketClose(&listener) == NetError::None, "v6: close listener");
}

// IPV6_V6ONLY option set/get roundtrip on an AF_INET6 stream socket.
void TestV6OnlyOption()
{
    SocketHandle s{};
    if (NetSocketCreate(kAddressFamilyInet6, kSocketTypeStream, kProtocolTcp, &s) != NetError::None) {
        Check(false, "v6only: create socket");
        return;
    }
    Check(NetSocketSetOption(&s, NetOption::IPv6Only, 1) == NetError::None, "v6only: set IPv6Only=1");
    CHAOS_IL2CPP_INT32 v = -1;
    Check(NetSocketGetOption(&s, NetOption::IPv6Only, &v) == NetError::None && v == 1, "v6only: get IPv6Only==1");
    Check(NetSocketSetOption(&s, NetOption::IPv6Only, 0) == NetError::None, "v6only: set IPv6Only=0");
    v = -1;
    Check(NetSocketGetOption(&s, NetOption::IPv6Only, &v) == NetError::None && v == 0, "v6only: get IPv6Only==0");
    Check(NetSocketClose(&s) == NetError::None, "v6only: close socket");
}

// Dual-stack semantics: AF_INET6 socket with IPv6Only=0 bound to in6addr_any
// accepts an IPv4 client; the accepted peer address is unmapped back to the
// IPv4 family (::ffff:127.0.0.1 -> kAddressFamilyInet / 127.0.0.1).
void TestDualStack()
{
    SocketHandle listener{};
    if (NetSocketCreate(kAddressFamilyInet6, kSocketTypeStream, kProtocolTcp, &listener) != NetError::None) {
        Check(false, "dual: create v6 listener");
        return;
    }
    Check(NetSocketSetOption(&listener, NetOption::IPv6Only, 0) == NetError::None, "dual: set IPv6Only=0");
    // Bind in6addr_any:0 (all-zero IPv6 address).
    NetAddress any{};
    any.family = kAddressFamilyInet6;
    Check(NetSocketBind(&listener, any) == NetError::None, "dual: bind in6addr_any:0");
    Check(NetSocketListen(&listener, 8) == NetError::None, "dual: listen backlog=8");
    NetAddress local{};
    Check(NetSocketGetLocalAddress(&listener, &local) == NetError::None, "dual: get local address");
    Check(local.port != 0, "dual: ephemeral port != 0");

    // IPv4 client dials 127.0.0.1:<v6 ephemeral port> through the dual stack.
    SocketHandle client{};
    Check(NetSocketCreate(kAddressFamilyInet, kSocketTypeStream, kProtocolTcp, &client) == NetError::None,
          "dual: create v4 client");
    NetAddress v4_target = LoopbackV4(local.port);
    Check(NetSocketConnect(&client, v4_target, 5000) == NetError::None, "dual: v4 client connect via dual stack");

    SocketHandle peer{};
    NetAddress peer_addr{};
    Check(NetSocketAccept(&listener, &peer, &peer_addr, 5000) == NetError::None, "dual: server accept");
    Check(peer_addr.family == kAddressFamilyInet, "dual: peer unmapped to family inet");
    Check(peer_addr.addr[0] == 127 && peer_addr.addr[1] == 0 &&
              peer_addr.addr[2] == 0 && peer_addr.addr[3] == 1,
          "dual: peer addr is 127.0.0.1");

    Check(NetSocketClose(&peer) == NetError::None, "dual: close peer");
    Check(NetSocketClose(&client) == NetError::None, "dual: close client");
    Check(NetSocketClose(&listener) == NetError::None, "dual: close listener");
}

// v6only=1 excludes v4-mapped clients: an IPv4 connect to the same port must
// be refused (the listen v6 socket is v6-only).
void TestV6OnlyRejectsV4()
{
    SocketHandle listener{};
    if (NetSocketCreate(kAddressFamilyInet6, kSocketTypeStream, kProtocolTcp, &listener) != NetError::None) {
        Check(false, "v6only1: create v6 listener");
        return;
    }
    Check(NetSocketSetOption(&listener, NetOption::IPv6Only, 1) == NetError::None, "v6only1: set IPv6Only=1");
    NetAddress any{};
    any.family = kAddressFamilyInet6;
    Check(NetSocketBind(&listener, any) == NetError::None, "v6only1: bind in6addr_any:0");
    Check(NetSocketListen(&listener, 8) == NetError::None, "v6only1: listen backlog=8");
    NetAddress local{};
    Check(NetSocketGetLocalAddress(&listener, &local) == NetError::None, "v6only1: get local address");

    SocketHandle client{};
    Check(NetSocketCreate(kAddressFamilyInet, kSocketTypeStream, kProtocolTcp, &client) == NetError::None,
          "v6only1: create v4 client");
    NetAddress v4_target = LoopbackV4(local.port);
    const NetError err = NetSocketConnect(&client, v4_target, 3000);
    Check(err != NetError::None, "v6only1: v4 connect refused");
    Check(NetSocketClose(&client) == NetError::None, "v6only1: close client");
    Check(NetSocketClose(&listener) == NetError::None, "v6only1: close listener");
}

}  // namespace

int main()
{
    std::printf("[NET-IPV6-TEST] chaos_net_ipv6_test starting\n");
    if (NetStartup() != NetError::None) {
        Check(false, "NetStartup");
    } else {
        TestV4Echo();
        TestV6Echo();
        TestV6OnlyOption();
        TestDualStack();
        TestV6OnlyRejectsV4();
        NetCleanup();
    }
    std::printf("[NET-IPV6-TEST] %s (%d failure(s))\n",
                g_failures == 0 ? "ALL PASS" : "FAILED", g_failures);
    return g_failures == 0 ? 0 : 1;
}
