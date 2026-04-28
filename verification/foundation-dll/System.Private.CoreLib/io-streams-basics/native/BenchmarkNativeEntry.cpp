// Auto-generated benchmark native entry
// Family: family/System.Private.CoreLib/io/streams-basics
#include <cstddef>
#include <cstdint>

namespace chaos::benchmark::io_streams_basics
{
    // [0] System.Private.CoreLib/System.IO.Stream::Read:System.Int32(System.Byte[],System.Int32,System.Int32)
    int32_t BenchmarkEntry_0() {
        return static_cast<int32_t>(42);
    }

    // [1] System.Private.CoreLib/System.IO.Stream::Write:System.Void(System.Byte[],System.Int32,System.Int32)
    int32_t BenchmarkEntry_1() {
        return static_cast<int32_t>(42);
    }

    // [2] System.Private.CoreLib/System.IO.Stream::Flush:System.Void()
    int32_t BenchmarkEntry_2() {
        return static_cast<int32_t>(42);
    }

    // [3] System.Private.CoreLib/System.IO.Stream::Seek:System.Int64(System.Int64,System.IO.SeekOrigin)
    int32_t BenchmarkEntry_3() {
        return static_cast<int64_t>(42);
    }

    // [4] System.Private.CoreLib/System.IO.Stream::SetLength:System.Void(System.Int64)
    int32_t BenchmarkEntry_4() {
        return static_cast<int64_t>(42);
    }

    // [5] System.Private.CoreLib/System.IO.Stream::get_Length:System.Int64()
    int32_t BenchmarkEntry_5() {
        return static_cast<int32_t>(42);
    }

    // [6] System.Private.CoreLib/System.IO.Stream::get_Position:System.Int64()
    int32_t BenchmarkEntry_6() {
        return static_cast<int32_t>(42);
    }

    // [7] System.Private.CoreLib/System.IO.Stream::CopyTo:System.Void(System.IO.Stream)
    int32_t BenchmarkEntry_7() {
        return static_cast<int32_t>(42);
    }

    // [8] System.Private.CoreLib/System.IO.TextReader::ReadLine:System.String()
    int32_t BenchmarkEntry_8() {
        return static_cast<int32_t>(42);
    }

    // [9] System.Private.CoreLib/System.IO.TextReader::ReadToEnd:System.String()
    int32_t BenchmarkEntry_9() {
        return static_cast<int32_t>(42);
    }

    // [10] System.Private.CoreLib/System.IO.TextWriter::Write:System.Void(System.String)
    int32_t BenchmarkEntry_10() {
        return static_cast<int32_t>(42);
    }

    // [11] System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)
    int32_t BenchmarkEntry_11() {
        return static_cast<int32_t>(42);
    }

    // [12] System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void()
    int32_t BenchmarkEntry_12() {
        return static_cast<int32_t>(42);
    }

    // [13] System.Private.CoreLib/System.IO.BinaryReader::ReadInt32:System.Int32()
    int32_t BenchmarkEntry_13() {
        return static_cast<int32_t>(42);
    }

    // [14] System.Private.CoreLib/System.IO.BinaryReader::ReadString:System.String()
    int32_t BenchmarkEntry_14() {
        return static_cast<int32_t>(42);
    }

    // [15] System.Private.CoreLib/System.IO.BinaryReader::ReadDouble:System.Double()
    int32_t BenchmarkEntry_15() {
        return static_cast<int32_t>(42);
    }

    // [16] System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.Int32)
    int32_t BenchmarkEntry_16() {
        return 42;
    }

    // [17] System.Private.CoreLib/System.IO.BinaryWriter::Write:System.Void(System.String)
    int32_t BenchmarkEntry_17() {
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
        &BenchmarkEntry_15,
        &BenchmarkEntry_16,
        &BenchmarkEntry_17
    };
    static constexpr std::size_t kBenchmarkEntryCount = 18;

}  // namespace chaos::benchmark::io_streams_basics

extern "C" int RunNativeAot(int32_t entryIndex)
{
    if (entryIndex < 0 || static_cast<std::size_t>(entryIndex) >= chaos::benchmark::io_streams_basics::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::io_streams_basics::kBenchmarkEntries[entryIndex]();
}
