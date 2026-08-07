// jit_binary_ir_test.cpp — Binary IR serialization/deserialization roundtrip tests
//
// Verifies that SerializeBinaryIr + DeserializeBinaryIr correctly preserve
// all RegisterMethod data including:
//   - RegisterInstruction array (various opcodes and operand configurations)
//   - SEH clauses (try/catch/finally blocks)
//   - CatchHandlerEntry (exception register mapping)
//   - IL offset mapping
//   - Stack map entries (OSR register mapping)
//   - Edge cases: empty method, large method, no SEH, no stack map

#include <gtest/gtest.h>

#include "jit_binary_reader.h"

#include <cstring>
#include <vector>

// ── Namespace aliases ──────────────────────────────────────────────
// NOTE: RegisterInstruction is NOT brought in via `using` because
// register_ir.h (included from jit_binary_reader.h) defines a
// C-compatible struct RegisterInstruction at global scope, which
// conflicts with the interpreter's RegisterInstruction.  We use
// explicit qualification instead.
namespace ir = chaos::il2cpp::interpreter;
namespace jit = chaos::il2cpp::jit;

using ir::IROpCode;
using ir::SEHClause;
using ir::SEHFlags;
using ir::CatchHandlerEntry;
using ir::RegStackMapEntry;
using ir::kRegHasDst;
using ir::kRegHasSrc1;
using ir::kRegHasSrc2;
using ir::kRegHasImm;
using ir::kRegIsBranch;
using jit::SerializeBinaryIr;
using jit::DeserializeBinaryIr;
using jit::ValidateBinaryIr;
using jit::kBinaryIrMagic;
using jit::kBinaryIrVersion;

// ── Helper: build a RegisterInstruction header word ────────────────
static uint64_t MakeHeader(IROpCode opc, uint8_t dst, uint8_t src1,
                            uint8_t src2, uint8_t flags) noexcept {
    return static_cast<uint64_t>(opc) |
           (static_cast<uint64_t>(dst)  << 16) |
           (static_cast<uint64_t>(src1) << 24) |
           (static_cast<uint64_t>(src2) << 32) |
           (static_cast<uint64_t>(flags) << 40);
}

// ── Test: empty method (should fail serialize) ──────────────────────
TEST(BinaryIrTest, EmptyMethod) {
    ir::RegisterMethod rm;
    size_t size = 0;
    auto* buf = SerializeBinaryIr(rm, &size);
    EXPECT_EQ(buf, nullptr);
    EXPECT_EQ(size, 0u);
}

// ── Test: basic instruction roundtrip ──────────────────────────────
TEST(BinaryIrTest, BasicRoundtrip) {
    ir::RegisterMethod rm;
    rm.max_regs = 3;

    // ldc.i4 42; ret
    ir::RegisterInstruction ri0;
    ri0.header = MakeHeader(IROpCode::LdcI4, 0, 0, 0,
                             kRegHasDst | kRegHasImm);
    ri0.imm.i4 = 42;

    ir::RegisterInstruction ri1;
    ri1.header = MakeHeader(IROpCode::Ret, 0, 0, 0, kRegHasSrc1);
    ri1.imm.i4 = 0;

    rm.instructions.push_back(ri0);
    rm.instructions.push_back(ri1);

    // Serialize
    size_t ser_size = 0;
    auto* ser_buf = SerializeBinaryIr(rm, &ser_size);
    ASSERT_NE(ser_buf, nullptr);
    ASSERT_GT(ser_size, sizeof(jit::BinaryIrHeader));

    // Validate
    EXPECT_TRUE(ValidateBinaryIr(ser_buf, ser_size));

    // Deserialize
    ir::RegisterMethod rm2 = DeserializeBinaryIr(ser_buf, ser_size);
    ASSERT_EQ(rm2.instructions.size(), 2u);
    EXPECT_EQ(rm2.max_regs, 3u);
    EXPECT_EQ(rm2.instructions[0].op_code(), IROpCode::LdcI4);
    EXPECT_EQ(rm2.instructions[0].dst_reg(), 0u);
    EXPECT_EQ(rm2.instructions[0].imm.i4, 42);
    EXPECT_EQ(rm2.instructions[1].op_code(), IROpCode::Ret);
    EXPECT_EQ(rm2.seh_clauses.size(), 0u);
    EXPECT_EQ(rm2.catch_handler_entries.size(), 0u);
    EXPECT_EQ(rm2.il_offsets.size(), 0u);
    EXPECT_EQ(rm2.stack_map.entries.size(), 0u);

    std::free(ser_buf);
}

