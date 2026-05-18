// codegen_helpers.h — Runtime helper functions for T4 generated native code
//
// These helpers are called by T4-generated x64 native code to perform
// operations that are too complex to inline (vector access, GC allocation, etc.).
// They are extern "C" for stable ABI from generated code.
#ifndef CHAOS_IL2CPP_CODEGEN_HELPERS_H_
#define CHAOS_IL2CPP_CODEGEN_HELPERS_H_

#include <cstdint>

// ── Deoptimization TLS state ─────────────────────────────────────────────
// Thread-local buffer used by the deopt trampoline to signal deoptimization
// back to InterpreterEntryDirect.

namespace chaos::il2cpp::codegen {

/// Magic value written to ret_buf[0] by the deopt trampoline to signal that
/// deoptimization occurred.  InterpreterEntryDirect checks for this value
/// after the T4 native call returns.
static constexpr uint64_t kDeoptMagic = 0xDE0D7FA57A11ULL;

/// Thread-local deoptimization state: written by DeoptTrapEntry (called from
/// the deopt trampoline in generated native code), read by
/// InterpreterEntryDirect to reconstruct RegisterFrame state.
struct DeoptTlsState {
    uint64_t gpr_file[64] = {};   // Reconstructed GPR register file
    double   fpr_file[32] = {};   // Reconstructed FPR register file
    uint8_t  gpr_tags[64] = {};   // Per-register ValueTag for GPRs
    uint8_t  fpr_tags[32] = {};   // Per-register ValueTag for FPRs
    uint32_t instr_pc     = 0;    // RegisterInstruction pc to resume at
    bool     deopt_happened = false; // Set to true when deopt trampoline fires
};

extern thread_local DeoptTlsState t_deopt_state;

}  // namespace chaos::il2cpp::codegen

// ── LdFld / StFld helpers ──────────────────────────────────────────────
// InterpreterObject::fields is a std::vector<InterpreterValue>, so field
// access requires a function call (vector bounds check, tag dispatch).
//
// LdFldHelper: reads fields[idx] and returns the raw uint64 bits.
// StFldHelper: stores value into fields[idx] with tag = Int64 (bit-preserving).

extern "C" uint64_t CodegenLdFld(void* obj, uint32_t field_idx) noexcept;
extern "C" void     CodegenStFld(void* obj, uint32_t field_idx, uint64_t value) noexcept;

// ── CallVirt helper (PIC dispatch + fallback) ──────────────────────────
// Called by T4-generated native code for CallVirt instructions.
// Walks PIC chains for a fast direct call; falls back to InterpreterDispatchRaw.
struct CodegenCallVirtArgs {
    uint64_t* gpr_base;         // 0: pointer to register file [RSP + kGprFileOff]
    const void* pic_data;       // 8: pm->pic_dispatch_data (or null)
    void* dispatch_ctx;         // 16: dispatch context for InterpreterDispatchRaw
    uint32_t instruction_idx;   // 24: current instruction index (for PIC chain lookup)
    uint32_t arg_count;         // 28: number of arguments
    uint32_t first_arg_reg;     // 32: src1_reg (first argument register)
    void* call_target;          // 40: instr.imm.ptr (method handle for fallback)
    uint32_t has_dst;           // 48: whether instruction has a dst register
    uint32_t is_instance_call;  // 52: from header bit 63
    void* ret_buf;              // 56: ret_buf pointer (written with kDeoptMagic on PIC miss)
};
// Total: 64 bytes (aligns to 8)

// Returns the uint64_t result value (0 if no return).
// Internal: reads args from gpr_base, calls direct_fn on PIC hit,
// or InterpreterDispatchRaw on miss.
extern "C" uint64_t CodegenCallVirt(const CodegenCallVirtArgs* args) noexcept;

// ── Box / NewObj helpers ─────────────────────────────────────────────────
// Box: wrap a raw value (uint64_t + tag) into an InterpreterObject.
// Returns pointer to the allocated object.
extern "C" void*    CodegenBox(uint64_t value, uint8_t tag, uint32_t type_token) noexcept;

// NewObj: allocate an InterpreterObject with initial field count.
// Returns pointer to the allocated object.
extern "C" void*    CodegenNewObj(uint32_t type_token, uint32_t field_count) noexcept;

// LdLen: get length of an interpreter array.
extern "C" int32_t  CodegenLdLen(void* arr) noexcept;

