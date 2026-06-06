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
    { 0, "System.Private.Xml/System.Xml.XPath.XPathDocument::CreateNavigator:System.Xml.XPath.XPathNavigator()", "generated", "System.Xml.XPath.XPathDocument", "CreateNavigator", True, "benchmark" },
    { 1, "System.Private.Xml/System.Xml.XPath.XPathExpression::AddSort:System.Void(System.Object,System.Collections.IComparer)", "generated", "System.Xml.XPath.XPathExpression", "AddSort", True, "benchmark" },
    { 2, "System.Private.Xml/System.Xml.XPath.XPathExpression::AddSort:System.Void(System.Object,System.Collections.IComparer)", "generated", "System.Xml.XPath.XPathExpression", "AddSort", True, "benchmark" },
    { 3, "System.Private.Xml/System.Xml.XPath.XPathExpression::AddSort:System.Void(System.Object,System.Xml.XPath.XmlSortOrder,System.Xml.XPath.XmlCaseOrder,System.String,System.Xml.XPath.XmlDataType)", "generated", "System.Xml.XPath.XPathExpression", "AddSort", True, "benchmark" },
    { 4, "System.Private.Xml/System.Xml.XPath.XPathExpression::Clone:System.Xml.XPath.XPathExpression()", "generated", "System.Xml.XPath.XPathExpression", "Clone", True, "benchmark" },
    { 5, "System.Private.Xml/System.Xml.XPath.XPathExpression::SetContext:System.Void(System.Xml.XmlNamespaceManager)", "generated", "System.Xml.XPath.XPathExpression", "SetContext", True, "benchmark" },
    { 6, "System.Private.Xml/System.Xml.XPath.XPathExpression::SetContext:System.Void(System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathExpression", "SetContext", True, "benchmark" },
    { 7, "System.Private.Xml/System.Xml.XPath.XPathExpression::Compile:System.Xml.XPath.XPathExpression(System.String)", "generated", "System.Xml.XPath.XPathExpression", "Compile", True, "benchmark" },
    { 8, "System.Private.Xml/System.Xml.XPath.XPathExpression::Compile:System.Xml.XPath.XPathExpression(System.String)", "generated", "System.Xml.XPath.XPathExpression", "Compile", True, "benchmark" },
    { 9, "System.Private.Xml/System.Xml.XPath.XPathExpression::Compile:System.Xml.XPath.XPathExpression(System.String,System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathExpression", "Compile", True, "benchmark" },
    { 10, "System.Private.Xml/System.Xml.XPath.XPathExpression::Compile:System.Xml.XPath.XPathExpression(System.String,System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathExpression", "Compile", True, "benchmark" },
    { 11, "System.Private.Xml/System.Xml.XPath.XPathItem::ValueAs:System.Object(System.Type)", "generated", "System.Xml.XPath.XPathItem", "ValueAs", True, "benchmark" },
    { 12, "System.Private.Xml/System.Xml.XPath.XPathItem::ValueAs:System.Object(System.Type,System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathItem", "ValueAs", True, "benchmark" },
    { 13, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SetValue:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "SetValue", True, "benchmark" },
    { 14, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SetValue:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "SetValue", True, "benchmark" },
    { 15, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SetTypedValue:System.Void(System.Object)", "generated", "System.Xml.XPath.XPathNavigator", "SetTypedValue", True, "benchmark" },
    { 16, "System.Private.Xml/System.Xml.XPath.XPathNavigator::ValueAs:System.Object(System.Type,System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathNavigator", "ValueAs", True, "benchmark" },
    { 17, "System.Private.Xml/System.Xml.XPath.XPathNavigator::CreateNavigator:System.Xml.XPath.XPathNavigator()", "generated", "System.Xml.XPath.XPathNavigator", "CreateNavigator", True, "benchmark" },
    { 18, "System.Private.Xml/System.Xml.XPath.XPathNavigator::LookupNamespace:System.String(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "LookupNamespace", True, "benchmark" },
    { 19, "System.Private.Xml/System.Xml.XPath.XPathNavigator::LookupNamespace:System.String(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "LookupNamespace", True, "benchmark" },
    { 20, "System.Private.Xml/System.Xml.XPath.XPathNavigator::LookupPrefix:System.String(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "LookupPrefix", True, "benchmark" },
    { 21, "System.Private.Xml/System.Xml.XPath.XPathNavigator::LookupPrefix:System.String(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "LookupPrefix", True, "benchmark" },
    { 22, "System.Private.Xml/System.Xml.XPath.XPathNavigator::GetNamespacesInScope:System.Collections.Generic.IDictionary<System.String,System.String>(System.Xml.XmlNamespaceScope)", "generated", "System.Xml.XPath.XPathNavigator", "GetNamespacesInScope", True, "benchmark" },
    { 23, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Clone:System.Xml.XPath.XPathNavigator()", "generated", "System.Xml.XPath.XPathNavigator", "Clone", True, "benchmark" },
    { 24, "System.Private.Xml/System.Xml.XPath.XPathNavigator::ReadSubtree:System.Xml.XmlReader()", "generated", "System.Xml.XPath.XPathNavigator", "ReadSubtree", True, "benchmark" },
    { 25, "System.Private.Xml/System.Xml.XPath.XPathNavigator::WriteSubtree:System.Void(System.Xml.XmlWriter)", "generated", "System.Xml.XPath.XPathNavigator", "WriteSubtree", True, "benchmark" },
    { 26, "System.Private.Xml/System.Xml.XPath.XPathNavigator::GetAttribute:System.String(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "GetAttribute", True, "benchmark" },
    { 27, "System.Private.Xml/System.Xml.XPath.XPathNavigator::GetAttribute:System.String(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "GetAttribute", True, "benchmark" },
    { 28, "System.Private.Xml/System.Xml.XPath.XPathNavigator::GetAttribute:System.String(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "GetAttribute", True, "benchmark" },
    { 29, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToAttribute:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToAttribute", True, "benchmark" },
    { 30, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToAttribute:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToAttribute", True, "benchmark" },
    { 31, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToAttribute:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToAttribute", True, "benchmark" },
    { 32, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFirstAttribute:System.Boolean()", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFirstAttribute", True, "benchmark" },
    { 33, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToNextAttribute:System.Boolean()", "generated", "System.Xml.XPath.XPathNavigator", "MoveToNextAttribute", True, "benchmark" },
    { 34, "System.Private.Xml/System.Xml.XPath.XPathNavigator::GetNamespace:System.String(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "GetNamespace", True, "benchmark" },
    { 35, "System.Private.Xml/System.Xml.XPath.XPathNavigator::GetNamespace:System.String(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "GetNamespace", True, "benchmark" },
    { 36, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToNamespace:System.Boolean(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToNamespace", True, "benchmark" },
    { 37, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToNamespace:System.Boolean(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToNamespace", True, "benchmark" },
    { 38, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFirstNamespace:System.Boolean(System.Xml.XPath.XPathNamespaceScope)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFirstNamespace", True, "benchmark" },
    { 39, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToNextNamespace:System.Boolean(System.Xml.XPath.XPathNamespaceScope)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToNextNamespace", True, "benchmark" },
    { 40, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFirstNamespace:System.Boolean()", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFirstNamespace", True, "benchmark" },
    { 41, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToNextNamespace:System.Boolean()", "generated", "System.Xml.XPath.XPathNavigator", "MoveToNextNamespace", True, "benchmark" },
    { 42, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToNext:System.Boolean()", "generated", "System.Xml.XPath.XPathNavigator", "MoveToNext", True, "benchmark" },
    { 43, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToPrevious:System.Boolean()", "generated", "System.Xml.XPath.XPathNavigator", "MoveToPrevious", True, "benchmark" },
    { 44, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFirst:System.Boolean()", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFirst", True, "benchmark" },
    { 45, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFirstChild:System.Boolean()", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFirstChild", True, "benchmark" },
    { 46, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToParent:System.Boolean()", "generated", "System.Xml.XPath.XPathNavigator", "MoveToParent", True, "benchmark" },
    { 47, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToRoot:System.Void()", "generated", "System.Xml.XPath.XPathNavigator", "MoveToRoot", True, "benchmark" },
    { 48, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveTo:System.Boolean(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "MoveTo", True, "benchmark" },
    { 49, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToId:System.Boolean(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToId", True, "benchmark" },
    { 50, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToId:System.Boolean(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToId", True, "benchmark" },
    { 51, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToChild:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToChild", True, "benchmark" },
    { 52, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToChild:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToChild", True, "benchmark" },
    { 53, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToChild:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToChild", True, "benchmark" },
    { 54, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToChild:System.Boolean(System.Xml.XPath.XPathNodeType)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToChild", True, "benchmark" },
    { 55, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFollowing:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFollowing", True, "benchmark" },
    { 56, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFollowing:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFollowing", True, "benchmark" },
    { 57, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFollowing:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFollowing", True, "benchmark" },
    { 58, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFollowing:System.Boolean(System.String,System.String,System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFollowing", True, "benchmark" },
    { 59, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFollowing:System.Boolean(System.String,System.String,System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFollowing", True, "benchmark" },
    { 60, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFollowing:System.Boolean(System.String,System.String,System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFollowing", True, "benchmark" },
    { 61, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFollowing:System.Boolean(System.Xml.XPath.XPathNodeType)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFollowing", True, "benchmark" },
    { 62, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToFollowing:System.Boolean(System.Xml.XPath.XPathNodeType,System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToFollowing", True, "benchmark" },
    { 63, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToNext:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToNext", True, "benchmark" },
    { 64, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToNext:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToNext", True, "benchmark" },
    { 65, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToNext:System.Boolean(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToNext", True, "benchmark" },
    { 66, "System.Private.Xml/System.Xml.XPath.XPathNavigator::MoveToNext:System.Boolean(System.Xml.XPath.XPathNodeType)", "generated", "System.Xml.XPath.XPathNavigator", "MoveToNext", True, "benchmark" },
    { 67, "System.Private.Xml/System.Xml.XPath.XPathNavigator::IsSamePosition:System.Boolean(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "IsSamePosition", True, "benchmark" },
    { 68, "System.Private.Xml/System.Xml.XPath.XPathNavigator::IsDescendant:System.Boolean(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "IsDescendant", True, "benchmark" },
    { 69, "System.Private.Xml/System.Xml.XPath.XPathNavigator::ComparePosition:System.Xml.XmlNodeOrder(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "ComparePosition", True, "benchmark" },
    { 70, "System.Private.Xml/System.Xml.XPath.XPathNavigator::CheckValidity:System.Boolean(System.Xml.Schema.XmlSchemaSet,System.Xml.Schema.ValidationEventHandler)", "generated", "System.Xml.XPath.XPathNavigator", "CheckValidity", True, "benchmark" },
    { 71, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Compile:System.Xml.XPath.XPathExpression(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "Compile", True, "benchmark" },
    { 72, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Compile:System.Xml.XPath.XPathExpression(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "Compile", True, "benchmark" },
    { 73, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectSingleNode:System.Xml.XPath.XPathNavigator(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "SelectSingleNode", True, "benchmark" },
    { 74, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectSingleNode:System.Xml.XPath.XPathNavigator(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "SelectSingleNode", True, "benchmark" },
    { 75, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectSingleNode:System.Xml.XPath.XPathNavigator(System.String,System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathNavigator", "SelectSingleNode", True, "benchmark" },
    { 76, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectSingleNode:System.Xml.XPath.XPathNavigator(System.String,System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathNavigator", "SelectSingleNode", True, "benchmark" },
    { 77, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectSingleNode:System.Xml.XPath.XPathNavigator(System.Xml.XPath.XPathExpression)", "generated", "System.Xml.XPath.XPathNavigator", "SelectSingleNode", True, "benchmark" },
    { 78, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Select:System.Xml.XPath.XPathNodeIterator(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "Select", True, "benchmark" },
    { 79, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Select:System.Xml.XPath.XPathNodeIterator(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "Select", True, "benchmark" },
    { 80, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Select:System.Xml.XPath.XPathNodeIterator(System.String,System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathNavigator", "Select", True, "benchmark" },
    { 81, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Select:System.Xml.XPath.XPathNodeIterator(System.String,System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathNavigator", "Select", True, "benchmark" },
    { 82, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Select:System.Xml.XPath.XPathNodeIterator(System.Xml.XPath.XPathExpression)", "generated", "System.Xml.XPath.XPathNavigator", "Select", True, "benchmark" },
    { 83, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Evaluate:System.Object(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "Evaluate", True, "benchmark" },
    { 84, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Evaluate:System.Object(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "Evaluate", True, "benchmark" },
    { 85, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Evaluate:System.Object(System.String,System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathNavigator", "Evaluate", True, "benchmark" },
    { 86, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Evaluate:System.Object(System.String,System.Xml.IXmlNamespaceResolver)", "generated", "System.Xml.XPath.XPathNavigator", "Evaluate", True, "benchmark" },
    { 87, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Evaluate:System.Object(System.Xml.XPath.XPathExpression)", "generated", "System.Xml.XPath.XPathNavigator", "Evaluate", True, "benchmark" },
    { 88, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Evaluate:System.Object(System.Xml.XPath.XPathExpression,System.Xml.XPath.XPathNodeIterator)", "generated", "System.Xml.XPath.XPathNavigator", "Evaluate", True, "benchmark" },
    { 89, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Matches:System.Boolean(System.Xml.XPath.XPathExpression)", "generated", "System.Xml.XPath.XPathNavigator", "Matches", True, "benchmark" },
    { 90, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Matches:System.Boolean(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "Matches", True, "benchmark" },
    { 91, "System.Private.Xml/System.Xml.XPath.XPathNavigator::Matches:System.Boolean(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "Matches", True, "benchmark" },
    { 92, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectChildren:System.Xml.XPath.XPathNodeIterator(System.Xml.XPath.XPathNodeType)", "generated", "System.Xml.XPath.XPathNavigator", "SelectChildren", True, "benchmark" },
    { 93, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectChildren:System.Xml.XPath.XPathNodeIterator(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "SelectChildren", True, "benchmark" },
    { 94, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectChildren:System.Xml.XPath.XPathNodeIterator(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "SelectChildren", True, "benchmark" },
    { 95, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectChildren:System.Xml.XPath.XPathNodeIterator(System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "SelectChildren", True, "benchmark" },
    { 96, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectAncestors:System.Xml.XPath.XPathNodeIterator(System.Xml.XPath.XPathNodeType,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectAncestors", True, "benchmark" },
    { 97, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectAncestors:System.Xml.XPath.XPathNodeIterator(System.Xml.XPath.XPathNodeType,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectAncestors", True, "benchmark" },
    { 98, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectAncestors:System.Xml.XPath.XPathNodeIterator(System.String,System.String,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectAncestors", True, "benchmark" },
    { 99, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectAncestors:System.Xml.XPath.XPathNodeIterator(System.String,System.String,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectAncestors", True, "benchmark" },
    { 100, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectAncestors:System.Xml.XPath.XPathNodeIterator(System.String,System.String,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectAncestors", True, "benchmark" },
    { 101, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectAncestors:System.Xml.XPath.XPathNodeIterator(System.String,System.String,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectAncestors", True, "benchmark" },
    { 102, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectDescendants:System.Xml.XPath.XPathNodeIterator(System.Xml.XPath.XPathNodeType,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectDescendants", True, "benchmark" },
    { 103, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectDescendants:System.Xml.XPath.XPathNodeIterator(System.Xml.XPath.XPathNodeType,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectDescendants", True, "benchmark" },
    { 104, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectDescendants:System.Xml.XPath.XPathNodeIterator(System.String,System.String,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectDescendants", True, "benchmark" },
    { 105, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectDescendants:System.Xml.XPath.XPathNodeIterator(System.String,System.String,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectDescendants", True, "benchmark" },
    { 106, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectDescendants:System.Xml.XPath.XPathNodeIterator(System.String,System.String,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectDescendants", True, "benchmark" },
    { 107, "System.Private.Xml/System.Xml.XPath.XPathNavigator::SelectDescendants:System.Xml.XPath.XPathNodeIterator(System.String,System.String,System.Boolean)", "generated", "System.Xml.XPath.XPathNavigator", "SelectDescendants", True, "benchmark" },
    { 108, "System.Private.Xml/System.Xml.XPath.XPathNavigator::PrependChild:System.Xml.XmlWriter()", "generated", "System.Xml.XPath.XPathNavigator", "PrependChild", True, "benchmark" },
    { 109, "System.Private.Xml/System.Xml.XPath.XPathNavigator::AppendChild:System.Xml.XmlWriter()", "generated", "System.Xml.XPath.XPathNavigator", "AppendChild", True, "benchmark" },
    { 110, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertAfter:System.Xml.XmlWriter()", "generated", "System.Xml.XPath.XPathNavigator", "InsertAfter", True, "benchmark" },
    { 111, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertBefore:System.Xml.XmlWriter()", "generated", "System.Xml.XPath.XPathNavigator", "InsertBefore", True, "benchmark" },
    { 112, "System.Private.Xml/System.Xml.XPath.XPathNavigator::CreateAttributes:System.Xml.XmlWriter()", "generated", "System.Xml.XPath.XPathNavigator", "CreateAttributes", True, "benchmark" },
    { 113, "System.Private.Xml/System.Xml.XPath.XPathNavigator::ReplaceRange:System.Xml.XmlWriter(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "ReplaceRange", True, "benchmark" },
    { 114, "System.Private.Xml/System.Xml.XPath.XPathNavigator::ReplaceSelf:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "ReplaceSelf", True, "benchmark" },
    { 115, "System.Private.Xml/System.Xml.XPath.XPathNavigator::ReplaceSelf:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "ReplaceSelf", True, "benchmark" },
    { 116, "System.Private.Xml/System.Xml.XPath.XPathNavigator::ReplaceSelf:System.Void(System.Xml.XmlReader)", "generated", "System.Xml.XPath.XPathNavigator", "ReplaceSelf", True, "benchmark" },
    { 117, "System.Private.Xml/System.Xml.XPath.XPathNavigator::ReplaceSelf:System.Void(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "ReplaceSelf", True, "benchmark" },
    { 118, "System.Private.Xml/System.Xml.XPath.XPathNavigator::AppendChild:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "AppendChild", True, "benchmark" },
    { 119, "System.Private.Xml/System.Xml.XPath.XPathNavigator::AppendChild:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "AppendChild", True, "benchmark" },
    { 120, "System.Private.Xml/System.Xml.XPath.XPathNavigator::AppendChild:System.Void(System.Xml.XmlReader)", "generated", "System.Xml.XPath.XPathNavigator", "AppendChild", True, "benchmark" },
    { 121, "System.Private.Xml/System.Xml.XPath.XPathNavigator::AppendChild:System.Void(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "AppendChild", True, "benchmark" },
    { 122, "System.Private.Xml/System.Xml.XPath.XPathNavigator::PrependChild:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "PrependChild", True, "benchmark" },
    { 123, "System.Private.Xml/System.Xml.XPath.XPathNavigator::PrependChild:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "PrependChild", True, "benchmark" },
    { 124, "System.Private.Xml/System.Xml.XPath.XPathNavigator::PrependChild:System.Void(System.Xml.XmlReader)", "generated", "System.Xml.XPath.XPathNavigator", "PrependChild", True, "benchmark" },
    { 125, "System.Private.Xml/System.Xml.XPath.XPathNavigator::PrependChild:System.Void(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "PrependChild", True, "benchmark" },
    { 126, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertBefore:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertBefore", True, "benchmark" },
    { 127, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertBefore:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertBefore", True, "benchmark" },
    { 128, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertBefore:System.Void(System.Xml.XmlReader)", "generated", "System.Xml.XPath.XPathNavigator", "InsertBefore", True, "benchmark" },
    { 129, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertBefore:System.Void(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "InsertBefore", True, "benchmark" },
    { 130, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertAfter:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertAfter", True, "benchmark" },
    { 131, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertAfter:System.Void(System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertAfter", True, "benchmark" },
    { 132, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertAfter:System.Void(System.Xml.XmlReader)", "generated", "System.Xml.XPath.XPathNavigator", "InsertAfter", True, "benchmark" },
    { 133, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertAfter:System.Void(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "InsertAfter", True, "benchmark" },
    { 134, "System.Private.Xml/System.Xml.XPath.XPathNavigator::DeleteRange:System.Void(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.XPath.XPathNavigator", "DeleteRange", True, "benchmark" },
    { 135, "System.Private.Xml/System.Xml.XPath.XPathNavigator::DeleteSelf:System.Void()", "generated", "System.Xml.XPath.XPathNavigator", "DeleteSelf", True, "benchmark" },
    { 136, "System.Private.Xml/System.Xml.XPath.XPathNavigator::PrependChildElement:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "PrependChildElement", True, "benchmark" },
    { 137, "System.Private.Xml/System.Xml.XPath.XPathNavigator::PrependChildElement:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "PrependChildElement", True, "benchmark" },
    { 138, "System.Private.Xml/System.Xml.XPath.XPathNavigator::PrependChildElement:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "PrependChildElement", True, "benchmark" },
    { 139, "System.Private.Xml/System.Xml.XPath.XPathNavigator::PrependChildElement:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "PrependChildElement", True, "benchmark" },
    { 140, "System.Private.Xml/System.Xml.XPath.XPathNavigator::AppendChildElement:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "AppendChildElement", True, "benchmark" },
    { 141, "System.Private.Xml/System.Xml.XPath.XPathNavigator::AppendChildElement:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "AppendChildElement", True, "benchmark" },
    { 142, "System.Private.Xml/System.Xml.XPath.XPathNavigator::AppendChildElement:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "AppendChildElement", True, "benchmark" },
    { 143, "System.Private.Xml/System.Xml.XPath.XPathNavigator::AppendChildElement:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "AppendChildElement", True, "benchmark" },
    { 144, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertElementBefore:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertElementBefore", True, "benchmark" },
    { 145, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertElementBefore:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertElementBefore", True, "benchmark" },
    { 146, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertElementBefore:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertElementBefore", True, "benchmark" },
    { 147, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertElementBefore:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertElementBefore", True, "benchmark" },
    { 148, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertElementAfter:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertElementAfter", True, "benchmark" },
    { 149, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertElementAfter:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertElementAfter", True, "benchmark" },
    { 150, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertElementAfter:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertElementAfter", True, "benchmark" },
    { 151, "System.Private.Xml/System.Xml.XPath.XPathNavigator::InsertElementAfter:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "InsertElementAfter", True, "benchmark" },
    { 152, "System.Private.Xml/System.Xml.XPath.XPathNavigator::CreateAttribute:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "CreateAttribute", True, "benchmark" },
    { 153, "System.Private.Xml/System.Xml.XPath.XPathNavigator::CreateAttribute:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "CreateAttribute", True, "benchmark" },
    { 154, "System.Private.Xml/System.Xml.XPath.XPathNavigator::CreateAttribute:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "CreateAttribute", True, "benchmark" },
    { 155, "System.Private.Xml/System.Xml.XPath.XPathNavigator::CreateAttribute:System.Void(System.String,System.String,System.String,System.String)", "generated", "System.Xml.XPath.XPathNavigator", "CreateAttribute", True, "benchmark" },
    { 156, "System.Private.Xml/System.Xml.XPath.XPathNodeIterator::Clone:System.Xml.XPath.XPathNodeIterator()", "generated", "System.Xml.XPath.XPathNodeIterator", "Clone", True, "benchmark" },
    { 157, "System.Private.Xml/System.Xml.XPath.XPathNodeIterator::GetEnumerator:System.Collections.IEnumerator()", "generated", "System.Xml.XPath.XPathNodeIterator", "GetEnumerator", True, "benchmark" },
    { 158, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.Xml.XmlReader)", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 159, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.Xml.XmlReader,System.Xml.Xsl.XsltSettings,System.Xml.XmlResolver)", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 160, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.Xml.XPath.IXPathNavigable)", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 161, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.Xml.XPath.IXPathNavigable,System.Xml.Xsl.XsltSettings,System.Xml.XmlResolver)", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 162, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.String)", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 163, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.String)", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 164, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.String,System.Xml.Xsl.XsltSettings,System.Xml.XmlResolver)", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 165, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.String,System.Xml.Xsl.XsltSettings,System.Xml.XmlResolver)", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 166, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.Type)", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 167, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.Reflection.MethodInfo,System.Byte[],System.Type[])", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 168, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.Reflection.MethodInfo,System.Byte[],System.Type[])", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 169, "System.Private.Xml/System.Xml.Xsl.XslCompiledTransform::Load:System.Void(System.Reflection.MethodInfo,System.Byte[],System.Type[])", "generated", "System.Xml.Xsl.XslCompiledTransform", "Load", True, "benchmark" },
    { 170, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::GetParam:System.Object(System.String,System.String)", "generated", "System.Xml.Xsl.XsltArgumentList", "GetParam", True, "benchmark" },
    { 171, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::GetParam:System.Object(System.String,System.String)", "generated", "System.Xml.Xsl.XsltArgumentList", "GetParam", True, "benchmark" },
    { 172, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::GetParam:System.Object(System.String,System.String)", "generated", "System.Xml.Xsl.XsltArgumentList", "GetParam", True, "benchmark" },
    { 173, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::GetExtensionObject:System.Object(System.String)", "generated", "System.Xml.Xsl.XsltArgumentList", "GetExtensionObject", True, "benchmark" },
    { 174, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::GetExtensionObject:System.Object(System.String)", "generated", "System.Xml.Xsl.XsltArgumentList", "GetExtensionObject", True, "benchmark" },
    { 175, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::AddParam:System.Void(System.String,System.String,System.Object)", "generated", "System.Xml.Xsl.XsltArgumentList", "AddParam", True, "benchmark" },
    { 176, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::AddParam:System.Void(System.String,System.String,System.Object)", "generated", "System.Xml.Xsl.XsltArgumentList", "AddParam", True, "benchmark" },
    { 177, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::AddParam:System.Void(System.String,System.String,System.Object)", "generated", "System.Xml.Xsl.XsltArgumentList", "AddParam", True, "benchmark" },
    { 178, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::AddExtensionObject:System.Void(System.String,System.Object)", "generated", "System.Xml.Xsl.XsltArgumentList", "AddExtensionObject", True, "benchmark" },
    { 179, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::AddExtensionObject:System.Void(System.String,System.Object)", "generated", "System.Xml.Xsl.XsltArgumentList", "AddExtensionObject", True, "benchmark" },
    { 180, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::RemoveParam:System.Object(System.String,System.String)", "generated", "System.Xml.Xsl.XsltArgumentList", "RemoveParam", True, "benchmark" },
    { 181, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::RemoveParam:System.Object(System.String,System.String)", "generated", "System.Xml.Xsl.XsltArgumentList", "RemoveParam", True, "benchmark" },
    { 182, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::RemoveParam:System.Object(System.String,System.String)", "generated", "System.Xml.Xsl.XsltArgumentList", "RemoveParam", True, "benchmark" },
    { 183, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::RemoveExtensionObject:System.Object(System.String)", "generated", "System.Xml.Xsl.XsltArgumentList", "RemoveExtensionObject", True, "benchmark" },
    { 184, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::RemoveExtensionObject:System.Object(System.String)", "generated", "System.Xml.Xsl.XsltArgumentList", "RemoveExtensionObject", True, "benchmark" },
    { 185, "System.Private.Xml/System.Xml.Xsl.XsltArgumentList::Clear:System.Void()", "generated", "System.Xml.Xsl.XsltArgumentList", "Clear", True, "benchmark" },
    { 186, "System.Private.Xml/System.Xml.Xsl.XsltContext::ResolveVariable:System.Xml.Xsl.IXsltContextVariable(System.String,System.String)", "generated", "System.Xml.Xsl.XsltContext", "ResolveVariable", True, "benchmark" },
    { 187, "System.Private.Xml/System.Xml.Xsl.XsltContext::ResolveVariable:System.Xml.Xsl.IXsltContextVariable(System.String,System.String)", "generated", "System.Xml.Xsl.XsltContext", "ResolveVariable", True, "benchmark" },
    { 188, "System.Private.Xml/System.Xml.Xsl.XsltContext::ResolveVariable:System.Xml.Xsl.IXsltContextVariable(System.String,System.String)", "generated", "System.Xml.Xsl.XsltContext", "ResolveVariable", True, "benchmark" },
    { 189, "System.Private.Xml/System.Xml.Xsl.XsltContext::ResolveFunction:System.Xml.Xsl.IXsltContextFunction(System.String,System.String,System.Xml.XPath.XPathResultType[])", "generated", "System.Xml.Xsl.XsltContext", "ResolveFunction", True, "benchmark" },
    { 190, "System.Private.Xml/System.Xml.Xsl.XsltContext::ResolveFunction:System.Xml.Xsl.IXsltContextFunction(System.String,System.String,System.Xml.XPath.XPathResultType[])", "generated", "System.Xml.Xsl.XsltContext", "ResolveFunction", True, "benchmark" },
    { 191, "System.Private.Xml/System.Xml.Xsl.XsltContext::ResolveFunction:System.Xml.Xsl.IXsltContextFunction(System.String,System.String,System.Xml.XPath.XPathResultType[])", "generated", "System.Xml.Xsl.XsltContext", "ResolveFunction", True, "benchmark" },
    { 192, "System.Private.Xml/System.Xml.Xsl.XsltContext::ResolveFunction:System.Xml.Xsl.IXsltContextFunction(System.String,System.String,System.Xml.XPath.XPathResultType[])", "generated", "System.Xml.Xsl.XsltContext", "ResolveFunction", True, "benchmark" },
    { 193, "System.Private.Xml/System.Xml.Xsl.XsltContext::ResolveFunction:System.Xml.Xsl.IXsltContextFunction(System.String,System.String,System.Xml.XPath.XPathResultType[])", "generated", "System.Xml.Xsl.XsltContext", "ResolveFunction", True, "benchmark" },
    { 194, "System.Private.Xml/System.Xml.Xsl.XsltContext::PreserveWhitespace:System.Boolean(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.Xsl.XsltContext", "PreserveWhitespace", True, "benchmark" },
    { 195, "System.Private.Xml/System.Xml.Xsl.XsltContext::CompareDocument:System.Int32(System.String,System.String)", "generated", "System.Xml.Xsl.XsltContext", "CompareDocument", True, "benchmark" },
    { 196, "System.Private.Xml/System.Xml.Xsl.XsltContext::CompareDocument:System.Int32(System.String,System.String)", "generated", "System.Xml.Xsl.XsltContext", "CompareDocument", True, "benchmark" },
    { 197, "System.Private.Xml/System.Xml.Xsl.XsltContext::CompareDocument:System.Int32(System.String,System.String)", "generated", "System.Xml.Xsl.XsltContext", "CompareDocument", True, "benchmark" },
    { 198, "System.Private.Xml/System.Xml.Xsl.XslTransform::Load:System.Void(System.Xml.XmlReader)", "generated", "System.Xml.Xsl.XslTransform", "Load", True, "benchmark" },
    { 199, "System.Private.Xml/System.Xml.Xsl.XslTransform::Load:System.Void(System.Xml.XmlReader,System.Xml.XmlResolver)", "generated", "System.Xml.Xsl.XslTransform", "Load", True, "benchmark" },
    { 200, "System.Private.Xml/System.Xml.Xsl.XslTransform::Load:System.Void(System.Xml.XPath.IXPathNavigable)", "generated", "System.Xml.Xsl.XslTransform", "Load", True, "benchmark" },
    { 201, "System.Private.Xml/System.Xml.Xsl.XslTransform::Load:System.Void(System.Xml.XPath.IXPathNavigable,System.Xml.XmlResolver)", "generated", "System.Xml.Xsl.XslTransform", "Load", True, "benchmark" },
    { 202, "System.Private.Xml/System.Xml.Xsl.XslTransform::Load:System.Void(System.Xml.XPath.XPathNavigator)", "generated", "System.Xml.Xsl.XslTransform", "Load", True, "benchmark" },
    { 203, "System.Private.Xml/System.Xml.Xsl.XslTransform::Load:System.Void(System.Xml.XPath.XPathNavigator,System.Xml.XmlResolver)", "generated", "System.Xml.Xsl.XslTransform", "Load", True, "benchmark" },
    { 204, "System.Private.Xml/System.Xml.Xsl.XslTransform::Load:System.Void(System.String)", "generated", "System.Xml.Xsl.XslTransform", "Load", True, "benchmark" },
    { 205, "System.Private.Xml/System.Xml.Xsl.XslTransform::Load:System.Void(System.String)", "generated", "System.Xml.Xsl.XslTransform", "Load", True, "benchmark" },
    { 206, "System.Private.Xml/System.Xml.Xsl.XslTransform::Load:System.Void(System.String,System.Xml.XmlResolver)", "generated", "System.Xml.Xsl.XslTransform", "Load", True, "benchmark" },
    { 207, "System.Private.Xml/System.Xml.Xsl.XslTransform::Load:System.Void(System.String,System.Xml.XmlResolver)", "generated", "System.Xml.Xsl.XslTransform", "Load", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 208;

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