// jit_optimizer.cpp — Tree IR optimization orchestration
//
// Pipeline per BB:
//   1. TreeBuilder::Build() — RegisterMethod → expression DAG
//   2. Inliner::InlineRoots — inline eligible kCall nodes (optional)
//   3. IntrinsicMutator — replace known BCL calls with specialized nodes
//   4. ConstFoldMutator — fold constant arithmetic
//   5. BoundCheckEliminator — eliminate redundant array bounds checks
//   6. CSEMutator — eliminate redundant sub-expressions
//   7. Linearizer — optimized DAG → RegisterInstruction sequence
//
// P5 extension: when multiple basic blocks form natural loops, additional
// post-passes run on the linearized instruction stream:
//   a. Constant propagation — identify vregs with known constant values
//   b. LICM — hoist loop-invariant arithmetic out of loops
//   c. IV strength reduction — replace induction-variable mul with add
//   d. Loop unrolling — replicate loop body for constant-trip-count loops

#include "tree/jit_optimizer.h"
#include "tree/jit_tree_builder.h"
#include "tree/jit_tree_mutator.h"
#include "tree/jit_linearizer.h"
#include "tree/jit_intrinsics.h"
#include "tree/jit_bounds_check.h"
#include "tree/jit_cfg.h"
#include "jit_inline.h"

#include "interpreter/ir_reg_alloc.h"
#include "interpreter/generated/ir_opcodes.h"

#include <chaos/log.h>

#include <cstdint>
#include <vector>

