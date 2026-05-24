// ── Hybrid mode end-to-end tests ─────────────────────────────────────
//
// Tests the Hybrid mode dispatch pipeline end-to-end:
//   1. HybridPrecode counter-based dispatch (counter → AOT → JIT)
//   2. HybridStubDispatchImpl counter decrement + CAS compile
//   3. PrecodeArena hybrid trampoline allocation and layout
//   4. Deopt AOT fallback path in JitMethod::aot_entry
//   5. Full end-to-end flow through PrecodeArena + HotpatchEntryV0
//
// These tests exercise the same dispatch path used by RegisterHybridMethods
// at startup, without requiring the full HotpatchNameRegistry bootstrap.

#include <gtest/gtest.h>

#include "jit_precode.h"
#include "jit_engine.h"
#include "jit_method.h"
#include "ir_reg_alloc.h"

#include <codegen_bridge.h>

#include <windows.h>
#include <cstdint>
#include <cstring>
#include <atomic>
#include <thread>

// ── Namespace aliases ──────────────────────────────────────────────
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasSrc1;
using chaos::il2cpp::interpreter::kRegHasSrc2;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::jit::Compile;
using chaos::il2cpp::jit::CanCompile;
using chaos::il2cpp::jit::JitMethod;
using chaos::il2cpp::jit::CompileConfig;
using chaos::il2cpp::jit::JitPrecode;
using chaos::il2cpp::jit::HybridPrecode;
using chaos::il2cpp::jit::PrecodeArena;
using chaos::il2cpp::jit::JitStubDispatchImpl;
using chaos::il2cpp::jit::HybridStubDispatchImpl;
using chaos::il2cpp::jit::kPrecodeUncompiled;
using chaos::il2cpp::jit::kPrecodeCompiling;
using chaos::il2cpp::jit::kPrecodeCompiled;
using chaos::il2cpp::jit::kJitUpgradeThreshold;

// ── Helpers (matching jit_mode_test.cpp pattern) ───────────────────

static uint64_t MakeHeader(IROpCode opc, uint8_t dst, uint8_t src1,
                           uint8_t src2, uint8_t flags) noexcept {
    return static_cast<uint64_t>(opc) |
           (static_cast<uint64_t>(dst)   << 16) |
           (static_cast<uint64_t>(src1)  << 24) |
           (static_cast<uint64_t>(src2)  << 32) |
           (static_cast<uint64_t>(flags) << 40);
}

static RegisterInstruction InstrI4(IROpCode opc, int32_t imm,
                                   uint8_t dst = 0, uint8_t flags = 0) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, dst, 0, 0, flags | kRegHasDst | kRegHasImm);
    ri.imm.i4 = imm;
    return ri;
}

