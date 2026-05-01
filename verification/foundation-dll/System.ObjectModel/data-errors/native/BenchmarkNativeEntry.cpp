// Auto-generated benchmark native entry
// Family: family/System.ObjectModel/data-errors
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::data_errors
{
    // [0] System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::DataErrorsChangedEventArgs::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::get_PropertyName:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.ObjectModel/System.ComponentModel.DataErrorsChangedEventArgs::set_PropertyName:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::GetErrors:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::add_ErrorsChanged:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::get_HasErrors:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::remove_ErrorsChanged:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.ObjectModel/System.ComponentModel.INotifyDataErrorInfo::set_HasErrors:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
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
        &BenchmarkEntry_7
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 8;

}  // namespace chaos::benchmark::data_errors

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::data_errors::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::data_errors::kBenchmarkEntries[entryIndex]();
}
