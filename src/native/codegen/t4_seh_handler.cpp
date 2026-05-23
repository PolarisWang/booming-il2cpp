#include "t4_seh_handler.h"
#include "native_method.h"

#include <chaos/log.h>

// Register demotion callbacks with runtime_core so method_replacement
// can demote T4 entries without creating a circular dependency.
#include <t4_demotion.h>

// GC event callbacks for deferred T4 code memory reclamation.
#include <gc_events.h>

#if defined(_WIN32) || defined(_WIN64)
  #define NOMINMAX
  #include <windows.h>
  #include <intrin.h>   // _ReturnAddress(), _AddressOfReturnAddress()
  #if defined(_MSC_VER)
    #include <intrin.h> // _mm_pause()
  #endif
#elif defined(__linux__)
  #include <sys/mman.h> // munmap
  #include <sched.h>    // sched_yield
#endif

// ── kSpinLimitHard — If the spinlock spins this many iterations without
//     acquiring the lock, emit a warning so we can diagnose contention.
static constexpr uint32_t kSpinLimitHard = 1024 * 1024;  // ~1M spins ~ O(10ms on 2GHz)

namespace chaos::il2cpp::codegen {

// ── T4 Code Registry ─────────────────────────────────────────────────────
// Maps code address ranges back to NativeMethod for VEH lookup.
// Fixed-size array (no heap allocation in exception context).
// Thread-safe: entries are append-only, never removed.
// A per-thread lookup cache exploits temporal locality (repeated VEH
// lookups within the same code page are O(1)).

static constexpr uint32_t kMaxT4CodeEntries = 2048;

struct T4CodeEntry {
    const void*       code_start = nullptr;   // RX code entry point
    uint32_t          code_size  = 0;          // bytes
    const NativeMethod* nm       = nullptr;
    uint32_t          patch_method_token = 0;  // PatchMethod token for hotpatch demotion
};

static T4CodeEntry    g_t4_code_entries[kMaxT4CodeEntries];
static uint32_t       g_t4_code_count = 0;
static long           g_t4_code_lock  = 0;    // 0=free, 1=locked (spinlock)
#if defined(_MSC_VER)
static thread_local uint32_t g_t4_lock_owner_tid = 0;
#endif

/// RAII guard for g_t4_code_lock.
/// Spins with pause/yield, emits a warning if kSpinLimitHard is exceeded,
/// and always releases on scope exit.
class T4RegistryLockGuard {
public:
    T4RegistryLockGuard() noexcept {
        Acquire();
    }
    ~T4RegistryLockGuard() noexcept {
        Release();
    }
    // Non-copyable, non-movable.
    T4RegistryLockGuard(const T4RegistryLockGuard&) = delete;
    T4RegistryLockGuard& operator=(const T4RegistryLockGuard&) = delete;

private:
    void Acquire() noexcept {
#if defined(_MSC_VER)
        uint32_t tid = GetCurrentThreadId();
#endif
        uint32_t spins = 0;
        while (InterlockedExchange(&g_t4_code_lock, 1) != 0) {
#if defined(_MSC_VER)
            _mm_pause();
#elif defined(__linux__)
            if (++spins % 64 == 0) sched_yield();
#endif
            if (++spins > kSpinLimitHard) {
                CHAOS_IL2CPP_LOG_WARN_M("codegen",
                    "T4RegistryLockGuard: spinning for {} iterations -- possible deadlock?",
                    spins);
                spins = 0;  // reset so we get periodic warnings, not just one
            }
        }
#if defined(_MSC_VER)
        g_t4_lock_owner_tid = tid;
#endif
    }