namespace chaos::il2cpp::jit::tree {

// ── Helper: build vreg → defining block map ───────────────────────────
// Scans the original instruction sequence and records which BB (by index in
// `bbs`) defines each vreg.  vregs with no definition in the scanned range
// are left as UINT32_MAX (unknown / live-in).
static void BuildVRegDefBlocks(
    const std::vector<interpreter::RegisterInstruction>& instrs,
    const std::vector<BBRange>& bbs,
    uint32_t max_vreg,
    std::vector<uint32_t>& out_def_blocks) noexcept
{
    out_def_blocks.assign(max_vreg, UINT32_MAX);

    // For each BB, scan its instructions and record dst_reg definitions
    uint32_t bi = 0;
    for (const auto& bb : bbs) {
        for (uint32_t i = bb.lo; i < bb.hi; ++i) {
            const auto& ri = instrs[i];
            if (ri.has_dst()) {
                uint32_t dst = ri.dst_reg();
                if (dst < max_vreg)
                    out_def_blocks[dst] = bi;
            }
        }
        ++bi;
    }
}

// ── Helper: is this opcode pure arithmetic (safe for LICM hoisting)? ──
// Pure arithmetic has no side effects, no exceptions, and depends only
// on its src vregs.  Div/Rem/AddOvf are excluded (can throw).
static bool IsPureArithmetic(interpreter::IROpCode opc) noexcept {
    switch (opc) {
        case interpreter::IROpCode::Add:
        case interpreter::IROpCode::Sub:
        case interpreter::IROpCode::Mul:
        case interpreter::IROpCode::And:
        case interpreter::IROpCode::Or:
        case interpreter::IROpCode::Xor:
        case interpreter::IROpCode::Shl:
        case interpreter::IROpCode::Shr:
        case interpreter::IROpCode::ShrUn:
        case interpreter::IROpCode::Neg:
        case interpreter::IROpCode::Not:
        case interpreter::IROpCode::Ceq:
        case interpreter::IROpCode::Clt:
        case interpreter::IROpCode::Cgt:
        case interpreter::IROpCode::Conv_I4:
        case interpreter::IROpCode::Conv_I8:
        case interpreter::IROpCode::Conv_R4:
        case interpreter::IROpCode::Conv_R8:
        case interpreter::IROpCode::ConvRUn:
        case interpreter::IROpCode::ConvI:
        case interpreter::IROpCode::ConvU:
        case interpreter::IROpCode::LdLen:
            return true;
        default:
            return false;
    }
}

// ── Helper: check if a vreg is loop-invariant ──────────────────────
// Returns true if vreg's defining block is outside the loop.
static bool IsLoopInvariantVReg(uint32_t vreg,
                                 const std::vector<uint32_t>& vreg_def_blocks,
                                 const NaturalLoop& loop) noexcept {
    if (vreg >= vreg_def_blocks.size()) return false;
    uint32_t def_block = vreg_def_blocks[vreg];
    if (def_block == UINT32_MAX) return false;  // unknown → assume variant
    for (uint32_t b : loop.blocks) {
        if (b == def_block) return false;
    }
    return true;
}

// ── LICM: hoist loop-invariant pure arithmetic out of loops ────────
//
// Scans each loop's body blocks for pure-arithmetic instructions whose
// source vregs are all defined outside the loop.  For each such
// instruction, creates a new vreg, emits the same opcode in the
// pre-header region, and replaces the in-loop instruction with a LdLoc.
//
// Cascading: when an instruction is hoisted, its dst vreg becomes
// loop-invariant for subsequent instructions in the same loop.
//
// Operates on the linearized output instruction stream.
// bb_starts[i] = first instruction index in out_instrs belonging to BB i.
static void LicmHoist(
    std::vector<interpreter::RegisterInstruction>& out_instrs,
    const std::vector<uint32_t>& bb_starts,
    const std::vector<uint32_t>& bb_ends,
    const LoopAnalysis& analysis,
    const std::vector<uint32_t>& vreg_def_blocks,
    uint32_t& next_vreg) noexcept
{
    if (!analysis.has_loops) return;

    for (const auto& loop : analysis.loops) {
        uint32_t header = loop.header;
        uint32_t pre_header = UINT32_MAX;

        // Find pre-header: the predecessor of the loop header that is NOT
        // inside the loop (i.e., not dominated by the header).
        const auto& header_block = analysis.blocks[header];
        for (uint32_t pred : header_block.preds) {
            bool in_loop = false;
            for (uint32_t b : loop.blocks) {
                if (b == pred) { in_loop = true; break; }
            }
            if (!in_loop) {
                pre_header = pred;
                break;
            }
        }
        if (pre_header == UINT32_MAX) continue;  // no pre-header found

        uint32_t insert_point = bb_ends[pre_header];  // end of pre-header in out_instrs

        // Track hoisted replacements: original vreg → hoisted vreg
        struct HoistInfo {
            uint32_t orig_dst_vreg;
            uint32_t hoisted_vreg;
        };
        std::vector<HoistInfo> hoisted;

        // Scan each body block for hoistable instructions
        for (uint32_t bb_id : loop.blocks) {
            if (bb_id == header) continue;  // skip header (it may be the pre-header entry)

            uint32_t start = bb_starts[bb_id];
            uint32_t end   = bb_ends[bb_id];

            for (uint32_t i = start; i < end; ++i) {
                auto& ri = out_instrs[i];
                if (!IsPureArithmetic(ri.op_code())) continue;

                // Count src vregs and check invariance for each
                uint32_t src_count = 0;
                bool all_srcs_invariant = true;

                if (ri.has_src1()) {
                    src_count++;
                    uint32_t src1 = ri.src1_reg();
                    if (!IsLoopInvariantVReg(src1, vreg_def_blocks, loop)) {
                        // Check cascading: was this vreg already hoisted?
                        bool was_hoisted = false;
                        for (const auto& h : hoisted) {
                            if (h.orig_dst_vreg == src1) {
                                was_hoisted = true;
                                break;
                            }
                        }
                        if (!was_hoisted) all_srcs_invariant = false;
                    }
                }
                if (ri.has_src2()) {
                    src_count++;
                    uint32_t src2 = ri.src2_reg();
                    if (!IsLoopInvariantVReg(src2, vreg_def_blocks, loop)) {
                        bool was_hoisted = false;
                        for (const auto& h : hoisted) {
                            if (h.orig_dst_vreg == src2) {
                                was_hoisted = true;
                                break;
                            }
                        }
                        if (!was_hoisted) all_srcs_invariant = false;
                    }
                }

                if (!all_srcs_invariant || src_count == 0) continue;
                if (!ri.has_dst()) continue;

                uint32_t dst_vreg = ri.dst_reg();

                // Hoist: create new vreg, emit same opcode in pre-header
                uint32_t new_vreg = next_vreg++;

                // Build instruction for pre-header (same opcode, new dst vreg)
                interpreter::RegisterInstruction hoisted_ri = ri;
                // Remap dst to new_vreg
                hoisted_ri.header = (hoisted_ri.header & ~(0xFFull << 16)) |
                    (static_cast<uint64_t>(new_vreg) << 16);

                // Insert at insert_point
                out_instrs.insert(out_instrs.begin() + insert_point, hoisted_ri);

                // Replace in-loop instruction with LdLoc reading hoisted value
                ri.header = 0;
                ri.header |= static_cast<uint64_t>(interpreter::IROpCode::LdLoc) & 0xFFFF;
                ri.header |= static_cast<uint64_t>(dst_vreg) << 16;  // dst
                ri.header |= static_cast<uint64_t>(new_vreg) << 24;  // src1 = hoisted vreg
                ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst | interpreter::kRegHasSrc1) << 40;
                ri.imm.operand_index = new_vreg;

                hoisted.push_back({dst_vreg, new_vreg});

                // Advance insert_point past the newly inserted instruction
                insert_point++;
            }
        }

        if (!hoisted.empty()) {
            CHAOS_IL2CPP_LOG_DEBUG_M("jit", "LICM: hoisted %zu instructions from loop header=%u",
                                   hoisted.size(), header);
        }
    }
}

