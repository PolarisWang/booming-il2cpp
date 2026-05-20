/// gc_parallel_mark_test — ParallelMark work-stealing unit tests (GoogleTest).

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "gc_parallel_mark.h"
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

OldGenPage* CreateFakePages(int count) {
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

} // anonymous namespace

TEST(ParallelMarkTest, PushPopLocalLifo) {
    constexpr int kNumPages = 1;
    auto* pages = CreateFakePages(kNumPages);
    ASSERT_NE(pages, nullptr);
    OldGenPage* page_ptrs[1] = { pages };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 1);
    ASSERT_NE(ctx, nullptr);

    auto* w0 = &ctx->workers[0];
    MarkChunk c1 = {0, 0, 0xFF};
    MarkChunk c2 = {0, 1, 0xFF00};
    MarkChunk c3 = {0, 2, 0xFF0000};

    PushChunk(w0, c1);
    PushChunk(w0, c2);
    PushChunk(w0, c3);

    MarkChunk popped;
    EXPECT_TRUE(PopChunk(w0, &popped));
    EXPECT_EQ(popped.word_index, c3.word_index);

    PopChunk(w0, &popped);
    PopChunk(w0, &popped);
    EXPECT_FALSE(PopChunk(w0, &popped));

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

TEST(ParallelMarkTest, StealFromIdle) {
    constexpr int kNumPages = 1;
    auto* pages = CreateFakePages(kNumPages);
    ASSERT_NE(pages, nullptr);
    OldGenPage* page_ptrs[1] = { pages };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 2);
    ASSERT_NE(ctx, nullptr);

    MarkChunk stolen;
    EXPECT_FALSE(StealChunk(ctx, 1, &stolen));

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

TEST(ParallelMarkTest, StealFromBusy) {
    constexpr int kNumPages = 1;
    auto* pages = CreateFakePages(kNumPages);
    ASSERT_NE(pages, nullptr);
    OldGenPage* page_ptrs[1] = { pages };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 2);
    ASSERT_NE(ctx, nullptr);

    MarkChunk c1 = {0, 0, 0xFF};
    MarkChunk c2 = {0, 1, 0xFF00};
    PushChunk(&ctx->workers[0], c1);
    PushChunk(&ctx->workers[0], c2);

    MarkChunk stolen;
    EXPECT_TRUE(StealChunk(ctx, 1, &stolen));

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

TEST(ParallelMarkTest, StealAll) {
    constexpr int kNumPages = 1;
    auto* pages = CreateFakePages(kNumPages);
    ASSERT_NE(pages, nullptr);
    OldGenPage* page_ptrs[1] = { pages };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 2);
    ASSERT_NE(ctx, nullptr);

    MarkChunk c1 = {0, 0, 0xFF};
    PushChunk(&ctx->workers[0], c1);

    MarkChunk stolen;
    StealChunk(ctx, 1, &stolen);

    MarkChunk popped;
    EXPECT_FALSE(PopChunk(&ctx->workers[0], &popped));

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

TEST(ParallelMarkTest, InitDestroy) {
    constexpr int kNumPages = 2;
    auto* pages = CreateFakePages(kNumPages);
    ASSERT_NE(pages, nullptr);
    OldGenPage* page_ptrs[2] = { pages, pages + 1 };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 1);
    ASSERT_NE(ctx, nullptr);
    EXPECT_EQ(ctx->page_count, kNumPages);

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}

TEST(ParallelMarkTest, FlushPending) {
    constexpr int kNumPages = 1;
    auto* pages = CreateFakePages(kNumPages);
    ASSERT_NE(pages, nullptr);
    OldGenPage* page_ptrs[1] = { pages };

    auto* ctx = InitParallelMarkContext(page_ptrs, kNumPages, 1);
    ASSERT_NE(ctx, nullptr);

    auto* w0 = &ctx->workers[0];
    MarkChunk chunks[3] = {
        {0, 0, 0xFF}, {0, 1, 0xFF00}, {0, 2, 0xFF0000},
    };
    for (int i = 0; i < 3; i++) {
        PushChunk(w0, chunks[i]);
    }

    FlushPending(w0);
    MarkChunk popped;
    EXPECT_TRUE(PopChunk(w0, &popped));

    DestroyParallelMarkContext(ctx);
    std::free(pages);
}
