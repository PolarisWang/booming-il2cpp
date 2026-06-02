/// ARM64 JIT code execution test
/// Tests that the JIT compiler generates valid ARM64 code that executes
/// correctly, producing the expected return values.
///
/// Unlike the x64 jit_native_test.cpp (which uses VirtualAlloc, SEH,
/// and Windows-specific APIs), this test is fully portable and runs
/// under qemu-aarch64 user-mode emulation.
///
/// Calling convention note:
/// The JIT engine stores return values to [kRetBuf + 0] where kRetBuf = X4.
/// On ARM64, kRetBuf is set to SP + kGprFileOff (SP + 0) in the prologue.
/// After the function returns, X4 still holds this address. We read it via
/// inline assembly to retrieve the result.

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cinttypes>

#include "jit_engine.h"
#include "jit_method.h"
#include "jit_helpers.h"
#include "IEncoder.h"
#include "Arm64Encoder.h"

// IR types needed for building RegisterMethod
#include "../interpreter/ir_reg_alloc.h"

using namespace chaos::il2cpp::interpreter;
using namespace chaos::il2cpp::jit;

static int g_failures = 0;
static int g_tests = 0;

#define CHECK(cond, ...) do {                                                   \
    ++g_tests;                                                                  \
    if (!(cond)) {                                                              \
        printf("  FAIL [%s:%d]: ", __FILE__, __LINE__);                         \
        printf(__VA_ARGS__);                                                    \
        printf("\n");                                                           \
        ++g_failures;                                                           \
    } else {                                                                    \
        printf("  PASS: ");                                                     \
        printf(__VA_ARGS__);                                                    \
        printf("\n");                                                           \
    }                                                                           \
} while(0)

// ── Hex dump for JIT code debugging ─────────────────────────────────────
static void DumpCode(const void* code, uint32_t size) noexcept {
    printf("    code bytes (%u):\n", size);
    const uint8_t* p = static_cast<const uint8_t*>(code);
    for (uint32_t i = 0; i < size && i < 512; i += 4) {
        if (i + 4 <= size) {
            uint32_t w_le = (uint32_t)p[i] | ((uint32_t)p[i+1] << 8) | ((uint32_t)p[i+2] << 16) | ((uint32_t)p[i+3] << 24);
            printf("    %3u: %02x %02x %02x %02x  (LE=%08x)\n", i, p[i], p[i+1], p[i+2], p[i+3], w_le);
        }
    }
}

// ── IR instruction helpers (same as jit_native_test.cpp) ────────────────
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

static RegisterInstruction InstrR8(IROpCode opc, double val,
                                   uint8_t dst = 0, uint8_t flags = 0) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, dst, 0, 0, flags | kRegHasDst | kRegHasImm);
    std::memcpy(&ri.imm.r8, &val, sizeof(val));
    return ri;
}

static RegisterInstruction InstrRet(uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Ret, 0, src, 0, kRegHasSrc1);
    ri.imm.i4 = 0;
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

static RegisterInstruction InstrDup(uint8_t dst, uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Dup, dst, src, 0,
                           kRegHasDst | kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrLdLoc(uint8_t dst, uint32_t local_idx) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::LdLoc, dst, 0, 0,
                           kRegHasDst | kRegHasImm);
    ri.imm.operand_index = local_idx;
    return ri;
}

static RegisterInstruction InstrLdLocA(uint8_t dst, uint32_t local_idx) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::LdLocA, dst, 0, 0,
                           kRegHasDst | kRegHasImm);
    ri.imm.operand_index = local_idx;
    return ri;
}

static RegisterInstruction InstrStInd(uint8_t addr_reg, uint8_t val_reg) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::StInd, 0, addr_reg, val_reg,
                           kRegHasSrc1 | kRegHasSrc2);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrLdInd(uint8_t addr_reg, uint8_t dst) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::LdInd, dst, addr_reg, 0,
                           kRegHasDst | kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrStLoc(uint32_t local_idx, uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::StLoc, 0, src, 0,
                           kRegHasSrc1 | kRegHasImm);
    ri.imm.operand_index = local_idx;
    return ri;
}

// ── Branch helpers ────────────────────────────────────────────────
static RegisterInstruction InstrBr(uint32_t target) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Br, 0, 0, 0,
                           kRegIsBranch | kRegHasImm);
    ri.imm.branch_target = target;
    return ri;
}

static RegisterInstruction InstrBrTrue(uint8_t src, uint32_t target) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::BrTrue, 0, src, 0,
                           kRegHasSrc1 | kRegIsBranch | kRegHasImm);
    ri.imm.branch_target = target;
    return ri;
}

static RegisterInstruction InstrBrFalse(uint8_t src, uint32_t target) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::BrFalse, 0, src, 0,
                           kRegHasSrc1 | kRegIsBranch | kRegHasImm);
    ri.imm.branch_target = target;
    return ri;
}

