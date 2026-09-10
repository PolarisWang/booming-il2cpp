/// common_finally_test — Unit tests for finally.h RAII guard.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/finally.h>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// FinallyGuard — basic execution
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonFinally, GuardExecutesOnScopeExit) {
    int flag = 0;
    {
        auto guard = make_finally_guard([&]() { flag = 42; });
        EXPECT_EQ(0, flag);
    }
    EXPECT_EQ(42, flag);
}

TEST(CommonFinally, GuardExecutesOnThrow) {
    int flag = 0;
    EXPECT_THROW({
        auto guard = make_finally_guard([&]() { flag = 77; });
        throw std::runtime_error("test");
    }, std::runtime_error);
    EXPECT_EQ(77, flag);
}

// ═══════════════════════════════════════════════════════════════════════════
// FinallyGuard — dismiss
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonFinally, DismissPreventsExecution) {
    int flag = 0;
    {
        auto guard = make_finally_guard([&]() { flag = 42; });
        guard.dismiss();
    }
    EXPECT_EQ(0, flag);
}

TEST(CommonFinally, DismissBeforeThrow) {
    int flag = 0;
    EXPECT_THROW({
        auto guard = make_finally_guard([&]() { flag = 99; });
        guard.dismiss();
        throw std::runtime_error("test");
    }, std::runtime_error);
    EXPECT_EQ(0, flag);
}

// ═══════════════════════════════════════════════════════════════════════════
// FinallyGuard — multiple guards
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonFinally, MultipleGuardsReverseOrder) {
    int order = 0;
    {
        auto g1 = make_finally_guard([&]() { order = order * 10 + 1; });
        auto g2 = make_finally_guard([&]() { order = order * 10 + 2; });
    }
    EXPECT_EQ(21, order);  // g2 runs first (reverse order of construction)
}
