#include "LinuxSehHandler.h"
#include "WinSehHandler.h"   // for GetWinSehHandler, shared TLS defs
#include "jit_seh.h"

#include <chaos/log.h>

#include <t4_demotion.h>
#include <gc_events.h>

#include <atomic>
#include <cstdint>
#include <cstring>

#include <signal.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <sched.h>
#include <unistd.h>

// ── kSpinLimitHard — spinlock warning threshold
static constexpr uint32_t kSpinLimitHard = 1024 * 1024;

namespace chaos::il2cpp::jit {

// ── TLS Lookup Cache ────────────────────────────────────────────────────────
static thread_local struct {
    uintptr_t         page_base   = 0;
    const JitMethod*  nm          = nullptr;
    uint32_t          generation  = 0;
} g_t4_lookup_cache;

// ── TLS Exception State ────────────────────────────────────────────────────
// Mirrors the Windows TLS state used by the SEH clause dispatcher.
// Defined in WinSehHandler.cpp under the Windows guard; we duplicate the
// declarations here for Linux.  The actual instances live at the same
// symbol names so the T4 personality/endfinally/leave helpers can find them.
// On Linux these are provided here.
thread_local void* g_t4_exception_obj = nullptr;
thread_local void* g_t4_throw_ret_addr = nullptr;
thread_local void* g_t4_frame_rsp = nullptr;
thread_local T4UnwindState g_t4_unwind = {};

// ── SEH Clause Table Constants ─────────────────────────────────────────────
static constexpr uint32_t kSehClauseEntrySize = 5 * sizeof(uint32_t);
static constexpr uint32_t kT4GprFileOff = 32;

// ═══════════════════════════════════════════════════════════════════════════
// Spinlock
// ═══════════════════════════════════════════════════════════════════════════

void LinuxSehHandler::AcquireLock() noexcept {
    uint32_t spins = 0;
    while (lock_.exchange(1, std::memory_order_acquire) != 0) {
        if (++spins % 64 == 0) sched_yield();
        if (spins > kSpinLimitHard) {
            CHAOS_IL2CPP_LOG_WARN_M("codegen",
                "LinuxSehHandler: spinning for {} iterations", spins);
            spins = 0;
        }
    }
}

void LinuxSehHandler::ReleaseLock() noexcept {
    lock_.store(0, std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════════════════
// Demotion helpers
// ═══════════════════════════════════════════════════════════════════════════

void LinuxSehHandler::EnqueueDemotedCode(void* code_start, uint32_t code_size) noexcept {
    if (code_start == nullptr || code_size == 0) return;
    for (uint32_t i = 0; i < kMaxPendingFreeRegions; i++) {
        if (pending_free_[i].active && pending_free_[i].code_start == code_start) return;
    }
    for (uint32_t i = 0; i < kMaxPendingFreeRegions; i++) {
        if (!pending_free_[i].active) {
            pending_free_[i].code_start = code_start;
            pending_free_[i].code_size  = code_size;
            pending_free_[i].active     = true;
            pending_free_count_++;
            return;
        }
    }
    CHAOS_IL2CPP_LOG_WARN_M("codegen",
        "pending-free table full ({} entries)", kMaxPendingFreeRegions);
}

void LinuxSehHandler::InvalidateLookupCache() noexcept {
    lookup_generation_.fetch_add(1, std::memory_order_release);
}

// ═══════════════════════════════════════════════════════════════════════════
// ISehHandler interface
// ═══════════════════════════════════════════════════════════════════════════

LinuxSehHandler::~LinuxSehHandler() noexcept {}

void LinuxSehHandler::RegisterCode(void* code_start, uint32_t code_size,
                                    const JitMethod* nm,
                                    uint32_t patch_method_token) noexcept {
    if (code_start == nullptr || code_size == 0 || nm == nullptr) return;

    {
        AcquireLock();
        if (count_ >= kMaxT4CodeEntries) {
            CHAOS_IL2CPP_LOG_WARN_M("codegen", "RegisterCode: registry full ({})", kMaxT4CodeEntries);
            ReleaseLock();
            return;
        }
        entries_[count_].code_start = code_start;
        entries_[count_].code_size  = code_size;
        entries_[count_].nm         = nm;
        entries_[count_].patch_method_token = patch_method_token;
        count_++;
        ReleaseLock();
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "RegisterCode: code={} size={} seh_offset={} token={}",
        code_start, code_size, nm->seh_table_offset, patch_method_token);

    // Register DWARF .eh_frame for libgcc stack unwinding (Linux only).
    if (nm->eh_frame_offset > 0) {
        const void* eh_frame = static_cast<const uint8_t*>(code_start) + nm->eh_frame_offset;
        __register_frame(eh_frame);
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "RegisterCode: registered .eh_frame at offset {}", nm->eh_frame_offset);
    }
}

void LinuxSehHandler::UnregisterCode(void* code_start) noexcept {
    if (code_start == nullptr) return;
    {
        AcquireLock();
        for (uint32_t i = 0; i < count_; i++) {
            if (entries_[i].code_start == code_start) {
                EnqueueDemotedCode(
                    const_cast<void*>(entries_[i].code_start),
                    entries_[i].code_size);
                entries_[i].nm = nullptr;
                break;
            }
        }
        ReleaseLock();
    }
    InvalidateLookupCache();
}

const JitMethod* LinuxSehHandler::FindCodeByAddress(const void* address) noexcept {
    uintptr_t addr_val = reinterpret_cast<uintptr_t>(address);
    uintptr_t page = addr_val >> 12;

    uint32_t gen = lookup_generation_.load(std::memory_order_acquire);
    if (g_t4_lookup_cache.nm != nullptr &&
        g_t4_lookup_cache.page_base == page &&
        g_t4_lookup_cache.generation == gen) {
        return g_t4_lookup_cache.nm;
    }

    for (uint32_t i = 0; i < count_; i++) {
        const auto& entry = entries_[i];
        const uint8_t* start = static_cast<const uint8_t*>(entry.code_start);
        if (start == nullptr) continue;
        const uint8_t* end = start + entry.code_size;
        const uint8_t* addr = static_cast<const uint8_t*>(address);
        if (addr >= start && addr < end) {
            g_t4_lookup_cache.page_base = page;
            g_t4_lookup_cache.nm = entry.nm;
            g_t4_lookup_cache.generation = gen;
            return entry.nm;
        }
    }
    return nullptr;
}

uint32_t LinuxSehHandler::DemoteByToken(uint32_t method_token) noexcept {
    if (method_token == 0) return 0;
    uint32_t count = 0;
    {
        AcquireLock();
        for (uint32_t i = 0; i < count_; i++) {
            if (entries_[i].patch_method_token == method_token &&
                entries_[i].nm != nullptr) {
                EnqueueDemotedCode(
                    const_cast<void*>(entries_[i].code_start),
                    entries_[i].code_size);
                entries_[i].nm = nullptr;
                count++;
            }
        }
        ReleaseLock();
    }
    if (count > 0) {
        InvalidateLookupCache();
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "DemoteByToken: token={} demoted {} entries", method_token, count);
    }
    return count;
}

uint32_t LinuxSehHandler::DemoteByCallSiteToken(uint32_t method_token) noexcept {
    if (method_token == 0) return 0;
    uint32_t count = 0;
    {
        AcquireLock();
        for (uint32_t i = 0; i < count_; i++) {
            const auto* nm = entries_[i].nm;
            if (nm == nullptr) continue;
            for (uint32_t j = 0; j < nm->call_site_count; j++) {
                if (nm->call_sites[j].method_token == method_token) {
                    EnqueueDemotedCode(
                        const_cast<void*>(entries_[i].code_start),
                        entries_[i].code_size);
                    entries_[i].nm = nullptr;
                    count++;
                    break;
                }
            }
        }
        ReleaseLock();
    }
    if (count > 0) {
        InvalidateLookupCache();
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "DemoteByCallSiteToken: token={} demoted {} entries", method_token, count);
    }
    return count;
}

