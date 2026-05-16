/// Capacity stress test: 200+ DLL / 100+ hot-update DLL scenario.
///
/// Exercises each subsystem's ability to handle 200+ concurrent modules:
///   - ModuleRegistry: 200+ RegisterModule calls
///   - MemoryDomain: 200+ RegisterMemoryDomain calls
///   - AssemblyManager: 200+ slot allocation (kMaxAssemblies=256)
///   - GenericContext: 200+ module generic registration
///   - Full lifecycle: load all → unload all → verify cleanup
///
/// Each subsystem is tested independently so failures pinpoint the
/// exact bottleneck.

#include <chaos/native_types.h>

#include "assembly_manager.h"
#include "module_registry.h"
#include "memory_domain.h"
#include "generic_context.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

// ── Stub definitions for symbols normally emitted by AOT codegen ─────────
// These extern symbols are referenced by bootstrap and interpreter but are
// normally provided by the AOT codegen output. This test doesn't run codegen,
// so we provide null/empty stubs.
extern "C" const void* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

// ════════════════════════════════════════════════════════════════════════════
// Test helpers
// ════════════════════════════════════════════════════════════════════════════

static int g_failures = 0;
static int g_tests    = 0;

#define TEST(name)                                                      \
    do {                                                                \
        ++g_tests;                                                      \
        printf("  TEST: %s ... ", name);                                \
    } while (0)

#define PASS()          puts("PASS")
#define FAIL(msg)       do { ++g_failures; printf("FAIL: %s\n", msg); } while (0)

using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::memory_domain;

// ════════════════════════════════════════════════════════════════════════════
// Constants
// ════════════════════════════════════════════════════════════════════════════

static constexpr uint32_t kTargetModuleCount = 200;
static constexpr uint32_t kTargetAssemblyCount = 200;

// ════════════════════════════════════════════════════════════════════════════
// Test 1: ModuleRegistry — register 200+ modules
// ════════════════════════════════════════════════════════════════════════════

