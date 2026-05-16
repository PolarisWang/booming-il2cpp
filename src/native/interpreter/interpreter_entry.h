#ifndef CHAOS_IL2CPP_INTERPRETER_ENTRY_H_
#define CHAOS_IL2CPP_INTERPRETER_ENTRY_H_

// ── InterpreterEntryDirect: hotpatch patched-method dispatch entry point ──
//
// When a dispatch table entry has kHotpatchActive set, the pattern-aware
// call site routes through interrupt_ptr → InterpreterEntryDirect.
//
// Flow:
//   1. PatchMethod* is extracted from method_key
//   2. AotCoreIr JSON → IRMethod deserialization happens lazily (on first call, cached thereafter)
//   3. Method signature is parsed to determine argument types
//   4. Arguments are read from args_buf into InterpreterValue[]
//   5. ExecutionFrame is built and InterpreterVM::Execute is called
//   6. Return value is written to ret_buf
//
// Thread safety: the lazy IR deserialization uses double-checked locking (DCLP)
// via a std::atomic flag on PatchMethod.  Subsequent calls are lock-free.

#include <chaos/native_types.h>

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::runtime_core {

// Forward declarations
struct PatchMethod;

// ── ArgBuffer ───────────────────────────────────────────────────────────
// Lightweight utility for packing/unpacking native ABI arguments.
// Used by InterpreterEntryDirect to convert between the flat call buffer
// passed by generated code and the InterpreterValue[] array expected
// by InterpreterVM.
class ArgBuffer {
public:
    explicit ArgBuffer(void* buffer) noexcept;

    // Read methods (advance offset)
    CHAOS_IL2CPP_INT32   ReadI32() noexcept;
    CHAOS_IL2CPP_INT64   ReadI64() noexcept;
    CHAOS_IL2CPP_FLOAT32 ReadF32() noexcept;
    CHAOS_IL2CPP_FLOAT64 ReadF64() noexcept;
    void*                ReadPtr() noexcept;

    // Write methods (advance offset)
    void WriteI32(CHAOS_IL2CPP_INT32 value) noexcept;
    void WriteI64(CHAOS_IL2CPP_INT64 value) noexcept;
    void WriteF32(CHAOS_IL2CPP_FLOAT32 value) noexcept;
    void WriteF64(CHAOS_IL2CPP_FLOAT64 value) noexcept;
    void WritePtr(void* value) noexcept;

    // Reset offset to 0.
    void Reset() noexcept { offset_ = 0; }

    // Current read/write offset.
    size_t Offset() const noexcept { return offset_; }

private:
    void*  buffer_;
    size_t offset_ = 0;
};

// ── InterpreterEntryDirect ──────────────────────────────────────────────
// Dispatch entry point for patched methods.
//
// method_key: reinterpret_cast<uintptr_t>(PatchMethod*)
// args_buf:   flat argument buffer (generated code places all args here)
// ret_buf:    flat buffer for return value (or nullptr for void methods)
//
// This function is the value of every dispatch entry's interrupt_ptr.
// All patched methods share the same entry point; the PatchMethod*
// (extracted from method_key) identifies which method to execute.
void InterpreterEntryDirect(
    uintptr_t method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

// ── InterpreterEntryDirectFast ─────────────────────────────────────────────
// Fast-path entry point that internally allocates args/ret buffers without
// zero-init overhead. Used by --patch-bench mode where the caller doesn't
// have method arguments. The caller only passes method_key; internal buffers
// are uninitialized stack memory.
//
// Compared to InterpreterEntryDirect:
//   - No args_buf from caller (saves 32 bytes of zero-init per call)
//   - No ret_buf from caller (saves 16 bytes of zero-init per call)
//   - Internal buffers are NOT zero-initialized
//   - Return value is written to internal ret_buf, then discarded
//
// Thread safety: same as InterpreterEntryDirect — lock-free after first call.
void InterpreterEntryDirectFast(
    uintptr_t method_key) noexcept;

// ── Register direct function table ─────────────────────────────────────────
// Called by generated code to register kAotDirectFnTable/SubjectIds/Count.
// Avoids linker errors when chaos_runtime_core.lib doesn't have family-specific
// extern symbols.
extern "C" void ChaosRegisterDirectFnTable(
    const char* const* subjects,
    const void* const* table,
    int count) noexcept;

// ── PatchMethod lazy IR lowering (declared for use by InterpreterEntryDirect) ─
// Lower a PatchMethod's IL to IR if not already cached.
// Uses double-checked locking for thread safety.
void PatchMethodLowerIR(uintptr_t method_key) noexcept;

// ── Reapply inlining after all methods in a patch context are pre-lowered ─
// When ApplyPatchFromMemory pre-lowers all methods, callee IR may not be
// available yet when a caller is lowered (depends on iteration order).
// This function re-runs InlineLeafCallees on each lowered method so that
// all missed inlining opportunities are captured.
// Called from ApplyPatchFromMemory after the initial pre-lowering pass.
void ReapplyInlining(PatchMethod* methods, uint32_t method_count) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_INTERPRETER_ENTRY_H_
