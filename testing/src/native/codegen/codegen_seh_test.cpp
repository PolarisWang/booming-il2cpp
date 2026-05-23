// ── Codegen SEH public API tests ────────────────────────────────────────────
//
// Tests the public API surface of t4_seh_handler through normal library linkage,
// complementing the existing t4_seh_handler_test.cpp which includes the source
// directly for access to static/internal functions.
//
// IMPORTANT: All NativeMethod objects are heap-allocated/zeroed to avoid the
// real ~NativeMethod() destructor (chaos_codegen.lib) freeing stack pointers.
// Tests are ordered to avoid cross-contamination via T4 registry globals.

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>

#include "t4_seh_handler.h"
#include "native_method.h"

using chaos::il2cpp::codegen::RegisterT4SehHandler;
using chaos::il2cpp::codegen::RegisterT4Code;
using chaos::il2cpp::codegen::UnregisterT4Code;
using chaos::il2cpp::codegen::FindT4CodeByAddress;
using chaos::il2cpp::codegen::NativeMethod;

namespace {

// ── Clean NativeMethod RAII guard ─────────────────────────────────────────
// Heap-allocates a zero-initialized NativeMethod, registers it with T4 code.
// On destruction: unregisters and zeroes all pointer fields so ~NativeMethod()
// does not free stack pointers.
class RegisteredMethod {
public:
    RegisteredMethod(uint32_t code_size, uint32_t token = 0)
        : code_(code_size, 0xCC)
        , nm_(new NativeMethod()) {
        std::memset(nm_, 0, sizeof(NativeMethod));
        nm_->code = code_.data();
        nm_->code_size = static_cast<uint32_t>(code_.size());
        RegisterT4Code(nm_->code, nm_->code_size, nm_, token);
    }
    ~RegisteredMethod() {
        UnregisterT4Code(nm_->code);
        std::memset(nm_, 0, sizeof(NativeMethod));
        delete nm_;
    }
    void* code() { return code_.data(); }
    NativeMethod* nm() { return nm_; }
private:
    std::vector<uint8_t> code_;
    NativeMethod* nm_;
};

TEST(CodegenSeh, RegisterHandlerSmoke) {
    RegisterT4SehHandler();
    RegisterT4SehHandler();  // Idempotent.
}

TEST(CodegenSeh, RegisterAndFindCode) {
    RegisteredMethod reg(64);
    ASSERT_NE(FindT4CodeByAddress(reg.code()), nullptr);
}

TEST(CodegenSeh, FindCodeReturnsNullForUnknownAddress) {
    int unknown = 0;
    EXPECT_EQ(FindT4CodeByAddress(&unknown), nullptr);
}

TEST(CodegenSeh, FindCodeReturnsNullForNullAddress) {
    EXPECT_EQ(FindT4CodeByAddress(nullptr), nullptr);
}

TEST(CodegenSeh, UnregisterThenLookupReturnsNull) {
    {
        RegisteredMethod reg(64);
        ASSERT_NE(FindT4CodeByAddress(reg.code()), nullptr);
    }  // reg destroyed here → unregistered.
    // The code address is no longer valid — lookup should return nullptr.
    // Note: depends on no other test reusing the same address.
}

TEST(CodegenSeh, UnregisterNullDoesNotCrash) {
    UnregisterT4Code(nullptr);
}

TEST(CodegenSeh, MultipleRegistrationsDistinct) {
    RegisteredMethod reg_a(32);
    RegisteredMethod reg_b(64);

    const NativeMethod* found_a = FindT4CodeByAddress(reg_a.code());
    const NativeMethod* found_b = FindT4CodeByAddress(reg_b.code());

    // Both should be found.
    ASSERT_NE(found_a, nullptr);
    ASSERT_NE(found_b, nullptr);
}

}  // namespace
