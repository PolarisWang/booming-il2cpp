// ── JIT debug info tests ──────────────────────────────────────────
//
// Validates:
//   1. JitDebugInfo struct layout (sizeof/offsetof, magic/version constants)
//   2. JitDebugInfo emission in code buffer (scan for kMagic, verify fields)
//   3. MetadataRegistry::GetMethodName resolution
//   4. Debug contract sync (g_chaos_jit_debug_entry_count after compile+register)

#include <gtest/gtest.h>

#include "jit_engine.h"
#include "jit_method.h"
#include "jit_seh.h"

#include "metadata_interface.h"
#include "jit/jit_debug_contract.h"

#include <cstdint>
#include <cstring>

// ── Namespace aliases ──────────────────────────────────────────────
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::jit::Compile;
using chaos::il2cpp::jit::CanCompile;
using chaos::il2cpp::jit::JitMethod;
using chaos::il2cpp::jit::JitDebugInfo;
using chaos::il2cpp::runtime_core::MetadataRegistry;

// ── Test fixture ──────────────────────────────────────────────────

class JitDebugInfoTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        // RegisterJitSehHandler must be called once before using
        // RegisterNativeCodeSection/UnregisterNativeCodeSection.
        // It is idempotent (Initialize is guarded internally).
        chaos::il2cpp::jit::RegisterJitSehHandler();
    }
};

// ═══════════════════════════════════════════════════════════════════
// Test 1: JitDebugInfo struct layout and constants
// ═══════════════════════════════════════════════════════════════════

TEST_F(JitDebugInfoTest, LayoutAndConstants) {
    // Magic/version constants
    EXPECT_EQ(JitDebugInfo::kMagic, 0x544A4442u);
    EXPECT_EQ(JitDebugInfo::kVersion, 2u);

    // Check that the struct is 28 bytes (7 × uint32_t)
    EXPECT_EQ(sizeof(JitDebugInfo), 7u * sizeof(uint32_t));

    // Verify field offsets are sequential (no padding since all uint32_t)
    JitDebugInfo di{};
    auto base = reinterpret_cast<uintptr_t>(&di);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&di.magic) - base, 0u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&di.version) - base, 4u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&di.code_size) - base, 8u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&di.instr_offset_count) - base, 12u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&di.instr_offsets_off) - base, 16u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&di.method_name_off) - base, 20u);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&di.method_name_len) - base, 24u);
}

// ═══════════════════════════════════════════════════════════════════
// Test 2: JitDebugInfo emission in code buffer
// ═══════════════════════════════════════════════════════════════════

static RegisterInstruction MakeInstr(IROpCode opc, uint8_t dst,
                                     int32_t imm, uint8_t flags) noexcept {
    RegisterInstruction ri;
    ri.header = static_cast<uint64_t>(opc) |
                (static_cast<uint64_t>(dst)   << 16) |
                (static_cast<uint64_t>(flags) << 40);
    ri.imm.i4 = imm;
    return ri;
}

static RegisterInstruction MakeRet(uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = static_cast<uint64_t>(IROpCode::Ret) |
                (static_cast<uint64_t>(src) << 24) |
                (static_cast<uint64_t>(kRegHasImm) << 40);
    ri.imm.i4 = 0;
    return ri;
}

