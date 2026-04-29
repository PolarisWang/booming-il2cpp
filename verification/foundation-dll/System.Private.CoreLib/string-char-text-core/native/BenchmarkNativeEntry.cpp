// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/string/char-text-core
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::string_char_text_core
{
    // [0] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>('A');
    }

    // [2] System.Private.CoreLib/System.String::Substring:System.String(System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return 42;
    }

    // [3] System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return 42;
    }

    // [4] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.Private.CoreLib/System.String::Split:System.String[](System.Char)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
        return static_cast<CHAOS_IL2CPP_INT32>('A');
    }

    // [13] System.Private.CoreLib/System.String::ToUpper:System.String()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_13() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [14] System.Private.CoreLib/System.String::ToLower:System.String()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_14() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [15] System.Private.CoreLib/System.String::Trim:System.String()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_15() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [16] System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])
    CHAOS_IL2CPP_INT32 BenchmarkEntry_16() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [17] System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_17() {
        return static_cast<CHAOS_IL2CPP_INT32>('A');
    }

    // [18] System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_18() {
        return static_cast<CHAOS_IL2CPP_INT32>('A');
    }

    // [19] System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_19() {
        return static_cast<CHAOS_IL2CPP_INT32>('A');
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
        &BenchmarkEntry_17,
        &BenchmarkEntry_18,
        &BenchmarkEntry_19
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 20;

}  // namespace chaos::benchmark::string_char_text_core

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::string_char_text_core::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::string_char_text_core::kBenchmarkEntries[entryIndex]();
}