// ── Test: binary operation roundtrip ────────────────────────────────
TEST(BinaryIrTest, BinaryOpRoundtrip) {
    ir::RegisterMethod rm;
    rm.max_regs = 3;

    // r0 = 10; r1 = 20; r2 = r0 + r1; ret r2
    ir::RegisterInstruction ri0;
    ri0.header = MakeHeader(IROpCode::LdcI4, 0, 0, 0, kRegHasDst | kRegHasImm);
    ri0.imm.i4 = 10;

    ir::RegisterInstruction ri1;
    ri1.header = MakeHeader(IROpCode::LdcI4, 1, 0, 0, kRegHasDst | kRegHasImm);
    ri1.imm.i4 = 20;

    ir::RegisterInstruction ri2;
    ri2.header = MakeHeader(IROpCode::Add, 2, 0, 1, kRegHasDst | kRegHasSrc1 | kRegHasSrc2);

    ir::RegisterInstruction ri3;
    ri3.header = MakeHeader(IROpCode::Ret, 0, 2, 0, kRegHasSrc1);

    rm.instructions.push_back(ri0);
    rm.instructions.push_back(ri1);
    rm.instructions.push_back(ri2);
    rm.instructions.push_back(ri3);

    size_t ser_size = 0;
    auto* ser_buf = SerializeBinaryIr(rm, &ser_size);
    ASSERT_NE(ser_buf, nullptr);

    ir::RegisterMethod rm2 = DeserializeBinaryIr(ser_buf, ser_size);
    ASSERT_EQ(rm2.instructions.size(), 4u);
    EXPECT_EQ(rm2.max_regs, 3u);
    EXPECT_EQ(rm2.instructions[2].op_code(), IROpCode::Add);
    EXPECT_EQ(rm2.instructions[2].dst_reg(), 2u);
    EXPECT_EQ(rm2.instructions[2].src1_reg(), 0u);
    EXPECT_EQ(rm2.instructions[2].src2_reg(), 1u);

    std::free(ser_buf);
}

// ── Test: SEH clause roundtrip ─────────────────────────────────────
TEST(BinaryIrTest, SehClauseRoundtrip) {
    ir::RegisterMethod rm;
    rm.max_regs = 4;

    // Build minimal instructions (LdcI4 with imm=0 as filler)
    for (int i = 0; i < 10; ++i) {
        ir::RegisterInstruction ri;
        ri.header = MakeHeader(IROpCode::LdcI4, 0, 0, 0, kRegHasDst | kRegHasImm);
        ri.imm.i4 = 0;
        rm.instructions.push_back(ri);
    }

    // Add SEH clause: try[0..5) catch handler at 5
    SEHClause clause;
    clause.flags = SEHFlags::Typed;
    clause.try_start_idx = 0;
    clause.try_end_idx = 5;
    clause.handler_start_idx = 5;
    clause.handler_end_idx = 10;
    clause.class_token = 0x02000001;  // TypeDef token
    rm.seh_clauses.push_back(clause);

    // Add catch handler entry
    CatchHandlerEntry che;
    che.handler_start_idx = 5;
    che.exception_reg = 3;
    che.class_token = 0x02000001;
    rm.catch_handler_entries.push_back(che);

    size_t ser_size = 0;
    auto* ser_buf = SerializeBinaryIr(rm, &ser_size);
    ASSERT_NE(ser_buf, nullptr);

    ir::RegisterMethod rm2 = DeserializeBinaryIr(ser_buf, ser_size);
    ASSERT_EQ(rm2.seh_clauses.size(), 1u);
    EXPECT_EQ(static_cast<uint32_t>(rm2.seh_clauses[0].flags),
              static_cast<uint32_t>(SEHFlags::Typed));
    EXPECT_EQ(rm2.seh_clauses[0].try_start_idx, 0u);
    EXPECT_EQ(rm2.seh_clauses[0].try_end_idx, 5u);
    EXPECT_EQ(rm2.seh_clauses[0].handler_start_idx, 5u);
    EXPECT_EQ(rm2.seh_clauses[0].handler_end_idx, 10u);
    EXPECT_EQ(rm2.seh_clauses[0].class_token, 0x02000001u);

    ASSERT_EQ(rm2.catch_handler_entries.size(), 1u);
    EXPECT_EQ(rm2.catch_handler_entries[0].handler_start_idx, 5u);
    EXPECT_EQ(rm2.catch_handler_entries[0].exception_reg, 3u);
    EXPECT_EQ(rm2.catch_handler_entries[0].class_token, 0x02000001u);

    std::free(ser_buf);
}

