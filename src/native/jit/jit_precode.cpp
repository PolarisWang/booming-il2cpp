// jit_precode.cpp — JitPrecode stub dispatch implementation
//
// Implements the PrecodeArena RWX page manager and JitStubDispatchImpl
// state machine for CoreCLR-style precode stub dispatch.
//
// Per-method trampoline (15 bytes):
//   0x49 0xBA <8B addr>   mov  r10, <precode_ptr>
//   0xE9 <4B rel32>        jmp  <shared_entry>
//
// Shared entry (JitStubEntry), emitted once per RWX page:
//   0x51                   push rcx
//   0x52                   push rdx
//   0x41 0x50             push r8
//   0x41 0x51             push r9
//   0x48 0x83 0xEC 0x20   sub  rsp, 0x20
//   0x4C 0x89 0xD1         mov  rcx, r10        ; precode* → first arg
//   0x48 0xB8 <8B addr>   mov  rax, <dispatch>  ; JitStubDispatchImpl addr
//   0xFF 0xD0             call rax
//   0x48 0x83 0xC4 0x20   add  rsp, 0x20
//   0x41 0x59             pop  r9
//   0x41 0x58             pop  r8
//   0x5A                  pop  rdx
//   0x59                  pop  rcx
//   0xFF 0xE0             jmp  rax              ; tail-call to compiled entry

#include "jit_precode.h"
#include "jit_inline.h" // g_inline_reverse_map

#include <codegen_bridge.h>     // HotpatchEntryV0
#include <aot_core_ir_reader.h> // DeserializeAotCoreIrMethod
#include <hotpatch_table.h>     // GetHotpatchNameRegistry
#include <ir_reg_alloc.h>       // AllocateRegisters
#include "jit_seh.h"            // RegisterNativeCodeSection
#include <jit_registration.h>   // JitEntry
#include <patch_loader.h>       // PatchMethod (full definition)

#include <mutex>
#include <tier_manager.h>       // TierManager::EnqueueJitRecompilation
#include <slot_map.h>           // ReverseSlotMap, g_reverse_slot_map
#include <chaos/log.h>          // CHAOS_IL2CPP_LOG_ERROR_M
#include <gc/gc_root_scanner.h> // GcRegisterSlotMap

#if defined(_WIN32) || defined(_WIN64)
#define NOMINMAX
#include <windows.h>
#include <intrin.h>
#include <exception_jmp.h> // CHAOS_SEH_FILTER_ALL (needs DWORD from windows.h)
#endif

#include <chaos/pal/pal_mem.h>
#include <chaos/pal/pal_cache.h>
#include <chaos/eh.h>              // CHAOS_EH_TRY / CHAOS_EH_CATCH_BEGIN
#include <generated_code_compat.h> // chaos_managed_exception for CHAOS_EH_CATCH_BEGIN

#include <cstdlib>
#include <cstring>
#include <thread>

#if defined(__x86_64__)
#include <immintrin.h>
#endif

