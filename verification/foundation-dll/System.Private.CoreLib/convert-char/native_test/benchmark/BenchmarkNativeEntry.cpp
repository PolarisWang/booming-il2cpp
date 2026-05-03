// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/convert/char
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::convert_char
{
    // [0] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_UINT8>(42);
    }

    // [1] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_UINT64>(42);
    }

    using BenchmarkEntryFn = CHAOS_IL2CPP_INT32 (*)();
    static constexpr BenchmarkEntryFn kBenchmarkEntries[] = {
        &BenchmarkEntry_0,
        &BenchmarkEntry_1
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 2;

}  // namespace chaos::benchmark::convert_char

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::convert_char::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::convert_char::kBenchmarkEntries[entryIndex]();
}
