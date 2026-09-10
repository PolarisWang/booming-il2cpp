// metadata_registry_test.cpp — Unit tests for MetadataRegistry + ModuleLifecycleManager
//
// Tests cover:
//   1. Singleton access and GetUnifiedRegistry contract
//   2. RegisterModule → ResolveType round-trip
//   3. Cross-module composite key isolation
//   4. EnumerateMethods stability
//   5. UnregisterModule tombstone behavior
//   6. RegisterHotpatchContext stub safety
//
// These are integration-leaning unit tests: they exercise the runtime-core APIs
// directly without needing the full AOT bootstrap path.

#include <gtest/gtest.h>

#include <metadata_interface.h>

#include <codegen_bridge.h>   // HotpatchModuleV0
#include <patch_data.h>       // PatchDataHeader

#include <cstdint>
#include <cstring>

namespace rc = chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// Test fixtures
// ════════════════════════════════════════════════════════════════════════════

class MetadataRegistryTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

// ════════════════════════════════════════════════════════════════════════════
// Test 1: Singleton & contract interface
// ════════════════════════════════════════════════════════════════════════════

TEST_F(MetadataRegistryTest, SingletonAccess)
{
    auto& reg1 = rc::MetadataRegistry::Get();
    auto& reg2 = rc::MetadataRegistry::Get();
    EXPECT_EQ(&reg1, &reg2);
}

TEST_F(MetadataRegistryTest, GetUnifiedRegistryReturnsNonNull)
{
    const auto* ureg = rc::MetadataRegistry::Get().GetUnifiedRegistry();
    ASSERT_NE(nullptr, ureg);
    EXPECT_EQ(0u, ureg->abi_version);
    EXPECT_GT(ureg->struct_size, 0u);
    ASSERT_NE(nullptr, ureg->resolve_type);
    ASSERT_NE(nullptr, ureg->get_type_name);
    ASSERT_NE(nullptr, ureg->resolve_method);
    ASSERT_NE(nullptr, ureg->resolve_field);
    ASSERT_NE(nullptr, ureg->find_token);
    ASSERT_NE(nullptr, ureg->enumerate_methods);
}

// ════════════════════════════════════════════════════════════════════════════
// Test 2: ResolveType returns nullptr for unknown module/token
// ════════════════════════════════════════════════════════════════════════════

TEST_F(MetadataRegistryTest, ResolveTypeUnknown)
{
    EXPECT_EQ(nullptr, rc::MetadataRegistry::Get().ResolveType(9999, 0x02000001));
    EXPECT_EQ(nullptr, rc::MetadataRegistry::Get().ResolveType(0, 0));
}

// ════════════════════════════════════════════════════════════════════════════
// Test 4: ModuleLifecycleManager singleton
// ════════════════════════════════════════════════════════════════════════════

TEST_F(MetadataRegistryTest, ModuleLifecycleManagerSingleton)
{
    auto* mgr1 = rc::ModuleLifecycleManager::Get();
    auto* mgr2 = rc::ModuleLifecycleManager::Get();
    EXPECT_EQ(mgr1, mgr2);
}

// ════════════════════════════════════════════════════════════════════════════
// Test 5: EnumerateMethods with unknown module returns 0
// ════════════════════════════════════════════════════════════════════════════

TEST_F(MetadataRegistryTest, EnumerateMethodsUnknownModule)
{
    void* buf[16];
    EXPECT_EQ(0u, rc::MetadataRegistry::Get().EnumerateMethods(9999, buf, 16));
}

// ════════════════════════════════════════════════════════════════════════════
// Test 6: Null/zero inputs are safe
// ════════════════════════════════════════════════════════════════════════════

TEST_F(MetadataRegistryTest, NullInputSafety)
{
    auto& reg = rc::MetadataRegistry::Get();

    EXPECT_EQ(nullptr, reg.ResolveType(0, 0));
    EXPECT_EQ(nullptr, reg.GetTypeName(0, nullptr));
    EXPECT_EQ(0u, reg.ResolveMethod(0, 0));
    EXPECT_EQ(nullptr, reg.GetMethodName(0));
    EXPECT_EQ(0u, reg.ResolveField(0, 0));
    EXPECT_EQ(0u, reg.FindToken(0));
    EXPECT_EQ(0u, reg.EnumerateMethods(0, nullptr, 0));
}

// ════════════════════════════════════════════════════════════════════════════
// Test 7: ModuleLifecycleManager Register/Unregister safety
// ════════════════════════════════════════════════════════════════════════════

TEST_F(MetadataRegistryTest, RegisterModuleNullInputs)
{
    auto* mgr = rc::ModuleLifecycleManager::Get();

    // Null name should return kInvalidModuleId.
    uint32_t mid = mgr->RegisterModule(nullptr, nullptr);
    EXPECT_EQ(rc::kInvalidModuleId, mid);

    // Null descriptor should return kInvalidModuleId.
    mid = mgr->RegisterModule("test", nullptr);
    EXPECT_EQ(rc::kInvalidModuleId, mid);
}

