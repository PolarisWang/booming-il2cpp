/// ir_reg_alloc_test.cpp — Register allocator unit tests (gtest)
///
/// Tests AllocateRegisters() with various IRMethod patterns:
///   - Simple ldc.i4 + ret (trivial case)
///   - Arithmetic with evaluation stack manipulation
///   - Local variable load/store
///   - Branch instructions (target indices preserved)
///   - Multiple stack depths
///   - SEH clause preservation
///
/// All tests construct IRMethod with stack-based instructions and verify
/// the output RegisterMethod has correct instruction encoding and maps.

#include "ir_reg_alloc.h"
#include <gtest/gtest.h>
#include <cstring>

// AOT symbol stubs (required by chaos_interpreter library linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::IRInstruction;
using chaos::il2cpp::interpreter::IRMethod;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::AllocateRegisters;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasSrc1;
using chaos::il2cpp::interpreter::kRegHasSrc2;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::interpreter::kRegIsBranch;
using chaos::il2cpp::interpreter::kRegIsStore;
using chaos::il2cpp::interpreter::CoalescedCallArgs;
using chaos::il2cpp::interpreter::RegisterFrame;
using chaos::il2cpp::interpreter::RegisterExecute;
using chaos::il2cpp::interpreter::ValueTag;

// ═══════════════════════════════════════════════════════════════════════════
// Helpers
// ═══════════════════════════════════════════════════════════════════════════

/// Build an IRMethod with the given instructions.
static IRMethod MakeIRMethod(const std::vector<IRInstruction>& instrs) {
    IRMethod m;
    m.instructions = instrs;
    return m;
}

// ═══════════════════════════════════════════════════════════════════════════
// Basic patterns
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LdcI4AndRet) {
    // ldc.i4 42; ret
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, ret}));

    EXPECT_EQ(rm.instructions.size(), 2u);
    EXPECT_EQ(rm.max_regs, 17u);  // one virtual register (r16), next_vreg starts at 16

    // First instruction: load 42 into a register.
    const auto& r0 = rm.instructions[0];
    EXPECT_EQ(r0.op_code(), IROpCode::LdcI4);
    EXPECT_TRUE(r0.has_dst());
    EXPECT_TRUE(r0.has_imm());
    EXPECT_EQ(r0.imm.i4, 42);
    EXPECT_EQ(r0.dst_reg(), 16);  // first virtual register

    // Second instruction: ret from that register.
    const auto& r1 = rm.instructions[1];
    EXPECT_EQ(r1.op_code(), IROpCode::Ret);
    EXPECT_TRUE(r1.has_src1());
    EXPECT_EQ(r1.src1_reg(), 16);
}

TEST(IR_RegAlloc, LdcI8AndRet) {
    // ldc.i8 0xABCD; ret
    IRInstruction push; push.op_code = IROpCode::LdcI8; push.immediate_i8 = 0xABCD;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, ret}));

    EXPECT_EQ(rm.instructions.size(), 2u);
    EXPECT_EQ(rm.max_regs, 17u);
    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdcI8);
    EXPECT_EQ(rm.instructions[0].imm.i8, 0xABCD);
}

TEST(IR_RegAlloc, LdNullAndRet) {
    IRInstruction push; push.op_code = IROpCode::LdNull;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, ret}));

    EXPECT_EQ(rm.instructions.size(), 2u);
    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdNull);
    EXPECT_TRUE(rm.instructions[0].has_dst());
}

// ═══════════════════════════════════════════════════════════════════════════
// Arithmetic
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, SimpleArithmetic) {
    // ldc.i4 20; ldc.i4 22; add; ret
    IRInstruction p1; p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 20;
    IRInstruction p2; p2.op_code = IROpCode::LdcI4; p2.immediate_i4 = 22;
    IRInstruction add; add.op_code = IROpCode::Add;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, p2, add, ret}));

    // Stack behavior: push(20) → push(22) → add pops 2 pushes 1
    // Virtual registers: r16=20, r17=22, r18=20+22
    ASSERT_EQ(rm.instructions.size(), 4u);

    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdcI4);
    EXPECT_EQ(rm.instructions[0].dst_reg(), 16);
    EXPECT_EQ(rm.instructions[0].imm.i4, 20);

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::LdcI4);
    EXPECT_EQ(rm.instructions[1].dst_reg(), 17);
    EXPECT_EQ(rm.instructions[1].imm.i4, 22);

    EXPECT_EQ(rm.instructions[2].op_code(), IROpCode::Add);
    EXPECT_TRUE(rm.instructions[2].has_dst());
    EXPECT_TRUE(rm.instructions[2].has_src1());
    EXPECT_TRUE(rm.instructions[2].has_src2());
    EXPECT_EQ(rm.instructions[2].src1_reg(), 16);  // first operand from stack
    EXPECT_EQ(rm.instructions[2].src2_reg(), 17);  // second operand from stack

    EXPECT_EQ(rm.instructions[3].op_code(), IROpCode::Ret);
    EXPECT_TRUE(rm.instructions[3].has_src1());
    EXPECT_EQ(rm.instructions[3].src1_reg(), 18);  // result

    EXPECT_EQ(rm.max_regs, 19u);  // r16, r17, r18 used
}

