// chaos_net_test -- native unit test for the chaos_net transport (NT-6).
// Covers: error table roundtrip, option set/get roundtrip, loopback TCP
// connect/accept/send/recv/close through the Layer D net_api surface, and
// the Layer B unconnected contract (10057).  Pure native, no managed code;
// run standalone (exit 0 == all PASS).
#include <chaos/net/net_api.h>
#include <chaos/net/net.h>
#include <chaos/net/error.h>
#include <chaos/net/socket_handle.h>

#include <cstdio>
#include <cstring>

// Layer B (socket_ops.cpp) entry points, declared in entry.cpp.
namespace chaos::net {
CHAOS_IL2CPP_INT32 SocketSendBytes(SocketHandle* handle, const CHAOS_IL2CPP_UINT8* bytes,
                                   CHAOS_IL2CPP_INTPTR byteLength, CHAOS_IL2CPP_INT32 offset,
                                   CHAOS_IL2CPP_INT32 size, CHAOS_IL2CPP_INT32 flags,
                                   CHAOS_IL2CPP_INT32* error) noexcept;
CHAOS_IL2CPP_INT32 SocketReceiveBytes(SocketHandle* handle, CHAOS_IL2CPP_UINT8* bytes,
                                      CHAOS_IL2CPP_INTPTR byteLength, CHAOS_IL2CPP_INT32 offset,
                                      CHAOS_IL2CPP_INT32 size, CHAOS_IL2CPP_INT32 flags,
                                      CHAOS_IL2CPP_INT32* error) noexcept;
}  // namespace chaos::net

using namespace chaos::net;

