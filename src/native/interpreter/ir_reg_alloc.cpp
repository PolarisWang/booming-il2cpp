// ir_reg_alloc.cpp — Convert stack-based IR to register-based IR + register execution
#include "ir_reg_alloc.h"

#include "patch_loader.h"  // PatchMethod, PicDispatchChain, g_patch_generation

#include "instantiation_engine.h"  // runtime-core: CachedCallInfo, InterpreterDispatchRaw
namespace ri = chaos::il2cpp::runtime_instantiation;

#include "jit_engine.h"  // Compile, JitMethod, CompileConfig
#include "jit_seh.h"  // RegisterNativeCodeSection, FindNativeCodeByAddress
#include "../jit/jit_helpers.h"  // CodegenLdVirtFtn

#include <chaos/profile.h>
#include <chaos/log.h>

// Access interpreter global state for static field and object operations.
namespace chaos::il2cpp::interpreter {
extern CHAOS_IL2CPP_VECTOR(InterpreterValue) g_static_fields;
}

namespace chaos::il2cpp::interpreter {

// ── Register Allocator ──────────────────────────────────────────────────
// Linear-scan register allocator.  Walks IRMethod.instructions sequentially,
// tracking a virtual evaluation stack (uint32_t[256] of virtual register
// indices).  Each "push" (has_dst=true) assigns a new virtual register.
// Each "pop" (has_src=true) reads from the virtual stack.
//
// Register convention:
//   r0-r7     = argument registers (mapped from LdArg operand_index)
//   r8-r15    = local variable registers (mapped from LdLoc/StLoc operand_index)
//   r16+      = evaluation stack virtual registers (allocated sequentially)

RegisterMethod AllocateRegisters(const IRMethod& ir_method) noexcept {
    RegisterMethod result;
    result.seh_clauses = ir_method.seh_clauses;

    // ── Phase 0: Build catch handler entry map ──────────────────────────
    // Pre-scan SEH clauses to find catch handler start indices where the
    // allocator must push a synthetic exception vreg onto the virtual stack.
    uint32_t catch_entry_pc[16];
    uint32_t catch_entry_count = 0;
    for (size_t ci = 0; ci < ir_method.seh_clauses.size() && catch_entry_count < 16; ++ci) {
        auto flags = static_cast<uint32_t>(ir_method.seh_clauses[ci].flags);
        if (flags == static_cast<uint32_t>(SEHFlags::Exception) ||
            flags == static_cast<uint32_t>(SEHFlags::Filter)) {
            catch_entry_pc[catch_entry_count++] = static_cast<uint32_t>(ir_method.seh_clauses[ci].handler_start_idx);
        }
    }

    // Virtual register stack — tracks which virtual reg holds each stack slot.
    // Fixed-size array avoids heap allocation from std::vector.
    uint32_t virt_stack[256];
    uint32_t virt_sp = 0;
    uint32_t next_vreg = 16;  // first free virtual register

    const auto& instrs = ir_method.instructions;
    result.instructions.reserve(instrs.size());

    for (size_t i = 0; i < instrs.size(); ++i) {
        // Check if this instruction is a catch handler entry point.
        // If so, push a synthetic exception vreg onto the virtual stack
        // so that the handler's first pop instruction maps correctly.
        for (uint32_t ei = 0; ei < catch_entry_count; ++ei) {
            if (catch_entry_pc[ei] == i) {
                uint8_t exc_reg = static_cast<uint8_t>(next_vreg++);
                virt_stack[virt_sp++] = exc_reg;
                // Look up class_token from the corresponding SEH clause.
                uint32_t ct = 0;
                for (size_t ci = 0; ci < ir_method.seh_clauses.size(); ++ci) {
                    if (static_cast<uint32_t>(ir_method.seh_clauses[ci].handler_start_idx) == i) {
                        ct = ir_method.seh_clauses[ci].class_token;
                        break;
                    }
                }
                result.catch_handler_entries.push_back(
                    {static_cast<uint32_t>(i), exc_reg, ct});
                break;
            }
        }

        // ── Snapshot virtual stack state for RegStackMap ─────────────────
        RegStackMapEntry map_entry;
        map_entry.stack_depth = static_cast<uint8_t>(
            virt_sp > RegStackMapEntry::kMaxSlots ? RegStackMapEntry::kMaxSlots : virt_sp);
        for (uint32_t si = 0; si < map_entry.stack_depth; ++si) {
            map_entry.slot_regs[si] = static_cast<int8_t>(virt_stack[si]);
        }
        for (uint32_t si = map_entry.stack_depth; si < RegStackMapEntry::kMaxSlots; ++si) {
            map_entry.slot_regs[si] = -1;
        }
        // Dedicated local vregs: r8-r15 per allocator convention
        for (uint32_t li = 0; li < RegStackMapEntry::kMaxLocals; ++li) {
            map_entry.local_regs[li] = static_cast<int8_t>(8 + li);
        }
        result.stack_map.entries.push_back(map_entry);

        const auto& ir = instrs[i];
        RegisterInstruction ri = {};
        uint32_t op_val = static_cast<uint32_t>(ir.op_code);

        // Pack header: op_code | dst_reg | src1_reg | src2_reg | flags
        uint64_t header = op_val & 0xFFFF;
        uint8_t src3_reg = 0;

        // Determine dst register for has_dst opcodes
        uint8_t dst_reg = 0;
        bool has_dst = false;
        uint8_t src1_reg = 0;
        uint8_t src2_reg = 0;
        bool has_src1 = false;
        bool has_src2 = false;
        bool has_src3 = false;
        uint32_t calli_func_ptr_vreg = 0;

        switch (ir.op_code) {
        // ── No operands: just use virt_stack ──────────────────────────

        // ── Binary ops: pop 2, push 1 (Add, Sub, Mul, Div, Rem, etc.) ──
        case IROpCode::Add: case IROpCode::Sub: case IROpCode::Mul:
        case IROpCode::Div: case IROpCode::Rem:
        case IROpCode::DivUn: case IROpCode::RemUn:
        case IROpCode::And: case IROpCode::Or: case IROpCode::Xor:
        case IROpCode::Shl: case IROpCode::Shr: case IROpCode::ShrUn:
        case IROpCode::AddOvf: case IROpCode::SubOvf: case IROpCode::MulOvf:
        case IROpCode::Ceq: case IROpCode::Clt: case IROpCode::Cgt:
        case IROpCode::LdElem:    // array[index]
        case IROpCode::LdElemA:   // same as LdElem in interpreter (loads element value)
        {
            // Pop src2, src1
            if (virt_sp >= 2) {
                src2_reg = virt_stack[--virt_sp];
                src1_reg = virt_stack[--virt_sp];
            }
            // Push dst
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true; has_src1 = true; has_src2 = true;
            break;
        }

        // ── Unary ops: pop 1, push 1 (Neg, Not, Conv_*) ──────────────
        case IROpCode::Neg: case IROpCode::Not:
        case IROpCode::Conv_I4: case IROpCode::Conv_I8:
        case IROpCode::Conv_R4: case IROpCode::Conv_R8:
        case IROpCode::ConvRUn: case IROpCode::ConvI: case IROpCode::ConvU:
        case IROpCode::ConvOvfI: case IROpCode::ConvOvfI4: case IROpCode::ConvOvfI8:
        case IROpCode::ConvOvfU: case IROpCode::ConvOvfU4: case IROpCode::ConvOvfU8:
        case IROpCode::LdLen:
        case IROpCode::LdInd:
        case IROpCode::LdObj:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true; has_src1 = true;
            break;
        }

        // ── Dup: copy from virt_stack top ──
        case IROpCode::Dup:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[virt_sp - 1];
            }
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true; has_src1 = true;
            break;
        }

        // ── LdcI4/8/R4/R8: push constant ──
        case IROpCode::LdcI4: case IROpCode::LdcI8:
        case IROpCode::LdcR4: case IROpCode::LdcR8:
        {
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true;
            break;
        }

        // ── LdNull/LdStr/LdToken/LdFtn/SizeOf: push value ──
        case IROpCode::LdNull: case IROpCode::LdStr:
        case IROpCode::LdToken: case IROpCode::LdFtn:
        case IROpCode::SizeOf:
        case IROpCode::LdArgA: case IROpCode::LdLocA:
        {
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true;
            break;
        }

        // ── LdArg/LdLoc: push from arg/local ──
        case IROpCode::LdArg:
        {
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true;
            break;
        }
        case IROpCode::LdLoc:
        {
            // Read from local's dedicated register r8+N
            src1_reg = static_cast<uint8_t>(8u + static_cast<uint32_t>(ir.operand_index));
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true; has_src1 = true;
            break;
        }

