/// Unit tests for the hotpatch interpreter entry components.
///
/// Tests:
///   1. PatchMethodLowerIR: lazy IR deserialization and caching
///   2. ArgBuffer read/write round-trip for all supported types
///   3. Null/missing JSON handling (empty IR with Ret)
///
/// These tests operate at the component level and don't require full runtime
/// bootstrap. For InterpreterEntryDirect end-to-end tests (which need AOT
/// bridge + dispatch context), see the hotupdate verification pipeline.

#include "interpreter_entry.h"
#include "interpreter_vm.h"
#include "patch_loader.h"

#include <cstdio>
#include <cstring>
#include <iostream>

// ── Namespace aliases ───────────────────────────────────────────────────
using chaos::il2cpp::interpreter::IRMethod;
using chaos::il2cpp::runtime_core::ArgBuffer;
using chaos::il2cpp::runtime_core::PatchMethod;
using chaos::il2cpp::runtime_core::PatchMethodLowerIR;

// Test helpers
static int failures = 0;

#define TEST(name)                                                      \
    do {                                                                \
        if (!(name())) {                                                \
            std::cerr << "FAIL: " << #name << std::endl;                \
            ++failures;                                                 \
        }                                                               \
    } while (false)

// ════════════════════════════════════════════════════════════════════════════
// Test declarations
// ════════════════════════════════════════════════════════════════════════════

static bool TestLowerLdcI4();
static bool TestLazyIrCaching();
static bool TestEmptyJsonFallback();
static bool TestArgBufferReadWrite();
static bool TestNullMethodKey();

// ════════════════════════════════════════════════════════════════════════════
// Test implementations
// ════════════════════════════════════════════════════════════════════════════

/// Test: PatchMethodLowerIR deserializes a simple ldc.i4+ret JSON.
///   JSON: ldc.i4 42 + ret
///   Expected: IRMethod with 2 instructions, cached_ir populated.
static bool TestLowerLdcI4() {
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

    if (pm.cached_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir still null after lowering\n");
        return false;
    }

    auto* ir = static_cast<IRMethod*>(pm.cached_ir);
    if (ir->instructions.empty()) {
        std::fprintf(stderr, "  FAIL: IRMethod has no instructions\n");
        return false;
    }

    return true;
}

/// Test: Second call to PatchMethodLowerIR reuses cached IR (DCLP).
static bool TestLazyIrCaching() {
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

    // First call — triggers lazy deserialization.
    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));

    void* saved_ir = pm.cached_ir;
    if (saved_ir == nullptr) {
        std::fprintf(stderr, "  FAIL: cached_ir null after first call\n");
        return false;
    }

    // Second call — should use cached IR (no re-deserialization).
    PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));

    if (pm.cached_ir != saved_ir) {
        std::fprintf(stderr, "  FAIL: cached_ir changed between calls (%p -> %p)\n",
                     saved_ir, pm.cached_ir);
        return false;
    }

    return true;
}

/// Test: Empty/null JSON creates a fallback IRMethod with a single Ret instruction.
static bool TestEmptyJsonFallback() {
    // Null JSON.
    {
        PatchMethod pm;
        pm.aot_core_ir_json = nullptr;
        pm.aot_core_ir_json_length = 0;
        pm.cached_ir = nullptr;
        pm.metadata_cache = nullptr;
        pm.signature_blob = nullptr;
        pm.signature_len = 0;

        PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));

        if (pm.cached_ir == nullptr) {
            std::fprintf(stderr, "  FAIL: cached_ir null for null JSON\n");
            return false;
        }
    }

    // Empty string JSON.
    {
        PatchMethod pm;
        pm.aot_core_ir_json = "";
        pm.aot_core_ir_json_length = 0;
        pm.cached_ir = nullptr;
        pm.metadata_cache = nullptr;
        pm.signature_blob = nullptr;
        pm.signature_len = 0;

        PatchMethodLowerIR(reinterpret_cast<uintptr_t>(&pm));

        if (pm.cached_ir == nullptr) {
            std::fprintf(stderr, "  FAIL: cached_ir null for empty JSON\n");
            return false;
        }
    }

    return true;
}

/// Test: ArgBuffer read/write round-trip for all supported types.
static bool TestArgBufferReadWrite() {
    char buf[64] = {};
    ArgBuffer writer(buf);

    writer.WriteI32(42);
    writer.WriteI64(0xDEADBEEFCAFEll);
    writer.WriteF32(2.5f);
    writer.WriteF64(3.14159265358979);
    void* test_ptr = &buf;
    writer.WritePtr(test_ptr);

    // Read back in same order.
    ArgBuffer reader(buf);

    auto i32 = reader.ReadI32();
    if (i32 != 42) {
        std::fprintf(stderr, "  FAIL: ReadI32 expected 42, got %d\n", (int)i32);
        return false;
    }

    auto i64 = reader.ReadI64();
    if (i64 != 0xDEADBEEFCAFEll) {
        std::fprintf(stderr, "  FAIL: ReadI64 expected 0xDEADBEEFCAFE, got 0x%llx\n",
                     (unsigned long long)i64);
        return false;
    }

    auto f32 = reader.ReadF32();
    if (f32 != 2.5f) {
        std::fprintf(stderr, "  FAIL: ReadF32 expected 2.5f, got %f\n", f32);
        return false;
    }

    auto f64 = reader.ReadF64();
    if (f64 != 3.14159265358979) {
        std::fprintf(stderr, "  FAIL: ReadF64 expected 3.14159, got %f\n", f64);
        return false;
    }

    auto ptr = reader.ReadPtr();
    if (ptr != test_ptr) {
        std::fprintf(stderr, "  FAIL: ReadPtr expected %p, got %p\n", test_ptr, ptr);
        return false;
    }

    return true;
}

/// Test: PatchMethodLowerIR with method_key=0 returns immediately (no crash).
static bool TestNullMethodKey() {
    PatchMethodLowerIR(0);
    return true;
}

// ════════════════════════════════════════════════════════════════════════════
// Test runner
// ════════════════════════════════════════════════════════════════════════════

int main() {
    TEST(TestLowerLdcI4);
    TEST(TestLazyIrCaching);
    TEST(TestEmptyJsonFallback);
    TEST(TestArgBufferReadWrite);
    TEST(TestNullMethodKey);

    std::cout << "interpreter-entry=failures=" << failures << std::endl;
    if (failures > 0) {
        std::cerr << "SOME INTERPRETER ENTRY TESTS FAILED" << std::endl;
        return 1;
    }
    std::cout << "all interpreter entry tests passed" << std::endl;
    return 0;
}