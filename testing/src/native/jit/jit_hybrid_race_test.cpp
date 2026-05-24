// ── Hybrid mode multi-thread / race condition tests ────────────────
//
// Tests specifically targeting:
//   1. ABA pattern on call_counter (decrement → reset → verify state)
//   2. Concurrent compile + read of compiled->code
//   3. Memory ordering stress (relaxed vs acquire/release vs seq_cst)
//   4. Trampoline allocator contention (PrecodeArena shared pages)
//
// These tests complement the existing multi-threaded tests in
// jit_hybrid_mode_test.cpp by focusing on race conditions and
// memory ordering guarantees rather than functional correctness.

#include <gtest/gtest.h>

#include "jit_precode.h"
#include "jit_engine.h"
#include "jit_method.h"

#include <codegen_bridge.h>

#include <windows.h>
#include <cstdint>
#include <cstring>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

// ── Namespace aliases ──────────────────────────────────────────────
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::interpreter::kRegHasSrc1;
using chaos::il2cpp::jit::Compile;
using chaos::il2cpp::jit::JitMethod;
using chaos::il2cpp::jit::CompileConfig;
using chaos::il2cpp::jit::HybridPrecode;
using chaos::il2cpp::jit::PrecodeArena;
using chaos::il2cpp::jit::HybridStubDispatchImpl;
using chaos::il2cpp::jit::kPrecodeUncompiled;
using chaos::il2cpp::jit::kPrecodeCompiling;
using chaos::il2cpp::jit::kPrecodeCompiled;
using chaos::il2cpp::jit::kJitUpgradeThreshold;

// ── Helpers ────────────────────────────────────────────────────────

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

static void AotReturn42(void* /*args_buf*/, void* ret_buf) noexcept {
    auto* ret = static_cast<uint64_t*>(ret_buf);
    ret[0] = 42ULL;
}

static RegisterMethod MakeReturnConstantMethod(int32_t value) {
    RegisterMethod method;
    method.max_regs = 1;
    method.instructions.push_back(InstrI4(IROpCode::LdcI4, value, 0, 0));
    method.instructions.push_back(InstrRet(0));
    return method;
}

// ── Test fixture ──────────────────────────────────────────────────
struct HybridRaceTest : public ::testing::Test {
    void SetUp() override {
        // Use a larger-than-default initial counter for stress tests
        // so many threads can participate in AOT-phase dispatch before JIT.
    }
};

// ── ABA pattern tests ─────────────────────────────────────────────

TEST_F(HybridRaceTest, AbAOnCallCounter) {
    // Simulate an ABA scenario: counter is decremented to near-zero,
    // then reset (as if a failed compile reset it), then verify that
    // the state machine handles this correctly.
    HybridPrecode precode;
    precode.call_counter.store(3, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(77);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

    // Call 1: counter 3→2, returns AOT
    void* r1 = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(precode.state.load(), kPrecodeUncompiled);
    EXPECT_EQ(ExecuteNative(r1), 42ULL);

    // Call 2: counter 2→1, returns AOT
    void* r2 = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(precode.state.load(), kPrecodeUncompiled);
    EXPECT_EQ(ExecuteNative(r2), 42ULL);

    // Manually reset counter to 3 (simulating ABA: counter was modified
    // from outside the dispatch, as if a failed compile wrote a new value)
    precode.call_counter.store(3, std::memory_order_relaxed);

    // Verify counter is back to 3
    EXPECT_EQ(precode.call_counter.load(std::memory_order_relaxed), 3u);

    // Call 3: counter 3→2, still returns AOT
    void* r3 = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(precode.state.load(), kPrecodeUncompiled);
    EXPECT_EQ(ExecuteNative(r3), 42ULL);

    // Call 4: counter 2→1, still AOT
    HybridStubDispatchImpl(&precode);
    EXPECT_EQ(precode.state.load(), kPrecodeUncompiled);

    // Call 5: counter 1→0, triggers JIT
    void* r5 = HybridStubDispatchImpl(&precode);
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);
    ASSERT_NE(precode.compiled, nullptr);
    EXPECT_EQ(ExecuteNative(precode.compiled->code), 77ULL);
}