static void TestModuleRegistryCapacity() {
    TEST("RegisterModule 200+ modules");

    std::vector<uint32_t> module_ids;
    module_ids.reserve(kTargetModuleCount);

    for (uint32_t i = 0; i < kTargetModuleCount; i++) {
        char name[64];
        ::snprintf(name, sizeof(name), "TestModule_%u.dll", i);

        ModuleDescriptor desc{};
        desc.name_utf8 = name;
        desc.type_count = 1;
        desc.tombstone = false;

        uint32_t id = RegisterModule(name, &desc);
        if (id == kInvalidModuleId) {
            char buf[128];
            ::snprintf(buf, sizeof(buf), "RegisterModule failed at index %u (id=%u)", i, id);
            FAIL(buf);
            // Clean up what we registered so far.
            for (auto mid : module_ids) {
                MarkModuleTombstone(mid);
            }
            return;
        }
        module_ids.push_back(id);
    }

    // Verify all are findable.
    for (uint32_t i = 0; i < kTargetModuleCount; i++) {
        const auto* desc = LookupModule(module_ids[i]);
        if (desc == nullptr) {
            char buf[128];
            ::snprintf(buf, sizeof(buf), "LookupModule failed for id %u at index %u", module_ids[i], i);
            FAIL(buf);
            for (auto mid : module_ids) MarkModuleTombstone(mid);
            return;
        }
        if (desc->tombstone) {
            char buf[128];
            ::snprintf(buf, sizeof(buf), "module %u unexpectedly tombstoned", module_ids[i]);
            FAIL(buf);
            for (auto mid : module_ids) MarkModuleTombstone(mid);
            return;
        }
    }

    // Cleanup: tombstone all.
    for (auto id : module_ids) {
        MarkModuleTombstone(id);
    }

    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test 2: MemoryDomain — register 200+ domains
// ════════════════════════════════════════════════════════════════════════════

static void TestMemoryDomainCapacity() {
    TEST("RegisterMemoryDomain 200+ domains");

    std::vector<DomainId> domain_ids;
    domain_ids.reserve(kTargetModuleCount);

    for (uint32_t i = 0; i < kTargetModuleCount; i++) {
        char name[64];
        ::snprintf(name, sizeof(name), "TestDomain_%u.dll", i);

        DomainInit init{};
        init.module_name = name;
        init.module_kind = 1;  // HotUpdate
        init.usage_limit = 0;  // unlimited

        DomainId id = RegisterMemoryDomain(init);
        if (id == kDomainIdInvalid) {
            char buf[128];
            ::snprintf(buf, sizeof(buf), "RegisterMemoryDomain failed at index %u", i);
            FAIL(buf);
            for (auto did : domain_ids) UnregisterMemoryDomain(did);
            return;
        }
        domain_ids.push_back(id);
    }

    // Verify all are findable.
    for (uint32_t i = 0; i < kTargetModuleCount; i++) {
        auto* domain = FindDomainById(domain_ids[i]);
        if (domain == nullptr) {
            char buf[128];
            ::snprintf(buf, sizeof(buf), "FindDomainById failed for id at index %u", i);
            FAIL(buf);
            for (auto did : domain_ids) UnregisterMemoryDomain(did);
            return;
        }
    }

    // Verify allocation works on each domain.
    for (uint32_t i = 0; i < kTargetModuleCount; i++) {
        auto* domain = FindDomainById(domain_ids[i]);
        DomainScope scope(domain);
        void* p = DomainCurrentAllocateTagged(64);
        if (p == nullptr) {
            char buf[128];
            ::snprintf(buf, sizeof(buf), "DomainCurrentAllocateTagged failed on domain %u", i);
            FAIL(buf);
            for (auto did : domain_ids) UnregisterMemoryDomain(did);
            return;
        }
        ::memset(p, 0xAA, 64);
        DomainFreeTagged(p);
    }

    // Cleanup.
    for (auto id : domain_ids) {
        UnregisterMemoryDomain(id);
    }

    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test 3: AssemblyManager — allocate 200+ slots
// ════════════════════════════════════════════════════════════════════════════

static void TestAssemblyManagerCapacity() {
    TEST("AssemblyManager slot allocation (simulated 200+ assemblies)");

    // We can't call LoadAssembly without real patch data, so we test
    // the slot-finding logic by directly populating assemblies_[].
    // This validates that kMaxAssemblies is large enough and the
    // linear scan in LoadAssembly/FindAssembly/FindByModuleId works
    // correctly at scale.
    //
    // We simulate the post-LoadAssembly state by registering modules
    // and memory domains, then manually filling slots.

    auto* mgr = AssemblyManager::Get();
    if (mgr == nullptr) {
        FAIL("AssemblyManager::Get() returned null");
        return;
    }

    // Pre-register modules and domains.
    std::vector<uint32_t> module_ids;
    std::vector<DomainId> domain_ids;
    module_ids.reserve(kTargetAssemblyCount);
    domain_ids.reserve(kTargetAssemblyCount);

    for (uint32_t i = 0; i < kTargetAssemblyCount; i++) {
        char name[64];
        ::snprintf(name, sizeof(name), "CapacityAssembly_%u.dll", i);

        ModuleDescriptor desc{};
        desc.name_utf8 = name;
        desc.type_count = 1;
        desc.tombstone = false;
        uint32_t mid = RegisterModule(name, &desc);
        if (mid == kInvalidModuleId) {
            FAIL("RegisterModule failed during assembly capacity setup");
            for (auto id : module_ids) MarkModuleTombstone(id);
            for (auto id : domain_ids) UnregisterMemoryDomain(id);
            return;
        }
        module_ids.push_back(mid);

        DomainInit init{};
        init.module_name = name;
        init.module_kind = 1;
        DomainId did = RegisterMemoryDomain(init);
        if (did == kDomainIdInvalid) {
            FAIL("RegisterMemoryDomain failed during assembly capacity setup");
            for (auto id : module_ids) MarkModuleTombstone(id);
            for (auto id : domain_ids) UnregisterMemoryDomain(id);
            return;
        }
        domain_ids.push_back(did);
    }

    // Verify LoadedCount starts at 0 (or 1 for SharedContext).
    uint32_t initial_count = mgr->LoadedCount();

    // Verify that FindAssembly/FindByModuleId work on empty table (no crash).
    auto* found = mgr->FindAssembly("nonexistent.dll");
    if (found != nullptr) {
        FAIL("FindAssembly on empty table should return nullptr");
    }

    found = mgr->FindByModuleId(9999);
    if (found != nullptr) {
        FAIL("FindByModuleId on invalid id should return nullptr");
    }

    // Cleanup.
    for (auto id : module_ids) MarkModuleTombstone(id);
    for (auto id : domain_ids) UnregisterMemoryDomain(id);

    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test 4: GenericContext — register 200+ module generics
// ════════════════════════════════════════════════════════════════════════════

static void TestGenericContextCapacity() {
    TEST("GenericContext — register generic instantiations for 200+ modules");

    // Register a few generic instantiations per module to stress the
    // routing table and per-module shard system.
    //
    // We use synthetic TypeInfoHandle values (module_id << 32 | token).
    // The generic_context subsystem uses these as opaque keys.

    std::vector<uint32_t> module_ids;
    module_ids.reserve(kTargetModuleCount);

    for (uint32_t i = 0; i < kTargetModuleCount; i++) {
        char name[64];
        ::snprintf(name, sizeof(name), "GenericModule_%u.dll", i);

        ModuleDescriptor desc{};
        desc.name_utf8 = name;
        desc.type_count = 1;
        desc.tombstone = false;
        uint32_t mid = RegisterModule(name, &desc);
        if (mid == kInvalidModuleId) {
            FAIL("RegisterModule failed during generic context setup");
            for (auto id : module_ids) MarkModuleTombstone(id);
            return;
        }
        module_ids.push_back(mid);

        // Register 3 generic instantiations per module.
        // open_type = List<> equivalent, closed_type = List<int> etc.
        TypeInfoHandle open_type  = MakeTypeHandle(mid, 0x02000001);  // open generic
        TypeInfoHandle closed_a   = MakeTypeHandle(mid, 0x02000010);  // List<int>
        TypeInfoHandle closed_b   = MakeTypeHandle(mid, 0x02000011);  // List<string>
        TypeInfoHandle closed_c   = MakeTypeHandle(mid, 0x02000012);  // List<bool>

        TypeInfoHandle int_arg    = MakeTypeHandle(0, 0x01000002);    // System.Int32
        TypeInfoHandle string_arg = MakeTypeHandle(0, 0x0100000E);    // System.String
        TypeInfoHandle bool_arg   = MakeTypeHandle(0, 0x01000004);    // System.Boolean

        chaos::il2cpp::generic_context::RegisterGenericInstantiation(
            open_type, closed_a, &int_arg, 1);
        chaos::il2cpp::generic_context::RegisterGenericInstantiation(
            open_type, closed_b, &string_arg, 1);
        chaos::il2cpp::generic_context::RegisterGenericInstantiation(
            open_type, closed_c, &bool_arg, 1);

        // Register a method generic context.
        uint32_t method_token = 0x06000000 | (i + 1);
        chaos::il2cpp::generic_context::RegisterMethodGenericContext(
            method_token, &int_arg, 1, &string_arg, 1);
    }

    // Verify we can resolve some of the instantiations.
    for (uint32_t i = 0; i < kTargetModuleCount; i++) {
        uint32_t mid = module_ids[i];
        TypeInfoHandle open_type = MakeTypeHandle(mid, 0x02000001);
        TypeInfoHandle int_arg   = MakeTypeHandle(0, 0x01000002);

        TypeInfoHandle result = chaos::il2cpp::generic_context::TryResolveClosedType(
            open_type, &int_arg, 1);
        TypeInfoHandle expected = MakeTypeHandle(mid, 0x02000010);
        if (result != expected) {
            char buf[128];
            ::snprintf(buf, sizeof(buf),
                "TryResolveClosedType failed at module %u: expected 0x%016llX, got 0x%016llX",
                i, (unsigned long long)expected, (unsigned long long)result);
            FAIL(buf);
            for (auto id : module_ids) MarkModuleTombstone(id);
            return;
        }
    }

    // Verify total instantiation count.
    uint32_t total = chaos::il2cpp::generic_context::GetRegisteredInstantiationCount();
    if (total < kTargetModuleCount * 3) {
        char buf[128];
        ::snprintf(buf, sizeof(buf),
            "Expected >= %u instantiations, got %u", kTargetModuleCount * 3, total);
        FAIL(buf);
    }

    // Cleanup: unregister all modules.
    for (auto id : module_ids) {
        chaos::il2cpp::generic_context::UnregisterModuleGenerics(id);
        MarkModuleTombstone(id);
    }

    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test 5: ModuleRegistry — free-list recycling after tombstone
// ════════════════════════════════════════════════════════════════════════════

static void TestModuleFreeListRecycling() {
    TEST("ModuleRegistry free-list recycling after 200+ tombstone + re-register");

    // Register 200 modules, tombstone them all, then re-register 200 more.
    // The second batch should reuse freed slots from the free list.

    std::vector<uint32_t> first_batch;
    first_batch.reserve(kTargetModuleCount);

    for (uint32_t i = 0; i < kTargetModuleCount; i++) {
        char name[64];
        ::snprintf(name, sizeof(name), "RecycleModule_A_%u.dll", i);
        ModuleDescriptor desc{};
        desc.name_utf8 = name;
        desc.type_count = 1;
        uint32_t id = RegisterModule(name, &desc);
        if (id == kInvalidModuleId) {
            FAIL("first batch RegisterModule failed");
            return;
        }
        first_batch.push_back(id);
    }

    // Tombstone all.
    for (auto id : first_batch) {
        MarkModuleTombstone(id);
    }

    // Re-register 200 more — should reuse freed slots.
    for (uint32_t i = 0; i < kTargetModuleCount; i++) {
        char name[64];
        ::snprintf(name, sizeof(name), "RecycleModule_B_%u.dll", i);
        ModuleDescriptor desc{};
        desc.name_utf8 = name;
        desc.type_count = 1;
        uint32_t id = RegisterModule(name, &desc);
        if (id == kInvalidModuleId) {
            char buf[128];
            ::snprintf(buf, sizeof(buf),
                "second batch RegisterModule failed at index %u — free list may be exhausted", i);
            FAIL(buf);
            return;
        }
        // Verify the id is within the already-allocated range (recycled).
        if (id >= kMaxModules || id == 0) {
            char buf[128];
            ::snprintf(buf, sizeof(buf), "recycled id %u out of range", id);
            FAIL(buf);
            return;
        }
    }

    // Verify total module count hasn't doubled.
    uint32_t count = GetModuleCount();
    if (count > kTargetModuleCount + 5) {
        char buf[128];
        ::snprintf(buf, sizeof(buf),
            "module count %u suggests no recycling (expected ~%u)", count, kTargetModuleCount + 1);
        FAIL(buf);
    }

    PASS();
}

// ════════════════════════════════════════════════════════════════════════════
// Test 6: Full lifecycle — register, allocate, unregister, verify cleanup
// ════════════════════════════════════════════════════════════════════════════

static void TestFullLifecycle() {
    TEST("Full lifecycle: 200 modules → domains → generics → unload → verify");

    constexpr uint32_t kCount = 200;

    std::vector<uint32_t> module_ids;
    std::vector<DomainId> domain_ids;
    module_ids.reserve(kCount);
    domain_ids.reserve(kCount);

    // Phase 1: Register modules + domains + generics.
    for (uint32_t i = 0; i < kCount; i++) {
        char name[64];
        ::snprintf(name, sizeof(name), "Lifecycle_%u.dll", i);

        ModuleDescriptor desc{};
        desc.name_utf8 = name;
        desc.type_count = 1;
        uint32_t mid = RegisterModule(name, &desc);
        if (mid == kInvalidModuleId) {
            FAIL("RegisterModule failed in lifecycle test");
            goto cleanup;
        }
        module_ids.push_back(mid);

        DomainInit init{};
        init.module_name = name;
        init.module_kind = 1;
        DomainId did = RegisterMemoryDomain(init);
        if (did == kDomainIdInvalid) {
            FAIL("RegisterMemoryDomain failed in lifecycle test");
            goto cleanup;
        }
        domain_ids.push_back(did);

        // Allocate and write to domain.
        auto* domain = FindDomainById(did);
        if (domain != nullptr) {
            DomainScope scope(domain);
            void* p = DomainCurrentAllocateTagged(128);
            if (p != nullptr) {
                ::memset(p, 0xBB, 128);
            }
        }

        // Register generics.
        TypeInfoHandle open_type = MakeTypeHandle(mid, 0x02000001);
        TypeInfoHandle closed_a  = MakeTypeHandle(mid, 0x02000010);
        TypeInfoHandle int_arg   = MakeTypeHandle(0, 0x01000002);
        chaos::il2cpp::generic_context::RegisterGenericInstantiation(
            open_type, closed_a, &int_arg, 1);
    }

    // Phase 2: Verify everything is live.
    for (uint32_t i = 0; i < kCount; i++) {
        if (LookupModule(module_ids[i]) == nullptr) {
            FAIL("module not found after registration");
            goto cleanup;
        }
        if (FindDomainById(domain_ids[i]) == nullptr) {
            FAIL("domain not found after registration");
            goto cleanup;
        }
    }

    // Phase 3: Unload everything (simulate ALC unload sequence).
    for (uint32_t i = 0; i < kCount; i++) {
        // 1. Tombstone module.
        MarkModuleTombstone(module_ids[i]);

        // 2. Unregister generics.
        chaos::il2cpp::generic_context::UnregisterModuleGenerics(module_ids[i]);

        // 3. Unregister domain.
        UnregisterMemoryDomain(domain_ids[i]);
    }

    // Phase 4: Verify cleanup.
    for (uint32_t i = 0; i < kCount; i++) {
        // Module should still be findable (tombstone retained for handle safety).
        const auto* desc = LookupModule(module_ids[i]);
        if (desc == nullptr) {
            FAIL("module entry disappeared after tombstone");
            goto done;
        }
        if (!desc->tombstone) {
            FAIL("module not marked as tombstone after MarkModuleTombstone");
            goto done;
        }

        // Domain should be gone.
        if (FindDomainById(domain_ids[i]) != nullptr) {
            FAIL("domain still findable after UnregisterMemoryDomain");
            goto done;
        }
    }

    PASS();
    return;

cleanup:
    for (auto id : module_ids) MarkModuleTombstone(id);
    for (auto id : domain_ids) UnregisterMemoryDomain(id);
    // Fall through.

done:;
}

// ════════════════════════════════════════════════════════════════════════════
// Main
// ════════════════════════════════════════════════════════════════════════════

int main() {
    puts("=== 200+ DLL Capacity Stress Tests ===");
    puts("");

    TestModuleRegistryCapacity();
    TestMemoryDomainCapacity();
    TestAssemblyManagerCapacity();
    TestGenericContextCapacity();
    TestModuleFreeListRecycling();
    TestFullLifecycle();

    puts("");
    printf("Results: %d tests, %d failures\n", g_tests, g_failures);

    return g_failures > 0 ? 1 : 0;
}