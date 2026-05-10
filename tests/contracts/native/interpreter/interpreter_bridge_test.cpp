/// Integration tests for the full InterpretMethodCall bridge path.
///
/// These tests exercise the RuntimeInstantiationBridgeV0::interpret_method_call
/// entry point end-to-end, including:
///   - AotCoreIr JSON deserialization inside LowerMethodBody
///   - V1 signature-aware argument marshalling
///   - Return value extraction via tag dispatch
///   - Exception propagation via ManagedExceptionCarrier
///
/// Unlike the interpreter_integration tests (which call InterpreterVM::Execute
/// directly), these tests go through the bridge function pointer, exercising
/// the exact code path used by MethodInvoke.

#include <chaos/native_types.h>              // CHAOS_IL2CPP_UINT32 etc — MUST be first

#include "interpreter_vm.h"                  // InterpreterValue, ValueTag
#include "runtime_core.h"                    // RuntimeStatus, ManagedExceptionCarrier
#include "reflection_query_model.h"          // descriptors, encode/decode
#include "generic_method_instantiation.h"    // RuntimeInstantiatedMethod
#include <runtime_instantiation.h>           // RuntimeInstantiationBridgeV0, ChaosRuntimeInstantiationGetBridgeV0

#include <iostream>
#include <cstring>
#include <cstdint>
#include <cstdarg>

// ── Namespace aliases ───────────────────────────────────────────────────
using chaos::il2cpp::runtime_core::EncodeReflectionQueryMethodHandle;
using chaos::il2cpp::runtime_core::ReflectionQueryMethodDescriptor;
using chaos::il2cpp::runtime_core::ReflectionQueryParameterDescriptor;
using chaos::il2cpp::runtime_core::ManagedExceptionCarrier;
using chaos::il2cpp::runtime_instantiation::RuntimeInstantiatedMethod;
using chaos::il2cpp::interpreter::InterpreterValue;
using chaos::il2cpp::interpreter::ValueTag;

// ── IROpCode numeric values for JSON construction ──────────────────────
// These match the generated ir_opcodes.h enum values.
constexpr int kOpLdcI4 = 0;
constexpr int kOpLdArg = 6;
constexpr int kOpAdd   = 25;
constexpr int kOpThrow = 48;
constexpr int kOpRet   = 53;

// ════════════════════════════════════════════════════════════════════════════
// Helpers
// ════════════════════════════════════════════════════════════════════════════

/// Build a tiny AotCoreIr JSON string for a simple method body.
/// Returns the number of bytes written (including null terminator).
/// \param buf        output buffer (must be at least 256 bytes)
/// \param json       constructed JSON with a fixed set of opcodes and operands
///
/// JSON format expected by DeserializeAotCoreIrMethod:
/// {
///   "instructions": [
///     {"opCode": N, "ilOffset": M, "operand": V},
///     ...
///   ],
///   "exceptionRegions": []
/// }
static uint32_t BuildJsonMethod(char* buf, size_t buf_size, const char* json_fmt, ...) {
    va_list args;
    va_start(args, json_fmt);
    int written = std::vsnprintf(buf, buf_size, json_fmt, args);
    va_end(args);
    if (written < 0 || static_cast<size_t>(written) >= buf_size) return 0;
    return static_cast<uint32_t>(written) + 1;  // include null terminator
}

/// Encode a method descriptor pointer as a valid MethodInfoHandle.
static auto EncodeMethod(ReflectionQueryMethodDescriptor* desc) {
    return EncodeReflectionQueryMethodHandle(desc);
}

/// Return the process-wide bridge instance via the C-linkage entry point.
static const RuntimeInstantiationBridgeV0* GetBridge() {
    return ChaosRuntimeInstantiationGetBridgeV0();
}

// ════════════════════════════════════════════════════════════════════════════
// Test declarations
// ════════════════════════════════════════════════════════════════════════════

