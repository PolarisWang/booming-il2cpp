/// bootstrap_register_test.cpp — RegisterCodegen unit tests.
///
/// Tests parameter validation, happy path, invoker table population,
/// and duplicate registration behavior.

#include "bootstrap_test_fixture.h"

using namespace chaos::il2cpp::bootstrap;

// ── Null/Invalid parameter tests ─────────────────────────────────────────

TEST_F(BootstrapTestFixture, RegisterNullCodeRegistration) {
    const auto* bridge = GetBridge();
    ASSERT_NE(bridge, nullptr);

    // Null code_registration
    BridgeStatus status = bridge->register_codegen(nullptr, &meta_reg_, &options_);
    EXPECT_EQ(status, CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT);
}

TEST_F(BootstrapTestFixture, RegisterNullMetadataRegistration) {
    const auto* bridge = GetBridge();
    ASSERT_NE(bridge, nullptr);

    // Null metadata_registration
    BridgeStatus status = bridge->register_codegen(&code_reg_, nullptr, &options_);
    EXPECT_EQ(status, CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT);
}

TEST_F(BootstrapTestFixture, RegisterNullOptions) {
    const auto* bridge = GetBridge();
    ASSERT_NE(bridge, nullptr);

    // Null options
    BridgeStatus status = bridge->register_codegen(&code_reg_, &meta_reg_, nullptr);
    EXPECT_EQ(status, CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT);
}

TEST_F(BootstrapTestFixture, RegisterInvalidStructSize) {
    const auto* bridge = GetBridge();
    ASSERT_NE(bridge, nullptr);

    // Invalid struct_size on code_registration
    CodeRegistrationV0 bad_reg = code_reg_;
    bad_reg.struct_size = 1;
    BridgeStatus status = bridge->register_codegen(&bad_reg, &meta_reg_, &options_);
    EXPECT_EQ(status, CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT);
}

// ── Happy path ───────────────────────────────────────────────────────────

TEST_F(BootstrapTestFixture, RegisterHappyPath) {
    BridgeStatus status = RegisterMinimal();
    EXPECT_EQ(status, CHAOS_BRIDGE_STATUS_OK);

    // State should reflect registration
    const auto* state = PeekBootstrapState();
    ASSERT_NE(state, nullptr);
    EXPECT_TRUE(state->is_registered);
    EXPECT_FALSE(state->is_bootstrapped);
    EXPECT_NE(state->code_registration, nullptr);
    EXPECT_NE(state->metadata_registration, nullptr);
    EXPECT_NE(state->options, nullptr);
}

// ── Invoker table ────────────────────────────────────────────────────────

TEST_F(BootstrapTestFixture, RegisterPopulatesInvokerTable) {
    ASSERT_EQ(RegisterMinimal(), CHAOS_BRIDGE_STATUS_OK);

    // Invokers for registered tokens should be findable
    for (uint32_t i = 0; i < kInvokerCount; i++) {
        void* invoker = FindInvokerPointer(kBaseInvokerToken + i);
        EXPECT_NE(invoker, nullptr)
            << "Invoker for token " << (kBaseInvokerToken + i) << " should be non-null";
        EXPECT_EQ(invoker, SentinelThunk(0xB0000000u, i));
    }

    // Unknown token should return nullptr
    void* invoker = FindInvokerPointer(0xDEADu);
    EXPECT_EQ(invoker, nullptr);
}

// ── Duplicate registration ───────────────────────────────────────────────

TEST_F(BootstrapTestFixture, RegisterDuplicateCall) {
    ASSERT_EQ(RegisterMinimal(), CHAOS_BRIDGE_STATUS_OK);

    // Second registration should succeed (overwrites state)
    BridgeStatus status = RegisterMinimal();
    EXPECT_EQ(status, CHAOS_BRIDGE_STATUS_OK);

    // State should still reflect valid registration
    const auto* state = PeekBootstrapState();
    ASSERT_NE(state, nullptr);
    EXPECT_TRUE(state->is_registered);
}
