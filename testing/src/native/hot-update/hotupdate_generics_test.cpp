// hotupdate_generics_test.cpp — Generics registration and dispatch test
//
// Tests the generic context registration APIs. Registers synthetic generic
// instantiations and method contexts, then verifies lookup and resolution.
// Uses the lower-level APIs (RegisterGenericInstantiation,
// RegisterMethodGenericContext, TryResolveClosedType, GetGenericContextForMethod)
// which exercise the same internal paths as RegisterModuleGenerics without
// requiring a full codegen bridge.

#include <generic_context.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

namespace gc = chaos::il2cpp::generic_context;

// ── Synthetic TypeInfoHandle values ────────────────────────────────────────
// These are opaque handles (pointers) with no actual TypeInfo behind them.
// The generic context system stores and returns them opaquely without
// dereferencing — only equality comparison is used.

static TypeInfoHandle MakeHandle(uint32_t id) {
    return static_cast<TypeInfoHandle>(0x10000000ULL + id);
}

static const TypeInfoHandle kOpenList    = MakeHandle(1);   // List<>
static const TypeInfoHandle kClosedListInt   = MakeHandle(2);   // List<int>
static const TypeInfoHandle kTypeArgInt      = MakeHandle(3);   // int
static const TypeInfoHandle kClosedListString = MakeHandle(4);   // List<string>
static const TypeInfoHandle kTypeArgString    = MakeHandle(5);   // string
static const TypeInfoHandle kOpenDict     = MakeHandle(6);   // Dictionary<,>
static const TypeInfoHandle kClosedDictStrInt = MakeHandle(7);   // Dictionary<string,int>
static const TypeInfoHandle kTypeArgInt2      = MakeHandle(8);   // int (second instance)

// AOT symbol stubs (required by linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// ── Fixture ─────────────────────────────────────────────────────────────────

class GenericsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // No global state to reset — generic context is a fresh module-shard map.
    }

    void TearDown() override {
        // Tests use no module_id (0 = AOT root), so no unregister needed.
    }
};

// ── Tests ───────────────────────────────────────────────────────────────────


TEST_F(GenericsTest, RegisterAndResolveSingleGeneric)
{
    // Note: other tests may have pre-populated List<string>, so we only
    // verify that List<int> resolves correctly (the "not found" assertion
    // is unreliable with shared state).

    // Register List<int> = closed_type(open=List<>, args=[int]).
    gc::RegisterGenericInstantiation(kOpenList, kClosedListInt, &kTypeArgInt, 1);

    // Resolve: TryResolveClosedType(open=List<>, args=[int]) → kClosedListInt.
    TypeInfoHandle resolved = gc::TryResolveClosedType(kOpenList, &kTypeArgInt, 1);
    EXPECT_EQ(resolved, kClosedListInt);
}

TEST_F(GenericsTest, RegisterAndResolveMultipleGenerics)
{
    // Register List<int> and List<string>.
    gc::RegisterGenericInstantiation(kOpenList, kClosedListInt, &kTypeArgInt, 1);
    gc::RegisterGenericInstantiation(kOpenList, kClosedListString, &kTypeArgString, 1);

    // Both should be resolvable.
    EXPECT_EQ(gc::TryResolveClosedType(kOpenList, &kTypeArgInt, 1),    kClosedListInt);
    EXPECT_EQ(gc::TryResolveClosedType(kOpenList, &kTypeArgString, 1), kClosedListString);
}

TEST_F(GenericsTest, MultipleTypeArgs)
{
    // Register Dictionary<string,int> = Dictionary<,>[string, int].
    TypeInfoHandle dict_args[] = { kTypeArgString, kTypeArgInt };
    gc::RegisterGenericInstantiation(kOpenDict, kClosedDictStrInt, dict_args, 2);

    TypeInfoHandle resolved = gc::TryResolveClosedType(kOpenDict, dict_args, 2);
    EXPECT_EQ(resolved, kClosedDictStrInt);

    // Wrong arg count → nullptr.
    EXPECT_EQ(gc::TryResolveClosedType(kOpenDict, &kTypeArgInt, 1), static_cast<TypeInfoHandle>(0));
}

TEST_F(GenericsTest, DuplicateRegistrationIsIdempotent)
{
    // Register same instantiation twice should not crash.
    gc::RegisterGenericInstantiation(kOpenList, kClosedListInt, &kTypeArgInt, 1);
    gc::RegisterGenericInstantiation(kOpenList, kClosedListInt, &kTypeArgInt, 1);

    EXPECT_EQ(gc::TryResolveClosedType(kOpenList, &kTypeArgInt, 1), kClosedListInt);
}

