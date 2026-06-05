// entry.cpp — Auto-generated
// CLI entry point for native-AOT verification dispatch.
//
// Modes:
//   (no args)            — fact: run all subject entries, print Passed: N/M
//   --benchmark N I      — benchmark method N for I iterations
//   --benchmark-all I    — benchmark ALL methods for I iterations, aggregated JSON
//   --hotupdate          — hotpatch fact: baseline + apply + semantic-check + revert
//   --hotupdate-and-benchmark N I — post-patch benchmark
//   --microbench         — interpreter microbenchmarks
//   --list               — list all subjects with their indices
//   --fact-json          — per-method JSON output for value-level verification

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <cstdint>
#include <cinttypes>
#include <memory>

#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#endif

namespace chaos { namespace il2cpp { namespace common { namespace log_internal {
bool g_log_use_stderr = false;
}}}}

#include <chaos/config.h>
#include <chaos/native_types.h>
#include <chaos/profile.h>
#include <runtime_core.h>

#include "chaos_runtime_host.h"

#include <chaos/hotpatch_dispatch.h>
#include <patch_loader.h>

extern "C" const int kAotMethodCount;
extern "C" const int kSubjectEntryCount;
extern "C" const int kSubjectSlotMap[];

extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept;

extern "C" const CodeRegistrationV0 chaos_codegen_code_registration;
extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration;
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options;
extern "C" void ChaosRegisterGcLayouts();

#define CHAOS_USE_DEFAULT_THUNKS nullptr

struct BenchmarkResult {
    double elapsed_ms;
    int64_t allocated_bytes;
};

extern "C" CHAOS_IL2CPP_INT32 RunFactAll();
extern "C" BenchmarkResult RunBenchmark(int entry_index, int iterations);
extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll();
extern "C" BenchmarkResult RunHotpatchBenchmark(int entry_index, int iterations);

extern const uint8_t kPatchData[];
extern const size_t kPatchDataSize;
extern const char* const kPatchDataHostClassName;

extern "C" void RunMicrobench();

// Subject metadata table for --list / --fact-json modes
struct SubjectEntry {
    int index;
    const char* subjectId;
    const char* assemblyName;
    const char* typeName;
    const char* methodName;
    bool isStatic;
    const char* kind;
};

