// Auto-generated benchmark native entry
// Family: family/System.Text.Json/converter-factory
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::converter_factory
{
    // [0] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::JsonConverterFactory::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Property:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::Method1:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::Method2:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::Validate:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Value:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::set_Value:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Count:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::set_Count:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_IsEmpty:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::set_IsEmpty:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::get_Key:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::set_Key:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
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
        &BenchmarkEntry_12
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 13;

}  // namespace chaos::benchmark::converter_factory

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::converter_factory::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::converter_factory::kBenchmarkEntries[entryIndex]();
}
