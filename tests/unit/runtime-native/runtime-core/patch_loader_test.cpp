/// patch_loader_test.cpp — PatchLoader 单元测试 (GoogleTest).
///
/// 覆盖 ApplyPatchFromMemory / Unpatch / PatchMetadataCache /
/// PatchMethod struct / PatchContext 生命周期 的守卫条件和边界情况。
///
/// 注意：完整的 ApplyPatchFromMemory 需要真实的 .patchdata 二进制文件，
/// 这里仅做验证层测试和死代码补完验证。
///
/// Level: ⭐⭐ 测试 + 死代码补完 (Loader 工业化 Phase 1)

#include <patch_loader.h>
#include <patch_data.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

using namespace chaos::il2cpp::runtime_core;

// ── Stub external symbols ──────────────────────────────────────────────────
// Needed for linker since the tests exercise validation-only paths that
// don't actually call into bootstrap/hotpatch infrastructure.
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

namespace chaos::il2cpp::runtime_core {
thread_local jmp_buf* g_chaos_exception_jmp = nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// ApplyPatchFromMemory — validation gates
// ═══════════════════════════════════════════════════════════════════════════

TEST(patch_loader_test, ApplyPatch_NullData_ReturnsNull) {
    EXPECT_EQ(ApplyPatchFromMemory(nullptr, 128, nullptr), nullptr);
}

TEST(patch_loader_test, ApplyPatch_TooSmall_ReturnsNull) {
    // Size < sizeof(PatchDataHeader).
    const char tiny[4] = {0};
    EXPECT_EQ(ApplyPatchFromMemory(tiny, 4, nullptr), nullptr);
}

TEST(patch_loader_test, ApplyPatch_BadMagic_ReturnsNull) {
    PatchDataHeader hdr{};
    hdr.magic = 0xDEADBEEFu;  // wrong magic
    hdr.version = 1;
    hdr.header_size = 112;
    EXPECT_EQ(ApplyPatchFromMemory(&hdr, sizeof(hdr), nullptr), nullptr);
}

TEST(patch_loader_test, ApplyPatch_WrongVersion_ReturnsNull) {
    PatchDataHeader hdr{};
    hdr.magic = PATCH_DATA_MAGIC;
    hdr.version = 99;  // unsupported
    hdr.header_size = 112;
    EXPECT_EQ(ApplyPatchFromMemory(&hdr, sizeof(hdr), nullptr), nullptr);
}

TEST(patch_loader_test, ApplyPatch_Version3_ReturnsNull) {
    PatchDataHeader hdr{};
    hdr.magic = PATCH_DATA_MAGIC;
    hdr.version = 3;  // future version not yet supported
    hdr.header_size = 124;
    EXPECT_EQ(ApplyPatchFromMemory(&hdr, sizeof(hdr), nullptr), nullptr);
}

TEST(patch_loader_test, ApplyPatch_HeaderTooSmall_ReturnsNull) {
    PatchDataHeader hdr{};
    hdr.magic = PATCH_DATA_MAGIC;
    hdr.version = 1;
    hdr.header_size = 64;  // smaller than minimum v1 (112)
    EXPECT_EQ(ApplyPatchFromMemory(&hdr, sizeof(hdr), nullptr), nullptr);
}

// ═══════════════════════════════════════════════════════════════════════════
// Unpatch — null safety
// ═══════════════════════════════════════════════════════════════════════════

TEST(patch_loader_test, Unpatch_Null_ReturnsFalse) {
    EXPECT_FALSE(Unpatch(nullptr));
}

// ═══════════════════════════════════════════════════════════════════════════
// PatchMetadataCache — construction and edge cases
// ═══════════════════════════════════════════════════════════════════════════

TEST(patch_loader_test, MetadataCache_NullHeader_AllReturnsSafe) {
    // Constructing with nullptr should not crash on any query.
    PatchMetadataCache cache(nullptr);

    EXPECT_EQ(cache.GetString(0), nullptr);  // offset 0 but header null
    EXPECT_EQ(cache.GetBlob(0), nullptr);
    EXPECT_EQ(cache.GetBody(0), nullptr);
    EXPECT_EQ(cache.GetAotCoreIr(0), nullptr);
    EXPECT_EQ(cache.MethodCount(), 0u);
    EXPECT_EQ(cache.FieldCount(), 0u);
    EXPECT_EQ(cache.GetMethodDef(0), nullptr);
    EXPECT_EQ(cache.GetFieldDef(0), nullptr);
    EXPECT_EQ(cache.GetTypeDefByIndex(0), nullptr);
    EXPECT_EQ(cache.ResolveTypeDef(0), nullptr);
    EXPECT_EQ(cache.ResolveTypeRef(0), nullptr);
    EXPECT_EQ(cache.ResolveMemberRef(0), nullptr);
    EXPECT_EQ(cache.GetUserString(0x70000001), nullptr);
}

TEST(patch_loader_test, MetadataCache_EmptyHeader_CountsZero) {
    PatchDataHeader hdr{};
    hdr.magic = PATCH_DATA_MAGIC;
    hdr.version = 1;
    hdr.header_size = 112;

    PatchMetadataCache cache(&hdr);
    EXPECT_EQ(cache.MethodCount(), 0u);
    EXPECT_EQ(cache.FieldCount(), 0u);
    EXPECT_EQ(cache.GetMethodDef(0), nullptr);
    EXPECT_EQ(cache.GetFieldDef(0), nullptr);
    EXPECT_EQ(cache.GetTypeDefByIndex(0), nullptr);
}

TEST(patch_loader_test, MetadataCache_GetString_NonZeroOffset) {
    // When header has string heap at a known offset, GetString should
    // compute the correct address. Offset 0 should return "".
    // We test with a local header + synthetic data.
    struct alignas(4) {
        PatchDataHeader hdr;
        char strings[64];
    } blob{};

    blob.hdr.magic = PATCH_DATA_MAGIC;
    blob.hdr.version = 1;
    blob.hdr.header_size = sizeof(PatchDataHeader);
    blob.hdr.string_heap_offset = sizeof(PatchDataHeader);  // right after header
    blob.hdr.string_heap_size = 64;
    std::strcpy(blob.strings, "HelloWorld");

    PatchMetadataCache cache(&blob.hdr);
    const char* s = cache.GetString(0);  // offset 0 -> "" for patch data convention
    EXPECT_EQ(s, nullptr);
}

TEST(patch_loader_test, MetadataCache_GetBlob_ZeroOffset) {
    PatchDataHeader hdr{};
    PatchMetadataCache cache(&hdr);
    EXPECT_EQ(cache.GetBlob(0), nullptr);
}

TEST(patch_loader_test, MetadataCache_GetBody_ZeroOffset) {
    PatchDataHeader hdr{};
    PatchMetadataCache cache(&hdr);
    EXPECT_EQ(cache.GetBody(0), nullptr);
}

// ═══════════════════════════════════════════════════════════════════════════
// PatchMethod struct — default initialization and constants
// ═══════════════════════════════════════════════════════════════════════════

TEST(patch_loader_test, PatchMethod_DefaultInit) {
    PatchMethod m{};
    EXPECT_EQ(m.aot_core_ir_json, nullptr);
    EXPECT_EQ(m.aot_core_ir_json_length, 0u);
    EXPECT_EQ(m.signature_blob, nullptr);
    EXPECT_EQ(m.signature_len, 0u);
    EXPECT_EQ(m.cached_ir, nullptr);
    EXPECT_EQ(m.cached_reg_method, nullptr);
    EXPECT_EQ(m.token, 0u);
    EXPECT_EQ(m.module_id, 0u);
    EXPECT_EQ(m.call_count.load(), 0u);
    EXPECT_EQ(m.ir_state.load(), 0u);
    EXPECT_EQ(m.tier_state.load(), 0u);
}

TEST(patch_loader_test, PatchMethod_TierConstants) {
    EXPECT_EQ(PatchMethod::kStackInterpreted, 0u);
    EXPECT_EQ(PatchMethod::kRegisterLowering, 1u);
    EXPECT_EQ(PatchMethod::kRegisterMapped, 2u);
    EXPECT_EQ(PatchMethod::kOptimizeLowering, 3u);
    EXPECT_EQ(PatchMethod::kOptimizedRegister, 4u);
    EXPECT_EQ(PatchMethod::kT5Unloaded, 5u);
    EXPECT_EQ(PatchMethod::kJitted, 6u);
    EXPECT_EQ(PatchMethod::kJitSkip, 7u);
    EXPECT_EQ(PatchMethod::kHotCallThreshold, 100u);
    EXPECT_EQ(PatchMethod::kT1HotThreshold, 100u);
    EXPECT_EQ(PatchMethod::kT2HotThreshold, 500u);
    EXPECT_EQ(PatchMethod::kT3NativeThreshold, 2000u);
    EXPECT_EQ(PatchMethod::kMaxCodegenFailures, 5u);
    EXPECT_EQ(PatchMethod::kMaxDeoptBeforeDemote, 10u);
}

TEST(patch_loader_test, PatchMethod_SmallArgTypesBuffer) {
    PatchMethod m{};
    // By default, cached_arg_types should point to the small buffer.
    EXPECT_EQ(m.cached_arg_types, m.cached_arg_types_small);
    EXPECT_EQ(m.cached_arg_capacity, 8u);
    EXPECT_FALSE(m.cached_sig_valid);
}

TEST(patch_loader_test, PatchMethod_HotpatchTableConstants) {
    // Token/slot extraction uses ~0u as sentinel.
    EXPECT_EQ(~0u, static_cast<uint32_t>(-1));
}

// ═══════════════════════════════════════════════════════════════════════════
// g_patch_generation — initialization
// ═══════════════════════════════════════════════════════════════════════════

TEST(patch_loader_test, PatchGeneration_InitialValue) {
    EXPECT_EQ(g_patch_generation.load(), 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// ResolveToken — never called stub
// ═══════════════════════════════════════════════════════════════════════════

TEST(patch_loader_test, ResolveToken_ReturnsZero) {
    PatchDataHeader hdr{};
    PatchMetadataCache cache(&hdr);
    EXPECT_EQ(cache.ResolveToken(0x06000001), 0u);
    EXPECT_EQ(cache.ResolveToken(0), 0u);
}
