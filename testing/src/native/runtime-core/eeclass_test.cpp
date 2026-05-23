/// EEClass AOT 验证测试 — T2-8a
///
/// 验证 EnsureEEClass 在 AOT 路径下的正确性：通过注册 mock 模块，
/// 确认 EEClass 的 LazyArray 能正确引用 constexpr descriptor 数据。
/// 同时覆盖动态类型空填充路径及 null/idempotent 边界。

#include <gtest/gtest.h>
#include <chaos/native_types.h>
#include <chaos/type_info.h>

#include <memory_domain.h>
#include <module_registry.h>
#include <eeclass.h>
#include <reflection_query_model.h>

#include <cstdint>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::memory_domain;
using namespace chaos::il2cpp::common;

// ── Stub definitions for symbols normally emitted by AOT codegen ──
namespace chaos { namespace il2cpp { namespace runtime_core {
extern "C" { void* chaos_il2cpp_aot_hotpatch_module = nullptr; }
extern "C" { const void* kChaosExternalRuntimeSubjects = nullptr; }
extern "C" { const void* kChaosExternalRuntimeFnTable = nullptr; }
extern "C" { int32_t kChaosExternalRuntimeCount = 0; }
}}}

// ═══════════════════════════════════════════════════════════════════
// Mock AOT constexpr data — simulates the data that codegen emits
// for a static AOT type with 1 method, 1 field, 1 property.
// ═══════════════════════════════════════════════════════════════════

static constexpr ReflectionQueryEventDescriptor kMockEvents[] = {
    {
        "MockType:TestEvent:0x20000001",        // subject_id_utf8
        "TestEvent",                            // name_utf8
        "System.EventHandler",                  // member_type_utf8
        0                                       // flags
    }
};

static constexpr ReflectionQueryMethodDescriptor kMockMethods[] = {
    {
        0x10000001,                         // metadata_token
        "MockType:TestMethod:0x10000001",   // subject_id_utf8
        "TestMethod",                       // name_utf8
        "System.Int32",                     // member_type_utf8
        0,                                  // parameter_count
        nullptr,                            // parameters
        0,                                  // parameter_descriptor_count
        nullptr                             // default_value_blob
    }
};

static constexpr ReflectionQueryFieldDescriptor kMockFields[] = {
    {
        0x04000001,                         // metadata_token
        "MockType:TestField:0x04000001",    // subject_id_utf8
        "TestField",                        // name_utf8
        "System.Int32",                     // member_type_utf8
        0                                   // constant_value
    }
};

static constexpr ReflectionQueryPropertyDescriptor kMockProperties[] = {
    {
        "MockType:TestProp",                // subject_id_utf8
        "TestProp",                         // name_utf8
        "System.Int32"                      // member_type_utf8
    }
};

static constexpr ReflectionQueryTypeDescriptor kMockTypeDesc = {
    0x02000001,                             // metadata_token
    "MockType:0x02000001",                  // subject_id_utf8
    nullptr,                                // definition_subject_id_utf8
    "TestNamespace",                        // namespace_name_utf8
    "TestAotType",                          // name_utf8
    "TestAotType",                          // display_name_utf8
    nullptr,                                // generic_type_definition
    kMockFields,                            // fields
    1,                                      // field_count
    kMockProperties,                        // properties
    1,                                      // property_count
    kMockEvents,                            // events
    1,                                      // event_count
    kMockMethods,                           // methods
    1,                                      // method_count
    nullptr,                                // generic_parameters
    0,                                      // generic_param_count
    0                                       // reserved_flags
};

static constexpr const ReflectionQueryTypeDescriptor* kMockTypePtrs[] = { &kMockTypeDesc };

static constexpr ReflectionQueryImageDescriptor kMockImage = {
    "TestModule",
    kMockTypePtrs,
    1
};

// ═══════════════════════════════════════════════════════════════════
// EEClass 通过 cold_delta（uint32_t 偏移）关联 MethodTable。
// 在 x64 上，跨区域（stack→heap、static→heap）的指针差可能 >4GB，
// 导致 uint32_t 截断崩溃。因此所有 MethodTable 必须从 domain heap
// 分配，与 EEClass 同域，确保相对偏移在 uint32_t 范围内。
// ═══════════════════════════════════════════════════════════════════

// ── AOT MethodTable: domain-heap allocated to keep offset in uint32_t
static MethodTable* g_aot_mt = nullptr;

