/// Integration tests for method_table and ABI manifest APIs.
///
/// Tests cover:
///   - WriteMethodTable / ResolveMethodTable / ClearMethodTableByGeneration
///   - SetMethodOrigin / GetMethodOrigin
///   - ChaosAbiManifestValidate (version, carrier range, checksum)
///   - ChaosAbiManifestCompatible / ChaosAbiManifestCheckMethodSignature
///   - ResolveMethodTableWithAbiCheck (valid + mismatched ABI)
///
/// These tests are self-contained and do NOT require the full bootstrap or
/// IL2CPP pipeline.  They link against chaos_runtime_core for the method_table
/// ABI manifest, and module_registry implementations.

#include "method_table.h"
#include "module_registry.h"
#include "abi_manifest.h"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <new>

// ── Test macro ──────────────────────────────────────────────────────────────
static int g_failures = 0;

#define TEST(name, expr)                                                      \
    do {                                                                      \
        if (!(expr)) {                                                        \
            std::fprintf(stderr, "FAIL: %s\n", name);                         \
            ++g_failures;                                                     \
        } else {                                                              \
            std::printf("  PASS: %s\n", name);                                \
        }                                                                     \
    } while (false)

// ── Helper: FNV-1a hash (matches abi_manifest.cpp) ──────────────────────────
static uint32_t HashPayload(const uint8_t* data, size_t byte_count) {
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < byte_count; i++) {
        hash ^= static_cast<uint32_t>(data[i]);
        hash *= 16777619u;
    }
    return hash;
}

// ── Helper: construct a valid ABI manifest in a buffer ──────────────────────
// Builds a manifest with 2 methods:
//   Method 0: return=Int32(1),  params={Int32(1), Float64(9)}
//   Method 1: return=Void(0),   params={ByRef(12)}
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
    entries[0].return_carrier  = CHAOS_ABI_CARRIER_INT32;   // 1
    entries[0].parameter_count = 2;
    entries[1].return_carrier  = CHAOS_ABI_CARRIER_VOID;    // 0
    entries[1].parameter_count = 1;

    auto* params = buf + sizeof(ChaosAbiManifestV0) +
                   kMethodCount * sizeof(ChaosAbiMethodEntryV0);
    params[0] = CHAOS_ABI_CARRIER_INT32;                    // method 0, param 0
    params[1] = CHAOS_ABI_CARRIER_FLOAT64;                  // method 0, param 1
    params[2] = CHAOS_ABI_CARRIER_BY_REF;                   // method 1, param 0

    // Compute checksum over entries + params payload
    size_t checksum_off = offsetof(ChaosAbiManifestV0, checksum) + sizeof(uint32_t);
    size_t payload_bytes = needed - checksum_off;
    manifest->checksum = HashPayload(buf + checksum_off, payload_bytes);

    return manifest;
}

// ── Test: basic method_table operations ─────────────────────────────────────
static bool TestMethodTableBasic() {
    // Write and read a valid entry
    int dummy_value = 42;
    void* dummy_ptr = &dummy_value;
    TEST("WriteMethodTable valid index", chaos::il2cpp::method_table::WriteMethodTable(0, dummy_ptr, 1u));
    void* read = chaos::il2cpp::method_table::ResolveMethodTable(0);
    TEST("ResolveMethodTable returns written pointer", read == dummy_ptr);

    // Out-of-range index
    TEST("WriteMethodTable out of range returns false",
         !chaos::il2cpp::method_table::WriteMethodTable(chaos::il2cpp::method_table::kMethodTableSize, dummy_ptr, 1u));
    TEST("ResolveMethodTable out of range returns nullptr",
         chaos::il2cpp::method_table::ResolveMethodTable(chaos::il2cpp::method_table::kMethodTableSize) == nullptr);

    // Clear by generation
    chaos::il2cpp::method_table::ClearMethodTableByGeneration(1u);
    TEST("ClearMethodTableByGeneration clears entry",
         chaos::il2cpp::method_table::ResolveMethodTable(0) == nullptr);

    // Invalid generation (kInvalidGeneration = 0) is a no-op
    chaos::il2cpp::method_table::WriteMethodTable(1, dummy_ptr, 2u);
    chaos::il2cpp::method_table::ClearMethodTableByGeneration(chaos::il2cpp::method_table::kInvalidGeneration);
    TEST("ClearMethodTableByGeneration 0 does not clear",
         chaos::il2cpp::method_table::ResolveMethodTable(1) == dummy_ptr);

    return true;
}