// ── Comparison branch helpers (src1, src2, target) ─────────────────────
static RegisterInstruction InstrBeq(uint8_t s1, uint8_t s2, uint32_t t) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Beq, 0, s1, s2,
                           kRegHasSrc1 | kRegHasSrc2 | kRegIsBranch | kRegHasImm);
    ri.imm.branch_target = t; return ri;
}
static RegisterInstruction InstrBne(uint8_t s1, uint8_t s2, uint32_t t) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::BneUn, 0, s1, s2,
                           kRegHasSrc1 | kRegHasSrc2 | kRegIsBranch | kRegHasImm);
    ri.imm.branch_target = t; return ri;
}
static RegisterInstruction InstrBlt(uint8_t s1, uint8_t s2, uint32_t t) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Blt, 0, s1, s2,
                           kRegHasSrc1 | kRegHasSrc2 | kRegIsBranch | kRegHasImm);
    ri.imm.branch_target = t; return ri;
}
static RegisterInstruction InstrBgt(uint8_t s1, uint8_t s2, uint32_t t) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Bgt, 0, s1, s2,
                           kRegHasSrc1 | kRegHasSrc2 | kRegIsBranch | kRegHasImm);
    ri.imm.branch_target = t; return ri;
}
static RegisterInstruction InstrBle(uint8_t s1, uint8_t s2, uint32_t t) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Ble, 0, s1, s2,
                           kRegHasSrc1 | kRegHasSrc2 | kRegIsBranch | kRegHasImm);
    ri.imm.branch_target = t; return ri;
}
static RegisterInstruction InstrBge(uint8_t s1, uint8_t s2, uint32_t t) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Bge, 0, s1, s2,
                           kRegHasSrc1 | kRegHasSrc2 | kRegIsBranch | kRegHasImm);
    ri.imm.branch_target = t; return ri;
}

// ── Call instruction builder ───────────────────────────────────────────
static RegisterInstruction InstrCall(void* target, uint8_t dst,
                                     uint8_t first_arg, uint32_t arg_count) noexcept {
    RegisterInstruction ri;
    uint8_t flags = kRegHasSrc1 | kRegHasImm | kRegIsCall;
    if (dst != 0xFF) flags |= kRegHasDst;
    ri.header = MakeHeader(IROpCode::Call, dst, first_arg, 0, flags) |
                (static_cast<uint64_t>(arg_count) << 48);
    ri.imm.ptr = target;
    return ri;
}

static JitMethod* CompileCall(RegisterMethod& rm) noexcept {
    CompileConfig cfg;
    cfg.enable_optimizer = false;
    cfg.enable_register_caching = false;
    cfg.enable_inlining = false;
    return Compile(rm, cfg);
}
extern "C" uint64_t Helper_Return42() noexcept { return 42; }

// NOTE: The JIT's managed call passes args in scratch registers (X1, X2, X8, X9
// on ARM64) rather than the platform ABI registers (X0-X7). These helpers read
// from scratch registers to match the JIT convention.
#if defined(__aarch64__)
extern "C" uint64_t Helper_Identity() noexcept {
    uint64_t arg;
    __asm__ volatile("mov %0, x1" : "=r"(arg));
    return arg;
}
extern "C" uint64_t Helper_Add() noexcept {
    uint64_t a, b;
    __asm__ volatile("mov %0, x1" : "=r"(a));
    __asm__ volatile("mov %0, x2" : "=r"(b));
    return a + b;
}
#endif

// ── FPR compilation wrapper (disables optimizer + register caching) ────
// The optimizer's dead-code elimination removes LdcR4/LdcR8/Conv_R4/Conv_R8
// when their FPR destination (vreg 64+) is never read by a subsequent IR
// instruction, because use_count only tracks GPRs (vreg < 64).
// Register caching (graph coloring) for FPRs skips the stack write in
// StoreFpr, but ExecuteNativeFpr reads from the stack — so we disable
// both optimization and register caching for FPR tests.
static JitMethod* CompileFpr(RegisterMethod& rm) noexcept {
    CompileConfig cfg;
    cfg.enable_optimizer = false;
    cfg.enable_register_caching = false;
    return Compile(rm, cfg);
}

// ── Execute native code and read return value ──────────────────────────
// On ARM64, the JIT stores the return value to [kRetBuf + 0] where
// kRetBuf = X4 = SP + kGprFileOff = SP (bottom of frame).
// After the JIT function returns and SP is restored, X4 still holds this
// address, and the result value is still at [X4] in the freed stack memory.
// We use a single inline-asm block to: set up null args, BLR to the JIT
// entry, and immediately LDR the result from [X4] — all before any
// compiler-generated code can clobber X4 or the stack memory.

// ARM64 frame layout constants (must match jit_engine.cpp):
//   kShadowSize = 0, kGprFileOff = 0, kFprSlotSize = 16
static constexpr uint32_t kFprFileOff = kGPRegisters * 8;  // 64 * 8 = 512

/// Execute JIT-compiled function and read GPR return value from [X4 + 0].
static uint64_t ExecuteNative(void* entry, uint64_t* args = nullptr) noexcept {
    uint64_t args_buf[8] = {};
    if (args) std::memcpy(args_buf, args, 8 * sizeof(uint64_t));
    std::printf("    calling entry=%p\n", entry);
    uint64_t result = 0;
    __asm__ volatile(
        "mov x0, %[args]\n\t"
        "mov x1, x0\n\t"
        "blr %[entry]\n\t"
        "ldr %[res], [x4]\n\t"
        : [res] "=r"(result)
        : [entry] "r"(entry), [args] "r"(args_buf)
        : "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9",
          "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x30",
          "memory", "cc"
    );
    std::printf("    result=%" PRIu64 " (0x%" PRIx64 ")\n", result, result);
    return result;
}

/// Execute JIT-compiled function and read FPR return value.
/// @param fpr_vreg  FPR virtual register number (64..95).
///                  64-bit double value is read from the lower 64 bits of the
///                  NEON Q register slot at [X4 + kFprFileOff + (vreg-64)*16].
static uint64_t ExecuteNativeFpr(void* entry, uint32_t fpr_vreg,
                                 uint64_t* args = nullptr) noexcept {
    uint64_t args_buf[8] = {};
    if (args) std::memcpy(args_buf, args, 8 * sizeof(uint64_t));
    std::printf("    calling entry=%p, fpr_vreg=%u\n", entry, fpr_vreg);
    uint64_t result = 0;
    uint64_t ret_offset = kFprFileOff + (fpr_vreg - kGPRegisters) * 16;
    __asm__ volatile(
        "mov x0, %[args]\n\t"
        "mov x1, x0\n\t"
        "blr %[entry]\n\t"
        "ldr %[res], [x4, %[off]]\n\t"
        : [res] "=r"(result)
        : [entry] "r"(entry), [args] "r"(args_buf), [off] "r"(ret_offset)
        : "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9",
          "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x30",
          "memory", "cc"
    );
    double dval;
    std::memcpy(&dval, &result, sizeof(dval));
    std::printf("    result_bits=0x%016" PRIx64 " (double: %.20g)\n", result, dval);
    return result;
}

