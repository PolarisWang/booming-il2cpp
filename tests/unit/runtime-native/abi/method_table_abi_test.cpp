/// Integration tests for method_table and ABI manifest APIs (GoogleTest).
///
/// Tests cover:
///   - WriteMethodTable / ResolveMethodTable / ClearMethodTableByGeneration
///   - SetMethodOrigin / GetMethodOrigin
///   - ChaosAbiManifestValidate (version, carrier range, checksum)
///   - ChaosAbiManifestCompatible / ChaosAbiManifestCheckMethodSignature
///   - ResolveMethodTableWithAbiCheck (valid + mismatched ABI)

#include "method_table.h"
#include "module_registry.h"
#include "abi_manifest.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <new>

// ── Helper: FNV-1a hash (matches abi_manifest.cpp) ──────────────────────────
static uint32_t HashPayload(const uint8_t* data, size_t byte_count) {
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < byte_count; i++) {
        hash ^= static_cast<uint32_t>(data[i]);
        hash *= 16777619u;
    }
    return hash;
}

// ── Helper: construct a valid ABI manifest in a buffer ───────────────────────
static const ChaosAbiManifestV0* BuildTestManifest(uint8_t* buf, size_t buf_size) {
    const uint32_t kMethodCount = 2;
    const uint32_t kParamCount  = 3;

    size_t needed = sizeof(ChaosAbiManifestV0) +
                    kMethodCount * sizeof(ChaosAbiMethodEntryV0) +
                    kParamCount;
    if (buf_size < needed) return nullptr;

    std::memset(buf, 0, buf_size);

    auto* manifest = new (buf) ChaosAbiManifestV0{};
    manifest->abi_version          = CHAOS_ABI_MANIFEST_VERSION;
    manifest->method_count         = kMethodCount;
    manifest->parameters_byte_count = kParamCount;

    auto* entries = reinterpret_cast<ChaosAbiMethodEntryV0*>(
        buf + sizeof(ChaosAbiManifestV0));
    entries[0].return_carrier  = CHAOS_ABI_CARRIER_INT32;
    entries[0].parameter_count = 2;
    entries[1].return_carrier  = CHAOS_ABI_CARRIER_VOID;
    entries[1].parameter_count = 1;

    auto* params = buf + sizeof(ChaosAbiManifestV0) +
                   kMethodCount * sizeof(ChaosAbiMethodEntryV0);
    params[0] = CHAOS_ABI_CARRIER_INT32;
    params[1] = CHAOS_ABI_CARRIER_FLOAT64;
    params[2] = CHAOS_ABI_CARRIER_BY_REF;

    size_t payload_offset = sizeof(ChaosAbiManifestV0);
    size_t payload_bytes = kMethodCount * sizeof(ChaosAbiMethodEntryV0) + kParamCount;
    manifest->checksum = HashPayload(buf + payload_offset, payload_bytes);

    return manifest;
}

// ════════════════════════════════════════════════════════════════════════════
// Test: basic method_table operations
// ════════════════════════════════════════════════════════════════════════════

TEST(MethodTableAbi, BasicWriteResolveClear) {
    int dummy_value = 42;
    void* dummy_ptr = &dummy_value;

    EXPECT_TRUE(chaos::il2cpp::method_table::WriteMethodTable(0, dummy_ptr, 1u));
    EXPECT_EQ(chaos::il2cpp::method_table::ResolveMethodTable(0), dummy_ptr);

    // Out-of-range index
    EXPECT_FALSE(chaos::il2cpp::method_table::WriteMethodTable(
        chaos::il2cpp::method_table::kMethodTableSize, dummy_ptr, 1u));
    EXPECT_EQ(chaos::il2cpp::method_table::ResolveMethodTable(
        chaos::il2cpp::method_table::kMethodTableSize), nullptr);

    // Clear by generation
    chaos::il2cpp::method_table::ClearMethodTableByGeneration(1u);
    EXPECT_EQ(chaos::il2cpp::method_table::ResolveMethodTable(0), nullptr);

    // Invalid generation (kInvalidGeneration = 0) is a no-op
    chaos::il2cpp::method_table::WriteMethodTable(1, dummy_ptr, 2u);
    chaos::il2cpp::method_table::ClearMethodTableByGeneration(
        chaos::il2cpp::method_table::kInvalidGeneration);
    EXPECT_EQ(chaos::il2cpp::method_table::ResolveMethodTable(1), dummy_ptr);
}

