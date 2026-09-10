#include "WinSehHandler.h"
#include "jit_seh.h" // for kMaxUnwindDepth, JitUnwindState, etc.
#include "jit_registry_lock_guard.h"

#include <chaos/log.h>
#include <chaos/native_types.h>
#include <chaos/pal/pal_mem.h>

#include <jit_demotion.h>
#include <gc_events.h>
#include <memory_domain.h>

// Static assertions for key structure layout consistency.
static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");
static_assert(sizeof(int32_t) == 4, "int32_t must be 4 bytes");

#if defined(_WIN32) || defined(_WIN64)
#define NOMINMAX
#include <windows.h>
#include <intrin.h> // _ReturnAddress(), _AddressOfReturnAddress()
#if defined(_MSC_VER)
#include <intrin.h> // _mm_pause()
#endif
#elif defined(__linux__)
#include <sched.h> // sched_yield
// GCC unwinder .eh_frame registration — no header provides this on all GCC versions.
extern "C" void __register_frame(const void*);
extern "C" void __deregister_frame(const void*);
#endif

// ── kSpinLimitHard — If the spinlock spins this many iterations without
//     acquiring the lock, emit a warning so we can diagnose contention.
static constexpr uint32_t kSpinLimitHard = 1024 * 1024; // ~1M spins ~ O(10ms on 2GHz)

