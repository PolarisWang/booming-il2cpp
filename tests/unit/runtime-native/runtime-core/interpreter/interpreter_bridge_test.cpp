/// interpreter_bridge_test.cpp — Integration tests for the full InterpretMethodCall bridge path (gtest)
///
/// These tests exercise the RuntimeInstantiationBridgeV0::interpret_method_call
/// entry point end-to-end, including:
///   - AotCoreIr JSON deserialization inside LowerMethodBody
///   - V1 signature-aware argument marshalling
///   - Return value extraction via tag dispatch
///   - Exception propagation via chaos_managed_exception

#include <chaos/native_types.h>

#include "interpreter_vm.h"
#include "runtime_core.h"
#include "reflection_query_model.h"
#include "generic_method_instantiation.h"
#include <runtime_instantiation.h>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>
#include <cstdarg>
#include <windows.h>

#include "exception_jmp.h"

// AOT symbol stubs (required by chaos_interpreter library linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

using chaos::il2cpp::runtime_core::EncodeReflectionQueryMethodHandle;
using chaos::il2cpp::runtime_core::ReflectionQueryMethodDescriptor;
using chaos::il2cpp::runtime_core::ReflectionQueryParameterDescriptor;
using chaos::il2cpp::runtime_instantiation::RuntimeInstantiatedMethod;
using chaos::il2cpp::interpreter::InterpreterValue;
using chaos::il2cpp::interpreter::ValueTag;

// IROpCode numeric values matching generated ir_opcodes.h
constexpr int kOpLdcI4 = 0;
constexpr int kOpLdArg = 6;
constexpr int kOpAdd   = 25;
constexpr int kOpThrow = 48;
constexpr int kOpRet   = 53;

// Helpers

static uint32_t BuildJsonMethod(char* buf, size_t buf_size, const char* json_fmt, ...) {
    va_list args;
    va_start(args, json_fmt);
    int written = std::vsnprintf(buf, buf_size, json_fmt, args);
    va_end(args);
    if (written < 0 || static_cast<size_t>(written) >= buf_size) return 0;
    return static_cast<uint32_t>(written) + 1;
}

static auto EncodeMethod(ReflectionQueryMethodDescriptor* desc) {
    return EncodeReflectionQueryMethodHandle(desc);
}