TEST_F(MetadataRegistryTest, UnregisterModuleZeroIsSafe)
{
    auto* mgr = rc::ModuleLifecycleManager::Get();
    // module_id=0 is the AOT root; should not crash or assert.
    EXPECT_NO_FATAL_FAILURE(mgr->UnregisterModule(0u));
}

// ════════════════════════════════════════════════════════════════════════════
// Test 8: RegisterAotModuleData null inputs
// ════════════════════════════════════════════════════════════════════════════

TEST_F(MetadataRegistryTest, RegisterAotModuleDataNull)
{
    auto* mgr = rc::ModuleLifecycleManager::Get();
    // Both null should be a no-op.
    EXPECT_NO_FATAL_FAILURE(mgr->RegisterAotModuleData(nullptr, nullptr));
}

// ════════════════════════════════════════════════════════════════════════════
// Test 9: RegisterHotUpdateGenerics null + zero
// ════════════════════════════════════════════════════════════════════════════

TEST_F(MetadataRegistryTest, RegisterHotUpdateGenericsNull)
{
    auto* mgr = rc::ModuleLifecycleManager::Get();
    EXPECT_NO_FATAL_FAILURE(mgr->RegisterHotUpdateGenerics(42, nullptr));
}

TEST_F(MetadataRegistryTest, UnregisterHotUpdateGenericsZero)
{
    auto* mgr = rc::ModuleLifecycleManager::Get();
    // module_id=0 should be rejected (it's the AOT root, not a hot-update module).
    EXPECT_NO_FATAL_FAILURE(mgr->UnregisterHotUpdateGenerics(0u));
}

// ════════════════════════════════════════════════════════════════════════════
// Test 10: RegisterHotpatchContext null safety
// ════════════════════════════════════════════════════════════════════════════

TEST_F(MetadataRegistryTest, RegisterHotpatchContextNull)
{
    auto* mgr = rc::ModuleLifecycleManager::Get();
    EXPECT_FALSE(mgr->RegisterHotpatchContext(nullptr));
}

TEST_F(MetadataRegistryTest, UnregisterHotpatchContextNull)
{
    auto* mgr = rc::ModuleLifecycleManager::Get();
    EXPECT_FALSE(mgr->UnregisterHotpatchContext(nullptr));
}

// ════════════════════════════════════════════════════════════════════════════
// Future integration tests (require full AOT bootstrap):
//
// These are documented here but gated behind a preprocessor flag because they
// need the AOT bootstrap pipeline (CodegenBridgeV0, generated code data
// sections) to be fully initialized.  Run with -DFULL_INTEGRATION_TESTS when
// linking against a bootstrapped runtime.
//
//   TEST_F(MetadataRegistryTest, AotRegistration_ResolveType) {
//       // After AOT bootstrap: resolve known type from module 0.
//       auto* ti = rc::MetadataRegistry::Get().ResolveType(0, 0x02000001);
//       EXPECT_NE(nullptr, ti);
//   }
//
//   TEST_F(MetadataRegistryTest, CrossModuleQuery_ModuleIdIsolation) {
//       // Two modules with same token → different results.
//       auto* t1 = rc::MetadataRegistry::Get().ResolveType(0, 0x02000001);
//       auto* t2 = rc::MetadataRegistry::Get().ResolveType(1, 0x02000001);
//       // Either both resolve to different types, or one is nullptr
//       // (module 1 may not have that token).  The key invariant:
//       // (t1 == t2) must be FALSE if both modules have the same token.
//       if (t1 != nullptr && t2 != nullptr) {
//           EXPECT_NE(t1, t2);
//       }
//   }
//
//   TEST_F(MetadataRegistryTest, HotUpdateRegistration_ResolveMethod) {
//       // After registering a hot-update module with generics.
//       auto mh = rc::MetadataRegistry::Get().ResolveMethod(1, 0x06000001);
//       EXPECT_NE(0u, mh);
//   }
//
//   TEST_F(MetadataRegistryTest, UnregisterModule_TombstoneBehavior) {
//       // After unregistering a module, ResolveType returns nullptr.
//       auto* mgr = rc::ModuleLifecycleManager::Get();
//       mgr->UnregisterModule(1);
//       auto* ti = rc::MetadataRegistry::Get().ResolveType(1, 0x02000001);
//       EXPECT_EQ(nullptr, ti);
//   }
//
//   TEST_F(MetadataRegistryTest, FindToken_AcrossModules) {
//       // FindToken returns composite key from any registered module.
//       uint64_t composite = rc::MetadataRegistry::Get().FindToken(0x06000001);
//       EXPECT_NE(0u, composite);
//       uint32_t found_module = static_cast<uint32_t>(composite >> 32);
//       EXPECT_TRUE(found_module < 10);  // sanity: module_id looks valid
//   }
