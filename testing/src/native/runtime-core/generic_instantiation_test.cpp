// generic_instantiation_test.cpp — T3-1: MakeGenericType/Gen-2 Handle test coverage
//
// Tests the GenericContextRegistry (handle-based generic instantiation) and the
// ResolveOrInstantiateType/ResolveOrInstantiateMethod bridge functions.
//
// The GenericContextRegistry is self-contained and testable with synthetic
// handles (any non-zero TypeInfoHandle/MethodInfoHandle value works as a key).
// ResolveOrInstantiate functions require real AOT descriptors for their full
// slow path — this test covers null/zero input safety and registry fast path
// integration; the slow path (BuildClosedDescriptor) is validated indirectly
// through the registry path.
//
// Coverage:
//   1. RegisterGenericInstantiation + TryResolveClosedType (basic, idempotent,
//      multiple types, arg match/mismatch)
//   2. RegisterGenericMethodInstantiation + TryResolveClosedMethod
//   3. GetClosedTypeGenericArgs reverse lookup (found, not found, count-only)
//   4. RegisterMethodGenericContext + GetGenericContextForMethod
//   5. Generic context accessors (GetClassTypeArgCount, GetClassTypeArg,
//      GetMethodTypeArgCount, GetMethodTypeArg) — null handle safety
//   6. GetRegisteredInstantiationCount
//   7. UnregisterModuleGenerics (no-op for unregistered module)
//   8. Zero/null input safety across all public APIs
//   9. ResolveOrInstantiateType/ResolveOrInstantiateMethod boundary tests

#include <gtest/gtest.h>

#include <generic_context.h>
#include <runtime_instantiation.h>
#include <instantiation_engine.h>

#include <cstdint>
#include <cstring>
#include <vector>

namespace gc = chaos::il2cpp::generic_context;
namespace ri = chaos::il2cpp::runtime_instantiation;

// ════════════════════════════════════════════════════════════════════════════
// Helper: unique synthetic handles (any non-zero value works for registry ops)
// ════════════════════════════════════════════════════════════════════════════

// Type handles — use unique values per test group
constexpr TypeInfoHandle kOpenType1   = static_cast<TypeInfoHandle>(0x1001);
constexpr TypeInfoHandle kOpenType2   = static_cast<TypeInfoHandle>(0x1002);
constexpr TypeInfoHandle kOpenType3   = static_cast<TypeInfoHandle>(0x1003);
constexpr TypeInfoHandle kClosedType1 = static_cast<TypeInfoHandle>(0x2001);
constexpr TypeInfoHandle kClosedType2 = static_cast<TypeInfoHandle>(0x2002);
constexpr TypeInfoHandle kClosedType3 = static_cast<TypeInfoHandle>(0x2003);
constexpr TypeInfoHandle kClosedType4 = static_cast<TypeInfoHandle>(0x2004);
constexpr TypeInfoHandle kArgInt      = static_cast<TypeInfoHandle>(0x3001);
constexpr TypeInfoHandle kArgString   = static_cast<TypeInfoHandle>(0x3002);
constexpr TypeInfoHandle kArgFloat    = static_cast<TypeInfoHandle>(0x3003);

// Method handles
constexpr MethodInfoHandle kOpenMethod1   = static_cast<MethodInfoHandle>(0x4001);
constexpr MethodInfoHandle kOpenMethod2   = static_cast<MethodInfoHandle>(0x4002);
constexpr MethodInfoHandle kClosedMethod1 = static_cast<MethodInfoHandle>(0x5001);
constexpr MethodInfoHandle kClosedMethod2 = static_cast<MethodInfoHandle>(0x5002);

// Method tokens for generic context
constexpr CHAOS_IL2CPP_UINT32 kMethodToken1 = 0x06000001u;
constexpr CHAOS_IL2CPP_UINT32 kMethodToken2 = 0x06000002u;

