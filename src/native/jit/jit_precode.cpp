// jit_precode.cpp — JitPrecode / HybridPrecode stub dispatch implementation
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
#include "jit_inline.h"            // g_inline_reverse_map

#include <codegen_bridge.h>   // HotpatchEntryV0
#include <aot_core_ir_reader.h>  // DeserializeAotCoreIrMethod
#include <hotpatch_table.h>      // GetHotpatchNameRegistry
#include <ir_reg_alloc.h>        // AllocateRegisters
#include <jit_registration.h>    // JitEntry
#include <tier_manager.h>        // TierManager::EnqueueJitRecompilation
#include <slot_map.h>            // ReverseSlotMap, g_reverse_slot_map
#include <chaos/log.h>           // CHAOS_IL2CPP_LOG_ERROR_M

#if defined(_WIN32) || defined(_WIN64)
  #define NOMINMAX
  #include <windows.h>
  #include <intrin.h>
  #include <exception_jmp.h>     // CHAOS_SEH_FILTER_ALL (needs DWORD from windows.h)
#endif

#include <chaos/eh.h>            // CHAOS_EH_TRY / CHAOS_EH_CATCH_BEGIN

#include <cstdlib>
#include <cstring>
#include <thread>

#if !defined(_WIN32) && !defined(_WIN64)
  #include <sys/mman.h>
  #include <unistd.h>
  #include <immintrin.h>
#endif

