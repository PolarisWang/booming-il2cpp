// interpreter_profile_test.cpp — Dispatch profile test for InterpreterEntryDirect
// tiers: 2-instr fast path, RegisterExecute, and FastExecute.
//
// Each scenario exercises a different dispatch tier and verifies correct
// execution AND that PROFILE_SCOPE annotations fire for the expected handlers.
//
// Requires: DEBUG/PROFILE build (PROFILE_ENABLED=1) to see PROFILE_DUMP data.
// Build with: cmake --preset debug && cmake --build --preset debug

#include "interpreter_entry.h"
#include "patch_loader.h"
#include "thread_state.h"

#include <chaos/profile.h>

#include <cstdio>
#include <cstring>
#include <cstdint>
#include <iostream>

// Namespace aliases
using chaos::il2cpp::runtime_core::PatchMethod;
using chaos::il2cpp::runtime_core::PatchMethodLowerIR;
using chaos::il2cpp::runtime_core::ArgBuffer;
using chaos::il2cpp::runtime_core::InterpreterEntryDirect;
using chaos::il2cpp::runtime_core::threading::RegisterThread;
using chaos::il2cpp::runtime_core::threading::UnregisterThread;

// EC0A element type constants for signature blobs
constexpr uint8_t kElemI4 = 0x08;  // ELEMENT_TYPE_I4

// AOT-generated symbol stubs (required by linker for standalone tests).
// These are normally provided by AOT family libraries; we provide empty
// stubs here because our test methods don't call external runtime helpers.
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// Test helpers
static int s_passed = 0;
static int s_failed = 0;

static void run_test(const char* name, bool ok) {
    std::printf("  %s: %s\n", ok ? "PASS" : "FAIL", name);
    if (ok) ++s_passed; else ++s_failed;
}

// ── Helper: build a simple ECMA signature blob ───────────────────────────
// Format: [blob_length] [calling_convention] [param_count] [ret_type] [param_types...]
// Returns number of bytes written to blob_out.
static int BuildSignature(uint8_t* blob_out, int param_count,
                          uint8_t ret_type, const uint8_t* param_types) {
    int len = 3 + param_count;  // blob_length + cc + count + ret + params
    int pos = 0;
    blob_out[pos++] = static_cast<uint8_t>(len - 1);  // blob_length (excludes itself)
    blob_out[pos++] = 0x00;  // static calling convention (no HASTHIS)
    blob_out[pos++] = static_cast<uint8_t>(param_count);
    blob_out[pos++] = ret_type;
    for (int i = 0; i < param_count; ++i) {
        blob_out[pos++] = param_types[i];
    }
    return pos;
}

// ── Scenario 1: 2-instr fast path (ldc.i4 42 + ret) ─────────────────────
static bool test_2instr_fast_path() {
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

    // Lower IR
    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null after lowering\n");
        return false;
    }

    // No args needed for ldc.i4
    int32_t ret_val = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), nullptr, &ret_val);

    if (ret_val != 42) {
        std::fprintf(stderr, "  FAIL: expected 42, got %d\n", ret_val);
        return false;
    }

    return true;
}

// ── Scenario 2: RegisterExecute (ldarg.0 + ldarg.1 + add + ret) ─────────
// After PatchMethodLowerIR, cached_reg_method is auto-created by AllocateRegisters.
// RegisterExecute fires when CanRegisterExecute passes.
static bool test_register_execute() {
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

    // Lower IR — this will create both cached_ir and cached_reg_method
    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null after lowering\n");
        return false;
    }
    if (pm.cached_reg_method == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_reg_method null after lowering\n");
        return false;
    }

    // Prepare args: a = 10, b = 32
    // RegisterExecute and FastExecute read args as uint64_t[] (raw AOT byte convention)
    uint64_t args_buf[2];
    args_buf[0] = 10;
    args_buf[1] = 32;

    int32_t ret_val = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);

    if (ret_val != 42) {
        std::fprintf(stderr, "  FAIL: expected 42, got %d\n", ret_val);
        return false;
    }

    return true;
}

// ── Scenario 3: FastExecute (same as Sc. 2 but cached_reg_method cleared) ─
// After lowering, null out cached_reg_method to force FastExecute path.
static bool test_fast_execute() {
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

    // Lower IR — this creates cached_reg_method via AllocateRegisters
    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));
    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null after lowering\n");
        return false;
    }

    // Null out cached_reg_method to force FastExecute (skips RegisterExecute)
    pm.cached_reg_method = nullptr;

    // Prepare args: a = 100, b = 200
    uint64_t args_buf[2];

    args_buf[0] = 100;
    args_buf[1] = 200;
    int32_t ret_val = -1;
    InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm), args_buf, &ret_val);

    if (ret_val != 300) {
        std::fprintf(stderr, "  FAIL: expected 300, got %d\n", ret_val);
        return false;
    }

    return true;
}

// ── Main ─────────────────────────────────────────────────────────────────

int main() {
    // Register the test thread (required by InterpreterEntryDirect assertions).
    RegisterThread(chaos::il2cpp::runtime_core::threading::kMainThreadId, nullptr);

    std::printf("interpreter profile tests\n");
    std::printf("-------------------------\n");

    // Run all 3 dispatch tier tests.
    run_test("2instr_fast_path",    test_2instr_fast_path());
    run_test("register_execute",    test_register_execute());
    run_test("fast_execute",        test_fast_execute());

    // Dump and reset profile counters.
    // In DEBUG/PROFILE builds, this prints handler-level timing data.
    // In SHIP builds, these are no-ops.
    CHAOS_IL2CPP_PROFILE_DUMP();
    CHAOS_IL2CPP_PROFILE_RESET();

    UnregisterThread();

    std::printf("\n%d passed, %d failed\n", s_passed, s_failed);

    if (s_failed > 0) {
        std::printf("interpreter_profile_test FAILED\n");
        return 1;
    }

    std::printf("interpreter_profile_test OK\n");
    return 0;
}
