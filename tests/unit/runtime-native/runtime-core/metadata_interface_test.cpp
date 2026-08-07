/// metadata_interface_test — MetadataRegistry + LookupCallTarget unit tests.
///
/// Tests the unified metadata resolution layer.  Since full AOT module setup
/// requires the bootstrap/loader infrastructure, these tests focus on
/// null-safety, zero-handle edge cases, and singleton access patterns.
///
/// Test cases:
///   1. MetadataRegistry singleton access
///   2. ResolveType with unregistered module → nullptr
///   3. GetTypeName with zero handle → nullptr
///   4. ResolveMethod with zero token → 0
///   5. ResolveField with unregistered module → 0
///   6. FindToken with unregistered token → 0
///   7. EnumerateMethods with unregistered module → 0
///   8. LookupCallTarget with null subject_id → 0
///   9. LookupCallTarget with null bridge → 0 (Path 1 cold)
///   10. ModuleLifecycleManager singleton access

#include <gtest/gtest.h>

#include <metadata_interface.h>
#include <type_registry.h>
#include <chaos/type_info.h>

using namespace chaos::il2cpp::runtime_core;

// ── Stub definitions for symbols normally emitted by AOT codegen ─────────
// These extern symbols are referenced by bootstrap and interpreter but are
// normally provided by the AOT codegen output.  This test doesn't run
// codegen, so we provide null/empty stubs.
namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" { void* chaos_il2cpp_aot_hotpatch_module = nullptr; }
extern "C" { const void* kChaosExternalRuntimeSubjects = nullptr; }
extern "C" { const void* kChaosExternalRuntimeFnTable = nullptr; }
extern "C" { int32_t kChaosExternalRuntimeCount = 0; }
}}}

// ════════════════════════════════════════════════════════════════════════════
// MetadataRegistry tests
// ════════════════════════════════════════════════════════════════════════════

TEST(MetadataInterface, RegistrySingleton) {
    auto& reg = MetadataRegistry::Get();
    auto& reg2 = MetadataRegistry::Get();
    EXPECT_EQ(&reg, &reg2);
}

TEST(MetadataInterface, ResolveTypeUnregistered) {
    auto& reg = MetadataRegistry::Get();
    // No AOT module is registered — should return nullptr.
    const auto* ti = reg.ResolveType(0xFFFF, 0x02000001);
    EXPECT_EQ(ti, nullptr);
}

TEST(MetadataInterface, ResolveTypeZeroModule) {
    auto& reg = MetadataRegistry::Get();
    // Module 0 may have AOT root data during bootstrap, but our test
    // doesn't set it up — expect nullptr.
    const auto* ti = reg.ResolveType(0, 0x02000001);
    EXPECT_EQ(ti, nullptr);
}

TEST(MetadataInterface, GetTypeNameNullHandle) {
    auto& reg = MetadataRegistry::Get();
    const char* out_ns = nullptr;
    const auto* name = reg.GetTypeName(0, &out_ns);
    EXPECT_EQ(name, nullptr);
}

TEST(MetadataInterface, GetTypeNameNullOutNs) {
    auto& reg = MetadataRegistry::Get();
    const auto* name = reg.GetTypeName(0, nullptr);
    EXPECT_EQ(name, nullptr);
}

TEST(MetadataInterface, ResolveMethodZero) {
    auto& reg = MetadataRegistry::Get();
    MethodInfoHandle h = reg.ResolveMethod(0, 0);
    EXPECT_EQ(h, 0u);
}

TEST(MetadataInterface, ResolveMethodUnregistered) {
    auto& reg = MetadataRegistry::Get();
    MethodInfoHandle h = reg.ResolveMethod(0xFFFF, 0x06000001);
    EXPECT_EQ(h, 0u);
}

TEST(MetadataInterface, ResolveFieldZero) {
    auto& reg = MetadataRegistry::Get();
    FieldInfoHandle h = reg.ResolveField(0, 0);
    EXPECT_EQ(h, 0u);
}

TEST(MetadataInterface, ResolveFieldUnregistered) {
    auto& reg = MetadataRegistry::Get();
    FieldInfoHandle h = reg.ResolveField(0xFFFF, 0x04000001);
    EXPECT_EQ(h, 0u);
}

TEST(MetadataInterface, FindTokenUnregistered) {
    auto& reg = MetadataRegistry::Get();
    // No modules set up — should return 0.
    uint64_t key = reg.FindToken(0x06000001);
    EXPECT_EQ(key, 0u);
}

TEST(MetadataInterface, FindTokenZero) {
    auto& reg = MetadataRegistry::Get();
    uint64_t key = reg.FindToken(0);
    EXPECT_EQ(key, 0u);
}

TEST(MetadataInterface, EnumerateMethodsUnregistered) {
    auto& reg = MetadataRegistry::Get();
    uint32_t n = reg.EnumerateMethods(0xFFFF, nullptr, 0);
    EXPECT_EQ(n, 0u);
}

