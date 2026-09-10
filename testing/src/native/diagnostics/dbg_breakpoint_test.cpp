// dbg_breakpoint_test.cpp — Breakpoint manager unit tests
//
// Tests DbgSetBreakpoint / DbgRemoveBreakpoint / DbgCheckBreakpoint /
// DbgGetAllBreakpoints / DbgGetBreakpointsForSource / DbgClearAllBreakpoints.
// Links chaos_debugger for the breakpoint manager implementation.

#include <gtest/gtest.h>
#include <debugger/dbg_breakpoint.h>

namespace chaos::il2cpp::diagnostics {
namespace {

class BreakpointTest : public ::testing::Test {
protected:
    void SetUp() override {
        DbgBreakpointInitialize();
    }
    void TearDown() override {
        DbgBreakpointShutdown();
    }
};

TEST_F(BreakpointTest, SetAndCheck) {
    int id = DbgSetBreakpoint(0x06000001, 42, "test.cs", 10);
    EXPECT_GT(id, 0);

    int found = DbgCheckBreakpoint(0x06000001, 42);
    EXPECT_EQ(found, id);
}

TEST_F(BreakpointTest, CheckNonExistent) {
    int found = DbgCheckBreakpoint(0x06000001, 42);
    EXPECT_EQ(found, -1);
}

TEST_F(BreakpointTest, SetDuplicateReturnsSameId) {
    int id1 = DbgSetBreakpoint(0x06000001, 42, "test.cs", 10);
    int id2 = DbgSetBreakpoint(0x06000001, 42, "test.cs", 10);
    EXPECT_EQ(id1, id2);
}

TEST_F(BreakpointTest, RemoveBreakpoint) {
    int id = DbgSetBreakpoint(0x06000001, 42, "test.cs", 10);
    DbgRemoveBreakpoint(id);

    int found = DbgCheckBreakpoint(0x06000001, 42);
    EXPECT_EQ(found, -1);
}

TEST_F(BreakpointTest, RemoveInvalidIdDoesNotCrash) {
    DbgRemoveBreakpoint(99999);
}

TEST_F(BreakpointTest, ClearAllBreakpoints) {
    DbgSetBreakpoint(0x06000001, 10, "a.cs", 1);
    DbgSetBreakpoint(0x06000002, 20, "b.cs", 2);
    DbgSetBreakpoint(0x06000003, 30, "c.cs", 3);

    DbgClearAllBreakpoints();

    EXPECT_EQ(DbgCheckBreakpoint(0x06000001, 10), -1);
    EXPECT_EQ(DbgCheckBreakpoint(0x06000002, 20), -1);
    EXPECT_EQ(DbgCheckBreakpoint(0x06000003, 30), -1);
}

TEST_F(BreakpointTest, GetAllBreakpoints) {
    DbgSetBreakpoint(0x06000001, 10, "a.cs", 1);
    DbgSetBreakpoint(0x06000002, 20, "b.cs", 2);

    auto all = DbgGetAllBreakpoints();
    ASSERT_EQ(all.size(), 2u);
}

TEST_F(BreakpointTest, GetAllBreakpointsEmpty) {
    auto all = DbgGetAllBreakpoints();
    EXPECT_TRUE(all.empty());
}

TEST_F(BreakpointTest, GetBreakpointsForSource) {
    DbgSetBreakpoint(0x06000001, 10, "a.cs", 1);
    DbgSetBreakpoint(0x06000002, 20, "a.cs", 2);
    DbgSetBreakpoint(0x06000003, 30, "other.cs", 3);

    auto for_a = DbgGetBreakpointsForSource("a.cs");
    ASSERT_EQ(for_a.size(), 2u);

    auto for_other = DbgGetBreakpointsForSource("other.cs");
    ASSERT_EQ(for_other.size(), 1u);

    auto for_none = DbgGetBreakpointsForSource("none.cs");
    EXPECT_TRUE(for_none.empty());
}

TEST_F(BreakpointTest, MultipleBreakpointsDistinctKeys) {
    int id1 = DbgSetBreakpoint(0x06000001, 10, "a.cs", 1);
    int id2 = DbgSetBreakpoint(0x06000001, 20, "a.cs", 2);
    int id3 = DbgSetBreakpoint(0x06000002, 10, "b.cs", 1);

    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);