namespace chaos::il2cpp::jit {

// ── Constants ──────────────────────────────────────────────────────────
static constexpr uint32_t kPageSize = 64 * 1024;         // 64KB per RWX page
static constexpr uint32_t kSharedEntrySize = 64;         // reserved
static constexpr uint32_t kSharedEntryUnwindOffset = 40; // UNWIND_INFO offset (must be 4-byte aligned for Win64)
#if defined(__aarch64__)
static constexpr uint32_t kTrampolineSize = 16; // LDR + BR + literal pool
#else
static constexpr uint32_t kTrampolineSize = 15; // mov r10, addr + jmp rel32
#endif

// ── Per-page descriptor ───────────────────────────────────────────────
struct Page {
    uint8_t* base;     // RWX memory base
    uint32_t pos;      // next free offset from base
    uint32_t capacity; // total page size
};

// ── PrecodeArena ──────────────────────────────────────────────────────

PrecodeArena::PrecodeArena() noexcept {}

PrecodeArena::~PrecodeArena() noexcept {
    for (auto& pg : pages_) {
        if (pg.base) {
            chaos::il2cpp::pal::PalVirtualFree(pg.base, pg.capacity);
        }
#if defined(_WIN64)
        if (pg.runtime_function) {
            std::free(pg.runtime_function);
        }
#endif
    }
}

// Forward declaration for RegisterPagePdata (defined after AllocateJitTrampoline).
#if defined(_WIN64)
static void RegisterPagePdata(uint8_t* page_base, void*& out_runtime_function, uint32_t entry_size) noexcept;
#endif

void PrecodeArena::EnsurePage() noexcept {
    if (!pages_.empty()) {
        auto& last = pages_.back();
        if (last.pos + kTrampolineSize <= last.capacity)
            return; // room on current page

        // ── Seal the full page to RX (W^X compliance) ─────────────
        // This page is now full — no more trampolines will be written
        // to it.  Seal to RX so it can only be executed, not modified.
        chaos::il2cpp::pal::PalVirtualProtect(last.base, last.capacity, chaos::il2cpp::pal::kPalMemReadExec);
    }

    // Allocate RW memory, then make it RWX for code emission.
    // This is more secure than allocating RWX directly (W^X principle).
    uint8_t* base = static_cast<uint8_t*>(chaos::il2cpp::pal::PalVirtualAlloc(kPageSize));
    if (base) {
        chaos::il2cpp::pal::PalVirtualProtect(base, kPageSize, chaos::il2cpp::pal::kPalMemReadWriteExec);
    }
    if (!base) {
        CHAOS_IL2CPP_LOG_ERROR_M("jit", "PrecodeArena: failed to allocate RWX page");
        return;
    }

    Page pg;
    pg.base = base;
    pg.pos = 0;
    pg.capacity = kPageSize;
    pages_.push_back(pg);

    // Emit the JIT shared entry at the start of every new page.
    // First page emits; subsequent pages copy from page 0.
    if (!entries_emitted_) {
        EmitJitSharedEntry();
        entries_emitted_ = true;
    } else {
        std::memcpy(base, pages_[0].base, jit_entry_size_);
        pages_.back().pos = jit_entry_size_;
    }

#if defined(_WIN64)
    // Register .pdata for the shared entry so the OS can unwind through
    // this non-leaf trampoline frame when JIT-compiled code crashes.
    RegisterPagePdata(pages_.back().base, pages_.back().runtime_function, jit_entry_size_);
#endif
}

void PrecodeArena::EmitJitSharedEntry() noexcept {
    if (pages_.empty())
        return;
    auto& pg = pages_.back();
    jit_entry_offset_ = pg.pos;

    uint8_t* p = pg.base + pg.pos;

#if defined(__aarch64__)
    // ARM64 shared entry: save X0-X7 stp, call dispatch, restore ldp, tail-call br.
    //                                                   offset  encoding  bytes (LE)
    // stp x0, x1, [sp, #-16]!   imm7=-2, rt2=1, rn=31   0       0xA9BF07E0  E0 07 BF A9
    // stp x2, x3, [sp, #-16]!   imm7=-2, rt2=3, rn=31   4       0xA9BF0FE2  E2 0F BF A9
    // stp x4, x5, [sp, #-16]!   imm7=-2, rt2=5, rn=31   8       0xA9BF17E4  E4 17 BF A9
    // stp x6, x7, [sp, #-16]!   imm7=-2, rt2=7, rn=31  12       0xA9BF1FE6  E6 1F BF A9
    // mov x0, x17                                      16       0xAA1103E0  E0 03 11 AA
    // ldr x1, [pc, #32]  imm=8, rt=1                   20       0x58000101  01 01 80 58
    // blr x1                                           24       0xD63F0020  20 00 3F D6
    // mov x17, x0                                      28       0xAA0003F1  F1 03 00 AA
    // ldp x6, x7, [sp], #16   imm7=2, rt2=7, rn=31    32       0xA8C11FE6  E6 1F C1 A8
    // ldp x4, x5, [sp], #16   imm7=2, rt2=5, rn=31    36       0xA8C117E4  E4 17 C1 A8
    // ldp x2, x3, [sp], #16   imm7=2, rt2=3, rn=31    40       0xA8C10FE2  E2 0F C1 A8
    // ldp x0, x1, [sp], #16   imm7=2, rt2=1, rn=31    44       0xA8C107E0  E0 07 C1 A8
    // br x17                                           48       0xD61F0220  20 02 1F D6
    // 8-byte literal (dispatch address)                52
    // nop padding to 64                                60       0xD503201F  1F 20 03 D5
    const uint32_t kSharedEntryInstrs[] = {0xA9BF07E0u, 0xA9BF0FE2u, 0xA9BF17E4u, 0xA9BF1FE6u, 0xAA1103E0u,
                                           0x58000101u, 0xD63F0020u, 0xAA0003F1u, 0xA8C11FE6u, 0xA8C117E4u,
                                           0xA8C10FE2u, 0xA8C107E0u, 0xD61F0220u};
    std::memcpy(p, kSharedEntryInstrs, sizeof(kSharedEntryInstrs));

    auto dispatch_addr = reinterpret_cast<uintptr_t>(&JitStubDispatchImpl);
    std::memcpy(p + 52, &dispatch_addr, sizeof(dispatch_addr));

    // Padding to kSharedEntrySize
    uint32_t nop = 0xD503201Fu;
    std::memcpy(p + 60, &nop, sizeof(nop));
#elif defined(_WIN64) || defined(_WIN32)
    // MSVC x64 calling convention: rcx = first arg, shadow space required
    // push rcx                ; 0x51
    p[0] = 0x51;
    // push rdx                ; 0x52
    p[1] = 0x52;
    // push r8                 ; 0x41 0x50
    p[2] = 0x41;
    p[3] = 0x50;
    // push r9                 ; 0x41 0x51
    p[4] = 0x41;
    p[5] = 0x51;
    // sub rsp, 0x28           ; 40 bytes (32 shadow + 8 alignment)
    p[6] = 0x48;
    p[7] = 0x83;
    p[8] = 0xEC;
    p[9] = 0x28;
    // mov rcx, r10            ; 0x4C 0x89 0xD1 — first arg = precode ptr
    p[10] = 0x4C;
    p[11] = 0x89;
    p[12] = 0xD1;
    // mov rax, <dispatch>     ; 0x48 0xB8 + 8B addr
    auto dispatch_addr = reinterpret_cast<uintptr_t>(&JitStubDispatchImpl);
    p[13] = 0x48;
    p[14] = 0xB8;
    std::memcpy(p + 15, &dispatch_addr, sizeof(dispatch_addr));
    // call rax                ; 0xFF 0xD0
    p[23] = 0xFF;
    p[24] = 0xD0;
    // add rsp, 0x28           ; 0x48 0x83 0xC4 0x28
    p[25] = 0x48;
    p[26] = 0x83;
    p[27] = 0xC4;
    p[28] = 0x28;
    // pop r9                  ; 0x41 0x59
    p[29] = 0x41;
    p[30] = 0x59;
    // pop r8                  ; 0x41 0x58
    p[31] = 0x41;
    p[32] = 0x58;
    // pop rdx                 ; 0x5A
    p[33] = 0x5A;
    // pop rcx                 ; 0x59
    p[34] = 0x59;
    // jmp rax                 ; 0xFF 0xE0
    p[35] = 0xFF;
    p[36] = 0xE0;

    // ── Padding to 4-byte alignment (offsets 37-39) ────────────────────
    // Win64 requires UNWIND_INFO to be DWORD-aligned (4 bytes).
    // Code ends at offset 36; pad to offset 40.
    p[37] = 0xCC;
    p[38] = 0xCC;
    p[39] = 0xCC;

    // ── UNWIND_INFO for .pdata registration (offsets 40-55) ─────────
    // Required so the OS can unwind through this non-leaf trampoline
    // when JIT-compiled code crashes.  Without .pdata, SEH unwind
    // triggers STATUS_INVALID_CRUNTIME_PARAMETER recursion.
    //
    // Prologue (10 bytes):
    //   push rcx       ; offset 0, 1 byte
    //   push rdx       ; offset 1, 1 byte
    //   push r8        ; offset 2, 2 bytes
    //   push r9        ; offset 4, 2 bytes
    //   sub rsp, 0x28  ; offset 6, 4 bytes
    // Version=1, Flags=0, SizeOfProlog=10 bytes, 5 codes, no FP
    p[40] = 0x01; // version_flags
    p[41] = 10;   // size_of_prolog (bytes: push rcx+rdx+r8+r9 + sub rsp,0x28 = 10)
    p[42] = 5;    // count_of_codes
    p[43] = 0x00; // frame_register=RSP, frame_offset=0

    // 5 UNWIND_CODE entries (reverse prologue order, 2 bytes each):
    p[44] = 6;
    p[45] = 0x24; // UWOP_ALLOC_SMALL(4):  sub rsp, 0x28
    p[46] = 4;
    p[47] = 0x09; // UWOP_PUSH_NONVOL(9):  push r9
    p[48] = 2;
    p[49] = 0x08; // UWOP_PUSH_NONVOL(8):  push r8
    p[50] = 1;
    p[51] = 0x02; // UWOP_PUSH_NONVOL(2):  push rdx
    p[52] = 0;
    p[53] = 0x01; // UWOP_PUSH_NONVOL(1):  push rcx

    // Pad to 4-byte boundary (14 bytes → 2 pad)
    p[54] = 0x00;
    p[55] = 0x00;
#else
    // System V AMD64 calling convention: rdi = first arg, no shadow space
    // mov rdi, r10            ; 0x4C 0x89 0xD7 — first arg = precode ptr
    p[0] = 0x4C;
    p[1] = 0x89;
    p[2] = 0xD7;
    // mov rax, <dispatch>     ; 0x48 0xB8 + 8B addr
    auto dispatch_addr = reinterpret_cast<uintptr_t>(&JitStubDispatchImpl);
    p[3] = 0x48;
    p[4] = 0xB8;
    std::memcpy(p + 5, &dispatch_addr, sizeof(dispatch_addr));
    // call rax                ; 0xFF 0xD0
    p[13] = 0xFF;
    p[14] = 0xD0;
    // jmp rax                 ; 0xFF 0xE0
    p[15] = 0xFF;
    p[16] = 0xE0;
#endif

    jit_entry_size_ = kSharedEntrySize;
    pg.pos += kSharedEntrySize;
}

void* PrecodeArena::AllocateJitTrampoline(JitPrecode* precode) noexcept {
    EnsurePage();
    if (pages_.empty())
        return nullptr;

    auto& pg = pages_.back();
    uint32_t offset = pg.pos;
    uint8_t* p = pg.base + offset;

#if defined(__aarch64__)
    // ARM64 trampoline (16 bytes):
    //   LDR X17, [PC, #8]     ; 4B — load precode address from literal pool
    //   BR X17                 ; 4B — jump to shared entry via X17
    //   <8-byte literal>       ; 8B — precode address
    auto precode_addr = reinterpret_cast<uintptr_t>(precode);
    // LDR X17, literal, imm19=2 (offset 8 bytes)
    p[0] = 0x51;
    p[1] = 0x00;
    p[2] = 0x80;
    p[3] = 0x58;
    // BR X17
    p[4] = 0x20;
    p[5] = 0x02;
    p[6] = 0x1F;
    p[7] = 0xD6;
    // Literal pool
    std::memcpy(p + 8, &precode_addr, sizeof(precode_addr));
#else
    // mov r10, <precode_addr>  ; 0x49 0xBA + 8B addr  (10 bytes)
    auto precode_addr = reinterpret_cast<uintptr_t>(precode);
    p[0] = 0x49;
    p[1] = 0xBA;
    std::memcpy(p + 2, &precode_addr, sizeof(precode_addr));

    // jmp <jit_shared_entry>   ; 0xE9 + rel32  (5 bytes)
    int32_t rel32 = static_cast<int32_t>(jit_entry_offset_) - static_cast<int32_t>(offset + 15);
    p[10] = 0xE9;
    std::memcpy(p + 11, &rel32, sizeof(rel32));
#endif

    pg.pos += kTrampolineSize;

    // Flush I-cache so ARM64 sees the newly written trampoline instructions.
    // No-op on x86 (hardware I-cache coherency).
    chaos::il2cpp::pal::PalFlushInstructionCache(pg.base + offset, kTrampolineSize);

    return pg.base + offset;
}

// ── RegisterPagePdata ────────────────────────────────────────────────────
// Registers .pdata for the shared entry on a PrecodeArena RWX page.
// This enables OS stack unwinding through the shared entry frame when a
// JIT-compiled method crashes, preventing STATUS_INVALID_CRUNTIME_PARAMETER
// recursion during SEH unwind.
#if defined(_WIN64)
static void RegisterPagePdata(uint8_t* page_base, void*& out_runtime_function, uint32_t entry_size) noexcept {
    auto* rf = static_cast<PRUNTIME_FUNCTION>(std::malloc(sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY)));
    if (!rf) {
        CHAOS_IL2CPP_LOG_ERROR_M("jit", "RegisterPagePdata: malloc failed");
        return;
    }
    rf->BeginAddress = 0;
    rf->EndAddress = entry_size;
    rf->UnwindInfoAddress = kSharedEntryUnwindOffset;
    out_runtime_function = rf;

