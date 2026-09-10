/// method_replacement_smoke_test.cpp — Method replacement smoke test (gtest)
///
/// Exercises: Register, Resolve, Revert, RevertAll, ActiveCount.

#include "method_replacement.h"

#include <gtest/gtest.h>

#include <cstdint>

// AOT symbol stubs (required by chaos_interpreter linkage)
extern "C" const char* kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int kChaosExternalRuntimeCount = 0;
extern "C" void* chaos_il2cpp_aot_hotpatch_module = nullptr;

namespace {

void* ReplacementThunk() {
    return reinterpret_cast<void*>(0x42u);
}

}  // namespace

TEST(MethodReplacement, Smoke) {
    using chaos::il2cpp::method_replacement::ActiveCount;
    using chaos::il2cpp::method_replacement::Register;
    using chaos::il2cpp::method_replacement::Resolve;
    using chaos::il2cpp::method_replacement::Revert;
    using chaos::il2cpp::method_replacement::RevertAll;

    RevertAll();

    EXPECT_TRUE(Register(0x06000001u, reinterpret_cast<void*>(&ReplacementThunk)));

    EXPECT_NE(Resolve(0x06000001u), nullptr);

    EXPECT_EQ(ActiveCount(), 1);

    EXPECT_TRUE(Revert(0x06000001u));

    EXPECT_EQ(ActiveCount(), 0);
}
