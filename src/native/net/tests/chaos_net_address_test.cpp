// NT-10 -- Primitives address interop unit test.
//
// Exercises the address.cpp conversion surface against real
// sockaddr_in / sockaddr_in6 and the IPAddress / IPEndPoint /
// SocketAddress byte carriers.  The managed closure has no
// System.Net.Primitives method bodies (assembly gap, see claim), so
// construction/conversion is validated at the native layer.
//
// Build: (from tests/)
//   cmake -S . -B build -A x64
//   cmake --build build --config RelWithDebInfo
//   ./build/RelWithDebInfo/chaos_net_address_test.exe
#include <chaos/net/address.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif
#include <cstdio>
#include <cstring>
using chaos::net::NetAddress;
using chaos::net::kAddressFamilyInet;
using chaos::net::kAddressFamilyInet6;
static int g_failures = 0;
static void Check(bool ok, const char* what) {
    if (ok) {
        std::printf("[NET-ADDR-TEST] PASS  %s\n", what);
    } else {
        std::printf("[NET-ADDR-TEST] FAIL  %s\n", what);
        ++g_failures;
    }
}
static bool AddrEq(const NetAddress& a, const NetAddress& b) {
    return a.family == b.family && a.port == b.port &&
           std::memcmp(a.addr, b.addr, 16) == 0;
}
// IPAddress: build loopback from byte payload, round-trip through
// sockaddr_in, and back to IP bytes + port.
static void TestIPv4LoopbackRoundtrip() {
    const CHAOS_IL2CPP_UINT8 loopback4[4] = {127, 0, 0, 1};
    const CHAOS_IL2CPP_UINT16 port = 54321;
    NetAddress a = chaos::net::NetAddressFromIPBytes(kAddressFamilyInet, loopback4, 4, port);
    Check(a.family == kAddressFamilyInet && a.port == port, "IPAddress v4: NetAddress built from bytes");
    sockaddr_in sa{};
    CHAOS_IL2CPP_INT32 n = chaos::net::NetAddressToSockaddr(a, &sa, sizeof(sa));
    Check(n == static_cast<CHAOS_IL2CPP_INT32>(sizeof(sa)), "IPAddress v4: -> sockaddr_in size");
    Check(sa.sin_family == AF_INET, "IPAddress v4: sockaddr family");
    NetAddress back{};
    Check(chaos::net::NetAddressFromSockaddr(&sa, static_cast<CHAOS_IL2CPP_INT32>(sizeof(sa)), &back),
          "IPAddress v4: sockaddr -> NetAddress");
    Check(AddrEq(a, back), "IPAddress v4: full roundtrip equality");
    CHAOS_IL2CPP_UINT8 ipOut[16]{0};
    CHAOS_IL2CPP_UINT16 portOut = 0;
    CHAOS_IL2CPP_INT32 m = chaos::net::NetAddressToIPBytes(back, ipOut, 16, &portOut);
    Check(m == 4, "IPEndPoint v4: -> 4 bytes");
    Check(portOut == port, "IPEndPoint v4: port roundtrip");
    Check(std::memcmp(ipOut, loopback4, 4) == 0, "IPEndPoint v4: address bytes equal");
}
// IPv6 ::1 loopback through sockaddr_in6.
static void TestIPv6LoopbackRoundtrip() {
    const CHAOS_IL2CPP_UINT8 loopback6[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    const CHAOS_IL2CPP_UINT16 port = 7;
    NetAddress a = chaos::net::NetAddressFromIPBytes(kAddressFamilyInet6, loopback6, 16, port);
    Check(a.family == kAddressFamilyInet6 && a.port == port, "IPAddress v6: NetAddress built from bytes");
    sockaddr_in6 sa6{};
    CHAOS_IL2CPP_INT32 n = chaos::net::NetAddressToSockaddr(a, &sa6, sizeof(sa6));
    Check(n == static_cast<CHAOS_IL2CPP_INT32>(sizeof(sa6)), "IPAddress v6: -> sockaddr_in6 size");
    Check(sa6.sin6_family == AF_INET6, "IPAddress v6: sockaddr family");
    NetAddress back{};
    Check(chaos::net::NetAddressFromSockaddr(&sa6, static_cast<CHAOS_IL2CPP_INT32>(sizeof(sa6)), &back),
          "IPAddress v6: sockaddr_in6 -> NetAddress");
    Check(AddrEq(a, back), "IPAddress v6: full roundtrip equality");
    CHAOS_IL2CPP_UINT8 ipOut[16]{0};
    CHAOS_IL2CPP_UINT16 portOut = 0;
    CHAOS_IL2CPP_INT32 m = chaos::net::NetAddressToIPBytes(back, ipOut, 16, &portOut);
    Check(m == 16, "IPEndPoint v6: -> 16 bytes");
    Check(portOut == port, "IPEndPoint v6: port roundtrip");
    Check(std::memcmp(ipOut, loopback6, 16) == 0, "IPEndPoint v6: address bytes equal");
}
// SocketAddress carrier: serialize -> deserialize roundtrip.
static void TestSocketAddressCarrier() {
    const CHAOS_IL2CPP_UINT8 loopback4[4] = {127, 0, 0, 1};
    const CHAOS_IL2CPP_UINT16 port = 9999;
    NetAddress a = chaos::net::NetAddressFromIPBytes(kAddressFamilyInet, loopback4, 4, port);
    CHAOS_IL2CPP_UINT8 raw[32]{0};
    CHAOS_IL2CPP_INT32 n = chaos::net::NetAddressToSocketAddressBytes(a, raw, sizeof(raw));
    Check(n == 10, "SocketAddress v4: serialized size (6 + 4)");

    NetAddress back{};
    Check(chaos::net::NetAddressFromSocketAddressBytes(raw, n, &back), "SocketAddress v4: deserialize");
    Check(AddrEq(a, back), "SocketAddress v4: roundtrip equality");

    const CHAOS_IL2CPP_UINT8 loopback6[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    NetAddress a6 = chaos::net::NetAddressFromIPBytes(kAddressFamilyInet6, loopback6, 16, port);
    CHAOS_IL2CPP_INT32 m = chaos::net::NetAddressToSocketAddressBytes(a6, raw, sizeof(raw));
    Check(m == 22, "SocketAddress v6: serialized size (6 + 16)");
    NetAddress back6{};
    Check(chaos::net::NetAddressFromSocketAddressBytes(raw, m, &back6), "SocketAddress v6: deserialize");
    Check(AddrEq(a6, back6), "SocketAddress v6: roundtrip equality");
}
// IPAddress.ToString / Parse text roundtrip.
static void TestTextRoundtrip() {
    NetAddress v4{};
    Check(chaos::net::NetAddressTryParseText("127.0.0.1", 9, &v4), "Parse v4 dotted-quad");
    char buf[64]{0};
    CHAOS_IL2CPP_INT32 len = chaos::net::NetAddressFormat(v4, buf, sizeof(buf));
    Check(len == 9 && std::strcmp(buf, "127.0.0.1") == 0, "Format v4 dotted-quad");

    NetAddress v6{};
    Check(chaos::net::NetAddressTryParseText("::1", 3, &v6), "Parse v6 colon-hex");
    char buf6[64]{0};
    CHAOS_IL2CPP_INT32 len6 = chaos::net::NetAddressFormat(v6, buf6, sizeof(buf6));
    Check(len6 == 3 && std::strcmp(buf6, "::1") == 0, "Format v6 colon-hex");

    NetAddress bad{};
    Check(!chaos::net::NetAddressTryParseText("999.1.2.3", 9, &bad), "Reject bad dotted-quad");
    Check(!chaos::net::NetAddressTryParseText("not-an-ip", 9, &bad), "Reject plain text");
}
// DnsEndPoint carrier helper.
static void TestDnsEndPointCarrier() {
    Check(chaos::net::DnsEndPointTryCreate("localhost", 9, 8080), "DnsEndPoint: host fits");
    Check(!chaos::net::DnsEndPointTryCreate(nullptr, 0, 8080), "DnsEndPoint: null rejected");
}
// Failure paths: family mismatch rejected, small buffer rejected.
static void TestFailurePaths() {
    NetAddress bad = chaos::net::NetAddressFromIPBytes(42, nullptr, 4, 0);
    Check(bad.family == 0, "IPAddress: unknown family -> invalid");
    NetAddress ok = chaos::net::LoopbackFor(kAddressFamilyInet, 4242);
    Check(ok.family == kAddressFamilyInet && ok.port == 4242, "LoopbackFor v4");
    NetAddress ok6 = chaos::net::LoopbackFor(kAddressFamilyInet6, 4242);
    Check(ok6.family == kAddressFamilyInet6, "LoopbackFor v6");
    sockaddr_in sa{};
    CHAOS_IL2CPP_INT32 tiny = chaos::net::NetAddressToSockaddr(ok, &sa, 4);
    Check(tiny == 0, "ToSockaddr: small buffer rejected");
}

int main() {
    TestIPv4LoopbackRoundtrip();
    TestIPv6LoopbackRoundtrip();
    TestSocketAddressCarrier();
    TestTextRoundtrip();
    TestDnsEndPointCarrier();
    TestFailurePaths();
    if (g_failures == 0) {
        std::printf("[NET-ADDR-TEST] ALL PASS\n");
        return 0;
    }
    std::printf("[NET-ADDR-TEST] FAILED: %d\n", g_failures);
    return 1;
}
