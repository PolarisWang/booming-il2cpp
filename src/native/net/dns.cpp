// Layer F -- DNS resolution implementation (NT-9).
//
// Synchronous path: TTL cache -> hosts file -> literal-IP fast path ->
// getaddrinfo(AF_UNSPEC).  Positive results (hosts & getaddrinfo) are
// cached for a default 30s TTL; literal answers are deterministic and are
// not cached.  Lookup order follows net-cpp-architecture.md §9 (hosts
// prioritized over DNS).
//
// Asynchronous path: a detached std::thread runs the sync resolution, then
// hands a heap Completion to async::PostCompletion so the Layer G dispatcher
// (runtime: ThreadPoolQueueUserWorkItem; tests: SpawnThreadDispatcher) fires
// the caller's done callback off the submission thread.  The DnsWork record
// is freed after done() returns (OnNetCompletion frees the Completion).
//
// Deliberately dependency-light: no log.h/fmt, no socket_ops.  On Windows
// the caller must have called NetStartup() (WSAStartup) before
// getaddrinfo can succeed; hosts/literal paths need no startup call.
// Expose POSIX address-resolution APIs (getaddrinfo/addrinfo) even under
// strict language modes: glibc/MSYS2 keep them behind _POSIX_C_SOURCE.
#if !defined(_POSIX_C_SOURCE) && !defined(_WIN32)
#define _POSIX_C_SOURCE 200809L
#endif

#include <chaos/net/dns.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2ipdef.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