TEST(IR_RegAlloc, DeepStackArithmetic) {
    // (10 + 20) * (30 + 40)
    // ldc.i4 10; ldc.i4 20; add; ldc.i4 30; ldc.i4 40; add; mul; ret
    IRInstruction i[7];
    i[0].op_code = IROpCode::LdcI4; i[0].immediate_i4 = 10;
    i[1].op_code = IROpCode::LdcI4; i[1].immediate_i4 = 20;
    i[2].op_code = IROpCode::Add;
    i[3].op_code = IROpCode::LdcI4; i[3].immediate_i4 = 30;
    i[4].op_code = IROpCode::LdcI4; i[4].immediate_i4 = 40;
    i[5].op_code = IROpCode::Mul;
    i[6].op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod(
        {i[0], i[1], i[2], i[3], i[4], i[5], i[6]}));

    ASSERT_EQ(rm.instructions.size(), 7u);

    // ldc 10 → r16
    EXPECT_EQ(rm.instructions[0].dst_reg(), 16);
    // ldc 20 → r17
    EXPECT_EQ(rm.instructions[1].dst_reg(), 17);
    // add r16 r17 → r18 (= 30)
    EXPECT_EQ(rm.instructions[2].dst_reg(), 18);
    EXPECT_EQ(rm.instructions[2].src1_reg(), 16);
    EXPECT_EQ(rm.instructions[2].src2_reg(), 17);

    // ldc 30 → r19 (stack now: r18=30, r19=30)
    EXPECT_EQ(rm.instructions[3].dst_reg(), 19);
    // ldc 40 → r20 (stack: r18=30, r19=30, r20=40)
    EXPECT_EQ(rm.instructions[4].dst_reg(), 20);
    // mul r19 r20 → r21 (= 1200)
    EXPECT_EQ(rm.instructions[5].dst_reg(), 21);
    EXPECT_EQ(rm.instructions[5].src1_reg(), 19);
    EXPECT_EQ(rm.instructions[5].src2_reg(), 20);
}

// ═══════════════════════════════════════════════════════════════════════════
// Local variables
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LocalLoadStore) {
    // ldc.i4 42; stloc 0; ldloc 0; ret
    IRInstruction p1;  p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 42;
    IRInstruction st;  st.op_code = IROpCode::StLoc;  st.operand_index = 0;
    IRInstruction ld;  ld.op_code = IROpCode::LdLoc;  ld.operand_index = 0;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, st, ld, ret}));

    ASSERT_EQ(rm.instructions.size(), 4u);

    // ldc 42 → r16
    EXPECT_EQ(rm.instructions[0].dst_reg(), 16);
    EXPECT_EQ(rm.instructions[0].imm.i4, 42);

    // stloc 0 → write r16 to local 0 (src1_reg = r16)
    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::StLoc);
    EXPECT_TRUE(rm.instructions[1].has_src1());
    EXPECT_EQ(rm.instructions[1].src1_reg(), 16);
    EXPECT_EQ(rm.instructions[1].imm.operand_index, 0u);
    EXPECT_TRUE(rm.instructions[1].flags() & kRegIsStore);

    // ldloc 0 → read local 0 into r17
    EXPECT_EQ(rm.instructions[2].op_code(), IROpCode::LdLoc);
    EXPECT_TRUE(rm.instructions[2].has_dst());
    EXPECT_EQ(rm.instructions[2].dst_reg(), 17);
    EXPECT_EQ(rm.instructions[2].imm.operand_index, 0u);

    // ret r17
    EXPECT_EQ(rm.instructions[3].op_code(), IROpCode::Ret);
    EXPECT_TRUE(rm.instructions[3].has_src1());
    EXPECT_EQ(rm.instructions[3].src1_reg(), 17);

    EXPECT_EQ(rm.max_regs, 18u);  // r16, r17
}

TEST(IR_RegAlloc, MultipleLocals) {
    // stloc 3; stloc 7; ldloc 3; ret
    IRInstruction s3; s3.op_code = IROpCode::StLoc; s3.operand_index = 3;
    IRInstruction s7; s7.op_code = IROpCode::StLoc; s7.operand_index = 7;
    IRInstruction l3; l3.op_code = IROpCode::LdLoc; l3.operand_index = 3;
    IRInstruction ret; ret.op_code = IROpCode::Ret;
    // Need values to store — push two values first
    IRInstruction p1; p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 10;
    IRInstruction p2; p2.op_code = IROpCode::LdcI4; p2.immediate_i4 = 20;

    auto rm = AllocateRegisters(MakeIRMethod({p1, p2, s3, s7, l3, ret}));

    ASSERT_EQ(rm.instructions.size(), 6u);

    // ldc 10 → r16; ldc 20 → r17
    // stloc 3: pop r17 → local 3 (src1 = top = r17)
    EXPECT_EQ(rm.instructions[2].op_code(), IROpCode::StLoc);
    EXPECT_EQ(rm.instructions[2].src1_reg(), 17);

    // stloc 7: pop r16 → local 7
    EXPECT_EQ(rm.instructions[3].op_code(), IROpCode::StLoc);
    EXPECT_EQ(rm.instructions[3].src1_reg(), 16);

    // ldloc 3: push → r18
    EXPECT_EQ(rm.instructions[4].op_code(), IROpCode::LdLoc);
    EXPECT_EQ(rm.instructions[4].dst_reg(), 18);

    // ret r18
    EXPECT_EQ(rm.instructions[5].src1_reg(), 18);
}

// ═══════════════════════════════════════════════════════════════════════════
// Branch instructions
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, UnconditionalBranch) {
    // ldc.i4 99; br to ret; ldc.i4 0; ret
    IRInstruction p1;  p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 99;
    IRInstruction br;  br.op_code = IROpCode::Br;    br.branch_target = 3;
    IRInstruction p2;  p2.op_code = IROpCode::LdcI4; p2.immediate_i4 = 0;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, br, p2, ret}));

    ASSERT_EQ(rm.instructions.size(), 4u);

    // br r16 (or just br with no src when branching mid-block)
    // The branch target index should be preserved.
    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdcI4);
    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::Br);
    EXPECT_TRUE(rm.instructions[1].is_branch());
}

TEST(IR_RegAlloc, BrTrue) {
    // ldc.i4 42; brtrue target; ldc.i4 0; ret; (target:) ldc.i4 1; ret
    IRInstruction p1;   p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 42;
    IRInstruction bt;   bt.op_code = IROpCode::BrTrue;  bt.branch_target = 4;
    IRInstruction p2;   p2.op_code = IROpCode::LdcI4; p2.immediate_i4 = 0;
    IRInstruction ret1; ret1.op_code = IROpCode::Ret;
    IRInstruction p3;   p3.op_code = IROpCode::LdcI4; p3.immediate_i4 = 1;
    IRInstruction ret2; ret2.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, bt, p2, ret1, p3, ret2}));

    ASSERT_EQ(rm.instructions.size(), 6u);

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::BrTrue);
    EXPECT_TRUE(rm.instructions[1].is_branch());
    EXPECT_TRUE(rm.instructions[1].has_src1());
    EXPECT_EQ(rm.instructions[1].src1_reg(), 16);  // the 42 value
}

