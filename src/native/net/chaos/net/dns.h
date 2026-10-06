// Layer F -- DNS resolution (NT-9).
//
// getaddrinfo-based name resolution with a hosts-file first lookup and a
// small TTL cache for positive results.  Design follows
// docs/dev/in-progress/net-cpp-architecture.md §9:
//   * synchronous  DnsResolveHost  -- getaddrinfo on the caller's thread
//   * asynchronous DnsResolveHostAsync -- worker thread runs getaddrinfo,
//     completion delivered through the Layer G pipeline (PostCompletion),
//     so the runtime's completion dispatcher decides the callback thread
//   * TTL cache of positive results (default 30s), keyed by lowercased host
//   * hosts file consulted before any network path (Windows
//     %SystemRoot%\System32\drivers\etc\hosts, POSIX /etc/hosts)
//   * literal dotted-quad / colon-hex IPs resolved without DNS
#pragma once
#include <chaos/net/net.h>
#include <chaos/net/net_api.h>

namespace chaos::net {

// Outcome of one DnsResolveHost query.
struct DnsResult {
    CHAOS_IL2CPP_UINT16 port;       // requested port, echoed back (host order)
    CHAOS_IL2CPP_UINT8  v4_count;   // number of valid entries in v4 (0..8)
    CHAOS_IL2CPP_UINT8  v6_count;   // number of valid entries in v6 (0..8)
    CHAOS_IL2CPP_UINT8  v4[8][4];   // resolved IPv4, network byte order
    CHAOS_IL2CPP_UINT8  v6[8][16];  // resolved IPv6, network byte order
    CHAOS_IL2CPP_UINT32 from_hosts; // 1 when served from the hosts file
    CHAOS_IL2CPP_UINT32 from_cache; // 1 when served from the TTL cache
};

// Asynchronous request record (see DnsResolveHostAsync).
struct DnsAsyncResolveRequest {
    const char* host;                 // borrowed; must outlive done
    CHAOS_IL2CPP_UINT16 port;
    DnsResult* result;                // caller-owned storage
    void (*done)(DnsAsyncResolveRequest*) noexcept;  // dispatcher thread
    void* platform{nullptr};          // internal DnsWork; freed before done
};

// Resolves host synchronously.  Lookup order: hosts file, literal-IP fast
// path, then getaddrinfo(AF_UNSPEC).  Positive results (hosts and
// getaddrinfo) are cached for kDnsCacheTtl (30s).  On success out is filled
// (at least one address) and NetError::None returned; on failure a non-None
// error is returned and *out is zeroed.  Requires NetStartup() beforehand
// (getaddrinfo on Windows needs WSAStartup).  host must be NUL-terminated.
NetError DnsResolveHost(const char* host, CHAOS_IL2CPP_UINT16 port,
                        DnsResult* out) noexcept;

// Asynchronous request.  Caller owns host/result and must keep them alive
// until done fires; resolution runs on a detached worker thread, and done
// is invoked on the completion-dispatcher thread (runtime: thread pool,
// tests: SpawnThreadDispatcher).  req->result is filled before done.
NetError DnsResolveHostAsync(DnsAsyncResolveRequest* req) noexcept;

// First resolved address as a NetAddress (used by DnsEndPoint / connect-
// by-name paths).  Literal IPs pass through unchanged.
NetError DnsResolveEndPoint(const char* host, CHAOS_IL2CPP_UINT16 port,
                            NetAddress* addr) noexcept;

// Removes all cache entries (tests / hosts-file retry).
void DnsCacheClear() noexcept;

// Test hook: redirects the hosts file path.  pass "" to restore the platform
// default.  Also clears the cache (paths are part of cache validity).
void DnsSetHostsPathForTest(const char* path) noexcept;

}  // namespace chaos::net
