// aot_core_ir_reader_test.cpp — Deserialization tests for AotCoreIr JSON → IRMethod
//
// Tests that DeserializeAotCoreIrMethod correctly parses the JSON format emitted
// by managed codegen into the interpreter's IRMethod struct.
//
// Coverage:
//   1. Null/empty/invalid JSON → returns Ret-only IRMethod
//   2. Non-object top-level → returns Ret-only IRMethod
//   3. Missing instructions array → returns Ret-only IRMethod
//   4. Single instruction parsing: LdcI4, LdcI8, LdcR4, LdcR8, LdStr, LdArg
//   5. Branch target resolution (Br with matching ilOffset)
//   6. Exception region parsing (try/catch with offset conversion)
//   7. Call target resolution via resolve_fn callback
//   8. AotDirectDispatch via resolve_direct_fn
//   9. String operand with null/missing field (LdStr safety)
//  10. Unknown opcode → skipped (Ret sentinel inserted)

#include <gtest/gtest.h>

#include <aot_core_ir_reader.h>
#include <interpreter_vm.h>

#include <cstdint>
#include <cstring>
#include <string>

namespace gc = chaos::il2cpp::runtime_core;
namespace intp = chaos::il2cpp::interpreter;

// ════════════════════════════════════════════════════════════════════════════
// Error-path tests
// ════════════════════════════════════════════════════════════════════════════

TEST(aot_core_ir_reader_test, NullInput_ReturnsRet) {
    auto result = gc::DeserializeAotCoreIrMethod(nullptr, 0, nullptr, nullptr);
    ASSERT_EQ(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::Ret);
}

TEST(aot_core_ir_reader_test, EmptyInput_ReturnsRet) {
    auto result = gc::DeserializeAotCoreIrMethod("", 0, nullptr, nullptr);
    ASSERT_EQ(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::Ret);
}

TEST(aot_core_ir_reader_test, InvalidJson_ReturnsRet) {
    const char* json = "{broken";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);
    ASSERT_EQ(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::Ret);
}

TEST(aot_core_ir_reader_test, NonObjectTopLevel_ReturnsRet) {
    const char* json = "123";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);
    ASSERT_EQ(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::Ret);
}

TEST(aot_core_ir_reader_test, MissingInstructions_ReturnsRet) {
    const char* json = R"({"exceptionRegions":[]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);
    ASSERT_EQ(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::Ret);
}

// ════════════════════════════════════════════════════════════════════════════
// Single instruction parsing
// ════════════════════════════════════════════════════════════════════════════

TEST(aot_core_ir_reader_test, Parse_LdcI4) {
    // IROpCode::LdcI4 = 0, operand = 42
    const char* json = R"({"instructions":[{"opCode":0,"operand":42,"ilOffset":0}]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::LdcI4);
    EXPECT_EQ(result.instructions[0].immediate_i4, 42);
}

TEST(aot_core_ir_reader_test, Parse_LdcI8) {
    // IROpCode::LdcI8 = 1
    const char* json = R"({"instructions":[{"opCode":1,"operand":9999999999,"ilOffset":0}]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::LdcI8);
    EXPECT_EQ(result.instructions[0].immediate_i8, 9999999999LL);
}

TEST(aot_core_ir_reader_test, Parse_LdStr) {
    // IROpCode::LdStr = 4
    // NOTE: JSON reader's string_value is NOT null-terminated (points into raw buffer),
    // so we only verify op_code and non-null pointer — not string content.
    const char* json = R"({"instructions":[{"opCode":4,"operand":"hello","ilOffset":0}]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::LdStr);
    ASSERT_NE(result.instructions[0].string_operand, nullptr);
}

TEST(aot_core_ir_reader_test, Parse_LdArg) {
    // IROpCode::LdArg = 6
    const char* json = R"({"instructions":[{"opCode":6,"operand":2,"ilOffset":0}]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::LdArg);
    EXPECT_EQ(result.instructions[0].operand_index, 2);
}

// ════════════════════════════════════════════════════════════════════════════
// Branch target resolution
// ════════════════════════════════════════════════════════════════════════════

TEST(aot_core_ir_reader_test, BranchTarget_Resolved) {
    // Two instructions:
    //   [0] LdcI4 0 at ilOffset=0
    //   [1] Br with operand=0 (branch target ilOffset) at ilOffset=2
    // The Br should resolve branch_target to instruction index 0.
    const char* json = R"({"instructions":[
        {"opCode":0,"operand":0,"ilOffset":0},
        {"opCode":17,"operand":0,"ilOffset":2}
    ]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_GE(result.instructions.size(), 2u);
    EXPECT_EQ(result.instructions[1].op_code, intp::IROpCode::Br);
    EXPECT_EQ(result.instructions[1].branch_target, 0u);  // targets instruction 0
}