    if (!RtlAddFunctionTable(rf, 1, reinterpret_cast<DWORD64>(page_base))) {
        CHAOS_IL2CPP_LOG_ERROR_M("jit", "RegisterPagePdata: RtlAddFunctionTable FAILED for page={}", (void*)page_base);
    } else {
        CHAOS_IL2CPP_LOG_INFO_M("jit", "RegisterPagePdata: .pdata registered for page={}, base={}", (void*)page_base,
                                (void*)page_base);
    }
}
#endif

// ── CompileWithCatch ───────────────────────────────────────────────────
// Wraps Compile() in SEH __try/__except so callers don't need C++ object
// unwinding in their SEH blocks (avoids MSVC C2712).
// Returns nullptr on managed exception during compilation.
static JitMethod* CompileWithCatch(const interpreter::RegisterMethod& ir, const CompileConfig& config) noexcept {
    JitMethod* jit = nullptr;
    CHAOS_EH_TRY
    jit = Compile(ir, config);
    CHAOS_EH_CATCH_BEGIN
    jit = nullptr;
    CHAOS_EH_END
    return jit;
}

// ── SafeCompileWithCatch ──────────────────────────────────────────────
// Wraps Compile() in raw SEH __try/__except to catch access violations
// during compilation.  The CHAOS_EH filter doesn't catch AVs, so we use
// a catch-all handler.  Returns nullptr on any SEH during compilation.
// Separate function to avoid MSVC C2712 (__try in function with unwind).
static JitMethod* SafeCompileWithCatch(const interpreter::RegisterMethod& ir, const CompileConfig& config) noexcept {
#if defined(_WIN32)
    JitMethod* jit = nullptr;
    __try {
        jit = Compile(ir, config);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        jit = nullptr;
    }
    return jit;
#else
    return Compile(ir, config);
#endif
}

