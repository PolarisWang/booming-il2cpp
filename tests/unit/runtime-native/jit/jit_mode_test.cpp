// ── JIT mode end-to-end tests ─────────────────────────────────────────
//
// Tests the T4 JIT dispatch pipeline end-to-end:
//   1. JitPrecode state machine (Uncompiled → Compiling → Compiled)
//   2. JitStubDispatchImpl compiles and returns executable code
//   3. direct_ptr patching after compilation
//   4. PrecodeArena trampoline allocation and layout
//
// These tests exercise the same dispatch path used by RegisterJitMethods
// at startup, without requiring the full HotpatchNameRegistry bootstrap.

#include <gtest/gtest.h>

#include "jit_precode.h"
#include "jit_engine.h"
#include "jit_method.h"
#include "ir_reg_alloc.h"

#include <codegen_bridge.h>

#include <cstdint>
#include <cstring>
#include <atomic>
#include <chrono>
#include <thread>
#if defined(_WIN64)
// T4 entries use RBX=args/RSI=ret (JIT conv); t4_jit_call.asm re-binds them.
extern "C" void T4CallNative(const void* entry, void* args, void* ret);
#endif


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
using chaos::il2cpp::jit::PrecodeArena;
using chaos::il2cpp::jit::JitStubDispatchImpl;
using chaos::il2cpp::jit::kPrecodeUncompiled;
using chaos::il2cpp::jit::kPrecodeCompiling;
using chaos::il2cpp::jit::kPrecodeCompiled;
using chaos::il2cpp::jit::CompileTier;

// ── Helpers (matching jit_il_smoke_test.cpp pattern) ────────────

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
#if defined(_WIN64)
    T4CallNative(entry, args_buf, ret_buf);
#else
    using NativeEntry = void (*)(void*, void*);
    auto native_entry = reinterpret_cast<NativeEntry>(entry);
    native_entry(args_buf, ret_buf);
#endif
    return ret_buf[0];
}

// ── Test fixture ──────────────────────────────────────────────────
struct JitModeTest : public ::testing::Test {
    // Create a simple method that returns a constant value.
    static RegisterMethod MakeReturnConstantMethod(int32_t value) {
        RegisterMethod method;
        method.max_regs = 1;
        method.instructions.push_back(InstrI4(IROpCode::LdcI4, value, 0, 0));
        method.instructions.push_back(InstrRet(0));
        return method;
    }

    // Create a method that adds two constants.
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

// ── JitStubDispatchImpl tests ────────────────────────────────────

TEST_F(JitModeTest, DispatchCompilesAndReturnsCode) {
    // Create a JitPrecode with a simple method
    JitPrecode precode;
    precode.ir = MakeReturnConstantMethod(42);
    precode.config = CompileConfig{};

    // Set up a fake HotpatchEntryV0 to receive the direct_ptr patch
    HotpatchEntryV0 entry{};
    precode.entry = &entry;

    // Call JitStubDispatchImpl — this should compile the method
    void* code = JitStubDispatchImpl(&precode);
    ASSERT_NE(code, nullptr);

    // State should be Compiled
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);

    // compiled should be non-null
    ASSERT_NE(precode.compiled, nullptr);

    // The code should be executable and return 42
    EXPECT_EQ(ExecuteNative(code), 42ULL);

    // direct_ptr should have been patched to the compiled code
    EXPECT_EQ(entry.direct_ptr, code);
}

TEST_F(JitModeTest, DispatchSecondCallReturnsSameCode) {
    JitPrecode precode;
    precode.ir = MakeAddMethod(10, 20);
    precode.config = CompileConfig{};

    HotpatchEntryV0 entry{};
    precode.entry = &entry;

    // First call — compiles
    void* code1 = JitStubDispatchImpl(&precode);
    ASSERT_NE(code1, nullptr);
    EXPECT_EQ(ExecuteNative(code1), 30ULL);

    // State should be Compiled now
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);

    // Second call — should return the same code immediately (no recompilation)
    void* code2 = JitStubDispatchImpl(&precode);
    ASSERT_EQ(code2, code1);
    EXPECT_EQ(ExecuteNative(code2), 30ULL);
}

