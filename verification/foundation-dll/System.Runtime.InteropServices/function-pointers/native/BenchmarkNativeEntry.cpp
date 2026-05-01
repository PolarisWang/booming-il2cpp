// Auto-generated benchmark native entry
// Family: family/System.Runtime.InteropServices/function-pointers
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::function_pointers
{
    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute::UnmanagedCallersOnlyAttribute::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    using BenchmarkEntryFn = CHAOS_IL2CPP_INT32 (*)();
    static constexpr BenchmarkEntryFn kBenchmarkEntries[] = {
        &BenchmarkEntry_0
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 1;

}  // namespace chaos::benchmark::function_pointers

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::function_pointers::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::function_pointers::kBenchmarkEntries[entryIndex]();
}