// ── Test: LdcI4 42 → Ret ──────────────────────────────────────────────
static void Test_LdcI4_Ret() {
    printf("\n── Test: LdcI4_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = { InstrI4(IROpCode::LdcI4, 42, 0), InstrRet(0) };
    rm.max_regs = 1;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    printf("    nm->code=%p nm->code_size=%u\n", nm->code, nm->code_size);
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 42, "LdcI4_Ret returned %" PRIu64 ", expected 42", result);
}

// ── Test: Add_Ret ─────────────────────────────────────────────────────
static void Test_Add_Ret() {
    printf("\n── Test: Add_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 12, 0),
        InstrI4(IROpCode::LdcI4, 30, 1),
        InstrBinary(IROpCode::Add, 2, 0, 1),
        InstrRet(2)
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 42, "Add_Ret returned %" PRIu64 ", expected 42", result);
}

// ── Test: Arithmetic chain (10+20)*3-5 = 85 ───────────────────────────
static void Test_ArithmeticChain() {
    printf("\n── Test: ArithmeticChain ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 10, 0),
        InstrI4(IROpCode::LdcI4, 20, 1),
        InstrBinary(IROpCode::Add, 2, 0, 1),
        InstrI4(IROpCode::LdcI4, 3, 3),
        InstrBinary(IROpCode::Mul, 4, 2, 3),
        InstrI4(IROpCode::LdcI4, 5, 5),
        InstrBinary(IROpCode::Sub, 6, 4, 5),
        InstrRet(6),
    };
    rm.max_regs = 7;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 85, "ArithmeticChain returned %" PRIu64 ", expected 85", result);
}

// ── Test: Sub_Ret ─────────────────────────────────────────────────────
static void Test_Sub_Ret() {
    printf("\n── Test: Sub_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 100, 0),
        InstrI4(IROpCode::LdcI4, 58, 1),
        InstrBinary(IROpCode::Sub, 2, 0, 1),
        InstrRet(2)
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 42, "Sub_Ret returned %" PRIu64 ", expected 42", result);
}

// ── Test: Mul_Ret ─────────────────────────────────────────────────────
static void Test_Mul_Ret() {
    printf("\n── Test: Mul_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 6, 0),
        InstrI4(IROpCode::LdcI4, 7, 1),
        InstrBinary(IROpCode::Mul, 2, 0, 1),
        InstrRet(2)
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 42, "Mul_Ret returned %" PRIu64 ", expected 42", result);
}

// ── Test: Local variable load/store ───────────────────────────────────
static void Test_LdLocStLoc() {
    printf("\n── Test: LdLocStLoc ──\n");
    RegisterMethod rm;
    // LdLoc/StLoc with local index 0 — RegisterMethod locals storage
    // is implicitly sized by max_regs + num_locals hint from IR.
    // We set max_regs=2 and emit instructions that use local slot 0.
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrStLoc(0, 0),
        InstrLdLoc(1, 0),
        InstrRet(1),
    };
    rm.max_regs = 12;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 42, "LdLocStLoc returned %" PRIu64 ", expected 42", result);
}

// ── Test: Multiple constant folding ───────────────────────────────────
static void Test_ConstFolding() {
    printf("\n── Test: ConstFolding ──\n");
    RegisterMethod rm;
    // (10 + 20) * 2 = 60
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 10, 0),
        InstrI4(IROpCode::LdcI4, 20, 1),
        InstrBinary(IROpCode::Add, 2, 0, 1),
        InstrI4(IROpCode::LdcI4, 2, 3),
        InstrBinary(IROpCode::Mul, 4, 2, 3),
        InstrRet(4),
    };
    rm.max_regs = 5;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 60, "ConstFolding returned %" PRIu64 ", expected 60", result);
}

// ── Test: Large immediate values ──────────────────────────────────────
static void Test_LargeImmediates() {
    printf("\n── Test: LargeImmediates ──\n");
    RegisterMethod rm;
    // Load 0x12345678 and return it (tests >12-bit immediate decomposition)
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, static_cast<int32_t>(0x12345678), 0),
        InstrRet(0),
    };
    rm.max_regs = 1;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    std::printf("    entry=%p code_size=%u\n", entry, nm->code_size);
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 0x12345678, "LargeImmediates returned %" PRIu64 ", expected 0x12345678", result);
}

// ── Test: CodeBuffer Seal and ownership transfer ──────────────────────
static void Test_Seal() {
    printf("\n── Test: Seal ──\n");
    RegisterMethod rm;
    rm.instructions = { InstrI4(IROpCode::LdcI4, 99, 0), InstrRet(0) };
    rm.max_regs = 1;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    CHECK(nm->code_size != 0, "Code size = %u", nm->code_size);
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 99, "Seal returned %" PRIu64 ", expected 99", result);
}

// ── Test: Unconditional branch Br bypass ────────────────────────────
static void Test_Br_Bypass() {
    printf("\n── Test: Br_Bypass ──\n");
    RegisterMethod rm;
    // Br skips the Ret v0, jumps to LdcI4 42 → Ret
    // [0] LdcI4 0 → v0
    // [1] Br → 3
    // [2] Ret v0          (skipped)
    // [3] LdcI4 42 → v0
    // [4] Ret v0
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrBr(3),
        InstrRet(0),
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrRet(0),
    };
    rm.max_regs = 1;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 42, "Br_Bypass returned %" PRIu64 ", expected 42", result);
}

