// simd_tests.cpp — Native JIT SIMD codegen tests
// Tests the JIT's SIMD support by constructing RegisterMethod with Simd
// opcodes, JIT-compiling via Compile(), and executing to verify results.
//
// Test levels:
//   Level 1 (L1): Compile-only — verify codegen doesn't crash/reject
//   Level 2 (L2): Execute JIT code with known FPR inputs, check results
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#include "ir_reg_alloc.h"
#include "jit_engine.h"
#include "jit_helpers.h"
#include "runtime_core.h"
#include "codegen_bridge.h"

using namespace chaos::il2cpp::interpreter;
using namespace chaos::il2cpp::jit;

// Simd opcode constants (matching jit_tree_node.h canonical values).
enum SimdOp : uint8_t {
    kSimdAdd     = 1,
    kSimdSub     = 2,
    kSimdMul     = 3,
    kSimdAnd     = 4,
    kSimdOr      = 5,
    kSimdXor     = 6,
    kSimdAndNot  = 7,
    kSimdEq      = 8,
    kSimdGt      = 9,
    kSimdShuffle = 10,
    kSimdAbs     = 15,
    kSimdExtract = 19,
    kSimdZero    = 24,
};

// Simd element types.
enum SimdElem : uint8_t {
    kElemI8  = 0,
    kElemI16 = 1,
    kElemI32 = 2,
    kElemI64 = 3,
    kElemF32 = 4,
    kElemF64 = 5,
};

// ── Helper: build a RegisterInstruction ─────────────────────────────

static RegisterInstruction Instr(IROpCode op, uint8_t dst = 0, uint8_t src1 = 0,
                                  uint8_t src2 = 0, uint8_t extra_flags = 0) noexcept {
    RegisterInstruction ri{};
    uint8_t flags = extra_flags;
    if (op != IROpCode::Ret && op != IROpCode::Br) {
        if (dst)  flags |= kRegHasDst;
        if (src1) flags |= kRegHasSrc1;
        if (src2) flags |= kRegHasSrc2;
    }
    if (op == IROpCode::Ret) flags |= kRegHasSrc1;
    ri.header = static_cast<uint64_t>(op);
    ri.header |= static_cast<uint64_t>(dst) << 16;
    ri.header |= static_cast<uint64_t>(src1) << 24;
    ri.header |= static_cast<uint64_t>(src2) << 32;
    ri.header |= static_cast<uint64_t>(flags) << 40;
    return ri;
}

static RegisterInstruction Simd(uint8_t dst, uint8_t src1, uint8_t src2,
                                 SimdOp op, SimdElem elem) noexcept {
    RegisterInstruction ri = Instr(IROpCode::Simd, dst, src1, src2, kRegHasImm);
    ri.imm.i8 = static_cast<int64_t>(op) | (static_cast<int64_t>(elem) << 8);
    return ri;
}

static RegisterInstruction SimdImm(uint8_t dst, uint8_t src1, uint8_t src2,
                                    SimdOp op, SimdElem elem, uint16_t imm) noexcept {
    RegisterInstruction ri = Simd(dst, src1, src2, op, elem);
    ri.imm.i8 |= static_cast<int64_t>(imm) << 16;
    return ri;
}

static RegisterInstruction LdcI4(uint8_t dst, int32_t val) noexcept {
    RegisterInstruction ri = Instr(IROpCode::LdcI4, dst, 0, 0, kRegHasImm);
    ri.imm.i4 = val;
    return ri;
}

static RegisterInstruction LdLoc(uint8_t dst, uint8_t src) noexcept {
    return Instr(IROpCode::LdLoc, dst, src);
}

static RegisterInstruction StLoc(uint8_t dst, uint8_t src) noexcept {
    RegisterInstruction ri = Instr(IROpCode::StLoc, 0, src);
    ri.header |= static_cast<uint64_t>(dst) << 16;  // dst vreg = local index
    return ri;
}

static RegisterInstruction Ret(uint8_t src) noexcept {
    return Instr(IROpCode::Ret, 0, src);
}

// ── CompileConfig defaults (all off for test stability) ─────────────
static CompileConfig TestConfig() noexcept {
    CompileConfig config;
    config.enable_optimizer = false;
    config.enable_deopt = false;
    config.enable_inlining = false;
    config.enable_liveness = false;
    config.enable_safepoint_polls = false;
    config.enable_register_caching = false;
    return config;
}

