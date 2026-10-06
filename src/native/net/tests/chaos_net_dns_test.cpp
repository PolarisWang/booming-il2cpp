// chaos_net_dns_test -- NT-9: DNS resolution (dns.cpp).
//
// Covers the Layer F lookup chain and the async pipeline:
//   1. hosts file (test hook path): name -> literal address, from_hosts=1,
//      multi-name lines, case-insensitive lookup.
//   2. literal-IP fast path: "127.0.0.1" / "::1" resolve without DNS.
//   3. TTL cache: second lookup of the same name hits the cache
//      (from_cache=1); DnsCacheClear() invalidates.
//   4. platform hosts (localhost): resolves to >=1 address via the real
//      system hosts file (override restored to default).
//   5. DnsResolveEndPoint: first address -> NetAddress (family/port/bytes).
//   6. async: DnsResolveHostAsync completes off the submission thread with
//      a filled result and the dispatcher running done.
//   7. unknown host -> NetError::DnsFailure.
// Build: (from tests/)  cmake -S . -B build -A x64
//        cmake --build build --config RelWithDebInfo
#include <chaos/net/dns.h>
#include <chaos/net/async_op.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <atomic>
#include <thread>
#include <chrono>

using chaos::net::DnsResult;
using chaos::net::DnsAsyncResolveRequest;
using chaos::net::NetError;
using chaos::net::NetAddress;
using chaos::net::kAddressFamilyInet;
using chaos::net::kAddressFamilyInet6;

static int g_failures = 0;
static std::atomic<unsigned> g_main_thread_id{0};

#define CHECK(cond, ...)                                                     \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::printf("[NET-DNS-TEST] FAIL %s:%d: ", __FILE__, __LINE__);  \
            std::printf(__VA_ARGS__);                                        \
            std::printf("\n");                                               \
            ++g_failures;                                                    \
        }                                                                    \
    } while (0)

static void Check(bool ok, const char* what) {
    if (ok) {
        std::printf("[NET-DNS-TEST] PASS  %s\n", what);
    } else {
        std::printf("[NET-DNS-TEST] FAIL  %s\n", what);
        ++g_failures;
    }
}

// Async completion dispatcher: runs on a fresh thread (proves done fires
// off the submission thread; runtime binary uses the thread pool instead).
void SpawnThreadDispatcher(void (*fn)(void*), void* arg) noexcept
{
    std::thread runner([fn, arg]() { fn(arg); });
    runner.detach();
}

// ── 1. hosts file (test-hook path) ────────────────────────────────────
static void TestHostsFileOverride() {
    std::printf("[NET-DNS-TEST] hosts-file override...\n");
    // Temp hosts file lives next to the test exe (build dir).
    const char* kHostsPath = "chaos_dns_test_hosts.txt";
    {
        std::ofstream f(kHostsPath, std::ios::out | std::ios::trunc);
        f << "# chaos dns test hosts\n";
        f << "127.0.0.1 chaos-test-host.local\n";
        f << "  10.0.0.5   alias.example.com  other.example.com\n";  // multi-name
        f << "::1      chaos-test-host6.local\n";
        f << "# trailing comment line - ignored\n";
    }
    chaos::net::DnsSetHostsPathForTest(kHostsPath);

    DnsResult r{};
    NetError e = chaos::net::DnsResolveHost("chaos-test-host.local", 8080, &r);
    Check(e == NetError::None, "hosts: chaos-test-host.local resolves");
    Check(e == NetError::None && r.from_hosts == 1, "hosts: from_hosts=1");
    Check(e == NetError::None && r.v4_count == 1 && r.v4[0][0] == 127 &&
          r.v4[0][1] == 0 && r.v4[0][2] == 0 && r.v4[0][3] == 1,
          "hosts: 127.0.0.1 bytes");
    Check(e == NetError::None && r.port == 8080, "hosts: port echoed");

    r = DnsResult{};
    e = chaos::net::DnsResolveHost("alias.example.com", 0, &r);
    Check(e == NetError::None && r.v4_count == 1 && r.v4[0][0] == 10 &&
          r.v4[0][1] == 0 && r.v4[0][2] == 0 && r.v4[0][3] == 5,
          "hosts: multi-name line (first alias)");

    r = DnsResult{};
    e = chaos::net::DnsResolveHost("OTHER.EXAMPLE.COM", 0, &r);
    Check(e == NetError::None && r.v4_count == 1 && r.v4[0][0] == 10,
          "hosts: case-insensitive lookup");

    r = DnsResult{};
    e = chaos::net::DnsResolveHost("chaos-test-host6.local", 53, &r);
    Check(e == NetError::None && r.from_hosts == 1 && r.v6_count == 1 &&
          r.v6[0][15] == 1, "hosts: ::1 v6 entry");
    for (int i = 0; i < 15; ++i) {
        CHECK(e == NetError::None && r.v6[0][i] == 0,
              "hosts: ::1 v6 byte[%d]=%u", i, r.v6[0][i]);
    }
}