TEST(MethodTableAbi, OriginTracking) {
    chaos::il2cpp::method_table::SetMethodOrigin(0, 1u, 5u);
    auto origin = chaos::il2cpp::method_table::GetMethodOrigin(0);
    EXPECT_EQ(origin.module_id, 1u);
    EXPECT_EQ(origin.manifest_method_index, 5u);

    // Reset via InitializeMethodTable
    chaos::il2cpp::method_table::InitializeMethodTable();
    origin = chaos::il2cpp::method_table::GetMethodOrigin(0);
    EXPECT_EQ(origin.module_id,
              chaos::il2cpp::runtime_core::kInvalidModuleId);

    // Out-of-range
    origin = chaos::il2cpp::method_table::GetMethodOrigin(
        chaos::il2cpp::method_table::kMethodTableSize);
    EXPECT_EQ(origin.module_id,
              chaos::il2cpp::runtime_core::kInvalidModuleId);
}

// ════════════════════════════════════════════════════════════════════════════
// Test: ChaosAbiManifestValidate
// ════════════════════════════════════════════════════════════════════════════

TEST(MethodTableAbi, AbiManifestValidate) {
    uint8_t buf[256] = {};

    EXPECT_EQ(ChaosAbiManifestValidate(nullptr),
              CHAOS_ABI_MANIFEST_ERR_NULL);

    const auto* valid = BuildTestManifest(buf, sizeof(buf));
    EXPECT_EQ(ChaosAbiManifestValidate(valid), CHAOS_ABI_MANIFEST_OK);

    // Wrong version
    uint8_t buf2[256] = {};
    auto* bad_ver = const_cast<ChaosAbiManifestV0*>(
        BuildTestManifest(buf2, sizeof(buf2)));
    bad_ver->abi_version = 0xFFFFFFFFu;
    EXPECT_EQ(ChaosAbiManifestValidate(bad_ver),
              CHAOS_ABI_MANIFEST_ERR_VERSION);

    // Invalid carrier code
    uint8_t buf3[256] = {};
    auto* bad_carrier = const_cast<ChaosAbiManifestV0*>(
        BuildTestManifest(buf3, sizeof(buf3)));
    auto* entries = reinterpret_cast<ChaosAbiMethodEntryV0*>(
        reinterpret_cast<uint8_t*>(bad_carrier) + sizeof(ChaosAbiManifestV0));
    entries[0].return_carrier = static_cast<ChaosAbiCarrierCode>(99);
    bad_carrier->checksum = 0;
    EXPECT_EQ(ChaosAbiManifestValidate(bad_carrier),
              CHAOS_ABI_MANIFEST_ERR_CARRIER);

    // Corrupted checksum
    uint8_t buf4[256] = {};
    auto* bad_cs = const_cast<ChaosAbiManifestV0*>(
        BuildTestManifest(buf4, sizeof(buf4)));
    bad_cs->checksum ^= 1;
    EXPECT_EQ(ChaosAbiManifestValidate(bad_cs),
              CHAOS_ABI_MANIFEST_ERR_CHECKSUM);

    // Zero checksum skips validation
    uint8_t buf5[256] = {};
    auto* zero_cs = const_cast<ChaosAbiManifestV0*>(
        BuildTestManifest(buf5, sizeof(buf5)));
    zero_cs->checksum = 0;
    EXPECT_EQ(ChaosAbiManifestValidate(zero_cs), CHAOS_ABI_MANIFEST_OK);
}

