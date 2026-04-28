// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/span/memory-buffers
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::span_memory_buffers
{
    // [0] System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)
    int32_t BenchmarkEntry_0() {
        return 42;
    }

    // [1] System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)
    int32_t BenchmarkEntry_1() {
        return 42;
    }

    // [2] System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)
    int32_t BenchmarkEntry_2() {
        return 42;
    }

    // [3] System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)
    int32_t BenchmarkEntry_3() {
        return 42;
    }

    // [4] System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)
    int32_t BenchmarkEntry_6() {
        return 42;
    }

    // [7] System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)
    int32_t BenchmarkEntry_7() {
        return 42;
    }

    // [8] System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)
    int32_t BenchmarkEntry_9() {
        return 42;
    }

    // [10] System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.Memory`1::ToArray:T[]()
    int32_t BenchmarkEntry_11() {
        return static_cast<int32_t>(42);
    }

    // [12] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)
    int32_t BenchmarkEntry_12() {
        return static_cast<int32_t>(42);
    }

    // [13] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)
    int32_t BenchmarkEntry_13() {
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
        &BenchmarkEntry_13
    };
    static constexpr std::size_t kBenchmarkEntryCount = 14;

}  // namespace chaos::benchmark::span_memory_buffers

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::span_memory_buffers::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::span_memory_buffers::kBenchmarkEntries[entryIndex]();
}
