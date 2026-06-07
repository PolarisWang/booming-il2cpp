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
//   --profile             — profile mode: GC stats, alloc volume, heap delta, code size

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
#include <profile_stats.h>

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
    { 0, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Void(System.Data.Constraint)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 1, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn[],System.Boolean)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 2, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn[],System.Boolean)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 3, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn[],System.Boolean)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 4, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn[],System.Boolean)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 5, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn[],System.Boolean)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 6, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn,System.Boolean)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 7, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn,System.Boolean)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 8, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn,System.Boolean)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 9, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn,System.Data.DataColumn)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 10, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn,System.Data.DataColumn)", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 11, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 12, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 13, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 14, "System.Data.Common/System.Data.ConstraintCollection::Add:System.Data.Constraint(System.String,System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.ConstraintCollection", "Add", True, "benchmark" },
    { 15, "System.Data.Common/System.Data.ConstraintCollection::AddRange:System.Void(System.Data.Constraint[])", "codegen", "System.Data.ConstraintCollection", "AddRange", True, "benchmark" },
    { 16, "System.Data.Common/System.Data.ConstraintCollection::AddRange:System.Void(System.Data.Constraint[])", "codegen", "System.Data.ConstraintCollection", "AddRange", True, "benchmark" },
    { 17, "System.Data.Common/System.Data.ConstraintCollection::AddRange:System.Void(System.Data.Constraint[])", "codegen", "System.Data.ConstraintCollection", "AddRange", True, "benchmark" },
    { 18, "System.Data.Common/System.Data.ConstraintCollection::CanRemove:System.Boolean(System.Data.Constraint)", "codegen", "System.Data.ConstraintCollection", "CanRemove", True, "benchmark" },
    { 19, "System.Data.Common/System.Data.ConstraintCollection::Clear:System.Void()", "codegen", "System.Data.ConstraintCollection", "Clear", True, "benchmark" },
    { 20, "System.Data.Common/System.Data.ConstraintCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.ConstraintCollection", "Contains", True, "benchmark" },
    { 21, "System.Data.Common/System.Data.ConstraintCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.ConstraintCollection", "Contains", True, "benchmark" },
    { 22, "System.Data.Common/System.Data.ConstraintCollection::IndexOf:System.Int32(System.Data.Constraint)", "codegen", "System.Data.ConstraintCollection", "IndexOf", True, "benchmark" },
    { 23, "System.Data.Common/System.Data.ConstraintCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.ConstraintCollection", "IndexOf", True, "benchmark" },
    { 24, "System.Data.Common/System.Data.ConstraintCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.ConstraintCollection", "IndexOf", True, "benchmark" },
    { 25, "System.Data.Common/System.Data.ConstraintCollection::Remove:System.Void(System.Data.Constraint)", "codegen", "System.Data.ConstraintCollection", "Remove", True, "benchmark" },
    { 26, "System.Data.Common/System.Data.ConstraintCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.ConstraintCollection", "RemoveAt", True, "benchmark" },
    { 27, "System.Data.Common/System.Data.ConstraintCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.ConstraintCollection", "RemoveAt", True, "benchmark" },
    { 28, "System.Data.Common/System.Data.ConstraintCollection::Remove:System.Void(System.String)", "codegen", "System.Data.ConstraintCollection", "Remove", True, "benchmark" },
    { 29, "System.Data.Common/System.Data.ConstraintCollection::Remove:System.Void(System.String)", "codegen", "System.Data.ConstraintCollection", "Remove", True, "benchmark" },
    { 30, "System.Data.Common/System.Data.DataColumn::SetOrdinal:System.Void(System.Int32)", "codegen", "System.Data.DataColumn", "SetOrdinal", True, "benchmark" },
    { 31, "System.Data.Common/System.Data.DataColumn::SetOrdinal:System.Void(System.Int32)", "codegen", "System.Data.DataColumn", "SetOrdinal", True, "benchmark" },
    { 32, "System.Data.Common/System.Data.DataColumnCollection::Add:System.Void(System.Data.DataColumn)", "codegen", "System.Data.DataColumnCollection", "Add", True, "benchmark" },
    { 33, "System.Data.Common/System.Data.DataColumnCollection::AddRange:System.Void(System.Data.DataColumn[])", "codegen", "System.Data.DataColumnCollection", "AddRange", True, "benchmark" },
    { 34, "System.Data.Common/System.Data.DataColumnCollection::AddRange:System.Void(System.Data.DataColumn[])", "codegen", "System.Data.DataColumnCollection", "AddRange", True, "benchmark" },
    { 35, "System.Data.Common/System.Data.DataColumnCollection::AddRange:System.Void(System.Data.DataColumn[])", "codegen", "System.Data.DataColumnCollection", "AddRange", True, "benchmark" },
    { 36, "System.Data.Common/System.Data.DataColumnCollection::Add:System.Data.DataColumn(System.String,System.Type,System.String)", "codegen", "System.Data.DataColumnCollection", "Add", True, "benchmark" },
    { 37, "System.Data.Common/System.Data.DataColumnCollection::Add:System.Data.DataColumn(System.String,System.Type,System.String)", "codegen", "System.Data.DataColumnCollection", "Add", True, "benchmark" },
    { 38, "System.Data.Common/System.Data.DataColumnCollection::Add:System.Data.DataColumn(System.String,System.Type,System.String)", "codegen", "System.Data.DataColumnCollection", "Add", True, "benchmark" },
    { 39, "System.Data.Common/System.Data.DataColumnCollection::Add:System.Data.DataColumn(System.String,System.Type)", "codegen", "System.Data.DataColumnCollection", "Add", True, "benchmark" },
    { 40, "System.Data.Common/System.Data.DataColumnCollection::Add:System.Data.DataColumn(System.String,System.Type)", "codegen", "System.Data.DataColumnCollection", "Add", True, "benchmark" },
    { 41, "System.Data.Common/System.Data.DataColumnCollection::Add:System.Data.DataColumn(System.String)", "codegen", "System.Data.DataColumnCollection", "Add", True, "benchmark" },
    { 42, "System.Data.Common/System.Data.DataColumnCollection::Add:System.Data.DataColumn(System.String)", "codegen", "System.Data.DataColumnCollection", "Add", True, "benchmark" },
    { 43, "System.Data.Common/System.Data.DataColumnCollection::Add:System.Data.DataColumn()", "codegen", "System.Data.DataColumnCollection", "Add", True, "benchmark" },
    { 44, "System.Data.Common/System.Data.DataColumnCollection::CanRemove:System.Boolean(System.Data.DataColumn)", "codegen", "System.Data.DataColumnCollection", "CanRemove", True, "benchmark" },
    { 45, "System.Data.Common/System.Data.DataColumnCollection::Clear:System.Void()", "codegen", "System.Data.DataColumnCollection", "Clear", True, "benchmark" },
    { 46, "System.Data.Common/System.Data.DataColumnCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.DataColumnCollection", "Contains", True, "benchmark" },
    { 47, "System.Data.Common/System.Data.DataColumnCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.DataColumnCollection", "Contains", True, "benchmark" },
    { 48, "System.Data.Common/System.Data.DataColumnCollection::IndexOf:System.Int32(System.Data.DataColumn)", "codegen", "System.Data.DataColumnCollection", "IndexOf", True, "benchmark" },
    { 49, "System.Data.Common/System.Data.DataColumnCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.DataColumnCollection", "IndexOf", True, "benchmark" },
    { 50, "System.Data.Common/System.Data.DataColumnCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.DataColumnCollection", "IndexOf", True, "benchmark" },
    { 51, "System.Data.Common/System.Data.DataColumnCollection::Remove:System.Void(System.Data.DataColumn)", "codegen", "System.Data.DataColumnCollection", "Remove", True, "benchmark" },
    { 52, "System.Data.Common/System.Data.DataColumnCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.DataColumnCollection", "RemoveAt", True, "benchmark" },
    { 53, "System.Data.Common/System.Data.DataColumnCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.DataColumnCollection", "RemoveAt", True, "benchmark" },
    { 54, "System.Data.Common/System.Data.DataColumnCollection::Remove:System.Void(System.String)", "codegen", "System.Data.DataColumnCollection", "Remove", True, "benchmark" },
    { 55, "System.Data.Common/System.Data.DataColumnCollection::Remove:System.Void(System.String)", "codegen", "System.Data.DataColumnCollection", "Remove", True, "benchmark" },
    { 56, "System.Data.Common/System.Data.DataReaderExtensions::GetBoolean:System.Boolean(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetBoolean", True, "benchmark" },
    { 57, "System.Data.Common/System.Data.DataReaderExtensions::GetBoolean:System.Boolean(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetBoolean", True, "benchmark" },
    { 58, "System.Data.Common/System.Data.DataReaderExtensions::GetByte:System.Byte(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetByte", True, "benchmark" },
    { 59, "System.Data.Common/System.Data.DataReaderExtensions::GetByte:System.Byte(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetByte", True, "benchmark" },
    { 60, "System.Data.Common/System.Data.DataReaderExtensions::GetBytes:System.Int64(System.Data.Common.DbDataReader,System.String,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.DataReaderExtensions", "GetBytes", True, "benchmark" },
    { 61, "System.Data.Common/System.Data.DataReaderExtensions::GetBytes:System.Int64(System.Data.Common.DbDataReader,System.String,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.DataReaderExtensions", "GetBytes", True, "benchmark" },
    { 62, "System.Data.Common/System.Data.DataReaderExtensions::GetBytes:System.Int64(System.Data.Common.DbDataReader,System.String,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.DataReaderExtensions", "GetBytes", True, "benchmark" },
    { 63, "System.Data.Common/System.Data.DataReaderExtensions::GetBytes:System.Int64(System.Data.Common.DbDataReader,System.String,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.DataReaderExtensions", "GetBytes", True, "benchmark" },
    { 64, "System.Data.Common/System.Data.DataReaderExtensions::GetBytes:System.Int64(System.Data.Common.DbDataReader,System.String,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.DataReaderExtensions", "GetBytes", True, "benchmark" },
    { 65, "System.Data.Common/System.Data.DataReaderExtensions::GetChar:System.Char(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetChar", True, "benchmark" },
    { 66, "System.Data.Common/System.Data.DataReaderExtensions::GetChar:System.Char(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetChar", True, "benchmark" },
    { 67, "System.Data.Common/System.Data.DataReaderExtensions::GetChars:System.Int64(System.Data.Common.DbDataReader,System.String,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.DataReaderExtensions", "GetChars", True, "benchmark" },
    { 68, "System.Data.Common/System.Data.DataReaderExtensions::GetChars:System.Int64(System.Data.Common.DbDataReader,System.String,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.DataReaderExtensions", "GetChars", True, "benchmark" },
    { 69, "System.Data.Common/System.Data.DataReaderExtensions::GetChars:System.Int64(System.Data.Common.DbDataReader,System.String,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.DataReaderExtensions", "GetChars", True, "benchmark" },
    { 70, "System.Data.Common/System.Data.DataReaderExtensions::GetChars:System.Int64(System.Data.Common.DbDataReader,System.String,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.DataReaderExtensions", "GetChars", True, "benchmark" },
    { 71, "System.Data.Common/System.Data.DataReaderExtensions::GetChars:System.Int64(System.Data.Common.DbDataReader,System.String,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.DataReaderExtensions", "GetChars", True, "benchmark" },
    { 72, "System.Data.Common/System.Data.DataReaderExtensions::GetData:System.Data.Common.DbDataReader(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetData", True, "benchmark" },
    { 73, "System.Data.Common/System.Data.DataReaderExtensions::GetData:System.Data.Common.DbDataReader(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetData", True, "benchmark" },
    { 74, "System.Data.Common/System.Data.DataReaderExtensions::GetDataTypeName:System.String(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetDataTypeName", True, "benchmark" },
    { 75, "System.Data.Common/System.Data.DataReaderExtensions::GetDataTypeName:System.String(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetDataTypeName", True, "benchmark" },
    { 76, "System.Data.Common/System.Data.DataReaderExtensions::GetDateTime:System.DateTime(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetDateTime", True, "benchmark" },
    { 77, "System.Data.Common/System.Data.DataReaderExtensions::GetDateTime:System.DateTime(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetDateTime", True, "benchmark" },
    { 78, "System.Data.Common/System.Data.DataReaderExtensions::GetDecimal:System.Decimal(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetDecimal", True, "benchmark" },
    { 79, "System.Data.Common/System.Data.DataReaderExtensions::GetDecimal:System.Decimal(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetDecimal", True, "benchmark" },
    { 80, "System.Data.Common/System.Data.DataReaderExtensions::GetDouble:System.Double(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetDouble", True, "benchmark" },
    { 81, "System.Data.Common/System.Data.DataReaderExtensions::GetDouble:System.Double(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetDouble", True, "benchmark" },
    { 82, "System.Data.Common/System.Data.DataReaderExtensions::GetFieldType:System.Type(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetFieldType", True, "benchmark" },
    { 83, "System.Data.Common/System.Data.DataReaderExtensions::GetFieldType:System.Type(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetFieldType", True, "benchmark" },
    { 84, "System.Data.Common/System.Data.DataReaderExtensions::GetFieldValue:System.Int32(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetFieldValue", True, "benchmark" },
    { 85, "System.Data.Common/System.Data.DataReaderExtensions::GetFieldValue:System.Int32(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetFieldValue", True, "benchmark" },
    { 86, "System.Data.Common/System.Data.DataReaderExtensions::GetFieldValueAsync:System.Threading.Tasks.Task<System.Int32>(System.Data.Common.DbDataReader,System.String,System.Threading.CancellationToken)", "codegen", "System.Data.DataReaderExtensions", "GetFieldValueAsync", True, "benchmark" },
    { 87, "System.Data.Common/System.Data.DataReaderExtensions::GetFieldValueAsync:System.Threading.Tasks.Task<System.Int32>(System.Data.Common.DbDataReader,System.String,System.Threading.CancellationToken)", "codegen", "System.Data.DataReaderExtensions", "GetFieldValueAsync", True, "benchmark" },
    { 88, "System.Data.Common/System.Data.DataReaderExtensions::GetFloat:System.Single(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetFloat", True, "benchmark" },
    { 89, "System.Data.Common/System.Data.DataReaderExtensions::GetFloat:System.Single(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetFloat", True, "benchmark" },
    { 90, "System.Data.Common/System.Data.DataReaderExtensions::GetGuid:System.Guid(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetGuid", True, "benchmark" },
    { 91, "System.Data.Common/System.Data.DataReaderExtensions::GetGuid:System.Guid(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetGuid", True, "benchmark" },
    { 92, "System.Data.Common/System.Data.DataReaderExtensions::GetInt16:System.Int16(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetInt16", True, "benchmark" },
    { 93, "System.Data.Common/System.Data.DataReaderExtensions::GetInt16:System.Int16(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetInt16", True, "benchmark" },
    { 94, "System.Data.Common/System.Data.DataReaderExtensions::GetInt32:System.Int32(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetInt32", True, "benchmark" },
    { 95, "System.Data.Common/System.Data.DataReaderExtensions::GetInt32:System.Int32(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetInt32", True, "benchmark" },
    { 96, "System.Data.Common/System.Data.DataReaderExtensions::GetInt64:System.Int64(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetInt64", True, "benchmark" },
    { 97, "System.Data.Common/System.Data.DataReaderExtensions::GetInt64:System.Int64(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetInt64", True, "benchmark" },
    { 98, "System.Data.Common/System.Data.DataReaderExtensions::GetProviderSpecificFieldType:System.Type(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetProviderSpecificFieldType", True, "benchmark" },
    { 99, "System.Data.Common/System.Data.DataReaderExtensions::GetProviderSpecificFieldType:System.Type(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetProviderSpecificFieldType", True, "benchmark" },
    { 100, "System.Data.Common/System.Data.DataReaderExtensions::GetProviderSpecificValue:System.Object(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetProviderSpecificValue", True, "benchmark" },
    { 101, "System.Data.Common/System.Data.DataReaderExtensions::GetProviderSpecificValue:System.Object(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetProviderSpecificValue", True, "benchmark" },
    { 102, "System.Data.Common/System.Data.DataReaderExtensions::GetStream:System.IO.Stream(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetStream", True, "benchmark" },
    { 103, "System.Data.Common/System.Data.DataReaderExtensions::GetStream:System.IO.Stream(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetStream", True, "benchmark" },
    { 104, "System.Data.Common/System.Data.DataReaderExtensions::GetString:System.String(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetString", True, "benchmark" },
    { 105, "System.Data.Common/System.Data.DataReaderExtensions::GetString:System.String(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetString", True, "benchmark" },
    { 106, "System.Data.Common/System.Data.DataReaderExtensions::GetTextReader:System.IO.TextReader(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetTextReader", True, "benchmark" },
    { 107, "System.Data.Common/System.Data.DataReaderExtensions::GetTextReader:System.IO.TextReader(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetTextReader", True, "benchmark" },
    { 108, "System.Data.Common/System.Data.DataReaderExtensions::GetValue:System.Object(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetValue", True, "benchmark" },
    { 109, "System.Data.Common/System.Data.DataReaderExtensions::GetValue:System.Object(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "GetValue", True, "benchmark" },
    { 110, "System.Data.Common/System.Data.DataReaderExtensions::IsDBNull:System.Boolean(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "IsDBNull", True, "benchmark" },
    { 111, "System.Data.Common/System.Data.DataReaderExtensions::IsDBNull:System.Boolean(System.Data.Common.DbDataReader,System.String)", "codegen", "System.Data.DataReaderExtensions", "IsDBNull", True, "benchmark" },
    { 112, "System.Data.Common/System.Data.DataReaderExtensions::IsDBNullAsync:System.Threading.Tasks.Task<System.Boolean>(System.Data.Common.DbDataReader,System.String,System.Threading.CancellationToken)", "codegen", "System.Data.DataReaderExtensions", "IsDBNullAsync", True, "benchmark" },
    { 113, "System.Data.Common/System.Data.DataReaderExtensions::IsDBNullAsync:System.Threading.Tasks.Task<System.Boolean>(System.Data.Common.DbDataReader,System.String,System.Threading.CancellationToken)", "codegen", "System.Data.DataReaderExtensions", "IsDBNullAsync", True, "benchmark" },
    { 114, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Void(System.Data.DataRelation)", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 115, "System.Data.Common/System.Data.DataRelationCollection::AddRange:System.Void(System.Data.DataRelation[])", "codegen", "System.Data.DataRelationCollection", "AddRange", True, "benchmark" },
    { 116, "System.Data.Common/System.Data.DataRelationCollection::AddRange:System.Void(System.Data.DataRelation[])", "codegen", "System.Data.DataRelationCollection", "AddRange", True, "benchmark" },
    { 117, "System.Data.Common/System.Data.DataRelationCollection::AddRange:System.Void(System.Data.DataRelation[])", "codegen", "System.Data.DataRelationCollection", "AddRange", True, "benchmark" },
    { 118, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 119, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 120, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 121, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 122, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn[],System.Data.DataColumn[],System.Boolean)", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 123, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn[],System.Data.DataColumn[],System.Boolean)", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 124, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn[],System.Data.DataColumn[],System.Boolean)", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 125, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn[],System.Data.DataColumn[],System.Boolean)", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 126, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 127, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 128, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.Data.DataColumn[],System.Data.DataColumn[])", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 129, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn,System.Data.DataColumn)", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 130, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn,System.Data.DataColumn)", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 131, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn,System.Data.DataColumn,System.Boolean)", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 132, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.String,System.Data.DataColumn,System.Data.DataColumn,System.Boolean)", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 133, "System.Data.Common/System.Data.DataRelationCollection::Add:System.Data.DataRelation(System.Data.DataColumn,System.Data.DataColumn)", "codegen", "System.Data.DataRelationCollection", "Add", True, "benchmark" },
    { 134, "System.Data.Common/System.Data.DataRelationCollection::Clear:System.Void()", "codegen", "System.Data.DataRelationCollection", "Clear", True, "benchmark" },
    { 135, "System.Data.Common/System.Data.DataRelationCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.DataRelationCollection", "Contains", True, "benchmark" },
    { 136, "System.Data.Common/System.Data.DataRelationCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.DataRelationCollection", "Contains", True, "benchmark" },
    { 137, "System.Data.Common/System.Data.DataRelationCollection::IndexOf:System.Int32(System.Data.DataRelation)", "codegen", "System.Data.DataRelationCollection", "IndexOf", True, "benchmark" },
    { 138, "System.Data.Common/System.Data.DataRelationCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.DataRelationCollection", "IndexOf", True, "benchmark" },
    { 139, "System.Data.Common/System.Data.DataRelationCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.DataRelationCollection", "IndexOf", True, "benchmark" },
    { 140, "System.Data.Common/System.Data.DataRelationCollection::CanRemove:System.Boolean(System.Data.DataRelation)", "codegen", "System.Data.DataRelationCollection", "CanRemove", True, "benchmark" },
    { 141, "System.Data.Common/System.Data.DataRelationCollection::Remove:System.Void(System.Data.DataRelation)", "codegen", "System.Data.DataRelationCollection", "Remove", True, "benchmark" },
    { 142, "System.Data.Common/System.Data.DataRelationCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.DataRelationCollection", "RemoveAt", True, "benchmark" },
    { 143, "System.Data.Common/System.Data.DataRelationCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.DataRelationCollection", "RemoveAt", True, "benchmark" },
    { 144, "System.Data.Common/System.Data.DataRelationCollection::Remove:System.Void(System.String)", "codegen", "System.Data.DataRelationCollection", "Remove", True, "benchmark" },
    { 145, "System.Data.Common/System.Data.DataRelationCollection::Remove:System.Void(System.String)", "codegen", "System.Data.DataRelationCollection", "Remove", True, "benchmark" },
    { 146, "System.Data.Common/System.Data.DataRow::AcceptChanges:System.Void()", "codegen", "System.Data.DataRow", "AcceptChanges", True, "benchmark" },
    { 147, "System.Data.Common/System.Data.DataRow::BeginEdit:System.Void()", "codegen", "System.Data.DataRow", "BeginEdit", True, "benchmark" },
    { 148, "System.Data.Common/System.Data.DataRow::CancelEdit:System.Void()", "codegen", "System.Data.DataRow", "CancelEdit", True, "benchmark" },
    { 149, "System.Data.Common/System.Data.DataRow::Delete:System.Void()", "codegen", "System.Data.DataRow", "Delete", True, "benchmark" },
    { 150, "System.Data.Common/System.Data.DataRow::EndEdit:System.Void()", "codegen", "System.Data.DataRow", "EndEdit", True, "benchmark" },
    { 151, "System.Data.Common/System.Data.DataRow::SetColumnError:System.Void(System.Int32,System.String)", "codegen", "System.Data.DataRow", "SetColumnError", True, "benchmark" },
    { 152, "System.Data.Common/System.Data.DataRow::SetColumnError:System.Void(System.Int32,System.String)", "codegen", "System.Data.DataRow", "SetColumnError", True, "benchmark" },
    { 153, "System.Data.Common/System.Data.DataRow::SetColumnError:System.Void(System.Int32,System.String)", "codegen", "System.Data.DataRow", "SetColumnError", True, "benchmark" },
    { 154, "System.Data.Common/System.Data.DataRow::SetColumnError:System.Void(System.String,System.String)", "codegen", "System.Data.DataRow", "SetColumnError", True, "benchmark" },
    { 155, "System.Data.Common/System.Data.DataRow::SetColumnError:System.Void(System.String,System.String)", "codegen", "System.Data.DataRow", "SetColumnError", True, "benchmark" },
    { 156, "System.Data.Common/System.Data.DataRow::SetColumnError:System.Void(System.String,System.String)", "codegen", "System.Data.DataRow", "SetColumnError", True, "benchmark" },
    { 157, "System.Data.Common/System.Data.DataRow::SetColumnError:System.Void(System.Data.DataColumn,System.String)", "codegen", "System.Data.DataRow", "SetColumnError", True, "benchmark" },
    { 158, "System.Data.Common/System.Data.DataRow::SetColumnError:System.Void(System.Data.DataColumn,System.String)", "codegen", "System.Data.DataRow", "SetColumnError", True, "benchmark" },
    { 159, "System.Data.Common/System.Data.DataRow::GetColumnError:System.String(System.Int32)", "codegen", "System.Data.DataRow", "GetColumnError", True, "benchmark" },
    { 160, "System.Data.Common/System.Data.DataRow::GetColumnError:System.String(System.Int32)", "codegen", "System.Data.DataRow", "GetColumnError", True, "benchmark" },
    { 161, "System.Data.Common/System.Data.DataRow::GetColumnError:System.String(System.String)", "codegen", "System.Data.DataRow", "GetColumnError", True, "benchmark" },
    { 162, "System.Data.Common/System.Data.DataRow::GetColumnError:System.String(System.String)", "codegen", "System.Data.DataRow", "GetColumnError", True, "benchmark" },
    { 163, "System.Data.Common/System.Data.DataRow::GetColumnError:System.String(System.Data.DataColumn)", "codegen", "System.Data.DataRow", "GetColumnError", True, "benchmark" },
    { 164, "System.Data.Common/System.Data.DataRow::ClearErrors:System.Void()", "codegen", "System.Data.DataRow", "ClearErrors", True, "benchmark" },
    { 165, "System.Data.Common/System.Data.DataRow::GetColumnsInError:System.Data.DataColumn[]()", "codegen", "System.Data.DataRow", "GetColumnsInError", True, "benchmark" },
    { 166, "System.Data.Common/System.Data.DataRow::GetChildRows:System.Data.DataRow[](System.String)", "codegen", "System.Data.DataRow", "GetChildRows", True, "benchmark" },
    { 167, "System.Data.Common/System.Data.DataRow::GetChildRows:System.Data.DataRow[](System.String)", "codegen", "System.Data.DataRow", "GetChildRows", True, "benchmark" },
    { 168, "System.Data.Common/System.Data.DataRow::GetChildRows:System.Data.DataRow[](System.String,System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "GetChildRows", True, "benchmark" },
    { 169, "System.Data.Common/System.Data.DataRow::GetChildRows:System.Data.DataRow[](System.String,System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "GetChildRows", True, "benchmark" },
    { 170, "System.Data.Common/System.Data.DataRow::GetChildRows:System.Data.DataRow[](System.Data.DataRelation)", "codegen", "System.Data.DataRow", "GetChildRows", True, "benchmark" },
    { 171, "System.Data.Common/System.Data.DataRow::GetChildRows:System.Data.DataRow[](System.Data.DataRelation,System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "GetChildRows", True, "benchmark" },
    { 172, "System.Data.Common/System.Data.DataRow::GetParentRow:System.Data.DataRow(System.String)", "codegen", "System.Data.DataRow", "GetParentRow", True, "benchmark" },
    { 173, "System.Data.Common/System.Data.DataRow::GetParentRow:System.Data.DataRow(System.String)", "codegen", "System.Data.DataRow", "GetParentRow", True, "benchmark" },
    { 174, "System.Data.Common/System.Data.DataRow::GetParentRow:System.Data.DataRow(System.String,System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "GetParentRow", True, "benchmark" },
    { 175, "System.Data.Common/System.Data.DataRow::GetParentRow:System.Data.DataRow(System.String,System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "GetParentRow", True, "benchmark" },
    { 176, "System.Data.Common/System.Data.DataRow::GetParentRow:System.Data.DataRow(System.Data.DataRelation)", "codegen", "System.Data.DataRow", "GetParentRow", True, "benchmark" },
    { 177, "System.Data.Common/System.Data.DataRow::GetParentRow:System.Data.DataRow(System.Data.DataRelation,System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "GetParentRow", True, "benchmark" },
    { 178, "System.Data.Common/System.Data.DataRow::GetParentRows:System.Data.DataRow[](System.String)", "codegen", "System.Data.DataRow", "GetParentRows", True, "benchmark" },
    { 179, "System.Data.Common/System.Data.DataRow::GetParentRows:System.Data.DataRow[](System.String)", "codegen", "System.Data.DataRow", "GetParentRows", True, "benchmark" },
    { 180, "System.Data.Common/System.Data.DataRow::GetParentRows:System.Data.DataRow[](System.String,System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "GetParentRows", True, "benchmark" },
    { 181, "System.Data.Common/System.Data.DataRow::GetParentRows:System.Data.DataRow[](System.String,System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "GetParentRows", True, "benchmark" },
    { 182, "System.Data.Common/System.Data.DataRow::GetParentRows:System.Data.DataRow[](System.Data.DataRelation)", "codegen", "System.Data.DataRow", "GetParentRows", True, "benchmark" },
    { 183, "System.Data.Common/System.Data.DataRow::GetParentRows:System.Data.DataRow[](System.Data.DataRelation,System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "GetParentRows", True, "benchmark" },
    { 184, "System.Data.Common/System.Data.DataRow::HasVersion:System.Boolean(System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "HasVersion", True, "benchmark" },
    { 185, "System.Data.Common/System.Data.DataRow::IsNull:System.Boolean(System.Int32)", "codegen", "System.Data.DataRow", "IsNull", True, "benchmark" },
    { 186, "System.Data.Common/System.Data.DataRow::IsNull:System.Boolean(System.Int32)", "codegen", "System.Data.DataRow", "IsNull", True, "benchmark" },
    { 187, "System.Data.Common/System.Data.DataRow::IsNull:System.Boolean(System.String)", "codegen", "System.Data.DataRow", "IsNull", True, "benchmark" },
    { 188, "System.Data.Common/System.Data.DataRow::IsNull:System.Boolean(System.String)", "codegen", "System.Data.DataRow", "IsNull", True, "benchmark" },
    { 189, "System.Data.Common/System.Data.DataRow::IsNull:System.Boolean(System.Data.DataColumn)", "codegen", "System.Data.DataRow", "IsNull", True, "benchmark" },
    { 190, "System.Data.Common/System.Data.DataRow::IsNull:System.Boolean(System.Data.DataColumn,System.Data.DataRowVersion)", "codegen", "System.Data.DataRow", "IsNull", True, "benchmark" },
    { 191, "System.Data.Common/System.Data.DataRow::RejectChanges:System.Void()", "codegen", "System.Data.DataRow", "RejectChanges", True, "benchmark" },
    { 192, "System.Data.Common/System.Data.DataRow::SetParentRow:System.Void(System.Data.DataRow)", "codegen", "System.Data.DataRow", "SetParentRow", True, "benchmark" },
    { 193, "System.Data.Common/System.Data.DataRow::SetParentRow:System.Void(System.Data.DataRow,System.Data.DataRelation)", "codegen", "System.Data.DataRow", "SetParentRow", True, "benchmark" },
    { 194, "System.Data.Common/System.Data.DataRow::SetAdded:System.Void()", "codegen", "System.Data.DataRow", "SetAdded", True, "benchmark" },
    { 195, "System.Data.Common/System.Data.DataRow::SetModified:System.Void()", "codegen", "System.Data.DataRow", "SetModified", True, "benchmark" },
    { 196, "System.Data.Common/System.Data.DataRowCollection::Add:System.Void(System.Data.DataRow)", "codegen", "System.Data.DataRowCollection", "Add", True, "benchmark" },
    { 197, "System.Data.Common/System.Data.DataRowCollection::InsertAt:System.Void(System.Data.DataRow,System.Int32)", "codegen", "System.Data.DataRowCollection", "InsertAt", True, "benchmark" },
    { 198, "System.Data.Common/System.Data.DataRowCollection::InsertAt:System.Void(System.Data.DataRow,System.Int32)", "codegen", "System.Data.DataRowCollection", "InsertAt", True, "benchmark" },
    { 199, "System.Data.Common/System.Data.DataRowCollection::IndexOf:System.Int32(System.Data.DataRow)", "codegen", "System.Data.DataRowCollection", "IndexOf", True, "benchmark" },
    { 200, "System.Data.Common/System.Data.DataRowCollection::Add:System.Data.DataRow(System.Object[])", "codegen", "System.Data.DataRowCollection", "Add", True, "benchmark" },
    { 201, "System.Data.Common/System.Data.DataRowCollection::Add:System.Data.DataRow(System.Object[])", "codegen", "System.Data.DataRowCollection", "Add", True, "benchmark" },
    { 202, "System.Data.Common/System.Data.DataRowCollection::Add:System.Data.DataRow(System.Object[])", "codegen", "System.Data.DataRowCollection", "Add", True, "benchmark" },
    { 203, "System.Data.Common/System.Data.DataRowCollection::Find:System.Data.DataRow(System.Object)", "codegen", "System.Data.DataRowCollection", "Find", True, "benchmark" },
    { 204, "System.Data.Common/System.Data.DataRowCollection::Find:System.Data.DataRow(System.Object[])", "codegen", "System.Data.DataRowCollection", "Find", True, "benchmark" },
    { 205, "System.Data.Common/System.Data.DataRowCollection::Find:System.Data.DataRow(System.Object[])", "codegen", "System.Data.DataRowCollection", "Find", True, "benchmark" },
    { 206, "System.Data.Common/System.Data.DataRowCollection::Find:System.Data.DataRow(System.Object[])", "codegen", "System.Data.DataRowCollection", "Find", True, "benchmark" },
    { 207, "System.Data.Common/System.Data.DataRowCollection::Clear:System.Void()", "codegen", "System.Data.DataRowCollection", "Clear", True, "benchmark" },
    { 208, "System.Data.Common/System.Data.DataRowCollection::Contains:System.Boolean(System.Object)", "codegen", "System.Data.DataRowCollection", "Contains", True, "benchmark" },
    { 209, "System.Data.Common/System.Data.DataRowCollection::Contains:System.Boolean(System.Object[])", "codegen", "System.Data.DataRowCollection", "Contains", True, "benchmark" },
    { 210, "System.Data.Common/System.Data.DataRowCollection::Contains:System.Boolean(System.Object[])", "codegen", "System.Data.DataRowCollection", "Contains", True, "benchmark" },
    { 211, "System.Data.Common/System.Data.DataRowCollection::Contains:System.Boolean(System.Object[])", "codegen", "System.Data.DataRowCollection", "Contains", True, "benchmark" },
    { 212, "System.Data.Common/System.Data.DataRowCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.DataRowCollection", "CopyTo", True, "benchmark" },
    { 213, "System.Data.Common/System.Data.DataRowCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.DataRowCollection", "CopyTo", True, "benchmark" },
    { 214, "System.Data.Common/System.Data.DataRowCollection::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Data.DataRowCollection", "GetEnumerator", True, "benchmark" },
    { 215, "System.Data.Common/System.Data.DataRowCollection::Remove:System.Void(System.Data.DataRow)", "codegen", "System.Data.DataRowCollection", "Remove", True, "benchmark" },
    { 216, "System.Data.Common/System.Data.DataRowCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.DataRowCollection", "RemoveAt", True, "benchmark" },
    { 217, "System.Data.Common/System.Data.DataRowCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.DataRowCollection", "RemoveAt", True, "benchmark" },
    { 218, "System.Data.Common/System.Data.DataRowExtensions::Field:System.Int32(System.Data.DataRow,System.String)", "codegen", "System.Data.DataRowExtensions", "Field", True, "benchmark" },
    { 219, "System.Data.Common/System.Data.DataRowExtensions::Field:System.Int32(System.Data.DataRow,System.String)", "codegen", "System.Data.DataRowExtensions", "Field", True, "benchmark" },
    { 220, "System.Data.Common/System.Data.DataRowExtensions::Field:System.Int32(System.Data.DataRow,System.Data.DataColumn)", "codegen", "System.Data.DataRowExtensions", "Field", True, "benchmark" },
    { 221, "System.Data.Common/System.Data.DataRowExtensions::Field:System.Int32(System.Data.DataRow,System.Int32)", "codegen", "System.Data.DataRowExtensions", "Field", True, "benchmark" },
    { 222, "System.Data.Common/System.Data.DataRowExtensions::Field:System.Int32(System.Data.DataRow,System.Int32)", "codegen", "System.Data.DataRowExtensions", "Field", True, "benchmark" },
    { 223, "System.Data.Common/System.Data.DataRowExtensions::Field:System.Int32(System.Data.DataRow,System.Int32,System.Data.DataRowVersion)", "codegen", "System.Data.DataRowExtensions", "Field", True, "benchmark" },
    { 224, "System.Data.Common/System.Data.DataRowExtensions::Field:System.Int32(System.Data.DataRow,System.Int32,System.Data.DataRowVersion)", "codegen", "System.Data.DataRowExtensions", "Field", True, "benchmark" },
    { 225, "System.Data.Common/System.Data.DataRowExtensions::Field:System.Int32(System.Data.DataRow,System.String,System.Data.DataRowVersion)", "codegen", "System.Data.DataRowExtensions", "Field", True, "benchmark" },
    { 226, "System.Data.Common/System.Data.DataRowExtensions::Field:System.Int32(System.Data.DataRow,System.String,System.Data.DataRowVersion)", "codegen", "System.Data.DataRowExtensions", "Field", True, "benchmark" },
    { 227, "System.Data.Common/System.Data.DataRowExtensions::Field:System.Int32(System.Data.DataRow,System.Data.DataColumn,System.Data.DataRowVersion)", "codegen", "System.Data.DataRowExtensions", "Field", True, "benchmark" },
    { 228, "System.Data.Common/System.Data.DataRowExtensions::SetField:System.Void(System.Data.DataRow,System.Int32,System.Int32)", "codegen", "System.Data.DataRowExtensions", "SetField", True, "benchmark" },
    { 229, "System.Data.Common/System.Data.DataRowExtensions::SetField:System.Void(System.Data.DataRow,System.Int32,System.Int32)", "codegen", "System.Data.DataRowExtensions", "SetField", True, "benchmark" },
    { 230, "System.Data.Common/System.Data.DataRowExtensions::SetField:System.Void(System.Data.DataRow,System.Int32,System.Int32)", "codegen", "System.Data.DataRowExtensions", "SetField", True, "benchmark" },
    { 231, "System.Data.Common/System.Data.DataRowExtensions::SetField:System.Void(System.Data.DataRow,System.String,System.Int32)", "codegen", "System.Data.DataRowExtensions", "SetField", True, "benchmark" },
    { 232, "System.Data.Common/System.Data.DataRowExtensions::SetField:System.Void(System.Data.DataRow,System.String,System.Int32)", "codegen", "System.Data.DataRowExtensions", "SetField", True, "benchmark" },
    { 233, "System.Data.Common/System.Data.DataRowExtensions::SetField:System.Void(System.Data.DataRow,System.String,System.Int32)", "codegen", "System.Data.DataRowExtensions", "SetField", True, "benchmark" },
    { 234, "System.Data.Common/System.Data.DataRowExtensions::SetField:System.Void(System.Data.DataRow,System.Data.DataColumn,System.Int32)", "codegen", "System.Data.DataRowExtensions", "SetField", True, "benchmark" },
    { 235, "System.Data.Common/System.Data.DataRowExtensions::SetField:System.Void(System.Data.DataRow,System.Data.DataColumn,System.Int32)", "codegen", "System.Data.DataRowExtensions", "SetField", True, "benchmark" },
    { 236, "System.Data.Common/System.Data.DataRowView::CreateChildView:System.Data.DataView(System.Data.DataRelation,System.Boolean)", "codegen", "System.Data.DataRowView", "CreateChildView", True, "benchmark" },
    { 237, "System.Data.Common/System.Data.DataRowView::CreateChildView:System.Data.DataView(System.Data.DataRelation,System.Boolean)", "codegen", "System.Data.DataRowView", "CreateChildView", True, "benchmark" },
    { 238, "System.Data.Common/System.Data.DataRowView::CreateChildView:System.Data.DataView(System.Data.DataRelation)", "codegen", "System.Data.DataRowView", "CreateChildView", True, "benchmark" },
    { 239, "System.Data.Common/System.Data.DataRowView::CreateChildView:System.Data.DataView(System.String,System.Boolean)", "codegen", "System.Data.DataRowView", "CreateChildView", True, "benchmark" },
    { 240, "System.Data.Common/System.Data.DataRowView::CreateChildView:System.Data.DataView(System.String,System.Boolean)", "codegen", "System.Data.DataRowView", "CreateChildView", True, "benchmark" },
    { 241, "System.Data.Common/System.Data.DataRowView::CreateChildView:System.Data.DataView(System.String,System.Boolean)", "codegen", "System.Data.DataRowView", "CreateChildView", True, "benchmark" },
    { 242, "System.Data.Common/System.Data.DataRowView::CreateChildView:System.Data.DataView(System.String)", "codegen", "System.Data.DataRowView", "CreateChildView", True, "benchmark" },
    { 243, "System.Data.Common/System.Data.DataRowView::CreateChildView:System.Data.DataView(System.String)", "codegen", "System.Data.DataRowView", "CreateChildView", True, "benchmark" },
    { 244, "System.Data.Common/System.Data.DataRowView::BeginEdit:System.Void()", "codegen", "System.Data.DataRowView", "BeginEdit", True, "benchmark" },
    { 245, "System.Data.Common/System.Data.DataRowView::CancelEdit:System.Void()", "codegen", "System.Data.DataRowView", "CancelEdit", True, "benchmark" },
    { 246, "System.Data.Common/System.Data.DataRowView::EndEdit:System.Void()", "codegen", "System.Data.DataRowView", "EndEdit", True, "benchmark" },
    { 247, "System.Data.Common/System.Data.DataRowView::Delete:System.Void()", "codegen", "System.Data.DataRowView", "Delete", True, "benchmark" },
    { 248, "System.Data.Common/System.Data.DataSet::AcceptChanges:System.Void()", "codegen", "System.Data.DataSet", "AcceptChanges", True, "benchmark" },
    { 249, "System.Data.Common/System.Data.DataSet::BeginInit:System.Void()", "codegen", "System.Data.DataSet", "BeginInit", True, "benchmark" },
    { 250, "System.Data.Common/System.Data.DataSet::EndInit:System.Void()", "codegen", "System.Data.DataSet", "EndInit", True, "benchmark" },
    { 251, "System.Data.Common/System.Data.DataSet::Clear:System.Void()", "codegen", "System.Data.DataSet", "Clear", True, "benchmark" },
    { 252, "System.Data.Common/System.Data.DataSet::Clone:System.Data.DataSet()", "codegen", "System.Data.DataSet", "Clone", True, "benchmark" },
    { 253, "System.Data.Common/System.Data.DataSet::Copy:System.Data.DataSet()", "codegen", "System.Data.DataSet", "Copy", True, "benchmark" },
    { 254, "System.Data.Common/System.Data.DataSet::GetChanges:System.Data.DataSet()", "codegen", "System.Data.DataSet", "GetChanges", True, "benchmark" },
    { 255, "System.Data.Common/System.Data.DataSet::GetChanges:System.Data.DataSet(System.Data.DataRowState)", "codegen", "System.Data.DataSet", "GetChanges", True, "benchmark" },
    { 256, "System.Data.Common/System.Data.DataSet::GetXml:System.String()", "codegen", "System.Data.DataSet", "GetXml", True, "benchmark" },
    { 257, "System.Data.Common/System.Data.DataSet::GetXmlSchema:System.String()", "codegen", "System.Data.DataSet", "GetXmlSchema", True, "benchmark" },
    { 258, "System.Data.Common/System.Data.DataSet::HasChanges:System.Boolean()", "codegen", "System.Data.DataSet", "HasChanges", True, "benchmark" },
    { 259, "System.Data.Common/System.Data.DataSet::HasChanges:System.Boolean(System.Data.DataRowState)", "codegen", "System.Data.DataSet", "HasChanges", True, "benchmark" },
    { 260, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.Xml.XmlReader,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 261, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.Xml.XmlReader,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 262, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.Xml.XmlReader,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 263, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.IO.Stream,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 264, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.IO.Stream,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 265, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.IO.Stream,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 266, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.IO.TextReader,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 267, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.IO.TextReader,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 268, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.IO.TextReader,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 269, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.String,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 270, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.String,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 271, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.String,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 272, "System.Data.Common/System.Data.DataSet::InferXmlSchema:System.Void(System.String,System.String[])", "codegen", "System.Data.DataSet", "InferXmlSchema", True, "benchmark" },
    { 273, "System.Data.Common/System.Data.DataSet::ReadXmlSchema:System.Void(System.Xml.XmlReader)", "codegen", "System.Data.DataSet", "ReadXmlSchema", True, "benchmark" },
    { 274, "System.Data.Common/System.Data.DataSet::ReadXmlSchema:System.Void(System.IO.Stream)", "codegen", "System.Data.DataSet", "ReadXmlSchema", True, "benchmark" },
    { 275, "System.Data.Common/System.Data.DataSet::ReadXmlSchema:System.Void(System.IO.Stream)", "codegen", "System.Data.DataSet", "ReadXmlSchema", True, "benchmark" },
    { 276, "System.Data.Common/System.Data.DataSet::ReadXmlSchema:System.Void(System.IO.TextReader)", "codegen", "System.Data.DataSet", "ReadXmlSchema", True, "benchmark" },
    { 277, "System.Data.Common/System.Data.DataSet::ReadXmlSchema:System.Void(System.IO.TextReader)", "codegen", "System.Data.DataSet", "ReadXmlSchema", True, "benchmark" },
    { 278, "System.Data.Common/System.Data.DataSet::ReadXmlSchema:System.Void(System.String)", "codegen", "System.Data.DataSet", "ReadXmlSchema", True, "benchmark" },
    { 279, "System.Data.Common/System.Data.DataSet::ReadXmlSchema:System.Void(System.String)", "codegen", "System.Data.DataSet", "ReadXmlSchema", True, "benchmark" },
    { 280, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.IO.Stream)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 281, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.IO.Stream)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 282, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.IO.Stream,System.Converter<System.Type,System.String>)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 283, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.IO.Stream,System.Converter<System.Type,System.String>)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 284, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.String)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 285, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.String)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 286, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.String,System.Converter<System.Type,System.String>)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 287, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.String,System.Converter<System.Type,System.String>)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 288, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.String,System.Converter<System.Type,System.String>)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 289, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.IO.TextWriter)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 290, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.IO.TextWriter)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 291, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.IO.TextWriter,System.Converter<System.Type,System.String>)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 292, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.IO.TextWriter,System.Converter<System.Type,System.String>)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 293, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.Xml.XmlWriter)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 294, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.Xml.XmlWriter,System.Converter<System.Type,System.String>)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 295, "System.Data.Common/System.Data.DataSet::WriteXmlSchema:System.Void(System.Xml.XmlWriter,System.Converter<System.Type,System.String>)", "codegen", "System.Data.DataSet", "WriteXmlSchema", True, "benchmark" },
    { 296, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.Xml.XmlReader)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 297, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.IO.Stream)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 298, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.IO.Stream)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 299, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.IO.TextReader)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 300, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.IO.TextReader)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 301, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.String)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 302, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.String)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 303, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.Xml.XmlReader,System.Data.XmlReadMode)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 304, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.IO.Stream,System.Data.XmlReadMode)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 305, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.IO.Stream,System.Data.XmlReadMode)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 306, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.IO.TextReader,System.Data.XmlReadMode)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 307, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.IO.TextReader,System.Data.XmlReadMode)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 308, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.String,System.Data.XmlReadMode)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 309, "System.Data.Common/System.Data.DataSet::ReadXml:System.Data.XmlReadMode(System.String,System.Data.XmlReadMode)", "codegen", "System.Data.DataSet", "ReadXml", True, "benchmark" },
    { 310, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.IO.Stream)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 311, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.IO.Stream)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 312, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.IO.TextWriter)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 313, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.IO.TextWriter)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 314, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.Xml.XmlWriter)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 315, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.String)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 316, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.String)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 317, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.IO.Stream,System.Data.XmlWriteMode)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 318, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.IO.Stream,System.Data.XmlWriteMode)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 319, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.IO.TextWriter,System.Data.XmlWriteMode)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 320, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.IO.TextWriter,System.Data.XmlWriteMode)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 321, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.Xml.XmlWriter,System.Data.XmlWriteMode)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 322, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.String,System.Data.XmlWriteMode)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 323, "System.Data.Common/System.Data.DataSet::WriteXml:System.Void(System.String,System.Data.XmlWriteMode)", "codegen", "System.Data.DataSet", "WriteXml", True, "benchmark" },
    { 324, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataSet)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 325, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataSet,System.Boolean)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 326, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataSet,System.Boolean)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 327, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataSet,System.Boolean,System.Data.MissingSchemaAction)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 328, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataSet,System.Boolean,System.Data.MissingSchemaAction)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 329, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataTable)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 330, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataTable,System.Boolean,System.Data.MissingSchemaAction)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 331, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataTable,System.Boolean,System.Data.MissingSchemaAction)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 332, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataRow[])", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 333, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataRow[])", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 334, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataRow[])", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 335, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataRow[],System.Boolean,System.Data.MissingSchemaAction)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 336, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataRow[],System.Boolean,System.Data.MissingSchemaAction)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 337, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataRow[],System.Boolean,System.Data.MissingSchemaAction)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 338, "System.Data.Common/System.Data.DataSet::Merge:System.Void(System.Data.DataRow[],System.Boolean,System.Data.MissingSchemaAction)", "codegen", "System.Data.DataSet", "Merge", True, "benchmark" },
    { 339, "System.Data.Common/System.Data.DataSet::RejectChanges:System.Void()", "codegen", "System.Data.DataSet", "RejectChanges", True, "benchmark" },
    { 340, "System.Data.Common/System.Data.DataSet::GetDataSetSchema:System.Xml.Schema.XmlSchemaComplexType(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.DataSet", "GetDataSetSchema", True, "benchmark" },
    { 341, "System.Data.Common/System.Data.DataSet::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption,System.Data.FillErrorEventHandler,System.Data.DataTable[])", "codegen", "System.Data.DataSet", "Load", True, "benchmark" },
    { 342, "System.Data.Common/System.Data.DataSet::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption,System.Data.FillErrorEventHandler,System.Data.DataTable[])", "codegen", "System.Data.DataSet", "Load", True, "benchmark" },
    { 343, "System.Data.Common/System.Data.DataSet::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption,System.Data.FillErrorEventHandler,System.Data.DataTable[])", "codegen", "System.Data.DataSet", "Load", True, "benchmark" },
    { 344, "System.Data.Common/System.Data.DataSet::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption,System.Data.DataTable[])", "codegen", "System.Data.DataSet", "Load", True, "benchmark" },
    { 345, "System.Data.Common/System.Data.DataSet::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption,System.Data.DataTable[])", "codegen", "System.Data.DataSet", "Load", True, "benchmark" },
    { 346, "System.Data.Common/System.Data.DataSet::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption,System.Data.DataTable[])", "codegen", "System.Data.DataSet", "Load", True, "benchmark" },
    { 347, "System.Data.Common/System.Data.DataSet::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption,System.String[])", "codegen", "System.Data.DataSet", "Load", True, "benchmark" },
    { 348, "System.Data.Common/System.Data.DataSet::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption,System.String[])", "codegen", "System.Data.DataSet", "Load", True, "benchmark" },
    { 349, "System.Data.Common/System.Data.DataSet::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption,System.String[])", "codegen", "System.Data.DataSet", "Load", True, "benchmark" },
    { 350, "System.Data.Common/System.Data.DataSet::CreateDataReader:System.Data.DataTableReader()", "codegen", "System.Data.DataSet", "CreateDataReader", True, "benchmark" },
    { 351, "System.Data.Common/System.Data.DataSet::CreateDataReader:System.Data.DataTableReader(System.Data.DataTable[])", "codegen", "System.Data.DataSet", "CreateDataReader", True, "benchmark" },
    { 352, "System.Data.Common/System.Data.DataSet::CreateDataReader:System.Data.DataTableReader(System.Data.DataTable[])", "codegen", "System.Data.DataSet", "CreateDataReader", True, "benchmark" },
    { 353, "System.Data.Common/System.Data.DataSet::CreateDataReader:System.Data.DataTableReader(System.Data.DataTable[])", "codegen", "System.Data.DataSet", "CreateDataReader", True, "benchmark" },
    { 354, "System.Data.Common/System.Data.DataTable::BeginInit:System.Void()", "codegen", "System.Data.DataTable", "BeginInit", True, "benchmark" },
    { 355, "System.Data.Common/System.Data.DataTable::EndInit:System.Void()", "codegen", "System.Data.DataTable", "EndInit", True, "benchmark" },
    { 356, "System.Data.Common/System.Data.DataTable::AcceptChanges:System.Void()", "codegen", "System.Data.DataTable", "AcceptChanges", True, "benchmark" },
    { 357, "System.Data.Common/System.Data.DataTable::Clone:System.Data.DataTable()", "codegen", "System.Data.DataTable", "Clone", True, "benchmark" },
    { 358, "System.Data.Common/System.Data.DataTable::Copy:System.Data.DataTable()", "codegen", "System.Data.DataTable", "Copy", True, "benchmark" },
    { 359, "System.Data.Common/System.Data.DataTable::Clear:System.Void()", "codegen", "System.Data.DataTable", "Clear", True, "benchmark" },
    { 360, "System.Data.Common/System.Data.DataTable::Compute:System.Object(System.String,System.String)", "codegen", "System.Data.DataTable", "Compute", True, "benchmark" },
    { 361, "System.Data.Common/System.Data.DataTable::Compute:System.Object(System.String,System.String)", "codegen", "System.Data.DataTable", "Compute", True, "benchmark" },
    { 362, "System.Data.Common/System.Data.DataTable::Compute:System.Object(System.String,System.String)", "codegen", "System.Data.DataTable", "Compute", True, "benchmark" },
    { 363, "System.Data.Common/System.Data.DataTable::GetChanges:System.Data.DataTable()", "codegen", "System.Data.DataTable", "GetChanges", True, "benchmark" },
    { 364, "System.Data.Common/System.Data.DataTable::GetChanges:System.Data.DataTable(System.Data.DataRowState)", "codegen", "System.Data.DataTable", "GetChanges", True, "benchmark" },
    { 365, "System.Data.Common/System.Data.DataTable::GetErrors:System.Data.DataRow[]()", "codegen", "System.Data.DataTable", "GetErrors", True, "benchmark" },
    { 366, "System.Data.Common/System.Data.DataTable::ImportRow:System.Void(System.Data.DataRow)", "codegen", "System.Data.DataTable", "ImportRow", True, "benchmark" },
    { 367, "System.Data.Common/System.Data.DataTable::NewRow:System.Data.DataRow()", "codegen", "System.Data.DataTable", "NewRow", True, "benchmark" },
    { 368, "System.Data.Common/System.Data.DataTable::RejectChanges:System.Void()", "codegen", "System.Data.DataTable", "RejectChanges", True, "benchmark" },
    { 369, "System.Data.Common/System.Data.DataTable::Select:System.Data.DataRow[]()", "codegen", "System.Data.DataTable", "Select", True, "benchmark" },
    { 370, "System.Data.Common/System.Data.DataTable::Select:System.Data.DataRow[](System.String)", "codegen", "System.Data.DataTable", "Select", True, "benchmark" },
    { 371, "System.Data.Common/System.Data.DataTable::Select:System.Data.DataRow[](System.String)", "codegen", "System.Data.DataTable", "Select", True, "benchmark" },
    { 372, "System.Data.Common/System.Data.DataTable::Select:System.Data.DataRow[](System.String,System.String)", "codegen", "System.Data.DataTable", "Select", True, "benchmark" },
    { 373, "System.Data.Common/System.Data.DataTable::Select:System.Data.DataRow[](System.String,System.String)", "codegen", "System.Data.DataTable", "Select", True, "benchmark" },
    { 374, "System.Data.Common/System.Data.DataTable::Select:System.Data.DataRow[](System.String,System.String)", "codegen", "System.Data.DataTable", "Select", True, "benchmark" },
    { 375, "System.Data.Common/System.Data.DataTable::Select:System.Data.DataRow[](System.String,System.String,System.Data.DataViewRowState)", "codegen", "System.Data.DataTable", "Select", True, "benchmark" },
    { 376, "System.Data.Common/System.Data.DataTable::Select:System.Data.DataRow[](System.String,System.String,System.Data.DataViewRowState)", "codegen", "System.Data.DataTable", "Select", True, "benchmark" },
    { 377, "System.Data.Common/System.Data.DataTable::Select:System.Data.DataRow[](System.String,System.String,System.Data.DataViewRowState)", "codegen", "System.Data.DataTable", "Select", True, "benchmark" },
    { 378, "System.Data.Common/System.Data.DataTable::BeginLoadData:System.Void()", "codegen", "System.Data.DataTable", "BeginLoadData", True, "benchmark" },
    { 379, "System.Data.Common/System.Data.DataTable::EndLoadData:System.Void()", "codegen", "System.Data.DataTable", "EndLoadData", True, "benchmark" },
    { 380, "System.Data.Common/System.Data.DataTable::LoadDataRow:System.Data.DataRow(System.Object[],System.Boolean)", "codegen", "System.Data.DataTable", "LoadDataRow", True, "benchmark" },
    { 381, "System.Data.Common/System.Data.DataTable::LoadDataRow:System.Data.DataRow(System.Object[],System.Boolean)", "codegen", "System.Data.DataTable", "LoadDataRow", True, "benchmark" },
    { 382, "System.Data.Common/System.Data.DataTable::LoadDataRow:System.Data.DataRow(System.Object[],System.Boolean)", "codegen", "System.Data.DataTable", "LoadDataRow", True, "benchmark" },
    { 383, "System.Data.Common/System.Data.DataTable::LoadDataRow:System.Data.DataRow(System.Object[],System.Boolean)", "codegen", "System.Data.DataTable", "LoadDataRow", True, "benchmark" },
    { 384, "System.Data.Common/System.Data.DataTable::LoadDataRow:System.Data.DataRow(System.Object[],System.Data.LoadOption)", "codegen", "System.Data.DataTable", "LoadDataRow", True, "benchmark" },
    { 385, "System.Data.Common/System.Data.DataTable::LoadDataRow:System.Data.DataRow(System.Object[],System.Data.LoadOption)", "codegen", "System.Data.DataTable", "LoadDataRow", True, "benchmark" },
    { 386, "System.Data.Common/System.Data.DataTable::LoadDataRow:System.Data.DataRow(System.Object[],System.Data.LoadOption)", "codegen", "System.Data.DataTable", "LoadDataRow", True, "benchmark" },
    { 387, "System.Data.Common/System.Data.DataTable::Merge:System.Void(System.Data.DataTable)", "codegen", "System.Data.DataTable", "Merge", True, "benchmark" },
    { 388, "System.Data.Common/System.Data.DataTable::Merge:System.Void(System.Data.DataTable,System.Boolean)", "codegen", "System.Data.DataTable", "Merge", True, "benchmark" },
    { 389, "System.Data.Common/System.Data.DataTable::Merge:System.Void(System.Data.DataTable,System.Boolean)", "codegen", "System.Data.DataTable", "Merge", True, "benchmark" },
    { 390, "System.Data.Common/System.Data.DataTable::Merge:System.Void(System.Data.DataTable,System.Boolean,System.Data.MissingSchemaAction)", "codegen", "System.Data.DataTable", "Merge", True, "benchmark" },
    { 391, "System.Data.Common/System.Data.DataTable::Merge:System.Void(System.Data.DataTable,System.Boolean,System.Data.MissingSchemaAction)", "codegen", "System.Data.DataTable", "Merge", True, "benchmark" },
    { 392, "System.Data.Common/System.Data.DataTable::Load:System.Void(System.Data.IDataReader)", "codegen", "System.Data.DataTable", "Load", True, "benchmark" },
    { 393, "System.Data.Common/System.Data.DataTable::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption)", "codegen", "System.Data.DataTable", "Load", True, "benchmark" },
    { 394, "System.Data.Common/System.Data.DataTable::Load:System.Void(System.Data.IDataReader,System.Data.LoadOption,System.Data.FillErrorEventHandler)", "codegen", "System.Data.DataTable", "Load", True, "benchmark" },
    { 395, "System.Data.Common/System.Data.DataTable::CreateDataReader:System.Data.DataTableReader()", "codegen", "System.Data.DataTable", "CreateDataReader", True, "benchmark" },
    { 396, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.Stream)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 397, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.Stream)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 398, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.Stream,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 399, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.Stream,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 400, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.Stream,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 401, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.TextWriter)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 402, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.TextWriter)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 403, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.TextWriter,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 404, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.TextWriter,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 405, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.TextWriter,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 406, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.Xml.XmlWriter)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 407, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.Xml.XmlWriter,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 408, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.Xml.XmlWriter,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 409, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.String)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 410, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.String)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 411, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.String,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 412, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.String,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 413, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.String,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 414, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.Stream,System.Data.XmlWriteMode)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 415, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.Stream,System.Data.XmlWriteMode)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 416, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.Stream,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 417, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.Stream,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 418, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.Stream,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 419, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.TextWriter,System.Data.XmlWriteMode)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 420, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.TextWriter,System.Data.XmlWriteMode)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 421, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.TextWriter,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 422, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.TextWriter,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 423, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.IO.TextWriter,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 424, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.Xml.XmlWriter,System.Data.XmlWriteMode)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 425, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.Xml.XmlWriter,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 426, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.Xml.XmlWriter,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 427, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.String,System.Data.XmlWriteMode)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 428, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.String,System.Data.XmlWriteMode)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 429, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.String,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 430, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.String,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 431, "System.Data.Common/System.Data.DataTable::WriteXml:System.Void(System.String,System.Data.XmlWriteMode,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXml", True, "benchmark" },
    { 432, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.IO.Stream)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 433, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.IO.Stream)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 434, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.IO.Stream,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 435, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.IO.Stream,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 436, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.IO.Stream,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 437, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.IO.TextWriter)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 438, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.IO.TextWriter)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 439, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.IO.TextWriter,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 440, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.IO.TextWriter,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 441, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.IO.TextWriter,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 442, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.Xml.XmlWriter)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 443, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.Xml.XmlWriter,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 444, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.Xml.XmlWriter,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 445, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.String)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 446, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.String)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 447, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.String,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 448, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.String,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 449, "System.Data.Common/System.Data.DataTable::WriteXmlSchema:System.Void(System.String,System.Boolean)", "codegen", "System.Data.DataTable", "WriteXmlSchema", True, "benchmark" },
    { 450, "System.Data.Common/System.Data.DataTable::ReadXml:System.Data.XmlReadMode(System.IO.Stream)", "codegen", "System.Data.DataTable", "ReadXml", True, "benchmark" },
    { 451, "System.Data.Common/System.Data.DataTable::ReadXml:System.Data.XmlReadMode(System.IO.Stream)", "codegen", "System.Data.DataTable", "ReadXml", True, "benchmark" },
    { 452, "System.Data.Common/System.Data.DataTable::ReadXml:System.Data.XmlReadMode(System.IO.TextReader)", "codegen", "System.Data.DataTable", "ReadXml", True, "benchmark" },
    { 453, "System.Data.Common/System.Data.DataTable::ReadXml:System.Data.XmlReadMode(System.IO.TextReader)", "codegen", "System.Data.DataTable", "ReadXml", True, "benchmark" },
    { 454, "System.Data.Common/System.Data.DataTable::ReadXml:System.Data.XmlReadMode(System.String)", "codegen", "System.Data.DataTable", "ReadXml", True, "benchmark" },
    { 455, "System.Data.Common/System.Data.DataTable::ReadXml:System.Data.XmlReadMode(System.String)", "codegen", "System.Data.DataTable", "ReadXml", True, "benchmark" },
    { 456, "System.Data.Common/System.Data.DataTable::ReadXml:System.Data.XmlReadMode(System.Xml.XmlReader)", "codegen", "System.Data.DataTable", "ReadXml", True, "benchmark" },
    { 457, "System.Data.Common/System.Data.DataTable::ReadXmlSchema:System.Void(System.IO.Stream)", "codegen", "System.Data.DataTable", "ReadXmlSchema", True, "benchmark" },
    { 458, "System.Data.Common/System.Data.DataTable::ReadXmlSchema:System.Void(System.IO.Stream)", "codegen", "System.Data.DataTable", "ReadXmlSchema", True, "benchmark" },
    { 459, "System.Data.Common/System.Data.DataTable::ReadXmlSchema:System.Void(System.IO.TextReader)", "codegen", "System.Data.DataTable", "ReadXmlSchema", True, "benchmark" },
    { 460, "System.Data.Common/System.Data.DataTable::ReadXmlSchema:System.Void(System.IO.TextReader)", "codegen", "System.Data.DataTable", "ReadXmlSchema", True, "benchmark" },
    { 461, "System.Data.Common/System.Data.DataTable::ReadXmlSchema:System.Void(System.String)", "codegen", "System.Data.DataTable", "ReadXmlSchema", True, "benchmark" },
    { 462, "System.Data.Common/System.Data.DataTable::ReadXmlSchema:System.Void(System.String)", "codegen", "System.Data.DataTable", "ReadXmlSchema", True, "benchmark" },
    { 463, "System.Data.Common/System.Data.DataTable::ReadXmlSchema:System.Void(System.Xml.XmlReader)", "codegen", "System.Data.DataTable", "ReadXmlSchema", True, "benchmark" },
    { 464, "System.Data.Common/System.Data.DataTable::GetDataTableSchema:System.Xml.Schema.XmlSchemaComplexType(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.DataTable", "GetDataTableSchema", True, "benchmark" },
    { 465, "System.Data.Common/System.Data.DataTableCollection::Add:System.Void(System.Data.DataTable)", "codegen", "System.Data.DataTableCollection", "Add", True, "benchmark" },
    { 466, "System.Data.Common/System.Data.DataTableCollection::AddRange:System.Void(System.Data.DataTable[])", "codegen", "System.Data.DataTableCollection", "AddRange", True, "benchmark" },
    { 467, "System.Data.Common/System.Data.DataTableCollection::AddRange:System.Void(System.Data.DataTable[])", "codegen", "System.Data.DataTableCollection", "AddRange", True, "benchmark" },
    { 468, "System.Data.Common/System.Data.DataTableCollection::AddRange:System.Void(System.Data.DataTable[])", "codegen", "System.Data.DataTableCollection", "AddRange", True, "benchmark" },
    { 469, "System.Data.Common/System.Data.DataTableCollection::Add:System.Data.DataTable(System.String)", "codegen", "System.Data.DataTableCollection", "Add", True, "benchmark" },
    { 470, "System.Data.Common/System.Data.DataTableCollection::Add:System.Data.DataTable(System.String)", "codegen", "System.Data.DataTableCollection", "Add", True, "benchmark" },
    { 471, "System.Data.Common/System.Data.DataTableCollection::Add:System.Data.DataTable(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "Add", True, "benchmark" },
    { 472, "System.Data.Common/System.Data.DataTableCollection::Add:System.Data.DataTable(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "Add", True, "benchmark" },
    { 473, "System.Data.Common/System.Data.DataTableCollection::Add:System.Data.DataTable(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "Add", True, "benchmark" },
    { 474, "System.Data.Common/System.Data.DataTableCollection::Add:System.Data.DataTable()", "codegen", "System.Data.DataTableCollection", "Add", True, "benchmark" },
    { 475, "System.Data.Common/System.Data.DataTableCollection::CanRemove:System.Boolean(System.Data.DataTable)", "codegen", "System.Data.DataTableCollection", "CanRemove", True, "benchmark" },
    { 476, "System.Data.Common/System.Data.DataTableCollection::Clear:System.Void()", "codegen", "System.Data.DataTableCollection", "Clear", True, "benchmark" },
    { 477, "System.Data.Common/System.Data.DataTableCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.DataTableCollection", "Contains", True, "benchmark" },
    { 478, "System.Data.Common/System.Data.DataTableCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.DataTableCollection", "Contains", True, "benchmark" },
    { 479, "System.Data.Common/System.Data.DataTableCollection::Contains:System.Boolean(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "Contains", True, "benchmark" },
    { 480, "System.Data.Common/System.Data.DataTableCollection::Contains:System.Boolean(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "Contains", True, "benchmark" },
    { 481, "System.Data.Common/System.Data.DataTableCollection::Contains:System.Boolean(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "Contains", True, "benchmark" },
    { 482, "System.Data.Common/System.Data.DataTableCollection::IndexOf:System.Int32(System.Data.DataTable)", "codegen", "System.Data.DataTableCollection", "IndexOf", True, "benchmark" },
    { 483, "System.Data.Common/System.Data.DataTableCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.DataTableCollection", "IndexOf", True, "benchmark" },
    { 484, "System.Data.Common/System.Data.DataTableCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.DataTableCollection", "IndexOf", True, "benchmark" },
    { 485, "System.Data.Common/System.Data.DataTableCollection::IndexOf:System.Int32(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "IndexOf", True, "benchmark" },
    { 486, "System.Data.Common/System.Data.DataTableCollection::IndexOf:System.Int32(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "IndexOf", True, "benchmark" },
    { 487, "System.Data.Common/System.Data.DataTableCollection::IndexOf:System.Int32(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "IndexOf", True, "benchmark" },
    { 488, "System.Data.Common/System.Data.DataTableCollection::Remove:System.Void(System.Data.DataTable)", "codegen", "System.Data.DataTableCollection", "Remove", True, "benchmark" },
    { 489, "System.Data.Common/System.Data.DataTableCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.DataTableCollection", "RemoveAt", True, "benchmark" },
    { 490, "System.Data.Common/System.Data.DataTableCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.DataTableCollection", "RemoveAt", True, "benchmark" },
    { 491, "System.Data.Common/System.Data.DataTableCollection::Remove:System.Void(System.String)", "codegen", "System.Data.DataTableCollection", "Remove", True, "benchmark" },
    { 492, "System.Data.Common/System.Data.DataTableCollection::Remove:System.Void(System.String)", "codegen", "System.Data.DataTableCollection", "Remove", True, "benchmark" },
    { 493, "System.Data.Common/System.Data.DataTableCollection::Remove:System.Void(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "Remove", True, "benchmark" },
    { 494, "System.Data.Common/System.Data.DataTableCollection::Remove:System.Void(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "Remove", True, "benchmark" },
    { 495, "System.Data.Common/System.Data.DataTableCollection::Remove:System.Void(System.String,System.String)", "codegen", "System.Data.DataTableCollection", "Remove", True, "benchmark" },
    { 496, "System.Data.Common/System.Data.DataTableExtensions::AsEnumerable:System.Data.EnumerableRowCollection<System.Data.DataRow>(System.Data.DataTable)", "codegen", "System.Data.DataTableExtensions", "AsEnumerable", True, "benchmark" },
    { 497, "System.Data.Common/System.Data.DataTableExtensions::CopyToDataTable:System.Data.DataTable(System.Collections.Generic.IEnumerable<System.Data.DataRow>)", "codegen", "System.Data.DataTableExtensions", "CopyToDataTable", True, "benchmark" },
    { 498, "System.Data.Common/System.Data.DataTableExtensions::CopyToDataTable:System.Data.DataTable(System.Collections.Generic.IEnumerable<System.Data.DataRow>)", "codegen", "System.Data.DataTableExtensions", "CopyToDataTable", True, "benchmark" },
    { 499, "System.Data.Common/System.Data.DataTableExtensions::CopyToDataTable:System.Void(System.Collections.Generic.IEnumerable<System.Data.DataRow>,System.Data.DataTable,System.Data.LoadOption)", "codegen", "System.Data.DataTableExtensions", "CopyToDataTable", True, "benchmark" },
    { 500, "System.Data.Common/System.Data.DataTableExtensions::CopyToDataTable:System.Void(System.Collections.Generic.IEnumerable<System.Data.DataRow>,System.Data.DataTable,System.Data.LoadOption)", "codegen", "System.Data.DataTableExtensions", "CopyToDataTable", True, "benchmark" },
    { 501, "System.Data.Common/System.Data.DataTableExtensions::CopyToDataTable:System.Void(System.Collections.Generic.IEnumerable<System.Data.DataRow>,System.Data.DataTable,System.Data.LoadOption,System.Data.FillErrorEventHandler)", "codegen", "System.Data.DataTableExtensions", "CopyToDataTable", True, "benchmark" },
    { 502, "System.Data.Common/System.Data.DataTableExtensions::CopyToDataTable:System.Void(System.Collections.Generic.IEnumerable<System.Data.DataRow>,System.Data.DataTable,System.Data.LoadOption,System.Data.FillErrorEventHandler)", "codegen", "System.Data.DataTableExtensions", "CopyToDataTable", True, "benchmark" },
    { 503, "System.Data.Common/System.Data.DataTableExtensions::AsDataView:System.Data.DataView(System.Data.DataTable)", "codegen", "System.Data.DataTableExtensions", "AsDataView", True, "benchmark" },
    { 504, "System.Data.Common/System.Data.DataTableExtensions::AsDataView:System.Data.DataView(System.Data.EnumerableRowCollection<System.Data.DataRow>)", "codegen", "System.Data.DataTableExtensions", "AsDataView", True, "benchmark" },
    { 505, "System.Data.Common/System.Data.DataTableReader::Close:System.Void()", "codegen", "System.Data.DataTableReader", "Close", True, "benchmark" },
    { 506, "System.Data.Common/System.Data.DataTableReader::GetSchemaTable:System.Data.DataTable()", "codegen", "System.Data.DataTableReader", "GetSchemaTable", True, "benchmark" },
    { 507, "System.Data.Common/System.Data.DataTableReader::NextResult:System.Boolean()", "codegen", "System.Data.DataTableReader", "NextResult", True, "benchmark" },
    { 508, "System.Data.Common/System.Data.DataTableReader::Read:System.Boolean()", "codegen", "System.Data.DataTableReader", "Read", True, "benchmark" },
    { 509, "System.Data.Common/System.Data.DataTableReader::GetProviderSpecificFieldType:System.Type(System.Int32)", "codegen", "System.Data.DataTableReader", "GetProviderSpecificFieldType", True, "benchmark" },
    { 510, "System.Data.Common/System.Data.DataTableReader::GetProviderSpecificFieldType:System.Type(System.Int32)", "codegen", "System.Data.DataTableReader", "GetProviderSpecificFieldType", True, "benchmark" },
    { 511, "System.Data.Common/System.Data.DataTableReader::GetProviderSpecificValue:System.Object(System.Int32)", "codegen", "System.Data.DataTableReader", "GetProviderSpecificValue", True, "benchmark" },
    { 512, "System.Data.Common/System.Data.DataTableReader::GetProviderSpecificValue:System.Object(System.Int32)", "codegen", "System.Data.DataTableReader", "GetProviderSpecificValue", True, "benchmark" },
    { 513, "System.Data.Common/System.Data.DataTableReader::GetProviderSpecificValues:System.Int32(System.Object[])", "codegen", "System.Data.DataTableReader", "GetProviderSpecificValues", True, "benchmark" },
    { 514, "System.Data.Common/System.Data.DataTableReader::GetProviderSpecificValues:System.Int32(System.Object[])", "codegen", "System.Data.DataTableReader", "GetProviderSpecificValues", True, "benchmark" },
    { 515, "System.Data.Common/System.Data.DataTableReader::GetProviderSpecificValues:System.Int32(System.Object[])", "codegen", "System.Data.DataTableReader", "GetProviderSpecificValues", True, "benchmark" },
    { 516, "System.Data.Common/System.Data.DataTableReader::GetBoolean:System.Boolean(System.Int32)", "codegen", "System.Data.DataTableReader", "GetBoolean", True, "benchmark" },
    { 517, "System.Data.Common/System.Data.DataTableReader::GetBoolean:System.Boolean(System.Int32)", "codegen", "System.Data.DataTableReader", "GetBoolean", True, "benchmark" },
    { 518, "System.Data.Common/System.Data.DataTableReader::GetByte:System.Byte(System.Int32)", "codegen", "System.Data.DataTableReader", "GetByte", True, "benchmark" },
    { 519, "System.Data.Common/System.Data.DataTableReader::GetByte:System.Byte(System.Int32)", "codegen", "System.Data.DataTableReader", "GetByte", True, "benchmark" },
    { 520, "System.Data.Common/System.Data.DataTableReader::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.DataTableReader", "GetBytes", True, "benchmark" },
    { 521, "System.Data.Common/System.Data.DataTableReader::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.DataTableReader", "GetBytes", True, "benchmark" },
    { 522, "System.Data.Common/System.Data.DataTableReader::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.DataTableReader", "GetBytes", True, "benchmark" },
    { 523, "System.Data.Common/System.Data.DataTableReader::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.DataTableReader", "GetBytes", True, "benchmark" },
    { 524, "System.Data.Common/System.Data.DataTableReader::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.DataTableReader", "GetBytes", True, "benchmark" },
    { 525, "System.Data.Common/System.Data.DataTableReader::GetChar:System.Char(System.Int32)", "codegen", "System.Data.DataTableReader", "GetChar", True, "benchmark" },
    { 526, "System.Data.Common/System.Data.DataTableReader::GetChar:System.Char(System.Int32)", "codegen", "System.Data.DataTableReader", "GetChar", True, "benchmark" },
    { 527, "System.Data.Common/System.Data.DataTableReader::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.DataTableReader", "GetChars", True, "benchmark" },
    { 528, "System.Data.Common/System.Data.DataTableReader::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.DataTableReader", "GetChars", True, "benchmark" },
    { 529, "System.Data.Common/System.Data.DataTableReader::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.DataTableReader", "GetChars", True, "benchmark" },
    { 530, "System.Data.Common/System.Data.DataTableReader::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.DataTableReader", "GetChars", True, "benchmark" },
    { 531, "System.Data.Common/System.Data.DataTableReader::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.DataTableReader", "GetChars", True, "benchmark" },
    { 532, "System.Data.Common/System.Data.DataTableReader::GetDataTypeName:System.String(System.Int32)", "codegen", "System.Data.DataTableReader", "GetDataTypeName", True, "benchmark" },
    { 533, "System.Data.Common/System.Data.DataTableReader::GetDataTypeName:System.String(System.Int32)", "codegen", "System.Data.DataTableReader", "GetDataTypeName", True, "benchmark" },
    { 534, "System.Data.Common/System.Data.DataTableReader::GetDateTime:System.DateTime(System.Int32)", "codegen", "System.Data.DataTableReader", "GetDateTime", True, "benchmark" },
    { 535, "System.Data.Common/System.Data.DataTableReader::GetDateTime:System.DateTime(System.Int32)", "codegen", "System.Data.DataTableReader", "GetDateTime", True, "benchmark" },
    { 536, "System.Data.Common/System.Data.DataTableReader::GetDecimal:System.Decimal(System.Int32)", "codegen", "System.Data.DataTableReader", "GetDecimal", True, "benchmark" },
    { 537, "System.Data.Common/System.Data.DataTableReader::GetDecimal:System.Decimal(System.Int32)", "codegen", "System.Data.DataTableReader", "GetDecimal", True, "benchmark" },
    { 538, "System.Data.Common/System.Data.DataTableReader::GetDouble:System.Double(System.Int32)", "codegen", "System.Data.DataTableReader", "GetDouble", True, "benchmark" },
    { 539, "System.Data.Common/System.Data.DataTableReader::GetDouble:System.Double(System.Int32)", "codegen", "System.Data.DataTableReader", "GetDouble", True, "benchmark" },
    { 540, "System.Data.Common/System.Data.DataTableReader::GetFieldType:System.Type(System.Int32)", "codegen", "System.Data.DataTableReader", "GetFieldType", True, "benchmark" },
    { 541, "System.Data.Common/System.Data.DataTableReader::GetFieldType:System.Type(System.Int32)", "codegen", "System.Data.DataTableReader", "GetFieldType", True, "benchmark" },
    { 542, "System.Data.Common/System.Data.DataTableReader::GetFloat:System.Single(System.Int32)", "codegen", "System.Data.DataTableReader", "GetFloat", True, "benchmark" },
    { 543, "System.Data.Common/System.Data.DataTableReader::GetFloat:System.Single(System.Int32)", "codegen", "System.Data.DataTableReader", "GetFloat", True, "benchmark" },
    { 544, "System.Data.Common/System.Data.DataTableReader::GetGuid:System.Guid(System.Int32)", "codegen", "System.Data.DataTableReader", "GetGuid", True, "benchmark" },
    { 545, "System.Data.Common/System.Data.DataTableReader::GetGuid:System.Guid(System.Int32)", "codegen", "System.Data.DataTableReader", "GetGuid", True, "benchmark" },
    { 546, "System.Data.Common/System.Data.DataTableReader::GetInt16:System.Int16(System.Int32)", "codegen", "System.Data.DataTableReader", "GetInt16", True, "benchmark" },
    { 547, "System.Data.Common/System.Data.DataTableReader::GetInt16:System.Int16(System.Int32)", "codegen", "System.Data.DataTableReader", "GetInt16", True, "benchmark" },
    { 548, "System.Data.Common/System.Data.DataTableReader::GetInt32:System.Int32(System.Int32)", "codegen", "System.Data.DataTableReader", "GetInt32", True, "benchmark" },
    { 549, "System.Data.Common/System.Data.DataTableReader::GetInt32:System.Int32(System.Int32)", "codegen", "System.Data.DataTableReader", "GetInt32", True, "benchmark" },
    { 550, "System.Data.Common/System.Data.DataTableReader::GetInt64:System.Int64(System.Int32)", "codegen", "System.Data.DataTableReader", "GetInt64", True, "benchmark" },
    { 551, "System.Data.Common/System.Data.DataTableReader::GetInt64:System.Int64(System.Int32)", "codegen", "System.Data.DataTableReader", "GetInt64", True, "benchmark" },
    { 552, "System.Data.Common/System.Data.DataTableReader::GetName:System.String(System.Int32)", "codegen", "System.Data.DataTableReader", "GetName", True, "benchmark" },
    { 553, "System.Data.Common/System.Data.DataTableReader::GetName:System.String(System.Int32)", "codegen", "System.Data.DataTableReader", "GetName", True, "benchmark" },
    { 554, "System.Data.Common/System.Data.DataTableReader::GetOrdinal:System.Int32(System.String)", "codegen", "System.Data.DataTableReader", "GetOrdinal", True, "benchmark" },
    { 555, "System.Data.Common/System.Data.DataTableReader::GetOrdinal:System.Int32(System.String)", "codegen", "System.Data.DataTableReader", "GetOrdinal", True, "benchmark" },
    { 556, "System.Data.Common/System.Data.DataTableReader::GetString:System.String(System.Int32)", "codegen", "System.Data.DataTableReader", "GetString", True, "benchmark" },
    { 557, "System.Data.Common/System.Data.DataTableReader::GetString:System.String(System.Int32)", "codegen", "System.Data.DataTableReader", "GetString", True, "benchmark" },
    { 558, "System.Data.Common/System.Data.DataTableReader::GetValue:System.Object(System.Int32)", "codegen", "System.Data.DataTableReader", "GetValue", True, "benchmark" },
    { 559, "System.Data.Common/System.Data.DataTableReader::GetValue:System.Object(System.Int32)", "codegen", "System.Data.DataTableReader", "GetValue", True, "benchmark" },
    { 560, "System.Data.Common/System.Data.DataTableReader::GetValues:System.Int32(System.Object[])", "codegen", "System.Data.DataTableReader", "GetValues", True, "benchmark" },
    { 561, "System.Data.Common/System.Data.DataTableReader::GetValues:System.Int32(System.Object[])", "codegen", "System.Data.DataTableReader", "GetValues", True, "benchmark" },
    { 562, "System.Data.Common/System.Data.DataTableReader::GetValues:System.Int32(System.Object[])", "codegen", "System.Data.DataTableReader", "GetValues", True, "benchmark" },
    { 563, "System.Data.Common/System.Data.DataTableReader::IsDBNull:System.Boolean(System.Int32)", "codegen", "System.Data.DataTableReader", "IsDBNull", True, "benchmark" },
    { 564, "System.Data.Common/System.Data.DataTableReader::IsDBNull:System.Boolean(System.Int32)", "codegen", "System.Data.DataTableReader", "IsDBNull", True, "benchmark" },
    { 565, "System.Data.Common/System.Data.DataTableReader::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Data.DataTableReader", "GetEnumerator", True, "benchmark" },
    { 566, "System.Data.Common/System.Data.DataView::AddNew:System.Data.DataRowView()", "codegen", "System.Data.DataView", "AddNew", True, "benchmark" },
    { 567, "System.Data.Common/System.Data.DataView::BeginInit:System.Void()", "codegen", "System.Data.DataView", "BeginInit", True, "benchmark" },
    { 568, "System.Data.Common/System.Data.DataView::EndInit:System.Void()", "codegen", "System.Data.DataView", "EndInit", True, "benchmark" },
    { 569, "System.Data.Common/System.Data.DataView::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.DataView", "CopyTo", True, "benchmark" },
    { 570, "System.Data.Common/System.Data.DataView::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.DataView", "CopyTo", True, "benchmark" },
    { 571, "System.Data.Common/System.Data.DataView::Delete:System.Void(System.Int32)", "codegen", "System.Data.DataView", "Delete", True, "benchmark" },
    { 572, "System.Data.Common/System.Data.DataView::Delete:System.Void(System.Int32)", "codegen", "System.Data.DataView", "Delete", True, "benchmark" },
    { 573, "System.Data.Common/System.Data.DataView::Find:System.Int32(System.Object)", "codegen", "System.Data.DataView", "Find", True, "benchmark" },
    { 574, "System.Data.Common/System.Data.DataView::Find:System.Int32(System.Object[])", "codegen", "System.Data.DataView", "Find", True, "benchmark" },
    { 575, "System.Data.Common/System.Data.DataView::Find:System.Int32(System.Object[])", "codegen", "System.Data.DataView", "Find", True, "benchmark" },
    { 576, "System.Data.Common/System.Data.DataView::Find:System.Int32(System.Object[])", "codegen", "System.Data.DataView", "Find", True, "benchmark" },
    { 577, "System.Data.Common/System.Data.DataView::FindRows:System.Data.DataRowView[](System.Object)", "codegen", "System.Data.DataView", "FindRows", True, "benchmark" },
    { 578, "System.Data.Common/System.Data.DataView::FindRows:System.Data.DataRowView[](System.Object[])", "codegen", "System.Data.DataView", "FindRows", True, "benchmark" },
    { 579, "System.Data.Common/System.Data.DataView::FindRows:System.Data.DataRowView[](System.Object[])", "codegen", "System.Data.DataView", "FindRows", True, "benchmark" },
    { 580, "System.Data.Common/System.Data.DataView::FindRows:System.Data.DataRowView[](System.Object[])", "codegen", "System.Data.DataView", "FindRows", True, "benchmark" },
    { 581, "System.Data.Common/System.Data.DataView::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Data.DataView", "GetEnumerator", True, "benchmark" },
    { 582, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable()", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 583, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.String)", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 584, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.String)", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 585, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.Boolean,System.String[])", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 586, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.Boolean,System.String[])", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 587, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.Boolean,System.String[])", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 588, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.Boolean,System.String[])", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 589, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.String,System.Boolean,System.String[])", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 590, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.String,System.Boolean,System.String[])", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 591, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.String,System.Boolean,System.String[])", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 592, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.String,System.Boolean,System.String[])", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 593, "System.Data.Common/System.Data.DataView::ToTable:System.Data.DataTable(System.String,System.Boolean,System.String[])", "codegen", "System.Data.DataView", "ToTable", True, "benchmark" },
    { 594, "System.Data.Common/System.Data.DataViewManager::CreateDataView:System.Data.DataView(System.Data.DataTable)", "codegen", "System.Data.DataViewManager", "CreateDataView", True, "benchmark" },
    { 595, "System.Data.Common/System.Data.DataViewSettingCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.DataViewSettingCollection", "CopyTo", True, "benchmark" },
    { 596, "System.Data.Common/System.Data.DataViewSettingCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.DataViewSettingCollection", "CopyTo", True, "benchmark" },
    { 597, "System.Data.Common/System.Data.DataViewSettingCollection::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Data.DataViewSettingCollection", "GetEnumerator", True, "benchmark" },
    { 598, "System.Data.Common/System.Data.DBConcurrencyException::CopyToRows:System.Void(System.Data.DataRow[])", "codegen", "System.Data.DBConcurrencyException", "CopyToRows", True, "benchmark" },
    { 599, "System.Data.Common/System.Data.DBConcurrencyException::CopyToRows:System.Void(System.Data.DataRow[])", "codegen", "System.Data.DBConcurrencyException", "CopyToRows", True, "benchmark" },
    { 600, "System.Data.Common/System.Data.DBConcurrencyException::CopyToRows:System.Void(System.Data.DataRow[])", "codegen", "System.Data.DBConcurrencyException", "CopyToRows", True, "benchmark" },
    { 601, "System.Data.Common/System.Data.DBConcurrencyException::CopyToRows:System.Void(System.Data.DataRow[],System.Int32)", "codegen", "System.Data.DBConcurrencyException", "CopyToRows", True, "benchmark" },
    { 602, "System.Data.Common/System.Data.DBConcurrencyException::CopyToRows:System.Void(System.Data.DataRow[],System.Int32)", "codegen", "System.Data.DBConcurrencyException", "CopyToRows", True, "benchmark" },
    { 603, "System.Data.Common/System.Data.DBConcurrencyException::CopyToRows:System.Void(System.Data.DataRow[],System.Int32)", "codegen", "System.Data.DBConcurrencyException", "CopyToRows", True, "benchmark" },
    { 604, "System.Data.Common/System.Data.DBConcurrencyException::CopyToRows:System.Void(System.Data.DataRow[],System.Int32)", "codegen", "System.Data.DBConcurrencyException", "CopyToRows", True, "benchmark" },
    { 605, "System.Data.Common/System.Data.EnumerableRowCollection<System.Int32>::GetEnumerator:System.Collections.Generic.IEnumerator<System.Int32>()", "codegen", "System.Data.EnumerableRowCollection<System.Int32>", "GetEnumerator", True, "benchmark" },
    { 606, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::Where:System.Data.EnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "Where", True, "benchmark" },
    { 607, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::Where:System.Data.EnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Boolean>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "Where", True, "benchmark" },
    { 608, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::OrderBy:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "OrderBy", True, "benchmark" },
    { 609, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::OrderBy:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "OrderBy", True, "benchmark" },
    { 610, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::OrderBy:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "OrderBy", True, "benchmark" },
    { 611, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::OrderBy:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "OrderBy", True, "benchmark" },
    { 612, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::OrderByDescending:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "OrderByDescending", True, "benchmark" },
    { 613, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::OrderByDescending:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "OrderByDescending", True, "benchmark" },
    { 614, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::OrderByDescending:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "OrderByDescending", True, "benchmark" },
    { 615, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::OrderByDescending:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "OrderByDescending", True, "benchmark" },
    { 616, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::ThenBy:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.OrderedEnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "ThenBy", True, "benchmark" },
    { 617, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::ThenBy:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.OrderedEnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "ThenBy", True, "benchmark" },
    { 618, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::ThenBy:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.OrderedEnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "ThenBy", True, "benchmark" },
    { 619, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::ThenBy:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.OrderedEnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "ThenBy", True, "benchmark" },
    { 620, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::ThenByDescending:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.OrderedEnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "ThenByDescending", True, "benchmark" },
    { 621, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::ThenByDescending:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.OrderedEnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "ThenByDescending", True, "benchmark" },
    { 622, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::ThenByDescending:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.OrderedEnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "ThenByDescending", True, "benchmark" },
    { 623, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::ThenByDescending:System.Data.OrderedEnumerableRowCollection<System.Int32>(System.Data.OrderedEnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "ThenByDescending", True, "benchmark" },
    { 624, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::Select:System.Data.EnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "Select", True, "benchmark" },
    { 625, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::Select:System.Data.EnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "Select", True, "benchmark" },
    { 626, "System.Data.Common/System.Data.EnumerableRowCollectionExtensions::Cast:System.Data.EnumerableRowCollection<System.Int32>(System.Data.EnumerableRowCollection)", "codegen", "System.Data.EnumerableRowCollectionExtensions", "Cast", True, "benchmark" },
    { 627, "System.Data.Common/System.Data.InternalDataCollectionBase::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.InternalDataCollectionBase", "CopyTo", True, "benchmark" },
    { 628, "System.Data.Common/System.Data.InternalDataCollectionBase::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.InternalDataCollectionBase", "CopyTo", True, "benchmark" },
    { 629, "System.Data.Common/System.Data.InternalDataCollectionBase::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Data.InternalDataCollectionBase", "GetEnumerator", True, "benchmark" },
    { 630, "System.Data.Common/System.Data.PropertyCollection::Clone:System.Object()", "codegen", "System.Data.PropertyCollection", "Clone", True, "benchmark" },
    { 631, "System.Data.Common/System.Data.TypedTableBase<System.Data.DataRow>::GetEnumerator:System.Collections.Generic.IEnumerator<System.Data.DataRow>()", "codegen", "System.Data.TypedTableBase<System.Data.DataRow>", "GetEnumerator", True, "benchmark" },
    { 632, "System.Data.Common/System.Data.TypedTableBase<System.Data.DataRow>::Cast:System.Data.EnumerableRowCollection<System.Int32>()", "codegen", "System.Data.TypedTableBase<System.Data.DataRow>", "Cast", True, "benchmark" },
    { 633, "System.Data.Common/System.Data.TypedTableBaseExtensions::Where:System.Data.EnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Boolean>)", "codegen", "System.Data.TypedTableBaseExtensions", "Where", True, "benchmark" },
    { 634, "System.Data.Common/System.Data.TypedTableBaseExtensions::Where:System.Data.EnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Boolean>)", "codegen", "System.Data.TypedTableBaseExtensions", "Where", True, "benchmark" },
    { 635, "System.Data.Common/System.Data.TypedTableBaseExtensions::OrderBy:System.Data.OrderedEnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Int32>)", "codegen", "System.Data.TypedTableBaseExtensions", "OrderBy", True, "benchmark" },
    { 636, "System.Data.Common/System.Data.TypedTableBaseExtensions::OrderBy:System.Data.OrderedEnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Int32>)", "codegen", "System.Data.TypedTableBaseExtensions", "OrderBy", True, "benchmark" },
    { 637, "System.Data.Common/System.Data.TypedTableBaseExtensions::OrderBy:System.Data.OrderedEnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.TypedTableBaseExtensions", "OrderBy", True, "benchmark" },
    { 638, "System.Data.Common/System.Data.TypedTableBaseExtensions::OrderBy:System.Data.OrderedEnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.TypedTableBaseExtensions", "OrderBy", True, "benchmark" },
    { 639, "System.Data.Common/System.Data.TypedTableBaseExtensions::OrderByDescending:System.Data.OrderedEnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Int32>)", "codegen", "System.Data.TypedTableBaseExtensions", "OrderByDescending", True, "benchmark" },
    { 640, "System.Data.Common/System.Data.TypedTableBaseExtensions::OrderByDescending:System.Data.OrderedEnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Int32>)", "codegen", "System.Data.TypedTableBaseExtensions", "OrderByDescending", True, "benchmark" },
    { 641, "System.Data.Common/System.Data.TypedTableBaseExtensions::OrderByDescending:System.Data.OrderedEnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.TypedTableBaseExtensions", "OrderByDescending", True, "benchmark" },
    { 642, "System.Data.Common/System.Data.TypedTableBaseExtensions::OrderByDescending:System.Data.OrderedEnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Int32>,System.Collections.Generic.IComparer<System.Int32>)", "codegen", "System.Data.TypedTableBaseExtensions", "OrderByDescending", True, "benchmark" },
    { 643, "System.Data.Common/System.Data.TypedTableBaseExtensions::Select:System.Data.EnumerableRowCollection<System.Int32>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Int32>)", "codegen", "System.Data.TypedTableBaseExtensions", "Select", True, "benchmark" },
    { 644, "System.Data.Common/System.Data.TypedTableBaseExtensions::Select:System.Data.EnumerableRowCollection<System.Int32>(System.Data.TypedTableBase<System.Data.DataRow>,System.Func<System.Data.DataRow,System.Int32>)", "codegen", "System.Data.TypedTableBaseExtensions", "Select", True, "benchmark" },
    { 645, "System.Data.Common/System.Data.TypedTableBaseExtensions::AsEnumerable:System.Data.EnumerableRowCollection<System.Data.DataRow>(System.Data.TypedTableBase<System.Data.DataRow>)", "codegen", "System.Data.TypedTableBaseExtensions", "AsEnumerable", True, "benchmark" },
    { 646, "System.Data.Common/System.Data.TypedTableBaseExtensions::ElementAtOrDefault:System.Data.DataRow(System.Data.TypedTableBase<System.Data.DataRow>,System.Int32)", "codegen", "System.Data.TypedTableBaseExtensions", "ElementAtOrDefault", True, "benchmark" },
    { 647, "System.Data.Common/System.Data.TypedTableBaseExtensions::ElementAtOrDefault:System.Data.DataRow(System.Data.TypedTableBase<System.Data.DataRow>,System.Int32)", "codegen", "System.Data.TypedTableBaseExtensions", "ElementAtOrDefault", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 648;

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
        int64_t result = 0;
        bool caught = false;
{% if is_jit %}
        try {
            result = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) {
            caught = true;
        }
{% else %}
        CHAOS_EH_TRY
            result = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
            caught = true;
        CHAOS_EH_END
{% end %}
        if (!first) printf(",");
        printf("{\"si\":%d,\"methodIndex\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
               si, i, caught ? "false" : "true", caught ? -1 : result);
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

// ── --profile: per-method GC/allocation/code-size profile ───────────
static int RunProfileMode() {
    const int kCount = kSubjectEntryCount;
    chaos::il2cpp::runtime_core::ProfileStoreInit(kCount);
    for (int si = 0; si < kCount; si++) {
        int i = kSubjectSlotMap[si];
        int64_t heap_before = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
        chaos::il2cpp::runtime_core::GetThreadProfileData().heap_before = heap_before;

{% if is_jit %}
        try {
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        } catch(...) { }
{% else %}
        CHAOS_EH_TRY
            chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
        CHAOS_EH_CATCH_BEGIN
        CHAOS_EH_END
{% endif %}

        int64_t heap_after = chaos::il2cpp::runtime_core::chaos_gc_get_heap_size();
        chaos::il2cpp::runtime_core::GetThreadProfileData().heap_after = heap_after;
        chaos::il2cpp::runtime_core::FlushThreadProfileData(i);
    }
    chaos::il2cpp::runtime_core::ProfileStoreFinalize();
    chaos::il2cpp::runtime_core::ProfileEmitJson();
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

    if (std::strcmp(argv[1], "--profile") == 0) { ret = RunProfileMode(); goto shutdown; }

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