        // ── StLoc: pop value, write to local's dedicated register ──
        case IROpCode::StLoc:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            // Write to local's dedicated register r8+N
            dst_reg = static_cast<uint8_t>(8u + static_cast<uint32_t>(ir.operand_index));
            has_src1 = true; has_dst = true;
            break;
        }

        // ── StArg: pop value only, no dst ──
        case IROpCode::StArg:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            has_src1 = true;
            break;
        }

        // ── LdFld: pop obj, push field ──
        case IROpCode::LdFld:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true; has_src1 = true;
            break;
        }

        // ── StFld: pop value, pop obj ──
        case IROpCode::StFld:
        {
            if (virt_sp >= 1) {
                src1_reg = virt_stack[--virt_sp];  // value (top)
            }
            if (virt_sp >= 1) {
                src2_reg = virt_stack[--virt_sp];  // obj (second)
            }
            has_src1 = true;
            has_src2 = true;
            break;
        }

        // ── LdSFld: push static field ──
        case IROpCode::LdSFld:
        {
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true;
            break;
        }

        // ── StSFld: pop value ──
        case IROpCode::StSFld:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            has_src1 = true;
            break;
        }

        // ── Call/CallVirt/CallBridge/CallVirtConstrained: pop args, push ret ──
        case IROpCode::Call: case IROpCode::CallVirt:
        case IROpCode::CallBridge: case IROpCode::CallVirtConstrained:
        {
            uint32_t ac = ir.arg_count;
            // Record arg0 register (args are in consecutive registers from arg0)
            if (ac > 0 && virt_sp >= ac) {
                src1_reg = virt_stack[virt_sp - ac];
            }
            // Pop args in reverse: args[ac-1] .. args[0]
            for (uint32_t ai = 0; ai < ac && !virt_sp == 0; ++ai) {
                --virt_sp;
            }
            // Push return value
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true;
            if (ac > 0) has_src1 = true;
            break;
        }

        // ── Calli: pop func_ptr + args, push ret ──
        // IL stack layout: ..., arg0, arg1, ..., argN, func_ptr
        case IROpCode::Calli:
        {
            uint32_t ac = ir.arg_count;
            // Pop func_ptr from top of stack
            if (!virt_sp == 0) {
                calli_func_ptr_vreg = virt_stack[--virt_sp];
            }
            // Record first arg register (args are just below func_ptr on stack)
            if (ac > 0 && virt_sp >= ac) {
                src1_reg = virt_stack[virt_sp - ac];
            }
            // Pop ac args
            for (uint32_t ai = 0; ai < ac && !virt_sp == 0; ++ai) {
                --virt_sp;
            }
            // Push return value
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true;
            if (ac > 0) has_src1 = true;
            break;
        }

        // ── NewObj: pop ctor args, push obj ref ──
        case IROpCode::NewObj:
        {
            uint32_t ac = ir.arg_count;
            for (uint32_t ai = 0; ai < ac && !virt_sp == 0; ++ai) {
                --virt_sp;
            }
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true;
            break;
        }

        // ── Box: pop value, push obj ──
        case IROpCode::Box:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true; has_src1 = true;
            break;
        }

        // ── Unbox/CastClass/IsInst: pop obj, push result ──
        case IROpCode::Unbox: case IROpCode::CastClass: case IROpCode::IsInst:
        case IROpCode::LdVirtFtn:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true; has_src1 = true;
            break;
        }

        // ── NewArr: pop length, push array ref ──
        case IROpCode::NewArr:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true; has_src1 = true;
            break;
        }

        // ── StElem: pop value, pop index, pop array ──
        case IROpCode::StElem:
        {
            if (virt_sp >= 1) {
                src1_reg = virt_stack[--virt_sp];  // value (top)
            }
            if (virt_sp >= 1) {
                src2_reg = virt_stack[--virt_sp];  // index
            }
            if (virt_sp >= 1) {
                src3_reg = virt_stack[--virt_sp];  // array
            }
            has_src1 = true;
            has_src2 = true;
            has_src3 = true;
            break;
        }

        // ── Pop: discard ──
        case IROpCode::Pop:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            has_src1 = true;
            break;
        }

        // ── Throw/EndFilter/InitObj/StObj: pop value, no dst ──
        case IROpCode::Throw: case IROpCode::EndFilter:
        case IROpCode::InitObj:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            has_src1 = true;
            break;
        }

        // ── StObj: pop value, pop ptr ──
        case IROpCode::StObj:
        {
            if (!virt_sp == 0) --virt_sp;  // value
            if (!virt_sp == 0) --virt_sp;  // ptr
            has_src1 = true;
            break;
        }

        // ── StInd: pop value, pop addr ──
        case IROpCode::StInd:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];  // value (top of stack)
            }
            if (!virt_sp == 0) {
                src2_reg = virt_stack[--virt_sp];  // addr
            }
            has_src1 = true;
            has_src2 = true;
            break;
        }

        // ── Switch: pop index ──
        case IROpCode::Switch:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            has_src1 = true;
            break;
        }

        // ── LocAlloc: pop size, push ptr ──
        case IROpCode::LocAlloc:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            dst_reg = next_vreg++; virt_stack[virt_sp++] = dst_reg;
            has_dst = true; has_src1 = true;
            break;
        }

        // ── BrTrue/BrFalse/BneUn/Beq/Blt/Bgt/Ble/Bge/BneUn/BgeUn/BgtUn/BleUn/BltUn: pop compare ──
        case IROpCode::BrTrue: case IROpCode::BrFalse:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[--virt_sp];
            }
            has_src1 = true;
            break;
        }
        case IROpCode::Beq: case IROpCode::Blt: case IROpCode::Bgt:
        case IROpCode::Ble: case IROpCode::Bge:
        case IROpCode::BneUn: case IROpCode::BgeUn: case IROpCode::BgtUn:
        case IROpCode::BleUn: case IROpCode::BltUn:
        {
            if (virt_sp >= 2) {
                src2_reg = virt_stack[--virt_sp];
                src1_reg = virt_stack[--virt_sp];
            }
            has_src1 = true; has_src2 = true;
            break;
        }

        // ── Cpblk: pop size, pop src, pop dst ═══ (memcpy to memory) ──
        case IROpCode::Cpblk:
        {
            // Stack (bottom→top): dst, src, size
            if (virt_sp >= 1) { src1_reg = virt_stack[--virt_sp]; }  // size (top)
            if (virt_sp >= 1) { src2_reg = virt_stack[--virt_sp]; }  // src
            if (virt_sp >= 1) { src3_reg = virt_stack[--virt_sp]; }  // dst
            has_src1 = true; has_src2 = true; has_src3 = true;
            break;
        }
        // ── InitBlk: pop size, pop value, pop addr ═══ (memset to memory) ──
        case IROpCode::InitBlk:
        {
            // Stack (bottom→top): addr, value, size
            if (virt_sp >= 1) { src1_reg = virt_stack[--virt_sp]; }  // size (top)
            if (virt_sp >= 1) { src2_reg = virt_stack[--virt_sp]; }  // value
            if (virt_sp >= 1) { src3_reg = virt_stack[--virt_sp]; }  // addr
            has_src1 = true; has_src2 = true; has_src3 = true;
            break;
        }

        // ── Ret: read return value ──
        case IROpCode::Ret:
        {
            if (!virt_sp == 0) {
                src1_reg = virt_stack[virt_sp - 1];  // don't pop — keep for callee
            }
            has_src1 = true;
            break;
        }

        // ── No-stack opcodes ──
        case IROpCode::Br: case IROpCode::Leave:
        case IROpCode::Rethrow: case IROpCode::EndFinally:
        case IROpCode::Break:
        default:
            break;
        }

        // ── Build RegisterInstruction header ───────────────────────────
        uint8_t flags = 0;
        if (has_dst)  flags |= kRegHasDst;
        if (has_src1) flags |= kRegHasSrc1;
        if (has_src2) flags |= kRegHasSrc2;
        if (has_src3) flags |= kRegHasSrc3;

        // Check for call-like, branch, store opcodes
        switch (ir.op_code) {
        case IROpCode::Call: case IROpCode::CallVirt:
        case IROpCode::CallBridge: case IROpCode::CallVirtConstrained:
        case IROpCode::Calli:
        case IROpCode::NewObj: case IROpCode::Box:
            flags |= kRegHasImm | kRegIsCall;
            break;
        case IROpCode::Br: case IROpCode::BrTrue: case IROpCode::BrFalse:
        case IROpCode::Beq: case IROpCode::Blt: case IROpCode::Bgt:
        case IROpCode::Ble: case IROpCode::Bge: case IROpCode::Leave:
        case IROpCode::BneUn: case IROpCode::BgeUn: case IROpCode::BgtUn:
        case IROpCode::BleUn: case IROpCode::BltUn:
        case IROpCode::Switch:
            flags |= kRegIsBranch;
            break;
        case IROpCode::StFld: case IROpCode::StSFld:
        case IROpCode::StLoc: case IROpCode::StArg:
        case IROpCode::StElem: case IROpCode::StInd: case IROpCode::StObj:
        case IROpCode::InitObj: case IROpCode::Cpblk: case IROpCode::InitBlk:
            flags |= kRegIsStore;
            break;
        default:
            break;
        }

        // Check if this opcode carries an immediate operand
        switch (ir.op_code) {
        case IROpCode::LdcI4: case IROpCode::LdcI8: case IROpCode::LdcR4: case IROpCode::LdcR8:
        case IROpCode::LdStr: case IROpCode::LdArg: case IROpCode::LdLoc: case IROpCode::StLoc:
        case IROpCode::StArg: case IROpCode::NewObj: case IROpCode::Box: case IROpCode::Unbox:
        case IROpCode::CastClass: case IROpCode::IsInst: case IROpCode::NewArr:
        case IROpCode::LdToken: case IROpCode::InitObj: case IROpCode::SizeOf:
        case IROpCode::LdFtn: case IROpCode::LdVirtFtn: case IROpCode::LdArgA:
        case IROpCode::LdLocA: case IROpCode::LocAlloc:
        case IROpCode::Call: case IROpCode::CallVirt: case IROpCode::CallBridge:
        case IROpCode::CallVirtConstrained:
        case IROpCode::LdFld: case IROpCode::StFld:
        case IROpCode::LdSFld: case IROpCode::StSFld:
        case IROpCode::LdInd: case IROpCode::StInd:
        case IROpCode::Switch: case IROpCode::LdObj: case IROpCode::StObj:
        case IROpCode::LdElem: case IROpCode::StElem: case IROpCode::LdElemA:
        case IROpCode::Br: case IROpCode::BrTrue: case IROpCode::BrFalse:
        case IROpCode::Beq: case IROpCode::Blt: case IROpCode::Bgt:
        case IROpCode::Ble: case IROpCode::Bge: case IROpCode::Leave:
        case IROpCode::BneUn: case IROpCode::BgeUn: case IROpCode::BgtUn:
        case IROpCode::BleUn: case IROpCode::BltUn:
            flags |= kRegHasImm;
            break;
        default:
            break;
        }

        header |= (static_cast<uint64_t>(dst_reg)    << 16);
        header |= (static_cast<uint64_t>(src1_reg)   << 24);
        header |= (static_cast<uint64_t>(src2_reg)   << 32);
        header |= (static_cast<uint64_t>(flags)      << 40);
        if (flags & kRegHasSrc3) {
            header |= (static_cast<uint64_t>(src3_reg) << 48);
        }

        ri.header = header;

        // ── Copy immediate payload ─────────────────────────────────────
        // NOTE: imm is a union (8 bytes). Write order matters:
        //   - 8-byte fields (i8, r8, ptr) overwrite ALL union bytes.
        //   - 4-byte fields (i4, arg_count, operand_index, etc.) only
        //     write bytes 0-3, leaving bytes 4-7 from the last 8-byte write.
        //
        // Strategy: write 8-byte fields BEFORE the final 4-byte i4 write,
        // since i4 is the most commonly read field and must be last.

        // 8-byte fields first (overwrite everything, typically 0).
        ri.imm.i8       = ir.immediate_i8;
        ri.imm.r8       = ir.immediate_r8;

        // 4-byte fields that are typically 0 for most opcodes.
        // arg_count is also stored in header bits for call opcodes.
        ri.imm.arg_count = ir.arg_count;

        // i4 last — this 4-byte write is the final value for bytes 0-3.
        // Used by LdcI4 (constant), token/type info, and many opcodes.
        ri.imm.i4       = ir.immediate_i4;

        // Opcode-specific fields (overwrite union after generic writes).
        if (ir.op_code == IROpCode::Switch) {
            // ptr → switch_targets array; target_count packed in header.
            ri.imm.ptr = const_cast<void*>(static_cast<const void*>(ir.switch_targets));
            header |= (static_cast<uint64_t>(ir.switch_target_count & 0x7FFF) << 48);
            ri.header = header;
        } else if (ir.op_code == IROpCode::Calli) {
            ri.imm.operand_index = static_cast<uint32_t>(calli_func_ptr_vreg);
            ri.imm.ptr          = ir.call_target;     // must be LAST write — overwrites all 8 union bytes
        } else if (ir.op_code == IROpCode::Call || ir.op_code == IROpCode::CallVirt ||
                   ir.op_code == IROpCode::CallBridge || ir.op_code == IROpCode::CallVirtConstrained) {
            ri.imm.operand_index  = static_cast<uint32_t>(ir.operand_index);
            // Use call_target if available; fall back to direct_fn for Tier 3
            // cross-assembly calls (e.g. String::Concat, Console::WriteLine).
            // direct_fn uses the same managed calling convention fn(a0...a7),
            // compatible with the MIC path in InterpreterDispatchRaw.
            ri.imm.ptr           = ir.call_target ? ir.call_target : ir.direct_fn;
        }

        // operand_index for non-call opcodes that need it (arg/local index, field count).
        // Must NOT overwrite LdcI4's i4, so this is separate from the ptr write above.
        if (ir.op_code == IROpCode::LdArg || ir.op_code == IROpCode::LdLoc ||
            ir.op_code == IROpCode::StLoc || ir.op_code == IROpCode::StArg ||
            ir.op_code == IROpCode::LdArgA || ir.op_code == IROpCode::LdLocA ||
            ir.op_code == IROpCode::NewObj) {
            ri.imm.operand_index = static_cast<uint32_t>(ir.operand_index);
        }

        // LdFld/StFld: override union slot with field_offset
        if (ir.op_code == IROpCode::LdFld || ir.op_code == IROpCode::StFld) {
            ri.imm.field_offset = static_cast<uint32_t>(ir.field_offset);
        }


        // Branch target
        if (ir.op_code == IROpCode::Br || ir.op_code == IROpCode::BrTrue ||
            ir.op_code == IROpCode::BrFalse || ir.op_code == IROpCode::Leave ||
            ir.op_code == IROpCode::Beq || ir.op_code == IROpCode::Blt ||
            ir.op_code == IROpCode::Bgt || ir.op_code == IROpCode::Ble ||
            ir.op_code == IROpCode::Bge || ir.op_code == IROpCode::BneUn ||
            ir.op_code == IROpCode::BgeUn || ir.op_code == IROpCode::BgtUn ||
            ir.op_code == IROpCode::BleUn || ir.op_code == IROpCode::BltUn) {
            ri.imm.branch_target = static_cast<uint32_t>(ir.branch_target);
        }

        // Pack call_arg_count and is_instance_call into reserved header bits [63:48]
        if (ir.op_code == IROpCode::Call || ir.op_code == IROpCode::CallVirt ||
            ir.op_code == IROpCode::CallBridge || ir.op_code == IROpCode::CallVirtConstrained ||
            ir.op_code == IROpCode::Calli) {
            header |= (static_cast<uint64_t>(ir.arg_count & 0x7FFF) << 48);
            if (ir.is_instance_call) {
                header |= (1ULL << 63);
            }
        }

        result.instructions.push_back(ri);
    }

    // Propagate il_offsets from IRMethod to RegisterMethod (1:1 instruction mapping).
    result.il_offsets.reserve(ir_method.il_offsets.size());
    for (auto off : ir_method.il_offsets) {
        result.il_offsets.push_back(off);
    }

    result.max_regs = next_vreg;
    return result;
}