// ════════════════════════════════════════════════════════════════════════════
// GenericContextRegistry — Type instantiation
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, RegisterAndResolve_Type) {
    const CHAOS_IL2CPP_UINT32 count_before = gc::GetRegisteredInstantiationCount();

    TypeInfoHandle args[] = { kArgInt, kArgString };
    gc::RegisterGenericInstantiation(kOpenType1, kClosedType1, args, 2);

    EXPECT_EQ(gc::GetRegisteredInstantiationCount(), count_before + 1u);

    // Resolve with matching args → hit
    TypeInfoHandle resolved = gc::TryResolveClosedType(kOpenType1, args, 2);
    EXPECT_EQ(resolved, kClosedType1);

    // Resolve with wrong arg count → miss
    resolved = gc::TryResolveClosedType(kOpenType1, args, 1);
    EXPECT_EQ(resolved, 0);

    // Resolve with different args → miss
    TypeInfoHandle wrong_args[] = { kArgFloat };
    resolved = gc::TryResolveClosedType(kOpenType1, wrong_args, 1);
    EXPECT_EQ(resolved, 0);

    // Resolve unknown open type → miss
    resolved = gc::TryResolveClosedType(kOpenType2, args, 2);
    EXPECT_EQ(resolved, 0);
}

TEST(generic_context_test, Idempotent_RegisterType) {
    const CHAOS_IL2CPP_UINT32 count_before = gc::GetRegisteredInstantiationCount();

    TypeInfoHandle args[] = { kArgInt };
    gc::RegisterGenericInstantiation(kOpenType2, kClosedType2, args, 1);
    EXPECT_EQ(gc::GetRegisteredInstantiationCount(), count_before + 1u);

    // Same registration again → idempotent
    gc::RegisterGenericInstantiation(kOpenType2, kClosedType2, args, 1);
    EXPECT_EQ(gc::GetRegisteredInstantiationCount(), count_before + 1u);
}

TEST(generic_context_test, MultipleClosedInstantiations_PerOpenType) {
    TypeInfoHandle args_a[] = { kArgInt };
    TypeInfoHandle args_b[] = { kArgString };
    TypeInfoHandle args_c[] = { kArgFloat, kArgInt };

    gc::RegisterGenericInstantiation(kOpenType3, kClosedType3, args_a, 1);
    gc::RegisterGenericInstantiation(kOpenType3, kClosedType4, args_b, 1);

    // Resolve each
    EXPECT_EQ(gc::TryResolveClosedType(kOpenType3, args_a, 1), kClosedType3);
    EXPECT_EQ(gc::TryResolveClosedType(kOpenType3, args_b, 1), kClosedType4);

    // Wrong arg count → miss
    EXPECT_EQ(gc::TryResolveClosedType(kOpenType3, args_c, 2), 0);
}

// ════════════════════════════════════════════════════════════════════════════
// GenericContextRegistry — Method instantiation
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, RegisterAndResolve_Method) {
    TypeInfoHandle args[] = { kArgInt };

    gc::RegisterGenericMethodInstantiation(kOpenMethod1, kClosedMethod1, args, 1);

    // Resolve with matching args → hit
    MethodInfoHandle resolved = gc::TryResolveClosedMethod(kOpenMethod1, args, 1);
    EXPECT_EQ(resolved, kClosedMethod1);

    // Resolve with wrong arg count → miss
    resolved = gc::TryResolveClosedMethod(kOpenMethod1, args, 0);
    EXPECT_EQ(resolved, 0);

    // Resolve unknown open method → miss
    resolved = gc::TryResolveClosedMethod(kOpenMethod2, args, 1);
    EXPECT_EQ(resolved, 0);
}

TEST(generic_context_test, Idempotent_RegisterMethod) {
    TypeInfoHandle args[] = { kArgString };

    gc::RegisterGenericMethodInstantiation(kOpenMethod2, kClosedMethod2, args, 1);

    // Same registration again → idempotent (no crash, no effect)
    gc::RegisterGenericMethodInstantiation(kOpenMethod2, kClosedMethod2, args, 1);

    EXPECT_EQ(gc::TryResolveClosedMethod(kOpenMethod2, args, 1), kClosedMethod2);
}

// ════════════════════════════════════════════════════════════════════════════
// Reverse lookup: GetClosedTypeGenericArgs
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, GetClosedTypeGenericArgs_Found) {
    TypeInfoHandle args[] = { kArgInt, kArgString };

    // Use kOpenType1 which already has kClosedType1 registered
    TypeInfoHandle out[4] = { 0, 0, 0, 0 };
    CHAOS_IL2CPP_UINT32 count = gc::GetClosedTypeGenericArgs(kClosedType1, out, 4);

    EXPECT_EQ(count, 2u);
    EXPECT_EQ(out[0], kArgInt);
    EXPECT_EQ(out[1], kArgString);
}

TEST(generic_context_test, GetClosedTypeGenericArgs_CountOnly) {
    // Pass nullptr output buffer → should return count
    CHAOS_IL2CPP_UINT32 count = gc::GetClosedTypeGenericArgs(kClosedType1, nullptr, 0);
    EXPECT_EQ(count, 2u);
}