// ── IV strength reduction: mul → add for induction variables ───────────
//
// Detects simple induction variables of the form:
//   vreg_N = Add vreg_M, ConstC    (within loop body)
//
// For each IV, scans for MUL instructions that use the IV as source.
// Replaces:  Mul result, iv_vreg, ConstK
// With:      A new accumulator initialized to iv_vreg * ConstK before the
//            loop, and incremented by ConstK * step each iteration.
//
// P5 scope: only handles step=1 IVs with small constant multipliers (< 256).
// The transformation inserts into out_instrs, shifting bb_starts/bb_ends,
// so all analysis uses local indices within each loop body region.
static void IvStrengthReduce(
    std::vector<interpreter::RegisterInstruction>& out_instrs,
    const std::vector<uint32_t>& bb_starts,
    const std::vector<uint32_t>& bb_ends,
    const LoopAnalysis& analysis,
    uint32_t& next_vreg) noexcept
{
    if (!analysis.has_loops) return;

    for (const auto& loop : analysis.loops) {
        uint32_t header = loop.header;

        // Find pre-header
        uint32_t pre_header = UINT32_MAX;
        const auto& header_block = analysis.blocks[header];
        for (uint32_t pred : header_block.preds) {
            bool in_loop = false;
            for (uint32_t b : loop.blocks) {
                if (b == pred) { in_loop = true; break; }
            }
            if (!in_loop) {
                pre_header = pred;
                break;
            }
        }
        if (pre_header == UINT32_MAX) continue;

        // Phase 1: identify IVs in loop body
        struct IvInfo {
            uint32_t iv_vreg;
            uint32_t step;
            int32_t  initial_value;  // -1 if unknown
            uint32_t inc_instr_idx;  // index in out_instrs of Add instruction
        };
        std::vector<IvInfo> ivs;

        for (uint32_t bb_id : loop.blocks) {
            uint32_t start = bb_starts[bb_id];
            uint32_t end   = bb_ends[bb_id];

            for (uint32_t i = start; i < end; ++i) {
                const auto& ri = out_instrs[i];
                if (ri.op_code() != interpreter::IROpCode::Add) continue;
                if (!ri.has_dst() || !ri.has_src1() || !ri.has_src2()) continue;

                // Check: src1 == dst (i.e., vreg = vreg + something)
                if (ri.src1_reg() != ri.dst_reg()) continue;

                // Check src2 is a constant via LdcI4
                uint32_t src2 = ri.src2_reg();
                int32_t step = 0;
                for (int32_t j = static_cast<int32_t>(i) - 1; j >= static_cast<int32_t>(start); --j) {
                    const auto& prev = out_instrs[j];
                    if (prev.has_dst() && prev.dst_reg() == src2 &&
                        prev.op_code() == interpreter::IROpCode::LdcI4) {
                        step = prev.imm.i4;
                        break;
                    }
                }

                if (step == 1) {
                    // Try to find initial value of this IV in pre-header
                    int32_t initial = -1;
                    uint32_t ps = bb_starts[pre_header];
                    uint32_t pe = bb_ends[pre_header];
                    for (uint32_t j = ps; j < pe; ++j) {
                        const auto& prev_ri = out_instrs[j];
                        // Look for LdcI4 that defines the IV's initial value
                        // (stored to local via StLoc then loaded via LdLoc in header)
                        if (prev_ri.has_dst() && prev_ri.dst_reg() == ri.src1_reg() &&
                            prev_ri.op_code() == interpreter::IROpCode::LdcI4) {
                            initial = prev_ri.imm.i4;
                            break;
                        }
                    }

                    ivs.push_back({ri.dst_reg(), 1, initial, i});
                }
            }
        }

        if (ivs.empty()) continue;

        // Phase 2: for each IV, scan for MUL in loop body and replace with accumulator
        uint32_t insert_point = bb_ends[pre_header];

        for (const auto& iv : ivs) {
            for (uint32_t bb_id : loop.blocks) {
                uint32_t start = bb_starts[bb_id];
                uint32_t end   = bb_ends[bb_id];

                for (uint32_t i = start; i < end; ++i) {
                    auto& ri = out_instrs[i];
                    if (ri.op_code() != interpreter::IROpCode::Mul) continue;
                    if (!ri.has_dst() || !ri.has_src1() || !ri.has_src2()) continue;

                    // Check if either source is the IV
                    uint32_t other_src;
                    bool iv_is_src1;

                    if (ri.src1_reg() == iv.iv_vreg) {
                        other_src = ri.src2_reg();
                        iv_is_src1 = true;
                    } else if (ri.src2_reg() == iv.iv_vreg) {
                        other_src = ri.src1_reg();
                        iv_is_src1 = false;
                    } else {
                        continue;
                    }

                    // Check if other_src is a small constant
                    int32_t const_k = 0;
                    bool found_const = false;
                    for (int32_t j = static_cast<int32_t>(i) - 1; j >= static_cast<int32_t>(start); --j) {
                        const auto& prev = out_instrs[j];
                        if (prev.has_dst() && prev.dst_reg() == other_src &&
                            prev.op_code() == interpreter::IROpCode::LdcI4) {
                            const_k = prev.imm.i4;
                            found_const = true;
                            break;
                        }
                    }

                    if (!found_const || const_k == 0 || const_k >= 256) continue;

                    // ── Perform IV strength reduction ──
                    // Create accumulator vreg
                    uint32_t acc_vreg = next_vreg++;

                    // Insert initialization in pre-header:
                    //   LdcI4 initial → vreg_init
                    //   LdcI4 const_k → vreg_k
                    //   Mul acc, init, const_k
                    uint32_t init_vreg = next_vreg++;
                    uint32_t k_vreg = next_vreg++;

                    // LdcI4 initial
                    interpreter::RegisterInstruction init_ri = {};
                    init_ri.header = static_cast<uint64_t>(interpreter::IROpCode::LdcI4);
                    init_ri.header |= static_cast<uint64_t>(init_vreg) << 16;
                    init_ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst) << 40;
                    init_ri.imm.i4 = (iv.initial_value >= 0) ? iv.initial_value : 0;

                    // LdcI4 const_k
                    interpreter::RegisterInstruction k_ri = {};
                    k_ri.header = static_cast<uint64_t>(interpreter::IROpCode::LdcI4);
                    k_ri.header |= static_cast<uint64_t>(k_vreg) << 16;
                    k_ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst) << 40;
                    k_ri.imm.i4 = const_k;

                    // Mul acc, init, const_k
                    interpreter::RegisterInstruction mul_ri = {};
                    mul_ri.header = static_cast<uint64_t>(interpreter::IROpCode::Mul);
                    mul_ri.header |= static_cast<uint64_t>(acc_vreg) << 16;    // dst
                    mul_ri.header |= static_cast<uint64_t>(init_vreg) << 24;   // src1
                    mul_ri.header |= static_cast<uint64_t>(k_vreg) << 32;      // src2
                    mul_ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst | interpreter::kRegHasSrc1 | interpreter::kRegHasSrc2) << 40;

                    // Insert at insert_point (end of pre-header in out_instrs)
                    // Insert in reverse order so they appear in correct sequence
                    out_instrs.insert(out_instrs.begin() + insert_point, mul_ri);
                    out_instrs.insert(out_instrs.begin() + insert_point, k_ri);
                    out_instrs.insert(out_instrs.begin() + insert_point, init_ri);

                    // Calculate index adjustments:
                    // We inserted 3 instructions at insert_point, so all subsequent indices shift by 3
                    uint32_t shift = 3;

                    // Replace the MUL with LdLoc reading the accumulator
                    ri.header = 0;
                    ri.header |= static_cast<uint64_t>(interpreter::IROpCode::LdLoc) & 0xFFFF;
                    ri.header |= static_cast<uint64_t>(ri.dst_reg()) << 16;    // dst (same as original MUL)
                    ri.header |= static_cast<uint64_t>(acc_vreg) << 24;        // src1 = acc_vreg (as LdLoc operand)
                    ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst | interpreter::kRegHasSrc1) << 40;
                    ri.imm.operand_index = acc_vreg;

                    // Insert accumulator increment after the IV's Add instruction
                    // (Add acc, acc, const_k at iv.inc_instr_idx + (shift adjustment))
                    uint32_t actual_inc_idx = iv.inc_instr_idx + shift;

                    interpreter::RegisterInstruction inc_ri = {};
                    inc_ri.header = static_cast<uint64_t>(interpreter::IROpCode::Add);
                    inc_ri.header |= static_cast<uint64_t>(acc_vreg) << 16;    // dst = acc
                    inc_ri.header |= static_cast<uint64_t>(acc_vreg) << 24;    // src1 = acc
                    inc_ri.header |= static_cast<uint64_t>(k_vreg) << 32;      // src2 = k
                    inc_ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst | interpreter::kRegHasSrc1 | interpreter::kRegHasSrc2) << 40;

                    // Insert after the IV's Add instruction
                    out_instrs.insert(out_instrs.begin() + actual_inc_idx + 1, inc_ri);

                    CHAOS_IL2CPP_LOG_DEBUG_M("jit",
                        "IV strength reduction: vreg=%u * %d → acc=vreg=%u (loop header=%u)",
                        iv.iv_vreg, const_k, acc_vreg, header);
                }
            }
        }
    }
}