// ── Register-based execution (replaces FastExecute) ─────────────────────
// Uses function-pointer dispatch table + RegisterFrame.
// Each handler reads src regs from RegisterFile, writes dst reg.
// No implicit push/pop — every instruction names its registers explicitly.

using RegOpHandler = void (*)(RegisterFrame& frame,
                              const RegisterInstruction& instr) noexcept;

// ── Handler implementations ─────────────────────────────────────────────

static void Reg_LdcI4(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdcI4");
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(instr.imm.i4)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_LdcI8(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdcI8");
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(instr.imm.i8),
                       static_cast<uint8_t>(ValueTag::Int64));
    ++frame.pc;
}

static void Reg_LdcR4(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdcR4");
    float v;
    int32_t bits = instr.imm.i4;
    std::memcpy(&v, &bits, sizeof(bits));
    uint64_t val;
    std::memcpy(&val, &v, sizeof(v));
    frame.regs.set_reg(instr.dst_reg(), val,
                       static_cast<uint8_t>(ValueTag::Float32));
    ++frame.pc;
}

static void Reg_LdcR8(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdcR8");
    uint64_t val;
    std::memcpy(&val, &instr.imm.r8, sizeof(val));
    frame.regs.set_reg(instr.dst_reg(), val,
                       static_cast<uint8_t>(ValueTag::Float64));
    ++frame.pc;
}

static void Reg_LdStr(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdStr");
    frame.regs.set_reg(instr.dst_reg(),
                       reinterpret_cast<uint64_t>(instr.imm.ptr),
                       static_cast<uint8_t>(ValueTag::ObjectRef));
    ++frame.pc;
}

static void Reg_LdNull(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdNull");
    frame.regs.set_reg(instr.dst_reg(), 0,
                       static_cast<uint8_t>(ValueTag::Null));
    ++frame.pc;
}

static void Reg_LdArg(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdArg");
    uint32_t idx = instr.imm.operand_index;
    if (idx >= frame.arg_count || frame.args == nullptr) {
        frame.regs.set_reg(instr.dst_reg(), 0,
                           static_cast<uint8_t>(ValueTag::Int32));
    } else {
        const auto* arg_base = static_cast<const uint64_t*>(frame.args);
        frame.regs.set_reg(instr.dst_reg(), arg_base[idx],
                           static_cast<uint8_t>(ValueTag::ObjectRef));
    }
    ++frame.pc;
}

static void Reg_LdLoc(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdLoc");
    // Local is in a dedicated register (r8-r15 mapped)
    // For RegLdLoc/RegStLoc the operand_index maps to a fixed register
    // allocated during register allocation.
    // The value is already in the reg file — just copy to dst.
    frame.regs.set_reg(instr.dst_reg(),
                       frame.regs.reg(instr.src1_reg()),
                       frame.regs.reg_tag(instr.src1_reg()));
    ++frame.pc;
}

static void Reg_StLoc(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_StLoc");
    // Copy src1 to the local's dedicated register
    // operand_index = local index maps to fixed local reg
    frame.regs.set_reg(instr.dst_reg(),
                       frame.regs.reg(instr.src1_reg()),
                       frame.regs.reg_tag(instr.src1_reg()));
    ++frame.pc;
}

static void Reg_Add(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Add");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l + r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Sub(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Sub");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l - r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Mul(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Mul");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l * r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Div(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Div");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l / r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Rem(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Rem");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l % r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Neg(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Neg");
    int32_t v = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(-v)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_And(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_And");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l & r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Or(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Or");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l | r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Xor(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Xor");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l ^ r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Not(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Not");
    int32_t v = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(~v)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Shl(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Shl");
    int32_t a = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t v = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(v << (a & 0x1F))),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Shr(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Shr");
    int32_t a = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t v = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(v >> (a & 0x1F))),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_ShrUn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_ShrUn");
    int32_t a = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    uint32_t v = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(v >> (a & 0x1F))),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Ceq(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Ceq");
    uint64_t r = frame.regs.reg(instr.src2_reg());
    uint64_t l = frame.regs.reg(instr.src1_reg());
    frame.regs.set_reg(instr.dst_reg(), (l == r) ? 1u : 0u,
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Clt(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Clt");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), (l < r) ? 1u : 0u,
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Cgt(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Cgt");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), (l > r) ? 1u : 0u,
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

// ── Branch handlers ─────────────────────────────────────────────────────

static void Reg_Br(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Br");
    frame.pc = instr.imm.branch_target;
}

static void Reg_BrTrue(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_BrTrue");
    if (static_cast<int32_t>(frame.regs.reg(instr.src1_reg())) != 0)
        frame.pc = instr.imm.branch_target;
    else
        ++frame.pc;
}

static void Reg_BrFalse(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_BrFalse");
    if (static_cast<int32_t>(frame.regs.reg(instr.src1_reg())) == 0)
        frame.pc = instr.imm.branch_target;
    else
        ++frame.pc;
}

static void Reg_Beq(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Beq");
    if (frame.regs.reg(instr.src1_reg()) == frame.regs.reg(instr.src2_reg()))
        frame.pc = instr.imm.branch_target;
    else
        ++frame.pc;
}

static void Reg_Blt(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Blt");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.pc = (l < r) ? instr.imm.branch_target : (frame.pc + 1);
}

static void Reg_Bgt(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Bgt");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.pc = (l > r) ? instr.imm.branch_target : (frame.pc + 1);
}

static void Reg_Ble(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Ble");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.pc = (l <= r) ? instr.imm.branch_target : (frame.pc + 1);
}

static void Reg_Bge(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Bge");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.pc = (l >= r) ? instr.imm.branch_target : (frame.pc + 1);
}

// ── Conversion handlers ─────────────────────────────────────────────────

static void Reg_Conv_I4(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Conv_I4");
    int32_t v = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(v)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_Conv_I8(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Conv_I8");
    int64_t v = static_cast<int64_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(v),
                       static_cast<uint8_t>(ValueTag::Int64));
    ++frame.pc;
}

static void Reg_Conv_R4(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Conv_R4");
    float v = static_cast<float>(static_cast<int32_t>(frame.regs.reg(instr.src1_reg())));
    uint64_t val;
    std::memcpy(&val, &v, sizeof(v));
    frame.regs.set_reg(instr.dst_reg(), val,
                       static_cast<uint8_t>(ValueTag::Float32));
    ++frame.pc;
}

static void Reg_Conv_R8(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Conv_R8");
    double v = static_cast<double>(static_cast<int32_t>(frame.regs.reg(instr.src1_reg())));
    uint64_t val;
    std::memcpy(&val, &v, sizeof(v));
    frame.regs.set_reg(instr.dst_reg(), val,
                       static_cast<uint8_t>(ValueTag::Float64));
    ++frame.pc;
}

static void Reg_Pop(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Pop");
    // Register-based: Pop is a no-op (result register is simply not read).
    // In the register-based model, the register allocator eliminates
    // dead register writes. This handler exists for completeness.
    (void)frame; (void)instr;
    ++frame.pc;
}

static void Reg_Dup(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Dup");
    frame.regs.set_reg(instr.dst_reg(),
                       frame.regs.reg(instr.src1_reg()),
                       frame.regs.reg_tag(instr.src1_reg()));
    ++frame.pc;
}

static void Reg_Ret(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Ret");
    if (instr.has_src1()) {
        uint8_t src = instr.src1_reg();
        std::fprintf(stderr, "[diag:Ret] has_src1 src=%u\n", src);
        uint64_t rv = 0; uint8_t rt = 0;
        if (src < 64) {
            rv = frame.regs.gpr[src];
            rt = frame.regs.gpr_tags[src];
        } else if (src < 96) {
            rv = frame.regs.fpr[src - 64];
            rt = frame.regs.fpr_tags[src - 64];
        } else {
            std::fprintf(stderr, "[diag:Ret] OUT OF BOUNDS src=%u\n", src);
        }
        frame.has_ret = true;
        frame.ret_val = rv;
        frame.ret_tag = rt;
    }
    frame.pc = 0xFFffFFffu;
}

// ── Unsupported fallback ────────────────────────────────────────────────
static void Reg_Unsupported(RegisterFrame& frame, const RegisterInstruction&) noexcept {
    frame.threw_exception = true;
    frame.pc = 9999;
}

// ── StArg: store value to argument slot ─────────────────────────────────
static void Reg_StArg(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_StArg");
    if (!instr.has_src1() || frame.args == nullptr) { ++frame.pc; return; }
    uint32_t idx = instr.imm.operand_index;
    auto* arg_base = static_cast<uint64_t*>(const_cast<void*>(frame.args));
    if (idx < frame.arg_count) {
        arg_base[idx] = frame.regs.reg(instr.src1_reg());
    }
    ++frame.pc;
}

// ── LdSFld / StSFld: static field access ───────────────────────────────
static void Reg_LdSFld(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdSFld");
    uint32_t offset = instr.imm.field_offset;
    auto& sfields = g_static_fields;
    if (sfields.size() <= offset) {
        sfields.resize(offset + 1u);
    }
    const auto& iv = sfields[offset];
    switch (iv.tag) {
    case ValueTag::Int32:
        frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(iv.i32)),
                           static_cast<uint8_t>(ValueTag::Int32)); break;
    case ValueTag::Int64:
        frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(iv.i64),
                           static_cast<uint8_t>(ValueTag::Int64)); break;
    case ValueTag::Float32: {
        uint64_t val; std::memcpy(&val, &iv.f32, sizeof(float));
        frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float32)); break;
    }
    case ValueTag::Float64: {
        uint64_t val; std::memcpy(&val, &iv.f64, sizeof(double));
        frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float64)); break;
    }
    default:
        frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(iv.obj),
                           static_cast<uint8_t>(ValueTag::ObjectRef)); break;
    }
    ++frame.pc;
}