namespace chaos::il2cpp::jit {

// ── TLS Lookup Cache ─────────────────────────────────────────────────────────
// Thread-local lookup cache: stores the JitMethod for the most recently
// accessed code page.  VEH handler / personality routine / EndFinallyHelper
// call FindCodeByAddress frequently, and consecutive calls often fall
// within the same method's code range.
static thread_local struct {
    uintptr_t page_base = 0; // address >> 12 (page-aligned)
    const JitMethod* nm = nullptr;
    uint32_t generation = 0; // lookup_generation_ at cache fill time
} g_jit_lookup_cache;

// ═══════════════════════════════════════════════════════════════════════════
// WinSehHandler — Spinlock implementation
// Uses std::atomic member lock_ for cross-platform safety.
// ═══════════════════════════════════════════════════════════════════════════

#if defined(_MSC_VER)
static thread_local uint32_t g_lock_owner_tid = 0;
static thread_local uint32_t g_lock_recursion = 0;
#elif defined(__linux__)
static thread_local pthread_t g_lock_owner {};
static thread_local uint32_t g_lock_recursion = 0;
#endif

void WinSehHandler::AcquireLock() noexcept {
#if defined(_MSC_VER)
    uint32_t tid = GetCurrentThreadId();
    if (tid == g_lock_owner_tid) {
        g_lock_recursion++;
        return;
    }
#elif defined(__linux__)
    pthread_t self = pthread_self();
    if (pthread_equal(self, g_lock_owner)) {
        g_lock_recursion++;
        return;
    }
#endif
    uint32_t spins = 0;
    while (lock_.exchange(1, std::memory_order_acquire) != 0) {
#if defined(_MSC_VER)
        _mm_pause();
#elif defined(__linux__)
        if (++spins % 64 == 0)
            sched_yield();
#endif
        if (++spins > kSpinLimitHard) {
            CHAOS_IL2CPP_LOG_WARN_M("codegen", "WinSehHandler: spinning for {} iterations -- possible deadlock?",
                                    spins);
            spins = 0; // reset so we get periodic warnings, not just one
        }
    }
#if defined(_MSC_VER)
    g_lock_owner_tid = tid;
    g_lock_recursion = 1;
#elif defined(__linux__)
    g_lock_owner = self;
    g_lock_recursion = 1;
#endif
}

void WinSehHandler::ReleaseLock() noexcept {
    if (--g_lock_recursion > 0)
        return;
#if defined(_MSC_VER)
    g_lock_owner_tid = 0;
#elif defined(__linux__)
    g_lock_owner = {};
#endif
    lock_.store(0, std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════════════════
// WinSehHandler — EnqueueDemotedCode / InvalidateLookupCache
// ═══════════════════════════════════════════════════════════════════════════

void WinSehHandler::EnqueueDemotedCode(void* code_start, uint32_t code_size) noexcept {
    if (code_start == nullptr || code_size == 0)
        return;

    // Deduplicate: if this exact address is already tracked, skip.
    for (auto& region : pending_free_) {
        if (region.active && region.code_start == code_start)
            return;
    }

    // Reuse an inactive slot, or add a new entry.
    for (auto& region : pending_free_) {
        if (!region.active) {
            region.code_start = code_start;
            region.code_size = code_size;
            region.active = true;
            return;
        }
    }

    pending_free_.push_back({code_start, code_size, true});
}

void WinSehHandler::InvalidateLookupCache() noexcept {
    lookup_generation_.fetch_add(1, std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════════════════
// WinSehHandler — ISehHandler interface implementation
//
// These class methods are compiled on ALL platforms.  They maintain the
// T4 code registry (entries_[], count_) and handle demotion/lookup logic.
// Platform-specific memory freeing (VirtualFree / munmap) is guarded
// inline.
// ═══════════════════════════════════════════════════════════════════════════

WinSehHandler::~WinSehHandler() noexcept {
    // The singleton lives for the process lifetime; cleanup is handled
    // by the OS on process exit.
}

void WinSehHandler::RegisterCode(void* code_start, uint32_t code_size, const JitMethod* nm,
                                 uint32_t patch_method_token) noexcept {
    if (code_start == nullptr || code_size == 0 || nm == nullptr)
        return;

    {
        JitRegistryLockGuard lock(this);
        JitCodeEntry entry;
        entry.code_start = code_start;
        entry.code_size = code_size;
        entry.nm = nm;
        entry.patch_method_token = patch_method_token;
        {
            auto* domain = chaos::il2cpp::memory_domain::CurrentDomain();
            entry.domain_id = domain ? domain->domain_id : 0;
        }
        entries_.push_back(entry);
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "RegisterCode: code={} size={} seh_offset={} token={}", code_start, code_size,
                             nm->seh_table_offset, patch_method_token);

#if defined(_WIN64)
    // Register .pdata/.xdata unwind info for OS stack unwinding.
    if (nm->runtime_function != nullptr) {
        if (!RtlAddFunctionTable(static_cast<PRUNTIME_FUNCTION>(nm->runtime_function), 1,
                                 reinterpret_cast<DWORD64>(code_start))) {
            CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "RtlAddFunctionTable failed for token={}", patch_method_token);
        }
    }
#elif defined(__linux__)
    // Register DWARF .eh_frame for libgcc stack unwinding.
    if (nm->eh_frame_offset > 0) {
        const void* eh_frame = static_cast<const uint8_t*>(code_start) + nm->eh_frame_offset;
        __register_frame(eh_frame);
        const_cast<JitMethod*>(nm)->eh_frame_registered = true;
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "RegisterCode: registered .eh_frame at offset {}", nm->eh_frame_offset);
    }
#endif
}

void WinSehHandler::UnregisterCode(void* code_start) noexcept {
    if (code_start == nullptr)
        return;
    {
        JitRegistryLockGuard lock(this);
        for (auto& entry : entries_) {
            if (entry.code_start == code_start) {
                // T2.3-C 方案3: keep the entry + code alive (see
                // ReclaimDemoted).  We only mark the JitMethod as externally
                // managed so teardown does not double-free it; the code itself
                // stays mapped and GC/SEH-resolvable for the process lifetime.
                if (entry.nm != nullptr) {
                    const_cast<JitMethod*>(entry.nm)->code_managed_externally = true;
                }
                break;
            }
        }
    }
    InvalidateLookupCache();
}

const JitMethod* WinSehHandler::FindCodeByAddress(const void* address) noexcept {
    uintptr_t addr_val = reinterpret_cast<uintptr_t>(address);
    uintptr_t page = addr_val >> 12;

    // Fast path: check the thread-local page-aligned cache.
    // Must verify the address is within the cached method's code range,
    // since multiple methods can share the same 4KB page and the cache
    // only stores the most recently looked-up JitMethod for that page.
    // Read cache fields first, then check generation — this eliminates
    // the TOCTOU window where UnregisterCode invalidates the cache
    // after we read nm but before we check the generation.
    auto* nm = g_jit_lookup_cache.nm;
    uintptr_t cached_page = g_jit_lookup_cache.page_base;
    uint32_t cached_gen = g_jit_lookup_cache.generation;
    uint32_t gen = lookup_generation_.load(std::memory_order_acquire);
    if (nm != nullptr && cached_page == page && cached_gen == gen) {
        const auto* cached_start = static_cast<const uint8_t*>(g_jit_lookup_cache.nm->code);
        const auto* cached_end = cached_start + g_jit_lookup_cache.nm->code_size;
        const auto* addr_bytes = static_cast<const uint8_t*>(address);
        if (addr_bytes >= cached_start && addr_bytes < cached_end) {
            return g_jit_lookup_cache.nm;
        }
        // Address is on the same page but outside the cached method's range.
        // Fall through to the linear scan — don't return a stale result.
    }

    // Slow path: linear scan the registry.
    for (const auto& entry : entries_) {
        const uint8_t* start = static_cast<const uint8_t*>(entry.code_start);
        if (start == nullptr)
            continue;
        const uint8_t* end = start + entry.code_size;
        const uint8_t* addr = static_cast<const uint8_t*>(address);
        if (addr >= start && addr < end) {
            // Populate cache for future lookups within the same page.
            g_jit_lookup_cache.page_base = page;
            g_jit_lookup_cache.nm = entry.nm;
            g_jit_lookup_cache.generation = gen;
            return entry.nm;
        }
    }
    return nullptr;
}

uint32_t WinSehHandler::DemoteByToken(uint32_t method_token) noexcept {
    if (method_token == 0)
        return 0;
    uint32_t count = 0;
    {
        JitRegistryLockGuard lock(this);
        for (auto& entry : entries_) {
            if (entry.patch_method_token == method_token && entry.nm != nullptr) {
                const_cast<JitMethod*>(entry.nm)->code_managed_externally = true;
                EnqueueDemotedCode(const_cast<void*>(entry.code_start), entry.code_size);
                entry.nm = nullptr;
                entry.code_start = nullptr;
                count++;
            }
        }
    }
    if (count > 0) {
        InvalidateLookupCache();
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "DemoteByToken: token={} demoted {} entries", method_token, count);
    }
    return count;
}

uint32_t WinSehHandler::DemoteByDomainId(uint32_t domain_id) noexcept {
    if (domain_id == 0)
        return 0; // core domain, never unloaded
    uint32_t count = 0;
    {
        JitRegistryLockGuard lock(this);
        for (auto& entry : entries_) {
            if (entry.domain_id == domain_id && entry.nm != nullptr) {
                const_cast<JitMethod*>(entry.nm)->code_managed_externally = true;
                EnqueueDemotedCode(const_cast<void*>(entry.code_start), entry.code_size);
                entry.nm = nullptr;
                entry.code_start = nullptr;
                count++;
            }
        }
    }
    if (count > 0) {
        InvalidateLookupCache();
        CHAOS_IL2CPP_LOG_INFO_M("codegen", "DemoteByDomainId: domain={} demoted {} entries", domain_id, count);
    }
    return count;
}

uint32_t WinSehHandler::DemoteByCallSiteToken(uint32_t method_token) noexcept {
    if (method_token == 0)
        return 0;
    uint32_t count = 0;
    {
        JitRegistryLockGuard lock(this);
        for (auto& entry : entries_) {
            const auto* nm = entry.nm;
            if (nm == nullptr)
                continue;
            for (uint32_t j = 0; j < nm->call_site_count; j++) {
                if (nm->call_sites[j].method_token == method_token) {
                    const_cast<JitMethod*>(nm)->code_managed_externally = true;
                    EnqueueDemotedCode(const_cast<void*>(entry.code_start), entry.code_size);
                    entry.nm = nullptr;
                    entry.code_start = nullptr;
                    count++;
                    break;
                }
            }
        }
    }
    if (count > 0) {
        InvalidateLookupCache();
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "DemoteByCallSiteToken: token={} demoted {} caller entries", method_token,
                                 count);
    }
    return count;
}

void WinSehHandler::ReclaimDemoted() noexcept {
    // T2.3-C 方案3: NEVER reclaim (free) demoted code.  Demoted/jit code is
    // moved to a process-lifetime allocation and its address is never reused,
    // so:
    //   (a) a thread still executing demoted code (its return address /
    //       saved registers point into the old region) can never run on freed
    //       memory (no use-after-free), and
    //   (b) an old stack frame's return address can never be re-matched to a
    //       recycled address that belongs to a different method's GC map.
    // This is the deliberate memory-for-safety tradeoff: old code + its root
    // maps stay live until process exit (bounded by the number of hot
    // updates, not by time).  The pending_free_ bookkeeping is retained so
    // the demotion accounting stays intact; nothing is VirtualFree'd here.
    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                             "ReclaimDemoted: {} demoted region(s) retained for process lifetime (T2.3-C 方案3)",
                             static_cast<unsigned>(pending_free_.size()));
}

