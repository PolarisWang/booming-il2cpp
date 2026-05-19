#include "t4_seh_handler.h"
#include "native_method.h"

#include <chaos/log.h>

// Register demotion callbacks with runtime_core so method_replacement
// can demote T4 entries without creating a circular dependency.
#include <t4_demotion.h>

#if defined(_WIN32) || defined(_WIN64)
  #define NOMINMAX
  #include <windows.h>
#endif

namespace chaos::il2cpp::codegen {

// ── T4 Code Registry ─────────────────────────────────────────────────────
// Maps code address ranges back to NativeMethod for VEH lookup.
// Fixed-size array (no heap allocation in exception context).
// Thread-safe: entries are append-only, never removed.

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

static void LockT4Registry() noexcept {
    while (InterlockedExchange(&g_t4_code_lock, 1) != 0) {
        // spin
    }
}

static void UnlockT4Registry() noexcept {
    InterlockedExchange(&g_t4_code_lock, 0);
}

void RegisterT4Code(void* code_start, uint32_t code_size,
                    const NativeMethod* nm,
                    uint32_t patch_method_token) noexcept {
    if (code_start == nullptr || code_size == 0 || nm == nullptr) return;

    LockT4Registry();
    if (g_t4_code_count >= kMaxT4CodeEntries) {
        CHAOS_IL2CPP_LOG_WARN_M("codegen", "RegisterT4Code: registry full ({} entries)", kMaxT4CodeEntries);
        UnlockT4Registry();
        return;
    }
    g_t4_code_entries[g_t4_code_count].code_start = code_start;
    g_t4_code_entries[g_t4_code_count].code_size  = code_size;
    g_t4_code_entries[g_t4_code_count].nm         = nm;
    g_t4_code_entries[g_t4_code_count].patch_method_token = patch_method_token;
    g_t4_code_count++;
    UnlockT4Registry();

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "RegisterT4Code: code={} size={} seh_offset={} token={}",
        code_start, code_size, nm->seh_table_offset, patch_method_token);
}

void UnregisterT4Code(void* code_start) noexcept {
    if (code_start == nullptr) return;
    LockT4Registry();
    for (uint32_t i = 0; i < g_t4_code_count; i++) {
        if (g_t4_code_entries[i].code_start == code_start) {
            g_t4_code_entries[i].nm = nullptr;
            break;
        }
    }
    UnlockT4Registry();
}

uint32_t DemoteT4ByToken(uint32_t method_token) noexcept {
    if (method_token == 0) return 0;
    uint32_t count = 0;
    LockT4Registry();
    for (uint32_t i = 0; i < g_t4_code_count; i++) {
        if (g_t4_code_entries[i].patch_method_token == method_token) {
            g_t4_code_entries[i].nm = nullptr;
            count++;
        }
    }
    UnlockT4Registry();
    if (count > 0) {
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "DemoteT4ByToken: token={} demoted {} entries", method_token, count);
    }
    return count;
}

uint32_t DemoteT4ByCallSiteToken(uint32_t method_token) noexcept {
    if (method_token == 0) return 0;
    uint32_t count = 0;
    LockT4Registry();
    for (uint32_t i = 0; i < g_t4_code_count; i++) {
        const auto* nm = g_t4_code_entries[i].nm;
        if (nm == nullptr) continue;
        // Scan call sites for matching method_token
        for (uint32_t j = 0; j < nm->call_site_count; j++) {
            if (nm->call_sites[j].method_token == method_token) {
                g_t4_code_entries[i].nm = nullptr;
                count++;
                break;
            }
        }
    }
    UnlockT4Registry();
    if (count > 0) {
        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "DemoteT4ByCallSiteToken: token={} demoted {} caller entries", method_token, count);
    }
    return count;
}

/// Find the NativeMethod covering a given code address.
/// Returns nullptr if not found.
const NativeMethod* FindT4CodeByAddress(const void* address) noexcept {
    for (uint32_t i = 0; i < g_t4_code_count; i++) {
        const auto& entry = g_t4_code_entries[i];
        const uint8_t* start = static_cast<const uint8_t*>(entry.code_start);
        if (start == nullptr) continue;
        const uint8_t* end = start + entry.code_size;
        const uint8_t* addr = static_cast<const uint8_t*>(address);
        if (addr >= start && addr < end) {
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

            // Skip fault handlers (flags=0x4) for V1
            uint32_t flags_fault = 0x4;
            if ((clause_flags & flags_fault) != 0) continue;

            CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                "T4 VEH: clause {} matches offset={} flags=0x{:x} try=[{},{}) handler={}",
                i, code_offset, clause_flags, try_start, try_end, handler_st);
            return handler_st;
        }
    }

    return 0xFFFFFFFFu;
}


// ── VEH Handler ─────────────────────────────────────────────────────────

#if defined(_WIN32) || defined(_WIN64)

/// Managed exception code used by CodegenThrow/CodegenRethrow.
static constexpr uint32_t kManagedSehExceptionCode = 0xE0000001;

/// Exception object pointer for the current thread (set by CodegenThrow,
/// read by the VEH handler to restore the exception object for catch blocks).
thread_local void* g_t4_exception_obj = nullptr;

static LONG WINAPI T4VectoredExceptionHandler(EXCEPTION_POINTERS* ep) noexcept {
    if (ep == nullptr || ep->ExceptionRecord == nullptr || ep->ContextRecord == nullptr) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // Get the exception address
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

    // Find matching SEH handler
    uint32_t handler_offset = FindSehHandlerForOffset(nm, code_offset);
    if (handler_offset == 0xFFFFFFFFu) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    // Store exception object pointer in RCX so the catch handler can
    // access it (Win64 first-arg register convention).
    ULONG_PTR ex_obj = reinterpret_cast<ULONG_PTR>(g_t4_exception_obj);
    ep->ContextRecord->Rcx = ex_obj;

    // Redirect RIP to the handler code in the T4 generated method
    void* handler_addr = static_cast<uint8_t*>(nm->code) + handler_offset;
    ep->ContextRecord->Rip = reinterpret_cast<ULONG_PTR>(handler_addr);

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "T4 VEH: exception at {} (offset={}) -> handler at {} (offset={}), ex_obj={}",
        exception_addr, code_offset, handler_addr, handler_offset,
        reinterpret_cast<void*>(ex_obj));

    return EXCEPTION_CONTINUE_EXECUTION;
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

    // Register T4 demotion callbacks so method_replacement can demote T4
    // entries through runtime_core without a circular build dependency.
    chaos::il2cpp::runtime_core::RegisterT4DemotionCallbacks(
        DemoteT4ByToken, DemoteT4ByCallSiteToken);
}

#else  // not Windows

void RegisterT4SehHandler() noexcept {
    // VEH is Windows-specific. On POSIX, signal handlers would be used instead.
    CHAOS_IL2CPP_LOG_DEBUG_M("codegen", "T4 SEH handler: not implemented for this platform");

    // Register T4 demotion callbacks (platform-independent).
    chaos::il2cpp::runtime_core::RegisterT4DemotionCallbacks(
        DemoteT4ByToken, DemoteT4ByCallSiteToken);
}

#endif  // _WIN32 || _WIN64

}  // namespace chaos::il2cpp::codegen