// ── LookupCallTarget tests ──────────────────────────────────────────────

TEST(MetadataInterface, LookupCallTargetNullSubject) {
    MethodInfoHandle h = LookupCallTarget(nullptr);
    EXPECT_EQ(h, 0u);
}

TEST(MetadataInterface, LookupCallTargetEmptySubject) {
    MethodInfoHandle h = LookupCallTarget("");
    EXPECT_EQ(h, 0u);
}

TEST(MetadataInterface, LookupCallTargetNullBridge) {
    // Path 1 (CodegenBridge) is skipped when bridge is nullptr.
    // Path 2 (HotpatchNameRegistry) is empty since no patches are loaded.
    // Path 3 (ExternalRuntimeDispatchTable) returns 0 by default.
    MethodInfoHandle h = LookupCallTarget("Some.Type.Method", nullptr);
    EXPECT_EQ(h, 0u);
}

TEST(MetadataInterface, LookupCallTargetWithImageNoBridge) {
    // Same as above but with a dummy image handle.
    // Should still return 0 since bridge is nullptr.
    MethodInfoHandle h = LookupCallTarget("Some.Type.Method", nullptr, 0x1000);
    EXPECT_EQ(h, 0u);
}

// ── ModuleLifecycleManager tests ────────────────────────────────────────

TEST(MetadataInterface, LifecycleManagerSingleton) {
    auto* mgr = ModuleLifecycleManager::Get();
    ASSERT_NE(mgr, nullptr);

    auto* mgr2 = ModuleLifecycleManager::Get();
    EXPECT_EQ(mgr, mgr2);
}

TEST(MetadataInterface, LifecycleManagerUnregisterInvalid) {
    auto* mgr = ModuleLifecycleManager::Get();
    // Unregistering an invalid module must not crash.
    mgr->UnregisterModule(0xFFFFFFFF);
    mgr->UnregisterModule(0);
}

TEST(MetadataInterface, LifecycleManagerRegisterNull) {
    auto* mgr = ModuleLifecycleManager::Get();
    // RegisterModule with null descriptor should fail gracefully.
    uint32_t mid = mgr->RegisterModule("NullDescTest", nullptr);
    EXPECT_EQ(mid, kInvalidModuleId);
}

TEST(MetadataInterface, LifecycleManagerRegisterHotpatchNull) {
    auto* mgr = ModuleLifecycleManager::Get();
    bool ok = mgr->RegisterHotpatchContext(nullptr);
    EXPECT_FALSE(ok);
}

TEST(MetadataInterface, LifecycleManagerUnregisterHotpatchNull) {
    auto* mgr = ModuleLifecycleManager::Get();
    bool ok = mgr->UnregisterHotpatchContext(nullptr);
    EXPECT_FALSE(ok);
}

TEST(MetadataInterface, LifecycleManagerRegisterAotModuleDataNull) {
    auto* mgr = ModuleLifecycleManager::Get();
    // Must not crash with null pointers.
    mgr->RegisterAotModuleData(nullptr, nullptr);
}

TEST(MetadataInterface, LifecycleManagerRegisterGenericsNull) {
    auto* mgr = ModuleLifecycleManager::Get();
    // Must not crash with invalid module_id.
    mgr->RegisterHotUpdateGenerics(0xFFFFFFFF, nullptr);
    mgr->UnregisterHotUpdateGenerics(0xFFFFFFFF);
}

TEST(MetadataInterface, LifecycleManagerRegisterGenericsInvalid) {
    // RegisterGenerics on unregistered module must not crash.
    auto* mgr = ModuleLifecycleManager::Get();
    mgr->RegisterHotUpdateGenerics(0, nullptr);
    mgr->UnregisterHotUpdateGenerics(0);
}

// ── ModuleAwareResolveXxx tests ─────────────────────────────────────────

TEST(MetadataInterface, ModuleAwareResolveTypeInfoZero) {
    const auto* ti = ModuleAwareResolveTypeInfo(0);
    EXPECT_EQ(ti, nullptr);
}

TEST(MetadataInterface, ModuleAwareResolveMethodZero) {
    MethodInfoHandle h = ModuleAwareResolveMethod(0, 0);
    EXPECT_EQ(h, 0u);
}

TEST(MetadataInterface, ModuleAwareResolveFieldZero) {
    FieldInfoHandle h = ModuleAwareResolveField(0, 0);
    EXPECT_EQ(h, 0u);
}

TEST(MetadataInterface, ModuleAwareResolveTokenZero) {
    uint32_t result = ModuleAwareResolveToken(0);
    EXPECT_EQ(result, 0u);
}

TEST(MetadataInterface, ModuleAwareResolveTokenUnregistered) {
    // When no AOT bridge is available, ModuleAwareResolveToken falls back
    // to returning the raw token as an opaque handle (callers resolve lazily).
    uint32_t result = ModuleAwareResolveToken(0x06000001);
    EXPECT_EQ(result, 0x06000001u);
}