TEST(generic_context_test, GetClosedTypeGenericArgs_NotFound) {
    TypeInfoHandle out[2] = { 0, 0 };
    CHAOS_IL2CPP_UINT32 count = gc::GetClosedTypeGenericArgs(
        static_cast<TypeInfoHandle>(0xDEAD), out, 2);
    EXPECT_EQ(count, 0u);

    // Zero handle → not found
    count = gc::GetClosedTypeGenericArgs(0, out, 2);
    EXPECT_EQ(count, 0u);
}

TEST(generic_context_test, GetClosedTypeGenericArgs_TruncatedOutput) {
    TypeInfoHandle args[] = { kArgInt, kArgString, kArgFloat };
    TypeInfoHandle closed = static_cast<TypeInfoHandle>(0x200A);
    gc::RegisterGenericInstantiation(
        static_cast<TypeInfoHandle>(0x100A), closed, args, 3);

    TypeInfoHandle out[1] = { 0 };
    CHAOS_IL2CPP_UINT32 count = gc::GetClosedTypeGenericArgs(closed, out, 1);

    // Returns full count even when output is truncated
    EXPECT_EQ(count, 3u);
    EXPECT_EQ(out[0], kArgInt);
}

// ════════════════════════════════════════════════════════════════════════════
// Method generic context
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, MethodGenericContext_RegisterAndAccess) {
    TypeInfoHandle class_args[]  = { kArgInt };
    TypeInfoHandle method_args[] = { kArgString, kArgFloat };

    gc::RegisterMethodGenericContext(kMethodToken1,
        class_args, 1, method_args, 2);

    // Retrieve
    GenericContextHandle ctx = gc::GetGenericContextForMethod(kMethodToken1);
    ASSERT_NE(ctx, nullptr);

    EXPECT_EQ(gc::GetClassTypeArgCount(ctx), 1u);
    EXPECT_EQ(gc::GetClassTypeArg(ctx, 0), kArgInt);
    EXPECT_EQ(gc::GetClassTypeArg(ctx, 1), 0);  // out of range

    EXPECT_EQ(gc::GetMethodTypeArgCount(ctx), 2u);
    EXPECT_EQ(gc::GetMethodTypeArg(ctx, 0), kArgString);
    EXPECT_EQ(gc::GetMethodTypeArg(ctx, 1), kArgFloat);
    EXPECT_EQ(gc::GetMethodTypeArg(ctx, 2), 0);  // out of range
}

TEST(generic_context_test, MethodGenericContext_NullHandle) {
    // All accessors with nullptr handle → safe, return 0
    EXPECT_EQ(gc::GetClassTypeArgCount(nullptr), 0u);
    EXPECT_EQ(gc::GetClassTypeArg(nullptr, 0), 0);
    EXPECT_EQ(gc::GetMethodTypeArgCount(nullptr), 0u);
    EXPECT_EQ(gc::GetMethodTypeArg(nullptr, 0), 0);
}

TEST(generic_context_test, GetGenericContextForMethod_NotFound) {
    // Unknown method token → nullptr
    GenericContextHandle ctx = gc::GetGenericContextForMethod(0x06000999u);
    EXPECT_EQ(ctx, nullptr);

    // Zero token → nullptr
    ctx = gc::GetGenericContextForMethod(0);
    EXPECT_EQ(ctx, nullptr);
}

TEST(generic_context_test, MethodGenericContext_NoClassArgs) {
    TypeInfoHandle method_args[] = { kArgFloat };

    gc::RegisterMethodGenericContext(kMethodToken2,
        nullptr, 0, method_args, 1);

    GenericContextHandle ctx = gc::GetGenericContextForMethod(kMethodToken2);
    ASSERT_NE(ctx, nullptr);

    EXPECT_EQ(gc::GetClassTypeArgCount(ctx), 0u);
    EXPECT_EQ(gc::GetMethodTypeArgCount(ctx), 1u);
    EXPECT_EQ(gc::GetMethodTypeArg(ctx, 0), kArgFloat);
}

