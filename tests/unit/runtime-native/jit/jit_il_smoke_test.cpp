// ── Codegen IL smoke tests ─────────────────────────────────────────
//
// Tests the codegen module's ability to process basic IL instruction
// sequences and generate native code.  Complements codegen_native_test.cpp
// by covering IL patterns that the existing test does not exercise.
//
// These tests validate:
//   1. IL instruction -> RegisterInstruction lowering
//   2. Basic opcode code generation paths
//   3. Edge cases in IR instruction handling

#include <gtest/gtest.h>

#include "jit_engine.h"
#include "jit_method.h"
#include "jit_helpers.h"
#include "ir_reg_alloc.h"

#if defined(_WIN32)
#include <windows.h>
#include <cstdint>

#include <codegen_bridge.h>
#else
#include <sys/mman.h>
#include <cstdint>

#include <codegen_bridge.h>
#endif

#include <gc_scheduler.h>
#include <gc/gc_young_gen.h>
#if defined(_WIN64)
// T4 entries use RBX=args/RSI=ret (JIT conv); t4_jit_call.asm re-binds them.
extern "C" void T4CallNative(const void* entry, void* args, void* ret);
#endif


// ── Namespace aliases ──────────────────────────────────────────────
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasSrc1;
using chaos::il2cpp::interpreter::kRegHasSrc2;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::interpreter::kRegIsBranch;
using chaos::il2cpp::interpreter::kRegIsStore;
using chaos::il2cpp::jit::Compile;
using chaos::il2cpp::jit::CanCompile;
using chaos::il2cpp::jit::JitMethod;
using chaos::il2cpp::jit::CompileConfig;

using chaos::il2cpp::runtime_core::TLAB;
using chaos::il2cpp::runtime_core::tls_tlab;

// ── TLAB priming helper ───────────────────────────────────────────
// Ensures the current thread has a valid TLAB so that generated code
// can access tls_tlab without dereferencing null pointers.
static void PrimeTlab() noexcept {
    static bool primed = false;
    if (!primed) {
        static constexpr size_t kTlabSize = 64 * 1024;  // 64 KB
#if defined(_WIN32)
        void* buf = VirtualAlloc(nullptr, kTlabSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
        void* buf = mmap(nullptr, kTlabSize, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
        if (buf) {
            tls_tlab.start = static_cast<char*>(buf);
            tls_tlab.current = static_cast<char*>(buf);
            tls_tlab.end = static_cast<char*>(buf) + kTlabSize;
            tls_tlab.start_scan = static_cast<char*>(buf);
            tls_tlab.current_scan = static_cast<char*>(buf);
        }
        primed = true;
    }
}

// ── Test fixture ──────────────────────────────────────────────────
struct CodegenIlSmokeTest : public ::testing::Test {
    static void SetUpTestSuite() {
        PrimeTlab();
    }
};

// ── Helpers (matching codegen_native_test.cpp pattern) ────────────

// Generated code uses (args, ret) two-pointer ABI.
static uint64_t ExecuteNative(void* entry) {
    uint64_t args_buf[8] = {};
    uint64_t ret_buf[2] = {};
#if defined(_WIN64)
    T4CallNative(entry, args_buf, ret_buf);
#else
    using NativeEntry = void (*)(void*, void*);
    auto native_entry = reinterpret_cast<NativeEntry>(entry);
    native_entry(args_buf, ret_buf);
#endif
    return ret_buf[0];
}

static void* SealAndGetEntry(JitMethod* nm) {
    if (nm == nullptr || nm->code == nullptr) return nullptr;
    return nm->code;
}

static uint64_t MakeHeader(IROpCode opc, uint8_t dst, uint8_t src1,
                           uint8_t src2, uint8_t flags) noexcept {
    return static_cast<uint64_t>(opc) |
           (static_cast<uint64_t>(dst)   << 16) |
           (static_cast<uint64_t>(src1)  << 24) |
           (static_cast<uint64_t>(src2)  << 32) |
           (static_cast<uint64_t>(flags) << 40);
}

static RegisterInstruction InstrI4(IROpCode opc, int32_t imm,
                                   uint8_t dst = 0, uint8_t flags = 0) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, dst, 0, 0, flags | kRegHasDst | kRegHasImm);
    ri.imm.i4 = imm;
    return ri;
}

static RegisterInstruction InstrBinary(IROpCode opc, uint8_t dst,
                                       uint8_t src1, uint8_t src2) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, dst, src1, src2,
                           kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrRet(uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Ret, 0, src, 0, kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrCondBranch(IROpCode opc, uint32_t target,
                                           uint8_t src1, uint8_t src2) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, 0, src1, src2,
                           kRegIsBranch | kRegHasImm | kRegHasSrc1 | kRegHasSrc2);
    ri.imm.branch_target = target;
    return ri;
}

