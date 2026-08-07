// interpreter_mixed_execution_test.cpp — Hot-update + InterpreterEntryDirect integration
//
// Tests that InterpreterEntryDirect correctly dispatches to method_replacement
// thunks. Constructs a minimal PatchMethod with valid AotCoreIr JSON (3
// instructions to bypass the 2-instr fast path), registers a replacement thunk,
// and verifies it is called through the InterpreterEntryDirect dispatch chain.
//
// GC-free: tests only the dispatch integration between interpreter entry and
// method replacement, no GC dependencies.

#include <method_replacement.h>
#include <interpreter_entry.h>
#include <patch_loader.h>
#include <thread_state.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

namespace mr = chaos::il2cpp::method_replacement;
namespace rc = chaos::il2cpp::runtime_core;
namespace threading = chaos::il2cpp::runtime_core::threading;

// AOT symbol stubs (required by linkage — interpreter_entry.cpp references these)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

// ── Constants ───────────────────────────────────────────────────────────────

static constexpr uint32_t kTokenA = 0x06000042u;
static constexpr uint32_t kTokenB = 0x06000043u;

// A 3-instruction AotCoreIr JSON: LdcI4(10) → LdcI4(20) → Ret
// 3 instructions bypasses the 2-instr fast path (Step 1c) in
// InterpreterEntryDirect, so the flow reaches the method_replacement::Resolve
// check before any tiered execution begins.
static const char kThreeInstrJson[] = R"({
    "instructions": [
        {"opCode": 0, "ilOffset": 0, "operand": 10},
        {"opCode": 0, "ilOffset": 1, "operand": 20},
        {"opCode": 53, "ilOffset": 2}
    ]
})";

// ── Replacement thunks ──────────────────────────────────────────────────────
// These have signature void(void* args_buf, void* ret_buf) matching
// the ReplacementFn typedef in InterpreterEntryDirect.

static bool g_thunk_called = false;

static void ThunkReturn42(void* /*args_buf*/, void* ret_buf) {
    g_thunk_called = true;
    if (ret_buf) {
        *static_cast<int32_t*>(ret_buf) = 0x42;
    }
}

static void ThunkReturn84(void* /*args_buf*/, void* ret_buf) {
    g_thunk_called = true;
    if (ret_buf) {
        *static_cast<int32_t*>(ret_buf) = 0x84;
    }
}

static void ThunkIncArg(void* args_buf, void* ret_buf) {
    g_thunk_called = true;
    int32_t val = 0;
    if (args_buf) val = *static_cast<const int32_t*>(args_buf);
    if (ret_buf) *static_cast<int32_t*>(ret_buf) = val + 1;
}

// ── Test fixture ────────────────────────────────────────────────────────────

class InterpreterMixedExecutionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // RegisterThread sets up TLS (tls_this_thread, tls_this_thread_id,
        // gc_mode=kGcModeCooperative) required by InterpreterEntryDirect assertions.
        threading::RegisterThread(threading::kMainThreadId, nullptr);
        mr::RevertAll();
        g_thunk_called = false;
    }

    void TearDown() override {
        mr::RevertAll();
        threading::UnregisterThread();
    }
};

// ── Tests ───────────────────────────────────────────────────────────────────

TEST_F(InterpreterMixedExecutionTest, ReplacementDispatchedThroughInterpreterEntry)
{
    rc::PatchMethod pm{};
    pm.token = kTokenA;
    pm.module_id = 0;
    pm.aot_core_ir_json = kThreeInstrJson;
    pm.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(kThreeInstrJson));
    uintptr_t method_key = reinterpret_cast<uintptr_t>(&pm);

    ASSERT_TRUE(mr::Register(kTokenA, reinterpret_cast<void*>(&ThunkReturn42)));

    int32_t result = 0;
    g_thunk_called = false;
    rc::InterpreterEntryDirect(method_key, nullptr, &result);

    EXPECT_TRUE(g_thunk_called);
    EXPECT_EQ(result, 0x42);

    mr::Revert(kTokenA);
}

TEST_F(InterpreterMixedExecutionTest, ReplacementReceivesArgs)
{
    rc::PatchMethod pm{};
    pm.token = kTokenA;
    pm.module_id = 0;
    pm.aot_core_ir_json = kThreeInstrJson;
    pm.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(kThreeInstrJson));
    uintptr_t method_key = reinterpret_cast<uintptr_t>(&pm);

    ASSERT_TRUE(mr::Register(kTokenA, reinterpret_cast<void*>(&ThunkIncArg)));

    int32_t arg = 41;
    int32_t result = 0;
    g_thunk_called = false;
    rc::InterpreterEntryDirect(method_key, &arg, &result);

    EXPECT_TRUE(g_thunk_called);
    EXPECT_EQ(result, 42);

    mr::Revert(kTokenA);
}

