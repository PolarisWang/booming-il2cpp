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
#include "jit_method.h"
#include "jit_seh.h"

// OsrResolveLoopHeader is extern "C" — declare it here for direct calls.
extern "C" void* OsrResolveLoopHeader() noexcept;

using chaos::il2cpp::jit::t_deopt_state;
using chaos::il2cpp::jit::DeoptTlsState;
using chaos::il2cpp::jit::RegisterNativeCodeSection;
using chaos::il2cpp::jit::UnregisterT4Code;
using chaos::il2cpp::jit::FindNativeCodeByAddress;
using chaos::il2cpp::jit::JitMethod;

namespace {

// ── Heap-allocated JitMethod guard ─────────────────────────────────────
// Creates a zero-initialized JitMethod on the heap, registers with
// RegisterNativeCodeSection.  On destruction, unregisters and clears all pointer fields
// so ~JitMethod() won't free stack/heap pointers twice.
class NativeMethodGuard {
public:
    NativeMethodGuard(void* code, uint32_t code_size,
                      uint32_t* instr_offsets = nullptr,
                      uint32_t instr_count = 0)
        : nm_(new JitMethod())
        , code_(code) {
        std::memset(nm_, 0, sizeof(JitMethod));
        nm_->code = code;
        nm_->code_size = code_size;
        nm_->instr_offsets = instr_offsets;
        nm_->instr_offset_count = instr_count;
    }
    ~NativeMethodGuard() {
        UnregisterT4Code(code_);
        // Clear all heap pointers so ~JitMethod() doesn't free them.
        std::memset(nm_, 0, sizeof(JitMethod));
        delete nm_;
    }
    JitMethod* get() { return nm_; }
    void register_t4(uint32_t token = 0) {
        RegisterNativeCodeSection(code_, nm_->code_size, nm_, token);
    }
private:
    JitMethod* nm_;
    void* code_;
};

TEST(CodegenOsr, ResolveLoopHeaderReturnsNullWhenNotInT4Code) {
    void* result = OsrResolveLoopHeader();
    EXPECT_EQ(result, nullptr);
}

TEST(CodegenOsr, DeoptTlsStateDefaults) {
    EXPECT_EQ(t_deopt_state.osr_resume_pc, 0u);
    EXPECT_EQ(t_deopt_state.instr_pc, 0u);
    EXPECT_FALSE(t_deopt_state.deopt_happened);
}

TEST(CodegenOsr, DeoptTlsStateSetAndClear) {
    t_deopt_state.osr_resume_pc = 42;
    t_deopt_state.instr_pc = 100;
    t_deopt_state.deopt_happened = true;

    EXPECT_EQ(t_deopt_state.osr_resume_pc, 42u);
    EXPECT_EQ(t_deopt_state.instr_pc, 100u);
    EXPECT_TRUE(t_deopt_state.deopt_happened);

    t_deopt_state = DeoptTlsState{};
    EXPECT_EQ(t_deopt_state.osr_resume_pc, 0u);
    EXPECT_EQ(t_deopt_state.instr_pc, 0u);
    EXPECT_FALSE(t_deopt_state.deopt_happened);
}

TEST(CodegenOsr, RegisterThenUnregisterEntry) {
    // Stack-allocate code (JitMethod is heap-allocated via guard).
    uint8_t fake_code[128] = {};
    NativeMethodGuard nmg(fake_code, sizeof(fake_code));

    RegisterNativeCodeSection(fake_code, sizeof(fake_code), nmg.get(), /*token=*/0);
    EXPECT_NE(FindNativeCodeByAddress(fake_code), nullptr);

    UnregisterT4Code(fake_code);
    EXPECT_EQ(FindNativeCodeByAddress(fake_code), nullptr);
}

TEST(CodegenOsr, OsrResumePcBoundsCheckUt) {
    uint32_t offsets[] = {0, 10, 20, 30};
    uint8_t fake_code[128] = {};
    NativeMethodGuard nmg(fake_code, sizeof(fake_code), offsets, 4);

    nmg.register_t4(/*token=*/0);
    EXPECT_NE(FindNativeCodeByAddress(fake_code), nullptr);

    t_deopt_state = DeoptTlsState{};
}

}  // namespace