TEST_F(JitModeTest, DispatchWithoutEntryStillCompiles) {
    // When entry is not set, JitStubDispatchImpl should still compile
    // the method and return valid code. direct_ptr patch is skipped.
    JitPrecode precode;
    precode.ir = MakeReturnConstantMethod(99);
    precode.config = CompileConfig{};
    precode.entry = nullptr;

    void* code = JitStubDispatchImpl(&precode);
    ASSERT_NE(code, nullptr);
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);
    EXPECT_NE(precode.compiled, nullptr);
}

TEST_F(JitModeTest, PrecodeStateMachine) {
    // Verify the full state machine transition with a valid entry
    JitPrecode precode;
    precode.ir = MakeReturnConstantMethod(7);
    precode.config = CompileConfig{};
    HotpatchEntryV0 entry{};
    precode.entry = &entry;

    EXPECT_EQ(precode.state.load(), kPrecodeUncompiled);

    // Call dispatch
    void* code = JitStubDispatchImpl(&precode);
    ASSERT_NE(code, nullptr);

    // State should now be Compiled
    EXPECT_EQ(precode.state.load(), kPrecodeCompiled);

    // Code should be executable and return the correct value
    EXPECT_EQ(ExecuteNative(code), 7ULL);

    // direct_ptr should have been patched
    EXPECT_EQ(entry.direct_ptr, code);
}

TEST_F(JitModeTest, PrecodeArenaAllocatesTrampoline) {
    PrecodeArena arena;

    // Create a precode and allocate a trampoline for it
    JitPrecode precode;
    precode.ir = MakeReturnConstantMethod(42);
    precode.config = CompileConfig{};

    void* trampoline = arena.AllocateJitTrampoline(&precode);
    ASSERT_NE(trampoline, nullptr);

    // The trampoline should be executable memory (RWX page)
    // Verify the first bytes match the expected trampoline encoding:
    //   x64:  mov r10, imm64   → 0x49 0xBA ...
    //   ARM64: LDR X17, [PC, #8] → LE: 0x51 0x00 0x80 0x58
    auto* bytes = static_cast<const uint8_t*>(trampoline);
#if defined(__aarch64__)
    EXPECT_EQ(bytes[0], 0x51);
    EXPECT_EQ(bytes[1], 0x00);
    EXPECT_EQ(bytes[2], 0x80);
    EXPECT_EQ(bytes[3], 0x58);
#else
    EXPECT_EQ(bytes[0], 0x49);
    EXPECT_EQ(bytes[1], 0xBA);
#endif
}

TEST_F(JitModeTest, PrecodeArenaMultipleTrampolines) {
    PrecodeArena arena;

    // Allocate trampolines for multiple methods
    JitPrecode precodes[5];
    void* trampolines[5];
    for (int i = 0; i < 5; i++) {
        precodes[i].ir = MakeReturnConstantMethod(i * 10);
        precodes[i].config = CompileConfig{};
        trampolines[i] = arena.AllocateJitTrampoline(&precodes[i]);
        ASSERT_NE(trampolines[i], nullptr);
    }

    // Verify the precode pointers are embedded in the trampolines
    for (int i = 0; i < 5; i++) {
        auto* bytes = static_cast<const uint8_t*>(trampolines[i]);
        // Check trampoline starts with correct arch-specific encoding
#if defined(__aarch64__)
        // ARM64: LDR X17, [PC, #8] → 0x51 0x00 0x80 0x58
        EXPECT_EQ(bytes[0], 0x51);
        EXPECT_EQ(bytes[1], 0x00);
        EXPECT_EQ(bytes[2], 0x80);
        EXPECT_EQ(bytes[3], 0x58);

        // Extract the precode address from the trampoline's literal pool at offset 8
        uintptr_t embedded_addr;
        std::memcpy(&embedded_addr, bytes + 8, sizeof(embedded_addr));
#else
        // x64: mov r10, imm64 → 0x49 0xBA ...
        EXPECT_EQ(bytes[0], 0x49);
        EXPECT_EQ(bytes[1], 0xBA);

        // Extract the precode address from the trampoline at offset 2
        uintptr_t embedded_addr;
        std::memcpy(&embedded_addr, bytes + 2, sizeof(embedded_addr));
#endif
        EXPECT_EQ(embedded_addr, reinterpret_cast<uintptr_t>(&precodes[i]));
    }
}