static RegisterInstruction InstrBinary(IROpCode opc, uint8_t dst,
                                       uint8_t src1, uint8_t src2) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, dst, src1, src2,
                           kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrRet(uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Ret, 0, src, 0, kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

// Generated code uses (args, ret) two-pointer ABI.
static uint64_t ExecuteNative(void* entry) {
    uint64_t args_buf[8] = {};
    uint64_t ret_buf[2] = {};
    using NativeEntry = void (*)(void*, void*);
    auto native_entry = reinterpret_cast<NativeEntry>(entry);
    native_entry(args_buf, ret_buf);
    return ret_buf[0];
}

// A simple AOT function that writes a constant to ret_buf.
static void AotReturn42(void* /*args_buf*/, void* ret_buf) noexcept {
    auto* ret = static_cast<uint64_t*>(ret_buf);
    ret[0] = 42ULL;
}

// ── Test fixture ──────────────────────────────────────────────────
struct HybridModeTest : public ::testing::Test {
    static RegisterMethod MakeReturnConstantMethod(int32_t value) {
        RegisterMethod method;
        method.max_regs = 1;
        method.instructions.push_back(InstrI4(IROpCode::LdcI4, value, 0, 0));
        method.instructions.push_back(InstrRet(0));
        return method;
    }

    static RegisterMethod MakeAddMethod(int32_t a, int32_t b) {
        RegisterMethod method;
        method.max_regs = 3;
        method.instructions.push_back(InstrI4(IROpCode::LdcI4, a, 0, 0));
        method.instructions.push_back(InstrI4(IROpCode::LdcI4, b, 1, 0));
        method.instructions.push_back(InstrBinary(IROpCode::Add, 2, 0, 1));
        method.instructions.push_back(InstrRet(2));
        return method;
    }
};

// ── HybridStubDispatchImpl tests ──────────────────────────────────

TEST_F(HybridModeTest, CounterReturnsAotEntryWhileCold) {
    HybridPrecode precode;
    precode.call_counter.store(5, std::memory_order_relaxed);
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

    // Call dispatch while counter > 0 — should return aot_entry
    for (int i = 0; i < 4; i++) {
        void* entry = HybridStubDispatchImpl(&precode);
        EXPECT_EQ(entry, precode.aot_entry)
            << "Call " << i << " should return aot_entry";

        // Counter should have decremented
        uint32_t remaining = precode.call_counter.load(std::memory_order_relaxed);
        EXPECT_EQ(remaining, static_cast<uint32_t>(4 - i))
            << "Counter mismatch at iteration " << i;
    }

    // State should still be Uncompiled (JIT not triggered)
    EXPECT_EQ(precode.state.load(), kPrecodeUncompiled);
}

TEST_F(HybridModeTest, CounterTriggersJitOnLastCall) {
    HybridPrecode precode;
    precode.call_counter.store(1, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(77);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

    // This call should trigger JIT compilation
    void* entry = HybridStubDispatchImpl(&precode);
    ASSERT_NE(entry, nullptr);

    // State should be Compiled
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);
    ASSERT_NE(precode.compiled, nullptr);

    // The returned entry should be the JIT-compiled code, not aot_entry
    EXPECT_EQ(entry, precode.compiled->code);
    EXPECT_NE(entry, precode.aot_entry);

    // The compiled code should return the correct value (77)
    EXPECT_EQ(ExecuteNative(entry), 77ULL);
}

TEST_F(HybridModeTest, ReturnsCompiledCodeAfterJit) {
    HybridPrecode precode;
    precode.call_counter.store(1, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(55);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

    // First call triggers JIT
    void* first_entry = HybridStubDispatchImpl(&precode);
    ASSERT_NE(first_entry, nullptr);
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);

    // Second call should return compiled code directly (no counter check)
    void* second_entry = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(second_entry, first_entry);
    EXPECT_EQ(ExecuteNative(second_entry), 55ULL);
}

TEST_F(HybridModeTest, CounterAndJitFullSequence) {
    // Full hybrid sequence: counter → AOT → JIT compilation → compiled
    HybridPrecode precode;
    precode.call_counter.store(3, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(99);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

    // Call 1: counter=3 → return aot_entry
    void* entry1 = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(entry1, precode.aot_entry);
    EXPECT_EQ(ExecuteNative(entry1), 42ULL);

    // Call 2: counter=2 → return aot_entry
    void* entry2 = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(entry2, precode.aot_entry);

    // Call 3: counter=1 → triggers JIT, returns compiled code
    void* entry3 = HybridStubDispatchImpl(&precode);
    ASSERT_NE(entry3, nullptr);
    EXPECT_NE(entry3, precode.aot_entry);
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);
    EXPECT_EQ(ExecuteNative(entry3), 99ULL);

    // Call 4: already compiled → returns compiled code directly
    void* entry4 = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(entry4, entry3);
    EXPECT_EQ(ExecuteNative(entry4), 99ULL);
}

TEST_F(HybridModeTest, AotEntryOnFailedCompile) {
    // When Compile() fails, the dispatch should still return aot_entry
    // and reset state for retry.
    HybridPrecode precode;
    precode.call_counter.store(1, std::memory_order_relaxed);
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

    // No ir set — Compile() will fail
    // Call should trigger JIT attempt, fail, and reset state
    void* entry = HybridStubDispatchImpl(&precode);
    // Should return aot_entry as fallback
    EXPECT_EQ(entry, precode.aot_entry);

    // State should be reset to Uncompiled
    EXPECT_EQ(precode.state.load(), kPrecodeUncompiled);

    // Counter should be reset to 1 for retry
    EXPECT_EQ(precode.call_counter.load(std::memory_order_relaxed), 1u);
}

TEST_F(HybridModeTest, DispatchWithoutEntryStillCompiles) {
    // When entry is not set, HybridStubDispatchImpl should still compile
    HybridPrecode precode;
    precode.call_counter.store(1, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(33);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);
    precode.entry = nullptr;

    // Trigger JIT
    void* code = HybridStubDispatchImpl(&precode);
    ASSERT_NE(code, nullptr);
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);
    ASSERT_NE(precode.compiled, nullptr);

    // Should return JIT code, not aot_entry
    EXPECT_NE(code, precode.aot_entry);
}

