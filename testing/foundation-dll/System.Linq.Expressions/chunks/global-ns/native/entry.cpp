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
    { 0, "System.Linq.Expressions/System.Dynamic.BinaryOperationBinder::FallbackBinaryOperation:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.BinaryOperationBinder", "FallbackBinaryOperation", True, "benchmark" },
    { 1, "System.Linq.Expressions/System.Dynamic.BinaryOperationBinder::FallbackBinaryOperation:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.BinaryOperationBinder", "FallbackBinaryOperation", True, "benchmark" },
    { 2, "System.Linq.Expressions/System.Dynamic.BinaryOperationBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.BinaryOperationBinder", "Bind", True, "benchmark" },
    { 3, "System.Linq.Expressions/System.Dynamic.BinaryOperationBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.BinaryOperationBinder", "Bind", True, "benchmark" },
    { 4, "System.Linq.Expressions/System.Dynamic.BinaryOperationBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.BinaryOperationBinder", "Bind", True, "benchmark" },
    { 5, "System.Linq.Expressions/System.Dynamic.BindingRestrictions::Merge:System.Dynamic.BindingRestrictions(System.Dynamic.BindingRestrictions)", "codegen", "System.Dynamic.BindingRestrictions", "Merge", True, "benchmark" },
    { 6, "System.Linq.Expressions/System.Dynamic.BindingRestrictions::GetTypeRestriction:System.Dynamic.BindingRestrictions(System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Dynamic.BindingRestrictions", "GetTypeRestriction", True, "benchmark" },
    { 7, "System.Linq.Expressions/System.Dynamic.BindingRestrictions::GetInstanceRestriction:System.Dynamic.BindingRestrictions(System.Linq.Expressions.Expression,System.Object)", "codegen", "System.Dynamic.BindingRestrictions", "GetInstanceRestriction", True, "benchmark" },
    { 8, "System.Linq.Expressions/System.Dynamic.BindingRestrictions::GetExpressionRestriction:System.Dynamic.BindingRestrictions(System.Linq.Expressions.Expression)", "codegen", "System.Dynamic.BindingRestrictions", "GetExpressionRestriction", True, "benchmark" },
    { 9, "System.Linq.Expressions/System.Dynamic.BindingRestrictions::Combine:System.Dynamic.BindingRestrictions(System.Collections.Generic.IList<System.Dynamic.DynamicMetaObject>)", "codegen", "System.Dynamic.BindingRestrictions", "Combine", True, "benchmark" },
    { 10, "System.Linq.Expressions/System.Dynamic.BindingRestrictions::Combine:System.Dynamic.BindingRestrictions(System.Collections.Generic.IList<System.Dynamic.DynamicMetaObject>)", "codegen", "System.Dynamic.BindingRestrictions", "Combine", True, "benchmark" },
    { 11, "System.Linq.Expressions/System.Dynamic.BindingRestrictions::ToExpression:System.Linq.Expressions.Expression()", "codegen", "System.Dynamic.BindingRestrictions", "ToExpression", True, "benchmark" },
    { 12, "System.Linq.Expressions/System.Dynamic.ConvertBinder::FallbackConvert:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.ConvertBinder", "FallbackConvert", True, "benchmark" },
    { 13, "System.Linq.Expressions/System.Dynamic.ConvertBinder::FallbackConvert:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.ConvertBinder", "FallbackConvert", True, "benchmark" },
    { 14, "System.Linq.Expressions/System.Dynamic.ConvertBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.ConvertBinder", "Bind", True, "benchmark" },
    { 15, "System.Linq.Expressions/System.Dynamic.ConvertBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.ConvertBinder", "Bind", True, "benchmark" },
    { 16, "System.Linq.Expressions/System.Dynamic.ConvertBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.ConvertBinder", "Bind", True, "benchmark" },
    { 17, "System.Linq.Expressions/System.Dynamic.CreateInstanceBinder::FallbackCreateInstance:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.CreateInstanceBinder", "FallbackCreateInstance", True, "benchmark" },
    { 18, "System.Linq.Expressions/System.Dynamic.CreateInstanceBinder::FallbackCreateInstance:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.CreateInstanceBinder", "FallbackCreateInstance", True, "benchmark" },
    { 19, "System.Linq.Expressions/System.Dynamic.CreateInstanceBinder::FallbackCreateInstance:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.CreateInstanceBinder", "FallbackCreateInstance", True, "benchmark" },
    { 20, "System.Linq.Expressions/System.Dynamic.CreateInstanceBinder::FallbackCreateInstance:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.CreateInstanceBinder", "FallbackCreateInstance", True, "benchmark" },
    { 21, "System.Linq.Expressions/System.Dynamic.CreateInstanceBinder::FallbackCreateInstance:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.CreateInstanceBinder", "FallbackCreateInstance", True, "benchmark" },
    { 22, "System.Linq.Expressions/System.Dynamic.CreateInstanceBinder::FallbackCreateInstance:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.CreateInstanceBinder", "FallbackCreateInstance", True, "benchmark" },
    { 23, "System.Linq.Expressions/System.Dynamic.CreateInstanceBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.CreateInstanceBinder", "Bind", True, "benchmark" },
    { 24, "System.Linq.Expressions/System.Dynamic.CreateInstanceBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.CreateInstanceBinder", "Bind", True, "benchmark" },
    { 25, "System.Linq.Expressions/System.Dynamic.CreateInstanceBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.CreateInstanceBinder", "Bind", True, "benchmark" },
    { 26, "System.Linq.Expressions/System.Dynamic.DeleteIndexBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DeleteIndexBinder", "Bind", True, "benchmark" },
    { 27, "System.Linq.Expressions/System.Dynamic.DeleteIndexBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DeleteIndexBinder", "Bind", True, "benchmark" },
    { 28, "System.Linq.Expressions/System.Dynamic.DeleteIndexBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DeleteIndexBinder", "Bind", True, "benchmark" },
    { 29, "System.Linq.Expressions/System.Dynamic.DeleteIndexBinder::FallbackDeleteIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DeleteIndexBinder", "FallbackDeleteIndex", True, "benchmark" },
    { 30, "System.Linq.Expressions/System.Dynamic.DeleteIndexBinder::FallbackDeleteIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DeleteIndexBinder", "FallbackDeleteIndex", True, "benchmark" },
    { 31, "System.Linq.Expressions/System.Dynamic.DeleteIndexBinder::FallbackDeleteIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DeleteIndexBinder", "FallbackDeleteIndex", True, "benchmark" },
    { 32, "System.Linq.Expressions/System.Dynamic.DeleteIndexBinder::FallbackDeleteIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.DeleteIndexBinder", "FallbackDeleteIndex", True, "benchmark" },
    { 33, "System.Linq.Expressions/System.Dynamic.DeleteIndexBinder::FallbackDeleteIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.DeleteIndexBinder", "FallbackDeleteIndex", True, "benchmark" },
    { 34, "System.Linq.Expressions/System.Dynamic.DeleteIndexBinder::FallbackDeleteIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.DeleteIndexBinder", "FallbackDeleteIndex", True, "benchmark" },
    { 35, "System.Linq.Expressions/System.Dynamic.DeleteMemberBinder::FallbackDeleteMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.DeleteMemberBinder", "FallbackDeleteMember", True, "benchmark" },
    { 36, "System.Linq.Expressions/System.Dynamic.DeleteMemberBinder::FallbackDeleteMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.DeleteMemberBinder", "FallbackDeleteMember", True, "benchmark" },
    { 37, "System.Linq.Expressions/System.Dynamic.DeleteMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DeleteMemberBinder", "Bind", True, "benchmark" },
    { 38, "System.Linq.Expressions/System.Dynamic.DeleteMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DeleteMemberBinder", "Bind", True, "benchmark" },
    { 39, "System.Linq.Expressions/System.Dynamic.DeleteMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DeleteMemberBinder", "Bind", True, "benchmark" },
    { 40, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindConvert:System.Dynamic.DynamicMetaObject(System.Dynamic.ConvertBinder)", "codegen", "System.Dynamic.DynamicMetaObject", "BindConvert", True, "benchmark" },
    { 41, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindGetMember:System.Dynamic.DynamicMetaObject(System.Dynamic.GetMemberBinder)", "codegen", "System.Dynamic.DynamicMetaObject", "BindGetMember", True, "benchmark" },
    { 42, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindSetMember:System.Dynamic.DynamicMetaObject(System.Dynamic.SetMemberBinder,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.DynamicMetaObject", "BindSetMember", True, "benchmark" },
    { 43, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindDeleteMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DeleteMemberBinder)", "codegen", "System.Dynamic.DynamicMetaObject", "BindDeleteMember", True, "benchmark" },
    { 44, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindGetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.GetIndexBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindGetIndex", True, "benchmark" },
    { 45, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindGetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.GetIndexBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindGetIndex", True, "benchmark" },
    { 46, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindGetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.GetIndexBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindGetIndex", True, "benchmark" },
    { 47, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindSetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.SetIndexBinder,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.DynamicMetaObject", "BindSetIndex", True, "benchmark" },
    { 48, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindSetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.SetIndexBinder,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.DynamicMetaObject", "BindSetIndex", True, "benchmark" },
    { 49, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindSetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.SetIndexBinder,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.DynamicMetaObject", "BindSetIndex", True, "benchmark" },
    { 50, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindDeleteIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DeleteIndexBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindDeleteIndex", True, "benchmark" },
    { 51, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindDeleteIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DeleteIndexBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindDeleteIndex", True, "benchmark" },
    { 52, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindDeleteIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DeleteIndexBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindDeleteIndex", True, "benchmark" },
    { 53, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindInvokeMember:System.Dynamic.DynamicMetaObject(System.Dynamic.InvokeMemberBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindInvokeMember", True, "benchmark" },
    { 54, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindInvokeMember:System.Dynamic.DynamicMetaObject(System.Dynamic.InvokeMemberBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindInvokeMember", True, "benchmark" },
    { 55, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindInvokeMember:System.Dynamic.DynamicMetaObject(System.Dynamic.InvokeMemberBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindInvokeMember", True, "benchmark" },
    { 56, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.InvokeBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindInvoke", True, "benchmark" },
    { 57, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.InvokeBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindInvoke", True, "benchmark" },
    { 58, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.InvokeBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindInvoke", True, "benchmark" },
    { 59, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindCreateInstance:System.Dynamic.DynamicMetaObject(System.Dynamic.CreateInstanceBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindCreateInstance", True, "benchmark" },
    { 60, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindCreateInstance:System.Dynamic.DynamicMetaObject(System.Dynamic.CreateInstanceBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindCreateInstance", True, "benchmark" },
    { 61, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindCreateInstance:System.Dynamic.DynamicMetaObject(System.Dynamic.CreateInstanceBinder,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObject", "BindCreateInstance", True, "benchmark" },
    { 62, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindUnaryOperation:System.Dynamic.DynamicMetaObject(System.Dynamic.UnaryOperationBinder)", "codegen", "System.Dynamic.DynamicMetaObject", "BindUnaryOperation", True, "benchmark" },
    { 63, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::BindBinaryOperation:System.Dynamic.DynamicMetaObject(System.Dynamic.BinaryOperationBinder,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.DynamicMetaObject", "BindBinaryOperation", True, "benchmark" },
    { 64, "System.Linq.Expressions/System.Dynamic.DynamicMetaObject::GetDynamicMemberNames:System.Collections.Generic.IEnumerable<System.String>()", "codegen", "System.Dynamic.DynamicMetaObject", "GetDynamicMemberNames", True, "benchmark" },
    { 65, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Bind:System.Linq.Expressions.Expression(System.Object[],System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.LabelTarget)", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Bind", True, "benchmark" },
    { 66, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Bind:System.Linq.Expressions.Expression(System.Object[],System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.LabelTarget)", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Bind", True, "benchmark" },
    { 67, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Bind:System.Linq.Expressions.Expression(System.Object[],System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.LabelTarget)", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Bind", True, "benchmark" },
    { 68, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Bind", True, "benchmark" },
    { 69, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Bind", True, "benchmark" },
    { 70, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Bind", True, "benchmark" },
    { 71, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::GetUpdateExpression:System.Linq.Expressions.Expression(System.Type)", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "GetUpdateExpression", True, "benchmark" },
    { 72, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Defer:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Defer", True, "benchmark" },
    { 73, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Defer:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Defer", True, "benchmark" },
    { 74, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Defer:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Defer", True, "benchmark" },
    { 75, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Defer:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Defer", True, "benchmark" },
    { 76, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Defer:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Defer", True, "benchmark" },
    { 77, "System.Linq.Expressions/System.Dynamic.DynamicMetaObjectBinder::Defer:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.DynamicMetaObjectBinder", "Defer", True, "benchmark" },
    { 78, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryGetMember:System.Boolean(System.Dynamic.GetMemberBinder,System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryGetMember", True, "benchmark" },
    { 79, "System.Linq.Expressions/System.Dynamic.DynamicObject::TrySetMember:System.Boolean(System.Dynamic.SetMemberBinder,System.Object)", "codegen", "System.Dynamic.DynamicObject", "TrySetMember", True, "benchmark" },
    { 80, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryDeleteMember:System.Boolean(System.Dynamic.DeleteMemberBinder)", "codegen", "System.Dynamic.DynamicObject", "TryDeleteMember", True, "benchmark" },
    { 81, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryInvokeMember:System.Boolean(System.Dynamic.InvokeMemberBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryInvokeMember", True, "benchmark" },
    { 82, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryInvokeMember:System.Boolean(System.Dynamic.InvokeMemberBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryInvokeMember", True, "benchmark" },
    { 83, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryInvokeMember:System.Boolean(System.Dynamic.InvokeMemberBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryInvokeMember", True, "benchmark" },
    { 84, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryConvert:System.Boolean(System.Dynamic.ConvertBinder,System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryConvert", True, "benchmark" },
    { 85, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryCreateInstance:System.Boolean(System.Dynamic.CreateInstanceBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryCreateInstance", True, "benchmark" },
    { 86, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryCreateInstance:System.Boolean(System.Dynamic.CreateInstanceBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryCreateInstance", True, "benchmark" },
    { 87, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryCreateInstance:System.Boolean(System.Dynamic.CreateInstanceBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryCreateInstance", True, "benchmark" },
    { 88, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryInvoke:System.Boolean(System.Dynamic.InvokeBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryInvoke", True, "benchmark" },
    { 89, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryInvoke:System.Boolean(System.Dynamic.InvokeBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryInvoke", True, "benchmark" },
    { 90, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryInvoke:System.Boolean(System.Dynamic.InvokeBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryInvoke", True, "benchmark" },
    { 91, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryBinaryOperation:System.Boolean(System.Dynamic.BinaryOperationBinder,System.Object,System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryBinaryOperation", True, "benchmark" },
    { 92, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryUnaryOperation:System.Boolean(System.Dynamic.UnaryOperationBinder,System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryUnaryOperation", True, "benchmark" },
    { 93, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryGetIndex:System.Boolean(System.Dynamic.GetIndexBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryGetIndex", True, "benchmark" },
    { 94, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryGetIndex:System.Boolean(System.Dynamic.GetIndexBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryGetIndex", True, "benchmark" },
    { 95, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryGetIndex:System.Boolean(System.Dynamic.GetIndexBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TryGetIndex", True, "benchmark" },
    { 96, "System.Linq.Expressions/System.Dynamic.DynamicObject::TrySetIndex:System.Boolean(System.Dynamic.SetIndexBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TrySetIndex", True, "benchmark" },
    { 97, "System.Linq.Expressions/System.Dynamic.DynamicObject::TrySetIndex:System.Boolean(System.Dynamic.SetIndexBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TrySetIndex", True, "benchmark" },
    { 98, "System.Linq.Expressions/System.Dynamic.DynamicObject::TrySetIndex:System.Boolean(System.Dynamic.SetIndexBinder,System.Object[],System.Object)", "codegen", "System.Dynamic.DynamicObject", "TrySetIndex", True, "benchmark" },
    { 99, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryDeleteIndex:System.Boolean(System.Dynamic.DeleteIndexBinder,System.Object[])", "codegen", "System.Dynamic.DynamicObject", "TryDeleteIndex", True, "benchmark" },
    { 100, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryDeleteIndex:System.Boolean(System.Dynamic.DeleteIndexBinder,System.Object[])", "codegen", "System.Dynamic.DynamicObject", "TryDeleteIndex", True, "benchmark" },
    { 101, "System.Linq.Expressions/System.Dynamic.DynamicObject::TryDeleteIndex:System.Boolean(System.Dynamic.DeleteIndexBinder,System.Object[])", "codegen", "System.Dynamic.DynamicObject", "TryDeleteIndex", True, "benchmark" },
    { 102, "System.Linq.Expressions/System.Dynamic.DynamicObject::GetDynamicMemberNames:System.Collections.Generic.IEnumerable<System.String>()", "codegen", "System.Dynamic.DynamicObject", "GetDynamicMemberNames", True, "benchmark" },
    { 103, "System.Linq.Expressions/System.Dynamic.DynamicObject::GetMetaObject:System.Dynamic.DynamicMetaObject(System.Linq.Expressions.Expression)", "codegen", "System.Dynamic.DynamicObject", "GetMetaObject", True, "benchmark" },
    { 104, "System.Linq.Expressions/System.Dynamic.GetIndexBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.GetIndexBinder", "Bind", True, "benchmark" },
    { 105, "System.Linq.Expressions/System.Dynamic.GetIndexBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.GetIndexBinder", "Bind", True, "benchmark" },
    { 106, "System.Linq.Expressions/System.Dynamic.GetIndexBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.GetIndexBinder", "Bind", True, "benchmark" },
    { 107, "System.Linq.Expressions/System.Dynamic.GetIndexBinder::FallbackGetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.GetIndexBinder", "FallbackGetIndex", True, "benchmark" },
    { 108, "System.Linq.Expressions/System.Dynamic.GetIndexBinder::FallbackGetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.GetIndexBinder", "FallbackGetIndex", True, "benchmark" },
    { 109, "System.Linq.Expressions/System.Dynamic.GetIndexBinder::FallbackGetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.GetIndexBinder", "FallbackGetIndex", True, "benchmark" },
    { 110, "System.Linq.Expressions/System.Dynamic.GetIndexBinder::FallbackGetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.GetIndexBinder", "FallbackGetIndex", True, "benchmark" },
    { 111, "System.Linq.Expressions/System.Dynamic.GetIndexBinder::FallbackGetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.GetIndexBinder", "FallbackGetIndex", True, "benchmark" },
    { 112, "System.Linq.Expressions/System.Dynamic.GetIndexBinder::FallbackGetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.GetIndexBinder", "FallbackGetIndex", True, "benchmark" },
    { 113, "System.Linq.Expressions/System.Dynamic.GetMemberBinder::FallbackGetMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.GetMemberBinder", "FallbackGetMember", True, "benchmark" },
    { 114, "System.Linq.Expressions/System.Dynamic.GetMemberBinder::FallbackGetMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.GetMemberBinder", "FallbackGetMember", True, "benchmark" },
    { 115, "System.Linq.Expressions/System.Dynamic.GetMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.GetMemberBinder", "Bind", True, "benchmark" },
    { 116, "System.Linq.Expressions/System.Dynamic.GetMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.GetMemberBinder", "Bind", True, "benchmark" },
    { 117, "System.Linq.Expressions/System.Dynamic.GetMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.GetMemberBinder", "Bind", True, "benchmark" },
    { 118, "System.Linq.Expressions/System.Dynamic.InvokeBinder::FallbackInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeBinder", "FallbackInvoke", True, "benchmark" },
    { 119, "System.Linq.Expressions/System.Dynamic.InvokeBinder::FallbackInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeBinder", "FallbackInvoke", True, "benchmark" },
    { 120, "System.Linq.Expressions/System.Dynamic.InvokeBinder::FallbackInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeBinder", "FallbackInvoke", True, "benchmark" },
    { 121, "System.Linq.Expressions/System.Dynamic.InvokeBinder::FallbackInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.InvokeBinder", "FallbackInvoke", True, "benchmark" },
    { 122, "System.Linq.Expressions/System.Dynamic.InvokeBinder::FallbackInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.InvokeBinder", "FallbackInvoke", True, "benchmark" },
    { 123, "System.Linq.Expressions/System.Dynamic.InvokeBinder::FallbackInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.InvokeBinder", "FallbackInvoke", True, "benchmark" },
    { 124, "System.Linq.Expressions/System.Dynamic.InvokeBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeBinder", "Bind", True, "benchmark" },
    { 125, "System.Linq.Expressions/System.Dynamic.InvokeBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeBinder", "Bind", True, "benchmark" },
    { 126, "System.Linq.Expressions/System.Dynamic.InvokeBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeBinder", "Bind", True, "benchmark" },
    { 127, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeMemberBinder", "Bind", True, "benchmark" },
    { 128, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeMemberBinder", "Bind", True, "benchmark" },
    { 129, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeMemberBinder", "Bind", True, "benchmark" },
    { 130, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::FallbackInvokeMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeMemberBinder", "FallbackInvokeMember", True, "benchmark" },
    { 131, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::FallbackInvokeMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeMemberBinder", "FallbackInvokeMember", True, "benchmark" },
    { 132, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::FallbackInvokeMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.InvokeMemberBinder", "FallbackInvokeMember", True, "benchmark" },
    { 133, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::FallbackInvokeMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.InvokeMemberBinder", "FallbackInvokeMember", True, "benchmark" },
    { 134, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::FallbackInvokeMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.InvokeMemberBinder", "FallbackInvokeMember", True, "benchmark" },
    { 135, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::FallbackInvokeMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.InvokeMemberBinder", "FallbackInvokeMember", True, "benchmark" },
    { 136, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::FallbackInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.InvokeMemberBinder", "FallbackInvoke", True, "benchmark" },
    { 137, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::FallbackInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.InvokeMemberBinder", "FallbackInvoke", True, "benchmark" },
    { 138, "System.Linq.Expressions/System.Dynamic.InvokeMemberBinder::FallbackInvoke:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.InvokeMemberBinder", "FallbackInvoke", True, "benchmark" },
    { 139, "System.Linq.Expressions/System.Dynamic.SetIndexBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.SetIndexBinder", "Bind", True, "benchmark" },
    { 140, "System.Linq.Expressions/System.Dynamic.SetIndexBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.SetIndexBinder", "Bind", True, "benchmark" },
    { 141, "System.Linq.Expressions/System.Dynamic.SetIndexBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.SetIndexBinder", "Bind", True, "benchmark" },
    { 142, "System.Linq.Expressions/System.Dynamic.SetIndexBinder::FallbackSetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.SetIndexBinder", "FallbackSetIndex", True, "benchmark" },
    { 143, "System.Linq.Expressions/System.Dynamic.SetIndexBinder::FallbackSetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.SetIndexBinder", "FallbackSetIndex", True, "benchmark" },
    { 144, "System.Linq.Expressions/System.Dynamic.SetIndexBinder::FallbackSetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.SetIndexBinder", "FallbackSetIndex", True, "benchmark" },
    { 145, "System.Linq.Expressions/System.Dynamic.SetIndexBinder::FallbackSetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.SetIndexBinder", "FallbackSetIndex", True, "benchmark" },
    { 146, "System.Linq.Expressions/System.Dynamic.SetIndexBinder::FallbackSetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.SetIndexBinder", "FallbackSetIndex", True, "benchmark" },
    { 147, "System.Linq.Expressions/System.Dynamic.SetIndexBinder::FallbackSetIndex:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[],System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.SetIndexBinder", "FallbackSetIndex", True, "benchmark" },
    { 148, "System.Linq.Expressions/System.Dynamic.SetMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.SetMemberBinder", "Bind", True, "benchmark" },
    { 149, "System.Linq.Expressions/System.Dynamic.SetMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.SetMemberBinder", "Bind", True, "benchmark" },
    { 150, "System.Linq.Expressions/System.Dynamic.SetMemberBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.SetMemberBinder", "Bind", True, "benchmark" },
    { 151, "System.Linq.Expressions/System.Dynamic.SetMemberBinder::FallbackSetMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.SetMemberBinder", "FallbackSetMember", True, "benchmark" },
    { 152, "System.Linq.Expressions/System.Dynamic.SetMemberBinder::FallbackSetMember:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.SetMemberBinder", "FallbackSetMember", True, "benchmark" },
    { 153, "System.Linq.Expressions/System.Dynamic.UnaryOperationBinder::FallbackUnaryOperation:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.UnaryOperationBinder", "FallbackUnaryOperation", True, "benchmark" },
    { 154, "System.Linq.Expressions/System.Dynamic.UnaryOperationBinder::FallbackUnaryOperation:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject)", "codegen", "System.Dynamic.UnaryOperationBinder", "FallbackUnaryOperation", True, "benchmark" },
    { 155, "System.Linq.Expressions/System.Dynamic.UnaryOperationBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.UnaryOperationBinder", "Bind", True, "benchmark" },
    { 156, "System.Linq.Expressions/System.Dynamic.UnaryOperationBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.UnaryOperationBinder", "Bind", True, "benchmark" },
    { 157, "System.Linq.Expressions/System.Dynamic.UnaryOperationBinder::Bind:System.Dynamic.DynamicMetaObject(System.Dynamic.DynamicMetaObject,System.Dynamic.DynamicMetaObject[])", "codegen", "System.Dynamic.UnaryOperationBinder", "Bind", True, "benchmark" },
    { 158, "System.Linq.Expressions/System.Linq.Expressions.BinaryExpression::Update:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.LambdaExpression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.BinaryExpression", "Update", True, "benchmark" },
    { 159, "System.Linq.Expressions/System.Linq.Expressions.BinaryExpression::Reduce:System.Linq.Expressions.Expression()", "codegen", "System.Linq.Expressions.BinaryExpression", "Reduce", True, "benchmark" },
    { 160, "System.Linq.Expressions/System.Linq.Expressions.BlockExpression::Update:System.Linq.Expressions.BlockExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.BlockExpression", "Update", True, "benchmark" },
    { 161, "System.Linq.Expressions/System.Linq.Expressions.BlockExpression::Update:System.Linq.Expressions.BlockExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.BlockExpression", "Update", True, "benchmark" },
    { 162, "System.Linq.Expressions/System.Linq.Expressions.CatchBlock::Update:System.Linq.Expressions.CatchBlock(System.Linq.Expressions.ParameterExpression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.CatchBlock", "Update", True, "benchmark" },
    { 163, "System.Linq.Expressions/System.Linq.Expressions.ConditionalExpression::Update:System.Linq.Expressions.ConditionalExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.ConditionalExpression", "Update", True, "benchmark" },
    { 164, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Reduce:System.Linq.Expressions.Expression()", "codegen", "System.Linq.Expressions.DynamicExpression", "Reduce", True, "benchmark" },
    { 165, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Update:System.Linq.Expressions.DynamicExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.DynamicExpression", "Update", True, "benchmark" },
    { 166, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Update:System.Linq.Expressions.DynamicExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.DynamicExpression", "Update", True, "benchmark" },
    { 167, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.DynamicExpression", "Dynamic", True, "benchmark" },
    { 168, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.DynamicExpression", "Dynamic", True, "benchmark" },
    { 169, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.DynamicExpression", "Dynamic", True, "benchmark" },
    { 170, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.DynamicExpression", "Dynamic", True, "benchmark" },
    { 171, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.DynamicExpression", "Dynamic", True, "benchmark" },
    { 172, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.DynamicExpression", "Dynamic", True, "benchmark" },
    { 173, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.DynamicExpression", "Dynamic", True, "benchmark" },
    { 174, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.DynamicExpression", "Dynamic", True, "benchmark" },
    { 175, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.DynamicExpression", "Dynamic", True, "benchmark" },
    { 176, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.DynamicExpression", "MakeDynamic", True, "benchmark" },
    { 177, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.DynamicExpression", "MakeDynamic", True, "benchmark" },
    { 178, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.DynamicExpression", "MakeDynamic", True, "benchmark" },
    { 179, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.DynamicExpression", "MakeDynamic", True, "benchmark" },
    { 180, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.DynamicExpression", "MakeDynamic", True, "benchmark" },
    { 181, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.DynamicExpression", "MakeDynamic", True, "benchmark" },
    { 182, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.DynamicExpression", "MakeDynamic", True, "benchmark" },
    { 183, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.DynamicExpression", "MakeDynamic", True, "benchmark" },
    { 184, "System.Linq.Expressions/System.Linq.Expressions.DynamicExpression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.DynamicExpression", "MakeDynamic", True, "benchmark" },
    { 185, "System.Linq.Expressions/System.Linq.Expressions.ElementInit::Update:System.Linq.Expressions.ElementInit(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.ElementInit", "Update", True, "benchmark" },
    { 186, "System.Linq.Expressions/System.Linq.Expressions.ElementInit::Update:System.Linq.Expressions.ElementInit(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.ElementInit", "Update", True, "benchmark" },
    { 187, "System.Linq.Expressions/System.Linq.Expressions.Expression::Assign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Assign", True, "benchmark" },
    { 188, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeBinary:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.ExpressionType,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "MakeBinary", True, "benchmark" },
    { 189, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeBinary:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.ExpressionType,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "MakeBinary", True, "benchmark" },
    { 190, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeBinary:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.ExpressionType,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "MakeBinary", True, "benchmark" },
    { 191, "System.Linq.Expressions/System.Linq.Expressions.Expression::Equal:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Equal", True, "benchmark" },
    { 192, "System.Linq.Expressions/System.Linq.Expressions.Expression::Equal:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Equal", True, "benchmark" },
    { 193, "System.Linq.Expressions/System.Linq.Expressions.Expression::Equal:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Equal", True, "benchmark" },
    { 194, "System.Linq.Expressions/System.Linq.Expressions.Expression::ReferenceEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "ReferenceEqual", True, "benchmark" },
    { 195, "System.Linq.Expressions/System.Linq.Expressions.Expression::NotEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "NotEqual", True, "benchmark" },
    { 196, "System.Linq.Expressions/System.Linq.Expressions.Expression::NotEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "NotEqual", True, "benchmark" },
    { 197, "System.Linq.Expressions/System.Linq.Expressions.Expression::NotEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "NotEqual", True, "benchmark" },
    { 198, "System.Linq.Expressions/System.Linq.Expressions.Expression::ReferenceNotEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "ReferenceNotEqual", True, "benchmark" },
    { 199, "System.Linq.Expressions/System.Linq.Expressions.Expression::GreaterThan:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "GreaterThan", True, "benchmark" },
    { 200, "System.Linq.Expressions/System.Linq.Expressions.Expression::GreaterThan:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "GreaterThan", True, "benchmark" },
    { 201, "System.Linq.Expressions/System.Linq.Expressions.Expression::GreaterThan:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "GreaterThan", True, "benchmark" },
    { 202, "System.Linq.Expressions/System.Linq.Expressions.Expression::LessThan:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "LessThan", True, "benchmark" },
    { 203, "System.Linq.Expressions/System.Linq.Expressions.Expression::LessThan:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "LessThan", True, "benchmark" },
    { 204, "System.Linq.Expressions/System.Linq.Expressions.Expression::LessThan:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "LessThan", True, "benchmark" },
    { 205, "System.Linq.Expressions/System.Linq.Expressions.Expression::GreaterThanOrEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "GreaterThanOrEqual", True, "benchmark" },
    { 206, "System.Linq.Expressions/System.Linq.Expressions.Expression::GreaterThanOrEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "GreaterThanOrEqual", True, "benchmark" },
    { 207, "System.Linq.Expressions/System.Linq.Expressions.Expression::GreaterThanOrEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "GreaterThanOrEqual", True, "benchmark" },
    { 208, "System.Linq.Expressions/System.Linq.Expressions.Expression::LessThanOrEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "LessThanOrEqual", True, "benchmark" },
    { 209, "System.Linq.Expressions/System.Linq.Expressions.Expression::LessThanOrEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "LessThanOrEqual", True, "benchmark" },
    { 210, "System.Linq.Expressions/System.Linq.Expressions.Expression::LessThanOrEqual:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Boolean,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "LessThanOrEqual", True, "benchmark" },
    { 211, "System.Linq.Expressions/System.Linq.Expressions.Expression::AndAlso:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "AndAlso", True, "benchmark" },
    { 212, "System.Linq.Expressions/System.Linq.Expressions.Expression::AndAlso:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "AndAlso", True, "benchmark" },
    { 213, "System.Linq.Expressions/System.Linq.Expressions.Expression::OrElse:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "OrElse", True, "benchmark" },
    { 214, "System.Linq.Expressions/System.Linq.Expressions.Expression::OrElse:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "OrElse", True, "benchmark" },
    { 215, "System.Linq.Expressions/System.Linq.Expressions.Expression::Coalesce:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Coalesce", True, "benchmark" },
    { 216, "System.Linq.Expressions/System.Linq.Expressions.Expression::Coalesce:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "Coalesce", True, "benchmark" },
    { 217, "System.Linq.Expressions/System.Linq.Expressions.Expression::Add:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Add", True, "benchmark" },
    { 218, "System.Linq.Expressions/System.Linq.Expressions.Expression::Add:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Add", True, "benchmark" },
    { 219, "System.Linq.Expressions/System.Linq.Expressions.Expression::AddAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "AddAssign", True, "benchmark" },
    { 220, "System.Linq.Expressions/System.Linq.Expressions.Expression::AddAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "AddAssign", True, "benchmark" },
    { 221, "System.Linq.Expressions/System.Linq.Expressions.Expression::AddAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "AddAssign", True, "benchmark" },
    { 222, "System.Linq.Expressions/System.Linq.Expressions.Expression::AddAssignChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "AddAssignChecked", True, "benchmark" },
    { 223, "System.Linq.Expressions/System.Linq.Expressions.Expression::AddAssignChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "AddAssignChecked", True, "benchmark" },
    { 224, "System.Linq.Expressions/System.Linq.Expressions.Expression::AddAssignChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "AddAssignChecked", True, "benchmark" },
    { 225, "System.Linq.Expressions/System.Linq.Expressions.Expression::AddChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "AddChecked", True, "benchmark" },
    { 226, "System.Linq.Expressions/System.Linq.Expressions.Expression::AddChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "AddChecked", True, "benchmark" },
    { 227, "System.Linq.Expressions/System.Linq.Expressions.Expression::Subtract:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Subtract", True, "benchmark" },
    { 228, "System.Linq.Expressions/System.Linq.Expressions.Expression::Subtract:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Subtract", True, "benchmark" },
    { 229, "System.Linq.Expressions/System.Linq.Expressions.Expression::SubtractAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "SubtractAssign", True, "benchmark" },
    { 230, "System.Linq.Expressions/System.Linq.Expressions.Expression::SubtractAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "SubtractAssign", True, "benchmark" },
    { 231, "System.Linq.Expressions/System.Linq.Expressions.Expression::SubtractAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "SubtractAssign", True, "benchmark" },
    { 232, "System.Linq.Expressions/System.Linq.Expressions.Expression::SubtractAssignChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "SubtractAssignChecked", True, "benchmark" },
    { 233, "System.Linq.Expressions/System.Linq.Expressions.Expression::SubtractAssignChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "SubtractAssignChecked", True, "benchmark" },
    { 234, "System.Linq.Expressions/System.Linq.Expressions.Expression::SubtractAssignChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "SubtractAssignChecked", True, "benchmark" },
    { 235, "System.Linq.Expressions/System.Linq.Expressions.Expression::SubtractChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "SubtractChecked", True, "benchmark" },
    { 236, "System.Linq.Expressions/System.Linq.Expressions.Expression::SubtractChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "SubtractChecked", True, "benchmark" },
    { 237, "System.Linq.Expressions/System.Linq.Expressions.Expression::Divide:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Divide", True, "benchmark" },
    { 238, "System.Linq.Expressions/System.Linq.Expressions.Expression::Divide:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Divide", True, "benchmark" },
    { 239, "System.Linq.Expressions/System.Linq.Expressions.Expression::DivideAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "DivideAssign", True, "benchmark" },
    { 240, "System.Linq.Expressions/System.Linq.Expressions.Expression::DivideAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "DivideAssign", True, "benchmark" },
    { 241, "System.Linq.Expressions/System.Linq.Expressions.Expression::DivideAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "DivideAssign", True, "benchmark" },
    { 242, "System.Linq.Expressions/System.Linq.Expressions.Expression::Modulo:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Modulo", True, "benchmark" },
    { 243, "System.Linq.Expressions/System.Linq.Expressions.Expression::Modulo:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Modulo", True, "benchmark" },
    { 244, "System.Linq.Expressions/System.Linq.Expressions.Expression::ModuloAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "ModuloAssign", True, "benchmark" },
    { 245, "System.Linq.Expressions/System.Linq.Expressions.Expression::ModuloAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "ModuloAssign", True, "benchmark" },
    { 246, "System.Linq.Expressions/System.Linq.Expressions.Expression::ModuloAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "ModuloAssign", True, "benchmark" },
    { 247, "System.Linq.Expressions/System.Linq.Expressions.Expression::Multiply:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Multiply", True, "benchmark" },
    { 248, "System.Linq.Expressions/System.Linq.Expressions.Expression::Multiply:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Multiply", True, "benchmark" },
    { 249, "System.Linq.Expressions/System.Linq.Expressions.Expression::MultiplyAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "MultiplyAssign", True, "benchmark" },
    { 250, "System.Linq.Expressions/System.Linq.Expressions.Expression::MultiplyAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "MultiplyAssign", True, "benchmark" },
    { 251, "System.Linq.Expressions/System.Linq.Expressions.Expression::MultiplyAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "MultiplyAssign", True, "benchmark" },
    { 252, "System.Linq.Expressions/System.Linq.Expressions.Expression::MultiplyAssignChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "MultiplyAssignChecked", True, "benchmark" },
    { 253, "System.Linq.Expressions/System.Linq.Expressions.Expression::MultiplyAssignChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "MultiplyAssignChecked", True, "benchmark" },
    { 254, "System.Linq.Expressions/System.Linq.Expressions.Expression::MultiplyAssignChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "MultiplyAssignChecked", True, "benchmark" },
    { 255, "System.Linq.Expressions/System.Linq.Expressions.Expression::MultiplyChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "MultiplyChecked", True, "benchmark" },
    { 256, "System.Linq.Expressions/System.Linq.Expressions.Expression::MultiplyChecked:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "MultiplyChecked", True, "benchmark" },
    { 257, "System.Linq.Expressions/System.Linq.Expressions.Expression::LeftShift:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "LeftShift", True, "benchmark" },
    { 258, "System.Linq.Expressions/System.Linq.Expressions.Expression::LeftShift:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "LeftShift", True, "benchmark" },
    { 259, "System.Linq.Expressions/System.Linq.Expressions.Expression::LeftShiftAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "LeftShiftAssign", True, "benchmark" },
    { 260, "System.Linq.Expressions/System.Linq.Expressions.Expression::LeftShiftAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "LeftShiftAssign", True, "benchmark" },
    { 261, "System.Linq.Expressions/System.Linq.Expressions.Expression::LeftShiftAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "LeftShiftAssign", True, "benchmark" },
    { 262, "System.Linq.Expressions/System.Linq.Expressions.Expression::RightShift:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "RightShift", True, "benchmark" },
    { 263, "System.Linq.Expressions/System.Linq.Expressions.Expression::RightShift:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "RightShift", True, "benchmark" },
    { 264, "System.Linq.Expressions/System.Linq.Expressions.Expression::RightShiftAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "RightShiftAssign", True, "benchmark" },
    { 265, "System.Linq.Expressions/System.Linq.Expressions.Expression::RightShiftAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "RightShiftAssign", True, "benchmark" },
    { 266, "System.Linq.Expressions/System.Linq.Expressions.Expression::RightShiftAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "RightShiftAssign", True, "benchmark" },
    { 267, "System.Linq.Expressions/System.Linq.Expressions.Expression::And:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "And", True, "benchmark" },
    { 268, "System.Linq.Expressions/System.Linq.Expressions.Expression::And:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "And", True, "benchmark" },
    { 269, "System.Linq.Expressions/System.Linq.Expressions.Expression::AndAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "AndAssign", True, "benchmark" },
    { 270, "System.Linq.Expressions/System.Linq.Expressions.Expression::AndAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "AndAssign", True, "benchmark" },
    { 271, "System.Linq.Expressions/System.Linq.Expressions.Expression::AndAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "AndAssign", True, "benchmark" },
    { 272, "System.Linq.Expressions/System.Linq.Expressions.Expression::Or:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Or", True, "benchmark" },
    { 273, "System.Linq.Expressions/System.Linq.Expressions.Expression::Or:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Or", True, "benchmark" },
    { 274, "System.Linq.Expressions/System.Linq.Expressions.Expression::OrAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "OrAssign", True, "benchmark" },
    { 275, "System.Linq.Expressions/System.Linq.Expressions.Expression::OrAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "OrAssign", True, "benchmark" },
    { 276, "System.Linq.Expressions/System.Linq.Expressions.Expression::OrAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "OrAssign", True, "benchmark" },
    { 277, "System.Linq.Expressions/System.Linq.Expressions.Expression::ExclusiveOr:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "ExclusiveOr", True, "benchmark" },
    { 278, "System.Linq.Expressions/System.Linq.Expressions.Expression::ExclusiveOr:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "ExclusiveOr", True, "benchmark" },
    { 279, "System.Linq.Expressions/System.Linq.Expressions.Expression::ExclusiveOrAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "ExclusiveOrAssign", True, "benchmark" },
    { 280, "System.Linq.Expressions/System.Linq.Expressions.Expression::ExclusiveOrAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "ExclusiveOrAssign", True, "benchmark" },
    { 281, "System.Linq.Expressions/System.Linq.Expressions.Expression::ExclusiveOrAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "ExclusiveOrAssign", True, "benchmark" },
    { 282, "System.Linq.Expressions/System.Linq.Expressions.Expression::Power:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Power", True, "benchmark" },
    { 283, "System.Linq.Expressions/System.Linq.Expressions.Expression::Power:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Power", True, "benchmark" },
    { 284, "System.Linq.Expressions/System.Linq.Expressions.Expression::PowerAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "PowerAssign", True, "benchmark" },
    { 285, "System.Linq.Expressions/System.Linq.Expressions.Expression::PowerAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "PowerAssign", True, "benchmark" },
    { 286, "System.Linq.Expressions/System.Linq.Expressions.Expression::PowerAssign:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.LambdaExpression)", "codegen", "System.Linq.Expressions.Expression", "PowerAssign", True, "benchmark" },
    { 287, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayIndex:System.Linq.Expressions.BinaryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "ArrayIndex", True, "benchmark" },
    { 288, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 289, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 290, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 291, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 292, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 293, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 294, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 295, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 296, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 297, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 298, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 299, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 300, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 301, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 302, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 303, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 304, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 305, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 306, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 307, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 308, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 309, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 310, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 311, "System.Linq.Expressions/System.Linq.Expressions.Expression::Block:System.Linq.Expressions.BlockExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Block", True, "benchmark" },
    { 312, "System.Linq.Expressions/System.Linq.Expressions.Expression::Catch:System.Linq.Expressions.CatchBlock(System.Type,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Catch", True, "benchmark" },
    { 313, "System.Linq.Expressions/System.Linq.Expressions.Expression::Catch:System.Linq.Expressions.CatchBlock(System.Linq.Expressions.ParameterExpression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Catch", True, "benchmark" },
    { 314, "System.Linq.Expressions/System.Linq.Expressions.Expression::Catch:System.Linq.Expressions.CatchBlock(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Catch", True, "benchmark" },
    { 315, "System.Linq.Expressions/System.Linq.Expressions.Expression::Catch:System.Linq.Expressions.CatchBlock(System.Linq.Expressions.ParameterExpression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Catch", True, "benchmark" },
    { 316, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeCatchBlock:System.Linq.Expressions.CatchBlock(System.Type,System.Linq.Expressions.ParameterExpression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "MakeCatchBlock", True, "benchmark" },
    { 317, "System.Linq.Expressions/System.Linq.Expressions.Expression::Condition:System.Linq.Expressions.ConditionalExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Condition", True, "benchmark" },
    { 318, "System.Linq.Expressions/System.Linq.Expressions.Expression::Condition:System.Linq.Expressions.ConditionalExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Condition", True, "benchmark" },
    { 319, "System.Linq.Expressions/System.Linq.Expressions.Expression::IfThen:System.Linq.Expressions.ConditionalExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "IfThen", True, "benchmark" },
    { 320, "System.Linq.Expressions/System.Linq.Expressions.Expression::IfThenElse:System.Linq.Expressions.ConditionalExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "IfThenElse", True, "benchmark" },
    { 321, "System.Linq.Expressions/System.Linq.Expressions.Expression::Constant:System.Linq.Expressions.ConstantExpression(System.Object)", "codegen", "System.Linq.Expressions.Expression", "Constant", True, "benchmark" },
    { 322, "System.Linq.Expressions/System.Linq.Expressions.Expression::Constant:System.Linq.Expressions.ConstantExpression(System.Object,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Constant", True, "benchmark" },
    { 323, "System.Linq.Expressions/System.Linq.Expressions.Expression::DebugInfo:System.Linq.Expressions.DebugInfoExpression(System.Linq.Expressions.SymbolDocumentInfo,System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Linq.Expressions.Expression", "DebugInfo", True, "benchmark" },
    { 324, "System.Linq.Expressions/System.Linq.Expressions.Expression::DebugInfo:System.Linq.Expressions.DebugInfoExpression(System.Linq.Expressions.SymbolDocumentInfo,System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Linq.Expressions.Expression", "DebugInfo", True, "benchmark" },
    { 325, "System.Linq.Expressions/System.Linq.Expressions.Expression::DebugInfo:System.Linq.Expressions.DebugInfoExpression(System.Linq.Expressions.SymbolDocumentInfo,System.Int32,System.Int32,System.Int32,System.Int32)", "codegen", "System.Linq.Expressions.Expression", "DebugInfo", True, "benchmark" },
    { 326, "System.Linq.Expressions/System.Linq.Expressions.Expression::ClearDebugInfo:System.Linq.Expressions.DebugInfoExpression(System.Linq.Expressions.SymbolDocumentInfo)", "codegen", "System.Linq.Expressions.Expression", "ClearDebugInfo", True, "benchmark" },
    { 327, "System.Linq.Expressions/System.Linq.Expressions.Expression::Empty:System.Linq.Expressions.DefaultExpression()", "codegen", "System.Linq.Expressions.Expression", "Empty", True, "benchmark" },
    { 328, "System.Linq.Expressions/System.Linq.Expressions.Expression::Default:System.Linq.Expressions.DefaultExpression(System.Type)", "codegen", "System.Linq.Expressions.Expression", "Default", True, "benchmark" },
    { 329, "System.Linq.Expressions/System.Linq.Expressions.Expression::ElementInit:System.Linq.Expressions.ElementInit(System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ElementInit", True, "benchmark" },
    { 330, "System.Linq.Expressions/System.Linq.Expressions.Expression::ElementInit:System.Linq.Expressions.ElementInit(System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ElementInit", True, "benchmark" },
    { 331, "System.Linq.Expressions/System.Linq.Expressions.Expression::ElementInit:System.Linq.Expressions.ElementInit(System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ElementInit", True, "benchmark" },
    { 332, "System.Linq.Expressions/System.Linq.Expressions.Expression::ElementInit:System.Linq.Expressions.ElementInit(System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "ElementInit", True, "benchmark" },
    { 333, "System.Linq.Expressions/System.Linq.Expressions.Expression::ElementInit:System.Linq.Expressions.ElementInit(System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "ElementInit", True, "benchmark" },
    { 334, "System.Linq.Expressions/System.Linq.Expressions.Expression::Reduce:System.Linq.Expressions.Expression()", "codegen", "System.Linq.Expressions.Expression", "Reduce", True, "benchmark" },
    { 335, "System.Linq.Expressions/System.Linq.Expressions.Expression::ReduceAndCheck:System.Linq.Expressions.Expression()", "codegen", "System.Linq.Expressions.Expression", "ReduceAndCheck", True, "benchmark" },
    { 336, "System.Linq.Expressions/System.Linq.Expressions.Expression::ReduceExtensions:System.Linq.Expressions.Expression()", "codegen", "System.Linq.Expressions.Expression", "ReduceExtensions", True, "benchmark" },
    { 337, "System.Linq.Expressions/System.Linq.Expressions.Expression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Dynamic", True, "benchmark" },
    { 338, "System.Linq.Expressions/System.Linq.Expressions.Expression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Dynamic", True, "benchmark" },
    { 339, "System.Linq.Expressions/System.Linq.Expressions.Expression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Dynamic", True, "benchmark" },
    { 340, "System.Linq.Expressions/System.Linq.Expressions.Expression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Dynamic", True, "benchmark" },
    { 341, "System.Linq.Expressions/System.Linq.Expressions.Expression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Dynamic", True, "benchmark" },
    { 342, "System.Linq.Expressions/System.Linq.Expressions.Expression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Dynamic", True, "benchmark" },
    { 343, "System.Linq.Expressions/System.Linq.Expressions.Expression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Dynamic", True, "benchmark" },
    { 344, "System.Linq.Expressions/System.Linq.Expressions.Expression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Dynamic", True, "benchmark" },
    { 345, "System.Linq.Expressions/System.Linq.Expressions.Expression::Dynamic:System.Linq.Expressions.DynamicExpression(System.Runtime.CompilerServices.CallSiteBinder,System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Dynamic", True, "benchmark" },
    { 346, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "MakeDynamic", True, "benchmark" },
    { 347, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "MakeDynamic", True, "benchmark" },
    { 348, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "MakeDynamic", True, "benchmark" },
    { 349, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "MakeDynamic", True, "benchmark" },
    { 350, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "MakeDynamic", True, "benchmark" },
    { 351, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "MakeDynamic", True, "benchmark" },
    { 352, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "MakeDynamic", True, "benchmark" },
    { 353, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "MakeDynamic", True, "benchmark" },
    { 354, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeDynamic:System.Linq.Expressions.DynamicExpression(System.Type,System.Runtime.CompilerServices.CallSiteBinder,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "MakeDynamic", True, "benchmark" },
    { 355, "System.Linq.Expressions/System.Linq.Expressions.Expression::Break:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget)", "codegen", "System.Linq.Expressions.Expression", "Break", True, "benchmark" },
    { 356, "System.Linq.Expressions/System.Linq.Expressions.Expression::Break:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Break", True, "benchmark" },
    { 357, "System.Linq.Expressions/System.Linq.Expressions.Expression::Break:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Break", True, "benchmark" },
    { 358, "System.Linq.Expressions/System.Linq.Expressions.Expression::Break:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Break", True, "benchmark" },
    { 359, "System.Linq.Expressions/System.Linq.Expressions.Expression::Continue:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget)", "codegen", "System.Linq.Expressions.Expression", "Continue", True, "benchmark" },
    { 360, "System.Linq.Expressions/System.Linq.Expressions.Expression::Continue:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Continue", True, "benchmark" },
    { 361, "System.Linq.Expressions/System.Linq.Expressions.Expression::Return:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget)", "codegen", "System.Linq.Expressions.Expression", "Return", True, "benchmark" },
    { 362, "System.Linq.Expressions/System.Linq.Expressions.Expression::Return:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Return", True, "benchmark" },
    { 363, "System.Linq.Expressions/System.Linq.Expressions.Expression::Return:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Return", True, "benchmark" },
    { 364, "System.Linq.Expressions/System.Linq.Expressions.Expression::Return:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Return", True, "benchmark" },
    { 365, "System.Linq.Expressions/System.Linq.Expressions.Expression::Goto:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget)", "codegen", "System.Linq.Expressions.Expression", "Goto", True, "benchmark" },
    { 366, "System.Linq.Expressions/System.Linq.Expressions.Expression::Goto:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Goto", True, "benchmark" },
    { 367, "System.Linq.Expressions/System.Linq.Expressions.Expression::Goto:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Goto", True, "benchmark" },
    { 368, "System.Linq.Expressions/System.Linq.Expressions.Expression::Goto:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Goto", True, "benchmark" },
    { 369, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeGoto:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.GotoExpressionKind,System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "MakeGoto", True, "benchmark" },
    { 370, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeIndex:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Reflection.PropertyInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "MakeIndex", True, "benchmark" },
    { 371, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeIndex:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Reflection.PropertyInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "MakeIndex", True, "benchmark" },
    { 372, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayAccess:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ArrayAccess", True, "benchmark" },
    { 373, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayAccess:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ArrayAccess", True, "benchmark" },
    { 374, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayAccess:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ArrayAccess", True, "benchmark" },
    { 375, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayAccess:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "ArrayAccess", True, "benchmark" },
    { 376, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayAccess:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "ArrayAccess", True, "benchmark" },
    { 377, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.String,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 378, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.String,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 379, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.String,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 380, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.String,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 381, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Reflection.PropertyInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 382, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Reflection.PropertyInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 383, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Reflection.PropertyInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 384, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Reflection.PropertyInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 385, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Reflection.PropertyInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 386, "System.Linq.Expressions/System.Linq.Expressions.Expression::Label:System.Linq.Expressions.LabelExpression(System.Linq.Expressions.LabelTarget)", "codegen", "System.Linq.Expressions.Expression", "Label", True, "benchmark" },
    { 387, "System.Linq.Expressions/System.Linq.Expressions.Expression::Label:System.Linq.Expressions.LabelExpression(System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Label", True, "benchmark" },
    { 388, "System.Linq.Expressions/System.Linq.Expressions.Expression::Label:System.Linq.Expressions.LabelTarget()", "codegen", "System.Linq.Expressions.Expression", "Label", True, "benchmark" },
    { 389, "System.Linq.Expressions/System.Linq.Expressions.Expression::Label:System.Linq.Expressions.LabelTarget(System.String)", "codegen", "System.Linq.Expressions.Expression", "Label", True, "benchmark" },
    { 390, "System.Linq.Expressions/System.Linq.Expressions.Expression::Label:System.Linq.Expressions.LabelTarget(System.String)", "codegen", "System.Linq.Expressions.Expression", "Label", True, "benchmark" },
    { 391, "System.Linq.Expressions/System.Linq.Expressions.Expression::Label:System.Linq.Expressions.LabelTarget(System.Type)", "codegen", "System.Linq.Expressions.Expression", "Label", True, "benchmark" },
    { 392, "System.Linq.Expressions/System.Linq.Expressions.Expression::Label:System.Linq.Expressions.LabelTarget(System.Type,System.String)", "codegen", "System.Linq.Expressions.Expression", "Label", True, "benchmark" },
    { 393, "System.Linq.Expressions/System.Linq.Expressions.Expression::Label:System.Linq.Expressions.LabelTarget(System.Type,System.String)", "codegen", "System.Linq.Expressions.Expression", "Label", True, "benchmark" },
    { 394, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 395, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 396, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 397, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 398, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 399, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 400, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 401, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 402, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 403, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 404, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 405, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 406, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.String,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 407, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.String,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 408, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.String,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 409, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 410, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 411, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 412, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 413, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 414, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 415, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 416, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 417, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 418, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 419, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 420, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 421, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 422, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 423, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 424, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 425, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 426, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 427, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 428, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 429, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 430, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 431, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Boolean,System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 432, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 433, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 434, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 435, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 436, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 437, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.String,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 438, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.String,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 439, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.String,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 440, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 441, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 442, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 443, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 444, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.String,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 445, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.String,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 446, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.String,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 447, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 448, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 449, "System.Linq.Expressions/System.Linq.Expressions.Expression::Lambda:System.Linq.Expressions.LambdaExpression(System.Type,System.Linq.Expressions.Expression,System.String,System.Boolean,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "Lambda", True, "benchmark" },
    { 450, "System.Linq.Expressions/System.Linq.Expressions.Expression::GetFuncType:System.Type(System.Type[])", "codegen", "System.Linq.Expressions.Expression", "GetFuncType", True, "benchmark" },
    { 451, "System.Linq.Expressions/System.Linq.Expressions.Expression::GetFuncType:System.Type(System.Type[])", "codegen", "System.Linq.Expressions.Expression", "GetFuncType", True, "benchmark" },
    { 452, "System.Linq.Expressions/System.Linq.Expressions.Expression::GetFuncType:System.Type(System.Type[])", "codegen", "System.Linq.Expressions.Expression", "GetFuncType", True, "benchmark" },
    { 453, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryGetFuncType:System.Boolean(System.Type[],System.Type)", "codegen", "System.Linq.Expressions.Expression", "TryGetFuncType", True, "benchmark" },
    { 454, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryGetFuncType:System.Boolean(System.Type[],System.Type)", "codegen", "System.Linq.Expressions.Expression", "TryGetFuncType", True, "benchmark" },
    { 455, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryGetFuncType:System.Boolean(System.Type[],System.Type)", "codegen", "System.Linq.Expressions.Expression", "TryGetFuncType", True, "benchmark" },
    { 456, "System.Linq.Expressions/System.Linq.Expressions.Expression::GetActionType:System.Type(System.Type[])", "codegen", "System.Linq.Expressions.Expression", "GetActionType", True, "benchmark" },
    { 457, "System.Linq.Expressions/System.Linq.Expressions.Expression::GetActionType:System.Type(System.Type[])", "codegen", "System.Linq.Expressions.Expression", "GetActionType", True, "benchmark" },
    { 458, "System.Linq.Expressions/System.Linq.Expressions.Expression::GetActionType:System.Type(System.Type[])", "codegen", "System.Linq.Expressions.Expression", "GetActionType", True, "benchmark" },
    { 459, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryGetActionType:System.Boolean(System.Type[],System.Type)", "codegen", "System.Linq.Expressions.Expression", "TryGetActionType", True, "benchmark" },
    { 460, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryGetActionType:System.Boolean(System.Type[],System.Type)", "codegen", "System.Linq.Expressions.Expression", "TryGetActionType", True, "benchmark" },
    { 461, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryGetActionType:System.Boolean(System.Type[],System.Type)", "codegen", "System.Linq.Expressions.Expression", "TryGetActionType", True, "benchmark" },
    { 462, "System.Linq.Expressions/System.Linq.Expressions.Expression::GetDelegateType:System.Type(System.Type[])", "codegen", "System.Linq.Expressions.Expression", "GetDelegateType", True, "benchmark" },
    { 463, "System.Linq.Expressions/System.Linq.Expressions.Expression::GetDelegateType:System.Type(System.Type[])", "codegen", "System.Linq.Expressions.Expression", "GetDelegateType", True, "benchmark" },
    { 464, "System.Linq.Expressions/System.Linq.Expressions.Expression::GetDelegateType:System.Type(System.Type[])", "codegen", "System.Linq.Expressions.Expression", "GetDelegateType", True, "benchmark" },
    { 465, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 466, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 467, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 468, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 469, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 470, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 471, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 472, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 473, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 474, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 475, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Linq.Expressions.ElementInit[])", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 476, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Linq.Expressions.ElementInit[])", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 477, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Linq.Expressions.ElementInit[])", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 478, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ElementInit>)", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 479, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListInit:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ElementInit>)", "codegen", "System.Linq.Expressions.Expression", "ListInit", True, "benchmark" },
    { 480, "System.Linq.Expressions/System.Linq.Expressions.Expression::Loop:System.Linq.Expressions.LoopExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Loop", True, "benchmark" },
    { 481, "System.Linq.Expressions/System.Linq.Expressions.Expression::Loop:System.Linq.Expressions.LoopExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.LabelTarget)", "codegen", "System.Linq.Expressions.Expression", "Loop", True, "benchmark" },
    { 482, "System.Linq.Expressions/System.Linq.Expressions.Expression::Loop:System.Linq.Expressions.LoopExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.LabelTarget,System.Linq.Expressions.LabelTarget)", "codegen", "System.Linq.Expressions.Expression", "Loop", True, "benchmark" },
    { 483, "System.Linq.Expressions/System.Linq.Expressions.Expression::Bind:System.Linq.Expressions.MemberAssignment(System.Reflection.MemberInfo,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Bind", True, "benchmark" },
    { 484, "System.Linq.Expressions/System.Linq.Expressions.Expression::Bind:System.Linq.Expressions.MemberAssignment(System.Reflection.MethodInfo,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Bind", True, "benchmark" },
    { 485, "System.Linq.Expressions/System.Linq.Expressions.Expression::Field:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.Reflection.FieldInfo)", "codegen", "System.Linq.Expressions.Expression", "Field", True, "benchmark" },
    { 486, "System.Linq.Expressions/System.Linq.Expressions.Expression::Field:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.String)", "codegen", "System.Linq.Expressions.Expression", "Field", True, "benchmark" },
    { 487, "System.Linq.Expressions/System.Linq.Expressions.Expression::Field:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.String)", "codegen", "System.Linq.Expressions.Expression", "Field", True, "benchmark" },
    { 488, "System.Linq.Expressions/System.Linq.Expressions.Expression::Field:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.Type,System.String)", "codegen", "System.Linq.Expressions.Expression", "Field", True, "benchmark" },
    { 489, "System.Linq.Expressions/System.Linq.Expressions.Expression::Field:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.Type,System.String)", "codegen", "System.Linq.Expressions.Expression", "Field", True, "benchmark" },
    { 490, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.String)", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 491, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.String)", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 492, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.Type,System.String)", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 493, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.Type,System.String)", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 494, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.Reflection.PropertyInfo)", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 495, "System.Linq.Expressions/System.Linq.Expressions.Expression::Property:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Property", True, "benchmark" },
    { 496, "System.Linq.Expressions/System.Linq.Expressions.Expression::PropertyOrField:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.String)", "codegen", "System.Linq.Expressions.Expression", "PropertyOrField", True, "benchmark" },
    { 497, "System.Linq.Expressions/System.Linq.Expressions.Expression::PropertyOrField:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.String)", "codegen", "System.Linq.Expressions.Expression", "PropertyOrField", True, "benchmark" },
    { 498, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeMemberAccess:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression,System.Reflection.MemberInfo)", "codegen", "System.Linq.Expressions.Expression", "MakeMemberAccess", True, "benchmark" },
    { 499, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberInit:System.Linq.Expressions.MemberInitExpression(System.Linq.Expressions.NewExpression,System.Linq.Expressions.MemberBinding[])", "codegen", "System.Linq.Expressions.Expression", "MemberInit", True, "benchmark" },
    { 500, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberInit:System.Linq.Expressions.MemberInitExpression(System.Linq.Expressions.NewExpression,System.Linq.Expressions.MemberBinding[])", "codegen", "System.Linq.Expressions.Expression", "MemberInit", True, "benchmark" },
    { 501, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberInit:System.Linq.Expressions.MemberInitExpression(System.Linq.Expressions.NewExpression,System.Linq.Expressions.MemberBinding[])", "codegen", "System.Linq.Expressions.Expression", "MemberInit", True, "benchmark" },
    { 502, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberInit:System.Linq.Expressions.MemberInitExpression(System.Linq.Expressions.NewExpression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.MemberBinding>)", "codegen", "System.Linq.Expressions.Expression", "MemberInit", True, "benchmark" },
    { 503, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberInit:System.Linq.Expressions.MemberInitExpression(System.Linq.Expressions.NewExpression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.MemberBinding>)", "codegen", "System.Linq.Expressions.Expression", "MemberInit", True, "benchmark" },
    { 504, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListBind:System.Linq.Expressions.MemberListBinding(System.Reflection.MemberInfo,System.Linq.Expressions.ElementInit[])", "codegen", "System.Linq.Expressions.Expression", "ListBind", True, "benchmark" },
    { 505, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListBind:System.Linq.Expressions.MemberListBinding(System.Reflection.MemberInfo,System.Linq.Expressions.ElementInit[])", "codegen", "System.Linq.Expressions.Expression", "ListBind", True, "benchmark" },
    { 506, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListBind:System.Linq.Expressions.MemberListBinding(System.Reflection.MemberInfo,System.Linq.Expressions.ElementInit[])", "codegen", "System.Linq.Expressions.Expression", "ListBind", True, "benchmark" },
    { 507, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListBind:System.Linq.Expressions.MemberListBinding(System.Reflection.MemberInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ElementInit>)", "codegen", "System.Linq.Expressions.Expression", "ListBind", True, "benchmark" },
    { 508, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListBind:System.Linq.Expressions.MemberListBinding(System.Reflection.MemberInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ElementInit>)", "codegen", "System.Linq.Expressions.Expression", "ListBind", True, "benchmark" },
    { 509, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListBind:System.Linq.Expressions.MemberListBinding(System.Reflection.MethodInfo,System.Linq.Expressions.ElementInit[])", "codegen", "System.Linq.Expressions.Expression", "ListBind", True, "benchmark" },
    { 510, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListBind:System.Linq.Expressions.MemberListBinding(System.Reflection.MethodInfo,System.Linq.Expressions.ElementInit[])", "codegen", "System.Linq.Expressions.Expression", "ListBind", True, "benchmark" },
    { 511, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListBind:System.Linq.Expressions.MemberListBinding(System.Reflection.MethodInfo,System.Linq.Expressions.ElementInit[])", "codegen", "System.Linq.Expressions.Expression", "ListBind", True, "benchmark" },
    { 512, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListBind:System.Linq.Expressions.MemberListBinding(System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ElementInit>)", "codegen", "System.Linq.Expressions.Expression", "ListBind", True, "benchmark" },
    { 513, "System.Linq.Expressions/System.Linq.Expressions.Expression::ListBind:System.Linq.Expressions.MemberListBinding(System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ElementInit>)", "codegen", "System.Linq.Expressions.Expression", "ListBind", True, "benchmark" },
    { 514, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberBind:System.Linq.Expressions.MemberMemberBinding(System.Reflection.MemberInfo,System.Linq.Expressions.MemberBinding[])", "codegen", "System.Linq.Expressions.Expression", "MemberBind", True, "benchmark" },
    { 515, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberBind:System.Linq.Expressions.MemberMemberBinding(System.Reflection.MemberInfo,System.Linq.Expressions.MemberBinding[])", "codegen", "System.Linq.Expressions.Expression", "MemberBind", True, "benchmark" },
    { 516, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberBind:System.Linq.Expressions.MemberMemberBinding(System.Reflection.MemberInfo,System.Linq.Expressions.MemberBinding[])", "codegen", "System.Linq.Expressions.Expression", "MemberBind", True, "benchmark" },
    { 517, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberBind:System.Linq.Expressions.MemberMemberBinding(System.Reflection.MemberInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.MemberBinding>)", "codegen", "System.Linq.Expressions.Expression", "MemberBind", True, "benchmark" },
    { 518, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberBind:System.Linq.Expressions.MemberMemberBinding(System.Reflection.MemberInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.MemberBinding>)", "codegen", "System.Linq.Expressions.Expression", "MemberBind", True, "benchmark" },
    { 519, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberBind:System.Linq.Expressions.MemberMemberBinding(System.Reflection.MethodInfo,System.Linq.Expressions.MemberBinding[])", "codegen", "System.Linq.Expressions.Expression", "MemberBind", True, "benchmark" },
    { 520, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberBind:System.Linq.Expressions.MemberMemberBinding(System.Reflection.MethodInfo,System.Linq.Expressions.MemberBinding[])", "codegen", "System.Linq.Expressions.Expression", "MemberBind", True, "benchmark" },
    { 521, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberBind:System.Linq.Expressions.MemberMemberBinding(System.Reflection.MethodInfo,System.Linq.Expressions.MemberBinding[])", "codegen", "System.Linq.Expressions.Expression", "MemberBind", True, "benchmark" },
    { 522, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberBind:System.Linq.Expressions.MemberMemberBinding(System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.MemberBinding>)", "codegen", "System.Linq.Expressions.Expression", "MemberBind", True, "benchmark" },
    { 523, "System.Linq.Expressions/System.Linq.Expressions.Expression::MemberBind:System.Linq.Expressions.MemberMemberBinding(System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.MemberBinding>)", "codegen", "System.Linq.Expressions.Expression", "MemberBind", True, "benchmark" },
    { 524, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Reflection.MethodInfo,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 525, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Reflection.MethodInfo,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 526, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Reflection.MethodInfo,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 527, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Reflection.MethodInfo,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 528, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Reflection.MethodInfo,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 529, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 530, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 531, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 532, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 533, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 534, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 535, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 536, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 537, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 538, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 539, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 540, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.String,System.Type[],System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 541, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.String,System.Type[],System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 542, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.String,System.Type[],System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 543, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.String,System.Type[],System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 544, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Type,System.String,System.Type[],System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 545, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Type,System.String,System.Type[],System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 546, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Type,System.String,System.Type[],System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 547, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Type,System.String,System.Type[],System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 548, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 549, "System.Linq.Expressions/System.Linq.Expressions.Expression::Call:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "Call", True, "benchmark" },
    { 550, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayIndex:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ArrayIndex", True, "benchmark" },
    { 551, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayIndex:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ArrayIndex", True, "benchmark" },
    { 552, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayIndex:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "ArrayIndex", True, "benchmark" },
    { 553, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayIndex:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "ArrayIndex", True, "benchmark" },
    { 554, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayIndex:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "ArrayIndex", True, "benchmark" },
    { 555, "System.Linq.Expressions/System.Linq.Expressions.Expression::NewArrayInit:System.Linq.Expressions.NewArrayExpression(System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "NewArrayInit", True, "benchmark" },
    { 556, "System.Linq.Expressions/System.Linq.Expressions.Expression::NewArrayInit:System.Linq.Expressions.NewArrayExpression(System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "NewArrayInit", True, "benchmark" },
    { 557, "System.Linq.Expressions/System.Linq.Expressions.Expression::NewArrayInit:System.Linq.Expressions.NewArrayExpression(System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "NewArrayInit", True, "benchmark" },
    { 558, "System.Linq.Expressions/System.Linq.Expressions.Expression::NewArrayInit:System.Linq.Expressions.NewArrayExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "NewArrayInit", True, "benchmark" },
    { 559, "System.Linq.Expressions/System.Linq.Expressions.Expression::NewArrayInit:System.Linq.Expressions.NewArrayExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "NewArrayInit", True, "benchmark" },
    { 560, "System.Linq.Expressions/System.Linq.Expressions.Expression::NewArrayBounds:System.Linq.Expressions.NewArrayExpression(System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "NewArrayBounds", True, "benchmark" },
    { 561, "System.Linq.Expressions/System.Linq.Expressions.Expression::NewArrayBounds:System.Linq.Expressions.NewArrayExpression(System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "NewArrayBounds", True, "benchmark" },
    { 562, "System.Linq.Expressions/System.Linq.Expressions.Expression::NewArrayBounds:System.Linq.Expressions.NewArrayExpression(System.Type,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "NewArrayBounds", True, "benchmark" },
    { 563, "System.Linq.Expressions/System.Linq.Expressions.Expression::NewArrayBounds:System.Linq.Expressions.NewArrayExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "NewArrayBounds", True, "benchmark" },
    { 564, "System.Linq.Expressions/System.Linq.Expressions.Expression::NewArrayBounds:System.Linq.Expressions.NewArrayExpression(System.Type,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "NewArrayBounds", True, "benchmark" },
    { 565, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo)", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 566, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 567, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 568, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 569, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 570, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 571, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>,System.Collections.Generic.IEnumerable<System.Reflection.MemberInfo>)", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 572, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>,System.Collections.Generic.IEnumerable<System.Reflection.MemberInfo>)", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 573, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>,System.Reflection.MemberInfo[])", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 574, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>,System.Reflection.MemberInfo[])", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 575, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Reflection.ConstructorInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>,System.Reflection.MemberInfo[])", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 576, "System.Linq.Expressions/System.Linq.Expressions.Expression::New:System.Linq.Expressions.NewExpression(System.Type)", "codegen", "System.Linq.Expressions.Expression", "New", True, "benchmark" },
    { 577, "System.Linq.Expressions/System.Linq.Expressions.Expression::Parameter:System.Linq.Expressions.ParameterExpression(System.Type)", "codegen", "System.Linq.Expressions.Expression", "Parameter", True, "benchmark" },
    { 578, "System.Linq.Expressions/System.Linq.Expressions.Expression::Variable:System.Linq.Expressions.ParameterExpression(System.Type)", "codegen", "System.Linq.Expressions.Expression", "Variable", True, "benchmark" },
    { 579, "System.Linq.Expressions/System.Linq.Expressions.Expression::Parameter:System.Linq.Expressions.ParameterExpression(System.Type,System.String)", "codegen", "System.Linq.Expressions.Expression", "Parameter", True, "benchmark" },
    { 580, "System.Linq.Expressions/System.Linq.Expressions.Expression::Parameter:System.Linq.Expressions.ParameterExpression(System.Type,System.String)", "codegen", "System.Linq.Expressions.Expression", "Parameter", True, "benchmark" },
    { 581, "System.Linq.Expressions/System.Linq.Expressions.Expression::Variable:System.Linq.Expressions.ParameterExpression(System.Type,System.String)", "codegen", "System.Linq.Expressions.Expression", "Variable", True, "benchmark" },
    { 582, "System.Linq.Expressions/System.Linq.Expressions.Expression::Variable:System.Linq.Expressions.ParameterExpression(System.Type,System.String)", "codegen", "System.Linq.Expressions.Expression", "Variable", True, "benchmark" },
    { 583, "System.Linq.Expressions/System.Linq.Expressions.Expression::RuntimeVariables:System.Linq.Expressions.RuntimeVariablesExpression(System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "RuntimeVariables", True, "benchmark" },
    { 584, "System.Linq.Expressions/System.Linq.Expressions.Expression::RuntimeVariables:System.Linq.Expressions.RuntimeVariablesExpression(System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "RuntimeVariables", True, "benchmark" },
    { 585, "System.Linq.Expressions/System.Linq.Expressions.Expression::RuntimeVariables:System.Linq.Expressions.RuntimeVariablesExpression(System.Linq.Expressions.ParameterExpression[])", "codegen", "System.Linq.Expressions.Expression", "RuntimeVariables", True, "benchmark" },
    { 586, "System.Linq.Expressions/System.Linq.Expressions.Expression::RuntimeVariables:System.Linq.Expressions.RuntimeVariablesExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "RuntimeVariables", True, "benchmark" },
    { 587, "System.Linq.Expressions/System.Linq.Expressions.Expression::RuntimeVariables:System.Linq.Expressions.RuntimeVariablesExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression", "RuntimeVariables", True, "benchmark" },
    { 588, "System.Linq.Expressions/System.Linq.Expressions.Expression::SwitchCase:System.Linq.Expressions.SwitchCase(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "SwitchCase", True, "benchmark" },
    { 589, "System.Linq.Expressions/System.Linq.Expressions.Expression::SwitchCase:System.Linq.Expressions.SwitchCase(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "SwitchCase", True, "benchmark" },
    { 590, "System.Linq.Expressions/System.Linq.Expressions.Expression::SwitchCase:System.Linq.Expressions.SwitchCase(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression[])", "codegen", "System.Linq.Expressions.Expression", "SwitchCase", True, "benchmark" },
    { 591, "System.Linq.Expressions/System.Linq.Expressions.Expression::SwitchCase:System.Linq.Expressions.SwitchCase(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "SwitchCase", True, "benchmark" },
    { 592, "System.Linq.Expressions/System.Linq.Expressions.Expression::SwitchCase:System.Linq.Expressions.SwitchCase(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.Expression", "SwitchCase", True, "benchmark" },
    { 593, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 594, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 595, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 596, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 597, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 598, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 599, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 600, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 601, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 602, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 603, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 604, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Linq.Expressions.SwitchCase[])", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 605, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.SwitchCase>)", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 606, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.SwitchCase>)", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 607, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.SwitchCase>)", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 608, "System.Linq.Expressions/System.Linq.Expressions.Expression::Switch:System.Linq.Expressions.SwitchExpression(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Reflection.MethodInfo,System.Collections.Generic.IEnumerable<System.Linq.Expressions.SwitchCase>)", "codegen", "System.Linq.Expressions.Expression", "Switch", True, "benchmark" },
    { 609, "System.Linq.Expressions/System.Linq.Expressions.Expression::SymbolDocument:System.Linq.Expressions.SymbolDocumentInfo(System.String)", "codegen", "System.Linq.Expressions.Expression", "SymbolDocument", True, "benchmark" },
    { 610, "System.Linq.Expressions/System.Linq.Expressions.Expression::SymbolDocument:System.Linq.Expressions.SymbolDocumentInfo(System.String)", "codegen", "System.Linq.Expressions.Expression", "SymbolDocument", True, "benchmark" },
    { 611, "System.Linq.Expressions/System.Linq.Expressions.Expression::SymbolDocument:System.Linq.Expressions.SymbolDocumentInfo(System.String,System.Guid)", "codegen", "System.Linq.Expressions.Expression", "SymbolDocument", True, "benchmark" },
    { 612, "System.Linq.Expressions/System.Linq.Expressions.Expression::SymbolDocument:System.Linq.Expressions.SymbolDocumentInfo(System.String,System.Guid)", "codegen", "System.Linq.Expressions.Expression", "SymbolDocument", True, "benchmark" },
    { 613, "System.Linq.Expressions/System.Linq.Expressions.Expression::SymbolDocument:System.Linq.Expressions.SymbolDocumentInfo(System.String,System.Guid,System.Guid)", "codegen", "System.Linq.Expressions.Expression", "SymbolDocument", True, "benchmark" },
    { 614, "System.Linq.Expressions/System.Linq.Expressions.Expression::SymbolDocument:System.Linq.Expressions.SymbolDocumentInfo(System.String,System.Guid,System.Guid)", "codegen", "System.Linq.Expressions.Expression", "SymbolDocument", True, "benchmark" },
    { 615, "System.Linq.Expressions/System.Linq.Expressions.Expression::SymbolDocument:System.Linq.Expressions.SymbolDocumentInfo(System.String,System.Guid,System.Guid,System.Guid)", "codegen", "System.Linq.Expressions.Expression", "SymbolDocument", True, "benchmark" },
    { 616, "System.Linq.Expressions/System.Linq.Expressions.Expression::SymbolDocument:System.Linq.Expressions.SymbolDocumentInfo(System.String,System.Guid,System.Guid,System.Guid)", "codegen", "System.Linq.Expressions.Expression", "SymbolDocument", True, "benchmark" },
    { 617, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryFault:System.Linq.Expressions.TryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "TryFault", True, "benchmark" },
    { 618, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryFinally:System.Linq.Expressions.TryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "TryFinally", True, "benchmark" },
    { 619, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryCatch:System.Linq.Expressions.TryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.CatchBlock[])", "codegen", "System.Linq.Expressions.Expression", "TryCatch", True, "benchmark" },
    { 620, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryCatch:System.Linq.Expressions.TryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.CatchBlock[])", "codegen", "System.Linq.Expressions.Expression", "TryCatch", True, "benchmark" },
    { 621, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryCatch:System.Linq.Expressions.TryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.CatchBlock[])", "codegen", "System.Linq.Expressions.Expression", "TryCatch", True, "benchmark" },
    { 622, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryCatchFinally:System.Linq.Expressions.TryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.CatchBlock[])", "codegen", "System.Linq.Expressions.Expression", "TryCatchFinally", True, "benchmark" },
    { 623, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryCatchFinally:System.Linq.Expressions.TryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.CatchBlock[])", "codegen", "System.Linq.Expressions.Expression", "TryCatchFinally", True, "benchmark" },
    { 624, "System.Linq.Expressions/System.Linq.Expressions.Expression::TryCatchFinally:System.Linq.Expressions.TryExpression(System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.CatchBlock[])", "codegen", "System.Linq.Expressions.Expression", "TryCatchFinally", True, "benchmark" },
    { 625, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeTry:System.Linq.Expressions.TryExpression(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.CatchBlock>)", "codegen", "System.Linq.Expressions.Expression", "MakeTry", True, "benchmark" },
    { 626, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeTry:System.Linq.Expressions.TryExpression(System.Type,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.CatchBlock>)", "codegen", "System.Linq.Expressions.Expression", "MakeTry", True, "benchmark" },
    { 627, "System.Linq.Expressions/System.Linq.Expressions.Expression::TypeIs:System.Linq.Expressions.TypeBinaryExpression(System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "TypeIs", True, "benchmark" },
    { 628, "System.Linq.Expressions/System.Linq.Expressions.Expression::TypeEqual:System.Linq.Expressions.TypeBinaryExpression(System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "TypeEqual", True, "benchmark" },
    { 629, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeUnary:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.ExpressionType,System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "MakeUnary", True, "benchmark" },
    { 630, "System.Linq.Expressions/System.Linq.Expressions.Expression::MakeUnary:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.ExpressionType,System.Linq.Expressions.Expression,System.Type,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "MakeUnary", True, "benchmark" },
    { 631, "System.Linq.Expressions/System.Linq.Expressions.Expression::Negate:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Negate", True, "benchmark" },
    { 632, "System.Linq.Expressions/System.Linq.Expressions.Expression::Negate:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Negate", True, "benchmark" },
    { 633, "System.Linq.Expressions/System.Linq.Expressions.Expression::UnaryPlus:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "UnaryPlus", True, "benchmark" },
    { 634, "System.Linq.Expressions/System.Linq.Expressions.Expression::UnaryPlus:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "UnaryPlus", True, "benchmark" },
    { 635, "System.Linq.Expressions/System.Linq.Expressions.Expression::NegateChecked:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "NegateChecked", True, "benchmark" },
    { 636, "System.Linq.Expressions/System.Linq.Expressions.Expression::NegateChecked:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "NegateChecked", True, "benchmark" },
    { 637, "System.Linq.Expressions/System.Linq.Expressions.Expression::Not:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Not", True, "benchmark" },
    { 638, "System.Linq.Expressions/System.Linq.Expressions.Expression::Not:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Not", True, "benchmark" },
    { 639, "System.Linq.Expressions/System.Linq.Expressions.Expression::IsFalse:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "IsFalse", True, "benchmark" },
    { 640, "System.Linq.Expressions/System.Linq.Expressions.Expression::IsFalse:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "IsFalse", True, "benchmark" },
    { 641, "System.Linq.Expressions/System.Linq.Expressions.Expression::IsTrue:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "IsTrue", True, "benchmark" },
    { 642, "System.Linq.Expressions/System.Linq.Expressions.Expression::IsTrue:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "IsTrue", True, "benchmark" },
    { 643, "System.Linq.Expressions/System.Linq.Expressions.Expression::OnesComplement:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "OnesComplement", True, "benchmark" },
    { 644, "System.Linq.Expressions/System.Linq.Expressions.Expression::OnesComplement:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "OnesComplement", True, "benchmark" },
    { 645, "System.Linq.Expressions/System.Linq.Expressions.Expression::TypeAs:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "TypeAs", True, "benchmark" },
    { 646, "System.Linq.Expressions/System.Linq.Expressions.Expression::Unbox:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Unbox", True, "benchmark" },
    { 647, "System.Linq.Expressions/System.Linq.Expressions.Expression::Convert:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Convert", True, "benchmark" },
    { 648, "System.Linq.Expressions/System.Linq.Expressions.Expression::Convert:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Type,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Convert", True, "benchmark" },
    { 649, "System.Linq.Expressions/System.Linq.Expressions.Expression::ConvertChecked:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "ConvertChecked", True, "benchmark" },
    { 650, "System.Linq.Expressions/System.Linq.Expressions.Expression::ConvertChecked:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Type,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "ConvertChecked", True, "benchmark" },
    { 651, "System.Linq.Expressions/System.Linq.Expressions.Expression::ArrayLength:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "ArrayLength", True, "benchmark" },
    { 652, "System.Linq.Expressions/System.Linq.Expressions.Expression::Quote:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Quote", True, "benchmark" },
    { 653, "System.Linq.Expressions/System.Linq.Expressions.Expression::Rethrow:System.Linq.Expressions.UnaryExpression()", "codegen", "System.Linq.Expressions.Expression", "Rethrow", True, "benchmark" },
    { 654, "System.Linq.Expressions/System.Linq.Expressions.Expression::Rethrow:System.Linq.Expressions.UnaryExpression(System.Type)", "codegen", "System.Linq.Expressions.Expression", "Rethrow", True, "benchmark" },
    { 655, "System.Linq.Expressions/System.Linq.Expressions.Expression::Throw:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Throw", True, "benchmark" },
    { 656, "System.Linq.Expressions/System.Linq.Expressions.Expression::Throw:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Type)", "codegen", "System.Linq.Expressions.Expression", "Throw", True, "benchmark" },
    { 657, "System.Linq.Expressions/System.Linq.Expressions.Expression::Increment:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Increment", True, "benchmark" },
    { 658, "System.Linq.Expressions/System.Linq.Expressions.Expression::Increment:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Increment", True, "benchmark" },
    { 659, "System.Linq.Expressions/System.Linq.Expressions.Expression::Decrement:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "Decrement", True, "benchmark" },
    { 660, "System.Linq.Expressions/System.Linq.Expressions.Expression::Decrement:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "Decrement", True, "benchmark" },
    { 661, "System.Linq.Expressions/System.Linq.Expressions.Expression::PreIncrementAssign:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "PreIncrementAssign", True, "benchmark" },
    { 662, "System.Linq.Expressions/System.Linq.Expressions.Expression::PreIncrementAssign:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "PreIncrementAssign", True, "benchmark" },
    { 663, "System.Linq.Expressions/System.Linq.Expressions.Expression::PreDecrementAssign:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "PreDecrementAssign", True, "benchmark" },
    { 664, "System.Linq.Expressions/System.Linq.Expressions.Expression::PreDecrementAssign:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "PreDecrementAssign", True, "benchmark" },
    { 665, "System.Linq.Expressions/System.Linq.Expressions.Expression::PostIncrementAssign:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "PostIncrementAssign", True, "benchmark" },
    { 666, "System.Linq.Expressions/System.Linq.Expressions.Expression::PostIncrementAssign:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "PostIncrementAssign", True, "benchmark" },
    { 667, "System.Linq.Expressions/System.Linq.Expressions.Expression::PostDecrementAssign:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.Expression", "PostDecrementAssign", True, "benchmark" },
    { 668, "System.Linq.Expressions/System.Linq.Expressions.Expression::PostDecrementAssign:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression,System.Reflection.MethodInfo)", "codegen", "System.Linq.Expressions.Expression", "PostDecrementAssign", True, "benchmark" },
    { 669, "System.Linq.Expressions/System.Linq.Expressions.Expression<System.Int32>::Compile:System.Int32()", "codegen", "System.Linq.Expressions.Expression<System.Int32>", "Compile", True, "benchmark" },
    { 670, "System.Linq.Expressions/System.Linq.Expressions.Expression<System.Int32>::Compile:System.Int32(System.Boolean)", "codegen", "System.Linq.Expressions.Expression<System.Int32>", "Compile", True, "benchmark" },
    { 671, "System.Linq.Expressions/System.Linq.Expressions.Expression<System.Int32>::Compile:System.Int32(System.Boolean)", "codegen", "System.Linq.Expressions.Expression<System.Int32>", "Compile", True, "benchmark" },
    { 672, "System.Linq.Expressions/System.Linq.Expressions.Expression<System.Int32>::Update:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression<System.Int32>", "Update", True, "benchmark" },
    { 673, "System.Linq.Expressions/System.Linq.Expressions.Expression<System.Int32>::Update:System.Linq.Expressions.Expression<System.Int32>(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.Expression<System.Int32>", "Update", True, "benchmark" },
    { 674, "System.Linq.Expressions/System.Linq.Expressions.Expression<System.Int32>::Compile:System.Int32(System.Runtime.CompilerServices.DebugInfoGenerator)", "codegen", "System.Linq.Expressions.Expression<System.Int32>", "Compile", True, "benchmark" },
    { 675, "System.Linq.Expressions/System.Linq.Expressions.ExpressionVisitor::Visit:System.Linq.Expressions.Expression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.ExpressionVisitor", "Visit", True, "benchmark" },
    { 676, "System.Linq.Expressions/System.Linq.Expressions.ExpressionVisitor::Visit:System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.Expression>(System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.ExpressionVisitor", "Visit", True, "benchmark" },
    { 677, "System.Linq.Expressions/System.Linq.Expressions.ExpressionVisitor::Visit:System.Collections.ObjectModel.ReadOnlyCollection<System.Int32>(System.Collections.ObjectModel.ReadOnlyCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Expressions.ExpressionVisitor", "Visit", True, "benchmark" },
    { 678, "System.Linq.Expressions/System.Linq.Expressions.ExpressionVisitor::Visit:System.Collections.ObjectModel.ReadOnlyCollection<System.Int32>(System.Collections.ObjectModel.ReadOnlyCollection<System.Int32>,System.Func<System.Int32,System.Int32>)", "codegen", "System.Linq.Expressions.ExpressionVisitor", "Visit", True, "benchmark" },
    { 679, "System.Linq.Expressions/System.Linq.Expressions.ExpressionVisitor::VisitAndConvert:System.Linq.Expressions.Expression(System.Linq.Expressions.Expression,System.String)", "codegen", "System.Linq.Expressions.ExpressionVisitor", "VisitAndConvert", True, "benchmark" },
    { 680, "System.Linq.Expressions/System.Linq.Expressions.ExpressionVisitor::VisitAndConvert:System.Linq.Expressions.Expression(System.Linq.Expressions.Expression,System.String)", "codegen", "System.Linq.Expressions.ExpressionVisitor", "VisitAndConvert", True, "benchmark" },
    { 681, "System.Linq.Expressions/System.Linq.Expressions.ExpressionVisitor::VisitAndConvert:System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.Expression>(System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.Expression>,System.String)", "codegen", "System.Linq.Expressions.ExpressionVisitor", "VisitAndConvert", True, "benchmark" },
    { 682, "System.Linq.Expressions/System.Linq.Expressions.ExpressionVisitor::VisitAndConvert:System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.Expression>(System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.Expression>,System.String)", "codegen", "System.Linq.Expressions.ExpressionVisitor", "VisitAndConvert", True, "benchmark" },
    { 683, "System.Linq.Expressions/System.Linq.Expressions.GotoExpression::Update:System.Linq.Expressions.GotoExpression(System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.GotoExpression", "Update", True, "benchmark" },
    { 684, "System.Linq.Expressions/System.Linq.Expressions.IndexExpression::Update:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.IndexExpression", "Update", True, "benchmark" },
    { 685, "System.Linq.Expressions/System.Linq.Expressions.IndexExpression::Update:System.Linq.Expressions.IndexExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.IndexExpression", "Update", True, "benchmark" },
    { 686, "System.Linq.Expressions/System.Linq.Expressions.InvocationExpression::Update:System.Linq.Expressions.InvocationExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.InvocationExpression", "Update", True, "benchmark" },
    { 687, "System.Linq.Expressions/System.Linq.Expressions.InvocationExpression::Update:System.Linq.Expressions.InvocationExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.InvocationExpression", "Update", True, "benchmark" },
    { 688, "System.Linq.Expressions/System.Linq.Expressions.LabelExpression::Update:System.Linq.Expressions.LabelExpression(System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.LabelExpression", "Update", True, "benchmark" },
    { 689, "System.Linq.Expressions/System.Linq.Expressions.LambdaExpression::Compile:System.Delegate()", "codegen", "System.Linq.Expressions.LambdaExpression", "Compile", True, "benchmark" },
    { 690, "System.Linq.Expressions/System.Linq.Expressions.LambdaExpression::Compile:System.Delegate(System.Boolean)", "codegen", "System.Linq.Expressions.LambdaExpression", "Compile", True, "benchmark" },
    { 691, "System.Linq.Expressions/System.Linq.Expressions.LambdaExpression::Compile:System.Delegate(System.Boolean)", "codegen", "System.Linq.Expressions.LambdaExpression", "Compile", True, "benchmark" },
    { 692, "System.Linq.Expressions/System.Linq.Expressions.LambdaExpression::Compile:System.Delegate(System.Runtime.CompilerServices.DebugInfoGenerator)", "codegen", "System.Linq.Expressions.LambdaExpression", "Compile", True, "benchmark" },
    { 693, "System.Linq.Expressions/System.Linq.Expressions.ListInitExpression::Reduce:System.Linq.Expressions.Expression()", "codegen", "System.Linq.Expressions.ListInitExpression", "Reduce", True, "benchmark" },
    { 694, "System.Linq.Expressions/System.Linq.Expressions.ListInitExpression::Update:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ElementInit>)", "codegen", "System.Linq.Expressions.ListInitExpression", "Update", True, "benchmark" },
    { 695, "System.Linq.Expressions/System.Linq.Expressions.ListInitExpression::Update:System.Linq.Expressions.ListInitExpression(System.Linq.Expressions.NewExpression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.ElementInit>)", "codegen", "System.Linq.Expressions.ListInitExpression", "Update", True, "benchmark" },
    { 696, "System.Linq.Expressions/System.Linq.Expressions.LoopExpression::Update:System.Linq.Expressions.LoopExpression(System.Linq.Expressions.LabelTarget,System.Linq.Expressions.LabelTarget,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.LoopExpression", "Update", True, "benchmark" },
    { 697, "System.Linq.Expressions/System.Linq.Expressions.MemberAssignment::Update:System.Linq.Expressions.MemberAssignment(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.MemberAssignment", "Update", True, "benchmark" },
    { 698, "System.Linq.Expressions/System.Linq.Expressions.MemberExpression::Update:System.Linq.Expressions.MemberExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.MemberExpression", "Update", True, "benchmark" },
    { 699, "System.Linq.Expressions/System.Linq.Expressions.MemberInitExpression::Reduce:System.Linq.Expressions.Expression()", "codegen", "System.Linq.Expressions.MemberInitExpression", "Reduce", True, "benchmark" },
    { 700, "System.Linq.Expressions/System.Linq.Expressions.MemberInitExpression::Update:System.Linq.Expressions.MemberInitExpression(System.Linq.Expressions.NewExpression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.MemberBinding>)", "codegen", "System.Linq.Expressions.MemberInitExpression", "Update", True, "benchmark" },
    { 701, "System.Linq.Expressions/System.Linq.Expressions.MemberInitExpression::Update:System.Linq.Expressions.MemberInitExpression(System.Linq.Expressions.NewExpression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.MemberBinding>)", "codegen", "System.Linq.Expressions.MemberInitExpression", "Update", True, "benchmark" },
    { 702, "System.Linq.Expressions/System.Linq.Expressions.MemberListBinding::Update:System.Linq.Expressions.MemberListBinding(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ElementInit>)", "codegen", "System.Linq.Expressions.MemberListBinding", "Update", True, "benchmark" },
    { 703, "System.Linq.Expressions/System.Linq.Expressions.MemberListBinding::Update:System.Linq.Expressions.MemberListBinding(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ElementInit>)", "codegen", "System.Linq.Expressions.MemberListBinding", "Update", True, "benchmark" },
    { 704, "System.Linq.Expressions/System.Linq.Expressions.MemberMemberBinding::Update:System.Linq.Expressions.MemberMemberBinding(System.Collections.Generic.IEnumerable<System.Linq.Expressions.MemberBinding>)", "codegen", "System.Linq.Expressions.MemberMemberBinding", "Update", True, "benchmark" },
    { 705, "System.Linq.Expressions/System.Linq.Expressions.MemberMemberBinding::Update:System.Linq.Expressions.MemberMemberBinding(System.Collections.Generic.IEnumerable<System.Linq.Expressions.MemberBinding>)", "codegen", "System.Linq.Expressions.MemberMemberBinding", "Update", True, "benchmark" },
    { 706, "System.Linq.Expressions/System.Linq.Expressions.MethodCallExpression::Update:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.MethodCallExpression", "Update", True, "benchmark" },
    { 707, "System.Linq.Expressions/System.Linq.Expressions.MethodCallExpression::Update:System.Linq.Expressions.MethodCallExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.MethodCallExpression", "Update", True, "benchmark" },
    { 708, "System.Linq.Expressions/System.Linq.Expressions.NewArrayExpression::Update:System.Linq.Expressions.NewArrayExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.NewArrayExpression", "Update", True, "benchmark" },
    { 709, "System.Linq.Expressions/System.Linq.Expressions.NewArrayExpression::Update:System.Linq.Expressions.NewArrayExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.NewArrayExpression", "Update", True, "benchmark" },
    { 710, "System.Linq.Expressions/System.Linq.Expressions.NewExpression::Update:System.Linq.Expressions.NewExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.NewExpression", "Update", True, "benchmark" },
    { 711, "System.Linq.Expressions/System.Linq.Expressions.NewExpression::Update:System.Linq.Expressions.NewExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>)", "codegen", "System.Linq.Expressions.NewExpression", "Update", True, "benchmark" },
    { 712, "System.Linq.Expressions/System.Linq.Expressions.RuntimeVariablesExpression::Update:System.Linq.Expressions.RuntimeVariablesExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.RuntimeVariablesExpression", "Update", True, "benchmark" },
    { 713, "System.Linq.Expressions/System.Linq.Expressions.RuntimeVariablesExpression::Update:System.Linq.Expressions.RuntimeVariablesExpression(System.Collections.Generic.IEnumerable<System.Linq.Expressions.ParameterExpression>)", "codegen", "System.Linq.Expressions.RuntimeVariablesExpression", "Update", True, "benchmark" },
    { 714, "System.Linq.Expressions/System.Linq.Expressions.SwitchCase::Update:System.Linq.Expressions.SwitchCase(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.SwitchCase", "Update", True, "benchmark" },
    { 715, "System.Linq.Expressions/System.Linq.Expressions.SwitchCase::Update:System.Linq.Expressions.SwitchCase(System.Collections.Generic.IEnumerable<System.Linq.Expressions.Expression>,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.SwitchCase", "Update", True, "benchmark" },
    { 716, "System.Linq.Expressions/System.Linq.Expressions.SwitchExpression::Update:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.SwitchCase>,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.SwitchExpression", "Update", True, "benchmark" },
    { 717, "System.Linq.Expressions/System.Linq.Expressions.SwitchExpression::Update:System.Linq.Expressions.SwitchExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.SwitchCase>,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.SwitchExpression", "Update", True, "benchmark" },
    { 718, "System.Linq.Expressions/System.Linq.Expressions.TryExpression::Update:System.Linq.Expressions.TryExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.CatchBlock>,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.TryExpression", "Update", True, "benchmark" },
    { 719, "System.Linq.Expressions/System.Linq.Expressions.TryExpression::Update:System.Linq.Expressions.TryExpression(System.Linq.Expressions.Expression,System.Collections.Generic.IEnumerable<System.Linq.Expressions.CatchBlock>,System.Linq.Expressions.Expression,System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.TryExpression", "Update", True, "benchmark" },
    { 720, "System.Linq.Expressions/System.Linq.Expressions.TypeBinaryExpression::Update:System.Linq.Expressions.TypeBinaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.TypeBinaryExpression", "Update", True, "benchmark" },
    { 721, "System.Linq.Expressions/System.Linq.Expressions.UnaryExpression::Reduce:System.Linq.Expressions.Expression()", "codegen", "System.Linq.Expressions.UnaryExpression", "Reduce", True, "benchmark" },
    { 722, "System.Linq.Expressions/System.Linq.Expressions.UnaryExpression::Update:System.Linq.Expressions.UnaryExpression(System.Linq.Expressions.Expression)", "codegen", "System.Linq.Expressions.UnaryExpression", "Update", True, "benchmark" },
    { 723, "System.Linq.Expressions/System.Runtime.CompilerServices.CallSiteBinder::Bind:System.Linq.Expressions.Expression(System.Object[],System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.LabelTarget)", "codegen", "System.Runtime.CompilerServices.CallSiteBinder", "Bind", True, "benchmark" },
    { 724, "System.Linq.Expressions/System.Runtime.CompilerServices.CallSiteBinder::Bind:System.Linq.Expressions.Expression(System.Object[],System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.LabelTarget)", "codegen", "System.Runtime.CompilerServices.CallSiteBinder", "Bind", True, "benchmark" },
    { 725, "System.Linq.Expressions/System.Runtime.CompilerServices.CallSiteBinder::Bind:System.Linq.Expressions.Expression(System.Object[],System.Collections.ObjectModel.ReadOnlyCollection<System.Linq.Expressions.ParameterExpression>,System.Linq.Expressions.LabelTarget)", "codegen", "System.Runtime.CompilerServices.CallSiteBinder", "Bind", True, "benchmark" },
    { 726, "System.Linq.Expressions/System.Runtime.CompilerServices.CallSiteBinder::BindDelegate:System.String(System.Runtime.CompilerServices.CallSite<System.String>,System.Object[])", "codegen", "System.Runtime.CompilerServices.CallSiteBinder", "BindDelegate", True, "benchmark" },
    { 727, "System.Linq.Expressions/System.Runtime.CompilerServices.CallSiteBinder::BindDelegate:System.String(System.Runtime.CompilerServices.CallSite<System.String>,System.Object[])", "codegen", "System.Runtime.CompilerServices.CallSiteBinder", "BindDelegate", True, "benchmark" },
    { 728, "System.Linq.Expressions/System.Runtime.CompilerServices.CallSiteBinder::BindDelegate:System.String(System.Runtime.CompilerServices.CallSite<System.String>,System.Object[])", "codegen", "System.Runtime.CompilerServices.CallSiteBinder", "BindDelegate", True, "benchmark" },
    { 729, "System.Linq.Expressions/System.Runtime.CompilerServices.CallSiteHelpers::IsInternalFrame:System.Boolean(System.Reflection.MethodBase)", "codegen", "System.Runtime.CompilerServices.CallSiteHelpers", "IsInternalFrame", True, "benchmark" },
    { 730, "System.Linq.Expressions/System.Runtime.CompilerServices.DebugInfoGenerator::CreatePdbGenerator:System.Runtime.CompilerServices.DebugInfoGenerator()", "codegen", "System.Runtime.CompilerServices.DebugInfoGenerator", "CreatePdbGenerator", True, "benchmark" },
    { 731, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::IndexOf:System.Int32(System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "IndexOf", True, "benchmark" },
    { 732, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::IndexOf:System.Int32(System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "IndexOf", True, "benchmark" },
    { 733, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Insert:System.Void(System.Int32,System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Insert", True, "benchmark" },
    { 734, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Insert:System.Void(System.Int32,System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Insert", True, "benchmark" },
    { 735, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Insert:System.Void(System.Int32,System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Insert", True, "benchmark" },
    { 736, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::RemoveAt:System.Void(System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "RemoveAt", True, "benchmark" },
    { 737, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::RemoveAt:System.Void(System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "RemoveAt", True, "benchmark" },
    { 738, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Add:System.Void(System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Add", True, "benchmark" },
    { 739, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Add:System.Void(System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Add", True, "benchmark" },
    { 740, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Clear:System.Void()", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Clear", True, "benchmark" },
    { 741, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Contains:System.Boolean(System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Contains", True, "benchmark" },
    { 742, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Contains:System.Boolean(System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Contains", True, "benchmark" },
    { 743, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Remove:System.Boolean(System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Remove", True, "benchmark" },
    { 744, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Remove:System.Boolean(System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Remove", True, "benchmark" },
    { 745, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::GetEnumerator:System.Collections.Generic.IEnumerator<System.Int32>()", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "GetEnumerator", True, "benchmark" },
    { 746, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Reverse:System.Void()", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Reverse", True, "benchmark" },
    { 747, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Reverse:System.Void(System.Int32,System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Reverse", True, "benchmark" },
    { 748, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Reverse:System.Void(System.Int32,System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Reverse", True, "benchmark" },
    { 749, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::Reverse:System.Void(System.Int32,System.Int32)", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "Reverse", True, "benchmark" },
    { 750, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::ToArray:System.Int32[]()", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "ToArray", True, "benchmark" },
    { 751, "System.Linq.Expressions/System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>::ToReadOnlyCollection:System.Collections.ObjectModel.ReadOnlyCollection<System.Int32>()", "codegen", "System.Runtime.CompilerServices.ReadOnlyCollectionBuilder<System.Int32>", "ToReadOnlyCollection", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 752;

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