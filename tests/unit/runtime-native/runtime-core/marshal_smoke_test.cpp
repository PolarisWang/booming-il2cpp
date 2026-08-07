// marshal_smoke_test.cpp — Smoke test for MarshalTestFixture.
//
// Verifies that the test fixture infrastructure correctly initializes the
// runtime so that marshal functions can execute.
// This is the Phase A validation gate.

#include "marshal_test_fixture.h"

#include <engine_binding.h>

#include <cstdint>

using namespace chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// Fixture smoke tests
// ════════════════════════════════════════════════════════════════════════════

class MarshalSmokeTest : public MarshalTestFixture {
};

TEST_F(MarshalSmokeTest, PreInitBridgeIsAvailable) {
    const CodegenBridgeV0* bridge = GetBridge();
    EXPECT_NE(bridge, nullptr);
}

TEST_F(MarshalSmokeTest, RuntimeStateIsAvailable) {
    EXPECT_NE(GetRuntimeState(), nullptr);
    EXPECT_NE(GetThreadState(), nullptr);
}

TEST_F(MarshalSmokeTest, MarshalAllocHGlobalReturnsNonNull) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    auto ptr = MarshalAllocHGlobal(rs, 64);
    EXPECT_NE(ptr, 0);

    EXPECT_TRUE(MarshalFreeHGlobal(rs, ptr));
}

TEST_F(MarshalSmokeTest, MarshalAllocCoTaskMemReturnsNonNull) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    auto ptr = MarshalAllocCoTaskMem(rs, 64);
    EXPECT_NE(ptr, 0);

    EXPECT_TRUE(MarshalFreeCoTaskMem(rs, ptr));
}

TEST_F(MarshalSmokeTest, MarshalReadWriteInt32) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    auto ptr = MarshalAllocCoTaskMem(rs, 16);
    ASSERT_NE(ptr, 0);

    MarshalWriteInt32(ptr, 0, 42);
    EXPECT_EQ(MarshalReadInt32(ptr, 0), 42);

    MarshalWriteInt32(ptr, 4, -1);
    EXPECT_EQ(MarshalReadInt32(ptr, 4), -1);

    MarshalFreeCoTaskMem(rs, ptr);
}

TEST_F(MarshalSmokeTest, MarshalAllocHGlobalNullState) {
    auto ptr = MarshalAllocHGlobal(nullptr, 64);
    EXPECT_EQ(ptr, 0);
}

TEST_F(MarshalSmokeTest, MarshalAllocCoTaskMemNegativeSize) {
    auto* rs = GetRuntimeState();
    ASSERT_NE(rs, nullptr);

    auto ptr = MarshalAllocCoTaskMem(rs, -1);
    EXPECT_EQ(ptr, 0);
}