namespace chaos::net {
namespace {

using async::AsyncOp;
using async::Completion;

// Positive-result TTL (architecture §9: "TTL 小表（正结果）").
constexpr std::chrono::milliseconds kDnsCacheTtl{30000};

struct CacheEntry {
    DnsResult result;
    std::chrono::steady_clock::time_point stored;
};

std::mutex g_cache_mutex;
std::unordered_map<std::string, CacheEntry> g_cache;

std::mutex g_path_mutex;
std::string g_hosts_path_override;  // test hook; empty == platform default

inline std::string ToLower(std::string s) noexcept
{
    for (char& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

// Platform-default hosts file path.
std::string DefaultHostsPath() noexcept
{
#ifdef _WIN32
    const char* root = std::getenv("SystemRoot");
    return std::string(root != nullptr ? root : "C:\Windows") +
           "\System32\drivers\etc\hosts";
#else
    return "/etc/hosts";
#endif
}

std::string HostsPath() noexcept
{
    std::lock_guard<std::mutex> lk(g_path_mutex);
    return g_hosts_path_override.empty() ? DefaultHostsPath() : g_hosts_path_override;
}

// Parses an address token text (dotted-quad or colon-hex) into out.
// Returns true and fills out when the token is a valid literal IP.
bool SetFromLiteral(const char* text, CHAOS_IL2CPP_UINT16 port, DnsResult* out) noexcept
{
    in_addr a4{};
    if (inet_pton(AF_INET, text, &a4) == 1) {
        DnsResult r{};
        r.port = port;
        r.v4_count = 1;
        std::memcpy(r.v4[0], &a4, 4);
        *out = r;
        return true;
    }
    in6_addr a6{};
    if (inet_pton(AF_INET6, text, &a6) == 1) {
        DnsResult r{};
        r.port = port;
        r.v6_count = 1;
        std::memcpy(r.v6[0], &a6, 16);
        *out = r;
        return true;
    }
    return false;
}

// Scans the hosts file for the (lowercased) host.  Line grammar:
//   [addr]<ws>name<ws>[name...]   ('#' starts a comment).
bool TryHostsLookup(const std::string& want, CHAOS_IL2CPP_UINT16 port, DnsResult* out) noexcept
{
    std::FILE* f = std::fopen(HostsPath().c_str(), "r");
    if (f == nullptr) {
        return false;
    }
    char line[512];
    bool found = false;
    while (std::fgets(line, sizeof(line), f) != nullptr) {
        char* comment = std::strchr(line, '#');
        if (comment != nullptr) {
            *comment = '\0';
        }
        const char* p = line;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
            ++p;
        }
        if (*p == '\0') {
            continue;
        }
        // Address token.
        const char* addr_start = p;
        while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') {
            ++p;
        }
        std::string addr(addr_start, static_cast<size_t>(p - addr_start));
        // Hostname tokens.
        for (;;) {
            while (*p == ' ' || *p == '\t') {
                ++p;
            }
            if (*p == '\0' || *p == '\r' || *p == '\n') {
                break;
            }
            const char* name_start = p;
            while (*p != '\0' && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') {
                ++p;
            }
            std::string name(name_start, static_cast<size_t>(p - name_start));
            if (ToLower(name) == want) {
                if (SetFromLiteral(addr.c_str(), port, out)) {
                    out->from_hosts = 1;
                    found = true;
                }
                break;
            }
        }
        if (found) {
            break;
        }
    }
    std::fclose(f);
    return found;
}

// getaddrinfo(AF_UNSPEC): fills up to 8 v4 + 8 v6 addresses.  Returns true
// when at least one address was produced.
bool TryGetAddrInfo(const char* host, CHAOS_IL2CPP_UINT16 port, DnsResult* out) noexcept
{
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;  // IPv4 + IPv6
    hints.ai_socktype = 0;        // any socket type
    hints.ai_flags = 0;
    addrinfo* res = nullptr;
    const int rc = getaddrinfo(host, nullptr, &hints, &res);
    if (rc != 0 || res == nullptr) {
        return false;
    }
    DnsResult r{};
    r.port = port;
    for (const addrinfo* ai = res; ai != nullptr; ai = ai->ai_next) {
        if (ai->ai_family == AF_INET && r.v4_count < 8) {
            const auto* sa = reinterpret_cast<const sockaddr_in*>(ai->ai_addr);
            std::memcpy(r.v4[r.v4_count], &sa->sin_addr, 4);
            ++r.v4_count;
        } else if (ai->ai_family == AF_INET6 && r.v6_count < 8) {
            const auto* sa6 = reinterpret_cast<const sockaddr_in6*>(ai->ai_addr);
            std::memcpy(r.v6[r.v6_count], &sa6->sin6_addr, 16);
            ++r.v6_count;
        }
    }
    freeaddrinfo(res);
    if (r.v4_count == 0 && r.v6_count == 0) {
        return false;
    }
    *out = r;
    return true;
}

bool CacheGet(const std::string& key, DnsResult* out) noexcept
{
    std::lock_guard<std::mutex> lk(g_cache_mutex);
    const auto it = g_cache.find(key);
    if (it == g_cache.end()) {
        return false;
    }
    const auto now = std::chrono::steady_clock::now();
    if (now - it->second.stored >= kDnsCacheTtl) {
        g_cache.erase(it);
        return false;
    }
    *out = it->second.result;
    out->from_cache = 1;
    return true;
}

void CachePut(const std::string& key, const DnsResult& r) noexcept
{
    CacheEntry e;
    e.result = r;
    e.stored = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lk(g_cache_mutex);
    g_cache[key] = e;
}

// ── async plumbing ────────────────────────────────────────────────────
// DnsWork rides the Layer G Completion pipeline: the worker thread fills
// req->result, posts a Completion owning &work->op; OnNetCompletion runs
// DnsWorkDone which forwards to req->done and frees the work record.
struct DnsWork {
    DnsAsyncResolveRequest* req;
    AsyncOp op;
};

void DnsWorkDone(AsyncOp* op) noexcept
{
    auto* work = static_cast<DnsWork*>(op->platform);
    DnsAsyncResolveRequest* req = work->req;
    req->platform = nullptr;
    if (req->done != nullptr) {
        req->done(req);
    }
    delete work;
}

}  // namespace

NetError DnsResolveHost(const char* host, CHAOS_IL2CPP_UINT16 port, DnsResult* out) noexcept
{
    if (host == nullptr || host[0] == '\0' || out == nullptr) {
        return NetError::DnsFailure;
    }
    DnsResult r{};
    r.port = port;
    const std::string key = ToLower(host);

    if (CacheGet(key, &r)) {
        r.port = port;  // caller's port wins on cache hits
        *out = r;
        return NetError::None;
    }
    if (TryHostsLookup(key, port, &r)) {
        CachePut(key, r);
        *out = r;
        return NetError::None;
    }
    if (SetFromLiteral(host, port, &r)) {
        *out = r;  // deterministic; intentionally not cached
        return NetError::None;
    }
    if (TryGetAddrInfo(host, port, &r)) {
        CachePut(key, r);
        *out = r;
        return NetError::None;
    }
    return NetError::DnsFailure;
}

NetError DnsResolveHostAsync(DnsAsyncResolveRequest* req) noexcept
{
    if (req == nullptr || req->host == nullptr || req->result == nullptr ||
        req->done == nullptr) {
        return NetError::DnsFailure;
    }
    auto* work = new DnsWork{req, AsyncOp{}};
    work->op.on_done = DnsWorkDone;
    work->op.platform = work;
    req->platform = work;
    std::thread worker([req]() noexcept {
        auto* w = static_cast<DnsWork*>(req->platform);
        const NetError err = DnsResolveHost(req->host, req->port, req->result);
        auto* c = new Completion{};
        c->op = &w->op;
        c->bytes = 0;
        c->error = err;
        async::PostCompletion(c);
    });
    worker.detach();
    return NetError::None;
}

NetError DnsResolveEndPoint(const char* host, CHAOS_IL2CPP_UINT16 port,
                            NetAddress* addr) noexcept
{
    if (addr == nullptr) {
        return NetError::DnsFailure;
    }
    DnsResult r{};
    const NetError err = DnsResolveHost(host, port, &r);
    if (err != NetError::None) {
        return err;
    }
    NetAddress a{};
    a.family = kAddressFamilyInet;
    a.port = port;
    if (r.v4_count > 0) {
        std::memcpy(a.addr, r.v4[0], 4);
    } else if (r.v6_count > 0) {
        a.family = kAddressFamilyInet6;
        std::memcpy(a.addr, r.v6[0], 16);
    } else {
        return NetError::DnsFailure;
    }
    *addr = a;
    return NetError::None;
}

void DnsCacheClear() noexcept
{
    std::lock_guard<std::mutex> lk(g_cache_mutex);
    g_cache.clear();
}

void DnsSetHostsPathForTest(const char* path) noexcept
{
    {
        std::lock_guard<std::mutex> lk(g_path_mutex);
        if (path == nullptr || path[0] == '\0') {
            g_hosts_path_override.clear();
        } else {
            g_hosts_path_override = path;
        }
    }
    // Paths are part of cache validity: drop everything.
    DnsCacheClear();
}

}  // namespace chaos::net
