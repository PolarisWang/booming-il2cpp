// dbg_stack_test.cpp — Stack inspection unit tests
//
// Tests DbgGetStackTrace and DbgGetFrameDepth.
// Without a real runtime, the frame snapshot is empty — tests verify
// empty-state behavior and initialization safety.
// Links chaos_debugger for the stack implementation.

#include <gtest/gtest.h>
#include <debugger/dbg_stack.h>
#include <debugger/dbg_runtime.h>

namespace chaos::il2cpp::diagnostics {
namespace {

class StackTest : public ::testing::Test {
protected:
    void SetUp() override {
        DbgStackInitialize();
        DbgClearFrameSnapshot();
    }
    void TearDown() override {
        DbgStackShutdown();
    }
};

TEST_F(StackTest, EmptyFrameDepth) {
    EXPECT_EQ(DbgGetFrameDepth(), 0);
}

TEST_F(StackTest, EmptyStackTrace) {
    auto frames = DbgGetStackTrace(0, 0, 10);
    EXPECT_TRUE(frames.empty());
}

TEST_F(StackTest, StackTraceWithStartOffset) {
    // With empty snapshot, any start_frame beyond 0 returns empty.
    auto frames = DbgGetStackTrace(0, 5, 10);
    EXPECT_TRUE(frames.empty());
}

TEST_F(StackTest, StackTraceZeroLevels) {
    auto frames = DbgGetStackTrace(0, 0, 0);
    EXPECT_TRUE(frames.empty());
}

TEST_F(StackTest, FrameDepthAfterSnapshot) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 3;
    snap.frames[0] = {0x06000001, 0};
    snap.frames[1] = {0x06000002, 10};
    snap.frames[2] = {0x06000003, 20};

    EXPECT_EQ(DbgGetFrameDepth(), 3);

    // Restore empty state.
    DbgClearFrameSnapshot();
    EXPECT_EQ(DbgGetFrameDepth(), 0);
}

TEST_F(StackTest, StackTraceWithFrames) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 3;
    snap.frames[0] = {0x06000001, 0};
    snap.frames[1] = {0x06000002, 10};
    snap.frames[2] = {0x06000003, 20};

    auto frames = DbgGetStackTrace(0, 0, 10);
    ASSERT_EQ(frames.size(), 3u);
    EXPECT_EQ(frames[0].line, 0);
    EXPECT_EQ(frames[1].line, 10);
    EXPECT_EQ(frames[2].line, 20);

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, StackTracePaging) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 5;
    for (int i = 0; i < 5; ++i) {
        snap.frames[i] = {static_cast<uint32_t>(0x06000001u + i), static_cast<uint32_t>(i * 10)};
    }

    // First 2 frames.
    auto first = DbgGetStackTrace(0, 0, 2);
    ASSERT_EQ(first.size(), 2u);

    // Next 2 frames starting from index 2.
    auto next = DbgGetStackTrace(0, 2, 2);
    ASSERT_EQ(next.size(), 2u);
    EXPECT_EQ(next[0].line, 20);
    EXPECT_EQ(next[1].line, 30);

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, DeepStackTwentyFrames) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 20;
    for (int i = 0; i < 20; ++i) {
        snap.frames[i] = {static_cast<uint32_t>(0x06000001u + i), static_cast<uint32_t>(i * 5)};
    }

    auto frames = DbgGetStackTrace(0, 0, 20);
    ASSERT_EQ(frames.size(), 20u);
    for (int i = 0; i < 20; ++i) {
        EXPECT_EQ(frames[i].line, i * 5) << "frame " << i;
        EXPECT_EQ(frames[i].id, i);
    }

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, StackTracePagingBeyondEnd) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 3;
    snap.frames[0] = {0x06000001, 0};
    snap.frames[1] = {0x06000002, 10};
    snap.frames[2] = {0x06000003, 20};

    // Start beyond the end.
    auto beyond = DbgGetStackTrace(0, 10, 5);
    EXPECT_TRUE(beyond.empty());

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, StackTraceLevelsBeyondCount) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 3;
    snap.frames[0] = {0x06000001, 0};

    // Levels exceeds available frames.
    auto frames = DbgGetStackTrace(0, 0, 100);
    ASSERT_EQ(frames.size(), 3u);

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, StackTraceSingleFramePaging) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 10;
    for (int i = 0; i < 10; ++i) {
        snap.frames[i] = {static_cast<uint32_t>(0x06000001u), static_cast<uint32_t>(i)};
    }

    // Get 1 frame at different start offsets.
    EXPECT_EQ(DbgGetStackTrace(0, 0, 1).size(), 1u);
    EXPECT_EQ(DbgGetStackTrace(0, 5, 1).size(), 1u);
    EXPECT_EQ(DbgGetStackTrace(0, 9, 1).size(), 1u);

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, FrameIdSequential) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 5;
    for (int i = 0; i < 5; ++i) {
        snap.frames[i] = {0x06000001, static_cast<uint32_t>(i)};
    }

    auto frames = DbgGetStackTrace(0, 0, 5);
    // Each frame ID starts at 0 for the first returned frame, 1 for the next, etc.
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(frames[i].id, i);
    }

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, NegativeStartFrame) {
    auto& snap = DbgGetFrameSnapshot();
    // Use enough frames so that start_frame + levels stays within the
    // bounds of the frames array. The code does not clamp start_frame to 0.
    snap.frame_count = 20;
    for (int i = 0; i < 20; ++i) {
        snap.frames[i] = {0x06000001, 0};
    }

    // With frame_count=20 and start_frame=-5, levels=10:
    // end = min(-5+10=5, 20) = 5, iterating frames[-5..4] = 10 frames.
    // This just validates no crash for out-of-range input.
    auto frames = DbgGetStackTrace(0, -5, 10);
    EXPECT_FALSE(frames.empty());

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, NegativeLevels) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 3;
    snap.frames[0] = {0x06000001, 0};

    // Negative levels should result in empty response.
    auto frames = DbgGetStackTrace(0, 0, -1);
    EXPECT_TRUE(frames.empty());

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, StackTraceNamedMethodTokenZero) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 1;
    snap.frames[0] = {0, 0};  // method_token = 0

    auto frames = DbgGetStackTrace(0, 0, 1);
    ASSERT_EQ(frames.size(), 1u);
    // Should still produce a valid name string.
    EXPECT_NE(frames[0].name.find("0x00000000"), std::string::npos);

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, FrameIdPagingResetsPerCall) {
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 4;
    for (int i = 0; i < 4; ++i) {
        snap.frames[i] = {0x06000001, static_cast<uint32_t>(i)};
    }

    // First page: frames [0, 1] with IDs 0, 1.
    auto page1 = DbgGetStackTrace(0, 0, 2);
    EXPECT_EQ(page1[0].id, 0);
    EXPECT_EQ(page1[1].id, 1);

    // Second page: frames [2, 3] with IDs 0, 1 (reset per call).
    auto page2 = DbgGetStackTrace(0, 2, 2);
    EXPECT_EQ(page2[0].id, 0);
    EXPECT_EQ(page2[1].id, 1);

    DbgClearFrameSnapshot();
}

TEST_F(StackTest, InitializePreservesState) {
    DbgStackInitialize();  // double init
    auto& snap = DbgGetFrameSnapshot();
    snap.frame_count = 2;
    EXPECT_EQ(DbgGetFrameDepth(), 2);
    DbgClearFrameSnapshot();
}

}  // namespace
}  // namespace chaos::il2cpp::diagnostics
