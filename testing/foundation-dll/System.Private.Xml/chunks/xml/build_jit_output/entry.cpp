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
    { 0, "System.Private.Xml/System.Xml.NameTable::Add:System.String(System.String)", "codegen", "System.Xml.NameTable", "Add", True, "benchmark" },
    { 1, "System.Private.Xml/System.Xml.NameTable::Add:System.String(System.String)", "codegen", "System.Xml.NameTable", "Add", True, "benchmark" },
    { 2, "System.Private.Xml/System.Xml.NameTable::Add:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.NameTable", "Add", True, "benchmark" },
    { 3, "System.Private.Xml/System.Xml.NameTable::Add:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.NameTable", "Add", True, "benchmark" },
    { 4, "System.Private.Xml/System.Xml.NameTable::Add:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.NameTable", "Add", True, "benchmark" },
    { 5, "System.Private.Xml/System.Xml.NameTable::Add:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.NameTable", "Add", True, "benchmark" },
    { 6, "System.Private.Xml/System.Xml.NameTable::Add:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.NameTable", "Add", True, "benchmark" },
    { 7, "System.Private.Xml/System.Xml.NameTable::Get:System.String(System.String)", "codegen", "System.Xml.NameTable", "Get", True, "benchmark" },
    { 8, "System.Private.Xml/System.Xml.NameTable::Get:System.String(System.String)", "codegen", "System.Xml.NameTable", "Get", True, "benchmark" },
    { 9, "System.Private.Xml/System.Xml.NameTable::Get:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.NameTable", "Get", True, "benchmark" },
    { 10, "System.Private.Xml/System.Xml.NameTable::Get:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.NameTable", "Get", True, "benchmark" },
    { 11, "System.Private.Xml/System.Xml.NameTable::Get:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.NameTable", "Get", True, "benchmark" },
    { 12, "System.Private.Xml/System.Xml.NameTable::Get:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.NameTable", "Get", True, "benchmark" },
    { 13, "System.Private.Xml/System.Xml.NameTable::Get:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.NameTable", "Get", True, "benchmark" },
    { 14, "System.Private.Xml/System.Xml.XmlAttribute::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlAttribute", "CloneNode", True, "benchmark" },
    { 15, "System.Private.Xml/System.Xml.XmlAttribute::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlAttribute", "CloneNode", True, "benchmark" },
    { 16, "System.Private.Xml/System.Xml.XmlAttribute::InsertBefore:System.Xml.XmlNode(System.Xml.XmlNode,System.Xml.XmlNode)", "codegen", "System.Xml.XmlAttribute", "InsertBefore", True, "benchmark" },
    { 17, "System.Private.Xml/System.Xml.XmlAttribute::InsertAfter:System.Xml.XmlNode(System.Xml.XmlNode,System.Xml.XmlNode)", "codegen", "System.Xml.XmlAttribute", "InsertAfter", True, "benchmark" },
    { 18, "System.Private.Xml/System.Xml.XmlAttribute::ReplaceChild:System.Xml.XmlNode(System.Xml.XmlNode,System.Xml.XmlNode)", "codegen", "System.Xml.XmlAttribute", "ReplaceChild", True, "benchmark" },
    { 19, "System.Private.Xml/System.Xml.XmlAttribute::RemoveChild:System.Xml.XmlNode(System.Xml.XmlNode)", "codegen", "System.Xml.XmlAttribute", "RemoveChild", True, "benchmark" },
    { 20, "System.Private.Xml/System.Xml.XmlAttribute::PrependChild:System.Xml.XmlNode(System.Xml.XmlNode)", "codegen", "System.Xml.XmlAttribute", "PrependChild", True, "benchmark" },
    { 21, "System.Private.Xml/System.Xml.XmlAttribute::AppendChild:System.Xml.XmlNode(System.Xml.XmlNode)", "codegen", "System.Xml.XmlAttribute", "AppendChild", True, "benchmark" },
    { 22, "System.Private.Xml/System.Xml.XmlAttribute::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlAttribute", "WriteTo", True, "benchmark" },
    { 23, "System.Private.Xml/System.Xml.XmlAttribute::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlAttribute", "WriteContentTo", True, "benchmark" },
    { 24, "System.Private.Xml/System.Xml.XmlAttributeCollection::SetNamedItem:System.Xml.XmlNode(System.Xml.XmlNode)", "codegen", "System.Xml.XmlAttributeCollection", "SetNamedItem", True, "benchmark" },
    { 25, "System.Private.Xml/System.Xml.XmlAttributeCollection::Prepend:System.Xml.XmlAttribute(System.Xml.XmlAttribute)", "codegen", "System.Xml.XmlAttributeCollection", "Prepend", True, "benchmark" },
    { 26, "System.Private.Xml/System.Xml.XmlAttributeCollection::Append:System.Xml.XmlAttribute(System.Xml.XmlAttribute)", "codegen", "System.Xml.XmlAttributeCollection", "Append", True, "benchmark" },
    { 27, "System.Private.Xml/System.Xml.XmlAttributeCollection::InsertBefore:System.Xml.XmlAttribute(System.Xml.XmlAttribute,System.Xml.XmlAttribute)", "codegen", "System.Xml.XmlAttributeCollection", "InsertBefore", True, "benchmark" },
    { 28, "System.Private.Xml/System.Xml.XmlAttributeCollection::InsertAfter:System.Xml.XmlAttribute(System.Xml.XmlAttribute,System.Xml.XmlAttribute)", "codegen", "System.Xml.XmlAttributeCollection", "InsertAfter", True, "benchmark" },
    { 29, "System.Private.Xml/System.Xml.XmlAttributeCollection::Remove:System.Xml.XmlAttribute(System.Xml.XmlAttribute)", "codegen", "System.Xml.XmlAttributeCollection", "Remove", True, "benchmark" },
    { 30, "System.Private.Xml/System.Xml.XmlAttributeCollection::RemoveAt:System.Xml.XmlAttribute(System.Int32)", "codegen", "System.Xml.XmlAttributeCollection", "RemoveAt", True, "benchmark" },
    { 31, "System.Private.Xml/System.Xml.XmlAttributeCollection::RemoveAt:System.Xml.XmlAttribute(System.Int32)", "codegen", "System.Xml.XmlAttributeCollection", "RemoveAt", True, "benchmark" },
    { 32, "System.Private.Xml/System.Xml.XmlAttributeCollection::RemoveAll:System.Void()", "codegen", "System.Xml.XmlAttributeCollection", "RemoveAll", True, "benchmark" },
    { 33, "System.Private.Xml/System.Xml.XmlCDataSection::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlCDataSection", "CloneNode", True, "benchmark" },
    { 34, "System.Private.Xml/System.Xml.XmlCDataSection::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlCDataSection", "CloneNode", True, "benchmark" },
    { 35, "System.Private.Xml/System.Xml.XmlCDataSection::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlCDataSection", "WriteTo", True, "benchmark" },
    { 36, "System.Private.Xml/System.Xml.XmlCDataSection::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlCDataSection", "WriteContentTo", True, "benchmark" },
    { 37, "System.Private.Xml/System.Xml.XmlCharacterData::Substring:System.String(System.Int32,System.Int32)", "codegen", "System.Xml.XmlCharacterData", "Substring", True, "benchmark" },
    { 38, "System.Private.Xml/System.Xml.XmlCharacterData::Substring:System.String(System.Int32,System.Int32)", "codegen", "System.Xml.XmlCharacterData", "Substring", True, "benchmark" },
    { 39, "System.Private.Xml/System.Xml.XmlCharacterData::Substring:System.String(System.Int32,System.Int32)", "codegen", "System.Xml.XmlCharacterData", "Substring", True, "benchmark" },
    { 40, "System.Private.Xml/System.Xml.XmlCharacterData::AppendData:System.Void(System.String)", "codegen", "System.Xml.XmlCharacterData", "AppendData", True, "benchmark" },
    { 41, "System.Private.Xml/System.Xml.XmlCharacterData::AppendData:System.Void(System.String)", "codegen", "System.Xml.XmlCharacterData", "AppendData", True, "benchmark" },
    { 42, "System.Private.Xml/System.Xml.XmlCharacterData::InsertData:System.Void(System.Int32,System.String)", "codegen", "System.Xml.XmlCharacterData", "InsertData", True, "benchmark" },
    { 43, "System.Private.Xml/System.Xml.XmlCharacterData::InsertData:System.Void(System.Int32,System.String)", "codegen", "System.Xml.XmlCharacterData", "InsertData", True, "benchmark" },
    { 44, "System.Private.Xml/System.Xml.XmlCharacterData::InsertData:System.Void(System.Int32,System.String)", "codegen", "System.Xml.XmlCharacterData", "InsertData", True, "benchmark" },
    { 45, "System.Private.Xml/System.Xml.XmlCharacterData::DeleteData:System.Void(System.Int32,System.Int32)", "codegen", "System.Xml.XmlCharacterData", "DeleteData", True, "benchmark" },
    { 46, "System.Private.Xml/System.Xml.XmlCharacterData::DeleteData:System.Void(System.Int32,System.Int32)", "codegen", "System.Xml.XmlCharacterData", "DeleteData", True, "benchmark" },
    { 47, "System.Private.Xml/System.Xml.XmlCharacterData::DeleteData:System.Void(System.Int32,System.Int32)", "codegen", "System.Xml.XmlCharacterData", "DeleteData", True, "benchmark" },
    { 48, "System.Private.Xml/System.Xml.XmlCharacterData::ReplaceData:System.Void(System.Int32,System.Int32,System.String)", "codegen", "System.Xml.XmlCharacterData", "ReplaceData", True, "benchmark" },
    { 49, "System.Private.Xml/System.Xml.XmlCharacterData::ReplaceData:System.Void(System.Int32,System.Int32,System.String)", "codegen", "System.Xml.XmlCharacterData", "ReplaceData", True, "benchmark" },
    { 50, "System.Private.Xml/System.Xml.XmlCharacterData::ReplaceData:System.Void(System.Int32,System.Int32,System.String)", "codegen", "System.Xml.XmlCharacterData", "ReplaceData", True, "benchmark" },
    { 51, "System.Private.Xml/System.Xml.XmlCharacterData::ReplaceData:System.Void(System.Int32,System.Int32,System.String)", "codegen", "System.Xml.XmlCharacterData", "ReplaceData", True, "benchmark" },
    { 52, "System.Private.Xml/System.Xml.XmlComment::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlComment", "CloneNode", True, "benchmark" },
    { 53, "System.Private.Xml/System.Xml.XmlComment::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlComment", "CloneNode", True, "benchmark" },
    { 54, "System.Private.Xml/System.Xml.XmlComment::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlComment", "WriteTo", True, "benchmark" },
    { 55, "System.Private.Xml/System.Xml.XmlComment::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlComment", "WriteContentTo", True, "benchmark" },
    { 56, "System.Private.Xml/System.Xml.XmlConvert::EncodeName:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "EncodeName", True, "benchmark" },
    { 57, "System.Private.Xml/System.Xml.XmlConvert::EncodeName:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "EncodeName", True, "benchmark" },
    { 58, "System.Private.Xml/System.Xml.XmlConvert::EncodeNmToken:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "EncodeNmToken", True, "benchmark" },
    { 59, "System.Private.Xml/System.Xml.XmlConvert::EncodeNmToken:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "EncodeNmToken", True, "benchmark" },
    { 60, "System.Private.Xml/System.Xml.XmlConvert::EncodeLocalName:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "EncodeLocalName", True, "benchmark" },
    { 61, "System.Private.Xml/System.Xml.XmlConvert::EncodeLocalName:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "EncodeLocalName", True, "benchmark" },
    { 62, "System.Private.Xml/System.Xml.XmlConvert::DecodeName:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "DecodeName", True, "benchmark" },
    { 63, "System.Private.Xml/System.Xml.XmlConvert::DecodeName:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "DecodeName", True, "benchmark" },
    { 64, "System.Private.Xml/System.Xml.XmlConvert::VerifyName:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyName", True, "benchmark" },
    { 65, "System.Private.Xml/System.Xml.XmlConvert::VerifyName:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyName", True, "benchmark" },
    { 66, "System.Private.Xml/System.Xml.XmlConvert::VerifyNCName:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyNCName", True, "benchmark" },
    { 67, "System.Private.Xml/System.Xml.XmlConvert::VerifyNCName:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyNCName", True, "benchmark" },
    { 68, "System.Private.Xml/System.Xml.XmlConvert::VerifyTOKEN:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyTOKEN", True, "benchmark" },
    { 69, "System.Private.Xml/System.Xml.XmlConvert::VerifyTOKEN:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyTOKEN", True, "benchmark" },
    { 70, "System.Private.Xml/System.Xml.XmlConvert::VerifyNMTOKEN:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyNMTOKEN", True, "benchmark" },
    { 71, "System.Private.Xml/System.Xml.XmlConvert::VerifyNMTOKEN:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyNMTOKEN", True, "benchmark" },
    { 72, "System.Private.Xml/System.Xml.XmlConvert::VerifyXmlChars:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyXmlChars", True, "benchmark" },
    { 73, "System.Private.Xml/System.Xml.XmlConvert::VerifyXmlChars:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyXmlChars", True, "benchmark" },
    { 74, "System.Private.Xml/System.Xml.XmlConvert::VerifyPublicId:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyPublicId", True, "benchmark" },
    { 75, "System.Private.Xml/System.Xml.XmlConvert::VerifyPublicId:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyPublicId", True, "benchmark" },
    { 76, "System.Private.Xml/System.Xml.XmlConvert::VerifyWhitespace:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyWhitespace", True, "benchmark" },
    { 77, "System.Private.Xml/System.Xml.XmlConvert::VerifyWhitespace:System.String(System.String)", "codegen", "System.Xml.XmlConvert", "VerifyWhitespace", True, "benchmark" },
    { 78, "System.Private.Xml/System.Xml.XmlConvert::IsStartNCNameChar:System.Boolean(System.Char)", "codegen", "System.Xml.XmlConvert", "IsStartNCNameChar", True, "benchmark" },
    { 79, "System.Private.Xml/System.Xml.XmlConvert::IsStartNCNameChar:System.Boolean(System.Char)", "codegen", "System.Xml.XmlConvert", "IsStartNCNameChar", True, "benchmark" },
    { 80, "System.Private.Xml/System.Xml.XmlConvert::IsNCNameChar:System.Boolean(System.Char)", "codegen", "System.Xml.XmlConvert", "IsNCNameChar", True, "benchmark" },
    { 81, "System.Private.Xml/System.Xml.XmlConvert::IsNCNameChar:System.Boolean(System.Char)", "codegen", "System.Xml.XmlConvert", "IsNCNameChar", True, "benchmark" },
    { 82, "System.Private.Xml/System.Xml.XmlConvert::IsXmlChar:System.Boolean(System.Char)", "codegen", "System.Xml.XmlConvert", "IsXmlChar", True, "benchmark" },
    { 83, "System.Private.Xml/System.Xml.XmlConvert::IsXmlChar:System.Boolean(System.Char)", "codegen", "System.Xml.XmlConvert", "IsXmlChar", True, "benchmark" },
    { 84, "System.Private.Xml/System.Xml.XmlConvert::IsXmlSurrogatePair:System.Boolean(System.Char,System.Char)", "codegen", "System.Xml.XmlConvert", "IsXmlSurrogatePair", True, "benchmark" },
    { 85, "System.Private.Xml/System.Xml.XmlConvert::IsXmlSurrogatePair:System.Boolean(System.Char,System.Char)", "codegen", "System.Xml.XmlConvert", "IsXmlSurrogatePair", True, "benchmark" },
    { 86, "System.Private.Xml/System.Xml.XmlConvert::IsXmlSurrogatePair:System.Boolean(System.Char,System.Char)", "codegen", "System.Xml.XmlConvert", "IsXmlSurrogatePair", True, "benchmark" },
    { 87, "System.Private.Xml/System.Xml.XmlConvert::IsPublicIdChar:System.Boolean(System.Char)", "codegen", "System.Xml.XmlConvert", "IsPublicIdChar", True, "benchmark" },
    { 88, "System.Private.Xml/System.Xml.XmlConvert::IsPublicIdChar:System.Boolean(System.Char)", "codegen", "System.Xml.XmlConvert", "IsPublicIdChar", True, "benchmark" },
    { 89, "System.Private.Xml/System.Xml.XmlConvert::IsWhitespaceChar:System.Boolean(System.Char)", "codegen", "System.Xml.XmlConvert", "IsWhitespaceChar", True, "benchmark" },
    { 90, "System.Private.Xml/System.Xml.XmlConvert::IsWhitespaceChar:System.Boolean(System.Char)", "codegen", "System.Xml.XmlConvert", "IsWhitespaceChar", True, "benchmark" },
    { 91, "System.Private.Xml/System.Xml.XmlConvert::ToBoolean:System.Boolean(System.String)", "codegen", "System.Xml.XmlConvert", "ToBoolean", True, "benchmark" },
    { 92, "System.Private.Xml/System.Xml.XmlConvert::ToBoolean:System.Boolean(System.String)", "codegen", "System.Xml.XmlConvert", "ToBoolean", True, "benchmark" },
    { 93, "System.Private.Xml/System.Xml.XmlConvert::ToChar:System.Char(System.String)", "codegen", "System.Xml.XmlConvert", "ToChar", True, "benchmark" },
    { 94, "System.Private.Xml/System.Xml.XmlConvert::ToChar:System.Char(System.String)", "codegen", "System.Xml.XmlConvert", "ToChar", True, "benchmark" },
    { 95, "System.Private.Xml/System.Xml.XmlConvert::ToDecimal:System.Decimal(System.String)", "codegen", "System.Xml.XmlConvert", "ToDecimal", True, "benchmark" },
    { 96, "System.Private.Xml/System.Xml.XmlConvert::ToDecimal:System.Decimal(System.String)", "codegen", "System.Xml.XmlConvert", "ToDecimal", True, "benchmark" },
    { 97, "System.Private.Xml/System.Xml.XmlConvert::ToSByte:System.SByte(System.String)", "codegen", "System.Xml.XmlConvert", "ToSByte", True, "benchmark" },
    { 98, "System.Private.Xml/System.Xml.XmlConvert::ToSByte:System.SByte(System.String)", "codegen", "System.Xml.XmlConvert", "ToSByte", True, "benchmark" },
    { 99, "System.Private.Xml/System.Xml.XmlConvert::ToInt16:System.Int16(System.String)", "codegen", "System.Xml.XmlConvert", "ToInt16", True, "benchmark" },
    { 100, "System.Private.Xml/System.Xml.XmlConvert::ToInt16:System.Int16(System.String)", "codegen", "System.Xml.XmlConvert", "ToInt16", True, "benchmark" },
    { 101, "System.Private.Xml/System.Xml.XmlConvert::ToInt32:System.Int32(System.String)", "codegen", "System.Xml.XmlConvert", "ToInt32", True, "benchmark" },
    { 102, "System.Private.Xml/System.Xml.XmlConvert::ToInt32:System.Int32(System.String)", "codegen", "System.Xml.XmlConvert", "ToInt32", True, "benchmark" },
    { 103, "System.Private.Xml/System.Xml.XmlConvert::ToInt64:System.Int64(System.String)", "codegen", "System.Xml.XmlConvert", "ToInt64", True, "benchmark" },
    { 104, "System.Private.Xml/System.Xml.XmlConvert::ToInt64:System.Int64(System.String)", "codegen", "System.Xml.XmlConvert", "ToInt64", True, "benchmark" },
    { 105, "System.Private.Xml/System.Xml.XmlConvert::ToByte:System.Byte(System.String)", "codegen", "System.Xml.XmlConvert", "ToByte", True, "benchmark" },
    { 106, "System.Private.Xml/System.Xml.XmlConvert::ToByte:System.Byte(System.String)", "codegen", "System.Xml.XmlConvert", "ToByte", True, "benchmark" },
    { 107, "System.Private.Xml/System.Xml.XmlConvert::ToUInt16:System.UInt16(System.String)", "codegen", "System.Xml.XmlConvert", "ToUInt16", True, "benchmark" },
    { 108, "System.Private.Xml/System.Xml.XmlConvert::ToUInt16:System.UInt16(System.String)", "codegen", "System.Xml.XmlConvert", "ToUInt16", True, "benchmark" },
    { 109, "System.Private.Xml/System.Xml.XmlConvert::ToUInt32:System.UInt32(System.String)", "codegen", "System.Xml.XmlConvert", "ToUInt32", True, "benchmark" },
    { 110, "System.Private.Xml/System.Xml.XmlConvert::ToUInt32:System.UInt32(System.String)", "codegen", "System.Xml.XmlConvert", "ToUInt32", True, "benchmark" },
    { 111, "System.Private.Xml/System.Xml.XmlConvert::ToUInt64:System.UInt64(System.String)", "codegen", "System.Xml.XmlConvert", "ToUInt64", True, "benchmark" },
    { 112, "System.Private.Xml/System.Xml.XmlConvert::ToUInt64:System.UInt64(System.String)", "codegen", "System.Xml.XmlConvert", "ToUInt64", True, "benchmark" },
    { 113, "System.Private.Xml/System.Xml.XmlConvert::ToSingle:System.Single(System.String)", "codegen", "System.Xml.XmlConvert", "ToSingle", True, "benchmark" },
    { 114, "System.Private.Xml/System.Xml.XmlConvert::ToSingle:System.Single(System.String)", "codegen", "System.Xml.XmlConvert", "ToSingle", True, "benchmark" },
    { 115, "System.Private.Xml/System.Xml.XmlConvert::ToDouble:System.Double(System.String)", "codegen", "System.Xml.XmlConvert", "ToDouble", True, "benchmark" },
    { 116, "System.Private.Xml/System.Xml.XmlConvert::ToDouble:System.Double(System.String)", "codegen", "System.Xml.XmlConvert", "ToDouble", True, "benchmark" },
    { 117, "System.Private.Xml/System.Xml.XmlConvert::ToTimeSpan:System.TimeSpan(System.String)", "codegen", "System.Xml.XmlConvert", "ToTimeSpan", True, "benchmark" },
    { 118, "System.Private.Xml/System.Xml.XmlConvert::ToTimeSpan:System.TimeSpan(System.String)", "codegen", "System.Xml.XmlConvert", "ToTimeSpan", True, "benchmark" },
    { 119, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String)", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 120, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String)", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 121, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String,System.String)", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 122, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String,System.String)", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 123, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String,System.String)", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 124, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String,System.String[])", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 125, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String,System.String[])", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 126, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String,System.String[])", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 127, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String,System.String[])", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 128, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String,System.Xml.XmlDateTimeSerializationMode)", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 129, "System.Private.Xml/System.Xml.XmlConvert::ToDateTime:System.DateTime(System.String,System.Xml.XmlDateTimeSerializationMode)", "codegen", "System.Xml.XmlConvert", "ToDateTime", True, "benchmark" },
    { 130, "System.Private.Xml/System.Xml.XmlConvert::ToDateTimeOffset:System.DateTimeOffset(System.String)", "codegen", "System.Xml.XmlConvert", "ToDateTimeOffset", True, "benchmark" },
    { 131, "System.Private.Xml/System.Xml.XmlConvert::ToDateTimeOffset:System.DateTimeOffset(System.String)", "codegen", "System.Xml.XmlConvert", "ToDateTimeOffset", True, "benchmark" },
    { 132, "System.Private.Xml/System.Xml.XmlConvert::ToDateTimeOffset:System.DateTimeOffset(System.String,System.String)", "codegen", "System.Xml.XmlConvert", "ToDateTimeOffset", True, "benchmark" },
    { 133, "System.Private.Xml/System.Xml.XmlConvert::ToDateTimeOffset:System.DateTimeOffset(System.String,System.String)", "codegen", "System.Xml.XmlConvert", "ToDateTimeOffset", True, "benchmark" },
    { 134, "System.Private.Xml/System.Xml.XmlConvert::ToDateTimeOffset:System.DateTimeOffset(System.String,System.String)", "codegen", "System.Xml.XmlConvert", "ToDateTimeOffset", True, "benchmark" },
    { 135, "System.Private.Xml/System.Xml.XmlConvert::ToDateTimeOffset:System.DateTimeOffset(System.String,System.String[])", "codegen", "System.Xml.XmlConvert", "ToDateTimeOffset", True, "benchmark" },
    { 136, "System.Private.Xml/System.Xml.XmlConvert::ToDateTimeOffset:System.DateTimeOffset(System.String,System.String[])", "codegen", "System.Xml.XmlConvert", "ToDateTimeOffset", True, "benchmark" },
    { 137, "System.Private.Xml/System.Xml.XmlConvert::ToDateTimeOffset:System.DateTimeOffset(System.String,System.String[])", "codegen", "System.Xml.XmlConvert", "ToDateTimeOffset", True, "benchmark" },
    { 138, "System.Private.Xml/System.Xml.XmlConvert::ToDateTimeOffset:System.DateTimeOffset(System.String,System.String[])", "codegen", "System.Xml.XmlConvert", "ToDateTimeOffset", True, "benchmark" },
    { 139, "System.Private.Xml/System.Xml.XmlConvert::ToGuid:System.Guid(System.String)", "codegen", "System.Xml.XmlConvert", "ToGuid", True, "benchmark" },
    { 140, "System.Private.Xml/System.Xml.XmlConvert::ToGuid:System.Guid(System.String)", "codegen", "System.Xml.XmlConvert", "ToGuid", True, "benchmark" },
    { 141, "System.Private.Xml/System.Xml.XmlDeclaration::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlDeclaration", "CloneNode", True, "benchmark" },
    { 142, "System.Private.Xml/System.Xml.XmlDeclaration::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlDeclaration", "CloneNode", True, "benchmark" },
    { 143, "System.Private.Xml/System.Xml.XmlDeclaration::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlDeclaration", "WriteTo", True, "benchmark" },
    { 144, "System.Private.Xml/System.Xml.XmlDeclaration::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlDeclaration", "WriteContentTo", True, "benchmark" },
    { 145, "System.Private.Xml/System.Xml.XmlDocument::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlDocument", "CloneNode", True, "benchmark" },
    { 146, "System.Private.Xml/System.Xml.XmlDocument::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlDocument", "CloneNode", True, "benchmark" },
    { 147, "System.Private.Xml/System.Xml.XmlDocument::CreateAttribute:System.Xml.XmlAttribute(System.String)", "codegen", "System.Xml.XmlDocument", "CreateAttribute", True, "benchmark" },
    { 148, "System.Private.Xml/System.Xml.XmlDocument::CreateAttribute:System.Xml.XmlAttribute(System.String)", "codegen", "System.Xml.XmlDocument", "CreateAttribute", True, "benchmark" },
    { 149, "System.Private.Xml/System.Xml.XmlDocument::CreateCDataSection:System.Xml.XmlCDataSection(System.String)", "codegen", "System.Xml.XmlDocument", "CreateCDataSection", True, "benchmark" },
    { 150, "System.Private.Xml/System.Xml.XmlDocument::CreateCDataSection:System.Xml.XmlCDataSection(System.String)", "codegen", "System.Xml.XmlDocument", "CreateCDataSection", True, "benchmark" },
    { 151, "System.Private.Xml/System.Xml.XmlDocument::CreateComment:System.Xml.XmlComment(System.String)", "codegen", "System.Xml.XmlDocument", "CreateComment", True, "benchmark" },
    { 152, "System.Private.Xml/System.Xml.XmlDocument::CreateComment:System.Xml.XmlComment(System.String)", "codegen", "System.Xml.XmlDocument", "CreateComment", True, "benchmark" },
    { 153, "System.Private.Xml/System.Xml.XmlDocument::CreateDocumentType:System.Xml.XmlDocumentType(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateDocumentType", True, "benchmark" },
    { 154, "System.Private.Xml/System.Xml.XmlDocument::CreateDocumentType:System.Xml.XmlDocumentType(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateDocumentType", True, "benchmark" },
    { 155, "System.Private.Xml/System.Xml.XmlDocument::CreateDocumentType:System.Xml.XmlDocumentType(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateDocumentType", True, "benchmark" },
    { 156, "System.Private.Xml/System.Xml.XmlDocument::CreateDocumentType:System.Xml.XmlDocumentType(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateDocumentType", True, "benchmark" },
    { 157, "System.Private.Xml/System.Xml.XmlDocument::CreateDocumentFragment:System.Xml.XmlDocumentFragment()", "codegen", "System.Xml.XmlDocument", "CreateDocumentFragment", True, "benchmark" },
    { 158, "System.Private.Xml/System.Xml.XmlDocument::CreateElement:System.Xml.XmlElement(System.String)", "codegen", "System.Xml.XmlDocument", "CreateElement", True, "benchmark" },
    { 159, "System.Private.Xml/System.Xml.XmlDocument::CreateElement:System.Xml.XmlElement(System.String)", "codegen", "System.Xml.XmlDocument", "CreateElement", True, "benchmark" },
    { 160, "System.Private.Xml/System.Xml.XmlDocument::CreateEntityReference:System.Xml.XmlEntityReference(System.String)", "codegen", "System.Xml.XmlDocument", "CreateEntityReference", True, "benchmark" },
    { 161, "System.Private.Xml/System.Xml.XmlDocument::CreateEntityReference:System.Xml.XmlEntityReference(System.String)", "codegen", "System.Xml.XmlDocument", "CreateEntityReference", True, "benchmark" },
    { 162, "System.Private.Xml/System.Xml.XmlDocument::CreateProcessingInstruction:System.Xml.XmlProcessingInstruction(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateProcessingInstruction", True, "benchmark" },
    { 163, "System.Private.Xml/System.Xml.XmlDocument::CreateProcessingInstruction:System.Xml.XmlProcessingInstruction(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateProcessingInstruction", True, "benchmark" },
    { 164, "System.Private.Xml/System.Xml.XmlDocument::CreateProcessingInstruction:System.Xml.XmlProcessingInstruction(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateProcessingInstruction", True, "benchmark" },
    { 165, "System.Private.Xml/System.Xml.XmlDocument::CreateXmlDeclaration:System.Xml.XmlDeclaration(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateXmlDeclaration", True, "benchmark" },
    { 166, "System.Private.Xml/System.Xml.XmlDocument::CreateXmlDeclaration:System.Xml.XmlDeclaration(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateXmlDeclaration", True, "benchmark" },
    { 167, "System.Private.Xml/System.Xml.XmlDocument::CreateXmlDeclaration:System.Xml.XmlDeclaration(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateXmlDeclaration", True, "benchmark" },
    { 168, "System.Private.Xml/System.Xml.XmlDocument::CreateXmlDeclaration:System.Xml.XmlDeclaration(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateXmlDeclaration", True, "benchmark" },
    { 169, "System.Private.Xml/System.Xml.XmlDocument::CreateTextNode:System.Xml.XmlText(System.String)", "codegen", "System.Xml.XmlDocument", "CreateTextNode", True, "benchmark" },
    { 170, "System.Private.Xml/System.Xml.XmlDocument::CreateTextNode:System.Xml.XmlText(System.String)", "codegen", "System.Xml.XmlDocument", "CreateTextNode", True, "benchmark" },
    { 171, "System.Private.Xml/System.Xml.XmlDocument::CreateSignificantWhitespace:System.Xml.XmlSignificantWhitespace(System.String)", "codegen", "System.Xml.XmlDocument", "CreateSignificantWhitespace", True, "benchmark" },
    { 172, "System.Private.Xml/System.Xml.XmlDocument::CreateSignificantWhitespace:System.Xml.XmlSignificantWhitespace(System.String)", "codegen", "System.Xml.XmlDocument", "CreateSignificantWhitespace", True, "benchmark" },
    { 173, "System.Private.Xml/System.Xml.XmlDocument::CreateNavigator:System.Xml.XPath.XPathNavigator()", "codegen", "System.Xml.XmlDocument", "CreateNavigator", True, "benchmark" },
    { 174, "System.Private.Xml/System.Xml.XmlDocument::CreateWhitespace:System.Xml.XmlWhitespace(System.String)", "codegen", "System.Xml.XmlDocument", "CreateWhitespace", True, "benchmark" },
    { 175, "System.Private.Xml/System.Xml.XmlDocument::CreateWhitespace:System.Xml.XmlWhitespace(System.String)", "codegen", "System.Xml.XmlDocument", "CreateWhitespace", True, "benchmark" },
    { 176, "System.Private.Xml/System.Xml.XmlDocument::GetElementsByTagName:System.Xml.XmlNodeList(System.String)", "codegen", "System.Xml.XmlDocument", "GetElementsByTagName", True, "benchmark" },
    { 177, "System.Private.Xml/System.Xml.XmlDocument::GetElementsByTagName:System.Xml.XmlNodeList(System.String)", "codegen", "System.Xml.XmlDocument", "GetElementsByTagName", True, "benchmark" },
    { 178, "System.Private.Xml/System.Xml.XmlDocument::CreateAttribute:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateAttribute", True, "benchmark" },
    { 179, "System.Private.Xml/System.Xml.XmlDocument::CreateAttribute:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateAttribute", True, "benchmark" },
    { 180, "System.Private.Xml/System.Xml.XmlDocument::CreateAttribute:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateAttribute", True, "benchmark" },
    { 181, "System.Private.Xml/System.Xml.XmlDocument::CreateElement:System.Xml.XmlElement(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateElement", True, "benchmark" },
    { 182, "System.Private.Xml/System.Xml.XmlDocument::CreateElement:System.Xml.XmlElement(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateElement", True, "benchmark" },
    { 183, "System.Private.Xml/System.Xml.XmlDocument::CreateElement:System.Xml.XmlElement(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateElement", True, "benchmark" },
    { 184, "System.Private.Xml/System.Xml.XmlDocument::GetElementsByTagName:System.Xml.XmlNodeList(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "GetElementsByTagName", True, "benchmark" },
    { 185, "System.Private.Xml/System.Xml.XmlDocument::GetElementsByTagName:System.Xml.XmlNodeList(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "GetElementsByTagName", True, "benchmark" },
    { 186, "System.Private.Xml/System.Xml.XmlDocument::GetElementsByTagName:System.Xml.XmlNodeList(System.String,System.String)", "codegen", "System.Xml.XmlDocument", "GetElementsByTagName", True, "benchmark" },
    { 187, "System.Private.Xml/System.Xml.XmlDocument::GetElementById:System.Xml.XmlElement(System.String)", "codegen", "System.Xml.XmlDocument", "GetElementById", True, "benchmark" },
    { 188, "System.Private.Xml/System.Xml.XmlDocument::GetElementById:System.Xml.XmlElement(System.String)", "codegen", "System.Xml.XmlDocument", "GetElementById", True, "benchmark" },
    { 189, "System.Private.Xml/System.Xml.XmlDocument::ImportNode:System.Xml.XmlNode(System.Xml.XmlNode,System.Boolean)", "codegen", "System.Xml.XmlDocument", "ImportNode", True, "benchmark" },
    { 190, "System.Private.Xml/System.Xml.XmlDocument::ImportNode:System.Xml.XmlNode(System.Xml.XmlNode,System.Boolean)", "codegen", "System.Xml.XmlDocument", "ImportNode", True, "benchmark" },
    { 191, "System.Private.Xml/System.Xml.XmlDocument::CreateAttribute:System.Xml.XmlAttribute(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateAttribute", True, "benchmark" },
    { 192, "System.Private.Xml/System.Xml.XmlDocument::CreateAttribute:System.Xml.XmlAttribute(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateAttribute", True, "benchmark" },
    { 193, "System.Private.Xml/System.Xml.XmlDocument::CreateAttribute:System.Xml.XmlAttribute(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateAttribute", True, "benchmark" },
    { 194, "System.Private.Xml/System.Xml.XmlDocument::CreateAttribute:System.Xml.XmlAttribute(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateAttribute", True, "benchmark" },
    { 195, "System.Private.Xml/System.Xml.XmlDocument::CreateElement:System.Xml.XmlElement(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateElement", True, "benchmark" },
    { 196, "System.Private.Xml/System.Xml.XmlDocument::CreateElement:System.Xml.XmlElement(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateElement", True, "benchmark" },
    { 197, "System.Private.Xml/System.Xml.XmlDocument::CreateElement:System.Xml.XmlElement(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateElement", True, "benchmark" },
    { 198, "System.Private.Xml/System.Xml.XmlDocument::CreateElement:System.Xml.XmlElement(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateElement", True, "benchmark" },
    { 199, "System.Private.Xml/System.Xml.XmlDocument::CreateNode:System.Xml.XmlNode(System.Xml.XmlNodeType,System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateNode", True, "benchmark" },
    { 200, "System.Private.Xml/System.Xml.XmlDocument::CreateNode:System.Xml.XmlNode(System.Xml.XmlNodeType,System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateNode", True, "benchmark" },
    { 201, "System.Private.Xml/System.Xml.XmlDocument::CreateNode:System.Xml.XmlNode(System.Xml.XmlNodeType,System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateNode", True, "benchmark" },
    { 202, "System.Private.Xml/System.Xml.XmlDocument::CreateNode:System.Xml.XmlNode(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateNode", True, "benchmark" },
    { 203, "System.Private.Xml/System.Xml.XmlDocument::CreateNode:System.Xml.XmlNode(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateNode", True, "benchmark" },
    { 204, "System.Private.Xml/System.Xml.XmlDocument::CreateNode:System.Xml.XmlNode(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateNode", True, "benchmark" },
    { 205, "System.Private.Xml/System.Xml.XmlDocument::CreateNode:System.Xml.XmlNode(System.String,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateNode", True, "benchmark" },
    { 206, "System.Private.Xml/System.Xml.XmlDocument::CreateNode:System.Xml.XmlNode(System.Xml.XmlNodeType,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateNode", True, "benchmark" },
    { 207, "System.Private.Xml/System.Xml.XmlDocument::CreateNode:System.Xml.XmlNode(System.Xml.XmlNodeType,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateNode", True, "benchmark" },
    { 208, "System.Private.Xml/System.Xml.XmlDocument::CreateNode:System.Xml.XmlNode(System.Xml.XmlNodeType,System.String,System.String)", "codegen", "System.Xml.XmlDocument", "CreateNode", True, "benchmark" },
    { 209, "System.Private.Xml/System.Xml.XmlDocument::ReadNode:System.Xml.XmlNode(System.Xml.XmlReader)", "codegen", "System.Xml.XmlDocument", "ReadNode", True, "benchmark" },
    { 210, "System.Private.Xml/System.Xml.XmlDocument::Load:System.Void(System.String)", "codegen", "System.Xml.XmlDocument", "Load", True, "benchmark" },
    { 211, "System.Private.Xml/System.Xml.XmlDocument::Load:System.Void(System.String)", "codegen", "System.Xml.XmlDocument", "Load", True, "benchmark" },
    { 212, "System.Private.Xml/System.Xml.XmlDocument::Load:System.Void(System.IO.Stream)", "codegen", "System.Xml.XmlDocument", "Load", True, "benchmark" },
    { 213, "System.Private.Xml/System.Xml.XmlDocument::Load:System.Void(System.IO.Stream)", "codegen", "System.Xml.XmlDocument", "Load", True, "benchmark" },
    { 214, "System.Private.Xml/System.Xml.XmlDocument::Load:System.Void(System.IO.TextReader)", "codegen", "System.Xml.XmlDocument", "Load", True, "benchmark" },
    { 215, "System.Private.Xml/System.Xml.XmlDocument::Load:System.Void(System.IO.TextReader)", "codegen", "System.Xml.XmlDocument", "Load", True, "benchmark" },
    { 216, "System.Private.Xml/System.Xml.XmlDocument::Load:System.Void(System.Xml.XmlReader)", "codegen", "System.Xml.XmlDocument", "Load", True, "benchmark" },
    { 217, "System.Private.Xml/System.Xml.XmlDocument::LoadXml:System.Void(System.String)", "codegen", "System.Xml.XmlDocument", "LoadXml", True, "benchmark" },
    { 218, "System.Private.Xml/System.Xml.XmlDocument::LoadXml:System.Void(System.String)", "codegen", "System.Xml.XmlDocument", "LoadXml", True, "benchmark" },
    { 219, "System.Private.Xml/System.Xml.XmlDocument::Save:System.Void(System.String)", "codegen", "System.Xml.XmlDocument", "Save", True, "benchmark" },
    { 220, "System.Private.Xml/System.Xml.XmlDocument::Save:System.Void(System.String)", "codegen", "System.Xml.XmlDocument", "Save", True, "benchmark" },
    { 221, "System.Private.Xml/System.Xml.XmlDocument::Save:System.Void(System.IO.Stream)", "codegen", "System.Xml.XmlDocument", "Save", True, "benchmark" },
    { 222, "System.Private.Xml/System.Xml.XmlDocument::Save:System.Void(System.IO.Stream)", "codegen", "System.Xml.XmlDocument", "Save", True, "benchmark" },
    { 223, "System.Private.Xml/System.Xml.XmlDocument::Save:System.Void(System.IO.TextWriter)", "codegen", "System.Xml.XmlDocument", "Save", True, "benchmark" },
    { 224, "System.Private.Xml/System.Xml.XmlDocument::Save:System.Void(System.IO.TextWriter)", "codegen", "System.Xml.XmlDocument", "Save", True, "benchmark" },
    { 225, "System.Private.Xml/System.Xml.XmlDocument::Save:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlDocument", "Save", True, "benchmark" },
    { 226, "System.Private.Xml/System.Xml.XmlDocument::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlDocument", "WriteTo", True, "benchmark" },
    { 227, "System.Private.Xml/System.Xml.XmlDocument::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlDocument", "WriteContentTo", True, "benchmark" },
    { 228, "System.Private.Xml/System.Xml.XmlDocument::Validate:System.Void(System.Xml.Schema.ValidationEventHandler)", "codegen", "System.Xml.XmlDocument", "Validate", True, "benchmark" },
    { 229, "System.Private.Xml/System.Xml.XmlDocument::Validate:System.Void(System.Xml.Schema.ValidationEventHandler,System.Xml.XmlNode)", "codegen", "System.Xml.XmlDocument", "Validate", True, "benchmark" },
    { 230, "System.Private.Xml/System.Xml.XmlDocumentFragment::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlDocumentFragment", "CloneNode", True, "benchmark" },
    { 231, "System.Private.Xml/System.Xml.XmlDocumentFragment::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlDocumentFragment", "CloneNode", True, "benchmark" },
    { 232, "System.Private.Xml/System.Xml.XmlDocumentFragment::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlDocumentFragment", "WriteTo", True, "benchmark" },
    { 233, "System.Private.Xml/System.Xml.XmlDocumentFragment::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlDocumentFragment", "WriteContentTo", True, "benchmark" },
    { 234, "System.Private.Xml/System.Xml.XmlDocumentType::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlDocumentType", "CloneNode", True, "benchmark" },
    { 235, "System.Private.Xml/System.Xml.XmlDocumentType::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlDocumentType", "CloneNode", True, "benchmark" },
    { 236, "System.Private.Xml/System.Xml.XmlDocumentType::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlDocumentType", "WriteTo", True, "benchmark" },
    { 237, "System.Private.Xml/System.Xml.XmlDocumentType::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlDocumentType", "WriteContentTo", True, "benchmark" },
    { 238, "System.Private.Xml/System.Xml.XmlElement::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlElement", "CloneNode", True, "benchmark" },
    { 239, "System.Private.Xml/System.Xml.XmlElement::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlElement", "CloneNode", True, "benchmark" },
    { 240, "System.Private.Xml/System.Xml.XmlElement::GetAttribute:System.String(System.String)", "codegen", "System.Xml.XmlElement", "GetAttribute", True, "benchmark" },
    { 241, "System.Private.Xml/System.Xml.XmlElement::GetAttribute:System.String(System.String)", "codegen", "System.Xml.XmlElement", "GetAttribute", True, "benchmark" },
    { 242, "System.Private.Xml/System.Xml.XmlElement::SetAttribute:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlElement", "SetAttribute", True, "benchmark" },
    { 243, "System.Private.Xml/System.Xml.XmlElement::SetAttribute:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlElement", "SetAttribute", True, "benchmark" },
    { 244, "System.Private.Xml/System.Xml.XmlElement::SetAttribute:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlElement", "SetAttribute", True, "benchmark" },
    { 245, "System.Private.Xml/System.Xml.XmlElement::RemoveAttribute:System.Void(System.String)", "codegen", "System.Xml.XmlElement", "RemoveAttribute", True, "benchmark" },
    { 246, "System.Private.Xml/System.Xml.XmlElement::RemoveAttribute:System.Void(System.String)", "codegen", "System.Xml.XmlElement", "RemoveAttribute", True, "benchmark" },
    { 247, "System.Private.Xml/System.Xml.XmlElement::GetAttributeNode:System.Xml.XmlAttribute(System.String)", "codegen", "System.Xml.XmlElement", "GetAttributeNode", True, "benchmark" },
    { 248, "System.Private.Xml/System.Xml.XmlElement::GetAttributeNode:System.Xml.XmlAttribute(System.String)", "codegen", "System.Xml.XmlElement", "GetAttributeNode", True, "benchmark" },
    { 249, "System.Private.Xml/System.Xml.XmlElement::SetAttributeNode:System.Xml.XmlAttribute(System.Xml.XmlAttribute)", "codegen", "System.Xml.XmlElement", "SetAttributeNode", True, "benchmark" },
    { 250, "System.Private.Xml/System.Xml.XmlElement::RemoveAttributeNode:System.Xml.XmlAttribute(System.Xml.XmlAttribute)", "codegen", "System.Xml.XmlElement", "RemoveAttributeNode", True, "benchmark" },
    { 251, "System.Private.Xml/System.Xml.XmlElement::GetElementsByTagName:System.Xml.XmlNodeList(System.String)", "codegen", "System.Xml.XmlElement", "GetElementsByTagName", True, "benchmark" },
    { 252, "System.Private.Xml/System.Xml.XmlElement::GetElementsByTagName:System.Xml.XmlNodeList(System.String)", "codegen", "System.Xml.XmlElement", "GetElementsByTagName", True, "benchmark" },
    { 253, "System.Private.Xml/System.Xml.XmlElement::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlElement", "GetAttribute", True, "benchmark" },
    { 254, "System.Private.Xml/System.Xml.XmlElement::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlElement", "GetAttribute", True, "benchmark" },
    { 255, "System.Private.Xml/System.Xml.XmlElement::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlElement", "GetAttribute", True, "benchmark" },
    { 256, "System.Private.Xml/System.Xml.XmlElement::SetAttribute:System.String(System.String,System.String,System.String)", "codegen", "System.Xml.XmlElement", "SetAttribute", True, "benchmark" },
    { 257, "System.Private.Xml/System.Xml.XmlElement::SetAttribute:System.String(System.String,System.String,System.String)", "codegen", "System.Xml.XmlElement", "SetAttribute", True, "benchmark" },
    { 258, "System.Private.Xml/System.Xml.XmlElement::SetAttribute:System.String(System.String,System.String,System.String)", "codegen", "System.Xml.XmlElement", "SetAttribute", True, "benchmark" },
    { 259, "System.Private.Xml/System.Xml.XmlElement::SetAttribute:System.String(System.String,System.String,System.String)", "codegen", "System.Xml.XmlElement", "SetAttribute", True, "benchmark" },
    { 260, "System.Private.Xml/System.Xml.XmlElement::RemoveAttribute:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlElement", "RemoveAttribute", True, "benchmark" },
    { 261, "System.Private.Xml/System.Xml.XmlElement::RemoveAttribute:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlElement", "RemoveAttribute", True, "benchmark" },
    { 262, "System.Private.Xml/System.Xml.XmlElement::RemoveAttribute:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlElement", "RemoveAttribute", True, "benchmark" },
    { 263, "System.Private.Xml/System.Xml.XmlElement::GetAttributeNode:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlElement", "GetAttributeNode", True, "benchmark" },
    { 264, "System.Private.Xml/System.Xml.XmlElement::GetAttributeNode:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlElement", "GetAttributeNode", True, "benchmark" },
    { 265, "System.Private.Xml/System.Xml.XmlElement::GetAttributeNode:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlElement", "GetAttributeNode", True, "benchmark" },
    { 266, "System.Private.Xml/System.Xml.XmlElement::SetAttributeNode:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlElement", "SetAttributeNode", True, "benchmark" },
    { 267, "System.Private.Xml/System.Xml.XmlElement::SetAttributeNode:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlElement", "SetAttributeNode", True, "benchmark" },
    { 268, "System.Private.Xml/System.Xml.XmlElement::SetAttributeNode:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlElement", "SetAttributeNode", True, "benchmark" },
    { 269, "System.Private.Xml/System.Xml.XmlElement::RemoveAttributeNode:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlElement", "RemoveAttributeNode", True, "benchmark" },
    { 270, "System.Private.Xml/System.Xml.XmlElement::RemoveAttributeNode:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlElement", "RemoveAttributeNode", True, "benchmark" },
    { 271, "System.Private.Xml/System.Xml.XmlElement::RemoveAttributeNode:System.Xml.XmlAttribute(System.String,System.String)", "codegen", "System.Xml.XmlElement", "RemoveAttributeNode", True, "benchmark" },
    { 272, "System.Private.Xml/System.Xml.XmlElement::GetElementsByTagName:System.Xml.XmlNodeList(System.String,System.String)", "codegen", "System.Xml.XmlElement", "GetElementsByTagName", True, "benchmark" },
    { 273, "System.Private.Xml/System.Xml.XmlElement::GetElementsByTagName:System.Xml.XmlNodeList(System.String,System.String)", "codegen", "System.Xml.XmlElement", "GetElementsByTagName", True, "benchmark" },
    { 274, "System.Private.Xml/System.Xml.XmlElement::GetElementsByTagName:System.Xml.XmlNodeList(System.String,System.String)", "codegen", "System.Xml.XmlElement", "GetElementsByTagName", True, "benchmark" },
    { 275, "System.Private.Xml/System.Xml.XmlElement::HasAttribute:System.Boolean(System.String)", "codegen", "System.Xml.XmlElement", "HasAttribute", True, "benchmark" },
    { 276, "System.Private.Xml/System.Xml.XmlElement::HasAttribute:System.Boolean(System.String)", "codegen", "System.Xml.XmlElement", "HasAttribute", True, "benchmark" },
    { 277, "System.Private.Xml/System.Xml.XmlElement::HasAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlElement", "HasAttribute", True, "benchmark" },
    { 278, "System.Private.Xml/System.Xml.XmlElement::HasAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlElement", "HasAttribute", True, "benchmark" },
    { 279, "System.Private.Xml/System.Xml.XmlElement::HasAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlElement", "HasAttribute", True, "benchmark" },
    { 280, "System.Private.Xml/System.Xml.XmlElement::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlElement", "WriteTo", True, "benchmark" },
    { 281, "System.Private.Xml/System.Xml.XmlElement::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlElement", "WriteContentTo", True, "benchmark" },
    { 282, "System.Private.Xml/System.Xml.XmlElement::RemoveAttributeAt:System.Xml.XmlNode(System.Int32)", "codegen", "System.Xml.XmlElement", "RemoveAttributeAt", True, "benchmark" },
    { 283, "System.Private.Xml/System.Xml.XmlElement::RemoveAttributeAt:System.Xml.XmlNode(System.Int32)", "codegen", "System.Xml.XmlElement", "RemoveAttributeAt", True, "benchmark" },
    { 284, "System.Private.Xml/System.Xml.XmlElement::RemoveAllAttributes:System.Void()", "codegen", "System.Xml.XmlElement", "RemoveAllAttributes", True, "benchmark" },
    { 285, "System.Private.Xml/System.Xml.XmlElement::RemoveAll:System.Void()", "codegen", "System.Xml.XmlElement", "RemoveAll", True, "benchmark" },
    { 286, "System.Private.Xml/System.Xml.XmlEntity::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlEntity", "CloneNode", True, "benchmark" },
    { 287, "System.Private.Xml/System.Xml.XmlEntity::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlEntity", "CloneNode", True, "benchmark" },
    { 288, "System.Private.Xml/System.Xml.XmlEntity::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlEntity", "WriteTo", True, "benchmark" },
    { 289, "System.Private.Xml/System.Xml.XmlEntity::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlEntity", "WriteContentTo", True, "benchmark" },
    { 290, "System.Private.Xml/System.Xml.XmlEntityReference::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlEntityReference", "CloneNode", True, "benchmark" },
    { 291, "System.Private.Xml/System.Xml.XmlEntityReference::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlEntityReference", "CloneNode", True, "benchmark" },
    { 292, "System.Private.Xml/System.Xml.XmlEntityReference::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlEntityReference", "WriteTo", True, "benchmark" },
    { 293, "System.Private.Xml/System.Xml.XmlEntityReference::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlEntityReference", "WriteContentTo", True, "benchmark" },
    { 294, "System.Private.Xml/System.Xml.XmlImplementation::HasFeature:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlImplementation", "HasFeature", True, "benchmark" },
    { 295, "System.Private.Xml/System.Xml.XmlImplementation::HasFeature:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlImplementation", "HasFeature", True, "benchmark" },
    { 296, "System.Private.Xml/System.Xml.XmlImplementation::HasFeature:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlImplementation", "HasFeature", True, "benchmark" },
    { 297, "System.Private.Xml/System.Xml.XmlImplementation::CreateDocument:System.Xml.XmlDocument()", "codegen", "System.Xml.XmlImplementation", "CreateDocument", True, "benchmark" },
    { 298, "System.Private.Xml/System.Xml.XmlNamedNodeMap::GetNamedItem:System.Xml.XmlNode(System.String)", "codegen", "System.Xml.XmlNamedNodeMap", "GetNamedItem", True, "benchmark" },
    { 299, "System.Private.Xml/System.Xml.XmlNamedNodeMap::GetNamedItem:System.Xml.XmlNode(System.String)", "codegen", "System.Xml.XmlNamedNodeMap", "GetNamedItem", True, "benchmark" },
    { 300, "System.Private.Xml/System.Xml.XmlNamedNodeMap::SetNamedItem:System.Xml.XmlNode(System.Xml.XmlNode)", "codegen", "System.Xml.XmlNamedNodeMap", "SetNamedItem", True, "benchmark" },
    { 301, "System.Private.Xml/System.Xml.XmlNamedNodeMap::RemoveNamedItem:System.Xml.XmlNode(System.String)", "codegen", "System.Xml.XmlNamedNodeMap", "RemoveNamedItem", True, "benchmark" },
    { 302, "System.Private.Xml/System.Xml.XmlNamedNodeMap::RemoveNamedItem:System.Xml.XmlNode(System.String)", "codegen", "System.Xml.XmlNamedNodeMap", "RemoveNamedItem", True, "benchmark" },
    { 303, "System.Private.Xml/System.Xml.XmlNamedNodeMap::Item:System.Xml.XmlNode(System.Int32)", "codegen", "System.Xml.XmlNamedNodeMap", "Item", True, "benchmark" },
    { 304, "System.Private.Xml/System.Xml.XmlNamedNodeMap::Item:System.Xml.XmlNode(System.Int32)", "codegen", "System.Xml.XmlNamedNodeMap", "Item", True, "benchmark" },
    { 305, "System.Private.Xml/System.Xml.XmlNamedNodeMap::GetNamedItem:System.Xml.XmlNode(System.String,System.String)", "codegen", "System.Xml.XmlNamedNodeMap", "GetNamedItem", True, "benchmark" },
    { 306, "System.Private.Xml/System.Xml.XmlNamedNodeMap::GetNamedItem:System.Xml.XmlNode(System.String,System.String)", "codegen", "System.Xml.XmlNamedNodeMap", "GetNamedItem", True, "benchmark" },
    { 307, "System.Private.Xml/System.Xml.XmlNamedNodeMap::GetNamedItem:System.Xml.XmlNode(System.String,System.String)", "codegen", "System.Xml.XmlNamedNodeMap", "GetNamedItem", True, "benchmark" },
    { 308, "System.Private.Xml/System.Xml.XmlNamedNodeMap::RemoveNamedItem:System.Xml.XmlNode(System.String,System.String)", "codegen", "System.Xml.XmlNamedNodeMap", "RemoveNamedItem", True, "benchmark" },
    { 309, "System.Private.Xml/System.Xml.XmlNamedNodeMap::RemoveNamedItem:System.Xml.XmlNode(System.String,System.String)", "codegen", "System.Xml.XmlNamedNodeMap", "RemoveNamedItem", True, "benchmark" },
    { 310, "System.Private.Xml/System.Xml.XmlNamedNodeMap::RemoveNamedItem:System.Xml.XmlNode(System.String,System.String)", "codegen", "System.Xml.XmlNamedNodeMap", "RemoveNamedItem", True, "benchmark" },
    { 311, "System.Private.Xml/System.Xml.XmlNamedNodeMap::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Xml.XmlNamedNodeMap", "GetEnumerator", True, "benchmark" },
    { 312, "System.Private.Xml/System.Xml.XmlNamespaceManager::PushScope:System.Void()", "codegen", "System.Xml.XmlNamespaceManager", "PushScope", True, "benchmark" },
    { 313, "System.Private.Xml/System.Xml.XmlNamespaceManager::PopScope:System.Boolean()", "codegen", "System.Xml.XmlNamespaceManager", "PopScope", True, "benchmark" },
    { 314, "System.Private.Xml/System.Xml.XmlNamespaceManager::AddNamespace:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlNamespaceManager", "AddNamespace", True, "benchmark" },
    { 315, "System.Private.Xml/System.Xml.XmlNamespaceManager::AddNamespace:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlNamespaceManager", "AddNamespace", True, "benchmark" },
    { 316, "System.Private.Xml/System.Xml.XmlNamespaceManager::AddNamespace:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlNamespaceManager", "AddNamespace", True, "benchmark" },
    { 317, "System.Private.Xml/System.Xml.XmlNamespaceManager::RemoveNamespace:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlNamespaceManager", "RemoveNamespace", True, "benchmark" },
    { 318, "System.Private.Xml/System.Xml.XmlNamespaceManager::RemoveNamespace:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlNamespaceManager", "RemoveNamespace", True, "benchmark" },
    { 319, "System.Private.Xml/System.Xml.XmlNamespaceManager::RemoveNamespace:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlNamespaceManager", "RemoveNamespace", True, "benchmark" },
    { 320, "System.Private.Xml/System.Xml.XmlNamespaceManager::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Xml.XmlNamespaceManager", "GetEnumerator", True, "benchmark" },
    { 321, "System.Private.Xml/System.Xml.XmlNamespaceManager::GetNamespacesInScope:System.Collections.Generic.IDictionary<System.String,System.String>(System.Xml.XmlNamespaceScope)", "codegen", "System.Xml.XmlNamespaceManager", "GetNamespacesInScope", True, "benchmark" },
    { 322, "System.Private.Xml/System.Xml.XmlNamespaceManager::LookupNamespace:System.String(System.String)", "codegen", "System.Xml.XmlNamespaceManager", "LookupNamespace", True, "benchmark" },
    { 323, "System.Private.Xml/System.Xml.XmlNamespaceManager::LookupNamespace:System.String(System.String)", "codegen", "System.Xml.XmlNamespaceManager", "LookupNamespace", True, "benchmark" },
    { 324, "System.Private.Xml/System.Xml.XmlNamespaceManager::LookupPrefix:System.String(System.String)", "codegen", "System.Xml.XmlNamespaceManager", "LookupPrefix", True, "benchmark" },
    { 325, "System.Private.Xml/System.Xml.XmlNamespaceManager::LookupPrefix:System.String(System.String)", "codegen", "System.Xml.XmlNamespaceManager", "LookupPrefix", True, "benchmark" },
    { 326, "System.Private.Xml/System.Xml.XmlNamespaceManager::HasNamespace:System.Boolean(System.String)", "codegen", "System.Xml.XmlNamespaceManager", "HasNamespace", True, "benchmark" },
    { 327, "System.Private.Xml/System.Xml.XmlNamespaceManager::HasNamespace:System.Boolean(System.String)", "codegen", "System.Xml.XmlNamespaceManager", "HasNamespace", True, "benchmark" },
    { 328, "System.Private.Xml/System.Xml.XmlNameTable::Get:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNameTable", "Get", True, "benchmark" },
    { 329, "System.Private.Xml/System.Xml.XmlNameTable::Get:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNameTable", "Get", True, "benchmark" },
    { 330, "System.Private.Xml/System.Xml.XmlNameTable::Get:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNameTable", "Get", True, "benchmark" },
    { 331, "System.Private.Xml/System.Xml.XmlNameTable::Get:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNameTable", "Get", True, "benchmark" },
    { 332, "System.Private.Xml/System.Xml.XmlNameTable::Get:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNameTable", "Get", True, "benchmark" },
    { 333, "System.Private.Xml/System.Xml.XmlNameTable::Get:System.String(System.String)", "codegen", "System.Xml.XmlNameTable", "Get", True, "benchmark" },
    { 334, "System.Private.Xml/System.Xml.XmlNameTable::Get:System.String(System.String)", "codegen", "System.Xml.XmlNameTable", "Get", True, "benchmark" },
    { 335, "System.Private.Xml/System.Xml.XmlNameTable::Add:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNameTable", "Add", True, "benchmark" },
    { 336, "System.Private.Xml/System.Xml.XmlNameTable::Add:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNameTable", "Add", True, "benchmark" },
    { 337, "System.Private.Xml/System.Xml.XmlNameTable::Add:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNameTable", "Add", True, "benchmark" },
    { 338, "System.Private.Xml/System.Xml.XmlNameTable::Add:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNameTable", "Add", True, "benchmark" },
    { 339, "System.Private.Xml/System.Xml.XmlNameTable::Add:System.String(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNameTable", "Add", True, "benchmark" },
    { 340, "System.Private.Xml/System.Xml.XmlNameTable::Add:System.String(System.String)", "codegen", "System.Xml.XmlNameTable", "Add", True, "benchmark" },
    { 341, "System.Private.Xml/System.Xml.XmlNameTable::Add:System.String(System.String)", "codegen", "System.Xml.XmlNameTable", "Add", True, "benchmark" },
    { 342, "System.Private.Xml/System.Xml.XmlNode::CreateNavigator:System.Xml.XPath.XPathNavigator()", "codegen", "System.Xml.XmlNode", "CreateNavigator", True, "benchmark" },
    { 343, "System.Private.Xml/System.Xml.XmlNode::SelectSingleNode:System.Xml.XmlNode(System.String)", "codegen", "System.Xml.XmlNode", "SelectSingleNode", True, "benchmark" },
    { 344, "System.Private.Xml/System.Xml.XmlNode::SelectSingleNode:System.Xml.XmlNode(System.String)", "codegen", "System.Xml.XmlNode", "SelectSingleNode", True, "benchmark" },
    { 345, "System.Private.Xml/System.Xml.XmlNode::SelectSingleNode:System.Xml.XmlNode(System.String,System.Xml.XmlNamespaceManager)", "codegen", "System.Xml.XmlNode", "SelectSingleNode", True, "benchmark" },
    { 346, "System.Private.Xml/System.Xml.XmlNode::SelectSingleNode:System.Xml.XmlNode(System.String,System.Xml.XmlNamespaceManager)", "codegen", "System.Xml.XmlNode", "SelectSingleNode", True, "benchmark" },
    { 347, "System.Private.Xml/System.Xml.XmlNode::SelectNodes:System.Xml.XmlNodeList(System.String)", "codegen", "System.Xml.XmlNode", "SelectNodes", True, "benchmark" },
    { 348, "System.Private.Xml/System.Xml.XmlNode::SelectNodes:System.Xml.XmlNodeList(System.String)", "codegen", "System.Xml.XmlNode", "SelectNodes", True, "benchmark" },
    { 349, "System.Private.Xml/System.Xml.XmlNode::SelectNodes:System.Xml.XmlNodeList(System.String,System.Xml.XmlNamespaceManager)", "codegen", "System.Xml.XmlNode", "SelectNodes", True, "benchmark" },
    { 350, "System.Private.Xml/System.Xml.XmlNode::SelectNodes:System.Xml.XmlNodeList(System.String,System.Xml.XmlNamespaceManager)", "codegen", "System.Xml.XmlNode", "SelectNodes", True, "benchmark" },
    { 351, "System.Private.Xml/System.Xml.XmlNode::InsertBefore:System.Xml.XmlNode(System.Xml.XmlNode,System.Xml.XmlNode)", "codegen", "System.Xml.XmlNode", "InsertBefore", True, "benchmark" },
    { 352, "System.Private.Xml/System.Xml.XmlNode::InsertAfter:System.Xml.XmlNode(System.Xml.XmlNode,System.Xml.XmlNode)", "codegen", "System.Xml.XmlNode", "InsertAfter", True, "benchmark" },
    { 353, "System.Private.Xml/System.Xml.XmlNode::ReplaceChild:System.Xml.XmlNode(System.Xml.XmlNode,System.Xml.XmlNode)", "codegen", "System.Xml.XmlNode", "ReplaceChild", True, "benchmark" },
    { 354, "System.Private.Xml/System.Xml.XmlNode::RemoveChild:System.Xml.XmlNode(System.Xml.XmlNode)", "codegen", "System.Xml.XmlNode", "RemoveChild", True, "benchmark" },
    { 355, "System.Private.Xml/System.Xml.XmlNode::PrependChild:System.Xml.XmlNode(System.Xml.XmlNode)", "codegen", "System.Xml.XmlNode", "PrependChild", True, "benchmark" },
    { 356, "System.Private.Xml/System.Xml.XmlNode::AppendChild:System.Xml.XmlNode(System.Xml.XmlNode)", "codegen", "System.Xml.XmlNode", "AppendChild", True, "benchmark" },
    { 357, "System.Private.Xml/System.Xml.XmlNode::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlNode", "CloneNode", True, "benchmark" },
    { 358, "System.Private.Xml/System.Xml.XmlNode::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlNode", "CloneNode", True, "benchmark" },
    { 359, "System.Private.Xml/System.Xml.XmlNode::Supports:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlNode", "Supports", True, "benchmark" },
    { 360, "System.Private.Xml/System.Xml.XmlNode::Supports:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlNode", "Supports", True, "benchmark" },
    { 361, "System.Private.Xml/System.Xml.XmlNode::Supports:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlNode", "Supports", True, "benchmark" },
    { 362, "System.Private.Xml/System.Xml.XmlNode::Clone:System.Xml.XmlNode()", "codegen", "System.Xml.XmlNode", "Clone", True, "benchmark" },
    { 363, "System.Private.Xml/System.Xml.XmlNode::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Xml.XmlNode", "GetEnumerator", True, "benchmark" },
    { 364, "System.Private.Xml/System.Xml.XmlNode::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlNode", "WriteTo", True, "benchmark" },
    { 365, "System.Private.Xml/System.Xml.XmlNode::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlNode", "WriteContentTo", True, "benchmark" },
    { 366, "System.Private.Xml/System.Xml.XmlNode::RemoveAll:System.Void()", "codegen", "System.Xml.XmlNode", "RemoveAll", True, "benchmark" },
    { 367, "System.Private.Xml/System.Xml.XmlNode::GetNamespaceOfPrefix:System.String(System.String)", "codegen", "System.Xml.XmlNode", "GetNamespaceOfPrefix", True, "benchmark" },
    { 368, "System.Private.Xml/System.Xml.XmlNode::GetNamespaceOfPrefix:System.String(System.String)", "codegen", "System.Xml.XmlNode", "GetNamespaceOfPrefix", True, "benchmark" },
    { 369, "System.Private.Xml/System.Xml.XmlNode::GetPrefixOfNamespace:System.String(System.String)", "codegen", "System.Xml.XmlNode", "GetPrefixOfNamespace", True, "benchmark" },
    { 370, "System.Private.Xml/System.Xml.XmlNode::GetPrefixOfNamespace:System.String(System.String)", "codegen", "System.Xml.XmlNode", "GetPrefixOfNamespace", True, "benchmark" },
    { 371, "System.Private.Xml/System.Xml.XmlNodeList::Item:System.Xml.XmlNode(System.Int32)", "codegen", "System.Xml.XmlNodeList", "Item", True, "benchmark" },
    { 372, "System.Private.Xml/System.Xml.XmlNodeList::Item:System.Xml.XmlNode(System.Int32)", "codegen", "System.Xml.XmlNodeList", "Item", True, "benchmark" },
    { 373, "System.Private.Xml/System.Xml.XmlNodeList::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.Xml.XmlNodeList", "GetEnumerator", True, "benchmark" },
    { 374, "System.Private.Xml/System.Xml.XmlNodeReader::GetAttribute:System.String(System.String)", "codegen", "System.Xml.XmlNodeReader", "GetAttribute", True, "benchmark" },
    { 375, "System.Private.Xml/System.Xml.XmlNodeReader::GetAttribute:System.String(System.String)", "codegen", "System.Xml.XmlNodeReader", "GetAttribute", True, "benchmark" },
    { 376, "System.Private.Xml/System.Xml.XmlNodeReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlNodeReader", "GetAttribute", True, "benchmark" },
    { 377, "System.Private.Xml/System.Xml.XmlNodeReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlNodeReader", "GetAttribute", True, "benchmark" },
    { 378, "System.Private.Xml/System.Xml.XmlNodeReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlNodeReader", "GetAttribute", True, "benchmark" },
    { 379, "System.Private.Xml/System.Xml.XmlNodeReader::GetAttribute:System.String(System.Int32)", "codegen", "System.Xml.XmlNodeReader", "GetAttribute", True, "benchmark" },
    { 380, "System.Private.Xml/System.Xml.XmlNodeReader::GetAttribute:System.String(System.Int32)", "codegen", "System.Xml.XmlNodeReader", "GetAttribute", True, "benchmark" },
    { 381, "System.Private.Xml/System.Xml.XmlNodeReader::MoveToAttribute:System.Boolean(System.String)", "codegen", "System.Xml.XmlNodeReader", "MoveToAttribute", True, "benchmark" },
    { 382, "System.Private.Xml/System.Xml.XmlNodeReader::MoveToAttribute:System.Boolean(System.String)", "codegen", "System.Xml.XmlNodeReader", "MoveToAttribute", True, "benchmark" },
    { 383, "System.Private.Xml/System.Xml.XmlNodeReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlNodeReader", "MoveToAttribute", True, "benchmark" },
    { 384, "System.Private.Xml/System.Xml.XmlNodeReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlNodeReader", "MoveToAttribute", True, "benchmark" },
    { 385, "System.Private.Xml/System.Xml.XmlNodeReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlNodeReader", "MoveToAttribute", True, "benchmark" },
    { 386, "System.Private.Xml/System.Xml.XmlNodeReader::MoveToAttribute:System.Void(System.Int32)", "codegen", "System.Xml.XmlNodeReader", "MoveToAttribute", True, "benchmark" },
    { 387, "System.Private.Xml/System.Xml.XmlNodeReader::MoveToAttribute:System.Void(System.Int32)", "codegen", "System.Xml.XmlNodeReader", "MoveToAttribute", True, "benchmark" },
    { 388, "System.Private.Xml/System.Xml.XmlNodeReader::MoveToFirstAttribute:System.Boolean()", "codegen", "System.Xml.XmlNodeReader", "MoveToFirstAttribute", True, "benchmark" },
    { 389, "System.Private.Xml/System.Xml.XmlNodeReader::MoveToNextAttribute:System.Boolean()", "codegen", "System.Xml.XmlNodeReader", "MoveToNextAttribute", True, "benchmark" },
    { 390, "System.Private.Xml/System.Xml.XmlNodeReader::MoveToElement:System.Boolean()", "codegen", "System.Xml.XmlNodeReader", "MoveToElement", True, "benchmark" },
    { 391, "System.Private.Xml/System.Xml.XmlNodeReader::Read:System.Boolean()", "codegen", "System.Xml.XmlNodeReader", "Read", True, "benchmark" },
    { 392, "System.Private.Xml/System.Xml.XmlNodeReader::Close:System.Void()", "codegen", "System.Xml.XmlNodeReader", "Close", True, "benchmark" },
    { 393, "System.Private.Xml/System.Xml.XmlNodeReader::Skip:System.Void()", "codegen", "System.Xml.XmlNodeReader", "Skip", True, "benchmark" },
    { 394, "System.Private.Xml/System.Xml.XmlNodeReader::ReadString:System.String()", "codegen", "System.Xml.XmlNodeReader", "ReadString", True, "benchmark" },
    { 395, "System.Private.Xml/System.Xml.XmlNodeReader::LookupNamespace:System.String(System.String)", "codegen", "System.Xml.XmlNodeReader", "LookupNamespace", True, "benchmark" },
    { 396, "System.Private.Xml/System.Xml.XmlNodeReader::LookupNamespace:System.String(System.String)", "codegen", "System.Xml.XmlNodeReader", "LookupNamespace", True, "benchmark" },
    { 397, "System.Private.Xml/System.Xml.XmlNodeReader::ResolveEntity:System.Void()", "codegen", "System.Xml.XmlNodeReader", "ResolveEntity", True, "benchmark" },
    { 398, "System.Private.Xml/System.Xml.XmlNodeReader::ReadAttributeValue:System.Boolean()", "codegen", "System.Xml.XmlNodeReader", "ReadAttributeValue", True, "benchmark" },
    { 399, "System.Private.Xml/System.Xml.XmlNodeReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadContentAsBase64", True, "benchmark" },
    { 400, "System.Private.Xml/System.Xml.XmlNodeReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadContentAsBase64", True, "benchmark" },
    { 401, "System.Private.Xml/System.Xml.XmlNodeReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadContentAsBase64", True, "benchmark" },
    { 402, "System.Private.Xml/System.Xml.XmlNodeReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadContentAsBase64", True, "benchmark" },
    { 403, "System.Private.Xml/System.Xml.XmlNodeReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadContentAsBase64", True, "benchmark" },
    { 404, "System.Private.Xml/System.Xml.XmlNodeReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadContentAsBinHex", True, "benchmark" },
    { 405, "System.Private.Xml/System.Xml.XmlNodeReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadContentAsBinHex", True, "benchmark" },
    { 406, "System.Private.Xml/System.Xml.XmlNodeReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadContentAsBinHex", True, "benchmark" },
    { 407, "System.Private.Xml/System.Xml.XmlNodeReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadContentAsBinHex", True, "benchmark" },
    { 408, "System.Private.Xml/System.Xml.XmlNodeReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadContentAsBinHex", True, "benchmark" },
    { 409, "System.Private.Xml/System.Xml.XmlNodeReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 410, "System.Private.Xml/System.Xml.XmlNodeReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 411, "System.Private.Xml/System.Xml.XmlNodeReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 412, "System.Private.Xml/System.Xml.XmlNodeReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 413, "System.Private.Xml/System.Xml.XmlNodeReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 414, "System.Private.Xml/System.Xml.XmlNodeReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 415, "System.Private.Xml/System.Xml.XmlNodeReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 416, "System.Private.Xml/System.Xml.XmlNodeReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 417, "System.Private.Xml/System.Xml.XmlNodeReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 418, "System.Private.Xml/System.Xml.XmlNodeReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlNodeReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 419, "System.Private.Xml/System.Xml.XmlNotation::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlNotation", "CloneNode", True, "benchmark" },
    { 420, "System.Private.Xml/System.Xml.XmlNotation::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlNotation", "CloneNode", True, "benchmark" },
    { 421, "System.Private.Xml/System.Xml.XmlNotation::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlNotation", "WriteTo", True, "benchmark" },
    { 422, "System.Private.Xml/System.Xml.XmlNotation::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlNotation", "WriteContentTo", True, "benchmark" },
    { 423, "System.Private.Xml/System.Xml.XmlProcessingInstruction::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlProcessingInstruction", "CloneNode", True, "benchmark" },
    { 424, "System.Private.Xml/System.Xml.XmlProcessingInstruction::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlProcessingInstruction", "CloneNode", True, "benchmark" },
    { 425, "System.Private.Xml/System.Xml.XmlProcessingInstruction::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlProcessingInstruction", "WriteTo", True, "benchmark" },
    { 426, "System.Private.Xml/System.Xml.XmlProcessingInstruction::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlProcessingInstruction", "WriteContentTo", True, "benchmark" },
    { 427, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsObject:System.Object()", "codegen", "System.Xml.XmlReader", "ReadContentAsObject", True, "benchmark" },
    { 428, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBoolean:System.Boolean()", "codegen", "System.Xml.XmlReader", "ReadContentAsBoolean", True, "benchmark" },
    { 429, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsDateTime:System.DateTime()", "codegen", "System.Xml.XmlReader", "ReadContentAsDateTime", True, "benchmark" },
    { 430, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsDateTimeOffset:System.DateTimeOffset()", "codegen", "System.Xml.XmlReader", "ReadContentAsDateTimeOffset", True, "benchmark" },
    { 431, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsDouble:System.Double()", "codegen", "System.Xml.XmlReader", "ReadContentAsDouble", True, "benchmark" },
    { 432, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsFloat:System.Single()", "codegen", "System.Xml.XmlReader", "ReadContentAsFloat", True, "benchmark" },
    { 433, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsDecimal:System.Decimal()", "codegen", "System.Xml.XmlReader", "ReadContentAsDecimal", True, "benchmark" },
    { 434, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsInt:System.Int32()", "codegen", "System.Xml.XmlReader", "ReadContentAsInt", True, "benchmark" },
    { 435, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsLong:System.Int64()", "codegen", "System.Xml.XmlReader", "ReadContentAsLong", True, "benchmark" },
    { 436, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsString:System.String()", "codegen", "System.Xml.XmlReader", "ReadContentAsString", True, "benchmark" },
    { 437, "System.Private.Xml/System.Xml.XmlReader::ReadContentAs:System.Object(System.Type,System.Xml.IXmlNamespaceResolver)", "codegen", "System.Xml.XmlReader", "ReadContentAs", True, "benchmark" },
    { 438, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsObject:System.Object()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsObject", True, "benchmark" },
    { 439, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsObject:System.Object(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsObject", True, "benchmark" },
    { 440, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsObject:System.Object(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsObject", True, "benchmark" },
    { 441, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsObject:System.Object(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsObject", True, "benchmark" },
    { 442, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBoolean:System.Boolean()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBoolean", True, "benchmark" },
    { 443, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBoolean:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBoolean", True, "benchmark" },
    { 444, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBoolean:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBoolean", True, "benchmark" },
    { 445, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBoolean:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBoolean", True, "benchmark" },
    { 446, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDateTime:System.DateTime()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDateTime", True, "benchmark" },
    { 447, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDateTime:System.DateTime(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDateTime", True, "benchmark" },
    { 448, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDateTime:System.DateTime(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDateTime", True, "benchmark" },
    { 449, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDateTime:System.DateTime(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDateTime", True, "benchmark" },
    { 450, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDouble:System.Double()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDouble", True, "benchmark" },
    { 451, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDouble:System.Double(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDouble", True, "benchmark" },
    { 452, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDouble:System.Double(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDouble", True, "benchmark" },
    { 453, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDouble:System.Double(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDouble", True, "benchmark" },
    { 454, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsFloat:System.Single()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsFloat", True, "benchmark" },
    { 455, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsFloat:System.Single(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsFloat", True, "benchmark" },
    { 456, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsFloat:System.Single(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsFloat", True, "benchmark" },
    { 457, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsFloat:System.Single(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsFloat", True, "benchmark" },
    { 458, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDecimal:System.Decimal()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDecimal", True, "benchmark" },
    { 459, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDecimal:System.Decimal(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDecimal", True, "benchmark" },
    { 460, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDecimal:System.Decimal(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDecimal", True, "benchmark" },
    { 461, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsDecimal:System.Decimal(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsDecimal", True, "benchmark" },
    { 462, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsInt:System.Int32()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsInt", True, "benchmark" },
    { 463, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsInt:System.Int32(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsInt", True, "benchmark" },
    { 464, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsInt:System.Int32(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsInt", True, "benchmark" },
    { 465, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsInt:System.Int32(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsInt", True, "benchmark" },
    { 466, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsLong:System.Int64()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsLong", True, "benchmark" },
    { 467, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsLong:System.Int64(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsLong", True, "benchmark" },
    { 468, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsLong:System.Int64(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsLong", True, "benchmark" },
    { 469, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsLong:System.Int64(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsLong", True, "benchmark" },
    { 470, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsString:System.String()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsString", True, "benchmark" },
    { 471, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsString:System.String(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsString", True, "benchmark" },
    { 472, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsString:System.String(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsString", True, "benchmark" },
    { 473, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsString:System.String(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsString", True, "benchmark" },
    { 474, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAs:System.Object(System.Type,System.Xml.IXmlNamespaceResolver)", "codegen", "System.Xml.XmlReader", "ReadElementContentAs", True, "benchmark" },
    { 475, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAs:System.Object(System.Type,System.Xml.IXmlNamespaceResolver,System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAs", True, "benchmark" },
    { 476, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAs:System.Object(System.Type,System.Xml.IXmlNamespaceResolver,System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementContentAs", True, "benchmark" },
    { 477, "System.Private.Xml/System.Xml.XmlReader::GetAttribute:System.String(System.String)", "codegen", "System.Xml.XmlReader", "GetAttribute", True, "benchmark" },
    { 478, "System.Private.Xml/System.Xml.XmlReader::GetAttribute:System.String(System.String)", "codegen", "System.Xml.XmlReader", "GetAttribute", True, "benchmark" },
    { 479, "System.Private.Xml/System.Xml.XmlReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlReader", "GetAttribute", True, "benchmark" },
    { 480, "System.Private.Xml/System.Xml.XmlReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlReader", "GetAttribute", True, "benchmark" },
    { 481, "System.Private.Xml/System.Xml.XmlReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlReader", "GetAttribute", True, "benchmark" },
    { 482, "System.Private.Xml/System.Xml.XmlReader::GetAttribute:System.String(System.Int32)", "codegen", "System.Xml.XmlReader", "GetAttribute", True, "benchmark" },
    { 483, "System.Private.Xml/System.Xml.XmlReader::GetAttribute:System.String(System.Int32)", "codegen", "System.Xml.XmlReader", "GetAttribute", True, "benchmark" },
    { 484, "System.Private.Xml/System.Xml.XmlReader::MoveToAttribute:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "MoveToAttribute", True, "benchmark" },
    { 485, "System.Private.Xml/System.Xml.XmlReader::MoveToAttribute:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "MoveToAttribute", True, "benchmark" },
    { 486, "System.Private.Xml/System.Xml.XmlReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "MoveToAttribute", True, "benchmark" },
    { 487, "System.Private.Xml/System.Xml.XmlReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "MoveToAttribute", True, "benchmark" },
    { 488, "System.Private.Xml/System.Xml.XmlReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "MoveToAttribute", True, "benchmark" },
    { 489, "System.Private.Xml/System.Xml.XmlReader::MoveToAttribute:System.Void(System.Int32)", "codegen", "System.Xml.XmlReader", "MoveToAttribute", True, "benchmark" },
    { 490, "System.Private.Xml/System.Xml.XmlReader::MoveToAttribute:System.Void(System.Int32)", "codegen", "System.Xml.XmlReader", "MoveToAttribute", True, "benchmark" },
    { 491, "System.Private.Xml/System.Xml.XmlReader::MoveToFirstAttribute:System.Boolean()", "codegen", "System.Xml.XmlReader", "MoveToFirstAttribute", True, "benchmark" },
    { 492, "System.Private.Xml/System.Xml.XmlReader::MoveToNextAttribute:System.Boolean()", "codegen", "System.Xml.XmlReader", "MoveToNextAttribute", True, "benchmark" },
    { 493, "System.Private.Xml/System.Xml.XmlReader::MoveToElement:System.Boolean()", "codegen", "System.Xml.XmlReader", "MoveToElement", True, "benchmark" },
    { 494, "System.Private.Xml/System.Xml.XmlReader::ReadAttributeValue:System.Boolean()", "codegen", "System.Xml.XmlReader", "ReadAttributeValue", True, "benchmark" },
    { 495, "System.Private.Xml/System.Xml.XmlReader::Read:System.Boolean()", "codegen", "System.Xml.XmlReader", "Read", True, "benchmark" },
    { 496, "System.Private.Xml/System.Xml.XmlReader::Close:System.Void()", "codegen", "System.Xml.XmlReader", "Close", True, "benchmark" },
    { 497, "System.Private.Xml/System.Xml.XmlReader::Skip:System.Void()", "codegen", "System.Xml.XmlReader", "Skip", True, "benchmark" },
    { 498, "System.Private.Xml/System.Xml.XmlReader::LookupNamespace:System.String(System.String)", "codegen", "System.Xml.XmlReader", "LookupNamespace", True, "benchmark" },
    { 499, "System.Private.Xml/System.Xml.XmlReader::LookupNamespace:System.String(System.String)", "codegen", "System.Xml.XmlReader", "LookupNamespace", True, "benchmark" },
    { 500, "System.Private.Xml/System.Xml.XmlReader::ResolveEntity:System.Void()", "codegen", "System.Xml.XmlReader", "ResolveEntity", True, "benchmark" },
    { 501, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBase64", True, "benchmark" },
    { 502, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBase64", True, "benchmark" },
    { 503, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBase64", True, "benchmark" },
    { 504, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBase64", True, "benchmark" },
    { 505, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBase64", True, "benchmark" },
    { 506, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 507, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 508, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 509, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 510, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 511, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBinHex", True, "benchmark" },
    { 512, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBinHex", True, "benchmark" },
    { 513, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBinHex", True, "benchmark" },
    { 514, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBinHex", True, "benchmark" },
    { 515, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBinHex", True, "benchmark" },
    { 516, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 517, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 518, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 519, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 520, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 521, "System.Private.Xml/System.Xml.XmlReader::ReadValueChunk:System.Int32(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadValueChunk", True, "benchmark" },
    { 522, "System.Private.Xml/System.Xml.XmlReader::ReadValueChunk:System.Int32(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadValueChunk", True, "benchmark" },
    { 523, "System.Private.Xml/System.Xml.XmlReader::ReadValueChunk:System.Int32(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadValueChunk", True, "benchmark" },
    { 524, "System.Private.Xml/System.Xml.XmlReader::ReadValueChunk:System.Int32(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadValueChunk", True, "benchmark" },
    { 525, "System.Private.Xml/System.Xml.XmlReader::ReadValueChunk:System.Int32(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadValueChunk", True, "benchmark" },
    { 526, "System.Private.Xml/System.Xml.XmlReader::ReadString:System.String()", "codegen", "System.Xml.XmlReader", "ReadString", True, "benchmark" },
    { 527, "System.Private.Xml/System.Xml.XmlReader::MoveToContent:System.Xml.XmlNodeType()", "codegen", "System.Xml.XmlReader", "MoveToContent", True, "benchmark" },
    { 528, "System.Private.Xml/System.Xml.XmlReader::ReadStartElement:System.Void()", "codegen", "System.Xml.XmlReader", "ReadStartElement", True, "benchmark" },
    { 529, "System.Private.Xml/System.Xml.XmlReader::ReadStartElement:System.Void(System.String)", "codegen", "System.Xml.XmlReader", "ReadStartElement", True, "benchmark" },
    { 530, "System.Private.Xml/System.Xml.XmlReader::ReadStartElement:System.Void(System.String)", "codegen", "System.Xml.XmlReader", "ReadStartElement", True, "benchmark" },
    { 531, "System.Private.Xml/System.Xml.XmlReader::ReadStartElement:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadStartElement", True, "benchmark" },
    { 532, "System.Private.Xml/System.Xml.XmlReader::ReadStartElement:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadStartElement", True, "benchmark" },
    { 533, "System.Private.Xml/System.Xml.XmlReader::ReadStartElement:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadStartElement", True, "benchmark" },
    { 534, "System.Private.Xml/System.Xml.XmlReader::ReadElementString:System.String()", "codegen", "System.Xml.XmlReader", "ReadElementString", True, "benchmark" },
    { 535, "System.Private.Xml/System.Xml.XmlReader::ReadElementString:System.String(System.String)", "codegen", "System.Xml.XmlReader", "ReadElementString", True, "benchmark" },
    { 536, "System.Private.Xml/System.Xml.XmlReader::ReadElementString:System.String(System.String)", "codegen", "System.Xml.XmlReader", "ReadElementString", True, "benchmark" },
    { 537, "System.Private.Xml/System.Xml.XmlReader::ReadElementString:System.String(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementString", True, "benchmark" },
    { 538, "System.Private.Xml/System.Xml.XmlReader::ReadElementString:System.String(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementString", True, "benchmark" },
    { 539, "System.Private.Xml/System.Xml.XmlReader::ReadElementString:System.String(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadElementString", True, "benchmark" },
    { 540, "System.Private.Xml/System.Xml.XmlReader::ReadEndElement:System.Void()", "codegen", "System.Xml.XmlReader", "ReadEndElement", True, "benchmark" },
    { 541, "System.Private.Xml/System.Xml.XmlReader::IsStartElement:System.Boolean()", "codegen", "System.Xml.XmlReader", "IsStartElement", True, "benchmark" },
    { 542, "System.Private.Xml/System.Xml.XmlReader::IsStartElement:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "IsStartElement", True, "benchmark" },
    { 543, "System.Private.Xml/System.Xml.XmlReader::IsStartElement:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "IsStartElement", True, "benchmark" },
    { 544, "System.Private.Xml/System.Xml.XmlReader::IsStartElement:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "IsStartElement", True, "benchmark" },
    { 545, "System.Private.Xml/System.Xml.XmlReader::IsStartElement:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "IsStartElement", True, "benchmark" },
    { 546, "System.Private.Xml/System.Xml.XmlReader::IsStartElement:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "IsStartElement", True, "benchmark" },
    { 547, "System.Private.Xml/System.Xml.XmlReader::ReadToFollowing:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "ReadToFollowing", True, "benchmark" },
    { 548, "System.Private.Xml/System.Xml.XmlReader::ReadToFollowing:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "ReadToFollowing", True, "benchmark" },
    { 549, "System.Private.Xml/System.Xml.XmlReader::ReadToFollowing:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadToFollowing", True, "benchmark" },
    { 550, "System.Private.Xml/System.Xml.XmlReader::ReadToFollowing:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadToFollowing", True, "benchmark" },
    { 551, "System.Private.Xml/System.Xml.XmlReader::ReadToFollowing:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadToFollowing", True, "benchmark" },
    { 552, "System.Private.Xml/System.Xml.XmlReader::ReadToDescendant:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "ReadToDescendant", True, "benchmark" },
    { 553, "System.Private.Xml/System.Xml.XmlReader::ReadToDescendant:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "ReadToDescendant", True, "benchmark" },
    { 554, "System.Private.Xml/System.Xml.XmlReader::ReadToDescendant:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadToDescendant", True, "benchmark" },
    { 555, "System.Private.Xml/System.Xml.XmlReader::ReadToDescendant:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadToDescendant", True, "benchmark" },
    { 556, "System.Private.Xml/System.Xml.XmlReader::ReadToDescendant:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadToDescendant", True, "benchmark" },
    { 557, "System.Private.Xml/System.Xml.XmlReader::ReadToNextSibling:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "ReadToNextSibling", True, "benchmark" },
    { 558, "System.Private.Xml/System.Xml.XmlReader::ReadToNextSibling:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "ReadToNextSibling", True, "benchmark" },
    { 559, "System.Private.Xml/System.Xml.XmlReader::ReadToNextSibling:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadToNextSibling", True, "benchmark" },
    { 560, "System.Private.Xml/System.Xml.XmlReader::ReadToNextSibling:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadToNextSibling", True, "benchmark" },
    { 561, "System.Private.Xml/System.Xml.XmlReader::ReadToNextSibling:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlReader", "ReadToNextSibling", True, "benchmark" },
    { 562, "System.Private.Xml/System.Xml.XmlReader::IsName:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "IsName", True, "benchmark" },
    { 563, "System.Private.Xml/System.Xml.XmlReader::IsName:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "IsName", True, "benchmark" },
    { 564, "System.Private.Xml/System.Xml.XmlReader::IsNameToken:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "IsNameToken", True, "benchmark" },
    { 565, "System.Private.Xml/System.Xml.XmlReader::IsNameToken:System.Boolean(System.String)", "codegen", "System.Xml.XmlReader", "IsNameToken", True, "benchmark" },
    { 566, "System.Private.Xml/System.Xml.XmlReader::ReadInnerXml:System.String()", "codegen", "System.Xml.XmlReader", "ReadInnerXml", True, "benchmark" },
    { 567, "System.Private.Xml/System.Xml.XmlReader::ReadOuterXml:System.String()", "codegen", "System.Xml.XmlReader", "ReadOuterXml", True, "benchmark" },
    { 568, "System.Private.Xml/System.Xml.XmlReader::ReadSubtree:System.Xml.XmlReader()", "codegen", "System.Xml.XmlReader", "ReadSubtree", True, "benchmark" },
    { 569, "System.Private.Xml/System.Xml.XmlReader::Dispose:System.Void()", "codegen", "System.Xml.XmlReader", "Dispose", True, "benchmark" },
    { 570, "System.Private.Xml/System.Xml.XmlReader::GetValueAsync:System.Threading.Tasks.Task<System.String>()", "codegen", "System.Xml.XmlReader", "GetValueAsync", True, "benchmark" },
    { 571, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsObjectAsync:System.Threading.Tasks.Task<System.Object>()", "codegen", "System.Xml.XmlReader", "ReadContentAsObjectAsync", True, "benchmark" },
    { 572, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsStringAsync:System.Threading.Tasks.Task<System.String>()", "codegen", "System.Xml.XmlReader", "ReadContentAsStringAsync", True, "benchmark" },
    { 573, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsAsync:System.Threading.Tasks.Task<System.Object>(System.Type,System.Xml.IXmlNamespaceResolver)", "codegen", "System.Xml.XmlReader", "ReadContentAsAsync", True, "benchmark" },
    { 574, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsObjectAsync:System.Threading.Tasks.Task<System.Object>()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsObjectAsync", True, "benchmark" },
    { 575, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsStringAsync:System.Threading.Tasks.Task<System.String>()", "codegen", "System.Xml.XmlReader", "ReadElementContentAsStringAsync", True, "benchmark" },
    { 576, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsAsync:System.Threading.Tasks.Task<System.Object>(System.Type,System.Xml.IXmlNamespaceResolver)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsAsync", True, "benchmark" },
    { 577, "System.Private.Xml/System.Xml.XmlReader::ReadAsync:System.Threading.Tasks.Task<System.Boolean>()", "codegen", "System.Xml.XmlReader", "ReadAsync", True, "benchmark" },
    { 578, "System.Private.Xml/System.Xml.XmlReader::SkipAsync:System.Threading.Tasks.Task()", "codegen", "System.Xml.XmlReader", "SkipAsync", True, "benchmark" },
    { 579, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBase64Async:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBase64Async", True, "benchmark" },
    { 580, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBase64Async:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBase64Async", True, "benchmark" },
    { 581, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBase64Async:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBase64Async", True, "benchmark" },
    { 582, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBase64Async:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBase64Async", True, "benchmark" },
    { 583, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBase64Async:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBase64Async", True, "benchmark" },
    { 584, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBase64Async:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBase64Async", True, "benchmark" },
    { 585, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBase64Async:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBase64Async", True, "benchmark" },
    { 586, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBase64Async:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBase64Async", True, "benchmark" },
    { 587, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBase64Async:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBase64Async", True, "benchmark" },
    { 588, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBase64Async:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBase64Async", True, "benchmark" },
    { 589, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBinHexAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBinHexAsync", True, "benchmark" },
    { 590, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBinHexAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBinHexAsync", True, "benchmark" },
    { 591, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBinHexAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBinHexAsync", True, "benchmark" },
    { 592, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBinHexAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBinHexAsync", True, "benchmark" },
    { 593, "System.Private.Xml/System.Xml.XmlReader::ReadContentAsBinHexAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadContentAsBinHexAsync", True, "benchmark" },
    { 594, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBinHexAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBinHexAsync", True, "benchmark" },
    { 595, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBinHexAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBinHexAsync", True, "benchmark" },
    { 596, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBinHexAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBinHexAsync", True, "benchmark" },
    { 597, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBinHexAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBinHexAsync", True, "benchmark" },
    { 598, "System.Private.Xml/System.Xml.XmlReader::ReadElementContentAsBinHexAsync:System.Threading.Tasks.Task<System.Int32>(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadElementContentAsBinHexAsync", True, "benchmark" },
    { 599, "System.Private.Xml/System.Xml.XmlReader::ReadValueChunkAsync:System.Threading.Tasks.Task<System.Int32>(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadValueChunkAsync", True, "benchmark" },
    { 600, "System.Private.Xml/System.Xml.XmlReader::ReadValueChunkAsync:System.Threading.Tasks.Task<System.Int32>(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadValueChunkAsync", True, "benchmark" },
    { 601, "System.Private.Xml/System.Xml.XmlReader::ReadValueChunkAsync:System.Threading.Tasks.Task<System.Int32>(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadValueChunkAsync", True, "benchmark" },
    { 602, "System.Private.Xml/System.Xml.XmlReader::ReadValueChunkAsync:System.Threading.Tasks.Task<System.Int32>(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadValueChunkAsync", True, "benchmark" },
    { 603, "System.Private.Xml/System.Xml.XmlReader::ReadValueChunkAsync:System.Threading.Tasks.Task<System.Int32>(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlReader", "ReadValueChunkAsync", True, "benchmark" },
    { 604, "System.Private.Xml/System.Xml.XmlReader::MoveToContentAsync:System.Threading.Tasks.Task<System.Xml.XmlNodeType>()", "codegen", "System.Xml.XmlReader", "MoveToContentAsync", True, "benchmark" },
    { 605, "System.Private.Xml/System.Xml.XmlReader::ReadInnerXmlAsync:System.Threading.Tasks.Task<System.String>()", "codegen", "System.Xml.XmlReader", "ReadInnerXmlAsync", True, "benchmark" },
    { 606, "System.Private.Xml/System.Xml.XmlReader::ReadOuterXmlAsync:System.Threading.Tasks.Task<System.String>()", "codegen", "System.Xml.XmlReader", "ReadOuterXmlAsync", True, "benchmark" },
    { 607, "System.Private.Xml/System.Xml.XmlReaderSettings::Clone:System.Xml.XmlReaderSettings()", "codegen", "System.Xml.XmlReaderSettings", "Clone", True, "benchmark" },
    { 608, "System.Private.Xml/System.Xml.XmlResolver::GetEntity:System.Object(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlResolver", "GetEntity", True, "benchmark" },
    { 609, "System.Private.Xml/System.Xml.XmlResolver::GetEntity:System.Object(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlResolver", "GetEntity", True, "benchmark" },
    { 610, "System.Private.Xml/System.Xml.XmlResolver::GetEntityAsync:System.Threading.Tasks.Task<System.Object>(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlResolver", "GetEntityAsync", True, "benchmark" },
    { 611, "System.Private.Xml/System.Xml.XmlResolver::GetEntityAsync:System.Threading.Tasks.Task<System.Object>(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlResolver", "GetEntityAsync", True, "benchmark" },
    { 612, "System.Private.Xml/System.Xml.XmlResolver::ResolveUri:System.Uri(System.Uri,System.String)", "codegen", "System.Xml.XmlResolver", "ResolveUri", True, "benchmark" },
    { 613, "System.Private.Xml/System.Xml.XmlResolver::ResolveUri:System.Uri(System.Uri,System.String)", "codegen", "System.Xml.XmlResolver", "ResolveUri", True, "benchmark" },
    { 614, "System.Private.Xml/System.Xml.XmlResolver::SupportsType:System.Boolean(System.Uri,System.Type)", "codegen", "System.Xml.XmlResolver", "SupportsType", True, "benchmark" },
    { 615, "System.Private.Xml/System.Xml.XmlSecureResolver::GetEntity:System.Object(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlSecureResolver", "GetEntity", True, "benchmark" },
    { 616, "System.Private.Xml/System.Xml.XmlSecureResolver::GetEntity:System.Object(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlSecureResolver", "GetEntity", True, "benchmark" },
    { 617, "System.Private.Xml/System.Xml.XmlSecureResolver::GetEntityAsync:System.Threading.Tasks.Task<System.Object>(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlSecureResolver", "GetEntityAsync", True, "benchmark" },
    { 618, "System.Private.Xml/System.Xml.XmlSecureResolver::GetEntityAsync:System.Threading.Tasks.Task<System.Object>(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlSecureResolver", "GetEntityAsync", True, "benchmark" },
    { 619, "System.Private.Xml/System.Xml.XmlSecureResolver::ResolveUri:System.Uri(System.Uri,System.String)", "codegen", "System.Xml.XmlSecureResolver", "ResolveUri", True, "benchmark" },
    { 620, "System.Private.Xml/System.Xml.XmlSecureResolver::ResolveUri:System.Uri(System.Uri,System.String)", "codegen", "System.Xml.XmlSecureResolver", "ResolveUri", True, "benchmark" },
    { 621, "System.Private.Xml/System.Xml.XmlSignificantWhitespace::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlSignificantWhitespace", "CloneNode", True, "benchmark" },
    { 622, "System.Private.Xml/System.Xml.XmlSignificantWhitespace::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlSignificantWhitespace", "CloneNode", True, "benchmark" },
    { 623, "System.Private.Xml/System.Xml.XmlSignificantWhitespace::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlSignificantWhitespace", "WriteTo", True, "benchmark" },
    { 624, "System.Private.Xml/System.Xml.XmlSignificantWhitespace::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlSignificantWhitespace", "WriteContentTo", True, "benchmark" },
    { 625, "System.Private.Xml/System.Xml.XmlText::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlText", "CloneNode", True, "benchmark" },
    { 626, "System.Private.Xml/System.Xml.XmlText::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlText", "CloneNode", True, "benchmark" },
    { 627, "System.Private.Xml/System.Xml.XmlText::SplitText:System.Xml.XmlText(System.Int32)", "codegen", "System.Xml.XmlText", "SplitText", True, "benchmark" },
    { 628, "System.Private.Xml/System.Xml.XmlText::SplitText:System.Xml.XmlText(System.Int32)", "codegen", "System.Xml.XmlText", "SplitText", True, "benchmark" },
    { 629, "System.Private.Xml/System.Xml.XmlText::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlText", "WriteTo", True, "benchmark" },
    { 630, "System.Private.Xml/System.Xml.XmlText::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlText", "WriteContentTo", True, "benchmark" },
    { 631, "System.Private.Xml/System.Xml.XmlTextReader::GetAttribute:System.String(System.String)", "codegen", "System.Xml.XmlTextReader", "GetAttribute", True, "benchmark" },
    { 632, "System.Private.Xml/System.Xml.XmlTextReader::GetAttribute:System.String(System.String)", "codegen", "System.Xml.XmlTextReader", "GetAttribute", True, "benchmark" },
    { 633, "System.Private.Xml/System.Xml.XmlTextReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlTextReader", "GetAttribute", True, "benchmark" },
    { 634, "System.Private.Xml/System.Xml.XmlTextReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlTextReader", "GetAttribute", True, "benchmark" },
    { 635, "System.Private.Xml/System.Xml.XmlTextReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlTextReader", "GetAttribute", True, "benchmark" },
    { 636, "System.Private.Xml/System.Xml.XmlTextReader::GetAttribute:System.String(System.Int32)", "codegen", "System.Xml.XmlTextReader", "GetAttribute", True, "benchmark" },
    { 637, "System.Private.Xml/System.Xml.XmlTextReader::GetAttribute:System.String(System.Int32)", "codegen", "System.Xml.XmlTextReader", "GetAttribute", True, "benchmark" },
    { 638, "System.Private.Xml/System.Xml.XmlTextReader::MoveToAttribute:System.Boolean(System.String)", "codegen", "System.Xml.XmlTextReader", "MoveToAttribute", True, "benchmark" },
    { 639, "System.Private.Xml/System.Xml.XmlTextReader::MoveToAttribute:System.Boolean(System.String)", "codegen", "System.Xml.XmlTextReader", "MoveToAttribute", True, "benchmark" },
    { 640, "System.Private.Xml/System.Xml.XmlTextReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlTextReader", "MoveToAttribute", True, "benchmark" },
    { 641, "System.Private.Xml/System.Xml.XmlTextReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlTextReader", "MoveToAttribute", True, "benchmark" },
    { 642, "System.Private.Xml/System.Xml.XmlTextReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlTextReader", "MoveToAttribute", True, "benchmark" },
    { 643, "System.Private.Xml/System.Xml.XmlTextReader::MoveToAttribute:System.Void(System.Int32)", "codegen", "System.Xml.XmlTextReader", "MoveToAttribute", True, "benchmark" },
    { 644, "System.Private.Xml/System.Xml.XmlTextReader::MoveToAttribute:System.Void(System.Int32)", "codegen", "System.Xml.XmlTextReader", "MoveToAttribute", True, "benchmark" },
    { 645, "System.Private.Xml/System.Xml.XmlTextReader::MoveToFirstAttribute:System.Boolean()", "codegen", "System.Xml.XmlTextReader", "MoveToFirstAttribute", True, "benchmark" },
    { 646, "System.Private.Xml/System.Xml.XmlTextReader::MoveToNextAttribute:System.Boolean()", "codegen", "System.Xml.XmlTextReader", "MoveToNextAttribute", True, "benchmark" },
    { 647, "System.Private.Xml/System.Xml.XmlTextReader::MoveToElement:System.Boolean()", "codegen", "System.Xml.XmlTextReader", "MoveToElement", True, "benchmark" },
    { 648, "System.Private.Xml/System.Xml.XmlTextReader::ReadAttributeValue:System.Boolean()", "codegen", "System.Xml.XmlTextReader", "ReadAttributeValue", True, "benchmark" },
    { 649, "System.Private.Xml/System.Xml.XmlTextReader::Read:System.Boolean()", "codegen", "System.Xml.XmlTextReader", "Read", True, "benchmark" },
    { 650, "System.Private.Xml/System.Xml.XmlTextReader::Close:System.Void()", "codegen", "System.Xml.XmlTextReader", "Close", True, "benchmark" },
    { 651, "System.Private.Xml/System.Xml.XmlTextReader::Skip:System.Void()", "codegen", "System.Xml.XmlTextReader", "Skip", True, "benchmark" },
    { 652, "System.Private.Xml/System.Xml.XmlTextReader::LookupNamespace:System.String(System.String)", "codegen", "System.Xml.XmlTextReader", "LookupNamespace", True, "benchmark" },
    { 653, "System.Private.Xml/System.Xml.XmlTextReader::LookupNamespace:System.String(System.String)", "codegen", "System.Xml.XmlTextReader", "LookupNamespace", True, "benchmark" },
    { 654, "System.Private.Xml/System.Xml.XmlTextReader::ResolveEntity:System.Void()", "codegen", "System.Xml.XmlTextReader", "ResolveEntity", True, "benchmark" },
    { 655, "System.Private.Xml/System.Xml.XmlTextReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadContentAsBase64", True, "benchmark" },
    { 656, "System.Private.Xml/System.Xml.XmlTextReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadContentAsBase64", True, "benchmark" },
    { 657, "System.Private.Xml/System.Xml.XmlTextReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadContentAsBase64", True, "benchmark" },
    { 658, "System.Private.Xml/System.Xml.XmlTextReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadContentAsBase64", True, "benchmark" },
    { 659, "System.Private.Xml/System.Xml.XmlTextReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadContentAsBase64", True, "benchmark" },
    { 660, "System.Private.Xml/System.Xml.XmlTextReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 661, "System.Private.Xml/System.Xml.XmlTextReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 662, "System.Private.Xml/System.Xml.XmlTextReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 663, "System.Private.Xml/System.Xml.XmlTextReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 664, "System.Private.Xml/System.Xml.XmlTextReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 665, "System.Private.Xml/System.Xml.XmlTextReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadContentAsBinHex", True, "benchmark" },
    { 666, "System.Private.Xml/System.Xml.XmlTextReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadContentAsBinHex", True, "benchmark" },
    { 667, "System.Private.Xml/System.Xml.XmlTextReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadContentAsBinHex", True, "benchmark" },
    { 668, "System.Private.Xml/System.Xml.XmlTextReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadContentAsBinHex", True, "benchmark" },
    { 669, "System.Private.Xml/System.Xml.XmlTextReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadContentAsBinHex", True, "benchmark" },
    { 670, "System.Private.Xml/System.Xml.XmlTextReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 671, "System.Private.Xml/System.Xml.XmlTextReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 672, "System.Private.Xml/System.Xml.XmlTextReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 673, "System.Private.Xml/System.Xml.XmlTextReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 674, "System.Private.Xml/System.Xml.XmlTextReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 675, "System.Private.Xml/System.Xml.XmlTextReader::ReadString:System.String()", "codegen", "System.Xml.XmlTextReader", "ReadString", True, "benchmark" },
    { 676, "System.Private.Xml/System.Xml.XmlTextReader::HasLineInfo:System.Boolean()", "codegen", "System.Xml.XmlTextReader", "HasLineInfo", True, "benchmark" },
    { 677, "System.Private.Xml/System.Xml.XmlTextReader::GetNamespacesInScope:System.Collections.Generic.IDictionary<System.String,System.String>(System.Xml.XmlNamespaceScope)", "codegen", "System.Xml.XmlTextReader", "GetNamespacesInScope", True, "benchmark" },
    { 678, "System.Private.Xml/System.Xml.XmlTextReader::ResetState:System.Void()", "codegen", "System.Xml.XmlTextReader", "ResetState", True, "benchmark" },
    { 679, "System.Private.Xml/System.Xml.XmlTextReader::GetRemainder:System.IO.TextReader()", "codegen", "System.Xml.XmlTextReader", "GetRemainder", True, "benchmark" },
    { 680, "System.Private.Xml/System.Xml.XmlTextReader::ReadChars:System.Int32(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadChars", True, "benchmark" },
    { 681, "System.Private.Xml/System.Xml.XmlTextReader::ReadChars:System.Int32(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadChars", True, "benchmark" },
    { 682, "System.Private.Xml/System.Xml.XmlTextReader::ReadChars:System.Int32(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadChars", True, "benchmark" },
    { 683, "System.Private.Xml/System.Xml.XmlTextReader::ReadChars:System.Int32(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadChars", True, "benchmark" },
    { 684, "System.Private.Xml/System.Xml.XmlTextReader::ReadChars:System.Int32(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadChars", True, "benchmark" },
    { 685, "System.Private.Xml/System.Xml.XmlTextReader::ReadBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadBase64", True, "benchmark" },
    { 686, "System.Private.Xml/System.Xml.XmlTextReader::ReadBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadBase64", True, "benchmark" },
    { 687, "System.Private.Xml/System.Xml.XmlTextReader::ReadBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadBase64", True, "benchmark" },
    { 688, "System.Private.Xml/System.Xml.XmlTextReader::ReadBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadBase64", True, "benchmark" },
    { 689, "System.Private.Xml/System.Xml.XmlTextReader::ReadBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadBase64", True, "benchmark" },
    { 690, "System.Private.Xml/System.Xml.XmlTextReader::ReadBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadBinHex", True, "benchmark" },
    { 691, "System.Private.Xml/System.Xml.XmlTextReader::ReadBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadBinHex", True, "benchmark" },
    { 692, "System.Private.Xml/System.Xml.XmlTextReader::ReadBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadBinHex", True, "benchmark" },
    { 693, "System.Private.Xml/System.Xml.XmlTextReader::ReadBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadBinHex", True, "benchmark" },
    { 694, "System.Private.Xml/System.Xml.XmlTextReader::ReadBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextReader", "ReadBinHex", True, "benchmark" },
    { 695, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartDocument:System.Void()", "codegen", "System.Xml.XmlTextWriter", "WriteStartDocument", True, "benchmark" },
    { 696, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartDocument:System.Void(System.Boolean)", "codegen", "System.Xml.XmlTextWriter", "WriteStartDocument", True, "benchmark" },
    { 697, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartDocument:System.Void(System.Boolean)", "codegen", "System.Xml.XmlTextWriter", "WriteStartDocument", True, "benchmark" },
    { 698, "System.Private.Xml/System.Xml.XmlTextWriter::WriteEndDocument:System.Void()", "codegen", "System.Xml.XmlTextWriter", "WriteEndDocument", True, "benchmark" },
    { 699, "System.Private.Xml/System.Xml.XmlTextWriter::WriteDocType:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteDocType", True, "benchmark" },
    { 700, "System.Private.Xml/System.Xml.XmlTextWriter::WriteDocType:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteDocType", True, "benchmark" },
    { 701, "System.Private.Xml/System.Xml.XmlTextWriter::WriteDocType:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteDocType", True, "benchmark" },
    { 702, "System.Private.Xml/System.Xml.XmlTextWriter::WriteDocType:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteDocType", True, "benchmark" },
    { 703, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartElement:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteStartElement", True, "benchmark" },
    { 704, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartElement:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteStartElement", True, "benchmark" },
    { 705, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartElement:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteStartElement", True, "benchmark" },
    { 706, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartElement:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteStartElement", True, "benchmark" },
    { 707, "System.Private.Xml/System.Xml.XmlTextWriter::WriteEndElement:System.Void()", "codegen", "System.Xml.XmlTextWriter", "WriteEndElement", True, "benchmark" },
    { 708, "System.Private.Xml/System.Xml.XmlTextWriter::WriteFullEndElement:System.Void()", "codegen", "System.Xml.XmlTextWriter", "WriteFullEndElement", True, "benchmark" },
    { 709, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartAttribute:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteStartAttribute", True, "benchmark" },
    { 710, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartAttribute:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteStartAttribute", True, "benchmark" },
    { 711, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartAttribute:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteStartAttribute", True, "benchmark" },
    { 712, "System.Private.Xml/System.Xml.XmlTextWriter::WriteStartAttribute:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteStartAttribute", True, "benchmark" },
    { 713, "System.Private.Xml/System.Xml.XmlTextWriter::WriteEndAttribute:System.Void()", "codegen", "System.Xml.XmlTextWriter", "WriteEndAttribute", True, "benchmark" },
    { 714, "System.Private.Xml/System.Xml.XmlTextWriter::WriteCData:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteCData", True, "benchmark" },
    { 715, "System.Private.Xml/System.Xml.XmlTextWriter::WriteCData:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteCData", True, "benchmark" },
    { 716, "System.Private.Xml/System.Xml.XmlTextWriter::WriteComment:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteComment", True, "benchmark" },
    { 717, "System.Private.Xml/System.Xml.XmlTextWriter::WriteComment:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteComment", True, "benchmark" },
    { 718, "System.Private.Xml/System.Xml.XmlTextWriter::WriteProcessingInstruction:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteProcessingInstruction", True, "benchmark" },
    { 719, "System.Private.Xml/System.Xml.XmlTextWriter::WriteProcessingInstruction:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteProcessingInstruction", True, "benchmark" },
    { 720, "System.Private.Xml/System.Xml.XmlTextWriter::WriteProcessingInstruction:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteProcessingInstruction", True, "benchmark" },
    { 721, "System.Private.Xml/System.Xml.XmlTextWriter::WriteEntityRef:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteEntityRef", True, "benchmark" },
    { 722, "System.Private.Xml/System.Xml.XmlTextWriter::WriteEntityRef:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteEntityRef", True, "benchmark" },
    { 723, "System.Private.Xml/System.Xml.XmlTextWriter::WriteCharEntity:System.Void(System.Char)", "codegen", "System.Xml.XmlTextWriter", "WriteCharEntity", True, "benchmark" },
    { 724, "System.Private.Xml/System.Xml.XmlTextWriter::WriteCharEntity:System.Void(System.Char)", "codegen", "System.Xml.XmlTextWriter", "WriteCharEntity", True, "benchmark" },
    { 725, "System.Private.Xml/System.Xml.XmlTextWriter::WriteWhitespace:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteWhitespace", True, "benchmark" },
    { 726, "System.Private.Xml/System.Xml.XmlTextWriter::WriteWhitespace:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteWhitespace", True, "benchmark" },
    { 727, "System.Private.Xml/System.Xml.XmlTextWriter::WriteString:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteString", True, "benchmark" },
    { 728, "System.Private.Xml/System.Xml.XmlTextWriter::WriteString:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteString", True, "benchmark" },
    { 729, "System.Private.Xml/System.Xml.XmlTextWriter::WriteSurrogateCharEntity:System.Void(System.Char,System.Char)", "codegen", "System.Xml.XmlTextWriter", "WriteSurrogateCharEntity", True, "benchmark" },
    { 730, "System.Private.Xml/System.Xml.XmlTextWriter::WriteSurrogateCharEntity:System.Void(System.Char,System.Char)", "codegen", "System.Xml.XmlTextWriter", "WriteSurrogateCharEntity", True, "benchmark" },
    { 731, "System.Private.Xml/System.Xml.XmlTextWriter::WriteSurrogateCharEntity:System.Void(System.Char,System.Char)", "codegen", "System.Xml.XmlTextWriter", "WriteSurrogateCharEntity", True, "benchmark" },
    { 732, "System.Private.Xml/System.Xml.XmlTextWriter::WriteChars:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteChars", True, "benchmark" },
    { 733, "System.Private.Xml/System.Xml.XmlTextWriter::WriteChars:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteChars", True, "benchmark" },
    { 734, "System.Private.Xml/System.Xml.XmlTextWriter::WriteChars:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteChars", True, "benchmark" },
    { 735, "System.Private.Xml/System.Xml.XmlTextWriter::WriteChars:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteChars", True, "benchmark" },
    { 736, "System.Private.Xml/System.Xml.XmlTextWriter::WriteChars:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteChars", True, "benchmark" },
    { 737, "System.Private.Xml/System.Xml.XmlTextWriter::WriteRaw:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteRaw", True, "benchmark" },
    { 738, "System.Private.Xml/System.Xml.XmlTextWriter::WriteRaw:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteRaw", True, "benchmark" },
    { 739, "System.Private.Xml/System.Xml.XmlTextWriter::WriteRaw:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteRaw", True, "benchmark" },
    { 740, "System.Private.Xml/System.Xml.XmlTextWriter::WriteRaw:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteRaw", True, "benchmark" },
    { 741, "System.Private.Xml/System.Xml.XmlTextWriter::WriteRaw:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteRaw", True, "benchmark" },
    { 742, "System.Private.Xml/System.Xml.XmlTextWriter::WriteRaw:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteRaw", True, "benchmark" },
    { 743, "System.Private.Xml/System.Xml.XmlTextWriter::WriteRaw:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteRaw", True, "benchmark" },
    { 744, "System.Private.Xml/System.Xml.XmlTextWriter::WriteBase64:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteBase64", True, "benchmark" },
    { 745, "System.Private.Xml/System.Xml.XmlTextWriter::WriteBase64:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteBase64", True, "benchmark" },
    { 746, "System.Private.Xml/System.Xml.XmlTextWriter::WriteBase64:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteBase64", True, "benchmark" },
    { 747, "System.Private.Xml/System.Xml.XmlTextWriter::WriteBase64:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteBase64", True, "benchmark" },
    { 748, "System.Private.Xml/System.Xml.XmlTextWriter::WriteBase64:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteBase64", True, "benchmark" },
    { 749, "System.Private.Xml/System.Xml.XmlTextWriter::WriteBinHex:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteBinHex", True, "benchmark" },
    { 750, "System.Private.Xml/System.Xml.XmlTextWriter::WriteBinHex:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteBinHex", True, "benchmark" },
    { 751, "System.Private.Xml/System.Xml.XmlTextWriter::WriteBinHex:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteBinHex", True, "benchmark" },
    { 752, "System.Private.Xml/System.Xml.XmlTextWriter::WriteBinHex:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteBinHex", True, "benchmark" },
    { 753, "System.Private.Xml/System.Xml.XmlTextWriter::WriteBinHex:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlTextWriter", "WriteBinHex", True, "benchmark" },
    { 754, "System.Private.Xml/System.Xml.XmlTextWriter::Close:System.Void()", "codegen", "System.Xml.XmlTextWriter", "Close", True, "benchmark" },
    { 755, "System.Private.Xml/System.Xml.XmlTextWriter::Flush:System.Void()", "codegen", "System.Xml.XmlTextWriter", "Flush", True, "benchmark" },
    { 756, "System.Private.Xml/System.Xml.XmlTextWriter::WriteName:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteName", True, "benchmark" },
    { 757, "System.Private.Xml/System.Xml.XmlTextWriter::WriteName:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteName", True, "benchmark" },
    { 758, "System.Private.Xml/System.Xml.XmlTextWriter::WriteQualifiedName:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteQualifiedName", True, "benchmark" },
    { 759, "System.Private.Xml/System.Xml.XmlTextWriter::WriteQualifiedName:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteQualifiedName", True, "benchmark" },
    { 760, "System.Private.Xml/System.Xml.XmlTextWriter::WriteQualifiedName:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteQualifiedName", True, "benchmark" },
    { 761, "System.Private.Xml/System.Xml.XmlTextWriter::LookupPrefix:System.String(System.String)", "codegen", "System.Xml.XmlTextWriter", "LookupPrefix", True, "benchmark" },
    { 762, "System.Private.Xml/System.Xml.XmlTextWriter::LookupPrefix:System.String(System.String)", "codegen", "System.Xml.XmlTextWriter", "LookupPrefix", True, "benchmark" },
    { 763, "System.Private.Xml/System.Xml.XmlTextWriter::WriteNmToken:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteNmToken", True, "benchmark" },
    { 764, "System.Private.Xml/System.Xml.XmlTextWriter::WriteNmToken:System.Void(System.String)", "codegen", "System.Xml.XmlTextWriter", "WriteNmToken", True, "benchmark" },
    { 765, "System.Private.Xml/System.Xml.XmlUrlResolver::GetEntity:System.Object(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlUrlResolver", "GetEntity", True, "benchmark" },
    { 766, "System.Private.Xml/System.Xml.XmlUrlResolver::GetEntity:System.Object(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlUrlResolver", "GetEntity", True, "benchmark" },
    { 767, "System.Private.Xml/System.Xml.XmlUrlResolver::GetEntityAsync:System.Threading.Tasks.Task<System.Object>(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlUrlResolver", "GetEntityAsync", True, "benchmark" },
    { 768, "System.Private.Xml/System.Xml.XmlUrlResolver::GetEntityAsync:System.Threading.Tasks.Task<System.Object>(System.Uri,System.String,System.Type)", "codegen", "System.Xml.XmlUrlResolver", "GetEntityAsync", True, "benchmark" },
    { 769, "System.Private.Xml/System.Xml.XmlUrlResolver::ResolveUri:System.Uri(System.Uri,System.String)", "codegen", "System.Xml.XmlUrlResolver", "ResolveUri", True, "benchmark" },
    { 770, "System.Private.Xml/System.Xml.XmlUrlResolver::ResolveUri:System.Uri(System.Uri,System.String)", "codegen", "System.Xml.XmlUrlResolver", "ResolveUri", True, "benchmark" },
    { 771, "System.Private.Xml/System.Xml.XmlValidatingReader::GetAttribute:System.String(System.String)", "codegen", "System.Xml.XmlValidatingReader", "GetAttribute", True, "benchmark" },
    { 772, "System.Private.Xml/System.Xml.XmlValidatingReader::GetAttribute:System.String(System.String)", "codegen", "System.Xml.XmlValidatingReader", "GetAttribute", True, "benchmark" },
    { 773, "System.Private.Xml/System.Xml.XmlValidatingReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlValidatingReader", "GetAttribute", True, "benchmark" },
    { 774, "System.Private.Xml/System.Xml.XmlValidatingReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlValidatingReader", "GetAttribute", True, "benchmark" },
    { 775, "System.Private.Xml/System.Xml.XmlValidatingReader::GetAttribute:System.String(System.String,System.String)", "codegen", "System.Xml.XmlValidatingReader", "GetAttribute", True, "benchmark" },
    { 776, "System.Private.Xml/System.Xml.XmlValidatingReader::GetAttribute:System.String(System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "GetAttribute", True, "benchmark" },
    { 777, "System.Private.Xml/System.Xml.XmlValidatingReader::GetAttribute:System.String(System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "GetAttribute", True, "benchmark" },
    { 778, "System.Private.Xml/System.Xml.XmlValidatingReader::MoveToAttribute:System.Boolean(System.String)", "codegen", "System.Xml.XmlValidatingReader", "MoveToAttribute", True, "benchmark" },
    { 779, "System.Private.Xml/System.Xml.XmlValidatingReader::MoveToAttribute:System.Boolean(System.String)", "codegen", "System.Xml.XmlValidatingReader", "MoveToAttribute", True, "benchmark" },
    { 780, "System.Private.Xml/System.Xml.XmlValidatingReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlValidatingReader", "MoveToAttribute", True, "benchmark" },
    { 781, "System.Private.Xml/System.Xml.XmlValidatingReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlValidatingReader", "MoveToAttribute", True, "benchmark" },
    { 782, "System.Private.Xml/System.Xml.XmlValidatingReader::MoveToAttribute:System.Boolean(System.String,System.String)", "codegen", "System.Xml.XmlValidatingReader", "MoveToAttribute", True, "benchmark" },
    { 783, "System.Private.Xml/System.Xml.XmlValidatingReader::MoveToAttribute:System.Void(System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "MoveToAttribute", True, "benchmark" },
    { 784, "System.Private.Xml/System.Xml.XmlValidatingReader::MoveToAttribute:System.Void(System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "MoveToAttribute", True, "benchmark" },
    { 785, "System.Private.Xml/System.Xml.XmlValidatingReader::MoveToFirstAttribute:System.Boolean()", "codegen", "System.Xml.XmlValidatingReader", "MoveToFirstAttribute", True, "benchmark" },
    { 786, "System.Private.Xml/System.Xml.XmlValidatingReader::MoveToNextAttribute:System.Boolean()", "codegen", "System.Xml.XmlValidatingReader", "MoveToNextAttribute", True, "benchmark" },
    { 787, "System.Private.Xml/System.Xml.XmlValidatingReader::MoveToElement:System.Boolean()", "codegen", "System.Xml.XmlValidatingReader", "MoveToElement", True, "benchmark" },
    { 788, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadAttributeValue:System.Boolean()", "codegen", "System.Xml.XmlValidatingReader", "ReadAttributeValue", True, "benchmark" },
    { 789, "System.Private.Xml/System.Xml.XmlValidatingReader::Read:System.Boolean()", "codegen", "System.Xml.XmlValidatingReader", "Read", True, "benchmark" },
    { 790, "System.Private.Xml/System.Xml.XmlValidatingReader::Close:System.Void()", "codegen", "System.Xml.XmlValidatingReader", "Close", True, "benchmark" },
    { 791, "System.Private.Xml/System.Xml.XmlValidatingReader::LookupNamespace:System.String(System.String)", "codegen", "System.Xml.XmlValidatingReader", "LookupNamespace", True, "benchmark" },
    { 792, "System.Private.Xml/System.Xml.XmlValidatingReader::LookupNamespace:System.String(System.String)", "codegen", "System.Xml.XmlValidatingReader", "LookupNamespace", True, "benchmark" },
    { 793, "System.Private.Xml/System.Xml.XmlValidatingReader::ResolveEntity:System.Void()", "codegen", "System.Xml.XmlValidatingReader", "ResolveEntity", True, "benchmark" },
    { 794, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadContentAsBase64", True, "benchmark" },
    { 795, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadContentAsBase64", True, "benchmark" },
    { 796, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadContentAsBase64", True, "benchmark" },
    { 797, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadContentAsBase64", True, "benchmark" },
    { 798, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadContentAsBase64", True, "benchmark" },
    { 799, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 800, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 801, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 802, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 803, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadElementContentAsBase64:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadElementContentAsBase64", True, "benchmark" },
    { 804, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadContentAsBinHex", True, "benchmark" },
    { 805, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadContentAsBinHex", True, "benchmark" },
    { 806, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadContentAsBinHex", True, "benchmark" },
    { 807, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadContentAsBinHex", True, "benchmark" },
    { 808, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadContentAsBinHex", True, "benchmark" },
    { 809, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 810, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 811, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 812, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 813, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadElementContentAsBinHex:System.Int32(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlValidatingReader", "ReadElementContentAsBinHex", True, "benchmark" },
    { 814, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadString:System.String()", "codegen", "System.Xml.XmlValidatingReader", "ReadString", True, "benchmark" },
    { 815, "System.Private.Xml/System.Xml.XmlValidatingReader::HasLineInfo:System.Boolean()", "codegen", "System.Xml.XmlValidatingReader", "HasLineInfo", True, "benchmark" },
    { 816, "System.Private.Xml/System.Xml.XmlValidatingReader::ReadTypedValue:System.Object()", "codegen", "System.Xml.XmlValidatingReader", "ReadTypedValue", True, "benchmark" },
    { 817, "System.Private.Xml/System.Xml.XmlWhitespace::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlWhitespace", "CloneNode", True, "benchmark" },
    { 818, "System.Private.Xml/System.Xml.XmlWhitespace::CloneNode:System.Xml.XmlNode(System.Boolean)", "codegen", "System.Xml.XmlWhitespace", "CloneNode", True, "benchmark" },
    { 819, "System.Private.Xml/System.Xml.XmlWhitespace::WriteTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlWhitespace", "WriteTo", True, "benchmark" },
    { 820, "System.Private.Xml/System.Xml.XmlWhitespace::WriteContentTo:System.Void(System.Xml.XmlWriter)", "codegen", "System.Xml.XmlWhitespace", "WriteContentTo", True, "benchmark" },
    { 821, "System.Private.Xml/System.Xml.XmlWriter::WriteStartDocument:System.Void()", "codegen", "System.Xml.XmlWriter", "WriteStartDocument", True, "benchmark" },
    { 822, "System.Private.Xml/System.Xml.XmlWriter::WriteStartDocument:System.Void(System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteStartDocument", True, "benchmark" },
    { 823, "System.Private.Xml/System.Xml.XmlWriter::WriteStartDocument:System.Void(System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteStartDocument", True, "benchmark" },
    { 824, "System.Private.Xml/System.Xml.XmlWriter::WriteEndDocument:System.Void()", "codegen", "System.Xml.XmlWriter", "WriteEndDocument", True, "benchmark" },
    { 825, "System.Private.Xml/System.Xml.XmlWriter::WriteDocType:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteDocType", True, "benchmark" },
    { 826, "System.Private.Xml/System.Xml.XmlWriter::WriteDocType:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteDocType", True, "benchmark" },
    { 827, "System.Private.Xml/System.Xml.XmlWriter::WriteDocType:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteDocType", True, "benchmark" },
    { 828, "System.Private.Xml/System.Xml.XmlWriter::WriteDocType:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteDocType", True, "benchmark" },
    { 829, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElement:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElement", True, "benchmark" },
    { 830, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElement:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElement", True, "benchmark" },
    { 831, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElement:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElement", True, "benchmark" },
    { 832, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElement:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElement", True, "benchmark" },
    { 833, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElement:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElement", True, "benchmark" },
    { 834, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElement:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElement", True, "benchmark" },
    { 835, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElement:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElement", True, "benchmark" },
    { 836, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElement:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElement", True, "benchmark" },
    { 837, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElement:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElement", True, "benchmark" },
    { 838, "System.Private.Xml/System.Xml.XmlWriter::WriteEndElement:System.Void()", "codegen", "System.Xml.XmlWriter", "WriteEndElement", True, "benchmark" },
    { 839, "System.Private.Xml/System.Xml.XmlWriter::WriteFullEndElement:System.Void()", "codegen", "System.Xml.XmlWriter", "WriteFullEndElement", True, "benchmark" },
    { 840, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 841, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 842, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 843, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 844, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 845, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 846, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 847, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 848, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 849, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 850, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeString:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeString", True, "benchmark" },
    { 851, "System.Private.Xml/System.Xml.XmlWriter::WriteStartAttribute:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartAttribute", True, "benchmark" },
    { 852, "System.Private.Xml/System.Xml.XmlWriter::WriteStartAttribute:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartAttribute", True, "benchmark" },
    { 853, "System.Private.Xml/System.Xml.XmlWriter::WriteStartAttribute:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartAttribute", True, "benchmark" },
    { 854, "System.Private.Xml/System.Xml.XmlWriter::WriteStartAttribute:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartAttribute", True, "benchmark" },
    { 855, "System.Private.Xml/System.Xml.XmlWriter::WriteStartAttribute:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartAttribute", True, "benchmark" },
    { 856, "System.Private.Xml/System.Xml.XmlWriter::WriteStartAttribute:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartAttribute", True, "benchmark" },
    { 857, "System.Private.Xml/System.Xml.XmlWriter::WriteStartAttribute:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartAttribute", True, "benchmark" },
    { 858, "System.Private.Xml/System.Xml.XmlWriter::WriteStartAttribute:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartAttribute", True, "benchmark" },
    { 859, "System.Private.Xml/System.Xml.XmlWriter::WriteStartAttribute:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartAttribute", True, "benchmark" },
    { 860, "System.Private.Xml/System.Xml.XmlWriter::WriteEndAttribute:System.Void()", "codegen", "System.Xml.XmlWriter", "WriteEndAttribute", True, "benchmark" },
    { 861, "System.Private.Xml/System.Xml.XmlWriter::WriteCData:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteCData", True, "benchmark" },
    { 862, "System.Private.Xml/System.Xml.XmlWriter::WriteCData:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteCData", True, "benchmark" },
    { 863, "System.Private.Xml/System.Xml.XmlWriter::WriteComment:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteComment", True, "benchmark" },
    { 864, "System.Private.Xml/System.Xml.XmlWriter::WriteComment:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteComment", True, "benchmark" },
    { 865, "System.Private.Xml/System.Xml.XmlWriter::WriteProcessingInstruction:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteProcessingInstruction", True, "benchmark" },
    { 866, "System.Private.Xml/System.Xml.XmlWriter::WriteProcessingInstruction:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteProcessingInstruction", True, "benchmark" },
    { 867, "System.Private.Xml/System.Xml.XmlWriter::WriteProcessingInstruction:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteProcessingInstruction", True, "benchmark" },
    { 868, "System.Private.Xml/System.Xml.XmlWriter::WriteEntityRef:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteEntityRef", True, "benchmark" },
    { 869, "System.Private.Xml/System.Xml.XmlWriter::WriteEntityRef:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteEntityRef", True, "benchmark" },
    { 870, "System.Private.Xml/System.Xml.XmlWriter::WriteCharEntity:System.Void(System.Char)", "codegen", "System.Xml.XmlWriter", "WriteCharEntity", True, "benchmark" },
    { 871, "System.Private.Xml/System.Xml.XmlWriter::WriteCharEntity:System.Void(System.Char)", "codegen", "System.Xml.XmlWriter", "WriteCharEntity", True, "benchmark" },
    { 872, "System.Private.Xml/System.Xml.XmlWriter::WriteWhitespace:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteWhitespace", True, "benchmark" },
    { 873, "System.Private.Xml/System.Xml.XmlWriter::WriteWhitespace:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteWhitespace", True, "benchmark" },
    { 874, "System.Private.Xml/System.Xml.XmlWriter::WriteString:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteString", True, "benchmark" },
    { 875, "System.Private.Xml/System.Xml.XmlWriter::WriteString:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteString", True, "benchmark" },
    { 876, "System.Private.Xml/System.Xml.XmlWriter::WriteSurrogateCharEntity:System.Void(System.Char,System.Char)", "codegen", "System.Xml.XmlWriter", "WriteSurrogateCharEntity", True, "benchmark" },
    { 877, "System.Private.Xml/System.Xml.XmlWriter::WriteSurrogateCharEntity:System.Void(System.Char,System.Char)", "codegen", "System.Xml.XmlWriter", "WriteSurrogateCharEntity", True, "benchmark" },
    { 878, "System.Private.Xml/System.Xml.XmlWriter::WriteSurrogateCharEntity:System.Void(System.Char,System.Char)", "codegen", "System.Xml.XmlWriter", "WriteSurrogateCharEntity", True, "benchmark" },
    { 879, "System.Private.Xml/System.Xml.XmlWriter::WriteChars:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteChars", True, "benchmark" },
    { 880, "System.Private.Xml/System.Xml.XmlWriter::WriteChars:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteChars", True, "benchmark" },
    { 881, "System.Private.Xml/System.Xml.XmlWriter::WriteChars:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteChars", True, "benchmark" },
    { 882, "System.Private.Xml/System.Xml.XmlWriter::WriteChars:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteChars", True, "benchmark" },
    { 883, "System.Private.Xml/System.Xml.XmlWriter::WriteChars:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteChars", True, "benchmark" },
    { 884, "System.Private.Xml/System.Xml.XmlWriter::WriteRaw:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteRaw", True, "benchmark" },
    { 885, "System.Private.Xml/System.Xml.XmlWriter::WriteRaw:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteRaw", True, "benchmark" },
    { 886, "System.Private.Xml/System.Xml.XmlWriter::WriteRaw:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteRaw", True, "benchmark" },
    { 887, "System.Private.Xml/System.Xml.XmlWriter::WriteRaw:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteRaw", True, "benchmark" },
    { 888, "System.Private.Xml/System.Xml.XmlWriter::WriteRaw:System.Void(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteRaw", True, "benchmark" },
    { 889, "System.Private.Xml/System.Xml.XmlWriter::WriteRaw:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteRaw", True, "benchmark" },
    { 890, "System.Private.Xml/System.Xml.XmlWriter::WriteRaw:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteRaw", True, "benchmark" },
    { 891, "System.Private.Xml/System.Xml.XmlWriter::WriteBase64:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBase64", True, "benchmark" },
    { 892, "System.Private.Xml/System.Xml.XmlWriter::WriteBase64:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBase64", True, "benchmark" },
    { 893, "System.Private.Xml/System.Xml.XmlWriter::WriteBase64:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBase64", True, "benchmark" },
    { 894, "System.Private.Xml/System.Xml.XmlWriter::WriteBase64:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBase64", True, "benchmark" },
    { 895, "System.Private.Xml/System.Xml.XmlWriter::WriteBase64:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBase64", True, "benchmark" },
    { 896, "System.Private.Xml/System.Xml.XmlWriter::WriteBinHex:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBinHex", True, "benchmark" },
    { 897, "System.Private.Xml/System.Xml.XmlWriter::WriteBinHex:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBinHex", True, "benchmark" },
    { 898, "System.Private.Xml/System.Xml.XmlWriter::WriteBinHex:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBinHex", True, "benchmark" },
    { 899, "System.Private.Xml/System.Xml.XmlWriter::WriteBinHex:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBinHex", True, "benchmark" },
    { 900, "System.Private.Xml/System.Xml.XmlWriter::WriteBinHex:System.Void(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBinHex", True, "benchmark" },
    { 901, "System.Private.Xml/System.Xml.XmlWriter::Close:System.Void()", "codegen", "System.Xml.XmlWriter", "Close", True, "benchmark" },
    { 902, "System.Private.Xml/System.Xml.XmlWriter::Flush:System.Void()", "codegen", "System.Xml.XmlWriter", "Flush", True, "benchmark" },
    { 903, "System.Private.Xml/System.Xml.XmlWriter::LookupPrefix:System.String(System.String)", "codegen", "System.Xml.XmlWriter", "LookupPrefix", True, "benchmark" },
    { 904, "System.Private.Xml/System.Xml.XmlWriter::LookupPrefix:System.String(System.String)", "codegen", "System.Xml.XmlWriter", "LookupPrefix", True, "benchmark" },
    { 905, "System.Private.Xml/System.Xml.XmlWriter::WriteNmToken:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteNmToken", True, "benchmark" },
    { 906, "System.Private.Xml/System.Xml.XmlWriter::WriteNmToken:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteNmToken", True, "benchmark" },
    { 907, "System.Private.Xml/System.Xml.XmlWriter::WriteName:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteName", True, "benchmark" },
    { 908, "System.Private.Xml/System.Xml.XmlWriter::WriteName:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteName", True, "benchmark" },
    { 909, "System.Private.Xml/System.Xml.XmlWriter::WriteQualifiedName:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteQualifiedName", True, "benchmark" },
    { 910, "System.Private.Xml/System.Xml.XmlWriter::WriteQualifiedName:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteQualifiedName", True, "benchmark" },
    { 911, "System.Private.Xml/System.Xml.XmlWriter::WriteQualifiedName:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteQualifiedName", True, "benchmark" },
    { 912, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Object)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 913, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 914, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.String)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 915, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 916, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 917, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.DateTime)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 918, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.DateTimeOffset)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 919, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Double)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 920, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Double)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 921, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Single)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 922, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Single)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 923, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Decimal)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 924, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Decimal)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 925, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 926, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 927, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Int64)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 928, "System.Private.Xml/System.Xml.XmlWriter::WriteValue:System.Void(System.Int64)", "codegen", "System.Xml.XmlWriter", "WriteValue", True, "benchmark" },
    { 929, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributes:System.Void(System.Xml.XmlReader,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteAttributes", True, "benchmark" },
    { 930, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributes:System.Void(System.Xml.XmlReader,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteAttributes", True, "benchmark" },
    { 931, "System.Private.Xml/System.Xml.XmlWriter::WriteNode:System.Void(System.Xml.XmlReader,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteNode", True, "benchmark" },
    { 932, "System.Private.Xml/System.Xml.XmlWriter::WriteNode:System.Void(System.Xml.XmlReader,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteNode", True, "benchmark" },
    { 933, "System.Private.Xml/System.Xml.XmlWriter::WriteNode:System.Void(System.Xml.XPath.XPathNavigator,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteNode", True, "benchmark" },
    { 934, "System.Private.Xml/System.Xml.XmlWriter::WriteNode:System.Void(System.Xml.XPath.XPathNavigator,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteNode", True, "benchmark" },
    { 935, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 936, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 937, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 938, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 939, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 940, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 941, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 942, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 943, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 944, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 945, "System.Private.Xml/System.Xml.XmlWriter::WriteElementString:System.Void(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementString", True, "benchmark" },
    { 946, "System.Private.Xml/System.Xml.XmlWriter::Dispose:System.Void()", "codegen", "System.Xml.XmlWriter", "Dispose", True, "benchmark" },
    { 947, "System.Private.Xml/System.Xml.XmlWriter::WriteStartDocumentAsync:System.Threading.Tasks.Task()", "codegen", "System.Xml.XmlWriter", "WriteStartDocumentAsync", True, "benchmark" },
    { 948, "System.Private.Xml/System.Xml.XmlWriter::WriteStartDocumentAsync:System.Threading.Tasks.Task(System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteStartDocumentAsync", True, "benchmark" },
    { 949, "System.Private.Xml/System.Xml.XmlWriter::WriteStartDocumentAsync:System.Threading.Tasks.Task(System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteStartDocumentAsync", True, "benchmark" },
    { 950, "System.Private.Xml/System.Xml.XmlWriter::WriteEndDocumentAsync:System.Threading.Tasks.Task()", "codegen", "System.Xml.XmlWriter", "WriteEndDocumentAsync", True, "benchmark" },
    { 951, "System.Private.Xml/System.Xml.XmlWriter::WriteDocTypeAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteDocTypeAsync", True, "benchmark" },
    { 952, "System.Private.Xml/System.Xml.XmlWriter::WriteDocTypeAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteDocTypeAsync", True, "benchmark" },
    { 953, "System.Private.Xml/System.Xml.XmlWriter::WriteDocTypeAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteDocTypeAsync", True, "benchmark" },
    { 954, "System.Private.Xml/System.Xml.XmlWriter::WriteDocTypeAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteDocTypeAsync", True, "benchmark" },
    { 955, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElementAsync:System.Threading.Tasks.Task(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElementAsync", True, "benchmark" },
    { 956, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElementAsync:System.Threading.Tasks.Task(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElementAsync", True, "benchmark" },
    { 957, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElementAsync:System.Threading.Tasks.Task(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElementAsync", True, "benchmark" },
    { 958, "System.Private.Xml/System.Xml.XmlWriter::WriteStartElementAsync:System.Threading.Tasks.Task(System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteStartElementAsync", True, "benchmark" },
    { 959, "System.Private.Xml/System.Xml.XmlWriter::WriteEndElementAsync:System.Threading.Tasks.Task()", "codegen", "System.Xml.XmlWriter", "WriteEndElementAsync", True, "benchmark" },
    { 960, "System.Private.Xml/System.Xml.XmlWriter::WriteFullEndElementAsync:System.Threading.Tasks.Task()", "codegen", "System.Xml.XmlWriter", "WriteFullEndElementAsync", True, "benchmark" },
    { 961, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeStringAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeStringAsync", True, "benchmark" },
    { 962, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeStringAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeStringAsync", True, "benchmark" },
    { 963, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeStringAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeStringAsync", True, "benchmark" },
    { 964, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributeStringAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteAttributeStringAsync", True, "benchmark" },
    { 965, "System.Private.Xml/System.Xml.XmlWriter::WriteCDataAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteCDataAsync", True, "benchmark" },
    { 966, "System.Private.Xml/System.Xml.XmlWriter::WriteCDataAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteCDataAsync", True, "benchmark" },
    { 967, "System.Private.Xml/System.Xml.XmlWriter::WriteCommentAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteCommentAsync", True, "benchmark" },
    { 968, "System.Private.Xml/System.Xml.XmlWriter::WriteCommentAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteCommentAsync", True, "benchmark" },
    { 969, "System.Private.Xml/System.Xml.XmlWriter::WriteProcessingInstructionAsync:System.Threading.Tasks.Task(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteProcessingInstructionAsync", True, "benchmark" },
    { 970, "System.Private.Xml/System.Xml.XmlWriter::WriteProcessingInstructionAsync:System.Threading.Tasks.Task(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteProcessingInstructionAsync", True, "benchmark" },
    { 971, "System.Private.Xml/System.Xml.XmlWriter::WriteProcessingInstructionAsync:System.Threading.Tasks.Task(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteProcessingInstructionAsync", True, "benchmark" },
    { 972, "System.Private.Xml/System.Xml.XmlWriter::WriteEntityRefAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteEntityRefAsync", True, "benchmark" },
    { 973, "System.Private.Xml/System.Xml.XmlWriter::WriteEntityRefAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteEntityRefAsync", True, "benchmark" },
    { 974, "System.Private.Xml/System.Xml.XmlWriter::WriteCharEntityAsync:System.Threading.Tasks.Task(System.Char)", "codegen", "System.Xml.XmlWriter", "WriteCharEntityAsync", True, "benchmark" },
    { 975, "System.Private.Xml/System.Xml.XmlWriter::WriteCharEntityAsync:System.Threading.Tasks.Task(System.Char)", "codegen", "System.Xml.XmlWriter", "WriteCharEntityAsync", True, "benchmark" },
    { 976, "System.Private.Xml/System.Xml.XmlWriter::WriteWhitespaceAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteWhitespaceAsync", True, "benchmark" },
    { 977, "System.Private.Xml/System.Xml.XmlWriter::WriteWhitespaceAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteWhitespaceAsync", True, "benchmark" },
    { 978, "System.Private.Xml/System.Xml.XmlWriter::WriteStringAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteStringAsync", True, "benchmark" },
    { 979, "System.Private.Xml/System.Xml.XmlWriter::WriteStringAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteStringAsync", True, "benchmark" },
    { 980, "System.Private.Xml/System.Xml.XmlWriter::WriteSurrogateCharEntityAsync:System.Threading.Tasks.Task(System.Char,System.Char)", "codegen", "System.Xml.XmlWriter", "WriteSurrogateCharEntityAsync", True, "benchmark" },
    { 981, "System.Private.Xml/System.Xml.XmlWriter::WriteSurrogateCharEntityAsync:System.Threading.Tasks.Task(System.Char,System.Char)", "codegen", "System.Xml.XmlWriter", "WriteSurrogateCharEntityAsync", True, "benchmark" },
    { 982, "System.Private.Xml/System.Xml.XmlWriter::WriteSurrogateCharEntityAsync:System.Threading.Tasks.Task(System.Char,System.Char)", "codegen", "System.Xml.XmlWriter", "WriteSurrogateCharEntityAsync", True, "benchmark" },
    { 983, "System.Private.Xml/System.Xml.XmlWriter::WriteCharsAsync:System.Threading.Tasks.Task(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteCharsAsync", True, "benchmark" },
    { 984, "System.Private.Xml/System.Xml.XmlWriter::WriteCharsAsync:System.Threading.Tasks.Task(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteCharsAsync", True, "benchmark" },
    { 985, "System.Private.Xml/System.Xml.XmlWriter::WriteCharsAsync:System.Threading.Tasks.Task(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteCharsAsync", True, "benchmark" },
    { 986, "System.Private.Xml/System.Xml.XmlWriter::WriteCharsAsync:System.Threading.Tasks.Task(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteCharsAsync", True, "benchmark" },
    { 987, "System.Private.Xml/System.Xml.XmlWriter::WriteCharsAsync:System.Threading.Tasks.Task(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteCharsAsync", True, "benchmark" },
    { 988, "System.Private.Xml/System.Xml.XmlWriter::WriteRawAsync:System.Threading.Tasks.Task(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteRawAsync", True, "benchmark" },
    { 989, "System.Private.Xml/System.Xml.XmlWriter::WriteRawAsync:System.Threading.Tasks.Task(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteRawAsync", True, "benchmark" },
    { 990, "System.Private.Xml/System.Xml.XmlWriter::WriteRawAsync:System.Threading.Tasks.Task(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteRawAsync", True, "benchmark" },
    { 991, "System.Private.Xml/System.Xml.XmlWriter::WriteRawAsync:System.Threading.Tasks.Task(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteRawAsync", True, "benchmark" },
    { 992, "System.Private.Xml/System.Xml.XmlWriter::WriteRawAsync:System.Threading.Tasks.Task(System.Char[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteRawAsync", True, "benchmark" },
    { 993, "System.Private.Xml/System.Xml.XmlWriter::WriteRawAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteRawAsync", True, "benchmark" },
    { 994, "System.Private.Xml/System.Xml.XmlWriter::WriteRawAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteRawAsync", True, "benchmark" },
    { 995, "System.Private.Xml/System.Xml.XmlWriter::WriteBase64Async:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBase64Async", True, "benchmark" },
    { 996, "System.Private.Xml/System.Xml.XmlWriter::WriteBase64Async:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBase64Async", True, "benchmark" },
    { 997, "System.Private.Xml/System.Xml.XmlWriter::WriteBase64Async:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBase64Async", True, "benchmark" },
    { 998, "System.Private.Xml/System.Xml.XmlWriter::WriteBase64Async:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBase64Async", True, "benchmark" },
    { 999, "System.Private.Xml/System.Xml.XmlWriter::WriteBase64Async:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBase64Async", True, "benchmark" },
    { 1000, "System.Private.Xml/System.Xml.XmlWriter::WriteBinHexAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBinHexAsync", True, "benchmark" },
    { 1001, "System.Private.Xml/System.Xml.XmlWriter::WriteBinHexAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBinHexAsync", True, "benchmark" },
    { 1002, "System.Private.Xml/System.Xml.XmlWriter::WriteBinHexAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBinHexAsync", True, "benchmark" },
    { 1003, "System.Private.Xml/System.Xml.XmlWriter::WriteBinHexAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBinHexAsync", True, "benchmark" },
    { 1004, "System.Private.Xml/System.Xml.XmlWriter::WriteBinHexAsync:System.Threading.Tasks.Task(System.Byte[],System.Int32,System.Int32)", "codegen", "System.Xml.XmlWriter", "WriteBinHexAsync", True, "benchmark" },
    { 1005, "System.Private.Xml/System.Xml.XmlWriter::FlushAsync:System.Threading.Tasks.Task()", "codegen", "System.Xml.XmlWriter", "FlushAsync", True, "benchmark" },
    { 1006, "System.Private.Xml/System.Xml.XmlWriter::WriteNmTokenAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteNmTokenAsync", True, "benchmark" },
    { 1007, "System.Private.Xml/System.Xml.XmlWriter::WriteNmTokenAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteNmTokenAsync", True, "benchmark" },
    { 1008, "System.Private.Xml/System.Xml.XmlWriter::WriteNameAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteNameAsync", True, "benchmark" },
    { 1009, "System.Private.Xml/System.Xml.XmlWriter::WriteNameAsync:System.Threading.Tasks.Task(System.String)", "codegen", "System.Xml.XmlWriter", "WriteNameAsync", True, "benchmark" },
    { 1010, "System.Private.Xml/System.Xml.XmlWriter::WriteQualifiedNameAsync:System.Threading.Tasks.Task(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteQualifiedNameAsync", True, "benchmark" },
    { 1011, "System.Private.Xml/System.Xml.XmlWriter::WriteQualifiedNameAsync:System.Threading.Tasks.Task(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteQualifiedNameAsync", True, "benchmark" },
    { 1012, "System.Private.Xml/System.Xml.XmlWriter::WriteQualifiedNameAsync:System.Threading.Tasks.Task(System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteQualifiedNameAsync", True, "benchmark" },
    { 1013, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributesAsync:System.Threading.Tasks.Task(System.Xml.XmlReader,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteAttributesAsync", True, "benchmark" },
    { 1014, "System.Private.Xml/System.Xml.XmlWriter::WriteAttributesAsync:System.Threading.Tasks.Task(System.Xml.XmlReader,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteAttributesAsync", True, "benchmark" },
    { 1015, "System.Private.Xml/System.Xml.XmlWriter::WriteNodeAsync:System.Threading.Tasks.Task(System.Xml.XmlReader,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteNodeAsync", True, "benchmark" },
    { 1016, "System.Private.Xml/System.Xml.XmlWriter::WriteNodeAsync:System.Threading.Tasks.Task(System.Xml.XmlReader,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteNodeAsync", True, "benchmark" },
    { 1017, "System.Private.Xml/System.Xml.XmlWriter::WriteNodeAsync:System.Threading.Tasks.Task(System.Xml.XPath.XPathNavigator,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteNodeAsync", True, "benchmark" },
    { 1018, "System.Private.Xml/System.Xml.XmlWriter::WriteNodeAsync:System.Threading.Tasks.Task(System.Xml.XPath.XPathNavigator,System.Boolean)", "codegen", "System.Xml.XmlWriter", "WriteNodeAsync", True, "benchmark" },
    { 1019, "System.Private.Xml/System.Xml.XmlWriter::WriteElementStringAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementStringAsync", True, "benchmark" },
    { 1020, "System.Private.Xml/System.Xml.XmlWriter::WriteElementStringAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementStringAsync", True, "benchmark" },
    { 1021, "System.Private.Xml/System.Xml.XmlWriter::WriteElementStringAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementStringAsync", True, "benchmark" },
    { 1022, "System.Private.Xml/System.Xml.XmlWriter::WriteElementStringAsync:System.Threading.Tasks.Task(System.String,System.String,System.String,System.String)", "codegen", "System.Xml.XmlWriter", "WriteElementStringAsync", True, "benchmark" },
    { 1023, "System.Private.Xml/System.Xml.XmlWriter::DisposeAsync:System.Threading.Tasks.ValueTask()", "codegen", "System.Xml.XmlWriter", "DisposeAsync", True, "benchmark" },
    { 1024, "System.Private.Xml/System.Xml.XmlWriterSettings::Clone:System.Xml.XmlWriterSettings()", "codegen", "System.Xml.XmlWriterSettings", "Clone", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 1025;

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