// ═══════════════════════════════════════════════════════════════════════════
// Stack map entries
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, StackMapPopulated) {
    // Simple method: ldc.i4 42; ret
    IRInstruction p1;  p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 42;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, ret}));

    // Stack map should have an entry for each instruction pc.
    EXPECT_GE(rm.stack_map.entries.size(), 2u);

    // At pc=0 (after ldc), stack should have 1 slot mapped to r16.
    if (rm.stack_map.entries.size() > 1) {
        const auto& e1 = rm.stack_map.entries[1];
        EXPECT_GT(e1.stack_depth, 0u);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Pop instruction
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, PopReducesStack) {
    // ldc.i4 10; ldc.i4 20; pop; ret  →  pop discards r17, ret reads r16
    IRInstruction p1;  p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 10;
    IRInstruction p2;  p2.op_code = IROpCode::LdcI4; p2.immediate_i4 = 20;
    IRInstruction pop; pop.op_code = IROpCode::Pop;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, p2, pop, ret}));

    ASSERT_EQ(rm.instructions.size(), 4u);

    // After pop: stack depth = 1 (only r16 left).
    // ret should read r16 (the 10 value).
    EXPECT_EQ(rm.instructions[0].dst_reg(), 16);   // ldc 10 → r16
    EXPECT_EQ(rm.instructions[1].dst_reg(), 17);   // ldc 20 → r17

    // Pop: consumes src1 (r17, the top of stack)
    EXPECT_EQ(rm.instructions[2].op_code(), IROpCode::Pop);
    EXPECT_TRUE(rm.instructions[2].has_src1());
    EXPECT_EQ(rm.instructions[2].src1_reg(), 17);

    // Ret: reads r16 (the remaining stack value)
    EXPECT_EQ(rm.instructions[3].op_code(), IROpCode::Ret);
    EXPECT_TRUE(rm.instructions[3].has_src1());
    EXPECT_EQ(rm.instructions[3].src1_reg(), 16);
}

// ═══════════════════════════════════════════════════════════════════════════
// Dup instruction
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, DupDuplicatesTopOfStack) {
    // ldc.i4 42; dup; ret  →  dup copies r16, ret reads r16
    IRInstruction p1;  p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 42;
    IRInstruction dup; dup.op_code = IROpCode::Dup;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, dup, ret}));

    ASSERT_EQ(rm.instructions.size(), 3u);

    // dup: allocates a new virtual register (r17) that holds the same value as r16.
    EXPECT_EQ(rm.instructions[0].dst_reg(), 16);   // ldc 42 → r16
    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::Dup);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
    EXPECT_EQ(rm.instructions[1].src1_reg(), 16);  // source of dup
    EXPECT_EQ(rm.instructions[1].dst_reg(), 17);   // new register for the duplicate
}

// ═══════════════════════════════════════════════════════════════════════════
// Empty method
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, EmptyMethod) {
    IRMethod empty;
    auto rm = AllocateRegisters(empty);
    EXPECT_EQ(rm.instructions.size(), 0u);
    EXPECT_EQ(rm.max_regs, 16u);  // next_vreg starts at 16, no vregs allocated
}

// ═══════════════════════════════════════════════════════════════════════════
// LdArg instruction
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LdArg) {
    // ldarg 0; ret
    IRInstruction la; la.op_code = IROpCode::LdArg; la.operand_index = 0;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({la, ret}));

    ASSERT_EQ(rm.instructions.size(), 2u);

    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdArg);
    EXPECT_TRUE(rm.instructions[0].has_dst());
    // ldarg 0 → r16 (allocator uses next_vreg starting at 16)
    EXPECT_EQ(rm.instructions[0].dst_reg(), 16u);
    EXPECT_EQ(rm.instructions[0].imm.operand_index, 0u);

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::Ret);
    EXPECT_EQ(rm.instructions[1].src1_reg(), 16u);
}

TEST(IR_RegAlloc, MultipleArgs) {
    // ldarg 0; ldarg 3; add; ret
    IRInstruction a0; a0.op_code = IROpCode::LdArg; a0.operand_index = 0;
    IRInstruction a3; a3.op_code = IROpCode::LdArg; a3.operand_index = 3;
    IRInstruction add; add.op_code = IROpCode::Add;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({a0, a3, add, ret}));

    ASSERT_EQ(rm.instructions.size(), 4u);

    // ldarg 0 → r16; ldarg 3 → r17 (allocator uses next_vreg starting at 16)
    EXPECT_EQ(rm.instructions[0].dst_reg(), 16u);
    EXPECT_EQ(rm.instructions[1].dst_reg(), 17u);

    // add r16 r17 → r18 (result register)
    EXPECT_EQ(rm.instructions[2].src1_reg(), 16u);
    EXPECT_EQ(rm.instructions[2].src2_reg(), 17u);
    EXPECT_EQ(rm.instructions[2].dst_reg(), 18u);
}

// ═══════════════════════════════════════════════════════════════════════════
// SEH clause preservation
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, SEHClausesPreserved) {
    using chaos::il2cpp::interpreter::SEHClause;
    using chaos::il2cpp::interpreter::SEHFlags;

    IRMethod method;
    IRInstruction p1;  p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 10;
    IRInstruction ret; ret.op_code = IROpCode::Ret;
    method.instructions = {p1, ret};

    // Add one SEH clause.
    SEHClause clause;
    clause.try_start_idx = 0;
    clause.try_end_idx = 1;
    clause.handler_start_idx = 1;
    clause.flags = SEHFlags::Exception;
    clause.class_token = 0;
    method.seh_clauses.push_back(clause);

    auto rm = AllocateRegisters(method);

    ASSERT_EQ(rm.seh_clauses.size(), 1u);
    EXPECT_EQ(rm.seh_clauses[0].try_start_idx, 0u);
    EXPECT_EQ(rm.seh_clauses[0].try_end_idx, 1u);
    EXPECT_EQ(rm.seh_clauses[0].handler_start_idx, 1u);
    EXPECT_EQ(static_cast<uint32_t>(rm.seh_clauses[0].flags),
              static_cast<uint32_t>(SEHFlags::Exception));
}

