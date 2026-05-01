// Auto-generated benchmark native entry
// Family: family/System.Runtime.InteropServices/exception-errors
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::exception_errors
{
    // [0] System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Runtime.InteropServices/System.DllNotFoundException::DllNotFoundException::Void(System.Object, System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::COMException::Void(System.Object, System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.COMException::ToString:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::SEHException::Void(System.Object, System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.SEHException::CanResume:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
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
        &BenchmarkEntry_10
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 11;

}  // namespace chaos::benchmark::exception_errors

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::exception_errors::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::exception_errors::kBenchmarkEntries[entryIndex]();
}