// ── 2. literal-IP fast path ───────────────────────────────────────────
static void TestLiteralFastPath() {
    std::printf("[NET-DNS-TEST] literal fast path...\n");
    DnsResult r{};
    NetError e = chaos::net::DnsResolveHost("127.0.0.1", 9999, &r);
    Check(e == NetError::None && r.v4_count == 1 && r.from_hosts == 0,
          "literal: 127.0.0.1 v4 without hosts");
    Check(e == NetError::None && r.port == 9999, "literal: port echoed");
    r = DnsResult{};
    e = chaos::net::DnsResolveHost("::1", 0, &r);
    Check(e == NetError::None && r.v6_count == 1 && r.from_hosts == 0,
          "literal: ::1 v6 without hosts");
}

// ── 3. TTL cache ──────────────────────────────────────────────────────
static void TestCache() {
    std::printf("[NET-DNS-TEST] TTL cache...\n");
    chaos::net::DnsCacheClear();
    DnsResult r{};
    CHECK(chaos::net::DnsResolveHost("chaos-test-host.local", 80, &r) == NetError::None,
          "cache: first resolve ok");
    Check(r.from_cache == 0, "cache: first resolve misses (from_cache=0)");
    r = DnsResult{};
    CHECK(chaos::net::DnsResolveHost("chaos-test-host.local", 80, &r) == NetError::None,
          "cache: second resolve ok");
    Check(r.from_cache == 1 && r.v4_count == 1, "cache: second resolve hits (from_cache=1)");
    // Port reuse: cached payload carries the first port; caller gets its own.
    r = DnsResult{};
    CHECK(chaos::net::DnsResolveHost("chaos-test-host.local", 1234, &r) == NetError::None,
          "cache: second resolve ok (different port)");
    Check(r.from_cache == 1 && r.port == 1234,
          "cache: caller port echoed on cache hit");
    chaos::net::DnsCacheClear();
    r = DnsResult{};
    CHECK(chaos::net::DnsResolveHost("chaos-test-host.local", 80, &r) == NetError::None,
          "cache: post-clear resolve ok");
    Check(r.from_cache == 0 && r.from_hosts == 1,
          "cache: DnsCacheClear invalidates (from_cache=0, from_hosts=1)");
}

// ── 4. platform hosts (localhost) ─────────────────────────────────────
static void TestPlatformHosts() {
    std::printf("[NET-DNS-TEST] platform hosts (localhost)...\n");
    chaos::net::DnsSetHostsPathForTest("");  // restore default
    chaos::net::DnsCacheClear();
    DnsResult r{};
    // localhost must resolve with >=1 address on any machine with a hosts
    // file (127.0.0.1 and/or ::1 guaranteed present).
    const NetError e = chaos::net::DnsResolveHost("localhost", 0, &r);
    CHECK(e == NetError::None, "platform: localhost resolves (err=%d)", (int)e);
    const bool v4_ok = r.v4_count >= 1 && r.v4[0][0] == 127;
    const bool v6_ok = r.v6_count >= 1 && r.v6[0][15] == 1;
    Check(v4_ok || v6_ok, "platform: localhost -> 127.0.0.1 or ::1");
    Check(r.from_cache == 0, "platform: first resolve not from cache");
}