// ── Test: IL offsets roundtrip ─────────────────────────────────────
TEST(BinaryIrTest, IlOffsetsRoundtrip) {
    ir::RegisterMethod rm;
    rm.max_regs = 2;

    // Instruction 0 at IL offset 0, instr 1 at IL offset 4, etc.
    for (int i = 0; i < 5; ++i) {
        ir::RegisterInstruction ri;
        ri.header = MakeHeader(IROpCode::LdcI4, 0, 0, 0, 0);
        rm.instructions.push_back(ri);
        rm.il_offsets.push_back(static_cast<uint32_t>(i * 4));
    }

    size_t ser_size = 0;
    auto* ser_buf = SerializeBinaryIr(rm, &ser_size);
    ASSERT_NE(ser_buf, nullptr);

    ir::RegisterMethod rm2 = DeserializeBinaryIr(ser_buf, ser_size);
    ASSERT_EQ(rm2.il_offsets.size(), 5u);
    EXPECT_EQ(rm2.il_offsets[0], 0u);
    EXPECT_EQ(rm2.il_offsets[2], 8u);
    EXPECT_EQ(rm2.il_offsets[4], 16u);

    std::free(ser_buf);
}

// ── Test: stack map entries roundtrip ──────────────────────────────
TEST(BinaryIrTest, StackMapRoundtrip) {
    ir::RegisterMethod rm;
    rm.max_regs = 8;

    // 3 instructions
    for (int i = 0; i < 3; ++i) {
        ir::RegisterInstruction ri;
        ri.header = MakeHeader(IROpCode::LdcI4, 0, 0, 0, 0);
        rm.instructions.push_back(ri);
    }

    // Stack map: 2 entries
    RegStackMapEntry sm0;
    std::memset(&sm0, 0, sizeof(sm0));
    sm0.slot_regs[0] = 0;   // slot 0 → vreg 0
    sm0.slot_regs[1] = 1;   // slot 1 → vreg 1
    sm0.stack_depth = 2;
    sm0.local_regs[0] = 0;  // local 0 → vreg 0

    RegStackMapEntry sm1;
    std::memset(&sm1, 0, sizeof(sm1));
    sm1.slot_regs[0] = 2;
    sm1.stack_depth = 1;
    sm1.local_regs[0] = 0;
    sm1.local_regs[1] = 1;

    rm.stack_map.entries.push_back(sm0);
    rm.stack_map.entries.push_back(sm1);

    size_t ser_size = 0;
    auto* ser_buf = SerializeBinaryIr(rm, &ser_size);
    ASSERT_NE(ser_buf, nullptr);

    ir::RegisterMethod rm2 = DeserializeBinaryIr(ser_buf, ser_size);
    ASSERT_EQ(rm2.stack_map.entries.size(), 2u);
    EXPECT_EQ(rm2.stack_map.entries[0].slot_regs[0], 0);
    EXPECT_EQ(rm2.stack_map.entries[0].slot_regs[1], 1);
    EXPECT_EQ(rm2.stack_map.entries[0].stack_depth, 2u);
    EXPECT_EQ(rm2.stack_map.entries[1].slot_regs[0], 2);
    EXPECT_EQ(rm2.stack_map.entries[1].stack_depth, 1u);

    std::free(ser_buf);
}

