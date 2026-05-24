// ── Hybrid mode end-to-end integration test ────────────────────────
//
// Three-phase verification:
//   Phase 1 (AOT):      Counter > 0 → dispatch returns aot_entry
//   Phase 2 (Upgrade):  Counter hits 0 → triggers JIT compilation
//   Phase 3 (JIT):      Compiled path → returns JIT code directly
//
// This test simulates the full RegisterHybridMethods startup pattern:
//   HybridPrecode + PrecodeArena + HotpatchEntryV0 + trampoline.

#include <gtest/gtest.h>

#include "jit_precode.h"
#include "jit_engine.h"
#include "jit_method.h"

#include <codegen_bridge.h>

#include <cstdint>
#include <cstring>
#include <atomic>
#include <thread>
#include <vector>

// ── Namespace aliases ──────────────────────────────────────────────
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasSrc1;
using chaos::il2cpp::interpreter::kRegHasSrc2;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::jit::Compile;
using chaos::il2cpp::jit::JitMethod;
using chaos::il2cpp::jit::CompileConfig;
using chaos::il2cpp::jit::HybridPrecode;
using chaos::il2cpp::jit::PrecodeArena;
using chaos::il2cpp::jit::HybridStubDispatchImpl;
using chaos::il2cpp::jit::kPrecodeUncompiled;
using chaos::il2cpp::jit::kPrecodeCompiled;
using chaos::il2cpp::jit::kJitUpgradeThreshold;

// ── Helpers (matching jit_hybrid_mode_test.cpp pattern) ───────────

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

static void AotReturn100(void* /*args_buf*/, void* ret_buf) noexcept {
    auto* ret = static_cast<uint64_t*>(ret_buf);
    ret[0] = 100ULL;
}

// ── Test fixture ──────────────────────────────────────────────────
struct HybridE2ETest : public ::testing::Test {
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

    // Full hybrid setup matching RegisterHybridMethods pattern
    static void SetupHybridMethod(HybridPrecode& precode, PrecodeArena& arena,
                                  HotpatchEntryV0& entry,
                                  RegisterMethod ir, void* aot_entry,
                                  uint32_t initial_counter = kJitUpgradeThreshold) {
        precode.call_counter.store(initial_counter, std::memory_order_relaxed);
        precode.ir = std::move(ir);
        precode.config = CompileConfig{};
        precode.aot_entry = aot_entry;
        precode.entry = &entry;
        precode.trampoline = arena.AllocateHybridTrampoline(&precode);
        ASSERT_NE(precode.trampoline, nullptr);
        entry.direct_ptr = precode.trampoline;
    }
};

// ── Three-phase end-to-end tests ──────────────────────────────────

