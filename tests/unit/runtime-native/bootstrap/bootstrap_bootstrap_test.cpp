/// bootstrap_bootstrap_test.cpp — BootstrapRuntime unit tests.
///
/// Tests preconditions, the full two-phase init lifecycle, and post-init
/// public API availability.

#include "bootstrap_test_fixture.h"

using namespace chaos::il2cpp::bootstrap;

// ── Precondition tests ───────────────────────────────────────────────────

TEST_F(BootstrapTestFixture, BootstrapWithoutRegister) {
    // BootstrapRuntime before RegisterCodegen should fail
    BridgeStatus status = BootstrapMinimal();
    EXPECT_EQ(status, CHAOS_BRIDGE_STATUS_NOT_READY);
}

// ── Happy path ───────────────────────────────────────────────────────────

TEST_F(BootstrapTestFixture, BootstrapHappyPath) {
    BridgeStatus status = FullInit();
    EXPECT_EQ(status, CHAOS_BRIDGE_STATUS_OK);

    const auto* state = PeekBootstrapState();
    ASSERT_NE(state, nullptr);
    EXPECT_TRUE(state->is_registered);
    EXPECT_TRUE(state->is_bootstrapped);
}

// ── Post-init API verification ───────────────────────────────────────────

TEST_F(BootstrapTestFixture, BootstrapPostInitAPIs) {
    ASSERT_EQ(FullInit(), CHAOS_BRIDGE_STATUS_OK);

    // 1. PeekBootstrapState — non-null, bootstrapped
    const auto* state = PeekBootstrapState();
    ASSERT_NE(state, nullptr);
    EXPECT_TRUE(state->is_bootstrapped);

    // 2. GetCodegenBridgeV0 — non-null, valid struct size
    const auto* bridge = GetCodegenBridgeV0();
    ASSERT_NE(bridge, nullptr);
    EXPECT_GE(bridge->struct_size, sizeof(CodegenBridgeV0));

    // 3. FindMethodPointerByToken — known tokens return non-null
    for (uint32_t i = 0; i < kMethodCount; i++) {
        void* ptr = FindMethodPointerByToken(kBaseMethodToken + i);
        EXPECT_NE(ptr, nullptr)
            << "Method pointer for token " << (kBaseMethodToken + i) << " should be non-null";
    }

    // 4. FindInvokerPointer — known invoker tokens return non-null
    for (uint32_t i = 0; i < kInvokerCount; i++) {
        void* ptr = FindInvokerPointer(kBaseInvokerToken + i);
        EXPECT_NE(ptr, nullptr)
            << "Invoker for token " << (kBaseInvokerToken + i) << " should be non-null";
    }

    // 5. FindRegisteredTypeCapabilityEntry — known type tokens return entry
    for (uint32_t i = 0; i < kTypeCapCount; i++) {
        const auto* entry = FindRegisteredTypeCapabilityEntry(kBaseTypeToken + i);
        EXPECT_NE(entry, nullptr)
            << "Type capability for token " << (kBaseTypeToken + i) << " should be non-null";
        if (entry) {
            EXPECT_EQ(entry->capability_info.value_size_bytes, 4u * (i + 1u));
        }
    }

    // 6. GetAotDomainId — non-zero (set during BootstrapRuntime step 5/6)
    auto domain_id = GetAotDomainId();
    EXPECT_NE(domain_id, 0u);

    // Unknown tokens should return nullptr
    EXPECT_EQ(FindMethodPointerByToken(0xDEADu), nullptr);
    EXPECT_EQ(FindInvokerPointer(0xDEADu), nullptr);
    EXPECT_EQ(FindRegisteredTypeCapabilityEntry(0xDEADu), nullptr);
}
