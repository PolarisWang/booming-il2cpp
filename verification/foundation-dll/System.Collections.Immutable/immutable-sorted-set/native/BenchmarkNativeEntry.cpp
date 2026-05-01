// Auto-generated benchmark native entry
// Family: family/System.Collections.Immutable/immutable-sorted-set
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::immutable_sorted_set
{
    // [0] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::ImmutableSortedSet::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::get_Property:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Method1:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Method2:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::Validate:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::get_Value:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::set_Value:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::get_Count:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::set_Count:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::get_IsEmpty:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::set_IsEmpty:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::get_Key:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.Collections.Immutable/System.Collections.Immutable.ImmutableSortedSet::set_Key:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
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
        &BenchmarkEntry_12
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 13;

}  // namespace chaos::benchmark::immutable_sorted_set

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::immutable_sorted_set::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::immutable_sorted_set::kBenchmarkEntries[entryIndex]();
}