// ════════════════════════════════════════════════════════════════════════════
// Test: ChaosAbiManifestCompatible
// ════════════════════════════════════════════════════════════════════════════

TEST(MethodTableAbi, AbiManifestCompatible) {
    uint8_t buf_a[256] = {};
    uint8_t buf_b[256] = {};

    const auto* manifest_a = BuildTestManifest(buf_a, sizeof(buf_a));
    const auto* manifest_b = BuildTestManifest(buf_b, sizeof(buf_b));

    EXPECT_NE(ChaosAbiManifestCompatible(manifest_a, manifest_b), 0);
    EXPECT_EQ(ChaosAbiManifestCompatible(nullptr, manifest_a), 0);
    EXPECT_EQ(ChaosAbiManifestCompatible(manifest_a, nullptr), 0);

    auto* diff = const_cast<ChaosAbiManifestV0*>(manifest_b);
    diff->method_count = 1;
    diff->checksum = 0;
    EXPECT_EQ(ChaosAbiManifestCompatible(manifest_a, diff), 0);
}

// ════════════════════════════════════════════════════════════════════════════
// Test: ChaosAbiManifestCheckMethodSignature
// ════════════════════════════════════════════════════════════════════════════

TEST(MethodTableAbi, CheckMethodSignature) {
    uint8_t buf[256] = {};
    const auto* manifest = BuildTestManifest(buf, sizeof(buf));

    uint8_t expected_params_0[] = { CHAOS_ABI_CARRIER_INT32,
                                    CHAOS_ABI_CARRIER_FLOAT64 };
    EXPECT_EQ(ChaosAbiManifestCheckMethodSignature(manifest, 0,
                 CHAOS_ABI_CARRIER_INT32, expected_params_0, 2),
              CHAOS_ABI_MANIFEST_OK);

    EXPECT_EQ(ChaosAbiManifestCheckMethodSignature(manifest, 0,
                 CHAOS_ABI_CARRIER_VOID, expected_params_0, 2),
              CHAOS_ABI_MANIFEST_ERR_CARRIER);

    EXPECT_EQ(ChaosAbiManifestCheckMethodSignature(manifest, 99,
                 CHAOS_ABI_CARRIER_INT32, expected_params_0, 2),
              CHAOS_ABI_MANIFEST_ERR_INDEX);

    uint8_t expected_params_short[] = { CHAOS_ABI_CARRIER_INT32 };
    EXPECT_EQ(ChaosAbiManifestCheckMethodSignature(manifest, 0,
                 CHAOS_ABI_CARRIER_INT32, expected_params_short, 1),
              CHAOS_ABI_MANIFEST_ERR_CARRIER);

    uint8_t expected_params_1[] = { CHAOS_ABI_CARRIER_BY_REF };
    EXPECT_EQ(ChaosAbiManifestCheckMethodSignature(manifest, 1,
                 CHAOS_ABI_CARRIER_VOID, expected_params_1, 1),
              CHAOS_ABI_MANIFEST_OK);
}

// ════════════════════════════════════════════════════════════════════════════
// Test: ChaosAbiManifestGetMethodParamOffset
// ════════════════════════════════════════════════════════════════════════════

TEST(MethodTableAbi, GetMethodParamOffset) {
    uint8_t buf[256] = {};
    const auto* manifest = BuildTestManifest(buf, sizeof(buf));

    EXPECT_EQ(ChaosAbiManifestGetMethodParamOffset(manifest, 0), 0u);
    EXPECT_EQ(ChaosAbiManifestGetMethodParamOffset(manifest, 1), 2u);
    EXPECT_EQ(ChaosAbiManifestGetMethodParamOffset(nullptr, 0), 0u);
    EXPECT_EQ(ChaosAbiManifestGetMethodParamOffset(manifest, 99), 0u);
}

