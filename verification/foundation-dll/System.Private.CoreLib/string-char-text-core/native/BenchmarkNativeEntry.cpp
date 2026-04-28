// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/string/char-text-core
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::string_char_text_core
{
    // [0] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>('A');
    }

    // [2] System.Private.CoreLib/System.String::Substring:System.String(System.Int32)
    int32_t BenchmarkEntry_2() {
        return 42;
    }

    // [3] System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)
    int32_t BenchmarkEntry_3() {
        return 42;
    }

    // [4] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)
    int32_t BenchmarkEntry_4() {
        return static_cast<int32_t>(42);
    }

    // [5] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
    int32_t BenchmarkEntry_11() {
        return static_cast<int32_t>(42);
    }

    // [12] System.Private.CoreLib/System.String::Split:System.String[](System.Char)
    int32_t BenchmarkEntry_12() {
        return static_cast<int32_t>('A');
    }

    // [13] System.Private.CoreLib/System.String::ToUpper:System.String()
    int32_t BenchmarkEntry_13() {
        return static_cast<int32_t>(42);
    }

    // [14] System.Private.CoreLib/System.String::ToLower:System.String()
    int32_t BenchmarkEntry_14() {
        return static_cast<int32_t>(42);
    }

    // [15] System.Private.CoreLib/System.String::Trim:System.String()
    int32_t BenchmarkEntry_15() {
        return static_cast<int32_t>(42);
    }

    // [16] System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])
    int32_t BenchmarkEntry_16() {
        return static_cast<int32_t>(42);
    }

    // [17] System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    int32_t BenchmarkEntry_17() {
        return static_cast<int32_t>('A');
    }

    // [18] System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    int32_t BenchmarkEntry_18() {
        return static_cast<int32_t>('A');
    }

    // [19] System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    int32_t BenchmarkEntry_19() {
        return static_cast<int32_t>('A');
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
        &BenchmarkEntry_15,
        &BenchmarkEntry_16,
        &BenchmarkEntry_17,
        &BenchmarkEntry_18,
        &BenchmarkEntry_19
    };
    static constexpr std::size_t kBenchmarkEntryCount = 20;

}  // namespace chaos::benchmark::string_char_text_core

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::string_char_text_core::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::string_char_text_core::kBenchmarkEntries[entryIndex]();
}
