// jit_optimizer_loops.cpp — Loop optimization passes (LICM, IV strength reduction, unrolling)
//
// Extracted from the original jit_optimizer.cpp to reduce its single-TU size.
// All functions are in internal:: namespace; declared in jit_optimizer_internal.h.
// No logic changes — pure file-level split.

#include "tree/jit_optimizer_internal.h"

#include <chaos/log.h>

#include <cstdint>
#include <vector>

namespace chaos::il2cpp::jit::tree {
namespace internal {

// ── Helper: build vreg → defining block map ───────────────────────────
static void BuildVRegDefBlocks(const std::vector<interpreter::RegisterInstruction>& instrs,
                               const std::vector<BBRange>& bbs, uint32_t max_vreg,
                               std::vector<uint32_t>& out_def_blocks) noexcept {
    out_def_blocks.assign(max_vreg, UINT32_MAX);
    uint32_t bi = 0;
    for (const auto& bb : bbs) {
        for (uint32_t i = bb.lo; i < bb.hi; ++i) {
            const auto& ri = instrs[i];
            if (ri.has_dst()) {
                uint32_t dst = ri.dst_reg();
                if (dst < max_vreg) out_def_blocks[dst] = bi;
            }
        }
        ++bi;
    }
}

// ── Helper: check if a vreg is loop-invariant ──────────────────────
// Returns true if vreg's defining block is outside the loop.
static bool IsLoopInvariantVReg(uint32_t vreg, const std::vector<uint32_t>& vreg_def_blocks,
                                const NaturalLoop& loop) noexcept {
    if (vreg >= vreg_def_blocks.size())
        return false;
    uint32_t def_block = vreg_def_blocks[vreg];
    if (def_block == UINT32_MAX)
        return false; // unknown → assume variant
    for (uint32_t b : loop.blocks) {
        if (b == def_block)
            return false;
    }
    return true;
}

// ── LICM: hoist loop-invariant pure arithmetic out of loops ────────
void LicmHoist(std::vector<interpreter::RegisterInstruction>& out_instrs,
               const std::vector<uint32_t>& bb_starts, const std::vector<uint32_t>& bb_ends,
               const LoopAnalysis& analysis, const std::vector<uint32_t>& vreg_def_blocks,
               uint32_t& next_vreg) noexcept {
    if (!analysis.has_loops)
        return;

    for (const auto& loop : analysis.loops) {
        uint32_t header = loop.header;
        uint32_t pre_header = UINT32_MAX;

        const auto& header_block = analysis.blocks[header];
        for (uint32_t pred : header_block.preds) {
            bool in_loop = false;
            for (uint32_t b : loop.blocks) {
                if (b == pred) { in_loop = true; break; }
            }
            if (!in_loop) { pre_header = pred; break; }
        }
        if (pre_header == UINT32_MAX) continue;

        uint32_t insert_point = bb_ends[pre_header];

        struct HoistInfo { uint32_t orig_dst_vreg; uint32_t hoisted_vreg; };
        std::vector<HoistInfo> hoisted;

        for (uint32_t bb_id : loop.blocks) {
            if (bb_id == header) continue;
            uint32_t start = bb_starts[bb_id];
            uint32_t end = bb_ends[bb_id];

            for (uint32_t i = start; i < end; ++i) {
                auto& ri = out_instrs[i];
                if (!IsPureArithmetic(ri.op_code())) continue;

                uint32_t src_count = 0;
                bool all_srcs_invariant = true;

                auto check_src = [&](uint32_t src_reg) {
                    src_count++;
                    if (!IsLoopInvariantVReg(src_reg, vreg_def_blocks, loop)) {
                        bool was_hoisted = false;
                        for (const auto& h : hoisted) {
                            if (h.orig_dst_vreg == src_reg) { was_hoisted = true; break; }
                        }
                        if (!was_hoisted) all_srcs_invariant = false;
                    }
                };

                if (ri.has_src1()) check_src(ri.src1_reg());
                if (ri.has_src2()) check_src(ri.src2_reg());

                if (!all_srcs_invariant || src_count == 0) continue;
                if (!ri.has_dst()) continue;

                uint32_t dst_vreg = ri.dst_reg();
                uint32_t new_vreg = next_vreg++;

                interpreter::RegisterInstruction hoisted_ri = ri;
                hoisted_ri.header = (hoisted_ri.header & ~(0xFFull << 16)) | (static_cast<uint64_t>(new_vreg) << 16);
                out_instrs.insert(out_instrs.begin() + insert_point, hoisted_ri);

                ri.header = 0;
                ri.header |= static_cast<uint64_t>(interpreter::IROpCode::LdLoc) & 0xFFFF;
                ri.header |= static_cast<uint64_t>(dst_vreg) << 16;
                ri.header |= static_cast<uint64_t>(new_vreg) << 24;
                ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst | interpreter::kRegHasSrc1) << 40;
                ri.imm.operand_index = new_vreg;

                hoisted.push_back({dst_vreg, new_vreg});
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
void IvStrengthReduce(std::vector<interpreter::RegisterInstruction>& out_instrs,
                      const std::vector<uint32_t>& bb_starts, const std::vector<uint32_t>& bb_ends,
                      const LoopAnalysis& analysis, uint32_t& next_vreg) noexcept {
    if (!analysis.has_loops) return;

    for (const auto& loop : analysis.loops) {
        uint32_t header = loop.header;
        uint32_t pre_header = UINT32_MAX;
        const auto& header_block = analysis.blocks[header];
        for (uint32_t pred : header_block.preds) {
            bool in_loop = false;
            for (uint32_t b : loop.blocks) { if (b == pred) { in_loop = true; break; } }
            if (!in_loop) { pre_header = pred; break; }
        }
        if (pre_header == UINT32_MAX) continue;

        struct IvInfo { uint32_t iv_vreg; uint32_t step; int32_t initial_value; uint32_t inc_instr_idx; };
        std::vector<IvInfo> ivs;

        for (uint32_t bb_id : loop.blocks) {
            uint32_t start = bb_starts[bb_id];
            uint32_t end = bb_ends[bb_id];
            for (uint32_t i = start; i < end; ++i) {
                const auto& ri = out_instrs[i];
                if (ri.op_code() != interpreter::IROpCode::Add) continue;
                if (!ri.has_dst() || !ri.has_src1() || !ri.has_src2()) continue;
                if (ri.src1_reg() != ri.dst_reg()) continue;

                uint32_t src2 = ri.src2_reg();
                int32_t step = 0;
                for (int32_t j = static_cast<int32_t>(i) - 1; j >= static_cast<int32_t>(start); --j) {
                    const auto& prev = out_instrs[j];
                    if (prev.has_dst() && prev.dst_reg() == src2 && prev.op_code() == interpreter::IROpCode::LdcI4) {
                        step = prev.imm.i4; break;
                    }
                }
                if (step != 1) continue;

                int32_t initial = -1;
                uint32_t ps = bb_starts[pre_header];
                uint32_t pe = bb_ends[pre_header];
                for (uint32_t j = ps; j < pe; ++j) {
                    const auto& prev_ri = out_instrs[j];
                    if (prev_ri.has_dst() && prev_ri.dst_reg() == ri.src1_reg() &&
                        prev_ri.op_code() == interpreter::IROpCode::LdcI4) {
                        initial = prev_ri.imm.i4; break;
                    }
                }
                ivs.push_back({ri.dst_reg(), 1, initial, i});
            }
        }
        if (ivs.empty()) continue;

        uint32_t insert_point = bb_ends[pre_header];
        for (const auto& iv : ivs) {
            for (uint32_t bb_id : loop.blocks) {
                uint32_t start = bb_starts[bb_id];
                uint32_t end = bb_ends[bb_id];
                for (uint32_t i = start; i < end; ++i) {
                    auto& ri = out_instrs[i];
                    if (ri.op_code() != interpreter::IROpCode::Mul) continue;
                    if (!ri.has_dst() || !ri.has_src1() || !ri.has_src2()) continue;
                    uint32_t other_src; bool iv_is_src1;
                    if (ri.src1_reg() == iv.iv_vreg) { other_src = ri.src2_reg(); iv_is_src1 = true; }
                    else if (ri.src2_reg() == iv.iv_vreg) { other_src = ri.src1_reg(); iv_is_src1 = false; }
                    else continue;

                    int32_t const_k = 0; bool found_const = false;
                    for (int32_t j = static_cast<int32_t>(i) - 1; j >= static_cast<int32_t>(start); --j) {
                        const auto& prev = out_instrs[j];
                        if (prev.has_dst() && prev.dst_reg() == other_src &&
                            prev.op_code() == interpreter::IROpCode::LdcI4) {
                            const_k = prev.imm.i4; found_const = true; break;
                        }
                    }
                    if (!found_const || const_k == 0 || const_k >= 256) continue;

                    uint32_t acc_vreg = next_vreg++;
                    uint32_t init_vreg = next_vreg++;
                    uint32_t k_vreg = next_vreg++;

                    interpreter::RegisterInstruction init_ri = {};
                    init_ri.header = static_cast<uint64_t>(interpreter::IROpCode::LdcI4);
                    init_ri.header |= static_cast<uint64_t>(init_vreg) << 16;
                    init_ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst) << 40;
                    init_ri.imm.i4 = (iv.initial_value >= 0) ? iv.initial_value : 0;

                    interpreter::RegisterInstruction k_ri = {};
                    k_ri.header = static_cast<uint64_t>(interpreter::IROpCode::LdcI4);
                    k_ri.header |= static_cast<uint64_t>(k_vreg) << 16;
                    k_ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst) << 40;
                    k_ri.imm.i4 = const_k;

                    interpreter::RegisterInstruction mul_ri = {};
                    mul_ri.header = static_cast<uint64_t>(interpreter::IROpCode::Mul);
                    mul_ri.header |= static_cast<uint64_t>(acc_vreg) << 16;
                    mul_ri.header |= static_cast<uint64_t>(init_vreg) << 24;
                    mul_ri.header |= static_cast<uint64_t>(k_vreg) << 32;
                    mul_ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst | interpreter::kRegHasSrc1 | interpreter::kRegHasSrc2) << 40;

                    out_instrs.insert(out_instrs.begin() + insert_point, mul_ri);
                    out_instrs.insert(out_instrs.begin() + insert_point, k_ri);
                    out_instrs.insert(out_instrs.begin() + insert_point, init_ri);
                    uint32_t shift = 3;

                    ri.header = 0;
                    ri.header |= static_cast<uint64_t>(interpreter::IROpCode::LdLoc) & 0xFFFF;
                    ri.header |= static_cast<uint64_t>(ri.dst_reg()) << 16;
                    ri.header |= static_cast<uint64_t>(acc_vreg) << 24;
                    ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst | interpreter::kRegHasSrc1) << 40;
                    ri.imm.operand_index = acc_vreg;

                    uint32_t actual_inc_idx = iv.inc_instr_idx + shift;
                    interpreter::RegisterInstruction inc_ri = {};
                    inc_ri.header = static_cast<uint64_t>(interpreter::IROpCode::Add);
                    inc_ri.header |= static_cast<uint64_t>(acc_vreg) << 16;
                    inc_ri.header |= static_cast<uint64_t>(acc_vreg) << 24;
                    inc_ri.header |= static_cast<uint64_t>(k_vreg) << 32;
                    inc_ri.header |= static_cast<uint64_t>(interpreter::kRegHasDst | interpreter::kRegHasSrc1 | interpreter::kRegHasSrc2) << 40;
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
void UnrollLoops(std::vector<interpreter::RegisterInstruction>& out_instrs,
                 const std::vector<uint32_t>& bb_starts, const std::vector<uint32_t>& bb_ends,
                 const LoopAnalysis& analysis, uint32_t& next_vreg,
                 uint32_t unroll_factor) noexcept {
    if (!analysis.has_loops) return;

    for (const auto& loop : analysis.loops) {
        if (loop.blocks.size() != 1) continue;

        uint32_t header = loop.header;
        uint32_t start = bb_starts[header];
        uint32_t end = bb_ends[header];
        uint32_t body_len = end - start;
        if (body_len < 4) continue;

        int32_t be_idx = -1; uint32_t be_target = 0;
        bool is_cond_branch = false;
        for (int32_t i = static_cast<int32_t>(end) - 1; i >= static_cast<int32_t>(start); --i) {
            const auto& ri = out_instrs[i];
            auto oc = ri.op_code();
            if (oc == interpreter::IROpCode::Br || oc == interpreter::IROpCode::BrTrue ||
                oc == interpreter::IROpCode::BrFalse) {
                if (ri.has_imm()) { be_idx = i; be_target = ri.imm.branch_target; is_cond_branch = (oc != interpreter::IROpCode::Br); }
                if (be_target >= start && be_target < end) break; else be_idx = -1;
            } else if (oc == interpreter::IROpCode::Blt || oc == interpreter::IROpCode::Ble ||
                       oc == interpreter::IROpCode::Bgt || oc == interpreter::IROpCode::Bge ||
                       oc == interpreter::IROpCode::Beq || oc == interpreter::IROpCode::BneUn ||
                       oc == interpreter::IROpCode::BltUn || oc == interpreter::IROpCode::BleUn ||
                       oc == interpreter::IROpCode::BgtUn || oc == interpreter::IROpCode::BgeUn) {
                if (ri.has_imm()) { be_idx = i; be_target = ri.imm.branch_target; is_cond_branch = true; }
                if (be_target >= start && be_target < end) break; else be_idx = -1;
            }
        }
        if (be_idx < 0) continue;

        uint32_t body_start = start;
        uint32_t body_end = static_cast<uint32_t>(be_idx);
        uint32_t body_instr_count = body_end - body_start;
        if (body_instr_count < 2) continue;
        if (body_instr_count * unroll_factor > 200) continue;

        uint32_t body_max_vreg = 0;
        for (uint32_t i = body_start; i < body_end; ++i) {
            const auto& ri = out_instrs[i];
            if (ri.has_dst() && ri.dst_reg() > body_max_vreg) body_max_vreg = ri.dst_reg();
            if (ri.has_src1() && ri.src1_reg() > body_max_vreg) body_max_vreg = ri.src1_reg();
            if (ri.has_src2() && ri.src2_reg() > body_max_vreg) body_max_vreg = ri.src2_reg();
            uint32_t op = static_cast<uint32_t>(ri.imm.operand_index);
            if (op > body_max_vreg) body_max_vreg = op;
        }
        uint32_t map_size = body_max_vreg + 1;
        if (map_size < 256) map_size = 256;

        uint32_t copy_insert_point = static_cast<uint32_t>(be_idx);
        for (uint32_t copy_num = 0; copy_num < unroll_factor - 1; ++copy_num) {
            std::vector<int32_t> vreg_map(map_size, -1);
            for (uint32_t i = body_start; i < body_end; ++i) {
                const auto& ri = out_instrs[i];
                if (ri.has_dst()) { uint8_t dst = ri.dst_reg(); if (dst < map_size && vreg_map[dst] == -1) vreg_map[dst] = static_cast<int32_t>(next_vreg++); }
                if (ri.op_code() == interpreter::IROpCode::LdLoc || ri.op_code() == interpreter::IROpCode::StLoc) {
                    uint32_t op = static_cast<uint32_t>(ri.imm.operand_index);
                    if (op < map_size && vreg_map[op] == -1) vreg_map[op] = static_cast<int32_t>(next_vreg++);
                }
            }
            for (uint32_t i = body_start; i < body_end; ++i) {
                const auto& ri = out_instrs[i];
                interpreter::RegisterInstruction copy_ri = ri;
                if (copy_ri.has_dst()) {
                    uint8_t old_dst = copy_ri.dst_reg(); int32_t new_dst = (old_dst < map_size) ? vreg_map[old_dst] : -1;
                    if (new_dst > 0) copy_ri.header = (copy_ri.header & ~(0xFFull << 16)) | (static_cast<uint64_t>(static_cast<uint8_t>(new_dst)) << 16);
                }
                if (copy_ri.has_src1()) {
                    uint8_t old_src1 = copy_ri.src1_reg(); int32_t new_src1 = (old_src1 < map_size) ? vreg_map[old_src1] : -1;
                    if (new_src1 > 0) copy_ri.header = (copy_ri.header & ~(0xFFull << 24)) | (static_cast<uint64_t>(static_cast<uint8_t>(new_src1)) << 24);
                }
                if (copy_ri.has_src2()) {
                    uint8_t old_src2 = copy_ri.src2_reg(); int32_t new_src2 = (old_src2 < map_size) ? vreg_map[old_src2] : -1;
                    if (new_src2 > 0) copy_ri.header = (copy_ri.header & ~(0xFFull << 32)) | (static_cast<uint64_t>(static_cast<uint8_t>(new_src2)) << 32);
                }
                uint32_t old_op = static_cast<uint32_t>(copy_ri.imm.operand_index);
                if (old_op < map_size) { int32_t new_op = vreg_map[old_op]; if (new_op > 0) copy_ri.imm.operand_index = static_cast<uint32_t>(new_op); }
                out_instrs.insert(out_instrs.begin() + copy_insert_point, copy_ri);
                copy_insert_point++;
            }
        }
        CHAOS_IL2CPP_LOG_DEBUG_M("jit", "Unrolled loop: header=%u body=%u instrs factor=%u",
                                 header, body_instr_count, unroll_factor);
    }
}

}  // namespace internal
}  // namespace chaos::il2cpp::jit::tree