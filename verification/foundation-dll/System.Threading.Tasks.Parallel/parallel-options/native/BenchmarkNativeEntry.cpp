// Auto-generated benchmark native entry
// Family: family/System.Threading.Tasks.Parallel/parallel-options
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::parallel_options
{
    // [0] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::ParallelOptions::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Property:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::Method1:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::Method2:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::Validate:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Value:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_Value:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Count:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_Count:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_IsEmpty:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_IsEmpty:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::get_Key:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.Threading.Tasks.Parallel/System.Threading.Tasks.ParallelOptions::set_Key:System.Void(P0)
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

}  // namespace chaos::benchmark::parallel_options

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::parallel_options::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::parallel_options::kBenchmarkEntries[entryIndex]();
}
