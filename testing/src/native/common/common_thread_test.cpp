#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/thread.h>

using namespace chaos::il2cpp::common;

TEST(CommonThread, AllocateManagedThreadId) {
    auto id1 = allocate_managed_thread_id();
    auto id2 = allocate_managed_thread_id();

    EXPECT_NE(id1, id2);
    EXPECT_GE(id1, 2);
    EXPECT_GE(id2, 2);
}

TEST(CommonThread, CurrentThreadObjectDefault) {
    EXPECT_EQ(0, current_thread_object);
}

TEST(CommonThread, CurrentManagedThreadIdDefault) {
    EXPECT_EQ(1, current_managed_thread_id);
}

TEST(CommonThread, TryGetNullEntry) {
    auto* entry = try_get_thread_runtime_entry(0);
    EXPECT_EQ(nullptr, entry);
}

TEST(CommonThread, TryGetUnregisteredEntry) {
    // A non-zero handle that isn't registered should return nullptr
    auto* entry = try_get_thread_runtime_entry(static_cast<CHAOS_IL2CPP_INTPTR>(0x12345678));
    EXPECT_EQ(nullptr, entry);
}
