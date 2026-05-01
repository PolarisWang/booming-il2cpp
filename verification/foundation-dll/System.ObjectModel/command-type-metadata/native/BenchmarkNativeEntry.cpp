// Auto-generated benchmark native entry
// Family: family/System.ObjectModel/command-type-metadata
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::command_type_metadata
{
    // [0] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::TypeConverterAttribute::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::TypeConverterAttribute::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::Equals:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::GetHashCode:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::get_ConverterTypeName:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::set_ConverterTypeName:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.ObjectModel/System.Reflection.ICustomTypeProvider::GetCustomType:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.ObjectModel/System.Windows.Input.ICommand::CanExecute:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.ObjectModel/System.Windows.Input.ICommand::Execute:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::ValueSerializerAttribute::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerType:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [13] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerTypeName:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_13() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [14] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::set_ValueSerializerType:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_14() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [15] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::set_ValueSerializerTypeName:System.Void(P0)
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

}  // namespace chaos::benchmark::command_type_metadata

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::command_type_metadata::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::command_type_metadata::kBenchmarkEntries[entryIndex]();
}