// ── Static field helpers ───────────────────────────────────────────────────
extern "C" uint64_t CodegenLdSFld(uint32_t field_offset) noexcept;
extern "C" void     CodegenStSFld(uint32_t field_offset, uint64_t value) noexcept;

// ── Array helpers ──────────────────────────────────────────────────────────
extern "C" void*    CodegenNewArr(int32_t length) noexcept;
extern "C" uint64_t CodegenLdElem(void* arr, int32_t index) noexcept;
extern "C" void     CodegenStElem(void* arr, int32_t index, uint64_t value) noexcept;

// ── Type check helpers ─────────────────────────────────────────────────────
// These follow the same convention as FastExecute (no-op cast, no type check):
// CastClass returns obj unchanged, IsInst returns obj unchanged.
// Full type checking requires throwing InvalidCastException which is handled
// in the InterpreterVM path.
extern "C" void*    CodegenCastClass(void* obj, uint32_t target_token) noexcept;
extern "C" void*    CodegenIsInst(void* obj, uint32_t target_token) noexcept;

// ── Unbox helper ───────────────────────────────────────────────────────────
// Extracts fields[0] from a boxed InterpreterObject.
extern "C" uint64_t CodegenUnbox(void* obj) noexcept;

// ── LdVirtFtn helper ────────────────────────────────────────────────────────
// Resolves a virtual function pointer for an object and method token.
// Calls vtable_registry::ResolveVirtualMethodPointer(type_token, method_token).
// Returns resolved function pointer, or null if resolution fails.
extern "C" void* CodegenLdVirtFtn(void* obj, uint32_t method_token) noexcept;

// ── InitObj helper ──────────────────────────────────────────────────────────
// Zero-initializes an InterpreterValue at the given pointer.
extern "C" void CodegenInitObj(void* ptr) noexcept;

// ── StObj helper ──────────────────────────────────────────────────────────────
// Writes value as an Int64-tagged InterpreterValue at the given managed pointer.
// ptr: pointer to InterpreterValue to write.  value: raw uint64 to store.
extern "C" void CodegenStObj(void* ptr, uint64_t value) noexcept;

// ── Cpblk helper ──────────────────────────────────────────────────────────────
// Copies count bytes from src to dst (std::memcpy wrapper for T4 codegen).
extern "C" void CodegenCpblk(void* dst, const void* src, uint32_t count) noexcept;

// ── InitBlk helper ────────────────────────────────────────────────────────────
// Fills count bytes at dst with value (std::memset wrapper for T4 codegen).
extern "C" void CodegenInitBlk(void* dst, uint32_t value, uint32_t count) noexcept;

// ── LdObj helper ─────────────────────────────────────────────────────────────
// Reads an InterpreterValue from a managed pointer and returns the raw uint64 bits.
// The ptr must point to a valid InterpreterValue (tagged union).
extern "C" uint64_t CodegenLdObj(void* ptr) noexcept;

// ── LocAlloc helper ──────────────────────────────────────────────────────────
// Allocates a zero-initialized block of memory on the heap (CHAOS_IL2CPP_MALLOC).
// size: number of bytes to allocate.
// Returns pointer to the allocated memory, or nullptr on failure.
extern "C" void* CodegenLocAlloc(uint32_t size) noexcept;

// ── Deoptimization trampoline entry point ─────────────────────────────────────
// Called from the deopt trampoline in generated native code.  The trampoline
// saves all caller-saved registers (forming a NativeContext on the stack) and
// passes the NativeContext pointer + codegen RSP.  This function reconstructs
// the register file from the stack frame spill slots, sets t_deopt_state, and
// the trampoline then writes kDeoptMagic to ret_buf[0] and returns.
extern "C" void DeoptTrapEntry(const void* ctx, uint64_t codegen_rsp) noexcept;

// ── Inline deoptimization state saver ─────────────────────────────────────────
// Called directly from EmitDeoptSequence in generated T4 native code.
// codegen_rsp is the RSP value after prologue (sub rsp, kFrameSize).
// Uses _ReturnAddress() to find the NativeMethod and DeoptEntry, then
// batch-copies all 64 GPRs + 32 FPRs from stack frame spill slots to
// t_deopt_state along with their type tags.
extern "C" void DeoptSaveFrameState(uint64_t codegen_rsp) noexcept;

#endif  // CHAOS_IL2CPP_CODEGEN_HELPERS_H_