// ── 5. DnsResolveEndPoint ─────────────────────────────────────────────
static void TestEndPoint() {
    std::printf("[NET-DNS-TEST] DnsResolveEndPoint...\n");
    NetAddress a{};
    NetError e = chaos::net::DnsResolveEndPoint("127.0.0.1", 54321, &a);
    Check(e == NetError::None && a.family == kAddressFamilyInet && a.port == 54321,
          "endpoint: literal v4 family/port");
    Check(e == NetError::None && a.addr[0] == 127 && a.addr[1] == 0 &&
          a.addr[2] == 0 && a.addr[3] == 1, "endpoint: literal v4 bytes");
    // DnsEndPointTryCreate-style name resolution (hosts file guaranteed).
    const char* kHostsPath = "chaos_dns_test_hosts.txt";
    {
        std::ofstream f(kHostsPath, std::ios::out | std::ios::trunc);
        f << "192.168.1.7 endpoint-test.local\n";
    }
    chaos::net::DnsSetHostsPathForTest(kHostsPath);
    NetAddress b{};
    e = chaos::net::DnsResolveEndPoint("endpoint-test.local", 7, &b);
    CHECK(e == NetError::None, "endpoint: name resolves (err=%d)", (int)e);
    Check(e == NetError::None && b.family == kAddressFamilyInet && b.port == 7,
          "endpoint: name family/port");
    Check(e == NetError::None && b.addr[0] == 192 && b.addr[1] == 168 &&
          b.addr[2] == 1 && b.addr[3] == 7, "endpoint: name bytes");
    chaos::net::DnsSetHostsPathForTest("");  // restore default
}

// ── 6. async ──────────────────────────────────────────────────────────
static std::atomic<int> g_async_done{0};
static std::atomic<unsigned> g_async_thread_id{0};
static NetError g_async_err{NetError::Unsupported};
static DnsResult g_async_result{};
static CHAOS_IL2CPP_UINT16 g_async_port_saved = 0;

void OnDnsAsyncDone(DnsAsyncResolveRequest* req) noexcept
{
    g_async_thread_id =
        (unsigned)std::hash<std::thread::id>{}(std::this_thread::get_id());
    g_async_result = *req->result;
    g_async_port_saved = req->port;
    g_async_done.store(1, std::memory_order_release);
}

static void TestAsync() {
    std::printf("[NET-DNS-TEST] async resolve...\n");
    chaos::net::DnsCacheClear();
    DnsResult result{};
    DnsAsyncResolveRequest req{};
    req.host = "localhost";
    req.port = 443;
    req.result = &result;
    req.done = OnDnsAsyncDone;
    NetError e = chaos::net::DnsResolveHostAsync(&req);
    CHECK(e == NetError::None, "async: submit err=%d", (int)e);

    const auto deadline = std::chrono::steady_clock::now() +
                          std::chrono::seconds(5);
    while (!g_async_done.load(std::memory_order_acquire) &&
           std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    CHECK(g_async_done.load(std::memory_order_acquire) == 1, "async: done fired");
    Check(g_async_thread_id.load() != g_main_thread_id.load(),
          "async: done ran off submission thread");
    Check(g_async_result.v4_count + g_async_result.v6_count >= 1,
          "async: result has addresses");
    Check(g_async_port_saved == 443, "async: port carried through");
    CHECK(req.platform == nullptr, "async: platform released after done");
    g_async_done.store(0);
}

// ── 7. NXDOMAIN-style failure ─────────────────────────────────────────
static void TestUnknownHost() {
    std::printf("[NET-DNS-TEST] unknown host...\n");
    // Point hosts at an empty config so the name cannot be resolved from
    // the file; a synthetic suffix avoids any real NXDOMAIN round-trip.
    const char* kHostsPath = "chaos_dns_test_hosts_empty.txt";
    {
        std::ofstream f(kHostsPath, std::ios::out | std::ios::trunc);
        f << "";
    }
    chaos::net::DnsSetHostsPathForTest(kHostsPath);
    DnsResult r{};
    const NetError e = chaos::net::DnsResolveHost(
        "no-such-host.invalid", 0, &r);
    CHECK(e != NetError::None, "unknown: non-None error (err=%d)", (int)e);
    chaos::net::DnsSetHostsPathForTest("");
}

int main()
{
    std::printf("[NET-DNS-TEST] chaos_net dns tests starting\n");
    g_main_thread_id =
        (unsigned)std::hash<std::thread::id>{}(std::this_thread::get_id());
    if (chaos::net::NetStartup() != NetError::None) {
        std::printf("[NET-DNS-TEST] FAIL NetStartup\n");
        return 1;
    }
    chaos::net::async::SetCompletionDispatcher(SpawnThreadDispatcher);

    TestHostsFileOverride();
    TestLiteralFastPath();
    TestCache();
    TestPlatformHosts();
    TestEndPoint();
    TestAsync();
    TestUnknownHost();

    chaos::net::NetCleanup();
    if (g_failures == 0) {
        std::printf("[NET-DNS-TEST] ALL PASS\n");
        return 0;
    }
    std::printf("[NET-DNS-TEST] FAILURES: %d\n", g_failures);
    return 1;
}