// ── Test: origin tracking ───────────────────────────────────────────────────
static bool TestOriginTracking() {
    // Set origin and read back
    chaos::il2cpp::method_table::SetMethodOrigin(0, 1u, 5u);
    auto origin = chaos::il2cpp::method_table::GetMethodOrigin(0);
    TEST("SetMethodOrigin + GetMethodOrigin module_id", origin.module_id == 1u);
    TEST("SetMethodOrigin + GetMethodOrigin manifest_index", origin.manifest_method_index == 5u);

    // Unset slot returns kInvalidModuleId after InitializeMethodTable
    chaos::il2cpp::method_table::InitializeMethodTable();
    origin = chaos::il2cpp::method_table::GetMethodOrigin(0);
    TEST("InitializeMethodTable resets origin to kInvalidModuleId",
         origin.module_id == chaos::il2cpp::runtime_core::kInvalidModuleId);

    // Out-of-range returns kInvalidModuleId
    origin = chaos::il2cpp::method_table::GetMethodOrigin(chaos::il2cpp::method_table::kMethodTableSize);
    TEST("GetMethodOrigin out of range returns kInvalidModuleId",
         origin.module_id == chaos::il2cpp::runtime_core::kInvalidModuleId);

    return true;
}

// ── Test: ChaosAbiManifestValidate ──────────────────────────────────────────
static bool TestAbiManifestValidate() {
    uint8_t buf[256] = {};

    // NULL manifest
    TEST("ChaosAbiManifestValidate null returns ERR_NULL",
         ChaosAbiManifestValidate(nullptr) == CHAOS_ABI_MANIFEST_ERR_NULL);

    // Valid manifest
    const auto* valid = BuildTestManifest(buf, sizeof(buf));
    TEST("ChaosAbiManifestValidate valid manifest",
         ChaosAbiManifestValidate(valid) == CHAOS_ABI_MANIFEST_OK);

    // Wrong version
    uint8_t buf2[256] = {};
    auto* bad_ver = const_cast<ChaosAbiManifestV0*>(BuildTestManifest(buf2, sizeof(buf2)));
    bad_ver->abi_version = 0xFFFFFFFFu;
    TEST("ChaosAbiManifestValidate wrong version",
         ChaosAbiManifestValidate(bad_ver) == CHAOS_ABI_MANIFEST_ERR_VERSION);

    // Invalid carrier code
    uint8_t buf3[256] = {};
    auto* bad_carrier = const_cast<ChaosAbiManifestV0*>(BuildTestManifest(buf3, sizeof(buf3)));
    auto* entries = reinterpret_cast<ChaosAbiMethodEntryV0*>(
        reinterpret_cast<uint8_t*>(bad_carrier) + sizeof(ChaosAbiManifestV0));
    entries[0].return_carrier = static_cast<ChaosAbiCarrierCode>(99);
    bad_carrier->checksum = 0;  // skip checksum since we changed data
    TEST("ChaosAbiManifestValidate invalid carrier",
         ChaosAbiManifestValidate(bad_carrier) == CHAOS_ABI_MANIFEST_ERR_CARRIER);

    // Corrupted checksum
    uint8_t buf4[256] = {};
    auto* bad_cs = const_cast<ChaosAbiManifestV0*>(BuildTestManifest(buf4, sizeof(buf4)));
    bad_cs->checksum ^= 1;
    TEST("ChaosAbiManifestValidate bad checksum",
         ChaosAbiManifestValidate(bad_cs) == CHAOS_ABI_MANIFEST_ERR_CHECKSUM);

    // Zero checksum skips checksum validation
    uint8_t buf5[256] = {};
    auto* zero_cs = const_cast<ChaosAbiManifestV0*>(BuildTestManifest(buf5, sizeof(buf5)));
    zero_cs->checksum = 0;
    TEST("ChaosAbiManifestValidate zero checksum skips validation",
         ChaosAbiManifestValidate(zero_cs) == CHAOS_ABI_MANIFEST_OK);

    return true;
}