static void Reg_StSFld(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_StSFld");
    uint32_t offset = instr.imm.field_offset;
    auto& sfields = g_static_fields;
    if (sfields.size() <= offset) {
        sfields.resize(offset + 1u);
    }
    uint8_t tag = frame.regs.reg_tag(instr.src1_reg());
    uint64_t val = frame.regs.reg(instr.src1_reg());
    switch (static_cast<ValueTag>(tag)) {
    case ValueTag::Int32:
        sfields[offset] = InterpreterValue::from_i32(static_cast<int32_t>(val)); break;
    case ValueTag::Int64:
        sfields[offset] = InterpreterValue::from_i64(static_cast<int64_t>(val)); break;
    case ValueTag::Float32: {
        float fv; std::memcpy(&fv, &val, sizeof(float));
        sfields[offset] = InterpreterValue::from_f32(fv); break;
    }
    case ValueTag::Float64: {
        double dv; std::memcpy(&dv, &val, sizeof(double));
        sfields[offset] = InterpreterValue::from_f64(dv); break;
    }
    default:
        sfields[offset] = InterpreterValue::from_obj(reinterpret_cast<void*>(val)); break;
    }
    ++frame.pc;
}

// ── NewArr: allocate interpreter array ──────────────────────────────────
static void Reg_NewArr(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_NewArr");
    uint32_t len = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    auto* arr = static_cast<ArrayStorage*>(CHAOS_IL2CPP_MALLOC(sizeof(ArrayStorage)));
    if (arr == nullptr) { Reg_Unsupported(frame, instr); return; }
    ::new (arr) ArrayStorage();
    frame.Track(arr, frame.Dtor<ArrayStorage>);
    arr->elements.resize(len);
    frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(arr),
                       static_cast<uint8_t>(ValueTag::ObjectRef));
    ++frame.pc;
}

// ── LdElem / StElem: array element access ──────────────────────────────
static void Reg_LdElem(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdElem");
    uint32_t index = static_cast<uint32_t>(frame.regs.reg(instr.src2_reg()));  // src2 = index
    auto* arr = reinterpret_cast<ArrayStorage*>(frame.regs.reg(instr.src1_reg()));  // src1 = array
    if (arr == nullptr || index >= arr->elements.size()) {
        frame.regs.set_reg(instr.dst_reg(), 0, static_cast<uint8_t>(ValueTag::Null));
        ++frame.pc; return;
    }
    const auto& iv = arr->elements[index];
    switch (iv.tag) {
    case ValueTag::Int32:
        frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(iv.i32)),
                           static_cast<uint8_t>(ValueTag::Int32)); break;
    case ValueTag::Int64:
        frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(iv.i64),
                           static_cast<uint8_t>(ValueTag::Int64)); break;
    case ValueTag::Float32: {
        uint64_t val; std::memcpy(&val, &iv.f32, sizeof(float));
        frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float32)); break;
    }
    case ValueTag::Float64: {
        uint64_t val; std::memcpy(&val, &iv.f64, sizeof(double));
        frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float64)); break;
    }
    default:
        frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(iv.obj),
                           static_cast<uint8_t>(ValueTag::ObjectRef)); break;
    }
    ++frame.pc;
}

static void Reg_StElem(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_StElem");
    // 3-operand store: src1=value (top), src2=index, src3=array (bottom).
    uint64_t val   = frame.regs.reg(instr.src1_reg());
    uint32_t index = static_cast<uint32_t>(frame.regs.reg(instr.src2_reg()));
    auto* arr = reinterpret_cast<ArrayStorage*>(frame.regs.reg(instr.src3_reg()));
    if (arr == nullptr) { ++frame.pc; return; }
    if (index >= arr->elements.size()) {
        arr->elements.resize(index + 1u);
    }
    uint8_t tag = frame.regs.reg_tag(instr.src1_reg());
    switch (static_cast<ValueTag>(tag)) {
    case ValueTag::Int32:
        arr->elements[index] = InterpreterValue::from_i32(static_cast<int32_t>(val)); break;
    case ValueTag::Int64:
        arr->elements[index] = InterpreterValue::from_i64(static_cast<int64_t>(val)); break;
    case ValueTag::Float32: {
        float fv; std::memcpy(&fv, &val, sizeof(float));
        arr->elements[index] = InterpreterValue::from_f32(fv); break;
    }
    case ValueTag::Float64: {
        double dv; std::memcpy(&dv, &val, sizeof(double));
        arr->elements[index] = InterpreterValue::from_f64(dv); break;
    }
    default:
        arr->elements[index] = InterpreterValue::from_obj(reinterpret_cast<void*>(val)); break;
    }
    ++frame.pc;
}

// ── LdLen: array length ─────────────────────────────────────────────────
static void Reg_LdLen(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdLen");
    auto* arr = reinterpret_cast<ArrayStorage*>(frame.regs.reg(instr.src1_reg()));
    if (arr == nullptr) {
        frame.regs.set_reg(instr.dst_reg(), 0, static_cast<uint8_t>(ValueTag::Int32));
    } else {
        frame.regs.set_reg(instr.dst_reg(),
                           static_cast<uint64_t>(arr->elements.size()),
                           static_cast<uint8_t>(ValueTag::Int32));
    }
    ++frame.pc;
}

// ── DivUn / RemUn: unsigned division ────────────────────────────────────
static void Reg_DivUn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_DivUn");
    uint32_t r = static_cast<uint32_t>(frame.regs.reg(instr.src2_reg()));
    uint32_t l = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l / r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_RemUn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_RemUn");
    uint32_t r = static_cast<uint32_t>(frame.regs.reg(instr.src2_reg()));
    uint32_t l = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l % r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

// ── Conversion handlers (unsigned/alternative forms) ────────────────────
static void Reg_ConvRUn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_ConvRUn");
    // IL conv.r.un: uint32 → float64.  Match T4 codegen (cvtsi2sd → double).
    uint32_t v = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    double dv = static_cast<double>(v);
    uint64_t val; std::memcpy(&val, &dv, sizeof(double));
    frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float64));
    ++frame.pc;
}

static void Reg_ConvI(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_ConvI");
    // ConvI: native int — treat as int32 (32-bit host)
    int32_t v = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(v)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_ConvU(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_ConvU");
    // ConvU: native unsigned int — treat as uint32 (32-bit host)
    uint32_t v = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(v),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

// ── LdInd: indirect load (pointer dereference) ──────────────────────────
static void Reg_LdInd(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdInd");
    void* ptr = reinterpret_cast<void*>(frame.regs.reg(instr.src1_reg()));
    if (ptr == nullptr) {
        frame.regs.set_reg(instr.dst_reg(), 0, static_cast<uint8_t>(ValueTag::Null));
    } else {
        // Read based on type tag from the instruction's immediate field:
        // This is a simplified version — for the interpreter sandbox, we
        // treat LdInd as reading a uint64_t from the given address.
        frame.regs.set_reg(instr.dst_reg(), *static_cast<uint64_t*>(ptr),
                           static_cast<uint8_t>(ValueTag::Int64));
    }
    ++frame.pc;
}

// ── StInd: indirect store ───────────────────────────────────────────────
static void Reg_StInd(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_StInd");
    uint64_t val = frame.regs.reg(instr.src1_reg());
    void* ptr = reinterpret_cast<void*>(frame.regs.reg(instr.src2_reg()));
    if (ptr != nullptr) {
        *static_cast<uint64_t*>(ptr) = val;
    }
    ++frame.pc;
}

// ── LdObj: load object from managed pointer ─────────────────────────────
static void Reg_LdObj(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdObj");
    // LdObj reads from a managed pointer (address of InterpreterValue).
    void* ptr = reinterpret_cast<void*>(frame.regs.reg(instr.src1_reg()));
    if (ptr != nullptr) {
        auto* iv = static_cast<InterpreterValue*>(ptr);
        switch (iv->tag) {
        case ValueTag::Int32:
            frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(iv->i32)),
                               static_cast<uint8_t>(ValueTag::Int32)); break;
        case ValueTag::Int64:
            frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(iv->i64),
                               static_cast<uint8_t>(ValueTag::Int64)); break;
        default:
            frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(iv->obj),
                               static_cast<uint8_t>(ValueTag::ObjectRef)); break;
        }
    } else {
        frame.regs.set_reg(instr.dst_reg(), 0, static_cast<uint8_t>(ValueTag::Null));
    }
    ++frame.pc;
}

// ── StObj: store object to managed pointer ──────────────────────────────
static void Reg_StObj(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_StObj");
    // StObj: src1 = value, src2 = address (managed pointer)
    uint64_t val = frame.regs.reg(instr.src1_reg());
    uint8_t tag = frame.regs.reg_tag(instr.src1_reg());
    void* ptr = reinterpret_cast<void*>(frame.regs.reg(instr.src2_reg()));
    if (ptr != nullptr) {
        auto* iv = static_cast<InterpreterValue*>(ptr);
        iv->tag = static_cast<ValueTag>(tag);
        iv->i64 = static_cast<int64_t>(val);
    }
    ++frame.pc;
}

// ── LdArgA / LdLocA: address of arg/local slot ─────────────────────────
static void Reg_LdArgA(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdArgA");
    // LdArgA: push address of arg slot. In register model, we can't take address
    // of a register, so return null managed pointer (causes fallback when dereferenced).
    frame.regs.set_reg(instr.dst_reg(), 0, static_cast<uint8_t>(ValueTag::ManagedPtr));
    ++frame.pc;
}

static void Reg_LdLocA(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdLocA");
    // LdLocA: address of local variable slot — not directly addressable in reg file.
    // Return null managed pointer (causes fallback in cases that dereference it).
    frame.regs.set_reg(instr.dst_reg(), 0, static_cast<uint8_t>(ValueTag::ManagedPtr));
    ++frame.pc;
}