// ═══════════════════════════════════════════════════════════════════════════
// GC event callback (static function, forwards to singleton)
// ═══════════════════════════════════════════════════════════════════════════

static void OnGcSafepoint(chaos::il2cpp::runtime_core::GcEvent /*event*/, void* /*user_data*/) noexcept {
    GetWinSehHandler().ReclaimDemoted();
}

// ═══════════════════════════════════════════════════════════════════════════
// WinSehHandler::Initialize — platform-independent registration
//
// On Windows, registers the VEH handler, GC event callback, and T4 demotion
// callbacks.  On non-Windows platforms, only registers the GC event callback
// and T4 demotion callbacks (VEH is Windows-specific).
// ═══════════════════════════════════════════════════════════════════════════

#if defined(_WIN32) || defined(_WIN64)
// Forward declaration for VEH callback (defined later in this TU).
static LONG WINAPI JitVectoredExceptionHandler(EXCEPTION_POINTERS* ep) noexcept;
#endif

void WinSehHandler::Initialize() noexcept {
    pending_free_.reserve(256);
    entries_.reserve(4096);

#if defined(_WIN32) || defined(_WIN64)
    // Register VEH handler (first in the handler chain = called first).
    PVOID handle = AddVectoredExceptionHandler(1, JitVectoredExceptionHandler);
    if (handle != nullptr) {
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "T4 VEH handler registered");
    } else {
        CHAOS_IL2CPP_LOG_ERROR_M("codegen", "T4 VEH handler registration FAILED");
    }
