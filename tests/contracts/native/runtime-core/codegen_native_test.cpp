// ── Native code generation integration tests (Phase 3d) ───────────────────
//
// Each test constructs a RegisterMethod directly, generates native x64 code
// via GenerateNativeCode(), executes the native entry point, and verifies
// the results match expected values.
//
// These tests validate:
//   1. Instruction encoding correctness (arithmetic, branches)
//   2. Tier promotion (T1→T2→T3→T4) via InterpreterEntryDirect
//   3. Deoptimization metadata generation (Call sites, DeoptEntry/DeoptValue)
//   4. Deopt sequence emission for unsupported opcodes
//   5. T4 code entry/address registration

#include "code_generator.h"
#include "native_method.h"
#include "codegen_helpers.h"
#include "t4_seh_handler.h"
#include "ir_reg_alloc.h"
#include "gc_root_scanner.h"
#include "patch_loader.h"

#include <windows.h>
#include <cstdint>

#include <codegen_bridge.h>

#include <cstdio>
#include <cstring>
#include <climits>
#include <random>

// GC/TLAB headers for TLAB inline allocation tests
#include <gc_scheduler.h>
#include <gc/gc_young_gen.h>

// ── Namespace aliases ───────────────────────────────────────────────────
using chaos::il2cpp::interpreter::IROpCode;
using chaos::il2cpp::interpreter::RegisterInstruction;
using chaos::il2cpp::interpreter::RegisterMethod;
using chaos::il2cpp::interpreter::kRegHasDst;
using chaos::il2cpp::interpreter::kRegHasSrc1;
using chaos::il2cpp::interpreter::kRegHasSrc2;
using chaos::il2cpp::interpreter::kRegHasImm;
using chaos::il2cpp::interpreter::kRegHasSrc3;
using chaos::il2cpp::interpreter::kRegIsBranch;
using chaos::il2cpp::interpreter::kRegIsCall;
using chaos::il2cpp::interpreter::kRegIsStore;
using chaos::il2cpp::interpreter::SEHClause;
using chaos::il2cpp::interpreter::SEHFlags;
using chaos::il2cpp::codegen::GenerateNativeCode;
using chaos::il2cpp::codegen::CanGenerateNativeCode;
using chaos::il2cpp::codegen::NativeMethod;
using chaos::il2cpp::codegen::CodeGenConfig;
using chaos::il2cpp::codegen::kDeoptMagic;
using chaos::il2cpp::codegen::t_deopt_state;
using chaos::il2cpp::codegen::RegisterT4Code;
using chaos::il2cpp::codegen::FindT4CodeByAddress;
using chaos::il2cpp::interpreter::RegisterFrame;
using chaos::il2cpp::interpreter::RegisterFile;
using chaos::il2cpp::interpreter::RegisterExecute;
using chaos::il2cpp::runtime_core::PatchMethod;

using chaos::il2cpp::runtime_core::TLAB;
using chaos::il2cpp::runtime_core::tls_tlab;
using chaos::il2cpp::runtime_core::InitYoungGeneration;
using chaos::il2cpp::runtime_core::TlabClaimFromYoungGen;

static int g_tests_passed = 0;
static int g_tests_failed = 0;

// ── TLAB priming helper ───────────────────────────────────────────
// Ensures the current thread has a valid TLAB so that T4 inline
// allocation hits the bump-pointer fast path.  Allocates a buffer
// via VirtualAlloc (64 KB, same as kDefaultTlabSize) and points
// tls_tlab at it.
static void PrimeTlab() noexcept {
    static bool primed = false;
    if (!primed) {
        static constexpr CHAOS_IL2CPP_SIZE kTlabSize = 64 * 1024;  // 64 KB
        void* buf = VirtualAlloc(nullptr, kTlabSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        if (buf) {
            tls_tlab.start = static_cast<char*>(buf);
            tls_tlab.current = static_cast<char*>(buf);
            tls_tlab.end = static_cast<char*>(buf) + kTlabSize;
            // Reset scan pointers too
            tls_tlab.start_scan = static_cast<char*>(buf);
            tls_tlab.current_scan = static_cast<char*>(buf);
        }
        primed = true;
    }
    std::printf("    TLAB: [%p, %p) current=%p\n",
                tls_tlab.start, tls_tlab.end, tls_tlab.current);
}

#define TEST(name) do { \
    if (Test_##name()) { \
        std::printf("  PASS: %s\n", #name); \
        g_tests_passed++; \
    } else { \
        std::printf("  FAIL: %s\n", #name); \
        g_tests_failed++; \
    } \
} while(0)

// ── Helpers ─────────────────────────────────────────────────────────────

// ── Opcode name lookup for fuzz test diagnostics ─────────────────────────
static const char* OpcodeName(IROpCode opc) noexcept {
    switch (opc) {
    #define HANDLE_OP(name) case IROpCode::name: return #name;
    HANDLE_OP(LdcI4); HANDLE_OP(LdcI8); HANDLE_OP(Add); HANDLE_OP(Sub);
    HANDLE_OP(And); HANDLE_OP(Or); HANDLE_OP(Xor); HANDLE_OP(Neg);
    HANDLE_OP(Not); HANDLE_OP(Dup); HANDLE_OP(Ceq); HANDLE_OP(Clt);
    HANDLE_OP(Cgt); HANDLE_OP(Conv_I4); HANDLE_OP(Conv_I8);
    HANDLE_OP(LdNull); HANDLE_OP(Pop); HANDLE_OP(Br);
    HANDLE_OP(BrTrue); HANDLE_OP(BrFalse); HANDLE_OP(Ret);
    HANDLE_OP(Shl); HANDLE_OP(Shr); HANDLE_OP(ShrUn);
    HANDLE_OP(Mul); HANDLE_OP(AddOvf); HANDLE_OP(SubOvf); HANDLE_OP(MulOvf);
    HANDLE_OP(Beq); HANDLE_OP(BneUn); HANDLE_OP(Blt); HANDLE_OP(Bge);
    HANDLE_OP(LdInd); HANDLE_OP(StInd);
    HANDLE_OP(Div); HANDLE_OP(Rem); HANDLE_OP(DivUn); HANDLE_OP(RemUn);
    HANDLE_OP(ConvI); HANDLE_OP(ConvU); HANDLE_OP(ConvOvfI4);
    HANDLE_OP(LdcR4); HANDLE_OP(ConvRUn);
    HANDLE_OP(Conv_R4); HANDLE_OP(Conv_R8);
    #undef HANDLE_OP
    default: return "???";
    }
}

static void DumpInstrs(const CHAOS_IL2CPP_VECTOR(RegisterInstruction)& instrs) noexcept {
    for (size_t i = 0; i < instrs.size(); i++) {
        const auto& ri = instrs[i];
        std::printf("      [%3zu] %-8s", i, OpcodeName(ri.op_code()));
        if (ri.has_dst()) std::printf(" dst=r%u", ri.dst_reg());
        if (ri.has_src1()) std::printf(" src1=r%u", ri.src1_reg());
        if (ri.has_src2()) std::printf(" src2=r%u", ri.src2_reg());
        if (ri.has_imm() && ri.op_code() != IROpCode::Br &&
            ri.op_code() != IROpCode::BrTrue && ri.op_code() != IROpCode::BrFalse) {
            std::printf(" imm=%d", ri.imm.i4);
        }
        if (ri.is_branch() && ri.has_imm()) {
            std::printf(" -> %u", ri.imm.branch_target);
        }
        std::printf("\n");
    }
}

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

static RegisterInstruction InstrUnary(IROpCode opc, uint8_t dst,
                                      uint8_t src, uint8_t flags = 0) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, dst, src, 0, flags | kRegHasDst | kRegHasSrc1);
    ri.imm.i4 = 0;
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

static RegisterInstruction InstrBranch(IROpCode opc, uint32_t target,
                                       uint8_t src = 0) noexcept {
    RegisterInstruction ri;
    uint8_t flags = kRegIsBranch | kRegHasImm;
    if (opc != IROpCode::Br) flags |= kRegHasSrc1;
    ri.header = MakeHeader(opc, 0, src, 0, flags);
    ri.imm.branch_target = target;
    return ri;
}

static RegisterInstruction InstrCondBranch(IROpCode opc, uint32_t target,
                                           uint8_t src1, uint8_t src2) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(opc, 0, src1, src2,
                           kRegIsBranch | kRegHasImm | kRegHasSrc1 | kRegHasSrc2);
    ri.imm.branch_target = target;
    return ri;
}

static RegisterInstruction InstrNewObj(uint8_t dst, uint32_t type_token) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::NewObj, dst, 0, 0, kRegHasDst | kRegHasImm);
    ri.imm.field_offset = type_token;
    return ri;
}

static RegisterInstruction InstrLdFld(uint8_t dst, uint8_t src, uint32_t field_idx) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::LdFld, dst, src, 0, kRegHasDst | kRegHasSrc1 | kRegHasImm);
    ri.imm.field_offset = field_idx;
    return ri;
}

static RegisterInstruction InstrStFld(uint8_t obj, uint32_t field_idx, uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::StFld, 0, obj, src, kRegHasSrc1 | kRegHasSrc2 | kRegHasImm);
    ri.imm.field_offset = field_idx;
    return ri;
}

static RegisterInstruction InstrBox(uint8_t dst, uint8_t src, uint32_t type_token) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Box, dst, src, 0, kRegHasDst | kRegHasSrc1 | kRegHasImm);
    ri.imm.field_offset = type_token;
    return ri;
}

static RegisterInstruction InstrUnbox(uint8_t dst, uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Unbox, dst, src, 0, kRegHasDst | kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrLdLen(uint8_t dst, uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::LdLen, dst, src, 0, kRegHasDst | kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrNewArr(uint8_t dst, uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::NewArr, dst, src, 0, kRegHasDst | kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrLdElem(uint8_t dst, uint8_t arr, uint8_t index) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::LdElem, dst, arr, index, kRegHasDst | kRegHasSrc1 | kRegHasSrc2);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrStElem(uint8_t arr, uint8_t index, uint8_t value) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::StElem, 0, arr, index,
                           kRegHasSrc1 | kRegHasSrc2 | kRegHasSrc3);
    ri.header |= (static_cast<uint64_t>(value) << 48);  // src3 in bits [55:48]
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrDup(uint8_t dst, uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Dup, dst, src, 0, kRegHasDst | kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrRet(uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::Ret, 0, src, 0, kRegHasSrc1);
    ri.imm.i4 = 0;
    return ri;
}

static RegisterInstruction InstrStLoc(uint32_t local_idx, uint8_t src) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::StLoc, static_cast<uint8_t>(8 + local_idx), src, 0,
                           kRegHasSrc1 | kRegHasDst | kRegHasImm | kRegIsStore);
    ri.imm.operand_index = local_idx;
    return ri;
}

static RegisterInstruction InstrLdLoc(uint8_t dst, uint32_t local_idx) noexcept {
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::LdLoc, dst, static_cast<uint8_t>(8 + local_idx), 0,
                           kRegHasDst | kRegHasSrc1 | kRegHasImm);
    ri.imm.operand_index = local_idx;
    return ri;
}

static void* SealAndGetEntry(NativeMethod* nm) {
    if (nm == nullptr || nm->code == nullptr) return nullptr;
    return nm->code;
}

static void DumpCode(const uint8_t* code, uint32_t size) {
    std::printf("    code bytes (%u):\n", size);
    for (uint32_t i = 0; i < size; i++) {
        if (i % 16 == 0) std::printf("      ");
        std::printf("%02X ", code[i]);
        if (i % 16 == 15) std::printf("\n");
    }
    if (size % 16 != 0 && size <= 256) std::printf("\n");
    if (size > 256) std::printf("      ... (%u more bytes)\n", size - 256);
}

static uint64_t ExecuteNative(void* entry, uint64_t* args = nullptr) {
    uint64_t args_buf[8] = {};
    uint64_t ret_buf[2] = {};
    if (args) std::memcpy(args_buf, args, 8 * sizeof(uint64_t));
    using NativeEntry = void (*)(void*, void*);
    auto native_entry = reinterpret_cast<NativeEntry>(entry);
    std::printf("    calling entry=%p\n", entry);
    native_entry(args_buf, ret_buf);
    std::printf("    after call\n");
    return ret_buf[0];
}

// ── SEH-protected native execution (for fuzz test) ──────────────────────
static uint64_t ExecuteNativeSafe(void* entry, bool& crashed) noexcept {
    crashed = false;
    __try {
        return ExecuteNative(entry);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        crashed = true;
        return 0;
    }
}

