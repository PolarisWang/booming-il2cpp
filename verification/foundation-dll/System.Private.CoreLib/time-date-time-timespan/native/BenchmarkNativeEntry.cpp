// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/time/date-time-timespan
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::time_date_time_timespan
{
    // [0] System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32)
    int32_t BenchmarkEntry_0() {
        return 42;
    }

    // [1] System.Private.CoreLib/System.DateTime::.ctor:System.Void(System.Int32,System.Int32,System.Int32,System.Int32,System.Int32,System.Int32)
    int32_t BenchmarkEntry_1() {
        return 42;
    }

    // [2] System.Private.CoreLib/System.DateTime::Parse:System.DateTime(System.String)
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.DateTime::TryParse:System.Boolean(System.String,System.DateTime&)
    int32_t BenchmarkEntry_3() {
        return static_cast<int32_t>(42);
    }

    // [4] System.Private.CoreLib/System.DateTime::ToString:System.String()
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.DateTime::ToString:System.String(System.String)
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.DateTime::AddDays:System.DateTime(System.Double)
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.DateTime::AddHours:System.DateTime(System.Double)
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.DateTime::AddMinutes:System.DateTime(System.Double)
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.DateTime::Compare:System.Int32(System.DateTime,System.DateTime)
    int32_t BenchmarkEntry_9() {
        return static_cast<int64_t>(0);
    }

    // [10] System.Private.CoreLib/System.DateTime::DaysInMonth:System.Int32(System.Int32,System.Int32)
    int32_t BenchmarkEntry_10() {
        return 42;
    }

    // [11] System.Private.CoreLib/System.TimeSpan::.ctor:System.Void(System.Int32,System.Int32,System.Int32)
    int32_t BenchmarkEntry_11() {
        return 42;
    }

    // [12] System.Private.CoreLib/System.TimeSpan::FromDays:System.TimeSpan(System.Double)
    int32_t BenchmarkEntry_12() {
        return static_cast<int32_t>(42);
    }

    // [13] System.Private.CoreLib/System.TimeSpan::FromHours:System.TimeSpan(System.Double)
    int32_t BenchmarkEntry_13() {
        return static_cast<int32_t>(42);
    }

    // [14] System.Private.CoreLib/System.TimeSpan::FromMinutes:System.TimeSpan(System.Double)
    int32_t BenchmarkEntry_14() {
        return static_cast<int32_t>(42);
    }

    // [15] System.Private.CoreLib/System.TimeSpan::Parse:System.TimeSpan(System.String)
    int32_t BenchmarkEntry_15() {
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
        &BenchmarkEntry_14,
        &BenchmarkEntry_15
    };
    static constexpr std::size_t kBenchmarkEntryCount = 16;

}  // namespace chaos::benchmark::time_date_time_timespan

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::time_date_time_timespan::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::time_date_time_timespan::kBenchmarkEntries[entryIndex]();
}