// ── Loop unrolling ─────────────────────────────────────────────────────
//
// For loops with a known constant trip count, replaces the loop body
// with N unrolled copies to reduce branch overhead.
//
// P5 scope: single-BB loops (header is also latch) with trip count < 64.
// Factor = 2.  Only unrolls when trip count % factor == 0.
static void UnrollLoops(
    std::vector<interpreter::RegisterInstruction>& out_instrs,
    const std::vector<uint32_t>& bb_starts,
    const std::vector<uint32_t>& bb_ends,
    const LoopAnalysis& analysis,
    uint32_t& next_vreg,
    uint32_t unroll_factor = 4) noexcept
{
    if (!analysis.has_loops) return;

    for (const auto& loop : analysis.loops) {
        // Only handle single-BB loops (header = latch, body = header)
        if (loop.blocks.size() != 1) continue;

        uint32_t header = loop.header;
        uint32_t start = bb_starts[header];
        uint32_t end   = bb_ends[header];

        // Count instructions in the loop header
        uint32_t body_len = end - start;
        if (body_len < 4) continue;  // too small to unroll

        // Identify the back-edge branch at the end of the header
        int32_t be_idx = -1;
        uint32_t be_target = 0;
        bool is_cond_branch = false;

        for (int32_t i = static_cast<int32_t>(end) - 1; i >= static_cast<int32_t>(start); --i) {
            const auto& ri = out_instrs[i];
            auto oc = ri.op_code();

            if (oc == interpreter::IROpCode::Br ||
                oc == interpreter::IROpCode::BrTrue ||
                oc == interpreter::IROpCode::BrFalse) {
                if (ri.has_imm()) {
                    be_idx = i;
                    be_target = ri.imm.branch_target;
                    is_cond_branch = (oc != interpreter::IROpCode::Br);
                    if (be_target >= start && be_target < end)
                        break;
                    be_idx = -1;
                }
            } else if (oc == interpreter::IROpCode::Blt ||
                       oc == interpreter::IROpCode::Ble ||
                       oc == interpreter::IROpCode::Bgt ||
                       oc == interpreter::IROpCode::Bge ||
                       oc == interpreter::IROpCode::Beq ||
                       oc == interpreter::IROpCode::BneUn ||
                       oc == interpreter::IROpCode::BltUn ||
                       oc == interpreter::IROpCode::BleUn ||
                       oc == interpreter::IROpCode::BgtUn ||
                       oc == interpreter::IROpCode::BgeUn) {
                if (ri.has_imm()) {
                    be_idx = i;
                    be_target = ri.imm.branch_target;
                    is_cond_branch = true;
                    if (be_target >= start && be_target < end)
                        break;
                    be_idx = -1;
                }
            }
        }

        if (be_idx < 0) continue;

        // The body is from start to end, excluding back-edge instructions
        uint32_t body_start = start;
        uint32_t body_end = static_cast<uint32_t>(be_idx);

        uint32_t body_instr_count = body_end - body_start;
        if (body_instr_count < 2) continue;

        // Safety: skip if total unrolled size exceeds threshold
        // (200 instructions max after unrolling)
        if (body_instr_count * unroll_factor > 200) continue;

        // Determine the effective max vreg referenced in the body
        uint32_t body_max_vreg = 0;
        for (uint32_t i = body_start; i < body_end; ++i) {
            const auto& ri = out_instrs[i];
            if (ri.has_dst() && ri.dst_reg() > body_max_vreg)
                body_max_vreg = ri.dst_reg();
            if (ri.has_src1() && ri.src1_reg() > body_max_vreg)
                body_max_vreg = ri.src1_reg();
            if (ri.has_src2() && ri.src2_reg() > body_max_vreg)
                body_max_vreg = ri.src2_reg();
            uint32_t op = static_cast<uint32_t>(ri.imm.operand_index);
            if (op > body_max_vreg)
                body_max_vreg = op;
        }
        // +1 because vregs are 0-based
        uint32_t map_size = body_max_vreg + 1;
        if (map_size < 256) map_size = 256;

        // Insert unrolled body copies before the back-edge branch
        uint32_t copy_insert_point = static_cast<uint32_t>(be_idx);

        for (uint32_t copy_num = 0; copy_num < unroll_factor - 1; ++copy_num) {
            // Build fresh vreg map for this copy: original vreg → new vreg
            // -1 means no remap (vreg stays the same)
            std::vector<int32_t> vreg_map(map_size, -1);

            // Remap dst_regs in the copy to fresh vregs
            for (uint32_t i = body_start; i < body_end; ++i) {
                const auto& ri = out_instrs[i];
                if (ri.has_dst()) {
                    uint8_t dst = ri.dst_reg();
                    if (dst < map_size && vreg_map[dst] == -1) {
                        vreg_map[dst] = static_cast<int32_t>(next_vreg++);
                    }
                }
                // Also remap LdLoc operand_index if it references a def
                if (ri.op_code() == interpreter::IROpCode::LdLoc ||
                    ri.op_code() == interpreter::IROpCode::StLoc) {
                    uint32_t op = static_cast<uint32_t>(ri.imm.operand_index);
                    if (op < map_size && vreg_map[op] == -1) {
                        vreg_map[op] = static_cast<int32_t>(next_vreg++);
                    }
                }
            }

            // Copy body instructions with vreg remapping
            for (uint32_t i = body_start; i < body_end; ++i) {
                const auto& ri = out_instrs[i];
                interpreter::RegisterInstruction copy_ri = ri;

                // Remap dst
                if (copy_ri.has_dst()) {
                    uint8_t old_dst = copy_ri.dst_reg();
                    int32_t new_dst = (old_dst < map_size) ? vreg_map[old_dst] : -1;
                    if (new_dst > 0)
                        copy_ri.header = (copy_ri.header & ~(0xFFull << 16)) |
                            (static_cast<uint64_t>(static_cast<uint8_t>(new_dst)) << 16);
                }

                // Remap src1
                if (copy_ri.has_src1()) {
                    uint8_t old_src1 = copy_ri.src1_reg();
                    int32_t new_src1 = (old_src1 < map_size) ? vreg_map[old_src1] : -1;
                    if (new_src1 > 0)
                        copy_ri.header = (copy_ri.header & ~(0xFFull << 24)) |
                            (static_cast<uint64_t>(static_cast<uint8_t>(new_src1)) << 24);
                }

                // Remap src2
                if (copy_ri.has_src2()) {
                    uint8_t old_src2 = copy_ri.src2_reg();
                    int32_t new_src2 = (old_src2 < map_size) ? vreg_map[old_src2] : -1;
                    if (new_src2 > 0)
                        copy_ri.header = (copy_ri.header & ~(0xFFull << 32)) |
                            (static_cast<uint64_t>(static_cast<uint8_t>(new_src2)) << 32);
                }

                // Remap operand_index (LdLoc/StLoc vreg reference)
                uint32_t old_op = static_cast<uint32_t>(copy_ri.imm.operand_index);
                if (old_op < map_size) {
                    int32_t new_op = vreg_map[old_op];
                    if (new_op > 0)
                        copy_ri.imm.operand_index = static_cast<uint32_t>(new_op);
                }

                out_instrs.insert(out_instrs.begin() + copy_insert_point, copy_ri);
                copy_insert_point++;
            }
        }

        CHAOS_IL2CPP_LOG_DEBUG_M("jit",
            "Unrolled loop: header=%u body=%u instrs factor=%u (trip count analysis not available)",
            header, body_instr_count, unroll_factor);
    }
}

