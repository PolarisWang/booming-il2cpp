#ifndef CHAOS_IL2CPP_T4_SEH_HANDLER_H_
#define CHAOS_IL2CPP_T4_SEH_HANDLER_H_

// ── T4 SEH VEH Handler ──────────────────────────────────────────────────
//
// Registers a Vectored Exception Handler (VEH) at startup that catches
// hardware exceptions (AV, div-by-zero, etc.) occurring in T4-generated
// native code and dispatches them through the method's embedded SEH clause
// table.
//
// Flow:
//   1. T4 code is registered via RegisterT4Code() after successful
//      GenerateNativeCode() during T3→T4 promotion.
//   2. VEH handler fires on exception → finds NativeMethod via code range
//   3. Walks SEH clause table → finds matching try block → redirects RIP
//      to the handler code embedded in the T4-generated instruction stream
//   4. Returns EXCEPTION_CONTINUE_EXECUTION to resume at handler
//
// Limitations (V1):
//   - Catch variable (exception object) is NOT placed in the register file
//     — handlers that reference the caught exception will see stale data
//   - Filter clauses (flags=0x1) are treated as catch-all
//   - No finally/fault unwinding — those redirect directly to handler code

#include <cstdint>

namespace chaos::il2cpp::codegen {

struct NativeMethod;

/// Register a range of T4-generated code for VEH lookup.
/// Called after GenerateNativeCode() succeeds during T3→T4 promotion.
/// @param code_start  Entry point of the generated code (RX memory)
/// @param code_size   Size of generated code in bytes
/// @param nm          NativeMethod containing SEH clause table metadata
void RegisterT4Code(void* code_start, uint32_t code_size,
                    const NativeMethod* nm) noexcept;

/// Unregister a T4 code range.  Called during T4 demotion.
/// Marks the entry inactive so the VEH handler won't dispatch to it.
void UnregisterT4Code(void* code_start) noexcept;

/// Register the global VEH handler for T4 SEH dispatch.
/// Must be called once at startup (from RuntimeInit).
void RegisterT4SehHandler() noexcept;

/// Find the NativeMethod covering a given code address.
/// Returns nullptr if not found.  Exported for deoptimization trampoline.
const NativeMethod* FindT4CodeByAddress(const void* address) noexcept;

}  // namespace chaos::il2cpp::codegen

#endif  // CHAOS_IL2CPP_T4_SEH_HANDLER_H_