static bool TestBridgeBasicAdd();
static bool TestBridgeSignatureAwareArg();
static bool TestBridgeExceptionPropagation();

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
    TEST(TestBridgeBasicAdd);
    TEST(TestBridgeSignatureAwareArg);
    TEST(TestBridgeExceptionPropagation);

    std::cout << "interpreter-bridge=failures=" << failures << std::endl;

    if (failures > 0) {
        std::cerr << "SOME BRIDGE TESTS FAILED" << std::endl;
        return 1;
    }
    std::cout << "all interpreter bridge tests passed" << std::endl;
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// Test implementations
// ═══════════════════════════════════════════════════════════════════════════

// Test: Basic method execution through InterpretMethodCall.
//   Method body: ldc.i4.1 + ldc.i4.2 + add + ret → returns 3
//   No parameter type info (V0 fallback).
//   Exercises: deserialization, execution, return value extraction.
static bool TestBridgeBasicAdd()
{
    // ── AotCoreIr JSON: ldc.i4.1 + ldc.i4.2 + add + ret ──
    char json_buf[256];
    uint32_t json_len = BuildJsonMethod(json_buf, sizeof(json_buf),
        R"({"instructions":[)"
        R"({"opCode":%d,"ilOffset":0,"operand":1},)"
        R"({"opCode":%d,"ilOffset":1,"operand":2},)"
        R"({"opCode":%d,"ilOffset":2},)"
        R"({"opCode":%d,"ilOffset":3}]})",
        kOpLdcI4, kOpLdcI4, kOpAdd, kOpRet);

    // ── Method descriptor (no parameter type info) ──
    const char* subject = "test_BridgeBasicAdd";
    ReflectionQueryMethodDescriptor desc = {};
    desc.subject_id_utf8 = subject;
    desc.name_utf8 = "test";
    desc.member_type_utf8 = "System.Int32";
    desc.parameter_count = 0;
    desc.parameter_descriptor_count = 0;
    desc.parameters = nullptr;

    // ── RuntimeInstantiatedMethod ──
    RuntimeInstantiatedMethod rt_method = {};
    rt_method.descriptor = desc;
    rt_method.aot_core_ir_json = json_buf;
    rt_method.aot_core_ir_json_length = json_len;
    rt_method.is_unloaded = false;

    const auto method_handle = EncodeMethod(&rt_method.descriptor);
    const auto* bridge = GetBridge();
    if (bridge == nullptr) return false;
    if (bridge->interpret_method_call == nullptr) return false;

    // ── Call bridge ──
    int32_t result_value = 0;
    ExceptionHandle ex = nullptr;
    const RuntimeStatus status = bridge->interpret_method_call(
        nullptr, nullptr,
        method_handle,
        nullptr,   // no this
        nullptr,   // no args
        0u,
        &result_value, sizeof(result_value),
        &ex);

    if (status != CHAOS_RUNTIME_STATUS_OK) return false;
    if (ex != nullptr) return false;

    return result_value == 3;
}