// ── PrecodeArena hybrid trampoline tests ──────────────────────────

TEST_F(HybridModeTest, PrecodeArenaAllocatesHybridTrampoline) {
    PrecodeArena arena;

    HybridPrecode precode;
    precode.call_counter.store(kJitUpgradeThreshold, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(42);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

    void* trampoline = arena.AllocateHybridTrampoline(&precode);
    ASSERT_NE(trampoline, nullptr);

    // The trampoline should be executable memory (RWX page)
    // Verify the first bytes: 0x49 0xBA = mov r10, imm64
    auto* bytes = static_cast<const uint8_t*>(trampoline);
    EXPECT_EQ(bytes[0], 0x49);
    EXPECT_EQ(bytes[1], 0xBA);
}

TEST_F(HybridModeTest, HybridPrecodeArenaMultipleTrampolines) {
    PrecodeArena arena;

    HybridPrecode precodes[5];
    void* trampolines[5];
    for (int i = 0; i < 5; i++) {
        precodes[i].call_counter.store(kJitUpgradeThreshold, std::memory_order_relaxed);
        precodes[i].ir = MakeReturnConstantMethod(i * 10);
        precodes[i].config = CompileConfig{};
        precodes[i].aot_entry = reinterpret_cast<void*>(&AotReturn42);
        trampolines[i] = arena.AllocateHybridTrampoline(&precodes[i]);
        ASSERT_NE(trampolines[i], nullptr);
    }

    // Verify the precode pointers are embedded in the trampolines
    for (int i = 0; i < 5; i++) {
        auto* bytes = static_cast<const uint8_t*>(trampolines[i]);
        EXPECT_EQ(bytes[0], 0x49);
        EXPECT_EQ(bytes[1], 0xBA);

        uintptr_t embedded_addr;
        std::memcpy(&embedded_addr, bytes + 2, sizeof(embedded_addr));
        EXPECT_EQ(embedded_addr, reinterpret_cast<uintptr_t>(&precodes[i]));
    }
}

TEST_F(HybridModeTest, FullDispatchThroughHybridArena) {
    PrecodeArena arena;

    // Set up precode + trampoline (same pattern as RegisterHybridMethods)
    HybridPrecode precode;
    precode.call_counter.store(3, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(88);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

    HotpatchEntryV0 entry{};
    precode.entry = &entry;
    precode.trampoline = arena.AllocateHybridTrampoline(&precode);
    ASSERT_NE(precode.trampoline, nullptr);

    // Set direct_ptr to the trampoline
    entry.direct_ptr = precode.trampoline;

    // Cold calls through dispatch — should return aot_entry
    void* aot_result = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(aot_result, precode.aot_entry);
    EXPECT_EQ(ExecuteNative(aot_result), 42ULL);

    void* aot_result2 = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(aot_result2, precode.aot_entry);

    // Third call — counter reaches 0, triggers JIT
    void* jit_result = HybridStubDispatchImpl(&precode);
    ASSERT_NE(jit_result, nullptr);
    EXPECT_NE(jit_result, precode.aot_entry);
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);
    EXPECT_EQ(ExecuteNative(jit_result), 88ULL);

    // direct_ptr should now point to compiled code
    EXPECT_EQ(entry.direct_ptr, jit_result);
    EXPECT_NE(entry.direct_ptr, precode.trampoline);

    // Subsequent calls — return compiled code directly
    void* jit_result2 = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(jit_result2, jit_result);
    EXPECT_EQ(ExecuteNative(jit_result2), 88ULL);
}

// ── JitMethod::aot_entry tests (deopt fallback) ───────────────────

TEST_F(HybridModeTest, AotEntrySetOnCompiledJitMethod) {
    // Verify that after HybridStubDispatchImpl compiles, the JitMethod
    // has aot_entry set to the HybridPrecode's aot_entry field.
    HybridPrecode precode;
    precode.call_counter.store(1, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(66);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

    void* code = HybridStubDispatchImpl(&precode);
    ASSERT_NE(code, nullptr);
    ASSERT_NE(precode.compiled, nullptr);

    // The compiled JitMethod should have aot_entry set
    EXPECT_EQ(precode.compiled->aot_entry, precode.aot_entry);
    EXPECT_NE(precode.compiled->aot_entry, nullptr);
}

// ── Multi-threaded tests ──────────────────────────────────────────

TEST_F(HybridModeTest, MultiThreadedHybridDispatchStress) {
    // Verify concurrent calls to HybridStubDispatchImpl
    PrecodeArena arena;

    auto* precode = new HybridPrecode();
    precode->call_counter.store(3, std::memory_order_relaxed);
    precode->ir = MakeReturnConstantMethod(123);
    precode->config = CompileConfig{};
    precode->aot_entry = reinterpret_cast<void*>(&AotReturn42);
    HotpatchEntryV0 entry{};
    precode->entry = &entry;
    precode->trampoline = arena.AllocateHybridTrampoline(precode);
    entry.direct_ptr = precode->trampoline;

    constexpr int kNumThreads = 8;
    std::atomic<int> ready_count{0};
    std::atomic<bool> start_flag{false};
    void* results[kNumThreads] = {};

    auto thread_fn = [&](int tid) {
        ready_count.fetch_add(1, std::memory_order_release);
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        results[tid] = HybridStubDispatchImpl(precode);
    };

    std::thread threads[kNumThreads];
    for (int i = 0; i < kNumThreads; i++) {
        threads[i] = std::thread(thread_fn, i);
    }

    while (ready_count.load(std::memory_order_acquire) < kNumThreads) {
        std::this_thread::yield();
    }

    start_flag.store(true, std::memory_order_release);

    for (auto& t : threads) {
        t.join();
    }

    // Some threads may have gotten aot_entry (counter, cold path),
    // some may have gotten compiled code.  At least one should be compiled.
    bool found_jit = false;
    bool found_aot = false;
    void* jit_code = nullptr;
    for (int i = 0; i < kNumThreads; i++) {
        if (results[i] == precode->aot_entry) {
            found_aot = true;
        } else if (results[i] != nullptr) {
            found_jit = true;
            jit_code = results[i];
        }
    }
    EXPECT_TRUE(found_aot) << "At least one thread should have gotten aot_entry";
    EXPECT_TRUE(found_jit) << "At least one thread should have gotten JIT code";
    ASSERT_NE(jit_code, nullptr);

    // Verify the JIT code produces the correct result
    EXPECT_EQ(ExecuteNative(jit_code), 123ULL);

    // State should be Compiled
    EXPECT_EQ(precode->state.load(), kPrecodeCompiled);

    delete precode;
}

TEST_F(HybridModeTest, MultiThreadedJitOnly) {
    // Verify concurrent calls where counter = 1 (all threads race for compilation)
    PrecodeArena arena;

    auto* precode = new HybridPrecode();
    precode->call_counter.store(1, std::memory_order_relaxed);
    precode->ir = MakeReturnConstantMethod(200);
    precode->config = CompileConfig{};
    precode->aot_entry = reinterpret_cast<void*>(&AotReturn42);
    HotpatchEntryV0 entry{};
    precode->entry = &entry;
    precode->trampoline = arena.AllocateHybridTrampoline(precode);
    entry.direct_ptr = precode->trampoline;

    constexpr int kNumThreads = 8;
    std::atomic<int> ready_count{0};
    std::atomic<bool> start_flag{false};
    void* results[kNumThreads] = {};

    auto thread_fn = [&](int tid) {
        ready_count.fetch_add(1, std::memory_order_release);
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        results[tid] = HybridStubDispatchImpl(precode);
    };

    std::thread threads[kNumThreads];
    for (int i = 0; i < kNumThreads; i++) {
        threads[i] = std::thread(thread_fn, i);
    }

    while (ready_count.load(std::memory_order_acquire) < kNumThreads) {
        std::this_thread::yield();
    }

    start_flag.store(true, std::memory_order_release);

    for (auto& t : threads) {
        t.join();
    }

    // Both aot_entry and compiled code are valid returns for counter=1 + CAS race.
    // The thread that wins CAS compiles; losers might see compiled code or aot_entry
    // depending on timing.  Find JIT code pointer across all threads.
    bool found_jit = false;
    void* jit_code = nullptr;
    for (int i = 0; i < kNumThreads; i++) {
        if (results[i] != nullptr && results[i] != precode->aot_entry) {
            found_jit = true;
            jit_code = results[i];
        }
    }
    EXPECT_TRUE(found_jit) << "At least one thread should have gotten JIT code";
    EXPECT_EQ(precode->state.load(), kPrecodeCompiled);
    if (jit_code != nullptr) {
        EXPECT_EQ(ExecuteNative(jit_code), 200ULL);
    }

    delete precode;
}
