// Auto-generated benchmark native entry
// Family: family/System.Runtime.InteropServices/marshalling-attributes
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::marshalling_attributes
{
    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::MarshalAsAttribute::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::get_Value:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.MarshalAsAttribute::set_Value:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.OptionalAttribute::OptionalAttribute::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.PreserveSigAttribute::PreserveSigAttribute::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    using BenchmarkEntryFn = CHAOS_IL2CPP_INT32 (*)();
    static constexpr BenchmarkEntryFn kBenchmarkEntries[] = {
        &BenchmarkEntry_0,
        &BenchmarkEntry_1,
        &BenchmarkEntry_2,
        &BenchmarkEntry_3,
        &BenchmarkEntry_4
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 5;

}  // namespace chaos::benchmark::marshalling_attributes

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::marshalling_attributes::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::marshalling_attributes::kBenchmarkEntries[entryIndex]();
}
