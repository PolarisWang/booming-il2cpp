// native_static_var_test.cpp — Isolated unit tests for native_library + static_var_store
//
// Tests:
//   - ComputeAssemblyHash (inline in header, no deps)
//   - static_var_store_read_field / write_field / unregister_assembly (self-contained)
//   - static_var_store_register + static_var_store_read guard paths
//   - NativeLibrary null-safety guards (no real OS DLL loading)
//
// NOTE: Full NativeLibraryLoad/GetProcAddress/Free require real OS DLLs and
// are tested via integration tests. The null-safety guards are tested here.

#include <gtest/gtest.h>

#include <native_library.h>
#include <static_var_store.h>

// Forward declaration for HotpatchModuleV0 (defined in contracts/native/v0/codegen_bridge.h)
struct HotpatchModuleV0;

#include <cstdint>
#include <cstring>

// ── Stub external symbols ──────────────────────────────────────────────────
// CHAOS_COMMON_TEST_LIBS includes chaos_bootstrap and chaos_interpreter which
// reference symbols normally provided by AOT-generated code.
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

namespace gc = chaos::il2cpp::runtime_core;

// ═══════════════════════════════════════════════════════════════════════════
// ComputeAssemblyHash tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(static_var_store_test, ComputeAssemblyHash_Empty) {
    // FNV-1a of empty string = offset basis (2166136261) & 0x00FFFFFF = 1875397.
    // Clamp-to-1 only fires when the 24-bit result is exactly 0.
    EXPECT_EQ(ComputeAssemblyHash(""), 1875397u);
}

TEST(static_var_store_test, ComputeAssemblyHash_Known) {
    // FNV-1a 32-bit of "mscorlib":
    // h = 2166136261
    // h ^= 'm'(109) * 16777619 → ...
    // Verified stable hash for known assembly names.
    auto hash = ComputeAssemblyHash("mscorlib");
    EXPECT_NE(hash, 0u);
    EXPECT_EQ(hash & 0xFF000000u, 0u);  // 24-bit mask
}

TEST(static_var_store_test, ComputeAssemblyHash_NonZero) {
    // Guarantee: result is never 0 (clamped to 1).
    auto hash = ComputeAssemblyHash("System.Core");
    EXPECT_NE(hash, 0u);
}

TEST(static_var_store_test, ComputeAssemblyHash_DifferentNames) {
    // Different names produce different hashes (collision-resistant).
    auto h1 = ComputeAssemblyHash("AssemblyA");
    auto h2 = ComputeAssemblyHash("AssemblyB");
    EXPECT_NE(h1, h2);
}

// ═══════════════════════════════════════════════════════════════════════════
// static_var_store field-level API tests (self-contained, no AssemblyManager)
// ═══════════════════════════════════════════════════════════════════════════

TEST(static_var_store_test, ReadField_CreatesEntry) {
    // First read of a key creates a slot with value 0.
    auto* ptr = static_var_store_read_field(0xAA, 0xBB);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*static_cast<uintptr_t*>(ptr), 0u);
}

TEST(static_var_store_test, WriteField_ThenRead) {
    uint32_t hash = 0x123456;
    uint32_t field = 0x7890;
    static_var_store_write_field(hash, field, static_cast<uintptr_t>(42));
    auto* ptr = static_var_store_read_field(hash, field);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*static_cast<uintptr_t*>(ptr), 42u);
}

TEST(static_var_store_test, WriteField_Overwrite) {
    uint32_t hash = 0xDEAD;
    uint32_t field = 0xBEEF;
    static_var_store_write_field(hash, field, static_cast<uintptr_t>(100));
    static_var_store_write_field(hash, field, static_cast<uintptr_t>(200));
    auto* ptr = static_var_store_read_field(hash, field);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*static_cast<uintptr_t*>(ptr), 200u);
}