    void Release() noexcept {
#if defined(_MSC_VER)
        g_t4_lock_owner_tid = 0;
#endif
        InterlockedExchange(&g_t4_code_lock, 0);
    }
};

// Global generation counter for TLS lookup cache invalidation.
// Each time T4 code is demoted/unregistered, this counter is incremented.
// Thread-local caches that hold a stale generation will automatically
// fall back to linear scan on their next lookup.  This avoids the
// cross-thread stale-NativeMethod* problem that existed with the old
// brute-force approach (which only cleared the current thread's cache).
static uint32_t g_t4_lookup_generation = 1;  // 0 is reserved for "not initialized"

// Thread-local lookup cache: stores the NativeMethod for the most recently
// accessed code page.  VEH handler / personality routine / EndFinallyHelper
// call FindT4CodeByAddress frequently, and consecutive calls often fall
// within the same method's code range, making this cache highly effective.
static thread_local struct {
    uintptr_t         page_base   = 0;  // address >> 12 (page-aligned)
    const NativeMethod* nm        = nullptr;
    uint32_t          generation = 0;   // g_t4_lookup_generation at cache fill time
} g_t4_lookup_cache;

// Invalidate the TLS lookup cache for all threads by bumping the global
// generation counter.  Threads that hold stale caches will see the mismatch
// in FindT4CodeByAddress and automatically re-scan.  This avoids the old
// approach of clearing only the current thread's TLS cache, which left
// other threads holding stale NativeMethod pointers.
static void InvalidateLookupCache() noexcept {
    InterlockedIncrement(reinterpret_cast<long*>(&g_t4_lookup_generation));
}

// ── Demoted Code Reclamation (C1) ──────────────────────────────────────────
// When T4 code is demoted, we can't immediately free the RX memory because
// other threads may still be executing it.  Instead we record the code range
// and defer VirtualFree to the next GC safepoint, which guarantees no thread
// is executing managed code (including T4 generated code).

static constexpr uint32_t kMaxPendingFreeRegions = 64;

struct PendingFreeRegion {
    void*    code_start = nullptr;
    uint32_t code_size  = 0;
    bool     active     = false;
};

static PendingFreeRegion g_pending_free[kMaxPendingFreeRegions];
static uint32_t          g_pending_free_count = 0;

static void EnqueueDemotedCode(void* code_start, uint32_t code_size) noexcept {
    if (code_start == nullptr || code_size == 0) return;

    // Deduplicate: if this exact address is already tracked, skip.
    for (uint32_t i = 0; i < kMaxPendingFreeRegions; i++) {
        if (g_pending_free[i].active && g_pending_free[i].code_start == code_start) {
            return;
        }
    }

    for (uint32_t i = 0; i < kMaxPendingFreeRegions; i++) {
        if (!g_pending_free[i].active) {
            g_pending_free[i].code_start = code_start;
            g_pending_free[i].code_size  = code_size;
            g_pending_free[i].active     = true;
            g_pending_free_count++;
            return;
        }
    }

    CHAOS_IL2CPP_LOG_WARN_M("codegen",
        "pending-free table full ({} entries)", kMaxPendingFreeRegions);
}

/// Free all demoted T4 code regions.  Called from the GC safepoint via
/// the GC event callback system (registered in RegisterT4SehHandler).
void ReclaimDemotedCode() noexcept {
    for (uint32_t i = 0; i < kMaxPendingFreeRegions; i++) {
        if (!g_pending_free[i].active) continue;

#if defined(_WIN64)
        BOOL ok = VirtualFree(g_pending_free[i].code_start, 0, MEM_RELEASE);
        if (!ok) {
            CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                "ReclaimDemotedCode: VirtualFree({}, {}) failed (already freed?)",
                g_pending_free[i].code_start, g_pending_free[i].code_size);
        }
#elif defined(__linux__)
        int ret = munmap(g_pending_free[i].code_start, g_pending_free[i].code_size);
        if (ret != 0) {
            CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                "ReclaimDemotedCode: munmap({}, {}) failed",
                g_pending_free[i].code_start, g_pending_free[i].code_size);
        }
#endif

        g_pending_free[i].active = false;
        g_pending_free[i].code_start = nullptr;
        g_pending_free[i].code_size = 0;
    }
    g_pending_free_count = 0;
}

/// GC event callback: fires at safepoint after GC completes.  Safe to
/// call VirtualFree/munmap because no thread is executing managed code.
static void OnGcSafepoint(chaos::il2cpp::runtime_core::GcEvent /*event*/,
                           void* /*user_data*/) noexcept {
    ReclaimDemotedCode();
}

void RegisterT4Code(void* code_start, uint32_t code_size,
                    const NativeMethod* nm,
                    uint32_t patch_method_token) noexcept {
    if (code_start == nullptr || code_size == 0 || nm == nullptr) return;

    {
        T4RegistryLockGuard lock;
        if (g_t4_code_count >= kMaxT4CodeEntries) {
            CHAOS_IL2CPP_LOG_WARN_M("codegen", "RegisterT4Code: registry full ({} entries)", kMaxT4CodeEntries);
            return;
        }
        g_t4_code_entries[g_t4_code_count].code_start = code_start;
        g_t4_code_entries[g_t4_code_count].code_size  = code_size;
        g_t4_code_entries[g_t4_code_count].nm         = nm;
        g_t4_code_entries[g_t4_code_count].patch_method_token = patch_method_token;
        g_t4_code_count++;
    }  // lock released here

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "RegisterT4Code: code={} size={} seh_offset={} token={}",
        code_start, code_size, nm->seh_table_offset, patch_method_token);

#if defined(_WIN64)
    // Register .pdata/.xdata unwind info for OS stack unwinding.
    if (nm->runtime_function != nullptr) {
        if (!RtlAddFunctionTable(
                static_cast<PRUNTIME_FUNCTION>(nm->runtime_function),
                1,
                reinterpret_cast<DWORD64>(code_start))) {
            CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                "RtlAddFunctionTable failed for token={}", patch_method_token);
        }
    }
#elif defined(__linux__)
    // Register DWARF .eh_frame for libgcc stack unwinding.
    if (nm->eh_frame_offset > 0) {
        const void* eh_frame = static_cast<const uint8_t*>(code_start) + nm->eh_frame_offset;
        __register_frame(eh_frame);
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "RegisterT4Code: registered .eh_frame at offset {}", nm->eh_frame_offset);
    }
#endif
}

void UnregisterT4Code(void* code_start) noexcept {
    if (code_start == nullptr) return;
    {
        T4RegistryLockGuard lock;
        for (uint32_t i = 0; i < g_t4_code_count; i++) {
            if (g_t4_code_entries[i].code_start == code_start) {
                EnqueueDemotedCode(
                    const_cast<void*>(g_t4_code_entries[i].code_start),
                    g_t4_code_entries[i].code_size);
                g_t4_code_entries[i].nm = nullptr;
                break;
            }
        }
    }  // lock released here
    InvalidateLookupCache();
}

uint32_t DemoteT4ByToken(uint32_t method_token) noexcept {
    if (method_token == 0) return 0;
    uint32_t count = 0;
    {
        T4RegistryLockGuard lock;
        for (uint32_t i = 0; i < g_t4_code_count; i++) {
            if (g_t4_code_entries[i].patch_method_token == method_token &&
                g_t4_code_entries[i].nm != nullptr) {
                EnqueueDemotedCode(
                    const_cast<void*>(g_t4_code_entries[i].code_start),
                    g_t4_code_entries[i].code_size);
                g_t4_code_entries[i].nm = nullptr;
                count++;
            }
        }
    }  // lock released here
    if (count > 0) {
        InvalidateLookupCache();
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "DemoteT4ByToken: token={} demoted {} entries", method_token, count);
    }
    return count;
}