void LinuxSehHandler::ReclaimDemoted() noexcept {
    for (uint32_t i = 0; i < kMaxPendingFreeRegions; i++) {
        if (!pending_free_[i].active) continue;
        int ret = munmap(pending_free_[i].code_start, pending_free_[i].code_size);
        if (ret != 0) {
            CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                "ReclaimDemoted: munmap({}, {}) failed",
                pending_free_[i].code_start, pending_free_[i].code_size);
        }
        pending_free_[i].active = false;
        pending_free_[i].code_start = nullptr;
        pending_free_[i].code_size = 0;
    }
    pending_free_count_ = 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// SEH clause table helpers (shared with WinSehHandler logic)
// ═══════════════════════════════════════════════════════════════════════════

/// Find the innermost catch/filter clause covering code_offset.
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

/// Collect finally/fault clauses nested within the catch clause's try range.
static void BuildUnwindList(const JitMethod* nm,
                             uint32_t catch_clause_idx,
                             uint32_t throw_offset) noexcept {
    if (nm->seh_table_offset == 0) return;

    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

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

        if ((cflags != 2 && cflags != 4)) continue;
        if (ctry_start < catch_try_start) continue;
        if (ctry_start >= catch_try_end) continue;
        if (ctry_start > throw_offset) continue;

        if (g_t4_unwind.unwind_count < kMaxUnwindDepth) {
            for (uint32_t j = g_t4_unwind.unwind_count; j > 0; --j) {
                g_t4_unwind.unwind_list[j] = g_t4_unwind.unwind_list[j - 1];
            }
            g_t4_unwind.unwind_list[0] = i;
            g_t4_unwind.unwind_count++;
        }
    }
}