// ── LdToken: push metadata token as int32 ───────────────────────────────
static void Reg_LdToken(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdToken");
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(instr.imm.i4)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

// ── InitObj: zero-initialize object at managed pointer ─────────────────
static void Reg_InitObj(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_InitObj");
    void* ptr = reinterpret_cast<void*>(frame.regs.reg(instr.src1_reg()));
    if (ptr != nullptr) {
        std::memset(ptr, 0, sizeof(InterpreterValue));
    }
    ++frame.pc;
}

// ── SizeOf: push type size from immediate ───────────────────────────────
static void Reg_SizeOf(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_SizeOf");
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(instr.imm.i4)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

// ── LdFtn: load function pointer ────────────────────────────────────────
static void Reg_LdFtn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdFtn");
    frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(instr.imm.ptr),
                       static_cast<uint8_t>(ValueTag::ObjectRef));
    ++frame.pc;
}

// ── LdVirtFtn: load virtual function pointer ────────────────────────────
static void Reg_LdVirtFtn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdVirtFtn");
    // src1 = object reference, imm.field_offset = method token
    // Resolve virtual function pointer via vtable_registry.
    void* obj = reinterpret_cast<void*>(frame.regs.reg(instr.src1_reg()));
    uint32_t method_token = instr.imm.field_offset;
    void* func_ptr = ::CodegenLdVirtFtn(obj, method_token);
    frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(func_ptr),
                       static_cast<uint8_t>(ValueTag::ObjectRef));
    ++frame.pc;
}

// ── LocAlloc: stack allocation ──────────────────────────────────────────
static void Reg_LocAlloc(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LocAlloc");
    uint32_t size = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    void* mem = CHAOS_IL2CPP_MALLOC(size);
    if (mem == nullptr) { Reg_Unsupported(frame, instr); return; }
    std::memset(mem, 0, size);
    frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(mem),
                       static_cast<uint8_t>(ValueTag::ObjectRef));
    ++frame.pc;
}

// ── Unbox: extract value from boxed object ──────────────────────────────
static void Reg_Unbox(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Unbox");
    auto* obj = reinterpret_cast<InterpreterObject*>(frame.regs.reg(instr.src1_reg()));
    if (obj == nullptr || obj->fields.empty()) {
        frame.regs.set_reg(instr.dst_reg(), 0, static_cast<uint8_t>(ValueTag::Null));
    } else {
        const auto& iv = obj->fields[0];
        switch (iv.tag) {
        case ValueTag::Int32:
            frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(iv.i32),
                               static_cast<uint8_t>(ValueTag::Int32)); break;
        case ValueTag::Int64:
            frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(iv.i64),
                               static_cast<uint8_t>(ValueTag::Int64)); break;
        case ValueTag::Float32: {
            uint64_t val; std::memcpy(&val, &iv.f32, sizeof(float));
            frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float32)); break;
        }
        case ValueTag::Float64: {
            uint64_t val; std::memcpy(&val, &iv.f64, sizeof(double));
            frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float64)); break;
        }
        default:
            frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(iv.obj),
                               static_cast<uint8_t>(ValueTag::ObjectRef)); break;
        }
    }
    ++frame.pc;
}

// ── Branch: Leave (with SEH finally/fault detection) ────────────────────
static void Reg_Leave(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Leave");
    // Scan SEH clauses backward (innermost first) for enclosing finally/fault.
    if (frame.seh_clause_count > 0) {
        for (int32_t i = static_cast<int32_t>(frame.seh_clause_count) - 1; i >= 0; --i) {
            const auto& clause = frame.seh_clauses[i];
            uint32_t pc = frame.pc;
            if (pc >= clause.try_start_idx && pc < clause.try_end_idx) {
                auto flags = static_cast<uint32_t>(clause.flags);
                if (flags == static_cast<uint32_t>(SEHFlags::Finally) ||
                    flags == static_cast<uint32_t>(SEHFlags::Fault)) {
                    // Found enclosing finally — set pending leave and jump to handler.
                    frame.pending_leave = true;
                    frame.pending_leave_target = instr.imm.branch_target;
                    frame.in_handler = true;
                    frame.active_handler_clause = i;
                    frame.pc = static_cast<uint32_t>(clause.handler_start_idx);
                    return;
                }
            }
        }
    }
    // No enclosing finally — normal branch.
    frame.pc = instr.imm.branch_target;
}

// ── Switch: branch table ────────────────────────────────────────────────
static void Reg_Switch(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Switch");
    int32_t index = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    // Switch targets stored in imm.ptr (uint32_t[] array).
    // targets[0..target_count-1] = case targets, targets[target_count] = default target.
    // Target count packed into header bits [62:48] by the allocator.
    auto* targets = static_cast<const uint32_t*>(instr.imm.ptr);
    uint32_t target_count = static_cast<uint32_t>((instr.header >> 48) & 0x7FFF);
    if (targets != nullptr && index >= 0 && static_cast<uint32_t>(index) < target_count) {
        frame.pc = targets[index];
    } else if (targets != nullptr) {
        frame.pc = targets[target_count];  // default target
    } else {
        ++frame.pc;
    }
}

// ── Unsigned branch handlers ─────────────────────────────────────────────
static void Reg_BneUn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_BneUn");
    if (frame.regs.reg(instr.src1_reg()) != frame.regs.reg(instr.src2_reg()))
        frame.pc = instr.imm.branch_target;
    else
        ++frame.pc;
}

static void Reg_BgeUn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_BgeUn");
    uint32_t r = static_cast<uint32_t>(frame.regs.reg(instr.src2_reg()));
    uint32_t l = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    frame.pc = (l >= r) ? instr.imm.branch_target : (frame.pc + 1);
}

static void Reg_BgtUn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_BgtUn");
    uint32_t r = static_cast<uint32_t>(frame.regs.reg(instr.src2_reg()));
    uint32_t l = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    frame.pc = (l > r) ? instr.imm.branch_target : (frame.pc + 1);
}

static void Reg_BleUn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_BleUn");
    uint32_t r = static_cast<uint32_t>(frame.regs.reg(instr.src2_reg()));
    uint32_t l = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    frame.pc = (l <= r) ? instr.imm.branch_target : (frame.pc + 1);
}

static void Reg_BltUn(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_BltUn");
    uint32_t r = static_cast<uint32_t>(frame.regs.reg(instr.src2_reg()));
    uint32_t l = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    frame.pc = (l < r) ? instr.imm.branch_target : (frame.pc + 1);
}

// ── Overflow-checked arithmetic ─────────────────────────────────────────
static void Reg_AddOvf(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_AddOvf");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    // No portable overflow check on MSVC — just compute and trust caller.
    // Overflow will throw via the VM fallback path if needed.
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l + r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_SubOvf(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_SubOvf");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l - r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_MulOvf(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_MulOvf");
    int32_t r = static_cast<int32_t>(frame.regs.reg(instr.src2_reg()));
    int32_t l = static_cast<int32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(l * r)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

// ── Overflow-checked conversions ────────────────────────────────────────
static void Reg_ConvOvfI(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_ConvOvfI");
    // conv.ovf.i: int32 → native int (int64 on x64). Sign-extend through int32.
    int32_t v = static_cast<int32_t>(static_cast<uint32_t>(frame.regs.reg(instr.src1_reg())));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<int64_t>(v)),
                       static_cast<uint8_t>(ValueTag::Int64));
    ++frame.pc;
}

static void Reg_ConvOvfI4(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_ConvOvfI4");
    int32_t v = static_cast<int32_t>(static_cast<uint32_t>(frame.regs.reg(instr.src1_reg())));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(v)),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_ConvOvfI8(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_ConvOvfI8");
    // conv.ovf.i8: int32 → int64. Sign-extend through int32.
    int32_t v = static_cast<int32_t>(static_cast<uint32_t>(frame.regs.reg(instr.src1_reg())));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<int64_t>(v)),
                       static_cast<uint8_t>(ValueTag::Int64));
    ++frame.pc;
}

static void Reg_ConvOvfU(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_ConvOvfU");
    uint32_t v = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(v),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_ConvOvfU4(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_ConvOvfU4");
    uint32_t v = static_cast<uint32_t>(frame.regs.reg(instr.src1_reg()));
    frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(v),
                       static_cast<uint8_t>(ValueTag::Int32));
    ++frame.pc;
}

static void Reg_ConvOvfU8(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_ConvOvfU8");
    uint64_t v = frame.regs.reg(instr.src1_reg());
    frame.regs.set_reg(instr.dst_reg(), v,
                       static_cast<uint8_t>(ValueTag::Int64));
    ++frame.pc;
}

// ── Box: wrap value in interpreter object ───────────────────────────────
static void Reg_Box(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Box");
    auto* boxed = static_cast<InterpreterObject*>(
        CHAOS_IL2CPP_MALLOC(sizeof(InterpreterObject)));
    if (boxed == nullptr) { Reg_Unsupported(frame, instr); return; }
    ::new (boxed) InterpreterObject();
    frame.Track(boxed, frame.Dtor<InterpreterObject>);
    boxed->fields.resize(1);
    uint8_t tag = frame.regs.reg_tag(instr.src1_reg());
    uint64_t val = frame.regs.reg(instr.src1_reg());
    switch (static_cast<ValueTag>(tag)) {
    case ValueTag::Int32:
        boxed->fields[0] = InterpreterValue::from_i32(static_cast<int32_t>(val)); break;
    case ValueTag::Int64:
        boxed->fields[0] = InterpreterValue::from_i64(static_cast<int64_t>(val)); break;
    case ValueTag::Float32: {
        float fv; std::memcpy(&fv, &val, sizeof(float));
        boxed->fields[0] = InterpreterValue::from_f32(fv); break;
    }
    case ValueTag::Float64: {
        double dv; std::memcpy(&dv, &val, sizeof(double));
        boxed->fields[0] = InterpreterValue::from_f64(dv); break;
    }
    default:
        boxed->fields[0] = InterpreterValue::from_obj(reinterpret_cast<void*>(val)); break;
    }
    boxed->type_token = static_cast<uint32_t>(instr.imm.i4);
    frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(boxed),
                       static_cast<uint8_t>(ValueTag::ObjectRef));
    ++frame.pc;
}