#endif

    // Register GC event callback for deferred T4 code memory reclamation.
    // On all platforms, this fires at GC safepoint (STW), guaranteeing no
    // thread is executing T4 code when we free demoted memory.
    chaos::il2cpp::runtime_core::GcRegisterEventCallback(OnGcSafepoint, nullptr);

    // Register T4 demotion callbacks so method_replacement can demote T4
    // entries through runtime_core without a circular build dependency.
    // The free functions (DemoteJittedMethod, DemoteJittedCallSite) in
    // jit_seh.cpp forward to this singleton, so we pass them directly.
    chaos::il2cpp::runtime_core::RegisterJitDemotionCallbacks(DemoteJittedMethod, DemoteJittedCallSite);
}

// ═══════════════════════════════════════════════════════════════════════════
// Global singleton accessor
// ═══════════════════════════════════════════════════════════════════════════

WinSehHandler& GetWinSehHandler() noexcept {
    static WinSehHandler instance;
    return instance;
}

#if defined(_WIN32) || defined(_WIN64)
ISehHandler& GetSehHandler() noexcept {
    return GetWinSehHandler();
}
#endif

// ═══════════════════════════════════════════════════════════════════════════
// Windows VEH-specific code
//
// The following functions are Windows-specific and guarded accordingly.
// On non-Windows platforms, ChaosJitRaiseException is provided as a stub
// that traps (since Windows VEH is not available on POSIX — use LinuxSehHandler instead).
// ═══════════════════════════════════════════════════════════════════════════

#if defined(_WIN32) || defined(_WIN64)

/// Managed exception code used by CodegenThrow/CodegenRethrow.
static constexpr uint32_t kManagedSehExceptionCode = 0xE0000001;

/// Exception object pointer for the current thread.
thread_local void* g_jit_exception_obj = nullptr;

/// Return address in T4 code where the exception was thrown.
thread_local void* g_jit_throw_ret_addr = nullptr;

/// T4 frame RSP at the throw point.
thread_local void* g_jit_frame_rsp = nullptr;

/// SEH V3: Thread-local finally/fault unwind state.
thread_local JitUnwindState g_jit_unwind = {};

// ── SEH Clause Table Constants ─────────────────────────────────────────────

static constexpr uint32_t kSehClauseEntrySize = 5 * sizeof(uint32_t);

// T4 frame layout constants (mirrored from code_generator.cpp).
static constexpr uint32_t kJitGprFileOffset = 32; // byte offset from T4 frame RSP to GPR register file
static constexpr uint32_t kJitGprCount = 64;      // number of GPR vreg slots

// ── SEH V3: Two-Phase Finally/Fault Unwind Functions ───────────────────────

/// Find the innermost catch/filter clause covering code_offset.
static bool FindSehCatchHandler(const JitMethod* nm, uint32_t code_offset, uint32_t* out_handler_offset,
                                uint32_t* out_clause_idx) noexcept {
    if (nm->seh_table_offset == 0)
        return false;

    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

    for (uint32_t i = count; i > 0; --i) {
        uint32_t idx = i - 1;
        const uint8_t* entry = clauses + idx * kSehClauseEntrySize;
        uint32_t cflags, ctry_start, ctry_end;
        std::memcpy(&cflags, entry + 0, sizeof(cflags));
        std::memcpy(&ctry_start, entry + 4, sizeof(ctry_start));
        std::memcpy(&ctry_end, entry + 8, sizeof(ctry_end));

        if ((cflags == 0 || cflags == 1) && code_offset >= ctry_start && code_offset < ctry_end) {
            uint32_t handler_st;
            std::memcpy(&handler_st, entry + 12, sizeof(handler_st));
            if (out_handler_offset)
                *out_handler_offset = handler_st;
            if (out_clause_idx)
                *out_clause_idx = idx;
            return true;
        }
    }
    return false;
}

