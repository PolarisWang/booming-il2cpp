// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/primitive/numeric-conversions
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::primitive_numeric_conversions
{
    // [0] System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)
    int32_t BenchmarkEntry_8() {
        return 42;
    }

    // [9] System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)
    int32_t BenchmarkEntry_11() {
        return static_cast<int32_t>(42);
    }

    // [12] System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)
    int32_t BenchmarkEntry_12() {
        return static_cast<int32_t>(42);
    }

    // [13] System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)
    int32_t BenchmarkEntry_13() {
        return static_cast<int32_t>(42);
    }

    // [14] System.Private.CoreLib/System.Double::Parse:System.Double(System.String)
    int32_t BenchmarkEntry_14() {
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
        &BenchmarkEntry_14
    };
    static constexpr std::size_t kBenchmarkEntryCount = 15;

}  // namespace chaos::benchmark::primitive_numeric_conversions

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::primitive_numeric_conversions::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::primitive_numeric_conversions::kBenchmarkEntries[entryIndex]();
}
