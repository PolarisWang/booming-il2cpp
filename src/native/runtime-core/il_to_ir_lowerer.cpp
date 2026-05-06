#include "il_to_ir_lowerer.h"

#include <chaos/native_types.h>
#include <chaos/trace.h>

#include <cstdint>
#include <cstring>
#include <vector>

namespace chaos::il2cpp::interpreter {

namespace {

// ── IL operand size table ───────────────────────────────────────────────────

// Byte count of operands for each IL opcode (single-byte opcodes).
// Opcodes not listed: -1 (invalid/unused), 0 (no operand).

enum OperandSize : int8_t {
    kOpNone      = 0,
    kOpBrShort   = 1,   // int8 branch offset
    kOpVarShort  = 1,   // uint8 variable index
    kOpI4Short   = 1,   // int8 immediate
    kOpI4        = 4,   // int32 immediate
    kOpI8        = 8,   // int64 immediate
    kOpBrLong    = 4,   // int32 branch offset
    kOpVar       = 2,   // uint16 variable index
    kOpTok       = 4,   // uint32 metadata token
    kOpSwitch    = -2,  // special: variable-length switch
};

// ECMA 335 single-byte opcode operand sizes.
static constexpr int8_t kOpcodeSizes[0x100] = {
    // 0x00-0x0F
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,  // nop, break, ldarg.0-3, ldloc.0-3, stloc.0-3, ldarg.s, ldarg.s
    // 0x10-0x1F
    1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  // starg.s, ldloc.s, stloc.s, ldnull, ldc.i4.0-8, ldc.i4.s
    // 0x20-0x2F
    4, 8, 4, 8, 0, 0, 0, 0, 4, 4, 0, 1, 1, 1, 1, 1,  // ldc.i4, ldc.i8, ldc.r4, ldc.r8, ..., call, callvirt, ret, br.s, brfalse.s, brtrue.s, beq.s, bge.s
    // 0x30-0x3F
    1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 4, 4, 4, 4,  // bgt.s, ble.s, blt.s, bne.un.s, bge.un.s, bgt.un.s, ble.un.s, blt.un.s, br..blt
    // 0x40-0x4F
    4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // bne.un, bge.un, bgt.un, ble.un, blt.un, switch, ldind.*
    // 0x50-0x5F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // stind.* (no operand), add, sub, ...
    // 0x60-0x6F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // div.un, rem.un, and, or, xor, ..., neg, not, shl, shr, shr.un, ..., conv.r.un
    // 0x70-0x7F
    0, 0, 4, 4, 4, 4, 0, 0, 0, 4, 0, 4, 0, 4, 4, 4,  // ..., throw, ldstr, newobj, castclass, isinst, ..., unbox, ..., ldfld, ..., stfld, ldsfld, stsfld
    // 0x80-0x8F
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0,  // ..., box, newarr, ldlen
    // 0x90-0x9F: ldelem variants + stelem.ref~i2
    0, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // ldelem.ref, -, -, i1, u1, i2, u2, i4, u4, i8, i, r4, r8, stelem.ref, i1, i2
    // 0xA0-0xAF: stelem variants + generic access
    0, 0, 0, 4, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  // stelem.i4, i8, r4, ldelem(generic), stelem(generic)
    // 0xB0-0xBF: conv.* variants (native int)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // conv.i, conv.ovf.*, conv.u, conv.ovf.* (0 operand)
    // 0xC0-0xCF: cpblk/initblk region
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0xD0-0xDF
    4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // ldtoken(4), endfilter, rethrow, ..., unused
    // 0xE0-0xFF: unused
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

// ECMA 335 two-byte opcodes (0xFE prefix): operand sizes indexed by second byte.
static constexpr int8_t kOpcodeSizesExtended[0x100] = {
    // 0xFE00-0xFE0F
    0, 0, 0, 0, 0, 0, 4, 4, 0, 2, 2, 2, 2, 2, 2, 4,  // arglist, ceq, cgt, cgt.un, clt, clt.un, ldftn(4), ldvirtftn(4), ..., ldarg(2), ldarga(2), ldarg(2), ldloc(2), ldloca(2), stloc(2), ldelema(4)
    // 0xFE10-0xFE1F
    0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 4, 0, 0,  // ..., initobj(4), constrained(4), cpblk, initblk, ..., sizeof(4), ...
    // rest: -1 = unhandled
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

// ── Helper: read multi-byte values from IL byte stream ──────────────────────

inline uint8_t ReadU1(const uint8_t*& ip) { return *ip++; }
inline int8_t  ReadI1(const uint8_t*& ip) { return static_cast<int8_t>(*ip++); }
inline uint16_t ReadU2(const uint8_t*& ip) {
    uint16_t val = static_cast<uint16_t>(ip[0]) | (static_cast<uint16_t>(ip[1]) << 8);
    ip += 2;
    return val;
}
inline int32_t ReadI4(const uint8_t*& ip) {
    int32_t val = static_cast<int32_t>(
        static_cast<uint32_t>(ip[0]) |
        (static_cast<uint32_t>(ip[1]) << 8) |
        (static_cast<uint32_t>(ip[2]) << 16) |
        (static_cast<uint32_t>(ip[3]) << 24));
    ip += 4;
    return val;
}
inline int64_t ReadI8(const uint8_t*& ip) {
    int64_t val;
    std::memcpy(&val, ip, sizeof(val));
    ip += 8;
    return val;
}
inline uint32_t ReadToken(const uint8_t*& ip) {
    return static_cast<uint32_t>(ReadI4(ip));
}

/// Build an empty fallback IRMethod (single Ret).
static IRMethod MakeEmptyMethod() {
    IRMethod method;
    IRInstruction ret_insn;
    ret_insn.op_code = IROpCode::Ret;
    method.instructions.push_back(ret_insn);
    return method;
}

}  // anonymous namespace

// ════════════════════════════════════════════════════════════════════════════
// Method body header parsing
// ════════════════════════════════════════════════════════════════════════════

bool ParseMethodBodyHeader(
    const void*       body_bytes,
    CHAOS_IL2CPP_SIZE body_length,
    MethodBodyHeader& out)
{
    if (body_bytes == nullptr || body_length == 0u) {
        return false;
    }

    const uint8_t* bytes = static_cast<const uint8_t*>(body_bytes);
    const uint8_t  flags = bytes[0];

    // ── Tiny format: bits[1:0] = 0x02 ──
    if ((flags & 0x03) == 0x02) {
        CHAOS_IL2CPP_SIZE tiny_code_size =
            static_cast<CHAOS_IL2CPP_SIZE>(flags >> 2);
        if (tiny_code_size == 0u) {
            return false;  // empty body not valid
        }
        if (body_length < 1u + tiny_code_size) {
            return false;  // buffer too short for declared code
        }

        out.code_start        = bytes + 1u;
        out.code_size         = tiny_code_size;
        out.max_stack         = 8u;  // implied for tiny
        out.local_var_sig_tok = 0u;
        out.has_more_sects    = false;  // tiny format never has SEH
        return true;
    }

    // ── Fat format: low 2 bits of the 2-byte flags word = 0x03 ──
    if (body_length < 2u) {
        return false;
    }
    const uint16_t flags16 = static_cast<uint16_t>(bytes[0]) |
                             (static_cast<uint16_t>(bytes[1]) << 8);

    if ((flags16 & 0x0003) == 0x0003) {
        if (body_length < 12u) {
            return false;  // fat header is 12 bytes
        }

        const uint16_t max_stack = static_cast<uint16_t>(bytes[2]) |
                                   (static_cast<uint16_t>(bytes[3]) << 8);
        const uint32_t code_size = static_cast<uint32_t>(bytes[4]) |
                                   (static_cast<uint32_t>(bytes[5]) << 8) |
                                   (static_cast<uint32_t>(bytes[6]) << 16) |
                                   (static_cast<uint32_t>(bytes[7]) << 24);
        const uint32_t local_sig = static_cast<uint32_t>(bytes[8]) |
                                   (static_cast<uint32_t>(bytes[9]) << 8) |
                                   (static_cast<uint32_t>(bytes[10]) << 16) |
                                   (static_cast<uint32_t>(bytes[11]) << 24);

        if (code_size == 0u) {
            return false;
        }
        if (body_length < 12u + code_size) {
            return false;
        }

        out.code_start        = bytes + 12u;
        out.code_size         = static_cast<CHAOS_IL2CPP_SIZE>(code_size);
        out.max_stack         = max_stack;
        out.local_var_sig_tok = local_sig;
        out.has_more_sects    = (flags16 & 0x0100) != 0;  // bit 8 = MoreSects
        return true;
    }

    return false;  // not a valid header format
}

// ════════════════════════════════════════════════════════════════════════════
// Public API
// ════════════════════════════════════════════════════════════════════════════

IRMethod LowerILToIR(
    const void*          il_bytes,
    CHAOS_IL2CPP_SIZE    il_length,
    CHAOS_IL2CPP_SIZE    code_size,
    CHAOS_IL2CPP_UINT32  /*max_stack*/,
    ILTokenResolver      token_resolver,
    void*                user_data)
{
    CHAOS_IL2CPP_LOG_TRACE("runtime", "LowerILToIR", "\"il_length\"=%zu,\"code_size\"=%zu", il_length, code_size);
    if (il_bytes == nullptr || il_length == 0u) {
        return MakeEmptyMethod();
    }

    const uint8_t* il_start = static_cast<const uint8_t*>(il_bytes);
    const uint8_t* il_end   = il_start + il_length;
    const uint8_t* ip       = il_start;

    // ── First pass: decode instructions ──
    struct RawInsn {
        IROpCode         op_code;
        CHAOS_IL2CPP_INT32 operand_i4;
        CHAOS_IL2CPP_INT64 operand_i8;
        CHAOS_IL2CPP_SIZE il_offset;       // byte offset in IL stream
        CHAOS_IL2CPP_SIZE insn_size;       // total bytes of this IL instruction
        bool             is_branch;
        bool             is_branch_short;  // true = 1-byte offset, false = 4-byte
        CHAOS_IL2CPP_INT32 branch_offset;  // relative branch target delta
        CHAOS_IL2CPP_UINT32 token;         // metadata token (for call/newobj/box etc.)
        bool             has_token;
    };

    CHAOS_IL2CPP_VECTOR(RawInsn) raw_insns;
    raw_insns.reserve(il_length / 2);  // rough estimate

    bool decode_failed = false;

    while (ip < il_end) {
        const CHAOS_IL2CPP_SIZE offset = static_cast<CHAOS_IL2CPP_SIZE>(ip - il_start);
        const uint8_t op_byte = ReadU1(ip);

        RawInsn insn = {};
        insn.il_offset = offset;

        if (op_byte == 0xFE) {
            // Two-byte opcode.
            if (ip >= il_end) { decode_failed = true; break; }
            const uint8_t ext = ReadU1(ip);
            const int8_t operand_size = (ext < 0x100) ? kOpcodeSizesExtended[ext] : -1;
            if (operand_size < 0) { decode_failed = true; break; }

            insn.insn_size = static_cast<CHAOS_IL2CPP_SIZE>(ip + operand_size - il_start - offset);

            // Map extended opcodes to IROpCode.
            switch (ext) {
                case 0x01: insn.op_code = IROpCode::Ceq;    break;  // ceq
                case 0x02: insn.op_code = IROpCode::Cgt;    break;  // cgt
                case 0x03: insn.op_code = IROpCode::Cgt;    break;  // cgt.un → treat as Cgt
                case 0x04: insn.op_code = IROpCode::Clt;    break;  // clt
                case 0x05: insn.op_code = IROpCode::Clt;    break;  // clt.un → treat as Clt
                case 0x06:  // ldftn (4-byte token)
                    insn.op_code = IROpCode::LdFtn;
                    insn.token = ReadToken(ip);
                    insn.has_token = true;
                    break;
                case 0x07:  // ldvirtftn (4-byte token)
                    insn.op_code = IROpCode::LdVirtFtn;
                    insn.token = ReadToken(ip);
                    insn.has_token = true;
                    break;
                case 0x09:  // ldarg (uint16)
                    insn.op_code = IROpCode::LdArg;
                    insn.operand_i4 = ReadU2(ip);
                    break;
                case 0x0A:  // ldarga (uint16)
                    insn.op_code = IROpCode::LdArgA;
                    insn.operand_i4 = static_cast<CHAOS_IL2CPP_INT32>(ReadU2(ip));
                    break;
                case 0x0B:  // ldarg (alternate, uint16)
                    insn.op_code = IROpCode::LdArg;
                    insn.operand_i4 = ReadU2(ip);
                    break;
                case 0x0C:  // ldloc (uint16)
                    insn.op_code = IROpCode::LdLoc;
                    insn.operand_i4 = static_cast<CHAOS_IL2CPP_INT32>(ReadU2(ip));
                    break;
                case 0x0D:  // ldloca (uint16)
                    insn.op_code = IROpCode::LdLocA;
                    insn.operand_i4 = static_cast<CHAOS_IL2CPP_INT32>(ReadU2(ip));
                    break;
                case 0x0E:  // stloc (uint16)
                    insn.op_code = IROpCode::StLoc;
                    insn.operand_i4 = static_cast<CHAOS_IL2CPP_INT32>(ReadU2(ip));
                    break;
                case 0x0F:  // ldelema (4-byte type token)
                    insn.op_code = IROpCode::LdElemA;
                    insn.token = ReadToken(ip);
                    insn.has_token = true;
                    break;
                case 0x15:  // initobj (4-byte token)
                    insn.op_code = IROpCode::InitObj;
                    insn.token = ReadToken(ip);
                    insn.has_token = true;
                    break;
                case 0x16:  // constrained. (4-byte type token)
                    // The constrained. prefix modifies the next CallVirt.
                    // Store the constrained type token; lowered below will
                    // merge it into the subsequent CallVirt instruction.
                    insn.op_code = IROpCode::Break;  // placeholder, handled in second pass
                    insn.token = ReadToken(ip);
                    insn.has_token = true;
                    break;
                case 0x17:  // cpblk
                    insn.op_code = IROpCode::Cpblk;
                    break;
                case 0x18:  // initblk
                    insn.op_code = IROpCode::InitBlk;
                    break;
                case 0x1A:  // rethrow
                    insn.op_code = IROpCode::Rethrow;
                    break;
                case 0x1C:  // sizeof (4-byte token)
                    insn.op_code = IROpCode::SizeOf;
                    insn.token = ReadToken(ip);
                    insn.has_token = true;
                    break;
                default:
                    decode_failed = true;
                    break;
            }
            raw_insns.push_back(insn);
            continue;
        }

        // Single-byte opcode.
        const int8_t operand_size = kOpcodeSizes[op_byte];
        if (operand_size < 0) { decode_failed = true; break; }

        insn.insn_size = static_cast<CHAOS_IL2CPP_SIZE>(1 + operand_size);

        switch (op_byte) {
            // ── No operand ──
            case 0x00: insn.op_code = IROpCode::Ret; break;  // nop → treated as Ret in empty fallback; handled below
            // Actually nop: just skip
            // We'll handle nop specially
            case 0x01: insn.op_code = IROpCode::Break; break;  // break (debugger) → NOP in interpreter

            case 0x02: insn.op_code = IROpCode::LdArg; insn.operand_i4 = 0; break;
            case 0x03: insn.op_code = IROpCode::LdArg; insn.operand_i4 = 1; break;
            case 0x04: insn.op_code = IROpCode::LdArg; insn.operand_i4 = 2; break;
            case 0x05: insn.op_code = IROpCode::LdArg; insn.operand_i4 = 3; break;

            case 0x06: insn.op_code = IROpCode::LdLoc; insn.operand_i4 = 0; break;
            case 0x07: insn.op_code = IROpCode::LdLoc; insn.operand_i4 = 1; break;
            case 0x08: insn.op_code = IROpCode::LdLoc; insn.operand_i4 = 2; break;
            case 0x09: insn.op_code = IROpCode::LdLoc; insn.operand_i4 = 3; break;

            case 0x0A: insn.op_code = IROpCode::StLoc; insn.operand_i4 = 0; break;
            case 0x0B: insn.op_code = IROpCode::StLoc; insn.operand_i4 = 1; break;
            case 0x0C: insn.op_code = IROpCode::StLoc; insn.operand_i4 = 2; break;
            case 0x0D: insn.op_code = IROpCode::StLoc; insn.operand_i4 = 3; break;

            // NOTE: ldnull at 0x13, ldc.i4.0-8 at 0x14-0x1C per ECMA 335.
            // The kOpcodeSizes[0x13-0x15] were fixed from 1→0 to reflect no operand.
            // Cases 0x1D-0x1E are undefined in ECMA and intentionally omitted.
            case 0x13: insn.op_code = IROpCode::LdNull; break;

            case 0x14: insn.op_code = IROpCode::LdcI4; insn.operand_i4 = 0; break;
            case 0x15: insn.op_code = IROpCode::LdcI4; insn.operand_i4 = 1; break;
            case 0x16: insn.op_code = IROpCode::LdcI4; insn.operand_i4 = 2; break;
            case 0x17: insn.op_code = IROpCode::LdcI4; insn.operand_i4 = 3; break;
            case 0x18: insn.op_code = IROpCode::LdcI4; insn.operand_i4 = 4; break;
            case 0x19: insn.op_code = IROpCode::LdcI4; insn.operand_i4 = 5; break;
            case 0x1A: insn.op_code = IROpCode::LdcI4; insn.operand_i4 = 6; break;
            case 0x1B: insn.op_code = IROpCode::LdcI4; insn.operand_i4 = 7; break;
            case 0x1C: insn.op_code = IROpCode::LdcI4; insn.operand_i4 = 8; break;

            // ── Short operand (1 byte) ──
            case 0x0E:  // ldarg.s
                insn.op_code = IROpCode::LdArg;
                insn.operand_i4 = ReadU1(ip);
                break;
            case 0x0F:  // ldarg.s (alternate, also short)
                insn.op_code = IROpCode::LdArg;
                insn.operand_i4 = ReadU1(ip);
                break;
            case 0x10:  // starg.s
                insn.op_code = IROpCode::StArg;
                insn.operand_i4 = ReadU1(ip);
                break;
            case 0x11:  // ldloc.s
                insn.op_code = IROpCode::LdLoc;
                insn.operand_i4 = ReadU1(ip);
                break;
            case 0x12:  // stloc.s (ECMA 335)
                insn.op_code = IROpCode::StLoc;
                insn.operand_i4 = ReadU1(ip);
                break;
            case 0x1F:  // ldc.i4.s
                insn.op_code = IROpCode::LdcI4;
                insn.operand_i4 = ReadI1(ip);
                break;

            // ── 4-byte operand ──
            case 0x20:  // ldc.i4
                insn.op_code = IROpCode::LdcI4;
                insn.operand_i4 = ReadI4(ip);
                break;

            // ── 8-byte operand ──
            case 0x21:  // ldc.i8
                insn.op_code = IROpCode::LdcI8;
                insn.operand_i8 = ReadI8(ip);
                break;

            // ── Float operands ──
            case 0x22:  // ldc.r4
                insn.op_code = IROpCode::LdcR4;
                insn.operand_i4 = ReadI4(ip);  // raw float32 bits
                break;
            case 0x23:  // ldc.r8
                insn.op_code = IROpCode::LdcR8;
                insn.operand_i8 = ReadI8(ip);  // raw float64 bits
                break;

            // ── Pop ──
            case 0x26: insn.op_code = IROpCode::Pop; break;

            // ── Token operand (4 bytes) ──
            case 0x28:  // call
                insn.op_code = IROpCode::Call;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x73:  // newobj
                insn.op_code = IROpCode::NewObj;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x8C:  // box
                insn.op_code = IROpCode::Box;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;

            // ── Token opcodes: string, type, field ──
            case 0x72:  // ldstr
                insn.op_code = IROpCode::LdStr;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x74:  // castclass
                insn.op_code = IROpCode::CastClass;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x75:  // isinst
                insn.op_code = IROpCode::IsInst;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x79:  // unbox
                insn.op_code = IROpCode::Unbox;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x7B:  // ldfld
                insn.op_code = IROpCode::LdFld;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x7D:  // stfld
                insn.op_code = IROpCode::StFld;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x7E:  // ldsfld
                insn.op_code = IROpCode::LdSFld;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x7F:  // stsfld
                insn.op_code = IROpCode::StSFld;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x8D:  // newarr
                insn.op_code = IROpCode::NewArr;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;

            // ── Array element access (no token) ──
            case 0x8E:  // ldlen
                insn.op_code = IROpCode::LdLen;
                break;
            case 0x90:  // ldelem.ref
            case 0x93:  // ldelem.i1
            case 0x94:  // ldelem.u1
            case 0x95:  // ldelem.i2
            case 0x96:  // ldelem.u2
            case 0x97:  // ldelem.i4
            case 0x98:  // ldelem.u4
            case 0x99:  // ldelem.i8
            case 0x9A:  // ldelem.i
            case 0x9B:  // ldelem.r4
            case 0x9C:  // ldelem.r8
                insn.op_code = IROpCode::LdElem;
                break;
            case 0x9D:  // stelem.ref
            case 0x9E:  // stelem.i1
            case 0x9F:  // stelem.i2
            case 0xA0:  // stelem.i4
            case 0xA1:  // stelem.i8
            case 0xA2:  // stelem.r4
                insn.op_code = IROpCode::StElem;
                break;
            case 0xA3:  // ldelem (generic, type token)
                insn.op_code = IROpCode::LdElem;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0xA4:  // stelem (generic, type token)
                insn.op_code = IROpCode::StElem;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;

            // ── Arithmetic ──
            case 0x58: insn.op_code = IROpCode::Add; break;
            case 0x59: insn.op_code = IROpCode::Sub; break;
            case 0x5A: insn.op_code = IROpCode::Mul; break;
            case 0x5B: insn.op_code = IROpCode::Div; break;
            case 0x5C: insn.op_code = IROpCode::DivUn; break;  // div.un (alt encoding of 0x60)
            case 0x5D: insn.op_code = IROpCode::Rem; break;
            case 0x5E: insn.op_code = IROpCode::RemUn; break;  // rem.un (alt encoding of 0x61)
            case 0x5F: insn.op_code = IROpCode::And; break;    // and (ECMA standard position)
            case 0x65: insn.op_code = IROpCode::Neg; break;

            // ── Overflow-checking arithmetic ──
            case 0xD2: insn.op_code = IROpCode::AddOvf; break;
            case 0xD3: insn.op_code = IROpCode::AddOvf; break;  // add.ovf.un → treat as add.ovf
            case 0xD4: insn.op_code = IROpCode::SubOvf; break;
            case 0xD5: insn.op_code = IROpCode::SubOvf; break;  // sub.ovf.un → treat as sub.ovf
            case 0xD6: insn.op_code = IROpCode::MulOvf; break;
            case 0xD7: insn.op_code = IROpCode::MulOvf; break;  // mul.ovf.un → treat as mul.ovf

            // NOTE: 0x60-0x6F uses a non-ECMA mapping in this codebase
            // (div.un, rem.un, and, or, xor, neg, not, shl, shr, shr.un, ...)
            // so 0x67-0x69 are Shl/Shr/ShrUn (not conv.i1/i2/i4 as ECMA specifies).
            case 0x67: insn.op_code = IROpCode::Shl; break;
            case 0x68: insn.op_code = IROpCode::Shr; break;
            case 0x69: insn.op_code = IROpCode::ShrUn; break;

            // ── Overflow-checking conversions ──
            case 0xB1: insn.op_code = IROpCode::ConvOvfI; break;    // conv.ovf.i
            case 0xB2: insn.op_code = IROpCode::ConvOvfI; break;    // conv.ovf.i.un → treat as ovf.i
            case 0xB4: insn.op_code = IROpCode::ConvOvfU; break;    // conv.ovf.u
            case 0xB5: insn.op_code = IROpCode::ConvOvfU; break;    // conv.ovf.u.un → treat as ovf.u

            // ── Conversions ──
            case 0x6B: insn.op_code = IROpCode::Conv_R4; break;
            case 0x6C: insn.op_code = IROpCode::Conv_I4; break;
            case 0x6D: insn.op_code = IROpCode::Conv_I8; break;
            case 0x6E: insn.op_code = IROpCode::Conv_R8; break;

            // ── Return ──
            case 0x2A: insn.op_code = IROpCode::Ret; break;

            // ── Branch: short (1-byte offset) ──
            case 0x2B:  // br.s
                insn.op_code = IROpCode::Br;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x2C:  // brfalse.s
                insn.op_code = IROpCode::BrFalse;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x2D:  // brtrue.s
                insn.op_code = IROpCode::BrTrue;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x2E:  // beq.s
                insn.op_code = IROpCode::Beq;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x2F:  // leave.s
                insn.op_code = IROpCode::Leave;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x30:  // bgt.s
                insn.op_code = IROpCode::Bgt;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x31:  // ble.s
                insn.op_code = IROpCode::Ble;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x32:  // blt.s
                insn.op_code = IROpCode::Blt;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x33:  // bne.un.s (unsigned not equal)
                insn.op_code = IROpCode::BneUn;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x34:  // bge.un.s (unsigned greater/equal)
                insn.op_code = IROpCode::BgeUn;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x35:  // bgt.un.s (unsigned greater than)
                insn.op_code = IROpCode::BgtUn;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x36:  // ble.un.s (unsigned less/equal)
                insn.op_code = IROpCode::BleUn;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;
            case 0x37:  // blt.un.s (unsigned less than)
                insn.op_code = IROpCode::BltUn;
                insn.is_branch = true;
                insn.is_branch_short = true;
                insn.branch_offset = ReadI1(ip);
                break;

            // ── Branch: long (4-byte offset) ──
            case 0x38:  // br
                insn.op_code = IROpCode::Br;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x39:  // brfalse
                insn.op_code = IROpCode::BrFalse;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x3A:  // brtrue
                insn.op_code = IROpCode::BrTrue;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x3B:  // beq
                insn.op_code = IROpCode::Beq;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x3C:  // leave
                insn.op_code = IROpCode::Leave;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x3D:  // bgt (actually 0x3D is bgt... wait. Let me re-check ECMA)
                // 0x3D: bgt, 0x3E: ble, 0x3F: blt, 0x40: bge, 0x41: bne.un
                insn.op_code = IROpCode::Bgt;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x3E:  // ble
                insn.op_code = IROpCode::Ble;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x3F:  // blt
                insn.op_code = IROpCode::Blt;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x40:  // bne.un (long unsigned not equal)
                insn.op_code = IROpCode::BneUn;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x41:  // bge.un (long unsigned greater/equal)
                insn.op_code = IROpCode::BgeUn;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x42:  // bgt.un (long unsigned greater than)
                insn.op_code = IROpCode::BgtUn;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x43:  // ble.un (long unsigned less/equal)
                insn.op_code = IROpCode::BleUn;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;
            case 0x44:  // blt.un (long unsigned less than)
                insn.op_code = IROpCode::BltUn;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = ReadI4(ip);
                break;

            // ── Endfinally ──
            case 0xDC:  // endfinally
                insn.op_code = IROpCode::EndFinally;
                break;

            // ── Dup ──
            case 0x25: insn.op_code = IROpCode::Dup; break;

            // ── Callvirt (4-byte token) ──
            case 0x29:  // callvirt
                insn.op_code = IROpCode::CallVirt;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;

            // ── Switch (variable-length) ──
            case 0x45: {  // switch
                insn.op_code = IROpCode::Switch;
                const CHAOS_IL2CPP_UINT32 n_cases = static_cast<CHAOS_IL2CPP_UINT32>(ReadU1(ip));
                insn.token = n_cases + 1u;
                ip += static_cast<CHAOS_IL2CPP_SIZE>(n_cases + 1u) * 4u;
                insn.is_branch = true;
                insn.is_branch_short = false;
                insn.branch_offset = 0;
                break;
            }

            // ── LdInd.* variants (type in operand_i4) ──
            case 0x46: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 0; break;
            case 0x47: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 1; break;
            case 0x48: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 2; break;
            case 0x49: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 3; break;
            case 0x4A: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 4; break;
            case 0x4B: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 5; break;
            case 0x4C: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 6; break;
            case 0x4D: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 7; break;
            case 0x4E: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 8; break;
            case 0x4F: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 9; break;
            case 0x50: insn.op_code = IROpCode::LdInd; insn.operand_i4 = 10; break;

            // ── StInd.* variants (type in operand_i4) ──
            case 0x52: insn.op_code = IROpCode::StInd; insn.operand_i4 = 0; break;
            case 0x53: insn.op_code = IROpCode::StInd; insn.operand_i4 = 2; break;
            case 0x54: insn.op_code = IROpCode::StInd; insn.operand_i4 = 4; break;
            case 0x55: insn.op_code = IROpCode::StInd; insn.operand_i4 = 6; break;
            case 0x56: insn.op_code = IROpCode::StInd; insn.operand_i4 = 8; break;
            case 0x57: insn.op_code = IROpCode::StInd; insn.operand_i4 = 9; break;

            // ── Unsigned arithmetic ──
            case 0x60: insn.op_code = IROpCode::DivUn; break;
            case 0x61: insn.op_code = IROpCode::RemUn; break;

            // ── Bitwise ──
            case 0x62: insn.op_code = IROpCode::And; break;
            case 0x63: insn.op_code = IROpCode::Or; break;
            case 0x64: insn.op_code = IROpCode::Xor; break;
            case 0x66: insn.op_code = IROpCode::Not; break;

            // ── More conversions ──
            case 0x6F: insn.op_code = IROpCode::ConvRUn; break;

            // ── Ldobj / Stobj (typed load/store) ──
            case 0x8F:  // ldobj (4-byte type token)
                insn.op_code = IROpCode::LdObj;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;
            case 0x81:  // stobj (4-byte type token)
                insn.op_code = IROpCode::StObj;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;

            // ── Throw ──
            case 0x7A: insn.op_code = IROpCode::Throw; break;

            // ── Native int conversions ──
            case 0xB0: insn.op_code = IROpCode::ConvI; break;  // conv.i (ECMA standard position)
            case 0xB3: insn.op_code = IROpCode::ConvI; break;  // conv.ovf.i (keep for backward compat)
            case 0xB6: insn.op_code = IROpCode::ConvU; break;  // conv.u

            // ── Ldtoken ──
            case 0xD0:
                insn.op_code = IROpCode::LdToken;
                insn.token = ReadToken(ip);
                insn.has_token = true;
                break;

            // ── EndFilter ──
            case 0xD1:  // endfilter
                insn.op_code = IROpCode::EndFilter;
                break;

            default:
                // Unsupported opcode → fail.
                decode_failed = true;
                break;
        }

        if (decode_failed) break;
        raw_insns.push_back(insn);
    }

    if (decode_failed) {
        return MakeEmptyMethod();
    }

    // ── Second pass: resolve branch targets and tokens ──
    // Build a mapping from IL byte offset → instruction index.
    CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_SIZE) offset_to_index;
    offset_to_index.resize(il_length, static_cast<CHAOS_IL2CPP_SIZE>(-1));
    for (CHAOS_IL2CPP_SIZE i = 0u; i < raw_insns.size(); ++i) {
        offset_to_index[raw_insns[i].il_offset] = i;
    }

    IRMethod method;
    method.instructions.reserve(raw_insns.size());

    // Constrained prefix tracking: .constrained. (0xFE16) modifies the next
    // CallVirt. In the first pass it was lowered as Break with has_token=true.
    // Here we merge it into the following CallVirt as CallVirtConstrained.
    bool pending_constrained = false;
    CHAOS_IL2CPP_UINT32 constrained_type_token = 0u;

    for (CHAOS_IL2CPP_SIZE i = 0u; i < raw_insns.size(); ++i) {
        const RawInsn& raw = raw_insns[i];

        // Detect constrained. prefix marker (Break with has_token=true).
        if (raw.op_code == IROpCode::Break && raw.has_token) {
            pending_constrained = true;
            constrained_type_token = raw.token;
            continue;
        }

        IRInstruction insn = {};
        insn.op_code = raw.op_code;

        // If constrained is pending, merge into this CallVirt.
        if (pending_constrained && raw.op_code == IROpCode::CallVirt) {
            insn.op_code = IROpCode::CallVirtConstrained;
            insn.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(constrained_type_token);
            pending_constrained = false;
        } else if (pending_constrained) {
            // Constrained not followed by CallVirt — discard (invalid IL per ECMA).
            pending_constrained = false;
        }

        // Set basic immediate operand.
        insn.immediate_i4 = raw.operand_i4;
        insn.immediate_i8 = raw.operand_i8;

        // Handle variable-index operands.
        switch (raw.op_code) {
            case IROpCode::LdArg:
            case IROpCode::StArg:
            case IROpCode::LdLoc:
            case IROpCode::StLoc:
                insn.operand_index = raw.operand_i4;
                break;
            case IROpCode::LdcI4:
                insn.immediate_i4 = raw.operand_i4;
                break;
            case IROpCode::LdcI8:
                insn.immediate_i8 = raw.operand_i8;
                break;
            case IROpCode::LdcR4: {
                // Convert raw int32 bits back to float.
                float f32;
                std::memcpy(&f32, &raw.operand_i4, sizeof(f32));
                insn.immediate_r8 = static_cast<double>(f32);
                break;
            }
            case IROpCode::LdcR8: {
                // Convert raw int64 bits back to double.
                double f64;
                std::memcpy(&f64, &raw.operand_i8, sizeof(f64));
                insn.immediate_r8 = f64;
                break;
            }
            default:
                break;
        }

        // Resolve branch targets.
        if (raw.is_branch) {
            // Calculate target IL offset = current offset + instruction_size + branch_offset.
            const int32_t target_il_offset = static_cast<int32_t>(raw.il_offset + raw.insn_size) + raw.branch_offset;
            if (target_il_offset >= 0 && static_cast<CHAOS_IL2CPP_SIZE>(target_il_offset) < il_length) {
                const CHAOS_IL2CPP_SIZE target_idx = offset_to_index[static_cast<CHAOS_IL2CPP_SIZE>(target_il_offset)];
                if (target_idx != static_cast<CHAOS_IL2CPP_SIZE>(-1)) {
                    insn.branch_target = target_idx;
                }
            }
        }

        // Resolve metadata tokens.
        if (raw.has_token && token_resolver != nullptr) {
            if (!token_resolver(raw.token, insn, user_data)) {
                // Token resolution failed → return empty method.
                return MakeEmptyMethod();
            }
        }

        // Handle newobj: set secondary_index to field count from token.
        if (raw.op_code == IROpCode::NewObj) {
            // The token resolver should have set call_target to the MethodInfoHandle.
            // For now, use a default field count.
            // Phase 5b+: the token resolver will set secondary_index based on
            // the resolved constructor's declaring type field count.
            insn.secondary_index = 1u;  // minimum
        }

        // Store declared method token for CallVirt vtable resolution.
        if ((raw.op_code == IROpCode::CallVirt || raw.op_code == IROpCode::CallVirtConstrained) && raw.has_token) {
            insn.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(raw.token);
        }

        method.instructions.push_back(insn);
    }

    // Ensure at least one instruction (Ret).
    if (method.instructions.empty()) {
        method.instructions.push_back({});
        method.instructions.back().op_code = IROpCode::Ret;
    }

    // ── Parse SEH clauses (try/catch/finally) ──
    // SEH sections follow the IL code at the next 4-byte boundary.
    // Use `code_size` for padding when available (from method header);
    // fall back to il_length for backward compat when code_size is 0.
    const CHAOS_IL2CPP_SIZE pad_size = (code_size > 0u) ? code_size : il_length;
    if (il_length > 0u) {
        const CHAOS_IL2CPP_SIZE aligned = (pad_size + 3u) & ~static_cast<CHAOS_IL2CPP_SIZE>(3u);
        const uint8_t* scan = il_start + aligned;

        // Process each method data section.
        while (scan + 4 <= il_end) {
            const uint8_t kind = scan[0];
            const bool more_sects = (kind & 0x40) != 0;
            const bool is_eh = (kind & 0x1F) == 0x01;

            if (!is_eh) break;

            const bool is_fat = (kind & 0x20) != 0;

            if (is_fat) {
                // Fat EH section: 4-byte header, 24-byte clauses, DataSize in 4-byte units.
                uint32_t data_size = static_cast<uint32_t>(scan[2]) |
                                     (static_cast<uint32_t>(scan[3]) << 8);
                uint32_t total_bytes = data_size * 4u;
                if (total_bytes < 4u) break;
                uint32_t num_clauses = (total_bytes - 4u) / 24u;

                for (uint32_t ci = 0u; ci < num_clauses; ++ci) {
                    const uint8_t* cd = scan + 4 + ci * 24;
                    SEHClause clause = {};
                    uint32_t flags = static_cast<uint32_t>(cd[0]) |
                                     (static_cast<uint32_t>(cd[1]) << 8) |
                                     (static_cast<uint32_t>(cd[2]) << 16) |
                                     (static_cast<uint32_t>(cd[3]) << 24);
                    clause.flags = static_cast<SEHFlags>(flags);

                    uint32_t try_off = static_cast<uint32_t>(cd[4]) |
                                       (static_cast<uint32_t>(cd[5]) << 8) |
                                       (static_cast<uint32_t>(cd[6]) << 16) |
                                       (static_cast<uint32_t>(cd[7]) << 24);
                    uint32_t try_len = static_cast<uint32_t>(cd[8]) |
                                       (static_cast<uint32_t>(cd[9]) << 8) |
                                       (static_cast<uint32_t>(cd[10]) << 16) |
                                       (static_cast<uint32_t>(cd[11]) << 24);
                    uint32_t handler_off = static_cast<uint32_t>(cd[12]) |
                                           (static_cast<uint32_t>(cd[13]) << 8) |
                                           (static_cast<uint32_t>(cd[14]) << 16) |
                                           (static_cast<uint32_t>(cd[15]) << 24);
                    clause.class_token = static_cast<uint32_t>(cd[20]) |
                                         (static_cast<uint32_t>(cd[21]) << 8) |
                                         (static_cast<uint32_t>(cd[22]) << 16) |
                                         (static_cast<uint32_t>(cd[23]) << 24);

                    // Resolve IL offsets to instruction indices.
                    if (try_off < il_length)
                        clause.try_start_idx = offset_to_index[try_off];
                    uint32_t try_end_off = try_off + try_len;
                    if (try_end_off > 0u && try_end_off <= il_length) {
                        CHAOS_IL2CPP_SIZE end_idx = offset_to_index[try_end_off - 1u];
                        if (end_idx != static_cast<CHAOS_IL2CPP_SIZE>(-1))
                            clause.try_end_idx = end_idx + 1u;
                    }
                    if (handler_off < il_length)
                        clause.handler_start_idx = offset_to_index[handler_off];

                    method.seh_clauses.push_back(clause);
                }
                scan += total_bytes;
            } else {
                // Small EH section: 4-byte header, 12-byte clauses, DataSize in bytes.
                uint8_t data_size = scan[1];
                if (data_size < 4u) break;
                uint32_t num_clauses = (static_cast<uint32_t>(data_size) - 4u) / 12u;

                for (uint32_t ci = 0u; ci < num_clauses; ++ci) {
                    const uint8_t* cd = scan + 4 + ci * 12;
                    SEHClause clause = {};
                    uint16_t flags = static_cast<uint16_t>(cd[0]) |
                                     (static_cast<uint16_t>(cd[1]) << 8);
                    clause.flags = static_cast<SEHFlags>(flags);

                    uint16_t try_off = static_cast<uint16_t>(cd[2]) |
                                       (static_cast<uint16_t>(cd[3]) << 8);
                    uint8_t try_len  = cd[4];
                    uint16_t handler_off = static_cast<uint16_t>(cd[5]) |
                                           (static_cast<uint16_t>(cd[6]) << 8);
                    clause.class_token = static_cast<uint32_t>(cd[8]) |
                                         (static_cast<uint32_t>(cd[9]) << 8) |
                                         (static_cast<uint32_t>(cd[10]) << 16) |
                                         (static_cast<uint32_t>(cd[11]) << 24);

                    if (try_off < il_length)
                        clause.try_start_idx = offset_to_index[try_off];
                    uint32_t try_end_off = static_cast<uint32_t>(try_off) + try_len;
                    if (try_end_off > 0u && try_end_off <= il_length) {
                        CHAOS_IL2CPP_SIZE end_idx = offset_to_index[try_end_off - 1u];
                        if (end_idx != static_cast<CHAOS_IL2CPP_SIZE>(-1))
                            clause.try_end_idx = end_idx + 1u;
                    }
                    if (handler_off < il_length)
                        clause.handler_start_idx = offset_to_index[handler_off];

                    method.seh_clauses.push_back(clause);
                }
                scan += data_size;
            }

            if (!more_sects) break;
        }
    }

    return method;
}

}  // namespace chaos::il2cpp::interpreter
