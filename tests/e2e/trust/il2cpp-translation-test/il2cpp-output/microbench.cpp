// microbench.cpp — default empty stub for RunMicrobench.
// Link this file to enable --microbench mode; replace with a real microbench
// implementation when benchmarking specific runtime internals.
//
// This file also exposes a verification stub that calls Program.Main(0)
// and prints its return code so the AOT pipeline's semantic correctness
// can be checked independently of the fact-mode crash/no-crash loop.
#include <cstdio>
#include <cstdint>

// Declared in generated native-aot.generated.cpp (extern "C" linkage).
extern "C" std::int32_t Il2CppTranslationTest_Program_Main_System_String(
    void* chaos_fn_arg_0);

extern "C" void RunMicrobench() {
    // Call Program.Main with a null args pointer (unused by the method).
    auto main_result = Il2CppTranslationTest_Program_Main_System_String(nullptr);
    std::printf("AOT.Program.Main() = %d\n", static_cast<int>(main_result));
    std::printf("Expected: 0 (all checks passed)\n");
}