// ── Compile-only test helper ────────────────────────────────────────
static bool CompileOnly(const char* name, RegisterMethod& rm) noexcept {
    std::printf("[SIMD] %s: ", name);
    CompileConfig config = TestConfig();
    JitMethod* jm = chaos::il2cpp::jit::Compile(rm, config);
    if (!jm) {
        std::printf("FAIL (Compile returned null)\n");
        return false;
    }
    std::printf("OK (code=%p, size=%u)\n", jm->code, jm->code_size);
    return true;
}

// ── Helper: build a RegisterMethod with LdcI4 + Simd + Ret ──────────
// r16 = LdcI4(42), f64 = SimdOp(f64, f65), Ret(r16)
static RegisterMethod MakeSimdMethod(SimdOp op, SimdElem elem,
                                      uint16_t simd_imm = 0) noexcept {
    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 42));
    if (simd_imm != 0)
        rm.instructions.push_back(SimdImm(64, 64, 65, op, elem, simd_imm));
    else
        rm.instructions.push_back(Simd(64, 64, 65, op, elem));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    return rm;
}

// ── Level 1: Compile-only tests for each SimdOp ─────────────────────

static bool TestSimdAddI64() noexcept {
    auto rm = MakeSimdMethod(kSimdAdd, kElemI32);
    return CompileOnly("TestSimdAddI32", rm);
}

static bool TestSimdSub() noexcept {
    auto rm = MakeSimdMethod(kSimdSub, kElemI32);
    return CompileOnly("TestSimdSubI32", rm);
}

static bool TestSimdMul() noexcept {
    auto rm = MakeSimdMethod(kSimdMul, kElemI16);
    return CompileOnly("TestSimdMulI16", rm);
}

static bool TestSimdAnd() noexcept {
    auto rm = MakeSimdMethod(kSimdAnd, kElemI32);
    return CompileOnly("TestSimdAndI32", rm);
}

static bool TestSimdOr() noexcept {
    auto rm = MakeSimdMethod(kSimdOr, kElemI32);
    return CompileOnly("TestSimdOrI32", rm);
}

static bool TestSimdXor() noexcept {
    auto rm = MakeSimdMethod(kSimdXor, kElemI32);
    return CompileOnly("TestSimdXorI32", rm);
}

static bool TestSimdAndNot() noexcept {
    auto rm = MakeSimdMethod(kSimdAndNot, kElemI32);
    return CompileOnly("TestSimdAndNotI32", rm);
}

static bool TestSimdEq() noexcept {
    auto rm = MakeSimdMethod(kSimdEq, kElemI32);
    return CompileOnly("TestSimdEqI32", rm);
}

static bool TestSimdGt() noexcept {
    auto rm = MakeSimdMethod(kSimdGt, kElemI32);
    return CompileOnly("TestSimdGtI32", rm);
}

static bool TestSimdShuffle() noexcept {
    auto rm = MakeSimdMethod(kSimdShuffle, kElemI32, 0xE4);  // identity shuffle
    return CompileOnly("TestSimdShuffleI32", rm);
}

static bool TestSimdAbs() noexcept {
    auto rm = MakeSimdMethod(kSimdAbs, kElemI32);
    return CompileOnly("TestSimdAbsI32", rm);
}

static bool TestSimdZero() noexcept {
    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 42));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdZero, kElemI32));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    return CompileOnly("TestSimdZero", rm);
}

// ── Test all element types for a single opcode (Add) ────────────────
static bool TestSimdAddAllElem() noexcept {
    RegisterMethod rm;

    // Build a single method that tests all element types sequentially.
    // This is inefficient to execute but Compile-only, so fine.
    rm.instructions.push_back(LdcI4(16, 42));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdAdd, kElemI8));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdAdd, kElemI16));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdAdd, kElemI32));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdAdd, kElemI64));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    return CompileOnly("TestSimdAddAllElem", rm);
}

// ── Test multiple opcodes in a single method ────────────────────────
static bool TestSimdMultiOp() noexcept {
    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 42));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdAdd, kElemI32));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdSub, kElemI32));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdXor, kElemI32));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    return CompileOnly("TestSimdMultiOp", rm);
}

// ── FMA compile-only tests ──────────────────────────────────────────
static bool TestSimdFmaNoCrash() noexcept {
    std::printf("[SIMD] TestSimdFmaNoCrash: ");
    CompileConfig config = TestConfig();

    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 42));
    RegisterInstruction fma;
    fma.header = static_cast<uint64_t>(IROpCode::SimdFma);
    fma.header |= static_cast<uint64_t>(64) << 16;  // dst = f64
    fma.header |= static_cast<uint64_t>(64) << 24;  // src1 = f64 (acc)
    fma.header |= static_cast<uint64_t>(65) << 32;  // src2 = f65
    fma.header |= static_cast<uint64_t>(kRegHasDst | kRegHasSrc1 | kRegHasSrc2 | kRegHasSrc3 | kRegHasImm) << 40;
    fma.header |= static_cast<uint64_t>(66) << 48;  // src3 = f66
    fma.imm.i8 = static_cast<int64_t>(0) | (static_cast<int64_t>(2) << 8);  // fma_op=add, elem_type=float
    rm.instructions.push_back(fma);
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;

    JitMethod* jm = chaos::il2cpp::jit::Compile(rm, config);
    if (!jm) {
        std::printf("FAIL (Compile returned null)\n");
        return false;
    }
    std::printf("OK (code=%p, size=%u)\n", jm->code, jm->code_size);
    return true;
}

