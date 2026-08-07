/// interpreter_profile_test.cpp — Dispatch profile test for InterpreterEntryDirect (gtest)
///
/// Exercises: 2-instr fast path, RegisterExecute, and FastExecute dispatch tiers.
/// Each scenario exercises a different dispatch tier and verifies correct execution.

#include "interpreter_entry.h"
#include "patch_loader.h"
#include "thread_state.h"

#include <chaos/profile.h>

#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>
#include <cstdint>

using chaos::il2cpp::runtime_core::PatchMethod;
using chaos::il2cpp::runtime_core::PatchMethodLowerIR;
using chaos::il2cpp::runtime_core::ArgBuffer;
using chaos::il2cpp::runtime_core::InterpreterEntryDirect;
using chaos::il2cpp::runtime_core::threading::RegisterThread;
using chaos::il2cpp::runtime_core::threading::UnregisterThread;

constexpr uint8_t kElemI4 = 0x08;

// AOT symbol stubs
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// Helper: build ECMA signature blob
static int BuildSignature(uint8_t* blob_out, int param_count,
                          uint8_t ret_type, const uint8_t* param_types) {
    int len = 3 + param_count;
    int pos = 0;
    blob_out[pos++] = static_cast<uint8_t>(len - 1);
    blob_out[pos++] = 0x00;
    blob_out[pos++] = static_cast<uint8_t>(param_count);
    blob_out[pos++] = ret_type;
    for (int i = 0; i < param_count; ++i)
        blob_out[pos++] = param_types[i];
    return pos;
}

// ════════════════════════════════════════════════════════════════════════════
// Test: 2-instr fast path (ldc.i4 42 + ret)
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Profile, TwoInstrFastPath) {
    RegisterThread(chaos::il2cpp::runtime_core::threading::kMainThreadId, nullptr);

    const char* json =
        R"({"instructions":[)"
        R"({"opCode":0,"ilOffset":0,"operand":42},)"
        R"({"opCode":53,"ilOffset":1}]})";

    uint8_t sig_buf[16];
    int sig_len = BuildSignature(sig_buf, 0, kElemI4, nullptr);

    PatchMethod pm;
    pm.aot_core_ir_json = json;
    pm.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(json));
    pm.signature_blob = sig_buf;
    pm.signature_len = static_cast<uint32_t>(sig_len);
    pm.cached_ir = nullptr;
    pm.cached_reg_method = nullptr;
    pm.metadata_cache = nullptr;

    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));
    ASSERT_NE(pm.cached_ir, nullptr);

    int32_t ret_val = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), nullptr, &ret_val);

    EXPECT_EQ(ret_val, 42);

    UnregisterThread();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: RegisterExecute (ldarg.0 + ldarg.1 + add + ret)
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Profile, RegisterExecute) {
    RegisterThread(chaos::il2cpp::runtime_core::threading::kMainThreadId, nullptr);

    const char* json =
        R"({"instructions":[)"
        R"({"opCode":6,"ilOffset":0,"operand":0},)"
        R"({"opCode":6,"ilOffset":1,"operand":1},)"
        R"({"opCode":25,"ilOffset":2},)"
        R"({"opCode":53,"ilOffset":3}]})";

    uint8_t sig_buf[16];
    uint8_t param_types[] = { kElemI4, kElemI4 };
    int sig_len = BuildSignature(sig_buf, 2, kElemI4, param_types);

    PatchMethod pm;
    pm.aot_core_ir_json = json;
    pm.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(json));
    pm.signature_blob = sig_buf;
    pm.signature_len = static_cast<uint32_t>(sig_len);
    pm.cached_ir = nullptr;
    pm.cached_reg_method = nullptr;
    pm.metadata_cache = nullptr;

    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));
    ASSERT_NE(pm.cached_ir, nullptr);
    ASSERT_NE(pm.cached_reg_method, nullptr) << "cached_reg_method null after lowering";

    uint64_t args_buf[2] = { 10, 32 };
    int32_t ret_val = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);

    EXPECT_EQ(ret_val, 42);

    UnregisterThread();
}

// ════════════════════════════════════════════════════════════════════════════
// Test: FastExecute (same as RegisterExecute but cached_reg_method cleared)
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Profile, FastExecute) {
    RegisterThread(chaos::il2cpp::runtime_core::threading::kMainThreadId, nullptr);

    const char* json =
        R"({"instructions":[)"
        R"({"opCode":6,"ilOffset":0,"operand":0},)"
        R"({"opCode":6,"ilOffset":1,"operand":1},)"
        R"({"opCode":25,"ilOffset":2},)"
        R"({"opCode":53,"ilOffset":3}]})";

    uint8_t sig_buf[16];
    uint8_t param_types[] = { kElemI4, kElemI4 };
    int sig_len = BuildSignature(sig_buf, 2, kElemI4, param_types);

    PatchMethod pm;
    pm.aot_core_ir_json = json;
    pm.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(json));
    pm.signature_blob = sig_buf;
    pm.signature_len = static_cast<uint32_t>(sig_len);
    pm.cached_ir = nullptr;
    pm.cached_reg_method = nullptr;
    pm.metadata_cache = nullptr;

    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));
    ASSERT_NE(pm.cached_ir, nullptr);

    // Null out cached_reg_method to force FastExecute path
    pm.cached_reg_method = nullptr;

    uint64_t args_buf[2] = { 100, 200 };
    int32_t ret_val = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);

    EXPECT_EQ(ret_val, 300);

    UnregisterThread();
}
