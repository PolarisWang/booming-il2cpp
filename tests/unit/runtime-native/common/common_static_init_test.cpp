/// common_static_init_test — Unit tests for static_init.h once-init.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/static_init.h>
#include <mutex>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// ensure_initialized — basic
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonStaticInit, EnsureInitialized) {
    CHAOS_IL2CPP_ONCE_FLAG flag;
    int counter = 0;
    ensure_initialized(flag, [&]() { counter++; });
    EXPECT_EQ(1, counter);
    // Second call should NOT increment
    ensure_initialized(flag, [&]() { counter++; });
    EXPECT_EQ(1, counter);
}

// ═══════════════════════════════════════════════════════════════════════════
// ensure_initialized — concurrent safety (smoke)
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonStaticInit, ConcurrentInit) {
    CHAOS_IL2CPP_ONCE_FLAG flag;
    std::atomic<int> counter{0};
    auto worker = [&]() {
        for (int i = 0; i < 10; i++) {
            ensure_initialized(flag, [&]() { counter.fetch_add(1); });
        }
    };
    std::thread t1(worker);
    std::thread t2(worker);
    t1.join();
    t2.join();
    // Init should have run exactly once despite concurrent calls
    EXPECT_EQ(1, counter.load());
}