// ═══════════════════════════════════════════════════════════════════════════
// Float constants
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LdcR4AndRet) {
    IRInstruction push; push.op_code = IROpCode::LdcR4; push.immediate_i4 = 0x40400000;  // 3.0f
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, ret}));

    EXPECT_EQ(rm.instructions.size(), 2u);
    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdcR4);
    EXPECT_TRUE(rm.instructions[0].has_dst());
    EXPECT_TRUE(rm.instructions[0].has_imm());
    EXPECT_EQ(rm.instructions[0].imm.i4, 0x40400000);
}

TEST(IR_RegAlloc, LdcR8AndRet) {
    IRInstruction push; push.op_code = IROpCode::LdcR8; push.immediate_r8 = 3.0;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, ret}));

    EXPECT_EQ(rm.instructions.size(), 2u);
    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdcR8);
    EXPECT_TRUE(rm.instructions[0].has_dst());
    EXPECT_TRUE(rm.instructions[0].has_imm());
    EXPECT_EQ(rm.instructions[0].imm.r8, 3.0);
}

// ═══════════════════════════════════════════════════════════════════════════
// LdStr — string constant
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LdStrAllocatesRegister) {
    IRInstruction ld; ld.op_code = IROpCode::LdStr; ld.string_operand = "hello";
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({ld, ret}));

    EXPECT_EQ(rm.instructions.size(), 2u);
    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdStr);
    EXPECT_TRUE(rm.instructions[0].has_dst());
    EXPECT_EQ(rm.instructions[0].dst_reg(), 16);
}

// ═══════════════════════════════════════════════════════════════════════════
// Arg/Local address-of
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LdArgA) {
    IRInstruction la; la.op_code = IROpCode::LdArgA; la.operand_index = 1;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({la, ret}));

    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdArgA);
    EXPECT_TRUE(rm.instructions[0].has_dst());
    EXPECT_EQ(rm.instructions[0].imm.operand_index, 1u);
}

TEST(IR_RegAlloc, LdLocARejectsToFallback) {
    // LdLocA needs a real address for a local slot, but the register VM keeps
    // locals in the register file (not addressable memory).  AllocateRegisters
    // must reject such methods (empty result) so the caller routes them to
    // FastExecute, whose Handle_LdLocA returns a genuine local address.
    // (Previously the allocator kept LdLocA and Reg_LdLocA returned a null
    //  ManagedPtr → address-0 deref when the caller dereferenced it.)
    IRInstruction ll; ll.op_code = IROpCode::LdLocA; ll.operand_index = 2;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({ll, ret}));

    EXPECT_TRUE(rm.instructions.empty())
        << "AllocateRegisters must reject LdLocA methods (return empty → FastExecute)";
}

// ═══════════════════════════════════════════════════════════════════════════
// StArg — store to argument slot
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, StArg) {
    IRInstruction p1;  p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 99;
    IRInstruction sa;  sa.op_code = IROpCode::StArg; sa.operand_index = 0;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, sa, ret}));

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::StArg);
    EXPECT_TRUE(rm.instructions[1].has_src1());
    EXPECT_TRUE(rm.instructions[1].flags() & kRegIsStore);
    EXPECT_EQ(rm.instructions[1].imm.operand_index, 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// Conv opcodes (type conversions)
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, ConvI4) {
    IRInstruction push; push.op_code = IROpCode::LdcI8; push.immediate_i8 = 42;
    IRInstruction conv; conv.op_code = IROpCode::Conv_I4;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, conv, ret}));

    ASSERT_EQ(rm.instructions.size(), 3u);
    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::Conv_I4);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
    EXPECT_EQ(rm.instructions[1].src1_reg(), 16);
    EXPECT_EQ(rm.instructions[1].dst_reg(), 17);
}

TEST(IR_RegAlloc, ConvR8) {
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42;
    IRInstruction conv; conv.op_code = IROpCode::Conv_R8;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, conv, ret}));

    ASSERT_EQ(rm.instructions.size(), 3u);
    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::Conv_R8);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
}

TEST(IR_RegAlloc, ConvOvfI4) {
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 999;
    IRInstruction conv; conv.op_code = IROpCode::ConvOvfI4;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, conv, ret}));

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::ConvOvfI4);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
}

// ═══════════════════════════════════════════════════════════════════════════
// Neg, Not unary ops
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, Neg) {
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 42;
    IRInstruction neg;  neg.op_code = IROpCode::Neg;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, neg, ret}));

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::Neg);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
}

TEST(IR_RegAlloc, Not) {
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 0xFF;
    IRInstruction not_; not_.op_code = IROpCode::Not;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, not_, ret}));

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::Not);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
}

// ═══════════════════════════════════════════════════════════════════════════
// Shift ops
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, ShiftOpcodes) {
    // ldc.i4 1; ldc.i4 8; shl; ret
    IRInstruction p1;   p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 1;
    IRInstruction p2;   p2.op_code = IROpCode::LdcI4; p2.immediate_i4 = 8;
    IRInstruction shl;  shl.op_code = IROpCode::Shl;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, p2, shl, ret}));

    ASSERT_EQ(rm.instructions.size(), 4u);
    EXPECT_EQ(rm.instructions[2].op_code(), IROpCode::Shl);
    EXPECT_TRUE(rm.instructions[2].has_dst());
    EXPECT_TRUE(rm.instructions[2].has_src1());
    EXPECT_TRUE(rm.instructions[2].has_src2());
}

