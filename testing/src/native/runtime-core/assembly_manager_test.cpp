// assembly_manager_test.cpp — Isolated unit tests for AssemblyManager API
//
// Tests the chaos::il2cpp::runtime_core::AssemblyManager class:
//   - Singleton access (Get)
//   - Query methods: FindAssembly, FindByModuleId, GetStaticField
//   - Lifecycle guards: LoadAssembly null-safety, UnloadAssembly null-safety
//   - State queries: LoadedCount, NextAlcId
//
// NOTE: LoadAssembly/UnloadAssembly with real data require full runtime
// infrastructure (memory_domain, ApplyPatchFromMemory, GC, etc.).
// These tests focus on the guards and query-only paths.

#include <gtest/gtest.h>

#include <assembly_manager.h>
#include <domain_unloader.h>

#include <cstdint>
#include <csetjmp>

// ── Stub external symbols ──────────────────────────────────────────────────
// The test links against chaos_bootstrap and chaos_interpreter which reference
// symbols normally provided by AOT-generated code. These stubs satisfy the
// linker since the tests don't exercise codegen-dependent paths.
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;

extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

namespace chaos::il2cpp::runtime_core {
thread_local jmp_buf* g_chaos_exception_jmp = nullptr;
}

namespace gc = chaos::il2cpp::runtime_core;

// ════════════════════════════════════════════════════════════════════════════
// AssemblyManager singleton
// ════════════════════════════════════════════════════════════════════════════

TEST(assembly_manager_test, Get_ReturnsNonNull) {
    auto* mgr = gc::AssemblyManager::Get();
    ASSERT_NE(mgr, nullptr);
}

TEST(assembly_manager_test, Get_SameInstance) {
    auto* mgr1 = gc::AssemblyManager::Get();
    auto* mgr2 = gc::AssemblyManager::Get();
    EXPECT_EQ(mgr1, mgr2);
}

// ════════════════════════════════════════════════════════════════════════════
// LoadedCount
// ════════════════════════════════════════════════════════════════════════════

TEST(assembly_manager_test, LoadedCount_InitiallyZero) {
    auto* mgr = gc::AssemblyManager::Get();
    EXPECT_EQ(mgr->LoadedCount(), 0u);
}

// ════════════════════════════════════════════════════════════════════════════
// FindAssembly
// ════════════════════════════════════════════════════════════════════════════

TEST(assembly_manager_test, FindAssembly_NullName_ReturnsNull) {
    auto* mgr = gc::AssemblyManager::Get();
    EXPECT_EQ(mgr->FindAssembly(nullptr), nullptr);
}

TEST(assembly_manager_test, FindAssembly_EmptyName_ReturnsNull) {
    auto* mgr = gc::AssemblyManager::Get();
    EXPECT_EQ(mgr->FindAssembly(""), nullptr);
}

TEST(assembly_manager_test, FindAssembly_Nonexistent_ReturnsNull) {
    auto* mgr = gc::AssemblyManager::Get();
    EXPECT_EQ(mgr->FindAssembly("Nonexistent.Assembly"), nullptr);
}

// ════════════════════════════════════════════════════════════════════════════
// FindByModuleId
// ════════════════════════════════════════════════════════════════════════════

TEST(assembly_manager_test, FindByModuleId_Zero_ReturnsNull) {
    auto* mgr = gc::AssemblyManager::Get();
    EXPECT_EQ(mgr->FindByModuleId(0), nullptr);
}

TEST(assembly_manager_test, FindByModuleId_Nonexistent_ReturnsNull) {
    auto* mgr = gc::AssemblyManager::Get();
    EXPECT_EQ(mgr->FindByModuleId(9999), nullptr);
}

// ════════════════════════════════════════════════════════════════════════════
// GetStaticField
// ════════════════════════════════════════════════════════════════════════════

TEST(assembly_manager_test, GetStaticField_ModuleZero_ReturnsNull) {
    auto* mgr = gc::AssemblyManager::Get();
    EXPECT_EQ(mgr->GetStaticField(0, 0), nullptr);
}

TEST(assembly_manager_test, GetStaticField_NonexistentModule_ReturnsNull) {
    auto* mgr = gc::AssemblyManager::Get();
    EXPECT_EQ(mgr->GetStaticField(9999, 0), nullptr);
}

// ════════════════════════════════════════════════════════════════════════════
// UnloadAssembly
// ════════════════════════════════════════════════════════════════════════════

TEST(assembly_manager_test, UnloadAssembly_Null_ReturnsFalse) {
    auto* mgr = gc::AssemblyManager::Get();
    EXPECT_FALSE(mgr->UnloadAssembly(nullptr));
}

// ════════════════════════════════════════════════════════════════════════════
// LoadAssembly null-safety
// ════════════════════════════════════════════════════════════════════════════

TEST(assembly_manager_test, LoadAssembly_NullData_ReturnsNull) {
    auto* mgr = gc::AssemblyManager::Get();
    EXPECT_EQ(mgr->LoadAssembly(nullptr, 0, "Test"), nullptr);
}

TEST(assembly_manager_test, LoadAssembly_EmptyData_ReturnsNull) {
    auto* mgr = gc::AssemblyManager::Get();
    const char dummy[4] = {0};
    EXPECT_EQ(mgr->LoadAssembly(dummy, 0, "Test"), nullptr);
}

TEST(assembly_manager_test, LoadAssembly_NullName_ReturnsNull) {
    auto* mgr = gc::AssemblyManager::Get();
    const char dummy[4] = {static_cast<char>(0xAB), static_cast<char>(0xCD),
                           static_cast<char>(0xEF), static_cast<char>(0x01)};
    EXPECT_EQ(mgr->LoadAssembly(dummy, sizeof(dummy), nullptr), nullptr);
}

// ════════════════════════════════════════════════════════════════════════════
// NextAlcId
// ════════════════════════════════════════════════════════════════════════════

TEST(assembly_manager_test, NextAlcId_StartsAtOne) {
    // Each test gets its own call, so IDs are monotonically increasing.
    // We just verify the API returns a non-zero ID.
    uint32_t id = gc::AssemblyManager::NextAlcId();
    EXPECT_NE(id, 0u);
}

// ════════════════════════════════════════════════════════════════════════════
// AssemblyLoadContext struct initialization
// ════════════════════════════════════════════════════════════════════════════

TEST(assembly_manager_test, AssemblyLoadContext_DefaultInit) {
    gc::AssemblyLoadContext ctx{};
    EXPECT_EQ(ctx.alc_id, 0u);
    EXPECT_EQ(ctx.module_id, 0u);
    EXPECT_EQ(ctx.is_loaded, false);
    EXPECT_EQ(ctx.patch_context, nullptr);
    EXPECT_EQ(ctx.static_field_ptr, nullptr);
    EXPECT_EQ(ctx.static_field_count, 0u);
    EXPECT_EQ(ctx.domain_id, 0u);
}

// ════════════════════════════════════════════════════════════════════════════
// DomainUnloader
// ════════════════════════════════════════════════════════════════════════════

TEST(assembly_manager_test, UnloadDomain_InvalidId_ReturnsFailure) {
    auto result = gc::UnloadDomain(0);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.cross_domain_refs_found, 0u);
    EXPECT_EQ(result.cross_domain_refs_cleared, 0u);
    EXPECT_EQ(result.lock_drain_count, 0u);
}
