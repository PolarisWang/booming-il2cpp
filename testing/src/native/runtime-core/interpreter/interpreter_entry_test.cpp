/// interpreter_entry_test.cpp — Unit tests for hotpatch interpreter entry components (gtest)
///
/// Tests:
///   1. PatchMethodLowerIR: lazy IR deserialization and caching
///   2. ArgBuffer read/write round-trip for all supported types
///   3. Null/missing JSON handling (empty IR with Ret)
///
/// These tests operate at the component level and don't require full runtime
/// bootstrap. For InterpreterEntryDirect end-to-end tests, see interpreter_profile_test.

#include "interpreter_entry.h"
#include "interpreter_vm.h"
#include "patch_loader.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>

// AOT symbol stubs (required by chaos_interpreter library linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

using chaos::il2cpp::interpreter::IRMethod;
using chaos::il2cpp::runtime_core::ArgBuffer;
using chaos::il2cpp::runtime_core::PatchMethod;
using chaos::il2cpp::runtime_core::PatchMethodLowerIR;

// ════════════════════════════════════════════════════════════════════════════
// Test: PatchMethodLowerIR deserializes a simple ldc.i4+ret JSON
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Entry, LowerLdcI4) {
    const char* json =
        R"({"instructions":[)"
        R"({"opCode":0,"ilOffset":0,"operand":42},)"
        R"({"opCode":53,"ilOffset":1}]})";

    PatchMethod pm;
    pm.aot_core_ir_json = json;
    pm.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(json));
    pm.cached_ir = nullptr;
    pm.metadata_cache = nullptr;
    pm.signature_blob = nullptr;
    pm.signature_len = 0;

    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));

    ASSERT_NE(pm.cached_ir, nullptr) << "cached_ir still null after lowering";
    auto* ir = static_cast<IRMethod*>(pm.cached_ir);
    EXPECT_FALSE(ir->instructions.empty()) << "IRMethod has no instructions";
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Second call to PatchMethodLowerIR reuses cached IR (DCLP)
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Entry, LazyIrCaching) {
    const char* json =
        R"({"instructions":[)"
        R"({"opCode":0,"ilOffset":0,"operand":99},)"
        R"({"opCode":53,"ilOffset":1}]})";

    PatchMethod pm;
    pm.aot_core_ir_json = json;
    pm.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(json));
    pm.cached_ir = nullptr;
    pm.metadata_cache = nullptr;
    pm.signature_blob = nullptr;
    pm.signature_len = 0;

    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));
    ASSERT_NE(pm.cached_ir, nullptr) << "cached_ir null after first call";

    void* saved_ir = pm.cached_ir;
    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));

    EXPECT_EQ(pm.cached_ir, saved_ir) << "cached_ir changed between calls (should reuse)";
}

// ════════════════════════════════════════════════════════════════════════════
// Test: Empty/null JSON creates a fallback IRMethod with a single Ret instruction
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Entry, EmptyJsonFallback) {
    // Null JSON
    {
        PatchMethod pm;
        pm.aot_core_ir_json = nullptr;
        pm.aot_core_ir_json_length = 0;
        pm.cached_ir = nullptr;
        pm.metadata_cache = nullptr;
        pm.signature_blob = nullptr;
        pm.signature_len = 0;

        PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));
        EXPECT_NE(pm.cached_ir, nullptr) << "cached_ir null for null JSON";
    }

    // Empty string JSON
    {
        PatchMethod pm;
        pm.aot_core_ir_json = "";
        pm.aot_core_ir_json_length = 0;
        pm.cached_ir = nullptr;
        pm.metadata_cache = nullptr;
        pm.signature_blob = nullptr;
        pm.signature_len = 0;

        PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));
        EXPECT_NE(pm.cached_ir, nullptr) << "cached_ir null for empty JSON";
    }
}

// ════════════════════════════════════════════════════════════════════════════
// Test: ArgBuffer read/write round-trip for all supported types
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Entry, ArgBufferReadWrite) {
    char buf[64] = {};
    ArgBuffer writer(buf);

    writer.WriteI32(42);
    writer.WriteI64(0xDEADBEEFCAFEll);
    writer.WriteF32(2.5f);
    writer.WriteF64(3.14159265358979);
    void* test_ptr = &buf;
    writer.WritePtr(test_ptr);

    ArgBuffer reader(buf);

    EXPECT_EQ(reader.ReadI32(), 42);
    EXPECT_EQ(reader.ReadI64(), 0xDEADBEEFCAFEll);
    EXPECT_FLOAT_EQ(reader.ReadF32(), 2.5f);
    EXPECT_DOUBLE_EQ(reader.ReadF64(), 3.14159265358979);
    EXPECT_EQ(reader.ReadPtr(), test_ptr);
}

// ════════════════════════════════════════════════════════════════════════════
// Test: PatchMethodLowerIR with method_key=0 returns immediately (no crash)
// ════════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Entry, NullMethodKey) {
    PatchMethodLowerIR(0);
}