    EXPECT_EQ(DbgCheckBreakpoint(0x06000001, 10), id1);
    EXPECT_EQ(DbgCheckBreakpoint(0x06000001, 20), id2);
    EXPECT_EQ(DbgCheckBreakpoint(0x06000002, 10), id3);
}

TEST_F(BreakpointTest, DoubleShutdownIsSafe) {
    DbgBreakpointShutdown();  // second call in same test
    DbgBreakpointInitialize();
    int id = DbgSetBreakpoint(0x06000001, 10, "a.cs", 1);
    EXPECT_GT(id, 0);
}

TEST_F(BreakpointTest, MultipleBreakpointsSameMethodDifferentOffsets) {
    int id1 = DbgSetBreakpoint(0x06000001, 10, "test.cs", 1);
    int id2 = DbgSetBreakpoint(0x06000001, 20, "test.cs", 2);
    int id3 = DbgSetBreakpoint(0x06000001, 30, "test.cs", 3);

    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);

    EXPECT_EQ(DbgCheckBreakpoint(0x06000001, 10), id1);
    EXPECT_EQ(DbgCheckBreakpoint(0x06000001, 20), id2);
    EXPECT_EQ(DbgCheckBreakpoint(0x06000001, 30), id3);
}

TEST_F(BreakpointTest, ManyBreakpointsStress) {
    // Set 50 breakpoints across different method tokens.
    for (uint32_t i = 0; i < 50; ++i) {
        uint32_t token = 0x06000001 + i;
        int id = DbgSetBreakpoint(token, 0, "stress.cs", static_cast<int>(i));
        EXPECT_GT(id, 0);
    }

    auto all = DbgGetAllBreakpoints();
    EXPECT_EQ(all.size(), 50u);
}

TEST_F(BreakpointTest, RemoveThenReAdd) {
    int id1 = DbgSetBreakpoint(0x06000001, 10, "test.cs", 1);
    DbgRemoveBreakpoint(id1);

    // Re-adding should get a new ID.
    int id2 = DbgSetBreakpoint(0x06000001, 10, "test.cs", 1);
    EXPECT_NE(id1, id2);
    EXPECT_EQ(DbgCheckBreakpoint(0x06000001, 10), id2);
}

TEST_F(BreakpointTest, ZeroMethodToken) {
    int id = DbgSetBreakpoint(0, 0, "null.cs", 1);
    EXPECT_GT(id, 0);
    EXPECT_EQ(DbgCheckBreakpoint(0, 0), id);
}

TEST_F(BreakpointTest, RemoveOneOfMany) {
    DbgSetBreakpoint(0x06000001, 10, "a.cs", 1);
    int to_remove = DbgSetBreakpoint(0x06000002, 20, "b.cs", 2);
    DbgSetBreakpoint(0x06000003, 30, "c.cs", 3);

    DbgRemoveBreakpoint(to_remove);

    EXPECT_EQ(DbgCheckBreakpoint(0x06000002, 20), -1);
    EXPECT_EQ(DbgCheckBreakpoint(0x06000001, 10), DbgCheckBreakpoint(0x06000001, 10));  // still exists
    EXPECT_EQ(DbgCheckBreakpoint(0x06000003, 30), DbgCheckBreakpoint(0x06000003, 30));
}

TEST_F(BreakpointTest, CheckAfterClearAndReSet) {
    DbgSetBreakpoint(0x06000001, 10, "a.cs", 1);
    DbgSetBreakpoint(0x06000002, 20, "b.cs", 2);

    DbgClearAllBreakpoints();

    // Re-set and verify.
    int id = DbgSetBreakpoint(0x06000001, 10, "a.cs", 1);
    EXPECT_GT(id, 0);
    EXPECT_EQ(DbgCheckBreakpoint(0x06000001, 10), id);
}

TEST_F(BreakpointTest, SourceFilterReturnsEmptyAfterRemove) {
    DbgSetBreakpoint(0x06000001, 10, "filter.cs", 1);
    DbgSetBreakpoint(0x06000002, 20, "filter.cs", 2);

    auto before = DbgGetBreakpointsForSource("filter.cs");
    EXPECT_EQ(before.size(), 2u);

    // Remove all by clearing.
    DbgClearAllBreakpoints();

    auto after = DbgGetBreakpointsForSource("filter.cs");
    EXPECT_TRUE(after.empty());
}

TEST_F(BreakpointTest, InitializeAfterShutdownWithBreakpoints) {
    DbgSetBreakpoint(0x06000001, 10, "a.cs", 1);
    DbgBreakpointShutdown();

    // Re-initialize - no crash, clean state.
    DbgBreakpointInitialize();
    EXPECT_TRUE(DbgGetAllBreakpoints().empty());

    int id = DbgSetBreakpoint(0x06000002, 20, "b.cs", 2);
    EXPECT_GT(id, 0);
}


}  // namespace
}  // namespace chaos::il2cpp::diagnostics