// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/collections/generic-core
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::collections_generic_core
{
    // [0] System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return 42;
    }

    // [6] System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [13] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_13() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [14] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_14() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [15] System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_15() {
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
        &BenchmarkEntry_13,
        &BenchmarkEntry_14,
        &BenchmarkEntry_15
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 16;

}  // namespace chaos::benchmark::collections_generic_core

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::collections_generic_core::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::collections_generic_core::kBenchmarkEntries[entryIndex]();
}