// ── Test: BrTrue with true condition (branch taken) ─────────────────
static void Test_BrTrue_Taken() {
    printf("\n── Test: BrTrue_Taken ──\n");
    RegisterMethod rm;
    // [0] LdcI4 1 → v0    (true)
    // [1] BrTrue v0 → 4
    // [2] LdcI4 0 → v1
    // [3] Ret v1           (would return 0 — not reached)
    // [4] LdcI4 42 → v2
    // [5] Ret v2
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 1, 0),
        InstrBrTrue(0, 4),
        InstrI4(IROpCode::LdcI4, 0, 1),
        InstrRet(1),
        InstrI4(IROpCode::LdcI4, 42, 2),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 42, "BrTrue_Taken returned %" PRIu64 ", expected 42", result);
}

// ── Test: BrTrue with false condition (fall through) ────────────────
static void Test_BrTrue_NotTaken() {
    printf("\n── Test: BrTrue_NotTaken ──\n");
    RegisterMethod rm;
    // [0] LdcI4 0 → v0    (false)
    // [1] BrTrue v0 → 4   not taken → fall through
    // [2] LdcI4 42 → v1
    // [3] Ret v1
    // [4] LdcI4 0 → v2    (would return 0 — not reached)
    // [5] Ret v2
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrBrTrue(0, 4),
        InstrI4(IROpCode::LdcI4, 42, 1),
        InstrRet(1),
        InstrI4(IROpCode::LdcI4, 0, 2),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 42, "BrTrue_NotTaken returned %" PRIu64 ", expected 42", result);
}

// ── Test: BrFalse with false condition (branch taken) ───────────────
static void Test_BrFalse_Taken() {
    printf("\n── Test: BrFalse_Taken ──\n");
    RegisterMethod rm;
    // [0] LdcI4 0 → v0    (false)
    // [1] BrFalse v0 → 4
    // [2] LdcI4 0 → v1
    // [3] Ret v1           (would return 0 — not reached)
    // [4] LdcI4 42 → v2
    // [5] Ret v2
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrBrFalse(0, 4),
        InstrI4(IROpCode::LdcI4, 0, 1),
        InstrRet(1),
        InstrI4(IROpCode::LdcI4, 42, 2),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm);
    if (nm == nullptr) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    void* entry = nm->code;
    uint64_t result = ExecuteNative(entry);
    CHECK(result == 42, "BrFalse_Taken returned %" PRIu64 ", expected 42", result);
}

