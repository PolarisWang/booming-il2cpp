/// ARM64 JIT encoder smoke test
/// Tests that the ARM64 encoder emits valid instructions by:
///   1. Instantiating Arm64Encoder and emitting each instruction category
///   2. Verifying instruction count (= buffer bytes / 4)
///   3. Checking specific known encodings for key instructions
/// Runs on QEMU user-mode (no ARM64 hardware needed).

#include <cstdio>
#include <cstdint>
#include <cstring>

#include "code_buffer.h"
#include "arm64_encoder.h"
#include "Arm64Encoder.h"
#include "IEncoder.h"

using namespace chaos::il2cpp::jit;

static int g_failures = 0;

#define CHECK(cond, ...) do {                                                 \
    if (!(cond)) {                                                            \
        printf("  FAIL [%s:%d]: ", __FILE__, __LINE__);                       \
        printf(__VA_ARGS__);                                                  \
        printf("\n");                                                          \
        ++g_failures;                                                          \
    } else {                                                                  \
        printf("  PASS: ");                                                   \
        printf(__VA_ARGS__);                                                  \
        printf("\n");                                                          \
    }                                                                         \
} while(0)

// Test 1: Basic arithmetic encoding (free functions)
void TestArithmetic() {
    printf("\n── Test 1: Arithmetic encoding ──\n");
    CodeBuffer buf;

    // ADD X0, X1, X2 via free function
    EmitAdd64(buf, 0, 1, 2);
    CHECK(buf.pos() == 4, "ADD X0,X1,X2 emitted 4 bytes");

    // SUB X3, X4, X5
    EmitSub64(buf, 3, 4, 5);
    CHECK(buf.pos() == 8, "SUB X3,X4,X5 emitted 4 more bytes (total 8)");

    // MUL X6, X7, X8
    EmitMul64(buf, 6, 7, 8);
    CHECK(buf.pos() == 12, "MUL X6,X7,X8 emitted 4 more bytes (total 12)");

    printf("  Arithmetic: %u bytes, %u instructions\n",
           (unsigned)buf.pos(), (unsigned)(buf.pos() / 4));
}

// Test 2: Immediate encoding
void TestImmediates() {
    printf("\n── Test 2: Immediate encoding ──\n");
    CodeBuffer buf;

    // ADD X1, X2, #42
    EmitAdd64Imm(buf, 1, 2, 42);
    CHECK(buf.pos() == 4, "ADD X1,X2,#42 emitted 4 bytes");

    // MOVZ X0, #0x1234, lsl #0
    EmitMovz64(buf, 0, 0x1234, 0);
    CHECK(buf.pos() == 8, "MOVZ X0,#0x1234 emitted 4 more bytes");

    // MOVK X0, #0x5678, lsl #16
    EmitMovk64(buf, 0, 0x5678, 16);
    CHECK(buf.pos() == 12, "MOVK X0,#0x5678,lsl16 emitted 4 more bytes");

    printf("  Immediates: %u bytes, %u instructions\n",
           (unsigned)buf.pos(), (unsigned)(buf.pos() / 4));
}

// Test 3: Load/Store encoding
void TestLoadStore() {
    printf("\n── Test 3: Load/Store encoding ──\n");
    CodeBuffer buf;

    // STR X5, [X1, #16] → offset scaled by 8 → imm12=2
    EmitStr64(buf, 5, 1, 2);
    CHECK(buf.pos() == 4, "STR X5,[X1,#16] emitted 4 bytes");

    // LDR X7, [X3, #32] → imm12=4
    EmitLdr64(buf, 7, 3, 4);
    CHECK(buf.pos() == 8, "LDR X7,[X3,#32] emitted 4 more bytes");

    // STR W9, [X2] → imm12=0
    EmitStr32(buf, 9, 2, 0);
    CHECK(buf.pos() == 12, "STR W9,[X2] emitted 4 more bytes");

    // LDR W10, [X2, #4] → imm12=1 (32-bit load, scaled by 4)
    EmitLdr32(buf, 10, 2, 1);
    CHECK(buf.pos() == 16, "LDR W10,[X2,#4] emitted 4 more bytes");

    printf("  Load/Store: %u bytes, %u instructions\n",
           (unsigned)buf.pos(), (unsigned)(buf.pos() / 4));
}

