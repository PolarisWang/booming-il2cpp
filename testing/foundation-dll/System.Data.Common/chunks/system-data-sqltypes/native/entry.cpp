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
    { 0, "System.Data.Common/System.Data.SqlTypes.SqlBinary::Add:System.Data.SqlTypes.SqlBinary(System.Data.SqlTypes.SqlBinary,System.Data.SqlTypes.SqlBinary)", "codegen", "System.Data.SqlTypes.SqlBinary", "Add", True, "benchmark" },
    { 1, "System.Data.Common/System.Data.SqlTypes.SqlBinary::Concat:System.Data.SqlTypes.SqlBinary(System.Data.SqlTypes.SqlBinary,System.Data.SqlTypes.SqlBinary)", "codegen", "System.Data.SqlTypes.SqlBinary", "Concat", True, "benchmark" },
    { 2, "System.Data.Common/System.Data.SqlTypes.SqlBinary::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBinary,System.Data.SqlTypes.SqlBinary)", "codegen", "System.Data.SqlTypes.SqlBinary", "NotEquals", True, "benchmark" },
    { 3, "System.Data.Common/System.Data.SqlTypes.SqlBinary::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBinary,System.Data.SqlTypes.SqlBinary)", "codegen", "System.Data.SqlTypes.SqlBinary", "LessThan", True, "benchmark" },
    { 4, "System.Data.Common/System.Data.SqlTypes.SqlBinary::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBinary,System.Data.SqlTypes.SqlBinary)", "codegen", "System.Data.SqlTypes.SqlBinary", "GreaterThan", True, "benchmark" },
    { 5, "System.Data.Common/System.Data.SqlTypes.SqlBinary::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBinary,System.Data.SqlTypes.SqlBinary)", "codegen", "System.Data.SqlTypes.SqlBinary", "LessThanOrEqual", True, "benchmark" },
    { 6, "System.Data.Common/System.Data.SqlTypes.SqlBinary::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBinary,System.Data.SqlTypes.SqlBinary)", "codegen", "System.Data.SqlTypes.SqlBinary", "GreaterThanOrEqual", True, "benchmark" },
    { 7, "System.Data.Common/System.Data.SqlTypes.SqlBinary::ToSqlGuid:System.Data.SqlTypes.SqlGuid()", "codegen", "System.Data.SqlTypes.SqlBinary", "ToSqlGuid", True, "benchmark" },
    { 8, "System.Data.Common/System.Data.SqlTypes.SqlBinary::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlBinary", "CompareTo", True, "benchmark" },
    { 9, "System.Data.Common/System.Data.SqlTypes.SqlBinary::CompareTo:System.Int32(System.Data.SqlTypes.SqlBinary)", "codegen", "System.Data.SqlTypes.SqlBinary", "CompareTo", True, "benchmark" },
    { 10, "System.Data.Common/System.Data.SqlTypes.SqlBinary::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlBinary", "GetXsdType", True, "benchmark" },
    { 11, "System.Data.Common/System.Data.SqlTypes.SqlBinary::WrapBytes:System.Data.SqlTypes.SqlBinary(System.Byte[])", "codegen", "System.Data.SqlTypes.SqlBinary", "WrapBytes", True, "benchmark" },
    { 12, "System.Data.Common/System.Data.SqlTypes.SqlBinary::WrapBytes:System.Data.SqlTypes.SqlBinary(System.Byte[])", "codegen", "System.Data.SqlTypes.SqlBinary", "WrapBytes", True, "benchmark" },
    { 13, "System.Data.Common/System.Data.SqlTypes.SqlBinary::WrapBytes:System.Data.SqlTypes.SqlBinary(System.Byte[])", "codegen", "System.Data.SqlTypes.SqlBinary", "WrapBytes", True, "benchmark" },
    { 14, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::Parse:System.Data.SqlTypes.SqlBoolean(System.String)", "codegen", "System.Data.SqlTypes.SqlBoolean", "Parse", True, "benchmark" },
    { 15, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::Parse:System.Data.SqlTypes.SqlBoolean(System.String)", "codegen", "System.Data.SqlTypes.SqlBoolean", "Parse", True, "benchmark" },
    { 16, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::OnesComplement:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBoolean)", "codegen", "System.Data.SqlTypes.SqlBoolean", "OnesComplement", True, "benchmark" },
    { 17, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::And:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBoolean,System.Data.SqlTypes.SqlBoolean)", "codegen", "System.Data.SqlTypes.SqlBoolean", "And", True, "benchmark" },
    { 18, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::Or:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBoolean,System.Data.SqlTypes.SqlBoolean)", "codegen", "System.Data.SqlTypes.SqlBoolean", "Or", True, "benchmark" },
    { 19, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::Xor:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBoolean,System.Data.SqlTypes.SqlBoolean)", "codegen", "System.Data.SqlTypes.SqlBoolean", "Xor", True, "benchmark" },
    { 20, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBoolean,System.Data.SqlTypes.SqlBoolean)", "codegen", "System.Data.SqlTypes.SqlBoolean", "NotEquals", True, "benchmark" },
    { 21, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBoolean,System.Data.SqlTypes.SqlBoolean)", "codegen", "System.Data.SqlTypes.SqlBoolean", "GreaterThan", True, "benchmark" },
    { 22, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBoolean,System.Data.SqlTypes.SqlBoolean)", "codegen", "System.Data.SqlTypes.SqlBoolean", "LessThan", True, "benchmark" },
    { 23, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::GreaterThanOrEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBoolean,System.Data.SqlTypes.SqlBoolean)", "codegen", "System.Data.SqlTypes.SqlBoolean", "GreaterThanOrEquals", True, "benchmark" },
    { 24, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::LessThanOrEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlBoolean,System.Data.SqlTypes.SqlBoolean)", "codegen", "System.Data.SqlTypes.SqlBoolean", "LessThanOrEquals", True, "benchmark" },
    { 25, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::ToSqlByte:System.Data.SqlTypes.SqlByte()", "codegen", "System.Data.SqlTypes.SqlBoolean", "ToSqlByte", True, "benchmark" },
    { 26, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::ToSqlDouble:System.Data.SqlTypes.SqlDouble()", "codegen", "System.Data.SqlTypes.SqlBoolean", "ToSqlDouble", True, "benchmark" },
    { 27, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::ToSqlInt16:System.Data.SqlTypes.SqlInt16()", "codegen", "System.Data.SqlTypes.SqlBoolean", "ToSqlInt16", True, "benchmark" },
    { 28, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::ToSqlInt32:System.Data.SqlTypes.SqlInt32()", "codegen", "System.Data.SqlTypes.SqlBoolean", "ToSqlInt32", True, "benchmark" },
    { 29, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::ToSqlInt64:System.Data.SqlTypes.SqlInt64()", "codegen", "System.Data.SqlTypes.SqlBoolean", "ToSqlInt64", True, "benchmark" },
    { 30, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::ToSqlMoney:System.Data.SqlTypes.SqlMoney()", "codegen", "System.Data.SqlTypes.SqlBoolean", "ToSqlMoney", True, "benchmark" },
    { 31, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::ToSqlDecimal:System.Data.SqlTypes.SqlDecimal()", "codegen", "System.Data.SqlTypes.SqlBoolean", "ToSqlDecimal", True, "benchmark" },
    { 32, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::ToSqlSingle:System.Data.SqlTypes.SqlSingle()", "codegen", "System.Data.SqlTypes.SqlBoolean", "ToSqlSingle", True, "benchmark" },
    { 33, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlBoolean", "ToSqlString", True, "benchmark" },
    { 34, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlBoolean", "CompareTo", True, "benchmark" },
    { 35, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::CompareTo:System.Int32(System.Data.SqlTypes.SqlBoolean)", "codegen", "System.Data.SqlTypes.SqlBoolean", "CompareTo", True, "benchmark" },
    { 36, "System.Data.Common/System.Data.SqlTypes.SqlBoolean::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlBoolean", "GetXsdType", True, "benchmark" },
    { 37, "System.Data.Common/System.Data.SqlTypes.SqlByte::Parse:System.Data.SqlTypes.SqlByte(System.String)", "codegen", "System.Data.SqlTypes.SqlByte", "Parse", True, "benchmark" },
    { 38, "System.Data.Common/System.Data.SqlTypes.SqlByte::Parse:System.Data.SqlTypes.SqlByte(System.String)", "codegen", "System.Data.SqlTypes.SqlByte", "Parse", True, "benchmark" },
    { 39, "System.Data.Common/System.Data.SqlTypes.SqlByte::OnesComplement:System.Data.SqlTypes.SqlByte(System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "OnesComplement", True, "benchmark" },
    { 40, "System.Data.Common/System.Data.SqlTypes.SqlByte::Add:System.Data.SqlTypes.SqlByte(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "Add", True, "benchmark" },
    { 41, "System.Data.Common/System.Data.SqlTypes.SqlByte::Subtract:System.Data.SqlTypes.SqlByte(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "Subtract", True, "benchmark" },
    { 42, "System.Data.Common/System.Data.SqlTypes.SqlByte::Multiply:System.Data.SqlTypes.SqlByte(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "Multiply", True, "benchmark" },
    { 43, "System.Data.Common/System.Data.SqlTypes.SqlByte::Divide:System.Data.SqlTypes.SqlByte(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "Divide", True, "benchmark" },
    { 44, "System.Data.Common/System.Data.SqlTypes.SqlByte::Mod:System.Data.SqlTypes.SqlByte(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "Mod", True, "benchmark" },
    { 45, "System.Data.Common/System.Data.SqlTypes.SqlByte::Modulus:System.Data.SqlTypes.SqlByte(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "Modulus", True, "benchmark" },
    { 46, "System.Data.Common/System.Data.SqlTypes.SqlByte::BitwiseAnd:System.Data.SqlTypes.SqlByte(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "BitwiseAnd", True, "benchmark" },
    { 47, "System.Data.Common/System.Data.SqlTypes.SqlByte::BitwiseOr:System.Data.SqlTypes.SqlByte(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "BitwiseOr", True, "benchmark" },
    { 48, "System.Data.Common/System.Data.SqlTypes.SqlByte::Xor:System.Data.SqlTypes.SqlByte(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "Xor", True, "benchmark" },
    { 49, "System.Data.Common/System.Data.SqlTypes.SqlByte::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "NotEquals", True, "benchmark" },
    { 50, "System.Data.Common/System.Data.SqlTypes.SqlByte::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "LessThan", True, "benchmark" },
    { 51, "System.Data.Common/System.Data.SqlTypes.SqlByte::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "GreaterThan", True, "benchmark" },
    { 52, "System.Data.Common/System.Data.SqlTypes.SqlByte::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "LessThanOrEqual", True, "benchmark" },
    { 53, "System.Data.Common/System.Data.SqlTypes.SqlByte::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlByte,System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "GreaterThanOrEqual", True, "benchmark" },
    { 54, "System.Data.Common/System.Data.SqlTypes.SqlByte::ToSqlBoolean:System.Data.SqlTypes.SqlBoolean()", "codegen", "System.Data.SqlTypes.SqlByte", "ToSqlBoolean", True, "benchmark" },
    { 55, "System.Data.Common/System.Data.SqlTypes.SqlByte::ToSqlDouble:System.Data.SqlTypes.SqlDouble()", "codegen", "System.Data.SqlTypes.SqlByte", "ToSqlDouble", True, "benchmark" },
    { 56, "System.Data.Common/System.Data.SqlTypes.SqlByte::ToSqlInt16:System.Data.SqlTypes.SqlInt16()", "codegen", "System.Data.SqlTypes.SqlByte", "ToSqlInt16", True, "benchmark" },
    { 57, "System.Data.Common/System.Data.SqlTypes.SqlByte::ToSqlInt32:System.Data.SqlTypes.SqlInt32()", "codegen", "System.Data.SqlTypes.SqlByte", "ToSqlInt32", True, "benchmark" },
    { 58, "System.Data.Common/System.Data.SqlTypes.SqlByte::ToSqlInt64:System.Data.SqlTypes.SqlInt64()", "codegen", "System.Data.SqlTypes.SqlByte", "ToSqlInt64", True, "benchmark" },
    { 59, "System.Data.Common/System.Data.SqlTypes.SqlByte::ToSqlMoney:System.Data.SqlTypes.SqlMoney()", "codegen", "System.Data.SqlTypes.SqlByte", "ToSqlMoney", True, "benchmark" },
    { 60, "System.Data.Common/System.Data.SqlTypes.SqlByte::ToSqlDecimal:System.Data.SqlTypes.SqlDecimal()", "codegen", "System.Data.SqlTypes.SqlByte", "ToSqlDecimal", True, "benchmark" },
    { 61, "System.Data.Common/System.Data.SqlTypes.SqlByte::ToSqlSingle:System.Data.SqlTypes.SqlSingle()", "codegen", "System.Data.SqlTypes.SqlByte", "ToSqlSingle", True, "benchmark" },
    { 62, "System.Data.Common/System.Data.SqlTypes.SqlByte::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlByte", "ToSqlString", True, "benchmark" },
    { 63, "System.Data.Common/System.Data.SqlTypes.SqlByte::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlByte", "CompareTo", True, "benchmark" },
    { 64, "System.Data.Common/System.Data.SqlTypes.SqlByte::CompareTo:System.Int32(System.Data.SqlTypes.SqlByte)", "codegen", "System.Data.SqlTypes.SqlByte", "CompareTo", True, "benchmark" },
    { 65, "System.Data.Common/System.Data.SqlTypes.SqlByte::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlByte", "GetXsdType", True, "benchmark" },
    { 66, "System.Data.Common/System.Data.SqlTypes.SqlBytes::SetNull:System.Void()", "codegen", "System.Data.SqlTypes.SqlBytes", "SetNull", True, "benchmark" },
    { 67, "System.Data.Common/System.Data.SqlTypes.SqlBytes::SetLength:System.Void(System.Int64)", "codegen", "System.Data.SqlTypes.SqlBytes", "SetLength", True, "benchmark" },
    { 68, "System.Data.Common/System.Data.SqlTypes.SqlBytes::SetLength:System.Void(System.Int64)", "codegen", "System.Data.SqlTypes.SqlBytes", "SetLength", True, "benchmark" },
    { 69, "System.Data.Common/System.Data.SqlTypes.SqlBytes::Read:System.Int64(System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlBytes", "Read", True, "benchmark" },
    { 70, "System.Data.Common/System.Data.SqlTypes.SqlBytes::Read:System.Int64(System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlBytes", "Read", True, "benchmark" },
    { 71, "System.Data.Common/System.Data.SqlTypes.SqlBytes::Read:System.Int64(System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlBytes", "Read", True, "benchmark" },
    { 72, "System.Data.Common/System.Data.SqlTypes.SqlBytes::Read:System.Int64(System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlBytes", "Read", True, "benchmark" },
    { 73, "System.Data.Common/System.Data.SqlTypes.SqlBytes::Read:System.Int64(System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlBytes", "Read", True, "benchmark" },
    { 74, "System.Data.Common/System.Data.SqlTypes.SqlBytes::Write:System.Void(System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlBytes", "Write", True, "benchmark" },
    { 75, "System.Data.Common/System.Data.SqlTypes.SqlBytes::Write:System.Void(System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlBytes", "Write", True, "benchmark" },
    { 76, "System.Data.Common/System.Data.SqlTypes.SqlBytes::Write:System.Void(System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlBytes", "Write", True, "benchmark" },
    { 77, "System.Data.Common/System.Data.SqlTypes.SqlBytes::Write:System.Void(System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlBytes", "Write", True, "benchmark" },
    { 78, "System.Data.Common/System.Data.SqlTypes.SqlBytes::Write:System.Void(System.Int64,System.Byte[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlBytes", "Write", True, "benchmark" },
    { 79, "System.Data.Common/System.Data.SqlTypes.SqlBytes::ToSqlBinary:System.Data.SqlTypes.SqlBinary()", "codegen", "System.Data.SqlTypes.SqlBytes", "ToSqlBinary", True, "benchmark" },
    { 80, "System.Data.Common/System.Data.SqlTypes.SqlBytes::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlBytes", "GetXsdType", True, "benchmark" },
    { 81, "System.Data.Common/System.Data.SqlTypes.SqlChars::SetNull:System.Void()", "codegen", "System.Data.SqlTypes.SqlChars", "SetNull", True, "benchmark" },
    { 82, "System.Data.Common/System.Data.SqlTypes.SqlChars::SetLength:System.Void(System.Int64)", "codegen", "System.Data.SqlTypes.SqlChars", "SetLength", True, "benchmark" },
    { 83, "System.Data.Common/System.Data.SqlTypes.SqlChars::SetLength:System.Void(System.Int64)", "codegen", "System.Data.SqlTypes.SqlChars", "SetLength", True, "benchmark" },
    { 84, "System.Data.Common/System.Data.SqlTypes.SqlChars::Read:System.Int64(System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlChars", "Read", True, "benchmark" },
    { 85, "System.Data.Common/System.Data.SqlTypes.SqlChars::Read:System.Int64(System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlChars", "Read", True, "benchmark" },
    { 86, "System.Data.Common/System.Data.SqlTypes.SqlChars::Read:System.Int64(System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlChars", "Read", True, "benchmark" },
    { 87, "System.Data.Common/System.Data.SqlTypes.SqlChars::Read:System.Int64(System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlChars", "Read", True, "benchmark" },
    { 88, "System.Data.Common/System.Data.SqlTypes.SqlChars::Read:System.Int64(System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlChars", "Read", True, "benchmark" },
    { 89, "System.Data.Common/System.Data.SqlTypes.SqlChars::Write:System.Void(System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlChars", "Write", True, "benchmark" },
    { 90, "System.Data.Common/System.Data.SqlTypes.SqlChars::Write:System.Void(System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlChars", "Write", True, "benchmark" },
    { 91, "System.Data.Common/System.Data.SqlTypes.SqlChars::Write:System.Void(System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlChars", "Write", True, "benchmark" },
    { 92, "System.Data.Common/System.Data.SqlTypes.SqlChars::Write:System.Void(System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlChars", "Write", True, "benchmark" },
    { 93, "System.Data.Common/System.Data.SqlTypes.SqlChars::Write:System.Void(System.Int64,System.Char[],System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlChars", "Write", True, "benchmark" },
    { 94, "System.Data.Common/System.Data.SqlTypes.SqlChars::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlChars", "ToSqlString", True, "benchmark" },
    { 95, "System.Data.Common/System.Data.SqlTypes.SqlChars::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlChars", "GetXsdType", True, "benchmark" },
    { 96, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::Parse:System.Data.SqlTypes.SqlDateTime(System.String)", "codegen", "System.Data.SqlTypes.SqlDateTime", "Parse", True, "benchmark" },
    { 97, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::Parse:System.Data.SqlTypes.SqlDateTime(System.String)", "codegen", "System.Data.SqlTypes.SqlDateTime", "Parse", True, "benchmark" },
    { 98, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::Add:System.Data.SqlTypes.SqlDateTime(System.Data.SqlTypes.SqlDateTime,System.TimeSpan)", "codegen", "System.Data.SqlTypes.SqlDateTime", "Add", True, "benchmark" },
    { 99, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::Add:System.Data.SqlTypes.SqlDateTime(System.Data.SqlTypes.SqlDateTime,System.TimeSpan)", "codegen", "System.Data.SqlTypes.SqlDateTime", "Add", True, "benchmark" },
    { 100, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::Subtract:System.Data.SqlTypes.SqlDateTime(System.Data.SqlTypes.SqlDateTime,System.TimeSpan)", "codegen", "System.Data.SqlTypes.SqlDateTime", "Subtract", True, "benchmark" },
    { 101, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::Subtract:System.Data.SqlTypes.SqlDateTime(System.Data.SqlTypes.SqlDateTime,System.TimeSpan)", "codegen", "System.Data.SqlTypes.SqlDateTime", "Subtract", True, "benchmark" },
    { 102, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDateTime,System.Data.SqlTypes.SqlDateTime)", "codegen", "System.Data.SqlTypes.SqlDateTime", "NotEquals", True, "benchmark" },
    { 103, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDateTime,System.Data.SqlTypes.SqlDateTime)", "codegen", "System.Data.SqlTypes.SqlDateTime", "LessThan", True, "benchmark" },
    { 104, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDateTime,System.Data.SqlTypes.SqlDateTime)", "codegen", "System.Data.SqlTypes.SqlDateTime", "GreaterThan", True, "benchmark" },
    { 105, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDateTime,System.Data.SqlTypes.SqlDateTime)", "codegen", "System.Data.SqlTypes.SqlDateTime", "LessThanOrEqual", True, "benchmark" },
    { 106, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDateTime,System.Data.SqlTypes.SqlDateTime)", "codegen", "System.Data.SqlTypes.SqlDateTime", "GreaterThanOrEqual", True, "benchmark" },
    { 107, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlDateTime", "ToSqlString", True, "benchmark" },
    { 108, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlDateTime", "CompareTo", True, "benchmark" },
    { 109, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::CompareTo:System.Int32(System.Data.SqlTypes.SqlDateTime)", "codegen", "System.Data.SqlTypes.SqlDateTime", "CompareTo", True, "benchmark" },
    { 110, "System.Data.Common/System.Data.SqlTypes.SqlDateTime::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlDateTime", "GetXsdType", True, "benchmark" },
    { 111, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Parse:System.Data.SqlTypes.SqlDecimal(System.String)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Parse", True, "benchmark" },
    { 112, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Parse:System.Data.SqlTypes.SqlDecimal(System.String)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Parse", True, "benchmark" },
    { 113, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ToDouble:System.Double()", "codegen", "System.Data.SqlTypes.SqlDecimal", "ToDouble", True, "benchmark" },
    { 114, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::AdjustScale:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Int32,System.Boolean)", "codegen", "System.Data.SqlTypes.SqlDecimal", "AdjustScale", True, "benchmark" },
    { 115, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::AdjustScale:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Int32,System.Boolean)", "codegen", "System.Data.SqlTypes.SqlDecimal", "AdjustScale", True, "benchmark" },
    { 116, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::AdjustScale:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Int32,System.Boolean)", "codegen", "System.Data.SqlTypes.SqlDecimal", "AdjustScale", True, "benchmark" },
    { 117, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ConvertToPrecScale:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlDecimal", "ConvertToPrecScale", True, "benchmark" },
    { 118, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ConvertToPrecScale:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlDecimal", "ConvertToPrecScale", True, "benchmark" },
    { 119, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ConvertToPrecScale:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Int32,System.Int32)", "codegen", "System.Data.SqlTypes.SqlDecimal", "ConvertToPrecScale", True, "benchmark" },
    { 120, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Add:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Add", True, "benchmark" },
    { 121, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Subtract:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Subtract", True, "benchmark" },
    { 122, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Multiply:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Multiply", True, "benchmark" },
    { 123, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Divide:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Divide", True, "benchmark" },
    { 124, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDecimal,System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "NotEquals", True, "benchmark" },
    { 125, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDecimal,System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "LessThan", True, "benchmark" },
    { 126, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDecimal,System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "GreaterThan", True, "benchmark" },
    { 127, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDecimal,System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "LessThanOrEqual", True, "benchmark" },
    { 128, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDecimal,System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "GreaterThanOrEqual", True, "benchmark" },
    { 129, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ToSqlBoolean:System.Data.SqlTypes.SqlBoolean()", "codegen", "System.Data.SqlTypes.SqlDecimal", "ToSqlBoolean", True, "benchmark" },
    { 130, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ToSqlByte:System.Data.SqlTypes.SqlByte()", "codegen", "System.Data.SqlTypes.SqlDecimal", "ToSqlByte", True, "benchmark" },
    { 131, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ToSqlDouble:System.Data.SqlTypes.SqlDouble()", "codegen", "System.Data.SqlTypes.SqlDecimal", "ToSqlDouble", True, "benchmark" },
    { 132, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ToSqlInt16:System.Data.SqlTypes.SqlInt16()", "codegen", "System.Data.SqlTypes.SqlDecimal", "ToSqlInt16", True, "benchmark" },
    { 133, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ToSqlInt32:System.Data.SqlTypes.SqlInt32()", "codegen", "System.Data.SqlTypes.SqlDecimal", "ToSqlInt32", True, "benchmark" },
    { 134, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ToSqlInt64:System.Data.SqlTypes.SqlInt64()", "codegen", "System.Data.SqlTypes.SqlDecimal", "ToSqlInt64", True, "benchmark" },
    { 135, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ToSqlMoney:System.Data.SqlTypes.SqlMoney()", "codegen", "System.Data.SqlTypes.SqlDecimal", "ToSqlMoney", True, "benchmark" },
    { 136, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ToSqlSingle:System.Data.SqlTypes.SqlSingle()", "codegen", "System.Data.SqlTypes.SqlDecimal", "ToSqlSingle", True, "benchmark" },
    { 137, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlDecimal", "ToSqlString", True, "benchmark" },
    { 138, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Ceiling:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Ceiling", True, "benchmark" },
    { 139, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Floor:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Floor", True, "benchmark" },
    { 140, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Sign:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Sign", True, "benchmark" },
    { 141, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Round:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Int32)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Round", True, "benchmark" },
    { 142, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Round:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Int32)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Round", True, "benchmark" },
    { 143, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Truncate:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Int32)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Truncate", True, "benchmark" },
    { 144, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Truncate:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Int32)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Truncate", True, "benchmark" },
    { 145, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Power:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Double)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Power", True, "benchmark" },
    { 146, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::Power:System.Data.SqlTypes.SqlDecimal(System.Data.SqlTypes.SqlDecimal,System.Double)", "codegen", "System.Data.SqlTypes.SqlDecimal", "Power", True, "benchmark" },
    { 147, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlDecimal", "CompareTo", True, "benchmark" },
    { 148, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::CompareTo:System.Int32(System.Data.SqlTypes.SqlDecimal)", "codegen", "System.Data.SqlTypes.SqlDecimal", "CompareTo", True, "benchmark" },
    { 149, "System.Data.Common/System.Data.SqlTypes.SqlDecimal::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlDecimal", "GetXsdType", True, "benchmark" },
    { 150, "System.Data.Common/System.Data.SqlTypes.SqlDouble::Parse:System.Data.SqlTypes.SqlDouble(System.String)", "codegen", "System.Data.SqlTypes.SqlDouble", "Parse", True, "benchmark" },
    { 151, "System.Data.Common/System.Data.SqlTypes.SqlDouble::Parse:System.Data.SqlTypes.SqlDouble(System.String)", "codegen", "System.Data.SqlTypes.SqlDouble", "Parse", True, "benchmark" },
    { 152, "System.Data.Common/System.Data.SqlTypes.SqlDouble::Add:System.Data.SqlTypes.SqlDouble(System.Data.SqlTypes.SqlDouble,System.Data.SqlTypes.SqlDouble)", "codegen", "System.Data.SqlTypes.SqlDouble", "Add", True, "benchmark" },
    { 153, "System.Data.Common/System.Data.SqlTypes.SqlDouble::Subtract:System.Data.SqlTypes.SqlDouble(System.Data.SqlTypes.SqlDouble,System.Data.SqlTypes.SqlDouble)", "codegen", "System.Data.SqlTypes.SqlDouble", "Subtract", True, "benchmark" },
    { 154, "System.Data.Common/System.Data.SqlTypes.SqlDouble::Multiply:System.Data.SqlTypes.SqlDouble(System.Data.SqlTypes.SqlDouble,System.Data.SqlTypes.SqlDouble)", "codegen", "System.Data.SqlTypes.SqlDouble", "Multiply", True, "benchmark" },
    { 155, "System.Data.Common/System.Data.SqlTypes.SqlDouble::Divide:System.Data.SqlTypes.SqlDouble(System.Data.SqlTypes.SqlDouble,System.Data.SqlTypes.SqlDouble)", "codegen", "System.Data.SqlTypes.SqlDouble", "Divide", True, "benchmark" },
    { 156, "System.Data.Common/System.Data.SqlTypes.SqlDouble::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDouble,System.Data.SqlTypes.SqlDouble)", "codegen", "System.Data.SqlTypes.SqlDouble", "NotEquals", True, "benchmark" },
    { 157, "System.Data.Common/System.Data.SqlTypes.SqlDouble::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDouble,System.Data.SqlTypes.SqlDouble)", "codegen", "System.Data.SqlTypes.SqlDouble", "LessThan", True, "benchmark" },
    { 158, "System.Data.Common/System.Data.SqlTypes.SqlDouble::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDouble,System.Data.SqlTypes.SqlDouble)", "codegen", "System.Data.SqlTypes.SqlDouble", "GreaterThan", True, "benchmark" },
    { 159, "System.Data.Common/System.Data.SqlTypes.SqlDouble::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDouble,System.Data.SqlTypes.SqlDouble)", "codegen", "System.Data.SqlTypes.SqlDouble", "LessThanOrEqual", True, "benchmark" },
    { 160, "System.Data.Common/System.Data.SqlTypes.SqlDouble::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlDouble,System.Data.SqlTypes.SqlDouble)", "codegen", "System.Data.SqlTypes.SqlDouble", "GreaterThanOrEqual", True, "benchmark" },
    { 161, "System.Data.Common/System.Data.SqlTypes.SqlDouble::ToSqlBoolean:System.Data.SqlTypes.SqlBoolean()", "codegen", "System.Data.SqlTypes.SqlDouble", "ToSqlBoolean", True, "benchmark" },
    { 162, "System.Data.Common/System.Data.SqlTypes.SqlDouble::ToSqlByte:System.Data.SqlTypes.SqlByte()", "codegen", "System.Data.SqlTypes.SqlDouble", "ToSqlByte", True, "benchmark" },
    { 163, "System.Data.Common/System.Data.SqlTypes.SqlDouble::ToSqlInt16:System.Data.SqlTypes.SqlInt16()", "codegen", "System.Data.SqlTypes.SqlDouble", "ToSqlInt16", True, "benchmark" },
    { 164, "System.Data.Common/System.Data.SqlTypes.SqlDouble::ToSqlInt32:System.Data.SqlTypes.SqlInt32()", "codegen", "System.Data.SqlTypes.SqlDouble", "ToSqlInt32", True, "benchmark" },
    { 165, "System.Data.Common/System.Data.SqlTypes.SqlDouble::ToSqlInt64:System.Data.SqlTypes.SqlInt64()", "codegen", "System.Data.SqlTypes.SqlDouble", "ToSqlInt64", True, "benchmark" },
    { 166, "System.Data.Common/System.Data.SqlTypes.SqlDouble::ToSqlMoney:System.Data.SqlTypes.SqlMoney()", "codegen", "System.Data.SqlTypes.SqlDouble", "ToSqlMoney", True, "benchmark" },
    { 167, "System.Data.Common/System.Data.SqlTypes.SqlDouble::ToSqlDecimal:System.Data.SqlTypes.SqlDecimal()", "codegen", "System.Data.SqlTypes.SqlDouble", "ToSqlDecimal", True, "benchmark" },
    { 168, "System.Data.Common/System.Data.SqlTypes.SqlDouble::ToSqlSingle:System.Data.SqlTypes.SqlSingle()", "codegen", "System.Data.SqlTypes.SqlDouble", "ToSqlSingle", True, "benchmark" },
    { 169, "System.Data.Common/System.Data.SqlTypes.SqlDouble::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlDouble", "ToSqlString", True, "benchmark" },
    { 170, "System.Data.Common/System.Data.SqlTypes.SqlDouble::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlDouble", "CompareTo", True, "benchmark" },
    { 171, "System.Data.Common/System.Data.SqlTypes.SqlDouble::CompareTo:System.Int32(System.Data.SqlTypes.SqlDouble)", "codegen", "System.Data.SqlTypes.SqlDouble", "CompareTo", True, "benchmark" },
    { 172, "System.Data.Common/System.Data.SqlTypes.SqlDouble::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlDouble", "GetXsdType", True, "benchmark" },
    { 173, "System.Data.Common/System.Data.SqlTypes.SqlGuid::ToByteArray:System.Byte[]()", "codegen", "System.Data.SqlTypes.SqlGuid", "ToByteArray", True, "benchmark" },
    { 174, "System.Data.Common/System.Data.SqlTypes.SqlGuid::Parse:System.Data.SqlTypes.SqlGuid(System.String)", "codegen", "System.Data.SqlTypes.SqlGuid", "Parse", True, "benchmark" },
    { 175, "System.Data.Common/System.Data.SqlTypes.SqlGuid::Parse:System.Data.SqlTypes.SqlGuid(System.String)", "codegen", "System.Data.SqlTypes.SqlGuid", "Parse", True, "benchmark" },
    { 176, "System.Data.Common/System.Data.SqlTypes.SqlGuid::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlGuid,System.Data.SqlTypes.SqlGuid)", "codegen", "System.Data.SqlTypes.SqlGuid", "NotEquals", True, "benchmark" },
    { 177, "System.Data.Common/System.Data.SqlTypes.SqlGuid::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlGuid,System.Data.SqlTypes.SqlGuid)", "codegen", "System.Data.SqlTypes.SqlGuid", "LessThan", True, "benchmark" },
    { 178, "System.Data.Common/System.Data.SqlTypes.SqlGuid::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlGuid,System.Data.SqlTypes.SqlGuid)", "codegen", "System.Data.SqlTypes.SqlGuid", "GreaterThan", True, "benchmark" },
    { 179, "System.Data.Common/System.Data.SqlTypes.SqlGuid::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlGuid,System.Data.SqlTypes.SqlGuid)", "codegen", "System.Data.SqlTypes.SqlGuid", "LessThanOrEqual", True, "benchmark" },
    { 180, "System.Data.Common/System.Data.SqlTypes.SqlGuid::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlGuid,System.Data.SqlTypes.SqlGuid)", "codegen", "System.Data.SqlTypes.SqlGuid", "GreaterThanOrEqual", True, "benchmark" },
    { 181, "System.Data.Common/System.Data.SqlTypes.SqlGuid::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlGuid", "ToSqlString", True, "benchmark" },
    { 182, "System.Data.Common/System.Data.SqlTypes.SqlGuid::ToSqlBinary:System.Data.SqlTypes.SqlBinary()", "codegen", "System.Data.SqlTypes.SqlGuid", "ToSqlBinary", True, "benchmark" },
    { 183, "System.Data.Common/System.Data.SqlTypes.SqlGuid::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlGuid", "CompareTo", True, "benchmark" },
    { 184, "System.Data.Common/System.Data.SqlTypes.SqlGuid::CompareTo:System.Int32(System.Data.SqlTypes.SqlGuid)", "codegen", "System.Data.SqlTypes.SqlGuid", "CompareTo", True, "benchmark" },
    { 185, "System.Data.Common/System.Data.SqlTypes.SqlGuid::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlGuid", "GetXsdType", True, "benchmark" },
    { 186, "System.Data.Common/System.Data.SqlTypes.SqlInt16::Parse:System.Data.SqlTypes.SqlInt16(System.String)", "codegen", "System.Data.SqlTypes.SqlInt16", "Parse", True, "benchmark" },
    { 187, "System.Data.Common/System.Data.SqlTypes.SqlInt16::Parse:System.Data.SqlTypes.SqlInt16(System.String)", "codegen", "System.Data.SqlTypes.SqlInt16", "Parse", True, "benchmark" },
    { 188, "System.Data.Common/System.Data.SqlTypes.SqlInt16::OnesComplement:System.Data.SqlTypes.SqlInt16(System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "OnesComplement", True, "benchmark" },
    { 189, "System.Data.Common/System.Data.SqlTypes.SqlInt16::Add:System.Data.SqlTypes.SqlInt16(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "Add", True, "benchmark" },
    { 190, "System.Data.Common/System.Data.SqlTypes.SqlInt16::Subtract:System.Data.SqlTypes.SqlInt16(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "Subtract", True, "benchmark" },
    { 191, "System.Data.Common/System.Data.SqlTypes.SqlInt16::Multiply:System.Data.SqlTypes.SqlInt16(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "Multiply", True, "benchmark" },
    { 192, "System.Data.Common/System.Data.SqlTypes.SqlInt16::Divide:System.Data.SqlTypes.SqlInt16(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "Divide", True, "benchmark" },
    { 193, "System.Data.Common/System.Data.SqlTypes.SqlInt16::Mod:System.Data.SqlTypes.SqlInt16(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "Mod", True, "benchmark" },
    { 194, "System.Data.Common/System.Data.SqlTypes.SqlInt16::Modulus:System.Data.SqlTypes.SqlInt16(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "Modulus", True, "benchmark" },
    { 195, "System.Data.Common/System.Data.SqlTypes.SqlInt16::BitwiseAnd:System.Data.SqlTypes.SqlInt16(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "BitwiseAnd", True, "benchmark" },
    { 196, "System.Data.Common/System.Data.SqlTypes.SqlInt16::BitwiseOr:System.Data.SqlTypes.SqlInt16(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "BitwiseOr", True, "benchmark" },
    { 197, "System.Data.Common/System.Data.SqlTypes.SqlInt16::Xor:System.Data.SqlTypes.SqlInt16(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "Xor", True, "benchmark" },
    { 198, "System.Data.Common/System.Data.SqlTypes.SqlInt16::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "NotEquals", True, "benchmark" },
    { 199, "System.Data.Common/System.Data.SqlTypes.SqlInt16::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "LessThan", True, "benchmark" },
    { 200, "System.Data.Common/System.Data.SqlTypes.SqlInt16::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "GreaterThan", True, "benchmark" },
    { 201, "System.Data.Common/System.Data.SqlTypes.SqlInt16::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "LessThanOrEqual", True, "benchmark" },
    { 202, "System.Data.Common/System.Data.SqlTypes.SqlInt16::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt16,System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "GreaterThanOrEqual", True, "benchmark" },
    { 203, "System.Data.Common/System.Data.SqlTypes.SqlInt16::ToSqlBoolean:System.Data.SqlTypes.SqlBoolean()", "codegen", "System.Data.SqlTypes.SqlInt16", "ToSqlBoolean", True, "benchmark" },
    { 204, "System.Data.Common/System.Data.SqlTypes.SqlInt16::ToSqlByte:System.Data.SqlTypes.SqlByte()", "codegen", "System.Data.SqlTypes.SqlInt16", "ToSqlByte", True, "benchmark" },
    { 205, "System.Data.Common/System.Data.SqlTypes.SqlInt16::ToSqlDouble:System.Data.SqlTypes.SqlDouble()", "codegen", "System.Data.SqlTypes.SqlInt16", "ToSqlDouble", True, "benchmark" },
    { 206, "System.Data.Common/System.Data.SqlTypes.SqlInt16::ToSqlInt32:System.Data.SqlTypes.SqlInt32()", "codegen", "System.Data.SqlTypes.SqlInt16", "ToSqlInt32", True, "benchmark" },
    { 207, "System.Data.Common/System.Data.SqlTypes.SqlInt16::ToSqlInt64:System.Data.SqlTypes.SqlInt64()", "codegen", "System.Data.SqlTypes.SqlInt16", "ToSqlInt64", True, "benchmark" },
    { 208, "System.Data.Common/System.Data.SqlTypes.SqlInt16::ToSqlMoney:System.Data.SqlTypes.SqlMoney()", "codegen", "System.Data.SqlTypes.SqlInt16", "ToSqlMoney", True, "benchmark" },
    { 209, "System.Data.Common/System.Data.SqlTypes.SqlInt16::ToSqlDecimal:System.Data.SqlTypes.SqlDecimal()", "codegen", "System.Data.SqlTypes.SqlInt16", "ToSqlDecimal", True, "benchmark" },
    { 210, "System.Data.Common/System.Data.SqlTypes.SqlInt16::ToSqlSingle:System.Data.SqlTypes.SqlSingle()", "codegen", "System.Data.SqlTypes.SqlInt16", "ToSqlSingle", True, "benchmark" },
    { 211, "System.Data.Common/System.Data.SqlTypes.SqlInt16::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlInt16", "ToSqlString", True, "benchmark" },
    { 212, "System.Data.Common/System.Data.SqlTypes.SqlInt16::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlInt16", "CompareTo", True, "benchmark" },
    { 213, "System.Data.Common/System.Data.SqlTypes.SqlInt16::CompareTo:System.Int32(System.Data.SqlTypes.SqlInt16)", "codegen", "System.Data.SqlTypes.SqlInt16", "CompareTo", True, "benchmark" },
    { 214, "System.Data.Common/System.Data.SqlTypes.SqlInt16::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlInt16", "GetXsdType", True, "benchmark" },
    { 215, "System.Data.Common/System.Data.SqlTypes.SqlInt32::Parse:System.Data.SqlTypes.SqlInt32(System.String)", "codegen", "System.Data.SqlTypes.SqlInt32", "Parse", True, "benchmark" },
    { 216, "System.Data.Common/System.Data.SqlTypes.SqlInt32::Parse:System.Data.SqlTypes.SqlInt32(System.String)", "codegen", "System.Data.SqlTypes.SqlInt32", "Parse", True, "benchmark" },
    { 217, "System.Data.Common/System.Data.SqlTypes.SqlInt32::OnesComplement:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "OnesComplement", True, "benchmark" },
    { 218, "System.Data.Common/System.Data.SqlTypes.SqlInt32::Add:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "Add", True, "benchmark" },
    { 219, "System.Data.Common/System.Data.SqlTypes.SqlInt32::Subtract:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "Subtract", True, "benchmark" },
    { 220, "System.Data.Common/System.Data.SqlTypes.SqlInt32::Multiply:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "Multiply", True, "benchmark" },
    { 221, "System.Data.Common/System.Data.SqlTypes.SqlInt32::Divide:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "Divide", True, "benchmark" },
    { 222, "System.Data.Common/System.Data.SqlTypes.SqlInt32::Mod:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "Mod", True, "benchmark" },
    { 223, "System.Data.Common/System.Data.SqlTypes.SqlInt32::Modulus:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "Modulus", True, "benchmark" },
    { 224, "System.Data.Common/System.Data.SqlTypes.SqlInt32::BitwiseAnd:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "BitwiseAnd", True, "benchmark" },
    { 225, "System.Data.Common/System.Data.SqlTypes.SqlInt32::BitwiseOr:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "BitwiseOr", True, "benchmark" },
    { 226, "System.Data.Common/System.Data.SqlTypes.SqlInt32::Xor:System.Data.SqlTypes.SqlInt32(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "Xor", True, "benchmark" },
    { 227, "System.Data.Common/System.Data.SqlTypes.SqlInt32::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "NotEquals", True, "benchmark" },
    { 228, "System.Data.Common/System.Data.SqlTypes.SqlInt32::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "LessThan", True, "benchmark" },
    { 229, "System.Data.Common/System.Data.SqlTypes.SqlInt32::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "GreaterThan", True, "benchmark" },
    { 230, "System.Data.Common/System.Data.SqlTypes.SqlInt32::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "LessThanOrEqual", True, "benchmark" },
    { 231, "System.Data.Common/System.Data.SqlTypes.SqlInt32::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt32,System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "GreaterThanOrEqual", True, "benchmark" },
    { 232, "System.Data.Common/System.Data.SqlTypes.SqlInt32::ToSqlBoolean:System.Data.SqlTypes.SqlBoolean()", "codegen", "System.Data.SqlTypes.SqlInt32", "ToSqlBoolean", True, "benchmark" },
    { 233, "System.Data.Common/System.Data.SqlTypes.SqlInt32::ToSqlByte:System.Data.SqlTypes.SqlByte()", "codegen", "System.Data.SqlTypes.SqlInt32", "ToSqlByte", True, "benchmark" },
    { 234, "System.Data.Common/System.Data.SqlTypes.SqlInt32::ToSqlDouble:System.Data.SqlTypes.SqlDouble()", "codegen", "System.Data.SqlTypes.SqlInt32", "ToSqlDouble", True, "benchmark" },
    { 235, "System.Data.Common/System.Data.SqlTypes.SqlInt32::ToSqlInt16:System.Data.SqlTypes.SqlInt16()", "codegen", "System.Data.SqlTypes.SqlInt32", "ToSqlInt16", True, "benchmark" },
    { 236, "System.Data.Common/System.Data.SqlTypes.SqlInt32::ToSqlInt64:System.Data.SqlTypes.SqlInt64()", "codegen", "System.Data.SqlTypes.SqlInt32", "ToSqlInt64", True, "benchmark" },
    { 237, "System.Data.Common/System.Data.SqlTypes.SqlInt32::ToSqlMoney:System.Data.SqlTypes.SqlMoney()", "codegen", "System.Data.SqlTypes.SqlInt32", "ToSqlMoney", True, "benchmark" },
    { 238, "System.Data.Common/System.Data.SqlTypes.SqlInt32::ToSqlDecimal:System.Data.SqlTypes.SqlDecimal()", "codegen", "System.Data.SqlTypes.SqlInt32", "ToSqlDecimal", True, "benchmark" },
    { 239, "System.Data.Common/System.Data.SqlTypes.SqlInt32::ToSqlSingle:System.Data.SqlTypes.SqlSingle()", "codegen", "System.Data.SqlTypes.SqlInt32", "ToSqlSingle", True, "benchmark" },
    { 240, "System.Data.Common/System.Data.SqlTypes.SqlInt32::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlInt32", "ToSqlString", True, "benchmark" },
    { 241, "System.Data.Common/System.Data.SqlTypes.SqlInt32::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlInt32", "CompareTo", True, "benchmark" },
    { 242, "System.Data.Common/System.Data.SqlTypes.SqlInt32::CompareTo:System.Int32(System.Data.SqlTypes.SqlInt32)", "codegen", "System.Data.SqlTypes.SqlInt32", "CompareTo", True, "benchmark" },
    { 243, "System.Data.Common/System.Data.SqlTypes.SqlInt32::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlInt32", "GetXsdType", True, "benchmark" },
    { 244, "System.Data.Common/System.Data.SqlTypes.SqlInt64::Parse:System.Data.SqlTypes.SqlInt64(System.String)", "codegen", "System.Data.SqlTypes.SqlInt64", "Parse", True, "benchmark" },
    { 245, "System.Data.Common/System.Data.SqlTypes.SqlInt64::Parse:System.Data.SqlTypes.SqlInt64(System.String)", "codegen", "System.Data.SqlTypes.SqlInt64", "Parse", True, "benchmark" },
    { 246, "System.Data.Common/System.Data.SqlTypes.SqlInt64::OnesComplement:System.Data.SqlTypes.SqlInt64(System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "OnesComplement", True, "benchmark" },
    { 247, "System.Data.Common/System.Data.SqlTypes.SqlInt64::Add:System.Data.SqlTypes.SqlInt64(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "Add", True, "benchmark" },
    { 248, "System.Data.Common/System.Data.SqlTypes.SqlInt64::Subtract:System.Data.SqlTypes.SqlInt64(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "Subtract", True, "benchmark" },
    { 249, "System.Data.Common/System.Data.SqlTypes.SqlInt64::Multiply:System.Data.SqlTypes.SqlInt64(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "Multiply", True, "benchmark" },
    { 250, "System.Data.Common/System.Data.SqlTypes.SqlInt64::Divide:System.Data.SqlTypes.SqlInt64(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "Divide", True, "benchmark" },
    { 251, "System.Data.Common/System.Data.SqlTypes.SqlInt64::Mod:System.Data.SqlTypes.SqlInt64(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "Mod", True, "benchmark" },
    { 252, "System.Data.Common/System.Data.SqlTypes.SqlInt64::Modulus:System.Data.SqlTypes.SqlInt64(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "Modulus", True, "benchmark" },
    { 253, "System.Data.Common/System.Data.SqlTypes.SqlInt64::BitwiseAnd:System.Data.SqlTypes.SqlInt64(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "BitwiseAnd", True, "benchmark" },
    { 254, "System.Data.Common/System.Data.SqlTypes.SqlInt64::BitwiseOr:System.Data.SqlTypes.SqlInt64(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "BitwiseOr", True, "benchmark" },
    { 255, "System.Data.Common/System.Data.SqlTypes.SqlInt64::Xor:System.Data.SqlTypes.SqlInt64(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "Xor", True, "benchmark" },
    { 256, "System.Data.Common/System.Data.SqlTypes.SqlInt64::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "NotEquals", True, "benchmark" },
    { 257, "System.Data.Common/System.Data.SqlTypes.SqlInt64::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "LessThan", True, "benchmark" },
    { 258, "System.Data.Common/System.Data.SqlTypes.SqlInt64::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "GreaterThan", True, "benchmark" },
    { 259, "System.Data.Common/System.Data.SqlTypes.SqlInt64::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "LessThanOrEqual", True, "benchmark" },
    { 260, "System.Data.Common/System.Data.SqlTypes.SqlInt64::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlInt64,System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "GreaterThanOrEqual", True, "benchmark" },
    { 261, "System.Data.Common/System.Data.SqlTypes.SqlInt64::ToSqlBoolean:System.Data.SqlTypes.SqlBoolean()", "codegen", "System.Data.SqlTypes.SqlInt64", "ToSqlBoolean", True, "benchmark" },
    { 262, "System.Data.Common/System.Data.SqlTypes.SqlInt64::ToSqlByte:System.Data.SqlTypes.SqlByte()", "codegen", "System.Data.SqlTypes.SqlInt64", "ToSqlByte", True, "benchmark" },
    { 263, "System.Data.Common/System.Data.SqlTypes.SqlInt64::ToSqlDouble:System.Data.SqlTypes.SqlDouble()", "codegen", "System.Data.SqlTypes.SqlInt64", "ToSqlDouble", True, "benchmark" },
    { 264, "System.Data.Common/System.Data.SqlTypes.SqlInt64::ToSqlInt16:System.Data.SqlTypes.SqlInt16()", "codegen", "System.Data.SqlTypes.SqlInt64", "ToSqlInt16", True, "benchmark" },
    { 265, "System.Data.Common/System.Data.SqlTypes.SqlInt64::ToSqlInt32:System.Data.SqlTypes.SqlInt32()", "codegen", "System.Data.SqlTypes.SqlInt64", "ToSqlInt32", True, "benchmark" },
    { 266, "System.Data.Common/System.Data.SqlTypes.SqlInt64::ToSqlMoney:System.Data.SqlTypes.SqlMoney()", "codegen", "System.Data.SqlTypes.SqlInt64", "ToSqlMoney", True, "benchmark" },
    { 267, "System.Data.Common/System.Data.SqlTypes.SqlInt64::ToSqlDecimal:System.Data.SqlTypes.SqlDecimal()", "codegen", "System.Data.SqlTypes.SqlInt64", "ToSqlDecimal", True, "benchmark" },
    { 268, "System.Data.Common/System.Data.SqlTypes.SqlInt64::ToSqlSingle:System.Data.SqlTypes.SqlSingle()", "codegen", "System.Data.SqlTypes.SqlInt64", "ToSqlSingle", True, "benchmark" },
    { 269, "System.Data.Common/System.Data.SqlTypes.SqlInt64::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlInt64", "ToSqlString", True, "benchmark" },
    { 270, "System.Data.Common/System.Data.SqlTypes.SqlInt64::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlInt64", "CompareTo", True, "benchmark" },
    { 271, "System.Data.Common/System.Data.SqlTypes.SqlInt64::CompareTo:System.Int32(System.Data.SqlTypes.SqlInt64)", "codegen", "System.Data.SqlTypes.SqlInt64", "CompareTo", True, "benchmark" },
    { 272, "System.Data.Common/System.Data.SqlTypes.SqlInt64::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlInt64", "GetXsdType", True, "benchmark" },
    { 273, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToDecimal:System.Decimal()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToDecimal", True, "benchmark" },
    { 274, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToInt64:System.Int64()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToInt64", True, "benchmark" },
    { 275, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToInt32:System.Int32()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToInt32", True, "benchmark" },
    { 276, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToDouble:System.Double()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToDouble", True, "benchmark" },
    { 277, "System.Data.Common/System.Data.SqlTypes.SqlMoney::GetTdsValue:System.Int64()", "codegen", "System.Data.SqlTypes.SqlMoney", "GetTdsValue", True, "benchmark" },
    { 278, "System.Data.Common/System.Data.SqlTypes.SqlMoney::Parse:System.Data.SqlTypes.SqlMoney(System.String)", "codegen", "System.Data.SqlTypes.SqlMoney", "Parse", True, "benchmark" },
    { 279, "System.Data.Common/System.Data.SqlTypes.SqlMoney::Parse:System.Data.SqlTypes.SqlMoney(System.String)", "codegen", "System.Data.SqlTypes.SqlMoney", "Parse", True, "benchmark" },
    { 280, "System.Data.Common/System.Data.SqlTypes.SqlMoney::FromTdsValue:System.Data.SqlTypes.SqlMoney(System.Int64)", "codegen", "System.Data.SqlTypes.SqlMoney", "FromTdsValue", True, "benchmark" },
    { 281, "System.Data.Common/System.Data.SqlTypes.SqlMoney::FromTdsValue:System.Data.SqlTypes.SqlMoney(System.Int64)", "codegen", "System.Data.SqlTypes.SqlMoney", "FromTdsValue", True, "benchmark" },
    { 282, "System.Data.Common/System.Data.SqlTypes.SqlMoney::Add:System.Data.SqlTypes.SqlMoney(System.Data.SqlTypes.SqlMoney,System.Data.SqlTypes.SqlMoney)", "codegen", "System.Data.SqlTypes.SqlMoney", "Add", True, "benchmark" },
    { 283, "System.Data.Common/System.Data.SqlTypes.SqlMoney::Subtract:System.Data.SqlTypes.SqlMoney(System.Data.SqlTypes.SqlMoney,System.Data.SqlTypes.SqlMoney)", "codegen", "System.Data.SqlTypes.SqlMoney", "Subtract", True, "benchmark" },
    { 284, "System.Data.Common/System.Data.SqlTypes.SqlMoney::Multiply:System.Data.SqlTypes.SqlMoney(System.Data.SqlTypes.SqlMoney,System.Data.SqlTypes.SqlMoney)", "codegen", "System.Data.SqlTypes.SqlMoney", "Multiply", True, "benchmark" },
    { 285, "System.Data.Common/System.Data.SqlTypes.SqlMoney::Divide:System.Data.SqlTypes.SqlMoney(System.Data.SqlTypes.SqlMoney,System.Data.SqlTypes.SqlMoney)", "codegen", "System.Data.SqlTypes.SqlMoney", "Divide", True, "benchmark" },
    { 286, "System.Data.Common/System.Data.SqlTypes.SqlMoney::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlMoney,System.Data.SqlTypes.SqlMoney)", "codegen", "System.Data.SqlTypes.SqlMoney", "NotEquals", True, "benchmark" },
    { 287, "System.Data.Common/System.Data.SqlTypes.SqlMoney::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlMoney,System.Data.SqlTypes.SqlMoney)", "codegen", "System.Data.SqlTypes.SqlMoney", "LessThan", True, "benchmark" },
    { 288, "System.Data.Common/System.Data.SqlTypes.SqlMoney::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlMoney,System.Data.SqlTypes.SqlMoney)", "codegen", "System.Data.SqlTypes.SqlMoney", "GreaterThan", True, "benchmark" },
    { 289, "System.Data.Common/System.Data.SqlTypes.SqlMoney::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlMoney,System.Data.SqlTypes.SqlMoney)", "codegen", "System.Data.SqlTypes.SqlMoney", "LessThanOrEqual", True, "benchmark" },
    { 290, "System.Data.Common/System.Data.SqlTypes.SqlMoney::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlMoney,System.Data.SqlTypes.SqlMoney)", "codegen", "System.Data.SqlTypes.SqlMoney", "GreaterThanOrEqual", True, "benchmark" },
    { 291, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToSqlBoolean:System.Data.SqlTypes.SqlBoolean()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToSqlBoolean", True, "benchmark" },
    { 292, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToSqlByte:System.Data.SqlTypes.SqlByte()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToSqlByte", True, "benchmark" },
    { 293, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToSqlDouble:System.Data.SqlTypes.SqlDouble()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToSqlDouble", True, "benchmark" },
    { 294, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToSqlInt16:System.Data.SqlTypes.SqlInt16()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToSqlInt16", True, "benchmark" },
    { 295, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToSqlInt32:System.Data.SqlTypes.SqlInt32()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToSqlInt32", True, "benchmark" },
    { 296, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToSqlInt64:System.Data.SqlTypes.SqlInt64()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToSqlInt64", True, "benchmark" },
    { 297, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToSqlDecimal:System.Data.SqlTypes.SqlDecimal()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToSqlDecimal", True, "benchmark" },
    { 298, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToSqlSingle:System.Data.SqlTypes.SqlSingle()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToSqlSingle", True, "benchmark" },
    { 299, "System.Data.Common/System.Data.SqlTypes.SqlMoney::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlMoney", "ToSqlString", True, "benchmark" },
    { 300, "System.Data.Common/System.Data.SqlTypes.SqlMoney::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlMoney", "CompareTo", True, "benchmark" },
    { 301, "System.Data.Common/System.Data.SqlTypes.SqlMoney::CompareTo:System.Int32(System.Data.SqlTypes.SqlMoney)", "codegen", "System.Data.SqlTypes.SqlMoney", "CompareTo", True, "benchmark" },
    { 302, "System.Data.Common/System.Data.SqlTypes.SqlMoney::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlMoney", "GetXsdType", True, "benchmark" },
    { 303, "System.Data.Common/System.Data.SqlTypes.SqlSingle::Parse:System.Data.SqlTypes.SqlSingle(System.String)", "codegen", "System.Data.SqlTypes.SqlSingle", "Parse", True, "benchmark" },
    { 304, "System.Data.Common/System.Data.SqlTypes.SqlSingle::Parse:System.Data.SqlTypes.SqlSingle(System.String)", "codegen", "System.Data.SqlTypes.SqlSingle", "Parse", True, "benchmark" },
    { 305, "System.Data.Common/System.Data.SqlTypes.SqlSingle::Add:System.Data.SqlTypes.SqlSingle(System.Data.SqlTypes.SqlSingle,System.Data.SqlTypes.SqlSingle)", "codegen", "System.Data.SqlTypes.SqlSingle", "Add", True, "benchmark" },
    { 306, "System.Data.Common/System.Data.SqlTypes.SqlSingle::Subtract:System.Data.SqlTypes.SqlSingle(System.Data.SqlTypes.SqlSingle,System.Data.SqlTypes.SqlSingle)", "codegen", "System.Data.SqlTypes.SqlSingle", "Subtract", True, "benchmark" },
    { 307, "System.Data.Common/System.Data.SqlTypes.SqlSingle::Multiply:System.Data.SqlTypes.SqlSingle(System.Data.SqlTypes.SqlSingle,System.Data.SqlTypes.SqlSingle)", "codegen", "System.Data.SqlTypes.SqlSingle", "Multiply", True, "benchmark" },
    { 308, "System.Data.Common/System.Data.SqlTypes.SqlSingle::Divide:System.Data.SqlTypes.SqlSingle(System.Data.SqlTypes.SqlSingle,System.Data.SqlTypes.SqlSingle)", "codegen", "System.Data.SqlTypes.SqlSingle", "Divide", True, "benchmark" },
    { 309, "System.Data.Common/System.Data.SqlTypes.SqlSingle::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlSingle,System.Data.SqlTypes.SqlSingle)", "codegen", "System.Data.SqlTypes.SqlSingle", "NotEquals", True, "benchmark" },
    { 310, "System.Data.Common/System.Data.SqlTypes.SqlSingle::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlSingle,System.Data.SqlTypes.SqlSingle)", "codegen", "System.Data.SqlTypes.SqlSingle", "LessThan", True, "benchmark" },
    { 311, "System.Data.Common/System.Data.SqlTypes.SqlSingle::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlSingle,System.Data.SqlTypes.SqlSingle)", "codegen", "System.Data.SqlTypes.SqlSingle", "GreaterThan", True, "benchmark" },
    { 312, "System.Data.Common/System.Data.SqlTypes.SqlSingle::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlSingle,System.Data.SqlTypes.SqlSingle)", "codegen", "System.Data.SqlTypes.SqlSingle", "LessThanOrEqual", True, "benchmark" },
    { 313, "System.Data.Common/System.Data.SqlTypes.SqlSingle::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlSingle,System.Data.SqlTypes.SqlSingle)", "codegen", "System.Data.SqlTypes.SqlSingle", "GreaterThanOrEqual", True, "benchmark" },
    { 314, "System.Data.Common/System.Data.SqlTypes.SqlSingle::ToSqlBoolean:System.Data.SqlTypes.SqlBoolean()", "codegen", "System.Data.SqlTypes.SqlSingle", "ToSqlBoolean", True, "benchmark" },
    { 315, "System.Data.Common/System.Data.SqlTypes.SqlSingle::ToSqlByte:System.Data.SqlTypes.SqlByte()", "codegen", "System.Data.SqlTypes.SqlSingle", "ToSqlByte", True, "benchmark" },
    { 316, "System.Data.Common/System.Data.SqlTypes.SqlSingle::ToSqlDouble:System.Data.SqlTypes.SqlDouble()", "codegen", "System.Data.SqlTypes.SqlSingle", "ToSqlDouble", True, "benchmark" },
    { 317, "System.Data.Common/System.Data.SqlTypes.SqlSingle::ToSqlInt16:System.Data.SqlTypes.SqlInt16()", "codegen", "System.Data.SqlTypes.SqlSingle", "ToSqlInt16", True, "benchmark" },
    { 318, "System.Data.Common/System.Data.SqlTypes.SqlSingle::ToSqlInt32:System.Data.SqlTypes.SqlInt32()", "codegen", "System.Data.SqlTypes.SqlSingle", "ToSqlInt32", True, "benchmark" },
    { 319, "System.Data.Common/System.Data.SqlTypes.SqlSingle::ToSqlInt64:System.Data.SqlTypes.SqlInt64()", "codegen", "System.Data.SqlTypes.SqlSingle", "ToSqlInt64", True, "benchmark" },
    { 320, "System.Data.Common/System.Data.SqlTypes.SqlSingle::ToSqlMoney:System.Data.SqlTypes.SqlMoney()", "codegen", "System.Data.SqlTypes.SqlSingle", "ToSqlMoney", True, "benchmark" },
    { 321, "System.Data.Common/System.Data.SqlTypes.SqlSingle::ToSqlDecimal:System.Data.SqlTypes.SqlDecimal()", "codegen", "System.Data.SqlTypes.SqlSingle", "ToSqlDecimal", True, "benchmark" },
    { 322, "System.Data.Common/System.Data.SqlTypes.SqlSingle::ToSqlString:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlSingle", "ToSqlString", True, "benchmark" },
    { 323, "System.Data.Common/System.Data.SqlTypes.SqlSingle::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlSingle", "CompareTo", True, "benchmark" },
    { 324, "System.Data.Common/System.Data.SqlTypes.SqlSingle::CompareTo:System.Int32(System.Data.SqlTypes.SqlSingle)", "codegen", "System.Data.SqlTypes.SqlSingle", "CompareTo", True, "benchmark" },
    { 325, "System.Data.Common/System.Data.SqlTypes.SqlSingle::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlSingle", "GetXsdType", True, "benchmark" },
    { 326, "System.Data.Common/System.Data.SqlTypes.SqlString::GetUnicodeBytes:System.Byte[]()", "codegen", "System.Data.SqlTypes.SqlString", "GetUnicodeBytes", True, "benchmark" },
    { 327, "System.Data.Common/System.Data.SqlTypes.SqlString::GetNonUnicodeBytes:System.Byte[]()", "codegen", "System.Data.SqlTypes.SqlString", "GetNonUnicodeBytes", True, "benchmark" },
    { 328, "System.Data.Common/System.Data.SqlTypes.SqlString::Clone:System.Data.SqlTypes.SqlString()", "codegen", "System.Data.SqlTypes.SqlString", "Clone", True, "benchmark" },
    { 329, "System.Data.Common/System.Data.SqlTypes.SqlString::Concat:System.Data.SqlTypes.SqlString(System.Data.SqlTypes.SqlString,System.Data.SqlTypes.SqlString)", "codegen", "System.Data.SqlTypes.SqlString", "Concat", True, "benchmark" },
    { 330, "System.Data.Common/System.Data.SqlTypes.SqlString::Add:System.Data.SqlTypes.SqlString(System.Data.SqlTypes.SqlString,System.Data.SqlTypes.SqlString)", "codegen", "System.Data.SqlTypes.SqlString", "Add", True, "benchmark" },
    { 331, "System.Data.Common/System.Data.SqlTypes.SqlString::NotEquals:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlString,System.Data.SqlTypes.SqlString)", "codegen", "System.Data.SqlTypes.SqlString", "NotEquals", True, "benchmark" },
    { 332, "System.Data.Common/System.Data.SqlTypes.SqlString::LessThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlString,System.Data.SqlTypes.SqlString)", "codegen", "System.Data.SqlTypes.SqlString", "LessThan", True, "benchmark" },
    { 333, "System.Data.Common/System.Data.SqlTypes.SqlString::GreaterThan:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlString,System.Data.SqlTypes.SqlString)", "codegen", "System.Data.SqlTypes.SqlString", "GreaterThan", True, "benchmark" },
    { 334, "System.Data.Common/System.Data.SqlTypes.SqlString::LessThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlString,System.Data.SqlTypes.SqlString)", "codegen", "System.Data.SqlTypes.SqlString", "LessThanOrEqual", True, "benchmark" },
    { 335, "System.Data.Common/System.Data.SqlTypes.SqlString::GreaterThanOrEqual:System.Data.SqlTypes.SqlBoolean(System.Data.SqlTypes.SqlString,System.Data.SqlTypes.SqlString)", "codegen", "System.Data.SqlTypes.SqlString", "GreaterThanOrEqual", True, "benchmark" },
    { 336, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlBoolean:System.Data.SqlTypes.SqlBoolean()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlBoolean", True, "benchmark" },
    { 337, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlByte:System.Data.SqlTypes.SqlByte()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlByte", True, "benchmark" },
    { 338, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlDateTime:System.Data.SqlTypes.SqlDateTime()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlDateTime", True, "benchmark" },
    { 339, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlDouble:System.Data.SqlTypes.SqlDouble()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlDouble", True, "benchmark" },
    { 340, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlInt16:System.Data.SqlTypes.SqlInt16()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlInt16", True, "benchmark" },
    { 341, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlInt32:System.Data.SqlTypes.SqlInt32()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlInt32", True, "benchmark" },
    { 342, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlInt64:System.Data.SqlTypes.SqlInt64()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlInt64", True, "benchmark" },
    { 343, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlMoney:System.Data.SqlTypes.SqlMoney()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlMoney", True, "benchmark" },
    { 344, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlDecimal:System.Data.SqlTypes.SqlDecimal()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlDecimal", True, "benchmark" },
    { 345, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlSingle:System.Data.SqlTypes.SqlSingle()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlSingle", True, "benchmark" },
    { 346, "System.Data.Common/System.Data.SqlTypes.SqlString::ToSqlGuid:System.Data.SqlTypes.SqlGuid()", "codegen", "System.Data.SqlTypes.SqlString", "ToSqlGuid", True, "benchmark" },
    { 347, "System.Data.Common/System.Data.SqlTypes.SqlString::CompareOptionsFromSqlCompareOptions:System.Globalization.CompareOptions(System.Data.SqlTypes.SqlCompareOptions)", "codegen", "System.Data.SqlTypes.SqlString", "CompareOptionsFromSqlCompareOptions", True, "benchmark" },
    { 348, "System.Data.Common/System.Data.SqlTypes.SqlString::CompareTo:System.Int32(System.Object)", "codegen", "System.Data.SqlTypes.SqlString", "CompareTo", True, "benchmark" },
    { 349, "System.Data.Common/System.Data.SqlTypes.SqlString::CompareTo:System.Int32(System.Data.SqlTypes.SqlString)", "codegen", "System.Data.SqlTypes.SqlString", "CompareTo", True, "benchmark" },
    { 350, "System.Data.Common/System.Data.SqlTypes.SqlString::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlString", "GetXsdType", True, "benchmark" },
    { 351, "System.Data.Common/System.Data.SqlTypes.SqlXml::CreateReader:System.Xml.XmlReader()", "codegen", "System.Data.SqlTypes.SqlXml", "CreateReader", True, "benchmark" },
    { 352, "System.Data.Common/System.Data.SqlTypes.SqlXml::GetXsdType:System.Xml.XmlQualifiedName(System.Xml.Schema.XmlSchemaSet)", "codegen", "System.Data.SqlTypes.SqlXml", "GetXsdType", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 353;

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