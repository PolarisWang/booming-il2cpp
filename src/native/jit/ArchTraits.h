#ifndef CHAOS_IL2CPP_ARCH_TRAITS_H_
#define CHAOS_IL2CPP_ARCH_TRAITS_H_

// ── Per-architecture constant aggregates ─────────────────────────────────────
//
// ArchTraits<Arch> provides compile-time constants for each supported
// architecture (x64, ARM64).  Used by jit_engine.cpp to replace raw x64
// register constants with architecture-agnostic names.

#include <cstdint>

namespace chaos::il2cpp::jit {

/// Architecture enum for compile-time dispatch.
enum class Arch : uint8_t {
    kX64 = 0,
    kARM64 = 1,
};

template <Arch A>
struct ArchTraits;

// ── x64 architecture traits ─────────────────────────────────────────────────

template <>
struct ArchTraits<Arch::kX64> {
    // Scratch / convention registers
    static constexpr uint8_t kScratchA = 0;       // RAX — primary scratch / accumulator
    static constexpr uint8_t kScratchB = 1;       // RCX — shift count / second scratch
    static constexpr uint8_t kScratchC = 2;       // RDX — div/rem / third scratch
    static constexpr uint8_t kArgsBuf = 3;        // RBX — args_buf base pointer
    static constexpr uint8_t kRetBuf = 6;         // RSI — ret_buf pointer
    static constexpr uint8_t kFrameReg = 5;       // RBP — frame pointer
    static constexpr uint8_t kStackReg = 4;       // RSP — stack pointer
    static constexpr uint8_t kTmpReg = 10;        // R10 — temporary
    static constexpr uint8_t kExtraScratch0 = 8;  // R8 — caller-saved scratch
    static constexpr uint8_t kExtraScratch1 = 9;  // R9 — caller-saved scratch
    static constexpr uint8_t kExtraScratch2 = 11; // R11 — caller-saved scratch (write barrier)

    // Virtual register counts
    static constexpr uint32_t kGprCount = 64;
    static constexpr uint32_t kFprCount = 32;

    // Callee-saved GPRs (for prologue/epilogue save/restore)
    static constexpr int kCalleeSavedGprs[] = {7, 12, 13, 14, 15}; // RDI, R12-R15
    static constexpr uint32_t kNumCalleeSavedGpr = 5;

    // Calling convention
    static constexpr uint32_t kArgRegCount = 4;   // RCX, RDX, R8, R9 (Win64)
    static constexpr uint32_t kShadowSpace = 32;  // Win64 shadow space
    static constexpr uint32_t kFprSaveSize = 32;  // YMM 256-bit slot
    static constexpr uint32_t kMaxInstrSize = 16; // bytes
};

// ── ARM64 architecture traits ──────────────────────────────────────────────

template <>
struct ArchTraits<Arch::kARM64> {
    // Scratch / convention registers
    static constexpr uint8_t kScratchA = 0;       // X0 — first arg / primary scratch
    static constexpr uint8_t kScratchB = 1;       // X1 — second arg / scratch
    static constexpr uint8_t kScratchC = 2;       // X2 — third arg / scratch
    static constexpr uint8_t kArgsBuf = 3;        // X3 — args_buf base
    static constexpr uint8_t kRetBuf = 4;         // X4 — ret_buf pointer
    static constexpr uint8_t kFrameReg = 29;      // X29 (FP) — frame pointer
    static constexpr uint8_t kStackReg = 31;      // SP (separate encoding)
    static constexpr uint8_t kTmpReg = 17;        // X17 (IP1) — intra-procedure scratch
    static constexpr uint8_t kExtraScratch0 = 8;  // X8 — caller-saved scratch
    static constexpr uint8_t kExtraScratch1 = 9;  // X9 — caller-saved scratch
    static constexpr uint8_t kExtraScratch2 = 11; // X11 — caller-saved scratch

    // Virtual register counts (same as x64 for IR compatibility)
    static constexpr uint32_t kGprCount = 64;
    static constexpr uint32_t kFprCount = 32;

    // Callee-saved GPRs: X19-X30 (12 regs)
    static constexpr int kCalleeSavedGprs[] = {19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
    static constexpr uint32_t kNumCalleeSavedGpr = 12;

    // Calling convention (AAPCS64)
    static constexpr uint32_t kArgRegCount = 8;  // X0-X7
    static constexpr uint32_t kShadowSpace = 0;  // No shadow space
    static constexpr uint32_t kFprSaveSize = 16; // 128-bit NEON slot
    static constexpr uint32_t kMaxInstrSize = 4; // Fixed 4-byte instructions
};

} // namespace chaos::il2cpp::jit

#endif // CHAOS_IL2CPP_ARCH_TRAITS_H_
