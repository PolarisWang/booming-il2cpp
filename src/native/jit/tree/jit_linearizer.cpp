// jit_linearizer.cpp — Tree → RegisterInstruction linearization

#include "tree/jit_linearizer.h"
#include "interpreter/ir_reg_alloc.h"
#include "interpreter/generated/ir_opcodes.h"

#include <cstdint>
#include <cstring>
#include <vector>

namespace chaos::il2cpp::jit::tree {

// ── Header builder for RegisterInstruction ────────────────────────────
static uint64_t MakeHdr(interpreter::IROpCode opc, uint8_t dst = 0,
                         uint8_t src1 = 0, uint8_t src2 = 0,
                         uint8_t flags = 0) noexcept {
    return static_cast<uint64_t>(opc) |
           (static_cast<uint64_t>(dst)   << 16) |
           (static_cast<uint64_t>(src1)  << 24) |
           (static_cast<uint64_t>(src2)  << 32) |
           (static_cast<uint64_t>(flags) << 40);
}

// ── Opcode mapping: tree NodeKind → IROpCode ──────────────────────────
static interpreter::IROpCode MapToIROpCode(NodeKind kind) noexcept {
    switch (kind) {
        case kLdcI4:    return interpreter::IROpCode::LdcI4;
        case kLdcI8:    return interpreter::IROpCode::LdcI8;
        case kLdcR4:    return interpreter::IROpCode::LdcR4;
        case kLdcR8:    return interpreter::IROpCode::LdcR8;
        case kLdNull:   return interpreter::IROpCode::LdNull;
        case kLdStr:    return interpreter::IROpCode::LdStr;
        case kLdArg:    return interpreter::IROpCode::LdArg;
        case kLdLoc:    return interpreter::IROpCode::LdLoc;
        case kNeg:      return interpreter::IROpCode::Neg;
        case kNot:      return interpreter::IROpCode::Not;
        case kConvI4:   return interpreter::IROpCode::Conv_I4;
        case kConvI8:   return interpreter::IROpCode::Conv_I8;
        case kConvR4:   return interpreter::IROpCode::Conv_R4;
        case kConvR8:   return interpreter::IROpCode::Conv_R8;
        case kAdd:      return interpreter::IROpCode::Add;
        case kSub:      return interpreter::IROpCode::Sub;
        case kMul:      return interpreter::IROpCode::Mul;
        case kDiv:      return interpreter::IROpCode::Div;
        case kRem:      return interpreter::IROpCode::Rem;
        case kAnd:      return interpreter::IROpCode::And;
        case kOr:       return interpreter::IROpCode::Or;
        case kXor:      return interpreter::IROpCode::Xor;
        case kShl:      return interpreter::IROpCode::Shl;
        case kShr:      return interpreter::IROpCode::Shr;
        case kShrUn:    return interpreter::IROpCode::ShrUn;
        case kCeq:      return interpreter::IROpCode::Ceq;
        case kClt:      return interpreter::IROpCode::Clt;
        case kCgt:      return interpreter::IROpCode::Cgt;
        case kLdLen:    return interpreter::IROpCode::LdLen;
        case kLdElem:   return interpreter::IROpCode::LdElem;
        case kLdElemA:  return interpreter::IROpCode::LdElemA;
        case kAbs:      return interpreter::IROpCode::Abs;
        case kMin:      return interpreter::IROpCode::Min;
        case kMax:      return interpreter::IROpCode::Max;
        case kLdFld:    return interpreter::IROpCode::LdFld;
        case kStLoc:    return interpreter::IROpCode::StLoc;
        case kReturn:   return interpreter::IROpCode::Ret;
        case kBox:      return interpreter::IROpCode::Box;
        case kUnbox:    return interpreter::IROpCode::Unbox;
        case kPopcnt:   return interpreter::IROpCode::Popcnt;
        case kLzcnt:    return interpreter::IROpCode::Lzcnt;
        default:        return static_cast<interpreter::IROpCode>(0); // unreachable for valid nodes
    }
}

Linearizer::Linearizer() noexcept {
    next_vreg_ = kBaseVReg;
    std::memset(emitted_vn_, 0, sizeof(emitted_vn_));
}

bool Linearizer::IsCseDuplicate(const ExprNode* node) const noexcept {
    if (!node) return false;
    uint32_t vn = node->vn_id();
    if (vn == 0 || vn >= kMaxVN) return false;
    return emitted_vn_[vn];
}

uint32_t Linearizer::EmitInstr(uint64_t header, int64_t imm,
                                std::vector<interpreter::RegisterInstruction>& out) noexcept {
    uint32_t dst = static_cast<uint8_t>((header >> 16) & 0xFF);
    if (dst == 0 && (header & 0x010000)) {
        // HasDst but dst=0 — allocate a new vreg
        dst = NextVReg();
        header = (header & 0xFFFEFFFF) | (static_cast<uint64_t>(dst) << 16);
    }
    interpreter::RegisterInstruction ri;
    ri.header = header;
    ri.imm.i8 = imm;
    out.push_back(ri);
    return dst;
}

uint32_t Linearizer::EmitInstr(uint64_t header, int32_t imm,
                                std::vector<interpreter::RegisterInstruction>& out) noexcept {
    uint32_t dst = EmitInstr(header, static_cast<int64_t>(imm), out);
    return dst;
}

uint32_t Linearizer::LinearizeNode(
    const ExprNode* node,
    std::vector<interpreter::RegisterInstruction>& out) noexcept
{
    if (!node) return 0;

    NodeKind k = node->kind();

    // ── Leaf nodes: emit directly ──────────────────────────────────────
    switch (k) {
    case kLdcI4:
        return EmitInstr(MakeHdr(interpreter::IROpCode::LdcI4, NextVReg(), 0, 0,
                                 interpreter::kRegHasDst | interpreter::kRegHasImm),
                         node->i4, out);

    case kLdcI8:
        return EmitInstr(MakeHdr(interpreter::IROpCode::LdcI8, NextVReg(), 0, 0,
                                 interpreter::kRegHasDst | interpreter::kRegHasImm),
                         node->i8, out);

    case kLdcR4:
        return EmitInstr(MakeHdr(interpreter::IROpCode::LdcR4, NextVReg(), 0, 0,
                                 interpreter::kRegHasDst | interpreter::kRegHasImm),
                         static_cast<int32_t>(node->i4), out);

    case kLdcR8:
        return EmitInstr(MakeHdr(interpreter::IROpCode::LdcR8, NextVReg(), 0, 0,
                                 interpreter::kRegHasDst | interpreter::kRegHasImm),
                         node->i8, out);

    case kLdNull:
        return EmitInstr(MakeHdr(interpreter::IROpCode::LdNull, NextVReg(), 0, 0,
                                 interpreter::kRegHasDst),
                         0, out);

    case kLdStr:
        return EmitInstr(MakeHdr(interpreter::IROpCode::LdStr, NextVReg(), 0, 0,
                                 interpreter::kRegHasDst | interpreter::kRegHasImm),
                         reinterpret_cast<int64_t>(node->ptr), out);

    case kLdArg: {
        uint32_t dst = NextVReg();
        interpreter::RegisterInstruction ri;
        ri.header = MakeHdr(interpreter::IROpCode::LdArg, dst, 0, 0,
                            interpreter::kRegHasDst | interpreter::kRegHasImm);
        ri.imm.operand_index = node->operand_index;
        out.push_back(ri);
        return dst;
    }

    case kLdLoc: {
        uint32_t dst = NextVReg();
        interpreter::RegisterInstruction ri;
        ri.header = MakeHdr(interpreter::IROpCode::LdLoc, dst, 0, 0,
                            interpreter::kRegHasDst | interpreter::kRegHasImm);
        ri.imm.operand_index = node->operand_index;
        out.push_back(ri);
        return dst;
    }

    default:
        break;
    }

    // ── CSE: if already computed, emit Dup instead of recomputing ──────
    if (IsCseDuplicate(node) &&
        ((k >= kNeg && k <= kCgtUn) || k == kAbs || k == kMin || k == kMax)) {
        // Node's VN was already emitted — emit a copy via Dup
        // (the vreg from the earlier computation is tracked separately)
        return 0;  // caller handles CSE by not emitting
    }

    // ── Unary nodes ────────────────────────────────────────────────────
    if ((k >= kNeg && k <= kLdLen) || k == kAbs || k == kPopcnt || k == kLzcnt) {
        uint32_t src_vreg = LinearizeNode(node->child0, out);
        if (src_vreg == 0) return 0;

        interpreter::IROpCode opc = MapToIROpCode(k);
        uint32_t dst = NextVReg();

        if (k == kLdLen) {
            // LdLen has no flags-based HasSrc1 in the current convention
            interpreter::RegisterInstruction ri;
            ri.header = MakeHdr(opc, dst, static_cast<uint8_t>(src_vreg), 0,
                                interpreter::kRegHasDst | interpreter::kRegHasSrc1);
            ri.imm.i4 = 0;
            out.push_back(ri);
        } else {
            interpreter::RegisterInstruction ri;
            ri.header = MakeHdr(opc, dst, static_cast<uint8_t>(src_vreg), 0,
                                interpreter::kRegHasDst | interpreter::kRegHasSrc1);
            ri.imm.i4 = 0;
            out.push_back(ri);
        }

        // Mark VN as emitted for CSE
        if (node->vn_id() > 0 && node->vn_id() < kMaxVN)
            emitted_vn_[node->vn_id()] = true;

        return dst;
    }

    // ── Binary nodes ───────────────────────────────────────────────────
    if ((k >= kAdd && k <= kCgtUn) || k == kMin || k == kMax) {
        uint32_t src1_vreg = LinearizeNode(node->child0, out);
        uint32_t src2_vreg = LinearizeNode(node->child1, out);
        if (src1_vreg == 0 && node->child0) return 0;
        if (src2_vreg == 0 && node->child1) return 0;

        interpreter::IROpCode opc = MapToIROpCode(k);
        uint32_t dst = NextVReg();

        interpreter::RegisterInstruction ri;
        ri.header = MakeHdr(opc, dst,
                             static_cast<uint8_t>(src1_vreg),
                             static_cast<uint8_t>(src2_vreg),
                             interpreter::kRegHasDst |
                             interpreter::kRegHasSrc1 |
                             interpreter::kRegHasSrc2);
        ri.imm.i4 = 0;
        out.push_back(ri);

        if (node->vn_id() > 0 && node->vn_id() < kMaxVN)
            emitted_vn_[node->vn_id()] = true;

        return dst;
    }

    // ── LdElem / LdElemA (array element load/address) ─────────────────────
    if (k == kLdElem || k == kLdElemA) {
        uint32_t arr_vreg = LinearizeNode(node->child0, out);
        uint32_t idx_vreg = LinearizeNode(node->child1, out);
        if (arr_vreg == 0 || idx_vreg == 0) return 0;

        interpreter::IROpCode opc = (node->flags & kFlagNoBoundsCheck)
            ? ((k == kLdElem)
                ? interpreter::IROpCode::LdElemNoChk
                : interpreter::IROpCode::LdElemANoChk)
            : MapToIROpCode(k);

        uint32_t dst = NextVReg();
        interpreter::RegisterInstruction ri;
        ri.header = MakeHdr(opc, dst,
                             static_cast<uint8_t>(arr_vreg),
                             static_cast<uint8_t>(idx_vreg),
                             interpreter::kRegHasDst |
                             interpreter::kRegHasSrc1 |
                             interpreter::kRegHasSrc2);
        ri.imm.i4 = 0;
        out.push_back(ri);

        if (node->vn_id() > 0 && node->vn_id() < kMaxVN)
            emitted_vn_[node->vn_id()] = true;

        return dst;
    }

    // ── SIMD operations ─────────────────────────────────────────────────
    if (k == kSimd) {
        uint32_t src1_vreg = LinearizeNode(node->child0, out);
        uint32_t src2_vreg = (node->child1) ? LinearizeNode(node->child1, out) : 0;
        if (src1_vreg == 0 && node->child0) return 0;

        uint32_t dst = NextVReg();
        // Pack simd_op(8) + elem_type(8) + simd_imm(16) into imm field
        int64_t simd_meta = static_cast<int64_t>(node->simd_op()) |
                           (static_cast<int64_t>(node->simd_elem_type()) << 8) |
                           (static_cast<int64_t>(node->simd_imm()) << 16);

        interpreter::RegisterInstruction ri;
        ri.header = MakeHdr(interpreter::IROpCode::Simd, dst,
                             static_cast<uint8_t>(src1_vreg),
                             static_cast<uint8_t>(src2_vreg),
                             interpreter::kRegHasDst |
                             interpreter::kRegHasSrc1 |
                             (src2_vreg ? interpreter::kRegHasSrc2 : 0));
        ri.imm.i8 = simd_meta;
        out.push_back(ri);

        if (node->vn_id() > 0 && node->vn_id() < kMaxVN)
            emitted_vn_[node->vn_id()] = true;

        return dst;
    }

    // ── Side-effect / root nodes ───────────────────────────────────────
    if (k == kStLoc) {
        uint32_t val_vreg = LinearizeNode(node->child0, out);
        if (val_vreg == 0 && node->child0) return 0;

        interpreter::RegisterInstruction ri;
        ri.header = MakeHdr(interpreter::IROpCode::StLoc, 0,
                             static_cast<uint8_t>(val_vreg), 0,
                             interpreter::kRegHasSrc1 | interpreter::kRegHasImm |
                             interpreter::kRegIsStore);
        ri.imm.operand_index = node->arg_count;
        out.push_back(ri);
        return 0;
    }

    if (k == kStElem) {
        uint32_t arr_vreg = LinearizeNode(node->child0, out);
        uint32_t val_vreg = (node->child1) ? LinearizeNode(node->child1, out) : 0;
        uint32_t idx_vreg = node->operand_index;
        if (arr_vreg == 0) return 0;

        interpreter::IROpCode opc = (node->flags & kFlagNoBoundsCheck)
            ? interpreter::IROpCode::StElemNoChk
            : interpreter::IROpCode::StElem;

        interpreter::RegisterInstruction ri;
        ri.header = MakeHdr(opc, 0,
                             static_cast<uint8_t>(arr_vreg),
                             static_cast<uint8_t>(idx_vreg),
                             interpreter::kRegHasSrc1 |
                             interpreter::kRegHasSrc2 |
                             interpreter::kRegHasSrc3 |
                             interpreter::kRegIsStore);
        // src3 = value vreg in reserved header bits [55:48]
        ri.header |= (static_cast<uint64_t>(val_vreg & 0xFF) << 48);
        ri.imm.i4 = 0;
        out.push_back(ri);
        return 0;
    }

    if (k == kReturn) {
        if (node->child0) {
            uint32_t val_vreg = LinearizeNode(node->child0, out);
            interpreter::RegisterInstruction ri;
            ri.header = MakeHdr(interpreter::IROpCode::Ret, 0,
                                 static_cast<uint8_t>(val_vreg), 0,
                                 interpreter::kRegHasSrc1 | interpreter::kRegIsBranch);
            ri.imm.i4 = 0;
            out.push_back(ri);
        } else {
            interpreter::RegisterInstruction ri;
            ri.header = MakeHdr(interpreter::IROpCode::Ret, 0, 0, 0,
                                 interpreter::kRegIsBranch);
            ri.imm.i4 = 0;
            out.push_back(ri);
        }
        return 0;
    }

    return 0;  // unhandled node kind
}

uint32_t Linearizer::Linearize(
    const ExprNode* root,
    std::vector<interpreter::RegisterInstruction>& out_instrs) noexcept
{
    return LinearizeNode(root, out_instrs);
}

void Linearizer::LinearizeRoots(
    const ExprNode* const* roots, uint32_t root_count,
    std::vector<interpreter::RegisterInstruction>& out_instrs) noexcept
{
    for (uint32_t i = 0; i < root_count; ++i) {
        LinearizeNode(roots[i], out_instrs);
    }
}

}  // namespace chaos::il2cpp::jit::tree
