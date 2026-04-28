// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/boxing/unboxing-casts
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::boxing_unboxing_casts
{
    // [0] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    int32_t BenchmarkEntry_10() {
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
        &BenchmarkEntry_10
    };
    static constexpr std::size_t kBenchmarkEntryCount = 11;

}  // namespace chaos::benchmark::boxing_unboxing_casts

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::boxing_unboxing_casts::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::boxing_unboxing_casts::kBenchmarkEntries[entryIndex]();
}