static const RuntimeInstantiationBridgeV0* GetBridge() {
    return ChaosRuntimeInstantiationGetBridgeV0();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Basic method execution through InterpretMethodCall
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Bridge, BasicAdd) {
    char json_buf[256];
    uint32_t json_len = BuildJsonMethod(json_buf, sizeof(json_buf),
        R"({"instructions":[)"
        R"({"opCode":%d,"ilOffset":0,"operand":1},)"
        R"({"opCode":%d,"ilOffset":1,"operand":2},)"
        R"({"opCode":%d,"ilOffset":2},)"
        R"({"opCode":%d,"ilOffset":3}]})",
        kOpLdcI4, kOpLdcI4, kOpAdd, kOpRet);

    const char* subject = "test_BridgeBasicAdd";
    ReflectionQueryMethodDescriptor desc = {};
    desc.subject_id_utf8 = subject;
    desc.name_utf8 = "test";
    desc.member_type_utf8 = "System.Int32";
    desc.parameter_count = 0;
    desc.parameter_descriptor_count = 0;
    desc.parameters = nullptr;

    RuntimeInstantiatedMethod rt_method = {};
    rt_method.descriptor = desc;
    rt_method.aot_core_ir_json = json_buf;
    rt_method.aot_core_ir_json_length = json_len;
    rt_method.is_unloaded = false;

    const auto method_handle = EncodeMethod(&rt_method.descriptor);
    const auto* bridge = GetBridge();
    ASSERT_NE(bridge, nullptr) << "bridge is null";
    ASSERT_NE(bridge->interpret_method_call, nullptr) << "interpret_method_call is null";

    int32_t result_value = 0;
    ExceptionHandle ex = nullptr;
    const RuntimeStatus status = bridge->interpret_method_call(
        nullptr, nullptr, method_handle,
        nullptr, nullptr, 0u,
        &result_value, sizeof(result_value), &ex);

    EXPECT_EQ(status, CHAOS_RUNTIME_STATUS_OK);
    EXPECT_EQ(ex, nullptr);
    EXPECT_EQ(result_value, 3);
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Signature-aware argument marshalling via bridge
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Bridge, SignatureAwareArg) {
    char json_buf[256];
    uint32_t json_len = BuildJsonMethod(json_buf, sizeof(json_buf),
        R"({"instructions":[)"
        R"({"opCode":%d,"ilOffset":0,"operand":0},)"
        R"({"opCode":%d,"ilOffset":1}]})",
        kOpLdArg, kOpRet);

    const char* param_type = "System.Int32";
    const char* param_name = "arg0";
    ReflectionQueryParameterDescriptor params[1] = {};
    params[0].parameter_index = 0;
    params[0].member_type_utf8 = param_type;
    params[0].name_utf8 = param_name;

    const char* subject = "test_BridgeSignatureAwareArg";
    ReflectionQueryMethodDescriptor desc = {};
    desc.subject_id_utf8 = subject;
    desc.name_utf8 = "test";
    desc.member_type_utf8 = "System.Int32";
    desc.parameter_count = 1;
    desc.parameter_descriptor_count = 1;
    desc.parameters = params;

    RuntimeInstantiatedMethod rt_method = {};
    rt_method.descriptor = desc;
    rt_method.aot_core_ir_json = json_buf;
    rt_method.aot_core_ir_json_length = json_len;
    rt_method.is_unloaded = false;

    const auto method_handle = EncodeMethod(&rt_method.descriptor);
    const auto* bridge = GetBridge();
    ASSERT_NE(bridge, nullptr);

    int32_t result_value = 0;
    int32_t arg = 42;
    void* argv[] = { &arg };
    ExceptionHandle ex = nullptr;
    const RuntimeStatus status = bridge->interpret_method_call(
        nullptr, nullptr, method_handle,
        nullptr, argv, 1u,
        &result_value, sizeof(result_value), &ex);

    EXPECT_EQ(status, CHAOS_RUNTIME_STATUS_OK);
    EXPECT_EQ(ex, nullptr);
    EXPECT_EQ(result_value, 42);
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Exception propagation through InterpretMethodCall
// ════════════════════════════════════════════════════════════════════════════

// SEH helper: extracted to avoid C2712 (cannot use __try in functions
// requiring C++ object unwinding — gtest's TEST() macro generates such code).
static bool TryInterpretMethodCallExpectSEH(const RuntimeInstantiationBridgeV0* bridge,
                                            MethodInfoHandle method_handle) {
    __try {
        int32_t result_value = 0;
        ExceptionHandle ex = nullptr;
        bridge->interpret_method_call(
            nullptr, nullptr, method_handle,
            nullptr, nullptr, 0u,
            &result_value, sizeof(result_value), &ex);
    } __except (GetExceptionCode() == 0xE0000001 ? EXCEPTION_EXECUTE_HANDLER
                                                  : EXCEPTION_CONTINUE_SEARCH) {
        return true;
    }
    return false;
}

TEST(Interpreter_Bridge, ExceptionPropagation) {
    char json_buf[256];
    uint32_t json_len = BuildJsonMethod(json_buf, sizeof(json_buf),
        R"({"instructions":[)"
        R"({"opCode":%d,"ilOffset":0,"operand":99},)"
        R"({"opCode":%d,"ilOffset":1},)"
        R"({"opCode":%d,"ilOffset":2}]})",
        kOpLdcI4, kOpThrow, kOpRet);

    const char* subject = "test_BridgeExceptionPropagation";
    ReflectionQueryMethodDescriptor desc = {};
    desc.subject_id_utf8 = subject;
    desc.name_utf8 = "test";
    desc.member_type_utf8 = "System.Void";
    desc.parameter_count = 0;
    desc.parameter_descriptor_count = 0;
    desc.parameters = nullptr;

    RuntimeInstantiatedMethod rt_method = {};
    rt_method.descriptor = desc;
    rt_method.aot_core_ir_json = json_buf;
    rt_method.aot_core_ir_json_length = json_len;
    rt_method.is_unloaded = false;

    const auto method_handle = EncodeMethod(&rt_method.descriptor);
    const auto* bridge = GetBridge();
    ASSERT_NE(bridge, nullptr);

    bool threw = TryInterpretMethodCallExpectSEH(bridge, method_handle);
    EXPECT_TRUE(threw) << "bridge should have thrown";
}
