// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/generic/nullable-value
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::generic_nullable_value
{
    // [0] System.Private.CoreLib/System.Nullable`1::get_HasValue:System.Boolean()
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.Nullable`1::get_Value:T()
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T()
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.Nullable`1::GetValueOrDefault:T(T)
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.Nullable`1::Equals:System.Boolean(System.Object)
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Nullable`1::GetHashCode:System.Int32()
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.Nullable`1::ToString:System.String()
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.ValueType::GetHashCode:System.Int32()
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.ValueType::ToString:System.String()
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    using BenchmarkEntryFn = int32_t (*)();
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
        &BenchmarkEntry_9
    };
    static constexpr std::size_t kBenchmarkEntryCount = 10;

}  // namespace chaos::benchmark::generic_nullable_value

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::generic_nullable_value::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::generic_nullable_value::kBenchmarkEntries[entryIndex]();
}
