// jit_precode.h — JitPrecode stub definitions
//
// Implements CoreCLR-style Precode Stub dispatch for JIT mode.
//
// JitPrecode: used in JIT mode.  direct_ptr points to a per-method trampoline
//   that loads the JitPrecode* and jumps to the shared JitStubEntry code.
//   On first call, the state machine compiles the RegisterMethod and atomically
//   replaces direct_ptr with the compiled code entry point.

#ifndef CHAOS_IL2CPP_JIT_PRECODE_H_
#define CHAOS_IL2CPP_JIT_PRECODE_H_

#include "jit_engine.h"        // Compile, CompileConfig
#include "ir_reg_alloc.h"      // RegisterMethod

#include <atomic>
#include <cstdint>
#include <vector>

// Forward declaration from codegen_bridge.h
struct HotpatchEntryV0;

namespace chaos::il2cpp::jit {

// ── Constants ───────────────────────────────────────────────────────────
static constexpr uint32_t kPrecodeUncompiled = 0;
static constexpr uint32_t kPrecodeCompiling  = 1;
static constexpr uint32_t kPrecodeCompiled   = 2;

/// Threshold for triggering JIT compilation after warmup calls.
/// Value 30 is a common JIT warmup detection constant matching the
/// interpreter tiering system.
static constexpr uint32_t kJitUpgradeThreshold = 30;

/// PGO Full JIT promotion threshold.  When enable_pgo is true and a method's
/// pgo_call_count exceeds this value, the method is enqueued for background
/// Full JIT recompilation.  Value 100 means 100 Quick JIT calls before upgrading.
static constexpr uint32_t kPgoFullJitThreshold = 100;

// ── JitPrecode ─────────────────────────────────────────────────────────
// Per-method state for JIT mode dispatch.
//
//   HotpatchEntryV0::direct_ptr → trampoline → JitStubEntry ← shared
//                                                   |
//                                          JitStubDispatchImpl(precode)
//                                                   |
//                                            Compile(ir) → patch entry
struct JitPrecode {
    std::atomic<uint32_t> state{kPrecodeUncompiled};  // 0/1/2 state machine
    interpreter::RegisterMethod  ir;                   // owned copy of the IR
    CompileConfig                config;               // compilation config
    JitMethod*                   compiled   = nullptr; // result of Compile()
    HotpatchEntryV0*             entry      = nullptr; // back-pointer to patch entry
    void*                        trampoline = nullptr; // executable trampoline code
    void*                        original_direct_ptr = nullptr; // saved AOT code pointer

    // ── PGO fields (only used when config.enable_pgo is true) ───────────────
    std::atomic<uint32_t> pgo_call_count{0};   // Call count for PGO-driven Tier 1 upgrade
    bool                  tier1_enqueued{false}; // True once Tier 1 recompilation is queued
};

// ── PrecodeArena: allocates trampolines from a shared executable page ───
class PrecodeArena {
public:
    PrecodeArena() noexcept;
    ~PrecodeArena() noexcept;

    PrecodeArena(const PrecodeArena&) = delete;
    PrecodeArena& operator=(const PrecodeArena&) = delete;

    /// Allocate a trampoline for a JitPrecode.
    /// Returns the trampoline entry point, or nullptr on failure.
    void* AllocateJitTrampoline(JitPrecode* precode) noexcept;

private:
    /// A single RWX page holding trampolines + shared entries.
    struct Page {
        uint8_t* base;       // RWX virtual address
        uint32_t pos;        // next free offset
        uint32_t capacity;   // total page size
    };

    void EnsurePage() noexcept;
    void EmitJitSharedEntry() noexcept;

    std::vector<Page> pages_;
    bool              entries_emitted_      = false;
    uint32_t          jit_entry_offset_     = 0;
    uint32_t          jit_entry_size_       = 0;
};

// ── JitStubDispatchImpl: JIT mode dispatch ─────────────────────────────
// Called from the shared JitStubEntry assembly code (via JIT trampoline).
// Receives the JitPrecode* and handles the state machine:
//   1. CAS state kUncompiled → kCompiling
//   2. Winner: calls Compile(), patches HotpatchEntryV0::direct_ptr
//   3. Loser: spin-waits until state == kCompiled
//   4. Returns compiled entry point
extern "C" void* JitStubDispatchImpl(JitPrecode* precode) noexcept;

// ── JitRecompileToTier1: Background Tier 1 recompilation ──────────────────
// Called by the TierManager background thread to recompile a Tier 0 method
// with the full Tier 1 pipeline.  Replaces precode->compiled with the new
// JitMethod and returns the new entry point.  Returns nullptr on failure
// (the method continues running Tier 0 code).
extern "C" void* JitRecompileToTier1(JitPrecode* precode) noexcept;

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_JIT_PRECODE_H_