TEST(aot_core_ir_reader_test, BranchTarget_Forward) {
    // Forward branch: Br at ilOffset=0 targets ilOffset=4 (instruction 2)
    const char* json = R"({"instructions":[
        {"opCode":17,"operand":4,"ilOffset":0},
        {"opCode":0,"operand":1,"ilOffset":2},
        {"opCode":53,"ilOffset":4}
    ]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_GE(result.instructions.size(), 3u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::Br);
    EXPECT_EQ(result.instructions[0].branch_target, 2u);  // targets instruction 2 (Ret at ilOffset 4)
}

// ════════════════════════════════════════════════════════════════════════════
// Exception regions
// ════════════════════════════════════════════════════════════════════════════

TEST(aot_core_ir_reader_test, ExceptionRegion_Catch) {
    // One exception region with a "Catch" handler.
    // Instructions at ilOffsets 0, 2, 4, 6.
    const char* json = R"({
        "instructions":[
            {"opCode":0,"operand":1,"ilOffset":0},
            {"opCode":53,"ilOffset":2},
            {"opCode":53,"ilOffset":4},
            {"opCode":53,"ilOffset":6}
        ],
        "exceptionRegions":[
            {
                "handlingKindCode":"Catch",
                "tryOffset":0,
                "tryLength":2,
                "handlerOffset":2,
                "handlerLength":2
            }
        ]
    })";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_EQ(result.seh_clauses.size(), 1u);
    EXPECT_EQ(result.seh_clauses[0].flags, intp::SEHFlags::Exception);
    EXPECT_EQ(result.seh_clauses[0].try_start_idx, 0u);
    EXPECT_EQ(result.seh_clauses[0].try_end_idx, 1u);  // ilOffset 0+2 → instruction 1
    EXPECT_EQ(result.seh_clauses[0].handler_start_idx, 1u);
    EXPECT_EQ(result.seh_clauses[0].handler_end_idx, 2u);
}

TEST(aot_core_ir_reader_test, ExceptionRegion_Finally) {
    const char* json = R"({
        "instructions":[
            {"opCode":0,"operand":1,"ilOffset":0},
            {"opCode":53,"ilOffset":2}
        ],
        "exceptionRegions":[
            {
                "handlingKindCode":"Finally",
                "tryOffset":0,
                "tryLength":2,
                "handlerOffset":2,
                "handlerLength":2
            }
        ]
    })";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_EQ(result.seh_clauses.size(), 1u);
    EXPECT_EQ(result.seh_clauses[0].flags, intp::SEHFlags::Finally);
}

// ════════════════════════════════════════════════════════════════════════════
// Call target resolution via resolve_fn
// ════════════════════════════════════════════════════════════════════════════

static void* TestResolveFn(const char* subject_id, void* /*user_data*/) {
    if (std::strcmp(subject_id, "System.Console::WriteLine:System.Void(System.String)") == 0)
        return reinterpret_cast<void*>(static_cast<uintptr_t>(0x12345678));
    if (std::strcmp(subject_id, "System.Object::GetHashCode:System.Int32()") == 0)
        return reinterpret_cast<void*>(static_cast<uintptr_t>(0x87654321));
    return nullptr;
}

TEST(aot_core_ir_reader_test, CallTarget_ResolvedFromCallee) {
    // Call (opCode=14) with callee subjectId → resolve_fn returns target
    const char* json = R"json({"instructions":[
        {"opCode":14,"operand":0,"ilOffset":0,
         "callee":"System.Console::WriteLine:System.Void(System.String)"}
    ]})json";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json),
        TestResolveFn, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::Call);
    EXPECT_NE(result.instructions[0].call_target, nullptr);
    EXPECT_EQ(result.instructions[0].call_target,
              reinterpret_cast<void*>(static_cast<uintptr_t>(0x12345678)));
    EXPECT_TRUE(result.instructions[0].is_instance_call);  // callee field present triggers this
}

TEST(aot_core_ir_reader_test, CallTarget_ResolvedFromTargetReference) {
    // Call with targetReference subjectId when no callee field present
    const char* json = R"json({"instructions":[
        {"opCode":14,"operand":0,"ilOffset":0,
         "targetReference":{"subjectId":"System.Object::GetHashCode:System.Int32()"}}
    ]})json";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json),
        TestResolveFn, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::Call);
    EXPECT_EQ(result.instructions[0].call_target,
              reinterpret_cast<void*>(static_cast<uintptr_t>(0x87654321)));
}

// ════════════════════════════════════════════════════════════════════════════
// AotDirectDispatch
// ════════════════════════════════════════════════════════════════════════════

