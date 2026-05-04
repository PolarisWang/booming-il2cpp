/// Integration tests for the full IL->IR->execution pipeline.
///
/// Each test constructs a raw ECMA 335 method body (tiny or fat header + IL
/// bytecodes), parses the header with ParseMethodBodyHeader, lowers the IL
/// with LowerILToIR, executes the resulting IRMethod via InterpreterVM, and
/// verifies the execution result.
///
/// These tests exercise the entire lowering + interpretation path end-to-end,
/// unlike interpreter_smoke.cpp which builds IRMethod manually.

#include "il_to_ir_lowerer.h"
#include "interpreter_vm.h"
#include "vtable_registry.h"
#include "token_resolver.h"

#include <iostream>
#include <cstring>
#include <cstdint>

// ── Namespace aliases ───────────────────────────────────────────────────
using chaos::il2cpp::interpreter::ExecutionFrame;
using chaos::il2cpp::interpreter::ExecutionResult;
using chaos::il2cpp::interpreter::IRInstruction;
using chaos::il2cpp::interpreter::IRMethod;
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::InterpreterObject;
using chaos::il2cpp::interpreter::InterpreterValue;
using chaos::il2cpp::interpreter::InterpreterVM;
using chaos::il2cpp::interpreter::ILTokenResolver;
using chaos::il2cpp::interpreter::LowerILToIR;
using chaos::il2cpp::interpreter::MethodBodyHeader;
using chaos::il2cpp::interpreter::ParseMethodBodyHeader;
using chaos::il2cpp::interpreter::SEHClause;
using chaos::il2cpp::interpreter::SEHFlags;
using chaos::il2cpp::interpreter::DefaultTokenResolver;
using chaos::il2cpp::interpreter::TokenResolverContext;
using chaos::il2cpp::interpreter::ValueTag;

// ════════════════════════════════════════════════════════════════════════════
// Helpers (file-scope linkage)
// ════════════════════════════════════════════════════════════════════════════

/// No-op token resolver for tests that don't use metadata tokens.
static bool NoopTokenResolver(CHAOS_IL2CPP_UINT32 /*token*/,
                              IRInstruction& /*instruction*/,
                              void* /*user_data*/)
{
    return true;
}

/// Encode a tiny method header (1 byte) into the buffer.
static uint8_t EncodeTinyHeader(uint8_t* buf, uint8_t code_size)
{
    buf[0] = static_cast<uint8_t>((code_size << 2) | 0x02u);
    return 1u;
}

/// Token resolver that stores the raw token as immediate_i4 for SizeOf/LdFtn/LdToken.
static bool PassthroughTokenResolver(CHAOS_IL2CPP_UINT32 token,
                                     IRInstruction& instruction,
                                     void* /*user_data*/)
{
    instruction.immediate_i4 = static_cast<CHAOS_IL2CPP_INT32>(token);
    return true;
}

/// Encode a fat method header (12 bytes) into the buffer.
static uint8_t EncodeFatHeader(uint8_t* buf, uint16_t max_stack,
                                uint32_t code_size, uint32_t local_sig_tok)
{
    buf[0] = 0x03u; buf[1] = 0x00u;
    buf[2] = static_cast<uint8_t>(max_stack & 0xFFu);
    buf[3] = static_cast<uint8_t>((max_stack >> 8) & 0xFFu);
    buf[4] = static_cast<uint8_t>(code_size & 0xFFu);
    buf[5] = static_cast<uint8_t>((code_size >> 8) & 0xFFu);
    buf[6] = static_cast<uint8_t>((code_size >> 16) & 0xFFu);
    buf[7] = static_cast<uint8_t>((code_size >> 24) & 0xFFu);
    buf[8]  = static_cast<uint8_t>(local_sig_tok & 0xFFu);
    buf[9]  = static_cast<uint8_t>((local_sig_tok >> 8) & 0xFFu);
    buf[10] = static_cast<uint8_t>((local_sig_tok >> 16) & 0xFFu);
    buf[11] = static_cast<uint8_t>((local_sig_tok >> 24) & 0xFFu);
    return 12u;
}

/// Parse a tiny-header method body, lower it, and execute.
static ExecutionResult LowerAndExecuteTiny(
    const uint8_t* il_code, CHAOS_IL2CPP_SIZE code_size,
    ExecutionFrame* frame)
{
    uint8_t method_body[1 + 64];
    uint8_t hdr_size = EncodeTinyHeader(method_body, static_cast<uint8_t>(code_size));
    std::memcpy(method_body + hdr_size, il_code, code_size);

    MethodBodyHeader header;
    if (!ParseMethodBodyHeader(method_body, hdr_size + code_size, header)) {
        ExecutionResult err;
        err.has_return_value = false;
        return err;
    }

    IRMethod method = LowerILToIR(
        header.code_start, header.code_size,
        header.code_size, header.max_stack,
        NoopTokenResolver, nullptr);

    const InterpreterVM vm = {};
    return vm.Execute(method, frame);
}

