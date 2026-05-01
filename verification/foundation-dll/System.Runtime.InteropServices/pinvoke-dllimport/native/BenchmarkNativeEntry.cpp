// Auto-generated benchmark native entry
// Family: family/System.Runtime.InteropServices/pinvoke-dllimport
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::pinvoke_dllimport
{
    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::DllImportAttribute::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::get_Value:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.DllImportAttribute::set_Value:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::LibraryImportAttribute::Void(System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_EntryPoint:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_LibraryName:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_SetLastError:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshalling:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::get_StringMarshallingCustomType:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_EntryPoint:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_SetLastError:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshalling:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_StringMarshallingCustomType:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.LibraryImportAttribute::set_LibraryName:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_13() {
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
        &BenchmarkEntry_13
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 14;

}  // namespace chaos::benchmark::pinvoke_dllimport

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::pinvoke_dllimport::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::pinvoke_dllimport::kBenchmarkEntries[entryIndex]();
}
