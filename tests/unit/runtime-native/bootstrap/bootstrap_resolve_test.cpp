/// bootstrap_resolve_test.cpp — Virtual method resolution and type capability tests.
///
/// Tests FindMethodPointerByToken, FindRegisteredTypeCapabilityEntry,
/// and the resolve chain through the bootstrap state.

#include "bootstrap_test_fixture.h"

using namespace chaos::il2cpp::bootstrap;

// ── FindMethodPointerByToken tests ───────────────────────────────────────

TEST_F(BootstrapTestFixture, FindMethodPointerNullImage) {
    // Before registration, all lookups return nullptr
    void* ptr = FindMethodPointerByToken(kBaseMethodToken);
    EXPECT_EQ(ptr, nullptr);

    ptr = FindMethodPointerByToken(0);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(BootstrapTestFixture, FindMethodPointerAfterRegister) {
    RegisterMinimal();

    // Known tokens resolve to the sentinel thunks
    for (uint32_t i = 0; i < kMethodCount; i++) {
        void* ptr = FindMethodPointerByToken(kBaseMethodToken + i);
        EXPECT_NE(ptr, nullptr)
            << "Method pointer for token " << (kBaseMethodToken + i) << " should be non-null";
        EXPECT_EQ(ptr, SentinelThunk(0xA0000000u, i));
    }

    // Unknown token returns nullptr
    EXPECT_EQ(FindMethodPointerByToken(0xDEADu), nullptr);
}

// ── FindRegisteredTypeCapabilityEntry tests ──────────────────────────────

TEST_F(BootstrapTestFixture, TypeCapabilityFound) {
    RegisterMinimal();

    for (uint32_t i = 0; i < kTypeCapCount; i++) {
        const auto* entry = FindRegisteredTypeCapabilityEntry(kBaseTypeToken + i);
        EXPECT_NE(entry, nullptr)
            << "Type capability for token " << (kBaseTypeToken + i) << " should be found";
        if (entry) {
            EXPECT_EQ(entry->type_token, kBaseTypeToken + i);
            EXPECT_EQ(entry->capability_info.value_size_bytes, 4u * (i + 1u));
        }
    }
}

TEST_F(BootstrapTestFixture, TypeCapabilityNotFound) {
    RegisterMinimal();

    const auto* entry = FindRegisteredTypeCapabilityEntry(0xDEADu);
    EXPECT_EQ(entry, nullptr);

    entry = FindRegisteredTypeCapabilityEntry(0);
    EXPECT_EQ(entry, nullptr);
}