// ── JitStubDispatchImpl ───────────────────────────────────────────────
//
// State machine:
//   kPrecodeUncompiled(0) → CAS → kPrecodeCompiling(1) → Compile → kPrecodeCompiled(2)
//   Losers spin-wait on kPrecodeCompiling, then return compiled->code.
//
// This function is only for JitPrecode dispatch.
extern "C" void* JitStubDispatchImpl(JitPrecode* precode) noexcept {
    // Fast relaxed check: already compiled?
    uint32_t s = precode->state.load(std::memory_order_acquire);
    if (s == kPrecodeCompiled) {
        // After DP1-a transfer (TransferPrecodeOwnership), precode->compiled
        // is null — the compiled JitMethod was handed off to the tier system.
        // Return original AOT code as fallback.  The direct_ptr has been
        // patched to JIT code, so this is only a race window.
        if (!precode->compiled) {
            return precode->original_direct_ptr;
        }

        // Hot-update stale check: if a callee was hotpatched after being inlined
        // into this method, the stale flag is set by InlineReverseMap.  Trigger
        // recompilation by resetting state to kPrecodeUncompiled.
        if (precode->compiled->stale.exchange(false)) {
            // CAS: Compiled → Uncompiled.  If another thread already grabbed
            // the recompilation slot, this CAS fails and we just re-read state.
            uint32_t expected_compiled = kPrecodeCompiled;
            precode->state.compare_exchange_strong(expected_compiled, kPrecodeUncompiled, std::memory_order_acq_rel);
            s = precode->state.load(std::memory_order_relaxed);
            // Fall through to the compile path below
        } else {
            // PGO: increment call counter when profiling is enabled
            if (precode->config.enable_pgo) {
                uint32_t count = precode->pgo_call_count.fetch_add(1, std::memory_order_relaxed);
                if (count > kPgoFullJitThreshold && !precode->tier1_enqueued) {
                    precode->tier1_enqueued = true;
                    runtime_core::TierManager::Get().EnqueueJitRecompilation(precode);
                }
            }
            return precode->compiled->code;
        }
    }

    // Attempt to become the compiler: Uncompiled → Compiling
    uint32_t expected = kPrecodeUncompiled;
    if (precode->state.compare_exchange_strong(expected, kPrecodeCompiling, std::memory_order_acq_rel)) {
        // Winner — compile the method
        void* fallback = precode->original_direct_ptr;
        JitMethod* jit = SafeCompileWithCatch(precode->ir, precode->config);
        if (!jit) {
            // Compilation failed — restore original direct_ptr (AOT code)
            // so future calls go directly to AOT without crashing.
            if (precode->entry && fallback) {
                precode->entry->direct_ptr = fallback;
            }
            precode->state.store(kPrecodeUncompiled, std::memory_order_release);
            CHAOS_IL2CPP_LOG_WARN_M("jit", "JitStubDispatchImpl: Compile() failed, falling back to AOT");
            return fallback;
        }

        precode->compiled = jit;

        // ── Register GC slot map and native code section ──────────
        // So the GC can find managed pointers on the stack, and SEH
        // can map fault addresses back to JITMethod for EH handling.
        if (jit->gc_slot_map) {
            chaos::il2cpp::runtime_core::GcRegisterSlotMap(jit->code, jit->gc_slot_map);
        }
        chaos::il2cpp::jit::RegisterNativeCodeSection(jit->code, jit->code_size, jit);

        // Atomically patch the HotpatchEntryV0::direct_ptr so future calls
        // skip the trampoline entirely and go straight to the compiled code.
        // Entry may be null in test scenarios; skip patching if so.
        // When enable_pgo is true, do NOT patch — calls continue through the
        // dispatch function so we can count calls and trigger Tier 1 recompilation.
        if (precode->entry && !precode->config.enable_pgo) {
            precode->entry->direct_ptr = jit->code;
        }

        // Release-store: make all prior writes visible to other threads
        precode->state.store(kPrecodeCompiled, std::memory_order_release);

        return jit->code;
    }

    // CAS failed — someone else is compiling (or it's already done)
    if (expected == kPrecodeCompiling) {
        // Spin-wait with pause/yield (bounded: ~1s timeout).
        int spins = 0;
        constexpr int kSpinTimeout = 100000000;
        while (precode->state.load(std::memory_order_acquire) == kPrecodeCompiling) {
#if defined(__aarch64__)
            __asm__ __volatile__("yield" ::: "memory");
#else
            _mm_pause();
#endif
            if (++spins > 1000) {
                std::this_thread::yield();
                if (spins > kSpinTimeout + 1000) {
                    // Compiler thread appears crashed (state stuck at Compiling).
                    // Fall back to original AOT code instead of spinning forever.
                    CHAOS_IL2CPP_LOG_WARN_M("jit",
                                            "JitStubDispatchImpl: spin-timeout on precode %p, "
                                            "falling back to AOT",
                                            static_cast<void*>(precode));
                    return precode->original_direct_ptr;
                }
                spins = 1000; // keep yielding, don't reset
            }
        }
    }

    // State is now kPrecodeCompiled (or we'd have timed out above).
    // After DP1-a transfer, precode->compiled may be null — fall back
    // to original AOT code if so.
    if (precode->compiled) {
        return precode->compiled->code;
    }
    return precode->original_direct_ptr;
}