/// Collect finally/fault clauses nested within the catch clause's try range.
static void BuildUnwindList(const JitMethod* nm, uint32_t catch_clause_idx, uint32_t throw_offset) noexcept {
    if (nm->seh_table_offset == 0)
        return;

    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

    const uint8_t* catch_entry = clauses + catch_clause_idx * kSehClauseEntrySize;
    uint32_t catch_try_start, catch_try_end;
    std::memcpy(&catch_try_start, catch_entry + 4, sizeof(catch_try_start));
    std::memcpy(&catch_try_end, catch_entry + 8, sizeof(catch_try_end));

    for (uint32_t i = 0; i < count; i++) {
        const uint8_t* entry = clauses + i * kSehClauseEntrySize;
        uint32_t cflags, ctry_start, ctry_end;
        std::memcpy(&cflags, entry + 0, sizeof(cflags));
        std::memcpy(&ctry_start, entry + 4, sizeof(ctry_start));
        std::memcpy(&ctry_end, entry + 8, sizeof(ctry_end));

        if ((cflags != 2 && cflags != 4))
            continue;
        if (ctry_start < catch_try_start)
            continue;
        if (ctry_start >= catch_try_end)
            continue;
        if (ctry_start > throw_offset)
            continue;

        if (g_jit_unwind.unwind_count < kMaxUnwindDepth) {
            for (uint32_t j = g_jit_unwind.unwind_count; j > 0; --j) {
                g_jit_unwind.unwind_list[j] = g_jit_unwind.unwind_list[j - 1];
            }
            g_jit_unwind.unwind_list[0] = i;
            g_jit_unwind.unwind_count++;
        }
    }
}

/// Reverse-search for the innermost finally covering code_offset.
static uint32_t FindInnermostFinally(const JitMethod* nm, uint32_t code_offset) noexcept {
    if (nm->seh_table_offset == 0)
        return UINT32_MAX;

    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

    for (uint32_t i = count; i > 0; --i) {
        uint32_t idx = i - 1;
        const uint8_t* entry = clauses + idx * kSehClauseEntrySize;
        uint32_t cflags, ctry_start, ctry_end;
        std::memcpy(&cflags, entry + 0, sizeof(cflags));
        std::memcpy(&ctry_start, entry + 4, sizeof(ctry_start));
        std::memcpy(&ctry_end, entry + 8, sizeof(ctry_end));

        if (cflags == 2 && code_offset >= ctry_start && code_offset < ctry_end) {
            return idx;
        }
    }
    return UINT32_MAX;
}

/// Read handler start byte offset for a clause index.
static uint32_t GetClauseHandlerOffset(const JitMethod* nm, uint32_t clause_idx) noexcept {
    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* entry = code + nm->seh_table_offset + sizeof(uint32_t) + clause_idx * kSehClauseEntrySize + 12;
    uint32_t val;
    std::memcpy(&val, entry, sizeof(val));
    return val;
}

/// Reset SEH V3 unwind state to defaults.
static void ResetUnwindState() noexcept {
    g_jit_unwind.unwind_count = 0;
    g_jit_unwind.unwind_index = 0;
    g_jit_unwind.exception_in_flight = false;
    g_jit_unwind.pending_leave = false;
    g_jit_unwind.leave_target_offset = 0;
    g_jit_unwind.has_catch = false;
    g_jit_unwind.catch_handler_offset = 0;
}

// ── ChaosJitRaiseException ───────────────────────────────────────────────────
// Called from T4-generated code for Throw/Rethrow instructions.

extern "C" void ChaosJitRaiseException(void* exception_obj) noexcept {
    g_jit_exception_obj = exception_obj;

#if defined(_MSC_VER)
    g_jit_throw_ret_addr = _ReturnAddress();
    g_jit_frame_rsp = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + 8);
#else
    g_jit_throw_ret_addr = __builtin_return_address(0);
    g_jit_frame_rsp = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(__builtin_frame_address(0)) + 16);
#endif

    RaiseException(kManagedSehExceptionCode, 0, 0, nullptr);

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "ChaosJitRaiseException: no handler found for exception, returning to INT3");
}

// ── JitVectoredExceptionHandler ──────────────────────────────────────────────

