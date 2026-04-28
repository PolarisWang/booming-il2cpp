// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/convert/char
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::convert_char
{
    // [0] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean)
    int32_t BenchmarkEntry_0() {
        return static_cast<uint8_t>(42);
    }

    // [1] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Byte)
    int32_t BenchmarkEntry_1() {
        return static_cast<uint8_t>(42);
    }

    // [2] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Char)
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>('A');
    }

    // [3] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.DateTime)
    int32_t BenchmarkEntry_3() {
        return static_cast<int64_t>(0);
    }

    // [4] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Decimal)
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Double)
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int16)
    int32_t BenchmarkEntry_6() {
        return static_cast<int16_t>(42);
    }

    // [7] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)
    int32_t BenchmarkEntry_7() {
        return 42;
    }

    // [8] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)
    int32_t BenchmarkEntry_8() {
        return static_cast<int64_t>(42);
    }

    // [9] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object)
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Object,System.IFormatProvider)
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.SByte)
    int32_t BenchmarkEntry_11() {
        return static_cast<int8_t>(42);
    }

    // [12] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Single)
    int32_t BenchmarkEntry_12() {
        return static_cast<int32_t>(42);
    }

    // [13] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String)
    int32_t BenchmarkEntry_13() {
        return static_cast<int32_t>(42);
    }

    // [14] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.String,System.IFormatProvider)
    int32_t BenchmarkEntry_14() {
        return static_cast<int32_t>(42);
    }

    // [15] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt16)
    int32_t BenchmarkEntry_15() {
        return static_cast<uint16_t>(42);
    }

    // [16] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt32)
    int32_t BenchmarkEntry_16() {
        return 42u;
    }

    // [17] System.Private.CoreLib/System.Convert::ToChar:System.Char(System.UInt64)
    int32_t BenchmarkEntry_17() {
        return static_cast<uint64_t>(42);
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
        &BenchmarkEntry_15,
        &BenchmarkEntry_16,
        &BenchmarkEntry_17
    };
    static constexpr std::size_t kBenchmarkEntryCount = 18;

}  // namespace chaos::benchmark::convert_char

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::convert_char::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::convert_char::kBenchmarkEntries[entryIndex]();
}
