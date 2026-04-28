// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/array/indexing-copy
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::array_indexing_copy
{
    // [0] System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)
    int32_t BenchmarkEntry_11() {
        return 42;
    }

    // [12] System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)
    int32_t BenchmarkEntry_12() {
        return 42;
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
        &BenchmarkEntry_12
    };
    static constexpr std::size_t kBenchmarkEntryCount = 13;

}  // namespace chaos::benchmark::array_indexing_copy

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::array_indexing_copy::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::array_indexing_copy::kBenchmarkEntries[entryIndex]();
}