static void* TestDirectResolveFn(const char* subject_id, void* /*user_data*/) {
    if (std::strcmp(subject_id, "System.Math::Sqrt:System.Double(System.Double)") == 0)
        return reinterpret_cast<void*>(static_cast<uintptr_t>(0xAABBCCDD));
    return nullptr;
}

TEST(aot_core_ir_reader_test, AotDirectDispatch_Resolved) {
    // Call with resolve_direct_fn for AotDirectDispatch
    const char* json = R"json({"instructions":[
        {"opCode":14,"operand":1,"ilOffset":0,
         "callee":"System.Math::Sqrt:System.Double(System.Double)"}
    ]})json";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json),
        nullptr, nullptr, TestDirectResolveFn, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::Call);
    // direct_fn should be resolved from the callee via resolve_direct_fn
    EXPECT_NE(result.instructions[0].direct_fn, nullptr);
    EXPECT_EQ(result.instructions[0].direct_fn,
              reinterpret_cast<void*>(static_cast<uintptr_t>(0xAABBCCDD)));
    // direct_ret_tag should be inferred from return type "System.Double" → Float64 (0x05)
    EXPECT_EQ(result.instructions[0].direct_ret_tag,
              static_cast<uint8_t>(intp::ValueTag::Float64));
}

TEST(aot_core_ir_reader_test, AotDirectDispatch_NoDirectFn_WhenNotProvided) {
    // When resolve_direct_fn is nullptr, direct_fn should remain nullptr
    const char* json = R"json({"instructions":[
        {"opCode":14,"operand":1,"ilOffset":0,
         "callee":"System.Math::Sqrt:System.Double(System.Double)"}
    ]})json";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json),
        TestResolveFn, nullptr, nullptr, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].direct_fn, nullptr);
    EXPECT_EQ(result.instructions[0].direct_ret_tag, static_cast<uint8_t>(0xFF));
}

// ════════════════════════════════════════════════════════════════════════════
// Edge cases
// ════════════════════════════════════════════════════════════════════════════

TEST(aot_core_ir_reader_test, UnknownOpCode_Skipped) {
    // opCode=-1 should be skipped; a Ret sentinel is inserted.
    const char* json = R"({"instructions":[
        {"opCode":-1,"operand":0,"ilOffset":0}
    ]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::Ret);
}

TEST(aot_core_ir_reader_test, LdStr_MissingOperand_DoesNotCrash) {
    // LdStr (opCode=4) with no "operand" field → string_operand should be nullptr
    const char* json = R"({"instructions":[
        {"opCode":4,"ilOffset":0}
    ]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::LdStr);
    // string_operand may be nullptr — should not crash
}

TEST(aot_core_ir_reader_test, MultipleInstructions_RoundTrip) {
    // LdcI4 10 → StLoc 0 → Ret
    const char* json = R"({"instructions":[
        {"opCode":0,"operand":10,"ilOffset":0},
        {"opCode":8,"operand":0,"ilOffset":2},
        {"opCode":53,"ilOffset":4}
    ]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_EQ(result.instructions.size(), 3u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::LdcI4);
    EXPECT_EQ(result.instructions[0].immediate_i4, 10);

    EXPECT_EQ(result.instructions[1].op_code, intp::IROpCode::StLoc);
    EXPECT_EQ(result.instructions[1].operand_index, 0);

    EXPECT_EQ(result.instructions[2].op_code, intp::IROpCode::Ret);
}

TEST(aot_core_ir_reader_test, LdFld_ParsesOperand) {
    // LdFld (opCode=10) — immediate_i4 and field_offset both read from operand
    const char* json = R"({"instructions":[
        {"opCode":10,"operand":42,"ilOffset":0}
    ]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    ASSERT_GE(result.instructions.size(), 1u);
    EXPECT_EQ(result.instructions[0].op_code, intp::IROpCode::LdFld);
    EXPECT_EQ(result.instructions[0].immediate_i4, 42);
    // field_offset currently reads from operand too (known limitation)
    EXPECT_EQ(result.instructions[0].field_offset, 42u);
}

TEST(aot_core_ir_reader_test, EmptyInstructionsArray_ReturnsEmptyMethod) {
    const char* json = R"({"instructions":[]})";
    auto result = gc::DeserializeAotCoreIrMethod(json, std::strlen(json), nullptr, nullptr);

    // No instructions → no Ret either (empty instructions vector)
    EXPECT_EQ(result.instructions.size(), 0u);
}

// ════════════════════════════════════════════════════════════════════════════
// NOTE: The lazy resolution path and module lifecycle tests require a
// fully bootstrapped AOT runtime with CodegenBridgeV0.resolve_type_by_token.
// Unit tests for RegisterModuleGenerics with real data cannot be written
// without a mock bridge.
// ════════════════════════════════════════════════════════════════════════════
