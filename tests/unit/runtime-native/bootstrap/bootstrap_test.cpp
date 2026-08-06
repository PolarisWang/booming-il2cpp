#include <gtest/gtest.h>
#include <chaos/common.h>
#include <bootstrap.h>

// Stubs for AOT runtime symbols normally provided by codegen/entrypoint
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

using namespace chaos::il2cpp::bootstrap;

TEST(BootstrapSmoke, PeekStateReturnsNonNullBeforeInit) {
    auto* state = PeekBootstrapState();
    EXPECT_NE(nullptr, state);
    EXPECT_FALSE(state->is_registered);
    EXPECT_FALSE(state->is_bootstrapped);
    EXPECT_EQ(0u, state->aot_domain_id);
}

TEST(BootstrapSmoke, GetAotDomainIdIsZeroBeforeInit) {
    auto id = GetAotDomainId();
    EXPECT_EQ(0u, id);
}

TEST(BootstrapSmoke, FindMethodPointerByZeroToken) {
    // Should not crash, should return nullptr for any token before bootstrap
    auto* ptr = FindMethodPointerByToken(0);
    EXPECT_EQ(nullptr, ptr);
}

TEST(BootstrapSmoke, FindInvokerPointerByZeroToken) {
    auto* ptr = FindInvokerPointer(0);
    EXPECT_EQ(nullptr, ptr);
}

TEST(BootstrapSmoke, GetCodegenBridgeReturnsNonNullBeforeInit) {
    auto* bridge = GetCodegenBridgeV0();
    EXPECT_NE(nullptr, bridge);
    // Bridge struct version should be valid even pre-init
    EXPECT_GE(bridge->struct_size, sizeof(CodegenBridgeV0));
}

TEST(BootstrapSmoke, FindTypeCapabilityIsNullBeforeInit) {
    auto* entry = FindRegisteredTypeCapabilityEntry(0);
    EXPECT_EQ(nullptr, entry);
}

TEST(BootstrapSmoke, BootstrapStateStructLayout) {
    // Verify the struct has expected fields at compile time
    BootstrapState state{};
    state.is_registered = false;
    state.is_bootstrapped = false;
    state.aot_domain_id = 0;
    state.aot_image_handle = 0;

    EXPECT_FALSE(state.is_registered);
    EXPECT_FALSE(state.is_bootstrapped);
    EXPECT_EQ(0u, state.aot_domain_id);
    EXPECT_EQ(0, state.aot_image_handle);
}