// ── Test: Beq with equal values (branch taken) ──────────────────────────
static void Test_Beq_Taken() {
    printf("\n── Test: Beq_Taken ──\n");
    RegisterMethod rm;
    // [0] LdcI4 42→v0 [1] LdcI4 42→v1 [2] Beq v0,v1→4 [3] Ret v0(shouldn't)
    // [4] LdcI4 42→v2 [5] Ret v2
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0), InstrI4(IROpCode::LdcI4, 42, 1),
        InstrBeq(0, 1, 4), InstrRet(0),
        InstrI4(IROpCode::LdcI4, 42, 2), InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Beq_Taken");
}
static void Test_Beq_NotTaken() {
    printf("\n── Test: Beq_NotTaken ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0), InstrI4(IROpCode::LdcI4, 43, 1),
        InstrBeq(0, 1, 5), InstrI4(IROpCode::LdcI4, 42, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 0, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Beq_NotTaken");
}

// ── Test: Bne (not equal) ──────────────────────────────────────────────
static void Test_Bne_Taken() {
    printf("\n── Test: Bne_Taken ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0), InstrI4(IROpCode::LdcI4, 43, 1),
        InstrBne(0, 1, 5), InstrI4(IROpCode::LdcI4, 0, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 42, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Bne_Taken");
}
static void Test_Bne_NotTaken() {
    printf("\n── Test: Bne_NotTaken ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0), InstrI4(IROpCode::LdcI4, 42, 1),
        InstrBne(0, 1, 5), InstrI4(IROpCode::LdcI4, 42, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 0, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Bne_NotTaken");
}

// ── Test: Blt (signed less than) ───────────────────────────────────────
static void Test_Blt_Taken() {
    printf("\n── Test: Blt_Taken ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 1, 0), InstrI4(IROpCode::LdcI4, 2, 1),
        InstrBlt(0, 1, 5), InstrI4(IROpCode::LdcI4, 0, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 42, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Blt_Taken");
}
static void Test_Blt_NotTaken() {
    printf("\n── Test: Blt_NotTaken ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 3, 0), InstrI4(IROpCode::LdcI4, 2, 1),
        InstrBlt(0, 1, 5), InstrI4(IROpCode::LdcI4, 42, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 0, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Blt_NotTaken");
}

// ── Test: Bgt (signed greater than) ────────────────────────────────────
static void Test_Bgt_Taken() {
    printf("\n── Test: Bgt_Taken ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 2, 0), InstrI4(IROpCode::LdcI4, 1, 1),
        InstrBgt(0, 1, 5), InstrI4(IROpCode::LdcI4, 0, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 42, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Bgt_Taken");
}
static void Test_Bgt_NotTaken() {
    printf("\n── Test: Bgt_NotTaken ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 1, 0), InstrI4(IROpCode::LdcI4, 2, 1),
        InstrBgt(0, 1, 5), InstrI4(IROpCode::LdcI4, 42, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 0, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Bgt_NotTaken");
}

// ── Test: Ble (signed less or equal) ───────────────────────────────────
static void Test_Ble_Taken() {
    printf("\n── Test: Ble_Taken ──\n");
    RegisterMethod rm;
    // 1 <= 2 → taken
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 1, 0), InstrI4(IROpCode::LdcI4, 2, 1),
        InstrBle(0, 1, 5), InstrI4(IROpCode::LdcI4, 0, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 42, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Ble_Taken");
}
static void Test_Ble_NotTaken() {
    printf("\n── Test: Ble_NotTaken ──\n");
    RegisterMethod rm;
    // 3 <= 2 → not taken
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 3, 0), InstrI4(IROpCode::LdcI4, 2, 1),
        InstrBle(0, 1, 5), InstrI4(IROpCode::LdcI4, 42, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 0, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Ble_NotTaken");
}

// ── Test: Bge (signed greater or equal) ────────────────────────────────
static void Test_Bge_Taken() {
    printf("\n── Test: Bge_Taken ──\n");
    RegisterMethod rm;
    // 2 >= 1 → taken
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 2, 0), InstrI4(IROpCode::LdcI4, 1, 1),
        InstrBge(0, 1, 5), InstrI4(IROpCode::LdcI4, 0, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 42, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Bge_Taken");
}
static void Test_Bge_NotTaken() {
    printf("\n── Test: Bge_NotTaken ──\n");
    RegisterMethod rm;
    // 0 >= 1 → not taken
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0, 0), InstrI4(IROpCode::LdcI4, 1, 1),
        InstrBge(0, 1, 5), InstrI4(IROpCode::LdcI4, 42, 2), InstrRet(2),
        InstrI4(IROpCode::LdcI4, 0, 3), InstrRet(3),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 42, "Bge_NotTaken");
}

// ── Test: Div (100 / 3 = 33) ───────────────────────────────────────────
static void Test_Div_Ret() {
    printf("\n── Test: Div_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 100, 0), InstrI4(IROpCode::LdcI4, 3, 1),
        InstrBinary(IROpCode::Div, 2, 0, 1), InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    DumpCode(nm->code, nm->code_size);
    CHECK(ExecuteNative(nm->code) == 33, "Div_Ret");
}

// ── Test: Rem (100 % 3 = 1) ────────────────────────────────────────────
static void Test_Rem_Ret() {
    printf("\n── Test: Rem_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 100, 0), InstrI4(IROpCode::LdcI4, 3, 1),
        InstrBinary(IROpCode::Rem, 2, 0, 1), InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    DumpCode(nm->code, nm->code_size);
    CHECK(ExecuteNative(nm->code) == 1, "Rem_Ret");
}

// ── Test: Neg (-42) ────────────────────────────────────────────────────
static void Test_Neg_Ret() {
    printf("\n── Test: Neg_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrBinary(IROpCode::Neg, 1, 0, 0), InstrRet(1),
    };
    rm.max_regs = 2;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == static_cast<uint64_t>(-42), "Neg_Ret returned 0x%lx, expected 0x%lx", ExecuteNative(nm->code), static_cast<uint64_t>(-42));
}

// ── Test: Not (~0 = 0xFFFFFFFF) ────────────────────────────────────────
static void Test_Not_Ret() {
    printf("\n── Test: Not_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrBinary(IROpCode::Not, 1, 0, 0), InstrRet(1),
    };
    rm.max_regs = 2;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    DumpCode(nm->code, nm->code_size);
    CHECK(ExecuteNative(nm->code) == 0xFFFFFFFFu, "Not_Ret returned 0x%lx, expected 0x%lx", ExecuteNative(nm->code), 0xFFFFFFFFu);
}

// ── Test: And ──────────────────────────────────────────────────────────
static void Test_And_Ret() {
    printf("\n── Test: And_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0xFF, 0), InstrI4(IROpCode::LdcI4, 0x0F, 1),
        InstrBinary(IROpCode::And, 2, 0, 1), InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 0x0F, "And_Ret");
}
static void Test_Or_Ret() {
    printf("\n── Test: Or_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0xF0, 0), InstrI4(IROpCode::LdcI4, 0x0F, 1),
        InstrBinary(IROpCode::Or, 2, 0, 1), InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 0xFF, "Or_Ret");
}
static void Test_Xor_Ret() {
    printf("\n── Test: Xor_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0xFF, 0), InstrI4(IROpCode::LdcI4, 0x0F, 1),
        InstrBinary(IROpCode::Xor, 2, 0, 1), InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 0xF0, "Xor_Ret");
}

// ── Test: Shl (1 << 5 = 32) ────────────────────────────────────────────
static void Test_Shl_Ret() {
    printf("\n── Test: Shl_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 1, 0), InstrI4(IROpCode::LdcI4, 5, 1),
        InstrBinary(IROpCode::Shl, 2, 0, 1), InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 32, "Shl_Ret");
}

// ── Test: Shr (-64 >> 2 = -16 as arithmetic shift) ─────────────────────
static void Test_Shr_Ret() {
    printf("\n── Test: Shr_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, -64, 0), InstrI4(IROpCode::LdcI4, 2, 1),
        InstrBinary(IROpCode::Shr, 2, 0, 1), InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == static_cast<uint64_t>(static_cast<int64_t>(-16)), "Shr_Ret");
}

// ── Test: ShrUn (unsigned shift right — constant folder 64-bit) ─────────
// LdcI4(0xFFFFFFC0) = -64 as int32_t, sign-extended to 0xFFFFFFFFFFFFFFC0
// ShrUn by 2 → 0x3FFFFFFFFFFFFFF0 (64-bit unsigned shift)
static void Test_ShrUn_Ret() {
    printf("\n── Test: ShrUn_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0xFFFFFFC0, 0), InstrI4(IROpCode::LdcI4, 2, 1),
        InstrBinary(IROpCode::ShrUn, 2, 0, 1), InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 0x3FFFFFFFFFFFFFF0u, "ShrUn_Ret");
}

// ── Test: ShiftLarge_Ret (32-bit shift overflowing into upper 32 bits) ───
// LdcI4(0x2468ACF0) << 32 → 0x2468ACF000000000 (64-bit shift result)
static void Test_ShiftLarge_Ret() {
    printf("\n── Test: ShiftLarge_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0x2468ACF0, 0), InstrI4(IROpCode::LdcI4, 32, 1),
        InstrBinary(IROpCode::Shl, 2, 0, 1), InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 0x2468ACF000000000u, "ShiftLarge_Ret");
}

// ── Test: CeqSimple_Ret (direct Ceq test without branching) ────────────
static void Test_CeqSimple_Ret() {
    printf("\n── Test: CeqSimple_Ret ──\n");
    RegisterMethod rm;
    // Ceq(42, 42) → should be 1
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrI4(IROpCode::LdcI4, 42, 1),
        InstrBinary(IROpCode::Ceq, 2, 0, 1),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result = ExecuteNative(nm->code);
    printf("    CeqSimple_Ret(42,42) = %lu (0x%lx)\n", result, result);
    CHECK(result == 1, "CeqSimple_Ret (42==42 should be 1)");
}

// ── Test: CeqBr_Ret (compare-equal + branch via CSET) ───────────────────
// Ceq(42, 42) → 1, BrTrue → jump to return 99
static void Test_CeqBr_Ret() {
    printf("\n── Test: CeqBr_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrI4(IROpCode::LdcI4, 42, 1),
        InstrBinary(IROpCode::Ceq, 2, 0, 1),
        InstrBrTrue(2, 6),  // if r2 != 0, jump to instr 6 (LdcI4 99 → Ret)
        InstrI4(IROpCode::LdcI4, 42, 3),  // fallthrough: r3 = 42, Ret
        InstrRet(3),
        InstrI4(IROpCode::LdcI4, 99, 0),  // target: r0 = 99, Ret
        InstrRet(0),
    };
    rm.max_regs = 4;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result = ExecuteNative(nm->code);
    printf("    CeqBr_Ret result = %lu (0x%lx)\n", result, result);
    CHECK(result == 99, "CeqBr_Ret (expected 99, BrTrue taken)");
}

// ── Test: SpillStress_Ret (many registers to force spilling) ────────────
// Load 8 values, sum them all (forces register pressure), return sum
static void Test_SpillStress_Ret() {
    printf("\n── Test: SpillStress_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 10, 0),   // r0 = 10
        InstrI4(IROpCode::LdcI4, 20, 1),   // r1 = 20
        InstrI4(IROpCode::LdcI4, 30, 2),   // r2 = 30
        InstrI4(IROpCode::LdcI4, 40, 3),   // r3 = 40
        InstrI4(IROpCode::LdcI4, 50, 4),   // r4 = 50
        InstrI4(IROpCode::LdcI4, 60, 5),   // r5 = 60
        InstrI4(IROpCode::LdcI4, 70, 6),   // r6 = 70
        InstrI4(IROpCode::LdcI4, 80, 7),   // r7 = 80
        // Sum chain: r8 = r0+r1, r9 = r8+r2, r10 = r9+r3, ...
        InstrBinary(IROpCode::Add, 8, 0, 1),   // r8 = 10+20 = 30
        InstrBinary(IROpCode::Add, 9, 8, 2),   // r9 = 30+30 = 60
        InstrBinary(IROpCode::Add, 10, 9, 3),  // r10 = 60+40 = 100
        InstrBinary(IROpCode::Add, 11, 10, 4), // r11 = 100+50 = 150
        InstrBinary(IROpCode::Add, 12, 11, 5), // r12 = 150+60 = 210
        InstrBinary(IROpCode::Add, 13, 12, 6), // r13 = 210+70 = 280
        InstrBinary(IROpCode::Add, 14, 13, 7), // r14 = 280+80 = 360
        InstrRet(14),
    };
    rm.max_regs = 15;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = Compile(rm); if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    CHECK(ExecuteNative(nm->code) == 360, "SpillStress_Ret (8-reg sum = 360)");
}

// ── Floating-point tests ─────────────────────────────────────────────────
// Float results are stored in FPRs (vreg 64+).  After JIT execution, X4
// still points to the GPR file base.  FPR vreg N is at offset
// kFprFileOff + (N - 64) * 16 from X4.  We use ExecuteNativeFpr() to read
// the 64-bit double value from the NEON Q register slot.

// Test: LdcR4 3.14f → promote to double → store to FPR v64
static void Test_LdcR4_Ret() {
    printf("\n── Test: LdcR4_Ret ──\n");
    RegisterMethod rm;
    float f = 3.14f;
    int32_t bits; std::memcpy(&bits, &f, sizeof(bits));
    // LdcR4 promotes float→double (value = 3.140000104904175), stores to FPR
    rm.instructions = {
        InstrI4(IROpCode::LdcR4, bits, 64),
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrRet(0),
    };
    rm.max_regs = 65;
    if (!CanCompile(rm)) { printf("  SKIP: CanCompile returned false\n"); return; }
    auto* nm = CompileFpr(rm);
    if (!nm) { printf("  FAIL: Compile returned null\n"); ++g_failures; return; }
    uint64_t result_bits = ExecuteNativeFpr(nm->code, 64);
    double dval; std::memcpy(&dval, &result_bits, sizeof(dval));
    double expected = static_cast<double>(3.14f);  // float→double promotion
    uint64_t expected_bits; std::memcpy(&expected_bits, &expected, sizeof(expected_bits));
    CHECK(result_bits == expected_bits,
          "LdcR4_Ret got 0x%016" PRIx64 " (%g), expected 0x%016" PRIx64 " (%g)",
          result_bits, dval, expected_bits, expected);
}

// Test: LdcR8 2.718 → store to FPR v64
static void Test_LdcR8_Ret() {
    printf("\n── Test: LdcR8_Ret ──\n");
    RegisterMethod rm;
    double val = 2.718;
    rm.instructions = {
        InstrR8(IROpCode::LdcR8, val, 64),
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrRet(0),
    };
    rm.max_regs = 65;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileFpr(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result_bits = ExecuteNativeFpr(nm->code, 64);
    double dval; std::memcpy(&dval, &result_bits, sizeof(dval));
    uint64_t expected_bits; std::memcpy(&expected_bits, &val, sizeof(expected_bits));
    CHECK(result_bits == expected_bits,
          "LdcR8_Ret got 0x%016" PRIx64 " (%g), expected 0x%016" PRIx64 " (%g)",
          result_bits, dval, expected_bits, val);
}

// Test: Conv_R4 — int32→float (LdcI4 42 → Conv_R4 → FPR v64)
static void Test_ConvR4_Ret() {
    printf("\n── Test: ConvR4_Ret ──\n");
    RegisterMethod rm;
    double expected = static_cast<float>(42);  // int32→float→double promotion
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrBinary(IROpCode::Conv_R4, 64, 0, 0),
        InstrI4(IROpCode::LdcI4, 0, 1),
        InstrRet(1),
    };
    rm.max_regs = 65;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileFpr(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result_bits = ExecuteNativeFpr(nm->code, 64);
    double dval; std::memcpy(&dval, &result_bits, sizeof(dval));
    uint64_t expected_bits; std::memcpy(&expected_bits, &expected, sizeof(expected_bits));
    CHECK(result_bits == expected_bits,
          "ConvR4_Ret got %g, expected %g", dval, expected);
}

// Test: Conv_R8 — int32→double (LdcI4 100 → Conv_R8 → FPR v64)
static void Test_ConvR8_Ret() {
    printf("\n── Test: ConvR8_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 100, 0),
        InstrBinary(IROpCode::Conv_R8, 64, 0, 0),
        InstrI4(IROpCode::LdcI4, 0, 1),
        InstrRet(1),
    };
    rm.max_regs = 65;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileFpr(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result_bits = ExecuteNativeFpr(nm->code, 64);
    double dval; std::memcpy(&dval, &result_bits, sizeof(dval));
    double expected = 100.0;
    uint64_t expected_bits; std::memcpy(&expected_bits, &expected, sizeof(expected_bits));
    CHECK(result_bits == expected_bits,
          "ConvR8_Ret got %g, expected %g", dval, expected);
}

// Test: Fadd — LdcR8(10.5) + LdcR8(20.5) = 31.0
static void Test_Fadd_Ret() {
    printf("\n── Test: Fadd_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrR8(IROpCode::LdcR8, 10.5, 64),
        InstrR8(IROpCode::LdcR8, 20.5, 65),
        InstrBinary(IROpCode::Add, 66, 64, 65),
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrRet(0),
    };
    rm.max_regs = 67;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileFpr(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result_bits = ExecuteNativeFpr(nm->code, 66);
    double dval; std::memcpy(&dval, &result_bits, sizeof(dval));
    double expected = 31.0;
    uint64_t expected_bits; std::memcpy(&expected_bits, &expected, sizeof(expected_bits));
    CHECK(result_bits == expected_bits,
          "Fadd_Ret got %g, expected %g", dval, expected);
}

// Test: Fsub — LdcR8(50.0) - LdcR8(25.0) = 25.0
static void Test_Fsub_Ret() {
    printf("\n── Test: Fsub_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrR8(IROpCode::LdcR8, 50.0, 64),
        InstrR8(IROpCode::LdcR8, 25.0, 65),
        InstrBinary(IROpCode::Sub, 66, 64, 65),
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrRet(0),
    };
    rm.max_regs = 67;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileFpr(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result_bits = ExecuteNativeFpr(nm->code, 66);
    double dval; std::memcpy(&dval, &result_bits, sizeof(dval));
    double expected = 25.0;
    uint64_t expected_bits; std::memcpy(&expected_bits, &expected, sizeof(expected_bits));
    CHECK(result_bits == expected_bits,
          "Fsub_Ret got %g, expected %g", dval, expected);
}

// Test: Fmul — LdcR8(3.0) * LdcR8(4.0) = 12.0
static void Test_Fmul_Ret() {
    printf("\n── Test: Fmul_Ret ──\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrR8(IROpCode::LdcR8, 3.0, 64),
        InstrR8(IROpCode::LdcR8, 4.0, 65),
        InstrBinary(IROpCode::Mul, 66, 64, 65),
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrRet(0),
    };
    rm.max_regs = 67;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileFpr(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result_bits = ExecuteNativeFpr(nm->code, 66);
    double dval; std::memcpy(&dval, &result_bits, sizeof(dval));
    double expected = 12.0;
    uint64_t expected_bits; std::memcpy(&expected_bits, &expected, sizeof(expected_bits));
    CHECK(result_bits == expected_bits,
          "Fmul_Ret got %g, expected %g", dval, expected);
}

// Test: Fdiv — LdcR8(10.0) / LdcR8(3.0) = 10.0/3.0
static void Test_Fdiv_Ret() {
    printf("\n── Test: Fdiv_Ret ──\n");
    RegisterMethod rm;
    double expected = 10.0 / 3.0;
    rm.instructions = {
        InstrR8(IROpCode::LdcR8, 10.0, 64),
        InstrR8(IROpCode::LdcR8, 3.0, 65),
        InstrBinary(IROpCode::Div, 66, 64, 65),
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrRet(0),
    };
    rm.max_regs = 67;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileFpr(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result_bits = ExecuteNativeFpr(nm->code, 66);
    double dval; std::memcpy(&dval, &result_bits, sizeof(dval));
    uint64_t expected_bits; std::memcpy(&expected_bits, &expected, sizeof(expected_bits));
    CHECK(result_bits == expected_bits,
          "Fdiv_Ret got %.20g, expected %.20g", dval, expected);
}

// Test: CallNoArg — Call Helper_Return42 via JIT Call instruction
static void Test_CallOneArg() {
    fflush(stdout);
    printf("\n── Test: CallOneArg ──\n");
    fflush(stdout);
    RegisterMethod rm;
    // Load 42 into vreg0, call Helper_Identity(vreg0) → result in vreg1, return vreg1
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrCall((void*)Helper_Identity, 1, 0, 1),
        InstrRet(1),
    };
    rm.max_regs = 2;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileCall(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result = ExecuteNative(nm->code);
    CHECK(result == 42, "CallOneArg got %" PRIu64 ", expected 42", result);
}

static void Test_CallAdd() {
    fflush(stdout);
    printf("\n── Test: CallAdd ──\n");
    fflush(stdout);
    RegisterMethod rm;
    // Load 20 into vreg0, 22 into vreg1, call Helper_Add(vreg0, vreg1) → result in vreg2, return vreg2
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 20, 0),
        InstrI4(IROpCode::LdcI4, 22, 1),
        InstrCall((void*)Helper_Add, 2, 0, 2),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileCall(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result = ExecuteNative(nm->code);
    CHECK(result == 42, "CallAdd got %" PRIu64 ", expected 42", result);
}

/// Test LdInd/StInd: allocate value into a local slot via address, read back.
/// Sequence:
///   [0] LdLocA(local=0) → vreg0        (address of local 0 slot)
///   [1] LdcI4(42) → vreg1              (value to store)
///   [2] StInd(vreg0, vreg1)            (store vreg1 to *vreg0)
///   [3] LdInd(vreg0) → vreg2           (load from *vreg0 into vreg2)
///   [4] Ret(vreg2)                     (return loaded value)
static void Test_LdInd_StInd() {
    fflush(stdout);
    printf("\n── Test: LdInd_StInd ──\n");
    fflush(stdout);
    RegisterMethod rm;
    rm.instructions = {
        InstrLdLocA(0, 0),
        InstrI4(IROpCode::LdcI4, 42, 1),
        InstrStInd(0, 1),
        InstrLdInd(0, 2),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileCall(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result = ExecuteNative(nm->code);
    CHECK(result == 42, "LdInd_StInd got %" PRIu64 ", expected 42", result);
}

static void Test_CallNoArg() {
    fflush(stdout);
    printf("\n── Test: CallNoArg ──\n");
    fflush(stdout);
    RegisterMethod rm;
    rm.instructions = {
        InstrCall((void*)Helper_Return42, 0, 0, 0),
        InstrRet(0),
    };
    rm.max_regs = 1;
    if (!CanCompile(rm)) { printf("  SKIP\n"); return; }
    auto* nm = CompileCall(rm);
    if (!nm) { printf("  FAIL\n"); ++g_failures; return; }
    uint64_t result = ExecuteNative(nm->code);
    CHECK(result == 42, "CallNoArg got %" PRIu64 ", expected 42", result);
}

int main() {
    printf("ARM64 JIT Code Execution Test\n");
    printf("════════════════════════════\n");

    Test_LdcI4_Ret();
    Test_Add_Ret();
    Test_Sub_Ret();
    Test_Mul_Ret();
    Test_ArithmeticChain();
    Test_LdLocStLoc();
    Test_ConstFolding();
    Test_LargeImmediates();
    Test_Seal();
    Test_Br_Bypass();
    Test_BrTrue_Taken();
    Test_BrTrue_NotTaken();
    Test_BrFalse_Taken();

    // Comparison branch tests
    Test_Beq_Taken();
    Test_Beq_NotTaken();
    Test_Bne_Taken();
    Test_Bne_NotTaken();
    Test_Blt_Taken();
    Test_Blt_NotTaken();
    Test_Bgt_Taken();
    Test_Bgt_NotTaken();
    Test_Ble_Taken();
    Test_Ble_NotTaken();
    Test_Bge_Taken();
    Test_Bge_NotTaken();

    // Arithmetic extended
    Test_Div_Ret();
    Test_Rem_Ret();
    Test_Neg_Ret();
    Test_Not_Ret();
    Test_And_Ret();
    Test_Or_Ret();
    Test_Xor_Ret();
    Test_Shl_Ret();
    Test_Shr_Ret();

    // Extended shift + compare tests
    Test_ShrUn_Ret();
    Test_ShiftLarge_Ret();
    Test_CeqSimple_Ret();
    Test_CeqBr_Ret();
    Test_SpillStress_Ret();

    // Floating-point tests
    Test_LdcR4_Ret();
    Test_LdcR8_Ret();
    Test_ConvR4_Ret();
    Test_ConvR8_Ret();
    Test_Fadd_Ret();
    Test_Fsub_Ret();
    Test_Fmul_Ret();
    Test_Fdiv_Ret();

    // Method call tests
    Test_LdInd_StInd();
    Test_CallOneArg();
    Test_CallAdd();
    Test_CallNoArg();

    printf("\n══ Results: %d / %d tests passed, %d failures ══\n",
           g_tests - g_failures, g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}

