// jit_tree_node.h — Lightweight expression tree IR for single-BB optimization
//
// Tree nodes form a DAG (not just a tree) because CSE may reuse sub-expressions
// across parents.  All nodes are allocated from a BumpArena and live for the
// duration of a single basic-block optimization pass — no per-node free needed.
//
// Node layout (16 bytes base + child pointers):
//   [0..3]   kind | vn_id (packed)
//   [4]      type_tag
//   [5..7]   padding
//   [8..15]  child0 / payload (void* or int64)
//   [16..23] child1 (for binary nodes)
// Total: 16 bytes (leaf) to 24 bytes (binary/call with inline args).

#ifndef CHAOS_IL2CPP_JIT_TREE_NODE_H_
#define CHAOS_IL2CPP_JIT_TREE_NODE_H_

#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::jit::tree {

// ── Node kind enumeration ───────────────────────────────────────────────
enum NodeKind : uint8_t {
    // Leaf nodes (no children)
    kNop       = 0,   // no-op
    kLdcI4     = 1,   // int32 constant
    kLdcI8     = 2,   // int64 constant
    kLdcR4     = 3,   // float constant
    kLdcR8     = 4,   // double constant
    kLdNull    = 5,   // null reference
    kLdArg     = 6,   // method argument by index
    kLdLoc     = 7,   // local variable by vreg
    kLdStr     = 8,   // string constant (pointer)

    // Unary nodes (1 child)
    kNeg       = 16,  // arithmetic negate
    kNot       = 17,  // bitwise not
    kConvI4    = 18,  // convert to int32
    kConvI8    = 19,  // convert to int64
    kConvR4    = 20,  // convert to float
    kConvR8    = 21,  // convert to double
    kConvU4    = 22,  // unsigned to int32
    kConvU8    = 23,  // unsigned to int64
    kLdLen     = 24,  // array length
    kBox       = 25,  // box value type
    kUnbox     = 26,  // unbox to value type

    // Binary nodes (2 children)
    kAdd       = 32,
    kSub       = 33,
    kMul       = 34,
    kDiv       = 35,
    kRem       = 36,
    kAnd       = 37,
    kOr        = 38,
    kXor       = 39,
    kShl       = 40,
    kShr       = 41,
    kShrUn     = 42,
    kCeq       = 43,  // compare equal → int32
    kClt       = 44,  // compare less-than (signed) → int32
    kCgt       = 45,  // compare greater-than (signed) → int32
    kCltUn     = 46,  // compare less-than (unsigned) → int32
    kCgtUn     = 47,  // compare greater-than (unsigned) → int32

    // Ternary / special nodes
    kLdFld     = 48,  // instance field load: (object, field_offset)
    kLdSFld    = 49,  // static field load: (field_offset, unused)
    kCall      = 50,  // method call: (method_token, args[])
    kCallVirt  = 51,  // virtual call: (method_token, this, args[])

    // Intrinsic nodes (Gap 3)
    kAbs       = 52,  // absolute value (unary, int32)
    kMin       = 53,  // minimum (binary, int32)
    kMax       = 54,  // maximum (binary, int32)

    // Store / side-effect nodes (roots in the DAG)
    kStLoc     = 64,  // local store: (value, local_vreg)
    kStFld     = 65,  // field store: (object, value, field_offset)
    kStElem    = 66,  // array element store: (array, index, value)
    kReturn    = 67,  // return: (value or null)
};

// ── Type tag (mirrors ValueTag from interpreter) ────────────────────────
enum TypeTag : uint8_t {
    kVoid     = 0,
    kInt32    = 1,
    kInt64    = 2,
    kFloat32  = 3,
    kFloat64  = 4,
    kObjectRef = 5,
    kManagedPtr = 6,
};

// ── Node structure — tagged union via kind discriminator ────────────────
// Layout:
//   offset 0: kind (1 byte)
//   offset 1: vn_id_lo (3 bytes, little-endian)
//   offset 4: type_tag (1 byte)
//   offset 5: child_count (1 byte)
//   offset 6: flags (1 byte)
//   offset 7: padding
//   offset 8: payload / child0 (8 bytes)
//  offset 16: child1 (8 bytes)
struct ExprNode {
    // Packed header: kind in bits [0..7], vn_id in bits [8..31]
    uint32_t kind_vn;     // kind:8 | vn_id:24
    uint8_t  type_tag;    // TypeTag
    uint8_t  child_count; // number of children (0, 1, or 2)
    uint8_t  flags;       // reserved for future use
    uint8_t  padding_;    // explicit padding