static LONG WINAPI JitVectoredExceptionHandler(EXCEPTION_POINTERS* ep) noexcept {
    auto& self = GetWinSehHandler();

    if (ep == nullptr || ep->ExceptionRecord == nullptr || ep->ContextRecord == nullptr) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    const uint32_t exc_code = ep->ExceptionRecord->ExceptionCode;

    if (exc_code == kManagedSehExceptionCode) {
        // Managed throw: use TLS-stored throw address for lookup.
        if (g_jit_throw_ret_addr == nullptr) {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        const JitMethod* nm = self.FindCodeByAddress(g_jit_throw_ret_addr);
        if (nm == nullptr || nm->seh_table_offset == 0) {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        const uint8_t* code_base = static_cast<const uint8_t*>(nm->code);
        uint32_t code_offset = static_cast<uint32_t>(static_cast<const uint8_t*>(g_jit_throw_ret_addr) - code_base);

        // Phase 1a: Find matching catch/filter handler.
        uint32_t catch_handler_offset_val = 0;
        uint32_t catch_clause_idx = 0;
        bool has_catch = FindSehCatchHandler(nm, code_offset, &catch_handler_offset_val, &catch_clause_idx);
        // Phase 1b: Build finally/fault unwind list.
        ResetUnwindState();
        if (has_catch) {
            g_jit_unwind.has_catch = true;
            g_jit_unwind.catch_handler_offset = catch_handler_offset_val;
            BuildUnwindList(nm, catch_clause_idx, code_offset);
        } else {
            const uint8_t* table_start = code_base + nm->seh_table_offset;
            uint32_t count;
            std::memcpy(&count, table_start, sizeof(count));
            const uint8_t* clauses = table_start + sizeof(uint32_t);
            for (uint32_t i = 0; i < count; i++) {
                const uint8_t* entry = clauses + i * kSehClauseEntrySize;
                uint32_t cflags, ctry_start, ctry_end;
                std::memcpy(&cflags, entry + 0, sizeof(cflags));
                std::memcpy(&ctry_start, entry + 4, sizeof(ctry_start));
                std::memcpy(&ctry_end, entry + 8, sizeof(ctry_end));
                if ((cflags == 2 || cflags == 4) && code_offset >= ctry_start && code_offset < ctry_end &&
                    g_jit_unwind.unwind_count < kMaxUnwindDepth) {
                    g_jit_unwind.unwind_list[g_jit_unwind.unwind_count++] = i;
                }
            }
        }
        g_jit_unwind.exception_in_flight = has_catch || g_jit_unwind.unwind_count > 0;

        if (!g_jit_unwind.has_catch && g_jit_unwind.unwind_count == 0) {
            g_jit_throw_ret_addr = nullptr;
            g_jit_frame_rsp = nullptr;
            ResetUnwindState();
            return EXCEPTION_CONTINUE_SEARCH;
        }

        // Write the exception object into ALL GPR register file slots.
        if (g_jit_frame_rsp != nullptr) {
            uint64_t* regfile =
                reinterpret_cast<uint64_t*>(reinterpret_cast<uint8_t*>(g_jit_frame_rsp) + kJitGprFileOffset);
            uint64_t ex_val = reinterpret_cast<uint64_t>(g_jit_exception_obj);
            for (uint32_t i = 0; i < kJitGprCount; i++) {
                regfile[i] = ex_val;
            }
        }

        ep->ContextRecord->Rcx = reinterpret_cast<ULONG_PTR>(g_jit_exception_obj);
        ep->ContextRecord->Rsp = reinterpret_cast<ULONG_PTR>(g_jit_frame_rsp);

        uint32_t target_handler_offset;
        if (g_jit_unwind.unwind_count > 0) {
            g_jit_unwind.unwind_index = 0;
            target_handler_offset = GetClauseHandlerOffset(nm, g_jit_unwind.unwind_list[0]);
        } else {
            target_handler_offset = catch_handler_offset_val;
        }
        void* handler_addr = static_cast<uint8_t*>(nm->code) + target_handler_offset;
        ep->ContextRecord->Rip = reinterpret_cast<ULONG_PTR>(handler_addr);

        g_jit_throw_ret_addr = nullptr;
        g_jit_frame_rsp = nullptr;

        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // ── Hardware exception (AV, div-by-zero, etc.) ──────────────────────
    void* exception_addr = ep->ExceptionRecord->ExceptionAddress;
    if (exception_addr == nullptr) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    const JitMethod* nm = self.FindCodeByAddress(exception_addr);
    if (nm == nullptr || nm->seh_table_offset == 0) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    const uint8_t* code_base = static_cast<const uint8_t*>(nm->code);
    uint32_t code_offset = static_cast<uint32_t>(static_cast<const uint8_t*>(exception_addr) - code_base);

    ResetUnwindState();
    {
        uint32_t catch_handler_offset_val = 0;
        uint32_t catch_clause_idx = 0;
        bool has_catch = FindSehCatchHandler(nm, code_offset, &catch_handler_offset_val, &catch_clause_idx);
        if (has_catch) {
            g_jit_unwind.has_catch = true;
            g_jit_unwind.catch_handler_offset = catch_handler_offset_val;
            BuildUnwindList(nm, catch_clause_idx, code_offset);
        } else {
            const uint8_t* tbl = code_base + nm->seh_table_offset;
            uint32_t clause_count;
            std::memcpy(&clause_count, tbl, sizeof(clause_count));
            const uint8_t* cls = tbl + sizeof(uint32_t);
            for (uint32_t i = 0; i < clause_count; i++) {
                const uint8_t* ent = cls + i * kSehClauseEntrySize;
                uint32_t cf, ts, te;
                std::memcpy(&cf, ent + 0, sizeof(cf));
                std::memcpy(&ts, ent + 4, sizeof(ts));
                std::memcpy(&te, ent + 8, sizeof(te));
                if ((cf == 2 || cf == 4) && code_offset >= ts && code_offset < te &&
                    g_jit_unwind.unwind_count < kMaxUnwindDepth) {
                    g_jit_unwind.unwind_list[g_jit_unwind.unwind_count++] = i;
                }
            }
        }
        g_jit_unwind.exception_in_flight = g_jit_unwind.has_catch || g_jit_unwind.unwind_count > 0;
    }
    if (!g_jit_unwind.has_catch && g_jit_unwind.unwind_count == 0) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    uint32_t target_handler_offset;
    if (g_jit_unwind.unwind_count > 0) {
        g_jit_unwind.unwind_index = 0;
        target_handler_offset = GetClauseHandlerOffset(nm, g_jit_unwind.unwind_list[0]);
    } else {
        target_handler_offset = g_jit_unwind.catch_handler_offset;
    }
    void* handler_addr = static_cast<uint8_t*>(nm->code) + target_handler_offset;
    ep->ContextRecord->Rcx = reinterpret_cast<ULONG_PTR>(g_jit_exception_obj);
    ep->ContextRecord->Rip = reinterpret_cast<ULONG_PTR>(handler_addr);

    return EXCEPTION_CONTINUE_EXECUTION;
}

// ── JitPersonalityRoutine ────────────────────────────────────────────────────

extern "C" EXCEPTION_DISPOSITION JitPersonalityRoutine(PEXCEPTION_RECORD ExceptionRecord, ULONG64 /*EstablisherFrame*/,
                                                       PCONTEXT ContextRecord,
                                                       PDISPATCHER_CONTEXT DispatcherContext) noexcept {
    auto& self = GetWinSehHandler();

    if (ExceptionRecord == nullptr || ContextRecord == nullptr || DispatcherContext == nullptr) {
        return ExceptionContinueSearch;
    }

    if (ExceptionRecord->ExceptionCode != kManagedSehExceptionCode) {
        return ExceptionContinueSearch;
    }

    const JitMethod* nm = self.FindCodeByAddress(reinterpret_cast<const void*>(DispatcherContext->ControlPc));
    if (nm == nullptr || nm->seh_table_offset == 0) {
        return ExceptionContinueSearch;
    }

    const uint8_t* code_base = static_cast<const uint8_t*>(nm->code);
    uint32_t code_offset =
        static_cast<uint32_t>(reinterpret_cast<const uint8_t*>(DispatcherContext->ControlPc) - code_base);

    ResetUnwindState();
    {
        uint32_t catch_handler_offset_val = 0;
        uint32_t catch_clause_idx = 0;
        bool has_catch = FindSehCatchHandler(nm, code_offset, &catch_handler_offset_val, &catch_clause_idx);
        if (has_catch) {
            g_jit_unwind.has_catch = true;
            g_jit_unwind.catch_handler_offset = catch_handler_offset_val;
            BuildUnwindList(nm, catch_clause_idx, code_offset);
        } else {
            const uint8_t* tbl = code_base + nm->seh_table_offset;
            uint32_t clause_count;
            std::memcpy(&clause_count, tbl, sizeof(clause_count));
            const uint8_t* cls = tbl + sizeof(uint32_t);
            for (uint32_t i = 0; i < clause_count; i++) {
                const uint8_t* ent = cls + i * kSehClauseEntrySize;
                uint32_t cf, ts, te;
                std::memcpy(&cf, ent + 0, sizeof(cf));
                std::memcpy(&ts, ent + 4, sizeof(ts));
                std::memcpy(&te, ent + 8, sizeof(te));
                if ((cf == 2 || cf == 4) && code_offset >= ts && code_offset < te &&
                    g_jit_unwind.unwind_count < kMaxUnwindDepth) {
                    g_jit_unwind.unwind_list[g_jit_unwind.unwind_count++] = i;
                }
            }
        }
        g_jit_unwind.exception_in_flight = g_jit_unwind.has_catch || g_jit_unwind.unwind_count > 0;
    }
    if (!g_jit_unwind.has_catch && g_jit_unwind.unwind_count == 0) {
        return ExceptionContinueSearch;
    }

    uint32_t target_handler_offset;
    if (g_jit_unwind.unwind_count > 0) {
        g_jit_unwind.unwind_index = 0;
        target_handler_offset = GetClauseHandlerOffset(nm, g_jit_unwind.unwind_list[0]);
    } else {
        target_handler_offset = g_jit_unwind.catch_handler_offset;
    }
    void* handler_addr = static_cast<uint8_t*>(nm->code) + target_handler_offset;
    ContextRecord->Rip = reinterpret_cast<ULONG_PTR>(handler_addr);
    ContextRecord->Rcx = reinterpret_cast<ULONG_PTR>(g_jit_exception_obj);

    return ExceptionCollidedUnwind;
}

// ── JitEndFinallyHelper ─────────────────────────────────────────────────────

extern "C" void* JitEndFinallyHelper() noexcept {
    auto& self = GetWinSehHandler();

    if (!g_jit_unwind.exception_in_flight && !g_jit_unwind.pending_leave) {
        return nullptr;
    }

    if (g_jit_unwind.exception_in_flight) {
        g_jit_unwind.unwind_index++;

        if (g_jit_unwind.unwind_index < g_jit_unwind.unwind_count) {
            uint32_t fi = g_jit_unwind.unwind_list[g_jit_unwind.unwind_index];
            void* ret_addr = _ReturnAddress();
            const JitMethod* nm = self.FindCodeByAddress(ret_addr);
            if (nm != nullptr) {
                return static_cast<uint8_t*>(nm->code) + GetClauseHandlerOffset(nm, fi);
            }
            return nullptr;
        }

        if (g_jit_unwind.has_catch) {
            uint32_t catch_off = g_jit_unwind.catch_handler_offset;
            void* ret_addr = _ReturnAddress();
            const JitMethod* nm = self.FindCodeByAddress(ret_addr);
            ResetUnwindState();
            if (nm != nullptr) {
                return static_cast<uint8_t*>(nm->code) + catch_off;
            }
            return nullptr;
        }

        ResetUnwindState();
        void* ex_obj = g_jit_exception_obj;
        ChaosJitRaiseException(ex_obj);
        return nullptr;
    }

    if (g_jit_unwind.pending_leave) {
        uint32_t leave_target = g_jit_unwind.leave_target_offset;
        ResetUnwindState();
        void* ret_addr = _ReturnAddress();
        const JitMethod* nm = self.FindCodeByAddress(ret_addr);
        if (nm != nullptr) {
            return static_cast<uint8_t*>(nm->code) + leave_target;
        }
        return nullptr;
    }

    return nullptr;
}

// ── JitLeaveHelper ─────────────────────────────────────────────────────────

extern "C" void* JitLeaveHelper(uint32_t target_instr_idx, uint32_t current_instr_idx) noexcept {
    auto& self = GetWinSehHandler();

    void* ret_addr = _ReturnAddress();
    const JitMethod* nm = self.FindCodeByAddress(ret_addr);
    if (nm == nullptr || nm->instr_offsets == nullptr) {
        return nullptr;
    }

    uint32_t current_offset = (current_instr_idx < nm->instr_offset_count) ? nm->instr_offsets[current_instr_idx] : 0;

    uint32_t finally_idx = FindInnermostFinally(nm, current_offset);
    if (finally_idx == UINT32_MAX) {
        return nullptr;
    }

    uint32_t target_offset = (target_instr_idx < nm->instr_offset_count) ? nm->instr_offsets[target_instr_idx] : 0;

    ResetUnwindState();
    g_jit_unwind.pending_leave = true;
    g_jit_unwind.leave_target_offset = target_offset;

    return static_cast<uint8_t*>(nm->code) + GetClauseHandlerOffset(nm, finally_idx);
}

#else // not Windows

// Non-Windows stub for ChaosJitRaiseException.
// VEH is Windows-only; on POSIX platforms this should never be called
// because JIT codegen with SEH is not enabled.  If called, crash.
extern "C" void ChaosJitRaiseException(void* /*exception_obj*/) noexcept {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_trap();
#else
    volatile int* p = nullptr;
    *p = 0;
#endif
}

#endif // _WIN32 || _WIN64

} // namespace chaos::il2cpp::jit
