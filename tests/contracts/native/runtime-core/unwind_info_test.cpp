// ── Unwind Info standalone test ──────────────────────────────────────
//
// Minimal test that verifies EmitUnwindInfo layout by reading from
// CodeBuffer::Data() BEFORE sealing (avoids Seal RX page issue).

#include "code_buffer.h"
#include "unwind_info.h"
#include <cstdio>
#include <windows.h>

using chaos::il2cpp::codegen::CodeBuffer;
using chaos::il2cpp::codegen::EmitUnwindInfo;
using chaos::il2cpp::codegen::AllocRuntimeFunction;
using chaos::il2cpp::codegen::RuntimeFunction;
using chaos::il2cpp::codegen::kPersonalityThunkSize;

static int g_pass = 0;
static int g_fail = 0;

#define TEST(name) do { \
    if (Test_##name()) { \
        std::printf("  PASS: %s\n", #name); g_pass++; \
    } else { \
        std::printf("  FAIL: %s\n", #name); g_fail++; \
    } \
} while(0)

static bool Test_Layout() {
    std::printf("  Test_Layout...\n");
    CodeBuffer buf;
    uint8_t push_reg_nums[] = {5, 3, 4};
    uint32_t push_offsets[] = {0, 2, 7};
    uint32_t off = EmitUnwindInfo(buf, 16, 64, 3,
        push_reg_nums, push_offsets, 7, 1, false);

    const uint8_t* d = buf.Data();
    if (!d) { std::printf("    FAIL: null data\n"); return false; }

    // Header
    if (d[off] != 1) { std::printf("    FAIL: vf=%u\n", d[off]); return false; }
    if (d[off+1] != 1) { std::printf("    FAIL: sop=%u\n", d[off+1]); return false; }
    if (d[off+2] != 5) { std::printf("    FAIL: coc=%u\n", d[off+2]); return false; }
    if (d[off+3] != 5) { std::printf("    FAIL: fr=%u\n", d[off+3]); return false; }

    std::printf("    OK: vf=%u sop=%u coc=%u fr=%u\n",
                d[off], d[off+1], d[off+2], d[off+3]);
    return true;
}

static bool Test_SehFlag() {
    std::printf("  Test_SehFlag...\n");
    CodeBuffer buf;
    uint8_t push_reg_nums[] = {5, 3, 4};
    uint32_t push_offsets[] = {0, 1, 2};
    uint32_t off = EmitUnwindInfo(buf, 16, 64, 3,
        push_reg_nums, push_offsets, 2, 1, true);

    const uint8_t* d = buf.Data();
    if (!d) { std::printf("    FAIL: null data\n"); return false; }

    uint8_t vf = d[off];
    if (vf != 9) { std::printf("    FAIL: vf=%u (expected 9)\n", vf); return false; }

    uint8_t cc = d[off+2];
    uint32_t thunk_off = off + 4 + cc*2 + ((4-(cc*2%4))%4);
    if (d[thunk_off] != 0x48 || d[thunk_off+1] != 0xB8) {
        std::printf("    FAIL: no thunk at +%u (%02X %02X)\n",
                    thunk_off, d[thunk_off], d[thunk_off+1]); return false;
    }
    if (d[thunk_off+10] != 0xFF || d[thunk_off+11] != 0xE0) {
        std::printf("    FAIL: thunk end wrong at +%u\n", thunk_off+10); return false;
    }
    std::printf("    OK: vf=%u thunk_at=%u\n", vf, thunk_off);
    return true;
}

static bool Test_AllocSmall() {
    std::printf("  Test_AllocSmall...\n");
    CodeBuffer buf;
    uint8_t push_reg_nums[] = {5};
    uint32_t push_offsets[] = {0};
    uint32_t off = EmitUnwindInfo(buf, 16, 128, 1,
        push_reg_nums, push_offsets, 1, 0, false);

    const uint8_t* d = buf.Data();
    if (!d) { return false; }

    uint8_t cc = d[off+2];
    if (cc != 3) { std::printf("    FAIL: cc=%u\n", cc); return false; }

    uint8_t op = d[off+4+1];
    if (op != ((2<<4)|15)) { std::printf("    FAIL: op=0x%02X\n", op); return false; }

    std::printf("    OK: cc=%u op=0x%02X\n", cc, op);
    return true;
}

static bool Test_AllocLarge() {
    std::printf("  Test_AllocLarge...\n");
    CodeBuffer buf;
    uint8_t push_reg_nums[] = {5};
    uint32_t push_offsets[] = {0};
    uint32_t frame_sub = 128u * 1024u + 8;
    uint32_t off = EmitUnwindInfo(buf, 16, frame_sub, 1,
        push_reg_nums, push_offsets, 1, 0, false);

    const uint8_t* d = buf.Data();
    if (!d) { return false; }

    uint8_t cc = d[off+2];
    if (cc != 4) { std::printf("    FAIL: cc=%u (expected 4)\n", cc); return false; }

    std::printf("    OK: cc=%u\n", cc);
    return true;
}

static bool Test_AllocRuntimeFunction() {
    std::printf("  Test_AllocRuntimeFunction...\n");
    auto* rf = AllocRuntimeFunction(42, 4096);
    if (!rf) { return false; }
    if (rf->begin_address != 0) { std::printf("    FAIL: begin=%u\n", rf->begin_address); return false; }
    if (rf->end_address != 4096) { std::printf("    FAIL: end=%u\n", rf->end_address); return false; }
    if (rf->unwind_info_address != 42) { std::printf("    FAIL: unwind=%u\n", rf->unwind_info_address); return false; }
    std::printf("    OK\n");
    std::free(rf);
    return true;
}

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::printf("Unwind Info Standalone Test\n===========================\n");

    TEST(Layout);
    TEST(SehFlag);
    TEST(AllocSmall);
    TEST(AllocLarge);
    TEST(AllocRuntimeFunction);

    std::printf("\nResults: %d passed, %d failed\n", g_pass, g_fail);
    return g_fail > 0 ? 1 : 0;
}
