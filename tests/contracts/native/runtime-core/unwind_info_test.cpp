// ── Unwind Info standalone test (rewritten for the real jit::EmitUnwindInfo) ──
//
// Originally written against a hypothetical `codegen::EmitUnwindInfo` with a
// different signature/byte layout. The real production emitter is
// `chaos::il2cpp::jit::EmitUnwindInfo` (src/native/jit/jit_unwind.{h,cpp}), which
// this test now exercises so it becomes a live regression test for the actual
// JIT T4 unwind-info emitter used in production (recall V2 SEH thunk layout).

#include "code_buffer.h"      // chaos::il2cpp::jit::CodeBuffer
#include "jit_unwind.h"       // chaos::il2cpp::jit::{EmitUnwindInfo,AllocRuntimeFunction,RuntimeFunction}
#include <cstdio>
#include <cstdlib>
#include <cstring>

using chaos::il2cpp::jit::CodeBuffer;
using chaos::il2cpp::jit::EmitUnwindInfo;
using chaos::il2cpp::jit::AllocRuntimeFunction;

static int g_pass = 0;
static int g_fail = 0;
#define TEST(name) do { \
    if (Test_##name()) { std::printf("  PASS: %s\n", #name); g_pass++; } \
    else            { std::printf("  FAIL: %s\n", #name); g_fail++; } \
} while (0)

// Layout for the real emitter: (prologue_size, frame_sub, num_push, regs,
// offsets, sub_rsp_offset, set_fpreg_offset, has_seh).
//   num_push=3, frame_sub=64 → alloc_small (1 alloc code) → code_count = 3+1+1 = 5.
//   Header: ver=1|(flags<<3); flags=0 (no seh) → 0x01.
//   size_of_prolog = 255 (safe max, real impl).
//   count_of_codes  = 5.
//   fp_reg_off      = (1<<4)|5 = 0x15 (FrameRegister=5/RBP, FrameOffset=1).
static bool Test_Layout() {
    std::printf("  Test_Layout...\n");
    CodeBuffer buf;
    uint8_t push_reg_nums[] = {5, 3, 4};        // rbp, rbx, rsi
    uint32_t push_offsets[] = {0, 2, 7};
    uint32_t off = EmitUnwindInfo(buf, 16, 64, 3, push_reg_nums, push_offsets, 7, 1, false);

    const uint8_t* d = buf.Data();
    if (!d) { std::printf("    FAIL: null data\n"); return false; }
    if (d[off + 0] != 0x01) { std::printf("    FAIL: header[0]=0x%02X (want 0x01 ver)\n", d[off]); return false; }
    if (d[off + 2] != 5)    { std::printf("    FAIL: header[2]=%u (want 5 codes)\n", d[off + 2]); return false; }
    if (d[off + 3] != 0x15) { std::printf("    FAIL: header[3]=0x%02X (want 0x15 fp_reg+off)\n", d[off + 3]); return false; }
    std::printf("    OK: header ver=0x%02X codes=%u fp=0x%02X\n", d[off], d[off + 2], d[off + 3]);
    return true;
}

// With has_seh=true, header version_flags becomes 1|(0x01<<3) = 0x09, and a
// 12-byte absolute JMP thunk (48 B8 <imm64> FF E0) is appended after the
// padded unwind codes. Verify version + thunk boundaries.
static bool Test_SehFlag() {
    std::printf("  Test_SehFlag...\n");
    CodeBuffer buf;
    uint8_t push_reg_nums[] = {5, 3, 4};
    uint32_t push_offsets[] = {0, 1, 2};
    uint32_t off = EmitUnwindInfo(buf, 16, 64, 3, push_reg_nums, push_offsets, 2, 1, true);

    const uint8_t* d = buf.Data();
    if (!d) { std::printf("    FAIL: null data\n"); return false; }

    uint8_t vf = d[off];
    if (vf != 0x09) { std::printf("    FAIL: header[0]=0x%02X (want 0x09 ver|EHANDLER)\n", vf); return false; }

    uint8_t code_count = d[off + 2];
    uint32_t code_bytes = code_count * 2;
    uint32_t pad = (4 - (code_bytes % 4)) % 4;
    uint32_t thunk_off = off + 4 + code_bytes + pad;
    if (d[thunk_off + 0] != 0x48 || d[thunk_off + 1] != 0xB8) {
        std::printf("    FAIL: no mov-rax thunk at +%u (%02X %02X)\n",
                    thunk_off, d[thunk_off], d[thunk_off + 1]); return false;
    }
    if (d[thunk_off + 10] != 0xFF || d[thunk_off + 11] != 0xE0) {
        std::printf("    FAIL: thunk end wrong at +%u (%02X %02X)\n",
                    thunk_off + 10, d[thunk_off + 10], d[thunk_off + 11]); return false;
    }
    std::printf("    OK: header[0]=0x%02X thunk_at=+%u\n", vf, thunk_off);
    return true;
}

// frame_sub=128 → alloc_small (128 in [8,128], %8==0) → code_count = 1+1+1 = 3.
// First code after header is UWOP_ALLOC_SMALL: op = (OpInfo<<4)|UWOP_ALLOC_SMALL(2),
// where OpInfo = scale-1 = (128/8)-1 = 15 → op = (0xF0)|2 = 0xF2.
static bool Test_AllocSmall() {
    std::printf("  Test_AllocSmall...\n");
    CodeBuffer buf;
    uint8_t push_reg_nums[] = {5};
    uint32_t push_offsets[] = {0};
    uint32_t off = EmitUnwindInfo(buf, 16, 128, 1, push_reg_nums, push_offsets, 1, 0, false);

    const uint8_t* d = buf.Data();
    if (!d) { return false; }

    uint8_t cc = d[off + 2];
    if (cc != 3) { std::printf("    FAIL: count_of_codes=%u (want 3)\n", cc); return false; }
    // First unwind code (after 4-byte header): UWOP_ALLOC_SMALL.
    uint8_t op = d[off + 4 + 1];
    if (op != 0xF2) { std::printf("    FAIL: op=0x%02X (want 0xF2 ALLOC_SMALL scale=16)\n", op); return false; }

    std::printf("    OK: codes=%u op=0x%02X\n", cc, op);
    return true;
}

// frame_sub = 128KiB+8 → too large for ALLOC_SMALL → ALLOC_LARGE (2 code slots).
// code_count = 1 + 1 + 2 = 4. Second code slot is the ALLOC_LARGE data.
static bool Test_AllocLarge() {
    std::printf("  Test_AllocLarge...\n");
    CodeBuffer buf;
    uint8_t push_reg_nums[] = {5};
    uint32_t push_offsets[] = {0};
    uint32_t frame_sub = 128u * 1024u + 8;
    uint32_t off = EmitUnwindInfo(buf, 16, frame_sub, 1, push_reg_nums, push_offsets, 1, 0, false);

    const uint8_t* d = buf.Data();
    if (!d) { return false; }

    uint8_t cc = d[off + 2];
    if (cc != 4) { std::printf("    FAIL: count_of_codes=%u (want 4 for ALLOC_LARGE)\n", cc); return false; }

    std::printf("    OK: codes=%u\n", cc);
    return true;
}

static bool Test_AllocRuntimeFunction() {
    std::printf("  Test_AllocRuntimeFunction...\n");
    auto* rf = AllocRuntimeFunction(42, 4096);
    if (!rf) { return false; }
    if (rf->begin_address != 0)     { std::printf("    FAIL: begin=%u\n", rf->begin_address); return false; }
    if (rf->end_address != 4096)    { std::printf("    FAIL: end=%u\n", rf->end_address); return false; }
    if (rf->unwind_info_address != 42) { std::printf("    FAIL: unwind=%u\n", rf->unwind_info_address); return false; }
    std::printf("    OK\n");
    std::free(rf);
    return true;
}

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::printf("Unwind Info Standalone Test (jit::EmitUnwindInfo)\n=========================================================\n");

    TEST(Layout);
    TEST(SehFlag);
    TEST(AllocSmall);
    TEST(AllocLarge);
    TEST(AllocRuntimeFunction);

    std::printf("\nResults: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail > 0 ? 1 : 0;
}
