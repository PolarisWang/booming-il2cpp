/// gc_parallel_mark_test — ParallelMark work-stealing unit tests.
///
/// Tests:
///   1. PushChunk / PopChunk LIFO order
///   2. StealChunk from idle worker returns false
///   3. StealChunk from busy worker succeeds
///   4. StealAll after chunks stolen
///   5. InitParallelMarkContext / DestroyParallelMarkContext
///   6. FlushPending reorders mark stack

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_parallel_mark.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_scheduler.h"
#include "gc_worker_pool.h"
#include "thread_state.h"
#include "gc_test_macros.h"

using namespace chaos::il2cpp::runtime_core;

static int g_failures = 0;
static int g_tests    = 0;
static int g_sub      = 0;

// Helper: create properly initialized fake old-gen pages.
static OldGenPage* CreateFakePages(int count) {
    size_t page_size = kOldGenPageSize;
    auto* mem = static_cast<char*>(std::calloc(count, page_size));
    if (!mem) return nullptr;
    for (int i = 0; i < count; i++) {
        auto* page = reinterpret_cast<OldGenPage*>(mem + i * page_size);
        page->page_size = kOldGenPageSize;
        page->bitmap_bytes = 256;
        page->payload_size = kOldGenPageSize - sizeof(OldGenPage) - page->bitmap_bytes;
    }
    return reinterpret_cast<OldGenPage*>(mem);
}

// ── Test 1: PushChunk / PopChunk LIFO ───────────────────────────────
void TestPushPopLocal() {
    TEST("PushPopLocal LIFO order");

    constexpr int kNumPages = 1;
    auto* pages = CreateFakePages(kNumPages);
    if (!pages) { GC_FAIL("CreateFakePages failed"); return; }
    OldGenPage* page_ptrs[1] = { pages };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 1);
    if (!ctx) { GC_FAIL("InitParallelMarkContext failed"); return; }

    auto* w0 = &ctx->workers[0];
    MarkChunk c1 = {0, 0, 0xFF};
    MarkChunk c2 = {0, 1, 0xFF00};
    MarkChunk c3 = {0, 2, 0xFF0000};

    PushChunk(w0, c1);
    PushChunk(w0, c2);
    PushChunk(w0, c3);

    MarkChunk popped;
    bool ok = PopChunk(w0, &popped);
    GC_CHECK(ok, "PopChunk succeeds");
    GC_CHECK(popped.word_index == c3.word_index, "PopChunk returns last pushed (LIFO)");

    PopChunk(w0, &popped);
    PopChunk(w0, &popped);
    ok = PopChunk(w0, &popped);
    GC_CHECK(!ok, "PopChunk returns false on empty");

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

// ── Test 2: Steal from idle worker ──────────────────────────────────
void TestStealFromIdle() {
    TEST("StealFromIdle returns false");

    constexpr int kNumPages = 1;
    auto* pages = CreateFakePages(kNumPages);
    if (!pages) { GC_FAIL("CreateFakePages failed"); return; }
    OldGenPage* page_ptrs[1] = { pages };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 2);
    if (!ctx) { GC_FAIL("InitParallelMarkContext failed"); return; }

    MarkChunk stolen;
    bool stole = StealChunk(ctx, 1, &stolen);  // worker 1 tries to steal
    GC_CHECK(!stole, "Steal from idle worker returns false");

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

// ── Test 3: Steal from busy worker succeeds ─────────────────────────
void TestStealFromBusy() {
    TEST("StealFromBusy succeeds");

    constexpr int kNumPages = 1;
    auto* pages = CreateFakePages(kNumPages);
    if (!pages) { GC_FAIL("CreateFakePages failed"); return; }
    OldGenPage* page_ptrs[1] = { pages };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 2);
    if (!ctx) { GC_FAIL("InitParallelMarkContext failed"); return; }

    MarkChunk c1 = {0, 0, 0xFF};
    MarkChunk c2 = {0, 1, 0xFF00};
    PushChunk(&ctx->workers[0], c1);
    PushChunk(&ctx->workers[0], c2);

    MarkChunk stolen;
    bool stole = StealChunk(ctx, 1, &stolen);
    GC_CHECK(stole, "Steal from busy worker succeeds");

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

// ── Test 4: StealAll — all chunks stolen → pop empty ────────────────
void TestStealAll() {
    TEST("StealAll leaves worker empty");

    constexpr int kNumPages = 1;
    auto* pages = CreateFakePages(kNumPages);
    if (!pages) { GC_FAIL("CreateFakePages failed"); return; }
    OldGenPage* page_ptrs[1] = { pages };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 2);
    if (!ctx) { GC_FAIL("InitParallelMarkContext failed"); return; }

    MarkChunk c1 = {0, 0, 0xFF};
    PushChunk(&ctx->workers[0], c1);

    MarkChunk stolen;
    StealChunk(ctx, 1, &stolen);

    MarkChunk popped;
    bool ok = PopChunk(&ctx->workers[0], &popped);
    GC_CHECK(!ok, "PopChunk empty after all chunks stolen");

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

// ── Test 5: Init / Destroy lifecycle ────────────────────────────────
void TestInitDestroy() {
    TEST("InitDestroy lifecycle");

    constexpr int kNumPages = 2;
    auto* pages = CreateFakePages(kNumPages);
    if (!pages) { GC_FAIL("CreateFakePages failed"); return; }
    OldGenPage* page_ptrs[2] = { pages, pages + 1 };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 1);
    GC_CHECK(ctx != nullptr, "InitParallelMarkContext returns non-null");
    GC_CHECK(ctx->page_count == kNumPages, "page_count set correctly");

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

// ── Test 6: FlushPending reorders ───────────────────────────────────
void TestFlushPending() {
    TEST("FlushPending reorders mark stack");

    constexpr int kNumPages = 1;
    auto* pages = CreateFakePages(kNumPages);
    if (!pages) { GC_FAIL("CreateFakePages failed"); return; }
    OldGenPage* page_ptrs[1] = { pages };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 1);
    if (!ctx) { GC_FAIL("InitParallelMarkContext failed"); return; }

    auto* w0 = &ctx->workers[0];
    MarkChunk chunks[3] = {
        {0, 0, 0xFF}, {0, 1, 0xFF00}, {0, 2, 0xFF0000},
    };
    for (int i = 0; i < 3; i++) {
        PushChunk(w0, chunks[i]);
    }

    FlushPending(w0);
    MarkChunk popped;
    bool ok = PopChunk(w0, &popped);
    GC_CHECK(ok, "PopChunk succeeds after FlushPending");

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

// ── Main ────────────────────────────────────────────────────────────
int main() {
    puts("ParallelMark unit tests");
    puts("═══════════════════════\n");

    TestPushPopLocal();
    TestStealFromIdle();
    TestStealFromBusy();
    TestStealAll();
    TestInitDestroy();
    TestFlushPending();

    printf("\nResults: %d tests, %d failures\n", g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