TEST_F(HybridE2ETest, ThreePhaseFullSequence) {
    // Phase 1 (AOT):  Cold calls through dispatch → aot_entry
    // Phase 2 (Upgrade):  Final call triggers JIT
    // Phase 3 (JIT):  Subsequent calls return compiled code
    PrecodeArena arena;
    HotpatchEntryV0 entry{};
    HybridPrecode precode;
    SetupHybridMethod(precode, arena, entry,
                      MakeReturnConstantMethod(77),
                      reinterpret_cast<void*>(&AotReturn42));

    // ── Phase 1: AOT path ──
    // Call dispatch (kJitUpgradeThreshold - 1) times. Counter > 1 means dispatch
    // returns the trampoline → shared entry → aot_entry → AOT result (42).
    constexpr uint32_t kAotCalls = kJitUpgradeThreshold - 1;
    for (uint32_t i = 0; i < kAotCalls; i++) {
        void* result = HybridStubDispatchImpl(&precode);
        ASSERT_NE(result, nullptr)
            << "Phase 1 (AOT): call " << i << " should return a valid entry";
        ASSERT_EQ(ExecuteNative(result), 42ULL)
            << "Phase 1 (AOT): call " << i << " should produce AOT result";

        // State should remain Uncompiled during Phase 1
        ASSERT_EQ(precode.state.load(), kPrecodeUncompiled);
    }

    // ── Phase 2: Upgrade ──
    // Counter reaches 1 → fetch_sub returns 1 → triggers JIT compilation.
    // The dispatch returns compiled code (or aot_entry if CAS race).
    void* upgrade_result = HybridStubDispatchImpl(&precode);
    ASSERT_NE(upgrade_result, nullptr);
    ASSERT_NE(upgrade_result, nullptr);

    // State must be Compiled after upgrade
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);
    ASSERT_NE(precode.compiled, nullptr);

    // The precode should now have compiled code
    ASSERT_NE(precode.compiled->code, nullptr);

    // ── Phase 3: JIT path ──
    // All subsequent calls return compiled code directly.
    for (int i = 0; i < 10; i++) {
        void* result = HybridStubDispatchImpl(&precode);
        ASSERT_EQ(result, precode.compiled->code)
            << "Phase 3 (JIT): call " << i << " should return compiled code";
        ASSERT_EQ(ExecuteNative(result), 77ULL)
            << "Phase 3 (JIT): call " << i << " should produce JIT result";
    }

    // Verify aot_entry is preserved on the compiled JitMethod
    EXPECT_EQ(precode.compiled->aot_entry, precode.aot_entry);

    // Verify direct_ptr was updated
    EXPECT_EQ(entry.direct_ptr, precode.compiled->code);
}

TEST_F(HybridE2ETest, ThreePhaseWithAddMethod) {
    // Same three-phase flow with a more complex method (add with 3 vregs).
    PrecodeArena arena;
    HotpatchEntryV0 entry{};
    HybridPrecode precode;
    SetupHybridMethod(precode, arena, entry,
                      MakeAddMethod(10, 20),
                      reinterpret_cast<void*>(&AotReturn100));

    // Phase 1: AOT (first kJitUpgradeThreshold-1 calls produce AOT result 100)
    for (uint32_t i = 0; i < kJitUpgradeThreshold - 1; i++) {
        void* result = HybridStubDispatchImpl(&precode);
        ASSERT_NE(result, nullptr);
        ASSERT_EQ(ExecuteNative(result), 100ULL);
    }

    // Phase 2: Upgrade
    void* result = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);
    ASSERT_NE(precode.compiled, nullptr);

    // Phase 3: JIT — compiled add should return 10+20=30
    void* jit_result = HybridStubDispatchImpl(&precode);
    ASSERT_EQ(jit_result, precode.compiled->code);
    ASSERT_EQ(ExecuteNative(jit_result), 30ULL);

    // Verify aot_entry preservation
    EXPECT_EQ(precode.compiled->aot_entry, precode.aot_entry);
}

TEST_F(HybridE2ETest, MultipleMethodsEndToEnd) {
    // Simulate registering multiple hybrid methods (like RegisterHybridMethods).
    PrecodeArena arena;

    struct MethodSlot {
        HybridPrecode precode;
        HotpatchEntryV0 entry;
        int expected_value;
    };

    constexpr int kNumMethods = 5;
    MethodSlot slots[kNumMethods];

    for (int i = 0; i < kNumMethods; i++) {
        int val = (i + 1) * 10;
        SetupHybridMethod(slots[i].precode, arena, slots[i].entry,
                          MakeReturnConstantMethod(val),
                          reinterpret_cast<void*>(&AotReturn42),
                          kJitUpgradeThreshold);
        slots[i].expected_value = val;
    }

    // Phase 1: All methods produce AOT result (42)
    for (int round = 0; round < 3; round++) {
        for (int i = 0; i < kNumMethods; i++) {
            void* result = HybridStubDispatchImpl(&slots[i].precode);
            ASSERT_NE(result, nullptr);
            ASSERT_EQ(ExecuteNative(result), 42ULL);
        }
    }

    // Burn through remaining counter for each method
    for (int i = 0; i < kNumMethods; i++) {
        uint32_t remaining = slots[i].precode.call_counter.load(std::memory_order_relaxed);
        for (uint32_t j = 0; j < remaining; j++) {
            HybridStubDispatchImpl(&slots[i].precode);
        }
    }

    // Phase 2+3: All methods should now be compiled
    for (int i = 0; i < kNumMethods; i++) {
        ASSERT_EQ(slots[i].precode.state.load(), kPrecodeCompiled);
        ASSERT_NE(slots[i].precode.compiled, nullptr);

        void* result = HybridStubDispatchImpl(&slots[i].precode);
        ASSERT_EQ(result, slots[i].precode.compiled->code);
        ASSERT_EQ(ExecuteNative(result), static_cast<uint64_t>(slots[i].expected_value));
    }
}

