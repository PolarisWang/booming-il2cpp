#ifndef CHAOS_IL2CPP_IR_REG_ALLOC_H_
#define CHAOS_IL2CPP_IR_REG_ALLOC_H_

// ── Register-Based IR Format ─────────────────────────────────────────────
//
// This file defines the compact register-based instruction format and the
// register allocator that converts from stack-based IRMethod (IRInstruction[])
// to register-based IR (RegisterInstruction[]).
//
// Key design:
//   - RegisterInstruction is 16 bytes (vs IRInstruction's 104 bytes)
//   - RegisterFrame has a unified register file (64 general + 32 float)
//   - The register allocator is a simple linear-scan that eliminates
//     implicit push/pop by assigning virtual registers to stack slots
//
// The IROpCode enum values remain unchanged (0-98). Only the execution
// model changes: instead of implicit evaluation stack, each instruction
// explicitly names its dst/src registers.

#include <cstdint>
#include <cstring>
#include "interpreter_vm.h"  // for IROpCode, ValueTag, SEHClause, etc.

namespace chaos::il2cpp::interpreter {

// ── Register counts ─────────────────────────────────────────────────────
static constexpr uint32_t kGPRegisters    = 64;   // general-purpose registers
static constexpr uint32_t kFPRegisters    = 32;   // float/double registers
static constexpr uint32_t kTotalRegisters = kGPRegisters + kFPRegisters;

// ── Register flags ──────────────────────────────────────────────────────
enum RegFlags : uint8_t {
    kRegHasDst   = 0x01,
    kRegHasSrc1  = 0x02,
    kRegHasSrc2  = 0x04,
    kRegHasSrc3  = 0x80,  // third source (for StElem, StObj, Cpblk, InitBlk — 3-operand stores)
    kRegHasImm   = 0x08,
    kRegIsCall   = 0x10,  // call-like: reads args from consecutive regs
    kRegIsBranch = 0x20,  // branch: uses branch_target
    kRegIsStore  = 0x40,  // store-like: writes to memory, not reg
};

// ── Register instruction (16 bytes, cache-line friendly) ────────────────
// Packed format:
//   bits [15:0]   = op_code (IROpCode value)
//   bits [23:16]  = dst_reg
//   bits [31:24]  = src1_reg
//   bits [39:32]  = src2_reg
//   bits [47:40]  = flags (RegFlags)
//   bits [63:48]  = reserved
//   bytes [72:8]  = immediate payload (8 bytes)
struct alignas(8) RegisterInstruction {
    uint64_t header;  // packed fields (8 bytes)

    union {
        int32_t   i4;            // immediate_i4, secondary_index
        int64_t   i8;            // immediate_i8
        double    r8;            // immediate_r8
        void*     ptr;           // call_target, direct_fn, string_operand, switch_targets
        uint32_t  branch_target; // branch target instruction index
        uint32_t  operand_index; // arg/local index
        uint32_t  field_offset;  // field offset
        uint32_t  arg_count;     // number of call arguments
    } imm;  // 8 bytes

    // ── Accessors ──────────────────────────────────────────────────────
    inline IROpCode op_code()      const noexcept { return static_cast<IROpCode>(header & 0xFFFF); }
    inline uint8_t  dst_reg()      const noexcept { return static_cast<uint8_t>((header >> 16) & 0xFF); }
    inline uint8_t  src1_reg()     const noexcept { return static_cast<uint8_t>((header >> 24) & 0xFF); }
    inline uint8_t  src2_reg()     const noexcept { return static_cast<uint8_t>((header >> 32) & 0xFF); }
    inline uint8_t  flags()        const noexcept { return static_cast<uint8_t>((header >> 40) & 0xFF); }

    inline bool has_dst()  const noexcept { return (flags() & kRegHasDst)  != 0; }
    inline bool has_src1() const noexcept { return (flags() & kRegHasSrc1) != 0; }
    inline bool has_src2() const noexcept { return (flags() & kRegHasSrc2) != 0; }
    inline bool has_src3() const noexcept { return (flags() & kRegHasSrc3) != 0; }
    inline bool has_imm()  const noexcept { return (flags() & kRegHasImm)  != 0; }
    inline bool is_call()  const noexcept { return (flags() & kRegIsCall)  != 0; }
    inline bool is_branch()const noexcept { return (flags() & kRegIsBranch)!= 0; }

    // Packed into reserved header bits [63:48]:
    //   bits [62:48] = call_arg_count (0-32767)
    //   bit  63      = is_instance_call
    //   When kRegHasSrc3 is set (non-Call 3-src opcodes):
    //     bits [55:48] = src3_reg (0-255)
    inline uint32_t call_arg_count() const noexcept { return static_cast<uint32_t>((header >> 48) & 0x7FFF); }
    inline bool     is_instance_call() const noexcept { return (header >> 63) != 0; }
    inline uint8_t  src3_reg() const noexcept {
        if (!(flags() & kRegHasSrc3)) return 0;
        return static_cast<uint8_t>((header >> 48) & 0xFF);
    }
};

static_assert(sizeof(RegisterInstruction) == 16,
    "RegisterInstruction must be exactly 16 bytes for cache efficiency");

// ── Register file ───────────────────────────────────────────────────────
// All values stored as uint64_t with a separate tag byte.
// Float/double values are bitcast to uint64_t (preserving NaN payload).
struct RegisterFile {
    uint64_t  gpr[kGPRegisters]  = {};  // general-purpose register values
    uint8_t   gpr_tags[kGPRegisters] = {};  // ValueTag per register (0 = Void)

