// entry_stubs.cpp — Link-time stubs for test framework symbols
// These stubs provide the Assert_Reset/Assert_Complete entry points
// referenced by the TPG-generated runtime-entry.cpp.  The Chaos.TestFramework.Sdk
// prebuilt lib does not export these symbols, so they are compiled from source
// as part of the SDK runtime_stubs/ collection.
//
// Copied to chaos-sdk/runtime_stubs/ by SdkEmitter.CopyRuntimeStubSources().

#include <cstdint>

extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset()
{
}

extern "C" std::int32_t Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete()
{
    return 0;
}
