// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/span/memory-buffers
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::span_memory_buffers
{
    // [0] System.Private.CoreLib/System.Span`1::get_Item:T&(System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return 42;
    }

    // [1] System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return 42;
    }

    // [2] System.Private.CoreLib/System.Span`1::Slice:System.Span`1(System.Int32,System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return 42;
    }

    // [3] System.Private.CoreLib/System.Span`1::ToArray:T[](System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return 42;
    }

    // [4] System.Private.CoreLib/System.Span`1::CopyTo:System.Void(System.Span`1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Private.CoreLib/System.Span`1::get_Empty:System.Span`1()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Private.CoreLib/System.ReadOnlySpan`1::get_Item:T&(System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return 42;
    }

    // [7] System.Private.CoreLib/System.ReadOnlySpan`1::Slice:System.ReadOnlySpan`1(System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return 42;
    }

    // [8] System.Private.CoreLib/System.ReadOnlySpan`1::ToArray:T[]()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Private.CoreLib/System.Memory`1::Slice:System.Memory`1(System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return 42;
    }

    // [10] System.Private.CoreLib/System.Memory`1::get_Span:System.Span`1()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.Private.CoreLib/System.Memory`1::ToArray:T[]()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.ReadOnlySpan`1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [13] System.Private.CoreLib/System.Runtime.InteropServices.MemoryMarshal::GetReference:T&(System.Span`1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_13() {
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
        &BenchmarkEntry_12,
        &BenchmarkEntry_13
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 14;

}  // namespace chaos::benchmark::span_memory_buffers

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::span_memory_buffers::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::span_memory_buffers::kBenchmarkEntries[entryIndex]();
}
