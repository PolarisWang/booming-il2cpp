// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/collections/generic-core
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::collections_generic_core
{
    // [0] System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)
    int32_t BenchmarkEntry_5() {
        return 42;
    }

    // [6] System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)
    int32_t BenchmarkEntry_11() {
        return static_cast<int32_t>(42);
    }

    // [12] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)
    int32_t BenchmarkEntry_12() {
        return static_cast<int32_t>(42);
    }

    // [13] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)
    int32_t BenchmarkEntry_13() {
        return static_cast<int32_t>(42);
    }

    // [14] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)
    int32_t BenchmarkEntry_14() {
        return static_cast<int32_t>(42);
    }

    // [15] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)
    int32_t BenchmarkEntry_15() {
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
        &BenchmarkEntry_9,
        &BenchmarkEntry_10,
        &BenchmarkEntry_11,
        &BenchmarkEntry_12,
        &BenchmarkEntry_13,
        &BenchmarkEntry_14,
        &BenchmarkEntry_15
    };
    static constexpr std::size_t kBenchmarkEntryCount = 16;

}  // namespace chaos::benchmark::collections_generic_core

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::collections_generic_core::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::collections_generic_core::kBenchmarkEntries[entryIndex]();
}
