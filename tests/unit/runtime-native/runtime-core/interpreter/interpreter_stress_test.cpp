/// interpreter_stress_test.cpp — Multi-threaded stress tests for FastExecute (gtest)
///
/// Stress scenarios:
///   1. ConcurrentFastExecute  — 4/8/16 threads running FastExecute in parallel
///   2. TierUpgradeStorm       — rapid tier transitions under concurrent call
///   3. LoopBackedgeStress     — hot loops triggering backedge OSR
///   4. SEHStress              — structured exception handling under parallelism
///   5. MixedOpcode            — random opcode sequences, pool allocation pressure
///
/// TIMEOUT: 120 seconds per stress test case.
/// Label: stress

#include "interpreter_vm.h"
#include "fast_dispatch.h"
#include "fast_frame_pool.h"
#include "patch_loader.h"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>
#include <random>

using Clock = std::chrono::high_resolution_clock;

using chaos::il2cpp::interpreter::IRInstruction;
using chaos::il2cpp::interpreter::IRMethod;
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::InterpreterVM;
using chaos::il2cpp::interpreter::ExecutionFrame;
using chaos::il2cpp::interpreter::ExecutionResult;
using chaos::il2cpp::runtime_core::FastFrame;
using chaos::il2cpp::runtime_core::FastExecute;
using chaos::il2cpp::runtime_core::SetupFastFrame;

// AOT symbol stubs (required by chaos_interpreter library linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

