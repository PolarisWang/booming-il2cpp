#include "t4_seh_handler.h"
#include "native_method.h"

#include <chaos/log.h>

// Register demotion callbacks with runtime_core so method_replacement
// can demote T4 entries without creating a circular dependency.
#include <t4_demotion.h>

#if defined(_WIN32) || defined(_WIN64)
  #define NOMINMAX
  #include <windows.h>
  #include <intrin.h>   // _ReturnAddress(), _AddressOfReturnAddress()
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

        // Find matching SEH handler
        uint32_t handler_offset = FindSehHandlerForOffset(nm, code_offset);
        if (handler_offset == 0xFFFFFFFFu) {
            // No matching handler in this method — let exception propagate.
            // Clear throw state for next potential catch in caller frame.
            g_t4_throw_ret_addr = nullptr;
            g_t4_frame_rsp = nullptr;
            return EXCEPTION_CONTINUE_SEARCH;
        }

        // Write the exception object into ALL GPR register file slots in the
        // T4 frame so the catch handler code finds it regardless of which vreg
        // the register allocator assigned.  This is safe because:
        //   1. The only live value at handler entry is the exception object
        //   2. Handler code loads other values from args/locals stack slots
        if (g_t4_frame_rsp != nullptr) {
            uint64_t* regfile = reinterpret_cast<uint64_t*>(
                reinterpret_cast<uint8_t*>(g_t4_frame_rsp) + kT4GprFileOff);
            uint64_t ex_val = reinterpret_cast<uint64_t>(g_t4_exception_obj);
            for (uint32_t i = 0; i < kT4GprCount; i++) {
                regfile[i] = ex_val;
            }
        }

        // Place exception object pointer in RCX (Win64 first-arg register)
        // so catch handlers that need to access the exception can read it.
        ep->ContextRecord->Rcx = reinterpret_cast<ULONG_PTR>(g_t4_exception_obj);

        // Restore RSP to the T4 frame's RSP (before ChaotT4RaiseException was
        // called).  The CONTEXT record's RSP points into RaiseException's
        // internal stack; the handler code expects the T4 frame's RSP so it
        // can access the register file, args_buf, and ret_buf correctly.
        ep->ContextRecord->Rsp = reinterpret_cast<ULONG_PTR>(g_t4_frame_rsp);

        // Redirect RIP to the handler code in the T4 generated method
        void* handler_addr = static_cast<uint8_t*>(nm->code) + handler_offset;
        ep->ContextRecord->Rip = reinterpret_cast<ULONG_PTR>(handler_addr);

        CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
            "T4 VEH (managed throw): ret_addr={} (offset={}) -> handler at {} (offset={}), ex_obj={}",
            g_t4_throw_ret_addr, code_offset, handler_addr, handler_offset,
            reinterpret_cast<void*>(g_t4_exception_obj));

        // Clear throw state for next throw
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
        "T4 VEH (hardware exception): exception at {} (offset={}) -> handler at {} (offset={}), ex_obj={}",
        exception_addr, code_offset, handler_addr, handler_offset,
        reinterpret_cast<void*>(ex_obj));

    return EXCEPTION_CONTINUE_EXECUTION;
}

// ── Win64 Personality Routine (V2) ──────────────────────────────────────
// Called by the OS unwinder during exception dispatch.  This is the second
// line of defense after the VEH handler:
//
//   VEH (first chance): catches managed exceptions, dispatches within the
//     current method via FindSehHandlerForOffset.  If no handler found in
//     the current method, returns CONTINUE_SEARCH.
//
//   Personality routine (second chance): when VEH returns CONTINUE_SEARCH,
//     the OS unwinder walks the call stack and calls the personality routine
//     for each frame.  If a T4 frame has a matching handler, the personality
//     routine redirects RIP and returns ExceptionCollidedUnwind.
//
// This function is invoked via a JMP thunk embedded in the code buffer
// after the UNWIND_INFO (see EmitUnwindInfo in unwind_info.cpp).

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
    // For hardware exceptions or native C++ exceptions, let the OS
    // continue searching — the VEH handler handles hardware exceptions
    // within the same method, and native C++ exceptions go through the
    // standard C++ runtime.
    if (ExceptionRecord->ExceptionCode != kManagedSehExceptionCode) {
        return ExceptionContinueSearch;
    }

    // Find NativeMethod covering the ControlPc (the return address that
    // triggered the unwind).  This is the instruction after the
    // `call ChaosT4RaiseException` in the T4 code.
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

    // Walk SEH clause table to find matching handler
    const uint8_t* table = code_base + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    const uint8_t* clauses = table + sizeof(uint32_t);

    for (uint32_t i = 0; i < count; i++) {
        const uint8_t* entry = clauses + i * (5 * sizeof(uint32_t));
        uint32_t clause_flags, try_start, try_end;
        std::memcpy(&clause_flags, entry + 0, sizeof(clause_flags));
        std::memcpy(&try_start,    entry + 4, sizeof(try_start));
        std::memcpy(&try_end,      entry + 8, sizeof(try_end));

        if (code_offset >= try_start && code_offset < try_end) {
            uint32_t handler_st;
            std::memcpy(&handler_st, entry + 12, sizeof(handler_st));

            // Redirect RIP to the handler code
            void* handler_addr = static_cast<uint8_t*>(nm->code) + handler_st;
            ContextRecord->Rip = reinterpret_cast<ULONG_PTR>(handler_addr);

            // RSP stays at the establisher frame (the T4 frame's RSP after
            // prologue).  The handler code expects the T4 frame layout.
            // EstablisherFrame is the T4 frame's RSP, set by RtlVirtualUnwind.

            // Place the exception object pointer in RCX (Win64 first arg).
            // DispatcherContext doesn't give us direct access to TLS, but we
            // can read g_t4_exception_obj which was set by ChaosT4RaiseException.
            ContextRecord->Rcx = reinterpret_cast<ULONG_PTR>(g_t4_exception_obj);

            CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
                "T4 personality: clause {} matches offset={} (ControlPc={}) "
                "-> handler at {} (offset={}), ex_obj={}",
                i, code_offset,
                reinterpret_cast<const void*>(DispatcherContext->ControlPc),
                handler_addr, handler_st,
                reinterpret_cast<void*>(g_t4_exception_obj));

            return ExceptionCollidedUnwind;
        }
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("codegen",
        "T4 personality: no handler for offset={} (ControlPc={})",
        code_offset,
        reinterpret_cast<const void*>(DispatcherContext->ControlPc));

    return ExceptionContinueSearch;
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