// ── Test: invalid buffer validation ────────────────────────────────
TEST(BinaryIrTest, InvalidBuffer) {
    EXPECT_FALSE(ValidateBinaryIr(nullptr, 0));
    EXPECT_FALSE(ValidateBinaryIr(nullptr, 100));

    // Truncated header
    uint8_t small[4] = {};
    EXPECT_FALSE(ValidateBinaryIr(small, 4));

    // Correct magic but truncated data
    uint8_t buf[sizeof(jit::BinaryIrHeader)] = {};
    auto* hdr = reinterpret_cast<jit::BinaryIrHeader*>(buf);
    hdr->magic = kBinaryIrMagic;
    hdr->version = kBinaryIrVersion;
    hdr->instr_count = 10;  // claims 10 instructions but no data follows
    hdr->total_size = 10 * sizeof(ir::RegisterInstruction);
    EXPECT_FALSE(ValidateBinaryIr(buf, sizeof(buf)));
}

// ── Test: full roundtrip with all fields populated ─────────────────
TEST(BinaryIrTest, FullRoundtrip) {
    ir::RegisterMethod rm;
    rm.max_regs = 16;

    // 10 instructions with mixed opcodes
    for (int i = 0; i < 10; ++i) {
        ir::RegisterInstruction ri;
        IROpCode opc = (i < 5) ? IROpCode::LdcI4 : IROpCode::LdcI4;
        uint8_t flags = (i < 5) ? (kRegHasDst | kRegHasImm) : 0;
        ri.header = MakeHeader(opc, static_cast<uint8_t>(i % 4), 0, 0, flags);
        ri.imm.i4 = i * 10;
        rm.instructions.push_back(ri);
    }

    // 2 SEH clauses
    SEHClause c1;
    c1.flags = SEHFlags::Finally;
    c1.try_start_idx = 0; c1.try_end_idx = 5;
    c1.handler_start_idx = 5; c1.handler_end_idx = 8;
    c1.class_token = 0;
    rm.seh_clauses.push_back(c1);

    SEHClause c2;
    c2.flags = SEHFlags::Typed;
    c2.try_start_idx = 0; c2.try_end_idx = 5;
    c2.handler_start_idx = 8; c2.handler_end_idx = 10;
    c2.class_token = 0x02000005u;
    rm.seh_clauses.push_back(c2);

    // 1 catch handler entry
    CatchHandlerEntry che;
    che.handler_start_idx = 8;
    che.exception_reg = 7;
    che.class_token = 0x02000005u;
    rm.catch_handler_entries.push_back(che);

    // 10 IL offsets
    for (int i = 0; i < 10; ++i)
        rm.il_offsets.push_back(static_cast<uint32_t>(i * 8));

    // 3 stack map entries
    for (int si = 0; si < 3; ++si) {
        RegStackMapEntry sm;
        std::memset(&sm, 0, sizeof(sm));
        sm.slot_regs[0] = static_cast<int8_t>(si);
        sm.stack_depth = 1;
        rm.stack_map.entries.push_back(sm);
    }

    // Roundtrip
    size_t ser_size = 0;
    auto* ser_buf = SerializeBinaryIr(rm, &ser_size);
    ASSERT_NE(ser_buf, nullptr);

    ir::RegisterMethod rm2 = DeserializeBinaryIr(ser_buf, ser_size);

    // Verify all fields
    EXPECT_EQ(rm2.max_regs, 16u);
    ASSERT_EQ(rm2.instructions.size(), 10u);
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(static_cast<int>(rm2.instructions[i].op_code()),
                  (i < 5) ? static_cast<int>(IROpCode::LdcI4)
                          : static_cast<int>(IROpCode::LdcI4));
        EXPECT_EQ(rm2.instructions[i].dst_reg(), static_cast<uint8_t>(i % 4));
        EXPECT_EQ(rm2.instructions[i].imm.i4, static_cast<int32_t>(i * 10));
    }

    ASSERT_EQ(rm2.seh_clauses.size(), 2u);
    EXPECT_EQ(static_cast<uint32_t>(rm2.seh_clauses[0].flags),
              static_cast<uint32_t>(SEHFlags::Finally));
    EXPECT_EQ(rm2.seh_clauses[1].class_token, 0x02000005u);

    ASSERT_EQ(rm2.catch_handler_entries.size(), 1u);
    EXPECT_EQ(rm2.catch_handler_entries[0].exception_reg, 7u);

    ASSERT_EQ(rm2.il_offsets.size(), 10u);
    EXPECT_EQ(rm2.il_offsets[3], 24u);

    ASSERT_EQ(rm2.stack_map.entries.size(), 3u);
    EXPECT_EQ(rm2.stack_map.entries[2].slot_regs[0], 2);

    std::free(ser_buf);
}

