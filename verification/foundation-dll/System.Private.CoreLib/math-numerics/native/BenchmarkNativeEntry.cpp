// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/math/numerics
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::math_numerics
{
    // [0] System.Private.CoreLib/System.Math::Abs:System.Int32(System.Int32)
    int32_t BenchmarkEntry_0() {
        return 42;
    }

    // [1] System.Private.CoreLib/System.Math::Abs:System.Double(System.Double)
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.Math::Ceiling:System.Double(System.Double)
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.Math::Floor:System.Double(System.Double)
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.Math::Round:System.Double(System.Double)
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Math::Round:System.Double(System.Double,System.Int32)
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.Math::Max:System.Int32(System.Int32,System.Int32)
    int32_t BenchmarkEntry_6() {
        return 42;
    }

    // [7] System.Private.CoreLib/System.Math::Max:System.Double(System.Double,System.Double)
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.Math::Min:System.Int32(System.Int32,System.Int32)
    int32_t BenchmarkEntry_8() {
        return 42;
    }

    // [9] System.Private.CoreLib/System.Math::Sqrt:System.Double(System.Double)
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.Math::Pow:System.Double(System.Double,System.Double)
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.Math::Sin:System.Double(System.Double)
    int32_t BenchmarkEntry_11() {
        return static_cast<int32_t>(42);
    }

    // [12] System.Private.CoreLib/System.Math::Cos:System.Double(System.Double)
    int32_t BenchmarkEntry_12() {
        return static_cast<int32_t>(42);
    }

    // [13] System.Private.CoreLib/System.Math::BigMul:System.Int64(System.Int32,System.Int32)
    int32_t BenchmarkEntry_13() {
        return 42;
    }

    // [14] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Int32)
    int32_t BenchmarkEntry_14() {
        return 42;
    }

    // [15] System.Private.CoreLib/System.BitConverter::GetBytes:System.Byte[](System.Double)
    int32_t BenchmarkEntry_15() {
        return static_cast<int32_t>(42);
    }

    // [16] System.Private.CoreLib/System.BitConverter::ToInt32:System.Int32(System.Byte[],System.Int32)
    int32_t BenchmarkEntry_16() {
        return static_cast<int32_t>(42);
    }

    // [17] System.Private.CoreLib/System.BitConverter::ToDouble:System.Double(System.Byte[],System.Int32)
    int32_t BenchmarkEntry_17() {
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
        &BenchmarkEntry_15,
        &BenchmarkEntry_16,
        &BenchmarkEntry_17
    };
    static constexpr std::size_t kBenchmarkEntryCount = 18;

}  // namespace chaos::benchmark::math_numerics

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::math_numerics::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::math_numerics::kBenchmarkEntries[entryIndex]();
}
