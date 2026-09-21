// register_ir.h — Register-based IR instruction format (C-compatible)
//
// Defines the packed 16-byte RegisterInstruction and associated types for
// register-based interpreter execution. This is the ABI contract between
// codegen (RegisterIREmitter) and the runtime (register VM).
//
// All integers are little-endian. Pointers in this header refer to
// offsets within the .patchdata reg_ir section.
//
// Reg_ir section format (in .patchdata, see patch_data.h):
//   [offsets: uint32_t[count]] [method blocks]
//   Each block: max_regs | instr_count | seh_count | RegisterInstruction[instr_count] | SEHClauseCompact[seh_count]

#ifndef CHAOS_REGISTER_IR_H_
#define CHAOS_REGISTER_IR_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ── Register counts ─────────────────────────────────────────────────────
#define CHAOS_REG_IR_GP_REGISTERS    64u  // general-purpose registers
#define CHAOS_REG_IR_FP_REGISTERS    32u  // float/double registers

// ── Register flags (1 byte) ─────────────────────────────────────────────
#define CHAOS_REG_IR_HAS_DST   0x01u
#define CHAOS_REG_IR_HAS_SRC1  0x02u
#define CHAOS_REG_IR_HAS_SRC2  0x04u
#define CHAOS_REG_IR_HAS_SRC3  0x80u  // third source (StElem, StObj, Cpblk, InitBlk)
#define CHAOS_REG_IR_HAS_IMM   0x08u
#define CHAOS_REG_IR_IS_CALL   0x10u  // call-like: reads args from consecutive regs
#define CHAOS_REG_IR_IS_BRANCH 0x20u  // branch: uses branch_target
#define CHAOS_REG_IR_IS_STORE  0x40u  // store-like: writes to memory, not reg

// ── Register instruction (16 bytes, 8-byte aligned) ─────────────────────
//
// Packed header (8 bytes):
//   bits [15:0]   = op_code (IROpCode value, 0-99)
//   bits [23:16]  = dst_reg
//   bits [31:24]  = src1_reg
//   bits [39:32]  = src2_reg
//   bits [47:40]  = flags (RegFlags)
//   bits [62:48]  = call_arg_count (0-32767) or src3_reg (when HAS_SRC3)
//   bit  63       = is_instance_call (call instructions only)
//
// Immediate payload (8 bytes):
//   Varies by opcode: i4, i8, r8, pointer, branch_target, etc.
typedef struct RegisterInstruction {
    uint64_t header;     // packed fields (8 bytes)
    uint64_t imm_payload; // immediate value (8 bytes)
} RegisterInstruction;

// ── Accessor macros (C-compatible) ──────────────────────────────────────
// These work with pointer-to-RegisterInstruction.
// Usage: REG_OPCODE(instr)  or  REG_DST(instr)

#define REG_OPCODE(ptr)       ((int32_t)((ptr)->header & 0xFFFF))
#define REG_DST(ptr)          ((uint8_t)(((ptr)->header >> 16) & 0xFF))
#define REG_SRC1(ptr)         ((uint8_t)(((ptr)->header >> 24) & 0xFF))
#define REG_SRC2(ptr)         ((uint8_t)(((ptr)->header >> 32) & 0xFF))
#define REG_FLAGS(ptr)        ((uint8_t)(((ptr)->header >> 40) & 0xFF))
#define REG_CALL_ARG_COUNT(ptr) ((uint32_t)(((ptr)->header >> 48) & 0x7FFF))
#define REG_IS_INSTANCE_CALL(ptr) (((ptr)->header >> 63) != 0)
#define REG_SRC3(ptr)         ((uint8_t)(((ptr)->header >> 48) & 0xFF))

#define REG_HAS_DST(ptr)      ((REG_FLAGS(ptr) & CHAOS_REG_IR_HAS_DST) != 0)
#define REG_HAS_SRC1(ptr)     ((REG_FLAGS(ptr) & CHAOS_REG_IR_HAS_SRC1) != 0)
#define REG_HAS_SRC2(ptr)     ((REG_FLAGS(ptr) & CHAOS_REG_IR_HAS_SRC2) != 0)
#define REG_HAS_IMM(ptr)      ((REG_FLAGS(ptr) & CHAOS_REG_IR_HAS_IMM) != 0)
#define REG_IS_CALL(ptr)      ((REG_FLAGS(ptr) & CHAOS_REG_IR_IS_CALL) != 0)
#define REG_IS_BRANCH(ptr)    ((REG_FLAGS(ptr) & CHAOS_REG_IR_IS_BRANCH) != 0)

// ── SEH clause (compact, 24 bytes) ──────────────────────────────────────
// 6 × uint32_t: flags | try_start | try_end | handler_start | handler_end | class_token
typedef struct RegisterSehClause {
    uint32_t flags;
    uint32_t try_start_idx;
    uint32_t try_end_idx;
    uint32_t handler_start_idx;
    uint32_t handler_end_idx;
    uint32_t class_token;
} RegisterSehClause;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CHAOS_REGISTER_IR_H_