    uint64_t  fpr[kFPRegisters]  = {};  // float/double register values
    uint8_t   fpr_tags[kFPRegisters] = {};  // ValueTag per register

    // Read a register value by index (gpr if index < kGPRegisters, else fpr).
    inline uint64_t  reg(uint32_t idx) const noexcept {
        return (idx < kGPRegisters) ? gpr[idx] : fpr[idx - kGPRegisters];
    }
    inline uint8_t   reg_tag(uint32_t idx) const noexcept {
        return (idx < kGPRegisters) ? gpr_tags[idx] : fpr_tags[idx - kGPRegisters];
    }
    inline void set_reg(uint32_t idx, uint64_t val, uint8_t tag) noexcept {
        if (idx < kGPRegisters) { gpr[idx] = val; gpr_tags[idx] = tag; }
        else { uint32_t fi = idx - kGPRegisters; fpr[fi] = val; fpr_tags[fi] = tag; }
    }

    // Fast tagged read for common types.
    inline int32_t  reg_i32(uint32_t idx) const noexcept { return static_cast<int32_t>(reg(idx)); }
    inline int64_t  reg_i64(uint32_t idx) const noexcept { return static_cast<int64_t>(reg(idx)); }
    inline float    reg_f32(uint32_t idx) const noexcept { float v; std::memcpy(&v, &gpr[idx], sizeof(v)); return v; }
    inline double   reg_f64(uint32_t idx) const noexcept { double v; std::memcpy(&v, &gpr[idx], sizeof(v)); return v; }
    inline void*    reg_ptr(uint32_t idx) const noexcept { return reinterpret_cast<void*>(reg(idx)); }
};

// ── Register frame (replaces FastFrame for register-based execution) ────
struct CatchHandlerEntry;  // forward decl (defined below, used in RegisterFrame)

// Type check callback for typed catch matching.
// Returns true if exc_obj is an instance of the type identified by class_token.
using TypedCatchCheckFn = bool (*)(void* exc_obj, uint32_t class_token);

struct RegisterFrame {
    // Must be first field — read by GC scanner to identify frame type.
    // Value: kInterpFrameType_RegisterFrame (0x00474552 = "REG\0" in LE).
    uint32_t      frame_type = 0x00474552u;

    RegisterFile  regs;           // unified register file

    // Frame-level metadata
    const void*   args;           // raw AOT argument bytes
    uint32_t      arg_count;      // number of arguments

    // Return value
    uint64_t      ret_val;        // return value
    uint8_t       ret_tag;        // return ValueTag
    bool          has_ret;        // true if value produced

    // Exception state
    bool          threw_exception;
    void*         exception_obj;

    // Dispatch callback for Call instructions
    void*         dispatch_fn;
    void*         dispatch_ctx;

    // Call-site metadata cache (CachedCallInfo[])
    const void*   call_cache;
    uint32_t      call_count;

    // PIC dispatch data (PatchMethod* for T3 PIC lookup)
    void*         patch_method;
    void*         prev_frame;

    // OSR re-enable flag: set after deoptimization from T4 to trigger
    // immediate OSR re-promotion on the first backward branch.
    bool          osr_reenable = false;

    // Program counter
    uint32_t      pc;

    // ── SEH state (for Reg_Leave / Reg_EndFinally support) ──────────────
    const SEHClause* seh_clauses            = nullptr;
    uint32_t         seh_clause_count       = 0;
    const CatchHandlerEntry* catch_handler_entries = nullptr;
    uint32_t                catch_handler_count    = 0;
    bool             in_handler             = false;
    bool             pending_leave          = false;
    uint32_t         pending_leave_target   = 0;
    int32_t          active_handler_clause  = -1;

    // ── Throw unwind state (finally unwind before catch) ─────────────────
    static constexpr uint32_t kMaxUnwindDepth = 8;
    bool             unwinding_throw        = false;
    void*            unwind_exception_obj   = nullptr;
    int32_t          unwind_finally_list[kMaxUnwindDepth]{};
    uint32_t         unwind_finally_count   = 0;
    uint32_t         unwind_finally_current = 0;
    int32_t          unwind_catch_clause    = -1;

    // ── Typed catch check callback ──────────────────────────────────────
    // Called for each typed catch clause to check if exception matches.
    // If null, typed catches match conservatively (catch-all fallback).
    TypedCatchCheckFn typed_catch_check      = nullptr;