uint32_t DemoteT4ByCallSiteToken(uint32_t method_token) noexcept {
    if (method_token == 0) return 0;
    uint32_t count = 0;
    {
        T4RegistryLockGuard lock;
        for (uint32_t i = 0; i < g_t4_code_count; i++) {
            const auto* nm = g_t4_code_entries[i].nm;
            if (nm == nullptr) continue;
            for (uint32_t j = 0; j < nm->call_site_count; j++) {
                if (nm->call_sites[j].method_token == method_token) {
                    EnqueueDemotedCode(
                        const_cast<void*>(g_t4_code_entries[i].code_start),
                        g_t4_code_entries[i].code_size);
                    g_t4_code_entries[i].nm = nullptr;
                    count++;
                    break;
                }
            }
        }
    }  // lock released here
    if (count > 0) {
        InvalidateLookupCache();
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "DemoteT4ByCallSiteToken: token={} demoted {} caller entries", method_token, count);
    }
    return count;
}

/// Find the NativeMethod covering a given code address.
/// Returns nullptr if not found.
/// Uses a thread-local page-aligned cache (O(1) on repeated lookups within
/// the same method's code range), falling back to linear scan on cold miss.
const NativeMethod* FindT4CodeByAddress(const void* address) noexcept {
    uintptr_t addr_val = reinterpret_cast<uintptr_t>(address);
    uintptr_t page = addr_val >> 12;

    // Fast path: check the thread-local page-aligned cache.
    // Generation check ensures we don't return a stale NativeMethod*
    // after a concurrent demotion on another thread invalidated the cache.
    if (g_t4_lookup_cache.nm != nullptr &&
        g_t4_lookup_cache.page_base == page &&
        g_t4_lookup_cache.generation == g_t4_lookup_generation) {
        return g_t4_lookup_cache.nm;
    }

    // Slow path: linear scan the registry.
    for (uint32_t i = 0; i < g_t4_code_count; i++) {
        const auto& entry = g_t4_code_entries[i];
        const uint8_t* start = static_cast<const uint8_t*>(entry.code_start);
        if (start == nullptr) continue;
        const uint8_t* end = start + entry.code_size;
        const uint8_t* addr = static_cast<const uint8_t*>(address);
        if (addr >= start && addr < end) {
            // Populate cache for future lookups within the same page.
            g_t4_lookup_cache.page_base = page;
            g_t4_lookup_cache.nm = entry.nm;
            return entry.nm;
        }
    }
    return nullptr;
}

// ── SEH Clause Table Layout ─────────────────────────────────────────────
// Appended after code in the generated native method buffer:
//   uint32_t  count
//   for each clause:
//     uint32_t  flags           (SEHFlags enum)
//     uint32_t  try_start_offset
//     uint32_t  try_end_offset
//     uint32_t  handler_start_offset
//     uint32_t  class_token

static constexpr uint32_t kSehClauseEntrySize = 5 * sizeof(uint32_t);

// T4 frame layout constants (mirrored from code_generator.cpp).
// The T4 frame register file starts at RSP + 32 (after 32-byte shadow space).
static constexpr uint32_t kT4GprFileOff = 32;  // byte offset from T4 frame RSP to GPR register file
static constexpr uint32_t kT4GprCount  = 64;   // number of GPR vreg slots

/// Walk SEH clause table and find the first matching handler for a given
/// code offset.  Returns the handler_start_offset (byte offset from code
/// entry), or 0xFFFFFFFF if no match.
static uint32_t FindSehHandlerForOffset(const NativeMethod* nm,
                                         uint32_t code_offset) noexcept {
    if (nm->seh_table_offset == 0) return 0xFFFFFFFFu;

    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;

    // Read clause count
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

    for (uint32_t i = 0; i < count; i++) {
        const uint8_t* entry = clauses + i * kSehClauseEntrySize;
        uint32_t clause_flags, try_start, try_end;
        std::memcpy(&clause_flags, entry + 0, sizeof(clause_flags));
        std::memcpy(&try_start,    entry + 4, sizeof(try_start));
        std::memcpy(&try_end,      entry + 8, sizeof(try_end));

        // Check if code_offset falls within this try block
        if (code_offset >= try_start && code_offset < try_end) {
            uint32_t handler_st;
            std::memcpy(&handler_st, entry + 12, sizeof(handler_st));

            // All handler types (catch, finally, fault, filter) are valid
            // targets for exception dispatch.  The interpreter-equivalent
            // behavior for finally/fault is: execute the handler, then
            // continue unwinding or transfer to catch.  For V1, we redirect
            // directly to the handler and let it run; the next exception
            // (from a rethrow inside finally) will re-enter the VEH.

            CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                "T4 VEH: clause {} matches offset={} flags=0x{:x} try=[{},{}) handler={}",
                i, code_offset, clause_flags, try_start, try_end, handler_st);
            return handler_st;
        }
    }

    return 0xFFFFFFFFu;
}

// ── SEH V3: Two-Phase Finally/Fault Unwind Functions ─────────────────────
//
// Phase 1: FindSehCatchHandler — reverse-iterates SEH clause table to find
//   the innermost catch/filter clause covering code_offset.
// Phase 1b: BuildUnwindList — forward-iterates, collecting finally/fault
//   (flags 0x2/0x4) nested within the catch's try range.
// Phase 2: Executed by T4EndFinallyHelper — advances unwind_index, returns
//   next handler offset or 0 when done.