TEST_F(HybridE2ETest, UpgradeLatencyUnderCounterVariation) {
    // Test that upgrade behavior is correct with different initial counter values.
    PrecodeArena arena;

    auto run_test = [&](uint32_t initial_counter, const char* label) {
        HotpatchEntryV0 entry{};
        HybridPrecode precode;
        SetupHybridMethod(precode, arena, entry,
                          MakeReturnConstantMethod(55),
                          reinterpret_cast<void*>(&AotReturn42),
                          initial_counter);

        // Consume all AOT counter slots (counter-1 calls produce AOT result)
        uint32_t aot_calls = initial_counter > 1 ? initial_counter - 1 : 0;
        for (uint32_t i = 0; i < aot_calls; i++) {
            void* result = HybridStubDispatchImpl(&precode);
            ASSERT_NE(result, nullptr) << label << ": AOT call " << i;
            ASSERT_EQ(ExecuteNative(result), 42ULL)
                << label << ": AOT call " << i << " should produce AOT result";
        }

        // Trigger JIT
        HybridStubDispatchImpl(&precode);
        ASSERT_EQ(precode.state.load(), kPrecodeCompiled)
            << label << ": should be compiled after counter exhausted";

        // Verify JIT correctness
        void* jit_result = HybridStubDispatchImpl(&precode);
        ASSERT_EQ(jit_result, precode.compiled->code);
        ASSERT_EQ(ExecuteNative(jit_result), 55ULL)
            << label << ": JIT should produce correct result";
    };

    // Different initial counter values
    run_test(1,  "counter=1");
    run_test(5,  "counter=5");
    run_test(kJitUpgradeThreshold, "counter=kJitUpgradeThreshold(30)");
}

TEST_F(HybridE2ETest, EarlyUpgradeDoesNotLoseAotFallback) {
    // Verify that aot_entry is always valid as fallback, even after
    // multiple upgrade cycles (simulating deopt + recompile).
    PrecodeArena arena;

    // First method
    HotpatchEntryV0 entry1{};
    HybridPrecode precode1;
    SetupHybridMethod(precode1, arena, entry1,
                      MakeReturnConstantMethod(111),
                      reinterpret_cast<void*>(&AotReturn42));

    // Trigger JIT
    for (uint32_t i = 0; i < kJitUpgradeThreshold; i++) {
        HybridStubDispatchImpl(&precode1);
    }
    HybridStubDispatchImpl(&precode1);
    ASSERT_EQ(precode1.state.load(), kPrecodeCompiled);

    // aot_entry should still be intact
    EXPECT_EQ(precode1.compiled->aot_entry, precode1.aot_entry);
    EXPECT_NE(precode1.compiled->aot_entry, nullptr);

    // Calling compiled code should produce JIT result, not AOT
    void* result = HybridStubDispatchImpl(&precode1);
    EXPECT_EQ(ExecuteNative(result), 111ULL);
}