// ── PrecodeArena (file-level static, no MSVC thread-safe init guard) ──────
// Function-local static would trigger _Init_thread_header TLS access (crash).
// File-level static initializes during CRT static init (single-threaded).
static PrecodeArena s_precode_arena;

// ── Original AOT direct_ptr side-map ─────────────────────────────────────
// Maps HotpatchEntryV0* → original AOT function pointer (saved before JIT
// trampoline installation).  Used by ResolveDirectFn during patch IR lowering
// so it gets the real AOT code pointer instead of the JIT trampoline.
// Key is a pointer → identity hash (no wyhash overhead).
static CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(void*, void*) g_original_aot_map;

// Callback registered via SetOriginalAotPtrCallback.  Returns the original AOT
// function pointer for a dispatch entry, or nullptr if this entry was never
// replaced by JIT (i.e., runs in pure AOT mode).
static void* GetOriginalAotPtr(HotpatchEntryV0* entry) noexcept {
    if (!entry)
        return nullptr;
    auto it = g_original_aot_map.find(static_cast<void*>(entry));
    if (it != g_original_aot_map.end())
        return it->second;
    return nullptr;
}

// ── JitPrecode side-map ──────────────────────────────────────────────────
// Maps HotpatchEntryV0* → JitPrecode* for DP1-a ownership transfer.
// Populated during RegisterJitEntryMethods.
static CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(void*, void*) g_precode_side_map;