// ── IL Smoke Tests ─────────────────────────────────────────────────

TEST_F(CodegenIlSmokeTest, LdcI4_Ret) {
    RegisterMethod method;
    method.max_regs = 1;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 42, 0, 0));
    method.instructions.push_back(InstrRet(0));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 42ULL);
}

TEST_F(CodegenIlSmokeTest, LdcI4_MaxValue) {
    RegisterMethod method;
    method.max_regs = 1;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, INT32_MAX, 0, 0));
    method.instructions.push_back(InstrRet(0));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), static_cast<uint64_t>(INT32_MAX));
}

TEST_F(CodegenIlSmokeTest, LdcI4_MinValue) {
    RegisterMethod method;
    method.max_regs = 1;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, INT32_MIN, 0, 0));
    method.instructions.push_back(InstrRet(0));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(static_cast<int32_t>(ExecuteNative(entry)), INT32_MIN);
}

TEST_F(CodegenIlSmokeTest, Add_Simple) {
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 10, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 20, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Add, 2, 0, 1));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(static_cast<int32_t>(ExecuteNative(entry)), 30);
}

TEST_F(CodegenIlSmokeTest, Add_NegativeValues) {
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, -50, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, -30, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Add, 2, 0, 1));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(static_cast<int32_t>(ExecuteNative(entry)), -80);
}

TEST_F(CodegenIlSmokeTest, Sub_NegativeResult) {
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 10, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 100, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Sub, 2, 0, 1));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(static_cast<int32_t>(ExecuteNative(entry)), -90);
}

TEST_F(CodegenIlSmokeTest, Mul_Zero) {
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 42, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 0, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Mul, 2, 0, 1));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 0ULL);
}

TEST_F(CodegenIlSmokeTest, Mul_ByOne) {
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 99, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 1, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Mul, 2, 0, 1));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 99ULL);
}

TEST_F(CodegenIlSmokeTest, BitwiseAnd) {
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 0xFF, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 0x0F, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::And, 2, 0, 1));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 0x0FULL);
}

TEST_F(CodegenIlSmokeTest, BitwiseOr) {
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 0xF0, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 0x0F, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Or, 2, 0, 1));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 0xFFULL);
}

TEST_F(CodegenIlSmokeTest, ShiftLeft) {
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 1, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 8, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Shl, 2, 0, 1));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 256ULL);
}

TEST_F(CodegenIlSmokeTest, Branch_LessThan) {
    // if (10 < 20) return 1; else return 0
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 10, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 20, 1, 0));
    method.instructions.push_back(InstrCondBranch(IROpCode::Bge, 6, 0, 1));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 1, 2, 0));
    method.instructions.push_back(InstrRet(2));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 0, 2, 0));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 1ULL);
}

TEST_F(CodegenIlSmokeTest, Branch_Equal) {
    // if (42 == 42) return 1; else return 0
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 42, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 42, 1, 0));
    method.instructions.push_back(InstrCondBranch(IROpCode::BneUn, 6, 0, 1));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 1, 2, 0));
    method.instructions.push_back(InstrRet(2));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 0, 2, 0));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 1ULL);
}

TEST_F(CodegenIlSmokeTest, Branch_NotEqual) {
    // if (10 != 20) return 1; else return 0
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 10, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 20, 1, 0));
    method.instructions.push_back(InstrCondBranch(IROpCode::Beq, 6, 0, 1));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 1, 2, 0));
    method.instructions.push_back(InstrRet(2));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 0, 2, 0));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 1ULL);
}

TEST_F(CodegenIlSmokeTest, RemOperation) {
    RegisterMethod method;
    method.max_regs = 3;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 100, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 30, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Rem, 2, 0, 1));
    method.instructions.push_back(InstrRet(2));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 10ULL);
}

TEST_F(CodegenIlSmokeTest, MultiInstrChain) {
    // (10 + 20) * 3 - 5 = 85
    RegisterMethod method;
    method.max_regs = 7;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 10, 0, 0));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 20, 1, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Add, 2, 0, 1));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 3, 3, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Mul, 4, 2, 3));
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, 5, 5, 0));
    method.instructions.push_back(InstrBinary(IROpCode::Sub, 6, 4, 5));
    method.instructions.push_back(InstrRet(6));

    ASSERT_TRUE(CanCompile(method));
    auto* nm = Compile(method);
    ASSERT_NE(nm, nullptr);
    void* entry = SealAndGetEntry(nm);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(ExecuteNative(entry), 85ULL);
}