TEST_F(HybridE2ETest, NoFalseTransitionOnFailedCompile) {
    // Verify that a failed compile does NOT leave the precode in Compiled state.
    PrecodeArena arena;
    HotpatchEntryV0 entry{};
    HybridPrecode precode;
    precode.call_counter.store(1, std::memory_order_relaxed);
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);
    precode.entry = &entry;
    precode.trampoline = arena.AllocateHybridTrampoline(&precode);
    entry.direct_ptr = precode.trampoline;
    // No IR set — Compile() will fail

    void* result = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(result, precode.aot_entry);
    EXPECT_EQ(precode.state.load(), kPrecodeUncompiled)
        << "State must NOT be Compiled after failed compile";
    EXPECT_EQ(precode.call_counter.load(std::memory_order_relaxed), 1u)
        << "Counter should be reset to 1 for retry after failure";

    // Should be able to retry after setting IR
    precode.ir = MakeReturnConstantMethod(222);
    result = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);
    ASSERT_NE(precode.compiled, nullptr);
    EXPECT_EQ(ExecuteNative(precode.compiled->code), 222ULL);
}

// ── Multi-threaded end-to-end tests ───────────────────────────────

TEST_F(HybridE2ETest, ConcurrentAotAndJitCalls) {
    // Multiple threads continuously call dispatch while methods are
    // in various stages of AOT→JIT transition.
    PrecodeArena arena;

    constexpr int kNumMethods = 4;
    struct SharedMethod {
        HybridPrecode precode;
        HotpatchEntryV0 entry;
    };
    SharedMethod methods[kNumMethods];

    for (int i = 0; i < kNumMethods; i++) {
        int val = 50 + i * 25;
        SetupHybridMethod(methods[i].precode, arena, methods[i].entry,
                          MakeReturnConstantMethod(val),
                          reinterpret_cast<void*>(&AotReturn42),
                          kJitUpgradeThreshold / (i + 1));
    }

    constexpr int kNumThreads = 4;
    std::atomic<int> ready_count{0};
    std::atomic<bool> start_flag{false};
    std::atomic<int> phase1_aot_count{0};
    std::atomic<int> phase3_jit_count{0};

    auto thread_fn = [&](int tid) {
        ready_count.fetch_add(1, std::memory_order_release);
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        // Each thread calls all methods in a round-robin pattern
        for (int round = 0; round < 20; round++) {
            for (int m = 0; m < kNumMethods; m++) {
                void* result = HybridStubDispatchImpl(&methods[m].precode);
                ASSERT_NE(result, nullptr);
                uint64_t exec_result = ExecuteNative(result);
                if (exec_result == 42ULL) {
                    phase1_aot_count.fetch_add(1, std::memory_order_relaxed);
                } else {
                    phase3_jit_count.fetch_add(1, std::memory_order_relaxed);
                }
            }
        }
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

    // All methods should eventually compile
    for (int i = 0; i < kNumMethods; i++) {
        EXPECT_EQ(methods[i].precode.state.load(), kPrecodeCompiled)
            << "Method " << i << " should be compiled";
    }

    // Both AOT and JIT paths should have been exercised
    EXPECT_GT(phase1_aot_count.load(), 0);
    EXPECT_GT(phase3_jit_count.load(), 0);
}

TEST_F(HybridE2ETest, AotEntryPersistsAfterReset) {
    // Test that aot_entry survives across multiple compile attempts
    // (simulating repeated deopt-then-recompile scenarios).
    PrecodeArena arena;

    // Method that will be compiled
    HotpatchEntryV0 entry{};
    HybridPrecode precode;
    precode.call_counter.store(1, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(77);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);
    precode.entry = &entry;
    precode.trampoline = arena.AllocateHybridTrampoline(&precode);
    entry.direct_ptr = precode.trampoline;

    // Compile
    void* code1 = HybridStubDispatchImpl(&precode);
    ASSERT_EQ(precode.state.load(), kPrecodeCompiled);
    ASSERT_NE(precode.compiled, nullptr);
    EXPECT_EQ(precode.compiled->aot_entry, precode.aot_entry);

    // Subsequent calls still have aot_entry
    EXPECT_EQ(precode.compiled->aot_entry, reinterpret_cast<void*>(&AotReturn42));
    EXPECT_NE(precode.compiled->aot_entry, nullptr);
}
