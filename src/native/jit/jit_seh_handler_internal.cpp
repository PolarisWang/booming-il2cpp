// jit_seh_handler_internal.cpp — JIT SEH handler standalone implementation (unit-test support)
//
// This file provides a self-contained implementation of the T4 SEH handler
// internal functions for unit testing. It is included directly by
// jit_seh_handler_test.cpp (38 tests) to allow access to static/internal
// functions that are now private members of WinSehHandler/LinuxSehHandler
// in the production code path.
//
// NOTE: This file is NOT compiled into chaos_jit.lib.  The production code
// uses WinSehHandler/LinuxSehHandler through the ISehHandler interface in
// jit_seh.cpp.  This standalone copy exists solely so the unit tests can
// verify internal SEH dispatch logic in isolation from the VEH infrastructure.
//
// The logic mirrors the equivalent code in WinSehHandler.cpp but operates on
// module-level global state instead of class members.

#include "jit_method.h"
#include "jit_seh.h"  // for JitUnwindState, kMaxUnwindDepth

#include <atomic>
#include <cstdint>
#include <cstring>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
  #define NOMINMAX
  #include <windows.h>
  #include <intrin.h>
#endif

namespace chaos::il2cpp::jit {

// ── Constants ─────────────────────────────────────────────────────────────────

static constexpr uint32_t kSehClauseEntrySize     = 5 * sizeof(uint32_t);

// ── Types ──────────────────────────────────────────────────────────────────

struct T4CodeEntry {
    const void*       code_start = nullptr;
    uint32_t          code_size  = 0;
    const JitMethod*  nm         = nullptr;
    uint32_t          patch_method_token = 0;
};

struct PendingFreeRegion {
    void*    code_start = nullptr;
    uint32_t code_size  = 0;
    bool     active     = false;
};

// Old name for JitUnwindState (used by jit_seh_handler_test.cpp).
using T4UnwindState = JitUnwindState;

// ── Global State ──────────────────────────────────────────────────────────────
//
// These globals mirror the private members of WinSehHandler.  In the production
// code path they are encapsulated in the class; here they are exposed so that
// jit_seh_handler_test.cpp can set up and tear down state between tests.

std::vector<T4CodeEntry> g_t4_code_entries;
long        g_t4_code_lock = 0;  // InterlockedExchange-compatible for tests

std::vector<PendingFreeRegion> g_pending_free;

uint32_t g_t4_lookup_generation = 1;

thread_local struct {
    uintptr_t         page_base = 0;
    const JitMethod*  nm        = nullptr;
    uint32_t          generation = 0;
} g_t4_lookup_cache;

thread_local JitUnwindState g_t4_unwind{};
thread_local void* g_t4_exception_obj   = nullptr;
thread_local void* g_t4_throw_ret_addr  = nullptr;
thread_local void* g_t4_frame_rsp       = nullptr;

// ── Spinlock Helpers ──────────────────────────────────────────────────────────

static void AcquireCodeLock() noexcept {
#if defined(_MSC_VER)
    while (InterlockedExchange(&g_t4_code_lock, 1) != 0) {
        _mm_pause();
    }
#else
    while (InterlockedExchange(&g_t4_code_lock, 1) != 0) {
        // yield hint
    }
#endif
}

static void ReleaseCodeLock() noexcept {
    InterlockedExchange(&g_t4_code_lock, 0);
}

// ── InvalidateLookupCache ─────────────────────────────────────────────────────

void InvalidateLookupCache() noexcept {
    g_t4_lookup_generation++;
}

// ── ResetUnwindState ──────────────────────────────────────────────────────────

void ResetUnwindState() noexcept {
    g_t4_unwind = {};
}

// ── EnqueueDemotedCode ────────────────────────────────────────────────────────
// Adds a demoted code region to the pending-free table (deferred to next GC
// safepoint).  Deduplicates by address and rejects null/zero-size entries.

void EnqueueDemotedCode(void* code_start, uint32_t code_size) noexcept {
    if (code_start == nullptr || code_size == 0) return;

    // Deduplicate: if this exact address is already tracked, skip.
    for (auto& region : g_pending_free) {
        if (region.active && region.code_start == code_start) return;
    }

    // Reuse an inactive slot, or add a new entry.
    for (auto& region : g_pending_free) {
        if (!region.active) {
            region.code_start = code_start;
            region.code_size  = code_size;
            region.active     = true;
            return;
        }
    }

    g_pending_free.push_back({code_start, code_size, true});
}

// ── ReclaimDemotedCode ────────────────────────────────────────────────────────
// T2.3-C 方案3: does NOT free demoted code.  Demoted/jit code regions stay
// live (process-lifetime allocation, address never reused) so no thread can
// run on freed memory and old frames stay GC/SEH-resolvable.  The pending
// table is retained for accounting; nothing is VirtualFree'd/munmap'd.

void ReclaimDemotedCode() noexcept {
    // Retained for process lifetime per 方案3 (see class handler docs); the
    // pending table is kept for accounting, nothing is freed here.
    (void)g_pending_free;
}

// ── FindNativeCodeByAddress ───────────────────────────────────────────────────
// Finds the JitMethod covering a given code address using a TLS page-aligned
// cache for fast repeated lookups within the same 4KB page.

const JitMethod* FindNativeCodeByAddress(const void* address) noexcept {
    uintptr_t addr_val = reinterpret_cast<uintptr_t>(address);
    uintptr_t page     = addr_val >> 12;

    // Fast path: check the thread-local page-aligned cache.
    uint32_t gen = g_t4_lookup_generation;
    if (g_t4_lookup_cache.nm != nullptr &&
        g_t4_lookup_cache.page_base == page &&
        g_t4_lookup_cache.generation == gen) {
        const auto* cached_start = static_cast<const uint8_t*>(g_t4_lookup_cache.nm->code);
        const auto* cached_end   = cached_start + g_t4_lookup_cache.nm->code_size;
        const auto* addr_bytes   = static_cast<const uint8_t*>(address);
        if (addr_bytes >= cached_start && addr_bytes < cached_end) {
            return g_t4_lookup_cache.nm;
        }
    }

    // Slow path: linear scan the registry.
    for (const auto& entry : g_t4_code_entries) {
        const uint8_t* start = static_cast<const uint8_t*>(entry.code_start);
        if (start == nullptr) continue;
        const uint8_t* end  = start + entry.code_size;
        const uint8_t* addr = static_cast<const uint8_t*>(address);
        if (addr >= start && addr < end) {
            g_t4_lookup_cache.page_base   = page;
            g_t4_lookup_cache.nm          = entry.nm;
            g_t4_lookup_cache.generation  = gen;
            return entry.nm;
        }
    }
    return nullptr;
}

// ── RegisterNativeCodeSection ─────────────────────────────────────────────────

void RegisterNativeCodeSection(void* code_start, uint32_t code_size,
                                const JitMethod* nm,
                                uint32_t patch_method_token) noexcept {
    if (code_start == nullptr || code_size == 0 || nm == nullptr) return;

    AcquireCodeLock();
    T4CodeEntry entry;
    entry.code_start         = code_start;
    entry.code_size          = code_size;
    entry.nm                 = nm;
    entry.patch_method_token = patch_method_token;
    g_t4_code_entries.push_back(entry);
    ReleaseCodeLock();
}

// ── UnregisterNativeCodeSection ───────────────────────────────────────────────

void UnregisterNativeCodeSection(void* code_start) noexcept {
    if (code_start == nullptr) return;

    AcquireCodeLock();
    for (auto& entry : g_t4_code_entries) {
        if (entry.code_start == code_start) {
            // T2.3-C 方案3: keep the entry + code alive.  Demoted code is never
            // freed and its address never reused, so an in-flight old frame's
            // return address stays GC/SEH-resolvable; we only mark the JitMethod
            // externally-managed so teardown does not double-free it.
            if (entry.nm != nullptr) {
                const_cast<JitMethod*>(entry.nm)->code_managed_externally = true;
            }
            break;
        }
    }
    ReleaseCodeLock();
    InvalidateLookupCache();
}

// ── DemoteJittedMethod ────────────────────────────────────────────────────────

uint32_t DemoteJittedMethod(uint32_t method_token) noexcept {
    if (method_token == 0) return 0;
    uint32_t count = 0;

    AcquireCodeLock();
    for (auto& entry : g_t4_code_entries) {
        if (entry.patch_method_token == method_token &&
            entry.nm != nullptr) {
            // T2.3-C 方案3: do NOT null the entry or enqueue the code for
            // free.  Demoted old code stays alive and address-registered for
            // the process lifetime, and its root maps (GcSlotMapV0 /
            // GcPointMapV0) stay registered, so an in-flight old frame's
            // return address still resolves to the old JitMethod and its GC
            // maps — no missed root, and ReclaimDemoted never frees it (no
            // use-after-free).  New calls are redirected by direct_ptr on
            // recompile; the registry is only consulted by GC/SEH.
            const_cast<JitMethod*>(entry.nm)->code_managed_externally = true;
            ++count;
        }
    }
    ReleaseCodeLock();

    if (count > 0) {
        InvalidateLookupCache();
    }
    return count;
}

// ── DemoteT4ByCallSiteToken ───────────────────────────────────────────────────

uint32_t DemoteT4ByCallSiteToken(uint32_t method_token) noexcept {
    if (method_token == 0) return 0;
    uint32_t count = 0;

    AcquireCodeLock();
    for (auto& entry : g_t4_code_entries) {
        const auto* nm = entry.nm;
        if (nm == nullptr) continue;
        for (uint32_t j = 0; j < nm->call_site_count; j++) {
            if (nm->call_sites[j].method_token == method_token) {
                // T2.3-C 方案3: keep the caller's entry + code alive (see
                // DemoteJittedMethod).  Old frames in this caller remain
                // GC/SEH-resolvable; nothing is enqueued for free.
                const_cast<JitMethod*>(nm)->code_managed_externally = true;
                ++count;
                break;
            }
        }
    }
    ReleaseCodeLock();

    if (count > 0) {
        InvalidateLookupCache();
    }
    return count;
}

// ── Internal SEH Clause Table Walkers ─────────────────────────────────────────

/// Find the innermost catch/filter clause (flags=0 or flags=1) covering
/// code_offset.  Returns true if a matching clause is found.
static bool FindSehCatchHandler(const JitMethod* nm,
                                 uint32_t code_offset,
                                 uint32_t* out_handler_offset,
                                 uint32_t* out_clause_idx) noexcept {
    if (nm->seh_table_offset == 0) return false;

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

/// Collect finally/fault clauses (flags=2 or flags=4) nested within the
/// catch clause's try range and covering the throw offset.  Populates
/// g_t4_unwind.unwind_list in innermost-first order.
static void BuildUnwindListForCatch(const JitMethod* nm,
                                     uint32_t catch_clause_idx,
                                     uint32_t throw_offset) noexcept {
    if (nm->seh_table_offset == 0) return;

    const uint8_t* code  = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

    // Read the catch clause's try range.
    const uint8_t* catch_entry = clauses + catch_clause_idx * kSehClauseEntrySize;
    uint32_t catch_try_start, catch_try_end;
    std::memcpy(&catch_try_start, catch_entry + 4, sizeof(catch_try_start));
    std::memcpy(&catch_try_end,   catch_entry + 8, sizeof(catch_try_end));

    for (uint32_t i = 0; i < count; i++) {
        const uint8_t* entry = clauses + i * kSehClauseEntrySize;
        uint32_t cflags, ctry_start, ctry_end;
        std::memcpy(&cflags,    entry + 0, sizeof(cflags));
        std::memcpy(&ctry_start, entry + 4, sizeof(ctry_start));
        std::memcpy(&ctry_end,  entry + 8, sizeof(ctry_end));

        if (cflags != 2 && cflags != 4) continue;
        if (ctry_start < catch_try_start || ctry_start >= catch_try_end) continue;
        if (ctry_start > throw_offset) continue;

        if (g_t4_unwind.unwind_count < kMaxUnwindDepth) {
            g_t4_unwind.unwind_list[g_t4_unwind.unwind_count++] = i;
        }
    }
}

/// Collect all finally/fault clauses covering code_offset (used when no
/// catch clause is found).  Populates g_t4_unwind.unwind_list in the order
/// encountered (reverse scan).
static void CollectFinallyFaultAtOffset(const JitMethod* nm,
                                         uint32_t code_offset) noexcept {
    if (nm->seh_table_offset == 0) return;

    const uint8_t* code  = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

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

/// Read the handler start byte offset for a given clause index.
static uint32_t GetClauseHandlerOffset(const JitMethod* nm,
                                        uint32_t clause_idx) noexcept {
    const uint8_t* entry = static_cast<const uint8_t*>(nm->code) +
                           nm->seh_table_offset + sizeof(uint32_t) +
                           clause_idx * kSehClauseEntrySize + 12;
    uint32_t val;
    std::memcpy(&val, entry, sizeof(val));
    return val;
}

// ── BuildSehExceptionDispatch ─────────────────────────────────────────────────
// Two-phase SEH dispatch builder:
//   Phase 1a: Find matching catch/filter handler for code_offset.
//   Phase 1b: Build finally/fault unwind list (nested within catch's try range).
// Returns true if a catch handler was found; false if only finally/fault or nothing.

bool BuildSehExceptionDispatch(const JitMethod* nm, uint32_t code_offset) noexcept {
    ResetUnwindState();

    if (nm == nullptr || nm->seh_table_offset == 0) {
        return false;
    }

    // Phase 1a: Find matching catch/filter handler.
    uint32_t catch_handler_offset_val = 0;
    uint32_t catch_clause_idx = 0;
    bool has_catch = FindSehCatchHandler(nm, code_offset,
                                          &catch_handler_offset_val,
                                          &catch_clause_idx);

    if (has_catch) {
        g_t4_unwind.has_catch             = true;
        g_t4_unwind.catch_handler_offset  = catch_handler_offset_val;
        BuildUnwindListForCatch(nm, catch_clause_idx, code_offset);
    } else {
        // No catch found — collect all finally/fault covering the offset.
        CollectFinallyFaultAtOffset(nm, code_offset);
    }

    g_t4_unwind.exception_in_flight = has_catch || g_t4_unwind.unwind_count > 0;
    return has_catch;
}

// ── FindSehHandlerForOffset ───────────────────────────────────────────────────
// Simple forward scan: returns the handler_start byte offset of the first
// clause covering code_offset.  Returns 0xFFFFFFFF if no clause matches.

uint32_t FindSehHandlerForOffset(const JitMethod* nm, uint32_t code_offset) noexcept {
    if (nm == nullptr || nm->seh_table_offset == 0) {
        return 0xFFFFFFFF;
    }

    const uint8_t* code  = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

    for (uint32_t i = 0; i < count; i++) {
        const uint8_t* entry = clauses + i * kSehClauseEntrySize;
        uint32_t cflags, ctry_start, ctry_end;
        std::memcpy(&cflags,    entry + 0, sizeof(cflags));
        std::memcpy(&ctry_start, entry + 4, sizeof(ctry_start));
        std::memcpy(&ctry_end,  entry + 8, sizeof(ctry_end));

        if (code_offset >= ctry_start && code_offset < ctry_end) {
            uint32_t handler_st;
            std::memcpy(&handler_st, entry + 12, sizeof(handler_st));
            return handler_st;
        }
    }
    return 0xFFFFFFFF;
}

// ── T4EndFinallyHelper ────────────────────────────────────────────────────────
// Called by EndFinally/EndFilter at runtime to advance the finally unwind chain.
// Returns the native byte offset of the next handler to execute, or nullptr if
// unwinding is complete.

void* T4EndFinallyHelper() noexcept {
    if (!g_t4_unwind.exception_in_flight && !g_t4_unwind.pending_leave) {
        return nullptr;
    }

    if (g_t4_unwind.exception_in_flight) {
        g_t4_unwind.unwind_index++;

        if (g_t4_unwind.unwind_index < g_t4_unwind.unwind_count) {
            uint32_t fi = g_t4_unwind.unwind_list[g_t4_unwind.unwind_index];
#if defined(_MSC_VER)
            const JitMethod* nm = FindNativeCodeByAddress(_ReturnAddress());
#else
            const JitMethod* nm = FindNativeCodeByAddress(
                __builtin_return_address(0));
#endif
            if (nm != nullptr) {
                return static_cast<uint8_t*>(nm->code) +
                       GetClauseHandlerOffset(nm, fi);
            }
            return nullptr;
        }

        if (g_t4_unwind.has_catch) {
            uint32_t catch_off = g_t4_unwind.catch_handler_offset;
#if defined(_MSC_VER)
            const JitMethod* nm = FindNativeCodeByAddress(_ReturnAddress());
#else
            const JitMethod* nm = FindNativeCodeByAddress(
                __builtin_return_address(0));
#endif
            ResetUnwindState();
            if (nm != nullptr) {
                return static_cast<uint8_t*>(nm->code) + catch_off;
            }
            return nullptr;
        }

        // No catch but finally/fault ran — rethrow (stub: just clean up).
        ResetUnwindState();
        return nullptr;
    }

    if (g_t4_unwind.pending_leave) {
        uint32_t leave_target = g_t4_unwind.leave_target_offset;
        ResetUnwindState();
#if defined(_MSC_VER)
        const JitMethod* nm = FindNativeCodeByAddress(_ReturnAddress());
#else
        const JitMethod* nm = FindNativeCodeByAddress(
            __builtin_return_address(0));
#endif
        if (nm != nullptr) {
            return static_cast<uint8_t*>(nm->code) + leave_target;
        }
        return nullptr;
    }

    return nullptr;
}

}  // namespace chaos::il2cpp::jit