// DP1-a: Atomically transfer precode-compiled JitMethod ownership to the tier
// system.  Called from SetPatchedBySlot when a method is hotpatch-activated.
// Returns true if the transfer succeeded (method_key kJitted ready), false if
// no transfer (no JitPrecode, not yet compiled, or keep-native).
// Thread-safety: atomic exchange on precode->compiled ensures the precode
// system loses ownership.  Precode's JitStubDispatchImpl will detect
// precode->compiled == nullptr and fall back to original_direct_ptr.
static bool TransferPrecodeOwnership(HotpatchEntryV0* entry, void* method_key) noexcept {
    if (!entry || !method_key)
        return false;
    auto it = g_precode_side_map.find(static_cast<void*>(entry));
    if (it == g_precode_side_map.end())
        return false;

    auto* precode = static_cast<JitPrecode*>(it->second);
    if (!precode)
        return false;

    // acquire-load to synchronize with JitStubDispatchImpl's release store.
    auto state = precode->state.load(std::memory_order_acquire);
    if (state != kPrecodeCompiled)
        return false; // not yet compiled

    // Atomic exchange: take ownership of compiled JitMethod.
    // compiled is a raw JitMethod* (not std::atomic), so use platform atomic builtins.
#if defined(_MSC_VER)
    auto* jit = static_cast<JitMethod*>(
        _InterlockedExchangePointer(reinterpret_cast<void* volatile*>(&precode->compiled), nullptr));
#else
    auto* jit = __atomic_exchange_n(&precode->compiled, nullptr, __ATOMIC_ACQUIRE);
#endif
    if (!jit)
        return false; // another thread already took it

    auto* pm = static_cast<chaos::il2cpp::runtime_core::PatchMethod*>(method_key);
    pm->cached_native_method = jit;
    entry->direct_ptr = jit->code;
    // Release store on tier_state (after direct_ptr write) ensures the
    // direct_ptr write is visible to any thread that load-acquires tier_state.
    pm->tier_state.store(chaos::il2cpp::runtime_core::PatchMethod::kJitted, std::memory_order_release);

    // Remove from side-map so JitRecompileToTier1 skips this entry.
    g_precode_side_map.erase(it);
    return true;
}