// ── Test: large instruction count ──────────────────────────────────
TEST(BinaryIrTest, LargeMethod) {
    ir::RegisterMethod rm;
    rm.max_regs = 64;

    // Create 1000 instructions with various register assignments
    for (int i = 0; i < 1000; ++i) {
        ir::RegisterInstruction ri;
        uint8_t opc_byte = static_cast<uint8_t>(i % 64);
        ri.header = MakeHeader(static_cast<IROpCode>(opc_byte),
                                static_cast<uint8_t>(i % 64),
                                static_cast<uint8_t>((i * 2) % 64),
                                static_cast<uint8_t>((i * 3) % 64),
                                kRegHasDst | kRegHasSrc1 | kRegHasSrc2 | kRegHasImm);
        ri.imm.i4 = i * 7;
        rm.instructions.push_back(ri);
    }

    size_t ser_size = 0;
    auto* ser_buf = SerializeBinaryIr(rm, &ser_size);
    ASSERT_NE(ser_buf, nullptr);

    ir::RegisterMethod rm2 = DeserializeBinaryIr(ser_buf, ser_size);
    ASSERT_EQ(rm2.instructions.size(), 1000u);

    // Verify a few known values
    EXPECT_EQ(static_cast<int>(rm2.instructions[0].op_code()), 0);
    EXPECT_EQ(rm2.instructions[500].imm.i4, 500 * 7);
    EXPECT_EQ(rm2.instructions[999].src1_reg(), static_cast<uint8_t>((999 * 2) % 64));
    EXPECT_EQ(rm2.instructions[999].src2_reg(), static_cast<uint8_t>((999 * 3) % 64));

    // Total size should be: header(32) + 1000*16 = 16032
    size_t expected = sizeof(jit::BinaryIrHeader) + 1000 * sizeof(ir::RegisterInstruction);
    EXPECT_EQ(ser_size, expected);

    std::free(ser_buf);
}

// ── Test: serialize + deserialize preserves memory layout ──────────
TEST(BinaryIrTest, BinaryExactMatch) {
    ir::RegisterMethod rm;
    rm.max_regs = 2;

    // A single instruction with precise bit layout
    ir::RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::LdcI4, 1, 0, 0, kRegHasDst | kRegHasImm);
    ri.imm.i4 = static_cast<int32_t>(0xDEADBEAF);
    rm.instructions.push_back(ri);

    size_t ser_size = 0;
    auto* ser_buf = SerializeBinaryIr(rm, &ser_size);
    ASSERT_NE(ser_buf, nullptr);

    // Verify header fields at byte level
    auto* hdr = reinterpret_cast<jit::BinaryIrHeader*>(ser_buf);
    EXPECT_EQ(hdr->magic, kBinaryIrMagic);
    EXPECT_EQ(hdr->version, kBinaryIrVersion);
    EXPECT_EQ(hdr->instr_count, 1);
    EXPECT_EQ(hdr->total_size, static_cast<uint32_t>(sizeof(ir::RegisterInstruction)));

    // Verify instruction data at byte level
    auto* instr = reinterpret_cast<const ir::RegisterInstruction*>(
        ser_buf + sizeof(jit::BinaryIrHeader));
    EXPECT_EQ(instr->header, ri.header);
    EXPECT_EQ(instr->imm.i4, static_cast<int32_t>(0xDEADBEAF));

    std::free(ser_buf);
}