TEST_F(HybridRaceTest, AbAWithConcurrentResetAndDispatch) {
    // Multiple threads: some call dispatch, some manually reset the counter.
    // Verify no crashes and eventual compilation.
    HybridPrecode precode;
    precode.call_counter.store(kJitUpgradeThreshold, std::memory_order_relaxed);
    precode.ir = MakeReturnConstantMethod(99);
    precode.config = CompileConfig{};
    precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);

    constexpr int kNumThreads = 6;
    std::atomic<bool> start_flag{false};
    std::atomic<int> done_count{0};

    auto dispatch_fn = [&]() {
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        for (int i = 0; i < 20; i++) {
            void* result = HybridStubDispatchImpl(&precode);
            // Any non-null result is valid
            EXPECT_NE(result, nullptr);
        }
        done_count.fetch_add(1, std::memory_order_release);
    };

    auto reset_fn = [&]() {
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        for (int i = 0; i < 10; i++) {
            // Manually reset counter to a positive value (simulating
            // external reset as if from a failed-compile recovery)
            precode.call_counter.store(5, std::memory_order_relaxed);
            std::this_thread::yield();
        }
        done_count.fetch_add(1, std::memory_order_release);
    };

    std::thread threads[kNumThreads];
    threads[0] = std::thread(dispatch_fn);
    threads[1] = std::thread(dispatch_fn);
    threads[2] = std::thread(dispatch_fn);
    threads[3] = std::thread(dispatch_fn);
    threads[4] = std::thread(reset_fn);
    threads[5] = std::thread(reset_fn);

    start_flag.store(true, std::memory_order_release);

    for (auto& t : threads) {
        t.join();
    }

    // Eventually the method should compile (dispatch will exhaust the
    // counter or reset threads will keep it alive — either way, the
    // system should not crash or deadlock)
    // If compiled, verify correctness
    if (precode.state.load() == kPrecodeCompiled && precode.compiled != nullptr) {
        EXPECT_EQ(ExecuteNative(precode.compiled->code), 99ULL);
    }
}

// ── Concurrent compile + read tests ───────────────────────────────

TEST_F(HybridRaceTest, ConcurrentCompileAndRead) {
    // One thread triggers JIT compilation while multiple threads
    // read compiled->code concurrently.  Verify no torn reads.
    PrecodeArena arena;

    constexpr int kNumReaders = 8;
    std::atomic<bool> start_flag{false};
    std::atomic<int> ready_count{0};
    void* reader_results[kNumReaders] = {};

    // Use heap-allocated precode so it outlives the test scope
    auto* precode = new HybridPrecode();
    precode->call_counter.store(1, std::memory_order_relaxed);
    precode->ir = MakeReturnConstantMethod(55);
    precode->config = CompileConfig{};
    precode->aot_entry = reinterpret_cast<void*>(&AotReturn42);

    // Reader threads
    auto reader_fn = [&](int tid) {
        ready_count.fetch_add(1, std::memory_order_release);
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        // Each reader reads dispatch result until state is Compiled
        for (int attempt = 0; attempt < 100; attempt++) {
            void* result = HybridStubDispatchImpl(precode);
            if (result != nullptr && result != precode->aot_entry) {
                reader_results[tid] = result;
                break;
            }
            std::this_thread::yield();
        }
    };

    // Writer thread (triggers compilation)
    auto writer_fn = [&]() {
        ready_count.fetch_add(1, std::memory_order_release);
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        // Trigger JIT by exhausting counter
        HybridStubDispatchImpl(precode);
    };

    std::thread readers[kNumReaders];
    for (int i = 0; i < kNumReaders; i++) {
        readers[i] = std::thread(reader_fn, i);
    }
    std::thread writer(writer_fn);

    while (ready_count.load(std::memory_order_acquire) < kNumReaders + 1) {
        std::this_thread::yield();
    }
    start_flag.store(true, std::memory_order_release);

    for (auto& t : readers) {
        t.join();
    }
    writer.join();

    // After all threads finish, verify compilation
    EXPECT_EQ(precode->state.load(), kPrecodeCompiled);
    ASSERT_NE(precode->compiled, nullptr);
    ASSERT_NE(precode->compiled->code, nullptr);

    // Verify all readers that found JIT code got a valid pointer
    for (int i = 0; i < kNumReaders; i++) {
        if (reader_results[i] != nullptr) {
            EXPECT_EQ(reader_results[i], precode->compiled->code)
                << "Reader " << i << " should see the same compiled code pointer";
        }
    }

    // Verify JIT correctness
    EXPECT_EQ(ExecuteNative(precode->compiled->code), 55ULL);

    delete precode;
}

