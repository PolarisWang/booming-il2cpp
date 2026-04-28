// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/threading/tasks-primitives
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::threading_tasks_primitives
{
    // [0] System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task(System.Action)
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.Threading.Tasks.Task::Run:System.Threading.Tasks.Task`1(System.Func`1)
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.Threading.Tasks.Task::Delay:System.Threading.Tasks.Task(System.Int32)
    int32_t BenchmarkEntry_2() {
        return 42;
    }

    // [3] System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Void()
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.Threading.Tasks.Task::Wait:System.Boolean(System.Int32)
    int32_t BenchmarkEntry_4() {
        return 42;
    }

    // [5] System.Private.CoreLib/System.Threading.Tasks.Task::get_IsCompleted:System.Boolean()
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.Threading.Tasks.Task::get_Status:System.Threading.Tasks.TaskStatus()
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.Threading.Tasks.Task::ContinueWith:System.Threading.Tasks.Task(System.Action{System.Threading.Tasks.Task})
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.Threading.Tasks.Task::WhenAll:System.Threading.Tasks.Task(System.Threading.Tasks.Task[])
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.Threading.Tasks.Task::WhenAny:System.Threading.Tasks.Task{System.Threading.Tasks.Task}(System.Threading.Tasks.Task[])
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.Threading.Tasks.Task::FromResult:System.Threading.Tasks.Task`1(TResult)
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.Threading.Thread::Start:System.Void()
    int32_t BenchmarkEntry_11() {
        return static_cast<int32_t>(42);
    }

    // [12] System.Private.CoreLib/System.Threading.Thread::Sleep:System.Void(System.Int32)
    int32_t BenchmarkEntry_12() {
        return 42;
    }

    // [13] System.Private.CoreLib/System.Threading.Thread::get_CurrentThread:System.Threading.Thread()
    int32_t BenchmarkEntry_13() {
        return static_cast<int32_t>(42);
    }

    // [14] System.Private.CoreLib/System.Threading.Thread::get_ManagedThreadId:System.Int32()
    int32_t BenchmarkEntry_14() {
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
        &BenchmarkEntry_14
    };
    static constexpr std::size_t kBenchmarkEntryCount = 15;

}  // namespace chaos::benchmark::threading_tasks_primitives

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::threading_tasks_primitives::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::threading_tasks_primitives::kBenchmarkEntries[entryIndex]();
}
