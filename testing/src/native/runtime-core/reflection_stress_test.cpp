/// Reflection stress/soak test.
///
/// Calls reflection APIs in high-frequency loops to verify stability:
///   - GetFields / GetMethods / GetProperties x 1000
///   - GetElementType / MakeArrayType x 1000
///   - Type query flags (IsArray, IsEnum, IsPublic, etc.) x 1000
///
/// Uses a registered module with synthetic ReflectionQuery descriptors
/// so every API path is exercised (not just short-circuit returns).

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/type_info.h>

#include <module_registry.h>
#include <memory_domain.h>
#include <reflection_api.h>
#include <reflection_query_model.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::memory_domain;
using namespace chaos::il2cpp::common;

// ── Stub definitions for symbols normally emitted by AOT codegen ─────────
namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" { void* chaos_il2cpp_aot_hotpatch_module = nullptr; }
extern "C" { const void* kChaosExternalRuntimeSubjects = nullptr; }
extern "C" { const void* kChaosExternalRuntimeFnTable = nullptr; }
extern "C" { int32_t kChaosExternalRuntimeCount = 0; }
}}}

// ════════════════════════════════════════════════════════════════════════════
// Synthetic descriptor fixtures (lifetime = entire test suite)
// ════════════════════════════════════════════════════════════════════════════

// ── Fields ────────────────────────────────────────────────────────────────
static constexpr ReflectionQueryFieldDescriptor kFields[] = {
    {
        0x04000001u,
        "MyTestType.Field1",
        "Field1",
        "System.Int32",
        0,
        kFieldFlagIsPublic
    },
    {
        0x04000002u,
        "MyTestType.Field2",
        "Field2",
        "System.String",
        0,
        kFieldFlagIsPublic
    }
};
static constexpr uint32_t kFieldCount = 2;

// ── Methods ───────────────────────────────────────────────────────────────
static constexpr ReflectionQueryParameterDescriptor kMethodParams[] = {
    {
        "MyTestType.Method1.param0",
        "value",
        0,
        "System.Int32",
        0,
        0
    }
};

static constexpr ReflectionQueryMethodDescriptor kMethods[] = {
    {
        0x06000001u,
        "MyTestType.Method1",
        "Method1",
        "System.Void",
        1,
        kMethodParams,
        1,
        nullptr,
        kMethodFlagIsPublic
    },
    {
        0x06000002u,
        "MyTestType.Method2",
        "Method2",
        "System.Int32",
        0,
        nullptr,
        0,
        nullptr,
        kMethodFlagIsPublic | kMethodFlagIsStatic
    }
};
static constexpr uint32_t kMethodCount = 2;

// ── Properties ────────────────────────────────────────────────────────────
static constexpr ReflectionQueryPropertyDescriptor kProperties[] = {
    {
        "MyTestType.MyProperty",
        "MyProperty",
        "System.Int32",
        kPropertyFlagCanRead | kPropertyFlagCanWrite
    }
};
static constexpr uint32_t kPropertyCount = 1;

// ── Type descriptor ───────────────────────────────────────────────────────
static constexpr ReflectionQueryTypeDescriptor kTestType = {
    0x02000001u,
    "MyTestType",
    "MyTestType",
    "MyNamespace",
    "MyTestType",
    "MyTestType",
    nullptr,  // generic_type_definition
    kFields,
    kFieldCount,
    kProperties,
    kPropertyCount,
    nullptr,  // events
    0,
    kMethods,
    kMethodCount,
    nullptr,  // generic_parameters
    0,
    kFlagIsPublic | kFlagIsSealed  // reserved_flags
};

// ── Image descriptor ──────────────────────────────────────────────────────
static constexpr const ReflectionQueryTypeDescriptor* kTypesArray[] = {
    &kTestType
};

static constexpr ReflectionQueryImageDescriptor kTestImage = {
    "MyTestAssembly",
    kTypesArray,
    1,
    1, 0, 0, 0  // version
};