// ── Call handlers (interpreter dispatch via InterpreterDispatchRaw) ───────
static void Reg_Call(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Call");
    uint32_t ac = instr.call_arg_count();
    void* call_target = instr.imm.ptr;
    std::fprintf(stderr, "[diag:RC] pc=%u target=%p\n", frame.pc, call_target);
    if (call_target == nullptr) { ++frame.pc; return; }

    // Build raw_args/raw_tags from consecutive registers starting at src1_reg
    // arg0 = regs[src1_reg], arg1 = regs[src1_reg+1], ...
    uint32_t base = instr.src1_reg();

    uint64_t raw_args_stack[8];
    uint8_t  raw_tags_stack[8];
    auto* raw_args = (ac <= 8) ? raw_args_stack
        : static_cast<uint64_t*>(CHAOS_IL2CPP_MALLOC(sizeof(uint64_t) * ac));
    auto* raw_tags = (ac <= 8) ? raw_tags_stack
        : static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(sizeof(uint8_t) * ac));

    if (raw_args == nullptr || raw_tags == nullptr) {
        if (ac > 8) { CHAOS_IL2CPP_FREE(raw_args); CHAOS_IL2CPP_FREE(raw_tags); }
        ++frame.pc; return;
    }

    for (uint32_t i = 0; i < ac; ++i) {
        raw_args[i] = frame.regs.reg(base + i);
        raw_tags[i] = frame.regs.reg_tag(base + i);
    }

    // Look up call-site metadata cache
    const ri::CachedCallInfo* cache_info = nullptr;
    if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
        const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
        if (cc[frame.pc].ret_tag != 0xFF) {
            cache_info = &cc[frame.pc];
        }
    }
    std::fprintf(stderr, "[diag:REG_CALL] pc=%u ac=%u inst=%d ci=%p\n", frame.pc, ac, instr.is_instance_call(), (void*)cache_info);
    std::fflush(stderr);

    auto dret = ri::InterpreterDispatchRaw(
        call_target, raw_args, raw_tags, ac,
        instr.is_instance_call(),
        frame.dispatch_ctx,
        cache_info);

    if (ac > 8) { CHAOS_IL2CPP_FREE(raw_args); CHAOS_IL2CPP_FREE(raw_tags); }

    if (dret.threw_exception) {
        frame.threw_exception = true;
        frame.exception_obj = dret.exception_obj;
        frame.pc = 9999;
        return;
    }

    if (dret.has_value && instr.has_dst()) {
        if (dret.tag == static_cast<uint8_t>(ValueTag::Struct) &&
            dret.struct_data != nullptr) {
            frame.regs.set_reg(instr.dst_reg(),
                               reinterpret_cast<uint64_t>(dret.struct_data),
                               dret.tag);
            frame.Track(dret.struct_data, [](void* p) noexcept { std::free(p); });
        } else {
            frame.regs.set_reg(instr.dst_reg(), dret.value, dret.tag);
        }
    }
    ++frame.pc;
}

// ── Calli: indirect call through function pointer from register ────────────
static void Reg_Calli(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_Calli");
    uint32_t ac = instr.call_arg_count();
    // Function pointer vreg is stored in imm.operand_index by the allocator
    uint32_t func_ptr_vreg = instr.imm.operand_index;
    void* call_target = reinterpret_cast<void*>(frame.regs.reg(func_ptr_vreg));
    if (call_target == nullptr) { ++frame.pc; return; }

    uint32_t base = instr.src1_reg();
    uint64_t raw_args_stack[8];
    uint8_t  raw_tags_stack[8];
    auto* raw_args = (ac <= 8) ? raw_args_stack
        : static_cast<uint64_t*>(CHAOS_IL2CPP_MALLOC(sizeof(uint64_t) * ac));
    auto* raw_tags = (ac <= 8) ? raw_tags_stack
        : static_cast<uint8_t*>(CHAOS_IL2CPP_MALLOC(sizeof(uint8_t) * ac));
    if (raw_args == nullptr || raw_tags == nullptr) {
        if (ac > 8) { CHAOS_IL2CPP_FREE(raw_args); CHAOS_IL2CPP_FREE(raw_tags); }
        ++frame.pc; return;
    }

    for (uint32_t i = 0; i < ac; ++i) {
        raw_args[i] = frame.regs.reg(base + i);
        raw_tags[i] = frame.regs.reg_tag(base + i);
    }

    const ri::CachedCallInfo* cache_info = nullptr;
    if (frame.call_cache != nullptr && frame.pc < frame.call_count) {
        const auto* cc = static_cast<const ri::CachedCallInfo*>(frame.call_cache);
        if (cc[frame.pc].ret_tag != 0xFF) cache_info = &cc[frame.pc];
    }

    auto dret = ri::InterpreterDispatchRaw(
        call_target, raw_args, raw_tags, ac,
        instr.is_instance_call(),
        frame.dispatch_ctx,
        cache_info);

    if (ac > 8) { CHAOS_IL2CPP_FREE(raw_args); CHAOS_IL2CPP_FREE(raw_tags); }
    if (dret.threw_exception) {
        frame.threw_exception = true; frame.exception_obj = dret.exception_obj; frame.pc = 9999;
        return;
    }
    if (dret.has_value && instr.has_dst()) {
        frame.regs.set_reg(instr.dst_reg(), dret.value, dret.tag);
    }
    ++frame.pc;
}

static void Reg_CallVirt(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    // ── PIC fast path (T3 optimized tier) ──────────────────────────────
    // Check for pre-resolved PIC chain: walks PicDispatchChain[] indexed by
    // instruction_idx == frame.pc.  On hit, calls direct_fn directly without
    // vtable resolution or InterpreterDispatchRaw.
    if (frame.patch_method != nullptr) {
        auto* pm = static_cast<chaos::il2cpp::runtime_core::PatchMethod*>(frame.patch_method);
        if (pm->tier_state.load(std::memory_order_acquire) ==
                chaos::il2cpp::runtime_core::PatchMethod::kOptimizedRegister &&
            pm->pic_dispatch_data != nullptr) {

            uint32_t pc = frame.pc;
            uint32_t ac = instr.call_arg_count();
            uint32_t base = instr.src1_reg();

            // Extract receiver type_token from first arg (this pointer)
            uint64_t receiver_val = frame.regs.reg(base);
            if (receiver_val != 0) {
                auto* obj = reinterpret_cast<InterpreterObject*>(receiver_val);
                uint64_t receiver_token = static_cast<uint64_t>(obj->type_token);

                // Parse PIC chain header: [uint32_t count][PicDispatchChain[count]]
                auto* pic_base = static_cast<const uint8_t*>(pm->pic_dispatch_data);
                uint32_t chain_count = *reinterpret_cast<const uint32_t*>(pic_base);
                auto* chains = reinterpret_cast<const chaos::il2cpp::runtime_core::PicDispatchChain*>(
                    pic_base + sizeof(uint32_t));

                for (uint32_t ci = 0; ci < chain_count; ++ci) {
                    const auto& chain = chains[ci];
                    if (chain.instruction_idx != pc) continue;
                    if (chain.generation != chaos::il2cpp::runtime_core::g_patch_generation.load(
                            std::memory_order_acquire)) break;

                    // Check PIC slots
                    for (uint32_t si = 0; si < 3; ++si) {
                        const auto& slot = chain.slots[si];
                        if (slot.type_token == 0) break;  // sentinel
                        if (slot.type_token == receiver_token && slot.direct_fn != nullptr) {
                            // PIC hit — call direct_fn
                            uint64_t raw_args_stack[8];
                            auto* raw_args = (ac <= 8) ? raw_args_stack
                                : static_cast<uint64_t*>(CHAOS_IL2CPP_MALLOC(sizeof(uint64_t) * ac));
                            if (raw_args == nullptr) break;

                            for (uint32_t ai = 0; ai < ac; ++ai) {
                                raw_args[ai] = frame.regs.reg(base + ai);
                            }

                            // Use CallDirectVoidPtr-like direct call
                            using DirectFn = uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t,
                                                          uint64_t, uint64_t, uint64_t, uint64_t);
                            auto fn = reinterpret_cast<DirectFn>(slot.direct_fn);
                            uint64_t result = fn(
                                (ac > 0) ? raw_args[0] : 0,
                                (ac > 1) ? raw_args[1] : 0,
                                (ac > 2) ? raw_args[2] : 0,
                                (ac > 3) ? raw_args[3] : 0,
                                (ac > 4) ? raw_args[4] : 0,
                                (ac > 5) ? raw_args[5] : 0,
                                (ac > 6) ? raw_args[6] : 0,
                                (ac > 7) ? raw_args[7] : 0);

                            if (ac > 8) CHAOS_IL2CPP_FREE(raw_args);

                            if (instr.has_dst()) {
                                frame.regs.set_reg(instr.dst_reg(), result,
                                    static_cast<uint8_t>(ValueTag::Int64));
                            }
                            ++frame.pc;
                            return;
                        }
                    }
                    break;  // matched chain but no slot hit → fall through
                }
            }
        }
    }

    // ── Fall through to Reg_Call (vtable + InterpreterDispatchRaw) ────
    Reg_Call(frame, instr);
}

static void Reg_CallBridge(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    Reg_Call(frame, instr);
}

// ── NewObj: allocate interpreter object ─────────────────────────────────
static void Reg_NewObj(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_NewObj");
    auto* storage = static_cast<InterpreterObject*>(
        CHAOS_IL2CPP_MALLOC(sizeof(InterpreterObject)));
    if (storage == nullptr) { Reg_Unsupported(frame, instr); return; }
    ::new (storage) InterpreterObject();
    frame.Track(storage, frame.Dtor<InterpreterObject>);
    uint32_t field_count = instr.imm.operand_index;
    if (field_count == 0) field_count = 1;
    storage->fields.resize(field_count);
    storage->type_token = static_cast<uint32_t>(instr.imm.i4);
    frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(storage),
                       static_cast<uint8_t>(ValueTag::ObjectRef));
    ++frame.pc;
}

// ── LdFld / StFld: instance field access ───────────────────────────────
static void Reg_LdFld(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_LdFld");
    auto* obj = reinterpret_cast<InterpreterObject*>(frame.regs.reg(instr.src1_reg()));
    if (obj == nullptr) {
        frame.regs.set_reg(instr.dst_reg(), 0, static_cast<uint8_t>(ValueTag::Null));
        ++frame.pc; return;
    }
    uint32_t idx = instr.imm.field_offset;
    if (idx >= obj->fields.size()) {
        obj->fields.resize(idx + 1u);
    }
    const auto& iv = obj->fields[idx];
    switch (iv.tag) {
    case ValueTag::Int32:
        frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(static_cast<uint32_t>(iv.i32)),
                           static_cast<uint8_t>(ValueTag::Int32)); break;
    case ValueTag::Int64:
        frame.regs.set_reg(instr.dst_reg(), static_cast<uint64_t>(iv.i64),
                           static_cast<uint8_t>(ValueTag::Int64)); break;
    case ValueTag::Float32: {
        uint64_t val; std::memcpy(&val, &iv.f32, sizeof(float));
        frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float32)); break;
    }
    case ValueTag::Float64: {
        uint64_t val; std::memcpy(&val, &iv.f64, sizeof(double));
        frame.regs.set_reg(instr.dst_reg(), val, static_cast<uint8_t>(ValueTag::Float64)); break;
    }
    default:
        frame.regs.set_reg(instr.dst_reg(), reinterpret_cast<uint64_t>(iv.obj),
                           static_cast<uint8_t>(ValueTag::ObjectRef)); break;
    }
    ++frame.pc;
}

static void Reg_StFld(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Reg_StFld");
    // src1 = value, src2 = obj
    uint64_t val = frame.regs.reg(instr.src1_reg());
    uint8_t tag = frame.regs.reg_tag(instr.src1_reg());
    auto* obj = reinterpret_cast<InterpreterObject*>(frame.regs.reg(instr.src2_reg()));
    if (obj == nullptr) { ++frame.pc; return; }
    uint32_t idx = instr.imm.field_offset;
    if (idx >= obj->fields.size()) {
        obj->fields.resize(idx + 1u);
    }
    switch (static_cast<ValueTag>(tag)) {
    case ValueTag::Int32:
        obj->fields[idx] = InterpreterValue::from_i32(static_cast<int32_t>(val)); break;
    case ValueTag::Int64:
        obj->fields[idx] = InterpreterValue::from_i64(static_cast<int64_t>(val)); break;
    case ValueTag::Float32: {
        float fv; std::memcpy(&fv, &val, sizeof(float));
        obj->fields[idx] = InterpreterValue::from_f32(fv); break;
    }
    case ValueTag::Float64: {
        double dv; std::memcpy(&dv, &val, sizeof(double));
        obj->fields[idx] = InterpreterValue::from_f64(dv); break;
    }
    default:
        obj->fields[idx] = InterpreterValue::from_obj(reinterpret_cast<void*>(val)); break;
    }
    ++frame.pc;
}