static const SubjectEntry kSubjectEntries[] = {
    { 0, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddEntry:System.Void(System.Reflection.PortableExecutable.DebugDirectoryEntryType,System.UInt32,System.UInt32)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddEntry", True, "benchmark" },
    { 1, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddEntry:System.Void(System.Reflection.PortableExecutable.DebugDirectoryEntryType,System.UInt32,System.UInt32)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddEntry", True, "benchmark" },
    { 2, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddEntry:System.Void(System.Reflection.PortableExecutable.DebugDirectoryEntryType,System.UInt32,System.UInt32)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddEntry", True, "benchmark" },
    { 3, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddEntry:System.Void(System.Reflection.PortableExecutable.DebugDirectoryEntryType,System.UInt32,System.UInt32,System.Int32,System.Action<System.Reflection.Metadata.BlobBuilder,System.Int32>)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddEntry", True, "benchmark" },
    { 4, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddEntry:System.Void(System.Reflection.PortableExecutable.DebugDirectoryEntryType,System.UInt32,System.UInt32,System.Int32,System.Action<System.Reflection.Metadata.BlobBuilder,System.Int32>)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddEntry", True, "benchmark" },
    { 5, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddEntry:System.Void(System.Reflection.PortableExecutable.DebugDirectoryEntryType,System.UInt32,System.UInt32,System.Int32,System.Action<System.Reflection.Metadata.BlobBuilder,System.Int32>)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddEntry", True, "benchmark" },
    { 6, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddEntry:System.Void(System.Reflection.PortableExecutable.DebugDirectoryEntryType,System.UInt32,System.UInt32,System.Int32,System.Action<System.Reflection.Metadata.BlobBuilder,System.Int32>)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddEntry", True, "benchmark" },
    { 7, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddCodeViewEntry:System.Void(System.String,System.Reflection.Metadata.BlobContentId,System.UInt16)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddCodeViewEntry", True, "benchmark" },
    { 8, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddCodeViewEntry:System.Void(System.String,System.Reflection.Metadata.BlobContentId,System.UInt16)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddCodeViewEntry", True, "benchmark" },
    { 9, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddCodeViewEntry:System.Void(System.String,System.Reflection.Metadata.BlobContentId,System.UInt16)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddCodeViewEntry", True, "benchmark" },
    { 10, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddCodeViewEntry:System.Void(System.String,System.Reflection.Metadata.BlobContentId,System.UInt16,System.Int32)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddCodeViewEntry", True, "benchmark" },
    { 11, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddCodeViewEntry:System.Void(System.String,System.Reflection.Metadata.BlobContentId,System.UInt16,System.Int32)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddCodeViewEntry", True, "benchmark" },
    { 12, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddCodeViewEntry:System.Void(System.String,System.Reflection.Metadata.BlobContentId,System.UInt16,System.Int32)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddCodeViewEntry", True, "benchmark" },
    { 13, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddReproducibleEntry:System.Void()", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddReproducibleEntry", True, "benchmark" },
    { 14, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddPdbChecksumEntry:System.Void(System.String,System.Collections.Immutable.ImmutableArray<System.Byte>)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddPdbChecksumEntry", True, "benchmark" },
    { 15, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddPdbChecksumEntry:System.Void(System.String,System.Collections.Immutable.ImmutableArray<System.Byte>)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddPdbChecksumEntry", True, "benchmark" },
    { 16, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddEmbeddedPortablePdbEntry:System.Void(System.Reflection.Metadata.BlobBuilder,System.UInt16)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddEmbeddedPortablePdbEntry", True, "benchmark" },
    { 17, "System.Reflection.Metadata/System.Reflection.PortableExecutable.DebugDirectoryBuilder::AddEmbeddedPortablePdbEntry:System.Void(System.Reflection.Metadata.BlobBuilder,System.UInt16)", "codegen", "System.Reflection.PortableExecutable.DebugDirectoryBuilder", "AddEmbeddedPortablePdbEntry", True, "benchmark" },
    { 18, "System.Reflection.Metadata/System.Reflection.PortableExecutable.ManagedPEBuilder::Sign:System.Void(System.Reflection.Metadata.BlobBuilder,System.Func<System.Collections.Generic.IEnumerable<System.Reflection.Metadata.Blob>,System.Byte[]>)", "codegen", "System.Reflection.PortableExecutable.ManagedPEBuilder", "Sign", True, "benchmark" },
    { 19, "System.Reflection.Metadata/System.Reflection.PortableExecutable.ManagedPEBuilder::Sign:System.Void(System.Reflection.Metadata.BlobBuilder,System.Func<System.Collections.Generic.IEnumerable<System.Reflection.Metadata.Blob>,System.Byte[]>)", "codegen", "System.Reflection.PortableExecutable.ManagedPEBuilder", "Sign", True, "benchmark" },
    { 20, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEBuilder::Serialize:System.Reflection.Metadata.BlobContentId(System.Reflection.Metadata.BlobBuilder)", "codegen", "System.Reflection.PortableExecutable.PEBuilder", "Serialize", True, "benchmark" },
    { 21, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEHeaderBuilder::CreateExecutableHeader:System.Reflection.PortableExecutable.PEHeaderBuilder()", "codegen", "System.Reflection.PortableExecutable.PEHeaderBuilder", "CreateExecutableHeader", True, "benchmark" },
    { 22, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEHeaderBuilder::CreateLibraryHeader:System.Reflection.PortableExecutable.PEHeaderBuilder()", "codegen", "System.Reflection.PortableExecutable.PEHeaderBuilder", "CreateLibraryHeader", True, "benchmark" },
    { 23, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEHeaders::TryGetDirectoryOffset:System.Boolean(System.Reflection.PortableExecutable.DirectoryEntry,System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEHeaders", "TryGetDirectoryOffset", True, "benchmark" },
    { 24, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEHeaders::TryGetDirectoryOffset:System.Boolean(System.Reflection.PortableExecutable.DirectoryEntry,System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEHeaders", "TryGetDirectoryOffset", True, "benchmark" },
    { 25, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEHeaders::GetContainingSectionIndex:System.Int32(System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEHeaders", "GetContainingSectionIndex", True, "benchmark" },
    { 26, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEHeaders::GetContainingSectionIndex:System.Int32(System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEHeaders", "GetContainingSectionIndex", True, "benchmark" },
    { 27, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEMemoryBlock::GetReader:System.Reflection.Metadata.BlobReader()", "codegen", "System.Reflection.PortableExecutable.PEMemoryBlock", "GetReader", True, "benchmark" },
    { 28, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEMemoryBlock::GetReader:System.Reflection.Metadata.BlobReader(System.Int32,System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEMemoryBlock", "GetReader", True, "benchmark" },
    { 29, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEMemoryBlock::GetReader:System.Reflection.Metadata.BlobReader(System.Int32,System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEMemoryBlock", "GetReader", True, "benchmark" },
    { 30, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEMemoryBlock::GetReader:System.Reflection.Metadata.BlobReader(System.Int32,System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEMemoryBlock", "GetReader", True, "benchmark" },
    { 31, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEMemoryBlock::GetContent:System.Collections.Immutable.ImmutableArray<System.Byte>()", "codegen", "System.Reflection.PortableExecutable.PEMemoryBlock", "GetContent", True, "benchmark" },
    { 32, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEMemoryBlock::GetContent:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Int32,System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEMemoryBlock", "GetContent", True, "benchmark" },
    { 33, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEMemoryBlock::GetContent:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Int32,System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEMemoryBlock", "GetContent", True, "benchmark" },
    { 34, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEMemoryBlock::GetContent:System.Collections.Immutable.ImmutableArray<System.Byte>(System.Int32,System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEMemoryBlock", "GetContent", True, "benchmark" },
    { 35, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::Dispose:System.Void()", "codegen", "System.Reflection.PortableExecutable.PEReader", "Dispose", True, "benchmark" },
    { 36, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::GetEntireImage:System.Reflection.PortableExecutable.PEMemoryBlock()", "codegen", "System.Reflection.PortableExecutable.PEReader", "GetEntireImage", True, "benchmark" },
    { 37, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::GetMetadata:System.Reflection.PortableExecutable.PEMemoryBlock()", "codegen", "System.Reflection.PortableExecutable.PEReader", "GetMetadata", True, "benchmark" },
    { 38, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::GetSectionData:System.Reflection.PortableExecutable.PEMemoryBlock(System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEReader", "GetSectionData", True, "benchmark" },
    { 39, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::GetSectionData:System.Reflection.PortableExecutable.PEMemoryBlock(System.Int32)", "codegen", "System.Reflection.PortableExecutable.PEReader", "GetSectionData", True, "benchmark" },
    { 40, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::GetSectionData:System.Reflection.PortableExecutable.PEMemoryBlock(System.String)", "codegen", "System.Reflection.PortableExecutable.PEReader", "GetSectionData", True, "benchmark" },
    { 41, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::GetSectionData:System.Reflection.PortableExecutable.PEMemoryBlock(System.String)", "codegen", "System.Reflection.PortableExecutable.PEReader", "GetSectionData", True, "benchmark" },
    { 42, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::ReadDebugDirectory:System.Collections.Immutable.ImmutableArray<System.Reflection.PortableExecutable.DebugDirectoryEntry>()", "codegen", "System.Reflection.PortableExecutable.PEReader", "ReadDebugDirectory", True, "benchmark" },
    { 43, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::ReadCodeViewDebugDirectoryData:System.Reflection.PortableExecutable.CodeViewDebugDirectoryData(System.Reflection.PortableExecutable.DebugDirectoryEntry)", "codegen", "System.Reflection.PortableExecutable.PEReader", "ReadCodeViewDebugDirectoryData", True, "benchmark" },
    { 44, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::ReadPdbChecksumDebugDirectoryData:System.Reflection.PortableExecutable.PdbChecksumDebugDirectoryData(System.Reflection.PortableExecutable.DebugDirectoryEntry)", "codegen", "System.Reflection.PortableExecutable.PEReader", "ReadPdbChecksumDebugDirectoryData", True, "benchmark" },
    { 45, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::TryOpenAssociatedPortablePdb:System.Boolean(System.String,System.Func<System.String,System.IO.Stream>,System.Reflection.Metadata.MetadataReaderProvider,System.String)", "codegen", "System.Reflection.PortableExecutable.PEReader", "TryOpenAssociatedPortablePdb", True, "benchmark" },
    { 46, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::TryOpenAssociatedPortablePdb:System.Boolean(System.String,System.Func<System.String,System.IO.Stream>,System.Reflection.Metadata.MetadataReaderProvider,System.String)", "codegen", "System.Reflection.PortableExecutable.PEReader", "TryOpenAssociatedPortablePdb", True, "benchmark" },
    { 47, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::TryOpenAssociatedPortablePdb:System.Boolean(System.String,System.Func<System.String,System.IO.Stream>,System.Reflection.Metadata.MetadataReaderProvider,System.String)", "codegen", "System.Reflection.PortableExecutable.PEReader", "TryOpenAssociatedPortablePdb", True, "benchmark" },
    { 48, "System.Reflection.Metadata/System.Reflection.PortableExecutable.PEReader::ReadEmbeddedPortablePdbDebugDirectoryData:System.Reflection.Metadata.MetadataReaderProvider(System.Reflection.PortableExecutable.DebugDirectoryEntry)", "codegen", "System.Reflection.PortableExecutable.PEReader", "ReadEmbeddedPortablePdbDebugDirectoryData", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 49;

static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchIfAvailable() {
    if (kPatchDataSize > 0u) {
        auto* patch_ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
            kPatchData, kPatchDataSize, kPatchDataHostClassName);
        if (patch_ctx == nullptr) {
            std::fprintf(stderr, "WARN: ApplyPatchFromMemory returned null (no patches applied)\n");
        } else {
            std::fprintf(stderr, "INFO: Applied patches to %u methods\n",
                         static_cast<unsigned>(patch_ctx->method_count));
        }
        return patch_ctx;
    }
    return nullptr;
}

static int RunFactMode() {
    const int kCount = kSubjectEntryCount;
    int passed_count = 0;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
{% if is_jit %}
        try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) {
            // caught — skip increment
        }
{% else %}
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            ++passed_count;
        CHAOS_EH_CATCH_BEGIN
            // caught — skip increment
        CHAOS_EH_END
{% endif %}
    }
    int failed_count = kCount - passed_count;
    printf("Passed: %d/%d\n", passed_count, kSubjectEntryCount);
    std::fflush(stdout);
    return failed_count;
}

static int RunFactJsonMode() {
    const int kCount = kSubjectEntryCount;
    printf("{\"factResults\":[");
    bool first = true;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        CHAOS_IL2CPP_INT32 result = 0;
        bool caught = false;
{% if is_jit %}
        try {
            result = chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) {
            caught = true;
        }
{% else %}
        CHAOS_EH_TRY
            result = chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
            caught = true;
        CHAOS_EH_END
{% end %}
        if (!first) printf(",");
        printf("{\"si\":%d,\"methodIndex\":%d,\"passed\":%s,\"exitCode\":%d}",
               si, i, caught ? "false" : "true", caught ? -1 : (int)result);
        first = false;
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

static int RunListMode() {
    printf("{\"subjects\":[");
    for (int i = 0; i < kSubjectEntryCount_Generated; i++) {
        if (i > 0) printf(",");
        printf("{\"index\":%d,\"subjectId\":\"%s\",\"assemblyName\":\"%s\","
               "\"typeName\":\"%s\",\"methodName\":\"%s\",\"isStatic\":%s,\"kind\":\"%s\"}",
               kSubjectEntries[i].index,
               kSubjectEntries[i].subjectId,
               kSubjectEntries[i].assemblyName,
               kSubjectEntries[i].typeName,
               kSubjectEntries[i].methodName,
               kSubjectEntries[i].isStatic ? "true" : "false",
               kSubjectEntries[i].kind);
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

static int RunBenchmarkMode(int entry_index, int iterations) {
    CHAOS_IL2CPP_PROFILE_SCOPE("RunBenchmarkMode");
    auto result = RunBenchmark(entry_index, iterations);
    if (result.elapsed_ms < 0.0) {
        printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}\n");
        return 1;
    }
    double ns_per_op = (result.elapsed_ms * 1e6) / iterations;
    double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
    double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
    printf(
        "{\"elapsedMilliseconds\":%.3f,\"calibratedMs\":%.3f,"
        "\"opsPerSecond\":%.0f,\"iterations\":%d,"
        "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}\n",
        result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
        result.allocated_bytes, alloc_per_op);
    std::fflush(stdout);
    return 0;
}

// ── --benchmark-all: benchmark ALL subject methods ─────────────────
static int RunBenchmarkAllMode(int iterations) {
    const int kCount = kSubjectEntryCount;
    printf("{\"benchmarkAll\":[");
    bool first = true;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        auto result = RunBenchmark(i, iterations);
        if (!first) printf(",");
        first = false;
        if (result.elapsed_ms < 0.0) {
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}", i);
        } else {
            double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
            double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
            printf(
                "{\"methodIndex\":%d,\"elapsedMilliseconds\":%.3f,"
                "\"opsPerSecond\":%.0f,\"iterations\":%d,"
                "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}",
                i, result.elapsed_ms, ops_per_sec, iterations,
                result.allocated_bytes, alloc_per_op);
        }
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

// ── --benchmark-range: benchmark a contiguous range of method table indices ─
// First argument: start index (inclusive), Second argument: end index (exclusive)
static int RunBenchmarkRangeMode(int iterations, int start_idx, int end_idx) {
    if (start_idx < 0) start_idx = 0;
    if (end_idx > kAotMethodCount) end_idx = kAotMethodCount;
    printf("{\"benchmarkAll\":[");
    bool first = true;
    for (int i = start_idx; i < end_idx; i++) {
        auto result = RunBenchmark(i, iterations);
        if (!first) printf(",");
        first = false;
        if (result.elapsed_ms < 0.0) {
            printf("{\"methodIndex\":%d,\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}", i);
        } else {
            double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
            double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
            printf(
                "{\"methodIndex\":%d,\"elapsedMilliseconds\":%.3f,"
                "\"opsPerSecond\":%.0f,\"iterations\":%d,"
                "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}",
                i, result.elapsed_ms, ops_per_sec, iterations,
                result.allocated_bytes, alloc_per_op);
        }
    }
    printf("]}\n");
    std::fflush(stdout);
    return 0;
}

// Load patch data from an external file (used by the verification pipeline).
// Returns nullptr if path is nullptr or file cannot be opened.
static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchFromFile(const char* path) {
    if (path == nullptr) return nullptr;
    FILE* f = fopen(path, "rb");
    if (!f) { std::fprintf(stderr, "WARN: cannot open patch data: %s\n", path); return nullptr; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    uint8_t* data = (uint8_t*)std::malloc(size > 0 ? size : 1);
    if (!data) { fclose(f); return nullptr; }
    size_t read_size = fread(data, 1, size, f);
    fclose(f);
    if (read_size == 0) { std::free(data); return nullptr; }
    auto* ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
        data, read_size, nullptr);
    std::free(data);
    if (ctx == nullptr) {
        std::fprintf(stderr, "WARN: ApplyPatchFromMemory returned null (no patches applied)\n");
    } else {
        std::fprintf(stderr, "INFO: Applied patches to %u methods from %s\n",
                     static_cast<unsigned>(ctx->method_count), path);
    }
    return ctx;
}

static int RunHotupdateMode(const char* patchDataPath = nullptr) {
    const int kCount = kSubjectEntryCount;
    // Dynamic allocation to support arbitrary method counts
    auto baseline_ok = std::make_unique<bool[]>(kCount);
    auto baseline_caught = std::make_unique<bool[]>(kCount);
    auto baseline_value = std::make_unique<int64_t[]>(kCount);
    for (int si = 0; si < kCount; si++) {
        baseline_ok[si] = false;
        baseline_caught[si] = false;
        baseline_value[si] = 0;
    }

    printf("{\n");
    printf("\"baselineFact\":[");
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t bv = 0;
        bool caught = false;
{% if is_jit %}
        try {
            bv = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            baseline_ok[si] = true;
        } catch(...) {
            caught = true;
        }
{% else %}
        CHAOS_EH_TRY
            bv = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            baseline_ok[si] = true;
        CHAOS_EH_CATCH_BEGIN
            caught = true;
        CHAOS_EH_END
{% end %}
        baseline_caught[si] = caught;
        baseline_value[si] = bv;
        if (si > 0) printf(",");
        printf("{\"si\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, caught ? "false" : "true", bv);
    }
    printf("],");
    auto* patch_ctx = patchDataPath ? ApplyHotpatchFromFile(patchDataPath) : ApplyHotpatchIfAvailable();
    int semantic_passed = 0;
    int semantic_changed_count = 0;
    printf("\"patchedFact\":[");
    for (int si = 0; si < kCount; si++) {
        if (!baseline_ok[si]) { continue; }
        int i = kSubjectSlotMap[si];
        int64_t patched_value = 0;
        bool patched_caught = false;
{% if is_jit %}
        try {
            patched_value = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) {
            patched_caught = true;
        }
{% else %}
        CHAOS_EH_TRY
            patched_value = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
            patched_caught = true;
        CHAOS_EH_END
{% end %}
        if (semantic_passed > 0) printf(",");
        printf("{\"si\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, patched_caught ? "false" : "true",
               patched_caught ? 0 : patched_value);
        semantic_passed++;
        if (baseline_caught[si] != patched_caught ||
            baseline_value[si] != patched_value) {
            semantic_changed_count++;
        }
    }
    printf("],");
    if (patch_ctx != nullptr) {
        chaos::il2cpp::runtime_core::Unpatch(patch_ctx);
    }
    bool all_revert = true;
    int revert_passed = 0;
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
{% if is_jit %}
        try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            revert_passed++;
        } catch(...) {
            all_revert = false;
        }
{% else %}
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
            revert_passed++;
        CHAOS_EH_CATCH_BEGIN
            all_revert = false;
        CHAOS_EH_END
{% end %}
    }
    bool all_semantic = (semantic_passed > 0 && semantic_changed_count > 0);
    printf(
        "\"passedMethods\":%d,\"failedMethods\":0,"
        "\"totalMethods\":%d,\"allSemantic\":%s,\"allRevert\":%s,"
        "\"semanticChangedCount\":%d}\n",
        semantic_passed, kCount, all_semantic ? "true" : "false",
        all_revert ? "true" : "false", semantic_changed_count);
    std::fflush(stdout);
    return 0;
}

static int RunMicrobenchMode() {
    RunMicrobench();
    return 0;
}

static int RunHotupdateBenchmarkMode(int entry_index, int iterations) {
    auto* patch_ctx = ApplyHotpatchIfAvailable();
    auto result = RunHotpatchBenchmark(entry_index, iterations);
    if (result.elapsed_ms < 0.0) {
        printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}\n");
        return 1;
    }
    double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
    double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
    printf(
        "{\"elapsedMilliseconds\":%.3f,\"calibratedMs\":%.3f,"
        "\"opsPerSecond\":%.0f,\"iterations\":%d,"
        "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}\n",
        result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
        result.allocated_bytes, alloc_per_op);
    std::fflush(stdout);
    return 0;
}

int main(int argc, char* argv[]) {
    chaos::il2cpp::common::log_internal::g_log_use_stderr = true;
    chaos::il2cpp::runtime_core::g_bgc_enabled = false;

    auto* chaos_host = new ChaosRuntimeHost();
    if (!chaos_host->Initialize("verification-entry")) {
        std::fprintf(stderr, "FATAL: ChaosRuntimeHost::Initialize failed\n");
        return 1;
    }
    if (!chaos_host->RegisterModule(
            &chaos_codegen_code_registration,
            &chaos_codegen_metadata_registration,
            &chaos_codegen_options)) {
        std::fprintf(stderr, "FATAL: RegisterModule failed\n");
        return 1;
    }
    ChaosRegisterGcLayouts();

    int ret = 0;
    if (argc < 2) { ret = RunFactMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--fact-json") == 0) { ret = RunFactJsonMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--list") == 0) { ret = RunListMode(); goto shutdown; }

    if (std::strcmp(argv[1], "--benchmark") == 0) {
        if (argc < 4) { printf("Usage: entry.exe --benchmark <index> <iterations>\n"); return 1; }
        ret = RunBenchmarkMode(std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--benchmark-all") == 0) {
        if (argc < 3) { printf("Usage: entry.exe --benchmark-all <iterations>\n"); return 1; }
        ret = RunBenchmarkAllMode(std::atoi(argv[2]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--benchmark-range") == 0) {
        if (argc < 5) { printf("Usage: entry.exe --benchmark-range <start> <end> <iterations>\n"); return 1; }
        ret = RunBenchmarkRangeMode(std::atoi(argv[4]), std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--hotupdate") == 0) {
        const char* patchDataPath = nullptr;
        if (argc >= 4 && std::strcmp(argv[2], "--patch-data") == 0) {
            patchDataPath = argv[3];
        }
        ret = RunHotupdateMode(patchDataPath);
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--hotupdate-and-benchmark") == 0) {
        if (argc < 4) { printf("Usage: entry.exe --hotupdate-and-benchmark <index> <iterations>\n"); return 1; }
        ret = RunHotupdateBenchmarkMode(std::atoi(argv[2]), std::atoi(argv[3]));
        goto shutdown;
    }

    if (std::strcmp(argv[1], "--microbench") == 0) { ret = RunMicrobenchMode(); goto shutdown; }

    printf("Unknown flag: %s\n", argv[1]);
    ret = 1;

shutdown:
    std::fflush(stdout);
{% if is_windows %}
    TerminateProcess(GetCurrentProcess(), static_cast<DWORD>(ret));
{% else %}
    _exit(ret);
{% end %}
}