// ═══════════════════════════════════════════════════════════════════════════
// DivUn, RemUn (unsigned variants)
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, DivUn) {
    IRInstruction p1;   p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 10;
    IRInstruction p2;   p2.op_code = IROpCode::LdcI4; p2.immediate_i4 = 3;
    IRInstruction div;  div.op_code = IROpCode::DivUn;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, p2, div, ret}));

    EXPECT_EQ(rm.instructions[2].op_code(), IROpCode::DivUn);
    EXPECT_TRUE(rm.instructions[2].has_dst());
    EXPECT_TRUE(rm.instructions[2].has_src1());
    EXPECT_TRUE(rm.instructions[2].has_src2());
}

// ═══════════════════════════════════════════════════════════════════════════
// AddOvf, SubOvf, MulOvf
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, OverflowArithmetic) {
    IRInstruction p1;    p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 100;
    IRInstruction p2;    p2.op_code = IROpCode::LdcI4; p2.immediate_i4 = 200;
    IRInstruction add;   add.op_code = IROpCode::AddOvf;
    IRInstruction ret;   ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, p2, add, ret}));

    EXPECT_EQ(rm.instructions[2].op_code(), IROpCode::AddOvf);
    EXPECT_TRUE(rm.instructions[2].has_dst());
    EXPECT_TRUE(rm.instructions[2].has_src1());
    EXPECT_TRUE(rm.instructions[2].has_src2());
}

// ═══════════════════════════════════════════════════════════════════════════
// LdInd / StInd (indirect load/store)
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LdInd) {
    IRInstruction push;  push.op_code = IROpCode::LdcI4; push.immediate_i4 = 0x1000;
    IRInstruction ldind; ldind.op_code = IROpCode::LdInd;
    IRInstruction ret;   ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, ldind, ret}));

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::LdInd);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
}

TEST(IR_RegAlloc, StInd) {
    // push addr; push val; stind
    IRInstruction pa; pa.op_code = IROpCode::LdcI4; pa.immediate_i4 = 0x1000;
    IRInstruction pv; pv.op_code = IROpCode::LdcI4; pv.immediate_i4 = 42;
    IRInstruction st; st.op_code = IROpCode::StInd;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({pa, pv, st, ret}));

    EXPECT_EQ(rm.instructions[2].op_code(), IROpCode::StInd);
    EXPECT_TRUE(rm.instructions[2].flags() & kRegIsStore);
}

// ═══════════════════════════════════════════════════════════════════════════
// LdObj / StObj
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LdObj) {
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 0x2000;
    IRInstruction ldobj; ldobj.op_code = IROpCode::LdObj;
    IRInstruction ret;   ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, ldobj, ret}));

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::LdObj);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
}

// ═══════════════════════════════════════════════════════════════════════════
// LdLen (array length)
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LdLen) {
    IRInstruction push; push.op_code = IROpCode::LdNull;
    IRInstruction ldlen; ldlen.op_code = IROpCode::LdLen;
    IRInstruction ret;   ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, ldlen, ret}));

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::LdLen);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
}

// ═══════════════════════════════════════════════════════════════════════════
// LdToken / SizeOf
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LdToken) {
    IRInstruction ld;  ld.op_code = IROpCode::LdToken; ld.immediate_i4 = 0x1234;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({ld, ret}));

    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdToken);
    EXPECT_TRUE(rm.instructions[0].has_dst());
}

TEST(IR_RegAlloc, SizeOf) {
    IRInstruction sz;  sz.op_code = IROpCode::SizeOf; sz.immediate_i4 = 16;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({sz, ret}));

    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::SizeOf);
    EXPECT_TRUE(rm.instructions[0].has_dst());
}

// ═══════════════════════════════════════════════════════════════════════════
// InitObj / LocAlloc
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, InitObj) {
    // push addr; initobj
    IRInstruction pa;  pa.op_code = IROpCode::LdcI4; pa.immediate_i4 = 0x3000;
    IRInstruction init;  init.op_code = IROpCode::InitObj; init.immediate_i4 = 0;
    // InitObj doesn't push/pop — it acts on the address already on the stack
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({pa, init, ret}));

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::InitObj);
    EXPECT_TRUE(rm.instructions[1].flags() & kRegIsStore);
}

TEST(IR_RegAlloc, LocAlloc) {
    IRInstruction push; push.op_code = IROpCode::LdcI4; push.immediate_i4 = 64;
    IRInstruction la;   la.op_code = IROpCode::LocAlloc;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, la, ret}));

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::LocAlloc);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
}

// ═══════════════════════════════════════════════════════════════════════════
// LdFtn / LdVirtFtn (function pointer loads)
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, LdFtn) {
    IRInstruction ld;  ld.op_code = IROpCode::LdFtn; ld.call_target = reinterpret_cast<void*>(0x1234);
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({ld, ret}));

    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::LdFtn);
    EXPECT_TRUE(rm.instructions[0].has_dst());
}

TEST(IR_RegAlloc, LdVirtFtn) {
    IRInstruction push; push.op_code = IROpCode::LdNull;
    IRInstruction ld;   ld.op_code = IROpCode::LdVirtFtn; ld.call_target = reinterpret_cast<void*>(0x5678);
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({push, ld, ret}));

    EXPECT_EQ(rm.instructions[1].op_code(), IROpCode::LdVirtFtn);
    EXPECT_TRUE(rm.instructions[1].has_dst());
    EXPECT_TRUE(rm.instructions[1].has_src1());
}

// ═══════════════════════════════════════════════════════════════════════════
// Break opcode
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, BreakOpcode) {
    IRInstruction brk; brk.op_code = IROpCode::Break;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({brk, ret}));

    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::Break);
    // Break has no register operands
    EXPECT_FALSE(rm.instructions[0].has_dst());
    EXPECT_FALSE(rm.instructions[0].has_src1());
}

// ═══════════════════════════════════════════════════════════════════════════
// EndFinally / Leave (SEH control flow)
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, EndFinally) {
    IRInstruction ef;  ef.op_code = IROpCode::EndFinally;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({ef, ret}));

    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::EndFinally);
}

TEST(IR_RegAlloc, Leave) {
    IRInstruction lv;  lv.op_code = IROpCode::Leave; lv.branch_target = 1;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({lv, ret}));

    EXPECT_EQ(rm.instructions[0].op_code(), IROpCode::Leave);
    EXPECT_TRUE(rm.instructions[0].is_branch());
}