TEST_F(JitDebugInfoTest, CodeBufferEmission) {
    RegisterMethod method;
    method.max_regs = 1;
    method.instructions.push_back(MakeInstr(IROpCode::LdcI4, 0, 42, kRegHasDst | kRegHasImm));
    method.instructions.push_back(MakeRet(0));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    ASSERT_NE(nm->code, nullptr);
    ASSERT_GT(nm->code_size, 0u);

    // debug_info_offset should be non-zero for non-tier0 methods
    EXPECT_GT(nm->debug_info_offset, 0u);

    // Scan the code buffer for the JitDebugInfo magic at the expected offset
    auto* code_bytes = static_cast<const uint8_t*>(nm->code);
    uint32_t magic_offset = nm->debug_info_offset +
        offsetof(JitDebugInfo, magic);
    uint32_t magic;
    std::memcpy(&magic, code_bytes + magic_offset, sizeof(magic));
    EXPECT_EQ(magic, JitDebugInfo::kMagic);

    // Read the full JitDebugInfo header
    JitDebugInfo di;
    std::memcpy(&di, code_bytes + nm->debug_info_offset, sizeof(di));
    EXPECT_EQ(di.magic, JitDebugInfo::kMagic);
    EXPECT_EQ(di.version, JitDebugInfo::kVersion);
    // On ARM64, register caching can add internal instructions (spill/reload),
    // making instr_offset_count larger than the source method's instruction count.
    // Use >= so the assertion is valid for both architectures.
    EXPECT_GE(di.instr_offset_count, method.instructions.size());
    EXPECT_LE(di.instr_offset_count, method.instructions.size() + 8u);
    EXPECT_GT(di.code_size, 0u);
    EXPECT_GT(di.instr_offsets_off, 0u);
    EXPECT_GT(di.method_name_off, 0u);
    EXPECT_GT(di.method_name_len, 0u);

    // Read method name from code buffer
    char name_buf[128] = {};
    uint32_t name_len = di.method_name_len;
    if (name_len > sizeof(name_buf) - 1) name_len = sizeof(name_buf) - 1;
    std::memcpy(name_buf, code_bytes + di.method_name_off, name_len);
    name_buf[name_len] = '\0';

    // Method name should match "T4_XXXXXXXX_XXXXXXXX" pattern
    EXPECT_EQ(name_buf[0], 'T');
    EXPECT_EQ(name_buf[1], '4');
    EXPECT_EQ(name_buf[2], '_');
    EXPECT_GT(std::strlen(name_buf), 10u);

    // Verify instr_offsets array matches JitMethod::instr_offsets
    if (nm->instr_offsets != nullptr && nm->instr_offset_count > 0) {
        uint32_t count = di.instr_offset_count;
        if (count > nm->instr_offset_count) {
            count = nm->instr_offset_count;
        }
        for (uint32_t i = 0; i < count; i++) {
            uint32_t embedded_off;
            std::memcpy(&embedded_off,
                        code_bytes + di.instr_offsets_off + i * sizeof(uint32_t),
                        sizeof(embedded_off));
            EXPECT_EQ(embedded_off, nm->instr_offsets[i]);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// Test 3: GetMethodName through MetadataRegistry
// ═══════════════════════════════════════════════════════════════════

TEST_F(JitDebugInfoTest, MetadataRegistryGetMethodName) {
    // Test that MetadataRegistry::GetMethodName handles null/invalid input
    EXPECT_EQ(MetadataRegistry::Get().GetMethodName(0), nullptr);

    // Test with a zero-initialized handle (module_id=0, token=0)
    // Should not crash, returns nullptr since nothing is registered
    EXPECT_EQ(MetadataRegistry::Get().GetMethodName(0xFFFFFFFFFFFFFFFFull),
              nullptr);
}

// ═══════════════════════════════════════════════════════════════════
// Test 4: Debug contract sync
// ═══════════════════════════════════════════════════════════════════

TEST_F(JitDebugInfoTest, DebugContractPointers) {
    // Verify the debug contract extern "C" pointers are non-null
    // (they're statically initialized in jit_debug_contract.cpp)
    EXPECT_NE(g_chaos_jit_debug_entries_ptr, nullptr);
    EXPECT_NE(g_chaos_jit_debug_entry_count_ptr, nullptr);
    EXPECT_GE(g_chaos_jit_debug_max_entries, 0);

    // Entry count should be >= 0
    EXPECT_GE(*static_cast<const uint32_t*>(g_chaos_jit_debug_entry_count_ptr), 0u);
}

TEST_F(JitDebugInfoTest, RegisterSyncsDebugContract) {
    RegisterMethod method;
    method.max_regs = 1;
    method.instructions.push_back(MakeInstr(IROpCode::LdcI4, 0, 99, kRegHasDst | kRegHasImm));
    method.instructions.push_back(MakeRet(0));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);

    uint32_t before_count = *static_cast<const uint32_t*>(
        g_chaos_jit_debug_entry_count_ptr);

    // Register the compiled method — this should sync to the debug contract
    chaos::il2cpp::jit::RegisterNativeCodeSection(
        nm->code, nm->code_size, nm, 0x12345678u);

    uint32_t after_count = *static_cast<const uint32_t*>(
        g_chaos_jit_debug_entry_count_ptr);

    // Entry count must increase by exactly 1
    EXPECT_EQ(after_count, before_count + 1);

    // Unregister to clean up
    chaos::il2cpp::jit::UnregisterNativeCodeSection(nm->code);
}

// ═══════════════════════════════════════════════════════════════════
// Test 5: Multiple registrations increase count correctly
// ═══════════════════════════════════════════════════════════════════

TEST_F(JitDebugInfoTest, MultipleRegistrations) {
    RegisterMethod method1, method2;
    method1.max_regs = 1;
    method1.instructions.push_back(MakeInstr(IROpCode::LdcI4, 0, 1, kRegHasDst | kRegHasImm));
    method1.instructions.push_back(MakeRet(0));

    method2.max_regs = 1;
    method2.instructions.push_back(MakeInstr(IROpCode::LdcI4, 0, 2, kRegHasDst | kRegHasImm));
    method2.instructions.push_back(MakeRet(0));

    ASSERT_TRUE(CanCompile(method1));
    ASSERT_TRUE(CanCompile(method2));
    auto* nm1 = Compile(method1);
    auto* nm2 = Compile(method2);
    ASSERT_NE(nm1, nullptr);
    ASSERT_NE(nm2, nullptr);

    uint32_t before = *static_cast<const uint32_t*>(
        g_chaos_jit_debug_entry_count_ptr);

    chaos::il2cpp::jit::RegisterNativeCodeSection(
        nm1->code, nm1->code_size, nm1, 0x11111111u);
    chaos::il2cpp::jit::RegisterNativeCodeSection(
        nm2->code, nm2->code_size, nm2, 0x22222222u);

    uint32_t after = *static_cast<const uint32_t*>(
        g_chaos_jit_debug_entry_count_ptr);
    EXPECT_EQ(after, before + 2);

    // Cleanup
    chaos::il2cpp::jit::UnregisterNativeCodeSection(nm1->code);
    chaos::il2cpp::jit::UnregisterNativeCodeSection(nm2->code);
}