// ── TypeInfoHot for type_info_ptrs (needed by GetMethods parent-chain walk) ──
// GetTypeInfoFromReflectionOrGcHandle falls back to treating the handle as a
// GC Type object pointer if type_info_ptrs is null, which crashes for
// non-pointer handles. We provide a minimal TypeInfoHot with parent=nullptr
// so the parent-chain walk terminates immediately.
static constexpr TypeInfoHot kTestTypeInfo{
    nullptr,  // parent (no base type walk)
    nullptr,  // vtable_array
    0,        // stable_id
    0,        // vtable_length
    0,        // warm_delta
    1,        // type_shape (reference type)
    0         // flags
};

static const TypeInfoHot* kTypeInfoPtrs[] = {
    &kTestTypeInfo
};

// ── Per-type arrays (Tier 1) for Module Registry ──────────────────────────
static constexpr uint32_t kTypeFlags[] = {
    kFlagIsPublic | kFlagIsSealed  // MyTestType
};

static constexpr const char* kTypeNames[] = {
    "MyTestType"
};

static constexpr const char* kTypeNamespaces[] = {
    "MyNamespace"
};

static constexpr uint32_t kTypeParentTokens[] = {
    0  // System.Object = token 0
};

// Module descriptor (static lifetime must outlive the test)
static ModuleDescriptor g_module_desc{
    "ReflectionStressTest.dll",
    &kTestImage,
    kTypeFlags,
    kTypeNames,
    kTypeNamespaces,
    kTypeParentTokens,
    kTypeInfoPtrs,  // type_info_ptrs (required by GetMethods parent-chain walk)
    nullptr,   // nested_type_children
    nullptr,   // nested_type_offset
    nullptr,   // generic_param_constraint_data
    nullptr,   // generic_param_constraint_offset
    1,         // type_count
    nullptr,   // custom_attribute_blob
    nullptr,   // custom_attribute_offset
    0,
    nullptr,   // custom_attribute_materializer
    nullptr, nullptr, nullptr, nullptr,
    0, 0, 0, 0,
    nullptr,   // abi_manifest
    false      // tombstone
};

// ── Registered module ID (set once during SetUpTestSuite) ─────────────────
static uint32_t g_module_id = kInvalidModuleId;

// ════════════════════════════════════════════════════════════════════════════
// Test fixture
// ════════════════════════════════════════════════════════════════════════════

class ReflectionStressTest : public ::testing::Test {
protected:
    static MemoryDomain* s_domain;
    static DomainId      s_domain_id;

    static void SetUpTestSuite() {
        // 1. Create a MemoryDomain for allocation context.
        DomainInit init{};
        init.module_name = "ReflectionStressDomain";
        init.module_kind = 1;
        init.usage_limit = 0;

        s_domain_id = RegisterMemoryDomain(init);
        ASSERT_NE(s_domain_id, kDomainIdInvalid);
        s_domain = FindDomainById(s_domain_id);
        ASSERT_NE(s_domain, nullptr);

        // 2. Register the test module.
        g_module_id = RegisterModule("ReflectionStressTest.dll", &g_module_desc);
        ASSERT_NE(g_module_id, kInvalidModuleId);
    }

    static void TearDownTestSuite() {
        // Cleanup: unregister module and domain.
        if (g_module_id != kInvalidModuleId) {
            MarkModuleTombstone(g_module_id);
            g_module_id = kInvalidModuleId;
        }
        if (s_domain_id != kDomainIdInvalid) {
            UnregisterMemoryDomain(s_domain_id);
            s_domain_id = kDomainIdInvalid;
            s_domain = nullptr;
        }
    }

    void SetUp() override {
        scope_depth_ = PushDomain(s_domain);
    }

    void TearDown() override {
        PopDomain(scope_depth_);
    }

    int scope_depth_ = -1;
};

MemoryDomain* ReflectionStressTest::s_domain = nullptr;
DomainId      ReflectionStressTest::s_domain_id = kDomainIdInvalid;

// ════════════════════════════════════════════════════════════════════════════
// Sanity: verify module and descriptor fixtures are findable
// ════════════════════════════════════════════════════════════════════════════