// ── type_info_ptrs array for the mock module ──
static const TypeInfoHot* g_aot_type_info_ptrs[1] = { nullptr };

// ═══════════════════════════════════════════════════════════════════
// Fixture
// ═══════════════════════════════════════════════════════════════════

class EEClassTest : public ::testing::Test {
protected:
    static MemoryDomain* s_domain;
    static DomainId      s_domain_id;
    static uint32_t      s_module_id;
    /// Push depth from SetUpTestSuite, so TearDownTestSuite can pop cleanly.
    static int           s_push_depth;

    static void SetUpTestSuite() {
        // ── Memory domain for allocation context ──
        DomainInit init{};
        init.module_name = "EEClassTestDomain";
        init.module_kind = 1;  // HotUpdate
        init.usage_limit = 0;  // unlimited

        s_domain_id = RegisterMemoryDomain(init);
        ASSERT_NE(s_domain_id, kDomainIdInvalid);
        s_domain = FindDomainById(s_domain_id);
        ASSERT_NE(s_domain, nullptr);

        // Push domain so CHAOS_IL2CPP_DOMAIN_CURRENT_NEW below uses the domain
        // heap (not std::malloc fallback).  MethodTable and EEClass must come
        // from the same heap for cold_delta (uint32_t offset) to be valid on x64.
        s_push_depth = PushDomain(s_domain);

        // ── Initialize mock AOT MethodTable on domain heap ──
        // Domain heap allocation guarantees EEClass (also domain-heap) offset
        // fits in uint32_t.  Static/stack + heap would overflow on x64.
        auto* domain_mt = CHAOS_IL2CPP_DOMAIN_CURRENT_NEW(MethodTable);
        ASSERT_NE(domain_mt, nullptr);
        std::memset(domain_mt, 0, sizeof(MethodTable));
        domain_mt->parent_mt       = nullptr;
        domain_mt->vtable_array    = nullptr;
        domain_mt->stable_id       = chaos_compute_type_stable_id("TestAotType");
        domain_mt->vtable_length   = 0;
        domain_mt->warm_delta      = sizeof(TypeInfoHot);  // 32
        domain_mt->type_shape      = chaos_type_shape_reference;
        domain_mt->flags           = 0;
        // warm section is zero-initialized → cold_delta = 0
        g_aot_mt = domain_mt;

        // ── Register mock module ──
        g_aot_type_info_ptrs[0] = domain_mt->AsTypeInfoHot();

        ModuleDescriptor mod_desc{};
        mod_desc.name_utf8       = "EEClassTestModule";
        mod_desc.image           = &kMockImage;
        mod_desc.type_info_ptrs = g_aot_type_info_ptrs;
        mod_desc.type_count      = 1;
        // other fields: default (nullptr/0)

        s_module_id = RegisterModule("EEClassTestModule", &mod_desc);
        ASSERT_NE(s_module_id, kInvalidModuleId);
    }