TEST_F(InterpreterMixedExecutionTest, MultipleTokens)
{
    rc::PatchMethod pm_a{};
    pm_a.token = kTokenA;
    pm_a.module_id = 0;
    pm_a.aot_core_ir_json = kThreeInstrJson;
    pm_a.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(kThreeInstrJson));

    rc::PatchMethod pm_b{};
    pm_b.token = kTokenB;
    pm_b.module_id = 0;
    pm_b.aot_core_ir_json = kThreeInstrJson;
    pm_b.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(kThreeInstrJson));

    ASSERT_TRUE(mr::Register(kTokenA, reinterpret_cast<void*>(&ThunkReturn42)));
    ASSERT_TRUE(mr::Register(kTokenB, reinterpret_cast<void*>(&ThunkReturn84)));

    int32_t result_a = 0;
    int32_t result_b = 0;

    g_thunk_called = false;
    rc::InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm_a), nullptr, &result_a);
    EXPECT_TRUE(g_thunk_called);
    EXPECT_EQ(result_a, 0x42);

    g_thunk_called = false;
    rc::InterpreterEntryDirect(reinterpret_cast<uintptr_t>(&pm_b), nullptr, &result_b);
    EXPECT_TRUE(g_thunk_called);
    EXPECT_EQ(result_b, 0x84);

    mr::Revert(kTokenA);
    mr::Revert(kTokenB);
}

TEST_F(InterpreterMixedExecutionTest, RevertThenReRegister)
{
    rc::PatchMethod pm{};
    pm.token = kTokenA;
    pm.module_id = 0;
    pm.aot_core_ir_json = kThreeInstrJson;
    pm.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(kThreeInstrJson));
    uintptr_t method_key = reinterpret_cast<uintptr_t>(&pm);

    // First registration → verify thunk.
    ASSERT_TRUE(mr::Register(kTokenA, reinterpret_cast<void*>(&ThunkReturn42)));
    int32_t result = 0;
    g_thunk_called = false;
    rc::InterpreterEntryDirect(method_key, nullptr, &result);
    EXPECT_TRUE(g_thunk_called);
    EXPECT_EQ(result, 0x42);

    // Revert → Resolve returns null.
    ASSERT_TRUE(mr::Revert(kTokenA));
    EXPECT_EQ(mr::Resolve(kTokenA), nullptr);

    // Re-register with different thunk → new thunk is dispatched.
    ASSERT_TRUE(mr::Register(kTokenA, reinterpret_cast<void*>(&ThunkReturn84)));
    g_thunk_called = false;
    result = 0;
    rc::InterpreterEntryDirect(method_key, nullptr, &result);
    EXPECT_TRUE(g_thunk_called);
    EXPECT_EQ(result, 0x84);

    mr::Revert(kTokenA);
}

TEST_F(InterpreterMixedExecutionTest, RevertClearsResolve)
{
    rc::PatchMethod pm{};
    pm.token = kTokenA;
    pm.module_id = 0;
    pm.aot_core_ir_json = kThreeInstrJson;
    pm.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(kThreeInstrJson));
    uintptr_t method_key = reinterpret_cast<uintptr_t>(&pm);

    ASSERT_TRUE(mr::Register(kTokenA, reinterpret_cast<void*>(&ThunkReturn42)));
    EXPECT_NE(mr::Resolve(kTokenA), nullptr);

    ASSERT_TRUE(mr::Revert(kTokenA));

    // After revert, Resolve returns null and InterpreterEntryDirect
    // will NOT call the thunk (falls through to tiered execution).
    EXPECT_EQ(mr::Resolve(kTokenA), nullptr);

    g_thunk_called = false;
    rc::InterpreterEntryDirect(method_key, nullptr, nullptr);
    EXPECT_FALSE(g_thunk_called);
}

TEST_F(InterpreterMixedExecutionTest, NullRetBuf)
{
    // Replacement with ret_buf=nullptr (void method) should not crash.
    rc::PatchMethod pm{};
    pm.token = kTokenA;
    pm.module_id = 0;
    pm.aot_core_ir_json = kThreeInstrJson;
    pm.aot_core_ir_json_length = static_cast<uint32_t>(std::strlen(kThreeInstrJson));
    uintptr_t method_key = reinterpret_cast<uintptr_t>(&pm);

    ASSERT_TRUE(mr::Register(kTokenA, reinterpret_cast<void*>(&ThunkReturn42)));

    g_thunk_called = false;
    EXPECT_NO_THROW({
        rc::InterpreterEntryDirect(method_key, nullptr, nullptr);
    });
    EXPECT_TRUE(g_thunk_called);

    mr::Revert(kTokenA);
}