// ── Test: ChaosAbiManifestCompatible ────────────────────────────────────────
static bool TestAbiManifestCompatible() {
    uint8_t buf_a[256] = {};
    uint8_t buf_b[256] = {};

    const auto* manifest_a = BuildTestManifest(buf_a, sizeof(buf_a));
    const auto* manifest_b = BuildTestManifest(buf_b, sizeof(buf_b));

    // Same data → compatible
    TEST("ChaosAbiManifestCompatible identical", ChaosAbiManifestCompatible(manifest_a, manifest_b) != 0);

    // Null → not compatible
    TEST("ChaosAbiManifestCompatible null expected", ChaosAbiManifestCompatible(nullptr, manifest_a) == 0);
    TEST("ChaosAbiManifestCompatible null actual", ChaosAbiManifestCompatible(manifest_a, nullptr) == 0);

    // Different method count → not compatible
    auto* diff = const_cast<ChaosAbiManifestV0*>(manifest_b);
    diff->method_count = 1;
    diff->checksum = 0;  // mismatch anyway
    TEST("ChaosAbiManifestCompatible different count",
         ChaosAbiManifestCompatible(manifest_a, diff) == 0);

    return true;
}

// ── Test: ChaosAbiManifestCheckMethodSignature ──────────────────────────────
static bool TestCheckMethodSignature() {
    uint8_t buf[256] = {};
    const auto* manifest = BuildTestManifest(buf, sizeof(buf));

    // Method 0: return=Int32(1), params={Int32(1), Float64(9)}
    uint8_t expected_params_0[] = { CHAOS_ABI_CARRIER_INT32, CHAOS_ABI_CARRIER_FLOAT64 };
    TEST("CheckMethodSignature method 0 match",
         ChaosAbiManifestCheckMethodSignature(manifest, 0,
             CHAOS_ABI_CARRIER_INT32, expected_params_0, 2) == CHAOS_ABI_MANIFEST_OK);

    // Wrong return
    TEST("CheckMethodSignature method 0 wrong return",
         ChaosAbiManifestCheckMethodSignature(manifest, 0,
             CHAOS_ABI_CARRIER_VOID, expected_params_0, 2) == CHAOS_ABI_MANIFEST_ERR_CARRIER);

    // Method out of range
    TEST("CheckMethodSignature method index out of range",
         ChaosAbiManifestCheckMethodSignature(manifest, 99,
             CHAOS_ABI_CARRIER_INT32, expected_params_0, 2) == CHAOS_ABI_MANIFEST_ERR_INDEX);

    // Wrong param count
    uint8_t expected_params_short[] = { CHAOS_ABI_CARRIER_INT32 };
    TEST("CheckMethodSignature wrong param count",
         ChaosAbiManifestCheckMethodSignature(manifest, 0,
             CHAOS_ABI_CARRIER_INT32, expected_params_short, 1) == CHAOS_ABI_MANIFEST_ERR_CARRIER);

    // Method 1: return=Void(0), params={ByRef(12)}
    uint8_t expected_params_1[] = { CHAOS_ABI_CARRIER_BY_REF };
    TEST("CheckMethodSignature method 1 (ByRef param) match",
         ChaosAbiManifestCheckMethodSignature(manifest, 1,
             CHAOS_ABI_CARRIER_VOID, expected_params_1, 1) == CHAOS_ABI_MANIFEST_OK);

    return true;
}

// ── Test: ChaosAbiManifestGetMethodParamOffset ──────────────────────────────
static bool TestGetMethodParamOffset() {
    uint8_t buf[256] = {};
    const auto* manifest = BuildTestManifest(buf, sizeof(buf));

    // Method 0: offset 0 (no preceding params)
    TEST("GetMethodParamOffset method 0",
         ChaosAbiManifestGetMethodParamOffset(manifest, 0) == 0u);

    // Method 1: offset = method 0's param count = 2
    TEST("GetMethodParamOffset method 1",
         ChaosAbiManifestGetMethodParamOffset(manifest, 1) == 2u);

    // Null manifest
    TEST("GetMethodParamOffset null",
         ChaosAbiManifestGetMethodParamOffset(nullptr, 0) == 0u);

    // Out of range index
    TEST("GetMethodParamOffset out of range",
         ChaosAbiManifestGetMethodParamOffset(manifest, 99) == 0u);

    return true;
}

