/// bootstrap_delegate_test.cpp — Delegate system unit tests.
///
/// Tests CreateDelegate, CombineDelegate, RemoveDelegate, and DelegateInvoke
/// through the CodegenBridgeV0 interface and icall resolution.
///
/// NOTE: Consolidated into a single monolithic test to avoid re-init
/// issues (BootstrapRuntime can only succeed once per process).

#include "bootstrap_test_fixture.h"

using namespace chaos::il2cpp::bootstrap;

#include <cstring>

// ── Local DelegateInstance definition (matching bootstrap.cpp layout) ────

struct TestDelegateInstance {
    uint32_t method_token;
    void* method_pointer;
    void* target_instance;
    TestDelegateInstance* next;
};

// ── Helper: get function pointers via icall resolution ───────────────────

using CombineFn = void* (*)(void*, void*);
using RemoveFn = void* (*)(void*, void*);

TEST_F(BootstrapTestFixture, DelegateLifecycle) {
    // ── Full two-phase init ────────────────────────────────────────
    BridgeStatus status = RegisterMinimal();
    ASSERT_EQ(status, CHAOS_BRIDGE_STATUS_OK) << "Register must succeed";
    status = BootstrapMinimal();
    ASSERT_EQ(status, CHAOS_BRIDGE_STATUS_OK) << "Bootstrap must succeed";

    const auto* bridge = GetBridge();
    ASSERT_NE(bridge, nullptr);

    // ── CreateDelegate tests ───────────────────────────────────────

    // null method handle → nullptr
    void* del = bridge->create_delegate(nullptr, nullptr, MethodInfoHandle{}, nullptr);
    EXPECT_EQ(del, nullptr);

    // Valid method handle (encoded as token value)
    MethodInfoHandle method_handle = static_cast<MethodInfoHandle>(kBaseMethodToken);
    del = bridge->create_delegate(nullptr, nullptr, method_handle, nullptr);
    ASSERT_NE(del, nullptr);
    if (del) {
        auto* node = static_cast<TestDelegateInstance*>(del);
        EXPECT_EQ(node->method_token, kBaseMethodToken);
        EXPECT_NE(node->method_pointer, nullptr);
        EXPECT_EQ(node->target_instance, nullptr);
        EXPECT_EQ(node->next, nullptr);
    }

    // Unknown token → nullptr (FindMethodPointerByToken returns null)
    MethodInfoHandle unknown_handle = static_cast<MethodInfoHandle>(0xDEADu);
    del = bridge->create_delegate(nullptr, nullptr, unknown_handle, nullptr);
    EXPECT_EQ(del, nullptr);

    // ── CombineDelegate tests (via icall resolution) ───────────────

    auto* combine = reinterpret_cast<CombineFn>(
        bridge->resolve_icall("System.Delegate::Combine(System.Delegate,System.Delegate)"));
    ASSERT_NE(combine, nullptr) << "CombineDelegate icall must resolve";

    // null left → returns right
    void* right = reinterpret_cast<void*>(static_cast<uintptr_t>(0x1234u));
    void* result = combine(nullptr, right);
    EXPECT_EQ(result, right);

    // null right → returns left
    void* left = reinterpret_cast<void*>(static_cast<uintptr_t>(0x5678u));
    result = combine(left, nullptr);
    EXPECT_EQ(result, left);

    // Both non-null — create actual delegates and combine
    MethodInfoHandle handle_a = static_cast<MethodInfoHandle>(kBaseMethodToken);
    MethodInfoHandle handle_b = static_cast<MethodInfoHandle>(kBaseMethodToken + 1);
    void* del_a = bridge->create_delegate(nullptr, nullptr, handle_a, nullptr);
    void* del_b = bridge->create_delegate(nullptr, nullptr, handle_b, nullptr);
    ASSERT_NE(del_a, nullptr);
    ASSERT_NE(del_b, nullptr);

    void* combined = combine(del_a, del_b);
    EXPECT_NE(combined, nullptr);
    // Combined should be a NEW node (cloned left), not the same as del_a
    EXPECT_NE(combined, del_a);

    // ── RemoveDelegate tests (via icall resolution) ────────────────

    auto* remove = reinterpret_cast<RemoveFn>(
        bridge->resolve_icall("System.Delegate::Remove(System.Delegate,System.Delegate)"));
    ASSERT_NE(remove, nullptr) << "RemoveDelegate icall must resolve";

    // Target not in source → returns source unchanged (no match by method_token)
    TestDelegateInstance unknown_node{};
    unknown_node.method_token = 0xDEADu;
    result = remove(del_a, &unknown_node);
    EXPECT_EQ(result, del_a);

    // Remove head → should return nullptr (the only node matches the target exactly)
    result = remove(del_a, del_a);
    EXPECT_EQ(result, nullptr);

    // ── DelegateInvoke test ────────────────────────────────────────
    ASSERT_NE(bridge->delegate_invoke, nullptr);
}
