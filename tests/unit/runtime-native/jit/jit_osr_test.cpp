// ── Codegen OSR unit tests ──────────────────────────────────────────────────
//
// Tests OsrResolveLoopHeader edge cases and DeoptTlsState management.
// Complements the integration-level Test_OsrEntry in codegen_native_test.cpp.
//
// OsrResolveLoopHeader uses _ReturnAddress() to find the calling T4 method.
// When called directly from test code (not from within registered T4 code),
// it returns nullptr — this validates the null-guard path.
//
// IMPORTANT: All JitMethod objects are heap-allocated to avoid the real
// ~JitMethod() destructor (in chaos_codegen.lib) from freeing stack
// pointers via CHAOS_IL2CPP_FREE.

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

#include "jit_helpers.h"
#include "jit_engine.h"   // kFprFileOff / kFprSlotSize / frame layout constants
#include "jit_deopt.h"    // DeoptTrap
#include "jit_method.h"
#include "jit_seh.h"

// OsrResolveLoopHeader is extern "C" — declare it here for direct calls.
extern "C" void* OsrResolveLoopHeader() noexcept;

using chaos::il2cpp::jit::DeoptRuntime;
using chaos::il2cpp::jit::DeoptTlsState;
using chaos::il2cpp::jit::FindNativeCodeByAddress;
using chaos::il2cpp::jit::g_jit_deopt_state;
using chaos::il2cpp::jit::JitMethod;
using chaos::il2cpp::jit::NativeContext;
using chaos::il2cpp::jit::RegisterNativeCodeSection;
using chaos::il2cpp::jit::UnregisterNativeCodeSection;

// Codegen frame-layout constants (defined in namespace chaos::il2cpp::jit in
// jit_engine.h).  Import the ones this TU references so the regression test
// mirrors the producer's FprOff() stride exactly.
using chaos::il2cpp::jit::kFprCount;
using chaos::il2cpp::jit::kFprFileOff;
using chaos::il2cpp::jit::kFprSlotSize;

