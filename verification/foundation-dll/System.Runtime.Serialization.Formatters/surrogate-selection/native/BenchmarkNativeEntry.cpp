// Auto-generated benchmark native entry
// Family: family/System.Runtime.Serialization.Formatters/surrogate-selection
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::surrogate_selection
{
    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::GetObjectData:System.Void(P0, P1, P2)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISerializationSurrogate::SetObjectData:System.Void(P0, P1, P2, P3)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::ChainSelector:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetNextSelector:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ISurrogateSelector::GetSurrogate:System.Void(P0, P1, P2)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::SurrogateSelector::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::AddSurrogate:System.Void(P0, P1, P2)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::ChainSelector:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetNextSelector:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::GetSurrogate:System.Void(P0, P1, P2)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.SurrogateSelector::RemoveSurrogate:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    using BenchmarkEntryFn = CHAOS_IL2CPP_INT32 (*)();
    static constexpr BenchmarkEntryFn kBenchmarkEntries[] = {
        &BenchmarkEntry_0,
        &BenchmarkEntry_1,
        &BenchmarkEntry_2,
        &BenchmarkEntry_3,
        &BenchmarkEntry_4,
        &BenchmarkEntry_5,
        &BenchmarkEntry_6,
        &BenchmarkEntry_7,
        &BenchmarkEntry_8,
        &BenchmarkEntry_9,
        &BenchmarkEntry_10
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 11;

}  // namespace chaos::benchmark::surrogate_selection

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::surrogate_selection::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::surrogate_selection::kBenchmarkEntries[entryIndex]();
}