// ── Test: ResolveMethodTableWithAbiCheck ────────────────────────────────────
static bool TestResolveWithAbiCheck() {
    using namespace chaos::il2cpp::method_table;
    using namespace chaos::il2cpp::runtime_core;

    // Register a test module with a valid ABI manifest
    uint8_t manifest_buf[256] = {};
    const auto* manifest = BuildTestManifest(manifest_buf, sizeof(manifest_buf));

    ModuleDescriptor test_desc = {};
    test_desc.name_utf8 = "method-table-abi-test";
    test_desc.abi_manifest = manifest;

    uint32_t mod_id = RegisterModule("method-table-abi-test", &test_desc);
    TEST("RegisterModule succeeded", mod_id != kInvalidModuleId);

    int dummy_value = 99;
    void* dummy_ptr = &dummy_value;

    // Write entry and set origin to method 0 (return=Int32, params={Int32, Float64})
    WriteMethodTable(10, dummy_ptr, 1u);
    SetMethodOrigin(10, mod_id, 0);

    // Valid ABI check: expected return = Int32(1), params = {Int32(1), Float64(9)}
    uint8_t valid_params[] = { CHAOS_ABI_CARRIER_INT32, CHAOS_ABI_CARRIER_FLOAT64 };
    void* result = ResolveMethodTableWithAbiCheck(10, CHAOS_ABI_CARRIER_INT32, valid_params, 2);
    TEST("ResolveMethodTableWithAbiCheck valid returns pointer", result == dummy_ptr);

    // Mismatched ABI check: wrong return carrier
    uint8_t mismatched_params[] = { CHAOS_ABI_CARRIER_INT32, CHAOS_ABI_CARRIER_FLOAT64 };
    result = ResolveMethodTableWithAbiCheck(10, CHAOS_ABI_CARRIER_VOID, mismatched_params, 2);
    TEST("ResolveMethodTableWithAbiCheck mismatch returns nullptr", result == nullptr);

    // Mismatched ABI check: wrong param count
    result = ResolveMethodTableWithAbiCheck(10, CHAOS_ABI_CARRIER_INT32, valid_params, 1);
    TEST("ResolveMethodTableWithAbiCheck wrong param count returns nullptr", result == nullptr);

    // Uninitialized slot
    TEST("ResolveMethodTableWithAbiCheck uninitialized returns nullptr",
         ResolveMethodTableWithAbiCheck(999, CHAOS_ABI_CARRIER_INT32, nullptr, 0) == nullptr);

    // Slot with no origin (after InitializeMethodTable)
    InitializeMethodTable();
    WriteMethodTable(10, dummy_ptr, 1u);
    // No SetMethodOrigin call — should skip ABI check and return pointer
    result = ResolveMethodTableWithAbiCheck(10, CHAOS_ABI_CARRIER_INT32, nullptr, 0);
    TEST("ResolveMethodTableWithAbiCheck no origin falls through", result == dummy_ptr);

    // Slot with origin pointing to module that has no manifest
    // Register a module with null abi_manifest
    ModuleDescriptor no_manifest_desc = {};
    no_manifest_desc.name_utf8 = "no-manifest-module";
    no_manifest_desc.abi_manifest = nullptr;
    uint32_t no_manifest_id = RegisterModule("no-manifest-module", &no_manifest_desc);
    TEST("RegisterModule without manifest succeeded", no_manifest_id != kInvalidModuleId);

    SetMethodOrigin(10, no_manifest_id, 0);
    result = ResolveMethodTableWithAbiCheck(10, CHAOS_ABI_CARRIER_INT32, nullptr, 0);
    TEST("ResolveMethodTableWithAbiCheck no manifest falls through", result == dummy_ptr);

    return true;
}

// ── Main ─────────────────────────────────────────────────────────────────────
int main() {
    std::printf("=== method_table + ABI manifest test ===\n\n");

    // Reset the method table to clean state
    chaos::il2cpp::method_table::InitializeMethodTable();

    std::printf("--- TestMethodTableBasic ---\n");
    TestMethodTableBasic();

    std::printf("\n--- TestOriginTracking ---\n");
    TestOriginTracking();

    std::printf("\n--- TestAbiManifestValidate ---\n");
    TestAbiManifestValidate();

    std::printf("\n--- TestAbiManifestCompatible ---\n");
    TestAbiManifestCompatible();

    std::printf("\n--- TestCheckMethodSignature ---\n");
    TestCheckMethodSignature();

    std::printf("\n--- TestGetMethodParamOffset ---\n");
    TestGetMethodParamOffset();

    std::printf("\n--- TestResolveWithAbiCheck ---\n");
    TestResolveWithAbiCheck();

    std::printf("\n=== %s ===\n", g_failures == 0 ? "ALL PASSED" : "FAILED");
    return g_failures == 0 ? 0 : 1;
}
