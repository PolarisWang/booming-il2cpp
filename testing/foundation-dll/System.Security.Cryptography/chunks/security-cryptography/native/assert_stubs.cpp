// assert_stubs.cpp — Stub implementations for test framework assertion symbols
// Provides definitions so the binary links even when the test framework
// SDK assembly is not AOT-compiled (e.g., manual TPG runs without --assembly-dir).
// When the test framework IS AOT-compiled, those real definitions override.

#include <cstdint>
#include <chaos/native_types.h>

extern "C" CHAOS_IL2CPP_INT32 Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete() noexcept
{
    // Stub: no assertions tracked locally, always return "no failures".
    return 0;
}

extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset() noexcept
{
    // Stub: no-op.
}