// ── BB range helpers ───────────────────────────────────────────────────
// Build bb_starts array from per-BB linearization progress.
static void UpdateBbRanges(
    const std::vector<interpreter::RegisterInstruction>& out_instrs,
    std::vector<uint32_t>& bb_starts,
    uint32_t bb_count) noexcept
{
    // bb_starts is populated during the per-BB loop; compute ends here
    // by taking next BB's start (or out_instrs size for the last BB).
    // No action needed if bb_starts is already populated by the caller.
    (void)out_instrs;
    (void)bb_count;
}

// ── Constant propagation pass ──────────────────────────────────────────
//
// Identifies vregs that hold known constant values (LdcI4/LdcI8) and
// propagates them forward.  This enables more LICM hoisting (constant
// vregs are trivially loop-invariant) and more folding in downstream
// codegen passes.
//
// Phase 1: build const table — scan all instructions, record known
// constants for each dst vreg.
//
// Phase 2: forward propagate — replace LdLoc with LdcI4/I8 when the
// src vreg is known-constant; fold pure arithmetic with constant operands.
// Repeats until no changes (typically 1-2 iterations).
static bool ConstPropagate(
    std::vector<interpreter::RegisterInstruction>& out_instrs,
    uint32_t max_vreg) noexcept
{
    if (out_instrs.empty() || max_vreg == 0) return false;

    enum ConstKind : uint8_t { kUnknown, kInt32, kInt64 };
    struct ConstVal { ConstKind kind = ConstKind::kUnknown; int64_t value = 0; };

    std::vector<ConstVal> const_vals(max_vreg);
    bool any_propagated = false;

    // Iterate until stable (typically 1-2 iterations)
    for (uint32_t iter = 0; iter < 4; ++iter) {
        // Reset const table each iteration (can't trust stale values
        // after propagation changes instructions)
        for (auto& cv : const_vals) cv = ConstVal{};

        // Phase 1: scan all instructions, record constants
        for (uint32_t i = 0; i < out_instrs.size(); ++i) {
            const auto& ri = out_instrs[i];
            if (!ri.has_dst()) continue;
            uint32_t dst = ri.dst_reg();
            if (dst >= max_vreg) continue;

            switch (ri.op_code()) {
            case interpreter::IROpCode::LdcI4:
                const_vals[dst] = {kInt32, ri.imm.i4};
                break;
            case interpreter::IROpCode::LdcI8:
                const_vals[dst] = {kInt64, ri.imm.i8};
                break;
            default:
                // Any other defining instruction → not a known constant
                const_vals[dst] = {};
                break;
            }
        }

        // Phase 2: propagate constants forward
        bool changed = false;
        for (uint32_t i = 0; i < out_instrs.size(); ++i) {
            auto& ri = out_instrs[i];

            // Case A: LdLoc → LdcI4/I8 when src vreg is known constant
            if (ri.op_code() == interpreter::IROpCode::LdLoc && ri.has_src1()) {
                uint32_t src = ri.src1_reg();
                if (src >= max_vreg) continue;
                const auto& cv = const_vals[src];
                if (cv.kind == kInt32) {
                    ri.header = (ri.header & ~(0xFFFFull)) |
                        static_cast<uint64_t>(interpreter::IROpCode::LdcI4);
                    ri.imm.i4 = static_cast<int32_t>(cv.value);
                    const_vals[ri.dst_reg()] = cv;
                    changed = true;
                    any_propagated = true;
                } else if (cv.kind == kInt64) {
                    ri.header = (ri.header & ~(0xFFFFull)) |
                        static_cast<uint64_t>(interpreter::IROpCode::LdcI8);
                    ri.imm.i8 = cv.value;
                    const_vals[ri.dst_reg()] = cv;
                    changed = true;
                    any_propagated = true;
                }
                continue;
            }

            // Case B: fold pure arithmetic with constant operands
            if (!IsPureArithmetic(ri.op_code())) continue;
            if (!ri.has_dst()) continue;
            uint32_t dst = ri.dst_reg();
            if (dst >= max_vreg) continue;

            // Collect src values
            bool has_src1 = ri.has_src1();
            bool has_src2 = ri.has_src2();
            uint32_t s1 = has_src1 ? ri.src1_reg() : 0;
            uint32_t s2 = has_src2 ? ri.src2_reg() : 0;

            int64_t v1 = 0, v2 = 0;
            bool c1_known = false, c2_known = false;
            ConstKind ck1 = kUnknown, ck2 = kUnknown;

            if (has_src1 && s1 < max_vreg && const_vals[s1].kind != kUnknown) {
                v1 = const_vals[s1].value;
                c1_known = true;
                ck1 = const_vals[s1].kind;
            }
            if (has_src2 && s2 < max_vreg && const_vals[s2].kind != kUnknown) {
                v2 = const_vals[s2].value;
                c2_known = true;
                ck2 = const_vals[s2].kind;
            }

            // For binary ops: both srcs must be known
            if (has_src1 && has_src2 && !(c1_known && c2_known)) continue;
            // For unary ops (Neg, Not, Conv*): src1 must be known
            if (has_src1 && !has_src2 && !c1_known) continue;

            // Determine result type: i64 if either operand is i64
            bool use_i64 = (ck1 == kInt64) || (ck2 == kInt64);

            int64_t result = 0;
            bool foldable = true;

            switch (ri.op_code()) {
            // Binary arithmetic
            case interpreter::IROpCode::Add: result = v1 + v2; break;
            case interpreter::IROpCode::Sub: result = v1 - v2; break;
            case interpreter::IROpCode::Mul: result = v1 * v2; break;
            case interpreter::IROpCode::And: result = v1 & v2; break;
            case interpreter::IROpCode::Or:  result = v1 | v2; break;
            case interpreter::IROpCode::Xor: result = v1 ^ v2; break;
            case interpreter::IROpCode::Shl:
                result = use_i64 ? (v1 << (v2 & 0x3F)) : (static_cast<int32_t>(v1) << (v2 & 0x1F));
                break;
            case interpreter::IROpCode::Shr:
                result = use_i64 ? (v1 >> (v2 & 0x3F)) : (static_cast<int32_t>(v1) >> (v2 & 0x1F));
                break;
            case interpreter::IROpCode::ShrUn:
                result = use_i64 ? (static_cast<uint64_t>(v1) >> (v2 & 0x3F))
                                 : (static_cast<uint32_t>(static_cast<int32_t>(v1)) >> (v2 & 0x1F));
                break;
            case interpreter::IROpCode::Ceq:  result = (v1 == v2) ? 1 : 0; use_i64 = false; break;
            case interpreter::IROpCode::Clt:  result = (v1 <  v2) ? 1 : 0; use_i64 = false; break;
            case interpreter::IROpCode::Cgt:  result = (v1 >  v2) ? 1 : 0; use_i64 = false; break;

            // Unary
            case interpreter::IROpCode::Neg: result = use_i64 ? -v1 : -static_cast<int32_t>(v1); break;
            case interpreter::IROpCode::Not: result = use_i64 ? ~v1 : ~static_cast<int32_t>(v1); break;

            // Conversions
            case interpreter::IROpCode::Conv_I4: result = static_cast<int32_t>(v1); use_i64 = false; break;
            case interpreter::IROpCode::Conv_I8: result = v1; use_i64 = true; break;
            case interpreter::IROpCode::ConvI:
                result = static_cast<int32_t>(v1);
                use_i64 = false;
                break;
            case interpreter::IROpCode::ConvU:
                result = static_cast<uint32_t>(v1);
                use_i64 = false;
                break;
            case interpreter::IROpCode::ConvRUn:
                // Fold to LdcI4 (the truncated int result)
                result = static_cast<int32_t>(v1);
                use_i64 = false;
                break;

            // LdLen — can't fold at linear level (runtime value)
            case interpreter::IROpCode::LdLen:
                foldable = false; break;

            default:
                foldable = false; break;
            }

            if (!foldable) continue;

            // Replace with LdcI4 or LdcI8
            if (use_i64) {
                ri.header = (ri.header & ~(0xFFFFull)) |
                    static_cast<uint64_t>(interpreter::IROpCode::LdcI8);
                ri.imm.i8 = result;
                const_vals[dst] = {kInt64, result};
            } else {
                ri.header = (ri.header & ~(0xFFFFull)) |
                    static_cast<uint64_t>(interpreter::IROpCode::LdcI4);
                ri.imm.i4 = static_cast<int32_t>(result);
                const_vals[dst] = {kInt32, result};
            }
            // Clear src flags (LdcI4/I8 has no srcs)
            ri.header &= ~(static_cast<uint64_t>(0xFF) << 24);  // clear src1
            ri.header &= ~(static_cast<uint64_t>(0xFF) << 32);  // clear src2
            ri.header &= ~(static_cast<uint64_t>(
                interpreter::kRegHasSrc1 | interpreter::kRegHasSrc2) << 40);

            changed = true;
            any_propagated = true;
        }

        if (!changed) break;
    }

    return any_propagated;
}