TEST_F(GenericsTest, RegisterAndGetMethodGenericContext)
{
    static constexpr uint32_t kMethodToken = 0x06000042u;

    // Register a method generic context with class args [int] and method args [string].
    TypeInfoHandle class_args[] = { kTypeArgInt };
    TypeInfoHandle method_args[] = { kTypeArgString };

    gc::RegisterMethodGenericContext(
        kMethodToken,
        class_args, 1,
        method_args, 1);

    // Retrieve.
    GenericContextHandle ctx = gc::GetGenericContextForMethod(kMethodToken);
    ASSERT_NE(ctx, nullptr);

    // Verify class type args.
    EXPECT_EQ(gc::GetClassTypeArgCount(ctx), 1u);
    EXPECT_EQ(gc::GetClassTypeArg(ctx, 0), kTypeArgInt);

    // Verify method type args.
    EXPECT_EQ(gc::GetMethodTypeArgCount(ctx), 1u);
    EXPECT_EQ(gc::GetMethodTypeArg(ctx, 0), kTypeArgString);
}

TEST_F(GenericsTest, MethodContextWithoutClassArgs)
{
    static constexpr uint32_t kMethodToken = 0x06000050u;

    // Static generic method: class_args = empty.
    TypeInfoHandle method_args[] = { kTypeArgInt, kTypeArgString };

    gc::RegisterMethodGenericContext(
        kMethodToken,
        nullptr, 0,
        method_args, 2);

    GenericContextHandle ctx = gc::GetGenericContextForMethod(kMethodToken);
    ASSERT_NE(ctx, nullptr);

    EXPECT_EQ(gc::GetClassTypeArgCount(ctx), 0u);
    EXPECT_EQ(gc::GetMethodTypeArgCount(ctx), 2u);
    EXPECT_EQ(gc::GetMethodTypeArg(ctx, 0), kTypeArgInt);
    EXPECT_EQ(gc::GetMethodTypeArg(ctx, 1), kTypeArgString);
}

TEST_F(GenericsTest, GetGenericContextForMethodReturnsNullForUnknownToken)
{
    GenericContextHandle ctx = gc::GetGenericContextForMethod(0xFFFFFFFFu);
    EXPECT_EQ(ctx, nullptr);
}


TEST_F(GenericsTest, MultipleOpenTypes)
{
    static const TypeInfoHandle kOpenComparable = MakeHandle(10);
    static const TypeInfoHandle kClosedComparableInt = MakeHandle(11);

    // Register under two different open types.
    gc::RegisterGenericInstantiation(kOpenList, kClosedListInt, &kTypeArgInt, 1);
    gc::RegisterGenericInstantiation(kOpenComparable, kClosedComparableInt, &kTypeArgInt, 1);

    // Each resolves independently.
    EXPECT_EQ(gc::TryResolveClosedType(kOpenList, &kTypeArgInt, 1), kClosedListInt);
    EXPECT_EQ(gc::TryResolveClosedType(kOpenComparable, &kTypeArgInt, 1), kClosedComparableInt);

    // Wrong open type → miss.
    EXPECT_EQ(gc::TryResolveClosedType(kOpenComparable, &kTypeArgString, 1), static_cast<TypeInfoHandle>(0));
}

TEST_F(GenericsTest, RegisteredInstantiationCount)
{
    // Note: runs last, so use fresh handles to avoid counting prior registrations.
    static const TypeInfoHandle kTestOpen   = MakeHandle(20);
    static const TypeInfoHandle kTestClosed = MakeHandle(21);
    static const TypeInfoHandle kTestArg    = MakeHandle(22);
    static const TypeInfoHandle kTestArg2   = MakeHandle(23);

    uint32_t before = gc::GetRegisteredInstantiationCount();

    gc::RegisterGenericInstantiation(kTestOpen, kTestClosed, &kTestArg, 1);
    EXPECT_EQ(gc::GetRegisteredInstantiationCount(), before + 1);

    gc::RegisterGenericInstantiation(kTestOpen, kTestArg, &kTestArg2, 1);
    EXPECT_EQ(gc::GetRegisteredInstantiationCount(), before + 2);

    // Duplicate should not increase count.
    gc::RegisterGenericInstantiation(kTestOpen, kTestClosed, &kTestArg, 1);
    EXPECT_EQ(gc::GetRegisteredInstantiationCount(), before + 2);
}