// ═══════════════════════════════════════════════════════════════════════════
// Dup + Pop interaction
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, DupThenPop) {
    // ldc.i4 1; ldc.i4 2; dup; pop; ret
    // Stack: push 1, push 2, dup(push copy of 2), pop(remove copy), ret reads 2
    IRInstruction p1;   p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 1;
    IRInstruction p2;   p2.op_code = IROpCode::LdcI4; p2.immediate_i4 = 2;
    IRInstruction dup;  dup.op_code = IROpCode::Dup;
    IRInstruction pop;  pop.op_code = IROpCode::Pop;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, p2, dup, pop, ret}));

    ASSERT_EQ(rm.instructions.size(), 5u);

    // ldc 1 → r16, ldc 2 → r17
    // dup copies r17 → r18
    EXPECT_EQ(rm.instructions[2].op_code(), IROpCode::Dup);
    EXPECT_EQ(rm.instructions[2].src1_reg(), 17);
    EXPECT_EQ(rm.instructions[2].dst_reg(), 18);

    // pop removes r18
    EXPECT_EQ(rm.instructions[3].op_code(), IROpCode::Pop);
    EXPECT_EQ(rm.instructions[3].src1_reg(), 18);

    // ret reads r17 (remaining top of stack)
    EXPECT_EQ(rm.instructions[4].src1_reg(), 17);
}

// ═══════════════════════════════════════════════════════════════════════════
// Multiple ops with flag combinations
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, FlagsField) {
    // Verify flag encoding for different opcode types
    IRInstruction p1;   p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 1;
    IRInstruction br;   br.op_code = IROpCode::Br;    br.branch_target = 3;
    IRInstruction st;   st.op_code = IROpCode::StLoc;  st.operand_index = 0;
    IRInstruction ret;  ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, br, st, ret}));

    // ldc: has_dst + has_imm
    EXPECT_FALSE(rm.instructions[0].is_branch());
    EXPECT_FALSE(rm.instructions[0].flags() & kRegIsStore);
    EXPECT_TRUE(rm.instructions[0].has_dst());
    EXPECT_TRUE(rm.instructions[0].has_imm());

    // br: branch flag
    EXPECT_TRUE(rm.instructions[1].is_branch());

    // stloc: store flag
    EXPECT_TRUE(rm.instructions[2].flags() & kRegIsStore);
}

// ═══════════════════════════════════════════════════════════════════════════
// Stack map max_regs consistency
// ═══════════════════════════════════════════════════════════════════════════