/// Lower and execute with a fat header and a custom token resolver.
static ExecutionResult LowerAndExecuteFat(
    const uint8_t* il_code, CHAOS_IL2CPP_SIZE code_size,
    uint16_t max_stack,
    ILTokenResolver token_resolver,
    ExecutionFrame* frame)
{
    uint8_t method_body[12 + 128];
    if (12u + code_size > sizeof(method_body)) {
        ExecutionResult err;
        err.has_return_value = false;
        return err;
    }
    EncodeFatHeader(method_body, max_stack, static_cast<uint32_t>(code_size), 0u);
    std::memcpy(method_body + 12u, il_code, code_size);

    MethodBodyHeader header;
    if (!ParseMethodBodyHeader(method_body, 12u + code_size, header)) {
        ExecutionResult err;
        err.has_return_value = false;
        return err;
    }

    IRMethod method = LowerILToIR(
        header.code_start, header.code_size,
        header.code_size, header.max_stack,
        token_resolver, nullptr);

    const InterpreterVM vm = {};
    return vm.Execute(method, frame);
}

// ════════════════════════════════════════════════════════════════════════════
// Test declarations
// ════════════════════════════════════════════════════════════════════════════

static bool TestLdcI4_0();
static bool TestLdcI4_S();
static bool TestLdcI4_2_3_Add();
static bool TestLdArgAdd();
static bool TestStLocLdLoc();
static bool TestBrTrue_Taken();
static bool TestBrTrue_NotTaken();
static bool TestBeq_Equal();
static bool TestBeq_NotEqual();
static bool TestSub();
static bool TestMul();
static bool TestLdNull();
static bool TestFatHeaderMaxStack();
static bool TestEmptyInput();
static bool TestInvalidOpcode();
static bool TestTinyHeaderRejectedForEmptyCode();

// ── Phase A new opcode tests ───────────────────────────────────────────────
static bool TestDup();
static bool TestDivUn();
static bool TestRemUn();
static bool TestBitwiseAnd();
static bool TestBitwiseOr();
static bool TestBitwiseXor();
static bool TestBitwiseNot();
static bool TestShiftLeft();
static bool TestShiftRightArith();
static bool TestShiftRightLogical();
static bool TestConvRUn();
static bool TestConvI();
static bool TestConvU();
static bool TestSizeOf();

static bool TestExtendedLdFtn();
static bool TestExtendedLdArg();
static bool TestExtendedLdLoc();

// ── CallVirt / VTable dispatch tests ─────────────────────────────────────
static bool TestCallVirtDirectResolution();
static bool TestCallVirtInheritanceChain();

// -- SEH exception handling tests ------------------------------------------
static bool TestThrowUnhandled();
static bool TestThrowCatch();
static bool TestThrowFinallyUnwind();
static bool TestLeaveFinally();
static bool TestRethrow();

// Runtime-generic-method simulation tests
static bool TestRuntimeMethodExecute();
static bool TestRuntimeMethodReturnValueDispatch();
static bool TestRuntimeMethodTypeParamResolution();
static bool TestRuntimeMethodExceptionPropagation();

// ════════════════════════════════════════════════════════════════════════════
// Test runner
// ════════════════════════════════════════════════════════════════════════════

static int failures = 0;

#define TEST(name)                                                      \
    do {                                                                \
        if (!(name())) {                                                \
            std::cerr << "FAIL: " << #name << std::endl;                \
            ++failures;                                                 \
        }                                                               \
    } while (false)