// ── FMA with double element type ────────────────────────────────────
static bool TestSimdFmaDouble() noexcept {
    std::printf("[SIMD] TestSimdFmaDouble: ");
    CompileConfig config = TestConfig();

    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 42));
    RegisterInstruction fma;
    fma.header = static_cast<uint64_t>(IROpCode::SimdFma);
    fma.header |= static_cast<uint64_t>(64) << 16;  // dst = f64
    fma.header |= static_cast<uint64_t>(64) << 24;  // src1 = f64 (acc)
    fma.header |= static_cast<uint64_t>(65) << 32;  // src2 = f65
    fma.header |= static_cast<uint64_t>(kRegHasDst | kRegHasSrc1 | kRegHasSrc2 | kRegHasSrc3 | kRegHasImm) << 40;
    fma.header |= static_cast<uint64_t>(66) << 48;  // src3 = f66
    fma.imm.i8 = static_cast<int64_t>(0) | (static_cast<int64_t>(5) << 8);  // fma_op=add, elem_type=double
    rm.instructions.push_back(fma);
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;

    JitMethod* jm = chaos::il2cpp::jit::Compile(rm, config);
    if (!jm) {
        std::printf("FAIL (Compile returned null)\n");
        return false;
    }
    std::printf("OK (code=%p, size=%u)\n", jm->code, jm->code_size);
    return true;
}

// ── Test runner ─────────────────────────────────────────────────────

// ── Level 2: Execution tests ───────────────────────────────────────
//
// The JIT entry prologue copies 96 quadwords (768 bytes) from args_buf:
//   args_buf[0..63]  → GPR vregs
//   args_buf[64..95] → FPR vreg lower 64 bits (first 8 bytes of each YMM slot)
// After execution, the GPR result is read from ret_buf[0].

static bool ExecuteSimdCheck(const char* name, RegisterMethod& rm,
                              int32_t fpr64_val, int32_t fpr65_val,
                              int32_t expected) noexcept {
    std::printf("[SIMD-L2] %s: ", name);
    CompileConfig config = TestConfig();

    JitMethod* jm = chaos::il2cpp::jit::Compile(rm, config);
    if (!jm) {
        std::printf("FAIL (Compile returned null)\n");
        return false;
    }

    // Extended args_buf: first 64 qwords = GPR, next 32 qwords = FPR
    uint64_t args_buf[128] = {};  // 1024 bytes
    args_buf[64] = static_cast<uint64_t>(static_cast<int64_t>(fpr64_val));
    args_buf[65] = static_cast<uint64_t>(static_cast<int64_t>(fpr65_val));

    uint64_t ret_buf[2] = {};
    using NativeEntry = void (*)(void*, void*);
    auto native_entry = reinterpret_cast<NativeEntry>(jm->code);
    native_entry(args_buf, ret_buf);

    int32_t result = static_cast<int32_t>(ret_buf[0]);
    if (result != expected) {
        std::printf("FAIL (got %d, expected %d)\n", result, expected);
        return false;
    }
    std::printf("OK\n");
    return true;
}

static bool TestSimdL2_AddI32() noexcept {
    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 0));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdAdd, kElemI32));
    rm.instructions.push_back(SimdImm(16, 64, 0, kSimdExtract, kElemI32, 0));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    return ExecuteSimdCheck("AddI32 [5+3]", rm, 5, 3, 8);
}

static bool TestSimdL2_SubI32() noexcept {
    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 0));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdSub, kElemI32));
    rm.instructions.push_back(SimdImm(16, 64, 0, kSimdExtract, kElemI32, 0));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    return ExecuteSimdCheck("SubI32 [10-3]", rm, 10, 3, 7);
}

static bool TestSimdL2_MulI16() noexcept {
    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 0));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdMul, kElemI16));
    rm.instructions.push_back(SimdImm(16, 64, 0, kSimdExtract, kElemI16, 0));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    return ExecuteSimdCheck("MulI16 [100*3]", rm, 100, 3, 300);
}