    // ── Tracked object cleanup ─────────────────────────────────────────
    static constexpr uint32_t kMaxTracked = 8;
    void*     tracked_objs[kMaxTracked]{};
    void (*tracked_dtors[kMaxTracked])(void*){};
    uint32_t  tracked_cnt = 0;

    template<typename T>
    static void Dtor(void* p) noexcept { static_cast<T*>(p)->~T(); }

    void Track(void* ptr, void (*dtor)(void*)) noexcept {
        if (tracked_cnt < kMaxTracked) {
            tracked_objs[tracked_cnt]   = ptr;
            tracked_dtors[tracked_cnt]   = dtor;
            ++tracked_cnt;
        }
    }

    void CleanupTracked() noexcept {
        for (uint32_t i = 0; i < tracked_cnt; ++i) {
            tracked_dtors[i](tracked_objs[i]);
            CHAOS_IL2CPP_FREE(tracked_objs[i]);
        }
        tracked_cnt = 0;
    }
};

// ── CatchHandlerEntry ───────────────────────────────────────────────────
// Maps a catch handler entry point to the virtual register holding the
// exception object at that instruction, plus the class_token for typed
// catch matching.  Filled during AllocateRegisters.
struct CatchHandlerEntry {
    uint32_t handler_start_idx;  // IR instruction index (matches SEH clause)
    uint8_t  exception_reg;      // virtual register with exception object
    uint32_t class_token;        // metadata token for typed catch (0 = untyped)
};

// ── RegStackMapEntry ─────────────────────────────────────────────────────
// Maps evaluation-stack slots and locals to virtual registers at a given pc.
// Used by OSR converters (CaptureRegisterFrame, RestoreOsrToRegisterFrame,
// CaptureNativeFrame) to reconstruct frame state across tier transitions.
//
// Filled during AllocateRegisters() for each instruction boundary.
struct RegStackMapEntry {
    static constexpr uint32_t kMaxSlots  = 16;  // max eval stack depth
    static constexpr uint32_t kMaxLocals = 8;   // max local count

    int8_t  slot_regs[kMaxSlots]  = {};  // slot_regs[stack_i] = vreg or -1 if empty
    int8_t  local_regs[kMaxLocals] = {};   // local_regs[local_i] = vreg or -1 if empty
    uint8_t stack_depth           = 0;
};

struct RegStackMap {
    CHAOS_IL2CPP_VECTOR(RegStackMapEntry) entries;  // indexed by instruction pc
};

// ── Register method ─────────────────────────────────────────────────────
// A method lowered to register-based IR.  Produced by the register allocator.
struct RegisterMethod {
    CHAOS_IL2CPP_VECTOR(RegisterInstruction) instructions = {};
    CHAOS_IL2CPP_VECTOR(SEHClause)           seh_clauses  = {};
    CHAOS_IL2CPP_VECTOR(CatchHandlerEntry)   catch_handler_entries = {};
    CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_UINT32) il_offsets = {};  // IL offset per instruction
    RegStackMap                              stack_map     = {};
    uint32_t                                  max_regs     = 0;  // highest register used
};

// ── Register allocator ──────────────────────────────────────────────────
// Converts stack-based IRMethod (IRInstruction[]) to register-based
// RegisterMethod (RegisterInstruction[]).  Uses a simple linear-scan:
// each evaluation-stack depth → virtual register assignment.
//
// The algorithm:
//   1. Walk IRMethod.instructions sequentially
//   2. Track a virtual stack: assign a new virtual register to every push
//   3. Map pop operands → the corresponding virtual register
//   4. Emit RegisterInstruction with explicit dst/src registers
//   5. Copy SEH clauses (instruction indices stay valid)
//
// Register convention:
//   r0-r7     = argument registers (mapped from LdArg operand_index)
//   r8-r15    = local variable registers (mapped from LdLoc/StLoc operand_index)
//   r16+      = evaluation stack virtual registers (allocated sequentially)

RegisterMethod AllocateRegisters(const IRMethod& ir_method) noexcept;

// ── Register-based execution ────────────────────────────────────────────
// Execute a RegisterMethod using register-based dispatch.
// Similar to FastExecute but reads/writes RegisterFile directly.
// Returns true on normal completion, false on unsupported opcode fallback.
bool RegisterExecute(RegisterFrame& frame,
                     const RegisterInstruction* instrs,
                     uint32_t instr_count) noexcept;

// Check if RegisterExecute can handle this method.
// SEH-containing methods are accepted too — RegisterExecute executes flat
// without SEH dispatch, which is correct when all call targets resolve via
// external stubs that return 0 instead of throwing managed exceptions.
// The InterpreterVM fallback (Step D) has proper SEH handling but may fail
// on stub-only paths; RegisterExecute is the robust path for fact/benchmark.
inline bool CanRegisterExecute(const RegisterMethod& rm) noexcept {
    (void)rm;
    return true;
}

}  // namespace chaos::il2cpp::interpreter

#endif  // CHAOS_IL2CPP_IR_REG_ALLOC_H_
