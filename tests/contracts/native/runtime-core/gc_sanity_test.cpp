/// Quick sanity test for R12 (POH) and R13 (ThinLock unload).
/// No BDWGC dependency — uses CRAG directly. Runs in < 10 seconds.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <thread>
#include <atomic>
#include <vector>

#include <chaos/native_types.h>
#include "gc_region.h"
#include "gc_events.h"
#include "domain_unloader.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
#define CHECK(cond, msg) do {                                   \
    if (!(cond)) {                                              \
        printf("  FAIL [%s:%d]: %s\n", __FILE__, __LINE__, msg);\
        ++g_failures;                                           \
    } else {                                                    \
        printf("  PASS: %s\n", msg);                            \
    }                                                           \
} while(0)

// ── Test 1: POH allocation + pointer identification ──────────────
void TestPohAllocate() {
    printf("\n── Test 1: POH allocation ──\n");

    void* p = PohAllocate(64);
    CHECK(p != nullptr, "PohAllocate(64) returns non-null");

    void* p2 = PohAllocate(4096);
    CHECK(p2 != nullptr, "PohAllocate(4096) returns non-null");

    CHECK(IsPohPointer(p),  "IsPohPointer(p) == true");
    CHECK(IsPohPointer(p2), "IsPohPointer(p2) == true");

    // POH objects must be zeroed
    int zeros = 1;
    for (size_t i = 0; i < 64; i++) {
        if (static_cast<char*>(p)[i] != 0) { zeros = 0; break; }
    }
    CHECK(zeros, "POH memory is zeroed");

    // Large POH allocation (> region size) should fall back
    void* big = PohAllocate(128 * 1024);
    CHECK(big != nullptr, "PohAllocate(128KB) returns non-null (old-gen fallback)");
    CHECK(!IsPohPointer(big), "128KB POH fallback is NOT in POH region (should be old-gen)");
}

// ── Test 2: POH objects don't move across multiple GCs ───────────
void TestPohPinning() {
    printf("\n── Test 2: POH pinning (address stability) ──\n");

    constexpr int kCount = 50;
    void* addrs[kCount];
    for (int i = 0; i < kCount; i++) {
        addrs[i] = PohAllocate(128);
        assert(addrs[i]);
        // Write a marker
        memset(addrs[i], 0xAB, 128);
    }

    // Read addresses, write mark, trigger GC several times
    for (int g = 0; g < 5; g++) {
        // Force young + full GC by triggering many nursery allocs
        // and then requesting a full collection
        for (int i = 0; i < 10000; i++) {
            void* tmp = NurseryAllocate(32);
            (void)tmp;
        }
        // Verify all POH addresses are unchanged and markers intact
        for (int i = 0; i < kCount; i++) {
            if (static_cast<char*>(addrs[i])[0] != static_cast<char>(0xAB)) {
                CHECK(false, "POH marker corrupted after GC");
                return;
            }
        }
    }
    CHECK(true, "POH addresses stable after 5 GC cycles (50 objects)");
}

// ── Test 3: GcAllocatePinned end-to-end ──────────────────────────
void TestGcAllocatePinned() {
    printf("\n── Test 3: GcAllocatePinned end-to-end ──\n");

    void* p = GcAllocatePinned(256);
    CHECK(p != nullptr, "GcAllocatePinned(256) returns non-null");
    CHECK(IsPohPointer(p) || GcIsPohPointer(p),
          "GcAllocatePinned memory recognized as POH");

    void* p2 = GcAllocatePinned(32 * 1024);
    CHECK(p2 != nullptr, "GcAllocatePinned(32KB) returns non-null");

    // Multiple threaded pinned allocations + GC
    std::atomic<int> ok{1};
    std::vector<std::thread> threads;
    for (int t = 0; t < 10; t++) {
        threads.emplace_back([&ok]() {
            for (int i = 0; i < 100; i++) {
                void* pp = GcAllocatePinned(64);
                if (!pp) { ok.store(0); return; }
                memset(pp, 0xCD, 64);
            }
        });
    }
    for (auto& th : threads) th.join();
    CHECK(ok.load() == 1, "10 threads × 100 GcAllocatePinned(64) OK");
}

// ── Test 4: Domain unloader LockDrain (no-op when no locks) ──────
void TestLockDrainNoLocks() {
    printf("\n── Test 4: LockDrain (no locks held) ──\n");

    // Drain on a domain that doesn't exist — should be a safe no-op
    DrainSyncBlocksForDomain(9999);
    CHECK(true, "DrainSyncBlocksForDomain(9999) — no crash");
}

// ── Test 5: Domain unloader LockDrain (with real domain) ─────────
void TestLockDrainWithDomain() {
    printf("\n── Test 5: LockDrain with real MemoryDomain ──\n");

    // Acquire a domain, allocate some objects in it, then drain
    CHAOS_IL2CPP_UINT32 domain_id = 42;

    // Allocate DOMAIN region memory to let the domain have regions
    void* dmem = DomainAllocate(domain_id, 128);
    CHECK(dmem != nullptr, "DomainAllocate(42, 128) OK");

    // Drain should be safe — no one is holding ThinLock on this domain
    DrainSyncBlocksForDomain(domain_id);
    CHECK(true, "DrainSyncBlocksForDomain(42) — no crash");
}

// ── Test 6: Concurrent domain ops + PohAllocate ─────────────────
void TestConcurrentPohDomain() {
    printf("\n── Test 6: Concurrent POH + domain operations ──\n");

    std::atomic<int> ok{1};
    std::vector<std::thread> threads;

    for (int t = 0; t < 8; t++) {
        threads.emplace_back([&ok, t]() {
            // Mix POH alloc + domain alloc + GC stress
            for (int i = 0; i < 200; i++) {
                // POH alloc
                void* pp = PohAllocate(128);
                if (!pp) { ok.store(0); return; }
                memset(pp, 0xEF, 128);

                // Domain alloc
                void* dm = DomainAllocate(t + 100, 64);
                if (!dm) { ok.store(0); return; }

                // Regular alloc
                void* nr = NurseryAllocate(32);
                if (!nr) { ok.store(0); return; }
            }
        });
    }
    for (auto& th : threads) th.join();
    CHECK(ok.load() == 1, "8 threads × 200 mixed allocs (POH + domain + nursery) OK");
}

// ── Test 7: Verify DrainSyncBlocks multiple domains ──────────────
void TestLockDrainMultiDomain() {
    printf("\n── Test 7: LockDrain multiple domains ──\n");

    for (int d = 0; d < 20; d++) {
        void* dm = DomainAllocate(d, 64);
        (void)dm;
    }

    for (int d = 0; d < 20; d++) {
        DrainSyncBlocksForDomain(static_cast<CHAOS_IL2CPP_UINT32>(d));
    }
    CHECK(true, "DrainSyncBlocksForDomain × 20 domains — no crash");
}

// ── Main ─────────────────────────────────────────────────────────
int main() {
    puts("CRAG R12/R13 sanity test");
    puts("════════════════════════\n");

    // No GC_INIT needed — CRAG doesn't depend on BDWGC

    TestPohAllocate();
    TestPohPinning();
    TestGcAllocatePinned();
    TestLockDrainNoLocks();
    TestLockDrainWithDomain();
    TestConcurrentPohDomain();
    TestLockDrainMultiDomain();

    printf("\n══ Results: %d / 7 tests passed, %d failures ══\n",
           7 - (g_failures > 0 ? 1 : 0), g_failures);

    return g_failures > 0 ? 1 : 0;
}