TEST_F(JitModeTest, FullDispatchThroughPrecodeArena) {
    PrecodeArena arena;

    // Set up precode + trampoline (same pattern as RegisterJitMethods)
    JitPrecode precode;
    precode.ir = MakeReturnConstantMethod(77);
    precode.config = CompileConfig{};

    HotpatchEntryV0 entry{};
    precode.entry = &entry;
    precode.trampoline = arena.AllocateJitTrampoline(&precode);
    ASSERT_NE(precode.trampoline, nullptr);

    // Set direct_ptr to the trampoline (first call goes through dispatch)
    entry.direct_ptr = precode.trampoline;

    // First call through dispatch — should compile
    void* code = JitStubDispatchImpl(&precode);
    ASSERT_NE(code, nullptr);
    EXPECT_EQ(ExecuteNative(code), 77ULL);

    // direct_ptr should now point to compiled code (patched by dispatch)
    EXPECT_EQ(entry.direct_ptr, code);
    EXPECT_NE(entry.direct_ptr, precode.trampoline);

    // Second call — should return compiled code directly
    void* code2 = JitStubDispatchImpl(&precode);
    EXPECT_EQ(code2, code);
    EXPECT_EQ(ExecuteNative(code2), 77ULL);
}

TEST_F(JitModeTest, MultiThreadedDispatchStress) {
    // Verify that concurrent calls to JitStubDispatchImpl work correctly
    // Only one thread should compile; others should spin-wait and get the same code.
    PrecodeArena arena;

    auto* precode = new JitPrecode();
    precode->ir = MakeReturnConstantMethod(123);
    precode->config = CompileConfig{};
    HotpatchEntryV0 entry{};
    precode->entry = &entry;
    precode->trampoline = arena.AllocateJitTrampoline(precode);
    entry.direct_ptr = precode->trampoline;

    constexpr int kNumThreads = 8;
    std::atomic<int> ready_count{0};
    std::atomic<bool> start_flag{false};
    void* results[kNumThreads] = {};

    auto thread_fn = [&](int tid) {
        ready_count.fetch_add(1, std::memory_order_release);
        // Spin until all threads are ready
        while (!start_flag.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        results[tid] = JitStubDispatchImpl(precode);
    };

    std::thread threads[kNumThreads];
    for (int i = 0; i < kNumThreads; i++) {
        threads[i] = std::thread(thread_fn, i);
    }

    // Wait for all threads to be ready
    while (ready_count.load(std::memory_order_acquire) < kNumThreads) {
        std::this_thread::yield();
    }

    // Signal all threads to start simultaneously
    start_flag.store(true, std::memory_order_release);

    // Join all threads
    for (auto& t : threads) {
        t.join();
    }

    // All threads should get the same (non-null) code pointer
    void* expected_code = results[0];
    ASSERT_NE(expected_code, nullptr);
    EXPECT_EQ(precode->state.load(), kPrecodeCompiled);
    EXPECT_EQ(entry.direct_ptr, expected_code);

    for (int i = 0; i < kNumThreads; i++) {
        EXPECT_EQ(results[i], expected_code)
            << "Thread " << i << " got different code pointer";
    }

    // Verify the code produces the correct result
    EXPECT_EQ(ExecuteNative(expected_code), 123ULL);

    delete precode;
}

// ── Tier 0 (Quick JIT) tests ─────────────────────────────────────

TEST_F(JitModeTest, Tier0CompilesAndReturnsCorrectCode) {
    // Tier 0 should produce executable code with minimal passes
    auto rm = MakeReturnConstantMethod(42);
    CompileConfig cfg;
    cfg.enable_register_caching = false;
    cfg.enable_optimizer = false;
    cfg.enable_liveness = false;
    cfg.enable_deopt = false;
    cfg.compile_tier = CompileTier::kQuick;

    auto* jm = Compile(rm, cfg);
    ASSERT_NE(jm, nullptr);
    ASSERT_NE(jm->code, nullptr);

    // Code should execute and return the correct value
    EXPECT_EQ(ExecuteNative(jm->code), 42ULL);

    // Tier 0 should NOT produce OSR entry, SEH table, or deopt metadata
    EXPECT_EQ(jm->osr_entry_offset, 0u);
    EXPECT_EQ(jm->seh_table_offset, 0u);
    EXPECT_EQ(jm->deopt_entry_count, 0u);

    // Tier 0 should have no GC slot map (conservative scanning only)
    EXPECT_EQ(jm->slot_map_data, nullptr);

    delete jm;
}

TEST_F(JitModeTest, Tier0CompilesAddMethod) {
    // Tier 0 with a binary operation should also produce correct results
    auto rm = MakeAddMethod(10, 20);
    CompileConfig cfg;
    cfg.enable_register_caching = false;
    cfg.enable_optimizer = false;
    cfg.enable_liveness = false;
    cfg.enable_deopt = false;
    cfg.compile_tier = CompileTier::kQuick;

    auto* jm = Compile(rm, cfg);
    ASSERT_NE(jm, nullptr);
    EXPECT_EQ(ExecuteNative(jm->code), 30ULL);
    delete jm;
}

TEST_F(JitModeTest, Tier0ReturnsNullptrForUnsupportedOpcodes) {
    // Tier 0 does not emit deopt sequences; unsupported opcodes return nullptr
    RegisterMethod rm;
    rm.max_regs = 1;
    // Use an opcode that doesn't exist / is not handled
    RegisterInstruction ri;
    ri.header = MakeHeader(static_cast<IROpCode>(0xFF), 0, 0, 0, 0);
    ri.imm.i4 = 0;
    rm.instructions.push_back(ri);

    CompileConfig cfg;
    cfg.enable_register_caching = false;
    cfg.enable_optimizer = false;
    cfg.enable_deopt = false;
    cfg.compile_tier = CompileTier::kQuick;

    auto* jm = Compile(rm, cfg);
    EXPECT_EQ(jm, nullptr);  // Should fail, not emit deopt
}

TEST_F(JitModeTest, Tier0CompileTimeIsFast) {
    // Verify Tier 0 compiles simple methods quickly (< 1ms per method)
    auto rm = MakeReturnConstantMethod(42);
    CompileConfig cfg;
    cfg.enable_register_caching = false;
    cfg.enable_optimizer = false;
    cfg.enable_liveness = false;
    cfg.enable_deopt = false;
    cfg.compile_tier = CompileTier::kQuick;

    auto start = std::chrono::high_resolution_clock::now();
    constexpr int kIterations = 100;
    for (int i = 0; i < kIterations; i++) {
        auto* jm = Compile(rm, cfg);
        ASSERT_NE(jm, nullptr);
        delete jm;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    auto avg_us = total_us / kIterations;

    // Average compile time should be well under 50µs for a 2-instr method.
    // On ARM64 QEMU, CPU-bound C++ runs ~100x slower due to TCG binary
    // translation. Real ARM64 hardware matches x64 performance (~1-3µs).
#if defined(__aarch64__)
    EXPECT_LT(avg_us, 500u) << "Tier 0 average compile time: " << avg_us << " µs";
#else
    EXPECT_LT(avg_us, 50u) << "Tier 0 average compile time: " << avg_us << " µs";
#endif
    std::printf("[  INFO  ] Tier 0 avg compile time: %llu µs over %d iterations\n",
                static_cast<unsigned long long>(avg_us), kIterations);
}