    // Child pointers / payload (mutually exclusive with children)
    union {
        // When child_count == 0: payload is the leaf value
        int32_t  i4;        // LdcI4
        int64_t  i8;        // LdcI8
        float    r4;        // LdcR4
        double   r8;        // LdcR8
        uint32_t operand_index;  // LdArg(index), LdLoc(vreg)
        uint32_t field_offset;   // LdFld/StFld offset
        uint32_t method_token;   // Call token
        uint32_t module_id;      // Call module id
        void*    ptr;            // LdStr: string pointer

        // For nodes with children
        ExprNode* child0;

        // Call payload: method_token + first arg vreg packed in 8 bytes
        // (method_token [8-11], call_arg0_vreg [12-15])
        struct {
            uint32_t call_method_token;
            uint32_t call_arg0_vreg;
        };
    };

    union {
        ExprNode* child1;   // second child (binary nodes)
        uint32_t  arg_count; // Call argument count (if child_count > 2)
    };

    // ── Accessors ────────────────────────────────────────────────────
    inline NodeKind kind() const noexcept {
        return static_cast<NodeKind>(kind_vn & 0xFF);
    }
    inline uint32_t vn_id() const noexcept {
        return kind_vn >> 8;
    }
    inline void set_kind(NodeKind k) noexcept {
        kind_vn = (kind_vn & 0xFFFFFF00) | static_cast<uint32_t>(k);
    }
    inline void set_vn_id(uint32_t id) noexcept {
        kind_vn = (kind_vn & 0xFF) | (id << 8);
    }
};

static_assert(sizeof(ExprNode) <= 24,
    "ExprNode should fit in 24 bytes for cache efficiency");

static_assert(sizeof(ExprNode::call_method_token) + sizeof(ExprNode::call_arg0_vreg) <= 8,
    "call_method_token + call_arg0_vreg should fit in the 8-byte child0 union slot");

// ── Factory helpers (used by TreeBuilder) ──────────────────────────────

/// Allocate an uninitialized ExprNode from the arena.
/// Caller must fill kind/type_tag/payload after allocation.
inline ExprNode* AllocNode(uint8_t*& arena_pos, uint8_t* arena_end,
                           NodeKind kind, TypeTag type_tag,
                           ExprNode* c0 = nullptr, ExprNode* c1 = nullptr) noexcept {
    if (arena_pos + sizeof(ExprNode) > arena_end) return nullptr;
    auto* n = reinterpret_cast<ExprNode*>(arena_pos);
    arena_pos += sizeof(ExprNode);
    n->kind_vn = static_cast<uint32_t>(kind);  // vn_id = 0 initially
    n->type_tag = static_cast<uint8_t>(type_tag);
    n->flags = 0;
    n->padding_ = 0;
    n->child0 = c0;
    n->child1 = c1;
    n->child_count = (c0 ? 1 : 0) + (c1 ? 1 : 0);
    return n;
}

/// Allocate a leaf node with an int32 payload.
inline ExprNode* LeafI4(uint8_t*& pos, uint8_t* end,
                         int32_t val, TypeTag tag = kInt32) noexcept {
    auto* n = AllocNode(pos, end, kLdcI4, tag);
    if (n) n->i4 = val;
    return n;
}

/// Allocate a leaf node with an int64 payload.
inline ExprNode* LeafI8(uint8_t*& pos, uint8_t* end,
                         int64_t val, TypeTag tag = kInt64) noexcept {
    auto* n = AllocNode(pos, end, kLdcI8, tag);
    if (n) n->i8 = val;
    return n;
}

/// Allocate a leaf node referencing an argument or local vreg.
inline ExprNode* LeafArg(uint8_t*& pos, uint8_t* end,
                          uint32_t idx, TypeTag tag = kInt32) noexcept {
    auto* n = AllocNode(pos, end, kLdArg, tag);
    if (n) n->operand_index = idx;
    return n;
}

inline ExprNode* LeafLoc(uint8_t*& pos, uint8_t* end,
                          uint32_t vreg, TypeTag tag = kInt32) noexcept {
    auto* n = AllocNode(pos, end, kLdLoc, tag);
    if (n) n->operand_index = vreg;
    return n;
}

/// Allocate a unary node.
inline ExprNode* Unary(uint8_t*& pos, uint8_t* end,
                        NodeKind kind, ExprNode* src,
                        TypeTag tag = kInt32) noexcept {
    return AllocNode(pos, end, kind, tag, src);
}

/// Allocate a binary node.
inline ExprNode* Binary(uint8_t*& pos, uint8_t* end,
                         NodeKind kind, ExprNode* s1, ExprNode* s2,
                         TypeTag tag = kInt32) noexcept {
    return AllocNode(pos, end, kind, tag, s1, s2);
}

}  // namespace chaos::il2cpp::jit::tree

#endif  // CHAOS_IL2CPP_JIT_TREE_NODE_H_
