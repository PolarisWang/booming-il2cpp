// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/object/equality-identity
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::object_equality_identity
{
    // [0] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object)
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.Object::ReferenceEquals:System.Boolean(System.Object,System.Object)
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.Object::GetHashCode:System.Int32()
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.Object::ToString:System.String()
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Object::GetType:System.Type()
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    int32_t BenchmarkEntry_6() {
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
        &BenchmarkEntry_6
    };
    static constexpr std::size_t kBenchmarkEntryCount = 7;

}  // namespace chaos::benchmark::object_equality_identity

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::object_equality_identity::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::object_equality_identity::kBenchmarkEntries[entryIndex]();
}