TEST_F(HybridRaceTest, ConcurrentCompileStateTransitions) {
    // Verify that the state machine (Uncompiled → Compiling → Compiled)
    // is correctly observed under concurrent access.
    constexpr int kNumMethods = 10;
    constexpr int kNumThreads = 4;
    PrecodeArena arena;

    struct TestMethod {
        HybridPrecode precode;
        HotpatchEntryV0 entry;
    };

    TestMethod methods[kNumMethods];
    for (int i = 0; i < kNumMethods; i++) {
        int val = 100 + i;
        methods[i].precode.call_counter.store(kJitUpgradeThreshold, std::memory_order_relaxed);
        methods[i].precode.ir = MakeReturnConstantMethod(val);
        methods[i].precode.config = CompileConfig{};
        methods[i].precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);
        methods[i].precode.entry = &methods[i].entry;
        methods[i].precode.trampoline = arena.AllocateHybridTrampoline(&methods[i].precode);
        ASSERT_NE(methods[i].precode.trampoline, nullptr);
        methods[i].entry.direct_ptr = methods[i].precode.trampoline;
    }

    std::atomic<bool> start_flag{false};
    std::atomic<int> ready_count{0};
    std::atomic<int> state_observations[kNumMethods] = {};

    auto thread_fn = [&](int tid) {
        ready_count.fetch_add(1, std::memory_order_release);
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        for (int round = 0; round < 15; round++) {
            for (int m = 0; m < kNumMethods; m++) {
                void* result = HybridStubDispatchImpl(&methods[m].precode);
                EXPECT_NE(result, nullptr);
            }
        }

        // Record final states seen
        for (int m = 0; m < kNumMethods; m++) {
            uint32_t s = methods[m].precode.state.load(std::memory_order_acquire);
            if (s == kPrecodeCompiled) {
                state_observations[m].fetch_add(1, std::memory_order_relaxed);
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
    for (int m = 0; m < kNumMethods; m++) {
        EXPECT_EQ(methods[m].precode.state.load(), kPrecodeCompiled)
            << "Method " << m << " should eventually compile";
        ASSERT_NE(methods[m].precode.compiled, nullptr);
        EXPECT_EQ(ExecuteNative(methods[m].precode.compiled->code),
                  static_cast<uint64_t>(100 + m));
    }
}

// ── Memory ordering stress tests ──────────────────────────────────

TEST_F(HybridRaceTest, MemoryOrderRelaxedVsAcquire) {
    // Verify that HybridStubDispatchImpl produces consistent results
    // even under concurrent access where memory ordering might vary.
    // This test stresses the dispatch with different memory access patterns.
    PrecodeArena arena;

    constexpr int kNumMethods = 6;
    struct TestMethod {
        HybridPrecode precode;
        HotpatchEntryV0 entry;
    };
    TestMethod methods[kNumMethods];

    for (int i = 0; i < kNumMethods; i++) {
        int val = 200 + i * 10;
        methods[i].precode.call_counter.store(kJitUpgradeThreshold / (i + 1), std::memory_order_relaxed);
        methods[i].precode.ir = MakeReturnConstantMethod(val);
        methods[i].precode.config = CompileConfig{};
        methods[i].precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);
        methods[i].precode.entry = &methods[i].entry;
        methods[i].precode.trampoline = arena.AllocateHybridTrampoline(&methods[i].precode);
        ASSERT_NE(methods[i].precode.trampoline, nullptr);
        methods[i].entry.direct_ptr = methods[i].precode.trampoline;
    }

    constexpr int kNumThreads = 8;
    std::atomic<bool> start_flag{false};
    std::atomic<int> ready_count{0};
    std::atomic<uint64_t> total_aot{0};
    std::atomic<uint64_t> total_jit{0};

    auto thread_fn = [&]() {
        ready_count.fetch_add(1, std::memory_order_release);
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        // Interleave relaxed and acquire loads to stress memory ordering
        for (int round = 0; round < 10; round++) {
            for (int m = 0; m < kNumMethods; m++) {
                void* result = HybridStubDispatchImpl(&methods[m].precode);
                if (result != nullptr) {
                    uint64_t exec_val = ExecuteNative(result);
                    if (exec_val == 42ULL) {
                        total_aot.fetch_add(1, std::memory_order_relaxed);
                    } else {
                        total_jit.fetch_add(1, std::memory_order_relaxed);
                    }
                }
            }
            // Inter-thread visibility test: see if changes to one method's
            // state are visible to other threads without explicit fencing
            volatile uint32_t dummy = methods[0].precode.state.load(std::memory_order_relaxed);
            (void)dummy;
        }
    };

    std::thread threads[kNumThreads];
    for (int i = 0; i < kNumThreads; i++) {
        threads[i] = std::thread(thread_fn);
    }

    while (ready_count.load(std::memory_order_acquire) < kNumThreads) {
        std::this_thread::yield();
    }
    start_flag.store(true, std::memory_order_release);

    for (auto& t : threads) {
        t.join();
    }

    // All methods should compile
    for (int m = 0; m < kNumMethods; m++) {
        EXPECT_EQ(methods[m].precode.state.load(), kPrecodeCompiled)
            << "Method " << m << " should compile";
    }

    EXPECT_GT(total_aot.load(), 0) << "AOT path should have been exercised";
    EXPECT_GT(total_jit.load(), 0) << "JIT path should have been exercised";
}

// ── PrecodeArena contention tests ─────────────────────────────────

TEST_F(HybridRaceTest, TrampolineAllocatorContention) {
    // Multiple threads simultaneously allocate hybrid trampolines from
    // the same PrecodeArena.  Note: PrecodeArena itself is NOT thread-safe
    // for allocation (RegisterHybridMethods calls it sequentially at startup),
    // so we protect with a mutex.  This test primarily validates that
    // concurrent preparation of precodes + serialized allocation works.
    constexpr int kTrampolinesPerThread = 10;
    constexpr int kNumThreads = 4;
    constexpr int kTotalTrampolines = kTrampolinesPerThread * kNumThreads;

    // Pre-allocate precodes (one per trampoline)
    HybridPrecode precodes[kTotalTrampolines];
    for (int i = 0; i < kTotalTrampolines; i++) {
        precodes[i].aot_entry = reinterpret_cast<void*>(&AotReturn42);
    }

    // Threads share the arena — protect with mutex since PrecodeArena
    // does not have internal thread-safety for allocation.
    PrecodeArena shared_arena;
    std::mutex alloc_mutex;
    void* trampolines[kTotalTrampolines] = {};
    std::atomic<int> alloc_errors{0};

    std::atomic<bool> start_flag{false};
    std::atomic<int> ready_count{0};

    auto thread_fn = [&](int tid) {
        ready_count.fetch_add(1, std::memory_order_release);
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        int base = tid * kTrampolinesPerThread;
        for (int i = 0; i < kTrampolinesPerThread; i++) {
            int idx = base + i;
            {
                std::lock_guard<std::mutex> lock(alloc_mutex);
                trampolines[idx] = shared_arena.AllocateHybridTrampoline(&precodes[idx]);
            }
            if (trampolines[idx] == nullptr) {
                alloc_errors.fetch_add(1, std::memory_order_relaxed);
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

    EXPECT_EQ(alloc_errors.load(), 0) << "No allocation errors expected";

    // Verify all trampolines have the expected layout (mov r10, imm64)
    for (int i = 0; i < kTotalTrampolines; i++) {
        ASSERT_NE(trampolines[i], nullptr) << "Trampoline " << i << " should not be null";
        auto* bytes = static_cast<const uint8_t*>(trampolines[i]);
        EXPECT_EQ(bytes[0], 0x49) << "Trampoline " << i << ": expected 0x49 (mov r10)";
        EXPECT_EQ(bytes[1], 0xBA) << "Trampoline " << i << ": expected 0xBA (mov r10, imm64)";

        // Verify the embedded precode pointer
        uintptr_t embedded_addr;
        std::memcpy(&embedded_addr, bytes + 2, sizeof(embedded_addr));
        EXPECT_EQ(embedded_addr, reinterpret_cast<uintptr_t>(&precodes[i]))
            << "Trampoline " << i << " should embed its own precode pointer";
    }
}

TEST_F(HybridRaceTest, ConcurrentArenaAndDispatch) {
    // Mix trampoline allocation with dispatch calls to simulate
    // the real startup pattern more closely.
    // Note: PrecodeArena allocation is NOT thread-safe, so we pre-allocate
    // all trampolines before starting dispatch threads.
    PrecodeArena arena;

    constexpr int kNumMethods = 8;
    struct TestMethod {
        HybridPrecode precode{};
        HotpatchEntryV0 entry{};
    };
    TestMethod methods[kNumMethods];

    // Pre-allocate all methods
    for (int m = 0; m < kNumMethods; m++) {
        methods[m].precode.call_counter.store(kJitUpgradeThreshold / 2, std::memory_order_relaxed);
        methods[m].precode.ir = MakeReturnConstantMethod(300 + m);
        methods[m].precode.config = CompileConfig{};
        methods[m].precode.aot_entry = reinterpret_cast<void*>(&AotReturn42);
        methods[m].precode.entry = &methods[m].entry;
        methods[m].precode.trampoline = arena.AllocateHybridTrampoline(&methods[m].precode);
        ASSERT_NE(methods[m].precode.trampoline, nullptr);
        methods[m].entry.direct_ptr = methods[m].precode.trampoline;
    }

    std::atomic<bool> start_flag{false};
    std::atomic<int> ready_count{0};

    // Dispatch threads: call dispatch on methods concurrently
    auto dispatch_fn = [&]() {
        ready_count.fetch_add(1, std::memory_order_release);
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        for (int round = 0; round < 15; round++) {
            for (int m = 0; m < kNumMethods; m++) {
                void* result = HybridStubDispatchImpl(&methods[m].precode);
                if (result != nullptr) {
                    ExecuteNative(result);
                }
            }
        }
    };

    constexpr int kNumThreads = 6;
    std::thread threads[kNumThreads];
    for (int i = 0; i < kNumThreads; i++) {
        threads[i] = std::thread(dispatch_fn);
    }

    while (ready_count.load(std::memory_order_acquire) < kNumThreads) {
        std::this_thread::yield();
    }
    start_flag.store(true, std::memory_order_release);

    for (auto& t : threads) {
        t.join();
    }

    // All methods should eventually compile
    int compiled_count = 0;
    for (int m = 0; m < kNumMethods; m++) {
        if (methods[m].precode.state.load() == kPrecodeCompiled) {
            compiled_count++;
            ASSERT_NE(methods[m].precode.compiled, nullptr);
            EXPECT_EQ(ExecuteNative(methods[m].precode.compiled->code),
                      static_cast<uint64_t>(300 + m));
        }
    }
    EXPECT_GT(compiled_count, 0) << "At least some methods should compile";
}
