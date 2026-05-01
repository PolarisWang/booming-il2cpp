// Auto-generated benchmark native entry
// Family: family/System.ObjectModel/property-change-notifications
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::property_change_notifications
{
    // [0] System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::add_PropertyChanged:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.ObjectModel/System.ComponentModel.INotifyPropertyChanged::remove_PropertyChanged:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::add_PropertyChanging:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.ObjectModel/System.ComponentModel.INotifyPropertyChanging::remove_PropertyChanging:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.ObjectModel/System.ComponentModel.PropertyChangedEventArgs::PropertyChangedEventArgs::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.ObjectModel/System.ComponentModel.PropertyChangedEventArgs::get_PropertyName:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.ObjectModel/System.ComponentModel.PropertyChangedEventArgs::set_PropertyName:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::PropertyChangedEventHandler::Void(System.Object, System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::BeginInvoke:System.Void(P0, P1, P2, P3)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::EndInvoke:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.ObjectModel/System.ComponentModel.PropertyChangedEventHandler::Invoke:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.ObjectModel/System.ComponentModel.PropertyChangingEventArgs::PropertyChangingEventArgs::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.ObjectModel/System.ComponentModel.PropertyChangingEventArgs::get_PropertyName:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [13] System.ObjectModel/System.ComponentModel.PropertyChangingEventArgs::set_PropertyName:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_13() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [14] System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::PropertyChangingEventHandler::Void(System.Object, System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_14() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [15] System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::BeginInvoke:System.Void(P0, P1, P2, P3)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_15() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [16] System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::EndInvoke:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_16() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [17] System.ObjectModel/System.ComponentModel.PropertyChangingEventHandler::Invoke:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_17() {
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
        &BenchmarkEntry_15,
        &BenchmarkEntry_16,
        &BenchmarkEntry_17
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 18;

}  // namespace chaos::benchmark::property_change_notifications

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::property_change_notifications::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::property_change_notifications::kBenchmarkEntries[entryIndex]();
}
