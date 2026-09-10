#ifndef CHAOS_IL2CPP_JIT_SEH_HANDLER_H_
#define CHAOS_IL2CPP_JIT_SEH_HANDLER_H_

// ── T4 SEH VEH Handler ──────────────────────────────────────────────────
//
// Registers a Vectored Exception Handler (VEH) at startup that catches
// hardware exceptions (AV, div-by-zero, etc.) and managed throws
// (via ChaosJitRaiseException) occurring in T4-generated native code and
// dispatches them through the method's embedded SEH clause table.
//
// Flow (managed throw):
//   1. T4 code's Throw/Rethrow instruction calls ChaosJitRaiseException()
//      with the managed exception object in RCX
//   2. ChaosJitRaiseException stores the exception object + frame state in
//      TLS and calls RaiseException(kManagedSehExceptionCode)
//   3. VEH handler fires on 0xE0000001 → finds JitMethod via TLS-stored
//      return address (g_jit_throw_ret_addr)
//   4. Walks SEH clause table → finds matching try block → writes exception
//      object into all GPR register file slots → redirects RIP to handler
//   5. Returns EXCEPTION_CONTINUE_EXECUTION to resume at handler code
//
// Flow (hardware exception):
//   1. CPU fault (AV, div-by-zero, etc.) triggers VEH
//   2. VEH handler finds JitMethod via ExceptionAddress
//   3. Walks SEH clause table → finds matching try block → redirects RIP
//      to the handler code embedded in the T4-generated instruction stream
//   4. Returns EXCEPTION_CONTINUE_EXECUTION to resume at handler
//
// Limitations (V1):
//   - Catch variable (exception object) is placed in ALL GPR register file
//     slots (overwrite-all approach).  Handlers that reference multiple vregs
//     at entry may see the exception object in non-exception vregs, which is
//     safe because handler code loads fresh values from args/locals slots.
//   - No finally/fault unwinding — those redirect directly to handler code.
//     A rethrow inside finally will re-enter the VEH for the next clause.
//   - Filter clauses (flags=0x1) are treated as catch-all.
//   - Only Windows VEH is supported.

#include <cstdint>

namespace chaos::il2cpp::jit {

struct JitMethod;

/// Register a range of T4-generated code for VEH lookup.
/// Called after Compile() succeeds during T3→T4 promotion.
/// @param code_start  Entry point of the generated code (RX memory)
/// @param code_size   Size of generated code in bytes
/// @param nm          JitMethod containing SEH clause table metadata
/// @param patch_method_token  AOT metadata token of the owning PatchMethod (for hotpatch demotion)
void RegisterNativeCodeSection(void* code_start, uint32_t code_size,
                    const JitMethod* nm,
                    uint32_t patch_method_token = 0) noexcept;

/// Unregister a T4 code range.  Called during T4 demotion.
/// Marks the entry inactive so the VEH handler won't dispatch to it.
void UnregisterNativeCodeSection(void* code_start) noexcept;

/// Register the global VEH handler for T4 SEH dispatch.
/// Must be called once at startup (from RuntimeInit).
void RegisterJitSehHandler() noexcept;

/// Find the JitMethod covering a given code address.
/// Returns nullptr if not found.  Exported for deoptimization trampoline.
const JitMethod* FindNativeCodeByAddress(const void* address) noexcept;

/// Raise a managed exception from T4-generated code.
/// Stores the exception object in TLS, captures the T4 frame's stack pointer,
/// and triggers a VEH exception (kManagedSehExceptionCode).  The registered
/// VEH handler catches this exception, walks the SEH clause table embedded in
/// the T4 generated code, and redirects RIP to the matching catch/finally
/// handler.  If no handler is found, RaiseException returns and this function
/// returns normally — the caller should emit a safety net (INT3) after the call.
/// @param exception_obj  Pointer to the managed exception object to throw.
extern "C" void ChaosJitRaiseException(void* exception_obj) noexcept;

/// T4 throw address (set by ChaosJitRaiseException, read by VEH handler).
/// Stores the return address in T4 code where the throw originated.
extern thread_local void* g_jit_throw_ret_addr;

/// T4 frame RSP (set by ChaosJitRaiseException, read by VEH handler).
/// Stores RSP of the T4 frame at the throw point, used to access the
/// register file for exception object placement.
extern thread_local void* g_jit_frame_rsp;

// ── SEH V3: Finally/Fault Unwind State ─────────────────────────────────────

static constexpr uint32_t kMaxUnwindDepth = 64;

/// Thread-local unwind state for T4 finally/fault two-phase execution.
/// Set up by the VEH handler (Phase 1), consumed by JitEndFinallyHelper.
struct JitUnwindState {
    uint32_t unwind_list[kMaxUnwindDepth];  // finally/fault clause indices (innermost→outermost)
    uint32_t unwind_count  = 0;             // number of finally/fault clauses to unwind
    uint32_t unwind_index  = 0;             // current position in unwind_list
    bool     exception_in_flight = false;   // true = unwinding for exception (not Leave)
    bool     pending_leave = false;         // true = Leave triggered unwind (not exception)
    uint32_t leave_target_offset = 0;       // native byte offset of leave target (for pending_leave)
    bool     has_catch    = false;          // true = a catch handler was found (Phase 1)
    uint32_t catch_handler_offset = 0;      // native byte offset of catch handler
};

/// Thread-local SEH V3 unwind state.
extern thread_local JitUnwindState g_jit_unwind;

/// Called by EndFinally/EndFilter at runtime to advance the finally unwind chain.
/// Returns the native byte offset of the next handler to execute, or 0 if
/// unwinding is complete (normal execution should continue after the EndFinally).
extern "C" void* JitEndFinallyHelper() noexcept;

/// Called by Leave at runtime when a finally/fault clause covers the current
/// try block.  Resolves byte offsets and finds the innermost finally.
/// Returns the native byte offset of the first finally handler, or 0 if no
/// finally/fault covers the path (caller should fall through to normal JMP).
extern "C" void* JitLeaveHelper(uint32_t target_instr_idx,
                                uint32_t current_instr_idx) noexcept;

/// Demote all T4 code entries matching the given method_token.
/// Clears their JitMethod reference so the VEH handler stops dispatching
/// to them.  Returns the number of entries demoted.
/// Called by method_replacement::Register() when a hotpatch is applied.
uint32_t DemoteJittedMethod(uint32_t method_token) noexcept;

/// Demote all T4 code entries whose call_sites reference the given method_token.
/// This handles the case where a T4 method calls a now-hotpatched method.
/// Returns the number of entries demoted.
uint32_t DemoteJittedCallSite(uint32_t method_token) noexcept;

/// Demote all T4 code entries belonging to the given domain.
/// Called during domain unload to prevent VEH/signal dispatching
/// into JIT code that references freed domain metadata.
/// @param domain_id  The domain whose JIT entries should be demoted.
/// @return Number of entries demoted.
uint32_t DemoteJittedCodeByDomain(uint32_t domain_id) noexcept;

/// Free all demoted T4 code regions whose VirtualFree was deferred to the
/// next GC safepoint.  Called from the GC event callback registered in
/// RegisterJitSehHandler.  Exposed for testing and explicit reclamation.
void ReclaimDemotedCode() noexcept;

}  // namespace chaos::il2cpp::jit

#endif  // CHAOS_IL2CPP_JIT_SEH_HANDLER_H_