// ── RegisterJitEntryMethods ───────────────────────────────────────────────
// Called once at startup (from runtime-entry.cpp) to register all methods
// for JIT compilation via precode dispatch.  For each entry:
//   1. Resolve JSON data: either from .jdata file offset or embedded string
//   2. Deserialize AotCoreIr JSON → IRMethod
//   3. AllocateRegisters → RegisterMethod
//   4. Heap-allocate JitPrecode with the RegisterMethod + CompileConfig
//   5. Allocate a PrecodeArena trampoline for the JitPrecode
//   6. Set HotpatchEntryV0::direct_ptr to the trampoline
//
// When jit_data is provided, each entry's json_offset + json_len points into
// the loaded .jdata file content. When jit_data is nullptr, old-style embedded
// string entries are not supported — all new codegen output uses .jdata files.
//
// After this call, first invocation of each method triggers JitStubDispatchImpl
// which calls Compile() and atomically replaces direct_ptr with compiled code.
extern "C" void RegisterJitEntryMethods(const JitEntry* entries, uint32_t count, const char* jit_data) noexcept {
    using namespace chaos::il2cpp::interpreter;
    using namespace chaos::il2cpp::runtime_core;

    for (uint32_t i = 0; i < count; ++i) {
        const auto& entry = entries[i];

        // Step 1: Resolve the JSON data.
        // When jit_data is provided, entries use file-offset-based addressing:
        //   entry.json_offset + entry.json_len point into the .jdata blob.
        // When jit_data is nullptr, fall back to the old embedded-string format
        //   where entry.json was originally a const char* pointer (backward compat
        //   with pre-existing test files). Since we changed JitEntry to store
        //   uint32_t json_offset, the old string-pointer entries are not supported
        //   — all new codegen output uses the .jdata file format.
        const char* json_ptr;
        if (jit_data && entry.json_offset < UINT32_MAX) {
            // New .jdata file format: json_offset is a byte offset into jit_data.
            json_ptr = jit_data + static_cast<size_t>(entry.json_offset);
        } else {
            CHAOS_IL2CPP_LOG_ERROR_M("jit", "RegisterJitEntryMethods: no jit_data provided for token 0x{:x} module {}",
                                     entry.token, entry.module_id);
            continue;
        }

        // Step 2: Deserialize AotCoreIr JSON → IRMethod
        auto ir = DeserializeAotCoreIrMethod(json_ptr, entry.json_len, nullptr, nullptr, nullptr, nullptr);

        // Step 3: Allocate registers → RegisterMethod (independent copy)
        auto rm = AllocateRegisters(ir);
        if (rm.instructions.empty()) {
            CHAOS_IL2CPP_LOG_WARN_M("jit", "RegisterJitEntryMethods: skipping token 0x{} (empty IR — keep AOT path)",
                                    entry.token);
            continue;
        }
        // ir goes out of scope — vectors auto-clean

        // Step 4: Heap-allocate JitPrecode (lives for program lifetime)
        auto* precode = new JitPrecode();
        precode->ir = std::move(rm);
        precode->config = CompileConfig {};
        precode->config.enable_pgo = true; // profile calls → trigger Tier 1 recompilation

        // Step 5: Look up the HotpatchEntryV0 for this method
        precode->entry = GetHotpatchNameRegistry().GetDispatchEntry(entry.module_id, entry.token);

        // Skip keep-native methods (Subject_N/CustomEntrySubject_N): their
        // direct_ptr must remain pointing to the original AOT function body.
        // Replacing it with a JIT trampoline would break benchmark dispatch
        // paths (ChaosDispatchMethodBenchDirect) that rely on direct_ptr for
        // direct AOT body calls.
        if (precode->entry && HotpatchShouldKeepNative(*precode->entry)) {
            delete precode;
            continue;
        }

        // Step 6: Allocate trampoline from the RWX arena
        precode->trampoline = s_precode_arena.AllocateJitTrampoline(precode);

        // Step 7: Point direct_ptr at the trampoline.
        // First call goes trampoline → JitStubEntry → JitStubDispatchImpl
        // → Compile() → direct_ptr atomically replaced with compiled code.
        // Save the original direct_ptr (AOT code) in case Compile() fails
        // and we need to fall back to AOT execution.
        if (precode->entry && precode->trampoline) {
            precode->original_direct_ptr = precode->entry->direct_ptr;
            precode->entry->direct_ptr = precode->trampoline;
            // Register in side-map so ResolveDirectFn can find the original
            // AOT pointer when resolving call targets during patch IR lowering.
            g_original_aot_map[static_cast<void*>(precode->entry)] = precode->original_direct_ptr;
            // Register in precode side-map for DP1-a ownership transfer.
            g_precode_side_map[static_cast<void*>(precode->entry)] = precode;
        } else {
            CHAOS_IL2CPP_LOG_ERROR_M("jit", "RegisterJitEntryMethods: failed for token 0x{:x} module {}", entry.token,
                                     entry.module_id);
        }
    }

    // Register the slot update callback (idempotent — only sets once).
    // When SetPatchedBySlot bumps version and fires the callback, this
    // updates all JIT slot tables to point at the new direct_ptr and
    // invalidates any callers that inlined the patched method.
    RegisterSlotUpdateCallback([](uint32_t callee_token, void* new_direct_ptr, HotpatchEntryV0* callee_entry) {
        // Update non-inlined call-site slots
        g_reverse_slot_map.UpdateAll(callee_token, new_direct_ptr);
        // Invalidate callers that inlined this method (version mismatch → stale)
        g_inline_reverse_map.InvalidateCallers(callee_token, callee_entry);
    });

    // Register the original AOT ptr resolver so ResolveDirectFn (used during
    // patch IR lowering) gets the real AOT code pointer, not the JIT trampoline.
    // In AOT mode, this callback returns nullptr and entry->direct_ptr is used as-is.
    SetOriginalAotPtrCallback(GetOriginalAotPtr);

    // Register the JitPrecode ownership transfer function for DP1-a.
    // SetPatchedBySlot calls this to transfer precode-compiled JitMethod
    // ownership to the tier system when a method is hotpatch-activated.
    SetPrecodeTransferCallback(TransferPrecodeOwnership);
}