/// Find the innermost catch/filter clause covering code_offset.
/// Returns true if found, with handler_offset and clause_idx set.
static bool FindSehCatchHandler(const NativeMethod* nm,
                                 uint32_t code_offset,
                                 uint32_t* out_handler_offset,
                                 uint32_t* out_clause_idx) noexcept {
    if (nm->seh_table_offset == 0) return false;

    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

    // Reverse search: find innermost matching catch/filter (flag 0x0 or 0x1).
    for (uint32_t i = count; i > 0; --i) {
        uint32_t idx = i - 1;
        const uint8_t* entry = clauses + idx * kSehClauseEntrySize;
        uint32_t cflags, ctry_start, ctry_end;
        std::memcpy(&cflags,    entry + 0, sizeof(cflags));
        std::memcpy(&ctry_start, entry + 4, sizeof(ctry_start));
        std::memcpy(&ctry_end,  entry + 8, sizeof(ctry_end));

        // Catch (0x0) or Filter (0x1)
        if ((cflags == 0 || cflags == 1) &&
            code_offset >= ctry_start && code_offset < ctry_end) {
            uint32_t handler_st;
            std::memcpy(&handler_st, entry + 12, sizeof(handler_st));
            if (out_handler_offset) *out_handler_offset = handler_st;
            if (out_clause_idx) *out_clause_idx = idx;
            return true;
        }
    }
    return false;
}

/// Collect finally/fault clauses (flags 0x2/0x4) nested within the catch
/// clause's try range, between try_start and throw_offset.
/// Entries are stored innermost-first (LIFO unwind order).
static void BuildUnwindList(const NativeMethod* nm,
                             uint32_t catch_clause_idx,
                             uint32_t throw_offset) noexcept {
    if (nm->seh_table_offset == 0) return;

    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

    // Get catch clause try range
    const uint8_t* catch_entry = clauses + catch_clause_idx * kSehClauseEntrySize;
    uint32_t catch_try_start, catch_try_end;
    std::memcpy(&catch_try_start, catch_entry + 4, sizeof(catch_try_start));
    std::memcpy(&catch_try_end,   catch_entry + 8, sizeof(catch_try_end));

    // Forward scan for finally/fault nested within catch's try range,
    // whose try_start is between catch_try_start and throw_offset.
    for (uint32_t i = 0; i < count; i++) {
        const uint8_t* entry = clauses + i * kSehClauseEntrySize;
        uint32_t cflags, ctry_start, ctry_end;
        std::memcpy(&cflags,    entry + 0, sizeof(cflags));
        std::memcpy(&ctry_start, entry + 4, sizeof(ctry_start));
        std::memcpy(&ctry_end,  entry + 8, sizeof(ctry_end));

        // Finally (0x2) or Fault (0x4)
        if ((cflags != 2 && cflags != 4)) continue;
        if (ctry_start < catch_try_start) continue;   // outside catch's try range
        if (ctry_start >= catch_try_end) continue;     // outside catch's try range
        if (ctry_start > throw_offset) continue;       // after throw point

        // Insert innermost-first: clauses appearing later (innermost) are
        // added earlier in the unwind_list so they execute first (LIFO).
        if (g_t4_unwind.unwind_count < kMaxUnwindDepth) {
            // Shift existing entries right and insert at front
            for (uint32_t j = g_t4_unwind.unwind_count; j > 0; --j) {
                g_t4_unwind.unwind_list[j] = g_t4_unwind.unwind_list[j - 1];
            }
            g_t4_unwind.unwind_list[0] = i;
            g_t4_unwind.unwind_count++;
        }
    }
}

/// Reverse-search for the innermost finally/fault covering code_offset.
/// Returns the clause index, or UINT32_MAX if not found.
static uint32_t FindInnermostFinally(const NativeMethod* nm,
                                      uint32_t code_offset) noexcept {
    if (nm->seh_table_offset == 0) return UINT32_MAX;

    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

    for (uint32_t i = count; i > 0; --i) {
        uint32_t idx = i - 1;
        const uint8_t* entry = clauses + idx * kSehClauseEntrySize;
        uint32_t cflags, ctry_start, ctry_end;
        std::memcpy(&cflags,    entry + 0, sizeof(cflags));
        std::memcpy(&ctry_start, entry + 4, sizeof(ctry_start));
        std::memcpy(&ctry_end,  entry + 8, sizeof(ctry_end));

        if (cflags == 2 &&
            code_offset >= ctry_start && code_offset < ctry_end) {
            return idx;
        }
    }
    return UINT32_MAX;
}

/// Read handler start byte offset for a clause index.
static uint32_t GetClauseHandlerOffset(const NativeMethod* nm,
                                        uint32_t clause_idx) noexcept {
    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* entry = code + nm->seh_table_offset + sizeof(uint32_t)
                           + clause_idx * kSehClauseEntrySize + 12;
    uint32_t val;
    std::memcpy(&val, entry, sizeof(val));
    return val;
}

/// Read try range for a clause index.
static void GetClauseTryRange(const NativeMethod* nm,
                               uint32_t clause_idx,
                               uint32_t* out_start,
                               uint32_t* out_end) noexcept {
    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* entry = code + nm->seh_table_offset + sizeof(uint32_t)
                           + clause_idx * kSehClauseEntrySize;
    std::memcpy(out_start, entry + 4, sizeof(uint32_t));
    std::memcpy(out_end,   entry + 8, sizeof(uint32_t));
}