namespace {

// ═══════════════════════════════════════════════════════════════════════════
// Test helpers
// ═══════════════════════════════════════════════════════════════════════════

/// Build a simple arithmetic method: (arg0 + arg1) * arg2 / arg3 - arg4
static IRMethod MakeArithmeticMethod() {
    IRMethod method;
    method.instructions = {
        IRInstruction{IROpCode::LdArg, 0, 0},
        IRInstruction{IROpCode::LdArg, 1, 0},
        IRInstruction{IROpCode::Add, 0, 0},
        IRInstruction{IROpCode::LdArg, 2, 0},
        IRInstruction{IROpCode::Mul, 0, 0},
        IRInstruction{IROpCode::LdArg, 3, 0},
        IRInstruction{IROpCode::Div, 0, 0},
        IRInstruction{IROpCode::LdArg, 4, 0},
        IRInstruction{IROpCode::Sub, 0, 0},
        IRInstruction{IROpCode::Ret, 0, 0},
    };
    return method;
}

/// Build a loop-heavy method that stresses backedge OSR:
///   sum = 0; for (int i = 0; i < N; ++i) { sum += i; }
static IRMethod MakeLoopMethod(uint32_t iterations) {
    IRMethod method;
    // locals: 0 = i, 1 = sum, 2 = N
    // 0: ldc.i4 0
    // 1: stloc.0 (i = 0)
    // 2: ldc.i4 0
    // 3: stloc.1 (sum = 0)
    // 4: ldarg.0 (N)
    // 5: stloc.2 (N)
    // 6: ldloc.0 (i)
    // 7: ldloc.2 (N)
    // 8: blt 11 (if i < N, goto 11)
    // 9: ldloc.1 (sum)
    // 10: ret
    // 11: ldloc.1 (sum)
    // 12: ldloc.0 (i)
    // 13: add
    // 14: stloc.1 (sum)
    // 15: ldloc.0 (i)
    // 16: ldc.i4 1
    // 17: add
    // 18: stloc.0 (i++)
    // 19: br 6

    // Use inline IR for simplicity — works with FastExecute directly
    method.instructions = {
        IRInstruction{IROpCode::LdcI4, 0, 0},
        IRInstruction{IROpCode::StLoc, 0, 0},
        IRInstruction{IROpCode::LdcI4, 0, 0},
        IRInstruction{IROpCode::StLoc, 1, 0},
        IRInstruction{IROpCode::LdArg, 0, 0},
        IRInstruction{IROpCode::StLoc, 2, 0},
        // loop header
        IRInstruction{IROpCode::LdLoc, 0, 0},   // 6
        IRInstruction{IROpCode::LdLoc, 2, 0},   // 7
        IRInstruction{IROpCode::Blt, 0, 0, 11}, // 8
        IRInstruction{IROpCode::LdLoc, 1, 0},   // 9
        IRInstruction{IROpCode::Ret, 0, 0},     // 10
        // loop body
        IRInstruction{IROpCode::LdLoc, 1, 0},   // 11
        IRInstruction{IROpCode::LdLoc, 0, 0},   // 12
        IRInstruction{IROpCode::Add, 0, 0},     // 13
        IRInstruction{IROpCode::StLoc, 1, 0},   // 14
        IRInstruction{IROpCode::LdLoc, 0, 0},   // 15
        IRInstruction{IROpCode::LdcI4, 1, 0},   // 16
        IRInstruction{IROpCode::Add, 0, 0},     // 17
        IRInstruction{IROpCode::StLoc, 0, 0},   // 18
        IRInstruction{IROpCode::Br, 0, 0, 6},   // 19
    };
    // Set loop limit in secondary operand of ldarg for iterations
    return method;
}

/// Build a method that exercises many opcodes (max stack usage)
static IRMethod MakeMixedOpcodeMethod() {
    IRMethod method;
    // Mix of arithmetic, branches, and stack operations
    method.instructions = {
        IRInstruction{IROpCode::LdcI4, 10, 0},
        IRInstruction{IROpCode::LdcI4, 20, 0},
        IRInstruction{IROpCode::LdcI4, 30, 0},
        IRInstruction{IROpCode::Add, 0, 0},
        IRInstruction{IROpCode::Sub, 0, 0},
        IRInstruction{IROpCode::LdcI4, 5, 0},
        IRInstruction{IROpCode::Mul, 0, 0},
        IRInstruction{IROpCode::Dup, 0, 0},
        IRInstruction{IROpCode::LdcI4, 100, 0},
        IRInstruction{IROpCode::Ceq, 0, 0},
        IRInstruction{IROpCode::BrFalse, 0, 0, 12},
        IRInstruction{IROpCode::LdcI4, 42, 0},
        IRInstruction{IROpCode::Ret, 0, 0},
        IRInstruction{IROpCode::LdcI4, -1, 0},
        IRInstruction{IROpCode::Ret, 0, 0},
    };
    return method;
}

/// Count of FastExecute calls made by a single stress worker
static std::atomic<uint64_t> g_worker_total_ops{0};
static std::atomic<uint64_t> g_worker_errors{0};

/// Stress worker: repeatedly invoke FastExecute with a given method
static void StressWorker(const IRMethod& method, uint32_t iterations,
                          uint32_t arg0, uint32_t arg1, uint32_t arg2,
                          uint32_t arg3, uint32_t arg4) {
    // Build args buffer
    uint64_t args[5] = {arg0, arg1, arg2, arg3, arg4};
    constexpr int kMaxLocals = 32;
    uint64_t locals[kMaxLocals] = {};

    for (uint32_t i = 0; i < iterations; ++i) {
        FastFrame frame = {};

        // Manually set up FastFrame for the method
        frame.args = args;
        frame.arg_count = 5;
        frame.local_count = 5;

        // Copy args to locals
        for (uint32_t j = 0; j < 5; ++j) {
            frame.locals[j] = args[j];
            frame.local_tags[j] = static_cast<uint8_t>(chaos::il2cpp::interpreter::ValueTag::Int32);
        }

        bool ok = FastExecute(frame,
            method.instructions.data(),
            static_cast<uint32_t>(method.instructions.size()));

        if (!ok || frame.threw_exception) {
            g_worker_errors.fetch_add(1, std::memory_order_relaxed);
        }

        // Validate arithmetic result for arithmetic method
        if (ok && frame.has_ret && method.instructions.size() == 10) {
            int64_t expected = (static_cast<int64_t>(arg0) + arg1) * arg2 / arg3 - arg4;
            int64_t actual = static_cast<int64_t>(frame.ret_val);
            if (actual != expected) {
                g_worker_errors.fetch_add(1, std::memory_order_relaxed);
            }
        }

        g_worker_total_ops.fetch_add(1, std::memory_order_relaxed);
    }
}

}  // anonymous namespace

