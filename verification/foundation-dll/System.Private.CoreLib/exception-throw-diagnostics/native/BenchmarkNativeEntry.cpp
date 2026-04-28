// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/exception/throw-diagnostics
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::exception_throw_diagnostics
{
    // [0] System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String,System.Exception)
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.Exception::get_Message:System.String()
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.Exception::get_InnerException:System.Exception()
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.Exception::get_StackTrace:System.String()
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.Exception::get_HResult:System.Int32()
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.Exception::ToString:System.String()
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.Exception::GetBaseException:System.Exception()
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.ArgumentException::.ctor:System.Void(System.String,System.String)
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.ArgumentNullException::.ctor:System.Void(System.String)
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.ArgumentOutOfRangeException::.ctor:System.Void(System.String)
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)
    int32_t BenchmarkEntry_11() {
        return static_cast<int32_t>(42);
    }

    // [12] System.Private.CoreLib/System.NotSupportedException::.ctor:System.Void(System.String)
    int32_t BenchmarkEntry_12() {
        return static_cast<int32_t>(42);
    }

    // [13] System.Private.CoreLib/System.NotImplementedException::.ctor:System.Void(System.String)
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

}  // namespace chaos::benchmark::exception_throw_diagnostics

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::exception_throw_diagnostics::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::exception_throw_diagnostics::kBenchmarkEntries[entryIndex]();
}