#if defined(_WIN32) || defined(_WIN64)

/// Managed exception code used by CodegenThrow/CodegenRethrow.
static constexpr uint32_t kManagedSehExceptionCode = 0xE0000001;

/// Exception object pointer for the current thread (set by CodegenThrow,
/// read by the VEH handler to restore the exception object for catch blocks).
thread_local void* g_t4_exception_obj = nullptr;

/// Return address in T4 code where the exception was thrown (set by
/// ChaosT4RaiseException, read by the VEH handler to locate the NativeMethod
/// for SEH clause table walk).  Stored separately from ExceptionAddress
/// because RaiseException sets ExceptionAddress to kernel32.dll internals
/// rather than the originating T4 code.
thread_local void* g_t4_throw_ret_addr = nullptr;

/// T4 frame RSP at the throw point (set by ChaosT4RaiseException, read by
/// VEH handler to locate the register file for exception object placement).
thread_local void* g_t4_frame_rsp = nullptr;

/// SEH V3: Thread-local finally/fault unwind state.
thread_local T4UnwindState g_t4_unwind = {};

/// Reset SEH V3 unwind state to defaults.
static void ResetUnwindState() noexcept {
    g_t4_unwind.unwind_count = 0;
    g_t4_unwind.unwind_index = 0;
    g_t4_unwind.exception_in_flight = false;
    g_t4_unwind.pending_leave = false;
    g_t4_unwind.leave_target_offset = 0;
    g_t4_unwind.has_catch = false;
    g_t4_unwind.catch_handler_offset = 0;
}

// ── ChaosT4RaiseException ───────────────────────────────────────────────
// Called from T4-generated code for Throw/Rethrow instructions.  Stores the
// managed exception object in TLS, captures the T4 frame's stack pointer for
// the VEH handler to place the exception object, and triggers a VEH exception.
// The registered VEH handler catches this, walks the SEH clause table, and
// redirects RIP to the matching handler.  If no handler is found,
// RaiseException returns normally (caller should emit INT3 as safety net).

extern "C" void ChaosT4RaiseException(void* exception_obj) noexcept {
    // Store the managed exception object in TLS for the VEH handler.
    g_t4_exception_obj = exception_obj;

    // Capture the return address (address in T4 code after the `call`).
    // Used by the VEH handler to find the NativeMethod and compute the
    // code offset for SEH clause table lookup.
#if defined(_MSC_VER)
    g_t4_throw_ret_addr = _ReturnAddress();

    // Capture T4 frame RSP: the return address is at the current RSP
    // (no frame pointer for this leaf-like function), so the T4 frame
    // starts at RSP + 8.
    g_t4_frame_rsp = reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + 8);
#else
    g_t4_throw_ret_addr = __builtin_return_address(0);
    // GCC/Clang: frame address minus the return address slot offset.
    g_t4_frame_rsp = reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(__builtin_frame_address(0)) + 16);
#endif

    // Trigger the VEH exception.  The VEH handler (T4VectoredExceptionHandler)
    // will catch this, find the SEH handler via g_t4_throw_ret_addr, write
    // the exception object into all register file vreg slots, and redirect
    // RIP to the handler code.
    RaiseException(kManagedSehExceptionCode, 0, 0, nullptr);

    // If RaiseException returns, no handler was found in this method.
    // The caller (T4 generated code) should have an INT3 safety net,
    // which will crash cleanly.
    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "ChaosT4RaiseException: no handler found for exception, returning to INT3");
}

