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
    { 0, "System.Text.Json/System.Text.Json.JsonDocument::Dispose:System.Void()", "codegen", "System.Text.Json.JsonDocument", "Dispose", True, "benchmark" },
    { 1, "System.Text.Json/System.Text.Json.JsonDocument::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter)", "codegen", "System.Text.Json.JsonDocument", "WriteTo", True, "benchmark" },
    { 2, "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.ReadOnlyMemory<System.Byte>,System.Text.Json.JsonDocumentOptions)", "codegen", "System.Text.Json.JsonDocument", "Parse", True, "benchmark" },
    { 3, "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.Buffers.ReadOnlySequence<System.Byte>,System.Text.Json.JsonDocumentOptions)", "codegen", "System.Text.Json.JsonDocument", "Parse", True, "benchmark" },
    { 4, "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.IO.Stream,System.Text.Json.JsonDocumentOptions)", "codegen", "System.Text.Json.JsonDocument", "Parse", True, "benchmark" },
    { 5, "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.IO.Stream,System.Text.Json.JsonDocumentOptions)", "codegen", "System.Text.Json.JsonDocument", "Parse", True, "benchmark" },
    { 6, "System.Text.Json/System.Text.Json.JsonDocument::ParseAsync:System.Threading.Tasks.Task<System.Text.Json.JsonDocument>(System.IO.Stream,System.Text.Json.JsonDocumentOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonDocument", "ParseAsync", True, "benchmark" },
    { 7, "System.Text.Json/System.Text.Json.JsonDocument::ParseAsync:System.Threading.Tasks.Task<System.Text.Json.JsonDocument>(System.IO.Stream,System.Text.Json.JsonDocumentOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonDocument", "ParseAsync", True, "benchmark" },
    { 8, "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.ReadOnlyMemory<System.Char>,System.Text.Json.JsonDocumentOptions)", "codegen", "System.Text.Json.JsonDocument", "Parse", True, "benchmark" },
    { 9, "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.String,System.Text.Json.JsonDocumentOptions)", "codegen", "System.Text.Json.JsonDocument", "Parse", True, "benchmark" },
    { 10, "System.Text.Json/System.Text.Json.JsonDocument::Parse:System.Text.Json.JsonDocument(System.String,System.Text.Json.JsonDocumentOptions)", "codegen", "System.Text.Json.JsonDocument", "Parse", True, "benchmark" },
    { 11, "System.Text.Json/System.Text.Json.JsonElement::GetArrayLength:System.Int32()", "codegen", "System.Text.Json.JsonElement", "GetArrayLength", True, "benchmark" },
    { 12, "System.Text.Json/System.Text.Json.JsonElement::GetPropertyCount:System.Int32()", "codegen", "System.Text.Json.JsonElement", "GetPropertyCount", True, "benchmark" },
    { 13, "System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.String)", "codegen", "System.Text.Json.JsonElement", "GetProperty", True, "benchmark" },
    { 14, "System.Text.Json/System.Text.Json.JsonElement::GetProperty:System.Text.Json.JsonElement(System.String)", "codegen", "System.Text.Json.JsonElement", "GetProperty", True, "benchmark" },
    { 15, "System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.String,System.Text.Json.JsonElement)", "codegen", "System.Text.Json.JsonElement", "TryGetProperty", True, "benchmark" },
    { 16, "System.Text.Json/System.Text.Json.JsonElement::TryGetProperty:System.Boolean(System.String,System.Text.Json.JsonElement)", "codegen", "System.Text.Json.JsonElement", "TryGetProperty", True, "benchmark" },
    { 17, "System.Text.Json/System.Text.Json.JsonElement::GetBoolean:System.Boolean()", "codegen", "System.Text.Json.JsonElement", "GetBoolean", True, "benchmark" },
    { 18, "System.Text.Json/System.Text.Json.JsonElement::GetString:System.String()", "codegen", "System.Text.Json.JsonElement", "GetString", True, "benchmark" },
    { 19, "System.Text.Json/System.Text.Json.JsonElement::TryGetBytesFromBase64:System.Boolean(System.Byte[])", "codegen", "System.Text.Json.JsonElement", "TryGetBytesFromBase64", True, "benchmark" },
    { 20, "System.Text.Json/System.Text.Json.JsonElement::TryGetBytesFromBase64:System.Boolean(System.Byte[])", "codegen", "System.Text.Json.JsonElement", "TryGetBytesFromBase64", True, "benchmark" },
    { 21, "System.Text.Json/System.Text.Json.JsonElement::TryGetBytesFromBase64:System.Boolean(System.Byte[])", "codegen", "System.Text.Json.JsonElement", "TryGetBytesFromBase64", True, "benchmark" },
    { 22, "System.Text.Json/System.Text.Json.JsonElement::GetBytesFromBase64:System.Byte[]()", "codegen", "System.Text.Json.JsonElement", "GetBytesFromBase64", True, "benchmark" },
    { 23, "System.Text.Json/System.Text.Json.JsonElement::TryGetSByte:System.Boolean(System.SByte)", "codegen", "System.Text.Json.JsonElement", "TryGetSByte", True, "benchmark" },
    { 24, "System.Text.Json/System.Text.Json.JsonElement::TryGetSByte:System.Boolean(System.SByte)", "codegen", "System.Text.Json.JsonElement", "TryGetSByte", True, "benchmark" },
    { 25, "System.Text.Json/System.Text.Json.JsonElement::GetSByte:System.SByte()", "codegen", "System.Text.Json.JsonElement", "GetSByte", True, "benchmark" },
    { 26, "System.Text.Json/System.Text.Json.JsonElement::TryGetByte:System.Boolean(System.Byte)", "codegen", "System.Text.Json.JsonElement", "TryGetByte", True, "benchmark" },
    { 27, "System.Text.Json/System.Text.Json.JsonElement::TryGetByte:System.Boolean(System.Byte)", "codegen", "System.Text.Json.JsonElement", "TryGetByte", True, "benchmark" },
    { 28, "System.Text.Json/System.Text.Json.JsonElement::GetByte:System.Byte()", "codegen", "System.Text.Json.JsonElement", "GetByte", True, "benchmark" },
    { 29, "System.Text.Json/System.Text.Json.JsonElement::TryGetInt16:System.Boolean(System.Int16)", "codegen", "System.Text.Json.JsonElement", "TryGetInt16", True, "benchmark" },
    { 30, "System.Text.Json/System.Text.Json.JsonElement::TryGetInt16:System.Boolean(System.Int16)", "codegen", "System.Text.Json.JsonElement", "TryGetInt16", True, "benchmark" },
    { 31, "System.Text.Json/System.Text.Json.JsonElement::GetInt16:System.Int16()", "codegen", "System.Text.Json.JsonElement", "GetInt16", True, "benchmark" },
    { 32, "System.Text.Json/System.Text.Json.JsonElement::TryGetUInt16:System.Boolean(System.UInt16)", "codegen", "System.Text.Json.JsonElement", "TryGetUInt16", True, "benchmark" },
    { 33, "System.Text.Json/System.Text.Json.JsonElement::TryGetUInt16:System.Boolean(System.UInt16)", "codegen", "System.Text.Json.JsonElement", "TryGetUInt16", True, "benchmark" },
    { 34, "System.Text.Json/System.Text.Json.JsonElement::GetUInt16:System.UInt16()", "codegen", "System.Text.Json.JsonElement", "GetUInt16", True, "benchmark" },
    { 35, "System.Text.Json/System.Text.Json.JsonElement::TryGetInt32:System.Boolean(System.Int32)", "codegen", "System.Text.Json.JsonElement", "TryGetInt32", True, "benchmark" },
    { 36, "System.Text.Json/System.Text.Json.JsonElement::TryGetInt32:System.Boolean(System.Int32)", "codegen", "System.Text.Json.JsonElement", "TryGetInt32", True, "benchmark" },
    { 37, "System.Text.Json/System.Text.Json.JsonElement::GetInt32:System.Int32()", "codegen", "System.Text.Json.JsonElement", "GetInt32", True, "benchmark" },
    { 38, "System.Text.Json/System.Text.Json.JsonElement::TryGetUInt32:System.Boolean(System.UInt32)", "codegen", "System.Text.Json.JsonElement", "TryGetUInt32", True, "benchmark" },
    { 39, "System.Text.Json/System.Text.Json.JsonElement::TryGetUInt32:System.Boolean(System.UInt32)", "codegen", "System.Text.Json.JsonElement", "TryGetUInt32", True, "benchmark" },
    { 40, "System.Text.Json/System.Text.Json.JsonElement::GetUInt32:System.UInt32()", "codegen", "System.Text.Json.JsonElement", "GetUInt32", True, "benchmark" },
    { 41, "System.Text.Json/System.Text.Json.JsonElement::TryGetInt64:System.Boolean(System.Int64)", "codegen", "System.Text.Json.JsonElement", "TryGetInt64", True, "benchmark" },
    { 42, "System.Text.Json/System.Text.Json.JsonElement::TryGetInt64:System.Boolean(System.Int64)", "codegen", "System.Text.Json.JsonElement", "TryGetInt64", True, "benchmark" },
    { 43, "System.Text.Json/System.Text.Json.JsonElement::GetInt64:System.Int64()", "codegen", "System.Text.Json.JsonElement", "GetInt64", True, "benchmark" },
    { 44, "System.Text.Json/System.Text.Json.JsonElement::TryGetUInt64:System.Boolean(System.UInt64)", "codegen", "System.Text.Json.JsonElement", "TryGetUInt64", True, "benchmark" },
    { 45, "System.Text.Json/System.Text.Json.JsonElement::TryGetUInt64:System.Boolean(System.UInt64)", "codegen", "System.Text.Json.JsonElement", "TryGetUInt64", True, "benchmark" },
    { 46, "System.Text.Json/System.Text.Json.JsonElement::GetUInt64:System.UInt64()", "codegen", "System.Text.Json.JsonElement", "GetUInt64", True, "benchmark" },
    { 47, "System.Text.Json/System.Text.Json.JsonElement::TryGetDouble:System.Boolean(System.Double)", "codegen", "System.Text.Json.JsonElement", "TryGetDouble", True, "benchmark" },
    { 48, "System.Text.Json/System.Text.Json.JsonElement::TryGetDouble:System.Boolean(System.Double)", "codegen", "System.Text.Json.JsonElement", "TryGetDouble", True, "benchmark" },
    { 49, "System.Text.Json/System.Text.Json.JsonElement::GetDouble:System.Double()", "codegen", "System.Text.Json.JsonElement", "GetDouble", True, "benchmark" },
    { 50, "System.Text.Json/System.Text.Json.JsonElement::TryGetSingle:System.Boolean(System.Single)", "codegen", "System.Text.Json.JsonElement", "TryGetSingle", True, "benchmark" },
    { 51, "System.Text.Json/System.Text.Json.JsonElement::TryGetSingle:System.Boolean(System.Single)", "codegen", "System.Text.Json.JsonElement", "TryGetSingle", True, "benchmark" },
    { 52, "System.Text.Json/System.Text.Json.JsonElement::GetSingle:System.Single()", "codegen", "System.Text.Json.JsonElement", "GetSingle", True, "benchmark" },
    { 53, "System.Text.Json/System.Text.Json.JsonElement::TryGetDecimal:System.Boolean(System.Decimal)", "codegen", "System.Text.Json.JsonElement", "TryGetDecimal", True, "benchmark" },
    { 54, "System.Text.Json/System.Text.Json.JsonElement::TryGetDecimal:System.Boolean(System.Decimal)", "codegen", "System.Text.Json.JsonElement", "TryGetDecimal", True, "benchmark" },
    { 55, "System.Text.Json/System.Text.Json.JsonElement::GetDecimal:System.Decimal()", "codegen", "System.Text.Json.JsonElement", "GetDecimal", True, "benchmark" },
    { 56, "System.Text.Json/System.Text.Json.JsonElement::TryGetDateTime:System.Boolean(System.DateTime)", "codegen", "System.Text.Json.JsonElement", "TryGetDateTime", True, "benchmark" },
    { 57, "System.Text.Json/System.Text.Json.JsonElement::GetDateTime:System.DateTime()", "codegen", "System.Text.Json.JsonElement", "GetDateTime", True, "benchmark" },
    { 58, "System.Text.Json/System.Text.Json.JsonElement::TryGetDateTimeOffset:System.Boolean(System.DateTimeOffset)", "codegen", "System.Text.Json.JsonElement", "TryGetDateTimeOffset", True, "benchmark" },
    { 59, "System.Text.Json/System.Text.Json.JsonElement::GetDateTimeOffset:System.DateTimeOffset()", "codegen", "System.Text.Json.JsonElement", "GetDateTimeOffset", True, "benchmark" },
    { 60, "System.Text.Json/System.Text.Json.JsonElement::TryGetGuid:System.Boolean(System.Guid)", "codegen", "System.Text.Json.JsonElement", "TryGetGuid", True, "benchmark" },
    { 61, "System.Text.Json/System.Text.Json.JsonElement::GetGuid:System.Guid()", "codegen", "System.Text.Json.JsonElement", "GetGuid", True, "benchmark" },
    { 62, "System.Text.Json/System.Text.Json.JsonElement::GetRawText:System.String()", "codegen", "System.Text.Json.JsonElement", "GetRawText", True, "benchmark" },
    { 63, "System.Text.Json/System.Text.Json.JsonElement::DeepEquals:System.Boolean(System.Text.Json.JsonElement,System.Text.Json.JsonElement)", "codegen", "System.Text.Json.JsonElement", "DeepEquals", True, "benchmark" },
    { 64, "System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.String)", "codegen", "System.Text.Json.JsonElement", "ValueEquals", True, "benchmark" },
    { 65, "System.Text.Json/System.Text.Json.JsonElement::ValueEquals:System.Boolean(System.String)", "codegen", "System.Text.Json.JsonElement", "ValueEquals", True, "benchmark" },
    { 66, "System.Text.Json/System.Text.Json.JsonElement::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter)", "codegen", "System.Text.Json.JsonElement", "WriteTo", True, "benchmark" },
    { 67, "System.Text.Json/System.Text.Json.JsonElement::Clone:System.Text.Json.JsonElement()", "codegen", "System.Text.Json.JsonElement", "Clone", True, "benchmark" },
    { 68, "System.Text.Json/System.Text.Json.JsonEncodedText::Encode:System.Text.Json.JsonEncodedText(System.String,System.Text.Encodings.Web.JavaScriptEncoder)", "codegen", "System.Text.Json.JsonEncodedText", "Encode", True, "benchmark" },
    { 69, "System.Text.Json/System.Text.Json.JsonEncodedText::Encode:System.Text.Json.JsonEncodedText(System.String,System.Text.Encodings.Web.JavaScriptEncoder)", "codegen", "System.Text.Json.JsonEncodedText", "Encode", True, "benchmark" },
    { 70, "System.Text.Json/System.Text.Json.JsonNamingPolicy::ConvertName:System.String(System.String)", "codegen", "System.Text.Json.JsonNamingPolicy", "ConvertName", True, "benchmark" },
    { 71, "System.Text.Json/System.Text.Json.JsonNamingPolicy::ConvertName:System.String(System.String)", "codegen", "System.Text.Json.JsonNamingPolicy", "ConvertName", True, "benchmark" },
    { 72, "System.Text.Json/System.Text.Json.JsonProperty::NameEquals:System.Boolean(System.String)", "codegen", "System.Text.Json.JsonProperty", "NameEquals", True, "benchmark" },
    { 73, "System.Text.Json/System.Text.Json.JsonProperty::NameEquals:System.Boolean(System.String)", "codegen", "System.Text.Json.JsonProperty", "NameEquals", True, "benchmark" },
    { 74, "System.Text.Json/System.Text.Json.JsonProperty::WriteTo:System.Void(System.Text.Json.Utf8JsonWriter)", "codegen", "System.Text.Json.JsonProperty", "WriteTo", True, "benchmark" },
    { 75, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.Text.Json.JsonDocument,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 76, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonDocument,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 77, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.Text.Json.JsonDocument,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 78, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.Text.Json.JsonElement,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 79, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.JsonElement,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 80, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.Text.Json.JsonElement,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 81, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.Text.Json.Nodes.JsonNode,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 82, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.Text.Json.Nodes.JsonNode,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 83, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.Text.Json.Nodes.JsonNode,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 84, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToDocument", True, "benchmark" },
    { 85, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToDocument", True, "benchmark" },
    { 86, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToDocument", True, "benchmark" },
    { 87, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToDocument", True, "benchmark" },
    { 88, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToDocument:System.Text.Json.JsonDocument(System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToDocument", True, "benchmark" },
    { 89, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToElement", True, "benchmark" },
    { 90, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToElement", True, "benchmark" },
    { 91, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToElement", True, "benchmark" },
    { 92, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToElement", True, "benchmark" },
    { 93, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToElement:System.Text.Json.JsonElement(System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToElement", True, "benchmark" },
    { 94, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToNode", True, "benchmark" },
    { 95, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToNode", True, "benchmark" },
    { 96, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToNode", True, "benchmark" },
    { 97, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToNode", True, "benchmark" },
    { 98, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToNode:System.Text.Json.Nodes.JsonNode(System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToNode", True, "benchmark" },
    { 99, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsync", True, "benchmark" },
    { 100, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsync", True, "benchmark" },
    { 101, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.IO.Stream,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 102, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.IO.Stream,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 103, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask<System.Object>(System.IO.Stream,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsync", True, "benchmark" },
    { 104, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask<System.Object>(System.IO.Stream,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsync", True, "benchmark" },
    { 105, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 106, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.IO.Stream,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 107, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsync", True, "benchmark" },
    { 108, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsync:System.Threading.Tasks.ValueTask<System.Int32>(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsync", True, "benchmark" },
    { 109, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 110, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 111, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable:System.Collections.Generic.IAsyncEnumerable<System.Int32>(System.IO.Stream,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsyncEnumerable", True, "benchmark" },
    { 112, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable:System.Collections.Generic.IAsyncEnumerable<System.Int32>(System.IO.Stream,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsyncEnumerable", True, "benchmark" },
    { 113, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable:System.Collections.Generic.IAsyncEnumerable<System.Int32>(System.IO.Stream,System.Boolean,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsyncEnumerable", True, "benchmark" },
    { 114, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable:System.Collections.Generic.IAsyncEnumerable<System.Int32>(System.IO.Stream,System.Boolean,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsyncEnumerable", True, "benchmark" },
    { 115, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable:System.Collections.Generic.IAsyncEnumerable<System.Int32>(System.IO.Stream,System.Boolean,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsyncEnumerable", True, "benchmark" },
    { 116, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable:System.Collections.Generic.IAsyncEnumerable<System.Int32>(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsyncEnumerable", True, "benchmark" },
    { 117, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable:System.Collections.Generic.IAsyncEnumerable<System.Int32>(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsyncEnumerable", True, "benchmark" },
    { 118, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable:System.Collections.Generic.IAsyncEnumerable<System.Int32>(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Boolean,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsyncEnumerable", True, "benchmark" },
    { 119, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable:System.Collections.Generic.IAsyncEnumerable<System.Int32>(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Boolean,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsyncEnumerable", True, "benchmark" },
    { 120, "System.Text.Json/System.Text.Json.JsonSerializer::DeserializeAsyncEnumerable:System.Collections.Generic.IAsyncEnumerable<System.Int32>(System.IO.Stream,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Boolean,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "DeserializeAsyncEnumerable", True, "benchmark" },
    { 121, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.String,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 122, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.String,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 123, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 124, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Object(System.String,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 125, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.String,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 126, "System.Text.Json/System.Text.Json.JsonSerializer::Deserialize:System.Int32(System.String,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Deserialize", True, "benchmark" },
    { 127, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte[](System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToUtf8Bytes", True, "benchmark" },
    { 128, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte[](System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToUtf8Bytes", True, "benchmark" },
    { 129, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte[](System.Object,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToUtf8Bytes", True, "benchmark" },
    { 130, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte[](System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToUtf8Bytes", True, "benchmark" },
    { 131, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeToUtf8Bytes:System.Byte[](System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "SerializeToUtf8Bytes", True, "benchmark" },
    { 132, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Int32,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 133, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Int32,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 134, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Int32,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 135, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 136, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 137, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 138, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 139, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Object,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 140, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 141, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 142, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 143, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 144, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Stream,System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 145, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 146, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 147, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.IO.Stream,System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 148, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 149, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 150, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Object,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 151, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 152, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.String(System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 153, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 154, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 155, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Int32,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 156, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Int32,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 157, "System.Text.Json/System.Text.Json.JsonSerializer::SerializeAsync:System.Threading.Tasks.Task(System.IO.Pipelines.PipeWriter,System.Object,System.Type,System.Text.Json.JsonSerializerOptions,System.Threading.CancellationToken)", "codegen", "System.Text.Json.JsonSerializer", "SerializeAsync", True, "benchmark" },
    { 158, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 159, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Int32,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 160, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Object,System.Type,System.Text.Json.JsonSerializerOptions)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 161, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 162, "System.Text.Json/System.Text.Json.JsonSerializer::Serialize:System.Void(System.Text.Json.Utf8JsonWriter,System.Int32,System.Text.Json.Serialization.Metadata.JsonTypeInfo<System.Int32>)", "codegen", "System.Text.Json.JsonSerializer", "Serialize", True, "benchmark" },
    { 163, "System.Text.Json/System.Text.Json.JsonSerializerOptions::GetConverter:System.Text.Json.Serialization.JsonConverter(System.Type)", "codegen", "System.Text.Json.JsonSerializerOptions", "GetConverter", True, "benchmark" },
    { 164, "System.Text.Json/System.Text.Json.JsonSerializerOptions::MakeReadOnly:System.Void()", "codegen", "System.Text.Json.JsonSerializerOptions", "MakeReadOnly", True, "benchmark" },
    { 165, "System.Text.Json/System.Text.Json.JsonSerializerOptions::MakeReadOnly:System.Void(System.Boolean)", "codegen", "System.Text.Json.JsonSerializerOptions", "MakeReadOnly", True, "benchmark" },
    { 166, "System.Text.Json/System.Text.Json.JsonSerializerOptions::MakeReadOnly:System.Void(System.Boolean)", "codegen", "System.Text.Json.JsonSerializerOptions", "MakeReadOnly", True, "benchmark" },
    { 167, "System.Text.Json/System.Text.Json.Utf8JsonReader::Read:System.Boolean()", "codegen", "System.Text.Json.Utf8JsonReader", "Read", True, "benchmark" },
    { 168, "System.Text.Json/System.Text.Json.Utf8JsonReader::Skip:System.Void()", "codegen", "System.Text.Json.Utf8JsonReader", "Skip", True, "benchmark" },
    { 169, "System.Text.Json/System.Text.Json.Utf8JsonReader::TrySkip:System.Boolean()", "codegen", "System.Text.Json.Utf8JsonReader", "TrySkip", True, "benchmark" },
    { 170, "System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.String)", "codegen", "System.Text.Json.Utf8JsonReader", "ValueTextEquals", True, "benchmark" },
    { 171, "System.Text.Json/System.Text.Json.Utf8JsonReader::ValueTextEquals:System.Boolean(System.String)", "codegen", "System.Text.Json.Utf8JsonReader", "ValueTextEquals", True, "benchmark" },
    { 172, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetString:System.String()", "codegen", "System.Text.Json.Utf8JsonReader", "GetString", True, "benchmark" },
    { 173, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetComment:System.String()", "codegen", "System.Text.Json.Utf8JsonReader", "GetComment", True, "benchmark" },
    { 174, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetBoolean:System.Boolean()", "codegen", "System.Text.Json.Utf8JsonReader", "GetBoolean", True, "benchmark" },
    { 175, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetBytesFromBase64:System.Byte[]()", "codegen", "System.Text.Json.Utf8JsonReader", "GetBytesFromBase64", True, "benchmark" },
    { 176, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetByte:System.Byte()", "codegen", "System.Text.Json.Utf8JsonReader", "GetByte", True, "benchmark" },
    { 177, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetSByte:System.SByte()", "codegen", "System.Text.Json.Utf8JsonReader", "GetSByte", True, "benchmark" },
    { 178, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt16:System.Int16()", "codegen", "System.Text.Json.Utf8JsonReader", "GetInt16", True, "benchmark" },
    { 179, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt32:System.Int32()", "codegen", "System.Text.Json.Utf8JsonReader", "GetInt32", True, "benchmark" },
    { 180, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetInt64:System.Int64()", "codegen", "System.Text.Json.Utf8JsonReader", "GetInt64", True, "benchmark" },
    { 181, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt16:System.UInt16()", "codegen", "System.Text.Json.Utf8JsonReader", "GetUInt16", True, "benchmark" },
    { 182, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt32:System.UInt32()", "codegen", "System.Text.Json.Utf8JsonReader", "GetUInt32", True, "benchmark" },
    { 183, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetUInt64:System.UInt64()", "codegen", "System.Text.Json.Utf8JsonReader", "GetUInt64", True, "benchmark" },
    { 184, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetSingle:System.Single()", "codegen", "System.Text.Json.Utf8JsonReader", "GetSingle", True, "benchmark" },
    { 185, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetDouble:System.Double()", "codegen", "System.Text.Json.Utf8JsonReader", "GetDouble", True, "benchmark" },
    { 186, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetDecimal:System.Decimal()", "codegen", "System.Text.Json.Utf8JsonReader", "GetDecimal", True, "benchmark" },
    { 187, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetDateTime:System.DateTime()", "codegen", "System.Text.Json.Utf8JsonReader", "GetDateTime", True, "benchmark" },
    { 188, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetDateTimeOffset:System.DateTimeOffset()", "codegen", "System.Text.Json.Utf8JsonReader", "GetDateTimeOffset", True, "benchmark" },
    { 189, "System.Text.Json/System.Text.Json.Utf8JsonReader::GetGuid:System.Guid()", "codegen", "System.Text.Json.Utf8JsonReader", "GetGuid", True, "benchmark" },
    { 190, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetBytesFromBase64:System.Boolean(System.Byte[])", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetBytesFromBase64", True, "benchmark" },
    { 191, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetBytesFromBase64:System.Boolean(System.Byte[])", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetBytesFromBase64", True, "benchmark" },
    { 192, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetBytesFromBase64:System.Boolean(System.Byte[])", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetBytesFromBase64", True, "benchmark" },
    { 193, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetByte:System.Boolean(System.Byte)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetByte", True, "benchmark" },
    { 194, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetByte:System.Boolean(System.Byte)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetByte", True, "benchmark" },
    { 195, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSByte:System.Boolean(System.SByte)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetSByte", True, "benchmark" },
    { 196, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSByte:System.Boolean(System.SByte)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetSByte", True, "benchmark" },
    { 197, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt16:System.Boolean(System.Int16)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetInt16", True, "benchmark" },
    { 198, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt16:System.Boolean(System.Int16)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetInt16", True, "benchmark" },
    { 199, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt32:System.Boolean(System.Int32)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetInt32", True, "benchmark" },
    { 200, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt32:System.Boolean(System.Int32)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetInt32", True, "benchmark" },
    { 201, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt64:System.Boolean(System.Int64)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetInt64", True, "benchmark" },
    { 202, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetInt64:System.Boolean(System.Int64)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetInt64", True, "benchmark" },
    { 203, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt16:System.Boolean(System.UInt16)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetUInt16", True, "benchmark" },
    { 204, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt16:System.Boolean(System.UInt16)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetUInt16", True, "benchmark" },
    { 205, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt32:System.Boolean(System.UInt32)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetUInt32", True, "benchmark" },
    { 206, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt32:System.Boolean(System.UInt32)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetUInt32", True, "benchmark" },
    { 207, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt64:System.Boolean(System.UInt64)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetUInt64", True, "benchmark" },
    { 208, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetUInt64:System.Boolean(System.UInt64)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetUInt64", True, "benchmark" },
    { 209, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSingle:System.Boolean(System.Single)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetSingle", True, "benchmark" },
    { 210, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetSingle:System.Boolean(System.Single)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetSingle", True, "benchmark" },
    { 211, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDouble:System.Boolean(System.Double)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetDouble", True, "benchmark" },
    { 212, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDouble:System.Boolean(System.Double)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetDouble", True, "benchmark" },
    { 213, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDecimal:System.Boolean(System.Decimal)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetDecimal", True, "benchmark" },
    { 214, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDecimal:System.Boolean(System.Decimal)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetDecimal", True, "benchmark" },
    { 215, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDateTime:System.Boolean(System.DateTime)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetDateTime", True, "benchmark" },
    { 216, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetDateTimeOffset:System.Boolean(System.DateTimeOffset)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetDateTimeOffset", True, "benchmark" },
    { 217, "System.Text.Json/System.Text.Json.Utf8JsonReader::TryGetGuid:System.Boolean(System.Guid)", "codegen", "System.Text.Json.Utf8JsonReader", "TryGetGuid", True, "benchmark" },
    { 218, "System.Text.Json/System.Text.Json.Utf8JsonWriter::Reset:System.Void(System.IO.Stream)", "codegen", "System.Text.Json.Utf8JsonWriter", "Reset", True, "benchmark" },
    { 219, "System.Text.Json/System.Text.Json.Utf8JsonWriter::Reset:System.Void(System.IO.Stream)", "codegen", "System.Text.Json.Utf8JsonWriter", "Reset", True, "benchmark" },
    { 220, "System.Text.Json/System.Text.Json.Utf8JsonWriter::Reset:System.Void(System.Buffers.IBufferWriter<System.Byte>)", "codegen", "System.Text.Json.Utf8JsonWriter", "Reset", True, "benchmark" },
    { 221, "System.Text.Json/System.Text.Json.Utf8JsonWriter::Reset:System.Void(System.Buffers.IBufferWriter<System.Byte>)", "codegen", "System.Text.Json.Utf8JsonWriter", "Reset", True, "benchmark" },
    { 222, "System.Text.Json/System.Text.Json.Utf8JsonWriter::Flush:System.Void()", "codegen", "System.Text.Json.Utf8JsonWriter", "Flush", True, "benchmark" },
    { 223, "System.Text.Json/System.Text.Json.Utf8JsonWriter::Dispose:System.Void()", "codegen", "System.Text.Json.Utf8JsonWriter", "Dispose", True, "benchmark" },
    { 224, "System.Text.Json/System.Text.Json.Utf8JsonWriter::DisposeAsync:System.Threading.Tasks.ValueTask()", "codegen", "System.Text.Json.Utf8JsonWriter", "DisposeAsync", True, "benchmark" },
    { 225, "System.Text.Json/System.Text.Json.Utf8JsonWriter::FlushAsync:System.Threading.Tasks.Task(System.Threading.CancellationToken)", "codegen", "System.Text.Json.Utf8JsonWriter", "FlushAsync", True, "benchmark" },
    { 226, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void()", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStartArray", True, "benchmark" },
    { 227, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void()", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStartObject", True, "benchmark" },
    { 228, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.Text.Json.JsonEncodedText)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStartArray", True, "benchmark" },
    { 229, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.Text.Json.JsonEncodedText)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStartObject", True, "benchmark" },
    { 230, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStartArray", True, "benchmark" },
    { 231, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartArray:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStartArray", True, "benchmark" },
    { 232, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStartObject", True, "benchmark" },
    { 233, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStartObject:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStartObject", True, "benchmark" },
    { 234, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteEndArray:System.Void()", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteEndArray", True, "benchmark" },
    { 235, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteEndObject:System.Void()", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteEndObject", True, "benchmark" },
    { 236, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.DateTime)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 237, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.DateTime)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 238, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.DateTime)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 239, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.DateTimeOffset)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 240, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.DateTimeOffset)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 241, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.DateTimeOffset)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 242, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Decimal)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 243, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Decimal)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 244, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Decimal)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 245, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Decimal)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 246, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Decimal)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 247, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Double)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 248, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Double)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 249, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Double)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 250, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Double)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 251, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Double)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 252, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Single)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 253, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Single)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 254, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Single)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 255, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Single)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 256, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Single)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 257, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.Guid)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 258, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.Guid)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 259, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.Guid)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 260, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.Text.Json.JsonEncodedText)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNull", True, "benchmark" },
    { 261, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNull", True, "benchmark" },
    { 262, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNull:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNull", True, "benchmark" },
    { 263, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.Text.Json.JsonEncodedText,System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteBoolean", True, "benchmark" },
    { 264, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.Text.Json.JsonEncodedText,System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteBoolean", True, "benchmark" },
    { 265, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.String,System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteBoolean", True, "benchmark" },
    { 266, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.String,System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteBoolean", True, "benchmark" },
    { 267, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBoolean:System.Void(System.String,System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteBoolean", True, "benchmark" },
    { 268, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Int64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 269, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Int64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 270, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Int64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 271, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Int64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 272, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Int64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 273, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Int32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 274, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.Int32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 275, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Int32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 276, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Int32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 277, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.Int32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 278, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.Text.Json.JsonEncodedText)", "codegen", "System.Text.Json.Utf8JsonWriter", "WritePropertyName", True, "benchmark" },
    { 279, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WritePropertyName", True, "benchmark" },
    { 280, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WritePropertyName:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WritePropertyName", True, "benchmark" },
    { 281, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.Text.Json.JsonEncodedText)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 282, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.Text.Json.JsonEncodedText)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 283, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.Text.Json.JsonEncodedText)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 284, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 285, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 286, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 287, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 288, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.Text.Json.JsonEncodedText,System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteString", True, "benchmark" },
    { 289, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.UInt64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 290, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.UInt64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 291, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.UInt64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 292, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.UInt64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 293, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.UInt64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 294, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.UInt32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 295, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.Text.Json.JsonEncodedText,System.UInt32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 296, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.UInt32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 297, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.UInt32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 298, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumber:System.Void(System.String,System.UInt32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumber", True, "benchmark" },
    { 299, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteCommentValue:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteCommentValue", True, "benchmark" },
    { 300, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteCommentValue:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteCommentValue", True, "benchmark" },
    { 301, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.DateTime)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStringValue", True, "benchmark" },
    { 302, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.DateTimeOffset)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStringValue", True, "benchmark" },
    { 303, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Decimal)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 304, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Decimal)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 305, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Double)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 306, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Double)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 307, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Single)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 308, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Single)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 309, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.Guid)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStringValue", True, "benchmark" },
    { 310, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNullValue:System.Void()", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNullValue", True, "benchmark" },
    { 311, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBooleanValue:System.Void(System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteBooleanValue", True, "benchmark" },
    { 312, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteBooleanValue:System.Void(System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteBooleanValue", True, "benchmark" },
    { 313, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.String,System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteRawValue", True, "benchmark" },
    { 314, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.String,System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteRawValue", True, "benchmark" },
    { 315, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.String,System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteRawValue", True, "benchmark" },
    { 316, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.Buffers.ReadOnlySequence<System.Byte>,System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteRawValue", True, "benchmark" },
    { 317, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteRawValue:System.Void(System.Buffers.ReadOnlySequence<System.Byte>,System.Boolean)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteRawValue", True, "benchmark" },
    { 318, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Int32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 319, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Int32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 320, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Int64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 321, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.Int64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 322, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.Text.Json.JsonEncodedText)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStringValue", True, "benchmark" },
    { 323, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStringValue", True, "benchmark" },
    { 324, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteStringValue:System.Void(System.String)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteStringValue", True, "benchmark" },
    { 325, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.UInt32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 326, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.UInt32)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 327, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.UInt64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },
    { 328, "System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteNumberValue:System.Void(System.UInt64)", "codegen", "System.Text.Json.Utf8JsonWriter", "WriteNumberValue", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 329;

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