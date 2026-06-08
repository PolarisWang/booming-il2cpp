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
    { 0, "System.ComponentModel.TypeConverter/System.ComponentModel.ArrayConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.ArrayConverter", "ConvertTo", True, "benchmark" },
    { 1, "System.ComponentModel.TypeConverter/System.ComponentModel.ArrayConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.ArrayConverter", "GetProperties", True, "benchmark" },
    { 2, "System.ComponentModel.TypeConverter/System.ComponentModel.ArrayConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.ArrayConverter", "GetProperties", True, "benchmark" },
    { 3, "System.ComponentModel.TypeConverter/System.ComponentModel.ArrayConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.ArrayConverter", "GetProperties", True, "benchmark" },
    { 4, "System.ComponentModel.TypeConverter/System.ComponentModel.ArrayConverter::GetPropertiesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.ArrayConverter", "GetPropertiesSupported", True, "benchmark" },
    { 5, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::FromExisting:System.ComponentModel.AttributeCollection(System.ComponentModel.AttributeCollection,System.Attribute[])", "codegen", "System.ComponentModel.AttributeCollection", "FromExisting", True, "benchmark" },
    { 6, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::FromExisting:System.ComponentModel.AttributeCollection(System.ComponentModel.AttributeCollection,System.Attribute[])", "codegen", "System.ComponentModel.AttributeCollection", "FromExisting", True, "benchmark" },
    { 7, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::FromExisting:System.ComponentModel.AttributeCollection(System.ComponentModel.AttributeCollection,System.Attribute[])", "codegen", "System.ComponentModel.AttributeCollection", "FromExisting", True, "benchmark" },
    { 8, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::Contains:System.Boolean(System.Attribute)", "codegen", "System.ComponentModel.AttributeCollection", "Contains", True, "benchmark" },
    { 9, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::Contains:System.Boolean(System.Attribute[])", "codegen", "System.ComponentModel.AttributeCollection", "Contains", True, "benchmark" },
    { 10, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::Contains:System.Boolean(System.Attribute[])", "codegen", "System.ComponentModel.AttributeCollection", "Contains", True, "benchmark" },
    { 11, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::Contains:System.Boolean(System.Attribute[])", "codegen", "System.ComponentModel.AttributeCollection", "Contains", True, "benchmark" },
    { 12, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.ComponentModel.AttributeCollection", "GetEnumerator", True, "benchmark" },
    { 13, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::Matches:System.Boolean(System.Attribute)", "codegen", "System.ComponentModel.AttributeCollection", "Matches", True, "benchmark" },
    { 14, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::Matches:System.Boolean(System.Attribute[])", "codegen", "System.ComponentModel.AttributeCollection", "Matches", True, "benchmark" },
    { 15, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::Matches:System.Boolean(System.Attribute[])", "codegen", "System.ComponentModel.AttributeCollection", "Matches", True, "benchmark" },
    { 16, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::Matches:System.Boolean(System.Attribute[])", "codegen", "System.ComponentModel.AttributeCollection", "Matches", True, "benchmark" },
    { 17, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.ComponentModel.AttributeCollection", "CopyTo", True, "benchmark" },
    { 18, "System.ComponentModel.TypeConverter/System.ComponentModel.AttributeCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.ComponentModel.AttributeCollection", "CopyTo", True, "benchmark" },
    { 19, "System.ComponentModel.TypeConverter/System.ComponentModel.BaseNumberConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.BaseNumberConverter", "CanConvertFrom", True, "benchmark" },
    { 20, "System.ComponentModel.TypeConverter/System.ComponentModel.BaseNumberConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.BaseNumberConverter", "ConvertFrom", True, "benchmark" },
    { 21, "System.ComponentModel.TypeConverter/System.ComponentModel.BaseNumberConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.BaseNumberConverter", "ConvertTo", True, "benchmark" },
    { 22, "System.ComponentModel.TypeConverter/System.ComponentModel.BaseNumberConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.BaseNumberConverter", "CanConvertTo", True, "benchmark" },
    { 23, "System.ComponentModel.TypeConverter/System.ComponentModel.BindableAttribute::IsDefaultAttribute:System.Boolean()", "codegen", "System.ComponentModel.BindableAttribute", "IsDefaultAttribute", True, "benchmark" },
    { 24, "System.ComponentModel.TypeConverter/System.ComponentModel.BindingList<System.Int32>::ResetBindings:System.Void()", "codegen", "System.ComponentModel.BindingList<System.Int32>", "ResetBindings", True, "benchmark" },
    { 25, "System.ComponentModel.TypeConverter/System.ComponentModel.BindingList<System.Int32>::ResetItem:System.Void(System.Int32)", "codegen", "System.ComponentModel.BindingList<System.Int32>", "ResetItem", True, "benchmark" },
    { 26, "System.ComponentModel.TypeConverter/System.ComponentModel.BindingList<System.Int32>::ResetItem:System.Void(System.Int32)", "codegen", "System.ComponentModel.BindingList<System.Int32>", "ResetItem", True, "benchmark" },
    { 27, "System.ComponentModel.TypeConverter/System.ComponentModel.BindingList<System.Int32>::CancelNew:System.Void(System.Int32)", "codegen", "System.ComponentModel.BindingList<System.Int32>", "CancelNew", True, "benchmark" },
    { 28, "System.ComponentModel.TypeConverter/System.ComponentModel.BindingList<System.Int32>::CancelNew:System.Void(System.Int32)", "codegen", "System.ComponentModel.BindingList<System.Int32>", "CancelNew", True, "benchmark" },
    { 29, "System.ComponentModel.TypeConverter/System.ComponentModel.BindingList<System.Int32>::EndNew:System.Void(System.Int32)", "codegen", "System.ComponentModel.BindingList<System.Int32>", "EndNew", True, "benchmark" },
    { 30, "System.ComponentModel.TypeConverter/System.ComponentModel.BindingList<System.Int32>::EndNew:System.Void(System.Int32)", "codegen", "System.ComponentModel.BindingList<System.Int32>", "EndNew", True, "benchmark" },
    { 31, "System.ComponentModel.TypeConverter/System.ComponentModel.BindingList<System.Int32>::AddNew:System.Int32()", "codegen", "System.ComponentModel.BindingList<System.Int32>", "AddNew", True, "benchmark" },
    { 32, "System.ComponentModel.TypeConverter/System.ComponentModel.BooleanConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.BooleanConverter", "CanConvertFrom", True, "benchmark" },
    { 33, "System.ComponentModel.TypeConverter/System.ComponentModel.BooleanConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.BooleanConverter", "ConvertFrom", True, "benchmark" },
    { 34, "System.ComponentModel.TypeConverter/System.ComponentModel.BooleanConverter::GetStandardValues:TypeConverter.StandardValuesCollection(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.BooleanConverter", "GetStandardValues", True, "benchmark" },
    { 35, "System.ComponentModel.TypeConverter/System.ComponentModel.BooleanConverter::GetStandardValuesExclusive:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.BooleanConverter", "GetStandardValuesExclusive", True, "benchmark" },
    { 36, "System.ComponentModel.TypeConverter/System.ComponentModel.BooleanConverter::GetStandardValuesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.BooleanConverter", "GetStandardValuesSupported", True, "benchmark" },
    { 37, "System.ComponentModel.TypeConverter/System.ComponentModel.CharConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.CharConverter", "CanConvertFrom", True, "benchmark" },
    { 38, "System.ComponentModel.TypeConverter/System.ComponentModel.CharConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.CharConverter", "ConvertTo", True, "benchmark" },
    { 39, "System.ComponentModel.TypeConverter/System.ComponentModel.CharConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.CharConverter", "ConvertFrom", True, "benchmark" },
    { 40, "System.ComponentModel.TypeConverter/System.ComponentModel.CollectionConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.CollectionConverter", "ConvertTo", True, "benchmark" },
    { 41, "System.ComponentModel.TypeConverter/System.ComponentModel.CollectionConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.CollectionConverter", "GetProperties", True, "benchmark" },
    { 42, "System.ComponentModel.TypeConverter/System.ComponentModel.CollectionConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.CollectionConverter", "GetProperties", True, "benchmark" },
    { 43, "System.ComponentModel.TypeConverter/System.ComponentModel.CollectionConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.CollectionConverter", "GetProperties", True, "benchmark" },
    { 44, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.ComponentConverter", "GetProperties", True, "benchmark" },
    { 45, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.ComponentConverter", "GetProperties", True, "benchmark" },
    { 46, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.ComponentConverter", "GetProperties", True, "benchmark" },
    { 47, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentConverter::GetPropertiesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.ComponentConverter", "GetPropertiesSupported", True, "benchmark" },
    { 48, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentEditor::EditComponent:System.Boolean(System.Object)", "codegen", "System.ComponentModel.ComponentEditor", "EditComponent", True, "benchmark" },
    { 49, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentEditor::EditComponent:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Object)", "codegen", "System.ComponentModel.ComponentEditor", "EditComponent", True, "benchmark" },
    { 50, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentResourceManager::ApplyResources:System.Void(System.Object,System.String)", "codegen", "System.ComponentModel.ComponentResourceManager", "ApplyResources", True, "benchmark" },
    { 51, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentResourceManager::ApplyResources:System.Void(System.Object,System.String)", "codegen", "System.ComponentModel.ComponentResourceManager", "ApplyResources", True, "benchmark" },
    { 52, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentResourceManager::ApplyResources:System.Void(System.Object,System.String,System.Globalization.CultureInfo)", "codegen", "System.ComponentModel.ComponentResourceManager", "ApplyResources", True, "benchmark" },
    { 53, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentResourceManager::ApplyResources:System.Void(System.Object,System.String,System.Globalization.CultureInfo)", "codegen", "System.ComponentModel.ComponentResourceManager", "ApplyResources", True, "benchmark" },
    { 54, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentResourceManager::ApplyResourcesToRegisteredType:System.Void(System.Object,System.String,System.Globalization.CultureInfo)", "codegen", "System.ComponentModel.ComponentResourceManager", "ApplyResourcesToRegisteredType", True, "benchmark" },
    { 55, "System.ComponentModel.TypeConverter/System.ComponentModel.ComponentResourceManager::ApplyResourcesToRegisteredType:System.Void(System.Object,System.String,System.Globalization.CultureInfo)", "codegen", "System.ComponentModel.ComponentResourceManager", "ApplyResourcesToRegisteredType", True, "benchmark" },
    { 56, "System.ComponentModel.TypeConverter/System.ComponentModel.Container::Add:System.Void(System.ComponentModel.IComponent)", "codegen", "System.ComponentModel.Container", "Add", True, "benchmark" },
    { 57, "System.ComponentModel.TypeConverter/System.ComponentModel.Container::Add:System.Void(System.ComponentModel.IComponent,System.String)", "codegen", "System.ComponentModel.Container", "Add", True, "benchmark" },
    { 58, "System.ComponentModel.TypeConverter/System.ComponentModel.Container::Add:System.Void(System.ComponentModel.IComponent,System.String)", "codegen", "System.ComponentModel.Container", "Add", True, "benchmark" },
    { 59, "System.ComponentModel.TypeConverter/System.ComponentModel.Container::Dispose:System.Void()", "codegen", "System.ComponentModel.Container", "Dispose", True, "benchmark" },
    { 60, "System.ComponentModel.TypeConverter/System.ComponentModel.Container::Remove:System.Void(System.ComponentModel.IComponent)", "codegen", "System.ComponentModel.Container", "Remove", True, "benchmark" },
    { 61, "System.ComponentModel.TypeConverter/System.ComponentModel.ContainerFilterService::FilterComponents:System.ComponentModel.ComponentCollection(System.ComponentModel.ComponentCollection)", "codegen", "System.ComponentModel.ContainerFilterService", "FilterComponents", True, "benchmark" },
    { 62, "System.ComponentModel.TypeConverter/System.ComponentModel.CultureInfoConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.CultureInfoConverter", "CanConvertFrom", True, "benchmark" },
    { 63, "System.ComponentModel.TypeConverter/System.ComponentModel.CultureInfoConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.CultureInfoConverter", "CanConvertTo", True, "benchmark" },
    { 64, "System.ComponentModel.TypeConverter/System.ComponentModel.CultureInfoConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.CultureInfoConverter", "ConvertFrom", True, "benchmark" },
    { 65, "System.ComponentModel.TypeConverter/System.ComponentModel.CultureInfoConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.CultureInfoConverter", "ConvertTo", True, "benchmark" },
    { 66, "System.ComponentModel.TypeConverter/System.ComponentModel.CultureInfoConverter::GetStandardValues:TypeConverter.StandardValuesCollection(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.CultureInfoConverter", "GetStandardValues", True, "benchmark" },
    { 67, "System.ComponentModel.TypeConverter/System.ComponentModel.CultureInfoConverter::GetStandardValuesExclusive:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.CultureInfoConverter", "GetStandardValuesExclusive", True, "benchmark" },
    { 68, "System.ComponentModel.TypeConverter/System.ComponentModel.CultureInfoConverter::GetStandardValuesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.CultureInfoConverter", "GetStandardValuesSupported", True, "benchmark" },
    { 69, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetAttributes:System.ComponentModel.AttributeCollection()", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetAttributes", True, "benchmark" },
    { 70, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetClassName:System.String()", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetClassName", True, "benchmark" },
    { 71, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetComponentName:System.String()", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetComponentName", True, "benchmark" },
    { 72, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetConverter:System.ComponentModel.TypeConverter()", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetConverter", True, "benchmark" },
    { 73, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetDefaultEvent:System.ComponentModel.EventDescriptor()", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetDefaultEvent", True, "benchmark" },
    { 74, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetDefaultProperty:System.ComponentModel.PropertyDescriptor()", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetDefaultProperty", True, "benchmark" },
    { 75, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetEditor:System.Object(System.Type)", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetEditor", True, "benchmark" },
    { 76, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection()", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetEvents", True, "benchmark" },
    { 77, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Attribute[])", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetEvents", True, "benchmark" },
    { 78, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Attribute[])", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetEvents", True, "benchmark" },
    { 79, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Attribute[])", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetEvents", True, "benchmark" },
    { 80, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection()", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetProperties", True, "benchmark" },
    { 81, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Attribute[])", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetProperties", True, "benchmark" },
    { 82, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Attribute[])", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetProperties", True, "benchmark" },
    { 83, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Attribute[])", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetProperties", True, "benchmark" },
    { 84, "System.ComponentModel.TypeConverter/System.ComponentModel.CustomTypeDescriptor::GetPropertyOwner:System.Object(System.ComponentModel.PropertyDescriptor)", "codegen", "System.ComponentModel.CustomTypeDescriptor", "GetPropertyOwner", True, "benchmark" },
    { 85, "System.ComponentModel.TypeConverter/System.ComponentModel.DataObjectAttribute::IsDefaultAttribute:System.Boolean()", "codegen", "System.ComponentModel.DataObjectAttribute", "IsDefaultAttribute", True, "benchmark" },
    { 86, "System.ComponentModel.TypeConverter/System.ComponentModel.DataObjectMethodAttribute::Match:System.Boolean(System.Object)", "codegen", "System.ComponentModel.DataObjectMethodAttribute", "Match", True, "benchmark" },
    { 87, "System.ComponentModel.TypeConverter/System.ComponentModel.DateOnlyConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.DateOnlyConverter", "CanConvertFrom", True, "benchmark" },
    { 88, "System.ComponentModel.TypeConverter/System.ComponentModel.DateOnlyConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.DateOnlyConverter", "CanConvertTo", True, "benchmark" },
    { 89, "System.ComponentModel.TypeConverter/System.ComponentModel.DateOnlyConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.DateOnlyConverter", "ConvertFrom", True, "benchmark" },
    { 90, "System.ComponentModel.TypeConverter/System.ComponentModel.DateOnlyConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.DateOnlyConverter", "ConvertTo", True, "benchmark" },
    { 91, "System.ComponentModel.TypeConverter/System.ComponentModel.DateTimeConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.DateTimeConverter", "CanConvertFrom", True, "benchmark" },
    { 92, "System.ComponentModel.TypeConverter/System.ComponentModel.DateTimeConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.DateTimeConverter", "CanConvertTo", True, "benchmark" },
    { 93, "System.ComponentModel.TypeConverter/System.ComponentModel.DateTimeConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.DateTimeConverter", "ConvertFrom", True, "benchmark" },
    { 94, "System.ComponentModel.TypeConverter/System.ComponentModel.DateTimeConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.DateTimeConverter", "ConvertTo", True, "benchmark" },
    { 95, "System.ComponentModel.TypeConverter/System.ComponentModel.DateTimeOffsetConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.DateTimeOffsetConverter", "CanConvertFrom", True, "benchmark" },
    { 96, "System.ComponentModel.TypeConverter/System.ComponentModel.DateTimeOffsetConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.DateTimeOffsetConverter", "CanConvertTo", True, "benchmark" },
    { 97, "System.ComponentModel.TypeConverter/System.ComponentModel.DateTimeOffsetConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.DateTimeOffsetConverter", "ConvertFrom", True, "benchmark" },
    { 98, "System.ComponentModel.TypeConverter/System.ComponentModel.DateTimeOffsetConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.DateTimeOffsetConverter", "ConvertTo", True, "benchmark" },
    { 99, "System.ComponentModel.TypeConverter/System.ComponentModel.DecimalConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.DecimalConverter", "CanConvertTo", True, "benchmark" },
    { 100, "System.ComponentModel.TypeConverter/System.ComponentModel.DecimalConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.DecimalConverter", "ConvertTo", True, "benchmark" },
    { 101, "System.ComponentModel.TypeConverter/System.ComponentModel.DesignTimeVisibleAttribute::IsDefaultAttribute:System.Boolean()", "codegen", "System.ComponentModel.DesignTimeVisibleAttribute", "IsDefaultAttribute", True, "benchmark" },
    { 102, "System.ComponentModel.TypeConverter/System.ComponentModel.EnumConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.EnumConverter", "CanConvertFrom", True, "benchmark" },
    { 103, "System.ComponentModel.TypeConverter/System.ComponentModel.EnumConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.EnumConverter", "CanConvertTo", True, "benchmark" },
    { 104, "System.ComponentModel.TypeConverter/System.ComponentModel.EnumConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.EnumConverter", "ConvertFrom", True, "benchmark" },
    { 105, "System.ComponentModel.TypeConverter/System.ComponentModel.EnumConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.EnumConverter", "ConvertTo", True, "benchmark" },
    { 106, "System.ComponentModel.TypeConverter/System.ComponentModel.EnumConverter::GetStandardValues:TypeConverter.StandardValuesCollection(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.EnumConverter", "GetStandardValues", True, "benchmark" },
    { 107, "System.ComponentModel.TypeConverter/System.ComponentModel.EnumConverter::GetStandardValuesExclusive:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.EnumConverter", "GetStandardValuesExclusive", True, "benchmark" },
    { 108, "System.ComponentModel.TypeConverter/System.ComponentModel.EnumConverter::GetStandardValuesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.EnumConverter", "GetStandardValuesSupported", True, "benchmark" },
    { 109, "System.ComponentModel.TypeConverter/System.ComponentModel.EnumConverter::IsValid:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Object)", "codegen", "System.ComponentModel.EnumConverter", "IsValid", True, "benchmark" },
    { 110, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptor::AddEventHandler:System.Void(System.Object,System.Delegate)", "codegen", "System.ComponentModel.EventDescriptor", "AddEventHandler", True, "benchmark" },
    { 111, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptor::RemoveEventHandler:System.Void(System.Object,System.Delegate)", "codegen", "System.ComponentModel.EventDescriptor", "RemoveEventHandler", True, "benchmark" },
    { 112, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Add:System.Int32(System.ComponentModel.EventDescriptor)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Add", True, "benchmark" },
    { 113, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Clear:System.Void()", "codegen", "System.ComponentModel.EventDescriptorCollection", "Clear", True, "benchmark" },
    { 114, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Contains:System.Boolean(System.ComponentModel.EventDescriptor)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Contains", True, "benchmark" },
    { 115, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Find:System.ComponentModel.EventDescriptor(System.String,System.Boolean)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Find", True, "benchmark" },
    { 116, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Find:System.ComponentModel.EventDescriptor(System.String,System.Boolean)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Find", True, "benchmark" },
    { 117, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Find:System.ComponentModel.EventDescriptor(System.String,System.Boolean)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Find", True, "benchmark" },
    { 118, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::IndexOf:System.Int32(System.ComponentModel.EventDescriptor)", "codegen", "System.ComponentModel.EventDescriptorCollection", "IndexOf", True, "benchmark" },
    { 119, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Insert:System.Void(System.Int32,System.ComponentModel.EventDescriptor)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Insert", True, "benchmark" },
    { 120, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Insert:System.Void(System.Int32,System.ComponentModel.EventDescriptor)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Insert", True, "benchmark" },
    { 121, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Remove:System.Void(System.ComponentModel.EventDescriptor)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Remove", True, "benchmark" },
    { 122, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.ComponentModel.EventDescriptorCollection", "RemoveAt", True, "benchmark" },
    { 123, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.ComponentModel.EventDescriptorCollection", "RemoveAt", True, "benchmark" },
    { 124, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.ComponentModel.EventDescriptorCollection", "GetEnumerator", True, "benchmark" },
    { 125, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Sort:System.ComponentModel.EventDescriptorCollection()", "codegen", "System.ComponentModel.EventDescriptorCollection", "Sort", True, "benchmark" },
    { 126, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Sort:System.ComponentModel.EventDescriptorCollection(System.String[])", "codegen", "System.ComponentModel.EventDescriptorCollection", "Sort", True, "benchmark" },
    { 127, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Sort:System.ComponentModel.EventDescriptorCollection(System.String[])", "codegen", "System.ComponentModel.EventDescriptorCollection", "Sort", True, "benchmark" },
    { 128, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Sort:System.ComponentModel.EventDescriptorCollection(System.String[])", "codegen", "System.ComponentModel.EventDescriptorCollection", "Sort", True, "benchmark" },
    { 129, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Sort:System.ComponentModel.EventDescriptorCollection(System.String[],System.Collections.IComparer)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Sort", True, "benchmark" },
    { 130, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Sort:System.ComponentModel.EventDescriptorCollection(System.String[],System.Collections.IComparer)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Sort", True, "benchmark" },
    { 131, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Sort:System.ComponentModel.EventDescriptorCollection(System.String[],System.Collections.IComparer)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Sort", True, "benchmark" },
    { 132, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Sort:System.ComponentModel.EventDescriptorCollection(System.Collections.IComparer)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Sort", True, "benchmark" },
    { 133, "System.ComponentModel.TypeConverter/System.ComponentModel.EventDescriptorCollection::Sort:System.ComponentModel.EventDescriptorCollection(System.Collections.IComparer)", "codegen", "System.ComponentModel.EventDescriptorCollection", "Sort", True, "benchmark" },
    { 134, "System.ComponentModel.TypeConverter/System.ComponentModel.ExpandableObjectConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.ExpandableObjectConverter", "GetProperties", True, "benchmark" },
    { 135, "System.ComponentModel.TypeConverter/System.ComponentModel.ExpandableObjectConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.ExpandableObjectConverter", "GetProperties", True, "benchmark" },
    { 136, "System.ComponentModel.TypeConverter/System.ComponentModel.ExpandableObjectConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.ExpandableObjectConverter", "GetProperties", True, "benchmark" },
    { 137, "System.ComponentModel.TypeConverter/System.ComponentModel.ExpandableObjectConverter::GetPropertiesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.ExpandableObjectConverter", "GetPropertiesSupported", True, "benchmark" },
    { 138, "System.ComponentModel.TypeConverter/System.ComponentModel.ExtenderProvidedPropertyAttribute::IsDefaultAttribute:System.Boolean()", "codegen", "System.ComponentModel.ExtenderProvidedPropertyAttribute", "IsDefaultAttribute", True, "benchmark" },
    { 139, "System.ComponentModel.TypeConverter/System.ComponentModel.GuidConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.GuidConverter", "CanConvertFrom", True, "benchmark" },
    { 140, "System.ComponentModel.TypeConverter/System.ComponentModel.GuidConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.GuidConverter", "CanConvertTo", True, "benchmark" },
    { 141, "System.ComponentModel.TypeConverter/System.ComponentModel.GuidConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.GuidConverter", "ConvertFrom", True, "benchmark" },
    { 142, "System.ComponentModel.TypeConverter/System.ComponentModel.GuidConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.GuidConverter", "ConvertTo", True, "benchmark" },
    { 143, "System.ComponentModel.TypeConverter/System.ComponentModel.InheritanceAttribute::IsDefaultAttribute:System.Boolean()", "codegen", "System.ComponentModel.InheritanceAttribute", "IsDefaultAttribute", True, "benchmark" },
    { 144, "System.ComponentModel.TypeConverter/System.ComponentModel.InstanceCreationEditor::CreateInstance:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.InstanceCreationEditor", "CreateInstance", True, "benchmark" },
    { 145, "System.ComponentModel.TypeConverter/System.ComponentModel.License::Dispose:System.Void()", "codegen", "System.ComponentModel.License", "Dispose", True, "benchmark" },
    { 146, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseContext::GetSavedLicenseKey:System.String(System.Type,System.Reflection.Assembly)", "codegen", "System.ComponentModel.LicenseContext", "GetSavedLicenseKey", True, "benchmark" },
    { 147, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseContext::GetService:System.Object(System.Type)", "codegen", "System.ComponentModel.LicenseContext", "GetService", True, "benchmark" },
    { 148, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseContext::SetSavedLicenseKey:System.Void(System.Type,System.String)", "codegen", "System.ComponentModel.LicenseContext", "SetSavedLicenseKey", True, "benchmark" },
    { 149, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseContext::SetSavedLicenseKey:System.Void(System.Type,System.String)", "codegen", "System.ComponentModel.LicenseContext", "SetSavedLicenseKey", True, "benchmark" },
    { 150, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::CreateWithContext:System.Object(System.Type,System.ComponentModel.LicenseContext)", "codegen", "System.ComponentModel.LicenseManager", "CreateWithContext", True, "benchmark" },
    { 151, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::CreateWithContext:System.Object(System.Type,System.ComponentModel.LicenseContext,System.Object[])", "codegen", "System.ComponentModel.LicenseManager", "CreateWithContext", True, "benchmark" },
    { 152, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::CreateWithContext:System.Object(System.Type,System.ComponentModel.LicenseContext,System.Object[])", "codegen", "System.ComponentModel.LicenseManager", "CreateWithContext", True, "benchmark" },
    { 153, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::CreateWithContext:System.Object(System.Type,System.ComponentModel.LicenseContext,System.Object[])", "codegen", "System.ComponentModel.LicenseManager", "CreateWithContext", True, "benchmark" },
    { 154, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::IsLicensed:System.Boolean(System.Type)", "codegen", "System.ComponentModel.LicenseManager", "IsLicensed", True, "benchmark" },
    { 155, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::IsValid:System.Boolean(System.Type)", "codegen", "System.ComponentModel.LicenseManager", "IsValid", True, "benchmark" },
    { 156, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::IsValid:System.Boolean(System.Type,System.Object,System.ComponentModel.License)", "codegen", "System.ComponentModel.LicenseManager", "IsValid", True, "benchmark" },
    { 157, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::LockContext:System.Void(System.Object)", "codegen", "System.ComponentModel.LicenseManager", "LockContext", True, "benchmark" },
    { 158, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::UnlockContext:System.Void(System.Object)", "codegen", "System.ComponentModel.LicenseManager", "UnlockContext", True, "benchmark" },
    { 159, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::Validate:System.Void(System.Type)", "codegen", "System.ComponentModel.LicenseManager", "Validate", True, "benchmark" },
    { 160, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseManager::Validate:System.ComponentModel.License(System.Type,System.Object)", "codegen", "System.ComponentModel.LicenseManager", "Validate", True, "benchmark" },
    { 161, "System.ComponentModel.TypeConverter/System.ComponentModel.LicenseProvider::GetLicense:System.ComponentModel.License(System.ComponentModel.LicenseContext,System.Type,System.Object,System.Boolean)", "codegen", "System.ComponentModel.LicenseProvider", "GetLicense", True, "benchmark" },
    { 162, "System.ComponentModel.TypeConverter/System.ComponentModel.LicFileLicenseProvider::GetLicense:System.ComponentModel.License(System.ComponentModel.LicenseContext,System.Type,System.Object,System.Boolean)", "codegen", "System.ComponentModel.LicFileLicenseProvider", "GetLicense", True, "benchmark" },
    { 163, "System.ComponentModel.TypeConverter/System.ComponentModel.ListBindableAttribute::IsDefaultAttribute:System.Boolean()", "codegen", "System.ComponentModel.ListBindableAttribute", "IsDefaultAttribute", True, "benchmark" },
    { 164, "System.ComponentModel.TypeConverter/System.ComponentModel.ListSortDescriptionCollection::Contains:System.Boolean(System.Object)", "codegen", "System.ComponentModel.ListSortDescriptionCollection", "Contains", True, "benchmark" },
    { 165, "System.ComponentModel.TypeConverter/System.ComponentModel.ListSortDescriptionCollection::IndexOf:System.Int32(System.Object)", "codegen", "System.ComponentModel.ListSortDescriptionCollection", "IndexOf", True, "benchmark" },
    { 166, "System.ComponentModel.TypeConverter/System.ComponentModel.ListSortDescriptionCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.ComponentModel.ListSortDescriptionCollection", "CopyTo", True, "benchmark" },
    { 167, "System.ComponentModel.TypeConverter/System.ComponentModel.ListSortDescriptionCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.ComponentModel.ListSortDescriptionCollection", "CopyTo", True, "benchmark" },
    { 168, "System.ComponentModel.TypeConverter/System.ComponentModel.MarshalByValueComponent::Dispose:System.Void()", "codegen", "System.ComponentModel.MarshalByValueComponent", "Dispose", True, "benchmark" },
    { 169, "System.ComponentModel.TypeConverter/System.ComponentModel.MarshalByValueComponent::GetService:System.Object(System.Type)", "codegen", "System.ComponentModel.MarshalByValueComponent", "GetService", True, "benchmark" },
    { 170, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Clone:System.Object()", "codegen", "System.ComponentModel.MaskedTextProvider", "Clone", True, "benchmark" },
    { 171, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Add:System.Boolean(System.Char)", "codegen", "System.ComponentModel.MaskedTextProvider", "Add", True, "benchmark" },
    { 172, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Add:System.Boolean(System.Char)", "codegen", "System.ComponentModel.MaskedTextProvider", "Add", True, "benchmark" },
    { 173, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Add:System.Boolean(System.Char,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Add", True, "benchmark" },
    { 174, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Add:System.Boolean(System.Char,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Add", True, "benchmark" },
    { 175, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Add:System.Boolean(System.Char,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Add", True, "benchmark" },
    { 176, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Add:System.Boolean(System.String)", "codegen", "System.ComponentModel.MaskedTextProvider", "Add", True, "benchmark" },
    { 177, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Add:System.Boolean(System.String)", "codegen", "System.ComponentModel.MaskedTextProvider", "Add", True, "benchmark" },
    { 178, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Add:System.Boolean(System.String,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Add", True, "benchmark" },
    { 179, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Add:System.Boolean(System.String,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Add", True, "benchmark" },
    { 180, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Add:System.Boolean(System.String,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Add", True, "benchmark" },
    { 181, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Clear:System.Void()", "codegen", "System.ComponentModel.MaskedTextProvider", "Clear", True, "benchmark" },
    { 182, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Clear:System.Void(System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Clear", True, "benchmark" },
    { 183, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindAssignedEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindAssignedEditPositionFrom", True, "benchmark" },
    { 184, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindAssignedEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindAssignedEditPositionFrom", True, "benchmark" },
    { 185, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindAssignedEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindAssignedEditPositionFrom", True, "benchmark" },
    { 186, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindAssignedEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindAssignedEditPositionInRange", True, "benchmark" },
    { 187, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindAssignedEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindAssignedEditPositionInRange", True, "benchmark" },
    { 188, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindAssignedEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindAssignedEditPositionInRange", True, "benchmark" },
    { 189, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindAssignedEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindAssignedEditPositionInRange", True, "benchmark" },
    { 190, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindEditPositionFrom", True, "benchmark" },
    { 191, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindEditPositionFrom", True, "benchmark" },
    { 192, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindEditPositionFrom", True, "benchmark" },
    { 193, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindEditPositionInRange", True, "benchmark" },
    { 194, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindEditPositionInRange", True, "benchmark" },
    { 195, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindEditPositionInRange", True, "benchmark" },
    { 196, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindEditPositionInRange", True, "benchmark" },
    { 197, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindNonEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindNonEditPositionFrom", True, "benchmark" },
    { 198, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindNonEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindNonEditPositionFrom", True, "benchmark" },
    { 199, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindNonEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindNonEditPositionFrom", True, "benchmark" },
    { 200, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindNonEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindNonEditPositionInRange", True, "benchmark" },
    { 201, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindNonEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindNonEditPositionInRange", True, "benchmark" },
    { 202, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindNonEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindNonEditPositionInRange", True, "benchmark" },
    { 203, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindNonEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindNonEditPositionInRange", True, "benchmark" },
    { 204, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindUnassignedEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindUnassignedEditPositionFrom", True, "benchmark" },
    { 205, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindUnassignedEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindUnassignedEditPositionFrom", True, "benchmark" },
    { 206, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindUnassignedEditPositionFrom:System.Int32(System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindUnassignedEditPositionFrom", True, "benchmark" },
    { 207, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindUnassignedEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindUnassignedEditPositionInRange", True, "benchmark" },
    { 208, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindUnassignedEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindUnassignedEditPositionInRange", True, "benchmark" },
    { 209, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindUnassignedEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindUnassignedEditPositionInRange", True, "benchmark" },
    { 210, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::FindUnassignedEditPositionInRange:System.Int32(System.Int32,System.Int32,System.Boolean)", "codegen", "System.ComponentModel.MaskedTextProvider", "FindUnassignedEditPositionInRange", True, "benchmark" },
    { 211, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::GetOperationResultFromHint:System.Boolean(System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "GetOperationResultFromHint", True, "benchmark" },
    { 212, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.Char,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 213, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.Char,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 214, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.Char,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 215, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.Char,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 216, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.Char,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 217, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.Char,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 218, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.Char,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 219, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.String,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 220, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.String,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 221, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.String,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 222, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.String,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 223, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.String,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 224, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.String,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 225, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::InsertAt:System.Boolean(System.String,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "InsertAt", True, "benchmark" },
    { 226, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::IsAvailablePosition:System.Boolean(System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "IsAvailablePosition", True, "benchmark" },
    { 227, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::IsAvailablePosition:System.Boolean(System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "IsAvailablePosition", True, "benchmark" },
    { 228, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::IsEditPosition:System.Boolean(System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "IsEditPosition", True, "benchmark" },
    { 229, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::IsEditPosition:System.Boolean(System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "IsEditPosition", True, "benchmark" },
    { 230, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::IsValidInputChar:System.Boolean(System.Char)", "codegen", "System.ComponentModel.MaskedTextProvider", "IsValidInputChar", True, "benchmark" },
    { 231, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::IsValidInputChar:System.Boolean(System.Char)", "codegen", "System.ComponentModel.MaskedTextProvider", "IsValidInputChar", True, "benchmark" },
    { 232, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::IsValidMaskChar:System.Boolean(System.Char)", "codegen", "System.ComponentModel.MaskedTextProvider", "IsValidMaskChar", True, "benchmark" },
    { 233, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::IsValidMaskChar:System.Boolean(System.Char)", "codegen", "System.ComponentModel.MaskedTextProvider", "IsValidMaskChar", True, "benchmark" },
    { 234, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::IsValidPasswordChar:System.Boolean(System.Char)", "codegen", "System.ComponentModel.MaskedTextProvider", "IsValidPasswordChar", True, "benchmark" },
    { 235, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::IsValidPasswordChar:System.Boolean(System.Char)", "codegen", "System.ComponentModel.MaskedTextProvider", "IsValidPasswordChar", True, "benchmark" },
    { 236, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Remove:System.Boolean()", "codegen", "System.ComponentModel.MaskedTextProvider", "Remove", True, "benchmark" },
    { 237, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Remove:System.Boolean(System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Remove", True, "benchmark" },
    { 238, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Remove:System.Boolean(System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Remove", True, "benchmark" },
    { 239, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::RemoveAt:System.Boolean(System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "RemoveAt", True, "benchmark" },
    { 240, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::RemoveAt:System.Boolean(System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "RemoveAt", True, "benchmark" },
    { 241, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::RemoveAt:System.Boolean(System.Int32,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "RemoveAt", True, "benchmark" },
    { 242, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::RemoveAt:System.Boolean(System.Int32,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "RemoveAt", True, "benchmark" },
    { 243, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::RemoveAt:System.Boolean(System.Int32,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "RemoveAt", True, "benchmark" },
    { 244, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::RemoveAt:System.Boolean(System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "RemoveAt", True, "benchmark" },
    { 245, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::RemoveAt:System.Boolean(System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "RemoveAt", True, "benchmark" },
    { 246, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::RemoveAt:System.Boolean(System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "RemoveAt", True, "benchmark" },
    { 247, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::RemoveAt:System.Boolean(System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "RemoveAt", True, "benchmark" },
    { 248, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 249, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 250, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 251, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 252, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 253, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 254, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 255, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 256, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 257, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 258, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.Char,System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 259, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 260, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 261, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 262, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 263, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 264, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 265, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 266, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 267, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 268, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 269, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Replace:System.Boolean(System.String,System.Int32,System.Int32,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Replace", True, "benchmark" },
    { 270, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Set:System.Boolean(System.String)", "codegen", "System.ComponentModel.MaskedTextProvider", "Set", True, "benchmark" },
    { 271, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Set:System.Boolean(System.String)", "codegen", "System.ComponentModel.MaskedTextProvider", "Set", True, "benchmark" },
    { 272, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Set:System.Boolean(System.String,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Set", True, "benchmark" },
    { 273, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Set:System.Boolean(System.String,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Set", True, "benchmark" },
    { 274, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::Set:System.Boolean(System.String,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "Set", True, "benchmark" },
    { 275, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::ToDisplayString:System.String()", "codegen", "System.ComponentModel.MaskedTextProvider", "ToDisplayString", True, "benchmark" },
    { 276, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyChar:System.Boolean(System.Char,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyChar", True, "benchmark" },
    { 277, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyChar:System.Boolean(System.Char,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyChar", True, "benchmark" },
    { 278, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyChar:System.Boolean(System.Char,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyChar", True, "benchmark" },
    { 279, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyEscapeChar:System.Boolean(System.Char,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyEscapeChar", True, "benchmark" },
    { 280, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyEscapeChar:System.Boolean(System.Char,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyEscapeChar", True, "benchmark" },
    { 281, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyEscapeChar:System.Boolean(System.Char,System.Int32)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyEscapeChar", True, "benchmark" },
    { 282, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyString:System.Boolean(System.String)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyString", True, "benchmark" },
    { 283, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyString:System.Boolean(System.String)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyString", True, "benchmark" },
    { 284, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyString:System.Boolean(System.String,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyString", True, "benchmark" },
    { 285, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyString:System.Boolean(System.String,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyString", True, "benchmark" },
    { 286, "System.ComponentModel.TypeConverter/System.ComponentModel.MaskedTextProvider::VerifyString:System.Boolean(System.String,System.Int32,System.ComponentModel.MaskedTextResultHint)", "codegen", "System.ComponentModel.MaskedTextProvider", "VerifyString", True, "benchmark" },
    { 287, "System.ComponentModel.TypeConverter/System.ComponentModel.MultilineStringConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.MultilineStringConverter", "ConvertTo", True, "benchmark" },
    { 288, "System.ComponentModel.TypeConverter/System.ComponentModel.MultilineStringConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.MultilineStringConverter", "GetProperties", True, "benchmark" },
    { 289, "System.ComponentModel.TypeConverter/System.ComponentModel.MultilineStringConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.MultilineStringConverter", "GetProperties", True, "benchmark" },
    { 290, "System.ComponentModel.TypeConverter/System.ComponentModel.MultilineStringConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.MultilineStringConverter", "GetProperties", True, "benchmark" },
    { 291, "System.ComponentModel.TypeConverter/System.ComponentModel.MultilineStringConverter::GetPropertiesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.MultilineStringConverter", "GetPropertiesSupported", True, "benchmark" },
    { 292, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.NullableConverter", "CanConvertFrom", True, "benchmark" },
    { 293, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.NullableConverter", "ConvertFrom", True, "benchmark" },
    { 294, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.NullableConverter", "CanConvertTo", True, "benchmark" },
    { 295, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.NullableConverter", "ConvertTo", True, "benchmark" },
    { 296, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::CreateInstance:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Collections.IDictionary)", "codegen", "System.ComponentModel.NullableConverter", "CreateInstance", True, "benchmark" },
    { 297, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::CreateInstance:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Collections.IDictionary)", "codegen", "System.ComponentModel.NullableConverter", "CreateInstance", True, "benchmark" },
    { 298, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::GetCreateInstanceSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.NullableConverter", "GetCreateInstanceSupported", True, "benchmark" },
    { 299, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.NullableConverter", "GetProperties", True, "benchmark" },
    { 300, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.NullableConverter", "GetProperties", True, "benchmark" },
    { 301, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.NullableConverter", "GetProperties", True, "benchmark" },
    { 302, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::GetPropertiesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.NullableConverter", "GetPropertiesSupported", True, "benchmark" },
    { 303, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::GetStandardValues:TypeConverter.StandardValuesCollection(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.NullableConverter", "GetStandardValues", True, "benchmark" },
    { 304, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::GetStandardValuesExclusive:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.NullableConverter", "GetStandardValuesExclusive", True, "benchmark" },
    { 305, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::GetStandardValuesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.NullableConverter", "GetStandardValuesSupported", True, "benchmark" },
    { 306, "System.ComponentModel.TypeConverter/System.ComponentModel.NullableConverter::IsValid:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Object)", "codegen", "System.ComponentModel.NullableConverter", "IsValid", True, "benchmark" },
    { 307, "System.ComponentModel.TypeConverter/System.ComponentModel.PasswordPropertyTextAttribute::IsDefaultAttribute:System.Boolean()", "codegen", "System.ComponentModel.PasswordPropertyTextAttribute", "IsDefaultAttribute", True, "benchmark" },
    { 308, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::AddValueChanged:System.Void(System.Object,System.EventHandler)", "codegen", "System.ComponentModel.PropertyDescriptor", "AddValueChanged", True, "benchmark" },
    { 309, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::AddValueChanged:System.Void(System.Object,System.EventHandler)", "codegen", "System.ComponentModel.PropertyDescriptor", "AddValueChanged", True, "benchmark" },
    { 310, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::CanResetValue:System.Boolean(System.Object)", "codegen", "System.ComponentModel.PropertyDescriptor", "CanResetValue", True, "benchmark" },
    { 311, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::GetChildProperties:System.ComponentModel.PropertyDescriptorCollection()", "codegen", "System.ComponentModel.PropertyDescriptor", "GetChildProperties", True, "benchmark" },
    { 312, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::GetChildProperties:System.ComponentModel.PropertyDescriptorCollection(System.Attribute[])", "codegen", "System.ComponentModel.PropertyDescriptor", "GetChildProperties", True, "benchmark" },
    { 313, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::GetChildProperties:System.ComponentModel.PropertyDescriptorCollection(System.Attribute[])", "codegen", "System.ComponentModel.PropertyDescriptor", "GetChildProperties", True, "benchmark" },
    { 314, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::GetChildProperties:System.ComponentModel.PropertyDescriptorCollection(System.Attribute[])", "codegen", "System.ComponentModel.PropertyDescriptor", "GetChildProperties", True, "benchmark" },
    { 315, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::GetChildProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object)", "codegen", "System.ComponentModel.PropertyDescriptor", "GetChildProperties", True, "benchmark" },
    { 316, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::GetChildProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.PropertyDescriptor", "GetChildProperties", True, "benchmark" },
    { 317, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::GetChildProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.PropertyDescriptor", "GetChildProperties", True, "benchmark" },
    { 318, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::GetChildProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.PropertyDescriptor", "GetChildProperties", True, "benchmark" },
    { 319, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::GetEditor:System.Object(System.Type)", "codegen", "System.ComponentModel.PropertyDescriptor", "GetEditor", True, "benchmark" },
    { 320, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::GetValue:System.Object(System.Object)", "codegen", "System.ComponentModel.PropertyDescriptor", "GetValue", True, "benchmark" },
    { 321, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::RemoveValueChanged:System.Void(System.Object,System.EventHandler)", "codegen", "System.ComponentModel.PropertyDescriptor", "RemoveValueChanged", True, "benchmark" },
    { 322, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::RemoveValueChanged:System.Void(System.Object,System.EventHandler)", "codegen", "System.ComponentModel.PropertyDescriptor", "RemoveValueChanged", True, "benchmark" },
    { 323, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::ResetValue:System.Void(System.Object)", "codegen", "System.ComponentModel.PropertyDescriptor", "ResetValue", True, "benchmark" },
    { 324, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::SetValue:System.Void(System.Object,System.Object)", "codegen", "System.ComponentModel.PropertyDescriptor", "SetValue", True, "benchmark" },
    { 325, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptor::ShouldSerializeValue:System.Boolean(System.Object)", "codegen", "System.ComponentModel.PropertyDescriptor", "ShouldSerializeValue", True, "benchmark" },
    { 326, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Add:System.Int32(System.ComponentModel.PropertyDescriptor)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Add", True, "benchmark" },
    { 327, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Clear:System.Void()", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Clear", True, "benchmark" },
    { 328, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Contains:System.Boolean(System.ComponentModel.PropertyDescriptor)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Contains", True, "benchmark" },
    { 329, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "CopyTo", True, "benchmark" },
    { 330, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "CopyTo", True, "benchmark" },
    { 331, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Find:System.ComponentModel.PropertyDescriptor(System.String,System.Boolean)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Find", True, "benchmark" },
    { 332, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Find:System.ComponentModel.PropertyDescriptor(System.String,System.Boolean)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Find", True, "benchmark" },
    { 333, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Find:System.ComponentModel.PropertyDescriptor(System.String,System.Boolean)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Find", True, "benchmark" },
    { 334, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::IndexOf:System.Int32(System.ComponentModel.PropertyDescriptor)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "IndexOf", True, "benchmark" },
    { 335, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Insert:System.Void(System.Int32,System.ComponentModel.PropertyDescriptor)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Insert", True, "benchmark" },
    { 336, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Insert:System.Void(System.Int32,System.ComponentModel.PropertyDescriptor)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Insert", True, "benchmark" },
    { 337, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Remove:System.Void(System.ComponentModel.PropertyDescriptor)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Remove", True, "benchmark" },
    { 338, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "RemoveAt", True, "benchmark" },
    { 339, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::RemoveAt:System.Void(System.Int32)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "RemoveAt", True, "benchmark" },
    { 340, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Sort:System.ComponentModel.PropertyDescriptorCollection()", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Sort", True, "benchmark" },
    { 341, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Sort:System.ComponentModel.PropertyDescriptorCollection(System.String[])", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Sort", True, "benchmark" },
    { 342, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Sort:System.ComponentModel.PropertyDescriptorCollection(System.String[])", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Sort", True, "benchmark" },
    { 343, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Sort:System.ComponentModel.PropertyDescriptorCollection(System.String[])", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Sort", True, "benchmark" },
    { 344, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Sort:System.ComponentModel.PropertyDescriptorCollection(System.String[],System.Collections.IComparer)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Sort", True, "benchmark" },
    { 345, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Sort:System.ComponentModel.PropertyDescriptorCollection(System.String[],System.Collections.IComparer)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Sort", True, "benchmark" },
    { 346, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Sort:System.ComponentModel.PropertyDescriptorCollection(System.String[],System.Collections.IComparer)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Sort", True, "benchmark" },
    { 347, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Sort:System.ComponentModel.PropertyDescriptorCollection(System.Collections.IComparer)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Sort", True, "benchmark" },
    { 348, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::Sort:System.ComponentModel.PropertyDescriptorCollection(System.Collections.IComparer)", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "Sort", True, "benchmark" },
    { 349, "System.ComponentModel.TypeConverter/System.ComponentModel.PropertyDescriptorCollection::GetEnumerator:System.Collections.IEnumerator()", "codegen", "System.ComponentModel.PropertyDescriptorCollection", "GetEnumerator", True, "benchmark" },
    { 350, "System.ComponentModel.TypeConverter/System.ComponentModel.RecommendedAsConfigurableAttribute::IsDefaultAttribute:System.Boolean()", "codegen", "System.ComponentModel.RecommendedAsConfigurableAttribute", "IsDefaultAttribute", True, "benchmark" },
    { 351, "System.ComponentModel.TypeConverter/System.ComponentModel.ReferenceConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.ReferenceConverter", "CanConvertFrom", True, "benchmark" },
    { 352, "System.ComponentModel.TypeConverter/System.ComponentModel.ReferenceConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.ReferenceConverter", "ConvertFrom", True, "benchmark" },
    { 353, "System.ComponentModel.TypeConverter/System.ComponentModel.ReferenceConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.ReferenceConverter", "ConvertTo", True, "benchmark" },
    { 354, "System.ComponentModel.TypeConverter/System.ComponentModel.ReferenceConverter::GetStandardValues:TypeConverter.StandardValuesCollection(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.ReferenceConverter", "GetStandardValues", True, "benchmark" },
    { 355, "System.ComponentModel.TypeConverter/System.ComponentModel.ReferenceConverter::GetStandardValuesExclusive:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.ReferenceConverter", "GetStandardValuesExclusive", True, "benchmark" },
    { 356, "System.ComponentModel.TypeConverter/System.ComponentModel.ReferenceConverter::GetStandardValuesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.ReferenceConverter", "GetStandardValuesSupported", True, "benchmark" },
    { 357, "System.ComponentModel.TypeConverter/System.ComponentModel.RunInstallerAttribute::IsDefaultAttribute:System.Boolean()", "codegen", "System.ComponentModel.RunInstallerAttribute", "IsDefaultAttribute", True, "benchmark" },
    { 358, "System.ComponentModel.TypeConverter/System.ComponentModel.StringConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.StringConverter", "CanConvertFrom", True, "benchmark" },
    { 359, "System.ComponentModel.TypeConverter/System.ComponentModel.StringConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.StringConverter", "ConvertFrom", True, "benchmark" },
    { 360, "System.ComponentModel.TypeConverter/System.ComponentModel.SyntaxCheck::CheckMachineName:System.Boolean(System.String)", "codegen", "System.ComponentModel.SyntaxCheck", "CheckMachineName", True, "benchmark" },
    { 361, "System.ComponentModel.TypeConverter/System.ComponentModel.SyntaxCheck::CheckMachineName:System.Boolean(System.String)", "codegen", "System.ComponentModel.SyntaxCheck", "CheckMachineName", True, "benchmark" },
    { 362, "System.ComponentModel.TypeConverter/System.ComponentModel.SyntaxCheck::CheckPath:System.Boolean(System.String)", "codegen", "System.ComponentModel.SyntaxCheck", "CheckPath", True, "benchmark" },
    { 363, "System.ComponentModel.TypeConverter/System.ComponentModel.SyntaxCheck::CheckPath:System.Boolean(System.String)", "codegen", "System.ComponentModel.SyntaxCheck", "CheckPath", True, "benchmark" },
    { 364, "System.ComponentModel.TypeConverter/System.ComponentModel.SyntaxCheck::CheckRootedPath:System.Boolean(System.String)", "codegen", "System.ComponentModel.SyntaxCheck", "CheckRootedPath", True, "benchmark" },
    { 365, "System.ComponentModel.TypeConverter/System.ComponentModel.SyntaxCheck::CheckRootedPath:System.Boolean(System.String)", "codegen", "System.ComponentModel.SyntaxCheck", "CheckRootedPath", True, "benchmark" },
    { 366, "System.ComponentModel.TypeConverter/System.ComponentModel.TimeOnlyConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.TimeOnlyConverter", "CanConvertFrom", True, "benchmark" },
    { 367, "System.ComponentModel.TypeConverter/System.ComponentModel.TimeOnlyConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.TimeOnlyConverter", "CanConvertTo", True, "benchmark" },
    { 368, "System.ComponentModel.TypeConverter/System.ComponentModel.TimeOnlyConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.TimeOnlyConverter", "ConvertFrom", True, "benchmark" },
    { 369, "System.ComponentModel.TypeConverter/System.ComponentModel.TimeOnlyConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.TimeOnlyConverter", "ConvertTo", True, "benchmark" },
    { 370, "System.ComponentModel.TypeConverter/System.ComponentModel.TimeSpanConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.TimeSpanConverter", "CanConvertFrom", True, "benchmark" },
    { 371, "System.ComponentModel.TypeConverter/System.ComponentModel.TimeSpanConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.TimeSpanConverter", "CanConvertTo", True, "benchmark" },
    { 372, "System.ComponentModel.TypeConverter/System.ComponentModel.TimeSpanConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.TimeSpanConverter", "ConvertFrom", True, "benchmark" },
    { 373, "System.ComponentModel.TypeConverter/System.ComponentModel.TimeSpanConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.TimeSpanConverter", "ConvertTo", True, "benchmark" },
    { 374, "System.ComponentModel.TypeConverter/System.ComponentModel.ToolboxItemAttribute::IsDefaultAttribute:System.Boolean()", "codegen", "System.ComponentModel.ToolboxItemAttribute", "IsDefaultAttribute", True, "benchmark" },
    { 375, "System.ComponentModel.TypeConverter/System.ComponentModel.ToolboxItemFilterAttribute::Match:System.Boolean(System.Object)", "codegen", "System.ComponentModel.ToolboxItemFilterAttribute", "Match", True, "benchmark" },
    { 376, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::CanConvertFrom:System.Boolean(System.Type)", "codegen", "System.ComponentModel.TypeConverter", "CanConvertFrom", True, "benchmark" },
    { 377, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.TypeConverter", "CanConvertFrom", True, "benchmark" },
    { 378, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::CanConvertTo:System.Boolean(System.Type)", "codegen", "System.ComponentModel.TypeConverter", "CanConvertTo", True, "benchmark" },
    { 379, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.TypeConverter", "CanConvertTo", True, "benchmark" },
    { 380, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFrom:System.Object(System.Object)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFrom", True, "benchmark" },
    { 381, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFrom", True, "benchmark" },
    { 382, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFromInvariantString:System.Object(System.String)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFromInvariantString", True, "benchmark" },
    { 383, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFromInvariantString:System.Object(System.String)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFromInvariantString", True, "benchmark" },
    { 384, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFromInvariantString:System.Object(System.ComponentModel.ITypeDescriptorContext,System.String)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFromInvariantString", True, "benchmark" },
    { 385, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFromInvariantString:System.Object(System.ComponentModel.ITypeDescriptorContext,System.String)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFromInvariantString", True, "benchmark" },
    { 386, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFromString:System.Object(System.String)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFromString", True, "benchmark" },
    { 387, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFromString:System.Object(System.String)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFromString", True, "benchmark" },
    { 388, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFromString:System.Object(System.ComponentModel.ITypeDescriptorContext,System.String)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFromString", True, "benchmark" },
    { 389, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFromString:System.Object(System.ComponentModel.ITypeDescriptorContext,System.String)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFromString", True, "benchmark" },
    { 390, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFromString:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.String)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFromString", True, "benchmark" },
    { 391, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertFromString:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.String)", "codegen", "System.ComponentModel.TypeConverter", "ConvertFromString", True, "benchmark" },
    { 392, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertTo:System.Object(System.Object,System.Type)", "codegen", "System.ComponentModel.TypeConverter", "ConvertTo", True, "benchmark" },
    { 393, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.TypeConverter", "ConvertTo", True, "benchmark" },
    { 394, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertToInvariantString:System.String(System.Object)", "codegen", "System.ComponentModel.TypeConverter", "ConvertToInvariantString", True, "benchmark" },
    { 395, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertToInvariantString:System.String(System.ComponentModel.ITypeDescriptorContext,System.Object)", "codegen", "System.ComponentModel.TypeConverter", "ConvertToInvariantString", True, "benchmark" },
    { 396, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertToString:System.String(System.Object)", "codegen", "System.ComponentModel.TypeConverter", "ConvertToString", True, "benchmark" },
    { 397, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertToString:System.String(System.ComponentModel.ITypeDescriptorContext,System.Object)", "codegen", "System.ComponentModel.TypeConverter", "ConvertToString", True, "benchmark" },
    { 398, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::ConvertToString:System.String(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.TypeConverter", "ConvertToString", True, "benchmark" },
    { 399, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::CreateInstance:System.Object(System.Collections.IDictionary)", "codegen", "System.ComponentModel.TypeConverter", "CreateInstance", True, "benchmark" },
    { 400, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::CreateInstance:System.Object(System.Collections.IDictionary)", "codegen", "System.ComponentModel.TypeConverter", "CreateInstance", True, "benchmark" },
    { 401, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::CreateInstance:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Collections.IDictionary)", "codegen", "System.ComponentModel.TypeConverter", "CreateInstance", True, "benchmark" },
    { 402, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::CreateInstance:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Collections.IDictionary)", "codegen", "System.ComponentModel.TypeConverter", "CreateInstance", True, "benchmark" },
    { 403, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetCreateInstanceSupported:System.Boolean()", "codegen", "System.ComponentModel.TypeConverter", "GetCreateInstanceSupported", True, "benchmark" },
    { 404, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetCreateInstanceSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.TypeConverter", "GetCreateInstanceSupported", True, "benchmark" },
    { 405, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object)", "codegen", "System.ComponentModel.TypeConverter", "GetProperties", True, "benchmark" },
    { 406, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object)", "codegen", "System.ComponentModel.TypeConverter", "GetProperties", True, "benchmark" },
    { 407, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeConverter", "GetProperties", True, "benchmark" },
    { 408, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeConverter", "GetProperties", True, "benchmark" },
    { 409, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.ComponentModel.ITypeDescriptorContext,System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeConverter", "GetProperties", True, "benchmark" },
    { 410, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetPropertiesSupported:System.Boolean()", "codegen", "System.ComponentModel.TypeConverter", "GetPropertiesSupported", True, "benchmark" },
    { 411, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetPropertiesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.TypeConverter", "GetPropertiesSupported", True, "benchmark" },
    { 412, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetStandardValues:System.Collections.ICollection()", "codegen", "System.ComponentModel.TypeConverter", "GetStandardValues", True, "benchmark" },
    { 413, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetStandardValues:TypeConverter.StandardValuesCollection(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.TypeConverter", "GetStandardValues", True, "benchmark" },
    { 414, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetStandardValuesExclusive:System.Boolean()", "codegen", "System.ComponentModel.TypeConverter", "GetStandardValuesExclusive", True, "benchmark" },
    { 415, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetStandardValuesExclusive:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.TypeConverter", "GetStandardValuesExclusive", True, "benchmark" },
    { 416, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetStandardValuesSupported:System.Boolean()", "codegen", "System.ComponentModel.TypeConverter", "GetStandardValuesSupported", True, "benchmark" },
    { 417, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::GetStandardValuesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.TypeConverter", "GetStandardValuesSupported", True, "benchmark" },
    { 418, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::IsValid:System.Boolean(System.Object)", "codegen", "System.ComponentModel.TypeConverter", "IsValid", True, "benchmark" },
    { 419, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeConverter::IsValid:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Object)", "codegen", "System.ComponentModel.TypeConverter", "IsValid", True, "benchmark" },
    { 420, "System.ComponentModel.TypeConverter/TypeConverter.StandardValuesCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "TypeConverter.StandardValuesCollection", "CopyTo", True, "benchmark" },
    { 421, "System.ComponentModel.TypeConverter/TypeConverter.StandardValuesCollection::CopyTo:System.Void(System.Array,System.Int32)", "codegen", "TypeConverter.StandardValuesCollection", "CopyTo", True, "benchmark" },
    { 422, "System.ComponentModel.TypeConverter/TypeConverter.StandardValuesCollection::GetEnumerator:System.Collections.IEnumerator()", "codegen", "TypeConverter.StandardValuesCollection", "GetEnumerator", True, "benchmark" },
    { 423, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::RegisterType:System.Void()", "codegen", "System.ComponentModel.TypeDescriptionProvider", "RegisterType", True, "benchmark" },
    { 424, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::CreateInstance:System.Object(System.IServiceProvider,System.Type,System.Type[],System.Object[])", "codegen", "System.ComponentModel.TypeDescriptionProvider", "CreateInstance", True, "benchmark" },
    { 425, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::CreateInstance:System.Object(System.IServiceProvider,System.Type,System.Type[],System.Object[])", "codegen", "System.ComponentModel.TypeDescriptionProvider", "CreateInstance", True, "benchmark" },
    { 426, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::CreateInstance:System.Object(System.IServiceProvider,System.Type,System.Type[],System.Object[])", "codegen", "System.ComponentModel.TypeDescriptionProvider", "CreateInstance", True, "benchmark" },
    { 427, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetCache:System.Collections.IDictionary(System.Object)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetCache", True, "benchmark" },
    { 428, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetExtendedTypeDescriptor:System.ComponentModel.ICustomTypeDescriptor(System.Object)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetExtendedTypeDescriptor", True, "benchmark" },
    { 429, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetExtendedTypeDescriptorFromRegisteredType:System.ComponentModel.ICustomTypeDescriptor(System.Object)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetExtendedTypeDescriptorFromRegisteredType", True, "benchmark" },
    { 430, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetFullComponentName:System.String(System.Object)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetFullComponentName", True, "benchmark" },
    { 431, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetReflectionType:System.Type(System.Type)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetReflectionType", True, "benchmark" },
    { 432, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetReflectionType:System.Type(System.Object)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetReflectionType", True, "benchmark" },
    { 433, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetReflectionType:System.Type(System.Type,System.Object)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetReflectionType", True, "benchmark" },
    { 434, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetRuntimeType:System.Type(System.Type)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetRuntimeType", True, "benchmark" },
    { 435, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetTypeDescriptor:System.ComponentModel.ICustomTypeDescriptor(System.Type)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetTypeDescriptor", True, "benchmark" },
    { 436, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetTypeDescriptor:System.ComponentModel.ICustomTypeDescriptor(System.Object)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetTypeDescriptor", True, "benchmark" },
    { 437, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetTypeDescriptor:System.ComponentModel.ICustomTypeDescriptor(System.Type,System.Object)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetTypeDescriptor", True, "benchmark" },
    { 438, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetTypeDescriptorFromRegisteredType:System.ComponentModel.ICustomTypeDescriptor(System.Type)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetTypeDescriptorFromRegisteredType", True, "benchmark" },
    { 439, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetTypeDescriptorFromRegisteredType:System.ComponentModel.ICustomTypeDescriptor(System.Object)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetTypeDescriptorFromRegisteredType", True, "benchmark" },
    { 440, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::GetTypeDescriptorFromRegisteredType:System.ComponentModel.ICustomTypeDescriptor(System.Type,System.Object)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "GetTypeDescriptorFromRegisteredType", True, "benchmark" },
    { 441, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::IsRegisteredType:System.Boolean(System.Type)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "IsRegisteredType", True, "benchmark" },
    { 442, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptionProvider::IsSupportedType:System.Boolean(System.Type)", "codegen", "System.ComponentModel.TypeDescriptionProvider", "IsSupportedType", True, "benchmark" },
    { 443, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::RegisterType:System.Void()", "codegen", "System.ComponentModel.TypeDescriptor", "RegisterType", True, "benchmark" },
    { 444, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddAttributes:System.ComponentModel.TypeDescriptionProvider(System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "AddAttributes", True, "benchmark" },
    { 445, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddAttributes:System.ComponentModel.TypeDescriptionProvider(System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "AddAttributes", True, "benchmark" },
    { 446, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddAttributes:System.ComponentModel.TypeDescriptionProvider(System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "AddAttributes", True, "benchmark" },
    { 447, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddAttributes:System.ComponentModel.TypeDescriptionProvider(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "AddAttributes", True, "benchmark" },
    { 448, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddAttributes:System.ComponentModel.TypeDescriptionProvider(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "AddAttributes", True, "benchmark" },
    { 449, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddAttributes:System.ComponentModel.TypeDescriptionProvider(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "AddAttributes", True, "benchmark" },
    { 450, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddEditorTable:System.Void(System.Type,System.Collections.Hashtable)", "codegen", "System.ComponentModel.TypeDescriptor", "AddEditorTable", True, "benchmark" },
    { 451, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddProvider:System.Void(System.ComponentModel.TypeDescriptionProvider,System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "AddProvider", True, "benchmark" },
    { 452, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddProvider:System.Void(System.ComponentModel.TypeDescriptionProvider,System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "AddProvider", True, "benchmark" },
    { 453, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddProviderTransparent:System.Void(System.ComponentModel.TypeDescriptionProvider,System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "AddProviderTransparent", True, "benchmark" },
    { 454, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::AddProviderTransparent:System.Void(System.ComponentModel.TypeDescriptionProvider,System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "AddProviderTransparent", True, "benchmark" },
    { 455, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateAssociation:System.Void(System.Object,System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "CreateAssociation", True, "benchmark" },
    { 456, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateEvent:System.ComponentModel.EventDescriptor(System.Type,System.String,System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateEvent", True, "benchmark" },
    { 457, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateEvent:System.ComponentModel.EventDescriptor(System.Type,System.String,System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateEvent", True, "benchmark" },
    { 458, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateEvent:System.ComponentModel.EventDescriptor(System.Type,System.String,System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateEvent", True, "benchmark" },
    { 459, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateEvent:System.ComponentModel.EventDescriptor(System.Type,System.String,System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateEvent", True, "benchmark" },
    { 460, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateEvent:System.ComponentModel.EventDescriptor(System.Type,System.ComponentModel.EventDescriptor,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateEvent", True, "benchmark" },
    { 461, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateEvent:System.ComponentModel.EventDescriptor(System.Type,System.ComponentModel.EventDescriptor,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateEvent", True, "benchmark" },
    { 462, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateEvent:System.ComponentModel.EventDescriptor(System.Type,System.ComponentModel.EventDescriptor,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateEvent", True, "benchmark" },
    { 463, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateInstance:System.Object(System.IServiceProvider,System.Type,System.Type[],System.Object[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateInstance", True, "benchmark" },
    { 464, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateInstance:System.Object(System.IServiceProvider,System.Type,System.Type[],System.Object[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateInstance", True, "benchmark" },
    { 465, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateInstance:System.Object(System.IServiceProvider,System.Type,System.Type[],System.Object[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateInstance", True, "benchmark" },
    { 466, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateProperty:System.ComponentModel.PropertyDescriptor(System.Type,System.String,System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateProperty", True, "benchmark" },
    { 467, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateProperty:System.ComponentModel.PropertyDescriptor(System.Type,System.String,System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateProperty", True, "benchmark" },
    { 468, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateProperty:System.ComponentModel.PropertyDescriptor(System.Type,System.String,System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateProperty", True, "benchmark" },
    { 469, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateProperty:System.ComponentModel.PropertyDescriptor(System.Type,System.String,System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateProperty", True, "benchmark" },
    { 470, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateProperty:System.ComponentModel.PropertyDescriptor(System.Type,System.ComponentModel.PropertyDescriptor,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateProperty", True, "benchmark" },
    { 471, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateProperty:System.ComponentModel.PropertyDescriptor(System.Type,System.ComponentModel.PropertyDescriptor,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateProperty", True, "benchmark" },
    { 472, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateProperty:System.ComponentModel.PropertyDescriptor(System.Type,System.ComponentModel.PropertyDescriptor,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "CreateProperty", True, "benchmark" },
    { 473, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetAssociation:System.Object(System.Type,System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetAssociation", True, "benchmark" },
    { 474, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetAttributes:System.ComponentModel.AttributeCollection(System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetAttributes", True, "benchmark" },
    { 475, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetAttributes:System.ComponentModel.AttributeCollection(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetAttributes", True, "benchmark" },
    { 476, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetAttributes:System.ComponentModel.AttributeCollection(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetAttributes", True, "benchmark" },
    { 477, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetAttributes:System.ComponentModel.AttributeCollection(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetAttributes", True, "benchmark" },
    { 478, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetClassName:System.String(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetClassName", True, "benchmark" },
    { 479, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetClassName:System.String(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetClassName", True, "benchmark" },
    { 480, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetClassName:System.String(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetClassName", True, "benchmark" },
    { 481, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetClassName:System.String(System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetClassName", True, "benchmark" },
    { 482, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetComponentName:System.String(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetComponentName", True, "benchmark" },
    { 483, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetComponentName:System.String(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetComponentName", True, "benchmark" },
    { 484, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetComponentName:System.String(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetComponentName", True, "benchmark" },
    { 485, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetConverter:System.ComponentModel.TypeConverter(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetConverter", True, "benchmark" },
    { 486, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetConverter:System.ComponentModel.TypeConverter(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetConverter", True, "benchmark" },
    { 487, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetConverter:System.ComponentModel.TypeConverter(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetConverter", True, "benchmark" },
    { 488, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetConverter:System.ComponentModel.TypeConverter(System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetConverter", True, "benchmark" },
    { 489, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetDefaultEvent:System.ComponentModel.EventDescriptor(System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetDefaultEvent", True, "benchmark" },
    { 490, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetDefaultEvent:System.ComponentModel.EventDescriptor(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetDefaultEvent", True, "benchmark" },
    { 491, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetDefaultEvent:System.ComponentModel.EventDescriptor(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetDefaultEvent", True, "benchmark" },
    { 492, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetDefaultEvent:System.ComponentModel.EventDescriptor(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetDefaultEvent", True, "benchmark" },
    { 493, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetDefaultProperty:System.ComponentModel.PropertyDescriptor(System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetDefaultProperty", True, "benchmark" },
    { 494, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetDefaultProperty:System.ComponentModel.PropertyDescriptor(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetDefaultProperty", True, "benchmark" },
    { 495, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetDefaultProperty:System.ComponentModel.PropertyDescriptor(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetDefaultProperty", True, "benchmark" },
    { 496, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetDefaultProperty:System.ComponentModel.PropertyDescriptor(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetDefaultProperty", True, "benchmark" },
    { 497, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEditor:System.Object(System.Object,System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEditor", True, "benchmark" },
    { 498, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEditor:System.Object(System.Object,System.Type,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEditor", True, "benchmark" },
    { 499, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEditor:System.Object(System.Object,System.Type,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEditor", True, "benchmark" },
    { 500, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEditor:System.Object(System.Type,System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEditor", True, "benchmark" },
    { 501, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 502, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 503, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 504, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 505, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 506, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 507, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 508, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 509, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 510, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 511, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Object,System.Attribute[],System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 512, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Object,System.Attribute[],System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 513, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Object,System.Attribute[],System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 514, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetEvents:System.ComponentModel.EventDescriptorCollection(System.Object,System.Attribute[],System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetEvents", True, "benchmark" },
    { 515, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetFullComponentName:System.String(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetFullComponentName", True, "benchmark" },
    { 516, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 517, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 518, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 519, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Type,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 520, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 521, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 522, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 523, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 524, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 525, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Attribute[])", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 526, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Attribute[],System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 527, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Attribute[],System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 528, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Attribute[],System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 529, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProperties:System.ComponentModel.PropertyDescriptorCollection(System.Object,System.Attribute[],System.Boolean)", "codegen", "System.ComponentModel.TypeDescriptor", "GetProperties", True, "benchmark" },
    { 530, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProvider:System.ComponentModel.TypeDescriptionProvider(System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetProvider", True, "benchmark" },
    { 531, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetProvider:System.ComponentModel.TypeDescriptionProvider(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetProvider", True, "benchmark" },
    { 532, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetReflectionType:System.Type(System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "GetReflectionType", True, "benchmark" },
    { 533, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::GetReflectionType:System.Type(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "GetReflectionType", True, "benchmark" },
    { 534, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::Refresh:System.Void(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "Refresh", True, "benchmark" },
    { 535, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::Refresh:System.Void(System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "Refresh", True, "benchmark" },
    { 536, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::Refresh:System.Void(System.Reflection.Module)", "codegen", "System.ComponentModel.TypeDescriptor", "Refresh", True, "benchmark" },
    { 537, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::Refresh:System.Void(System.Reflection.Assembly)", "codegen", "System.ComponentModel.TypeDescriptor", "Refresh", True, "benchmark" },
    { 538, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::CreateDesigner:System.ComponentModel.Design.IDesigner(System.ComponentModel.IComponent,System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "CreateDesigner", True, "benchmark" },
    { 539, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::RemoveAssociation:System.Void(System.Object,System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "RemoveAssociation", True, "benchmark" },
    { 540, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::RemoveAssociations:System.Void(System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "RemoveAssociations", True, "benchmark" },
    { 541, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::RemoveProvider:System.Void(System.ComponentModel.TypeDescriptionProvider,System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "RemoveProvider", True, "benchmark" },
    { 542, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::RemoveProvider:System.Void(System.ComponentModel.TypeDescriptionProvider,System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "RemoveProvider", True, "benchmark" },
    { 543, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::RemoveProviderTransparent:System.Void(System.ComponentModel.TypeDescriptionProvider,System.Type)", "codegen", "System.ComponentModel.TypeDescriptor", "RemoveProviderTransparent", True, "benchmark" },
    { 544, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::RemoveProviderTransparent:System.Void(System.ComponentModel.TypeDescriptionProvider,System.Object)", "codegen", "System.ComponentModel.TypeDescriptor", "RemoveProviderTransparent", True, "benchmark" },
    { 545, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::SortDescriptorArray:System.Void(System.Collections.IList)", "codegen", "System.ComponentModel.TypeDescriptor", "SortDescriptorArray", True, "benchmark" },
    { 546, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeDescriptor::SortDescriptorArray:System.Void(System.Collections.IList)", "codegen", "System.ComponentModel.TypeDescriptor", "SortDescriptorArray", True, "benchmark" },
    { 547, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeListConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.TypeListConverter", "CanConvertFrom", True, "benchmark" },
    { 548, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeListConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.TypeListConverter", "CanConvertTo", True, "benchmark" },
    { 549, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeListConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.TypeListConverter", "ConvertFrom", True, "benchmark" },
    { 550, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeListConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.TypeListConverter", "ConvertTo", True, "benchmark" },
    { 551, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeListConverter::GetStandardValues:TypeConverter.StandardValuesCollection(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.TypeListConverter", "GetStandardValues", True, "benchmark" },
    { 552, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeListConverter::GetStandardValuesExclusive:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.TypeListConverter", "GetStandardValuesExclusive", True, "benchmark" },
    { 553, "System.ComponentModel.TypeConverter/System.ComponentModel.TypeListConverter::GetStandardValuesSupported:System.Boolean(System.ComponentModel.ITypeDescriptorContext)", "codegen", "System.ComponentModel.TypeListConverter", "GetStandardValuesSupported", True, "benchmark" },
    { 554, "System.ComponentModel.TypeConverter/System.ComponentModel.VersionConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.VersionConverter", "CanConvertFrom", True, "benchmark" },
    { 555, "System.ComponentModel.TypeConverter/System.ComponentModel.VersionConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.ComponentModel.VersionConverter", "CanConvertTo", True, "benchmark" },
    { 556, "System.ComponentModel.TypeConverter/System.ComponentModel.VersionConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.ComponentModel.VersionConverter", "ConvertFrom", True, "benchmark" },
    { 557, "System.ComponentModel.TypeConverter/System.ComponentModel.VersionConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.ComponentModel.VersionConverter", "ConvertTo", True, "benchmark" },
    { 558, "System.ComponentModel.TypeConverter/System.ComponentModel.VersionConverter::IsValid:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Object)", "codegen", "System.ComponentModel.VersionConverter", "IsValid", True, "benchmark" },
    { 559, "System.ComponentModel.TypeConverter/System.UriTypeConverter::CanConvertFrom:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.UriTypeConverter", "CanConvertFrom", True, "benchmark" },
    { 560, "System.ComponentModel.TypeConverter/System.UriTypeConverter::CanConvertTo:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Type)", "codegen", "System.UriTypeConverter", "CanConvertTo", True, "benchmark" },
    { 561, "System.ComponentModel.TypeConverter/System.UriTypeConverter::ConvertFrom:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object)", "codegen", "System.UriTypeConverter", "ConvertFrom", True, "benchmark" },
    { 562, "System.ComponentModel.TypeConverter/System.UriTypeConverter::ConvertTo:System.Object(System.ComponentModel.ITypeDescriptorContext,System.Globalization.CultureInfo,System.Object,System.Type)", "codegen", "System.UriTypeConverter", "ConvertTo", True, "benchmark" },
    { 563, "System.ComponentModel.TypeConverter/System.UriTypeConverter::IsValid:System.Boolean(System.ComponentModel.ITypeDescriptorContext,System.Object)", "codegen", "System.UriTypeConverter", "IsValid", True, "benchmark" },

};

static const int kSubjectEntryCount_Generated = 564;

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