#pragma once
#include <chaos/net/net_api.h>

// Layer D -- platform-neutral address interop (NT-10).
//
// Primitives carriers: System.Net.Primitives exposes IPAddress /
// IPEndPoint / EndPoint / SocketAddress / DnsEndPoint over the managed
// boundary.  The managed closure (system-net-sockets chunk) only carries
// these types as PARAMETER types -- there are no managed method bodies for
// them yet (no System.Net.Primitives assembly in foundation-dll), so the
// construction/conversion surface is provided on the native side:
//
//   NetAddress (chaos_net internal) <-> sockaddr_in / sockaddr_in6
//   NetAddress <-> IPAddress byte payload + family             (IPAddress)
//   NetAddress <-> address bytes + port                        (IPEndPoint)
//   NetAddress <-> raw SocketAddress byte layout              (SocketAddress)
//   dotted-quad / colon-hex text <-> NetAddress               (ToString/Parse)
//
// DnsEndPoint (host + port, resolved lazily) is DNS territory (NT-9,
// worktree-1b dns.cpp); NT-10 supplies its data carrier via the generic
// host pairing in address.cpp (DnsEndPoint helpers).
namespace chaos::net {

// NetAddress -> sockaddr_in / sockaddr_in6.  `storage` must point to a
// buffer of at least capacity bytes; returns bytes written (16 or 28) or
// 0 when the family is unsupported / buffer too small.  Port is converted
// to network byte order.
CHAOS_IL2CPP_INT32 NetAddressToSockaddr(const NetAddress& addr, void* storage,
                                        CHAOS_IL2CPP_INT32 capacity) noexcept;

// sockaddr_in / sockaddr_in6 -> NetAddress.  Pass the real length
// (sizeof sockaddr_in / sockaddr_in6); port comes back in host order.
// Returns false on unsupported family or a length mismatch.
bool NetAddressFromSockaddr(const void* storage, CHAOS_IL2CPP_INT32 length,
                            NetAddress* out) noexcept;

// Loopback addresses (127.0.0.1 / ::1).
NetAddress LoopbackFor(CHAOS_IL2CPP_INT32 family, CHAOS_IL2CPP_UINT16 port) noexcept;

// IPAddress helper: build NetAddress from the IPAddress byte payload
// (4 bytes IPv4 in network order, or 16 bytes IPv6) + AddressFamily.
NetAddress NetAddressFromIPBytes(CHAOS_IL2CPP_INT32 family,
                                 const CHAOS_IL2CPP_UINT8* bytes,
                                 CHAOS_IL2CPP_INT32 byteCount,
                                 CHAOS_IL2CPP_UINT16 port) noexcept;

// IPEndPoint helper: split NetAddress back into address payload +
// port.  Writes at most capacity bytes; returns bytes written (4 or 16)
// or 0 when capacity is too small.
CHAOS_IL2CPP_INT32 NetAddressToIPBytes(const NetAddress& addr,
                                       CHAOS_IL2CPP_UINT8* bytes,
                                       CHAOS_IL2CPP_INT32 capacity,
                                       CHAOS_IL2CPP_UINT16* port) noexcept;

// SocketAddress helper: serialize the raw (family + address + port) byte
// layout used by System.Net.SocketAddress -- [4B family LE][2B port BE]
// [addr bytes].  Returns bytes written or 0 on size mismatch.
CHAOS_IL2CPP_INT32 NetAddressToSocketAddressBytes(const NetAddress& addr,
                                                  CHAOS_IL2CPP_UINT8* bytes,
                                                  CHAOS_IL2CPP_INT32 capacity) noexcept;
bool NetAddressFromSocketAddressBytes(const CHAOS_IL2CPP_UINT8* bytes,
                                      CHAOS_IL2CPP_INT32 byteCount,
                                      NetAddress* out) noexcept;

// DnsEndPoint helper: host + port carrier.  host must fit the 256-byte
// inline buffer; returns false on overflow.
bool DnsEndPointTryCreate(const char* host, CHAOS_IL2CPP_INT32 hostLen,
                          CHAOS_IL2CPP_UINT16 port) noexcept;

// Dotted-quad / colon-hex text <-> NetAddress (no DNS; literal only).
// Format returns the string length (without NUL) or 0 on mismatch.
CHAOS_IL2CPP_INT32 NetAddressFormat(const NetAddress& addr, char* out,
                                    CHAOS_IL2CPP_INT32 capacity) noexcept;
bool NetAddressTryParseText(const char* text, CHAOS_IL2CPP_INT32 len,
                            NetAddress* out) noexcept;

}  // namespace chaos::net
