// jit_tree_builder.cpp — Expression tree DAG construction

#include "tree/jit_tree_builder.h"
#include "interpreter/ir_reg_alloc.h"        // RegisterInstruction, RegisterMethod
#include "interpreter/generated/ir_opcodes.h" // IROpCode

#include <chaos/log.h>

#include <cstdlib>
#include <cstring>

namespace chaos::il2cpp::jit::tree {

// ── Opcode mapping: IROpCode → tree NodeKind ──────────────────────────
// Returns kNop for opcodes that don't map to tree nodes.
static NodeKind MapOpcode(interpreter::IROpCode opc) noexcept {
    switch (opc) {
        case interpreter::IROpCode::LdcI4:    return kLdcI4;
        case interpreter::IROpCode::LdcI8:    return kLdcI8;
        case interpreter::IROpCode::LdcR4:    return kLdcR4;
        case interpreter::IROpCode::LdcR8:    return kLdcR8;
        case interpreter::IROpCode::LdNull:   return kLdNull;
        case interpreter::IROpCode::LdStr:    return kLdStr;
        case interpreter::IROpCode::LdArg:    return kLdArg;
        case interpreter::IROpCode::LdLoc:    return kLdLoc;
        case interpreter::IROpCode::Neg:      return kNeg;
        case interpreter::IROpCode::Not:      return kNot;
        case interpreter::IROpCode::Conv_I4:  return kConvI4;
        case interpreter::IROpCode::Conv_I8:  return kConvI8;
        case interpreter::IROpCode::Conv_R4:  return kConvR4;
        case interpreter::IROpCode::Conv_R8:  return kConvR8;
        case interpreter::IROpCode::Add:      return kAdd;
        case interpreter::IROpCode::Sub:      return kSub;
        case interpreter::IROpCode::Mul:      return kMul;
        case interpreter::IROpCode::Div:      return kDiv;
        case interpreter::IROpCode::Rem:      return kRem;
        case interpreter::IROpCode::And:      return kAnd;
        case interpreter::IROpCode::Or:       return kOr;
        case interpreter::IROpCode::Xor:      return kXor;
        case interpreter::IROpCode::Shl:      return kShl;
        case interpreter::IROpCode::Shr:      return kShr;
        case interpreter::IROpCode::ShrUn:    return kShrUn;
        case interpreter::IROpCode::Ceq:      return kCeq;
        case interpreter::IROpCode::Clt:      return kClt;
        case interpreter::IROpCode::Cgt:      return kCgt;
        case interpreter::IROpCode::LdLen:    return kLdLen;
        case interpreter::IROpCode::LdElem:   return kLdElem;
        case interpreter::IROpCode::LdElemA:  return kLdElemA;
        case interpreter::IROpCode::Call:     return kCall;
        case interpreter::IROpCode::CallVirt: return kCallVirt;
        default: return kNop;
    }
}

// ── Type inference helpers ─────────────────────────────────────────────
static TypeTag ValueTagToTypeTag(uint8_t value_tag) noexcept {
    switch (value_tag) {
        case 1: return kInt32;
        case 2: return kInt64;
        case 3: return kFloat32;
        case 4: return kFloat64;
        case 5: return kObjectRef;
        default: return kInt32;
    }
}

// ── TreeBuilder implementation ─────────────────────────────────────────

TreeBuilder::TreeBuilder() noexcept {
    std::memset(vreg_to_node_, 0, sizeof(vreg_to_node_));
}

TreeBuilder::~TreeBuilder() noexcept {
    std::free(arena_);
}

bool TreeBuilder::EnsureArena(uint32_t bytes) noexcept {
    if (arena_pos_ + bytes <= arena_end_)
        return true;

    // Grow: double until sufficient
    uint32_t needed = arena_size_;
    while (needed < bytes) needed *= 2;
    if (needed < arena_size_ * 2) needed = arena_size_ * 2;

    uint8_t* new_arena = static_cast<uint8_t*>(std::realloc(arena_, needed));
    if (!new_arena) return false;

    uint32_t used = static_cast<uint32_t>(arena_pos_ - arena_);
    arena_ = new_arena;
    arena_pos_ = arena_ + used;
    arena_end_ = arena_ + needed;
    arena_size_ = needed;
    return true;
}

ExprNode* TreeBuilder::AllocNode(NodeKind kind, TypeTag tag,
                                  ExprNode* c0, ExprNode* c1) noexcept {
    if (!EnsureArena(sizeof(ExprNode))) return nullptr;
    auto* n = reinterpret_cast<ExprNode*>(arena_pos_);
    arena_pos_ += sizeof(ExprNode);
    n->kind_vn = static_cast<uint32_t>(kind);
    n->type_tag = static_cast<uint8_t>(tag);
    n->child_count = (c0 ? 1 : 0) + (c1 ? 1 : 0);
    n->flags = 0;
    n->padding_ = 0;
    n->child0 = c0;
    n->child1 = c1;
    return n;
}

ExprNode* TreeBuilder::ResolveVReg(uint32_t vreg, uint8_t type_tag) noexcept {
    if (vreg < 64 && vreg_to_node_[vreg])
        return vreg_to_node_[vreg];
    // Live-in / BB parameter: create LdLoc leaf
    auto* n = AllocNode(kLdLoc, ValueTagToTypeTag(type_tag));
    if (n) {
        n->operand_index = vreg;
        // Give it a VN so CSE can dedup multiple uses of the same param
        uint32_t id = vn_.GetOrCreate(VNKey::Leaf(kLdLoc, vreg));
        n->set_vn_id(id);
        vn_.SetComputed(id);  // LdLoc is always "available"
    }
    return n;
}

TreeBuildResult TreeBuilder::Build(const interpreter::RegisterMethod& rm,
                                    uint32_t lo, uint32_t hi) noexcept {
    if (rm.instructions.empty()) return {};
    return Build(rm.instructions.data(), lo, hi);
}

TreeBuildResult TreeBuilder::Build(const interpreter::RegisterInstruction* instrs,
                                    uint32_t lo, uint32_t hi) noexcept {
    // Reset state
    arena_pos_ = arena_;
    root_count_ = 0;
    has_unsupported_ = false;
    vn_.Clear();
    std::memset(vreg_to_node_, 0, sizeof(vreg_to_node_));
    std::memset(newarr_constant_size_, 0, sizeof(newarr_constant_size_));

    uint32_t count = hi - lo;
    if (count == 0) return {};

    // Phase 1: Forward scan — build expression DAG
    for (uint32_t i = lo; i < hi; ++i) {
        const auto& ri = instrs[i];
        auto opc = ri.op_code();
        NodeKind nk = MapOpcode(opc);
        uint8_t dst = ri.has_dst() ? ri.dst_reg() : 0xFF;
        uint32_t arg_count = ri.is_call() ? ri.call_arg_count() : 0;

        // Side-effect root candidates
        bool is_root = false;

        // ── Handle by instruction category ──────────────────────────
        ExprNode* node = nullptr;
        ExprNode* s1 = nullptr;
        ExprNode* s2 = nullptr;

        // Track NewArr with constant size for LdLen folding
        if (opc == interpreter::IROpCode::NewArr && dst < 64) {
            int32_t const_size = -1;
            // Look backward in the same BB for LdcI4 defining src1_reg
            if (ri.has_src1()) {
                for (int32_t j = static_cast<int32_t>(i) - 1; j >= static_cast<int32_t>(lo); --j) {
                    const auto& prev = instrs[j];
                    if (prev.has_dst() && prev.dst_reg() == ri.src1_reg() &&
                        prev.op_code() == interpreter::IROpCode::LdcI4) {
                        const_size = prev.imm.i4;
                        break;
                    }
                }
            }
            if (const_size > 0 && const_size < 65536) {
                newarr_constant_size_[dst] = static_cast<uint32_t>(const_size);
            }
            // Register dst as a LdLoc leaf (NewArr itself doesn't produce a tree node)
            if (!vreg_to_node_[dst]) {
                auto* leaf = AllocNode(kLdLoc, kObjectRef);
                if (leaf) {
                    leaf->operand_index = dst;
                    uint32_t id = vn_.GetOrCreate(VNKey::Leaf(kLdLoc, dst));
                    leaf->set_vn_id(id);
                    vn_.SetComputed(id);
                    vreg_to_node_[dst] = leaf;
                }
            }
            continue;  // NewArr has no expression node in the DAG
        }

        if (opc == interpreter::IROpCode::StLoc) {
            // Store local: root with value child
            ExprNode* value = ri.has_src1() ? ResolveVReg(ri.src1_reg(), 0) : nullptr;
            node = AllocNode(kStLoc, kVoid, value);
            if (node) {
                node->arg_count = ri.imm.operand_index;  // local vreg (child1/arg_count slot — child1 unused for StLoc)
                node->set_vn_id(vn_.GetOrCreate(VNKey::Leaf(kStLoc, ri.imm.operand_index)));
            }
            is_root = true;
        }
        else if (opc == interpreter::IROpCode::StFld || opc == interpreter::IROpCode::StSFld ||
                 opc == interpreter::IROpCode::LdFld || opc == interpreter::IROpCode::LdSFld) {
            // Field operations not supported in tree IR yet. The field_offset
            // payload in ExprNode overlaps with child0/child1, causing pointer
            // corruption. Abort so OptimizeWithTreeIR falls back to the linear
            // optimizer, preserving original instructions unchanged.
            has_unsupported_ = true;
            break;
        }
        else if (opc == interpreter::IROpCode::Dup) {
            // Copy instruction: dst is an alias of src1's node.  Without this,
            // Dup maps to kNop and is dropped, so a later use of dst resolves to
            // an uninitialized live-in leaf instead of the copied value.
            if (ri.has_src1()) {
                if (uint8_t s1r = ri.src1_reg(); ri.has_dst() && s1r < 64) {
                    ExprNode* src_node = ResolveVReg(s1r, 0);
                    uint8_t dst = ri.dst_reg();
                    vreg_to_node_[dst] = src_node;
                    // Preserve the source VN so CSE dedups dst with its source.
                    if (src_node) vn_.SetComputed(src_node->vn_id());
                }
            }
            is_root = false;
            node = nullptr;
        }
        else if (opc == interpreter::IROpCode::Box || opc == interpreter::IROpCode::Unbox) {
            // Box/Unbox value passthrough for non-escaping boxed values.
            // Without this, Box/Unbox map to kNop and are dropped, so a later use
            // of the unbox dst resolves to an uninitialized live-in leaf.  By
            // aliasing the boxed value to its source (Box) / the boxed value
            // (Unbox), a Box-of-constant immediately consumed by Unbox folds to
            // the constant itself — matching the linear optimizer's Unbox-elim.
            if (ri.has_src1() && ri.has_dst() && ri.src1_reg() < 64) {
                uint8_t box_dst = ri.dst_reg();
                uint8_t box_src = ri.src1_reg();
                // Register src as a leaf first so the value is resolvable.
                ExprNode* val_node = ResolveVReg(box_src, 0);
                vreg_to_node_[box_dst] = val_node;
                if (val_node) vn_.SetComputed(val_node->vn_id());
            }
            is_root = (opc == interpreter::IROpCode::Box);  // Box has an allocation side effect
            node = nullptr;
        }
        else if (opc == interpreter::IROpCode::Ret) {
            // Return: root with optional value child
            ExprNode* val = ri.has_src1() ? ResolveVReg(ri.src1_reg(), 0) : nullptr;
            node = AllocNode(kReturn, kVoid, val);
            is_root = true;
        }
        else if (opc == interpreter::IROpCode::StElem) {
            // Store array element: root with array, index, value
            ExprNode* arr = ri.has_src1() ? ResolveVReg(ri.src1_reg(), 5) : nullptr;
            ExprNode* val = ri.src3_reg() ? ResolveVReg(ri.src3_reg(), 0) : nullptr;
            node = AllocNode(kStElem, kVoid, arr, val);
            if (node) {
                node->operand_index = ri.has_src2() ? ri.src2_reg() : 0;
            }
            is_root = true;
        }
        else if (nk == kCall || nk == kCallVirt) {
            // Call: side-effect root.  Build argument nodes.
            // Arguments are passed in consecutive registers starting at src1_reg.
            ExprNode* call_node = AllocNode(nk, kInt32);
            if (call_node) {
                call_node->call_method_token = static_cast<uint32_t>(
                    reinterpret_cast<uintptr_t>(ri.imm.ptr));
                call_node->call_arg0_vreg = ri.has_src1() ? ri.src1_reg() : 0;
                // For simplicity, store arg_count and first arg reg in payload
                call_node->arg_count = arg_count;
                // Args are implicit — store first arg vreg for intrinsic
                // reconstruction during linearization
            }
            node = call_node;
            is_root = true;

            // Calls also produce a result (dst_reg) if has_dst.
            // Register the call node as dst if applicable.
        }
        else if (nk != kNop && dst < 64) {
            // Pure expression: build with src children
            s1 = nullptr;
            s2 = nullptr;

            if (ri.has_src1())
                s1 = ResolveVReg(ri.src1_reg(), 0);
            if (ri.has_src2())
                s2 = ResolveVReg(ri.src2_reg(), 0);

            // Immediate-carrying opcodes
            if (opc == interpreter::IROpCode::LdcI4) {
                node = AllocNode(nk, kInt32);
                if (node) node->i4 = ri.imm.i4;
            } else if (opc == interpreter::IROpCode::LdcI8) {
                node = AllocNode(nk, kInt64);
                if (node) node->i8 = ri.imm.i8;
            } else if (opc == interpreter::IROpCode::LdcR4) {
                node = AllocNode(nk, kFloat32);
                if (node) node->r4 = ri.imm.r8;  // stored as double in imm, cast to float
            } else if (opc == interpreter::IROpCode::LdcR8) {
                node = AllocNode(nk, kFloat64);
                if (node) node->r8 = ri.imm.r8;
            } else if (opc == interpreter::IROpCode::LdNull) {
                node = AllocNode(nk, kObjectRef);
            } else if (opc == interpreter::IROpCode::LdStr) {
                node = AllocNode(nk, kObjectRef);
                if (node) node->ptr = ri.imm.ptr;
            } else if (opc == interpreter::IROpCode::LdArg) {
                node = AllocNode(nk, kInt32);
                if (node) node->operand_index = ri.imm.operand_index;
            } else if (opc == interpreter::IROpCode::LdLoc) {
                node = AllocNode(nk, kInt32);
                if (node) node->operand_index = ri.imm.operand_index;
            } else {
                // Binary/unary opcode
                node = AllocNode(nk, kInt32, s1, s2);
                // Carry immediate if the opcode has one (e.g., Shl with imm count)
                if (ri.has_imm() && opc != interpreter::IROpCode::LdcI4 &&
                    opc != interpreter::IROpCode::LdcI8) {
                    if (node) node->i4 = ri.imm.i4;
                }
            }
        }

        // Link node to dst vreg and assign VN
        if (node && dst < 64) {
            vreg_to_node_[dst] = node;

            // ── LdLen constant folding ─────────────────────────────
            // If LdLen's array ref is a known NewArr with constant size,
            // fold LdLen → LdcI4 at compile time.
            if (nk == kLdLen && s1 && s1->kind() == kLdLoc) {
                uint32_t arr_vreg = s1->operand_index;
                if (arr_vreg < 64 && newarr_constant_size_[arr_vreg] > 0) {
                    int32_t const_len = static_cast<int32_t>(newarr_constant_size_[arr_vreg]);
                    node->set_kind(kLdcI4);
                    node->type_tag = kInt32;
                    node->child0 = nullptr;
                    node->child_count = 0;
                    node->i4 = const_len;
                    nk = kLdcI4;
                }
            }

            // Assign VN (calls get unique VNs — never CSE'd)
            if (nk == kCall || nk == kCallVirt) {
                node->set_vn_id(vn_.GetOrCreate(
                    VNKey::Leaf(nk, reinterpret_cast<int64_t>(ri.imm.ptr))));
            } else if (nk == kLdcI4) {
                node->set_vn_id(vn_.GetOrCreate(VNKey::Leaf(nk, node->i4)));
            } else if (nk == kLdcI8) {
                node->set_vn_id(vn_.GetOrCreate(VNKey::Leaf(nk, node->i8)));
            } else if (nk == kLdArg) {
                node->set_vn_id(vn_.GetOrCreate(VNKey::Leaf(nk, node->operand_index)));
            } else if (nk == kLdLoc) {
                node->set_vn_id(vn_.GetOrCreate(VNKey::Leaf(nk, node->operand_index)));
            } else if (nk >= kNeg && nk <= kLdLen) {
                // Unary: include src VN
                uint32_t svn = s1 ? s1->vn_id() : 0;
                node->set_vn_id(vn_.GetOrCreate(VNKey::Unary(nk, svn)));
            } else if (nk >= kAdd && nk <= kCgtUn) {
                // Binary: include both src VNs
                uint32_t sv1 = s1 ? s1->vn_id() : 0;
                uint32_t sv2 = s2 ? s2->vn_id() : 0;
                node->set_vn_id(vn_.GetOrCreate(VNKey::Binary(nk, sv1, sv2)));
            } else if (nk == kLdElem || nk == kLdElemA) {
                // LdElem/LdElemA: binary with array VN + index VN
                uint32_t sv1 = s1 ? s1->vn_id() : 0;
                uint32_t sv2 = s2 ? s2->vn_id() : 0;
                node->set_vn_id(vn_.GetOrCreate(VNKey::Binary(nk, sv1, sv2)));
            }
        }

        // Collect side-effect roots
        if (node && is_root) {
            if (root_count_ < kMaxRoots)
                roots_[root_count_++] = node;
        }
    }

    // Check for unsupported instructions (field ops, etc.) — abort the tree
    // IR build so OptimizeWithTreeIR falls through to the linear optimizer.
    if (has_unsupported_) return {};

    // Build result
    TreeBuildResult result;
    result.root_count = root_count_;
    result.roots = (root_count_ > 0) ? roots_ : nullptr;
    result.first_node = reinterpret_cast<ExprNode*>(arena_);
    result.arena = arena_;
    return result;
}

// ── Basic block analysis ──────────────────────────────────────────────

std::vector<BBRange> FindBasicBlocks(
    const interpreter::RegisterInstruction* instrs,
    uint32_t count) noexcept
{
    std::vector<BBRange> bbs;
    uint32_t bb_start = 0;

    for (uint32_t i = 0; i < count; ++i) {
        auto opc = instrs[i].op_code();

        // Terminators that end a BB
        bool is_terminator = (opc == interpreter::IROpCode::Br ||
                              opc == interpreter::IROpCode::BrTrue ||
                              opc == interpreter::IROpCode::BrFalse ||
                              opc == interpreter::IROpCode::Beq ||
                              opc == interpreter::IROpCode::BneUn ||
                              opc == interpreter::IROpCode::Blt ||
                              opc == interpreter::IROpCode::BltUn ||
                              opc == interpreter::IROpCode::Bgt ||
                              opc == interpreter::IROpCode::BgtUn ||
                              opc == interpreter::IROpCode::Ble ||
                              opc == interpreter::IROpCode::BleUn ||
                              opc == interpreter::IROpCode::Bge ||
                              opc == interpreter::IROpCode::BgeUn ||
                              opc == interpreter::IROpCode::Ret ||
                              opc == interpreter::IROpCode::Throw ||
                              opc == interpreter::IROpCode::Rethrow ||
                              opc == interpreter::IROpCode::Switch ||
                              opc == interpreter::IROpCode::Leave ||
                              opc == interpreter::IROpCode::EndFinally ||
                              opc == interpreter::IROpCode::EndFilter);

        if (is_terminator) {
            bbs.push_back({bb_start, i + 1});
            bb_start = i + 1;
        }
    }

    // Trailing BB (fall-through after last terminator)
    if (bb_start < count)
        bbs.push_back({bb_start, count});

    return bbs;
}

}  // namespace chaos::il2cpp::jit::tree