int main()
{
    TEST(TestLdcI4_0);
    TEST(TestLdcI4_S);
    TEST(TestLdcI4_2_3_Add);
    TEST(TestLdArgAdd);
    TEST(TestStLocLdLoc);
    TEST(TestBrTrue_Taken);
    TEST(TestBrTrue_NotTaken);
    TEST(TestBeq_Equal);
    TEST(TestBeq_NotEqual);
    TEST(TestSub);
    TEST(TestMul);
    TEST(TestLdNull);
    TEST(TestFatHeaderMaxStack);
    TEST(TestEmptyInput);
    TEST(TestInvalidOpcode);
    TEST(TestTinyHeaderRejectedForEmptyCode);

    // Phase A new opcode tests
    TEST(TestDup);
    TEST(TestDivUn);
    TEST(TestRemUn);
    TEST(TestBitwiseAnd);
    TEST(TestBitwiseOr);
    TEST(TestBitwiseXor);
    TEST(TestBitwiseNot);
    TEST(TestShiftLeft);
    TEST(TestShiftRightArith);
    TEST(TestShiftRightLogical);
    TEST(TestConvRUn);
    TEST(TestConvI);
    TEST(TestConvU);
    TEST(TestSizeOf);
    TEST(TestExtendedLdFtn);
    TEST(TestExtendedLdArg);
    TEST(TestExtendedLdLoc);

    // CallVirt / VTable dispatch tests
    TEST(TestCallVirtDirectResolution);
    TEST(TestCallVirtInheritanceChain);

    // SEH exception handling tests
    TEST(TestThrowUnhandled);
    TEST(TestThrowCatch);
    TEST(TestThrowFinallyUnwind);
    TEST(TestLeaveFinally);
    TEST(TestRethrow);

    // Runtime-generic-method simulation tests (bridge core logic)
    TEST(TestRuntimeMethodExecute);
    TEST(TestRuntimeMethodReturnValueDispatch);
    TEST(TestRuntimeMethodTypeParamResolution);
    TEST(TestRuntimeMethodExceptionPropagation);

    std::cout << "interpreter-integration=failures=" << failures << std::endl;

    if (failures > 0) {
        std::cerr << "SOME INTEGRATION TESTS FAILED" << std::endl;
        return 1;
    }
    std::cout << "all interpreter integration tests passed" << std::endl;
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// Test implementations
// ═══════════════════════════════════════════════════════════════════════════

bool TestLdcI4_0()
{
    // IL: ldc.i4.0 (0x16) + ret (0x2A) = 2 bytes
    const uint8_t il_code[] = { 0x16, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 0;
}

bool TestLdcI4_S()
{
    // IL: ldc.i4.s 42 (0x1F, 42) + ret (0x2A) = 3 bytes
    const uint8_t il_code[] = { 0x1F, 42, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestLdcI4_2_3_Add()
{
    // IL: ldc.i4.2 (0x18) + ldc.i4.3 (0x19) + add (0x58) + ret (0x2A)
    const uint8_t il_code[] = { 0x18, 0x19, 0x58, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 5;
}

bool TestLdArgAdd()
{
    // IL: ldarg.0 (0x02) + ldarg.1 (0x03) + add (0x58) + ret (0x2A)
    const uint8_t il_code[] = { 0x02, 0x03, 0x58, 0x2A };
    ExecutionFrame frame = {};
    frame.arguments.push_back(InterpreterValue::from_i32(10));
    frame.arguments.push_back(InterpreterValue::from_i32(32));
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestStLocLdLoc()
{
    // IL: ldc.i4.s 42 (0x1F, 42) + stloc.0 (0x0A) + ldloc.0 (0x06) + ret (0x2A)
    // = 5 bytes
    const uint8_t il_code[] = { 0x1F, 42, 0x0A, 0x06, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestSub()
{
    // IL: ldc.i4.s 10 (0x1F, 10) + ldc.i4.3 (0x19) + sub (0x59) + ret (0x2A)
    const uint8_t il_code[] = { 0x1F, 10, 0x19, 0x59, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 7;
}

bool TestMul()
{
    // IL: ldc.i4.6 (0x1C) + ldc.i4.7 (0x1D) + mul (0x5A) + ret (0x2A)
    const uint8_t il_code[] = { 0x1C, 0x1D, 0x5A, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestLdNull()
{
    // IL: ldnull (0x14) + pop (0x26) + ldc.i4.0 (0x16) + ret (0x2A)
    const uint8_t il_code[] = { 0x14, 0x26, 0x16, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 0;
}

bool TestBrTrue_Taken()
{
    // IL (8 bytes):
    //   0: ldc.i4.1 (0x17)         push 1 (truthy)
    //   1: brtrue.s +2 (0x2D, 2)   branch to offset 5 if top != 0
    //   3: ldc.i4.0 (0x16)         fallthrough: push 0
    //   4: ret (0x2A)
    //   5: ldc.i4.s 42 (0x1F, 42)  target: push 42
    //   7: ret (0x2A)
    const uint8_t il_code[] = { 0x17, 0x2D, 0x02, 0x16, 0x2A, 0x1F, 42, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestBrTrue_NotTaken()
{
    // IL (8 bytes):
    //   0: ldc.i4.0 (0x16)         push 0 (falsy)
    //   1: brtrue.s +2 (0x2D, 2)   NOT taken (top is 0)
    //   3: ldc.i4.1 (0x17)         falls through: push 1
    //   4: ret (0x2A)
    //   5: ldc.i4.s 42 (0x1F, 42)
    //   7: ret (0x2A)
    const uint8_t il_code[] = { 0x16, 0x2D, 0x02, 0x17, 0x2A, 0x1F, 42, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 1;
}

bool TestBeq_Equal()
{
    // IL (8 bytes):
    //   0: ldarg.0 (0x02)
    //   1: ldarg.1 (0x03)
    //   2: beq.s +2 (0x2E, 2)      branch to offset 6 if arg0 == arg1
    //   4: ldc.i4.0 (0x16)         not equal: push 0
    //   5: ret (0x2A)
    //   6: ldc.i4.1 (0x17)         equal: push 1
    //   7: ret (0x2A)
    const uint8_t il_code[] = { 0x02, 0x03, 0x2E, 0x02, 0x16, 0x2A, 0x17, 0x2A };
    ExecutionFrame frame = {};
    frame.arguments.push_back(InterpreterValue::from_i32(42));
    frame.arguments.push_back(InterpreterValue::from_i32(42));
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 1;
}

bool TestBeq_NotEqual()
{
    const uint8_t il_code[] = { 0x02, 0x03, 0x2E, 0x02, 0x16, 0x2A, 0x17, 0x2A };
    ExecutionFrame frame = {};
    frame.arguments.push_back(InterpreterValue::from_i32(10));
    frame.arguments.push_back(InterpreterValue::from_i32(99));
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 0;
}

bool TestFatHeaderMaxStack()
{
    // Same IL as TestLdcI4_S but with a fat header and explicit max_stack.
    const uint8_t il_code[] = { 0x1F, 42, 0x2A };
    constexpr uint32_t code_size = 3;
    constexpr uint16_t max_stack = 16;

    uint8_t method_body[12 + code_size];
    EncodeFatHeader(method_body, max_stack, code_size, 0u);
    std::memcpy(method_body + 12, il_code, code_size);

    MethodBodyHeader header;
    if (!ParseMethodBodyHeader(method_body, sizeof(method_body), header)) {
        return false;
    }
    if (header.max_stack != max_stack) return false;
    if (header.code_size != code_size) return false;
    if (header.local_var_sig_tok != 0u) return false;

    IRMethod method = LowerILToIR(
        header.code_start, header.code_size,
        header.code_size, header.max_stack,
        NoopTokenResolver, nullptr);

    const InterpreterVM vm = {};
    ExecutionFrame frame = {};
    const ExecutionResult result = vm.Execute(method, &frame);
    return result.has_return_value && result.int32_value == 42;
}

bool TestEmptyInput()
{
    // LowerILToIR with null bytes returns an empty method (single Ret).
    IRMethod method = LowerILToIR(
        nullptr, 0u, 0u, 8u, NoopTokenResolver, nullptr);
    if (method.instructions.size() != 1u) return false;
    if (method.instructions[0].op_code != IROpCode::Ret) return false;

    const InterpreterVM vm = {};
    ExecutionFrame frame = {};
    const ExecutionResult result = vm.Execute(method, &frame);
    return !result.has_return_value;
}

bool TestInvalidOpcode()
{
    // An unrecognised opcode (0xFF) should cause LowerILToIR to return an
    // empty method (single Ret, no return value).
    const uint8_t il_code[] = { 0xFF, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(
        il_code, sizeof(il_code), &frame);
    return !result.has_return_value;
}

bool TestTinyHeaderRejectedForEmptyCode()
{
    // A tiny header encoding a code_size of 0 should be rejected.
    uint8_t body[] = { 0x02u };
    MethodBodyHeader header;
    if (ParseMethodBodyHeader(body, sizeof(body), header)) {
        return false;
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// Phase A: New opcode tests
// ═══════════════════════════════════════════════════════════════════════════

bool TestDup()
{
    // IL: ldc.i4.5 (0x1B) + ldc.i4.3 (0x19) + dup (0x25) + add (0x58) + add (0x58) + ret (0x2A)
    // Stack: [5], [5,3], [5,3,3], [5,6], [11] -> ret 11
    const uint8_t il_code[] = { 0x1B, 0x19, 0x25, 0x58, 0x58, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 11;
}

bool TestDivUn()
{
    // IL: ldc.i4.s -1 (0x1F, 0xFF) + ldc.i4.2 (0x18) + div.un (0x60) + ret (0x2A)
    // Unsigned -1 / 2 = 0xFFFFFFFF / 2 = 2147483647
    const uint8_t il_code[] = { 0x1F, 0xFF, 0x18, 0x60, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 2147483647;
}

bool TestRemUn()
{
    // IL: ldc.i4.s -1 (0x1F, 0xFF) + ldc.i4.3 (0x19) + rem.un (0x61) + ret (0x2A)
    // Unsigned -1 % 3: 4294967295 % 3 = 0
    const uint8_t il_code[] = { 0x1F, 0xFF, 0x19, 0x61, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 0;
}

bool TestBitwiseAnd()
{
    // IL: ldc.i4.3 (0x19) + ldc.i4.6 (0x1A) + and (0x62) + ret (0x2A)
    // 3 & 6 = 2
    const uint8_t il_code[] = { 0x19, 0x1C, 0x62, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 2;
}

bool TestBitwiseOr()
{
    // IL: ldc.i4.3 (0x19) + ldc.i4.6 (0x1A) + or (0x63) + ret (0x2A)
    // 3 | 6 = 7
    const uint8_t il_code[] = { 0x19, 0x1A, 0x63, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 7;
}

bool TestBitwiseXor()
{
    // IL: ldc.i4.3 (0x19) + ldc.i4.6 (0x1A) + xor (0x64) + ret (0x2A)
    // 3 ^ 6 = 5
    const uint8_t il_code[] = { 0x19, 0x1C, 0x64, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 5;
}

bool TestBitwiseNot()
{
    // IL: ldc.i4.0 (0x16) + not (0x66) + ret (0x2A)
    // ~0 = -1
    const uint8_t il_code[] = { 0x16, 0x66, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == -1;
}

bool TestShiftLeft()
{
    // IL: ldc.i4.3 (0x19) + ldc.i4.2 (0x18) + shl (0x67) + ret (0x2A)
    // 3 << 2 = 12
    const uint8_t il_code[] = { 0x19, 0x18, 0x67, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 12;
}

bool TestShiftRightArith()
{
    // IL: ldc.i4.s -8 (0x1F, 0xF8) + ldc.i4.2 (0x18) + shr (0x68) + ret (0x2A)
    // -8 >> 2 = -2 (arithmetic shift, sign-extending)
    const uint8_t il_code[] = { 0x1F, 0xF8, 0x18, 0x68, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == -2;
}

bool TestShiftRightLogical()
{
    // IL: ldc.i4.s -8 (0x1F, 0xF8) + ldc.i4.2 (0x18) + shr.un (0x69) + ret (0x2A)
    // -8 >>> 2 = 0x3FFFFFFE = 1073741822 (logical shift, zero-filling)
    const uint8_t il_code[] = { 0x1F, 0xF8, 0x18, 0x69, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 1073741822;
}

bool TestConvRUn()
{
    // IL: ldc.i4.3 (0x19) + conv.r.un (0x6F) + conv.i4 (0x6C) + ret (0x2A)
    // unsigned 3 → 3.0 → 3
    const uint8_t il_code[] = { 0x19, 0x6F, 0x6C, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 3;
}

bool TestConvI()
{
    // IL: ldc.i4.5 (0x1B) + conv.i (0xB3) + ret (0x2A)
    const uint8_t il_code[] = { 0x1B, 0xB3, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 5;
}

bool TestConvU()
{
    // IL: ldc.i4.5 (0x1B) + conv.u (0xB6) + ret (0x2A)
    const uint8_t il_code[] = { 0x1B, 0xB6, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 5;
}

bool TestSizeOf()
{
    // sizeof requires a token; use fat header + PassthroughTokenResolver.
    // IL: sizeof(4-byte-token) (FE 0x1C, tok[3..0]) + ret (0x2A)
    // Token 0x12345 → stored as immediate_i4 = 0x12345
    const uint8_t il_code[] = { 0xFE, 0x1C, 0x45, 0x23, 0x01, 0x00, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteFat(
        il_code, sizeof(il_code), 8u, PassthroughTokenResolver, &frame);
    return result.has_return_value && result.int32_value == static_cast<CHAOS_IL2CPP_INT32>(0x12345u);
}

bool TestExtendedLdFtn()
{
    // ldftn(4-byte-token) (FE 0x06, tok) + pop (0x26) + ldc.i4.1 (0x17) + ret (0x2A)
    const uint8_t il_code[] = { 0xFE, 0x06, 0xAA, 0xBB, 0xCC, 0xDD, 0x26, 0x17, 0x2A };
    ExecutionFrame frame = {};
    // ldftn + pop leaves nothing; then ldc.i4.1 + ret = returns 1
    // No crash from ldftn is the basic check.
    ExecutionResult result = LowerAndExecuteFat(
        il_code, sizeof(il_code), 8u, PassthroughTokenResolver, &frame);
    return result.has_return_value && result.int32_value == 1;
}

bool TestExtendedLdArg()
{
    // FE 0x09: ldarg (uint16) + ret
    // IL: FE 0x09, 0x00, 0x00 (= ldarg 0), ret (0x2A)
    const uint8_t il_code[] = { 0xFE, 0x09, 0x00, 0x00, 0x2A };
    ExecutionFrame frame = {};
    frame.arguments.push_back(InterpreterValue::from_i32(99));
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 99;
}

bool TestExtendedLdLoc()
{
    // FE 0x0C: ldloc (uint16) + ret
    // IL: ldc.i4.s 42 (0x1F, 42) + stloc.0 (0x0A) + FE 0x0C, 0x00, 0x00 (= ldloc 0) + ret (0x2A)
    const uint8_t il_code[] = { 0x1F, 42, 0x0A, 0xFE, 0x0C, 0x00, 0x00, 0x2A };
    ExecutionFrame frame = {};
    ExecutionResult result = LowerAndExecuteTiny(il_code, sizeof(il_code), &frame);
    return result.has_return_value && result.int32_value == 42;
}

// ═══════════════════════════════════════════════════════════════════════════
// CallVirt / VTable dispatch tests
// ═══════════════════════════════════════════════════════════════════════════

bool TestCallVirtDirectResolution()
{
    using namespace chaos::il2cpp::vtable_registry;

    // Register base class vtable: type_token=0x100, method_token=0x200 → 0xBEEF
    VTableSlot base_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) }
    };
    TypeVTable base_vtable = {};
    base_vtable.type_token = 0x100u;
    base_vtable.base_token = 0u;
    base_vtable.slot_count = 1u;
    base_vtable.slots = base_slots;
    RegisterTypeVTable(&base_vtable);

    // Build IRMethod manually: ldarg.0 (this) → callvirt method_token=0x200
    IRMethod method;

    IRInstruction ldarg;
    ldarg.op_code = IROpCode::LdArg;
    ldarg.operand_index = 0;
    method.instructions.push_back(ldarg);

    IRInstruction callvirt;
    callvirt.op_code = IROpCode::CallVirt;
    callvirt.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0x200u);  // declared method token
    callvirt.arg_count = 1u;  // just 'this'
    method.instructions.push_back(callvirt);

    // Create an object whose type token matches the registered vtable.
    auto* storage = new InterpreterObject();
    storage->type_token = 0x100u;
    storage->fields.resize(1u);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(storage));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Must dispatch externally with the resolved vtable pointer.
    return result.needs_external_dispatch &&
           result.call_target == reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu));
}

bool TestCallVirtInheritanceChain()
{
    using namespace chaos::il2cpp::vtable_registry;

    // Register base vtable: type_token=0x100, method_token=0x200 → 0xBEEF
    VTableSlot base_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xBEEFu)) }
    };
    TypeVTable base_vtable = {};
    base_vtable.type_token = 0x100u;
    base_vtable.base_token = 0u;
    base_vtable.slot_count = 1u;
    base_vtable.slots = base_slots;
    RegisterTypeVTable(&base_vtable);

    // Register derived vtable: type_token=0x101, base_token=0x100, method_token=0x200 → 0xCAFE
    VTableSlot derived_slots[] = {
        { 0x200u, reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu)) }
    };
    TypeVTable derived_vtable = {};
    derived_vtable.type_token = 0x101u;
    derived_vtable.base_token = 0x100u;
    derived_vtable.slot_count = 1u;
    derived_vtable.slots = derived_slots;
    RegisterTypeVTable(&derived_vtable);

    // Build IRMethod: ldarg.0 → callvirt method_token=0x200
    IRMethod method;

    IRInstruction ldarg;
    ldarg.op_code = IROpCode::LdArg;
    ldarg.operand_index = 0;
    method.instructions.push_back(ldarg);

    IRInstruction callvirt;
    callvirt.op_code = IROpCode::CallVirt;
    callvirt.secondary_index = static_cast<CHAOS_IL2CPP_SIZE>(0x200u);
    callvirt.arg_count = 1u;
    method.instructions.push_back(callvirt);

    // Create a derived-type object (type_token = 0x101).
    auto* storage = new InterpreterObject();
    storage->type_token = 0x101u;
    storage->fields.resize(1u);

    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_obj(storage));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Must resolve to the DERIVED class override (0xCAFE), not base (0xBEEF).
    return result.needs_external_dispatch &&
           result.call_target == reinterpret_cast<void*>(static_cast<CHAOS_IL2CPP_UINTPTR>(0xCAFEu));
}

// ═══════════════════════════════════════════════════════════════════════════
// SEH test implementations
// ═══════════════════════════════════════════════════════════════════════════

// --- TestThrowUnhandled ----------------------------------------------------
// Throw with no SEH clause → threw_exception should be true.
bool TestThrowUnhandled()
{
    IRMethod method;

    IRInstruction push;
    push.op_code = IROpCode::LdcI4;
    push.immediate_i4 = 1;
    method.instructions.push_back(push);

    IRInstruction throw_insn;
    throw_insn.op_code = IROpCode::Throw;
    method.instructions.push_back(throw_insn);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return result.threw_exception;
}

// --- TestThrowCatch --------------------------------------------------------
// Throw inside a try block with a catch handler → caught, handler returns 1.
bool TestThrowCatch()
{
    IRMethod method;

    // 0: push 42 (dummy exception object)
    IRInstruction push;
    push.op_code = IROpCode::LdcI4;
    push.immediate_i4 = 42;
    method.instructions.push_back(push);

    // 1: throw
    IRInstruction throw_insn;
    throw_insn.op_code = IROpCode::Throw;
    method.instructions.push_back(throw_insn);

    // 2: (skipped) push 0
    IRInstruction skipped_push;
    skipped_push.op_code = IROpCode::LdcI4;
    skipped_push.immediate_i4 = 0;
    method.instructions.push_back(skipped_push);

    // 3: (skipped) ret
    IRInstruction skipped_ret;
    skipped_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(skipped_ret);

    // 4: (handler) push 1
    IRInstruction handler_push;
    handler_push.op_code = IROpCode::LdcI4;
    handler_push.immediate_i4 = 1;
    method.instructions.push_back(handler_push);

    // 5: (handler) ret
    IRInstruction handler_ret;
    handler_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(handler_ret);

    // SEH: catch clause covering instructions 0-1
    SEHClause catch_clause = {};
    catch_clause.flags = SEHFlags::Exception;
    catch_clause.try_start_idx = 0;
    catch_clause.try_end_idx = 2;
    catch_clause.handler_start_idx = 4;
    catch_clause.handler_end_idx = 6;
    method.seh_clauses.push_back(catch_clause);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return !result.threw_exception && result.has_return_value && result.int32_value == 1;
}

// --- TestThrowFinallyUnwind ------------------------------------------------
// Throw inside a try that has both a finally AND an outer-matching catch.
// The finally must execute (unwind) before the catch receives control.
bool TestThrowFinallyUnwind()
{
    IRMethod method;

    // 0: push 42 (dummy exception object)
    IRInstruction push;
    push.op_code = IROpCode::LdcI4;
    push.immediate_i4 = 42;
    method.instructions.push_back(push);

    // 1: throw
    IRInstruction throw_insn;
    throw_insn.op_code = IROpCode::Throw;
    method.instructions.push_back(throw_insn);

    // 2: (skipped) push 0
    IRInstruction skipped;
    skipped.op_code = IROpCode::LdcI4;
    skipped.immediate_i4 = 0;
    method.instructions.push_back(skipped);

    // 3: (skipped) ret
    IRInstruction skipped_ret;
    skipped_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(skipped_ret);

    // 4: (finally handler) EndFinally — unwind continues to catch
    IRInstruction endfinally;
    endfinally.op_code = IROpCode::EndFinally;
    method.instructions.push_back(endfinally);

    // 5: (catch handler) push 1
    IRInstruction catch_push;
    catch_push.op_code = IROpCode::LdcI4;
    catch_push.immediate_i4 = 1;
    method.instructions.push_back(catch_push);

    // 6: (catch handler) ret
    IRInstruction catch_ret;
    catch_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(catch_ret);

    // Clause 0: finally (innermost → runs first during unwind)
    SEHClause finally_clause = {};
    finally_clause.flags = SEHFlags::Finally;
    finally_clause.try_start_idx = 0;
    finally_clause.try_end_idx = 2;
    finally_clause.handler_start_idx = 4;
    finally_clause.handler_end_idx = 5;
    method.seh_clauses.push_back(finally_clause);

    // Clause 1: catch (outermost → catches after finally unwind)
    SEHClause catch_clause = {};
    catch_clause.flags = SEHFlags::Exception;
    catch_clause.try_start_idx = 0;
    catch_clause.try_end_idx = 2;
    catch_clause.handler_start_idx = 5;
    catch_clause.handler_end_idx = 7;
    method.seh_clauses.push_back(catch_clause);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return !result.threw_exception && result.has_return_value && result.int32_value == 1;
}

// --- TestLeaveFinally ------------------------------------------------------
// Leave inside a try block that has a finally → finally runs, then resumes at target.
bool TestLeaveFinally()
{
    IRMethod method;

    // 0: Leave(branch_target=3) → finds finally → saves target, jumps to handler at 5
    IRInstruction leave;
    leave.op_code = IROpCode::Leave;
    leave.branch_target = 3;
    method.instructions.push_back(leave);

    // 1: (skipped) push 0
    IRInstruction skipped;
    skipped.op_code = IROpCode::LdcI4;
    skipped.immediate_i4 = 0;
    method.instructions.push_back(skipped);

    // 2: (skipped) ret
    IRInstruction skipped_ret;
    skipped_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(skipped_ret);

    // 3: (leave target) push 1
    IRInstruction target_push;
    target_push.op_code = IROpCode::LdcI4;
    target_push.immediate_i4 = 1;
    method.instructions.push_back(target_push);

    // 4: ret
    IRInstruction target_ret;
    target_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(target_ret);

    // 5: (finally handler) EndFinally → pending_leave → resumes at 3
    IRInstruction endfinally;
    endfinally.op_code = IROpCode::EndFinally;
    method.instructions.push_back(endfinally);

    // Finally clause covering instruction 0
    SEHClause finally_clause = {};
    finally_clause.flags = SEHFlags::Finally;
    finally_clause.try_start_idx = 0;
    finally_clause.try_end_idx = 1;
    finally_clause.handler_start_idx = 5;
    finally_clause.handler_end_idx = 6;
    method.seh_clauses.push_back(finally_clause);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return !result.threw_exception && result.has_return_value && result.int32_value == 1;
}

// ── Runtime-generic-method simulation tests ──────────────────────────────
// These tests simulate what the RuntimeInstantiationBridgeV0::interpret_method_call
// does: lower IL → build frame → execute → extract result.

// Test: Lower IL for a generic-like method and execute with args.
// Simulates what the bridge does: LowerILToIR → ExecutionFrame → Execute.
static bool TestRuntimeMethodExecute()
{
    // IL: add(ldarg.0, ldarg.1) → ret
    // Using tiny header: 0x02 | (code_size << 2)
    // code_size = 3 bytes: ldarg.0 (02), ldarg.1 (03), add (58), ret (2a) → actually 4 bytes
    // Wait, let me count: ldarg.0=0x02, ldarg.1=0x03, add=0x58, ret=0x2A → 4 bytes
    // tiny header = 0x02 | (4 << 2) = 0x12
    uint8_t il[] = {
        0x12,       // tiny header: code_size=4
        0x02,       // ldarg.0
        0x03,       // ldarg.1
        0x58,       // add
        0x2A        // ret
    };

    MethodBodyHeader header;
    if (!ParseMethodBodyHeader(il, sizeof(il), header)) {
        return false;
    }

    // Token resolver: none needed for this IL (no metadata tokens).
    IRMethod method = LowerILToIR(
        header.code_start, sizeof(il) - (header.code_start - il),
        header.code_size, header.max_stack,
        nullptr, nullptr);

    if (method.instructions.empty()) {
        return false;
    }

    // Build frame with two int32 arguments.
    ExecutionFrame frame;
    frame.arguments.push_back(InterpreterValue::from_i32(10));
    frame.arguments.push_back(InterpreterValue::from_i32(20));

    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    return result.has_return_value
        && result.return_value.tag == ValueTag::Int32
        && result.return_value.i32 == 30;
}

// Test: Return value extraction via InterpreterValue tag dispatch.
// The bridge uses tag dispatch to write return_value into raw out_return_value.
static bool TestRuntimeMethodReturnValueDispatch()
{
    // IL: ldc.i4.s 42 → ret
    uint8_t il[] = {
        0x0E,       // tiny header: code_size=3  (0x02 | (3<<2))
        0x1F,       // ldc.i4.s               (short form, 1 opcode + 1 operand)
        0x2A,       // operand = 42
        0x2A        // ret
    };

    MethodBodyHeader header;
    if (!ParseMethodBodyHeader(il, sizeof(il), header)) {
        return false;
    }

    IRMethod method = LowerILToIR(
        header.code_start, sizeof(il) - (header.code_start - il),
        header.code_size, header.max_stack,
        nullptr, nullptr);

    if (method.instructions.empty()) {
        return false;
    }

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    if (!result.has_return_value || result.return_value.tag != ValueTag::Int32) {
        return false;
    }

    // Simulate the bridge's tag dispatch.
    int32_t extracted = 0;
    switch (result.return_value.tag) {
        case ValueTag::Int32:
            extracted = result.return_value.i32;
            break;
        default:
            return false;
    }

    return extracted == 42;
}

// Test: Generic type parameter resolution via DefaultTokenResolver.
// Simulates what happens during lowering of a generic method body that
// references ELEMENT_TYPE_VAR.  The resolution is done using type_args
// from the TokenResolverContext.
static bool TestRuntimeMethodTypeParamResolution()
{
    // IL for a method that takes a type parameter and boxes it:
    // ldarg.0, box 0x11000000, ret
    // The token 0x11000000 represents ELEMENT_TYPE_VAR with index 0 (!0).
    uint8_t il[] = {
        0x0A,       // tiny header: code_size=2
        0x02,       // ldarg.0
        0x8C,       // box <token>
        0x00, 0x00, 0x00, 0x11,  // token = 0x11000000 (ELEMENT_TYPE_VAR, index 0)
        0x2A        // ret
    };

    MethodBodyHeader header;
    if (!ParseMethodBodyHeader(il, sizeof(il), header)) {
        return false;
    }

    // Set up TokenResolverContext with type_args.
    // Use a sentinel pointer as the "resolved type handle" for the type param.
    TypeInfoHandle dummy_type = static_cast<TypeInfoHandle>(0xDEADBEEFu);

    auto ctx = chaos::il2cpp::interpreter::TokenResolverContext();
    ctx.type_args = &dummy_type;
    ctx.arg_count = 1u;
    // No bridge needed — generic param resolution happens before bridge check.

    IRMethod method = LowerILToIR(
        header.code_start, sizeof(il) - (header.code_start - il),
        header.code_size, header.max_stack,
        chaos::il2cpp::interpreter::DefaultTokenResolver, &ctx);

    if (method.instructions.empty()) {
        return false;
    }

    // After lowering, the box instruction should have its call_target set
    // to dummy_type (resolved from ELEMENT_TYPE_VAR).
    for (const auto& insn : method.instructions) {
        if (insn.op_code == IROpCode::Box) {
            void* expected = reinterpret_cast<void*>(
                static_cast<CHAOS_IL2CPP_UINTPTR>(0xDEADBEEFu));
            return insn.call_target == expected;
        }
    }

    return false;  // Box instruction not found
}

// Test: Exception propagation matching bridge path.
// When the interpreter throws an unhandled exception, threw_exception is set
// and exception_value captures the exception object.
// The bridge checks threw_exception and throws ManagedExceptionCarrier.
static bool TestRuntimeMethodExceptionPropagation()
{
    IRMethod method;

    // 0: push 99 (exception object)
    IRInstruction push;
    push.op_code = IROpCode::LdcI4;
    push.immediate_i4 = 99;
    method.instructions.push_back(push);

    // 1: throw
    IRInstruction throw_insn;
    throw_insn.op_code = IROpCode::Throw;
    method.instructions.push_back(throw_insn);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Verify: threw_exception is true, and exception_value captures the object.
    if (!result.threw_exception) {
        return false;
    }

    // The exception_value should be Int32 with value 99.
    return result.exception_value.tag == ValueTag::Int32
        && result.exception_value.i32 == 99;
}

// Rethrow inside a catch handler → propagates out.
bool TestRethrow()
{
    IRMethod method;

    // 0: push 42 (dummy exception object)
    IRInstruction push;
    push.op_code = IROpCode::LdcI4;
    push.immediate_i4 = 42;
    method.instructions.push_back(push);

    // 1: throw → caught by catch handler at 4
    IRInstruction throw_insn;
    throw_insn.op_code = IROpCode::Throw;
    method.instructions.push_back(throw_insn);

    // 2: (skipped) push 0
    IRInstruction skipped;
    skipped.op_code = IROpCode::LdcI4;
    skipped.immediate_i4 = 0;
    method.instructions.push_back(skipped);

    // 3: (skipped) ret
    IRInstruction skipped_ret;
    skipped_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(skipped_ret);

    // 4: (catch handler) rethrow → exception_obj was pushed on stack → pops it
    IRInstruction rethrow;
    rethrow.op_code = IROpCode::Rethrow;
    method.instructions.push_back(rethrow);

    // 5: (unreachable) ret
    IRInstruction unreachable_ret;
    unreachable_ret.op_code = IROpCode::Ret;
    method.instructions.push_back(unreachable_ret);

    // Catch clause covering instructions 0-1
    SEHClause catch_clause = {};
    catch_clause.flags = SEHFlags::Exception;
    catch_clause.try_start_idx = 0;
    catch_clause.try_end_idx = 2;
    catch_clause.handler_start_idx = 4;
    catch_clause.handler_end_idx = 6;
    method.seh_clauses.push_back(catch_clause);

    ExecutionFrame frame;
    const InterpreterVM vm = {};
    ExecutionResult result = vm.Execute(method, &frame);

    // Rethrow throws again, no further catch → threw_exception = true
    return result.threw_exception;
}