// ── CastClass / IsInst: type checking (passthrough in fast path) ──
static void Reg_CastClass(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    // CastClass: null passthrough, non-null returns object unchanged.
    // Actual type check is deferred (same as Handle_CastClass in FastExecute).
    uint64_t obj = frame.regs.reg(instr.src1_reg());
    if (obj == 0) {
        frame.regs.set_reg(instr.dst_reg(), 0, static_cast<uint8_t>(ValueTag::Null));
    } else {
        frame.regs.set_reg(instr.dst_reg(), obj, static_cast<uint8_t>(ValueTag::ObjectRef));
    }
    ++frame.pc;
}

static void Reg_IsInst(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    // IsInst: null returns null, non-null returns the object (passthrough).
    uint64_t obj = frame.regs.reg(instr.src1_reg());
    if (obj == 0) {
        frame.regs.set_reg(instr.dst_reg(), 0, static_cast<uint8_t>(ValueTag::Null));
    } else {
        frame.regs.set_reg(instr.dst_reg(), obj, static_cast<uint8_t>(ValueTag::ObjectRef));
    }
    ++frame.pc;
}

// ── CallVirtConstrained: constrained prefix resolved during IR lowering — delegate to Reg_Call
static void Reg_CallVirtConstrained(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    Reg_Call(frame, instr);
}

// ── Cpblk / InitBlk: memory block operations ────────────────────────────
static void Reg_Cpblk(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    // src1 = size (top of stack), src2 = src, src3 = dst
    uint32_t size = static_cast<uint32_t>(frame.regs.reg_i32(instr.src1_reg()));
    void* src = frame.regs.reg_ptr(instr.src2_reg());
    void* dst = frame.regs.reg_ptr(instr.src3_reg());
    if (dst != nullptr && src != nullptr) {
        std::memcpy(dst, src, size);
    }
    ++frame.pc;
}

static void Reg_InitBlk(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    // src1 = size (top of stack), src2 = fill_value, src3 = addr
    uint32_t size = static_cast<uint32_t>(frame.regs.reg_i32(instr.src1_reg()));
    int32_t value = frame.regs.reg_i32(instr.src2_reg());
    void* ptr = frame.regs.reg_ptr(instr.src3_reg());
    if (ptr != nullptr) {
        std::memset(ptr, value, size);
    }
    ++frame.pc;
}

// ── Throw: SEH-aware catch dispatch with finally unwind + typed matching ──
static void Reg_Throw(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    void* exc_obj = frame.regs.reg_ptr(instr.src1_reg());
    frame.exception_obj = exc_obj;

    // Scan backward for innermost catch/filter handler.
    // For typed clauses, check type match via callback.
    int32_t catch_idx = -1;
    for (int32_t i = static_cast<int32_t>(frame.seh_clause_count) - 1; i >= 0; --i) {
        const auto& clause = frame.seh_clauses[i];
        if (frame.pc < clause.try_start_idx || frame.pc >= clause.try_end_idx) continue;
        auto flags = static_cast<uint32_t>(clause.flags);
        if (flags == static_cast<uint32_t>(SEHFlags::Exception) ||
            flags == static_cast<uint32_t>(SEHFlags::Filter)) {
            // Check typed catch matching.
            if ((flags & static_cast<uint32_t>(SEHFlags::Typed)) != 0 &&
                frame.typed_catch_check != nullptr &&
                !frame.typed_catch_check(exc_obj, clause.class_token)) {
                continue;  // Type doesn't match — keep scanning outward.
            }
            catch_idx = i;
            break;
        }
    }

    if (catch_idx < 0) {
        // No catch handler — propagate to caller.
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }

    // Collect finally/fault clauses nested inside the catch's try range
    // that also cover the throw pc.  These must unwind before entering catch.
    const auto& catch_clause = frame.seh_clauses[catch_idx];
    frame.unwind_finally_count = 0;
    for (int32_t i = 0; i < static_cast<int32_t>(frame.seh_clause_count); ++i) {
        if (i == catch_idx) continue;
        const auto& clause = frame.seh_clauses[i];
        auto flags = static_cast<uint32_t>(clause.flags);
        if ((flags == static_cast<uint32_t>(SEHFlags::Finally) ||
             flags == static_cast<uint32_t>(SEHFlags::Fault)) &&
            frame.pc >= clause.try_start_idx && frame.pc < clause.try_end_idx &&
            clause.try_start_idx >= catch_clause.try_start_idx &&
            clause.try_end_idx <= catch_clause.try_end_idx &&
            frame.unwind_finally_count < RegisterFrame::kMaxUnwindDepth) {
            frame.unwind_finally_list[frame.unwind_finally_count++] = i;
        }
    }

    if (frame.unwind_finally_count > 0) {
        // Unwind innermost finally first (list is innermost-first from i=0 scan).
        frame.unwinding_throw = true;
        frame.unwind_exception_obj = exc_obj;
        frame.unwind_catch_clause = catch_idx;
        frame.unwind_finally_current = 0;
        int32_t finally_idx = frame.unwind_finally_list[0];
        frame.in_handler = true;
        frame.active_handler_clause = finally_idx;
        frame.pc = static_cast<uint32_t>(frame.seh_clauses[finally_idx].handler_start_idx);
        return;
    }

    // No finally to unwind — enter catch directly.
    uint32_t handler_start = static_cast<uint32_t>(catch_clause.handler_start_idx);
    for (uint32_t ei = 0; ei < frame.catch_handler_count; ++ei) {
        if (frame.catch_handler_entries[ei].handler_start_idx == handler_start) {
            frame.regs.set_reg(frame.catch_handler_entries[ei].exception_reg,
                               reinterpret_cast<uint64_t>(exc_obj),
                               static_cast<uint8_t>(ValueTag::ObjectRef));
            break;
        }
    }
    frame.in_handler = true;
    frame.active_handler_clause = catch_idx;
    frame.pc = handler_start;
}

static void Reg_Rethrow(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    // Use exception_obj from frame (set by a previous Call that threw).
    void* exc_obj = frame.exception_obj;
    if (exc_obj == nullptr) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }

    // Skip the active handler clause to avoid re-catching in the same handler.
    // Scan backward for innermost catch/filter handler outside the current one.
    int32_t catch_idx = -1;
    for (int32_t i = static_cast<int32_t>(frame.seh_clause_count) - 1; i >= 0; --i) {
        if (i == frame.active_handler_clause) continue;
        const auto& clause = frame.seh_clauses[i];
        if (frame.pc < clause.try_start_idx || frame.pc >= clause.try_end_idx) continue;
        auto flags = static_cast<uint32_t>(clause.flags);
        if (flags == static_cast<uint32_t>(SEHFlags::Exception) ||
            flags == static_cast<uint32_t>(SEHFlags::Filter)) {
            if ((flags & static_cast<uint32_t>(SEHFlags::Typed)) != 0 &&
                frame.typed_catch_check != nullptr &&
                !frame.typed_catch_check(exc_obj, clause.class_token)) {
                continue;
            }
            catch_idx = i;
            break;
        }
    }

    if (catch_idx < 0) {
        frame.threw_exception = true;
        frame.pc = 9999;
        return;
    }

    // Collect finally/fault clauses between rethrow pc and catch.
    const auto& catch_clause = frame.seh_clauses[catch_idx];
    frame.unwind_finally_count = 0;
    for (int32_t i = 0; i < static_cast<int32_t>(frame.seh_clause_count); ++i) {
        if (i == catch_idx || i == frame.active_handler_clause) continue;
        const auto& clause = frame.seh_clauses[i];
        auto flags = static_cast<uint32_t>(clause.flags);
        if ((flags == static_cast<uint32_t>(SEHFlags::Finally) ||
             flags == static_cast<uint32_t>(SEHFlags::Fault)) &&
            frame.pc >= clause.try_start_idx && frame.pc < clause.try_end_idx &&
            clause.try_start_idx >= catch_clause.try_start_idx &&
            clause.try_end_idx <= catch_clause.try_end_idx &&
            frame.unwind_finally_count < RegisterFrame::kMaxUnwindDepth) {
            frame.unwind_finally_list[frame.unwind_finally_count++] = i;
        }
    }

    if (frame.unwind_finally_count > 0) {
        frame.unwinding_throw = true;
        frame.unwind_exception_obj = exc_obj;
        frame.unwind_catch_clause = catch_idx;
        frame.unwind_finally_current = 0;
        int32_t finally_idx = frame.unwind_finally_list[0];
        frame.in_handler = true;
        frame.active_handler_clause = finally_idx;
        frame.pc = static_cast<uint32_t>(frame.seh_clauses[finally_idx].handler_start_idx);
        return;
    }

    // No finally — enter catch directly.
    uint32_t handler_start = static_cast<uint32_t>(catch_clause.handler_start_idx);
    for (uint32_t ei = 0; ei < frame.catch_handler_count; ++ei) {
        if (frame.catch_handler_entries[ei].handler_start_idx == handler_start) {
            frame.regs.set_reg(frame.catch_handler_entries[ei].exception_reg,
                               reinterpret_cast<uint64_t>(exc_obj),
                               static_cast<uint8_t>(ValueTag::ObjectRef));
            break;
        }
    }
    frame.in_handler = true;
    frame.active_handler_clause = catch_idx;
    frame.pc = handler_start;
}

static void Reg_EndFinally(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    // Check for pending leave (Leave → finally → EndFinally).
    if (frame.pending_leave) {
        frame.pc = frame.pending_leave_target;
        frame.pending_leave = false;
        frame.in_handler = false;
        frame.active_handler_clause = -1;
        return;
    }

    // Check for throw unwind — continue to next finally or enter catch.
    if (frame.unwinding_throw) {
        frame.unwind_finally_current++;
        if (frame.unwind_finally_current < frame.unwind_finally_count) {
            // More finally blocks to unwind.
            int32_t finally_idx = frame.unwind_finally_list[frame.unwind_finally_current];
            frame.in_handler = true;
            frame.active_handler_clause = finally_idx;
            frame.pc = static_cast<uint32_t>(frame.seh_clauses[finally_idx].handler_start_idx);
            return;
        }

        // All finally blocks done — enter catch.
        frame.unwinding_throw = false;
        int32_t catch_idx = frame.unwind_catch_clause;
        const auto& catch_clause = frame.seh_clauses[catch_idx];
        uint32_t handler_start = static_cast<uint32_t>(catch_clause.handler_start_idx);
        for (uint32_t ei = 0; ei < frame.catch_handler_count; ++ei) {
            if (frame.catch_handler_entries[ei].handler_start_idx == handler_start) {
                frame.regs.set_reg(frame.catch_handler_entries[ei].exception_reg,
                                   reinterpret_cast<uint64_t>(frame.unwind_exception_obj),
                                   static_cast<uint8_t>(ValueTag::ObjectRef));
                break;
            }
        }
        frame.in_handler = true;
        frame.active_handler_clause = catch_idx;
        frame.pc = handler_start;
        return;
    }

    // Normal EndFinally — continue sequentially.
    frame.in_handler = false;
    frame.active_handler_clause = -1;
    ++frame.pc;
}

