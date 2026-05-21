#ifndef CHAOS_IL2CPP_CODEGEN_UNWIND_INFO_H_
#define CHAOS_IL2CPP_CODEGEN_UNWIND_INFO_H_

// ── Win64 .pdata/.xdata unwind info helpers ──────────────────────────────────
//
// Emits UNWIND_INFO and RUNTIME_FUNCTION for T4-generated native code so the
// OS can unwind through T4 frames (debugger call stacks, CaptureStackBackTrace,
// SetUnhandledExceptionFilter).
//
// V1: Stack walking only (no UNW_FLAG_EHANDLER).  Exception dispatch still
// goes through the existing VEH handler (t4_seh_handler.cpp).  Personality
// routine integration is deferred to V2.
//
// Reference: https://learn.microsoft.com/en-us/cpp/build/exception-handling-x64

#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::codegen {

class CodeBuffer;

#if defined(_WIN64)

/// Binary layout of a Win64 UNWIND_CODE (2 bytes).
struct UnwindCode {
    uint8_t code_offset;   // Offset from function start
    uint8_t op_info;       // UnwindOp(4) | OpInfo(4)
};

/// Win64 UNWIND_OP_CODES for x64.
enum UnwindOp : uint8_t {
    UWOP_PUSH_NONVOL = 0,  // Push a nonvolatile register (OpInfo = register number)
    UWOP_ALLOC_LARGE = 1,  // Allocate large stack (OpInfo=0: 8-byte scaled, OpInfo=1: fixed)
    UWOP_ALLOC_SMALL = 2,  // Allocate small stack (size = OpInfo*8 + 8)
    UWOP_SET_FPREG    = 3,  // Establish frame pointer (OpInfo = 0)
    UWOP_SAVE_NONVOL  = 4,  // Save nonvolatile register (not used for push-based prologue)
    UWOP_SAVE_XMM128  = 8,  // Save XMM register (not used for push-based prologue)
};

/// Binary layout of a Win64 UNWIND_INFO header (variable length).
/// Emitted into the code buffer as raw bytes.
#pragma pack(push, 1)
struct UnwindInfoHeader {
    uint8_t version_flags;      // Version(3) | Flags(5)
    uint8_t size_of_prolog;     // Prologue length / 16 (rounded up)
    uint8_t count_of_codes;     // Number of UNWIND_CODE entries
    uint8_t frame_reg_offset;   // FrameRegister(4) | FrameOffset(4)
    // Followed by UNWIND_CODE[count_of_codes] (padded to 4-byte boundary)
    // Followed by optional: ExceptionHandler RVA (if UNW_FLAG_EHANDLER)
    // Followed by optional: handler-specific data
};
#pragma pack(pop)

/// Binary layout of a Win64 RUNTIME_FUNCTION (.pdata entry, 12 bytes).
#pragma pack(push, 1)
struct RuntimeFunction {
    uint32_t begin_address;      // Relative to BaseAddress of RtlAddFunctionTable
    uint32_t end_address;        // Relative to BaseAddress
    uint32_t unwind_info_address; // Relative to BaseAddress
};
#pragma pack(pop)

/// Emit a complete UNWIND_INFO structure into the code buffer.
///
/// @param buf              Code buffer to emit into.
/// @param prologue_size    Total prologue size in bytes.
/// @param frame_sub_size   Value passed to sub rsp, K (K value, for ALLOC_SMALL/LARGE encoding).
/// @param num_push_regs    Number of PUSH NONVOL register operations (includes rbp, rbx, rsi, cached).
/// @param push_reg_nums    x64 register numbers for each push, in prologue order (first push = rbp first).
/// @param push_reg_offsets Byte offset of each push/sub instruction (for CodeOffset in UNWIND_CODE).
/// @param sub_rsp_offset   Byte offset of the sub rsp instruction.
/// @param set_fpreg_offset Byte offset of the mov rbp, rsp instruction.
/// @param has_seh          If true, set UNW_FLAG_EHANDLER and emit a JMP thunk
///                         to T4PersonalityRoutine after the unwind codes (V2).
///
/// @return The byte offset from the start of the code buffer where UNWIND_INFO begins.
///         The caller stores this in NativeMethod for RUNTIME_FUNCTION.UnwindInfoAddress.
uint32_t EmitUnwindInfo(
    CodeBuffer& buf,
    uint32_t prologue_size,
    uint32_t frame_sub_size,
    uint32_t num_push_regs,
    const uint8_t* push_reg_nums,
    const uint32_t* push_reg_offsets,
    uint32_t sub_rsp_offset,
    uint32_t set_fpreg_offset,
    bool has_seh = false) noexcept;


/// Allocate and populate a RUNTIME_FUNCTION structure on the heap.
///
/// @param unwind_info_offset  Offset from code start to UNWIND_INFO (returned by EmitUnwindInfo).
/// @param code_size           Total code size in bytes (excluding SEH table / metadata).
///
/// @return Heap-allocated RuntimeFunction, or nullptr on allocation failure.
///         Caller must free with CHAOS_IL2CPP_FREE.
RuntimeFunction* AllocRuntimeFunction(uint32_t unwind_info_offset,
                                      uint32_t code_size) noexcept;

/// Personality routine for managed exception dispatch across T4 frames (V2).
/// Declared extern "C" for stable ABI. Defined in t4_seh_handler.cpp.
extern "C" void T4PersonalityRoutine();

/// Size of the JMP thunk emitted after UNWIND_INFO when has_seh=true.
/// Thunk: mov rax, imm64 (10 bytes) + jmp rax (2 bytes) = 12 bytes.
/// Used by code_generator.cpp to extend RUNTIME_FUNCTION.end_address.
static constexpr uint32_t kPersonalityThunkSize = 12;

#endif  // _WIN64

}  // namespace chaos::il2cpp::codegen

#endif  // CHAOS_IL2CPP_CODEGEN_UNWIND_INFO_H_