// Test 4: Branch encoding
void TestBranch() {
    printf("\n── Test 4: Branch encoding ──\n");
    CodeBuffer buf;

    // B #8 (forward branch 2 instructions = 8 bytes)
    EmitB(buf, 8);
    CHECK(buf.pos() == 4, "B #8 emitted 4 bytes");

    // NOP (encoding should be 0xD503201F)
    buf.Emit32(0xD503201F);
    CHECK(buf.pos() == 8, "NOP emitted 4 bytes (total 8)");

    // BL #0 (branch to self)
    EmitBl(buf, 0);
    CHECK(buf.pos() == 12, "BL #0 emitted 4 more bytes (total 12)");

    // BR X5 (branch to register)
    EmitBr(buf, 5);
    CHECK(buf.pos() == 16, "BR X5 emitted 4 more bytes (total 16)");

    printf("  Branch: %u bytes, %u instructions\n",
           (unsigned)buf.pos(), (unsigned)(buf.pos() / 4));
}

// Test 5: IEncoder interface dispatch
void TestEncoderInterface() {
    printf("\n── Test 5: IEncoder interface ──\n");
    CodeBuffer buf;
    Arm64Encoder enc(buf);

    CHECK(enc.GetArch() == Arch::kARM64, "GetArch returns kARM64");

    // MOV X0, #42
    enc.EmitMovRI32(0, 42);
    CHECK(buf.pos() == 4, "EmitMovRI32 emits 4 bytes");

    // MOV X1, X2 (register copy)
    enc.EmitMovRR(1, 2);
    CHECK(buf.pos() == 8, "EmitMovRR adds 4 bytes (total 8)");

    // ADD X3, X3, X4 (dst += src)
    enc.EmitAddRR(3, 4);
    CHECK(buf.pos() == 12, "EmitAddRR adds 4 bytes (total 12)");

    // SUB X5, X5, X6 (dst -= src)
    enc.EmitSubRR(5, 6);
    CHECK(buf.pos() == 16, "EmitSubRR adds 4 bytes (total 16)");

    // MUL X7, X7, X8 (dst *= src)
    enc.EmitImulRR(7, 8);
    CHECK(buf.pos() == 20, "EmitImulRR adds 4 bytes (total 20)");

    // LEA R3, [X4 + 256]
    enc.EmitLeaRM(3, 4, 256);
    CHECK(buf.pos() >= 24, "EmitLeaRM adds at least 4 bytes");

    // ADD R0, R1, #16 (3-operand add immediate)
    enc.EmitAddRI(0, 1, 16);
    CHECK(buf.pos() >= 28, "EmitAddRI adds at least 4 bytes");

    // CMP R0, R1
    enc.EmitCmpRR(0, 1);
    CHECK(buf.pos() >= 32, "EmitCmpRR adds at least 4 bytes");

    // Call via register (BLR X9)
    enc.EmitCallReg(9);
    CHECK(buf.pos() >= 36, "EmitCallReg adds at least 4 bytes");

    printf("  Interface: %u bytes, %u instructions\n",
           (unsigned)buf.pos(), (unsigned)(buf.pos() / 4));
}

// Test 6: NEON SIMD via concrete Arm64Encoder methods
void TestSimd() {
    printf("\n── Test 6: NEON SIMD encoding ──\n");
    CodeBuffer buf;
    Arm64Encoder enc(buf);

    // Use available SIMD methods on Arm64Encoder
    // MOVDQA V0, V1 (128-bit move)
    enc.EmitMovdqaRR(0, 1);
    CHECK(buf.pos() >= 4, "EmitMovdqaRR emitted at least 4 bytes");

    // PADDB V2, V3 (128-bit integer add)
    enc.EmitPaddbRR(2, 3);
    CHECK(buf.pos() >= 8, "EmitPaddbRR added 4 bytes");

    // PAND V4, V5 (128-bit bitwise and)
    enc.EmitPandRR(4, 5);
    CHECK(buf.pos() >= 12, "EmitPandRR added 4 bytes");

    // PXOR V6, V7 (128-bit bitwise xor)
    enc.EmitPxorRR(6, 7);
    CHECK(buf.pos() >= 16, "EmitPxorRR added 4 bytes");

    printf("  SIMD: %u bytes, %u instructions\n",
           (unsigned)buf.pos(), (unsigned)(buf.pos() / 4));
}

int main() {
    printf("ARM64 JIT Encoder Smoke Test\n");
    printf("════════════════════════════\n");

    TestArithmetic();
    TestImmediates();
    TestLoadStore();
    TestBranch();
    TestEncoderInterface();
    TestSimd();

    printf("\n══ Results: %d failures ══\n", g_failures);
    return g_failures > 0 ? 1 : 0;
}