TEST(MethodTableAbi, GetMethodParamOffsetPrefixSum) {
    uint8_t buf[256] = {};
    static const uint32_t kPrefixSum[] = {0u, 2u, 3u};

    const auto* manifest = BuildTestManifest(buf, sizeof(buf));
    auto* mutable_manifest = const_cast<ChaosAbiManifestV0*>(manifest);
    mutable_manifest->param_offset_prefix_sum = kPrefixSum;

    EXPECT_EQ(ChaosAbiManifestGetMethodParamOffset(manifest, 0), 0u);
    EXPECT_EQ(ChaosAbiManifestGetMethodParamOffset(manifest, 1), 2u);
    EXPECT_EQ(ChaosAbiManifestGetMethodParamOffset(nullptr, 0), 0u);
    EXPECT_EQ(ChaosAbiManifestGetMethodParamOffset(manifest, 99), 0u);
}

// ════════════════════════════════════════════════════════════════════════════
// Test: ResolveMethodTableWithAbiCheck
// ════════════════════════════════════════════════════════════════════════════

TEST(MethodTableAbi, ResolveWithAbiCheck) {
    using namespace chaos::il2cpp::method_table;
    using namespace chaos::il2cpp::runtime_core;

    uint8_t manifest_buf[256] = {};
    const auto* manifest = BuildTestManifest(manifest_buf, sizeof(manifest_buf));

    ModuleDescriptor test_desc = {};
    test_desc.name_utf8 = "method-table-abi-test";
    test_desc.abi_manifest = manifest;

    uint32_t mod_id = RegisterModule("method-table-abi-test", &test_desc);
    ASSERT_NE(mod_id, kInvalidModuleId);

    int dummy_value = 99;
    void* dummy_ptr = &dummy_value;

    WriteMethodTable(10, dummy_ptr, 1u);
    SetMethodOrigin(10, mod_id, 0);

    uint8_t valid_params[] = { CHAOS_ABI_CARRIER_INT32,
                               CHAOS_ABI_CARRIER_FLOAT64 };
    void* result = ResolveMethodTableWithAbiCheck(10, CHAOS_ABI_CARRIER_INT32,
                                                  valid_params, 2);
    EXPECT_EQ(result, dummy_ptr);

    // Mismatched return carrier
    result = ResolveMethodTableWithAbiCheck(10, CHAOS_ABI_CARRIER_VOID,
                                            valid_params, 2);
    EXPECT_EQ(result, nullptr);

    // Wrong param count
    result = ResolveMethodTableWithAbiCheck(10, CHAOS_ABI_CARRIER_INT32,
                                            valid_params, 1);
    EXPECT_EQ(result, nullptr);

    // Uninitialized slot
    EXPECT_EQ(ResolveMethodTableWithAbiCheck(999, CHAOS_ABI_CARRIER_INT32,
                                             nullptr, 0), nullptr);

    // Slot with no origin (after InitializeMethodTable) — falls through
    InitializeMethodTable();
    WriteMethodTable(10, dummy_ptr, 1u);
    result = ResolveMethodTableWithAbiCheck(10, CHAOS_ABI_CARRIER_INT32,
                                            nullptr, 0);
    EXPECT_EQ(result, dummy_ptr);

    // Module with no manifest — falls through
    ModuleDescriptor no_manifest_desc = {};
    no_manifest_desc.name_utf8 = "no-manifest-module";
    no_manifest_desc.abi_manifest = nullptr;
    uint32_t no_manifest_id = RegisterModule("no-manifest-module",
                                              &no_manifest_desc);
    ASSERT_NE(no_manifest_id, kInvalidModuleId);

    SetMethodOrigin(10, no_manifest_id, 0);
    result = ResolveMethodTableWithAbiCheck(10, CHAOS_ABI_CARRIER_INT32,
                                            nullptr, 0);
    EXPECT_EQ(result, dummy_ptr);
}