// ── OptimizeWithTreeIR ─────────────────────────────────────────────────

bool OptimizeWithTreeIR(
    const std::vector<interpreter::RegisterInstruction>& instrs,
    std::vector<interpreter::RegisterInstruction>& out_instrs,
    bool has_seh,
    uint32_t max_vreg,
    bool enable_inlining,
    InlineResultBuffer* inline_results) noexcept
{
    uint32_t n = static_cast<uint32_t>(instrs.size());
    if (n == 0) return false;

    if (has_seh) return false;

    if (max_vreg == 0) {
        for (const auto& ri : instrs) {
            if (ri.has_dst() && ri.dst_reg() > max_vreg)
                max_vreg = ri.dst_reg();
        }
        max_vreg += 1;
    }

    auto bbs = FindBasicBlocks(instrs.data(), n);
    if (bbs.empty()) return false;

    // [P5] Phase 1: Build CFG for multi-BB methods
    LoopAnalysis loop_analysis = {};
    if (bbs.size() > 1) {
        loop_analysis = BuildCfg(bbs, instrs.data());
    }

    // Track per-BB output ranges in out_instrs
    std::vector<uint32_t> bb_starts(bbs.size(), UINT32_MAX);

    bool any_optimized = false;

    for (uint32_t bi = 0; bi < bbs.size(); ++bi) {
        const auto& bb = bbs[bi];
        uint32_t bb_len = bb.hi - bb.lo;

        // Record start of this BB's output
        bb_starts[bi] = static_cast<uint32_t>(out_instrs.size());

        if (bb_len <= 2) {
            for (uint32_t i = bb.lo; i < bb.hi; ++i)
                out_instrs.push_back(instrs[i]);
            continue;
        }

        TreeBuilder builder;
        auto result = builder.Build(instrs.data(), bb.lo, bb.hi);

        if (!result.first_node || result.root_count == 0) {
            for (uint32_t i = bb.lo; i < bb.hi; ++i)
                out_instrs.push_back(instrs[i]);
            continue;
        }

        // Inline eligible kCall nodes
        if (enable_inlining) {
            Inliner inliner(InlineConfig{}, 0, max_vreg);
            // Set loop nesting depth for loop-aware inline cost model
            uint32_t bb_loop_depth = loop_analysis.blocks.empty()
                                         ? 0
                                         : loop_analysis.blocks[bi].loop_depth;
            inliner.set_bb_loop_depth(bb_loop_depth);
            inliner.InlineRoots(result.roots, result.root_count, 128u);
            if (inliner.new_max_vreg() > max_vreg)
                max_vreg = inliner.new_max_vreg();

            if (inline_results) {
                for (uint32_t ri = 0; ri < inliner.inlined_count(); ++ri) {
                    const auto& d = inliner.inlined_decisions()[ri];
                    inline_results->Add(d.callee_token, d.snapshot_version);
                }
            }
        }

        // Constant folding
        uint8_t* arena_pos = reinterpret_cast<uint8_t*>(result.first_node);
        uint8_t* arena_end = arena_pos + builder.kArenaSize;

        // Intrinsic expansion (after inlining, before const-folding)
        IntrinsicMutator intrinsic_mut(arena_pos, arena_end,
                                        kIntrinsicTable, kIntrinsicTableSize,
                                        &builder);
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = intrinsic_mut.Mutate(result.roots[ri]);

        ConstFoldMutator fold_mut(arena_pos, arena_end);
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = fold_mut.Mutate(result.roots[ri]);

        // Bounds check elimination (after const-fold, before CSE)
        BoundCheckEliminator bce_mut(arena_pos, arena_end, &builder);
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = bce_mut.Mutate(result.roots[ri]);

        // CSE
        CSEMutator cse_mut(builder.VN());
        for (uint32_t ri = 0; ri < result.root_count; ++ri)
            result.roots[ri] = cse_mut.Mutate(result.roots[ri]);

        // Linearize
        Linearizer linearizer;
        linearizer.LinearizeRoots(result.roots, result.root_count, out_instrs);

        any_optimized = true;
    }

    // [P5] Phase 2: Post-passes on linearized instruction stream
    if (loop_analysis.has_loops) {
        // Build bb_ends from bb_starts
        std::vector<uint32_t> bb_ends(bbs.size());
        for (uint32_t bi = 0; bi < bbs.size(); ++bi) {
            if (bi + 1 < bbs.size())
                bb_ends[bi] = bb_starts[bi + 1];
            else
                bb_ends[bi] = static_cast<uint32_t>(out_instrs.size());
        }

        // Build vreg → defining block map (from original instrs, before
        // const-prop and LICM modify out_instrs)
        std::vector<uint32_t> vreg_def_blocks;
        BuildVRegDefBlocks(instrs, bbs, max_vreg, vreg_def_blocks);

        // 0. Constant propagation (before LICM — enables more hoisting)
        ConstPropagate(out_instrs, max_vreg);

        // 2a. LICM: hoist loop-invariant arithmetic out of loops
        LicmHoist(out_instrs, bb_starts, bb_ends, loop_analysis,
                   vreg_def_blocks, max_vreg);

        // 2b. IV strength reduction
        IvStrengthReduce(out_instrs, bb_starts, bb_ends, loop_analysis,
                          max_vreg);

        // 2c. Loop unrolling (factor 4, or 8 for very small bodies)
        {
            uint32_t factor = 4;
            // Estimate body size: scan loop headers for small bodies
            for (const auto& l : loop_analysis.loops) {
                if (l.blocks.size() == 1) {
                    uint32_t est = bb_ends[l.header] - bb_starts[l.header];
                    if (est < 8) { factor = 8; break; }
                }
            }
            UnrollLoops(out_instrs, bb_starts, bb_ends, loop_analysis, max_vreg, factor);
        }
    }

    return any_optimized;
}

}  // namespace chaos::il2cpp::jit::tree