static LONG WINAPI T4VectoredExceptionHandler(EXCEPTION_POINTERS* ep) noexcept {
    if (ep == nullptr || ep->ExceptionRecord == nullptr || ep->ContextRecord == nullptr) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // ── Dispatch by exception code ──────────────────────────────────────
    const uint32_t exc_code = ep->ExceptionRecord->ExceptionCode;

    if (exc_code == kManagedSehExceptionCode) {
        // Managed throw (ChaosT4RaiseException): use TLS-stored throw
        // address for lookup instead of ExceptionAddress (which points
        // into RaiseException/kernel32 internals).
        if (g_t4_throw_ret_addr == nullptr) {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        // Find NativeMethod covering the T4 throw address
        const NativeMethod* nm = FindT4CodeByAddress(g_t4_throw_ret_addr);
        if (nm == nullptr) {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        // No SEH table → can't dispatch
        if (nm->seh_table_offset == 0) {
            return EXCEPTION_CONTINUE_SEARCH;
        }

        // Compute code offset from the T4 throw return address
        const uint8_t* code_base = static_cast<const uint8_t*>(nm->code);
        uint32_t code_offset = static_cast<uint32_t>(
            static_cast<const uint8_t*>(g_t4_throw_ret_addr) - code_base);

        // ── SEH V3: Two-phase exception dispatch ───────────────────────
        // Phase 1a: Find matching catch/filter handler (innermost first).
        uint32_t catch_handler_offset_val = 0;
        uint32_t catch_clause_idx = 0;
        bool has_catch = FindSehCatchHandler(nm, code_offset,
                                             &catch_handler_offset_val,
                                             &catch_clause_idx);
        // Phase 1b: Build finally/fault unwind list.
        ResetUnwindState();
        if (has_catch) {
            g_t4_unwind.has_catch = true;
            g_t4_unwind.catch_handler_offset = catch_handler_offset_val;
            BuildUnwindList(nm, catch_clause_idx, code_offset);
        } else {
            // No catch — scan for finally/fault covering the throw offset.
            const uint8_t* table_start = code_base + nm->seh_table_offset;
            uint32_t count;
            std::memcpy(&count, table_start, sizeof(count));
            const uint8_t* clauses = table_start + sizeof(uint32_t);
            for (uint32_t i = 0; i < count; i++) {
                const uint8_t* entry = clauses + i * kSehClauseEntrySize;
                uint32_t cflags, ctry_start, ctry_end;
                std::memcpy(&cflags,    entry + 0, sizeof(cflags));
                std::memcpy(&ctry_start, entry + 4, sizeof(ctry_start));
                std::memcpy(&ctry_end,  entry + 8, sizeof(ctry_end));
                if ((cflags == 2 || cflags == 4) &&
                    code_offset >= ctry_start && code_offset < ctry_end &&
                    g_t4_unwind.unwind_count < kMaxUnwindDepth) {
                    g_t4_unwind.unwind_list[g_t4_unwind.unwind_count++] = i;
                }
            }
        }
        g_t4_unwind.exception_in_flight = has_catch || g_t4_unwind.unwind_count > 0;

        if (!g_t4_unwind.has_catch && g_t4_unwind.unwind_count == 0) {
            // No handler found in this method — let exception propagate.
            g_t4_throw_ret_addr = nullptr;
            g_t4_frame_rsp = nullptr;
            ResetUnwindState();
            return EXCEPTION_CONTINUE_SEARCH;
        }

        // Write the exception object into ALL GPR register file slots in the
        // T4 frame so the handler code finds it regardless of which vreg
        // the register allocator assigned.
        if (g_t4_frame_rsp != nullptr) {
            uint64_t* regfile = reinterpret_cast<uint64_t*>(
                reinterpret_cast<uint8_t*>(g_t4_frame_rsp) + kT4GprFileOff);
            uint64_t ex_val = reinterpret_cast<uint64_t>(g_t4_exception_obj);
            for (uint32_t i = 0; i < kT4GprCount; i++) {
                regfile[i] = ex_val;
            }
        }

        // Phase 2: Redirect to first handler.
        ep->ContextRecord->Rcx = reinterpret_cast<ULONG_PTR>(g_t4_exception_obj);
        ep->ContextRecord->Rsp = reinterpret_cast<ULONG_PTR>(g_t4_frame_rsp);

        uint32_t target_handler_offset;
        if (g_t4_unwind.unwind_count > 0) {
            // Redirect to innermost finally/fault first.
            g_t4_unwind.unwind_index = 0;
            uint32_t fi = g_t4_unwind.unwind_list[0];
            target_handler_offset = GetClauseHandlerOffset(nm, fi);
        } else {
            // No finally/fault — go directly to catch.
            target_handler_offset = catch_handler_offset_val;
        }
        void* handler_addr = static_cast<uint8_t*>(nm->code) + target_handler_offset;
        ep->ContextRecord->Rip = reinterpret_cast<ULONG_PTR>(handler_addr);

        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "T4 VEH (managed throw V3): ret_addr={} (offset={}) -> handler at {} (offset={}), "
            "ex_obj={}, has_catch={}, unwind_count={}",
            g_t4_throw_ret_addr, code_offset, handler_addr, target_handler_offset,
            reinterpret_cast<void*>(g_t4_exception_obj),
            g_t4_unwind.has_catch, g_t4_unwind.unwind_count);

        g_t4_throw_ret_addr = nullptr;
        g_t4_frame_rsp = nullptr;

        return EXCEPTION_CONTINUE_EXECUTION;
    }

    // ── Hardware exception (AV, div-by-zero, etc.) ──────────────────────
    void* exception_addr = ep->ExceptionRecord->ExceptionAddress;
    if (exception_addr == nullptr) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // Find NativeMethod covering this address
    const NativeMethod* nm = FindT4CodeByAddress(exception_addr);
    if (nm == nullptr) {
        return EXCEPTION_CONTINUE_SEARCH;  // Not in T4 code
    }

    // If no SEH table, can't dispatch
    if (nm->seh_table_offset == 0) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // Compute offset within the generated code
    const uint8_t* code_base = static_cast<const uint8_t*>(nm->code);
    uint32_t code_offset = static_cast<uint32_t>(
        static_cast<const uint8_t*>(exception_addr) - code_base);

    // ── SEH V3: Two-phase dispatch for hardware exceptions ─────────
    ResetUnwindState();
    {
        uint32_t catch_handler_offset_val = 0;
        uint32_t catch_clause_idx = 0;
        bool has_catch = FindSehCatchHandler(nm, code_offset,
                                             &catch_handler_offset_val,
                                             &catch_clause_idx);
        if (has_catch) {
            g_t4_unwind.has_catch = true;
            g_t4_unwind.catch_handler_offset = catch_handler_offset_val;
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
                if ((cf == 2 || cf == 4) &&
                    code_offset >= ts && code_offset < te &&
                    g_t4_unwind.unwind_count < kMaxUnwindDepth) {
                    g_t4_unwind.unwind_list[g_t4_unwind.unwind_count++] = i;
                }
            }
        }
        g_t4_unwind.exception_in_flight = g_t4_unwind.has_catch || g_t4_unwind.unwind_count > 0;
    }
    if (!g_t4_unwind.has_catch && g_t4_unwind.unwind_count == 0) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    uint32_t target_handler_offset;
    if (g_t4_unwind.unwind_count > 0) {
        g_t4_unwind.unwind_index = 0;
        target_handler_offset = GetClauseHandlerOffset(nm, g_t4_unwind.unwind_list[0]);
    } else {
        target_handler_offset = g_t4_unwind.catch_handler_offset;
    }
    void* handler_addr = static_cast<uint8_t*>(nm->code) + target_handler_offset;
    ep->ContextRecord->Rcx = reinterpret_cast<ULONG_PTR>(g_t4_exception_obj);
    ep->ContextRecord->Rip = reinterpret_cast<ULONG_PTR>(handler_addr);

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "T4 VEH (hardware V3): exception at {} (offset={}) -> handler at {} (offset={}), "
        "has_catch={}, unwind_count={}",
        exception_addr, code_offset, handler_addr, target_handler_offset,
        g_t4_unwind.has_catch, g_t4_unwind.unwind_count);

    return EXCEPTION_CONTINUE_EXECUTION;
}