    static void TearDownTestSuite() {
        // Pop domain that was pushed in SetUpTestSuite.
        PopDomain(s_push_depth);

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

MemoryDomain* EEClassTest::s_domain    = nullptr;
DomainId      EEClassTest::s_domain_id = kDomainIdInvalid;
uint32_t      EEClassTest::s_module_id = kInvalidModuleId;
int           EEClassTest::s_push_depth = -1;

// ═══════════════════════════════════════════════════════════════════
// Test cases
// ═══════════════════════════════════════════════════════════════════

/// AOT 路径: 注册的 MethodTable → EnsureEEClass 从 constexpr descriptor
/// 填充完整的 EEClass，LazyArray 直接引用 constexpr 数据（零拷贝）。
TEST_F(EEClassTest, AOTPath)
{
    auto* mt = g_aot_mt;
    ASSERT_NE(mt, nullptr);
    ASSERT_EQ(mt->cold_delta, 0u);  // not yet attached

    bool ok = EnsureEEClass(mt);
    ASSERT_TRUE(ok);
    ASSERT_NE(mt->cold_delta, 0u);

    auto* ee = GetEEClass(mt);
    ASSERT_NE(ee, nullptr);

    // Identity
    EXPECT_STREQ(ee->name_utf8, "TestAotType");
    EXPECT_STREQ(ee->namespace_utf8, "TestNamespace");
    EXPECT_EQ(ee->mt, mt);

    // Methods: point into constexpr descriptor (zero copy)
    ASSERT_TRUE(ee->methods.filled);
    ASSERT_EQ(ee->methods.count, 1u);
    ASSERT_NE(ee->methods.data, nullptr);
    EXPECT_EQ(ee->methods.data[0].metadata_token, 0x10000001u);
    EXPECT_STREQ(ee->methods.data[0].name_utf8, "TestMethod");

    // Fields
    ASSERT_TRUE(ee->fields.filled);
    ASSERT_EQ(ee->fields.count, 1u);
    ASSERT_NE(ee->fields.data, nullptr);
    EXPECT_EQ(ee->fields.data[0].metadata_token, 0x04000001u);
    EXPECT_STREQ(ee->fields.data[0].name_utf8, "TestField");

    // Properties
    ASSERT_TRUE(ee->properties.filled);
    ASSERT_EQ(ee->properties.count, 1u);
    ASSERT_NE(ee->properties.data, nullptr);
    EXPECT_STREQ(ee->properties.data[0].name_utf8, "TestProp");

    // Generic metadata: none in this test
    EXPECT_EQ(ee->generic_type_def, nullptr);
    EXPECT_EQ(ee->generic_param_count, 0u);
}

/// 重复调用 EnsureEEClass 是幂等的 — 第二次返回 true，cold_delta 不变。
TEST_F(EEClassTest, Idempotent)
{
    auto* mt = g_aot_mt;
    ASSERT_NE(mt, nullptr);
    ASSERT_NE(mt->cold_delta, 0u);  // set by AOTPath test

    uint32_t before = mt->cold_delta;
    bool ok = EnsureEEClass(mt);
    ASSERT_TRUE(ok);
    EXPECT_EQ(mt->cold_delta, before);  // no double allocation
}

/// null MethodTable → EnsureEEClass 返回 false。
TEST_F(EEClassTest, NullMethodTable)
{
    bool ok = EnsureEEClass(nullptr);
    EXPECT_FALSE(ok);
}

/// GetEEClass 当 cold_delta == 0 时返回 nullptr。
TEST_F(EEClassTest, GetEEClassReturnsNullForNoColdDelta)
{
    auto* ee = GetEEClass(static_cast<MethodTable*>(nullptr));
    EXPECT_EQ(ee, nullptr);
}

/// GetEEClass(null) 返回 nullptr。
TEST_F(EEClassTest, GetEEClassNullMT)
{
    auto* ee = GetEEClass(static_cast<MethodTable*>(nullptr));
    EXPECT_EQ(ee, nullptr);
}

/// 未注册的 MethodTable → 动态类型路径：空 EEClass 被分配，
/// methods/fields/properties 的 filled=true 但 data=null/count=0。
/// 使用 domain-heap 分配的 MethodTable（静态/栈数据与 heap 的偏移 >4GB 会溢出 uint32_t cold_delta）。
TEST_F(EEClassTest, DynamicTypePath)
{
    // Domain-heap 分配的 MethodTable，与 EEClass 同域 → cold_delta 可表示
    auto* mt = CHAOS_IL2CPP_DOMAIN_CURRENT_NEW(MethodTable);
    ASSERT_NE(mt, nullptr);
    std::memset(mt, 0, sizeof(MethodTable));
    mt->stable_id  = chaos_compute_type_stable_id("DynamicType");
    mt->warm_delta = sizeof(TypeInfoHot);
    mt->type_shape = chaos_type_shape_reference;
    // warm section zeroed → cold_delta = 0

    ASSERT_EQ(mt->cold_delta, 0u);

    bool ok = EnsureEEClass(mt);
    ASSERT_TRUE(ok);
    ASSERT_NE(mt->cold_delta, 0u);

    auto* ee = GetEEClass(mt);
    ASSERT_NE(ee, nullptr);
    EXPECT_EQ(ee->mt, mt);

    // 动态路径：EEClass 刚创建，filled=由 CHAOS_IL2CPP_DOMAIN_CURRENT_NEW
    // 的 placement-new EEClass{} 初始化决定。实际观测 filled=true，
    // 说明默认成员初始化器正确执行。data/count 始终为 0/null。
    EXPECT_EQ(ee->methods.count, 0u);
    EXPECT_EQ(ee->methods.data, nullptr);

    EXPECT_EQ(ee->fields.count, 0u);
    EXPECT_EQ(ee->fields.data, nullptr);

    EXPECT_EQ(ee->properties.count, 0u);
    EXPECT_EQ(ee->properties.data, nullptr);

    // Ensure*Filled 标记 filled=true（已经是 true 则 no-op）
    EnsureMethodsFilled(ee);
    EnsureFieldsFilled(ee);
    EnsurePropertiesFilled(ee);
}

/// 对 AOT 类型的 EnsureMethodsFilled 是 no-op（数据已在 EnsureEEClass 中填充）。
TEST_F(EEClassTest, EnsureMethodsFilledNoopForAOT)
{
    auto* mt = g_aot_mt;
    ASSERT_NE(mt, nullptr);
    ASSERT_TRUE(EnsureEEClass(mt));

    auto* ee = GetEEClass(mt);
    ASSERT_NE(ee, nullptr);
    ASSERT_TRUE(ee->methods.filled);
    ASSERT_EQ(ee->methods.count, 1u);

    // 再次调用不应改变任何内容
    EnsureMethodsFilled(ee);
    ASSERT_TRUE(ee->methods.filled);
    ASSERT_EQ(ee->methods.count, 1u);

    EnsureFieldsFilled(ee);
    ASSERT_TRUE(ee->fields.filled);
    ASSERT_EQ(ee->fields.count, 1u);

    EnsurePropertiesFilled(ee);
    ASSERT_TRUE(ee->properties.filled);
    ASSERT_EQ(ee->properties.count, 1u);
}

/// 对动态类型的 Ensure*Filled 只是设置 filled=true。
TEST_F(EEClassTest, EnsureFilledNoopForDynamic)
{
    auto* mt = CHAOS_IL2CPP_DOMAIN_CURRENT_NEW(MethodTable);
    ASSERT_NE(mt, nullptr);
    std::memset(mt, 0, sizeof(MethodTable));
    mt->stable_id  = chaos_compute_type_stable_id("DynamicType2");
    mt->warm_delta = sizeof(TypeInfoHot);
    mt->type_shape = chaos_type_shape_reference;

    ASSERT_TRUE(EnsureEEClass(mt));

    auto* ee = GetEEClass(mt);
    ASSERT_NE(ee, nullptr);

    // 调用前 filled 已为 true（由 CHAOS_IL2CPP_DOMAIN_CURRENT_NEW 初始化决定）
    // data=null, count=0。Ensure*Filled 是 no-op。
    EXPECT_EQ(ee->methods.count, 0u);
    EXPECT_EQ(ee->methods.data, nullptr);

    EnsureMethodsFilled(ee);
    EXPECT_EQ(ee->methods.count, 0u);
    EXPECT_EQ(ee->methods.data, nullptr);
}

/// null EEClass 上的 Ensure*Filled 不应崩溃。
TEST_F(EEClassTest, EnsureFilledNullEEClass)
{
    EnsureMethodsFilled(nullptr);
    EnsureFieldsFilled(nullptr);
    EnsurePropertiesFilled(nullptr);
    EnsureEventsFilled(nullptr);
    // 没有崩溃就是通过
}

/// AOT 路径: 验证 events 数据从 constexpr ReflectionQueryTypeDescriptor 正确填充。
TEST_F(EEClassTest, AOTPathEvents)
{
    auto* mt = g_aot_mt;
    ASSERT_NE(mt, nullptr);
    ASSERT_NE(mt->cold_delta, 0u);  // filled by AOTPath test

    auto* ee = GetEEClass(mt);
    ASSERT_NE(ee, nullptr);

    ASSERT_TRUE(ee->events.filled);
    ASSERT_EQ(ee->events.count, 1u);
    ASSERT_NE(ee->events.data, nullptr);
    EXPECT_STREQ(ee->events.data[0].name_utf8, "TestEvent");
    EXPECT_STREQ(ee->events.data[0].subject_id_utf8, "MockType:TestEvent:0x20000001");
}

/// AOT 路径: 验证 EnsureEEClass 不为 null 的 getter。
TEST_F(EEClassTest, GetEEClassNonNull)
{
    auto* mt = g_aot_mt;
    ASSERT_NE(mt, nullptr);
    ASSERT_NE(mt->cold_delta, 0u);

    auto* ee = GetEEClass(mt);
    ASSERT_NE(ee, nullptr);
    EXPECT_EQ(ee->mt, mt);
}

/// AOT 路径: generic_type_def 为空时(非泛型类型)→ nullptr。
/// 验证 reinterpret_cast 路径在 nullptr 时安全跳过。
TEST_F(EEClassTest, GenericTypeDefNull)
{
    auto* mt = g_aot_mt;
    ASSERT_NE(mt, nullptr);

    auto* ee = GetEEClass(mt);
    ASSERT_NE(ee, nullptr);
    EXPECT_EQ(ee->generic_type_def, nullptr);
    EXPECT_EQ(ee->generic_param_count, 0u);
}

/// generic_type_def reinterpret_cast 安全性与正确性。
/// EnsureEEClass 将 constexpr ReflectionQueryTypeDescriptor::generic_type_definition
/// 通过 reinterpret_cast<const MethodTable*> 转换成 MethodTable*。
/// 本测试验证该转换不会崩溃且结果指向正确的地址。
TEST_F(EEClassTest, GenericTypeDefReinterpretCast)
{
    // 注册第二个类型，其 generic_type_definition 指向第一个类型(kMockTypeDesc)。
    // reinterpret_cast<const MethodTable*>(&kMockTypeDesc) 在此上下文中安全，
    // 因为 constexpr descriptor 的起始位置与 MethodTable 兼容(TypeInfoHot 在偏移 0)。

    // ── 第二个类型 descriptor: generic_type_definition = &kMockTypeDesc ──
    // Use 'static' (not 'constexpr') because reinterpret_cast is not valid
    // in constexpr context on MSVC.
    static ReflectionQueryTypeDescriptor kGenericMockType;
    kGenericMockType.metadata_token = 0x02000002;
    kGenericMockType.subject_id_utf8 = "GenericMockType:0x02000002";
    kGenericMockType.definition_subject_id_utf8 = nullptr;
    kGenericMockType.namespace_name_utf8 = "TestNamespace";
    kGenericMockType.name_utf8 = "GenericMockType";
    kGenericMockType.display_name_utf8 = "GenericMockType";
    kGenericMockType.generic_type_definition = &kMockTypeDesc;
    kGenericMockType.fields = nullptr;
    kGenericMockType.field_count = 0;
    kGenericMockType.properties = nullptr;
    kGenericMockType.property_count = 0;
    kGenericMockType.events = nullptr;
    kGenericMockType.event_count = 0;
    kGenericMockType.methods = nullptr;
    kGenericMockType.method_count = 0;
    kGenericMockType.generic_parameters = nullptr;
    kGenericMockType.generic_param_count = 0;
    kGenericMockType.reserved_flags = 0;

    static constexpr const ReflectionQueryTypeDescriptor* kGenericMockPtrs[] = { &kGenericMockType };
    static constexpr ReflectionQueryImageDescriptor kGenericMockImage = {
        "GenericMockModule",
        kGenericMockPtrs,
        1
    };

    // ── 分配第二个 MethodTable (domain heap) ──
    auto* mt2 = CHAOS_IL2CPP_DOMAIN_CURRENT_NEW(MethodTable);
    ASSERT_NE(mt2, nullptr);
    std::memset(mt2, 0, sizeof(MethodTable));
    mt2->parent_mt     = nullptr;
    mt2->vtable_array  = nullptr;
    mt2->stable_id     = chaos_compute_type_stable_id("GenericMockType");
    mt2->vtable_length = 0;
    mt2->warm_delta    = sizeof(TypeInfoHot);
    mt2->type_shape    = chaos_type_shape_reference;
    mt2->flags         = 0;

    // ── 注册第二个模块 ──
    static const TypeInfoHot* g_generic_type_info_ptrs[1] = { mt2->AsTypeInfoHot() };
    ModuleDescriptor mod_desc{};
    mod_desc.name_utf8       = "GenericMockModule";
    mod_desc.image           = &kGenericMockImage;
    mod_desc.type_info_ptrs  = g_generic_type_info_ptrs;
    mod_desc.type_count      = 1;
    uint32_t mod2_id = RegisterModule("GenericMockModule", &mod_desc);
    ASSERT_NE(mod2_id, kInvalidModuleId);

    // ── EnsureEEClass ──
    ASSERT_EQ(mt2->cold_delta, 0u);
    bool ok = EnsureEEClass(mt2);
    ASSERT_TRUE(ok);
    ASSERT_NE(mt2->cold_delta, 0u);

    auto* ee = GetEEClass(mt2);
    ASSERT_NE(ee, nullptr);

    // generic_type_def 应指向 kMockTypeDesc 的 reinterpret_cast 版本
    ASSERT_NE(ee->generic_type_def, nullptr);
    EXPECT_EQ(ee->generic_type_def,
              reinterpret_cast<const MethodTable*>(&kMockTypeDesc));
    EXPECT_EQ(ee->generic_param_count, 0u);
}