// Test: Signature-aware argument marshalling via bridge.
//   Method body: ldarg.0 + ret → returns the first argument unchanged.
//   Parameter descriptor: member_type_utf8 = "System.Int32"
//   Exercises: V1 marshalling path with explicit type info.
static bool TestBridgeSignatureAwareArg()
{
    // ── AotCoreIr JSON: ldarg.0 + ret ──
    char json_buf[256];
    uint32_t json_len = BuildJsonMethod(json_buf, sizeof(json_buf),
        R"({"instructions":[)"
        R"({"opCode":%d,"ilOffset":0,"operand":0},)"
        R"({"opCode":%d,"ilOffset":1}]})",
        kOpLdArg, kOpRet);

    // ── Parameter descriptor ──
    const char* param_type = "System.Int32";
    const char* param_name = "arg0";
    ReflectionQueryParameterDescriptor params[1] = {};
    params[0].parameter_index = 0;
    params[0].member_type_utf8 = param_type;
    params[0].name_utf8 = param_name;

    // ── Method descriptor ──
    const char* subject = "test_BridgeSignatureAwareArg";
    ReflectionQueryMethodDescriptor desc = {};
    desc.subject_id_utf8 = subject;
    desc.name_utf8 = "test";
    desc.member_type_utf8 = "System.Int32";
    desc.parameter_count = 1;
    desc.parameter_descriptor_count = 1;
    desc.parameters = params;

    // ── RuntimeInstantiatedMethod ──
    RuntimeInstantiatedMethod rt_method = {};
    rt_method.descriptor = desc;
    rt_method.aot_core_ir_json = json_buf;
    rt_method.aot_core_ir_json_length = json_len;
    rt_method.is_unloaded = false;

    const auto method_handle = EncodeMethod(&rt_method.descriptor);
    const auto* bridge = GetBridge();
    if (bridge == nullptr) return false;

    // ── Call bridge with arg = 42 ──
    int32_t result_value = 0;
    int32_t arg = 42;
    void* argv[] = { &arg };
    ExceptionHandle ex = nullptr;
    const RuntimeStatus status = bridge->interpret_method_call(
        nullptr, nullptr,
        method_handle,
        nullptr,   // no this
        argv, 1u,
        &result_value, sizeof(result_value),
        &ex);

    if (status != CHAOS_RUNTIME_STATUS_OK) return false;
    if (ex != nullptr) return false;

    return result_value == 42;
}

// Test: Exception propagation through InterpretMethodCall.
//   Method body: ldc.i4 99 + throw → triggers threw_exception.
//   The bridge converts threw_exception → ManagedExceptionCarrier.
//   We catch the C++ exception and verify it.
static bool TestBridgeExceptionPropagation()
{
    // ── AotCoreIr JSON: ldc.i4.99 + throw + ret (unreachable) ──
    char json_buf[256];
    uint32_t json_len = BuildJsonMethod(json_buf, sizeof(json_buf),
        R"({"instructions":[)"
        R"({"opCode":%d,"ilOffset":0,"operand":99},)"
        R"({"opCode":%d,"ilOffset":1},)"
        R"({"opCode":%d,"ilOffset":2}]})",
        kOpLdcI4, kOpThrow, kOpRet);

    // ── Method descriptor ──
    const char* subject = "test_BridgeExceptionPropagation";
    ReflectionQueryMethodDescriptor desc = {};
    desc.subject_id_utf8 = subject;
    desc.name_utf8 = "test";
    desc.member_type_utf8 = "System.Void";
    desc.parameter_count = 0;
    desc.parameter_descriptor_count = 0;
    desc.parameters = nullptr;

    // ── RuntimeInstantiatedMethod ──
    RuntimeInstantiatedMethod rt_method = {};
    rt_method.descriptor = desc;
    rt_method.aot_core_ir_json = json_buf;
    rt_method.aot_core_ir_json_length = json_len;
    rt_method.is_unloaded = false;

    const auto method_handle = EncodeMethod(&rt_method.descriptor);
    const auto* bridge = GetBridge();
    if (bridge == nullptr) return false;

    // ── Call bridge — expect ManagedExceptionCarrier ──
    RuntimeStatus status = CHAOS_RUNTIME_STATUS_OK;

    try {
        int32_t result_value = 0;
        ExceptionHandle ex = nullptr;
        status = bridge->interpret_method_call(
            nullptr, nullptr,
            method_handle,
            nullptr,   // no this
            nullptr,   // no args
            0u,
            &result_value, sizeof(result_value),
            &ex);
        // Should not reach here — the bridge should throw.
        (void)status;
        return false;
    } catch (const ManagedExceptionCarrier& carrier) {
        // Expected: the bridge caught threw_exception and threw.
        // carrier.exception is nullptr because the thrown value is Int32(99),
        // not an ObjectRef with a type_token.  The bridge's V1 exception
        // bridge skips object_new for non-ObjectRef values.
        //
        // This test validates that the ManagedExceptionCarrier throw path
        // is correctly triggered, not the quality of the exception handle.
        return carrier.exception == nullptr;
    } catch (...) {
        // Unexpected exception type.
        return false;
    }
}