/// Reverse-search for the innermost finally covering code_offset.
static uint32_t FindInnermostFinally(const JitMethod* nm,
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
static uint32_t GetClauseHandlerOffset(const JitMethod* nm,
                                        uint32_t clause_idx) noexcept {
    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* entry = code + nm->seh_table_offset + sizeof(uint32_t)
                           + clause_idx * kSehClauseEntrySize + 12;
    uint32_t val;
    std::memcpy(&val, entry, sizeof(val));
    return val;
}

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

// ═══════════════════════════════════════════════════════════════════════════
// Linux signal handler for T4 hardware exceptions (SIGSEGV, SIGBUS)
// ═══════════════════════════════════════════════════════════════════════════

/// Managed exception code (same as Windows kManagedSehExceptionCode concept).
/// On Linux, we use SIGUSR1 for managed throws and SIGSEGV for hardware faults.
static constexpr int kManagedExceptionSignal = SIGUSR1;

/// Signal number used for current exception dispatch.
static thread_local int g_current_signal = 0;

/// POSIX signal handler for T4-generated code exceptions.
///
/// Handles two categories:
///   1. Hardware exceptions (SIGSEGV, SIGBUS) — from AV, null deref, etc.
///   2. Managed exceptions (SIGUSR1) — from ChaosT4RaiseException
static void T4SignalHandler(int sig, siginfo_t* info, void* ucontext) noexcept {
    auto& self = GetLinuxSehHandler();
    auto* ctx = static_cast<ucontext_t*>(ucontext);

    if (ctx == nullptr) return;

    // Determine the faulting address.
    void* fault_addr = nullptr;
    if (sig == SIGSEGV || sig == SIGBUS) {
        fault_addr = info->si_addr;
    }

    // For managed exceptions, the return address is in TLS.
    // For hardware exceptions, use the instruction pointer from the context.
    void* code_addr = (sig == kManagedExceptionSignal)
        ? g_t4_throw_ret_addr
        : reinterpret_cast<void*>(ctx->uc_mcontext.gregs[REG_RIP]);

    if (code_addr == nullptr) return;

    const JitMethod* nm = self.FindCodeByAddress(code_addr);
    if (nm == nullptr || nm->seh_table_offset == 0) return;

    const uint8_t* code_base = static_cast<const uint8_t*>(nm->code);
    uint32_t code_offset = static_cast<uint32_t>(
        static_cast<const uint8_t*>(code_addr) - code_base);

    // Find matching catch/filter handler.
    uint32_t catch_handler_offset_val = 0;
    uint32_t catch_clause_idx = 0;
    bool has_catch = FindSehCatchHandler(nm, code_offset,
                                          &catch_handler_offset_val,
                                          &catch_clause_idx);

    // Build finally/fault unwind list.
    ResetUnwindState();
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

    if (!g_t4_unwind.has_catch && g_t4_unwind.unwind_count == 0) {
        if (sig == kManagedExceptionSignal) {
            g_t4_throw_ret_addr = nullptr;
            g_t4_frame_rsp = nullptr;
            ResetUnwindState();
        }
        return;  // No handler found — let the default handler deal with it.
    }

    // Write exception object into all GPR register file slots (for managed throws).
    if (sig == kManagedExceptionSignal && g_t4_frame_rsp != nullptr) {
        uint64_t* regfile = reinterpret_cast<uint64_t*>(
            reinterpret_cast<uint8_t*>(g_t4_frame_rsp) + kT4GprFileOff);
        uint64_t ex_val = reinterpret_cast<uint64_t>(g_t4_exception_obj);
        for (uint32_t i = 0; i < kT4GprFileOff / sizeof(uint64_t); i++) {
            regfile[i] = ex_val;
        }
    }

    // Set up return state for the handler.
    uint32_t target_handler_offset;
    if (g_t4_unwind.unwind_count > 0) {
        g_t4_unwind.unwind_index = 0;
        target_handler_offset = GetClauseHandlerOffset(nm, g_t4_unwind.unwind_list[0]);
    } else {
        target_handler_offset = catch_handler_offset_val;
    }

    void* handler_addr = static_cast<uint8_t*>(nm->code) + target_handler_offset;

    // Modify the context to redirect execution to the handler.
    ctx->uc_mcontext.gregs[REG_RIP] = reinterpret_cast<greg_t>(handler_addr);
    if (sig == kManagedExceptionSignal && g_t4_frame_rsp != nullptr) {
        ctx->uc_mcontext.gregs[REG_RSP] = reinterpret_cast<greg_t>(g_t4_frame_rsp);
        ctx->uc_mcontext.gregs[REG_RCX] = reinterpret_cast<greg_t>(g_t4_exception_obj);
    }

    // Clear TLS throw state.
    if (sig == kManagedExceptionSignal) {
        g_t4_throw_ret_addr = nullptr;
        g_t4_frame_rsp = nullptr;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// ChaosT4RaiseException (Linux implementation)
// ═══════════════════════════════════════════════════════════════════════════

extern "C" void ChaosT4RaiseException(void* exception_obj) noexcept {
    g_t4_exception_obj = exception_obj;
    g_t4_throw_ret_addr = __builtin_return_address(0);
    g_t4_frame_rsp = reinterpret_cast<void*>(
        reinterpret_cast<uintptr_t>(__builtin_frame_address(0)) + 16);

    // Send ourselves SIGUSR1 — the signal handler will process the exception.
    pthread_t self = pthread_self();
    pthread_kill(self, kManagedExceptionSignal);

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "ChaosT4RaiseException: no handler found, returning to INT3");
}

// ═══════════════════════════════════════════════════════════════════════════
// T4EndFinallyHelper (Linux implementation)
// ═══════════════════════════════════════════════════════════════════════════

extern "C" void* T4EndFinallyHelper() noexcept {
    auto& self = GetLinuxSehHandler();

    if (!g_t4_unwind.exception_in_flight && !g_t4_unwind.pending_leave) {
        return nullptr;
    }

    if (g_t4_unwind.exception_in_flight) {
        g_t4_unwind.unwind_index++;

        if (g_t4_unwind.unwind_index < g_t4_unwind.unwind_count) {
            uint32_t fi = g_t4_unwind.unwind_list[g_t4_unwind.unwind_index];
            void* ret_addr = __builtin_return_address(0);
            const JitMethod* nm = self.FindCodeByAddress(ret_addr);
            if (nm != nullptr) {
                return static_cast<uint8_t*>(nm->code) + GetClauseHandlerOffset(nm, fi);
            }
            return nullptr;
        }

        if (g_t4_unwind.has_catch) {
            uint32_t catch_off = g_t4_unwind.catch_handler_offset;
            void* ret_addr = __builtin_return_address(0);
            const JitMethod* nm = self.FindCodeByAddress(ret_addr);
            ResetUnwindState();
            if (nm != nullptr) {
                return static_cast<uint8_t*>(nm->code) + catch_off;
            }
            return nullptr;
        }

        ResetUnwindState();
        void* ex_obj = g_t4_exception_obj;
        ChaosT4RaiseException(ex_obj);
        return nullptr;
    }

    if (g_t4_unwind.pending_leave) {
        uint32_t leave_target = g_t4_unwind.leave_target_offset;
        ResetUnwindState();
        void* ret_addr = __builtin_return_address(0);
        const JitMethod* nm = self.FindCodeByAddress(ret_addr);
        if (nm != nullptr) {
            return static_cast<uint8_t*>(nm->code) + leave_target;
        }
        return nullptr;
    }

    return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// T4LeaveHelper (Linux implementation)
// ═══════════════════════════════════════════════════════════════════════════

extern "C" void* T4LeaveHelper(uint32_t target_instr_idx,
                                uint32_t current_instr_idx) noexcept {
    auto& self = GetLinuxSehHandler();

    void* ret_addr = __builtin_return_address(0);
    const JitMethod* nm = self.FindCodeByAddress(ret_addr);
    if (nm == nullptr || nm->instr_offsets == nullptr) {
        return nullptr;
    }

    uint32_t current_offset = (current_instr_idx < nm->instr_offset_count)
        ? nm->instr_offsets[current_instr_idx] : 0;

    uint32_t finally_idx = FindInnermostFinally(nm, current_offset);
    if (finally_idx == UINT32_MAX) {
        return nullptr;
    }

    uint32_t target_offset = (target_instr_idx < nm->instr_offset_count)
        ? nm->instr_offsets[target_instr_idx] : 0;

    ResetUnwindState();
    g_t4_unwind.pending_leave = true;
    g_t4_unwind.leave_target_offset = target_offset;

    return static_cast<uint8_t*>(nm->code) + GetClauseHandlerOffset(nm, finally_idx);
}

// ═══════════════════════════════════════════════════════════════════════════
// Initialize
// ═══════════════════════════════════════════════════════════════════════════

// GC event callback for deferred memory reclamation.
static void OnGcSafepoint(chaos::il2cpp::runtime_core::GcEvent /*event*/,
                           void* /*user_data*/) noexcept {
    GetLinuxSehHandler().ReclaimDemoted();
}

void LinuxSehHandler::Initialize() noexcept {
    // Register signal handlers for T4 exception dispatch.
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = T4SignalHandler;
    sa.sa_flags = SA_SIGINFO | SA_ONSTACK;

    // Hardware exceptions that may occur in T4 code.
    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGBUS,  &sa, nullptr);

    // Managed exceptions raised by T4 code (ChaosT4RaiseException).
    sigaction(kManagedExceptionSignal, &sa, nullptr);

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "T4 signal handlers registered (SIGSEGV, SIGBUS, SIGUSR1)");

    // Register GC event callback for deferred T4 code memory reclamation.
    chaos::il2cpp::runtime_core::GcRegisterEventCallback(OnGcSafepoint, nullptr);

    // Register T4 demotion callbacks.
    chaos::il2cpp::runtime_core::RegisterT4DemotionCallbacks(
        DemoteT4ByToken, DemoteT4ByCallSiteToken);
}

// ═══════════════════════════════════════════════════════════════════════════
// Platform-agnostic singleton accessor
// ═══════════════════════════════════════════════════════════════════════════

#if defined(__linux__)
ISehHandler& GetSehHandler() noexcept {
    return GetLinuxSehHandler();
}
#endif

}  // namespace chaos::il2cpp::jit
