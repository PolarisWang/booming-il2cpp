// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/reflection/member-complete
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::reflection_member_complete
{
    // [0] System.Private.CoreLib/System.Reflection.MemberInfo:get_Name():System.String
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo:get_MemberType():System.Reflection.MemberTypes
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo:get_DeclaringType():System.Type
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    using BenchmarkEntryFn = CHAOS_IL2CPP_INT32 (*)();
    static constexpr BenchmarkEntryFn kBenchmarkEntries[] = {
        &BenchmarkEntry_0,
        &BenchmarkEntry_1,
        &BenchmarkEntry_2
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 3;

}  // namespace chaos::benchmark::reflection_member_complete

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::reflection_member_complete::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::reflection_member_complete::kBenchmarkEntries[entryIndex]();
}
