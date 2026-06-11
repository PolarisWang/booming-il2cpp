// entry_stubs.cpp ¡ª Link-time stubs for test framework symbols
#include <cstdint>
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset() {}
extern "C" std::int32_t Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete() { return 0; }