namespace {

// ── Heap-allocated JitMethod guard ─────────────────────────────────────
// Creates a zero-initialized JitMethod on the heap, registers with
// RegisterNativeCodeSection.  On destruction, unregisters and clears all pointer fields
// so ~JitMethod() won't free stack/heap pointers twice.
class NativeMethodGuard {
public:
    NativeMethodGuard(void* code, uint32_t code_size, uint32_t* instr_offsets = nullptr, uint32_t instr_count = 0)
        : nm_(new JitMethod()), code_(code) {
        std::memset(nm_, 0, sizeof(JitMethod));
        nm_->code = code;
        nm_->code_size = code_size;
        nm_->instr_offsets = instr_offsets;
        nm_->instr_offset_count = instr_count;
    }
    ~NativeMethodGuard() {
        UnregisterNativeCodeSection(code_);
        // Clear all heap pointers so ~JitMethod() doesn't free them.
        std::memset(nm_, 0, sizeof(JitMethod));
        delete nm_;
    }
    JitMethod* get() { return nm_; }
    void register_t4(uint32_t token = 0) { RegisterNativeCodeSection(code_, nm_->code_size, nm_, token); }

private:
    JitMethod* nm_;
    void* code_;
};

TEST(CodegenOsr, ResolveLoopHeaderReturnsNullWhenNotInT4Code) {
    void* result = OsrResolveLoopHeader();
    EXPECT_EQ(result, nullptr);
}

TEST(CodegenOsr, DeoptTlsStateDefaults) {
    EXPECT_EQ(g_jit_deopt_state.osr_resume_pc, 0u);
    EXPECT_EQ(g_jit_deopt_state.instr_pc, 0u);
    EXPECT_FALSE(g_jit_deopt_state.deopt_happened);
}

TEST(CodegenOsr, DeoptTlsStateSetAndClear) {
    g_jit_deopt_state.osr_resume_pc = 42;
    g_jit_deopt_state.instr_pc = 100;
    g_jit_deopt_state.deopt_happened = true;

    EXPECT_EQ(g_jit_deopt_state.osr_resume_pc, 42u);
    EXPECT_EQ(g_jit_deopt_state.instr_pc, 100u);
    EXPECT_TRUE(g_jit_deopt_state.deopt_happened);

    g_jit_deopt_state = DeoptTlsState {};
    EXPECT_EQ(g_jit_deopt_state.osr_resume_pc, 0u);
    EXPECT_EQ(g_jit_deopt_state.instr_pc, 0u);
    EXPECT_FALSE(g_jit_deopt_state.deopt_happened);
}

TEST(CodegenOsr, RegisterThenUnregisterEntry) {
    // Stack-allocate code (JitMethod is heap-allocated via guard).
    uint8_t fake_code[128] = {};
    NativeMethodGuard nmg(fake_code, sizeof(fake_code));

    RegisterNativeCodeSection(fake_code, sizeof(fake_code), nmg.get(), /*token=*/0);
    EXPECT_NE(FindNativeCodeByAddress(fake_code), nullptr);

    // T2.3-C 方案3: demoted/Unregistered code stays alive and address-registered
    // (never reused), so an in-flight old frame's return address stays
    // GC/SEH-resolvable.  Unregister marks it externally-managed but does NOT
    // remove it from the lookup registry.
    UnregisterNativeCodeSection(fake_code);
    EXPECT_NE(FindNativeCodeByAddress(fake_code), nullptr);
    EXPECT_TRUE(nmg.get()->code_managed_externally);
}

TEST(CodegenOsr, OsrResumePcBoundsCheckUt) {
    uint32_t offsets[] = {0, 10, 20, 30};
    uint8_t fake_code[128] = {};
    NativeMethodGuard nmg(fake_code, sizeof(fake_code), offsets, 4);

    nmg.register_t4(/*token=*/0);
    EXPECT_NE(FindNativeCodeByAddress(fake_code), nullptr);

    g_jit_deopt_state = DeoptTlsState {};
}

// ── FPR deopt spill-stride regression guard ────────────────────────────────
// DeoptTrap (and DeoptSaveFrameState) read the FPR spill slots back from the
// codegen stack frame.  The producer (StoreFpr / FprOff in jit_engine.h) lays
// each virtual FPR out kFprSlotSize bytes apart (32 on x64, 16 on ARM64).
// The consumers previously read at a fixed 8-byte stride, which desyncs every
// virtual FPR >= 65 (it re-reads the tail of the previous slot's value).
// This test lays out the frame exactly like codegen does and asserts the
// stride-kFprSlotSize readback so a regression to stride 8 fails deterministically.
TEST(CodegenOsr, DeoptTrapReadsFprSlotsAtFprSlotSizeStride) {
    // Codegen x64 frame: GPR file @ [32..544), FPR file @ [kFprFileOff..), each
    // FPR slot = kFprSlotSize bytes.  Allocate enough for the FPR file plus tail.
    constexpr uint32_t kBufSize = kFprFileOff + kFprCount * kFprSlotSize + 16;
    alignas(64) uint8_t frame[kBufSize] = {};
    const uint64_t codegen_rsp = reinterpret_cast<uint64_t>(frame);

    // Write a distinct double into each FPR slot's base (the low 8 bytes of the
    // slot that StoreFpr / DeoptTrap read as double).  All other bytes stay 0.
    double expected[kFprCount];
    for (uint32_t vr = 0; vr < kFprCount; ++vr) {
        expected[vr] = static_cast<double>(1000.0 + vr); // distinct, non-trivial
        std::memcpy(frame + kFprFileOff + vr * kFprSlotSize, &expected[vr], sizeof(expected[vr]));
    }

    // DeoptTrap batch-copies GPR+FPR from the frame into out_gpr_file/out_fpr_file.
    // FindEntry returns nullptr for a zeroed JitMethod (no deopt_entries), which is
    // handled safely — the batch copy happens before the entry lookup.
    JitMethod nm = {};
    NativeContext ctx; // unused for the batch copy when no deopt values
    uint64_t out_gpr[64] = {};
    double out_fpr[kFprCount] = {};
    DeoptRuntime::DeoptTrap(&nm, /*return_address=*/0, ctx, codegen_rsp,
                            out_gpr, out_fpr, nullptr, nullptr);

    // Every virtual FPR >= 0 must read back the value at its OWN slot (stride
    // kFprSlotSize).  A stride-8 bug reads vr>=1 from the previous slot's tail.
    for (uint32_t vr = 0; vr < kFprCount; ++vr) {
        EXPECT_DOUBLE_EQ(out_fpr[vr], expected[vr])
            << "FPR virtual reg " << vr << " read back wrong value: stride mismatch";
    }
}

} // namespace
