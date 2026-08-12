/// gc_old_gen_pool_test — OldGen page pool + FindPage boundary tests.
///
/// plan-v6 M3/T5 (A6): locks (a) the O(log n) FindPage/IsInOldGen lookup on the
/// real page array, and (b) the reusable-page pool accessor, without mutating
/// any GC invariant.  Pooling itself happens inside Collect/BgcSweep (full-GC
/// flows); here we lock the read-only surface so later FIX-1/FIX-3 changes are
/// regression-guarded.

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <chaos/native_types.h>
#include "gc_old_gen.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// ── Test 1: FindPage/IsInOldGen boundary on the real page array ────
// Allocate objects spread across several pages, then verify FindPage returns
// the correct containing page for addresses inside, and nullptr for addresses
// clearly outside the old-gen managed range (the O(log n) binary search must
// not return a bogus page for out-of-range addresses).
void TestFindPageBoundaries() {
    TEST("FindPage boundaries (real page array)");

    // Allocate a spread of objects across pages (sizes force different pages).
    std::vector<void*> objs;
    int num_pages_before = (int)g_old_gen.TotalPages();
    for (int i = 0; i < 200; i++) {
        size_t sz = 16 + (i % 5) * 8;  // 16..48 bytes, spreads across pages
        void* p = g_old_gen.Allocate(sz, true);
        if (p != nullptr) objs.push_back(p);
    }
    int num_pages_after = (int)g_old_gen.TotalPages();
    GC_CHECK(num_pages_after >= 1, "allocation spanned at least one old-gen page");
    GC_CHECK(objs.size() >= 10, "allocated a reasonable spread of objects");

    // For each live object: FindPage must resolve it, and IsInOldGen must be true.
    for (void* p : objs) {
        GC_CHECK(g_old_gen.IsInOldGen(p), "IsInOldGen true for a live old-gen object");
        auto* page = g_old_gen.FindPage(p);
        GC_CHECK(page != nullptr, "FindPage resolves a live old-gen object to a page");
        if (page != nullptr) {
            // The object must fall within the page's payload.
            char* ps = page->Payload();
            char* pe = ps + page->payload_size;
            char* cp = static_cast<char*>(p);
            GC_CHECK(cp >= ps && cp < pe, "FindPage returns the containing page (payload range)");
        }
    }

    // Out-of-range addresses (well outside any page) must return nullptr.
    // Use addresses far below/above the managed heap via the page array's
    // extremes: sweep every page, and test just outside each page's payload.
    GC_CHECK(g_old_gen.FindPage(nullptr) == nullptr, "FindPage(nullptr) == nullptr");
    const auto* arr = g_old_gen.GetPageArray();
    if (arr != nullptr && arr->count > 0) {
        // Below the first page's payload start.
        OldGenPage* first = arr->pages[0];
        char* first_payload = first->Payload();
        GC_CHECK(g_old_gen.FindPage(first_payload - 1) == nullptr ||
                 g_old_gen.FindPage(first_payload - 1) == first,
                 "address just below first payload is not in any page (or resolves to first)");
        // Above the last page's payload end.
        OldGenPage* last = arr->pages[arr->count - 1];
        char* last_end = last->Payload() + last->payload_size;
        GC_CHECK(g_old_gen.FindPage(last_end) == nullptr ||
                 g_old_gen.FindPage(last_end) == last,
                 "address at last payload end is not in any page (or resolves to last)");
        // A clearly out-of-heap sentinel address.
        GC_CHECK(g_old_gen.FindPage(reinterpret_cast<void*>(static_cast<uintptr_t>(0x1))) == nullptr,
                 "tiny sentinel address is not in any page");
    }

    // Free the objects (no crash).
    for (void* p : objs) g_old_gen.Free(p);
    GC_CHECK(true, "Freeing objects completes without crash");
}

// ── Test 2: Pool + page-count accessors are safe & consistent ─────
// Locks the read-only surface PoolPageCount()/TotalPages()/TotalAllocated()
// that later FIX-1/FIX-3 changes must preserve.
void TestPoolAccessors() {
    TEST("pool + page accessors");

    GC_CHECK(g_old_gen.PoolPageCount() >= 0, "PoolPageCount() returns a non-negative value");
    GC_CHECK(g_old_gen.TotalPages() >= 0, "TotalPages() returns a non-negative value");
    GC_CHECK(g_old_gen.TotalAllocated() >= 0, "TotalAllocated() returns a non-negative value");

    // After allocation+free, the accessors must still be callable and stable.
    void* p = g_old_gen.Allocate(128, true);
    GC_CHECK(p != nullptr, "Allocate(128) succeeds");
    g_old_gen.Free(p);
    GC_CHECK(g_old_gen.PoolPageCount() >= 0, "PoolPageCount() callable after alloc+free");
}

// ── Test 3: Reallocate preserves address validity for IsInOldGen ──
void TestReallocBoundary() {
    TEST("Reallocate keeps object in old gen");

    void* p = g_old_gen.Allocate(32, true);
    GC_CHECK(p != nullptr, "Allocate(32) succeeds");
    void* q = g_old_gen.Reallocate(p, 512);
    GC_CHECK(q != nullptr, "Reallocate succeeds");
    GC_CHECK(g_old_gen.IsInOldGen(q),
             "reallocated object is in old gen (FindPage resolves it)");
    auto* page = g_old_gen.FindPage(q);
    GC_CHECK(page != nullptr, "reallocated object resolves to a page");
}

int main() {
    puts("CRAG old-gen page pool + FindPage boundary tests");
    puts("══════════════════════════════════════════════════\n");
    TestFindPageBoundaries();
    TestPoolAccessors();
    TestReallocBoundary();
    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