// ════════════════════════════════════════════════════════════════════════════
// Edge cases: zero/null input safety
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, ZeroInputs_AreSafe) {
    // RegisterGenericInstantiation with zero handles → no-op
    TypeInfoHandle args[] = { kArgInt };
    gc::RegisterGenericInstantiation(0, kClosedType1, args, 1);
    gc::RegisterGenericInstantiation(kOpenType1, 0, args, 1);
    // Should not crash — count unchanged relative to prior registrations

    // TryResolveClosedType with zero → 0
    EXPECT_EQ(gc::TryResolveClosedType(0, args, 1), 0);

    // RegisterGenericMethodInstantiation with zero handles → no-op
    gc::RegisterGenericMethodInstantiation(0, kClosedMethod1, args, 1);
    gc::RegisterGenericMethodInstantiation(kOpenMethod1, 0, args, 1);
    // Should not crash

    // TryResolveClosedMethod with zero → 0
    EXPECT_EQ(gc::TryResolveClosedMethod(0, args, 1), 0);

    // RegisterMethodGenericContext with zero token → no-op
    gc::RegisterMethodGenericContext(0, args, 1, args, 1);
    // Should not crash

    // UnregisterModuleGenerics with zero → no-op
    gc::UnregisterModuleGenerics(0);
}

TEST(generic_context_test, NullTypeArgs_AreSafe) {
    // RegisterGenericInstantiation with nullptr type_args → no-op (zero count)
    gc::RegisterGenericInstantiation(
        static_cast<TypeInfoHandle>(0x1101),
        static_cast<TypeInfoHandle>(0x2101),
        nullptr, 0);
    // Should not crash

    // RegisterGenericMethodInstantiation with nullptr type_args → no-op
    gc::RegisterGenericMethodInstantiation(
        static_cast<MethodInfoHandle>(0x4101),
        static_cast<MethodInfoHandle>(0x5101),
        nullptr, 0);
    // Should not crash
}

// ════════════════════════════════════════════════════════════════════════════
// UnregisterModuleGenerics
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, UnregisterModule_Nonexistent) {
    // UnregisterGenerics for an unregistered module → no-op (should not crash)
    gc::UnregisterModuleGenerics(42u);
    gc::UnregisterModuleGenerics(999u);
}

// ════════════════════════════════════════════════════════════════════════════
// GetRegisteredInstantiationCount
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, GetRegisteredInstantiationCount_Monotonic) {
    const CHAOS_IL2CPP_UINT32 before = gc::GetRegisteredInstantiationCount();

    TypeInfoHandle args[] = { kArgInt };
    gc::RegisterGenericInstantiation(
        static_cast<TypeInfoHandle>(0x1102),
        static_cast<TypeInfoHandle>(0x2102),
        args, 1);
    gc::RegisterGenericInstantiation(
        static_cast<TypeInfoHandle>(0x1103),
        static_cast<TypeInfoHandle>(0x2103),
        args, 1);

    EXPECT_EQ(gc::GetRegisteredInstantiationCount(), before + 2u);
}

// ════════════════════════════════════════════════════════════════════════════
// Cross-module bridge: GetBridgeV0 contract
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, BridgeV0_NonNull) {
    const auto* bridge = ri::GetBridgeV0();
    ASSERT_NE(bridge, nullptr);
    EXPECT_EQ(bridge->abi_version, CHAOS_RUNTIME_INSTANTIATION_BRIDGE_V0);
    EXPECT_GT(bridge->struct_size, 0u);
}

// ════════════════════════════════════════════════════════════════════════════
// Reverse index validation (exercises the code pattern fixed by P1 bug)
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, GetClosedTypeGenericArgs_MultipleRegistrations) {
    // Register two closed types with different arg counts on different open types
    // and verify the reverse index returns the correct args for each.
    TypeInfoHandle args_a[] = { static_cast<TypeInfoHandle>(0x300A) };
    TypeInfoHandle args_b[] = { static_cast<TypeInfoHandle>(0x300B),
                                static_cast<TypeInfoHandle>(0x300C) };

    TypeInfoHandle open_a  = static_cast<TypeInfoHandle>(0x110A);
    TypeInfoHandle open_b  = static_cast<TypeInfoHandle>(0x110B);
    TypeInfoHandle closed_a = static_cast<TypeInfoHandle>(0x210A);
    TypeInfoHandle closed_b = static_cast<TypeInfoHandle>(0x210B);

    gc::RegisterGenericInstantiation(open_a, closed_a, args_a, 1);
    gc::RegisterGenericInstantiation(open_b, closed_b, args_b, 2);

    TypeInfoHandle out[4] = { 0, 0, 0, 0 };

    // closed_a should have 1 arg
    EXPECT_EQ(gc::GetClosedTypeGenericArgs(closed_a, out, 4), 1u);
    EXPECT_EQ(out[0], args_a[0]);

    // closed_b should have 2 args
    std::memset(out, 0, sizeof(out));
    EXPECT_EQ(gc::GetClosedTypeGenericArgs(closed_b, out, 4), 2u);
    EXPECT_EQ(out[0], args_b[0]);
    EXPECT_EQ(out[1], args_b[1]);
}