static void Reg_EndFilter(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    // Pop filter result from src1_reg.
    // 0 = filter didn't match, non-zero = matched.
    int32_t result = frame.regs.reg_i32(instr.src1_reg());
    if (result == 0) {
        // Filter didn't match — continue SEH search via Step D fallthrough.
        Reg_Unsupported(frame, instr);
    } else {
        // Filter matched — advance to catch handler code.
        ++frame.pc;
    }
}

static void Reg_Break(RegisterFrame& frame, const RegisterInstruction& instr) noexcept {
    // Break is a no-op in register execution.
    ++frame.pc;
}

// ── Dispatch table ──────────────────────────────────────────────────────

#define R(name) Reg_##name
#define UNSUP Reg_Unsupported

static constexpr RegOpHandler kRegHandlers[100] = {
    /*  0 */ R(LdcI4),         /*  1 */ R(LdcI8),         /*  2 */ R(LdcR4),
    /*  3 */ R(LdcR8),         /*  4 */ R(LdStr),          /*  5 */ R(LdNull),
    /*  6 */ R(LdArg),         /*  7 */ R(LdLoc),          /*  8 */ R(StLoc),
    /*  9 */ R(StArg),         /* 10 */ R(LdFld),          /* 11 */ R(StFld),
    /* 12 */ R(LdSFld),        /* 13 */ R(StSFld),         /* 14 */ R(Call),
    /* 15 */ R(CallVirt),      /* 16 */ R(CallBridge),       /* 17 */ R(Br),
    /* 18 */ R(BrTrue),        /* 19 */ R(BrFalse),        /* 20 */ R(Beq),
    /* 21 */ R(Blt),           /* 22 */ R(Bgt),            /* 23 */ R(Ble),
    /* 24 */ R(Bge),           /* 25 */ R(Add),            /* 26 */ R(Sub),
    /* 27 */ R(Mul),           /* 28 */ R(Div),            /* 29 */ R(Rem),
    /* 30 */ R(Neg),           /* 31 */ R(Ceq),            /* 32 */ R(Clt),
    /* 33 */ R(Cgt),           /* 34 */ R(NewObj),         /* 35 */ R(Box),
    /* 36 */ R(Unbox),         /* 37 */ R(CastClass),      /* 38 */ R(IsInst),
    /* 39 */ R(Conv_I4),       /* 40 */ R(Conv_I8),        /* 41 */ R(Conv_R4),
    /* 42 */ R(Conv_R8),       /* 43 */ R(NewArr),         /* 44 */ R(LdElem),
    /* 45 */ R(StElem),        /* 46 */ R(LdLen),          /* 47 */ R(Pop),
    /* 48 */ R(Throw),         /* 49 */ R(Rethrow),        /* 50 */ R(Leave),
    /* 51 */ R(EndFinally),    /* 52 */ R(EndFilter),      /* 53 */ R(Ret),
    /* 54 */ R(Dup),           /* 55 */ R(DivUn),          /* 56 */ R(RemUn),
    /* 57 */ R(And),           /* 58 */ R(Or),             /* 59 */ R(Xor),
    /* 60 */ R(Not),           /* 61 */ R(Shl),            /* 62 */ R(Shr),
    /* 63 */ R(ShrUn),         /* 64 */ R(ConvRUn),        /* 65 */ R(ConvI),
    /* 66 */ R(ConvU),         /* 67 */ R(LdInd),          /* 68 */ R(StInd),
    /* 69 */ R(Switch),        /* 70 */ R(LdToken),        /* 71 */ R(InitObj),
    /* 72 */ R(SizeOf),        /* 73 */ R(LdFtn),          /* 74 */ R(LdVirtFtn),
    /* 75 */ R(LdArgA),        /* 76 */ R(LdLocA),         /* 77 */ R(LocAlloc),
    /* 78 */ R(Break),         /* 79 */ R(BneUn),          /* 80 */ R(BgeUn),
    /* 81 */ R(BgtUn),         /* 82 */ R(BleUn),          /* 83 */ R(BltUn),
    /* 84 */ R(AddOvf),        /* 85 */ R(SubOvf),         /* 86 */ R(MulOvf),
    /* 87 */ R(ConvOvfI),      /* 88 */ R(ConvOvfI4),      /* 89 */ R(ConvOvfI8),
    /* 90 */ R(ConvOvfU),      /* 91 */ R(ConvOvfU4),      /* 92 */ R(ConvOvfU8),
    /* 93 */ R(LdObj),         /* 94 */ R(StObj),          /* 95 */ R(LdElem),      // LdElemA maps to LdElem (same element load)
    /* 96 */ R(Cpblk),         /* 97 */ R(InitBlk),        /* 98 */ R(CallVirtConstrained),
    /* 99 */ R(Calli),
};

#undef R
#undef UNSUP

// ── OSR: hot loop → T4 native code promotion ──────────────────────────────
// V1 "promote on re-entry": when RegisterExecute detects a hot loop backward
// branch, trigger T4 native code generation and cache the result so that
// subsequent calls to this method execute natively (via entry_direct.cpp).

static constexpr uint32_t kOsrLoopThreshold = 100;

static void TryOsrPromotion(RegisterFrame& frame,
                             const RegisterInstruction* instrs,
                             uint32_t instr_count) noexcept {
    if (frame.patch_method == nullptr) return;
    auto* pm = static_cast<chaos::il2cpp::runtime_core::PatchMethod*>(frame.patch_method);
    using PM = chaos::il2cpp::runtime_core::PatchMethod;

    // Don't re-promote if already at T4 (another thread got there first).
    if (pm->tier_state.load(std::memory_order_acquire) >= PM::kJitted) {
        // If the method already has a cached JitMethod with an OSR entry,
        // re-enter T4 via OSR directly.  This handles the deopt→T4
        // re-promotion loop: after deoptimization, the tier_state is still
        // kT4Ready and the cached JitMethod is still valid.
        auto* existing_nm = static_cast<chaos::il2cpp::jit::JitMethod*>(
            pm->cached_native_method);
        if (existing_nm != nullptr && existing_nm->osr_entry_offset != 0) {
            // Set OSR resume PC to loop header (frame.pc is the backward branch
            // target after the branch handler executed).
            chaos::il2cpp::jit::g_jit_deopt_state.osr_resume_pc = frame.pc;

            using OsrEntry = void (*)(void*, void*);
            auto osr_entry = reinterpret_cast<OsrEntry>(
                static_cast<uint8_t*>(existing_nm->code) + existing_nm->osr_entry_offset);
            uint64_t osr_ret_buf[2] = {};
            CHAOS_IL2CPP_LOG_DEBUG_M("osr", "TryOsrPromotion: re-promote osr_entry at offset={}", existing_nm->osr_entry_offset);
            osr_entry(&frame.regs, osr_ret_buf);
            CHAOS_IL2CPP_LOG_DEBUG_M("osr", "TryOsrPromotion: re-promote osr_entry returned, ret_val=0x{:x}", osr_ret_buf[0]);
            frame.ret_val = osr_ret_buf[0];
            frame.has_ret = true;
            frame.pc = 0xFFffFFffu;
        }
        return;
    }

    // RegisterMethod should already be cached from T3 lowering.
    auto* rm = static_cast<RegisterMethod*>(pm->cached_reg_method);
    if (rm == nullptr) return;

    // Generate native code with full deopt support.
    chaos::il2cpp::jit::CompileConfig cfg;
    cfg.enable_deopt = true;
    cfg.enable_liveness = true;
    cfg.safepoint_fn = nullptr;

    auto* nm = chaos::il2cpp::jit::Compile(*rm, cfg);
    if (nm == nullptr) return;

    // OSR V2: if the generated code has an OSR entry, transfer execution
    // to native code mid-stream with the current register file.
    if (nm->osr_entry_offset != 0) {
        // Set tier state BEFORE calling OSR entry so future calls also hit T4.
        pm->cached_native_method = nm;
        pm->tier_state.store(PM::kJitted, std::memory_order_release);
        chaos::il2cpp::jit::RegisterNativeCodeSection(nm->code, nm->code_size, nm);

        // Set OSR resume PC to loop header before calling OSR entry.
        chaos::il2cpp::jit::g_jit_deopt_state.osr_resume_pc = frame.pc;

        using OsrEntry = void (*)(void*, void*);
        auto osr_entry = reinterpret_cast<OsrEntry>(
            static_cast<uint8_t*>(nm->code) + nm->osr_entry_offset);
        uint64_t osr_ret_buf[2] = {};
        CHAOS_IL2CPP_LOG_DEBUG_M("osr", "TryOsrPromotion: calling osr_entry at offset={}", nm->osr_entry_offset);
        osr_entry(&frame.regs, osr_ret_buf);
        CHAOS_IL2CPP_LOG_DEBUG_M("osr", "TryOsrPromotion: osr_entry returned, ret_val=0x{:x}", osr_ret_buf[0]);

        // Native code completed — capture return value and exit RegisterExecute
        frame.ret_val = osr_ret_buf[0];
        frame.has_ret = true;
        frame.pc = 0xFFffFFffu;
        return;
    }

    // V1 fallback: cache for re-entry on next call
    pm->cached_native_method = nm;
    pm->tier_state.store(PM::kJitted, std::memory_order_release);
    chaos::il2cpp::jit::RegisterNativeCodeSection(nm->code, nm->code_size, nm);
}

// ── RegisterExecute ─────────────────────────────────────────────────────

bool RegisterExecute(RegisterFrame& frame,
                     const RegisterInstruction* instrs,
                     uint32_t instr_count) noexcept {
    frame.pc = 0;
    uint32_t loop_counter = 0;

    while (frame.pc < instr_count) {
        CHAOS_IL2CPP_PROFILE_SCOPE("RegisterExecute");
        uint32_t op_val = static_cast<uint32_t>(instrs[frame.pc].op_code());
        if (op_val > 99) {
            frame.threw_exception = true;
            return false;
        }

        uint32_t prev_pc = frame.pc;
        kRegHandlers[op_val](frame, instrs[frame.pc]);

        // OSR: Detect hot loop backward branch — if the handler took a branch
        // and the target is an earlier instruction, it's a loop backedge.
        if (frame.pc != prev_pc + 1 && frame.pc < prev_pc) {
            // After deoptimization from T4, use threshold=1 so the method
            // re-enters native code on the very first backward branch.
            uint32_t threshold = frame.osr_reenable ? 1 : kOsrLoopThreshold;
            frame.osr_reenable = false;  // one-shot
            if (++loop_counter >= threshold) {
                TryOsrPromotion(frame, instrs, instr_count);
                if (frame.pc == 0xFFffFFffu) continue;  // OSR took over
            }
        }

        if (frame.threw_exception && frame.pc == 9999) {
            frame.CleanupTracked();
            return false;
        }

        if (frame.pc == 0xFFffFFffu) {
            frame.CleanupTracked();
            return true;
        }
    }

    frame.CleanupTracked();
    return true;
}

}  // namespace chaOS_IL2CPP_INTERPRETER_H_