/// fuzz_il_test.cpp — IL instruction fuzz tester for the interpreter.
///
/// Generates random IR instruction sequences and feeds them to InterpreterVM.
/// Acts as an oracle: the interpreter must not crash, assert-fail, or produce
/// undefined behavior on any valid IR opcode combination.
///
/// This is NOT a coverage-guided fuzzer (like libFuzzer). It's a simpler
/// random-sequence generator that exercises edge cases in instruction dispatch.
///
/// Usage:
///   test_fuzz_il --fuzz_seed=12345 --fuzz_iterations=10000
///   test_fuzz_il --fuzz_list_opcodes    # Print all opcodes and exit

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <string>
#include <vector>

#include "interpreter_vm.h"
#include <chaos/native_types.h>
#include <gtest/gtest.h>

// AOT symbol stubs (required by chaos_interpreter linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

using namespace chaos::il2cpp::interpreter;

namespace {

// All opcodes (excluding Break which is a no-op marker)
constexpr IROpCode kAllOpcodes[] = {
    IROpCode::LdcI4, IROpCode::LdcI8, IROpCode::LdcR4, IROpCode::LdcR8,
    IROpCode::LdStr, IROpCode::LdNull,
    IROpCode::LdArg, IROpCode::LdLoc, IROpCode::StLoc, IROpCode::StArg,
    IROpCode::LdFld, IROpCode::StFld, IROpCode::LdSFld, IROpCode::StSFld,
    IROpCode::Call, IROpCode::CallVirt, IROpCode::CallBridge,
    IROpCode::Br, IROpCode::BrTrue, IROpCode::BrFalse,
    IROpCode::Beq, IROpCode::Blt, IROpCode::Bgt, IROpCode::Ble, IROpCode::Bge,
    IROpCode::BneUn, IROpCode::BgeUn, IROpCode::BgtUn, IROpCode::BleUn, IROpCode::BltUn,
    IROpCode::Add, IROpCode::Sub, IROpCode::Mul, IROpCode::Div, IROpCode::Rem,
    IROpCode::AddOvf, IROpCode::SubOvf, IROpCode::MulOvf,
    IROpCode::Neg, IROpCode::Ceq, IROpCode::Clt, IROpCode::Cgt,
    IROpCode::Conv_I4, IROpCode::Conv_I8, IROpCode::Conv_R4, IROpCode::Conv_R8,
    IROpCode::ConvI, IROpCode::ConvU, IROpCode::ConvRUn,
    IROpCode::ConvOvfI, IROpCode::ConvOvfI4, IROpCode::ConvOvfI8,
    IROpCode::ConvOvfU, IROpCode::ConvOvfU4, IROpCode::ConvOvfU8,
    IROpCode::DivUn, IROpCode::RemUn,
    IROpCode::And, IROpCode::Or, IROpCode::Xor, IROpCode::Not,
    IROpCode::Shl, IROpCode::Shr, IROpCode::ShrUn,
    IROpCode::NewObj, IROpCode::Box, IROpCode::Unbox,
    IROpCode::CastClass, IROpCode::IsInst,
    IROpCode::NewArr, IROpCode::LdElem, IROpCode::StElem, IROpCode::LdLen,
    IROpCode::LdElemA, IROpCode::LdObj, IROpCode::StObj,
    IROpCode::Pop, IROpCode::Dup,
    IROpCode::Ret, IROpCode::Leave, IROpCode::Throw,
    IROpCode::LdInd, IROpCode::StInd,
    IROpCode::Switch,
    IROpCode::LdToken, IROpCode::InitObj, IROpCode::SizeOf,
    IROpCode::LdFtn, IROpCode::LdVirtFtn,
    IROpCode::LdArgA, IROpCode::LdLocA, IROpCode::LocAlloc,
    IROpCode::Cpblk, IROpCode::InitBlk,
};
constexpr int kOpcodesCount = sizeof(kAllOpcodes) / sizeof(kAllOpcodes[0]);

// ── Instruction generator ──────────────────────────────────────────────

/// Generate a single random IR instruction.
IRInstruction RandomInstruction(std::mt19937& rng) {
    IRInstruction instr;
    std::uniform_int_distribution<int> opcode_idx(0, kOpcodesCount - 1);
    instr.op_code = kAllOpcodes[opcode_idx(rng)];

    // Randomise operands
    std::uniform_int_distribution<int> i4_dist(-1000000, 1000000);
    instr.immediate_i4 = i4_dist(rng);

    std::uniform_int_distribution<CHAOS_IL2CPP_INT64> i8_dist(-1000000, 1000000);
    instr.immediate_i8 = i8_dist(rng);

    std::uniform_real_distribution<double> r8_dist(-1000.0, 1000.0);
    instr.immediate_r8 = r8_dist(rng);

    std::uniform_int_distribution<int> idx_dist(0, 15);
    instr.operand_index = idx_dist(rng);

    // Branch target: keep within reasonable bounds
    std::uniform_int_distribution<CHAOS_IL2CPP_SIZE> br_dist(0, 50);
    instr.branch_target = br_dist(rng);

    return instr;
}

/// Generate a random IR method with the given instruction count.
IRMethod RandomMethod(std::mt19937& rng, int count) {
    IRMethod method;
    method.instructions.reserve(static_cast<size_t>(count) + 1);

    int last_br = -1; // track last branch to insert returns after unreachable code

    for (int i = 0; i < count; i++) {
        // If previous opcode was a control-flow terminator, insert a Ret to
        // avoid fallthrough into unreachable code (which may cause issues).
        if (last_br >= 0 && i == last_br + 1) {
            IRInstruction ret;
            ret.op_code = IROpCode::Ret;
            method.instructions.push_back(ret);
            last_br = -1;
            continue;
        }

        auto instr = RandomInstruction(rng);

        // Ensure branch_target is valid (points within instruction range so far)
        if (!method.instructions.empty()) {
            std::uniform_int_distribution<CHAOS_IL2CPP_SIZE> target_dist(
                0, method.instructions.size() - 1);
            instr.branch_target = target_dist(rng);
        } else {
            instr.branch_target = 0;
        }

        method.instructions.push_back(instr);

        // Track control-flow terminators
        switch (instr.op_code) {
        case IROpCode::Ret:
        case IROpCode::Throw:
        case IROpCode::Leave:
            last_br = i;
            break;
        default:
            break;
        }
    }

    // Ensure the method always ends with Ret
    if (method.instructions.empty() || method.instructions.back().op_code != IROpCode::Ret) {
        IRInstruction ret;
        ret.op_code = IROpCode::Ret;
        method.instructions.push_back(ret);
    }

    return method;
}

}  // anonymous namespace