namespace chaos::il2cpp::jit {

// ── Constants ──────────────────────────────────────────────────────────
static constexpr uint32_t kPageSize        = 64 * 1024;  // 64KB per RWX page
static constexpr uint32_t kSharedEntrySize = 64;          // reserved (actual ~37 B)
static constexpr uint32_t kTrampolineSize  = 15;          // per-method stub

// ── Per-page descriptor ───────────────────────────────────────────────
struct Page {
    uint8_t* base;       // RWX memory base
    uint32_t pos;        // next free offset from base
    uint32_t capacity;   // total page size
};

// ── PrecodeArena ──────────────────────────────────────────────────────

PrecodeArena::PrecodeArena() noexcept {
}

PrecodeArena::~PrecodeArena() noexcept {
    for (auto& pg : pages_) {
        if (pg.base) {
#if defined(_WIN32) || defined(_WIN64)
            VirtualFree(pg.base, 0, MEM_RELEASE);
#else
            munmap(pg.base, pg.capacity);
#endif
        }
    }
}

void PrecodeArena::EnsurePage() noexcept {
    if (!pages_.empty()) {
        auto& last = pages_.back();
        if (last.pos + kTrampolineSize <= last.capacity)
            return;  // room on current page
    }

    // Allocate a new RWX page
    uint8_t* base = nullptr;
#if defined(_WIN32) || defined(_WIN64)
    base = static_cast<uint8_t*>(
        VirtualAlloc(nullptr, kPageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
#else
    base = static_cast<uint8_t*>(
        mmap(nullptr, kPageSize, PROT_READ | PROT_WRITE | PROT_EXEC,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (base == MAP_FAILED) base = nullptr;
#endif
    if (!base) {
        CHAOS_IL2CPP_LOG_ERROR_M("jit", "PrecodeArena: failed to allocate RWX page");
        return;
    }

    Page pg;
    pg.base     = base;
    pg.pos      = 0;
    pg.capacity = kPageSize;
    pages_.push_back(pg);

    // Emit both shared entries (JIT + Hybrid) at the start of every new page.
    // First page emits the entries; subsequent pages copy from page 0.
    if (!entries_emitted_) {
        EmitJitSharedEntry();
        EmitHybridSharedEntry();
        entries_emitted_ = true;
    } else {
        uint32_t total_header = jit_entry_size_ + hybrid_entry_size_;
        std::memcpy(base, pages_[0].base, total_header);
        pages_.back().pos = total_header;
    }
}

void PrecodeArena::EmitJitSharedEntry() noexcept {
    if (pages_.empty()) return;
    auto& pg = pages_.back();
    jit_entry_offset_ = pg.pos;

    uint8_t* p = pg.base + pg.pos;

    // push rcx                ; 0x51
    p[0] = 0x51;

    // push rdx                ; 0x52
    p[1] = 0x52;

    // push r8                 ; 0x41 0x50
    p[2] = 0x41; p[3] = 0x50;

    // push r9                 ; 0x41 0x51
    p[4] = 0x41; p[5] = 0x51;

    // sub rsp, 0x20           ; 0x48 0x83 0xEC 0x20
    p[6] = 0x48; p[7] = 0x83; p[8] = 0xEC; p[9] = 0x20;

    // mov rcx, r10            ; 0x4C 0x89 0xD1
    p[10] = 0x4C; p[11] = 0x89; p[12] = 0xD1;

    // mov rax, <dispatch>     ; 0x48 0xB8 + 8B addr
    auto dispatch_addr = reinterpret_cast<uintptr_t>(&JitStubDispatchImpl);
    p[13] = 0x48; p[14] = 0xB8;
    std::memcpy(p + 15, &dispatch_addr, sizeof(dispatch_addr));

    // call rax                ; 0xFF 0xD0
    p[23] = 0xFF; p[24] = 0xD0;

    // add rsp, 0x20           ; 0x48 0x83 0xC4 0x20
    p[25] = 0x48; p[26] = 0x83; p[27] = 0xC4; p[28] = 0x20;

    // pop r9                  ; 0x41 0x59
    p[29] = 0x41; p[30] = 0x59;

    // pop r8                  ; 0x41 0x58
    p[31] = 0x41; p[32] = 0x58;

    // pop rdx                 ; 0x5A
    p[33] = 0x5A;

    // pop rcx                 ; 0x59
    p[34] = 0x59;

    // jmp rax                 ; 0xFF 0xE0
    p[35] = 0xFF; p[36] = 0xE0;

    jit_entry_size_ = kSharedEntrySize;
    pg.pos += kSharedEntrySize;
}

void PrecodeArena::EmitHybridSharedEntry() noexcept {
    if (pages_.empty()) return;
    auto& pg = pages_.back();
    hybrid_entry_offset_ = pg.pos;

    uint8_t* p = pg.base + pg.pos;

    // push rcx                ; 0x51
    p[0] = 0x51;

    // push rdx                ; 0x52
    p[1] = 0x52;

    // push r8                 ; 0x41 0x50
    p[2] = 0x41; p[3] = 0x50;

    // push r9                 ; 0x41 0x51
    p[4] = 0x41; p[5] = 0x51;

    // sub rsp, 0x20           ; 0x48 0x83 0xEC 0x20
    p[6] = 0x48; p[7] = 0x83; p[8] = 0xEC; p[9] = 0x20;

    // mov rcx, r10            ; 0x4C 0x89 0xD1
    p[10] = 0x4C; p[11] = 0x89; p[12] = 0xD1;

    // mov rax, <dispatch>     ; 0x48 0xB8 + 8B addr
    auto dispatch_addr = reinterpret_cast<uintptr_t>(&HybridStubDispatchImpl);
    p[13] = 0x48; p[14] = 0xB8;
    std::memcpy(p + 15, &dispatch_addr, sizeof(dispatch_addr));

    // call rax                ; 0xFF 0xD0
    p[23] = 0xFF; p[24] = 0xD0;

    // add rsp, 0x20           ; 0x48 0x83 0xC4 0x20
    p[25] = 0x48; p[26] = 0x83; p[27] = 0xC4; p[28] = 0x20;

    // pop r9                  ; 0x41 0x59
    p[29] = 0x41; p[30] = 0x59;

    // pop r8                  ; 0x41 0x58
    p[31] = 0x41; p[32] = 0x58;

    // pop rdx                 ; 0x5A
    p[33] = 0x5A;

    // pop rcx                 ; 0x59
    p[34] = 0x59;

    // jmp rax                 ; 0xFF 0xE0
    p[35] = 0xFF; p[36] = 0xE0;

    hybrid_entry_size_ = kSharedEntrySize;
    pg.pos += kSharedEntrySize;
}

void* PrecodeArena::AllocateJitTrampoline(JitPrecode* precode) noexcept {
    EnsurePage();
    if (pages_.empty()) return nullptr;

    auto& pg = pages_.back();
    uint32_t offset = pg.pos;
    uint8_t* p = pg.base + offset;

    // mov r10, <precode_addr>  ; 0x49 0xBA + 8B addr  (10 bytes)
    auto precode_addr = reinterpret_cast<uintptr_t>(precode);
    p[0] = 0x49; p[1] = 0xBA;
    std::memcpy(p + 2, &precode_addr, sizeof(precode_addr));

    // jmp <jit_shared_entry>   ; 0xE9 + rel32  (5 bytes)
    int32_t rel32 = static_cast<int32_t>(jit_entry_offset_) - static_cast<int32_t>(offset + 15);
    p[10] = 0xE9;
    std::memcpy(p + 11, &rel32, sizeof(rel32));

    pg.pos += kTrampolineSize;
    return pg.base + offset;
}

void* PrecodeArena::AllocateHybridTrampoline(HybridPrecode* precode) noexcept {
    // Hybrid uses the same 15-byte trampoline layout as JIT.
    // The trampoline jumps to the Hybrid shared entry, which calls
    // HybridStubDispatchImpl.  HybridStubDispatchImpl decrements the
    // call_counter and triggers JIT compilation when the counter reaches 0,
    // returning aot_entry until JIT code is ready.
    EnsurePage();
    if (pages_.empty()) return nullptr;

    auto& pg = pages_.back();
    uint32_t offset = pg.pos;
    uint8_t* p = pg.base + offset;

    // mov r10, <precode_addr>  ; 0x49 0xBA + 8B addr  (10 bytes)
    auto precode_addr = reinterpret_cast<uintptr_t>(precode);
    p[0] = 0x49; p[1] = 0xBA;
    std::memcpy(p + 2, &precode_addr, sizeof(precode_addr));

    // jmp <hybrid_shared_entry> ; 0xE9 + rel32  (5 bytes)
    int32_t rel32 = static_cast<int32_t>(hybrid_entry_offset_) - static_cast<int32_t>(offset + 15);
    p[10] = 0xE9;
    std::memcpy(p + 11, &rel32, sizeof(rel32));

    pg.pos += kTrampolineSize;
    return pg.base + offset;
}

// ── CompileWithCatch ───────────────────────────────────────────────────
// Wraps Compile() in SEH __try/__except so callers don't need C++ object
// unwinding in their SEH blocks (avoids MSVC C2712).
// Returns nullptr on managed exception during compilation.
static JitMethod* CompileWithCatch(const interpreter::RegisterMethod& ir,
                                    const CompileConfig& config) noexcept {
    JitMethod* jit = nullptr;
    CHAOS_EH_TRY
        jit = Compile(ir, config);
    CHAOS_EH_CATCH_BEGIN
        jit = nullptr;
    CHAOS_EH_END
    return jit;
}

// ── JitStubDispatchImpl ───────────────────────────────────────────────
//
// State machine:
//   kPrecodeUncompiled(0) → CAS → kPrecodeCompiling(1) → Compile → kPrecodeCompiled(2)
//   Losers spin-wait on kPrecodeCompiling, then return compiled->code.
//
// HybridPrecode has a separate dispatch entry (HybridStubDispatchImpl) with
// counter-based logic. This function is only for JitPrecode.
extern "C" void* JitStubDispatchImpl(JitPrecode* precode) noexcept {
    // Fast relaxed check: already compiled?
    uint32_t s = precode->state.load(std::memory_order_relaxed);
    if (s == kPrecodeCompiled) {
        // Hot-update stale check: if a callee was hotpatched after being inlined
        // into this method, the stale flag is set by InlineReverseMap.  Trigger
        // recompilation by resetting state to kPrecodeUncompiled.
        if (precode->compiled && precode->compiled->stale.exchange(false)) {
            // CAS: Compiled → Uncompiled.  If another thread already grabbed
            // the recompilation slot, this CAS fails and we just re-read state.
            uint32_t expected_compiled = kPrecodeCompiled;
            precode->state.compare_exchange_strong(expected_compiled, kPrecodeUncompiled,
                                                    std::memory_order_acq_rel);
            s = precode->state.load(std::memory_order_relaxed);
            // Fall through to the compile path below
        } else {
            // PGO: increment call counter when profiling is enabled
            if (precode->config.enable_pgo) {
                uint32_t count = precode->pgo_call_count.fetch_add(1, std::memory_order_relaxed);
                if (count > kPgoFullJitThreshold && !precode->tier1_enqueued) {
                    precode->tier1_enqueued = true;
                    runtime_core::TierManager::Get().EnqueueJitRecompilation(precode, false);
                }
            }
            return precode->compiled->code;
        }
    }

    // Attempt to become the compiler: Uncompiled → Compiling
    uint32_t expected = kPrecodeUncompiled;
    if (precode->state.compare_exchange_strong(expected, kPrecodeCompiling,
                                                std::memory_order_acq_rel)) {
        // Winner — compile the method
        JitMethod* jit = CompileWithCatch(precode->ir, precode->config);
        if (!jit) {
            // Compilation failed — reset state so we can retry
            precode->state.store(kPrecodeUncompiled, std::memory_order_release);
            CHAOS_IL2CPP_LOG_ERROR_M("jit",
                "JitStubDispatchImpl: Compile() failed for method");
            return nullptr;
        }

        precode->compiled = jit;

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
        // Spin-wait with pause/yield
        int spins = 0;
        while (precode->state.load(std::memory_order_acquire) == kPrecodeCompiling) {
#if defined(_WIN32) || defined(_WIN64)
            _mm_pause();
#else
            _mm_pause();
#endif
            if (++spins > 1000) {
                std::this_thread::yield();
                spins = 0;
            }
        }
    }

    // State is now kPrecodeCompiled (or we'd have looped forever above)
    return precode->compiled->code;
}

// ── RegisterJitEntryMethods ───────────────────────────────────────────────
// Called once at startup (from runtime-entry.cpp) to register all methods
// for JIT compilation via precode dispatch.  For each entry:
//   1. Deserialize AotCoreIr JSON → IRMethod
//   2. AllocateRegisters → RegisterMethod
//   3. Heap-allocate JitPrecode with the RegisterMethod + CompileConfig
//   4. Allocate a PrecodeArena trampoline for the JitPrecode
//   5. Set HotpatchEntryV0::direct_ptr to the trampoline
//
// After this call, first invocation of each method triggers JitStubDispatchImpl
// which calls Compile() and atomically replaces direct_ptr with compiled code.
extern "C" void RegisterJitEntryMethods(const JitEntry* entries, uint32_t count) noexcept {
    using namespace chaos::il2cpp::interpreter;
    using namespace chaos::il2cpp::runtime_core;

    // Function-local static arena — lives for program lifetime
    static PrecodeArena arena;

    for (uint32_t i = 0; i < count; ++i) {
        const auto& entry = entries[i];
        std::fprintf(stderr, "DIAG: JIT entry %u/%u token=0x%x module=%u json_len=%u\n",
                     i, count, entry.token, entry.module_id, entry.json_len);

        // Step 1: Deserialize AotCoreIr JSON → IRMethod
        auto ir = DeserializeAotCoreIrMethod(
            entry.json, entry.json_len, nullptr, nullptr, nullptr, nullptr);
        std::fprintf(stderr, "DIAG: entry %u deserialize OK\n", i);

        // Step 2: Allocate registers → RegisterMethod (independent copy)
        auto rm = AllocateRegisters(ir);
        std::fprintf(stderr, "DIAG: entry %u alloc_regs OK\n", i);
        // ir goes out of scope — vectors auto-clean

        // Step 3: Heap-allocate JitPrecode (lives for program lifetime)
        auto* precode = new JitPrecode();
        std::fprintf(stderr, "DIAG: entry %u new JitPrecode OK precode=%p\n", i, (void*)precode);
        precode->ir = std::move(rm);
        precode->config = CompileConfig{};

        // Step 4: Look up the HotpatchEntryV0 for this method
        precode->entry = GetHotpatchNameRegistry().GetDispatchEntry(
            entry.module_id, entry.token);
        std::fprintf(stderr, "DIAG: entry %u GetDispatchEntry OK entry=%p\n", i, (void*)precode->entry);

        // Step 5: Allocate trampoline from the RWX arena
        precode->trampoline = arena.AllocateJitTrampoline(precode);
        std::fprintf(stderr, "DIAG: entry %u AllocateJitTrampoline OK trampoline=%p\n", i, (void*)precode->trampoline);

        // Step 6: Point direct_ptr at the trampoline.
        // First call goes trampoline → JitStubEntry → JitStubDispatchImpl
        // → Compile() → direct_ptr atomically replaced with compiled code.
        if (precode->entry && precode->trampoline) {
            precode->entry->direct_ptr = precode->trampoline;
        } else {
            CHAOS_IL2CPP_LOG_ERROR_M("jit",
                "RegisterJitEntryMethods: failed for token 0x%x module %u",
                entry.token, entry.module_id);
        }
    }

    // Register the slot update callback (idempotent — only sets once).
    // When SetPatchedBySlot bumps version and fires the callback, this
    // updates all JIT slot tables to point at the new direct_ptr and
    // invalidates any callers that inlined the patched method.
    RegisterSlotUpdateCallback([](uint32_t callee_token, void* new_direct_ptr,
                                   HotpatchEntryV0* callee_entry) {
        // Update non-inlined call-site slots
        g_reverse_slot_map.UpdateAll(callee_token, new_direct_ptr);
        // Invalidate callers that inlined this method (version mismatch → stale)
        g_inline_reverse_map.InvalidateCallers(callee_token, callee_entry);
    });
}

// ── HybridStubDispatchImpl ────────────────────────────────────────────
//
// HybridPrecode dispatch: counter-based AOT→JIT upgrade.
//   1. Fast check: if already compiled, return compiled->code immediately
//   2. Decrement call_counter (relaxed atomic — precision not critical)
//   3. If old == 1 (this was the last AOT call), CAS to become compiler:
//      - Winner: calls Compile(), patches HotpatchEntryV0::direct_ptr
//      - If Compile fails: reset state + counter so retry is possible
//   4. Return compiled code if ready, otherwise aot_entry
//
// Unlike JitStubDispatchImpl, callers do NOT spin-wait — they execute AOT
// while JIT is being compiled in the background. Only the triggering thread
// may spin briefly after its own Compile() completes.
extern "C" void* HybridStubDispatchImpl(HybridPrecode* precode) noexcept {
    // Fast relaxed check: already compiled?
    uint32_t s = precode->state.load(std::memory_order_relaxed);
    if (s == kPrecodeCompiled) {
        // PGO: increment call counter when profiling is enabled
        if (precode->config.enable_pgo) {
            uint32_t count = precode->pgo_call_count.fetch_add(1, std::memory_order_relaxed);
            if (count > kPgoFullJitThreshold && !precode->tier1_enqueued) {
                precode->tier1_enqueued = true;
                runtime_core::TierManager::Get().EnqueueJitRecompilation(precode, true);
            }
        }
        return precode->compiled->code;
    }

    // Decrement call counter (relaxed — the counter is a heuristic, not a
    // correctness-critical value).  fetch_sub returns the value BEFORE decrement.
    uint32_t old = precode->call_counter.fetch_sub(1, std::memory_order_relaxed);

    if (old == 1) {
        // Counter just reached 0 — trigger JIT compilation.
        // CAS protects against multiple threads racing here.
        uint32_t expected = kPrecodeUncompiled;
        if (precode->state.compare_exchange_strong(expected, kPrecodeCompiling,
                                                    std::memory_order_acq_rel)) {
            // Winner — compile the method
            JitMethod* jit = CompileWithCatch(precode->ir, precode->config);
            if (jit) {
                // Store the AOT entry for deoptimization fallback.
                // When JIT-compiled code deopts (kDeoptMagic), the runtime
                // checks JitMethod::aot_entry and redirects to AOT instead
                // of falling through to the interpreter.
                jit->aot_entry = precode->aot_entry;
                precode->compiled = jit;
                // When enable_pgo is true, do NOT patch direct_ptr —
                // calls continue through dispatch for PGO counting.
                if (precode->entry && !precode->config.enable_pgo) {
                    precode->entry->direct_ptr = jit->code;
                }
                precode->state.store(kPrecodeCompiled, std::memory_order_release);
            } else {
                // Compilation failed — reset state and counter so we can retry
                precode->state.store(kPrecodeUncompiled, std::memory_order_release);
                precode->call_counter.store(1, std::memory_order_relaxed);
                CHAOS_IL2CPP_LOG_ERROR_M("jit",
                    "HybridStubDispatchImpl: Compile() failed for method");
            }
        }
        // CAS lost — another thread is compiling. Keep executing AOT.
    }

    // Check if compilation completed (either by us or another thread)
    if (precode->state.load(std::memory_order_acquire) == kPrecodeCompiled) {
        return precode->compiled->code;
    }

    // Not yet compiled — return AOT entry for this call
    return precode->aot_entry;
}

// ── RegisterHybridMethods ───────────────────────────────────────────────
// Called once at startup to register all methods for Hybrid mode.
// In Hybrid mode, the AOT C++ function body is already compiled into the
// binary.  This function:
//   1. Deserializes AotCoreIr JSON → IRMethod → AllocateRegisters → RegisterMethod
//   2. Heap-allocates a HybridPrecode with the RegisterMethod + CompileConfig
//   3. Saves the existing AOT entry (from HotpatchEntryV0::direct_ptr)
//   4. Sets call_counter = kJitUpgradeThreshold
//   5. Allocates a PrecodeArena trampoline for the HybridPrecode
//   6. Replaces HotpatchEntryV0::direct_ptr with the trampoline
//
// After this call, cold invocations execute AOT via the trampoline's
// return of aot_entry.  When counter reaches 0, JIT compilation triggers
// and subsequent calls go directly to the JIT-compiled code.
extern "C" void RegisterHybridMethods(const HybridEntry* entries, uint32_t count) noexcept {
    using namespace chaos::il2cpp::interpreter;
    using namespace chaos::il2cpp::runtime_core;

    // Function-local static arena — lives for program lifetime
    static PrecodeArena arena;

    for (uint32_t i = 0; i < count; ++i) {
        const auto& entry = entries[i];

        // Step 1: Deserialize AotCoreIr JSON → IRMethod
        auto ir = DeserializeAotCoreIrMethod(
            entry.json, entry.json_len, nullptr, nullptr, nullptr, nullptr);

        // Step 2: Allocate registers → RegisterMethod (independent copy)
        auto rm = AllocateRegisters(ir);

        // Step 3: Heap-allocate HybridPrecode (lives for program lifetime)
        auto* precode = new HybridPrecode();
        precode->ir = std::move(rm);
        precode->config = CompileConfig{};
        precode->call_counter.store(kJitUpgradeThreshold, std::memory_order_relaxed);

        // Step 4: Look up the HotpatchEntryV0 for this method
        precode->entry = GetHotpatchNameRegistry().GetDispatchEntry(
            entry.module_id, entry.token);

        // Step 5: Save the AOT entry (existing direct_ptr points to AOT function)
        if (precode->entry) {
            precode->aot_entry = precode->entry->direct_ptr;
        }

        // Step 6: Allocate trampoline from the RWX arena (Hybrid shared entry)
        precode->trampoline = arena.AllocateHybridTrampoline(precode);

        // Step 7: Point direct_ptr at the trampoline.
        // Cold calls go: trampoline → HybridStubEntry → HybridStubDispatchImpl
        // → counter decrement → aot_entry (until JIT compilation triggers).
        if (precode->entry && precode->trampoline) {
            precode->entry->direct_ptr = precode->trampoline;
        } else {
            CHAOS_IL2CPP_LOG_ERROR_M("jit",
                "RegisterHybridMethods: failed for token 0x%x module %u",
                entry.token, entry.module_id);
        }
    }

    // Register the slot update callback (idempotent — only sets once).
    RegisterSlotUpdateCallback([](uint32_t callee_token, void* new_direct_ptr,
                                   HotpatchEntryV0* callee_entry) {
        g_reverse_slot_map.UpdateAll(callee_token, new_direct_ptr);
        g_inline_reverse_map.InvalidateCallers(callee_token, callee_entry);
    });
}

// ── JitRecompileToTier1 ─────────────────────────────────────────────────
// Called by the TierManager background thread to recompile a method with the
// full Tier 1 pipeline.  Replaces precode->compiled with the new JitMethod
// and returns the new entry point.  The caller (TierManager) handles patching
// direct_ptr after this returns.
//
// Thread-safety: only called from the single TierManager background thread,
// so there is no contention on the precode fields.  The dispatch function
// (JitStubDispatchImpl/HybridStubDispatchImpl) may still be reading
// precode->compiled from other threads — those reads see the old pointer
// until the TierManager atomically stores direct_ptr.
extern "C" void* JitRecompileToTier1(void* precode, bool is_hybrid) noexcept {
    using namespace chaos::il2cpp::interpreter;

    if (is_hybrid) {
        auto* hp = static_cast<HybridPrecode*>(precode);
        CompileConfig tier1_cfg = hp->config;
        tier1_cfg.compile_tier = CompileTier::kFull;
        tier1_cfg.enable_pgo = false;  // Tier 1 is final — no further profiling
        auto* jit = Compile(hp->ir, tier1_cfg);
        if (!jit) {
            CHAOS_IL2CPP_LOG_ERROR_M("jit",
                "JitRecompileToTier1(hybrid): Compile(Tier1) failed");
            return nullptr;
        }
        // Copy the aot_entry from old JitMethod
        if (hp->compiled) {
            jit->aot_entry = hp->compiled->aot_entry;
        }
        // Free the Tier 0 JitMethod and install Tier 1
        delete hp->compiled;
        hp->compiled = jit;
        return jit->code;
    } else {
        auto* jp = static_cast<JitPrecode*>(precode);
        CompileConfig tier1_cfg = jp->config;
        tier1_cfg.compile_tier = CompileTier::kFull;
        tier1_cfg.enable_pgo = false;  // Tier 1 is final — no further profiling
        auto* jit = Compile(jp->ir, tier1_cfg);
        if (!jit) {
            CHAOS_IL2CPP_LOG_ERROR_M("jit",
                "JitRecompileToTier1(jit): Compile(Tier1) failed");
            return nullptr;
        }
        // Free the Tier 0 JitMethod and install Tier 1
        delete jp->compiled;
        jp->compiled = jit;
        return jit->code;
    }
}

}  // namespace chaos::il2cpp::jit