// ── Win64 Personality Routine (V2) ──────────────────────────────────────
// Called by the OS unwinder during exception dispatch.  This is the second
// line of defense after the VEH handler:

extern "C" EXCEPTION_DISPOSITION T4PersonalityRoutine(
    PEXCEPTION_RECORD ExceptionRecord,
    ULONG64 /*EstablisherFrame*/,
    PCONTEXT ContextRecord,
    PDISPATCHER_CONTEXT DispatcherContext) noexcept {

    if (ExceptionRecord == nullptr || ContextRecord == nullptr ||
        DispatcherContext == nullptr) {
        return ExceptionContinueSearch;
    }

    // Only handle managed exceptions (0xE0000001).
    if (ExceptionRecord->ExceptionCode != kManagedSehExceptionCode) {
        return ExceptionContinueSearch;
    }

    // Find NativeMethod covering the ControlPc
    const NativeMethod* nm = FindT4CodeByAddress(
        reinterpret_cast<const void*>(DispatcherContext->ControlPc));
    if (nm == nullptr) {
        return ExceptionContinueSearch;
    }

    // No SEH table — can't dispatch
    if (nm->seh_table_offset == 0) {
        return ExceptionContinueSearch;
    }

    // Compute code offset from the T4 code base
    const uint8_t* code_base = static_cast<const uint8_t*>(nm->code);
    uint32_t code_offset = static_cast<uint32_t>(
        reinterpret_cast<const uint8_t*>(DispatcherContext->ControlPc) - code_base);

    // ── SEH V3: Two-phase dispatch for personality routine ────────
    ResetUnwindState();
    {
        uint32_t catch_handler_offset_val = 0;
        uint32_t catch_clause_idx = 0;
        bool has_catch = FindSehCatchHandler(nm, code_offset,
                                             &catch_handler_offset_val,
                                             &catch_clause_idx);
        if (has_catch) {
            g_t4_unwind.has_catch = true;
            g_t4_unwind.catch_handler_offset = catch_handler_offset_val;
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
                if ((cf == 2 || cf == 4) &&
                    code_offset >= ts && code_offset < te &&
                    g_t4_unwind.unwind_count < kMaxUnwindDepth) {
                    g_t4_unwind.unwind_list[g_t4_unwind.unwind_count++] = i;
                }
            }
        }
        g_t4_unwind.exception_in_flight = g_t4_unwind.has_catch || g_t4_unwind.unwind_count > 0;
    }
    if (!g_t4_unwind.has_catch && g_t4_unwind.unwind_count == 0) {
        return ExceptionContinueSearch;
    }

    uint32_t target_handler_offset;
    if (g_t4_unwind.unwind_count > 0) {
        g_t4_unwind.unwind_index = 0;
        target_handler_offset = GetClauseHandlerOffset(nm, g_t4_unwind.unwind_list[0]);
    } else {
        target_handler_offset = g_t4_unwind.catch_handler_offset;
    }
    void* handler_addr = static_cast<uint8_t*>(nm->code) + target_handler_offset;
    ContextRecord->Rip = reinterpret_cast<ULONG_PTR>(handler_addr);
    ContextRecord->Rcx = reinterpret_cast<ULONG_PTR>(g_t4_exception_obj);

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "T4 personality (V3): clause matches offset={} (ControlPc={}) "
        "-> handler at {}, has_catch={}, unwind_count={}",
        code_offset,
        reinterpret_cast<const void*>(DispatcherContext->ControlPc),
        handler_addr,
        g_t4_unwind.has_catch, g_t4_unwind.unwind_count);

    return ExceptionCollidedUnwind;
}