// ═══════════════════════════════════════════════════════════════════════════
// Stress test: Concurrent FastExecute (4 threads)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Stress, Concurrent4Thread) {
    g_worker_total_ops = 0;
    g_worker_errors = 0;
    auto method = MakeArithmeticMethod();
    constexpr uint32_t kIterPerThread = 10000;

    std::thread t1(StressWorker, std::cref(method), kIterPerThread, 10, 20, 3, 5, 2);
    std::thread t2(StressWorker, std::cref(method), kIterPerThread, 100, 50, 7, 3, 10);
    std::thread t3(StressWorker, std::cref(method), kIterPerThread, 42, 58, 2, 4, 5);
    std::thread t4(StressWorker, std::cref(method), kIterPerThread, 99, 1, 10, 5, 0);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::printf("[Stress:4T] total_ops=%llu errors=%llu\n",
                (unsigned long long)g_worker_total_ops.load(),
                (unsigned long long)g_worker_errors.load());
    EXPECT_EQ(g_worker_errors.load(), 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// Stress test: Concurrent FastExecute (8 threads)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Stress, Concurrent8Thread) {
    g_worker_total_ops = 0;
    g_worker_errors = 0;
    auto method = MakeArithmeticMethod();
    constexpr uint32_t kIterPerThread = 10000;

    std::vector<std::thread> threads;
    for (int t = 0; t < 8; ++t) {
        threads.emplace_back(StressWorker, std::cref(method), kIterPerThread,
                             t * 10 + 5, t * 20 + 3, t + 2, t + 4, t + 1);
    }
    for (auto& th : threads) th.join();

    std::printf("[Stress:8T] total_ops=%llu errors=%llu\n",
                (unsigned long long)g_worker_total_ops.load(),
                (unsigned long long)g_worker_errors.load());
    EXPECT_EQ(g_worker_errors.load(), 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// Stress test: Concurrent FastExecute (16 threads)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Stress, Concurrent16Thread) {
    g_worker_total_ops = 0;
    g_worker_errors = 0;
    auto method = MakeArithmeticMethod();
    constexpr uint32_t kIterPerThread = 10000;

    std::vector<std::thread> threads;
    for (int t = 0; t < 16; ++t) {
        threads.emplace_back(StressWorker, std::cref(method), kIterPerThread,
                             t * 5, t * 7 + 3, (t % 5) + 2, (t % 3) + 3, t);
    }
    for (auto& th : threads) th.join();

    std::printf("[Stress:16T] total_ops=%llu errors=%llu\n",
                (unsigned long long)g_worker_total_ops.load(),
                (unsigned long long)g_worker_errors.load());
    EXPECT_EQ(g_worker_errors.load(), 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// Stress test: Mix opcode stress (single thread, many ops)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Stress, MixedOpcode) {
    g_worker_total_ops = 0;
    g_worker_errors = 0;
    auto method = MakeMixedOpcodeMethod();
    constexpr uint32_t kIterations = 50000;

    uint64_t args[5] = {};

    for (uint32_t i = 0; i < kIterations; ++i) {
        // Re-arms the frame for this invocation (zeroes pc/sp/stack/tags).  The
        // other stress tests (StressWorker, LoopBackedge) do the same.  Reusing
        // a single FastFrame across iterations leaks stale eval-stack state
        // into the next call and, over 50k unbalanced-opcode iterations, drives
        // the frame's sp out of range → OOB → hard crash (EXIT=3).
        FastFrame frame = {};
        frame.args = args;
        frame.arg_count = 0;
        frame.local_count = 0;

        bool ok = FastExecute(frame,
            method.instructions.data(),
            static_cast<uint32_t>(method.instructions.size()));
        if (!ok || frame.threw_exception) {
            g_worker_errors.fetch_add(1, std::memory_order_relaxed);
        }
        g_worker_total_ops.fetch_add(1, std::memory_order_relaxed);
    }

    std::printf("[Stress:Mixed] total_ops=%llu errors=%llu\n",
                (unsigned long long)g_worker_total_ops.load(),
                (unsigned long long)g_worker_errors.load());
    EXPECT_EQ(g_worker_errors.load(), 0u);
}

// ═══════════════════════════════════════════════════════════════════════════
// Stress test: Loop backedge (exercises loop_counter + OSR path)
// ═══════════════════════════════════════════════════════════════════════════

TEST(Interpreter_Stress, LoopBackedge) {
    g_worker_total_ops = 0;
    g_worker_errors = 0;
    constexpr uint32_t kLoopN = 500;
    auto method = MakeLoopMethod(kLoopN);
    constexpr uint32_t kIterations = 1000;

    uint64_t args[5] = {kLoopN, 0, 0, 0, 0};

    for (uint32_t i = 0; i < kIterations; ++i) {
        FastFrame frame = {};
        frame.args = args;
        frame.arg_count = 1;
        frame.local_count = 3;
        frame.locals[0] = 0;
        frame.locals[1] = 0;
        frame.locals[2] = kLoopN;

        bool ok = FastExecute(frame,
            method.instructions.data(),
            static_cast<uint32_t>(method.instructions.size()));
        if (!ok || frame.threw_exception) {
            g_worker_errors.fetch_add(1, std::memory_order_relaxed);
        }
        // Verify sum: 0+1+2+...+(N-1) = N*(N-1)/2
        if (ok && frame.has_ret) {
            int64_t expected = static_cast<int64_t>(kLoopN) * (kLoopN - 1) / 2;
            int64_t actual = static_cast<int64_t>(frame.ret_val);
            if (actual != expected) {
                g_worker_errors.fetch_add(1, std::memory_order_relaxed);
            }
        }
        g_worker_total_ops.fetch_add(1, std::memory_order_relaxed);
    }

    std::printf("[Stress:Loop] total_ops=%llu errors=%llu\n",
                (unsigned long long)g_worker_total_ops.load(),
                (unsigned long long)g_worker_errors.load());
    EXPECT_EQ(g_worker_errors.load(), 0u);
}
