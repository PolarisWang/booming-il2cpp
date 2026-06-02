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

// ── Execute native code and read return value ──────────────────────────
// On ARM64, the JIT stores the return value to [kRetBuf + 0] where
// kRetBuf = X4 = SP + kGprFileOff = SP (bottom of frame).
// After the JIT function returns and SP is restored, X4 still holds this
// address, and the result value is still at [X4] in the freed stack memory.
// We use a single inline-asm block to: set up null args, BLR to the JIT
// entry, and immediately LDR the result from [X4] — all before any
// compiler-generated code can clobber X4 or the stack memory.
static uint64_t ExecuteNative(void* entry, uint64_t* args = nullptr) noexcept {
    uint64_t args_buf[8] = {};
    if (args) std::memcpy(args_buf, args, 8 * sizeof(uint64_t));
    std::printf("    calling entry=%p\n", entry);
    uint64_t result = 0;
    // Single asm block: set args (X0, X1 = null), BLR entry, read [X4].
    // X4 is caller-saved — the JIT stores the result at [kRetBuf] = [X4],
    // and the epilogue does not clear X4, so after return X4 still points
    // to the result slot.  We load from [X4] before any stack reuse.
    // Passing args_buf in X0 and X1 so BLR has valid (but ignored) params.
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

    printf("\n══ Results: %d / %d tests passed, %d failures ══\n",
           g_tests - g_failures, g_tests, g_failures);
    return g_failures > 0 ? 1 : 0;
}
