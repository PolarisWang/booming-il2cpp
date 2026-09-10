// jit_vn.h — Value numbering table for CSE detection
//
// Maps expression descriptors (opcode + operand VNs + immediate) to canonical
// value numbers.  Two expressions with the same VN are guaranteed to produce
// the same value, enabling CSE elimination.
//
// Thread safety: not thread-safe.  A new VNTable is created per basic block.
//
// Usage:
//   VNTable vn;
//   uint32_t id1 = vn.GetOrCreate(VNKey::Binary(kAdd, vn_of_a, vn_of_b));
//   uint32_t id2 = vn.GetOrCreate(VNKey::Binary(kAdd, vn_of_a, vn_of_b));
//   assert(id1 == id2);  // same expression → same VN

#ifndef CHAOS_IL2CPP_JIT_VN_H_
#define CHAOS_IL2CPP_JIT_VN_H_

#include "tree/jit_tree_node.h"

#include <cstdint>
#include <cstring>

#include <chaos/unordered_dense.h>

namespace chaos::il2cpp::jit::tree {

// ── VNKey: descriptor for value numbering ──────────────────────────────
// Encodes (kind, vn1, vn2, imm_lo32) into two 64-bit words.
struct VNKey {
    uint64_t word0; // kind(8) : vn1(24) : vn2(24) : reserved(8)
    uint64_t word1; // immediate value (full 64 bits, 0 for most ops)

    static VNKey Leaf(NodeKind kind, int64_t imm = 0) noexcept {
        VNKey k;
        k.word0 = static_cast<uint64_t>(kind);
        k.word1 = static_cast<uint64_t>(imm);
        return k;
    }

    static VNKey Unary(NodeKind kind, uint32_t vn_src) noexcept {
        VNKey k;
        k.word0 = static_cast<uint64_t>(kind) | (static_cast<uint64_t>(vn_src & 0xFFFFFF) << 8);
        k.word1 = 0;
        return k;
    }

    static VNKey Binary(NodeKind kind, uint32_t vn1, uint32_t vn2, uint32_t imm32 = 0) noexcept {
        VNKey k;
        k.word0 = static_cast<uint64_t>(kind) | (static_cast<uint64_t>(vn1 & 0xFFFFFF) << 8) |
                  (static_cast<uint64_t>(vn2 & 0xFFFFFF) << 32);
        k.word1 = imm32;
        return k;
    }

    bool operator==(const VNKey& o) const noexcept { return word0 == o.word0 && word1 == o.word1; }
};

static_assert(sizeof(VNKey) == 16, "VNKey should be 16 bytes");

struct VNKeyHash {
    inline size_t operator()(const VNKey& k) const noexcept {
        // Simple xor-based mix — good enough for VNKey's structured data
        return static_cast<size_t>(k.word0 ^ (k.word1 * 0x9E3779B97F4A7C15ULL));
    }
};

// ── VNTable ────────────────────────────────────────────────────────────
class VNTable {
public:
    VNTable() noexcept = default;
    VNTable(const VNTable&) = delete;
    VNTable& operator=(const VNTable&) = delete;

    /// Get or create a value number for the given expression descriptor.
    /// Returns the canonical vn_id.  Consecutive calls with the same key
    /// return the same vn_id.
    uint32_t GetOrCreate(const VNKey& key) noexcept {
        auto it = map_.find(key);
        if (it != map_.end())
            return it->second;

        uint32_t id = next_vn_++;
        map_.emplace(key, id);
        return id;
    }

    /// Check whether a value number has been computed (i.e., its defining
    /// expression has been emitted).  Used by CSE to decide whether to
    /// replace a redundant sub-expression with a reference to the earlier
    /// computation's result register.
    bool IsComputed(uint32_t vn_id) const noexcept {
        if (vn_id >= computed_.size())
            return false;
        return computed_[vn_id];
    }

    /// Mark a value number as computed.  Called after the TreeBuilder
    /// identifies which expression defines this VN.
    void SetComputed(uint32_t vn_id) noexcept {
        if (vn_id >= computed_.size())
            computed_.resize(vn_id + 1, false);
        computed_[vn_id] = true;
    }

    /// Check if a VN is known at all (has been created).
    bool HasVN(uint32_t vn_id) const noexcept { return vn_id < next_vn_; }

    /// Total number of distinct value numbers created.
    uint32_t Count() const noexcept { return next_vn_; }

    /// Reset table state (equivalent to constructing a fresh VNTable).
    void Clear() noexcept {
        map_.clear();
        next_vn_ = 1;
        computed_.clear();
    }

private:
    using Map = ankerl::unordered_dense::map<VNKey, uint32_t, VNKeyHash>;
    Map map_;

    uint32_t next_vn_ = 1;       // 0 reserved for "unassigned"
    std::vector<bool> computed_; // indexed by vn_id
};

} // namespace chaos::il2cpp::jit::tree

#endif // CHAOS_IL2CPP_JIT_VN_H_