// ── T4EndFinallyHelper ───────────────────────────────────────────────────
// Called from T4-generated EndFinally/EndFilter instructions at runtime.
// Uses _ReturnAddress() to find the NativeMethod and the current code offset.
//
// Returns the native byte offset of the next handler to execute, or 0 if
// unwinding is complete (EndFinally falls through to normal execution).
extern "C" void* T4EndFinallyHelper() noexcept {
    // If no exception in flight and no pending leave, this is a normal
    // fall-through (shouldn't happen since EndFinally shouldn't be reached
    // without exception/leave, but safe to handle).
    if (!g_t4_unwind.exception_in_flight && !g_t4_unwind.pending_leave) {
    // T4EndFinallyHelper: no state (unexpected EndFinally)
    CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "T4EndFinallyHelper: no unwind state");
        return nullptr;
    }

    if (g_t4_unwind.exception_in_flight) {
        // Advance to next finally/fault in unwind list.
        g_t4_unwind.unwind_index++;

        if (g_t4_unwind.unwind_index < g_t4_unwind.unwind_count) {
            // More finally/fault to execute — return next handler offset.
            uint32_t fi = g_t4_unwind.unwind_list[g_t4_unwind.unwind_index];

            void* ret_addr = _ReturnAddress();
            const NativeMethod* nm = FindT4CodeByAddress(ret_addr);
            if (nm != nullptr) {
                return static_cast<uint8_t*>(nm->code) + GetClauseHandlerOffset(nm, fi);
            }
            return nullptr;
        }

        // All finally/fault executed.
        if (g_t4_unwind.has_catch) {
            // Transfer to catch handler.
            uint32_t catch_off = g_t4_unwind.catch_handler_offset;
            void* ret_addr = _ReturnAddress();
            const NativeMethod* nm = FindT4CodeByAddress(ret_addr);
            ResetUnwindState();
            if (nm != nullptr) {
                return static_cast<uint8_t*>(nm->code) + catch_off;
            }
            return nullptr;
        }

        // No catch — re-raise the exception to propagate to caller.
        ResetUnwindState();
        void* ex_obj = g_t4_exception_obj;
        ChaosT4RaiseException(ex_obj);
        return nullptr;
    }

    if (g_t4_unwind.pending_leave) {
        uint32_t leave_target = g_t4_unwind.leave_target_offset;
        ResetUnwindState();
        void* ret_addr = _ReturnAddress();
        const NativeMethod* nm = FindT4CodeByAddress(ret_addr);
        if (nm != nullptr) {
            void* target = static_cast<uint8_t*>(nm->code) + leave_target;
            return target;
        }
        return nullptr;
    }

    return nullptr;
}

// ── T4LeaveHelper ────────────────────────────────────────────────────────
// Called from T4-generated Leave instructions when a finally/fault clause
// covers the current try block.
//
// Resolution: uses _ReturnAddress() to find the NativeMethod, then reads
// instr_offsets[] to resolve target_instr_idx and current_instr_idx to
// native byte offsets.
//
// Returns the native byte offset of the innermost finally handler, or 0 if
// no finally/fault covers the current offset (caller falls through to
// normal JMP).
extern "C" void* T4LeaveHelper(uint32_t target_instr_idx,
                                uint32_t current_instr_idx) noexcept {
    void* ret_addr = _ReturnAddress();
    const NativeMethod* nm = FindT4CodeByAddress(ret_addr);
    if (nm == nullptr || nm->instr_offsets == nullptr) {
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "T4LeaveHelper: FAILED to find NativeMethod");
        return nullptr;
    }

    // Resolve current byte offset
    uint32_t current_offset = (current_instr_idx < nm->instr_offset_count)
        ? nm->instr_offsets[current_instr_idx] : 0;

    // Find innermost finally covering current offset
    uint32_t finally_idx = FindInnermostFinally(nm, current_offset);
    if (finally_idx == UINT32_MAX) {
        return nullptr;  // No finally — normal JMP
    }

    // Resolve leave target byte offset
    uint32_t target_offset = (target_instr_idx < nm->instr_offset_count)
        ? nm->instr_offsets[target_instr_idx] : 0;

    // Set up pending_leave state
    ResetUnwindState();
    g_t4_unwind.pending_leave = true;
    g_t4_unwind.leave_target_offset = target_offset;

    void* handler = static_cast<uint8_t*>(nm->code) + GetClauseHandlerOffset(nm, finally_idx);
    return handler;
}

void RegisterT4SehHandler() noexcept {
    // Register VEH handler (first in the handler chain = last called).
    // PVOID WINAPI AddVectoredExceptionHandler(ULONG First, PVECTORED_EXCEPTION_HANDLER Handler);
    // First=0 → handler is appended (called last, after all other handlers).
    // First=1 → handler is prepended (called first).
    // Use First=1 so our handler runs before the OS default handler.
    PVOID handle = AddVectoredExceptionHandler(1, T4VectoredExceptionHandler);
    if (handle != nullptr) {
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "T4 VEH handler registered");
    } else {
        CHAOS_IL2CPP_LOG_ERROR_M("codegen", "T4 VEH handler registration FAILED");
    }

    // Register GC event callback for deferred T4 code memory reclamation.
    // Fires at GC safepoint (STW), guaranteeing no thread is executing T4 code.
    chaos::il2cpp::runtime_core::GcRegisterEventCallback(OnGcSafepoint, nullptr);

    // Register T4 demotion callbacks so method_replacement can demote T4
    // entries through runtime_core without a circular build dependency.
    chaos::il2cpp::runtime_core::RegisterT4DemotionCallbacks(
        DemoteT4ByToken, DemoteT4ByCallSiteToken);
}

#else  // not Windows

void RegisterT4SehHandler() noexcept {
    // VEH is Windows-specific. On POSIX, signal handlers would be used instead.
    CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "T4 SEH handler: not implemented for this platform");

    // Register GC event callback for deferred T4 code memory reclamation.
    chaos::il2cpp::runtime_core::GcRegisterEventCallback(OnGcSafepoint, nullptr);

    // Register T4 demotion callbacks (platform-independent).
    chaos::il2cpp::runtime_core::RegisterT4DemotionCallbacks(
        DemoteT4ByToken, DemoteT4ByCallSiteToken);
}

// Non-Windows stub for ChaosT4RaiseException.
// VEH is Windows-only; on POSIX platforms this should never be called
// because T4 codegen with SEH is not enabled.  If called, crash with INT3.
extern "C" void ChaosT4RaiseException(void* /*exception_obj*/) noexcept {
    #if defined(__GNUC__) || defined(__clang__)
        __builtin_trap();
    #else
        // Fallback: intentional null-deref for crash
        volatile int* p = nullptr;
        *p = 0;
    #endif
}

#endif  // _WIN32 || _WIN64

}  // namespace chaos::il2cpp::codegen