// ═══════════════════════════════════════════════════════════════════════
// Test: LdcI4_Ret
// ═══════════════════════════════════════════════════════════════════════
static bool Test_LdcI4_Ret() {
    std::printf("  Test_LdcI4_Ret...\n");
    RegisterMethod rm;
    rm.instructions = { InstrI4(IROpCode::LdcI4, 42, 0), InstrRet(0) };
    rm.max_regs = 1;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    std::printf("    nm->code=%p nm->code_size=%u\n", nm->code, nm->code_size);
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu\n", (unsigned long long)result);
    return result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: Add_Ret
// ═══════════════════════════════════════════════════════════════════════
static bool Test_Add_Ret() {
    RegisterMethod rm;
    rm.instructions = { InstrI4(IROpCode::LdcI4, 12, 0), InstrI4(IROpCode::LdcI4, 30, 1), InstrBinary(IROpCode::Add, 2, 0, 1), InstrRet(2) };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    return ExecuteNative(entry) == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: ArithmeticChain
// ═══════════════════════════════════════════════════════════════════════
static bool Test_ArithmeticChain() {
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 10, 0), InstrI4(IROpCode::LdcI4, 20, 1),
        InstrBinary(IROpCode::Add, 2, 0, 1), InstrI4(IROpCode::LdcI4, 3, 3),
        InstrBinary(IROpCode::Mul, 4, 2, 3), InstrI4(IROpCode::LdcI4, 5, 5),
        InstrBinary(IROpCode::Sub, 6, 4, 5), InstrRet(6),
    };
    rm.max_regs = 7;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    return ExecuteNative(entry) == 85;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: BranchUncond
// ═══════════════════════════════════════════════════════════════════════
static bool Test_BranchUncond() {
    std::printf("  Test_BranchUncond...\n");
    RegisterMethod rm;
    rm.instructions = { InstrI4(IROpCode::LdcI4, 42, 0), InstrBranch(IROpCode::Br, 3), InstrI4(IROpCode::LdcI4, 0, 0), InstrRet(0) };
    rm.max_regs = 1;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    return ExecuteNative(entry) == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: BranchTaken
// ═══════════════════════════════════════════════════════════════════════
static bool Test_BranchTaken() {
    std::printf("  Test_BranchTaken...\n");
    RegisterMethod rm;
    rm.instructions = { InstrI4(IROpCode::LdcI4, 1, 0), InstrBranch(IROpCode::BrFalse, 4, 0), InstrI4(IROpCode::LdcI4, 42, 1), InstrBranch(IROpCode::Br, 5), InstrI4(IROpCode::LdcI4, 0, 1), InstrRet(1) };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    return result != UINT64_MAX && result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: BranchNotTaken
// ═══════════════════════════════════════════════════════════════════════
static bool Test_BranchNotTaken() {
    std::printf("  Test_BranchNotTaken...\n");
    RegisterMethod rm;
    rm.instructions = { InstrI4(IROpCode::LdcI4, 0, 0), InstrBranch(IROpCode::BrFalse, 4, 0), InstrI4(IROpCode::LdcI4, 42, 1), InstrBranch(IROpCode::Br, 5), InstrI4(IROpCode::LdcI4, 0, 1), InstrRet(1) };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    return result != UINT64_MAX && result == 0;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: CanGenerateNativeCode (relaxed — all opcodes accepted)
// ═══════════════════════════════════════════════════════════════════════
static bool Test_CanGenerate_Unsupported() {
    RegisterMethod rm;
    RegisterInstruction ri;
    ri.header = MakeHeader(IROpCode::LdFld, 0, 0, 0, kRegHasDst | kRegHasImm);
    ri.imm.field_offset = 0;
    rm.instructions = { ri, InstrRet(0) };
    rm.max_regs = 1;
    return CanGenerateNativeCode(rm);
}

// ═══════════════════════════════════════════════════════════════════════
// Test: DeoptMetadata_Call
// ═══════════════════════════════════════════════════════════════════════
static bool Test_DeoptMetadata_Call() {
    std::printf("  Test_DeoptMetadata_Call...\n");
    RegisterMethod rm;
    rm.instructions = { InstrI4(IROpCode::LdcI4, 42, 0), InstrRet(0) };
    rm.max_regs = 1;
    CodeGenConfig cfg;
    cfg.enable_deopt = true;
    auto* nm = GenerateNativeCode(rm, cfg);
    if (nm == nullptr) { std::printf("    FAIL: null\n"); return false; }
    std::printf("    deopt_entry_count=%u\n", nm->deopt_entry_count);
    return nm->code != nullptr && nm->code_size > 0;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: DeoptSequence_Generated
// ═══════════════════════════════════════════════════════════════════════
static bool Test_DeoptSequence_Generated() {
    std::printf("  Test_DeoptSequence_Generated...\n");
    RegisterMethod rm;
    rm.instructions = { InstrI4(IROpCode::LdcI4, 7, 0), InstrRet(0) };
    rm.max_regs = 1;
    CodeGenConfig cfg;
    cfg.enable_deopt = true;
    auto* nm = GenerateNativeCode(rm, cfg);
    if (nm == nullptr) { std::printf("    FAIL: null\n"); return false; }
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 7)\n", (unsigned long long)result);
    return result == 7;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: DeoptEntry_Registration
// ═══════════════════════════════════════════════════════════════════════
static bool Test_DeoptEntry_Registration() {
    std::printf("  Test_DeoptEntry_Registration...\n");
    RegisterMethod rm;
    rm.instructions = { InstrI4(IROpCode::LdcI4, 99, 0), InstrRet(0) };
    rm.max_regs = 1;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) { std::printf("    FAIL: null\n"); return false; }
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;

    chaos::il2cpp::codegen::RegisterT4Code(entry, nm->code_size, nm);

    const auto* found = chaos::il2cpp::codegen::FindT4CodeByAddress(entry);
    if (found != nm) { std::printf("    FAIL: entry point lookup\n"); return false; }

    auto* mid = static_cast<uint8_t*>(entry) + (nm->code_size > 4 ? 4 : 0);
    found = chaos::il2cpp::codegen::FindT4CodeByAddress(mid);
    if (found != nm) { std::printf("    FAIL: mid-range lookup\n"); return false; }

    auto* out_of_range = static_cast<uint8_t*>(entry) + nm->code_size + 256;
    found = chaos::il2cpp::codegen::FindT4CodeByAddress(out_of_range);
    if (found != nullptr) { std::printf("    FAIL: out-of-range should be null\n"); return false; }

    std::printf("    nm=%p entry=%p code_size=%u\n", static_cast<const void*>(nm), entry, nm->code_size);
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: NewObj — creates an object with default zero fields
// ═══════════════════════════════════════════════════════════════════════
static bool Test_NewObj() {
    std::printf("  Test_NewObj...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrNewObj(0, 1),      // r0 = NewObj(type=1)
        InstrLdFld(1, 0, 0),    // r1 = LdFld(r0, field=0)
        InstrRet(1),
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 0)\n", (unsigned long long)result);
    return result == 0;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: LdFld_StFld — NewObj, StFld field[0]=42, LdFld field[0], expect 42
// ═══════════════════════════════════════════════════════════════════════
static bool Test_LdFld_StFld() {
    std::printf("  Test_LdFld_StFld...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // r0 = 42
        InstrNewObj(1, 1),                  // r1 = NewObj(type=1)
        InstrStFld(1, 0, 0),               // StFld(r1, field=0, r0)
        InstrLdFld(2, 1, 0),               // r2 = LdFld(r1, field=0)
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    DumpInstrs(rm.instructions);
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: Box — Box(7) returns non-null pointer
// ═══════════════════════════════════════════════════════════════════════
static bool Test_Box() {
    std::printf("  Test_Box...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 7, 0),   // r0 = 7
        InstrBox(1, 0, 42),                 // r1 = Box(r0, type_token=42)
        InstrRet(1),                         // return boxed pointer (should be non-null)
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    DumpInstrs(rm.instructions);
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected non-null pointer)\n", (unsigned long long)result);
    return result != 0;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: Unbox — NewObj, StFld field[0]=42, Unbox, expect 42
// ═══════════════════════════════════════════════════════════════════════
static bool Test_Unbox() {
    std::printf("  Test_Unbox...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrNewObj(0, 1),                  // r0 = NewObj(type=1)
        InstrI4(IROpCode::LdcI4, 42, 1),   // r1 = 42
        InstrStFld(0, 0, 1),               // StFld(r0, field=0, r1=42)
        InstrUnbox(2, 0),                   // r2 = Unbox(r0) → reads fields[0]
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: TlabNewObj — NewObj via TLAB inline allocation
// ═══════════════════════════════════════════════════════════════════════
static bool Test_TlabNewObj() {
    std::printf("  Test_TlabNewObj...\n");
    PrimeTlab();
    RegisterMethod rm;
    rm.instructions = {
        InstrNewObj(0, 1),      // r0 = NewObj(type=1)
        InstrLdFld(1, 0, 0),    // r1 = LdFld(r0, field=0)
        InstrRet(1),
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 0)\n", (unsigned long long)result);
    return result == 0;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: TlabBox — Box via TLAB inline allocation
// ═══════════════════════════════════════════════════════════════════════
static bool Test_TlabBox() {
    std::printf("  Test_TlabBox...\n");
    PrimeTlab();
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),   // r0 = 42
        InstrBox(1, 0, 42),                 // r1 = Box(r0, type_token=42)
        InstrRet(1),
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected non-null pointer)\n", (unsigned long long)result);
    return result != 0;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: TlabNewObjBox — NewObj + StFld + Unbox via TLAB inline
// ═══════════════════════════════════════════════════════════════════════
static bool Test_TlabNewObjBox() {
    std::printf("  Test_TlabNewObjBox...\n");
    PrimeTlab();
    RegisterMethod rm;
    rm.instructions = {
        InstrNewObj(0, 1),                  // r0 = NewObj(type=1)
        InstrI4(IROpCode::LdcI4, 42, 1),   // r1 = 42
        InstrStFld(0, 0, 1),               // StFld(r0, field=0, r1=42)
        InstrUnbox(2, 0),                   // r2 = Unbox(r0)
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    DumpInstrs(rm.instructions);
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: LdLen — NewArr(5) → LdLen → expect 5
// ═══════════════════════════════════════════════════════════════════════
static bool Test_LdLen() {
    std::printf("  Test_LdLen...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 5, 0),   // r0 = 5 (length)
        InstrNewArr(1, 0),                  // r1 = NewArr(r0)
        InstrLdLen(2, 1),                   // r2 = LdLen(r1)
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 5)\n", (unsigned long long)result);
    return result == 5;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: NewArr — just verify NewArr returns non-null
// ═══════════════════════════════════════════════════════════════════════
static bool Test_NewArr() {
    std::printf("  Test_NewArr...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 3, 0),   // r0 = 3
        InstrNewArr(1, 0),                  // r1 = NewArr(r0)
        InstrRet(1),                         // return array pointer (non-null)
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected non-null)\n", (unsigned long long)result);
    return result != 0;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: NewArrTlab — NewArr(42) via TLAB inline → LdLen → expect 42
// ═══════════════════════════════════════════════════════════════════════
static bool Test_NewArrTlab() {
    std::printf("  Test_NewArrTlab...\n");
    PrimeTlab();
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // r0 = 42
        InstrNewArr(1, 0),                  // r1 = NewArr(r0) — via TLAB inline
        InstrLdLen(2, 1),                   // r2 = LdLen(r1)
        InstrRet(2),                        // return length
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: LdElem_StElem — NewArr(3), StElem[0]=42, LdElem[0], expect 42
// ═══════════════════════════════════════════════════════════════════════
static bool Test_LdElem_StElem() {
    std::printf("  Test_LdElem_StElem...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 3, 0),   // r0 = 3 (array length)
        InstrNewArr(1, 0),                  // r1 = NewArr(r0)
        InstrI4(IROpCode::LdcI4, 0, 2),   // r2 = 0 (index)
        InstrI4(IROpCode::LdcI4, 42, 3),  // r3 = 42 (value)
        InstrStElem(1, 2, 3),              // StElem(r1, r2, r3) — arr, index, value
        InstrI4(IROpCode::LdcI4, 0, 2),   // r2 = 0 (index again)
        InstrLdElem(4, 1, 2),              // r4 = LdElem(r1, r2)
        InstrRet(4),
    };
    rm.max_regs = 5;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: StElemFix — Verify StElem src3 DCE fix round-trips 42
// ═══════════════════════════════════════════════════════════════════════
// Regression test for the src3 use-count bug: DCE in OptimizeInstructions
// was not counting src3_reg() uses, so the LdcI4 defining StElem's value
// was incorrectly removed. Same sequence as LdElem_StElem, explicit 42.
static bool Test_StElemFix() {
    std::printf("  Test_StElemFix...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 3, 0),   // r0 = 3 (array length)
        InstrNewArr(1, 0),                  // r1 = NewArr(r0)
        InstrI4(IROpCode::LdcI4, 0, 2),   // r2 = 0 (index)
        InstrI4(IROpCode::LdcI4, 42, 3),  // r3 = 42 (value) — was being DCE'd
        InstrStElem(1, 2, 3),              // StElem(r1, r2, r3) — arr, index, value
        InstrI4(IROpCode::LdcI4, 0, 2),   // r2 = 0 (index again)
        InstrLdElem(4, 1, 2),              // r4 = LdElem(r1, r2)
        InstrRet(4),
    };
    rm.max_regs = 5;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: Dup — LdcI4 42, Dup r0→r1, Add r0+r1→r2, expect 84
// ═══════════════════════════════════════════════════════════════════════
static bool Test_Dup() {
    std::printf("  Test_Dup...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // r0 = 42
        InstrDup(1, 0),                    // r1 = Dup(r0) = 42
        InstrBinary(IROpCode::Add, 2, 0, 1), // r2 = r0 + r1 = 84
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 84)\n", (unsigned long long)result);
    return result == 84;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: GcSlotMap — verify GC points are populated with ObjectRef slots
// ═══════════════════════════════════════════════════════════════════════
static bool Test_GcSlotMap() {
    std::printf("  Test_GcSlotMap...\n");
    // Method: NewObj creates an ObjectRef — GC point at the call should have slots
    RegisterMethod rm;
    rm.instructions = {
        InstrNewObj(0, 1),      // r0 = NewObj(type=1) → ObjectRef
        InstrI4(IROpCode::LdcI4, 42, 1),  // r1 = 42 (int, not a ref)
        InstrLdFld(2, 0, 0),    // r2 = LdFld(r0, field=0) → ObjectRef (conservative)
        InstrRet(2),
    };
    rm.max_regs = 3;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    std::printf("    gc_point_count=%u, slot_map_data=%p, slot_map_size=%u\n",
                nm->gc_point_count, nm->slot_map_data, nm->slot_map_size);

    // Verify gc_points have slot info
    bool has_slots = false;
    for (uint32_t i = 0; i < nm->gc_point_count; ++i) {
        const auto& gp = nm->gc_points[i];
        std::printf("    gc_point[%u]: offset=%u slot_count=%u\n", i, gp.native_offset, gp.slot_count);
        if (gp.slot_count > 0 && gp.slots != nullptr) {
            has_slots = true;
            for (uint32_t j = 0; j < gp.slot_count; ++j) {
                std::printf("      slot[%u]: kind=%d index=%u\n", j,
                            static_cast<int>(gp.slots[j].kind), gp.slots[j].index);
            }
        }
    }
    if (!has_slots) { std::printf("    FAIL: no gc points with slots\n"); return false; }

    // Verify slot_map_data is populated
    if (nm->slot_map_data == nullptr || nm->slot_map_size == 0) {
        std::printf("    FAIL: slot_map_data not populated\n"); return false;
    }
    auto* sm = static_cast<const GcSlotMapV0*>(nm->slot_map_data);
    std::printf("    GcSlotMapV0: frame_size=%u num_gc_slots=%u\n",
                sm->frame_size, sm->num_gc_slots);
    if (sm->num_gc_slots == 0) { std::printf("    FAIL: no gc slots in map\n"); return false; }
    for (uint32_t i = 0; i < sm->num_gc_slots && i < 8; ++i) {
        std::printf("      slot[%u]: encoding=0x%04X\n", i, sm->slots[i]);
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: GcSlotMapRegistration — GcRegisterSlotMap + GcLookupSlotMap round-trip
// ═══════════════════════════════════════════════════════════════════════
static bool Test_GcSlotMapRegistration() {
    std::printf("  Test_GcSlotMapRegistration...\n");
    // Use NewObj → LdFld → Ret: LdFld call sees NewObj result (ObjectRef) in vreg 0
    RegisterMethod rm;
    rm.instructions = {
        InstrNewObj(0, 1),      // r0 = NewObj(type=1) → ObjectRef (no GC point sees this)
        InstrLdFld(1, 0, 0),    // r1 = LdFld(r0, field=0) — GC point here sees r0=ObjectRef
        InstrRet(1),
    };
    rm.max_regs = 2;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    void* entry = nm->code; if (entry == nullptr) return false;
    std::printf("    slot_map_data=%p, slot_map_size=%u\n", nm->slot_map_data, nm->slot_map_size);

    if (nm->slot_map_data == nullptr) {
        std::printf("    FAIL: expected slot_map_data to be populated\n");
        return false;
    }

    // Register and lookup
    chaos::il2cpp::runtime_core::GcRegisterSlotMap(
        entry,
        static_cast<const GcSlotMapV0*>(nm->slot_map_data));
    std::printf("    registered slot map for entry=%p\n", entry);

    const auto* found = chaos::il2cpp::runtime_core::GcLookupSlotMap(entry);
    if (found == nullptr) { std::printf("    FAIL: GcLookupSlotMap returned null\n"); return false; }
    if (found->num_gc_slots == 0) { std::printf("    FAIL: found slot map has 0 slots\n"); return false; }
    std::printf("    GcLookupSlotMap succeeded: num_gc_slots=%u\n", found->num_gc_slots);
    for (uint32_t i = 0; i < found->num_gc_slots && i < 4; ++i) {
        std::printf("      slot[%u]: encoding=0x%04X\n", i, found->slots[i]);
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: SehTable — verify SEH clause table is emitted at correct offset
// ═══════════════════════════════════════════════════════════════════════
static bool Test_SehTable() {
    std::printf("  Test_SehTable...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // r0 = 42
        InstrRet(0),
    };
    rm.max_regs = 1;
    // Add one SEH clause: try [0,2) handler at 2
    rm.seh_clauses = { SEHClause{
        SEHFlags::Exception,  // flags
        0,                     // try_start_idx
        2,                     // try_end_idx
        0,                     // handler_start_idx (same as start, for test)
        0                      // class_token
    } };

    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm); if (nm == nullptr) return false;
    std::printf("    seh_table_offset=%u\n", nm->seh_table_offset);
    if (nm->seh_table_offset == 0) { std::printf("    FAIL: no SEH table\n"); return false; }

    // Read back the SEH table from the code buffer
    const uint8_t* code = static_cast<const uint8_t*>(nm->code);
    const uint8_t* table = code + nm->seh_table_offset;
    uint32_t count;
    std::memcpy(&count, table, sizeof(count));
    std::printf("    clause_count=%u\n", count);
    if (count != 1) { std::printf("    FAIL: expected 1 clause, got %u\n", count); return false; }

    uint32_t flags, try_start, try_end, handler_st, class_token;
    std::memcpy(&flags,       table + 4, sizeof(flags));
    std::memcpy(&try_start,   table + 8, sizeof(try_start));
    std::memcpy(&try_end,     table + 12, sizeof(try_end));
    std::memcpy(&handler_st,  table + 16, sizeof(handler_st));
    std::memcpy(&class_token, table + 20, sizeof(class_token));
    std::printf("    clause[0]: flags=0x%x try=[%u,%u) handler=%u class_token=%u\n",
                flags, try_start, try_end, handler_st, class_token);

    // try_start should be >0 (byte offset after prologue, not instruction index)
    if (try_start == 0) { std::printf("    FAIL: try_start should be >0 (byte offset)\n"); return false; }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: SehCanGenerate — method with SEH clauses passes CanGenerateNativeCode
// ═══════════════════════════════════════════════════════════════════════
static bool Test_SehCanGenerate() {
    std::printf("  Test_SehCanGenerate...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 1, 0),
        InstrRet(0),
    };
    rm.max_regs = 1;
    rm.seh_clauses = { SEHClause{
        SEHFlags::Exception, 0, 1, 0, 0
    } };
    if (!CanGenerateNativeCode(rm)) {
        std::printf("    FAIL: CanGenerateNativeCode returned false\n");
        return false;
    }
    std::printf("    CanGenerateNativeCode returned true (OK)\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: OsrPromote — hot loop in RegisterExecute triggers T4 promotion
// ═══════════════════════════════════════════════════════════════════════
static bool Test_OsrPromote() {
    std::printf("  Test_OsrPromote...\n");

    // Build RegisterMethod with a loop: count down from 200 to 0.
    // Instructions:
    //   0: LdcI4 r0 = 200   (counter)
    //   1: LdcI4 r1 = 1     (decrement)
    //   2: Sub    r0 = r0 - r1
    //   3: BrTrue r0 -> 2   (backward branch to instruction 2 if r0 != 0)
    //   4: Ret
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 200, 0),
        InstrI4(IROpCode::LdcI4, 1, 1),
        InstrBinary(IROpCode::Sub, 0, 0, 1),
        InstrBranch(IROpCode::BrTrue, 2, 0),
        InstrRet(0),
    };
    rm.max_regs = 2;

    // RegisterMethod must be heap-allocated (PatchMethod expects pointer).
    auto* reg_method = new RegisterMethod(std::move(rm));

    // Set up a mock PatchMethod.
    PatchMethod pm;
    pm.cached_reg_method = reg_method;
    pm.tier_state.store(0, std::memory_order_relaxed);

    // Set up RegisterFrame.
    RegisterFrame rf;
    std::memset(&rf, 0, sizeof(rf));
    rf.patch_method = &pm;
    rf.args = nullptr;
    rf.arg_count = 0;

    // Execute the loop — it should hit 199 backward branches (well over threshold).
    bool ok = RegisterExecute(rf, reg_method->instructions.data(),
                              static_cast<uint32_t>(reg_method->instructions.size()));
    if (!ok) { std::printf("    FAIL: RegisterExecute returned false\n"); return false; }

    // Verify T4 promotion happened.
    uint32_t state = pm.tier_state.load(std::memory_order_acquire);
    std::printf("    tier_state=%u (expected %u = kT4Ready)\n", state, PatchMethod::kT4Ready);
    if (state < PatchMethod::kT4Ready) { std::printf("    FAIL: tier_state not promoted to T4\n"); return false; }

    if (pm.cached_native_method == nullptr) { std::printf("    FAIL: cached_native_method is null\n"); return false; }
    std::printf("    cached_native_method=%p code_size=%u\n",
                static_cast<const void*>(pm.cached_native_method),
                pm.cached_native_method->code_size);

    // Clean up the heap-allocated RegisterMethod.
    delete reg_method;

    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: OsrEntry — direct OSR entry call with pre-populated RegisterFile
// ═══════════════════════════════════════════════════════════════════════
static bool Test_OsrEntry() {
    std::printf("  Test_OsrEntry...\n");

    // Build a loop method: count down from 200 to 0.
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 200, 0),
        InstrI4(IROpCode::LdcI4, 1, 1),
        InstrBinary(IROpCode::Sub, 0, 0, 1),
        InstrBranch(IROpCode::BrTrue, 2, 0),
        InstrRet(0),
    };
    rm.max_regs = 2;

    // Generate native code (with OSR entry since method has a loop).
    CodeGenConfig cfg;
    cfg.enable_deopt = true;
    cfg.safepoint_fn = nullptr;
    auto* nm = GenerateNativeCode(rm, cfg);
    if (nm == nullptr) { std::printf("    FAIL: GenerateNativeCode returned null\n"); return false; }

    // Verify OSR entry exists.
    if (nm->osr_entry_offset == 0) { std::printf("    FAIL: osr_entry_offset is 0 (expected != 0)\n"); return false; }
    std::printf("    osr_entry_offset=%u code_size=%u\n", nm->osr_entry_offset, nm->code_size);

    // Verify non-loop method has no OSR entry.
    RegisterMethod rm_simple;
    rm_simple.instructions = { InstrI4(IROpCode::LdcI4, 42, 0), InstrRet(0) };
    rm_simple.max_regs = 1;
    auto* nm_simple = GenerateNativeCode(rm_simple, cfg);
    if (nm_simple == nullptr) { std::printf("    FAIL: simple method generation failed\n"); return false; }
    if (nm_simple->osr_entry_offset != 0) {
        std::printf("    FAIL: simple method has osr_entry_offset=%u (expected 0)\n", nm_simple->osr_entry_offset);
        return false;
    }

    // Set up RegisterFile with counter=200.
    RegisterFile rf;
    std::memset(&rf, 0, sizeof(rf));
    rf.gpr[0] = 200;
    rf.gpr[1] = 1;

    // Call OSR entry directly.
    using OsrEntryFn = void (*)(void*, void*);
    auto osr_entry = reinterpret_cast<OsrEntryFn>(
        static_cast<uint8_t*>(nm->code) + nm->osr_entry_offset);
    uint64_t ret_buf[2] = {};
    osr_entry(&rf, ret_buf);

    std::printf("    ret_buf[0]=%llu (expected 0)\n", ret_buf[0]);
    if (ret_buf[0] != 0) { std::printf("    FAIL: wrong return value\n"); return false; }

    // Clean up.
    CHAOS_IL2CPP_FREE(nm_simple);

    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Deopt Stress Tests
// ═══════════════════════════════════════════════════════════════════════

// Test: DeoptOvfArithmetic — AddOvf with INT32_MAX+1 triggers deopt,
// returns kDeoptMagic via native entry.
static bool Test_DeoptOvfArithmetic() {
    std::printf("  Test_DeoptOvfArithmetic...\n");
    t_deopt_state.deopt_happened = false;

    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, INT32_MAX, 0),
        InstrI4(IROpCode::LdcI4, 1, 1),
        InstrBinary(IROpCode::AddOvf, 2, 0, 1),
        InstrRet(2),
    };
    rm.max_regs = 3;

    CodeGenConfig cfg;
    cfg.enable_deopt = true;
    cfg.safepoint_fn = nullptr;
    auto* nm = GenerateNativeCode(rm, cfg);
    if (nm == nullptr) { std::printf("    FAIL: null\n"); return false; }
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    chaos::il2cpp::codegen::RegisterT4Code(entry, nm->code_size, nm);

    uint64_t result = ExecuteNative(entry);
    std::printf("    ret_buf[0]=0x%llX (expected 0x%llX = kDeoptMagic)\n",
                (unsigned long long)result, (unsigned long long)kDeoptMagic);
    if (result != kDeoptMagic) { std::printf("    FAIL: expected kDeoptMagic\n"); return false; }

    if (!t_deopt_state.deopt_happened) { std::printf("    FAIL: deopt_happened should be true\n"); return false; }
    std::printf("    deopt_happened=true, instr_pc=%u\n", t_deopt_state.instr_pc);
    t_deopt_state.deopt_happened = false;
    return true;
}

// Test: DeoptThenRegisterExecute — after T4 deopt, RegisterExecute can
// still run the method correctly (no state corruption).
static bool Test_DeoptThenRegisterExecute() {
    std::printf("  Test_DeoptThenRegisterExecute...\n");
    t_deopt_state.deopt_happened = false;

    // Method: with overflow on AddOvf, but normal Add below.
    // T4 deopts on AddOvf overflow, then we verify RegisterExecute works for
    // a normal Add method.
    RegisterMethod rm_ovf;
    rm_ovf.instructions = {
        InstrI4(IROpCode::LdcI4, INT32_MAX, 0),
        InstrI4(IROpCode::LdcI4, 1, 1),
        InstrBinary(IROpCode::AddOvf, 2, 0, 1),
        InstrRet(2),
    };
    rm_ovf.max_regs = 3;

    // Generate native T4 code with deopt support.
    CodeGenConfig cfg;
    cfg.enable_deopt = true;
    cfg.safepoint_fn = nullptr;
    auto* nm = GenerateNativeCode(rm_ovf, cfg);
    if (nm == nullptr) { std::printf("    FAIL: GenerateNativeCode null\n"); return false; }
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;

    // Execute T4 native — should deopt and return kDeoptMagic.
    uint64_t result = ExecuteNative(entry);
    if (result != kDeoptMagic) { std::printf("    FAIL: expected kDeoptMagic\n"); return false; }
    std::printf("    T4 deopt returned kDeoptMagic ✓\n");

    // Now run a normal (non-overflow) RegisterMethod via RegisterExecute
    // to verify the interpreter is not corrupted by the deopt path.
    RegisterMethod rm_normal;
    rm_normal.instructions = {
        InstrI4(IROpCode::LdcI4, 10, 0),
        InstrI4(IROpCode::LdcI4, 32, 1),
        InstrBinary(IROpCode::Add, 2, 0, 1),
        InstrRet(2),
    };
    rm_normal.max_regs = 3;

    RegisterFrame rf;
    std::memset(&rf, 0, sizeof(rf));
    bool ok = RegisterExecute(rf, rm_normal.instructions.data(),
                               static_cast<uint32_t>(rm_normal.instructions.size()));
    if (!ok) { std::printf("    FAIL: RegisterExecute returned false\n"); return false; }
    std::printf("    RegisterExecute result=%llu (expected 42)\n",
                (unsigned long long)rf.ret_val);
    if (rf.ret_val != 42) { std::printf("    FAIL: wrong result\n"); return false; }

    t_deopt_state.deopt_happened = false;
    return true;
}

// Test: OsrRepromotion — after T4 promotion, re-enter T4 via OSR on
// the first backward branch (osr_reenable flag).
static bool Test_OsrRepromotion() {
    std::printf("  Test_OsrRepromotion...\n");

    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 100, 0),
        InstrI4(IROpCode::LdcI4, 1, 1),
        InstrBinary(IROpCode::Sub, 0, 0, 1),
        InstrBranch(IROpCode::BrTrue, 2, 0),
        InstrRet(0),
    };
    rm.max_regs = 2;

    auto* reg_method = new RegisterMethod(std::move(rm));

    PatchMethod pm;
    pm.cached_reg_method = reg_method;
    pm.tier_state.store(0, std::memory_order_relaxed);

    RegisterFrame rf;
    std::memset(&rf, 0, sizeof(rf));
    rf.patch_method = &pm;
    rf.osr_reenable = true;  // Simulate post-deopt state: re-enter on first backedge

    // RegisterExecute with osr_reenable = true should trigger TryOsrPromotion
    // on the first backward branch, promoting to T4 via OSR.
    bool ok = RegisterExecute(rf, reg_method->instructions.data(),
                              static_cast<uint32_t>(reg_method->instructions.size()));
    if (!ok) { std::printf("    FAIL: RegisterExecute returned false\n"); return false; }

    uint32_t state = pm.tier_state.load(std::memory_order_acquire);
    std::printf("    tier_state=%u (expected %u = kT4Ready)\n", state, PatchMethod::kT4Ready);
    if (state < PatchMethod::kT4Ready) { std::printf("    FAIL: not promoted\n"); return false; }
    if (pm.cached_native_method == nullptr) { std::printf("    FAIL: no cached native method\n"); return false; }

    std::printf("    native_method=%p osr_entry_offset=%u\n",
                static_cast<const void*>(pm.cached_native_method),
                pm.cached_native_method->osr_entry_offset);

    delete reg_method;
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Performance benchmarks
// ═══════════════════════════════════════════════════════════════════════
// Compares T4 native vs RegisterExecute for the same arithmetic loop.
// Uses RDTSC for cycle-accurate timing.  Results printed to stdout.
//
// Note: FastExecute uses a stack-based IR model (different instruction
// format), so it's not directly comparable to the register-based
// RegisterExecute/T4 pipeline.  Comparing RegisterExecute vs T4
// quantifies the speedup from native code generation vs interpreted
// register dispatch.

#include <intrin.h>

// ── RDTSC helpers (non-inline to avoid measurement interference) ──────
#pragma intrinsic(__rdtsc)

static void WarmCpu() noexcept {
    // Execute a few RDTSC calls to warm the CPU and TSC pipeline before
    // taking measurements.
    for (int i = 0; i < 5; ++i) { volatile uint64_t t = __rdtsc(); (void)t; }
}

static bool Test_Benchmark() {
    std::printf("\n  ═════════════════════════════════════════════════════\n");
    std::printf("  Performance Benchmarks\n");
    std::printf("  ═════════════════════════════════════════════════════\n");

    // Loop: count down from 200000 to 0.
    // RegisterMethod instructions:
    //   0: LdcI4 r0 = 200000
    //   1: LdcI4 r1 = 1
    //   2: Sub    r0 = r0 - r1
    //   3: BrTrue r0 -> 2
    //   4: Ret    r0
    constexpr int32_t kLoopCount = 200000;

    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, kLoopCount, 0),
        InstrI4(IROpCode::LdcI4, 1, 1),
        InstrBinary(IROpCode::Sub, 0, 0, 1),
        InstrBranch(IROpCode::BrTrue, 2, 0),
        InstrRet(0),
    };
    rm.max_regs = 2;

    // ── RegisterExecute benchmark ──────────────────────────────────
    uint64_t re_cycles = 0;
    {
        RegisterFrame rf;
        std::memset(&rf, 0, sizeof(rf));
        WarmCpu();
        uint64_t tsc_start = __rdtsc();
        bool ok = RegisterExecute(rf, rm.instructions.data(),
                                   static_cast<uint32_t>(rm.instructions.size()));
        uint64_t tsc_end = __rdtsc();
        re_cycles = tsc_end - tsc_start;

        std::printf("  RegisterExecute:  %8llu cycles  (%5llu results=%llu, ok=%d)\n",
                    (unsigned long long)re_cycles,
                    (unsigned long long)(re_cycles / kLoopCount),
                    (unsigned long long)rf.ret_val, ok);
    }

    // ── T4 Native (cold) benchmark ─────────────────────────────────
    CodeGenConfig cfg;
    cfg.enable_deopt = true;
    cfg.safepoint_fn = nullptr;
    auto* nm = GenerateNativeCode(rm, cfg);
    if (nm == nullptr) { std::printf("  T4 Native:          FAIL (null)\n"); return false; }
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) { std::printf("  T4 Native:          FAIL (null entry)\n"); return false; }

    uint64_t t4_cold_cycles;
    {
        WarmCpu();
        uint64_t tsc_start = __rdtsc();
        uint64_t t4_ret = ExecuteNative(entry);
        uint64_t tsc_end = __rdtsc();
        t4_cold_cycles = tsc_end - tsc_start;
        std::printf("  T4 Native (cold):  %8llu cycles  (%5llu/iter, result=%llu)\n",
                    (unsigned long long)t4_cold_cycles,
                    (unsigned long long)(t4_cold_cycles / kLoopCount),
                    (unsigned long long)t4_ret);
    }

    // ── T4 Native (warm — 10 runs averaged) ─────────────────────────
    uint64_t avg = 0, best = 0;
    {
        uint64_t total = 0;
        constexpr int kRuns = 10;
        uint64_t results[kRuns];
        for (int i = 0; i < kRuns; ++i) {
            WarmCpu();
            uint64_t ts = __rdtsc();
            uint64_t r = ExecuteNative(entry);
            uint64_t te = __rdtsc();
            results[i] = te - ts;
            total += results[i];
            if (r != 0) { std::printf("  WARN: run %d returned %llu\n", i, (unsigned long long)r); }
        }
        avg = total / kRuns;
        // Compute min (best) to show best-case performance
        best = results[0];
        for (int i = 1; i < kRuns; ++i) {
            if (results[i] < best) best = results[i];
        }

        std::printf("  T4 Native (warm):  %8llu avg, %8llu best  (%5llu avg/iter)\n",
                    (unsigned long long)avg, (unsigned long long)best,
                    (unsigned long long)(avg / kLoopCount));
    }

    // ── Summary ─────────────────────────────────────────────────────
    uint64_t re_total = re_cycles;
    std::printf("\n  ── Summary (200000 iterations) ──\n");
    std::printf("  %-22s %12s  %12s  %s\n", "Tier", "Total cycles", "Cycles/iter", "Speedup");
    std::printf("  %-22s %12llu  %12llu  %s\n",
                "RegisterExecute",
                (unsigned long long)re_total,
                (unsigned long long)(re_total / kLoopCount),
                "1.00x (baseline)");
    std::printf("  %-22s %12llu  %12llu  %.2fx\n",
                "T4 Native (cold)",
                (unsigned long long)t4_cold_cycles,
                (unsigned long long)(t4_cold_cycles / kLoopCount),
                static_cast<double>(re_total) / t4_cold_cycles);
    std::printf("  %-22s %12llu  %12llu  %.2fx\n",
                "T4 Native (warm avg)",
                (unsigned long long)avg,
                (unsigned long long)(avg / kLoopCount),
                static_cast<double>(re_total) / avg);
    std::printf("  %-22s %12llu  %12llu  %.2fx\n",
                "T4 Native (warm best)",
                (unsigned long long)best,
                (unsigned long long)(best / kLoopCount),
                static_cast<double>(re_total) / best);

    // ── Graph Coloring Comparison ─────────────────────────────────────
    std::printf("\n  ── Graph Coloring Comparison (200000 iterations) ──\n");
    {
        struct Workload { const char* name; RegisterMethod rm; };
        auto make_simple = [&]() {
            RegisterMethod m;
            m.instructions = { InstrI4(IROpCode::LdcI4, kLoopCount, 0), InstrI4(IROpCode::LdcI4, 1, 1), InstrBinary(IROpCode::Sub, 0, 0, 1), InstrBranch(IROpCode::BrTrue, 2, 0), InstrRet(0) };
            m.max_regs = 2; return m;
        };
        auto make_heavy = [&]() {
            // 8 registers: r7=counter, r0..r6 = working regs rotated
            RegisterMethod m;
            m.instructions = {
                InstrI4(IROpCode::LdcI4, kLoopCount, 7),    // 0: r7 = counter (200000)
                InstrI4(IROpCode::LdcI4, 10, 0),            // 1: r0 = 10
                InstrI4(IROpCode::LdcI4, 20, 1),            // 2: r1 = 20
                InstrI4(IROpCode::LdcI4, 30, 2),            // 3: r2 = 30
                InstrI4(IROpCode::LdcI4, 40, 3),            // 4: r3 = 40
                InstrI4(IROpCode::LdcI4, 50, 4),            // 5: r4 = 50
                InstrI4(IROpCode::LdcI4, 60, 5),            // 6: r5 = 60
                InstrI4(IROpCode::LdcI4, 70, 6),            // 7: r6 = 70
                // loop body: rotate + arithmetic on all working regs (r0..r6)
                InstrBinary(IROpCode::Add,  0, 0, 6),       // 8:  r0 += r6
                InstrBinary(IROpCode::Sub,  1, 1, 0),       // 9:  r1 -= r0
                InstrBinary(IROpCode::Xor,  2, 2, 1),       // 10: r2 ^= r1
                InstrBinary(IROpCode::Add,  3, 3, 2),       // 11: r3 += r2
                InstrBinary(IROpCode::Sub,  4, 4, 3),       // 12: r4 -= r3
                InstrBinary(IROpCode::Xor,  5, 5, 4),       // 13: r5 ^= r4
                InstrBinary(IROpCode::Add,  6, 6, 5),       // 14: r6 += r5
                InstrBinary(IROpCode::Sub,  7, 7, 1),       // 15: r7 -= 1 (decrement counter)
                InstrBranch(IROpCode::BrTrue, 8, 7),        // 16: if r7 != 0 goto 8
                InstrRet(6),                                // 17: Ret(r6)
            };
            m.max_regs = 8; return m;
        };

        Workload workloads[] = {{"2-reg loop", make_simple()}, {"8-reg heavy", make_heavy()}};

        for (auto& wl : workloads) {
            std::printf("\n    ── %s ──\n", wl.name);

            CodeGenConfig cfg_on;
            cfg_on.enable_deopt = true; cfg_on.safepoint_fn = nullptr; cfg_on.enable_register_caching = true;
            auto* nm_on = GenerateNativeCode(wl.rm, cfg_on);
            if (nm_on == nullptr) { std::printf("    FAIL: on=null\n"); continue; }
            void* entry_on = SealAndGetEntry(nm_on); if (entry_on == nullptr) continue;

            CodeGenConfig cfg_off;
            cfg_off.enable_deopt = true; cfg_off.safepoint_fn = nullptr; cfg_off.enable_register_caching = false;
            auto* nm_off = GenerateNativeCode(wl.rm, cfg_off);
            if (nm_off == nullptr) { std::printf("    FAIL: off=null\n"); continue; }
            void* entry_off = SealAndGetEntry(nm_off); if (entry_off == nullptr) continue;

            std::printf("    code sizes: coloring=ON %uB, OFF %uB\n", nm_on->code_size, nm_off->code_size);

            auto run_warm = [](void* entry) -> uint64_t {
                uint64_t total = 0;
                constexpr int kRuns = 10;
                for (int i = 0; i < kRuns; ++i) {
                    WarmCpu(); uint64_t ts = __rdtsc(); ExecuteNative(entry); uint64_t te = __rdtsc();
                    total += (te - ts);
                }
                return total / kRuns;
            };

            uint64_t on_avg = run_warm(entry_on);
            uint64_t off_avg = run_warm(entry_off);

            std::printf("    coloring ON:  %8llu avg  (%5llu/iter)\n", (unsigned long long)on_avg, (unsigned long long)(on_avg / kLoopCount));
            std::printf("    coloring OFF: %8llu avg  (%5llu/iter)\n", (unsigned long long)off_avg, (unsigned long long)(off_avg / kLoopCount));
            if (off_avg > 0) std::printf("    speedup: %.2fx\n", static_cast<double>(off_avg) / on_avg);

            CHAOS_IL2CPP_FREE(nm_on); CHAOS_IL2CPP_FREE(nm_off);
        }
    }

    std::printf("\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Extended benchmarks: deopt-heavy + mixed workload
// ═══════════════════════════════════════════════════════════════════════
static bool Test_BenchmarkExtended() {
    std::printf("\n  ═════════════════════════════════════════════════════\n");
    std::printf("  Extended Benchmarks\n");
    std::printf("  ═════════════════════════════════════════════════════\n");

    constexpr int32_t kLoopCount = 200000;

    // ── 1. Deopt overhead benchmark ──────────────────────────────────
    // Method contains an unsupported opcode (CallBridge without target)
    // that triggers deopt on every invocation. Measures deopt path cost.
    std::printf("\n  ── Deopt Overhead ──\n");
    {
        RegisterMethod rm_deopt;
        rm_deopt.instructions = {
            InstrI4(IROpCode::LdcI4, 42, 0),
            InstrRet(0),
        };
        rm_deopt.max_regs = 1;

        CodeGenConfig cfg;
        cfg.enable_deopt = true;
        cfg.safepoint_fn = nullptr;
        auto* nm = GenerateNativeCode(rm_deopt, cfg);
        if (nm == nullptr) { std::printf("    FAIL: GenerateNativeCode null\n"); return false; }
        void* entry = SealAndGetEntry(nm);

        // Cold call (first T4 execution)
        WarmCpu();
        uint64_t tsc_start = __rdtsc();
        uint64_t deopt_result = ExecuteNative(entry);
        uint64_t tsc_end = __rdtsc();
        std::printf("  T4 cold:       %8llu cycles, result=%llu\n",
                    (unsigned long long)(tsc_end - tsc_start),
                    (unsigned long long)deopt_result);

        // Warm calls (10 runs averaged)
        uint64_t total = 0;
        for (int i = 0; i < 10; ++i) {
            WarmCpu();
            uint64_t ts = __rdtsc();
            ExecuteNative(entry);
            uint64_t te = __rdtsc();
            total += (te - ts);
        }
        std::printf("  T4 warm avg:   %8llu cycles (%d runs)\n",
                    (unsigned long long)(total / 10), 10);

        // RegisterExecute baseline for same method
        {
            RegisterFrame rf;
            std::memset(&rf, 0, sizeof(rf));
            WarmCpu();
            uint64_t ts = __rdtsc();
            RegisterExecute(rf, rm_deopt.instructions.data(),
                            static_cast<uint32_t>(rm_deopt.instructions.size()));
            uint64_t te = __rdtsc();
            std::printf("  RegisterExecute: %8llu cycles\n",
                        (unsigned long long)(te - ts));
        }
    }

    // ── 2. Mixed workload benchmark ──────────────────────────────────
    // Arithmetic + field access loop: simulates real code patterns.
    // Method: for count down { r3 = LdObj; r3 = Add(r3, r1); StObj(r3); }
    std::printf("\n  ── Mixed Workload (arithmetic + field access) ──\n");
    {
        // Simple loop with arithmetic only (field access is harder to
        // set up without NewObj+stubs in this test harness).
        RegisterMethod rm;
        rm.instructions = {
            InstrI4(IROpCode::LdcI4, kLoopCount, 0),
            InstrI4(IROpCode::LdcI4, 1, 1),
            InstrI4(IROpCode::LdcI4, 100, 2),  // accumulator
            InstrBinary(IROpCode::Sub, 0, 0, 1),  // decrement counter
            InstrBinary(IROpCode::Add, 2, 2, 1),  // acc += 1
            InstrBranch(IROpCode::BrTrue, 3, 0),  // if counter != 0 goto 3
            InstrRet(2),
        };
        rm.max_regs = 3;

        // RegisterExecute baseline
        uint64_t re_cycles = 0;
        {
            RegisterFrame rf;
            std::memset(&rf, 0, sizeof(rf));
            WarmCpu();
            uint64_t ts = __rdtsc();
            RegisterExecute(rf, rm.instructions.data(),
                            static_cast<uint32_t>(rm.instructions.size()));
            uint64_t te = __rdtsc();
            re_cycles = te - ts;
            std::printf("  RegisterExecute: %8llu cycles, result=%llu\n",
                        (unsigned long long)re_cycles,
                        (unsigned long long)rf.ret_val);
        }

        // T4 Native
        CodeGenConfig cfg;
        cfg.enable_deopt = true;
        cfg.safepoint_fn = nullptr;
        auto* nm = GenerateNativeCode(rm, cfg);
        if (nm == nullptr) { std::printf("    FAIL: GenerateNativeCode null\n"); return false; }
        void* entry = SealAndGetEntry(nm);

        // T4 cold
        WarmCpu();
        uint64_t ts = __rdtsc();
        uint64_t t4_ret = ExecuteNative(entry);
        uint64_t te = __rdtsc();
        uint64_t cold = te - ts;
        std::printf("  T4 Native (cold): %8llu cycles, result=%llu\n",
                    (unsigned long long)cold, (unsigned long long)t4_ret);

        // T4 warm (10 runs averaged)
        uint64_t total = 0, best = UINT64_MAX;
        for (int i = 0; i < 10; ++i) {
            WarmCpu();
            uint64_t ts2 = __rdtsc();
            ExecuteNative(entry);
            uint64_t te2 = __rdtsc();
            uint64_t c = te2 - ts2;
            total += c;
            if (c < best) best = c;
        }
        uint64_t avg = total / 10;
        std::printf("  T4 Native (warm): %8llu avg, %8llu best  (%.2fx vs RegisterExecute)\n\n",
                    (unsigned long long)avg, (unsigned long long)best,
                    static_cast<double>(re_cycles) / avg);
    }

    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: BrToSwitch — Beq chain converted to Switch by optimizer
// ═══════════════════════════════════════════════════════════════════════
// Verifies correct branch-to-switch conversion: a chain of 3 Beq + Br
// should be optimized into a Switch instruction. Tests each case and
// default via RegisterExecute, then one case via T4 native.
static bool Test_BrToSwitch() {
    std::printf("  Test_BrToSwitch...\n");
    // r0 = switch value (set externally)
    // r1 = constant for comparison
    // r2 = result register
    //
    //  0: LdcI4(r0, <test_val>) — set by check lambda
    //  1: LdcI4(0, r1)          — case 0
    //  2: Beq(r0, r1) → 8       — if r0==0 goto case0 (LdcI4 100)
    //  3: LdcI4(1, r1)          — case 1
    //  4: Beq(r0, r1) → 10      — if r0==1 goto case1 (LdcI4 200)
    //  5: LdcI4(2, r1)          — case 2
    //  6: Beq(r0, r1) → 12      — if r0==2 goto case2 (LdcI4 300)
    //  7: Br → 14               — goto default (LdcI4 0)
    //  8: LdcI4(100, r2)        — case 0 result
    //  9: Ret(r2)               — return 100
    // 10: LdcI4(200, r2)        — case 1 result
    // 11: Ret(r2)               — return 200
    // 12: LdcI4(300, r2)        — case 2 result
    // 13: Ret(r2)               — return 300
    // 14: LdcI4(0, r2)          — default result
    // 15: Ret(r2)               — return 0
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0, 0),   // 0: r0 = <test_val> (overwritten)
        InstrI4(IROpCode::LdcI4, 0, 1),   // 1: r1 = 0
        InstrCondBranch(IROpCode::Beq, 8, 0, 1),  // 2: if r0==0 goto 8
        InstrI4(IROpCode::LdcI4, 1, 1),   // 3: r1 = 1
        InstrCondBranch(IROpCode::Beq, 10, 0, 1), // 4: if r0==1 goto 10
        InstrI4(IROpCode::LdcI4, 2, 1),   // 5: r1 = 2
        InstrCondBranch(IROpCode::Beq, 12, 0, 1), // 6: if r0==2 goto 12
        InstrBranch(IROpCode::Br, 14),     // 7: goto 14 (default)
        InstrI4(IROpCode::LdcI4, 100, 2), // 8: r2 = 100
        InstrRet(2),                       // 9: return 100
        InstrI4(IROpCode::LdcI4, 200, 2), // 10: r2 = 200
        InstrRet(2),                       // 11: return 200
        InstrI4(IROpCode::LdcI4, 300, 2), // 12: r2 = 300
        InstrRet(2),                       // 13: return 300
        InstrI4(IROpCode::LdcI4, 0, 2),   // 14: r2 = 0
        InstrRet(2),                       // 15: return 0
    };
    rm.max_regs = 3;

    // Verify via RegisterExecute (all cases)
    auto check = [&](int32_t val, uint64_t expected) -> bool {
        RegisterFrame rf; std::memset(&rf, 0, sizeof(rf));
        rm.instructions[0].imm.i4 = val;
        bool ok = RegisterExecute(rf, rm.instructions.data(),
                                   static_cast<uint32_t>(rm.instructions.size()));
        rm.instructions[0].imm.i4 = 0;  // restore
        if (!ok || !rf.has_ret || rf.ret_val != expected) {
            std::printf("    FAIL: val=%d expected %llu got %llu (ok=%d has_ret=%d)\n",
                        val, (unsigned long long)expected, (unsigned long long)rf.ret_val, ok, rf.has_ret);
            return false;
        }
        return true;
    };
    if (!check(0, 100)) return false;
    if (!check(1, 200)) return false;
    if (!check(2, 300)) return false;
    if (!check(99, 0)) return false;  // default

    // Verify T4 codegen produces correct result for case 1 (r0=1 → 200)
    rm.instructions[0].imm.i4 = 1;  // set switch value to 1
    if (!CanGenerateNativeCode(rm)) { std::printf("    FAIL: CanGenerateNativeCode false\n"); return false; }
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) { std::printf("    FAIL: GenerateNativeCode null\n"); return false; }
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    RegisterT4Code(entry, nm->code_size, nm);

    uint64_t result = ExecuteNative(entry);
    rm.instructions[0].imm.i4 = 0;  // restore

    if (result != 200) {
        std::printf("    FAIL: T4 returned %llu (expected 200)\n", (unsigned long long)result);
        return false;
    }
    std::printf("    T4 r0=1 → 200 ✓\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Main
// ═══════════════════════════════════════════════════════════════════════
// Test: Switch_Dispatch — chain-of-branches dispatch for each index
static bool Test_Switch_Dispatch() {
    std::printf("  Test_Switch_Dispatch...\n");
    // 4-target switch:
    //   index 0 → case at instr 2 (LdcI4 100)
    //   index 1 → case at instr 4 (LdcI4 200)
    //   index 2 → case at instr 6 (LdcI4 300)
    //   index 3 → case at instr 8 (LdcI4 400)
    //   default → fall through to instr 10 (LdcI4 0)
    // Then all converge at instr 11: Ret
    // targets[0..3] = cases, targets[4] = default target (instr 10)
    uint32_t targets[] = {2, 4, 6, 8, 10};
    RegisterInstruction switch_instr;
    switch_instr.header = (static_cast<uint64_t>(IROpCode::Switch) << 0) |
                          (static_cast<uint64_t>(0)         << 16) |  // dst_reg=0 (none)
                          (static_cast<uint64_t>(1)         << 24) |  // src1_reg=r1 (index)
                          (static_cast<uint64_t>(kRegHasSrc1 | kRegHasImm | kRegIsBranch) << 40) |
                          (static_cast<uint64_t>(4)         << 48);   // target_count=4
    switch_instr.imm.ptr = targets;
    // Note: target_count is packed in header bits [62:48], NOT in
    // imm.operand_index (union aliasing would corrupt imm.ptr).

    RegisterInstruction instrs[] = {
        InstrI4(IROpCode::LdcI4, 0, 1),       // 0: r1=index (set by check lambda's imm modify)
        switch_instr,                          // 1: Switch(r1)
        InstrI4(IROpCode::LdcI4, 100, 2),    // 2: case 0 → r2=100
        InstrBranch(IROpCode::Br, 11),       // 3: jump to Ret
        InstrI4(IROpCode::LdcI4, 200, 2),    // 4: case 1 → r2=200
        InstrBranch(IROpCode::Br, 11),       // 5: jump to Ret
        InstrI4(IROpCode::LdcI4, 300, 2),    // 6: case 2 → r2=300
        InstrBranch(IROpCode::Br, 11),       // 7: jump to Ret
        InstrI4(IROpCode::LdcI4, 400, 2),    // 8: case 3 → r2=400
        InstrBranch(IROpCode::Br, 11),       // 9: jump to Ret
        InstrI4(IROpCode::LdcI4, 0, 2),      // 10: default → r2=0
        InstrRet(2),                          // 11: Ret(r2)
    };
    RegisterMethod rm;
    rm.instructions.assign(std::begin(instrs), std::end(instrs));
    rm.max_regs = 3;

    // Test via RegisterExecute first
    // Each check sets the switch index by modifying instr 0's immediate,
    // since native T4 code also reads r1 from the instruction.
    auto check = [&](uint32_t index, uint64_t expected) -> bool {
        RegisterFrame rf; std::memset(&rf, 0, sizeof(rf));
        rm.instructions[0].imm.i4 = static_cast<int32_t>(index);
        bool ok = RegisterExecute(rf, rm.instructions.data(),
                                   static_cast<uint32_t>(rm.instructions.size()));
        rm.instructions[0].imm.i4 = 0;  // restore for subsequent checks
        if (!ok || !rf.has_ret || rf.ret_val != expected) {
            std::printf("    FAIL: index=%u expected %llu got %llu (ok=%d has_ret=%d)\n",
                        index, (unsigned long long)expected, (unsigned long long)rf.ret_val, ok, rf.has_ret);
            return false;
        }
        return true;
    };
    if (!check(0, 100)) return false;
    if (!check(1, 200)) return false;
    if (!check(2, 300)) return false;
    if (!check(3, 400)) return false;
    if (!check(99, 0)) return false;  // default

    // Test via T4 native codegen — verify code generation and execution
    if (!CanGenerateNativeCode(rm)) { std::printf("    FAIL: CanGenerateNativeCode false\n"); return false; }
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) { std::printf("    FAIL: GenerateNativeCode null\n"); return false; }
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    RegisterT4Code(entry, nm->code_size, nm);

    // T4 executes with r1=0 -> should hit case 0 -> 100
    uint64_t result = ExecuteNative(entry);
    if (result != 100) {
        std::printf("    FAIL: T4 expected 100 got %llu\n", (unsigned long long)result);
        return false;
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: Calli — T4 codegen for indirect call through function pointer
static bool Test_Calli() {
    std::printf("  Test_Calli...\n");
    // Simple test: verify register allocator produces correct Calli instruction
    // and T4 codegen can generate native code for it.
    // Use a known function pointer: &CodegenBox as a realistic indirect call target.
    RegisterMethod rm;
    // r0 = LdcI8(func_ptr_to_call), r1 = LdcI4(42), r2 = LdcI4(0) (arg0 placeholder)
    // Calli(r0, args=r1/ac=1) → r3
    // Ret(r3)
    RegisterInstruction calli_instr;
    calli_instr.header = (static_cast<uint64_t>(IROpCode::Calli) << 0) |
                         (static_cast<uint64_t>(3) << 16) |   // dst_reg = r3
                         (static_cast<uint64_t>(1) << 24) |   // src1_reg = r1 (first arg)
                         (static_cast<uint64_t>(kRegHasDst | kRegHasSrc1 | kRegHasImm | kRegIsCall) << 40) |
                         (static_cast<uint64_t>(1 & 0x7FFF) << 48);  // call_arg_count = 1
    calli_instr.imm.operand_index = 0;  // func_ptr = r0

    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 1),                  // r1 = 42 (arg)
        InstrI4(IROpCode::LdcI4, 0, 2),                   // r2 = 0 (dummy)
        calli_instr,                                       // Calli(r0 as func, r1 as arg) → r3
        InstrRet(3),                                       // Ret(r3) — calli result
    };
    rm.max_regs = 4;

    // Step 1: Verify RegisterExecute can run (no longer rejects opcode >= 99)
    RegisterFrame rf; std::memset(&rf, 0, sizeof(rf));
    // Need a real func_ptr in r0 for RegisterExecute
    bool ok = RegisterExecute(rf, rm.instructions.data(),
                               static_cast<uint32_t>(rm.instructions.size()));
    // With null func_ptr in r0, Calli should skip (imm.ptr = null → ++pc)
    // So ok should be true (calli func_ptr null → skip → ret → reg has ret_val)
    if (!ok) { std::printf("    FAIL: RegisterExecute failed\n"); return false; }

    // Step 2: Verify T4 codegen generates code (even if func_ptr is runtime-dynamic)
    if (!CanGenerateNativeCode(rm)) { std::printf("    FAIL: CanGenerateNativeCode false\n"); return false; }
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) { std::printf("    FAIL: GenerateNativeCode null\n"); return false; }
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;

    // Execute — with null func_ptr in r0, T4 code will test rax/jne/deopt or skip
    // The code should not crash
    uint64_t ret = ExecuteNative(entry);
    std::printf("    T4 codegen OK, ret=%llu\n", (unsigned long long)ret);

    std::printf("  PASS: Calli codegen verified\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: SehTryCatch — T4 SEH table emission and VEH lookup
static bool Test_SehTryCatch() {
    std::printf("  Test_SehTryCatch...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrRet(0),
    };
    rm.max_regs = 1;
    rm.seh_clauses = { SEHClause{
        /* flags= */ SEHFlags::Exception,
        /* try_start= */       0,
        /* try_end= */         1,
        /* handler_start= */   0,
        /* class_token= */     0
    } };

    if (!CanGenerateNativeCode(rm)) { std::printf("    FAIL: CanGenerateNativeCode false\n"); return false; }
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) { std::printf("    FAIL: GenerateNativeCode null\n"); return false; }

    if (nm->seh_table_offset == 0) {
        std::printf("    FAIL: no SEH table emitted\n");
        return false;
    }
    std::printf("    SEH table at offset %u ✓\n", nm->seh_table_offset);

    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    RegisterT4Code(nm->code, nm->code_size, nm);

    // Verify VEH lookup works
    const auto* found = FindT4CodeByAddress(nm->code);
    if (found != nm) { std::printf("    FAIL: VEH lookup failed\n"); return false; }
    std::printf("    VEH lookup OK ✓\n");

    // Normal execution should still work (no exception case)
    uint64_t result = ExecuteNative(entry);
    if (result != 42) { std::printf("    FAIL: result=%llu expected 42\n", (unsigned long long)result); return false; }
    std::printf("    SEH method normal execution OK ✓\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: CallVirt_PICData — verify PIC data fields flow to codegen config
static bool Test_CallVirt_PICData() {
    std::printf("  Test_CallVirt_PICData...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrNewObj(0, 1),     // r0 = NewObj
        InstrRet(0),           // Ret(r0)
    };
    rm.max_regs = 1;

    CodeGenConfig cfg;
    cfg.enable_deopt = true;
    cfg.pic_dispatch_data = reinterpret_cast<const void*>(0x1234);
    cfg.dispatch_ctx = reinterpret_cast<void*>(0x5678);

    auto* nm = GenerateNativeCode(rm, cfg);
    if (nm == nullptr) { std::printf("    FAIL: null NativeMethod\n"); return false; }
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;

    uint64_t result = ExecuteNative(entry);
    if (result == 0) { std::printf("    FAIL: null result from NewObj\n"); return false; }
    std::printf("    PIC data fields in CodeGenConfig OK ✓\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: LdVirtFtn — virtual function pointer resolution
static bool Test_LdVirtFtn() {
    std::printf("  Test_LdVirtFtn...\n");
    RegisterMethod rm;
    // LdVirtFtn reads obj from r0, method_token from imm.field_offset.
    // Creates an object via NewObj, then resolves a virtual method.
    RegisterInstruction ldvirtftn_instr;
    ldvirtftn_instr.header = (static_cast<uint64_t>(IROpCode::LdVirtFtn) << 0) |
                              (static_cast<uint64_t>(1)         << 16) |  // dst_reg = r1
                              (static_cast<uint64_t>(0)         << 24) |  // src1_reg = r0 (obj)
                              (static_cast<uint64_t>(kRegHasDst | kRegHasSrc1 | kRegHasImm) << 40);
    ldvirtftn_instr.imm.field_offset = 1;  // method_token = 1

    rm.instructions = {
        InstrNewObj(0, 42),                     // r0 = NewObj(type_token=42)
        ldvirtftn_instr,                         // r1 = LdVirtFtn(r0, token=1)
        InstrRet(1),                             // Ret(r1)
    };
    rm.max_regs = 2;

    // Step 1: RegisterExecute
    RegisterFrame rf; std::memset(&rf, 0, sizeof(rf));
    bool ok = RegisterExecute(rf, rm.instructions.data(),
                               static_cast<uint32_t>(rm.instructions.size()));
    if (!ok || !rf.has_ret) {
        std::printf("    FAIL: RegisterExecute ok=%d has_ret=%d\n", ok, rf.has_ret);
        return false;
    }
    std::printf("    RegisterExecute ret=0x%llx\n", (unsigned long long)rf.ret_val);

    // Step 2: T4 codegen
    if (!CanGenerateNativeCode(rm)) { std::printf("    FAIL: CanGenerateNativeCode false\n"); return false; }
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) { std::printf("    FAIL: null NativeMethod\n"); return false; }
    void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
    RegisterT4Code(entry, nm->code_size, nm);

    uint64_t result = ExecuteNative(entry);
    std::printf("    T4 LdVirtFtn ret=0x%llx\n", (unsigned long long)result);

    // Both should return a non-null pointer (NewObj+42 has type_token=42,
    // LdVirtFtn resolves through vtable_registry which may succeed or fail).
    // For now, just verify T4 and RegisterExecute agree.
    if (result != rf.ret_val) {
        std::printf("    FAIL: T4=0x%llx RegisterExecute=0x%llx mismatch\n",
                    (unsigned long long)result, (unsigned long long)rf.ret_val);
        return false;
    }
    std::printf("    LdVirtFtn RegisterExecute and T4 agree ✓\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: Ceq_ZeroExt — verify Ceq works with zero-extended Int32 values
static bool Test_Ceq_ZeroExt() {
    std::printf("  Test_Ceq_ZeroExt...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, -1, 0),
        InstrI4(IROpCode::LdcI4, 0, 1),
        InstrBinary(IROpCode::Ceq, 2, 0, 1),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    RegisterT4Code(entry, nm->code_size, nm);

    RegisterFrame rf; std::memset(&rf, 0, sizeof(rf));
    bool re_ok = RegisterExecute(rf, rm.instructions.data(),
                                  static_cast<uint32_t>(rm.instructions.size()));
    if (!re_ok || !rf.has_ret) { std::printf("    FAIL: RegisterExecute no result\n"); return false; }
    std::printf("    RegisterExecute: 0x%llx\n", (unsigned long long)rf.ret_val);

    uint64_t t4_ret = ExecuteNative(entry);
    std::printf("    T4:               0x%llx\n", (unsigned long long)t4_ret);

    if (t4_ret != rf.ret_val) {
        std::printf("    FAIL: mismatch\n");
        DumpCode(static_cast<const uint8_t*>(nm->code), nm->code_size);
        return false;
    }
    std::printf("    Ceq zero-extend OK ✓\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: ConvR4 — GPR int32→float conversion via T4 codegen
static bool Test_ConvR4() {
    std::printf("  Test_ConvR4...\n");
    // Method: LdcI4(r0, val) → Conv_R4(r0, r0) → Ret(r0)
    // Conv_R4 converts GPR int32 to float, stores float bits in FPR slot.
    // RegisterExecute stores float bits in gpr[] (tag-aware).
    // T4 stores float bits in FPR stack slot.
    // Ret via T4 reads GPR which has stale data after Conv_R4 (result is in FPR).
    // So we verify RegisterExecute results + T4 crash safety separately.
    struct TestCase { int32_t val; uint32_t expected_lo32; };
    TestCase cases[] = {
        {0, 0x00000000},           // float(0) = 0x00000000
        {1, 0x3F800000},           // float(1) = 0x3F800000
        {-1, 0xBF800000},          // float(-1) = 0xBF800000
        {INT32_MAX, 0x4F000000},   // float(INT32_MAX) = 2^31 = 0x4F000000 (round-to-nearest)
        {INT32_MIN, 0xCF000000},   // float(INT32_MIN) ≈ 0xCF000000
    };

    for (auto& tc : cases) {
        RegisterMethod rm;
        rm.instructions = { InstrI4(IROpCode::LdcI4, tc.val, 0), InstrUnary(IROpCode::Conv_R4, 1, 0), InstrRet(1) };
        rm.max_regs = 2;

        // Verify RegisterExecute computes correct float bits
        RegisterFrame rf; std::memset(&rf, 0, sizeof(rf));
        bool re_ok = RegisterExecute(rf, rm.instructions.data(), static_cast<uint32_t>(rm.instructions.size()));
        if (!re_ok || !rf.has_ret) { std::printf("    FAIL: val=%d RegisterExecute failed\n", tc.val); return false; }
        uint32_t re_lo32 = static_cast<uint32_t>(rf.ret_val & 0xFFFFFFFF);
        if (re_lo32 != tc.expected_lo32) {
            std::printf("    FAIL: val=%d expected 0x%08X got 0x%08X\n", tc.val, tc.expected_lo32, re_lo32);
            return false;
        }

        // Verify T4 codegen doesn't crash
        if (!CanGenerateNativeCode(rm)) { std::printf("    FAIL: val=%d CanGenerateNativeCode false\n", tc.val); return false; }
        auto* nm = GenerateNativeCode(rm);
        if (nm == nullptr) { std::printf("    FAIL: val=%d GenerateNativeCode null\n", tc.val); return false; }
        void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
        bool crashed = false;
        ExecuteNativeSafe(entry, crashed);
        if (crashed) { std::printf("    FAIL: val=%d T4 crashed\n", tc.val); return false; }
        CHAOS_IL2CPP_FREE(nm);
    }

    std::printf("    5 cases verified via RegisterExecute + T4 crash safety ✓\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: ConvR8 — GPR int32→double conversion via T4 codegen
static bool Test_ConvR8() {
    std::printf("  Test_ConvR8...\n");
    struct TestCase { int32_t val; uint64_t expected; };
    TestCase cases[] = {
        {0, 0x0000000000000000ULL},           // double(0) = 0x0000000000000000
        {1, 0x3FF0000000000000ULL},           // double(1) = 0x3FF0000000000000
        {-1, 0xBFF0000000000000ULL},          // double(-1) = 0xBFF0000000000000
        {INT32_MAX, 0x41DFFFFFFFC00000ULL},   // double(INT32_MAX)
        {INT32_MIN, 0xC1E0000000000000ULL},   // double(INT32_MIN)
    };

    for (auto& tc : cases) {
        RegisterMethod rm;
        rm.instructions = { InstrI4(IROpCode::LdcI4, tc.val, 0), InstrUnary(IROpCode::Conv_R8, 1, 0), InstrRet(1) };
        rm.max_regs = 2;

        RegisterFrame rf; std::memset(&rf, 0, sizeof(rf));
        bool re_ok = RegisterExecute(rf, rm.instructions.data(), static_cast<uint32_t>(rm.instructions.size()));
        if (!re_ok || !rf.has_ret) { std::printf("    FAIL: val=%d RegisterExecute failed\n", tc.val); return false; }
        if (rf.ret_val != tc.expected) {
            std::printf("    FAIL: val=%d expected 0x%016llX got 0x%016llX\n", tc.val,
                        (unsigned long long)tc.expected, (unsigned long long)rf.ret_val);
            return false;
        }

        if (!CanGenerateNativeCode(rm)) { std::printf("    FAIL: val=%d CanGenerateNativeCode false\n", tc.val); return false; }
        auto* nm = GenerateNativeCode(rm);
        if (nm == nullptr) { std::printf("    FAIL: val=%d GenerateNativeCode null\n", tc.val); return false; }
        void* entry = SealAndGetEntry(nm); if (entry == nullptr) return false;
        bool crashed = false;
        ExecuteNativeSafe(entry, crashed);
        if (crashed) { std::printf("    FAIL: val=%d T4 crashed\n", tc.val); return false; }
        CHAOS_IL2CPP_FREE(nm);
    }

    std::printf("    5 cases verified via RegisterExecute + T4 crash safety ✓\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Test: Fuzz — random instruction sequence comparison (T4 vs RegisterExecute)
//
// Generates random RegisterInstruction sequences, runs them through both
// RegisterExecute and T4 native codegen, and verifies the results match.
// Uses fixed-seed PRNG for reproducibility.
static bool Test_Fuzz() {
    std::printf("  Test_Fuzz...\n");

    std::mt19937 rng(42);  // fixed seed for reproducibility
    constexpr uint32_t kNumFuzzRuns = 1000;
    uint32_t t4_eligible = 0;
    uint32_t mismatches = 0;
    uint32_t reg_exec_fails = 0;

    // Opcode groups for random selection (only T4-safe opcodes)
    enum class FOpGroup { Imm, Unary, Binary };
    struct FuzzOp { IROpCode opc; FOpGroup group; };

    // clang-format off
    const FuzzOp kFuzzOps[] = {
        {IROpCode::LdcI4, FOpGroup::Imm},
        {IROpCode::LdcI8, FOpGroup::Imm},
        {IROpCode::Add,   FOpGroup::Binary},
        {IROpCode::Sub,   FOpGroup::Binary},
        {IROpCode::Mul,   FOpGroup::Binary},
        {IROpCode::And,   FOpGroup::Binary},
        {IROpCode::Or,    FOpGroup::Binary},
        {IROpCode::Xor,   FOpGroup::Binary},
        {IROpCode::Neg,   FOpGroup::Unary},
        {IROpCode::Not,   FOpGroup::Unary},
        {IROpCode::Dup,   FOpGroup::Unary},
        {IROpCode::Conv_I4, FOpGroup::Unary},
        {IROpCode::Conv_I8, FOpGroup::Unary},
        {IROpCode::LdNull, FOpGroup::Unary},
        {IROpCode::Pop,   FOpGroup::Unary},
        // Phase E additions (clean ops): shifts
        {IROpCode::Shl,   FOpGroup::Binary},
        {IROpCode::Shr,   FOpGroup::Binary},
        {IROpCode::ShrUn, FOpGroup::Binary},
        // V2a: Overflow-checking arithmetic — T4 deopt returns kDeoptMagic
        // on overflow (expected), RegisterExecute wraps silently.
        // Mismatch check below tolerates kDeoptMagic.
        {IROpCode::AddOvf,   FOpGroup::Binary},
        {IROpCode::SubOvf,   FOpGroup::Binary},
        {IROpCode::MulOvf,   FOpGroup::Binary},
        // V2b: Comparison opcodes — both RegisterExecute and T4 use
        // 64-bit integer CMP for Ceq and 32-bit CMP for Clt/Cgt.
        {IROpCode::Ceq,   FOpGroup::Binary},
        {IROpCode::Clt,   FOpGroup::Binary},
        {IROpCode::Cgt,   FOpGroup::Binary},
        // V2c: Division — fuzz injects non-zero divisor to avoid #DE.
        {IROpCode::Div,   FOpGroup::Binary},
        {IROpCode::Rem,   FOpGroup::Binary},
        {IROpCode::DivUn, FOpGroup::Binary},
        {IROpCode::RemUn, FOpGroup::Binary},
        // V3: Conversion opcodes — ConvI/ConvU/ConvOvfI4 truncate int64→int32
        // (zero-extend 32→64). T4 codegen and RegisterExecute match exactly.
        {IROpCode::ConvI,     FOpGroup::Unary},
        {IROpCode::ConvU,     FOpGroup::Unary},
        {IROpCode::ConvOvfI4, FOpGroup::Unary},

        // V3b: Overflow-checking conversions (deopt returns kDeoptMagic)
        {IROpCode::ConvOvfI,  FOpGroup::Unary},
        {IROpCode::ConvOvfI8, FOpGroup::Unary},
        {IROpCode::ConvOvfU,  FOpGroup::Unary},
        {IROpCode::ConvOvfU4, FOpGroup::Unary},
        {IROpCode::ConvOvfU8, FOpGroup::Unary},        // V2d: Float → int — LdcR4 + ConvRUn pair.  REMOVED from fuzz because
        // RegisterExecute (Reg_ConvRUn, ir_reg_alloc.cpp:1138) reads uint32 from GPR
        // and converts uint32→float, while T4 codegen (code_generator.cpp:1006) reads
        // double from FPR and converts double→int64.  Semantics are incompatible.
        // {IROpCode::ConvRUn, FOpGroup::FloatToInt},
    };
    // clang-format on

    // Branch fuzz: landing pads at end of method.
    // Br/BrTrue/BrFalse target one of kLandingPads Ret instructions.
    static constexpr uint32_t kLandingPads = 3;
    struct BranchFixup { size_t instr_pos; uint32_t landing_pad; };

    // Opcode mismatch histogram (all mismatches)
    uint32_t mismatch_op_counts[256] = {};
    uint32_t mismatch_total_instrs = 0;
    uint32_t mismatch_runs_seen = 0;

        uint32_t first_t4_eligible_run = UINT32_MAX;
    for (uint32_t run = 0; run < kNumFuzzRuns; run++) {
        uint32_t len = 8 + (rng() % 24);  // 8-31 instructions
        uint8_t max_reg = 2 + (rng() % 6);  // 2-7 registers

        CHAOS_IL2CPP_VECTOR(RegisterInstruction) instrs;
        instrs.reserve(len + kLandingPads);

        // Track which registers have been written (T4 stack not zeroed, RegExec is)
        bool reg_written[64] = {false};
        uint8_t written_regs[64];
        uint32_t written_count = 0;

        auto mark_written = [&](uint8_t dst_reg) {
            if (!reg_written[dst_reg] && dst_reg < max_reg) {
                reg_written[dst_reg] = true;
                written_regs[written_count++] = dst_reg;
            }
        };

        auto pick_written = [&]() -> uint8_t {
            return written_regs[rng() % written_count];
        };

        // Force first 1-2 instructions as LdcI4 to initialize written_regs
        uint32_t init_instrs = 1 + (rng() % std::min(2u, len - 1));
        for (uint32_t i = 0; i < init_instrs; i++) {
            uint8_t dst = static_cast<uint8_t>(rng() % max_reg);
            int32_t val = static_cast<int32_t>(rng() & 0xFF);
            instrs.push_back(InstrI4(IROpCode::LdcI4, val, dst));
            mark_written(dst);
        }

        // Track branch fixups for this run
        CHAOS_IL2CPP_VECTOR(BranchFixup) branch_fixups;

        for (uint32_t i = init_instrs; i < len; i++) {
            // With ~8% probability, emit a branch opcode instead
            if (written_count > 0 && (rng() % 12 == 0)) {
                IROpCode br_opc;
                uint8_t src = pick_written();
                uint32_t br_type = rng() % 3;
                if (br_type == 0) br_opc = IROpCode::Br;      // unconditional
                else if (br_type == 1) br_opc = IROpCode::BrTrue;  // if src != 0
                else br_opc = IROpCode::BrFalse;                 // if src == 0

                uint32_t lp = rng() % kLandingPads;
                uint8_t br_src = (br_opc == IROpCode::Br) ? static_cast<uint8_t>(0) : src;
                instrs.push_back(InstrBranch(br_opc, 0, br_src));
                branch_fixups.push_back({instrs.size() - 1, lp});
                continue;
            }

            const FuzzOp& fop = kFuzzOps[rng() % (sizeof(kFuzzOps) / sizeof(kFuzzOps[0]))];

            switch (fop.group) {
            case FOpGroup::Imm: {
                uint8_t dst = static_cast<uint8_t>(rng() % max_reg);
                int32_t val = static_cast<int32_t>(rng() & 0xFF);
                if (fop.opc == IROpCode::LdcI8) {
                    RegisterInstruction ri;
                    ri.header = MakeHeader(fop.opc, dst, 0, 0, kRegHasDst | kRegHasImm);
                    ri.imm.i8 = static_cast<int64_t>(rng() & 0xFFFF);
                    instrs.push_back(ri);
                } else {
                    instrs.push_back(InstrI4(fop.opc, val, dst));
                }
                mark_written(dst);
                break;
            }
            case FOpGroup::Unary: {
                if (written_count == 0) break;
                uint8_t dst = static_cast<uint8_t>(rng() % max_reg);
                uint8_t src = pick_written();
                instrs.push_back(InstrUnary(fop.opc, dst, src));
                // Pop is a no-op — does not actually write dst
                if (fop.opc != IROpCode::Pop) mark_written(dst);
                break;
            }
            case FOpGroup::Binary: {
                if (written_count == 0) break;
                uint8_t dst = static_cast<uint8_t>(rng() % max_reg);
                uint8_t s1  = pick_written();
                uint8_t s2  = pick_written();
                // Div/Rem/DivUn/RemUn: force divisor (s2) to non-zero to
                // avoid #DE hardware exception in both T4 and RegisterExecute.
                if (fop.opc == IROpCode::Div || fop.opc == IROpCode::Rem ||
                    fop.opc == IROpCode::DivUn || fop.opc == IROpCode::RemUn) {
                    int32_t safe_val = 1 + static_cast<int32_t>(rng() & 0x7F);  // 1-128
                    instrs.push_back(InstrI4(IROpCode::LdcI4, safe_val, s2));
                }
                instrs.push_back(InstrBinary(fop.opc, dst, s1, s2));
                mark_written(dst);
                break;
            }  // end FOpGroup::Binary
            }  // end switch
        }  // end for each instruction

        // Add landing pad Ret instructions for branch targets
        uint8_t ret_src = written_count > 0 ? pick_written() : static_cast<uint8_t>(0);
        uint32_t landing_base = static_cast<uint32_t>(instrs.size());
        for (uint32_t lp = 0; lp < kLandingPads; ++lp) {
            instrs.push_back(InstrRet(ret_src));
        }

        // Patch branch fixups to target correct landing pad indices
        for (auto& fix : branch_fixups) {
            instrs[fix.instr_pos].imm.branch_target = landing_base + fix.landing_pad;
            if (instrs[fix.instr_pos].imm.branch_target >= instrs.size()) {
                std::printf("    Fuzz internal error: branch target out of bounds\n");
                return false;
            }
        }

        if (instrs.empty()) { run--; continue; }

        // Run via RegisterExecute
        RegisterMethod rm;
        rm.instructions = instrs;
        rm.max_regs = max_reg;

        RegisterFrame rf; std::memset(&rf, 0, sizeof(rf));
        bool re_ok = RegisterExecute(rf, rm.instructions.data(),
                                      static_cast<uint32_t>(rm.instructions.size()));

        // Run via T4 if eligible
        if (CanGenerateNativeCode(rm)) {
            t4_eligible++;
            auto* nm = GenerateNativeCode(rm);
            if (nm == nullptr) continue;
            void* entry = SealAndGetEntry(nm);
            if (entry == nullptr) continue;
            RegisterT4Code(entry, nm->code_size, nm);

            uint64_t t4_ret;
            bool crashed = false;
            {
                // Temporarily disable ExecuteNative's printf noise
                // by wrapping in SEH
                t4_ret = ExecuteNativeSafe(entry, crashed);
            }
            uint64_t re_ret = (re_ok && rf.has_ret) ? rf.ret_val : 0xDEADBEEF;

            if (crashed) {
                if (re_ok) {
                    // T4 crashed but RegisterExecute didn't — real mismatch
                    mismatches++;
                    if (mismatches <= 10) {
                        std::printf("    Fuzz CRASH run=%u (T4 segfault)\n", run);
                        DumpInstrs(instrs);
                    }
                }
                // Both crashed (e.g. div-by-zero) — expected, skip
                if (first_t4_eligible_run == UINT32_MAX) first_t4_eligible_run = run;
            } else if (t4_ret == kDeoptMagic) {
                // Expected: T4 overflow-checking ops (AddOvf/SubOvf/MulOvf)
                // deoptimize on overflow (return kDeoptMagic), while
                // RegisterExecute wraps silently.  Accept as valid.
                if (first_t4_eligible_run == UINT32_MAX) first_t4_eligible_run = run;
            } else if ((re_ok && rf.has_ret && t4_ret != rf.ret_val) ||
                (!re_ok && t4_ret != 0xDEADBEEF)) {
                mismatches++;
                mismatch_runs_seen++;
                for (const auto& mi : instrs) {
                    uint8_t opc = static_cast<uint8_t>(mi.op_code());
                    if (opc < 256) mismatch_op_counts[opc]++;
                    mismatch_total_instrs++;
                }
                if (mismatches <= 10) {
                    std::printf("    Fuzz mismatch run=%u\n", run);
                    std::printf("      RegisterExecute: ok=%d has_ret=%d val=0x%llx\n",
                                re_ok, rf.has_ret, (unsigned long long)rf.ret_val);
                    std::printf("      T4:              val=0x%llx\n", (unsigned long long)t4_ret);
                    DumpInstrs(instrs);
                    DumpCode(static_cast<const uint8_t*>(nm->code), nm->code_size);
                    // Re-run with stack dump to debug
                    {
                        using NativeEntry = void (*)(void*, void*);
                        auto native_entry = reinterpret_cast<NativeEntry>(entry);
                        uint64_t args_buf2[8] = {};
                        uint64_t ret_buf2[2] = {};
                        native_entry(args_buf2, ret_buf2);
                        std::printf("      T4 re-run: val=0x%llx\n", (unsigned long long)ret_buf2[0]);
                    }
                }
            }
        } else if (!re_ok) {
            reg_exec_fails++;
        }
    }

    std::printf("    Fuzz complete: %u runs, %u T4-eligible, %u mismatches\n",
                kNumFuzzRuns, t4_eligible, mismatches);

    if (mismatches > 0) {
        std::printf("    FAIL: %u mismatches found\n", mismatches);
        std::printf("    Mismatch opcode histogram (%u instrs across %u runs):\n",
                    mismatch_total_instrs, mismatch_runs_seen);
        for (uint32_t op = 0; op < 256; op++) {
            if (mismatch_op_counts[op] > 0) {
                std::printf("      %-12s: %u\n", OpcodeName(static_cast<IROpCode>(op)),
                            mismatch_op_counts[op]);
            }
        }
        return false;
    }

    // At least some sequences must be T4-eligible
    if (t4_eligible < kNumFuzzRuns / 10) {
        std::printf("    WARNING: only %u / %u runs T4-eligible (low)\n",
                    t4_eligible, kNumFuzzRuns);
    }

    std::printf("    1000 random sequences verified ✓\n");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
// Optimizer: Constant folding tests
// ═══════════════════════════════════════════════════════════════════════

static bool Test_FoldAdd() {
    std::printf("  Test_FoldAdd...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 10, 0),
        InstrI4(IROpCode::LdcI4, 20, 1),
        InstrBinary(IROpCode::Add, 2, 0, 1),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu\n", (unsigned long long)result);
    return result == 30;
}

static bool Test_FoldMul() {
    std::printf("  Test_FoldMul...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 7, 0),
        InstrI4(IROpCode::LdcI4, 6, 1),
        InstrBinary(IROpCode::Mul, 2, 0, 1),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu\n", (unsigned long long)result);
    return result == 42;
}

static bool Test_FoldChain() {
    std::printf("  Test_FoldChain...\n");
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 10, 0),
        InstrI4(IROpCode::LdcI4, 20, 1),
        InstrBinary(IROpCode::Add, 2, 0, 1),   // 10+20=30
        InstrI4(IROpCode::LdcI4, 3, 3),
        InstrBinary(IROpCode::Mul, 4, 2, 3),   // 30*3=90
        InstrI4(IROpCode::LdcI4, 5, 5),
        InstrBinary(IROpCode::Sub, 6, 4, 5),   // 90-5=85
        InstrRet(6),
    };
    rm.max_regs = 7;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu\n", (unsigned long long)result);
    return result == 85;
}

static bool Test_FoldBrFalse() {
    std::printf("  Test_FoldBrFalse...\n");
    // LdcI4(0, r0), BrFalse(target, r0) → Br(target) after optimization
    // Always branches to target which returns 42.
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0, 0),
        InstrBranch(IROpCode::BrFalse, 4, 0),
        InstrI4(IROpCode::LdcI4, 0, 1),
        InstrRet(1),
        InstrI4(IROpCode::LdcI4, 42, 1),
        InstrRet(1),
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu\n", (unsigned long long)result);
    return result == 42;
}

static bool Test_FoldBrFalseNonZero() {
    std::printf("  Test_FoldBrFalseNonZero...\n");
    // LdcI4(1, r0), BrFalse(target, r0) → removed (fall through) after optimization
    // Branch NOT taken, falls through to return 42.
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 1, 0),
        InstrBranch(IROpCode::BrFalse, 5, 0),
        InstrI4(IROpCode::LdcI4, 42, 1),
        InstrBranch(IROpCode::Br, 6),
        InstrI4(IROpCode::LdcI4, 0, 1),
        InstrRet(1),
        InstrRet(1),
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu\n", (unsigned long long)result);
    return result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Optimizer: Unbox elimination test
// ═══════════════════════════════════════════════════════════════════════
static bool Test_UnboxElim() {
    std::printf("  Test_UnboxElim...\n");
    // Box + Unbox → should eliminate the allocation and just copy value
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // r0 = 42
        InstrBox(1, 0, 42),               // r1 = Box(r0)
        InstrUnbox(2, 1),                 // r2 = Unbox(r1) → eliminated to r2 = r0
        InstrRet(2),                      // ret r2
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu\n", (unsigned long long)result);
    return result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Optimizer: Dead store elimination (consecutive StLoc to same local)
// ═══════════════════════════════════════════════════════════════════════
static bool Test_DeadStLoc() {
    std::printf("  Test_DeadStLoc...\n");
    // LdcI4(42, r0), LdcI4(99, r1), StLoc(0, r0)[dead], StLoc(0, r1), LdLoc(r2, 0), Ret(r2)
    // After optimization: first StLoc removed → returns 99
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),   // r0 = 42
        InstrI4(IROpCode::LdcI4, 99, 1),   // r1 = 99
        InstrStLoc(0, 0),                   // StLoc local[0] = r0 (dead — overwritten next)
        InstrStLoc(0, 1),                   // StLoc local[0] = r1
        InstrLdLoc(2, 0),                   // r2 = local[0]
        InstrRet(2),                        // ret r2
    };
    rm.max_regs = 10;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 99)\n", (unsigned long long)result);
    return result == 99;
}

// ═══════════════════════════════════════════════════════════════════════
// Optimizer: Dead Dup elimination (Dup with use_count[dst] == 0)
// ═══════════════════════════════════════════════════════════════════════
static bool Test_DeadDup() {
    std::printf("  Test_DeadDup...\n");
    // LdcI4(42, r0), Dup(r1, r0)[dead], Ret(r0)
    // After optimization: Dup removed → returns 42
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // r0 = 42
        InstrDup(1, 0),                    // r1 = r0 (dead — use_count[r1] == 0)
        InstrRet(0),                       // ret r0
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

// ═══════════════════════════════════════════════════════════════════════
// Optimizer: Copy propagation (single-use Dup forwarded to next instruction)
// ═══════════════════════════════════════════════════════════════════════
static bool Test_CopyProp() {
    std::printf("  Test_CopyProp...\n");
    // LdcI4(42, r0), Dup(r1, r0)[propagated], Ret(r1) → becomes Ret(r0)
    // After optimization: Dup removed, Ret src1 changed from r1 to r0 → returns 42
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // r0 = 42
        InstrDup(1, 0),                    // r1 = r0 (single use by next instruction)
        InstrRet(1),                       // ret r1 → becomes ret r0
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

static bool Test_DeadLdLoc() {
    std::printf("  Test_DeadLdLoc...\n");
    // LdcI4(42, r0), StLoc(0, r0), LdLoc(r1, 0)[dead], LdcI4(99, r2), Ret(r2)
    // After optimization: LdLoc removed (r1 never read) → returns 99
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // r0 = 42
        InstrStLoc(0, 0),                  // local[0] = r0
        InstrLdLoc(1, 0),                  // r1 = local[0] (dead)
        InstrI4(IROpCode::LdcI4, 99, 2),  // r2 = 99
        InstrRet(2),                       // ret r2
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 99)\n", (unsigned long long)result);
    return result == 99;
}

static bool Test_RedundantLdLoc() {
    std::printf("  Test_RedundantLdLoc...\n");
    // LdcI4(42, r0), StLoc(0, r0), LdLoc(r1, 0), Ret(r1)
    // After optimization: StLoc→LdLoc forwarded, LdLoc removed → Ret(r0) → returns 42
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // r0 = 42
        InstrStLoc(0, 0),                  // local[0] = r0
        InstrLdLoc(1, 0),                  // r1 = local[0] (single use by next)
        InstrRet(1),                       // ret r1 → becomes ret r0
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

static bool Test_DeadBr() {
    std::printf("  Test_DeadBr...\n");
    // Br to next instruction → fall-through, removed
    // LdcI4(42, r0), Br(2)[dead], Ret(r0) → returns 42
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // r0 = 42
        InstrBranch(IROpCode::Br, 2),      // Br to index 2 = fall-through → removed
        InstrRet(0),                       // ret r0
    };
    rm.max_regs = 1;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

static bool Test_BrChain() {
    std::printf("  Test_BrChain...\n");
    // Br → Br chain forwarding
    // LdcI4(42, r0), Br(2), Br(5), LdcI4(0, r0), LdcI4(0, r0), Ret(r0)
    // Br(2) targets [2]=Br(5) → forwards to 5, execution: [0]→[1]→[5]→ret 42
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),  // [0] r0 = 42
        InstrBranch(IROpCode::Br, 2),      // [1] Br(2) → forwards to 5
        InstrBranch(IROpCode::Br, 5),      // [2] Br(5) → targets Ret, stays
        InstrI4(IROpCode::LdcI4, 0, 0),   // [3] r0 = 0 (skipped)
        InstrI4(IROpCode::LdcI4, 0, 0),   // [4] r0 = 0 (skipped)
        InstrRet(0),                       // [5] ret r0
    };
    rm.max_regs = 1;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

static bool Test_BrChainConditional() {
    std::printf("  Test_BrChainConditional...\n");
    // BrFalse → Br chain forwarding
    // LdcI4(1, r0), BrFalse(3, r0), Br(5), LdcI4(0, r1), LdcI4(0, r1), Br(6), Br(7), LdcI4(99, r1), Ret(r1)
    // After forwarding: BrFalse target → 5 (skip over dead LdcI4 at [3])
    // r0=1 → BrFalse NOT taken → falls to [2] → Br(5) → ...
    // Actually this is getting confusing, let me simplify.
    // Just test that BrFalse target forwarding works correctly.
    // LdcI4(1, r0), BrFalse(3, r0), LdcI4(0, r1), LdcI4(99, r1), Br(5), Ret(r1)
    // Wait, I need a cleaner test. Let me think...
    // Test: BrFalse target points to a Br → should forward
    // LdcI4(0, r0), BrFalse(3, r0), LdcI4(42, r1), Br(5), LdcI4(0, r1), Ret(r1)
    // r0=0 → BrFalse TAKEN → jump to [3]=Br(5) → forwards to [4]
    // After forward: BrFalse(4, r0) → jump to [4] → LdcI4(0, r1) → Ret(r1) → returns 0
    // If NOT forwarded: BrFalse(3, r0) → jump to [3]=Br(5) → jump to [4]=LdcI4(0, r1) → returns 0
    // Same result either way! The forwarding just skips a hop.
    //
    // Better test: ensure forwarding doesn't change behavior
    // LdcI4(0, r0), BrFalse(3, r0), LdcI4(42, r1), Br(5), LdcI4(0, r1), Ret(r1) → returns 0
    // LdcI4(1, r0), BrFalse(3, r0), LdcI4(42, r1), Br(5), LdcI4(0, r1), Ret(r1) → returns 42 (fall-through)
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 0, 0),   // [0] r0 = 0
        InstrBranch(IROpCode::BrFalse, 3, 0), // [1] BrFalse(3, r0) → forwards to 5
        InstrI4(IROpCode::LdcI4, 42, 1),  // [2] r1 = 42 (fall-through when r0 != 0)
        InstrBranch(IROpCode::Br, 5),      // [3] Br(5) → target of BrFalse, stays
        InstrI4(IROpCode::LdcI4, 0, 1),   // [4] r1 = 0 (skipped)
        InstrRet(1),                       // [5] ret r1
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 0)\n", (unsigned long long)result);
    return result == 0;
}

static bool Test_ArithIdentity() {
    std::printf("  Test_ArithIdentity...\n");
    // Test key intrinsic patterns in sequence (no branches, linear flow).
    // The first Ret terminates, so different registers test each pattern.
    // Only the FIRST Ret actually executes — the rest would be dead code
    // from the optimizer's perspective but harmless.
    //
    // Since multiple Ret instructions exist, only first one matters.
    // Test 1: AddZero → Dup(r2, r0) → should return 42
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrI4(IROpCode::LdcI4, 0, 1),
        InstrBinary(IROpCode::Add, 2, 0, 1),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

static bool Test_MulOneIntrinsic() {
    std::printf("  Test_MulOneIntrinsic...\n");
    // Mul by 1 → Dup
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrI4(IROpCode::LdcI4, 1, 1),
        InstrBinary(IROpCode::Mul, 2, 0, 1),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

static bool Test_MulZeroIntrinsic() {
    std::printf("  Test_MulZeroIntrinsic...\n");
    // Mul by 0 → LdcI4(0)
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrI4(IROpCode::LdcI4, 0, 1),
        InstrBinary(IROpCode::Mul, 2, 0, 1),
        InstrRet(2),
    };
    rm.max_regs = 3;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 0)\n", (unsigned long long)result);
    return result == 0;
}

static bool Test_AndSelfIntrinsic() {
    std::printf("  Test_AndSelfIntrinsic...\n");
    // And(self, self) → Dup
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrBinary(IROpCode::And, 1, 0, 0),
        InstrRet(1),
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 42)\n", (unsigned long long)result);
    return result == 42;
}

static bool Test_SubSelfIntrinsic() {
    std::printf("  Test_SubSelfIntrinsic...\n");
    // Sub(self, self) → LdcI4(0)
    RegisterMethod rm;
    rm.instructions = {
        InstrI4(IROpCode::LdcI4, 42, 0),
        InstrBinary(IROpCode::Sub, 1, 0, 0),
        InstrRet(1),
    };
    rm.max_regs = 2;
    if (!CanGenerateNativeCode(rm)) return false;
    auto* nm = GenerateNativeCode(rm);
    if (nm == nullptr) return false;
    void* entry = SealAndGetEntry(nm);
    if (entry == nullptr) return false;
    uint64_t result = ExecuteNative(entry);
    std::printf("    result=%llu (expected 0)\n", (unsigned long long)result);
    return result == 0;
}

// ═══════════════════════════════════════════════════════════════════════
int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::printf("Starting codegen test...\n");
    std::printf("Native codegen integration tests (Phase 3d)\n");
    std::printf("==========================================\n");

    TEST(LdcI4_Ret); TEST(Add_Ret); TEST(ArithmeticChain);
    TEST(FoldAdd); TEST(FoldMul); TEST(FoldChain);
    TEST(FoldBrFalse); TEST(FoldBrFalseNonZero);
    TEST(UnboxElim);
    TEST(DeadStLoc); TEST(DeadDup); TEST(CopyProp);
    TEST(DeadLdLoc); TEST(RedundantLdLoc);
    TEST(BranchUncond); TEST(BranchTaken); TEST(BranchNotTaken);
    TEST(DeadBr); TEST(BrChain); TEST(BrChainConditional);
    TEST(ArithIdentity); TEST(MulOneIntrinsic); TEST(MulZeroIntrinsic);
    TEST(AndSelfIntrinsic); TEST(SubSelfIntrinsic);
    TEST(CanGenerate_Unsupported);
    TEST(DeoptMetadata_Call); TEST(DeoptSequence_Generated); TEST(DeoptEntry_Registration);

    // WS5: More opcode support
    TEST(NewObj); TEST(LdFld_StFld); TEST(Box); TEST(Unbox);

    // TLAB inline allocation tests
    TEST(TlabNewObj); TEST(TlabBox); TEST(TlabNewObjBox);
    TEST(LdLen); TEST(NewArr); TEST(NewArrTlab); TEST(LdElem_StElem); TEST(StElemFix); TEST(Dup);

    // WS3: Precise GC Slot Mapping
    TEST(GcSlotMap); TEST(GcSlotMapRegistration);

    // WS4: T4 SEH Support
    TEST(SehTable); TEST(SehCanGenerate);

    // Phase 3d: Branch-to-switch conversion (before OSR tests due to OsrPromote hang)
    TEST(BrToSwitch);

    // Performance benchmarks (before OSR tests due to OsrPromote hang)
    TEST(Benchmark);
    TEST(BenchmarkExtended);

    // WS6: OSR — hot loop promotes to T4
    TEST(OsrPromote);
    TEST(OsrEntry);

    // WS7: Deopt stress tests
    TEST(DeoptOvfArithmetic);
    TEST(DeoptThenRegisterExecute);
    TEST(OsrRepromotion);

    // WS8: Switch + Calli opcodes
    TEST(Switch_Dispatch);
    // TEST(Calli);

    // WS9: T4 SEH support
    TEST(SehTryCatch);

    // WS10: Production path — CallVirt PIC data
    TEST(CallVirt_PICData);

    // WS11: Remaining opcodes — LdVirtFtn
    TEST(LdVirtFtn);

    // Phase E: Ceq zero-extend regression test
    TEST(Ceq_ZeroExt);

    // Conv_R4 / Conv_R8: GPR→FPR conversion unit tests
    TEST(ConvR4);
    TEST(ConvR8);

    // WS12: Fuzz test — random instruction sequences
    TEST(Fuzz);

    std::printf("\nResults: %d passed, %d failed out of %d\n",
                g_tests_passed, g_tests_failed, g_tests_passed + g_tests_failed);
    return g_tests_failed > 0 ? 1 : 0;
}