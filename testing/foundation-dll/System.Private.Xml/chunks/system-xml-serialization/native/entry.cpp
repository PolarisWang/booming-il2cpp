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
    { 0, "System.Private.Xml/System.Xml.Serialization.CodeIdentifier::MakePascal:System.String(System.String)", "generated", "System.Xml.Serialization.CodeIdentifier", "MakePascal", True, "benchmark" },
    { 1, "System.Private.Xml/System.Xml.Serialization.CodeIdentifier::MakePascal:System.String(System.String)", "generated", "System.Xml.Serialization.CodeIdentifier", "MakePascal", True, "benchmark" },
    { 2, "System.Private.Xml/System.Xml.Serialization.CodeIdentifier::MakeCamel:System.String(System.String)", "generated", "System.Xml.Serialization.CodeIdentifier", "MakeCamel", True, "benchmark" },
    { 3, "System.Private.Xml/System.Xml.Serialization.CodeIdentifier::MakeCamel:System.String(System.String)", "generated", "System.Xml.Serialization.CodeIdentifier", "MakeCamel", True, "benchmark" },
    { 4, "System.Private.Xml/System.Xml.Serialization.CodeIdentifier::MakeValid:System.String(System.String)", "generated", "System.Xml.Serialization.CodeIdentifier", "MakeValid", True, "benchmark" },
    { 5, "System.Private.Xml/System.Xml.Serialization.CodeIdentifier::MakeValid:System.String(System.String)", "generated", "System.Xml.Serialization.CodeIdentifier", "MakeValid", True, "benchmark" },
    { 6, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::Clear:System.Void()", "generated", "System.Xml.Serialization.CodeIdentifiers", "Clear", True, "benchmark" },
    { 7, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::MakeRightCase:System.String(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "MakeRightCase", True, "benchmark" },
    { 8, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::MakeRightCase:System.String(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "MakeRightCase", True, "benchmark" },
    { 9, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::MakeUnique:System.String(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "MakeUnique", True, "benchmark" },
    { 10, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::MakeUnique:System.String(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "MakeUnique", True, "benchmark" },
    { 11, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::AddReserved:System.Void(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "AddReserved", True, "benchmark" },
    { 12, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::AddReserved:System.Void(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "AddReserved", True, "benchmark" },
    { 13, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::RemoveReserved:System.Void(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "RemoveReserved", True, "benchmark" },
    { 14, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::RemoveReserved:System.Void(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "RemoveReserved", True, "benchmark" },
    { 15, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::AddUnique:System.String(System.String,System.Object)", "generated", "System.Xml.Serialization.CodeIdentifiers", "AddUnique", True, "benchmark" },
    { 16, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::AddUnique:System.String(System.String,System.Object)", "generated", "System.Xml.Serialization.CodeIdentifiers", "AddUnique", True, "benchmark" },
    { 17, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::IsInUse:System.Boolean(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "IsInUse", True, "benchmark" },
    { 18, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::IsInUse:System.Boolean(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "IsInUse", True, "benchmark" },
    { 19, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::Add:System.Void(System.String,System.Object)", "generated", "System.Xml.Serialization.CodeIdentifiers", "Add", True, "benchmark" },
    { 20, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::Add:System.Void(System.String,System.Object)", "generated", "System.Xml.Serialization.CodeIdentifiers", "Add", True, "benchmark" },
    { 21, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::Remove:System.Void(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "Remove", True, "benchmark" },
    { 22, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::Remove:System.Void(System.String)", "generated", "System.Xml.Serialization.CodeIdentifiers", "Remove", True, "benchmark" },
    { 23, "System.Private.Xml/System.Xml.Serialization.CodeIdentifiers::ToArray:System.Object(System.Type)", "generated", "System.Xml.Serialization.CodeIdentifiers", "ToArray", True, "benchmark" },
    { 24, "System.Private.Xml/System.Xml.Serialization.SoapAttributeOverrides::Add:System.Void(System.Type,System.Xml.Serialization.SoapAttributes)", "generated", "System.Xml.Serialization.SoapAttributeOverrides", "Add", True, "benchmark" },
    { 25, "System.Private.Xml/System.Xml.Serialization.SoapAttributeOverrides::Add:System.Void(System.Type,System.String,System.Xml.Serialization.SoapAttributes)", "generated", "System.Xml.Serialization.SoapAttributeOverrides", "Add", True, "benchmark" },
    { 26, "System.Private.Xml/System.Xml.Serialization.SoapAttributeOverrides::Add:System.Void(System.Type,System.String,System.Xml.Serialization.SoapAttributes)", "generated", "System.Xml.Serialization.SoapAttributeOverrides", "Add", True, "benchmark" },
    { 27, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::IncludeTypes:System.Void(System.Reflection.ICustomAttributeProvider)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "IncludeTypes", True, "benchmark" },
    { 28, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::IncludeType:System.Void(System.Type)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "IncludeType", True, "benchmark" },
    { 29, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Type)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportTypeMapping", True, "benchmark" },
    { 30, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Type,System.String)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportTypeMapping", True, "benchmark" },
    { 31, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Type,System.String)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportTypeMapping", True, "benchmark" },
    { 32, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[])", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 33, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[])", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 34, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[])", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 35, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[])", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 36, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[])", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 37, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 38, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 39, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 40, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 41, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 42, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 43, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 44, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 45, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 46, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 47, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean,System.Xml.Serialization.XmlMappingAccess)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 48, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean,System.Xml.Serialization.XmlMappingAccess)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 49, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean,System.Xml.Serialization.XmlMappingAccess)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 50, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean,System.Xml.Serialization.XmlMappingAccess)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 51, "System.Private.Xml/System.Xml.Serialization.SoapReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean,System.Xml.Serialization.XmlMappingAccess)", "generated", "System.Xml.Serialization.SoapReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 52, "System.Private.Xml/System.Xml.Serialization.XmlAnyElementAttributes::Add:System.Int32(System.Xml.Serialization.XmlAnyElementAttribute)", "generated", "System.Xml.Serialization.XmlAnyElementAttributes", "Add", True, "benchmark" },
    { 53, "System.Private.Xml/System.Xml.Serialization.XmlAnyElementAttributes::Insert:System.Void(System.Int32,System.Xml.Serialization.XmlAnyElementAttribute)", "generated", "System.Xml.Serialization.XmlAnyElementAttributes", "Insert", True, "benchmark" },
    { 54, "System.Private.Xml/System.Xml.Serialization.XmlAnyElementAttributes::Insert:System.Void(System.Int32,System.Xml.Serialization.XmlAnyElementAttribute)", "generated", "System.Xml.Serialization.XmlAnyElementAttributes", "Insert", True, "benchmark" },
    { 55, "System.Private.Xml/System.Xml.Serialization.XmlAnyElementAttributes::IndexOf:System.Int32(System.Xml.Serialization.XmlAnyElementAttribute)", "generated", "System.Xml.Serialization.XmlAnyElementAttributes", "IndexOf", True, "benchmark" },
    { 56, "System.Private.Xml/System.Xml.Serialization.XmlAnyElementAttributes::Contains:System.Boolean(System.Xml.Serialization.XmlAnyElementAttribute)", "generated", "System.Xml.Serialization.XmlAnyElementAttributes", "Contains", True, "benchmark" },
    { 57, "System.Private.Xml/System.Xml.Serialization.XmlAnyElementAttributes::Remove:System.Void(System.Xml.Serialization.XmlAnyElementAttribute)", "generated", "System.Xml.Serialization.XmlAnyElementAttributes", "Remove", True, "benchmark" },
    { 58, "System.Private.Xml/System.Xml.Serialization.XmlArrayItemAttributes::Add:System.Int32(System.Xml.Serialization.XmlArrayItemAttribute)", "generated", "System.Xml.Serialization.XmlArrayItemAttributes", "Add", True, "benchmark" },
    { 59, "System.Private.Xml/System.Xml.Serialization.XmlArrayItemAttributes::Insert:System.Void(System.Int32,System.Xml.Serialization.XmlArrayItemAttribute)", "generated", "System.Xml.Serialization.XmlArrayItemAttributes", "Insert", True, "benchmark" },
    { 60, "System.Private.Xml/System.Xml.Serialization.XmlArrayItemAttributes::Insert:System.Void(System.Int32,System.Xml.Serialization.XmlArrayItemAttribute)", "generated", "System.Xml.Serialization.XmlArrayItemAttributes", "Insert", True, "benchmark" },
    { 61, "System.Private.Xml/System.Xml.Serialization.XmlArrayItemAttributes::IndexOf:System.Int32(System.Xml.Serialization.XmlArrayItemAttribute)", "generated", "System.Xml.Serialization.XmlArrayItemAttributes", "IndexOf", True, "benchmark" },
    { 62, "System.Private.Xml/System.Xml.Serialization.XmlArrayItemAttributes::Contains:System.Boolean(System.Xml.Serialization.XmlArrayItemAttribute)", "generated", "System.Xml.Serialization.XmlArrayItemAttributes", "Contains", True, "benchmark" },
    { 63, "System.Private.Xml/System.Xml.Serialization.XmlArrayItemAttributes::Remove:System.Void(System.Xml.Serialization.XmlArrayItemAttribute)", "generated", "System.Xml.Serialization.XmlArrayItemAttributes", "Remove", True, "benchmark" },
    { 64, "System.Private.Xml/System.Xml.Serialization.XmlAttributeOverrides::Add:System.Void(System.Type,System.Xml.Serialization.XmlAttributes)", "generated", "System.Xml.Serialization.XmlAttributeOverrides", "Add", True, "benchmark" },
    { 65, "System.Private.Xml/System.Xml.Serialization.XmlAttributeOverrides::Add:System.Void(System.Type,System.String,System.Xml.Serialization.XmlAttributes)", "generated", "System.Xml.Serialization.XmlAttributeOverrides", "Add", True, "benchmark" },
    { 66, "System.Private.Xml/System.Xml.Serialization.XmlAttributeOverrides::Add:System.Void(System.Type,System.String,System.Xml.Serialization.XmlAttributes)", "generated", "System.Xml.Serialization.XmlAttributeOverrides", "Add", True, "benchmark" },
    { 67, "System.Private.Xml/System.Xml.Serialization.XmlElementAttributes::Add:System.Int32(System.Xml.Serialization.XmlElementAttribute)", "generated", "System.Xml.Serialization.XmlElementAttributes", "Add", True, "benchmark" },
    { 68, "System.Private.Xml/System.Xml.Serialization.XmlElementAttributes::Insert:System.Void(System.Int32,System.Xml.Serialization.XmlElementAttribute)", "generated", "System.Xml.Serialization.XmlElementAttributes", "Insert", True, "benchmark" },
    { 69, "System.Private.Xml/System.Xml.Serialization.XmlElementAttributes::Insert:System.Void(System.Int32,System.Xml.Serialization.XmlElementAttribute)", "generated", "System.Xml.Serialization.XmlElementAttributes", "Insert", True, "benchmark" },
    { 70, "System.Private.Xml/System.Xml.Serialization.XmlElementAttributes::IndexOf:System.Int32(System.Xml.Serialization.XmlElementAttribute)", "generated", "System.Xml.Serialization.XmlElementAttributes", "IndexOf", True, "benchmark" },
    { 71, "System.Private.Xml/System.Xml.Serialization.XmlElementAttributes::Contains:System.Boolean(System.Xml.Serialization.XmlElementAttribute)", "generated", "System.Xml.Serialization.XmlElementAttributes", "Contains", True, "benchmark" },
    { 72, "System.Private.Xml/System.Xml.Serialization.XmlElementAttributes::Remove:System.Void(System.Xml.Serialization.XmlElementAttribute)", "generated", "System.Xml.Serialization.XmlElementAttributes", "Remove", True, "benchmark" },
    { 73, "System.Private.Xml/System.Xml.Serialization.XmlMapping::SetKey:System.Void(System.String)", "generated", "System.Xml.Serialization.XmlMapping", "SetKey", True, "benchmark" },
    { 74, "System.Private.Xml/System.Xml.Serialization.XmlMapping::SetKey:System.Void(System.String)", "generated", "System.Xml.Serialization.XmlMapping", "SetKey", True, "benchmark" },
    { 75, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::IncludeTypes:System.Void(System.Reflection.ICustomAttributeProvider)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "IncludeTypes", True, "benchmark" },
    { 76, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::IncludeType:System.Void(System.Type)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "IncludeType", True, "benchmark" },
    { 77, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Type)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportTypeMapping", True, "benchmark" },
    { 78, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Type,System.String)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportTypeMapping", True, "benchmark" },
    { 79, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Type,System.String)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportTypeMapping", True, "benchmark" },
    { 80, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Type,System.Xml.Serialization.XmlRootAttribute)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportTypeMapping", True, "benchmark" },
    { 81, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Type,System.Xml.Serialization.XmlRootAttribute,System.String)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportTypeMapping", True, "benchmark" },
    { 82, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Type,System.Xml.Serialization.XmlRootAttribute,System.String)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportTypeMapping", True, "benchmark" },
    { 83, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 84, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 85, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 86, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 87, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 88, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 89, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 90, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 91, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 92, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 93, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 94, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 95, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 96, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 97, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 98, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean,System.Xml.Serialization.XmlMappingAccess)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 99, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean,System.Xml.Serialization.XmlMappingAccess)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 100, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean,System.Xml.Serialization.XmlMappingAccess)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 101, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean,System.Xml.Serialization.XmlMappingAccess)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 102, "System.Private.Xml/System.Xml.Serialization.XmlReflectionImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.XmlReflectionMember[],System.Boolean,System.Boolean,System.Boolean,System.Xml.Serialization.XmlMappingAccess)", "generated", "System.Xml.Serialization.XmlReflectionImporter", "ImportMembersMapping", True, "benchmark" },
    { 103, "System.Private.Xml/System.Xml.Serialization.XmlSchemaExporter::ExportTypeMapping:System.Void(System.Xml.Serialization.XmlTypeMapping)", "generated", "System.Xml.Serialization.XmlSchemaExporter", "ExportTypeMapping", True, "benchmark" },
    { 104, "System.Private.Xml/System.Xml.Serialization.XmlSchemaExporter::ExportTypeMapping:System.Xml.XmlQualifiedName(System.Xml.Serialization.XmlMembersMapping)", "generated", "System.Xml.Serialization.XmlSchemaExporter", "ExportTypeMapping", True, "benchmark" },
    { 105, "System.Private.Xml/System.Xml.Serialization.XmlSchemaExporter::ExportMembersMapping:System.Void(System.Xml.Serialization.XmlMembersMapping)", "generated", "System.Xml.Serialization.XmlSchemaExporter", "ExportMembersMapping", True, "benchmark" },
    { 106, "System.Private.Xml/System.Xml.Serialization.XmlSchemaExporter::ExportMembersMapping:System.Void(System.Xml.Serialization.XmlMembersMapping,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemaExporter", "ExportMembersMapping", True, "benchmark" },
    { 107, "System.Private.Xml/System.Xml.Serialization.XmlSchemaExporter::ExportMembersMapping:System.Void(System.Xml.Serialization.XmlMembersMapping,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemaExporter", "ExportMembersMapping", True, "benchmark" },
    { 108, "System.Private.Xml/System.Xml.Serialization.XmlSchemaExporter::ExportAnyType:System.String(System.String)", "generated", "System.Xml.Serialization.XmlSchemaExporter", "ExportAnyType", True, "benchmark" },
    { 109, "System.Private.Xml/System.Xml.Serialization.XmlSchemaExporter::ExportAnyType:System.String(System.String)", "generated", "System.Xml.Serialization.XmlSchemaExporter", "ExportAnyType", True, "benchmark" },
    { 110, "System.Private.Xml/System.Xml.Serialization.XmlSchemaExporter::ExportAnyType:System.String(System.Xml.Serialization.XmlMembersMapping)", "generated", "System.Xml.Serialization.XmlSchemaExporter", "ExportAnyType", True, "benchmark" },
    { 111, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportDerivedTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Xml.XmlQualifiedName,System.Type)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportDerivedTypeMapping", True, "benchmark" },
    { 112, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportDerivedTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Xml.XmlQualifiedName,System.Type,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportDerivedTypeMapping", True, "benchmark" },
    { 113, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportDerivedTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Xml.XmlQualifiedName,System.Type,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportDerivedTypeMapping", True, "benchmark" },
    { 114, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportSchemaType:System.Xml.Serialization.XmlTypeMapping(System.Xml.XmlQualifiedName)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportSchemaType", True, "benchmark" },
    { 115, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportSchemaType:System.Xml.Serialization.XmlTypeMapping(System.Xml.XmlQualifiedName,System.Type)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportSchemaType", True, "benchmark" },
    { 116, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportSchemaType:System.Xml.Serialization.XmlTypeMapping(System.Xml.XmlQualifiedName,System.Type,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportSchemaType", True, "benchmark" },
    { 117, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportSchemaType:System.Xml.Serialization.XmlTypeMapping(System.Xml.XmlQualifiedName,System.Type,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportSchemaType", True, "benchmark" },
    { 118, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportTypeMapping:System.Xml.Serialization.XmlTypeMapping(System.Xml.XmlQualifiedName)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportTypeMapping", True, "benchmark" },
    { 119, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.Xml.XmlQualifiedName)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 120, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportAnyType:System.Xml.Serialization.XmlMembersMapping(System.Xml.XmlQualifiedName,System.String)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportAnyType", True, "benchmark" },
    { 121, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportAnyType:System.Xml.Serialization.XmlMembersMapping(System.Xml.XmlQualifiedName,System.String)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportAnyType", True, "benchmark" },
    { 122, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.Xml.XmlQualifiedName[])", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 123, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.Xml.XmlQualifiedName[])", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 124, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.Xml.XmlQualifiedName[])", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 125, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.Xml.XmlQualifiedName[],System.Type,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 126, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.Xml.XmlQualifiedName[],System.Type,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 127, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.Xml.XmlQualifiedName[],System.Type,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 128, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.Xml.XmlQualifiedName[],System.Type,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 129, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.SoapSchemaMember[])", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 130, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.SoapSchemaMember[])", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 131, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.SoapSchemaMember[])", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 132, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.SoapSchemaMember[])", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 133, "System.Private.Xml/System.Xml.Serialization.XmlSchemaImporter::ImportMembersMapping:System.Xml.Serialization.XmlMembersMapping(System.String,System.String,System.Xml.Serialization.SoapSchemaMember[])", "generated", "System.Xml.Serialization.XmlSchemaImporter", "ImportMembersMapping", True, "benchmark" },
    { 134, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::GetSchemas:System.Collections.IList(System.String)", "generated", "System.Xml.Serialization.XmlSchemas", "GetSchemas", True, "benchmark" },
    { 135, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::GetSchemas:System.Collections.IList(System.String)", "generated", "System.Xml.Serialization.XmlSchemas", "GetSchemas", True, "benchmark" },
    { 136, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Add:System.Int32(System.Xml.Schema.XmlSchema)", "generated", "System.Xml.Serialization.XmlSchemas", "Add", True, "benchmark" },
    { 137, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Add:System.Int32(System.Xml.Schema.XmlSchema,System.Uri)", "generated", "System.Xml.Serialization.XmlSchemas", "Add", True, "benchmark" },
    { 138, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Add:System.Void(System.Xml.Serialization.XmlSchemas)", "generated", "System.Xml.Serialization.XmlSchemas", "Add", True, "benchmark" },
    { 139, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::AddReference:System.Void(System.Xml.Schema.XmlSchema)", "generated", "System.Xml.Serialization.XmlSchemas", "AddReference", True, "benchmark" },
    { 140, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Insert:System.Void(System.Int32,System.Xml.Schema.XmlSchema)", "generated", "System.Xml.Serialization.XmlSchemas", "Insert", True, "benchmark" },
    { 141, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Insert:System.Void(System.Int32,System.Xml.Schema.XmlSchema)", "generated", "System.Xml.Serialization.XmlSchemas", "Insert", True, "benchmark" },
    { 142, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::IndexOf:System.Int32(System.Xml.Schema.XmlSchema)", "generated", "System.Xml.Serialization.XmlSchemas", "IndexOf", True, "benchmark" },
    { 143, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Contains:System.Boolean(System.Xml.Schema.XmlSchema)", "generated", "System.Xml.Serialization.XmlSchemas", "Contains", True, "benchmark" },
    { 144, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Contains:System.Boolean(System.String)", "generated", "System.Xml.Serialization.XmlSchemas", "Contains", True, "benchmark" },
    { 145, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Contains:System.Boolean(System.String)", "generated", "System.Xml.Serialization.XmlSchemas", "Contains", True, "benchmark" },
    { 146, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Remove:System.Void(System.Xml.Schema.XmlSchema)", "generated", "System.Xml.Serialization.XmlSchemas", "Remove", True, "benchmark" },
    { 147, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Find:System.Object(System.Xml.XmlQualifiedName,System.Type)", "generated", "System.Xml.Serialization.XmlSchemas", "Find", True, "benchmark" },
    { 148, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::IsDataSet:System.Boolean(System.Xml.Schema.XmlSchema)", "generated", "System.Xml.Serialization.XmlSchemas", "IsDataSet", True, "benchmark" },
    { 149, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Compile:System.Void(System.Xml.Schema.ValidationEventHandler,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemas", "Compile", True, "benchmark" },
    { 150, "System.Private.Xml/System.Xml.Serialization.XmlSchemas::Compile:System.Void(System.Xml.Schema.ValidationEventHandler,System.Boolean)", "generated", "System.Xml.Serialization.XmlSchemas", "Compile", True, "benchmark" },
    { 151, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.IO.TextWriter,System.Object)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 152, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.IO.TextWriter,System.Object)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 153, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.IO.TextWriter,System.Object,System.Xml.Serialization.XmlSerializerNamespaces)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 154, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.IO.TextWriter,System.Object,System.Xml.Serialization.XmlSerializerNamespaces)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 155, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.IO.Stream,System.Object)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 156, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.IO.Stream,System.Object)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 157, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Xml.Serialization.XmlSerializerNamespaces)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 158, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.IO.Stream,System.Object,System.Xml.Serialization.XmlSerializerNamespaces)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 159, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.Xml.XmlWriter,System.Object)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 160, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.Xml.XmlWriter,System.Object,System.Xml.Serialization.XmlSerializerNamespaces)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 161, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.Xml.XmlWriter,System.Object,System.Xml.Serialization.XmlSerializerNamespaces,System.String)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 162, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Serialize:System.Void(System.Xml.XmlWriter,System.Object,System.Xml.Serialization.XmlSerializerNamespaces,System.String,System.String)", "generated", "System.Xml.Serialization.XmlSerializer", "Serialize", True, "benchmark" },
    { 163, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Deserialize:System.Object(System.IO.Stream)", "generated", "System.Xml.Serialization.XmlSerializer", "Deserialize", True, "benchmark" },
    { 164, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Deserialize:System.Object(System.IO.Stream)", "generated", "System.Xml.Serialization.XmlSerializer", "Deserialize", True, "benchmark" },
    { 165, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Deserialize:System.Object(System.IO.TextReader)", "generated", "System.Xml.Serialization.XmlSerializer", "Deserialize", True, "benchmark" },
    { 166, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Deserialize:System.Object(System.IO.TextReader)", "generated", "System.Xml.Serialization.XmlSerializer", "Deserialize", True, "benchmark" },
    { 167, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Deserialize:System.Object(System.Xml.XmlReader)", "generated", "System.Xml.Serialization.XmlSerializer", "Deserialize", True, "benchmark" },
    { 168, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Deserialize:System.Object(System.Xml.XmlReader,System.Xml.Serialization.XmlDeserializationEvents)", "generated", "System.Xml.Serialization.XmlSerializer", "Deserialize", True, "benchmark" },
    { 169, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Deserialize:System.Object(System.Xml.XmlReader,System.String)", "generated", "System.Xml.Serialization.XmlSerializer", "Deserialize", True, "benchmark" },
    { 170, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Deserialize:System.Object(System.Xml.XmlReader,System.String)", "generated", "System.Xml.Serialization.XmlSerializer", "Deserialize", True, "benchmark" },
    { 171, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Deserialize:System.Object(System.Xml.XmlReader,System.String,System.Xml.Serialization.XmlDeserializationEvents)", "generated", "System.Xml.Serialization.XmlSerializer", "Deserialize", True, "benchmark" },
    { 172, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::Deserialize:System.Object(System.Xml.XmlReader,System.String,System.Xml.Serialization.XmlDeserializationEvents)", "generated", "System.Xml.Serialization.XmlSerializer", "Deserialize", True, "benchmark" },
    { 173, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::CanDeserialize:System.Boolean(System.Xml.XmlReader)", "generated", "System.Xml.Serialization.XmlSerializer", "CanDeserialize", True, "benchmark" },
    { 174, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::FromMappings:System.Xml.Serialization.XmlSerializer[](System.Xml.Serialization.XmlMapping[])", "generated", "System.Xml.Serialization.XmlSerializer", "FromMappings", True, "benchmark" },
    { 175, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::FromMappings:System.Xml.Serialization.XmlSerializer[](System.Xml.Serialization.XmlMapping[])", "generated", "System.Xml.Serialization.XmlSerializer", "FromMappings", True, "benchmark" },
    { 176, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::FromMappings:System.Xml.Serialization.XmlSerializer[](System.Xml.Serialization.XmlMapping[])", "generated", "System.Xml.Serialization.XmlSerializer", "FromMappings", True, "benchmark" },
    { 177, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::FromMappings:System.Xml.Serialization.XmlSerializer[](System.Xml.Serialization.XmlMapping[],System.Type)", "generated", "System.Xml.Serialization.XmlSerializer", "FromMappings", True, "benchmark" },
    { 178, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::FromMappings:System.Xml.Serialization.XmlSerializer[](System.Xml.Serialization.XmlMapping[],System.Type)", "generated", "System.Xml.Serialization.XmlSerializer", "FromMappings", True, "benchmark" },
    { 179, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::FromMappings:System.Xml.Serialization.XmlSerializer[](System.Xml.Serialization.XmlMapping[],System.Type)", "generated", "System.Xml.Serialization.XmlSerializer", "FromMappings", True, "benchmark" },
    { 180, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::FromTypes:System.Xml.Serialization.XmlSerializer[](System.Type[])", "generated", "System.Xml.Serialization.XmlSerializer", "FromTypes", True, "benchmark" },
    { 181, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::FromTypes:System.Xml.Serialization.XmlSerializer[](System.Type[])", "generated", "System.Xml.Serialization.XmlSerializer", "FromTypes", True, "benchmark" },
    { 182, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::FromTypes:System.Xml.Serialization.XmlSerializer[](System.Type[])", "generated", "System.Xml.Serialization.XmlSerializer", "FromTypes", True, "benchmark" },
    { 183, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::GetXmlSerializerAssemblyName:System.String(System.Type)", "generated", "System.Xml.Serialization.XmlSerializer", "GetXmlSerializerAssemblyName", True, "benchmark" },
    { 184, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::GetXmlSerializerAssemblyName:System.String(System.Type,System.String)", "generated", "System.Xml.Serialization.XmlSerializer", "GetXmlSerializerAssemblyName", True, "benchmark" },
    { 185, "System.Private.Xml/System.Xml.Serialization.XmlSerializer::GetXmlSerializerAssemblyName:System.String(System.Type,System.String)", "generated", "System.Xml.Serialization.XmlSerializer", "GetXmlSerializerAssemblyName", True, "benchmark" },
    { 186, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Xml.Serialization.XmlAttributeOverrides,System.Type[],System.Xml.Serialization.XmlRootAttribute,System.String)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 187, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Xml.Serialization.XmlAttributeOverrides,System.Type[],System.Xml.Serialization.XmlRootAttribute,System.String)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 188, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Xml.Serialization.XmlAttributeOverrides,System.Type[],System.Xml.Serialization.XmlRootAttribute,System.String)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 189, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Xml.Serialization.XmlRootAttribute)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 190, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Type[])", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 191, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Type[])", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 192, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Type[])", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 193, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Xml.Serialization.XmlAttributeOverrides)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 194, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Xml.Serialization.XmlTypeMapping)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 195, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 196, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.String)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 197, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.String)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 198, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Xml.Serialization.XmlAttributeOverrides,System.Type[],System.Xml.Serialization.XmlRootAttribute,System.String,System.String)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 199, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Xml.Serialization.XmlAttributeOverrides,System.Type[],System.Xml.Serialization.XmlRootAttribute,System.String,System.String)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 200, "System.Private.Xml/System.Xml.Serialization.XmlSerializerFactory::CreateSerializer:System.Xml.Serialization.XmlSerializer(System.Type,System.Xml.Serialization.XmlAttributeOverrides,System.Type[],System.Xml.Serialization.XmlRootAttribute,System.String,System.String)", "generated", "System.Xml.Serialization.XmlSerializerFactory", "CreateSerializer", True, "benchmark" },
    { 201, "System.Private.Xml/System.Xml.Serialization.XmlSerializerImplementation::CanSerialize:System.Boolean(System.Type)", "generated", "System.Xml.Serialization.XmlSerializerImplementation", "CanSerialize", True, "benchmark" },
    { 202, "System.Private.Xml/System.Xml.Serialization.XmlSerializerImplementation::GetSerializer:System.Xml.Serialization.XmlSerializer(System.Type)", "generated", "System.Xml.Serialization.XmlSerializerImplementation", "GetSerializer", True, "benchmark" },
    { 203, "System.Private.Xml/System.Xml.Serialization.XmlSerializerNamespaces::Add:System.Void(System.String,System.String)", "generated", "System.Xml.Serialization.XmlSerializerNamespaces", "Add", True, "benchmark" },
    { 204, "System.Private.Xml/System.Xml.Serialization.XmlSerializerNamespaces::Add:System.Void(System.String,System.String)", "generated", "System.Xml.Serialization.XmlSerializerNamespaces", "Add", True, "benchmark" },
    { 205, "System.Private.Xml/System.Xml.Serialization.XmlSerializerNamespaces::Add:System.Void(System.String,System.String)", "generated", "System.Xml.Serialization.XmlSerializerNamespaces", "Add", True, "benchmark" },
    { 206, "System.Private.Xml/System.Xml.Serialization.XmlSerializerNamespaces::ToArray:System.Xml.XmlQualifiedName[]()", "generated", "System.Xml.Serialization.XmlSerializerNamespaces", "ToArray", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 207;

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