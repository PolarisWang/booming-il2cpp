// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/threading/monitor-interlocked
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::threading_monitor_interlocked
{
    // [0] System.Private.CoreLib/System.Threading.Monitor::Enter:System.Void(System.Object)
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.Threading.Monitor::Exit:System.Void(System.Object)
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object)
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.Threading.Monitor::TryEnter:System.Boolean(System.Object,System.Int32)
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.Threading.Monitor::Pulse:System.Void(System.Object)
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Threading.Monitor::PulseAll:System.Void(System.Object)
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.Threading.Monitor::Wait:System.Boolean(System.Object)
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.Threading.Interlocked::Increment:System.Int32(System.Int32&)
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.Threading.Interlocked::Decrement:System.Int32(System.Int32&)
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.Threading.Interlocked::Exchange:System.Int32(System.Int32&,System.Int32)
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.Threading.Interlocked::CompareExchange:System.Int32(System.Int32&,System.Int32,System.Int32)
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.Threading.Interlocked::Add:System.Int32(System.Int32&,System.Int32)
    int32_t BenchmarkEntry_11() {
        return static_cast<int32_t>(42);
    }

    // [12] System.Private.CoreLib/System.Threading.Interlocked::MemoryBarrier:System.Void()
    int32_t BenchmarkEntry_12() {
        return static_cast<int32_t>(42);
    }

    // [13] System.Private.CoreLib/System.Threading.Volatile::Read:System.Int32(System.Int32&)
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

}  // namespace chaos::benchmark::threading_monitor_interlocked

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::threading_monitor_interlocked::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::threading_monitor_interlocked::kBenchmarkEntries[entryIndex]();
}
