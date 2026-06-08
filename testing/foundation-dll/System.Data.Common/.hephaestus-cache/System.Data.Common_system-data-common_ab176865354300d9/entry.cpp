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
    { 0, "System.Data.Common/System.Data.Common.DataAdapter::ShouldSerializeAcceptChangesDuringFill:System.Boolean()", "codegen", "System.Data.Common.DataAdapter", "ShouldSerializeAcceptChangesDuringFill", True, "benchmark" },
    { 1, "System.Data.Common/System.Data.Common.DataAdapter::ResetFillLoadOption:System.Void()", "codegen", "System.Data.Common.DataAdapter", "ResetFillLoadOption", True, "benchmark" },
    { 2, "System.Data.Common/System.Data.Common.DataAdapter::ShouldSerializeFillLoadOption:System.Boolean()", "codegen", "System.Data.Common.DataAdapter", "ShouldSerializeFillLoadOption", True, "benchmark" },
    { 3, "System.Data.Common/System.Data.Common.DataAdapter::FillSchema:System.Data.DataTable[](System.Data.DataSet,System.Data.SchemaType)", "codegen", "System.Data.Common.DataAdapter", "FillSchema", True, "benchmark" },
    { 4, "System.Data.Common/System.Data.Common.DataAdapter::Fill:System.Int32(System.Data.DataSet)", "codegen", "System.Data.Common.DataAdapter", "Fill", True, "benchmark" },
    { 5, "System.Data.Common/System.Data.Common.DataAdapter::GetFillParameters:System.Data.IDataParameter[]()", "codegen", "System.Data.Common.DataAdapter", "GetFillParameters", True, "benchmark" },
    { 6, "System.Data.Common/System.Data.Common.DataAdapter::Update:System.Int32(System.Data.DataSet)", "codegen", "System.Data.Common.DataAdapter", "Update", True, "benchmark" },
    { 7, "System.Data.Common/System.Data.Common.DataColumnMapping::GetDataColumnBySchemaAction:System.Data.DataColumn(System.Data.DataTable,System.Type,System.Data.MissingSchemaAction)", "codegen", "System.Data.Common.DataColumnMapping", "GetDataColumnBySchemaAction", True, "benchmark" },
    { 8, "System.Data.Common/System.Data.Common.DataColumnMapping::GetDataColumnBySchemaAction:System.Data.DataColumn(System.String,System.String,System.Data.DataTable,System.Type,System.Data.MissingSchemaAction)", "codegen", "System.Data.Common.DataColumnMapping", "GetDataColumnBySchemaAction", True, "benchmark" },
    { 9, "System.Data.Common/System.Data.Common.DataColumnMapping::GetDataColumnBySchemaAction:System.Data.DataColumn(System.String,System.String,System.Data.DataTable,System.Type,System.Data.MissingSchemaAction)", "codegen", "System.Data.Common.DataColumnMapping", "GetDataColumnBySchemaAction", True, "benchmark" },
    { 10, "System.Data.Common/System.Data.Common.DataColumnMapping::GetDataColumnBySchemaAction:System.Data.DataColumn(System.String,System.String,System.Data.DataTable,System.Type,System.Data.MissingSchemaAction)", "codegen", "System.Data.Common.DataColumnMapping", "GetDataColumnBySchemaAction", True, "benchmark" },
    { 11, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Add:System.Int32(System.Object)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Add", True, "benchmark" },
    { 12, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Add:System.Data.Common.DataColumnMapping(System.String,System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Add", True, "benchmark" },
    { 13, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Add:System.Data.Common.DataColumnMapping(System.String,System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Add", True, "benchmark" },
    { 14, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Add:System.Data.Common.DataColumnMapping(System.String,System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Add", True, "benchmark" },
    { 15, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::AddRange:System.Void(System.Data.Common.DataColumnMapping[])", "codegen", "System.Data.Common.DataColumnMappingCollection", "AddRange", True, "benchmark" },
    { 16, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::AddRange:System.Void(System.Data.Common.DataColumnMapping[])", "codegen", "System.Data.Common.DataColumnMappingCollection", "AddRange", True, "benchmark" },
    { 17, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::AddRange:System.Void(System.Data.Common.DataColumnMapping[])", "codegen", "System.Data.Common.DataColumnMappingCollection", "AddRange", True, "benchmark" },
    { 18, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::AddRange:System.Void(System.Array)", "codegen", "System.Data.Common.DataColumnMappingCollection", "AddRange", True, "benchmark" },
    { 19, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Clear:System.Void()", "codegen", "System.Data.Common.DataColumnMappingCollection", "Clear", True, "benchmark" },
    { 20, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Contains", True, "benchmark" },
    { 21, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Contains", True, "benchmark" },
    { 22, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Contains:System.Boolean(System.Object)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Contains", True, "benchmark" },
    { 23, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.Common.DataColumnMappingCollection", "CopyTo", True, "benchmark" },
    { 24, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.Common.DataColumnMappingCollection", "CopyTo", True, "benchmark" },
    { 25, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::GetByDataSetColumn:System.Data.Common.DataColumnMapping(System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "GetByDataSetColumn", True, "benchmark" },
    { 26, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::GetByDataSetColumn:System.Data.Common.DataColumnMapping(System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "GetByDataSetColumn", True, "benchmark" },
    { 27, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Data.Common.DataColumnMappingCollection", "GetEnumerator", True, "benchmark" },
    { 28, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::IndexOf:System.Int32(System.Object)", "codegen", "System.Data.Common.DataColumnMappingCollection", "IndexOf", True, "benchmark" },
    { 29, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "IndexOf", True, "benchmark" },
    { 30, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "IndexOf", True, "benchmark" },
    { 31, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::IndexOfDataSetColumn:System.Int32(System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "IndexOfDataSetColumn", True, "benchmark" },
    { 32, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::IndexOfDataSetColumn:System.Int32(System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "IndexOfDataSetColumn", True, "benchmark" },
    { 33, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Insert:System.Void(System.Int32,System.Object)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Insert", True, "benchmark" },
    { 34, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Insert:System.Void(System.Int32,System.Object)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Insert", True, "benchmark" },
    { 35, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Insert:System.Void(System.Int32,System.Data.Common.DataColumnMapping)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Insert", True, "benchmark" },
    { 36, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Insert:System.Void(System.Int32,System.Data.Common.DataColumnMapping)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Insert", True, "benchmark" },
    { 37, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.Common.DataColumnMappingCollection", "RemoveAt", True, "benchmark" },
    { 38, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.Common.DataColumnMappingCollection", "RemoveAt", True, "benchmark" },
    { 39, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::RemoveAt:System.Void(System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "RemoveAt", True, "benchmark" },
    { 40, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::RemoveAt:System.Void(System.String)", "codegen", "System.Data.Common.DataColumnMappingCollection", "RemoveAt", True, "benchmark" },
    { 41, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Remove:System.Void(System.Object)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Remove", True, "benchmark" },
    { 42, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::Remove:System.Void(System.Data.Common.DataColumnMapping)", "codegen", "System.Data.Common.DataColumnMappingCollection", "Remove", True, "benchmark" },
    { 43, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::GetDataColumn:System.Data.DataColumn(System.Data.Common.DataColumnMappingCollection,System.String,System.Type,System.Data.DataTable,System.Data.MissingMappingAction,System.Data.MissingSchemaAction)", "codegen", "System.Data.Common.DataColumnMappingCollection", "GetDataColumn", True, "benchmark" },
    { 44, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::GetDataColumn:System.Data.DataColumn(System.Data.Common.DataColumnMappingCollection,System.String,System.Type,System.Data.DataTable,System.Data.MissingMappingAction,System.Data.MissingSchemaAction)", "codegen", "System.Data.Common.DataColumnMappingCollection", "GetDataColumn", True, "benchmark" },
    { 45, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::GetColumnMappingBySchemaAction:System.Data.Common.DataColumnMapping(System.Data.Common.DataColumnMappingCollection,System.String,System.Data.MissingMappingAction)", "codegen", "System.Data.Common.DataColumnMappingCollection", "GetColumnMappingBySchemaAction", True, "benchmark" },
    { 46, "System.Data.Common/System.Data.Common.DataColumnMappingCollection::GetColumnMappingBySchemaAction:System.Data.Common.DataColumnMapping(System.Data.Common.DataColumnMappingCollection,System.String,System.Data.MissingMappingAction)", "codegen", "System.Data.Common.DataColumnMappingCollection", "GetColumnMappingBySchemaAction", True, "benchmark" },
    { 47, "System.Data.Common/System.Data.Common.DataTableMapping::GetDataColumn:System.Data.DataColumn(System.String,System.Type,System.Data.DataTable,System.Data.MissingMappingAction,System.Data.MissingSchemaAction)", "codegen", "System.Data.Common.DataTableMapping", "GetDataColumn", True, "benchmark" },
    { 48, "System.Data.Common/System.Data.Common.DataTableMapping::GetDataColumn:System.Data.DataColumn(System.String,System.Type,System.Data.DataTable,System.Data.MissingMappingAction,System.Data.MissingSchemaAction)", "codegen", "System.Data.Common.DataTableMapping", "GetDataColumn", True, "benchmark" },
    { 49, "System.Data.Common/System.Data.Common.DataTableMapping::GetColumnMappingBySchemaAction:System.Data.Common.DataColumnMapping(System.String,System.Data.MissingMappingAction)", "codegen", "System.Data.Common.DataTableMapping", "GetColumnMappingBySchemaAction", True, "benchmark" },
    { 50, "System.Data.Common/System.Data.Common.DataTableMapping::GetColumnMappingBySchemaAction:System.Data.Common.DataColumnMapping(System.String,System.Data.MissingMappingAction)", "codegen", "System.Data.Common.DataTableMapping", "GetColumnMappingBySchemaAction", True, "benchmark" },
    { 51, "System.Data.Common/System.Data.Common.DataTableMapping::GetDataTableBySchemaAction:System.Data.DataTable(System.Data.DataSet,System.Data.MissingSchemaAction)", "codegen", "System.Data.Common.DataTableMapping", "GetDataTableBySchemaAction", True, "benchmark" },
    { 52, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Add:System.Int32(System.Object)", "codegen", "System.Data.Common.DataTableMappingCollection", "Add", True, "benchmark" },
    { 53, "System.Data.Common/System.Data.Common.DataTableMappingCollection::AddRange:System.Void(System.Data.Common.DataTableMapping[])", "codegen", "System.Data.Common.DataTableMappingCollection", "AddRange", True, "benchmark" },
    { 54, "System.Data.Common/System.Data.Common.DataTableMappingCollection::AddRange:System.Void(System.Data.Common.DataTableMapping[])", "codegen", "System.Data.Common.DataTableMappingCollection", "AddRange", True, "benchmark" },
    { 55, "System.Data.Common/System.Data.Common.DataTableMappingCollection::AddRange:System.Void(System.Data.Common.DataTableMapping[])", "codegen", "System.Data.Common.DataTableMappingCollection", "AddRange", True, "benchmark" },
    { 56, "System.Data.Common/System.Data.Common.DataTableMappingCollection::AddRange:System.Void(System.Array)", "codegen", "System.Data.Common.DataTableMappingCollection", "AddRange", True, "benchmark" },
    { 57, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Add:System.Data.Common.DataTableMapping(System.String,System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "Add", True, "benchmark" },
    { 58, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Add:System.Data.Common.DataTableMapping(System.String,System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "Add", True, "benchmark" },
    { 59, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Add:System.Data.Common.DataTableMapping(System.String,System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "Add", True, "benchmark" },
    { 60, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Clear:System.Void()", "codegen", "System.Data.Common.DataTableMappingCollection", "Clear", True, "benchmark" },
    { 61, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "Contains", True, "benchmark" },
    { 62, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "Contains", True, "benchmark" },
    { 63, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Contains:System.Boolean(System.Object)", "codegen", "System.Data.Common.DataTableMappingCollection", "Contains", True, "benchmark" },
    { 64, "System.Data.Common/System.Data.Common.DataTableMappingCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.Common.DataTableMappingCollection", "CopyTo", True, "benchmark" },
    { 65, "System.Data.Common/System.Data.Common.DataTableMappingCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.Common.DataTableMappingCollection", "CopyTo", True, "benchmark" },
    { 66, "System.Data.Common/System.Data.Common.DataTableMappingCollection::GetByDataSetTable:System.Data.Common.DataTableMapping(System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "GetByDataSetTable", True, "benchmark" },
    { 67, "System.Data.Common/System.Data.Common.DataTableMappingCollection::GetByDataSetTable:System.Data.Common.DataTableMapping(System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "GetByDataSetTable", True, "benchmark" },
    { 68, "System.Data.Common/System.Data.Common.DataTableMappingCollection::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Data.Common.DataTableMappingCollection", "GetEnumerator", True, "benchmark" },
    { 69, "System.Data.Common/System.Data.Common.DataTableMappingCollection::IndexOf:System.Int32(System.Object)", "codegen", "System.Data.Common.DataTableMappingCollection", "IndexOf", True, "benchmark" },
    { 70, "System.Data.Common/System.Data.Common.DataTableMappingCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "IndexOf", True, "benchmark" },
    { 71, "System.Data.Common/System.Data.Common.DataTableMappingCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "IndexOf", True, "benchmark" },
    { 72, "System.Data.Common/System.Data.Common.DataTableMappingCollection::IndexOfDataSetTable:System.Int32(System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "IndexOfDataSetTable", True, "benchmark" },
    { 73, "System.Data.Common/System.Data.Common.DataTableMappingCollection::IndexOfDataSetTable:System.Int32(System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "IndexOfDataSetTable", True, "benchmark" },
    { 74, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Insert:System.Void(System.Int32,System.Object)", "codegen", "System.Data.Common.DataTableMappingCollection", "Insert", True, "benchmark" },
    { 75, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Insert:System.Void(System.Int32,System.Object)", "codegen", "System.Data.Common.DataTableMappingCollection", "Insert", True, "benchmark" },
    { 76, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Insert:System.Void(System.Int32,System.Data.Common.DataTableMapping)", "codegen", "System.Data.Common.DataTableMappingCollection", "Insert", True, "benchmark" },
    { 77, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Insert:System.Void(System.Int32,System.Data.Common.DataTableMapping)", "codegen", "System.Data.Common.DataTableMappingCollection", "Insert", True, "benchmark" },
    { 78, "System.Data.Common/System.Data.Common.DataTableMappingCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.Common.DataTableMappingCollection", "RemoveAt", True, "benchmark" },
    { 79, "System.Data.Common/System.Data.Common.DataTableMappingCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.Common.DataTableMappingCollection", "RemoveAt", True, "benchmark" },
    { 80, "System.Data.Common/System.Data.Common.DataTableMappingCollection::RemoveAt:System.Void(System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "RemoveAt", True, "benchmark" },
    { 81, "System.Data.Common/System.Data.Common.DataTableMappingCollection::RemoveAt:System.Void(System.String)", "codegen", "System.Data.Common.DataTableMappingCollection", "RemoveAt", True, "benchmark" },
    { 82, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Remove:System.Void(System.Object)", "codegen", "System.Data.Common.DataTableMappingCollection", "Remove", True, "benchmark" },
    { 83, "System.Data.Common/System.Data.Common.DataTableMappingCollection::Remove:System.Void(System.Data.Common.DataTableMapping)", "codegen", "System.Data.Common.DataTableMappingCollection", "Remove", True, "benchmark" },
    { 84, "System.Data.Common/System.Data.Common.DataTableMappingCollection::GetTableMappingBySchemaAction:System.Data.Common.DataTableMapping(System.Data.Common.DataTableMappingCollection,System.String,System.String,System.Data.MissingMappingAction)", "codegen", "System.Data.Common.DataTableMappingCollection", "GetTableMappingBySchemaAction", True, "benchmark" },
    { 85, "System.Data.Common/System.Data.Common.DataTableMappingCollection::GetTableMappingBySchemaAction:System.Data.Common.DataTableMapping(System.Data.Common.DataTableMappingCollection,System.String,System.String,System.Data.MissingMappingAction)", "codegen", "System.Data.Common.DataTableMappingCollection", "GetTableMappingBySchemaAction", True, "benchmark" },
    { 86, "System.Data.Common/System.Data.Common.DataTableMappingCollection::GetTableMappingBySchemaAction:System.Data.Common.DataTableMapping(System.Data.Common.DataTableMappingCollection,System.String,System.String,System.Data.MissingMappingAction)", "codegen", "System.Data.Common.DataTableMappingCollection", "GetTableMappingBySchemaAction", True, "benchmark" },
    { 87, "System.Data.Common/System.Data.Common.DbBatch::ExecuteReader:System.Data.Common.DbDataReader(System.Data.CommandBehavior)", "codegen", "System.Data.Common.DbBatch", "ExecuteReader", True, "benchmark" },
    { 88, "System.Data.Common/System.Data.Common.DbBatch::ExecuteReaderAsync:System.Threading.Tasks.Task<System.Data.Common.DbDataReader>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbBatch", "ExecuteReaderAsync", True, "benchmark" },
    { 89, "System.Data.Common/System.Data.Common.DbBatch::ExecuteReaderAsync:System.Threading.Tasks.Task<System.Data.Common.DbDataReader>(System.Data.CommandBehavior,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbBatch", "ExecuteReaderAsync", True, "benchmark" },
    { 90, "System.Data.Common/System.Data.Common.DbBatch::ExecuteNonQuery:System.Int32()", "codegen", "System.Data.Common.DbBatch", "ExecuteNonQuery", True, "benchmark" },
    { 91, "System.Data.Common/System.Data.Common.DbBatch::ExecuteNonQueryAsync:System.Threading.Tasks.Task<System.Int32>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbBatch", "ExecuteNonQueryAsync", True, "benchmark" },
    { 92, "System.Data.Common/System.Data.Common.DbBatch::ExecuteScalar:System.Object()", "codegen", "System.Data.Common.DbBatch", "ExecuteScalar", True, "benchmark" },
    { 93, "System.Data.Common/System.Data.Common.DbBatch::ExecuteScalarAsync:System.Threading.Tasks.Task<System.Object>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbBatch", "ExecuteScalarAsync", True, "benchmark" },
    { 94, "System.Data.Common/System.Data.Common.DbBatch::Prepare:System.Void()", "codegen", "System.Data.Common.DbBatch", "Prepare", True, "benchmark" },
    { 95, "System.Data.Common/System.Data.Common.DbBatch::PrepareAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbBatch", "PrepareAsync", True, "benchmark" },
    { 96, "System.Data.Common/System.Data.Common.DbBatch::Cancel:System.Void()", "codegen", "System.Data.Common.DbBatch", "Cancel", True, "benchmark" },
    { 97, "System.Data.Common/System.Data.Common.DbBatch::CreateBatchCommand:System.Data.Common.DbBatchCommand()", "codegen", "System.Data.Common.DbBatch", "CreateBatchCommand", True, "benchmark" },
    { 98, "System.Data.Common/System.Data.Common.DbBatch::Dispose:System.Void()", "codegen", "System.Data.Common.DbBatch", "Dispose", True, "benchmark" },
    { 99, "System.Data.Common/System.Data.Common.DbBatch::DisposeAsync:System.Threading.Tasks.ValueTask()", "codegen", "System.Data.Common.DbBatch", "DisposeAsync", True, "benchmark" },
    { 100, "System.Data.Common/System.Data.Common.DbBatchCommand::CreateParameter:System.Data.Common.DbParameter()", "codegen", "System.Data.Common.DbBatchCommand", "CreateParameter", True, "benchmark" },
    { 101, "System.Data.Common/System.Data.Common.DbBatchCommandCollection::GetEnumerator:System.Collections.Generic.IEnumerator<System.Data.Common.DbBatchCommand>()", "codegen", "System.Data.Common.DbBatchCommandCollection", "GetEnumerator", True, "benchmark" },
    { 102, "System.Data.Common/System.Data.Common.DbBatchCommandCollection::Add:System.Void(System.Data.Common.DbBatchCommand)", "codegen", "System.Data.Common.DbBatchCommandCollection", "Add", True, "benchmark" },
    { 103, "System.Data.Common/System.Data.Common.DbBatchCommandCollection::Clear:System.Void()", "codegen", "System.Data.Common.DbBatchCommandCollection", "Clear", True, "benchmark" },
    { 104, "System.Data.Common/System.Data.Common.DbBatchCommandCollection::Contains:System.Boolean(System.Data.Common.DbBatchCommand)", "codegen", "System.Data.Common.DbBatchCommandCollection", "Contains", True, "benchmark" },
    { 105, "System.Data.Common/System.Data.Common.DbBatchCommandCollection::Remove:System.Boolean(System.Data.Common.DbBatchCommand)", "codegen", "System.Data.Common.DbBatchCommandCollection", "Remove", True, "benchmark" },
    { 106, "System.Data.Common/System.Data.Common.DbBatchCommandCollection::IndexOf:System.Int32(System.Data.Common.DbBatchCommand)", "codegen", "System.Data.Common.DbBatchCommandCollection", "IndexOf", True, "benchmark" },
    { 107, "System.Data.Common/System.Data.Common.DbBatchCommandCollection::Insert:System.Void(System.Int32,System.Data.Common.DbBatchCommand)", "codegen", "System.Data.Common.DbBatchCommandCollection", "Insert", True, "benchmark" },
    { 108, "System.Data.Common/System.Data.Common.DbBatchCommandCollection::Insert:System.Void(System.Int32,System.Data.Common.DbBatchCommand)", "codegen", "System.Data.Common.DbBatchCommandCollection", "Insert", True, "benchmark" },
    { 109, "System.Data.Common/System.Data.Common.DbBatchCommandCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.Common.DbBatchCommandCollection", "RemoveAt", True, "benchmark" },
    { 110, "System.Data.Common/System.Data.Common.DbBatchCommandCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.Common.DbBatchCommandCollection", "RemoveAt", True, "benchmark" },
    { 111, "System.Data.Common/System.Data.Common.DbCommand::Cancel:System.Void()", "codegen", "System.Data.Common.DbCommand", "Cancel", True, "benchmark" },
    { 112, "System.Data.Common/System.Data.Common.DbCommand::CreateParameter:System.Data.Common.DbParameter()", "codegen", "System.Data.Common.DbCommand", "CreateParameter", True, "benchmark" },
    { 113, "System.Data.Common/System.Data.Common.DbCommand::ExecuteNonQuery:System.Int32()", "codegen", "System.Data.Common.DbCommand", "ExecuteNonQuery", True, "benchmark" },
    { 114, "System.Data.Common/System.Data.Common.DbCommand::ExecuteReader:System.Data.Common.DbDataReader()", "codegen", "System.Data.Common.DbCommand", "ExecuteReader", True, "benchmark" },
    { 115, "System.Data.Common/System.Data.Common.DbCommand::ExecuteReader:System.Data.Common.DbDataReader(System.Data.CommandBehavior)", "codegen", "System.Data.Common.DbCommand", "ExecuteReader", True, "benchmark" },
    { 116, "System.Data.Common/System.Data.Common.DbCommand::ExecuteNonQueryAsync:System.Threading.Tasks.Task<System.Int32>()", "codegen", "System.Data.Common.DbCommand", "ExecuteNonQueryAsync", True, "benchmark" },
    { 117, "System.Data.Common/System.Data.Common.DbCommand::ExecuteNonQueryAsync:System.Threading.Tasks.Task<System.Int32>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbCommand", "ExecuteNonQueryAsync", True, "benchmark" },
    { 118, "System.Data.Common/System.Data.Common.DbCommand::ExecuteReaderAsync:System.Threading.Tasks.Task<System.Data.Common.DbDataReader>()", "codegen", "System.Data.Common.DbCommand", "ExecuteReaderAsync", True, "benchmark" },
    { 119, "System.Data.Common/System.Data.Common.DbCommand::ExecuteReaderAsync:System.Threading.Tasks.Task<System.Data.Common.DbDataReader>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbCommand", "ExecuteReaderAsync", True, "benchmark" },
    { 120, "System.Data.Common/System.Data.Common.DbCommand::ExecuteReaderAsync:System.Threading.Tasks.Task<System.Data.Common.DbDataReader>(System.Data.CommandBehavior)", "codegen", "System.Data.Common.DbCommand", "ExecuteReaderAsync", True, "benchmark" },
    { 121, "System.Data.Common/System.Data.Common.DbCommand::ExecuteReaderAsync:System.Threading.Tasks.Task<System.Data.Common.DbDataReader>(System.Data.CommandBehavior,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbCommand", "ExecuteReaderAsync", True, "benchmark" },
    { 122, "System.Data.Common/System.Data.Common.DbCommand::ExecuteScalarAsync:System.Threading.Tasks.Task<System.Object>()", "codegen", "System.Data.Common.DbCommand", "ExecuteScalarAsync", True, "benchmark" },
    { 123, "System.Data.Common/System.Data.Common.DbCommand::ExecuteScalarAsync:System.Threading.Tasks.Task<System.Object>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbCommand", "ExecuteScalarAsync", True, "benchmark" },
    { 124, "System.Data.Common/System.Data.Common.DbCommand::ExecuteScalar:System.Object()", "codegen", "System.Data.Common.DbCommand", "ExecuteScalar", True, "benchmark" },
    { 125, "System.Data.Common/System.Data.Common.DbCommand::Prepare:System.Void()", "codegen", "System.Data.Common.DbCommand", "Prepare", True, "benchmark" },
    { 126, "System.Data.Common/System.Data.Common.DbCommand::PrepareAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbCommand", "PrepareAsync", True, "benchmark" },
    { 127, "System.Data.Common/System.Data.Common.DbCommand::DisposeAsync:System.Threading.Tasks.ValueTask()", "codegen", "System.Data.Common.DbCommand", "DisposeAsync", True, "benchmark" },
    { 128, "System.Data.Common/System.Data.Common.DbCommandBuilder::GetInsertCommand:System.Data.Common.DbCommand()", "codegen", "System.Data.Common.DbCommandBuilder", "GetInsertCommand", True, "benchmark" },
    { 129, "System.Data.Common/System.Data.Common.DbCommandBuilder::GetInsertCommand:System.Data.Common.DbCommand(System.Boolean)", "codegen", "System.Data.Common.DbCommandBuilder", "GetInsertCommand", True, "benchmark" },
    { 130, "System.Data.Common/System.Data.Common.DbCommandBuilder::GetInsertCommand:System.Data.Common.DbCommand(System.Boolean)", "codegen", "System.Data.Common.DbCommandBuilder", "GetInsertCommand", True, "benchmark" },
    { 131, "System.Data.Common/System.Data.Common.DbCommandBuilder::GetUpdateCommand:System.Data.Common.DbCommand()", "codegen", "System.Data.Common.DbCommandBuilder", "GetUpdateCommand", True, "benchmark" },
    { 132, "System.Data.Common/System.Data.Common.DbCommandBuilder::GetUpdateCommand:System.Data.Common.DbCommand(System.Boolean)", "codegen", "System.Data.Common.DbCommandBuilder", "GetUpdateCommand", True, "benchmark" },
    { 133, "System.Data.Common/System.Data.Common.DbCommandBuilder::GetUpdateCommand:System.Data.Common.DbCommand(System.Boolean)", "codegen", "System.Data.Common.DbCommandBuilder", "GetUpdateCommand", True, "benchmark" },
    { 134, "System.Data.Common/System.Data.Common.DbCommandBuilder::GetDeleteCommand:System.Data.Common.DbCommand()", "codegen", "System.Data.Common.DbCommandBuilder", "GetDeleteCommand", True, "benchmark" },
    { 135, "System.Data.Common/System.Data.Common.DbCommandBuilder::GetDeleteCommand:System.Data.Common.DbCommand(System.Boolean)", "codegen", "System.Data.Common.DbCommandBuilder", "GetDeleteCommand", True, "benchmark" },
    { 136, "System.Data.Common/System.Data.Common.DbCommandBuilder::GetDeleteCommand:System.Data.Common.DbCommand(System.Boolean)", "codegen", "System.Data.Common.DbCommandBuilder", "GetDeleteCommand", True, "benchmark" },
    { 137, "System.Data.Common/System.Data.Common.DbCommandBuilder::QuoteIdentifier:System.String(System.String)", "codegen", "System.Data.Common.DbCommandBuilder", "QuoteIdentifier", True, "benchmark" },
    { 138, "System.Data.Common/System.Data.Common.DbCommandBuilder::QuoteIdentifier:System.String(System.String)", "codegen", "System.Data.Common.DbCommandBuilder", "QuoteIdentifier", True, "benchmark" },
    { 139, "System.Data.Common/System.Data.Common.DbCommandBuilder::RefreshSchema:System.Void()", "codegen", "System.Data.Common.DbCommandBuilder", "RefreshSchema", True, "benchmark" },
    { 140, "System.Data.Common/System.Data.Common.DbCommandBuilder::UnquoteIdentifier:System.String(System.String)", "codegen", "System.Data.Common.DbCommandBuilder", "UnquoteIdentifier", True, "benchmark" },
    { 141, "System.Data.Common/System.Data.Common.DbCommandBuilder::UnquoteIdentifier:System.String(System.String)", "codegen", "System.Data.Common.DbCommandBuilder", "UnquoteIdentifier", True, "benchmark" },
    { 142, "System.Data.Common/System.Data.Common.DbConnection::BeginTransaction:System.Data.Common.DbTransaction()", "codegen", "System.Data.Common.DbConnection", "BeginTransaction", True, "benchmark" },
    { 143, "System.Data.Common/System.Data.Common.DbConnection::BeginTransaction:System.Data.Common.DbTransaction(System.Data.IsolationLevel)", "codegen", "System.Data.Common.DbConnection", "BeginTransaction", True, "benchmark" },
    { 144, "System.Data.Common/System.Data.Common.DbConnection::BeginTransactionAsync:System.Threading.Tasks.ValueTask<System.Data.Common.DbTransaction>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "BeginTransactionAsync", True, "benchmark" },
    { 145, "System.Data.Common/System.Data.Common.DbConnection::BeginTransactionAsync:System.Threading.Tasks.ValueTask<System.Data.Common.DbTransaction>(System.Data.IsolationLevel,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "BeginTransactionAsync", True, "benchmark" },
    { 146, "System.Data.Common/System.Data.Common.DbConnection::Close:System.Void()", "codegen", "System.Data.Common.DbConnection", "Close", True, "benchmark" },
    { 147, "System.Data.Common/System.Data.Common.DbConnection::CloseAsync:System.Threading.Tasks.Task()", "codegen", "System.Data.Common.DbConnection", "CloseAsync", True, "benchmark" },
    { 148, "System.Data.Common/System.Data.Common.DbConnection::DisposeAsync:System.Threading.Tasks.ValueTask()", "codegen", "System.Data.Common.DbConnection", "DisposeAsync", True, "benchmark" },
    { 149, "System.Data.Common/System.Data.Common.DbConnection::ChangeDatabase:System.Void(System.String)", "codegen", "System.Data.Common.DbConnection", "ChangeDatabase", True, "benchmark" },
    { 150, "System.Data.Common/System.Data.Common.DbConnection::ChangeDatabase:System.Void(System.String)", "codegen", "System.Data.Common.DbConnection", "ChangeDatabase", True, "benchmark" },
    { 151, "System.Data.Common/System.Data.Common.DbConnection::ChangeDatabaseAsync:System.Threading.Tasks.Task(System.String,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "ChangeDatabaseAsync", True, "benchmark" },
    { 152, "System.Data.Common/System.Data.Common.DbConnection::ChangeDatabaseAsync:System.Threading.Tasks.Task(System.String,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "ChangeDatabaseAsync", True, "benchmark" },
    { 153, "System.Data.Common/System.Data.Common.DbConnection::CreateBatch:System.Data.Common.DbBatch()", "codegen", "System.Data.Common.DbConnection", "CreateBatch", True, "benchmark" },
    { 154, "System.Data.Common/System.Data.Common.DbConnection::CreateCommand:System.Data.Common.DbCommand()", "codegen", "System.Data.Common.DbConnection", "CreateCommand", True, "benchmark" },
    { 155, "System.Data.Common/System.Data.Common.DbConnection::EnlistTransaction:System.Void(System.Transactions.Transaction)", "codegen", "System.Data.Common.DbConnection", "EnlistTransaction", True, "benchmark" },
    { 156, "System.Data.Common/System.Data.Common.DbConnection::GetSchema:System.Data.DataTable()", "codegen", "System.Data.Common.DbConnection", "GetSchema", True, "benchmark" },
    { 157, "System.Data.Common/System.Data.Common.DbConnection::GetSchema:System.Data.DataTable(System.String)", "codegen", "System.Data.Common.DbConnection", "GetSchema", True, "benchmark" },
    { 158, "System.Data.Common/System.Data.Common.DbConnection::GetSchema:System.Data.DataTable(System.String)", "codegen", "System.Data.Common.DbConnection", "GetSchema", True, "benchmark" },
    { 159, "System.Data.Common/System.Data.Common.DbConnection::GetSchema:System.Data.DataTable(System.String,System.String[])", "codegen", "System.Data.Common.DbConnection", "GetSchema", True, "benchmark" },
    { 160, "System.Data.Common/System.Data.Common.DbConnection::GetSchema:System.Data.DataTable(System.String,System.String[])", "codegen", "System.Data.Common.DbConnection", "GetSchema", True, "benchmark" },
    { 161, "System.Data.Common/System.Data.Common.DbConnection::GetSchema:System.Data.DataTable(System.String,System.String[])", "codegen", "System.Data.Common.DbConnection", "GetSchema", True, "benchmark" },
    { 162, "System.Data.Common/System.Data.Common.DbConnection::GetSchema:System.Data.DataTable(System.String,System.String[])", "codegen", "System.Data.Common.DbConnection", "GetSchema", True, "benchmark" },
    { 163, "System.Data.Common/System.Data.Common.DbConnection::GetSchemaAsync:System.Threading.Tasks.Task<System.Data.DataTable>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "GetSchemaAsync", True, "benchmark" },
    { 164, "System.Data.Common/System.Data.Common.DbConnection::GetSchemaAsync:System.Threading.Tasks.Task<System.Data.DataTable>(System.String,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "GetSchemaAsync", True, "benchmark" },
    { 165, "System.Data.Common/System.Data.Common.DbConnection::GetSchemaAsync:System.Threading.Tasks.Task<System.Data.DataTable>(System.String,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "GetSchemaAsync", True, "benchmark" },
    { 166, "System.Data.Common/System.Data.Common.DbConnection::GetSchemaAsync:System.Threading.Tasks.Task<System.Data.DataTable>(System.String,System.String[],System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "GetSchemaAsync", True, "benchmark" },
    { 167, "System.Data.Common/System.Data.Common.DbConnection::GetSchemaAsync:System.Threading.Tasks.Task<System.Data.DataTable>(System.String,System.String[],System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "GetSchemaAsync", True, "benchmark" },
    { 168, "System.Data.Common/System.Data.Common.DbConnection::GetSchemaAsync:System.Threading.Tasks.Task<System.Data.DataTable>(System.String,System.String[],System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "GetSchemaAsync", True, "benchmark" },
    { 169, "System.Data.Common/System.Data.Common.DbConnection::GetSchemaAsync:System.Threading.Tasks.Task<System.Data.DataTable>(System.String,System.String[],System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "GetSchemaAsync", True, "benchmark" },
    { 170, "System.Data.Common/System.Data.Common.DbConnection::Open:System.Void()", "codegen", "System.Data.Common.DbConnection", "Open", True, "benchmark" },
    { 171, "System.Data.Common/System.Data.Common.DbConnection::OpenAsync:System.Threading.Tasks.Task()", "codegen", "System.Data.Common.DbConnection", "OpenAsync", True, "benchmark" },
    { 172, "System.Data.Common/System.Data.Common.DbConnection::OpenAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbConnection", "OpenAsync", True, "benchmark" },
    { 173, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::Add:System.Void(System.String,System.Object)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "Add", True, "benchmark" },
    { 174, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::Add:System.Void(System.String,System.Object)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "Add", True, "benchmark" },
    { 175, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::AppendKeyValuePair:System.Void(System.Text.StringBuilder,System.String,System.String)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "AppendKeyValuePair", True, "benchmark" },
    { 176, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::AppendKeyValuePair:System.Void(System.Text.StringBuilder,System.String,System.String)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "AppendKeyValuePair", True, "benchmark" },
    { 177, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::AppendKeyValuePair:System.Void(System.Text.StringBuilder,System.String,System.String)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "AppendKeyValuePair", True, "benchmark" },
    { 178, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::AppendKeyValuePair:System.Void(System.Text.StringBuilder,System.String,System.String,System.Boolean)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "AppendKeyValuePair", True, "benchmark" },
    { 179, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::AppendKeyValuePair:System.Void(System.Text.StringBuilder,System.String,System.String,System.Boolean)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "AppendKeyValuePair", True, "benchmark" },
    { 180, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::AppendKeyValuePair:System.Void(System.Text.StringBuilder,System.String,System.String,System.Boolean)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "AppendKeyValuePair", True, "benchmark" },
    { 181, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::Clear:System.Void()", "codegen", "System.Data.Common.DbConnectionStringBuilder", "Clear", True, "benchmark" },
    { 182, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::ContainsKey:System.Boolean(System.String)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "ContainsKey", True, "benchmark" },
    { 183, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::ContainsKey:System.Boolean(System.String)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "ContainsKey", True, "benchmark" },
    { 184, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::EquivalentTo:System.Boolean(System.Data.Common.DbConnectionStringBuilder)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "EquivalentTo", True, "benchmark" },
    { 185, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::Remove:System.Boolean(System.String)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "Remove", True, "benchmark" },
    { 186, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::Remove:System.Boolean(System.String)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "Remove", True, "benchmark" },
    { 187, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::ShouldSerialize:System.Boolean(System.String)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "ShouldSerialize", True, "benchmark" },
    { 188, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::ShouldSerialize:System.Boolean(System.String)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "ShouldSerialize", True, "benchmark" },
    { 189, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::TryGetValue:System.Boolean(System.String,System.Object)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "TryGetValue", True, "benchmark" },
    { 190, "System.Data.Common/System.Data.Common.DbConnectionStringBuilder::TryGetValue:System.Boolean(System.String,System.Object)", "codegen", "System.Data.Common.DbConnectionStringBuilder", "TryGetValue", True, "benchmark" },
    { 191, "System.Data.Common/System.Data.Common.DbDataAdapter::FillSchema:System.Data.DataTable(System.Data.DataTable,System.Data.SchemaType)", "codegen", "System.Data.Common.DbDataAdapter", "FillSchema", True, "benchmark" },
    { 192, "System.Data.Common/System.Data.Common.DbDataAdapter::FillSchema:System.Data.DataTable[](System.Data.DataSet,System.Data.SchemaType)", "codegen", "System.Data.Common.DbDataAdapter", "FillSchema", True, "benchmark" },
    { 193, "System.Data.Common/System.Data.Common.DbDataAdapter::FillSchema:System.Data.DataTable[](System.Data.DataSet,System.Data.SchemaType,System.String)", "codegen", "System.Data.Common.DbDataAdapter", "FillSchema", True, "benchmark" },
    { 194, "System.Data.Common/System.Data.Common.DbDataAdapter::FillSchema:System.Data.DataTable[](System.Data.DataSet,System.Data.SchemaType,System.String)", "codegen", "System.Data.Common.DbDataAdapter", "FillSchema", True, "benchmark" },
    { 195, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Data.DataSet)", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 196, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Data.DataSet,System.String)", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 197, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Data.DataSet,System.String)", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 198, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Data.DataSet,System.Int32,System.Int32,System.String)", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 199, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Data.DataSet,System.Int32,System.Int32,System.String)", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 200, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Data.DataSet,System.Int32,System.Int32,System.String)", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 201, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Data.DataTable)", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 202, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Int32,System.Int32,System.Data.DataTable[])", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 203, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Int32,System.Int32,System.Data.DataTable[])", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 204, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Int32,System.Int32,System.Data.DataTable[])", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 205, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Int32,System.Int32,System.Data.DataTable[])", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 206, "System.Data.Common/System.Data.Common.DbDataAdapter::Fill:System.Int32(System.Int32,System.Int32,System.Data.DataTable[])", "codegen", "System.Data.Common.DbDataAdapter", "Fill", True, "benchmark" },
    { 207, "System.Data.Common/System.Data.Common.DbDataAdapter::GetFillParameters:System.Data.IDataParameter[]()", "codegen", "System.Data.Common.DbDataAdapter", "GetFillParameters", True, "benchmark" },
    { 208, "System.Data.Common/System.Data.Common.DbDataAdapter::Update:System.Int32(System.Data.DataSet)", "codegen", "System.Data.Common.DbDataAdapter", "Update", True, "benchmark" },
    { 209, "System.Data.Common/System.Data.Common.DbDataAdapter::Update:System.Int32(System.Data.DataRow[])", "codegen", "System.Data.Common.DbDataAdapter", "Update", True, "benchmark" },
    { 210, "System.Data.Common/System.Data.Common.DbDataAdapter::Update:System.Int32(System.Data.DataRow[])", "codegen", "System.Data.Common.DbDataAdapter", "Update", True, "benchmark" },
    { 211, "System.Data.Common/System.Data.Common.DbDataAdapter::Update:System.Int32(System.Data.DataRow[])", "codegen", "System.Data.Common.DbDataAdapter", "Update", True, "benchmark" },
    { 212, "System.Data.Common/System.Data.Common.DbDataAdapter::Update:System.Int32(System.Data.DataTable)", "codegen", "System.Data.Common.DbDataAdapter", "Update", True, "benchmark" },
    { 213, "System.Data.Common/System.Data.Common.DbDataAdapter::Update:System.Int32(System.Data.DataSet,System.String)", "codegen", "System.Data.Common.DbDataAdapter", "Update", True, "benchmark" },
    { 214, "System.Data.Common/System.Data.Common.DbDataAdapter::Update:System.Int32(System.Data.DataSet,System.String)", "codegen", "System.Data.Common.DbDataAdapter", "Update", True, "benchmark" },
    { 215, "System.Data.Common/System.Data.Common.DbDataReader::Close:System.Void()", "codegen", "System.Data.Common.DbDataReader", "Close", True, "benchmark" },
    { 216, "System.Data.Common/System.Data.Common.DbDataReader::CloseAsync:System.Threading.Tasks.Task()", "codegen", "System.Data.Common.DbDataReader", "CloseAsync", True, "benchmark" },
    { 217, "System.Data.Common/System.Data.Common.DbDataReader::Dispose:System.Void()", "codegen", "System.Data.Common.DbDataReader", "Dispose", True, "benchmark" },
    { 218, "System.Data.Common/System.Data.Common.DbDataReader::DisposeAsync:System.Threading.Tasks.ValueTask()", "codegen", "System.Data.Common.DbDataReader", "DisposeAsync", True, "benchmark" },
    { 219, "System.Data.Common/System.Data.Common.DbDataReader::GetDataTypeName:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetDataTypeName", True, "benchmark" },
    { 220, "System.Data.Common/System.Data.Common.DbDataReader::GetDataTypeName:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetDataTypeName", True, "benchmark" },
    { 221, "System.Data.Common/System.Data.Common.DbDataReader::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Data.Common.DbDataReader", "GetEnumerator", True, "benchmark" },
    { 222, "System.Data.Common/System.Data.Common.DbDataReader::GetFieldType:System.Type(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetFieldType", True, "benchmark" },
    { 223, "System.Data.Common/System.Data.Common.DbDataReader::GetFieldType:System.Type(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetFieldType", True, "benchmark" },
    { 224, "System.Data.Common/System.Data.Common.DbDataReader::GetName:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetName", True, "benchmark" },
    { 225, "System.Data.Common/System.Data.Common.DbDataReader::GetName:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetName", True, "benchmark" },
    { 226, "System.Data.Common/System.Data.Common.DbDataReader::GetOrdinal:System.Int32(System.String)", "codegen", "System.Data.Common.DbDataReader", "GetOrdinal", True, "benchmark" },
    { 227, "System.Data.Common/System.Data.Common.DbDataReader::GetOrdinal:System.Int32(System.String)", "codegen", "System.Data.Common.DbDataReader", "GetOrdinal", True, "benchmark" },
    { 228, "System.Data.Common/System.Data.Common.DbDataReader::GetSchemaTable:System.Data.DataTable()", "codegen", "System.Data.Common.DbDataReader", "GetSchemaTable", True, "benchmark" },
    { 229, "System.Data.Common/System.Data.Common.DbDataReader::GetSchemaTableAsync:System.Threading.Tasks.Task<System.Data.DataTable>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbDataReader", "GetSchemaTableAsync", True, "benchmark" },
    { 230, "System.Data.Common/System.Data.Common.DbDataReader::GetColumnSchemaAsync:System.Threading.Tasks.Task<System.Collections.ObjectModel.ReadOnlyCollection<System.Data.Common.DbColumn>>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbDataReader", "GetColumnSchemaAsync", True, "benchmark" },
    { 231, "System.Data.Common/System.Data.Common.DbDataReader::GetBoolean:System.Boolean(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetBoolean", True, "benchmark" },
    { 232, "System.Data.Common/System.Data.Common.DbDataReader::GetBoolean:System.Boolean(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetBoolean", True, "benchmark" },
    { 233, "System.Data.Common/System.Data.Common.DbDataReader::GetByte:System.Byte(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetByte", True, "benchmark" },
    { 234, "System.Data.Common/System.Data.Common.DbDataReader::GetByte:System.Byte(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetByte", True, "benchmark" },
    { 235, "System.Data.Common/System.Data.Common.DbDataReader::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetBytes", True, "benchmark" },
    { 236, "System.Data.Common/System.Data.Common.DbDataReader::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetBytes", True, "benchmark" },
    { 237, "System.Data.Common/System.Data.Common.DbDataReader::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetBytes", True, "benchmark" },
    { 238, "System.Data.Common/System.Data.Common.DbDataReader::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetBytes", True, "benchmark" },
    { 239, "System.Data.Common/System.Data.Common.DbDataReader::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetBytes", True, "benchmark" },
    { 240, "System.Data.Common/System.Data.Common.DbDataReader::GetChar:System.Char(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetChar", True, "benchmark" },
    { 241, "System.Data.Common/System.Data.Common.DbDataReader::GetChar:System.Char(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetChar", True, "benchmark" },
    { 242, "System.Data.Common/System.Data.Common.DbDataReader::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetChars", True, "benchmark" },
    { 243, "System.Data.Common/System.Data.Common.DbDataReader::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetChars", True, "benchmark" },
    { 244, "System.Data.Common/System.Data.Common.DbDataReader::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetChars", True, "benchmark" },
    { 245, "System.Data.Common/System.Data.Common.DbDataReader::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetChars", True, "benchmark" },
    { 246, "System.Data.Common/System.Data.Common.DbDataReader::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetChars", True, "benchmark" },
    { 247, "System.Data.Common/System.Data.Common.DbDataReader::GetData:System.Data.Common.DbDataReader(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetData", True, "benchmark" },
    { 248, "System.Data.Common/System.Data.Common.DbDataReader::GetData:System.Data.Common.DbDataReader(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetData", True, "benchmark" },
    { 249, "System.Data.Common/System.Data.Common.DbDataReader::GetDateTime:System.DateTime(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetDateTime", True, "benchmark" },
    { 250, "System.Data.Common/System.Data.Common.DbDataReader::GetDateTime:System.DateTime(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetDateTime", True, "benchmark" },
    { 251, "System.Data.Common/System.Data.Common.DbDataReader::GetDecimal:System.Decimal(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetDecimal", True, "benchmark" },
    { 252, "System.Data.Common/System.Data.Common.DbDataReader::GetDecimal:System.Decimal(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetDecimal", True, "benchmark" },
    { 253, "System.Data.Common/System.Data.Common.DbDataReader::GetDouble:System.Double(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetDouble", True, "benchmark" },
    { 254, "System.Data.Common/System.Data.Common.DbDataReader::GetDouble:System.Double(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetDouble", True, "benchmark" },
    { 255, "System.Data.Common/System.Data.Common.DbDataReader::GetFloat:System.Single(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetFloat", True, "benchmark" },
    { 256, "System.Data.Common/System.Data.Common.DbDataReader::GetFloat:System.Single(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetFloat", True, "benchmark" },
    { 257, "System.Data.Common/System.Data.Common.DbDataReader::GetGuid:System.Guid(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetGuid", True, "benchmark" },
    { 258, "System.Data.Common/System.Data.Common.DbDataReader::GetGuid:System.Guid(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetGuid", True, "benchmark" },
    { 259, "System.Data.Common/System.Data.Common.DbDataReader::GetInt16:System.Int16(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetInt16", True, "benchmark" },
    { 260, "System.Data.Common/System.Data.Common.DbDataReader::GetInt16:System.Int16(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetInt16", True, "benchmark" },
    { 261, "System.Data.Common/System.Data.Common.DbDataReader::GetInt32:System.Int32(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetInt32", True, "benchmark" },
    { 262, "System.Data.Common/System.Data.Common.DbDataReader::GetInt32:System.Int32(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetInt32", True, "benchmark" },
    { 263, "System.Data.Common/System.Data.Common.DbDataReader::GetInt64:System.Int64(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetInt64", True, "benchmark" },
    { 264, "System.Data.Common/System.Data.Common.DbDataReader::GetInt64:System.Int64(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetInt64", True, "benchmark" },
    { 265, "System.Data.Common/System.Data.Common.DbDataReader::GetProviderSpecificFieldType:System.Type(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetProviderSpecificFieldType", True, "benchmark" },
    { 266, "System.Data.Common/System.Data.Common.DbDataReader::GetProviderSpecificFieldType:System.Type(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetProviderSpecificFieldType", True, "benchmark" },
    { 267, "System.Data.Common/System.Data.Common.DbDataReader::GetProviderSpecificValue:System.Object(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetProviderSpecificValue", True, "benchmark" },
    { 268, "System.Data.Common/System.Data.Common.DbDataReader::GetProviderSpecificValue:System.Object(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetProviderSpecificValue", True, "benchmark" },
    { 269, "System.Data.Common/System.Data.Common.DbDataReader::GetProviderSpecificValues:System.Int32(System.Object[])", "codegen", "System.Data.Common.DbDataReader", "GetProviderSpecificValues", True, "benchmark" },
    { 270, "System.Data.Common/System.Data.Common.DbDataReader::GetProviderSpecificValues:System.Int32(System.Object[])", "codegen", "System.Data.Common.DbDataReader", "GetProviderSpecificValues", True, "benchmark" },
    { 271, "System.Data.Common/System.Data.Common.DbDataReader::GetProviderSpecificValues:System.Int32(System.Object[])", "codegen", "System.Data.Common.DbDataReader", "GetProviderSpecificValues", True, "benchmark" },
    { 272, "System.Data.Common/System.Data.Common.DbDataReader::GetString:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetString", True, "benchmark" },
    { 273, "System.Data.Common/System.Data.Common.DbDataReader::GetString:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetString", True, "benchmark" },
    { 274, "System.Data.Common/System.Data.Common.DbDataReader::GetStream:System.IO.Stream(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetStream", True, "benchmark" },
    { 275, "System.Data.Common/System.Data.Common.DbDataReader::GetStream:System.IO.Stream(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetStream", True, "benchmark" },
    { 276, "System.Data.Common/System.Data.Common.DbDataReader::GetTextReader:System.IO.TextReader(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetTextReader", True, "benchmark" },
    { 277, "System.Data.Common/System.Data.Common.DbDataReader::GetTextReader:System.IO.TextReader(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetTextReader", True, "benchmark" },
    { 278, "System.Data.Common/System.Data.Common.DbDataReader::GetValue:System.Object(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetValue", True, "benchmark" },
    { 279, "System.Data.Common/System.Data.Common.DbDataReader::GetValue:System.Object(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetValue", True, "benchmark" },
    { 280, "System.Data.Common/System.Data.Common.DbDataReader::GetFieldValue:System.Int32(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetFieldValue", True, "benchmark" },
    { 281, "System.Data.Common/System.Data.Common.DbDataReader::GetFieldValue:System.Int32(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetFieldValue", True, "benchmark" },
    { 282, "System.Data.Common/System.Data.Common.DbDataReader::GetFieldValueAsync:System.Threading.Tasks.Task<System.Int32>(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetFieldValueAsync", True, "benchmark" },
    { 283, "System.Data.Common/System.Data.Common.DbDataReader::GetFieldValueAsync:System.Threading.Tasks.Task<System.Int32>(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "GetFieldValueAsync", True, "benchmark" },
    { 284, "System.Data.Common/System.Data.Common.DbDataReader::GetFieldValueAsync:System.Threading.Tasks.Task<System.Int32>(System.Int32,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbDataReader", "GetFieldValueAsync", True, "benchmark" },
    { 285, "System.Data.Common/System.Data.Common.DbDataReader::GetFieldValueAsync:System.Threading.Tasks.Task<System.Int32>(System.Int32,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbDataReader", "GetFieldValueAsync", True, "benchmark" },
    { 286, "System.Data.Common/System.Data.Common.DbDataReader::GetValues:System.Int32(System.Object[])", "codegen", "System.Data.Common.DbDataReader", "GetValues", True, "benchmark" },
    { 287, "System.Data.Common/System.Data.Common.DbDataReader::GetValues:System.Int32(System.Object[])", "codegen", "System.Data.Common.DbDataReader", "GetValues", True, "benchmark" },
    { 288, "System.Data.Common/System.Data.Common.DbDataReader::GetValues:System.Int32(System.Object[])", "codegen", "System.Data.Common.DbDataReader", "GetValues", True, "benchmark" },
    { 289, "System.Data.Common/System.Data.Common.DbDataReader::IsDBNull:System.Boolean(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "IsDBNull", True, "benchmark" },
    { 290, "System.Data.Common/System.Data.Common.DbDataReader::IsDBNull:System.Boolean(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "IsDBNull", True, "benchmark" },
    { 291, "System.Data.Common/System.Data.Common.DbDataReader::IsDBNullAsync:System.Threading.Tasks.Task<System.Boolean>(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "IsDBNullAsync", True, "benchmark" },
    { 292, "System.Data.Common/System.Data.Common.DbDataReader::IsDBNullAsync:System.Threading.Tasks.Task<System.Boolean>(System.Int32)", "codegen", "System.Data.Common.DbDataReader", "IsDBNullAsync", True, "benchmark" },
    { 293, "System.Data.Common/System.Data.Common.DbDataReader::IsDBNullAsync:System.Threading.Tasks.Task<System.Boolean>(System.Int32,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbDataReader", "IsDBNullAsync", True, "benchmark" },
    { 294, "System.Data.Common/System.Data.Common.DbDataReader::IsDBNullAsync:System.Threading.Tasks.Task<System.Boolean>(System.Int32,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbDataReader", "IsDBNullAsync", True, "benchmark" },
    { 295, "System.Data.Common/System.Data.Common.DbDataReader::NextResult:System.Boolean()", "codegen", "System.Data.Common.DbDataReader", "NextResult", True, "benchmark" },
    { 296, "System.Data.Common/System.Data.Common.DbDataReader::Read:System.Boolean()", "codegen", "System.Data.Common.DbDataReader", "Read", True, "benchmark" },
    { 297, "System.Data.Common/System.Data.Common.DbDataReader::ReadAsync:System.Threading.Tasks.Task<System.Boolean>()", "codegen", "System.Data.Common.DbDataReader", "ReadAsync", True, "benchmark" },
    { 298, "System.Data.Common/System.Data.Common.DbDataReader::ReadAsync:System.Threading.Tasks.Task<System.Boolean>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbDataReader", "ReadAsync", True, "benchmark" },
    { 299, "System.Data.Common/System.Data.Common.DbDataReader::NextResultAsync:System.Threading.Tasks.Task<System.Boolean>()", "codegen", "System.Data.Common.DbDataReader", "NextResultAsync", True, "benchmark" },
    { 300, "System.Data.Common/System.Data.Common.DbDataReader::NextResultAsync:System.Threading.Tasks.Task<System.Boolean>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbDataReader", "NextResultAsync", True, "benchmark" },
    { 301, "System.Data.Common/System.Data.Common.DbDataReaderExtensions::GetColumnSchema:System.Collections.ObjectModel.ReadOnlyCollection<System.Data.Common.DbColumn>(System.Data.Common.DbDataReader)", "codegen", "System.Data.Common.DbDataReaderExtensions", "GetColumnSchema", True, "benchmark" },
    { 302, "System.Data.Common/System.Data.Common.DbDataReaderExtensions::CanGetColumnSchema:System.Boolean(System.Data.Common.DbDataReader)", "codegen", "System.Data.Common.DbDataReaderExtensions", "CanGetColumnSchema", True, "benchmark" },
    { 303, "System.Data.Common/System.Data.Common.DbDataRecord::GetBoolean:System.Boolean(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetBoolean", True, "benchmark" },
    { 304, "System.Data.Common/System.Data.Common.DbDataRecord::GetBoolean:System.Boolean(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetBoolean", True, "benchmark" },
    { 305, "System.Data.Common/System.Data.Common.DbDataRecord::GetByte:System.Byte(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetByte", True, "benchmark" },
    { 306, "System.Data.Common/System.Data.Common.DbDataRecord::GetByte:System.Byte(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetByte", True, "benchmark" },
    { 307, "System.Data.Common/System.Data.Common.DbDataRecord::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetBytes", True, "benchmark" },
    { 308, "System.Data.Common/System.Data.Common.DbDataRecord::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetBytes", True, "benchmark" },
    { 309, "System.Data.Common/System.Data.Common.DbDataRecord::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetBytes", True, "benchmark" },
    { 310, "System.Data.Common/System.Data.Common.DbDataRecord::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetBytes", True, "benchmark" },
    { 311, "System.Data.Common/System.Data.Common.DbDataRecord::GetBytes:System.Int64(System.Int32,System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetBytes", True, "benchmark" },
    { 312, "System.Data.Common/System.Data.Common.DbDataRecord::GetChar:System.Char(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetChar", True, "benchmark" },
    { 313, "System.Data.Common/System.Data.Common.DbDataRecord::GetChar:System.Char(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetChar", True, "benchmark" },
    { 314, "System.Data.Common/System.Data.Common.DbDataRecord::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetChars", True, "benchmark" },
    { 315, "System.Data.Common/System.Data.Common.DbDataRecord::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetChars", True, "benchmark" },
    { 316, "System.Data.Common/System.Data.Common.DbDataRecord::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetChars", True, "benchmark" },
    { 317, "System.Data.Common/System.Data.Common.DbDataRecord::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetChars", True, "benchmark" },
    { 318, "System.Data.Common/System.Data.Common.DbDataRecord::GetChars:System.Int64(System.Int32,System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetChars", True, "benchmark" },
    { 319, "System.Data.Common/System.Data.Common.DbDataRecord::GetData:System.Data.IDataReader(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetData", True, "benchmark" },
    { 320, "System.Data.Common/System.Data.Common.DbDataRecord::GetData:System.Data.IDataReader(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetData", True, "benchmark" },
    { 321, "System.Data.Common/System.Data.Common.DbDataRecord::GetDataTypeName:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetDataTypeName", True, "benchmark" },
    { 322, "System.Data.Common/System.Data.Common.DbDataRecord::GetDataTypeName:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetDataTypeName", True, "benchmark" },
    { 323, "System.Data.Common/System.Data.Common.DbDataRecord::GetDateTime:System.DateTime(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetDateTime", True, "benchmark" },
    { 324, "System.Data.Common/System.Data.Common.DbDataRecord::GetDateTime:System.DateTime(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetDateTime", True, "benchmark" },
    { 325, "System.Data.Common/System.Data.Common.DbDataRecord::GetDecimal:System.Decimal(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetDecimal", True, "benchmark" },
    { 326, "System.Data.Common/System.Data.Common.DbDataRecord::GetDecimal:System.Decimal(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetDecimal", True, "benchmark" },
    { 327, "System.Data.Common/System.Data.Common.DbDataRecord::GetDouble:System.Double(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetDouble", True, "benchmark" },
    { 328, "System.Data.Common/System.Data.Common.DbDataRecord::GetDouble:System.Double(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetDouble", True, "benchmark" },
    { 329, "System.Data.Common/System.Data.Common.DbDataRecord::GetFieldType:System.Type(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetFieldType", True, "benchmark" },
    { 330, "System.Data.Common/System.Data.Common.DbDataRecord::GetFieldType:System.Type(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetFieldType", True, "benchmark" },
    { 331, "System.Data.Common/System.Data.Common.DbDataRecord::GetFloat:System.Single(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetFloat", True, "benchmark" },
    { 332, "System.Data.Common/System.Data.Common.DbDataRecord::GetFloat:System.Single(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetFloat", True, "benchmark" },
    { 333, "System.Data.Common/System.Data.Common.DbDataRecord::GetGuid:System.Guid(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetGuid", True, "benchmark" },
    { 334, "System.Data.Common/System.Data.Common.DbDataRecord::GetGuid:System.Guid(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetGuid", True, "benchmark" },
    { 335, "System.Data.Common/System.Data.Common.DbDataRecord::GetInt16:System.Int16(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetInt16", True, "benchmark" },
    { 336, "System.Data.Common/System.Data.Common.DbDataRecord::GetInt16:System.Int16(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetInt16", True, "benchmark" },
    { 337, "System.Data.Common/System.Data.Common.DbDataRecord::GetInt32:System.Int32(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetInt32", True, "benchmark" },
    { 338, "System.Data.Common/System.Data.Common.DbDataRecord::GetInt32:System.Int32(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetInt32", True, "benchmark" },
    { 339, "System.Data.Common/System.Data.Common.DbDataRecord::GetInt64:System.Int64(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetInt64", True, "benchmark" },
    { 340, "System.Data.Common/System.Data.Common.DbDataRecord::GetInt64:System.Int64(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetInt64", True, "benchmark" },
    { 341, "System.Data.Common/System.Data.Common.DbDataRecord::GetName:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetName", True, "benchmark" },
    { 342, "System.Data.Common/System.Data.Common.DbDataRecord::GetName:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetName", True, "benchmark" },
    { 343, "System.Data.Common/System.Data.Common.DbDataRecord::GetOrdinal:System.Int32(System.String)", "codegen", "System.Data.Common.DbDataRecord", "GetOrdinal", True, "benchmark" },
    { 344, "System.Data.Common/System.Data.Common.DbDataRecord::GetOrdinal:System.Int32(System.String)", "codegen", "System.Data.Common.DbDataRecord", "GetOrdinal", True, "benchmark" },
    { 345, "System.Data.Common/System.Data.Common.DbDataRecord::GetString:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetString", True, "benchmark" },
    { 346, "System.Data.Common/System.Data.Common.DbDataRecord::GetString:System.String(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetString", True, "benchmark" },
    { 347, "System.Data.Common/System.Data.Common.DbDataRecord::GetValue:System.Object(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetValue", True, "benchmark" },
    { 348, "System.Data.Common/System.Data.Common.DbDataRecord::GetValue:System.Object(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "GetValue", True, "benchmark" },
    { 349, "System.Data.Common/System.Data.Common.DbDataRecord::GetValues:System.Int32(System.Object[])", "codegen", "System.Data.Common.DbDataRecord", "GetValues", True, "benchmark" },
    { 350, "System.Data.Common/System.Data.Common.DbDataRecord::GetValues:System.Int32(System.Object[])", "codegen", "System.Data.Common.DbDataRecord", "GetValues", True, "benchmark" },
    { 351, "System.Data.Common/System.Data.Common.DbDataRecord::GetValues:System.Int32(System.Object[])", "codegen", "System.Data.Common.DbDataRecord", "GetValues", True, "benchmark" },
    { 352, "System.Data.Common/System.Data.Common.DbDataRecord::IsDBNull:System.Boolean(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "IsDBNull", True, "benchmark" },
    { 353, "System.Data.Common/System.Data.Common.DbDataRecord::IsDBNull:System.Boolean(System.Int32)", "codegen", "System.Data.Common.DbDataRecord", "IsDBNull", True, "benchmark" },
    { 354, "System.Data.Common/System.Data.Common.DbDataSource::CreateConnection:System.Data.Common.DbConnection()", "codegen", "System.Data.Common.DbDataSource", "CreateConnection", True, "benchmark" },
    { 355, "System.Data.Common/System.Data.Common.DbDataSource::OpenConnection:System.Data.Common.DbConnection()", "codegen", "System.Data.Common.DbDataSource", "OpenConnection", True, "benchmark" },
    { 356, "System.Data.Common/System.Data.Common.DbDataSource::OpenConnectionAsync:System.Threading.Tasks.ValueTask<System.Data.Common.DbConnection>(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbDataSource", "OpenConnectionAsync", True, "benchmark" },
    { 357, "System.Data.Common/System.Data.Common.DbDataSource::CreateCommand:System.Data.Common.DbCommand(System.String)", "codegen", "System.Data.Common.DbDataSource", "CreateCommand", True, "benchmark" },
    { 358, "System.Data.Common/System.Data.Common.DbDataSource::CreateCommand:System.Data.Common.DbCommand(System.String)", "codegen", "System.Data.Common.DbDataSource", "CreateCommand", True, "benchmark" },
    { 359, "System.Data.Common/System.Data.Common.DbDataSource::CreateBatch:System.Data.Common.DbBatch()", "codegen", "System.Data.Common.DbDataSource", "CreateBatch", True, "benchmark" },
    { 360, "System.Data.Common/System.Data.Common.DbDataSource::Dispose:System.Void()", "codegen", "System.Data.Common.DbDataSource", "Dispose", True, "benchmark" },
    { 361, "System.Data.Common/System.Data.Common.DbDataSource::DisposeAsync:System.Threading.Tasks.ValueTask()", "codegen", "System.Data.Common.DbDataSource", "DisposeAsync", True, "benchmark" },
    { 362, "System.Data.Common/System.Data.Common.DbDataSourceEnumerator::GetDataSources:System.Data.DataTable()", "codegen", "System.Data.Common.DbDataSourceEnumerator", "GetDataSources", True, "benchmark" },
    { 363, "System.Data.Common/System.Data.Common.DbParameter::ResetDbType:System.Void()", "codegen", "System.Data.Common.DbParameter", "ResetDbType", True, "benchmark" },
    { 364, "System.Data.Common/System.Data.Common.DbParameterCollection::Add:System.Int32(System.Object)", "codegen", "System.Data.Common.DbParameterCollection", "Add", True, "benchmark" },
    { 365, "System.Data.Common/System.Data.Common.DbParameterCollection::AddRange:System.Void(System.Array)", "codegen", "System.Data.Common.DbParameterCollection", "AddRange", True, "benchmark" },
    { 366, "System.Data.Common/System.Data.Common.DbParameterCollection::Contains:System.Boolean(System.Object)", "codegen", "System.Data.Common.DbParameterCollection", "Contains", True, "benchmark" },
    { 367, "System.Data.Common/System.Data.Common.DbParameterCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.Common.DbParameterCollection", "Contains", True, "benchmark" },
    { 368, "System.Data.Common/System.Data.Common.DbParameterCollection::Contains:System.Boolean(System.String)", "codegen", "System.Data.Common.DbParameterCollection", "Contains", True, "benchmark" },
    { 369, "System.Data.Common/System.Data.Common.DbParameterCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.Common.DbParameterCollection", "CopyTo", True, "benchmark" },
    { 370, "System.Data.Common/System.Data.Common.DbParameterCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.Data.Common.DbParameterCollection", "CopyTo", True, "benchmark" },
    { 371, "System.Data.Common/System.Data.Common.DbParameterCollection::Clear:System.Void()", "codegen", "System.Data.Common.DbParameterCollection", "Clear", True, "benchmark" },
    { 372, "System.Data.Common/System.Data.Common.DbParameterCollection::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Data.Common.DbParameterCollection", "GetEnumerator", True, "benchmark" },
    { 373, "System.Data.Common/System.Data.Common.DbParameterCollection::IndexOf:System.Int32(System.Object)", "codegen", "System.Data.Common.DbParameterCollection", "IndexOf", True, "benchmark" },
    { 374, "System.Data.Common/System.Data.Common.DbParameterCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.Common.DbParameterCollection", "IndexOf", True, "benchmark" },
    { 375, "System.Data.Common/System.Data.Common.DbParameterCollection::IndexOf:System.Int32(System.String)", "codegen", "System.Data.Common.DbParameterCollection", "IndexOf", True, "benchmark" },
    { 376, "System.Data.Common/System.Data.Common.DbParameterCollection::Insert:System.Void(System.Int32,System.Object)", "codegen", "System.Data.Common.DbParameterCollection", "Insert", True, "benchmark" },
    { 377, "System.Data.Common/System.Data.Common.DbParameterCollection::Insert:System.Void(System.Int32,System.Object)", "codegen", "System.Data.Common.DbParameterCollection", "Insert", True, "benchmark" },
    { 378, "System.Data.Common/System.Data.Common.DbParameterCollection::Remove:System.Void(System.Object)", "codegen", "System.Data.Common.DbParameterCollection", "Remove", True, "benchmark" },
    { 379, "System.Data.Common/System.Data.Common.DbParameterCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.Common.DbParameterCollection", "RemoveAt", True, "benchmark" },
    { 380, "System.Data.Common/System.Data.Common.DbParameterCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.Data.Common.DbParameterCollection", "RemoveAt", True, "benchmark" },
    { 381, "System.Data.Common/System.Data.Common.DbParameterCollection::RemoveAt:System.Void(System.String)", "codegen", "System.Data.Common.DbParameterCollection", "RemoveAt", True, "benchmark" },
    { 382, "System.Data.Common/System.Data.Common.DbParameterCollection::RemoveAt:System.Void(System.String)", "codegen", "System.Data.Common.DbParameterCollection", "RemoveAt", True, "benchmark" },
    { 383, "System.Data.Common/System.Data.Common.DbProviderFactories::TryGetFactory:System.Boolean(System.String,System.Data.Common.DbProviderFactory)", "codegen", "System.Data.Common.DbProviderFactories", "TryGetFactory", True, "benchmark" },
    { 384, "System.Data.Common/System.Data.Common.DbProviderFactories::TryGetFactory:System.Boolean(System.String,System.Data.Common.DbProviderFactory)", "codegen", "System.Data.Common.DbProviderFactories", "TryGetFactory", True, "benchmark" },
    { 385, "System.Data.Common/System.Data.Common.DbProviderFactories::GetFactory:System.Data.Common.DbProviderFactory(System.String)", "codegen", "System.Data.Common.DbProviderFactories", "GetFactory", True, "benchmark" },
    { 386, "System.Data.Common/System.Data.Common.DbProviderFactories::GetFactory:System.Data.Common.DbProviderFactory(System.String)", "codegen", "System.Data.Common.DbProviderFactories", "GetFactory", True, "benchmark" },
    { 387, "System.Data.Common/System.Data.Common.DbProviderFactories::GetFactory:System.Data.Common.DbProviderFactory(System.Data.DataRow)", "codegen", "System.Data.Common.DbProviderFactories", "GetFactory", True, "benchmark" },
    { 388, "System.Data.Common/System.Data.Common.DbProviderFactories::GetFactory:System.Data.Common.DbProviderFactory(System.Data.Common.DbConnection)", "codegen", "System.Data.Common.DbProviderFactories", "GetFactory", True, "benchmark" },
    { 389, "System.Data.Common/System.Data.Common.DbProviderFactories::GetFactoryClasses:System.Data.DataTable()", "codegen", "System.Data.Common.DbProviderFactories", "GetFactoryClasses", True, "benchmark" },
    { 390, "System.Data.Common/System.Data.Common.DbProviderFactories::GetProviderInvariantNames:System.Collections.Generic.IEnumerable<System.String>()", "codegen", "System.Data.Common.DbProviderFactories", "GetProviderInvariantNames", True, "benchmark" },
    { 391, "System.Data.Common/System.Data.Common.DbProviderFactories::RegisterFactory:System.Void(System.String,System.String)", "codegen", "System.Data.Common.DbProviderFactories", "RegisterFactory", True, "benchmark" },
    { 392, "System.Data.Common/System.Data.Common.DbProviderFactories::RegisterFactory:System.Void(System.String,System.String)", "codegen", "System.Data.Common.DbProviderFactories", "RegisterFactory", True, "benchmark" },
    { 393, "System.Data.Common/System.Data.Common.DbProviderFactories::RegisterFactory:System.Void(System.String,System.String)", "codegen", "System.Data.Common.DbProviderFactories", "RegisterFactory", True, "benchmark" },
    { 394, "System.Data.Common/System.Data.Common.DbProviderFactories::RegisterFactory:System.Void(System.String,System.Type)", "codegen", "System.Data.Common.DbProviderFactories", "RegisterFactory", True, "benchmark" },
    { 395, "System.Data.Common/System.Data.Common.DbProviderFactories::RegisterFactory:System.Void(System.String,System.Type)", "codegen", "System.Data.Common.DbProviderFactories", "RegisterFactory", True, "benchmark" },
    { 396, "System.Data.Common/System.Data.Common.DbProviderFactories::RegisterFactory:System.Void(System.String,System.Data.Common.DbProviderFactory)", "codegen", "System.Data.Common.DbProviderFactories", "RegisterFactory", True, "benchmark" },
    { 397, "System.Data.Common/System.Data.Common.DbProviderFactories::RegisterFactory:System.Void(System.String,System.Data.Common.DbProviderFactory)", "codegen", "System.Data.Common.DbProviderFactories", "RegisterFactory", True, "benchmark" },
    { 398, "System.Data.Common/System.Data.Common.DbProviderFactories::UnregisterFactory:System.Boolean(System.String)", "codegen", "System.Data.Common.DbProviderFactories", "UnregisterFactory", True, "benchmark" },
    { 399, "System.Data.Common/System.Data.Common.DbProviderFactories::UnregisterFactory:System.Boolean(System.String)", "codegen", "System.Data.Common.DbProviderFactories", "UnregisterFactory", True, "benchmark" },
    { 400, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateBatch:System.Data.Common.DbBatch()", "codegen", "System.Data.Common.DbProviderFactory", "CreateBatch", True, "benchmark" },
    { 401, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateBatchCommand:System.Data.Common.DbBatchCommand()", "codegen", "System.Data.Common.DbProviderFactory", "CreateBatchCommand", True, "benchmark" },
    { 402, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateCommand:System.Data.Common.DbCommand()", "codegen", "System.Data.Common.DbProviderFactory", "CreateCommand", True, "benchmark" },
    { 403, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateCommandBuilder:System.Data.Common.DbCommandBuilder()", "codegen", "System.Data.Common.DbProviderFactory", "CreateCommandBuilder", True, "benchmark" },
    { 404, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateConnection:System.Data.Common.DbConnection()", "codegen", "System.Data.Common.DbProviderFactory", "CreateConnection", True, "benchmark" },
    { 405, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateConnectionStringBuilder:System.Data.Common.DbConnectionStringBuilder()", "codegen", "System.Data.Common.DbProviderFactory", "CreateConnectionStringBuilder", True, "benchmark" },
    { 406, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateDataAdapter:System.Data.Common.DbDataAdapter()", "codegen", "System.Data.Common.DbProviderFactory", "CreateDataAdapter", True, "benchmark" },
    { 407, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateParameter:System.Data.Common.DbParameter()", "codegen", "System.Data.Common.DbProviderFactory", "CreateParameter", True, "benchmark" },
    { 408, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateDataSourceEnumerator:System.Data.Common.DbDataSourceEnumerator()", "codegen", "System.Data.Common.DbProviderFactory", "CreateDataSourceEnumerator", True, "benchmark" },
    { 409, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateDataSource:System.Data.Common.DbDataSource(System.String)", "codegen", "System.Data.Common.DbProviderFactory", "CreateDataSource", True, "benchmark" },
    { 410, "System.Data.Common/System.Data.Common.DbProviderFactory::CreateDataSource:System.Data.Common.DbDataSource(System.String)", "codegen", "System.Data.Common.DbProviderFactory", "CreateDataSource", True, "benchmark" },
    { 411, "System.Data.Common/System.Data.Common.DbTransaction::Commit:System.Void()", "codegen", "System.Data.Common.DbTransaction", "Commit", True, "benchmark" },
    { 412, "System.Data.Common/System.Data.Common.DbTransaction::CommitAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbTransaction", "CommitAsync", True, "benchmark" },
    { 413, "System.Data.Common/System.Data.Common.DbTransaction::Dispose:System.Void()", "codegen", "System.Data.Common.DbTransaction", "Dispose", True, "benchmark" },
    { 414, "System.Data.Common/System.Data.Common.DbTransaction::DisposeAsync:System.Threading.Tasks.ValueTask()", "codegen", "System.Data.Common.DbTransaction", "DisposeAsync", True, "benchmark" },
    { 415, "System.Data.Common/System.Data.Common.DbTransaction::Rollback:System.Void()", "codegen", "System.Data.Common.DbTransaction", "Rollback", True, "benchmark" },
    { 416, "System.Data.Common/System.Data.Common.DbTransaction::RollbackAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbTransaction", "RollbackAsync", True, "benchmark" },
    { 417, "System.Data.Common/System.Data.Common.DbTransaction::SaveAsync:System.Threading.Tasks.Task(System.String,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbTransaction", "SaveAsync", True, "benchmark" },
    { 418, "System.Data.Common/System.Data.Common.DbTransaction::SaveAsync:System.Threading.Tasks.Task(System.String,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbTransaction", "SaveAsync", True, "benchmark" },
    { 419, "System.Data.Common/System.Data.Common.DbTransaction::RollbackAsync:System.Threading.Tasks.Task(System.String,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbTransaction", "RollbackAsync", True, "benchmark" },
    { 420, "System.Data.Common/System.Data.Common.DbTransaction::RollbackAsync:System.Threading.Tasks.Task(System.String,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbTransaction", "RollbackAsync", True, "benchmark" },
    { 421, "System.Data.Common/System.Data.Common.DbTransaction::ReleaseAsync:System.Threading.Tasks.Task(System.String,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbTransaction", "ReleaseAsync", True, "benchmark" },
    { 422, "System.Data.Common/System.Data.Common.DbTransaction::ReleaseAsync:System.Threading.Tasks.Task(System.String,System.Threading.CancellationToken)", "codegen", "System.Data.Common.DbTransaction", "ReleaseAsync", True, "benchmark" },
    { 423, "System.Data.Common/System.Data.Common.DbTransaction::Save:System.Void(System.String)", "codegen", "System.Data.Common.DbTransaction", "Save", True, "benchmark" },
    { 424, "System.Data.Common/System.Data.Common.DbTransaction::Save:System.Void(System.String)", "codegen", "System.Data.Common.DbTransaction", "Save", True, "benchmark" },
    { 425, "System.Data.Common/System.Data.Common.DbTransaction::Rollback:System.Void(System.String)", "codegen", "System.Data.Common.DbTransaction", "Rollback", True, "benchmark" },
    { 426, "System.Data.Common/System.Data.Common.DbTransaction::Rollback:System.Void(System.String)", "codegen", "System.Data.Common.DbTransaction", "Rollback", True, "benchmark" },
    { 427, "System.Data.Common/System.Data.Common.DbTransaction::Release:System.Void(System.String)", "codegen", "System.Data.Common.DbTransaction", "Release", True, "benchmark" },
    { 428, "System.Data.Common/System.Data.Common.DbTransaction::Release:System.Void(System.String)", "codegen", "System.Data.Common.DbTransaction", "Release", True, "benchmark" },
    { 429, "System.Data.Common/System.Data.Common.RowUpdatedEventArgs::CopyToRows:System.Void(System.Data.DataRow[])", "codegen", "System.Data.Common.RowUpdatedEventArgs", "CopyToRows", True, "benchmark" },
    { 430, "System.Data.Common/System.Data.Common.RowUpdatedEventArgs::CopyToRows:System.Void(System.Data.DataRow[])", "codegen", "System.Data.Common.RowUpdatedEventArgs", "CopyToRows", True, "benchmark" },
    { 431, "System.Data.Common/System.Data.Common.RowUpdatedEventArgs::CopyToRows:System.Void(System.Data.DataRow[])", "codegen", "System.Data.Common.RowUpdatedEventArgs", "CopyToRows", True, "benchmark" },
    { 432, "System.Data.Common/System.Data.Common.RowUpdatedEventArgs::CopyToRows:System.Void(System.Data.DataRow[],System.Int32)", "codegen", "System.Data.Common.RowUpdatedEventArgs", "CopyToRows", True, "benchmark" },
    { 433, "System.Data.Common/System.Data.Common.RowUpdatedEventArgs::CopyToRows:System.Void(System.Data.DataRow[],System.Int32)", "codegen", "System.Data.Common.RowUpdatedEventArgs", "CopyToRows", True, "benchmark" },
    { 434, "System.Data.Common/System.Data.Common.RowUpdatedEventArgs::CopyToRows:System.Void(System.Data.DataRow[],System.Int32)", "codegen", "System.Data.Common.RowUpdatedEventArgs", "CopyToRows", True, "benchmark" },
    { 435, "System.Data.Common/System.Data.Common.RowUpdatedEventArgs::CopyToRows:System.Void(System.Data.DataRow[],System.Int32)", "codegen", "System.Data.Common.RowUpdatedEventArgs", "CopyToRows", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 436;

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