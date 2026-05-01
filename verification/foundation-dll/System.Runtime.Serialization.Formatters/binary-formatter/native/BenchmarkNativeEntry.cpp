// Auto-generated benchmark native entry
// Family: family/System.Runtime.Serialization.Formatters/binary-formatter
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::binary_formatter
{
    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::BinaryFormatter::Void(System.Object, System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Deserialize:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::Serialize:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_AssemblyFormat:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Binder:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_Context:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_FilterLevel:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_SurrogateSelector:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::get_TypeFormat:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_AssemblyFormat:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Binder:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_Context:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [13] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_FilterLevel:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_13() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [14] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_SurrogateSelector:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_14() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [15] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.Binary.BinaryFormatter::set_TypeFormat:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_15() {
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
        &BenchmarkEntry_10,
        &BenchmarkEntry_11,
        &BenchmarkEntry_12,
        &BenchmarkEntry_13,
        &BenchmarkEntry_14,
        &BenchmarkEntry_15
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 16;

}  // namespace chaos::benchmark::binary_formatter

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::binary_formatter::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::binary_formatter::kBenchmarkEntries[entryIndex]();
}
