// Layer D -- platform-neutral address interop implementation (NT-10).
//
// NetAddress <-> sockaddr_in / sockaddr_in6, IPAddress byte payload,
// SocketAddress raw layout, and literal text forms.  Pure conversion
// code -- no sockets, no DNS.  Port handling: NetAddress::port is
// HOST byte order; sockaddr ports are NETWORK byte order (converted
// with the platform htons/ntohs helpers).
#include <chaos/net/address.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <windows.h>
#include <cstring>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstring>
#endif

namespace chaos::net {
namespace {

// Host <-> network byte order for 16-bit ports.  On Windows htons/
// ntohs come from winsock2; on POSIX from arpa/inet.h.
inline CHAOS_IL2CPP_UINT16 HostToNetwork(CHAOS_IL2CPP_UINT16 v) noexcept {
#ifdef _WIN32
    return htons(v);
#else
    return ::htons(v);
#endif
}
inline CHAOS_IL2CPP_UINT16 NetworkToHost(CHAOS_IL2CPP_UINT16 v) noexcept {
#ifdef _WIN32
    return ntohs(v);
#else
    return ::ntohs(v);
#endif
}

}  // namespace

CHAOS_IL2CPP_INT32 NetAddressToSockaddr(const NetAddress& addr, void* storage,
                                        CHAOS_IL2CPP_INT32 capacity) noexcept
{
    if (addr.family == kAddressFamilyInet) {
        if (capacity < static_cast<CHAOS_IL2CPP_INT32>(sizeof(sockaddr_in))) {
            return 0;
        }
        sockaddr_in* sa = static_cast<sockaddr_in*>(storage);
        std::memset(sa, 0, sizeof(sockaddr_in));
        sa->sin_family = AF_INET;
        sa->sin_port = HostToNetwork(addr.port);
        std::memcpy(&sa->sin_addr, addr.addr, 4);  // first 4 bytes = IPv4
        return static_cast<CHAOS_IL2CPP_INT32>(sizeof(sockaddr_in));
    }
    if (addr.family == kAddressFamilyInet6) {
        if (capacity < static_cast<CHAOS_IL2CPP_INT32>(sizeof(sockaddr_in6))) {
            return 0;
        }
        sockaddr_in6* sa6 = static_cast<sockaddr_in6*>(storage);
        std::memset(sa6, 0, sizeof(sockaddr_in6));
        sa6->sin6_family = AF_INET6;
        sa6->sin6_port = HostToNetwork(addr.port);
        std::memcpy(&sa6->sin6_addr, addr.addr, 16);
        return static_cast<CHAOS_IL2CPP_INT32>(sizeof(sockaddr_in6));
    }
    return 0;
}

bool NetAddressFromSockaddr(const void* storage, CHAOS_IL2CPP_INT32 length,
                            NetAddress* out) noexcept
{
    if (out == nullptr || storage == nullptr) {
        return false;
    }
    const sockaddr* base = static_cast<const sockaddr*>(storage);
    if (base->sa_family == AF_INET) {
        if (length < static_cast<CHAOS_IL2CPP_INT32>(sizeof(sockaddr_in))) {
            return false;
        }
        const sockaddr_in* sa = static_cast<const sockaddr_in*>(storage);
        NetAddress a{};
        a.family = kAddressFamilyInet;
        a.port = NetworkToHost(sa->sin_port);
        std::memcpy(a.addr, &sa->sin_addr, 4);
        *out = a;
        return true;
    }
    if (base->sa_family == AF_INET6) {
        if (length < static_cast<CHAOS_IL2CPP_INT32>(sizeof(sockaddr_in6))) {
            return false;
        }
        const sockaddr_in6* sa6 = static_cast<const sockaddr_in6*>(storage);
        NetAddress a{};
        a.family = kAddressFamilyInet6;
        a.port = NetworkToHost(sa6->sin6_port);
        std::memcpy(a.addr, &sa6->sin6_addr, 16);
        *out = a;
        return true;
    }
    return false;
}

NetAddress LoopbackFor(CHAOS_IL2CPP_INT32 family, CHAOS_IL2CPP_UINT16 port) noexcept
{
    NetAddress a{};
    a.family = family;
    a.port = port;
    if (family == kAddressFamilyInet) {
        a.addr[0] = 127; a.addr[1] = 0; a.addr[2] = 0; a.addr[3] = 1;
    } else if (family == kAddressFamilyInet6) {
        a.addr[15] = 1;  // ::1
    }
    return a;
}

NetAddress NetAddressFromIPBytes(CHAOS_IL2CPP_INT32 family,
                                 const CHAOS_IL2CPP_UINT8* bytes,
                                 CHAOS_IL2CPP_INT32 byteCount,
                                 CHAOS_IL2CPP_UINT16 port) noexcept
{
    NetAddress a{};
    const CHAOS_IL2CPP_INT32 expected = (family == kAddressFamilyInet) ? 4 : 16;
    if (bytes == nullptr || byteCount != expected ||
        (family != kAddressFamilyInet && family != kAddressFamilyInet6)) {
        return a;  // zeroed (family == 0 => invalid)
    }
    a.family = family;
    a.port = port;
    std::memcpy(a.addr, bytes, static_cast<size_t>(expected));
    return a;
}

CHAOS_IL2CPP_INT32 NetAddressToIPBytes(const NetAddress& addr,
                                       CHAOS_IL2CPP_UINT8* bytes,
                                       CHAOS_IL2CPP_INT32 capacity,
                                       CHAOS_IL2CPP_UINT16* port) noexcept
{
    if (bytes == nullptr || port == nullptr) {
        return 0;
    }
    CHAOS_IL2CPP_INT32 n = 0;
    if (addr.family == kAddressFamilyInet) {
        n = 4;
    } else if (addr.family == kAddressFamilyInet6) {
        n = 16;
    } else {
        return 0;
    }
    if (capacity < n) {
        return 0;
    }
    std::memcpy(bytes, addr.addr, static_cast<size_t>(n));
    *port = addr.port;
    return n;
}

CHAOS_IL2CPP_INT32 NetAddressToSocketAddressBytes(const NetAddress& addr,
                                                  CHAOS_IL2CPP_UINT8* bytes,
                                                  CHAOS_IL2CPP_INT32 capacity) noexcept
{
    if (bytes == nullptr) {
        return 0;
    }
    CHAOS_IL2CPP_INT32 addrLen = 0;
    if (addr.family == kAddressFamilyInet) {
        addrLen = 4;
    } else if (addr.family == kAddressFamilyInet6) {
        addrLen = 16;
    } else {
        return 0;
    }
    // Layout: [4B family LE][2B port BE][addr bytes].
    const CHAOS_IL2CPP_INT32 total = 6 + addrLen;
    if (capacity < total) {
        return 0;
    }
    CHAOS_IL2CPP_INT32 fam = addr.family;
    std::memcpy(bytes, &fam, 4);
    const CHAOS_IL2CPP_UINT16 portBe = HostToNetwork(addr.port);
    std::memcpy(bytes + 4, &portBe, 2);
    std::memcpy(bytes + 6, addr.addr, static_cast<size_t>(addrLen));
    return total;
}

bool NetAddressFromSocketAddressBytes(const CHAOS_IL2CPP_UINT8* bytes,
                                      CHAOS_IL2CPP_INT32 byteCount,
                                      NetAddress* out) noexcept
{
    if (bytes == nullptr || out == nullptr || byteCount < 10) {
        return false;
    }
    CHAOS_IL2CPP_INT32 fam = 0;
    std::memcpy(&fam, bytes, 4);
    CHAOS_IL2CPP_INT32 addrLen = 0;
    if (fam == kAddressFamilyInet) {
        addrLen = 4;
    } else if (fam == kAddressFamilyInet6) {
        addrLen = 16;
    } else {
        return false;
    }
    if (byteCount < 6 + addrLen) {
        return false;
    }
    CHAOS_IL2CPP_UINT16 portBe = 0;
    std::memcpy(&portBe, bytes + 4, 2);
    NetAddress a{};
    a.family = fam;
    a.port = NetworkToHost(portBe);
    std::memcpy(a.addr, bytes + 6, static_cast<size_t>(addrLen));
    *out = a;
    return true;
}

bool DnsEndPointTryCreate(const char* host, CHAOS_IL2CPP_INT32 hostLen,
                          CHAOS_IL2CPP_UINT16 port) noexcept
{
    // Carrier helper (NT-10): validates the host fits the 256-byte
    // DnsEndPoint buffer.  Actual resolution is NT-9 dns.cpp.
    (void)port;
    return host != nullptr && hostLen > 0 && hostLen < 256;
}

CHAOS_IL2CPP_INT32 NetAddressFormat(const NetAddress& addr, char* out,
                                    CHAOS_IL2CPP_INT32 capacity) noexcept
{
    if (out == nullptr || capacity <= 0) {
        return 0;
    }
    char buf[INET6_ADDRSTRLEN]{0};
    const void* src = nullptr;
    CHAOS_IL2CPP_INT32 len = 0;
    if (addr.family == kAddressFamilyInet) {
        src = addr.addr;
#ifdef _WIN32
        if (inet_ntop(AF_INET, src, buf, sizeof(buf)) == nullptr) {
            return 0;
        }
#else
        if (::inet_ntop(AF_INET, src, buf, sizeof(buf)) == nullptr) {
            return 0;
        }
#endif
    } else if (addr.family == kAddressFamilyInet6) {
        src = addr.addr;
#ifdef _WIN32
        if (inet_ntop(AF_INET6, src, buf, sizeof(buf)) == nullptr) {
            return 0;
        }
#else
        if (::inet_ntop(AF_INET6, src, buf, sizeof(buf)) == nullptr) {
            return 0;
        }
#endif
    } else {
        return 0;
    }
    len = static_cast<CHAOS_IL2CPP_INT32>(std::strlen(buf));
    if (capacity < len + 1) {
        return 0;
    }
    std::memcpy(out, buf, static_cast<size_t>(len) + 1);
    return len;
}

bool NetAddressTryParseText(const char* text, CHAOS_IL2CPP_INT32 len,
                            NetAddress* out) noexcept
{
    if (text == nullptr || out == nullptr || len <= 0 || len >= 64) {
        return false;
    }
    char buf[64]{0};
    std::memcpy(buf, text, static_cast<size_t>(len));
    NetAddress a{};
    in_addr in4{};
#ifdef _WIN32
    if (inet_pton(AF_INET, buf, &in4) == 1) {
#else
    if (::inet_pton(AF_INET, buf, &in4) == 1) {
#endif
        a.family = kAddressFamilyInet;
        std::memcpy(a.addr, &in4, 4);
        *out = a;
        return true;
    }
    in6_addr in6{};
#ifdef _WIN32
    if (inet_pton(AF_INET6, buf, &in6) == 1) {
#else
    if (::inet_pton(AF_INET6, buf, &in6) == 1) {
#endif
        a.family = kAddressFamilyInet6;
        std::memcpy(a.addr, &in6, 16);
        *out = a;
        return true;
    }
    return false;
}

}  // namespace chaos::net