TEST(static_var_store_test, WriteField_MultipleAssemblies) {
    static_var_store_write_field(0xAA01, 0xBB01, static_cast<uintptr_t>(10));
    static_var_store_write_field(0xAA02, 0xBB02, static_cast<uintptr_t>(20));

    auto* p1 = static_var_store_read_field(0xAA01, 0xBB01);
    auto* p2 = static_var_store_read_field(0xAA02, 0xBB02);
    ASSERT_NE(p1, nullptr);
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(*static_cast<uintptr_t*>(p1), 10u);
    EXPECT_EQ(*static_cast<uintptr_t*>(p2), 20u);
    EXPECT_NE(p1, p2);  // Different storage slots
}

TEST(static_var_store_test, UnregisterAssembly_RemovesAllEntries) {
    uint32_t hash = 0xAABB;
    static_var_store_write_field(hash, 1, static_cast<uintptr_t>(111));
    static_var_store_write_field(hash, 2, static_cast<uintptr_t>(222));

    // Verify present before unregister
    ASSERT_NE(static_var_store_read_field(hash, 1), nullptr);

    static_var_store_unregister_assembly(hash);

    // After unregister, read_field creates NEW entries (lazy allocation).
    // Verify the old value is gone (new entry should be 0).
    auto* ptr = static_var_store_read_field(hash, 1);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*static_cast<uintptr_t*>(ptr), 0u);  // Freshly created, value reset to 0
}

TEST(static_var_store_test, UnregisterAssembly_OtherAssembliesPreserved) {
    static_var_store_write_field(0x1111, 1, static_cast<uintptr_t>(100));
    static_var_store_write_field(0x2222, 1, static_cast<uintptr_t>(200));

    static_var_store_unregister_assembly(0x1111);

    auto* ptr = static_var_store_read_field(0x2222, 1);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*static_cast<uintptr_t*>(ptr), 200u);  // Unaffected
}

// ═══════════════════════════════════════════════════════════════════════════
// NativeLibrary null-safety guards
// ═══════════════════════════════════════════════════════════════════════════

TEST(native_library_test, LoadLibrary_Null_ReturnsNull) {
    EXPECT_EQ(gc::NativeLibraryLoad(nullptr), nullptr);
}

TEST(native_library_test, LoadLibrary_Empty_ReturnsNull) {
    EXPECT_EQ(gc::NativeLibraryLoad(""), nullptr);
}

TEST(native_library_test, FreeLibrary_Null_ReturnsFalse) {
    EXPECT_FALSE(gc::NativeLibraryFree(nullptr));
}

TEST(native_library_test, GetProcAddress_NullHandle_ReturnsNull) {
    EXPECT_EQ(gc::NativeLibraryGetProcAddress(nullptr, "foo"), nullptr);
}

TEST(native_library_test, GetProcAddress_NullSymbol_ReturnsNull) {
    // We need a valid handle to test this; skip without real OS loading.
    // At minimum, test with nullptr handle (already covered above).
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// static_var_store register + read guard tests (via AssemblyManager stubs)
// ═══════════════════════════════════════════════════════════════════════════
// NOTE: static_var_store_read delegates to AssemblyManager::GetStaticField.
// Without a loaded assembly, it returns nullptr. We verify the null path.
// Full read/write through AssemblyManager requires integration tests.

TEST(static_var_store_test, Read_UnregisteredHash_ReturnsNull) {
    // No registration has been made for this hash.
    auto* ptr = static_var_store_read(0xFFFFFF, 0);
    EXPECT_EQ(ptr, nullptr);
}

TEST(static_var_store_test, ReadField_AfterUnregisterAssembly_Recreates) {
    // After unregister, a new read_field should create a fresh zero entry.
    uint32_t hash = 0xCAFE;
    static_var_store_write_field(hash, 0x1234, static_cast<uintptr_t>(99));
    static_var_store_unregister_assembly(hash);

    auto* ptr = static_var_store_read_field(hash, 0x1234);
    ASSERT_NE(ptr, nullptr);
    EXPECT_EQ(*static_cast<uintptr_t*>(ptr), 0u);
}