static bool TestSimdL2_Bitwise() noexcept {
    bool ok = true;
    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 0));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdAnd, kElemI32));
    rm.instructions.push_back(SimdImm(16, 64, 0, kSimdExtract, kElemI32, 0));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    if (!ExecuteSimdCheck("AndI32 [0xFF00FF & 0xFFFF]", rm, 0xFF00FF, 0xFFFF, 0xFF)) ok = false;

    rm.instructions[1] = Simd(64, 64, 65, kSimdOr, kElemI32);
    if (!ExecuteSimdCheck("OrI32 [0xFF00 | 0x00FF]", rm, 0xFF00, 0x00FF, 0xFFFF)) ok = false;

    rm.instructions[1] = Simd(64, 64, 65, kSimdXor, kElemI32);
    if (!ExecuteSimdCheck("XorI32 [0xFFFF ^ 0xFF00]", rm, 0xFFFF, 0xFF00, 0x00FF)) ok = false;

    return ok;
}

static bool TestSimdL2_EqGtI32() noexcept {
    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 0));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdEq, kElemI32));
    rm.instructions.push_back(SimdImm(16, 64, 0, kSimdExtract, kElemI32, 0));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    if (!ExecuteSimdCheck("EqI32 [42==42]", rm, 42, 42, -1)) return false;

    rm.instructions[1] = Simd(64, 64, 65, kSimdGt, kElemI32);
    if (!ExecuteSimdCheck("GtI32 [10>5]", rm, 10, 5, -1)) return false;

    return true;
}

static bool TestSimdL2_AbsI32() noexcept {
    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 0));
    rm.instructions.push_back(Simd(64, 64, 0, kSimdAbs, kElemI32));
    rm.instructions.push_back(SimdImm(16, 64, 0, kSimdExtract, kElemI32, 0));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    return ExecuteSimdCheck("AbsI32 [|-42|]", rm, -42, 0, 42);
}

static bool TestSimdL2_ZeroI32() noexcept {
    RegisterMethod rm;
    rm.instructions.push_back(LdcI4(16, 0));
    rm.instructions.push_back(Simd(64, 64, 65, kSimdZero, kElemI32));
    rm.instructions.push_back(SimdImm(16, 64, 0, kSimdExtract, kElemI32, 0));
    rm.instructions.push_back(Ret(16));
    rm.max_regs = 96;
    // Zero ignores src; dst should be all zeros regardless of input
    return ExecuteSimdCheck("ZeroI32 [0]", rm, 42, 0, 0);
}

static bool RunOneTest(const char* name, bool (*test_fn)(), int& passed, int& total) noexcept {
    ++total;
    std::printf("[BEGIN] %s ... ", name);
    std::fflush(stdout);
    bool ok = test_fn();
    std::printf(ok ? "PASS\n" : "FAIL\n");
    std::fflush(stdout);
    if (ok) ++passed;
    return ok;
}
#define RUN_TEST(name) RunOneTest(#name, name, passed, total)

extern "C" void RunSimdTests() {
    std::printf("=== SIMD JIT Codegen Tests ===\n");
    int passed = 0, total = 0;

    // Arithmetic
    RUN_TEST(TestSimdAddI64);
    RUN_TEST(TestSimdSub);
    RUN_TEST(TestSimdMul);

    // Bitwise
    RUN_TEST(TestSimdAnd);
    RUN_TEST(TestSimdOr);
    RUN_TEST(TestSimdXor);
    RUN_TEST(TestSimdAndNot);

    // Compare
    RUN_TEST(TestSimdEq);
    RUN_TEST(TestSimdGt);

    // Shuffle / Abs / Zero
    RUN_TEST(TestSimdShuffle);
    RUN_TEST(TestSimdAbs);
    RUN_TEST(TestSimdZero);

    // Multi-op and mixed element types
    RUN_TEST(TestSimdAddAllElem);
    RUN_TEST(TestSimdMultiOp);

    // FMA
    RUN_TEST(TestSimdFmaNoCrash);
    RUN_TEST(TestSimdFmaDouble);

    // Level 2: Execution tests
    std::printf("--- L2 Execution ---\n");
    RUN_TEST(TestSimdL2_AddI32);
    RUN_TEST(TestSimdL2_SubI32);
    RUN_TEST(TestSimdL2_MulI16);
    RUN_TEST(TestSimdL2_Bitwise);
    RUN_TEST(TestSimdL2_EqGtI32);
    RUN_TEST(TestSimdL2_AbsI32);
    RUN_TEST(TestSimdL2_ZeroI32);

    std::printf("Passed: %d/%d\n", passed, total);
    std::fflush(stdout);
}