TEST_F(ReflectionStressTest, Sanity_ModuleRegistration)
{
    ASSERT_NE(g_module_id, kInvalidModuleId);
    auto* mod = LookupModule(g_module_id);
    ASSERT_NE(mod, nullptr);
    ASSERT_NE(mod->image, nullptr);
    ASSERT_EQ(mod->image->type_count, 1u);
    ASSERT_NE(mod->image->types[0], nullptr);
    ASSERT_EQ(mod->image->types[0]->metadata_token, 0x02000001u);
    ASSERT_NE(mod->image->types[0]->fields, nullptr);
    ASSERT_NE(mod->image->types[0]->methods, nullptr);
    ASSERT_NE(mod->image->types[0]->properties, nullptr);
}

// ════════════════════════════════════════════════════════════════════════════
// Test 1: GetFields / GetMethods / GetProperties x 1000
// ════════════════════════════════════════════════════════════════════════════

TEST_F(ReflectionStressTest, GetFieldsMethodsProperties_1000Iterations)
{
    auto handle = static_cast<CHAOS_IL2CPP_INTPTR>(
        MakeTypeHandle(g_module_id, 0x02000001u));

    for (int i = 0; i < 1000; i++) {
        CHAOS_IL2CPP_INTPTR fields_result = ChaosReflectionGetFields(handle);
        CHAOS_IL2CPP_INTPTR methods_result = ChaosReflectionGetMethods(handle);
        CHAOS_IL2CPP_INTPTR props_result = ChaosReflectionGetProperties(handle);

        ASSERT_NE(fields_result, 0) << "GetFields returned 0 at iteration " << i;
        ASSERT_NE(methods_result, 0) << "GetMethods returned 0 at iteration " << i;
        ASSERT_NE(props_result, 0) << "GetProperties returned 0 at iteration " << i;
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test 2: GetElementType / MakeArrayType x 1000
// ════════════════════════════════════════════════════════════════════════════

TEST_F(ReflectionStressTest, GetElementType_MakeArrayType_1000Iterations)
{
    // Use a tag-bit encoded handle (direct descriptor pointer).
    auto handle = static_cast<CHAOS_IL2CPP_INTPTR>(
        EncodeReflectionQueryTypeHandle(&kTestType));

    for (int i = 0; i < 1000; i++) {
        // GetElementType on a non-array type returns 0 we just verify no crash.
        CHAOS_IL2CPP_INTPTR elem = ChaosTypeGetElementType(handle);
        (void)elem;

        // MakeArrayType creates an array type descriptor.
        CHAOS_IL2CPP_INTPTR arr = ChaosTypeMakeArrayType(handle);
        (void)arr;
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test 3: Type query flags x 1000
// ════════════════════════════════════════════════════════════════════════════

TEST_F(ReflectionStressTest, TypeQueryFlags_1000Iterations)
{
    // Module Registry handle (needed for ResolveTypeRef in flag stubs).
    auto handle = static_cast<CHAOS_IL2CPP_INTPTR>(
        MakeTypeHandle(g_module_id, 0x02000001u));

    for (int i = 0; i < 1000; i++) {
        CHAOS_IL2CPP_INTPTR is_arr    = ChaosReflectionGetIsArray(handle);
        CHAOS_IL2CPP_INTPTR is_enum   = ChaosReflectionGetIsEnum(handle);
        CHAOS_IL2CPP_INTPTR is_pub    = ChaosReflectionGetIsPublic(handle);
        CHAOS_IL2CPP_INTPTR is_iface  = ChaosReflectionGetIsInterface(handle);
        CHAOS_IL2CPP_INTPTR is_abstr  = ChaosReflectionGetIsAbstract(handle);
        CHAOS_IL2CPP_INTPTR is_sealed = ChaosReflectionGetIsSealed(handle);
        CHAOS_IL2CPP_INTPTR is_vt     = ChaosReflectionGetIsValueType(handle);
        CHAOS_IL2CPP_INTPTR is_gt     = ChaosReflectionGetIsGenericType(handle);

        // Our type: IsPublic=1, IsSealed=1, everything else=0.
        ASSERT_EQ(is_arr,    0) << "IsArray should be 0 at iteration " << i;
        ASSERT_EQ(is_enum,   0) << "IsEnum should be 0 at iteration " << i;
        ASSERT_EQ(is_pub,    1) << "IsPublic should be 1 at iteration " << i;
        ASSERT_EQ(is_iface,  0) << "IsInterface should be 0 at iteration " << i;
        ASSERT_EQ(is_abstr,  0) << "IsAbstract should be 0 at iteration " << i;
        ASSERT_EQ(is_sealed, 1) << "IsSealed should be 1 at iteration " << i;
        ASSERT_EQ(is_vt,     0) << "IsValueType should be 0 at iteration " << i;
        ASSERT_EQ(is_gt,     0) << "IsGenericType should be 0 at iteration " << i;
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test 4: Combined stress interleave all APIs in a single loop
// ════════════════════════════════════════════════════════════════════════════

TEST_F(ReflectionStressTest, CombinedStress_1000Iterations)
{
    auto handle = static_cast<CHAOS_IL2CPP_INTPTR>(
        MakeTypeHandle(g_module_id, 0x02000001u));
    auto tag_handle = static_cast<CHAOS_IL2CPP_INTPTR>(
        EncodeReflectionQueryTypeHandle(&kTestType));

    for (int i = 0; i < 1000; i++) {
        // Member queries (Module Registry handle)
        CHAOS_IL2CPP_INTPTR f = ChaosReflectionGetFields(handle);
        CHAOS_IL2CPP_INTPTR m = ChaosReflectionGetMethods(handle);
        CHAOS_IL2CPP_INTPTR p = ChaosReflectionGetProperties(handle);
        ASSERT_NE(f, 0) << "GetFields failed at iteration " << i;
        ASSERT_NE(m, 0) << "GetMethods failed at iteration " << i;
        ASSERT_NE(p, 0) << "GetProperties failed at iteration " << i;

        // Type manipulation (tag-bit handle)
        CHAOS_IL2CPP_INTPTR elem = ChaosTypeGetElementType(tag_handle);
        (void)elem;
        CHAOS_IL2CPP_INTPTR arr = ChaosTypeMakeArrayType(tag_handle);
        (void)arr;

        // Flag queries (Module Registry handle)
        ASSERT_EQ(ChaosReflectionGetIsArray(handle),    0);
        ASSERT_EQ(ChaosReflectionGetIsEnum(handle),     0);
        ASSERT_EQ(ChaosReflectionGetIsPublic(handle),   1);
        ASSERT_EQ(ChaosReflectionGetIsSealed(handle),   1);
        ASSERT_EQ(ChaosReflectionGetIsInterface(handle), 0);
        ASSERT_EQ(ChaosReflectionGetIsAbstract(handle), 0);
        ASSERT_EQ(ChaosReflectionGetIsValueType(handle), 0);
        ASSERT_EQ(ChaosReflectionGetIsGenericType(handle), 0);
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test 5: Additional reflection API soak
// ════════════════════════════════════════════════════════════════════════════

TEST_F(ReflectionStressTest, AdditionalReflectionApis_1000Iterations)
{
    auto handle = static_cast<CHAOS_IL2CPP_INTPTR>(
        EncodeReflectionQueryTypeHandle(&kTestType));

    for (int i = 0; i < 1000; i++) {
        // GetBaseType returns generic_type_definition (nullptr for us -> 0).
        CHAOS_IL2CPP_INTPTR base = ChaosReflectionGetBaseType(handle);
        (void)base;

        // GetTypeFullName / GetNamespace string table lookup.
        CHAOS_IL2CPP_INTPTR fullname = ChaosReflectionGetTypeFullName(handle);
        (void)fullname;

        CHAOS_IL2CPP_INTPTR ns = ChaosReflectionGetNamespace(handle);
        (void)ns;

        // GetDeclaringType returns generic_type_definition (nullptr -> 0).
        CHAOS_IL2CPP_INTPTR decl = ChaosReflectionGetDeclaringType(handle);
        (void)decl;

        // GetMemberName reads name_utf8 from descriptor.
        CHAOS_IL2CPP_INTPTR name = ChaosReflectionGetMemberName(handle);
        (void)name;

        // GetMetadataToken reads metadata_token from descriptor.
        CHAOS_IL2CPP_INTPTR token = ChaosReflectionGetMetadataToken(handle);
        (void)token;
    }
}
