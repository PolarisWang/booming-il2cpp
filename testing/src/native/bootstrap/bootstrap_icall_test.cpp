/// bootstrap_icall_test.cpp — Icall resolution unit tests.
///
/// Tests ResolveIcall through the CodegenBridgeV0 interface for known,
/// unknown, and null icall names.
///
/// NOTE: All tests are consolidated into one to avoid issues with
/// BootstrapRuntime re-entry (the runtime can only bootstrap once
/// per process).

#include "bootstrap_test_fixture.h"

using namespace chaos::il2cpp::bootstrap;

TEST_F(BootstrapTestFixture, ResolveIcallLifecycle) {
    // ── Before bootstrap: all icalls return nullptr ────────────────
    BridgeStatus status = RegisterMinimal();
    ASSERT_EQ(status, CHAOS_BRIDGE_STATUS_OK);

    const auto* state = PeekBootstrapState();
    ASSERT_NE(state, nullptr);
    EXPECT_TRUE(state->is_registered);
    EXPECT_FALSE(state->is_bootstrapped);

    const auto* bridge = GetBridge();
    ASSERT_NE(bridge, nullptr);

    // Before BootstrapRuntime, is_bootstrapped=false → ResolveIcall returns nullptr
    void* func = bridge->resolve_icall("System.Console::WriteLine(System.String)");
    EXPECT_EQ(func, nullptr)
        << "ResolveIcall should return nullptr before BootstrapRuntime";

    // ── After bootstrap: known icalls resolve ──────────────────────
    status = BootstrapMinimal();
    ASSERT_EQ(status, CHAOS_BRIDGE_STATUS_OK) << "BootstrapMinimal must succeed";

    // Verify state after bootstrap
    state = PeekBootstrapState();
    ASSERT_NE(state, nullptr);
    ASSERT_TRUE(state->is_bootstrapped) << "State must be bootstrapped after BootstrapMinimal";

    // Get resolve_icall function pointer directly for stable access
    auto* resolve_fn = bridge->resolve_icall;

    // Null name → nullptr
    func = resolve_fn(nullptr);
    EXPECT_EQ(func, nullptr);

    // Console.WriteLine (no spaces in the icall constant)
    func = resolve_fn("System.Console::WriteLine(System.String)");
    EXPECT_NE(func, nullptr);

    // Delegate.Combine (no spaces in the icall constant: System.Delegate,System.Delegate)
    func = resolve_fn(
        "System.Delegate::Combine(System.Delegate,System.Delegate)");
    EXPECT_NE(func, nullptr);

    // String.Concat pair (no spaces in the icall constant)
    func = resolve_fn(
        "System.String::Concat(System.String,System.String)");
    EXPECT_NE(func, nullptr);

    // String.Concat triple (no spaces in the icall constant)
    func = resolve_fn(
        "System.String::Concat(System.String,System.String,System.String)");
    EXPECT_NE(func, nullptr);

    // Unknown icall → nullptr
    func = resolve_fn("NonExistentModule::NonExistentMethod()");
    EXPECT_EQ(func, nullptr);

    // Empty string → nullptr (no known icall is empty)
    func = resolve_fn("");
    EXPECT_EQ(func, nullptr);
}