// ── JitRecompileToTier1 ─────────────────────────────────────────────────
// Called by the TierManager background thread to recompile a method with the
// full Tier 1 pipeline.  Replaces precode->compiled with the new JitMethod
// and returns the new entry point.  The caller (TierManager) handles patching
// direct_ptr after this returns.
//
// Thread-safety: only called from the single TierManager background thread,
// so there is no contention on the precode fields.
extern "C" void* JitRecompileToTier1(JitPrecode* precode) noexcept {
    using namespace chaos::il2cpp::interpreter;

    CompileConfig tier1_cfg = precode->config;
    tier1_cfg.compile_tier = CompileTier::kFull;
    tier1_cfg.enable_pgo = false; // Tier 1 is final — no further profiling
    auto* jit = Compile(precode->ir, tier1_cfg);
    if (!jit) {
        CHAOS_IL2CPP_LOG_ERROR_M("jit", "JitRecompileToTier1: Compile(Tier1) failed");
        return nullptr;
    }
    // ── Retire the old JitMethod (deferred free via deferred-delete queue) ──
    // Cannot delete immediately: another thread may still be executing the old
    // compiled code or accessing its metadata (slot map, deopt entries).
    // Push to a static deferred-delete queue instead.  The queue is drained
    // periodically (every 1024 pushes) to bound memory growth.
    // M4: mutex-guarded for thread safety (TierManager background thread).
    if (precode->retired) {
        static std::vector<JitMethod*> s_deferred_delete;
        static size_t s_deferred_count = 0;
        static std::mutex s_deferred_mutex;
        {
            std::lock_guard<std::mutex> lock(s_deferred_mutex);
            s_deferred_delete.push_back(precode->retired);
            if (++s_deferred_count >= 1024) {
                for (auto* p : s_deferred_delete)
                    delete p;
                s_deferred_delete.clear();
                s_deferred_count = 0;
            }
        }
    }
    precode->retired = precode->compiled;
    precode->compiled = jit;
    return jit->code;
}

} // namespace chaos::il2cpp::jit