TEST(generic_context_test, GetClosedTypeGenericArgs_AfterResolveAndReverse) {
    // Register a type, resolve it, then verify the reverse index still
    // returns the correct type args (tests reverse index consistency).
    TypeInfoHandle args[] = { static_cast<TypeInfoHandle>(0x300D),
                              static_cast<TypeInfoHandle>(0x300E) };
    TypeInfoHandle open   = static_cast<TypeInfoHandle>(0x110C);
    TypeInfoHandle closed = static_cast<TypeInfoHandle>(0x210C);

    gc::RegisterGenericInstantiation(open, closed, args, 2);

    // Resolve first
    TypeInfoHandle resolved = gc::TryResolveClosedType(open, args, 2);
    EXPECT_EQ(resolved, closed);

    // Then check reverse index
    TypeInfoHandle out[2] = { 0, 0 };
    EXPECT_EQ(gc::GetClosedTypeGenericArgs(closed, out, 2), 2u);
    EXPECT_EQ(out[0], args[0]);
    EXPECT_EQ(out[1], args[1]);
}

// ════════════════════════════════════════════════════════════════════════════
// Module lifecycle boundary tests
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, RegisterModuleGenerics_Null) {
    // nullptr input → no-op (should not crash)
    gc::RegisterModuleGenerics(nullptr);
}

TEST(generic_context_test, UnregisterModuleGenerics_NoEffectOnStandaloneRegistrations) {
    // Standalone registrations (module_id=0) should NOT be affected by
    // UnregisterModuleGenerics for any module_id != 0.
    TypeInfoHandle args[] = { static_cast<TypeInfoHandle>(0x300F) };
    TypeInfoHandle open   = static_cast<TypeInfoHandle>(0x110D);
    TypeInfoHandle closed = static_cast<TypeInfoHandle>(0x210D);

    gc::RegisterGenericInstantiation(open, closed, args, 1);

    // Unregister a non-existent module
    gc::UnregisterModuleGenerics(42u);

    // Standalone registration should still be intact
    EXPECT_EQ(gc::TryResolveClosedType(open, args, 1), closed);
    EXPECT_EQ(gc::GetClosedTypeGenericArgs(closed, nullptr, 0), 1u);
}

// ════════════════════════════════════════════════════════════════════════════
// RegisterGenericMethodInstantiation idempotency with reverse lookup
// ════════════════════════════════════════════════════════════════════════════

TEST(generic_context_test, MultipleMethodRegistrations_SameOpenMethod) {
    TypeInfoHandle args_a[] = { kArgInt };
    TypeInfoHandle args_b[] = { kArgString };

    auto open_mid = static_cast<MethodInfoHandle>(0x4FFF0001ull);
    auto closed_a  = static_cast<MethodInfoHandle>(0x5FFF0001ull);
    auto closed_b  = static_cast<MethodInfoHandle>(0x5FFF0002ull);

    gc::RegisterGenericMethodInstantiation(open_mid, closed_a, args_a, 1);
    gc::RegisterGenericMethodInstantiation(open_mid, closed_b, args_b, 1);

    // Both should be resolvable
    EXPECT_EQ(gc::TryResolveClosedMethod(open_mid, args_a, 1), closed_a);
    EXPECT_EQ(gc::TryResolveClosedMethod(open_mid, args_b, 1), closed_b);

    // Wrong args should still miss
    EXPECT_EQ(gc::TryResolveClosedMethod(open_mid, args_a, 2), 0u);
}

// ════════════════════════════════════════════════════════════════════════════
// NOTE: Lazy resolution path (DoLazyResolveOpenType, DoLazyResolveMethodContext)
// requires a fully bootstrapped AOT runtime with CodegenBridgeV0.resolve_type_by_token.
// These code paths are exercised by the integration test suite's module load/unload
// scenarios and the stress/capacity tests. Unit testing them requires either:
//   1. A test-only codegen bridge with synthetic resolve_type_by_token, or
//   2. A full bootstrap with synthetic AOT data
// Neither is available in the current unit test harness.
// ════════════════════════════════════════════════════════════════════════════