namespace {

int g_failures = 0;

void Check(bool ok, const char* what)
{
    std::printf("[NET-TEST] %s: %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) {
        ++g_failures;
    }
}

bool BytesEq(const CHAOS_IL2CPP_UINT8* a, const char* b, int n)
{
    return std::memcmp(a, b, static_cast<size_t>(n)) == 0;
}

void TestErrorTable()
{
    Check(NativeCodeToSocketError(0) == SocketError::Success, "err: 0 -> Success");
    Check(NativeCodeToSocketError(10057) == SocketError::NotConnected, "err: 10057 -> NotConnected");
    Check(NativeCodeToSocketError(10054) == SocketError::ConnectionReset, "err: 10054 -> ConnectionReset");
    Check(NativeCodeToSocketError(10060) == SocketError::TimedOut, "err: 10060 -> TimedOut");
    Check(NativeCodeToSocketError(10035) == SocketError::WouldBlock, "err: 10035 -> WouldBlock");
    Check(NativeCodeToSocketError(99999) == SocketError::OperationNotSupported, "err: unknown -> OperationNotSupported");
}

void TestOptions()
{
    // Datagram (UDP) socket: SO_BROADCAST + generic options.
    SocketHandle h{};
    NetError err = NetSocketCreate(kAddressFamilyInet, kSocketTypeDgram, kProtocolUdp, &h);
    Check(err == NetError::None, "opt: create dgram socket");
    if (err != NetError::None) {
        return;
    }
    Check(NetSocketSetOption(&h, NetOption::ReuseAddress, 1) == NetError::None, "opt: set ReuseAddress=1");
    Check(NetSocketSetOption(&h, NetOption::EnableBroadcast, 1) == NetError::None, "opt: set EnableBroadcast=1");
    Check(NetSocketSetOption(&h, NetOption::ReceiveTimeout, 2000) == NetError::None, "opt: set ReceiveTimeout=2000");
    Check(NetSocketSetOption(&h, NetOption::SendTimeout, 1500) == NetError::None, "opt: set SendTimeout=1500");

    CHAOS_IL2CPP_INT32 v = -1;
    Check(NetSocketGetOption(&h, NetOption::ReuseAddress, &v) == NetError::None && v == 1, "opt: get ReuseAddress==1");
    v = -1;
    Check(NetSocketGetOption(&h, NetOption::EnableBroadcast, &v) == NetError::None && v == 1, "opt: get EnableBroadcast==1");
    v = -1;
    Check(NetSocketGetOption(&h, NetOption::ReceiveTimeout, &v) == NetError::None && v == 2000, "opt: get ReceiveTimeout==2000");
    v = -1;
    Check(NetSocketGetOption(&h, NetOption::SendTimeout, &v) == NetError::None && v == 1500, "opt: get SendTimeout==1500");
    Check(NetSocketClose(&h) == NetError::None, "opt: close dgram socket");

    // Stream (TCP) socket: TCP_NODELAY / SO_KEEPALIVE / SO_LINGER.
    SocketHandle s{};
    err = NetSocketCreate(kAddressFamilyInet, kSocketTypeStream, kProtocolTcp, &s);
    Check(err == NetError::None, "opt: create stream socket");
    if (err != NetError::None) {
        return;
    }
    Check(NetSocketSetOption(&s, NetOption::NoDelay, 1) == NetError::None, "opt: set NoDelay=1");
    Check(NetSocketSetOption(&s, NetOption::KeepAlive, 1) == NetError::None, "opt: set KeepAlive=1");
    Check(NetSocketSetOption(&s, NetOption::Linger, 0) == NetError::None, "opt: set Linger=0 (off)");
    Check(NetSocketSetOption(&s, NetOption::Linger, 5) == NetError::None, "opt: set Linger=5 (on)");

    v = -1;
    Check(NetSocketGetOption(&s, NetOption::NoDelay, &v) == NetError::None && v == 1, "opt: get NoDelay==1");
    v = -1;
    Check(NetSocketGetOption(&s, NetOption::KeepAlive, &v) == NetError::None && v == 1, "opt: get KeepAlive==1");
    v = -1;
    Check(NetSocketGetOption(&s, NetOption::Linger, &v) == NetError::None && v == 5, "opt: get Linger==5");
    Check(NetSocketClose(&s) == NetError::None, "opt: close stream socket");
}

void TestLoopbackEcho()
{
    SocketHandle listener{};
    if (NetSocketCreate(kAddressFamilyInet, kSocketTypeStream, kProtocolTcp, &listener) != NetError::None) {
        Check(false, "echo: create listener");
        return;
    }
    // Bind ephemeral port on loopback, then listen.
    NetAddress bind_addr = LoopbackV4(0);
    Check(NetSocketBind(&listener, bind_addr) == NetError::None, "echo: bind 127.0.0.1:0");
    Check(NetSocketListen(&listener, 8) == NetError::None, "echo: listen backlog=8");

    NetAddress local{};
    Check(NetSocketGetLocalAddress(&listener, &local) == NetError::None, "echo: get local address");
    Check(local.port != 0, "echo: ephemeral port != 0");

    SocketHandle client{};
    Check(NetSocketCreate(kAddressFamilyInet, kSocketTypeStream, kProtocolTcp, &client) == NetError::None,
          "echo: create client");
    Check(NetSocketConnect(&client, local, 5000) == NetError::None, "echo: client connect");

    SocketHandle peer{};
    NetAddress peer_addr{};
    Check(NetSocketAccept(&listener, &peer, &peer_addr, 5000) == NetError::None, "echo: server accept");
    Check(peer_addr.family == kAddressFamilyInet, "echo: peer addr family inet");
    Check(peer_addr.port != 0, "echo: peer addr ephemeral port != 0");
    Check(peer_addr.addr[0] == 127 && peer_addr.addr[1] == 0 &&
              peer_addr.addr[2] == 0 && peer_addr.addr[3] == 1,
          "echo: peer addr is 127.0.0.1");

    const char ping[] = "ping";
    CHAOS_IL2CPP_INT32 sent = 0;
    Check(NetSocketSend(&client, reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(ping), 4, 0, &sent, 5000) == NetError::None &&
              sent == 4,
          "echo: client send 4 bytes");

    CHAOS_IL2CPP_UINT8 rx[8]{};
    CHAOS_IL2CPP_INT32 got = 0;
    Check(NetSocketRecv(&peer, rx, 4, 0, &got, 5000) == NetError::None && got == 4 &&
              BytesEq(rx, ping, 4),
          "echo: server recv 'ping'");

    const char pong[] = "pong";
    sent = 0;
    Check(NetSocketSend(&peer, reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(pong), 4, 0, &sent, 5000) == NetError::None &&
              sent == 4,
          "echo: server send 4 bytes");

    CHAOS_IL2CPP_UINT8 ry[8]{};
    got = 0;
    Check(NetSocketRecv(&client, ry, 4, 0, &got, 5000) == NetError::None && got == 4 &&
              BytesEq(ry, pong, 4),
          "echo: client recv 'pong'");

    // Graceful close: server closes first, client recv sees 0.
    Check(NetSocketClose(&peer) == NetError::None, "echo: close server peer");
    got = -1;
    Check(NetSocketRecv(&client, ry, 4, 0, &got, 5000) == NetError::None && got == 0,
          "echo: client recv 0 after peer close");
    Check(NetSocketClose(&client) == NetError::None, "echo: close client");
    Check(NetSocketClose(&listener) == NetError::None, "echo: close listener");
}

void TestUnconnectedContract()
{
    SocketHandle h{};
    Check(NetSocketCreate(kAddressFamilyInet, kSocketTypeStream, kProtocolTcp, &h) == NetError::None,
          "unconnected: create socket");
    CHAOS_IL2CPP_INT32 sent = 0;
    const CHAOS_IL2CPP_UINT8 buf[2] = {0, 1};
    // No connect: send must fail as NotConnected and the fine-grained error
    // must be 10057 (abi.md / NT-4 contract).
    const NetError err = NetSocketSend(&h, buf, 2, 0, &sent, 1000);
    Check(err == NetError::NotConnected, "unconnected: send -> NotConnected");
    Check(NetSocketLastError() == SocketError::NotConnected, "unconnected: last error == 10057");
    Check(NetSocketClose(&h) == NetError::None, "unconnected: close");
}

void TestLayerBUnconnected()
{
    // Layer B entry (managed path lives in socket_ops.cpp): a Created
    // handle without a real fd reports NotConnected (10057), never touching
    // winsock.  Guards the fact subjects (Send_17_3/4 etc.).
    SocketHandle* h = SocketForInstance(static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0001));
    Check(h != nullptr, "layerB: table slot");
    if (h == nullptr) {
        return;
    }
    CHAOS_IL2CPP_INT32 error = 0;
    const CHAOS_IL2CPP_UINT8 buf[2] = {0, 0};
    const CHAOS_IL2CPP_INT32 rc = SocketSendBytes(h, buf, 2, 0, 2, 0, &error);
    Check(rc == 0 && error == 10057, "layerB: SocketSendBytes -> rc=0 err=10057");
    SocketHandle* h2 = SocketForInstance(static_cast<CHAOS_IL2CPP_INTPTR>(0xCAFE0002));
    Check(h2 != nullptr, "layerB: table slot 2");
    error = 0;
    CHAOS_IL2CPP_UINT8 rbuf[2] = {0, 0};
    const CHAOS_IL2CPP_INT32 rc2 = SocketReceiveBytes(h2, rbuf, 2, 0, 2, 0, &error);
    Check(rc2 == 0 && error == 10057, "layerB: SocketReceiveBytes -> rc=0 err=10057");
}

}  // namespace

int main()
{
    std::printf("[NET-TEST] chaos_net_test starting\n");
    if (NetStartup() != NetError::None) {
        Check(false, "NetStartup");
    } else {
        TestErrorTable();
        TestOptions();
        TestLoopbackEcho();
        TestUnconnectedContract();
        TestLayerBUnconnected();
        NetCleanup();
    }
    std::printf("[NET-TEST] %s (%d failure(s))\n",
                g_failures == 0 ? "ALL PASS" : "FAILED", g_failures);
    return g_failures == 0 ? 0 : 1;
}