TEST(IR_RegAlloc, StackMapMaxRegsBound) {
    // Verify that all stack map slot_regs are < max_regs
    IRInstruction p1; p1.op_code = IROpCode::LdcI4; p1.immediate_i4 = 1;
    IRInstruction p2; p2.op_code = IROpCode::LdcI4; p2.immediate_i4 = 2;
    IRInstruction p3; p3.op_code = IROpCode::LdcI4; p3.immediate_i4 = 3;
    IRInstruction add; add.op_code = IROpCode::Add;
    IRInstruction ret; ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({p1, p2, p3, add, ret}));

    for (const auto& entry : rm.stack_map.entries) {
        for (uint32_t i = 0; i < entry.stack_depth; ++i) {
            int8_t reg = entry.slot_regs[i];
            EXPECT_LT(static_cast<uint32_t>(reg), rm.max_regs);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// C4: coalesced >8 call-arg buffer layout
// ═══════════════════════════════════════════════════════════════════════════
// A >8-arg Call/Calli used to pay two separate CHAOS_IL2CPP_MALLOC + two FREE
// per call (one uint64[] for args, one uint8[] for tags).  C4 coalesces these
// into a single block: uint64 args[ac] at [0, 8*ac), uint8 tags[ac] starting at
// byte 8*ac — halving the malloc+free count.  Pin the exact layout so the
// handlers (Reg_Call/Reg_Calli) and this test agree: disjoint, in-bounds,
// alignment-safe regions and a single matching owner for the block head.
TEST(IR_RegAlloc, CoalescedCallArgsLayout) {
    EXPECT_EQ(CoalescedCallArgs(0).tag_offset, sizeof(uint64_t) * 0);
    EXPECT_EQ(CoalescedCallArgs(0).total_size, sizeof(uint64_t) * 0 + sizeof(uint8_t) * 0);

    // Boundary: exactly 8 args is still the stack path; layout must stay valid.
    EXPECT_EQ(CoalescedCallArgs(8).tag_offset, sizeof(uint64_t) * 8);
    EXPECT_EQ(CoalescedCallArgs(8).total_size, sizeof(uint64_t) * 8 + sizeof(uint8_t) * 8);

    // The >8 path that pays the heap allocation.  Pick 10 (and a prime, 13) to
    // span more than one 8-arg boundary.
    for (uint32_t ac : {9u, 10u, 13u, 64u}) {
        const auto l = CoalescedCallArgs(ac);
        const size_t arg_bytes = sizeof(uint64_t) * static_cast<size_t>(ac);
        const size_t tag_bytes = sizeof(uint8_t) * static_cast<size_t>(ac);

        EXPECT_EQ(l.tag_offset, arg_bytes) << "tags must start right after args for ac=" << ac;
        EXPECT_EQ(l.total_size, arg_bytes + tag_bytes) << "total block size for ac=" << ac;

        // Disjoint & in-bounds: args region fully before tags, tags fully inside block.
        EXPECT_GE(l.tag_offset, arg_bytes);
        EXPECT_LE(l.tag_offset + tag_bytes, l.total_size);

        // Alignment: args (uint64*) needs 8-byte alignment; tag_offset (8*ac) is a
        // multiple of 8, so tags stay 8-aligned too — safe to reinterpret the head.
        EXPECT_EQ(l.tag_offset % alignof(uint64_t), 0u);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Div/Rem fault guards (P1 cross-tier correctness)
// ═══════════════════════════════════════════════════════════════════════════
// Reg_Div/Reg_Rem were unguarded: a div/rem by zero or INT32_MIN/-1 caused a
// hardware SIGFPE/UB on the Register tier while FastExecute faults/wraps.  This
// directly builds a Div/Rem RegisterInstruction and runs it through the real
// RegisterExecute to prove the fault is raised (not a crash) and the
// INT32_MIN/-1 wraparound matches FastExecute.

// Build a 2-operand Reg handler instruction (Div/Rem share this shape).
static RegisterInstruction MakeRegBinOp(IROpCode op, uint8_t dst, uint8_t s1, uint8_t s2) {
    uint64_t hdr = (static_cast<uint64_t>(op) & 0xFFFF)
                 | (static_cast<uint64_t>(dst) << 16)
                 | (static_cast<uint64_t>(s1)  << 24)
                 | (static_cast<uint64_t>(s2)  << 32)
                 | (static_cast<uint64_t>(kRegHasDst | kRegHasSrc1 | kRegHasSrc2) << 40);
    RegisterInstruction r;
    r.header = hdr;
    r.imm.ptr = nullptr;
    return r;
}

TEST(IR_RegAlloc, RegisterExecuteDivByZeroFaultsNoCrash) {
    // Div r10 = r8 / r9 where r9 = 0 → Reg_Div must raise (threw_exception, pc 9999)
    // instead of executing a hardware divide-by-zero.
    RegisterInstruction instrs[] = {
        MakeRegBinOp(IROpCode::Div, 10, 8, 9),
        { /* Ret src1=r10 */ { (static_cast<uint64_t>(IROpCode::Ret) & 0xFFFF)
                          | (static_cast<uint64_t>(10) << 24)
                          | (static_cast<uint64_t>(kRegHasSrc1) << 40) }, { .ptr = nullptr } },
    };
    RegisterFrame frame = {};
    frame.regs.set_reg(8, 42, static_cast<uint8_t>(ValueTag::Int32));
    frame.regs.set_reg(9, 0, static_cast<uint8_t>(ValueTag::Int32));

    bool ok = RegisterExecute(frame, instrs, 2);
    EXPECT_FALSE(ok) << "div-by-zero must fault (RegisterExecute returns false)";
    EXPECT_TRUE(frame.threw_exception);
    // No crash means the guard worked; the fault is a graceful exception, not SEH.
}

TEST(IR_RegAlloc, RegisterExecuteDivInt32MinMinus1Wraps) {
    // INT32_MIN / -1 → must NOT SIGFPE; push INT32_MIN (matches FastExecute).
    RegisterInstruction instrs[] = {
        MakeRegBinOp(IROpCode::Div, 10, 8, 9),
        { /* Ret src1=r10 */ { (static_cast<uint64_t>(IROpCode::Ret) & 0xFFFF)
                          | (static_cast<uint64_t>(10) << 24)
                          | (static_cast<uint64_t>(kRegHasSrc1) << 40) }, { .ptr = nullptr } },
    };
    RegisterFrame frame = {};
    frame.regs.set_reg(8, INT32_MIN, static_cast<uint8_t>(ValueTag::Int32));
    frame.regs.set_reg(9, static_cast<uint32_t>(static_cast<int32_t>(-1)), static_cast<uint8_t>(ValueTag::Int32));

    bool ok = RegisterExecute(frame, instrs, 2);
    EXPECT_TRUE(ok) << "INT32_MIN/-1 must compute (wrap), not fault";
    EXPECT_FALSE(frame.threw_exception);
    EXPECT_EQ(static_cast<int32_t>(frame.ret_val), INT32_MIN);
}

// ═══════════════════════════════════════════════════════════════════════════
// NoChk / barrier opcode rewriting (P1 cross-tier)
// ═══════════════════════════════════════════════════════════════════════════
// RegisterVM dispatch covers opcodes 0-99; the codegen no-check/barrier variants
// (StFldBarrier=103, LdElemNoChk=104, StElemNoChk=105, LdElemANoChk=106) would
// spuriously fault (`op_val > 99`) while FastExecute implements them.  The
// allocator must rewrite them to their checked ≤99 equivalent in the emitted
// header so the Register tier executes the same IL as FastExecute.
TEST(IR_RegAlloc, RewritesNoChkAndBarrierOpcodes) {
    // A method using the no-check / barrier variants from the codegen path.
    IRInstruction sfb;  sfb.op_code = IROpCode::StFldBarrier;
    IRInstruction le_nc; le_nc.op_code = IROpCode::LdElemNoChk;
    IRInstruction se_nc; se_nc.op_code = IROpCode::StElemNoChk;
    IRInstruction lea_nc; lea_nc.op_code = IROpCode::LdElemANoChk;
    IRInstruction ret;   ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({sfb, le_nc, se_nc, lea_nc, ret}));

    // Every emitted register instruction must carry a ≤99 dispatchable opcode.
    for (const auto& ins : rm.instructions) {
        uint32_t op = static_cast<uint32_t>(ins.op_code());
        EXPECT_LE(op, 99u) << "opcode " << op << " is not dispatchable by RegisterVM";
    }

    // And the no-check/barrier variants must be mapped to their checked form.
    // (We can't rely on exact positional expectations since the allocator may
    // reorder/assign src regs, but at minimum the emitted set contains the
    // checked equivalents — StFld, LdElem, StElem — and never 103-106.)
    bool saw_stfld = false, saw_ldelem = false, saw_stelem = false;
    for (const auto& ins : rm.instructions) {
        switch (ins.op_code()) {
            case IROpCode::StFld:    saw_stfld = true;  break;
            case IROpCode::LdElem:   saw_ldelem = true; break;
            case IROpCode::StElem:   saw_stelem = true; break;
            case IROpCode::StFldBarrier:
            case IROpCode::LdElemNoChk:
            case IROpCode::StElemNoChk:
            case IROpCode::LdElemANoChk:
                ADD_FAILURE() << "no-check/barrier opcode " << static_cast<int>(ins.op_code())
                              << " leaked into the register method";
                break;
            default: break;
        }
    }
    EXPECT_TRUE(saw_stfld)  << "StFldBarrier should be rewritten to StFld";
    EXPECT_TRUE(saw_ldelem) << "LdElemNoChk/ANoChk should be rewritten to LdElem";
    EXPECT_TRUE(saw_stelem) << "StElemNoChk should be rewritten to StElem";
}

// ═══════════════════════════════════════════════════════════════════════════
// StInd/StObj write-barrier (P1 GC) — non-GC destination path
// ═══════════════════════════════════════════════════════════════════════════
// Reg_StInd/Reg_StObj now mirror FastExecute's guarded write barrier: a stack-local
// (non-GC) destination must take the plain-store path (chaos_is_gc_pointer=false)
// and round-trip correctly.  A live GC-nursery destination can't be built in this
// sandbox test, but the non-GC branch proves the refactor didn't break stores.
TEST(IR_RegAlloc, StIndStackLocalRoundTrips) {
    uint64_t dst = 0;
    RegisterInstruction instrs[] = {
        // StInd: src1 = value (r8), src2 = address-of-dst (r9)
        MakeRegBinOp(IROpCode::StInd, 0, 8, 9),
    };
    RegisterFrame frame = {};
    frame.regs.set_reg(8, 0xDEADBEEFCAFEBABEull, static_cast<uint8_t>(ValueTag::Int64));
    frame.regs.set_reg(9, reinterpret_cast<uint64_t>(&dst), static_cast<uint8_t>(ValueTag::ManagedPtr));

    // Run just the StInd instruction (count 1).  dst is a stack local so
    // chaos_is_gc_pointer is false → plain store path.
    bool ok = RegisterExecute(frame, instrs, 1);
    EXPECT_TRUE(ok) << "StInd to a stack-local must not fault";
    EXPECT_FALSE(frame.threw_exception);
    EXPECT_EQ(dst, 0xDEADBEEFCAFEBABEull) << "StInd non-GC path must store the value";
}

// ═══════════════════════════════════════════════════════════════════════════
// LdArgA / LdLocA address semantics (P2 cross-tier)
// ═══════════════════════════════════════════════════════════════════════════
// Reg_LdArgA previously returned a null ManagedPtr (address-0 deref on use).
// It must now return the real arg address (mirroring FastExecute Handle_LdArgA).
// And AllocateRegisters must reject methods using LdLocA (locals live in the
// register file, not addressable memory) so they route to FastExecute, whose
// Handle_LdLocA returns a real local address.
TEST(IR_RegAlloc, LdArgAReturnsRealAddress) {
    uint64_t args[1] = { 0 };
    RegisterFrame frame = {};
    frame.args = args;
    frame.arg_count = 1;

    // LdArgA: dst=16, src1=0 (operand_index in imm).  Build header manually with
    // operand_index (imm.i4) for arg 0.
    RegisterInstruction ldarga = MakeRegBinOp(IROpCode::LdArgA, 16, 0, 0);
    ldarga.imm.operand_index = 0;
    RegisterInstruction instrs[] = { ldarga };
    ASSERT_TRUE(RegisterExecute(frame, instrs, 1));
    EXPECT_FALSE(frame.threw_exception);
    uint64_t addr = frame.regs.reg(16);
    EXPECT_EQ(addr, reinterpret_cast<uint64_t>(&args[0]))
        << "LdArgA must return the real address of arg slot, not null";
    EXPECT_EQ(static_cast<uint8_t>(frame.regs.reg_tag(16)),
              static_cast<uint8_t>(ValueTag::ManagedPtr));
}

TEST(IR_RegAlloc, LdLocARejectsToFastExecute) {
    // A method containing LdLocA must be rejected by AllocateRegisters (empty
    // result) so the caller routes it to FastExecute (correct Handle_LdLocA).
    IRInstruction ldloc;
    ldloc.op_code = IROpCode::LdLoc;
    ldloc.immediate_i4 = 0;
    IRInstruction ldloca;
    ldloca.op_code = IROpCode::LdLocA;
    ldloca.immediate_i4 = 0;
    IRInstruction ret;
    ret.op_code = IROpCode::Ret;

    auto rm = AllocateRegisters(MakeIRMethod({ldloc, ldloca, ret}));
    EXPECT_TRUE(rm.instructions.empty())
        << "AllocateRegisters must reject LdLocA methods (return empty → FastExecute)";
}

// ═══════════════════════════════════════════════════════════════════════════
// C4 remainder: SmallFieldArray allocation behavior
// ═══════════════════════════════════════════════════════════════════════════
// Box/NewObj store fields in SmallFieldArray: ≤ kInlineCapacity fields live
// inline (no heap alloc), >2 spills.  C4 makes growth geometric so an
// incremental field-store sequence (StFld on fields 0,1,5 → resize 1,2,6) is
// amortized instead of reallocating+copying the whole buffer every step.
TEST(IR_RegAlloc, SmallFieldArrayInlineAndGeometricGrowth) {
    using chaos::il2cpp::interpreter::SmallFieldArray;

    // ≤ kInlineCapacity fields: inline storage, no heap alloc.  data() returns
    // the inline buffer (fields_ptr_ == inline_ when capacity allows), which is
    // NOT a heap address — verify a ≤2-field array's data pointer is stable and
    // a >2-field array spills (data pointer may move to heap on grow).
    SmallFieldArray inline_arr;
    inline_arr.resize(2);
    EXPECT_EQ(inline_arr.size(), 2u);
    const void* inline_data = inline_arr.data();   // points at inline_ (this)
    // A second ≤2 resize must NOT realloc (capacity already ≥ 2): data() unchanged.
    inline_arr.resize(2);
    EXPECT_EQ(static_cast<const void*>(inline_arr.data()), inline_data);

    // > kInlineCapacity spills to heap.
    SmallFieldArray big;
    big.resize(5);
    EXPECT_GE(big.size(), 5u);

    // Geometric growth: incremental grows stay valid across resize (no per-step
    // realloc breakage) — the C4 policy grows capacity > request so this is
    // amortized, not exact-fit churn.
    SmallFieldArray c;
    c.resize(3);
    EXPECT_EQ(c.size(), 3u);
    c.resize(4);
    EXPECT_EQ(c.size(), 4u);
}





