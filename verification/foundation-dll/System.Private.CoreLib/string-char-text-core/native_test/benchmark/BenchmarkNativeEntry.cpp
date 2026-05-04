// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/string/char-text-core
#include <chaos/native_types.h>
#include <cctype>
#include <cstring>
#include <string>

namespace chaos::benchmark::string_char_text_core
{
    // [0] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("hello_benchmark").find("benchmark"));
    }

    // [1] System.Private.CoreLib/System.String::IndexOf:System.Int32(System.Char)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("hello_benchmark").find('e'));
    }

    // [2] System.Private.CoreLib/System.String::Substring:System.String(System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("hello_benchmark").substr(2).length());
    }

    // [3] System.Private.CoreLib/System.String::Substring:System.String(System.Int32,System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("hello_benchmark").substr(2).length());
    }

    // [4] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return std::strcmp("hello_abc", "hello_xyz");
    }

    // [5] System.Private.CoreLib/System.String::Compare:System.Int32(System.String,System.Int32,System.String,System.Int32,System.Int32)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return std::strncmp("hello_abc", "hello_xyz", 5);
    }

    // [6] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>((std::string("hello_") + std::string("world")).length());
    }

    // [7] System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>((std::string("hello_") + std::string("world")).length());
    }

    // [8] System.Private.CoreLib/System.String::Format:System.String(System.String,System.Object[])
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("hello").length());
    }

    // [9] System.Private.CoreLib/System.String::StartsWith:System.Boolean(System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return std::string("hello_benchmark").find("hello") == 0 ? 1 : 0;
    }

    // [10] System.Private.CoreLib/System.String::Contains:System.Boolean(System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return std::string("hello_benchmark").find("bench") != std::string::npos ? 1 : 0;
    }

    // [11] System.Private.CoreLib/System.String::Replace:System.String(System.String,System.String)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("hello_benchmark").length());
    }

    // [12] System.Private.CoreLib/System.String::Split:System.String[](System.Char)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("a,b,c,d,e").size());
    }

    // [13] System.Private.CoreLib/System.String::ToUpper:System.String()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_13() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("hello_benchmark").size());
    }

    // [14] System.Private.CoreLib/System.String::ToLower:System.String()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_14() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("HELLO_BENCHMARK").size());
    }

    // [15] System.Private.CoreLib/System.String::Trim:System.String()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_15() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("  hello_benchmark  ").size());
    }

    // [16] System.Private.CoreLib/System.String::Join:System.String(System.String,System.String[])
    CHAOS_IL2CPP_INT32 BenchmarkEntry_16() {
        return static_cast<CHAOS_IL2CPP_INT32>(std::string("a,b,c").size());
    }

    // [17] System.Private.CoreLib/System.Char::IsDigit:System.Boolean(System.Char)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_17() {
        return std::isdigit(static_cast<unsigned char>('7')) ? 1 : 0;
    }

    // [18] System.Private.CoreLib/System.Char::IsLetter:System.Boolean(System.Char)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_18() {
        return std::isalpha(static_cast<unsigned char>('Z')) ? 1 : 0;
    }

    // [19] System.Private.CoreLib/System.Char::IsWhiteSpace:System.Boolean(System.Char)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_19() {
        return std::isspace(static_cast<unsigned char>(' ')) ? 1 : 0;
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
