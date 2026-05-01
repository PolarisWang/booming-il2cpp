// Auto-generated benchmark native entry
// Family: family/System.Runtime.InteropServices/objective-c-interop
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::objective_c_interop
{
    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::CreateReferenceTrackingHandle:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::Initialize:System.Void(P0, P1, P2, P3)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendCallback:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendPendingException:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::ObjectiveCMarshal+UnhandledExceptionPropagationHandler::Void(System.Object, System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::BeginInvoke:System.Void(P0, P1, P2, P3, P4)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::EndInvoke:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler::Invoke:System.Void(P0, P1, P2)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCTrackedTypeAttribute::ObjectiveCTrackedTypeAttribute::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
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
        &BenchmarkEntry_8
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 9;

}  // namespace chaos::benchmark::objective_c_interop

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::objective_c_interop::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::objective_c_interop::kBenchmarkEntries[entryIndex]();
}