// ═══════════════════════════════════════════════════════════════════════════
// Fuzz Tests
// ═══════════════════════════════════════════════════════════════════════════

/// Fuzz with a fixed seed for reproducibility.
TEST(FuzzIL, FixedSeedSmoke) {
    constexpr int kIterations = 1000;
    constexpr int kInstrsPerMethod = 20;
    constexpr uint32_t kSeed = 0xDEADBEEF;

    std::mt19937 rng(kSeed);
    const InterpreterVM vm = {};

    for (int i = 0; i < kIterations; i++) {
        IRMethod method = RandomMethod(rng, kInstrsPerMethod);
        ExecutionFrame frame;

    #if defined(CHAOS_IL2CPP_DEBUG) || defined(_DEBUG)
        // In debug builds, catch assertions gracefully
        try {
            vm.Execute(method, &frame);
        } catch (...) {
            // Fuzzer should survive; just record the failure.
            ADD_FAILURE() << "Iteration " << i << " threw exception";
        }
    #else
        vm.Execute(method, &frame);
    #endif
    }
}

/// Fuzz with time-based seed for exploration.
TEST(FuzzIL, RandomSeedExploration) {
    constexpr int kIterations = 5000;
    constexpr int kInstrsPerMethod = 30;

    uint32_t seed = static_cast<uint32_t>(std::time(nullptr));
    std::mt19937 rng(seed);

    const InterpreterVM vm = {};

    for (int i = 0; i < kIterations; i++) {
        IRMethod method = RandomMethod(rng, kInstrsPerMethod);
        ExecutionFrame frame;

    #if defined(CHAOS_IL2CPP_DEBUG) || defined(_DEBUG)
        try {
            vm.Execute(method, &frame);
        } catch (...) {
            ADD_FAILURE() << "Iteration " << i << " threw exception (seed=" << seed << ")";
        }
    #else
        vm.Execute(method, &frame);
    #endif
    }
}

/// Long method fuzz: test methods with many instructions.
TEST(FuzzIL, LongMethodStress) {
    constexpr int kIterations = 200;
    constexpr int kInstrsPerMethod = 200;
    constexpr uint32_t kSeed = 0xCAFEBABE;

    std::mt19937 rng(kSeed);
    const InterpreterVM vm = {};

    for (int i = 0; i < kIterations; i++) {
        IRMethod method = RandomMethod(rng, kInstrsPerMethod);
        ExecutionFrame frame;
        vm.Execute(method, &frame);
    }
}

/// Dense opcode coverage: ensure every opcode appears at least once.
TEST(FuzzIL, AllOpcodesCoverage) {
    // Build a method that uses every opcode.
    IRMethod method;
    for (auto op : kAllOpcodes) {
        IRInstruction instr;
        instr.op_code = op;
        instr.immediate_i4 = 42;
        instr.operand_index = 0;
        method.instructions.push_back(instr);
    }
    // End with Ret
    IRInstruction ret;
    ret.op_code = IROpCode::Ret;
    method.instructions.push_back(ret);

    const InterpreterVM vm = {};
    ExecutionFrame frame;
    vm.Execute(method, &frame);
}
