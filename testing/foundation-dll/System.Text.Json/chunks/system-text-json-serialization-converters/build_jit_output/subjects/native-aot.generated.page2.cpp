#include <chaos/common.h>
#include <chaos/type_info.h>
#include "runtime_core.h"
#include <chaos/eh.h>
#include "codegen_bridge.h"
#include "module_registry.h"
#include "abi_manifest.h"
#include "hotpatch_table.h"
#include "runtime_vtable.h"
#include "runtime_instantiation.h"
#include "reflection_query_model.h"
#include "load_store_chaos_bridge.h"
#include "interpreter_entry.h"
#include <gc/gc_bgc_inline.h>
#include <gc/gc_card_table.h>
#include <ChaosGeneratedRuntimePrelude.h>
#include "enum_stubs.h"
#include "async_stubs.h"
#include "enum_metadata.generated.h"
#include "native-aot.generated.header.h"

// Forward declaration for dispatch table entries (defined in runtime_stubs.cpp)
extern "C" void InterpreterEntryDirect(
    CHAOS_IL2CPP_UINTPTR method_key,
    void*     args_buf,
    void*     ret_buf) noexcept;

#pragma warning(push)
#pragma warning(disable: 4065 4244)

namespace chaos::il2cpp::codegen::CombinedSubjects {

// Bring runtime_core and jit declarations into scope for unqualified lookup
using namespace chaos::il2cpp::runtime_core;
using namespace chaos::il2cpp::jit;

extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Boolean_System_Boolean_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_System_Byte_System_String(CHAOS_IL2CPP_UINT8 chaos_fn_arg_0, CHAOS_IL2CPP_UINT8 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_array_System_Byte_array_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Char_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Decimal_System_Decimal_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Double_System_Double_System_String(double chaos_fn_arg_0, double chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int16_System_Int16_System_String(CHAOS_IL2CPP_INT16 chaos_fn_arg_0, CHAOS_IL2CPP_INT16 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int32_System_Int32_System_String(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int64_System_Int64_System_String(CHAOS_IL2CPP_INT64 chaos_fn_arg_0, CHAOS_IL2CPP_INT64 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Object_System_Object_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_SByte_System_SByte_System_String(CHAOS_IL2CPP_INT8 chaos_fn_arg_0, CHAOS_IL2CPP_INT8 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Single_System_Single_System_String(float chaos_fn_arg_0, float chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_String_System_String_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt16_System_UInt16_System_String(CHAOS_IL2CPP_UINT16 chaos_fn_arg_0, CHAOS_IL2CPP_UINT16 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt32_System_UInt32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt64_System_UInt64_System_String(CHAOS_IL2CPP_UINT64 chaos_fn_arg_0, CHAOS_IL2CPP_UINT64 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_0_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreNotEqual_0_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" CHAOS_IL2CPP_INT32 Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete(void);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Fail_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT32 Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_get_ExitCode(void);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsFalse_System_Boolean_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNotNull_System_Object_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNull_System_Object_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsTrue_System_Boolean_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_RecordFailure(void);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset(void);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Add_1_int_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Add_1_int_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Add_3_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Clear_4__4(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Contains_5_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_GetEnumerator_10__10(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_GetValues_0__0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_IndexOf_6_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Insert_7_int_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Insert_7_int_JsonNode_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Remove_8_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_RemoveAt_9_int_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_RemoveAt_9_int_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_WriteTo_2_Utf8JsonWriter_JsonSerializerOptions_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Contains_5_JsonNode_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_GetEnumerator_10__10(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_GetValues_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_IndexOf_6_JsonNode_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Remove_8_JsonNode_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_AsArray_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_AsObject_1__1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_AsValue_2__2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_AsArray_0__0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_AsObject_1__1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_AsValue_2__2(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_DeepClone_5__5(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_DeepEquals_9_JsonNode_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetElementIndex_8__8(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetPath_3__3(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetPropertyName_7__7(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetValue_4__4(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetValueKind_6__6(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ReplaceWith_10_int_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ReplaceWith_10_int_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ToJsonString_14_JsonSerializerOptions_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_WriteTo_15_Utf8JsonWriter_JsonSerializerOptions_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_DeepClone_5__5(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_DeepEquals_9_JsonNode_JsonNode_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetElementIndex_8__8(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetPath_3__3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetPropertyName_7__7(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetValue_4__4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetValueKind_6__6(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_ToJsonString_14_JsonSerializerOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Add_2_string_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Add_2_string_JsonNode_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Add_3_KeyValuePairstringJsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Clear_4__4(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_ContainsKey_5_string_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_ContainsKey_5_string_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_GetAt_8_int_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_GetAt_8_int_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_GetEnumerator_7__7(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_IndexOf_11_string_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_IndexOf_11_string_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Insert_12_int_string_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Insert_12_int_string_JsonNode_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Insert_12_int_string_JsonNode_2(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Remove_6_string_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Remove_6_string_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_RemoveAt_13_int_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_RemoveAt_13_int_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_10_int_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_10_int_JsonNode_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_9_int_string_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_9_int_string_JsonNode_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_9_int_string_JsonNode_2(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_TryGetPropertyValue_0_string_JsonNode_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_TryGetPropertyValue_0_string_JsonNode_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_WriteTo_1_Utf8JsonWriter_JsonSerializerOptions_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_ContainsKey_5_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_ContainsKey_5_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_GetAt_8_int_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_GetAt_8_int_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_GetEnumerator_7__7(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_IndexOf_11_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_IndexOf_11_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Remove_6_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Remove_6_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_TryGetPropertyValue_0_string_JsonNode_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_TryGetPropertyValue_0_string_JsonNode_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_Benchmark_TryGetValue_0_int_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_Benchmark_TryGetValue_0_int_1(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_TryGetValue_0_int_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_TryGetValue_0_int_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporterTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporterTests_Benchmark_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporterTests_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterAttributeTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterAttributeTests_Benchmark_CreateConverter_0_System_Type_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterAttributeTests_CreateConverter_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactoryTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactoryTests_Benchmark_CreateConverter_0_System_Type_JsonSerializerOptions_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactoryTests_CreateConverter_0_System_Type_JsonSerializerOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_CanConvert_0_System_Type_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_1(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_CanConvert_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterTests_Benchmark_CanConvert_0_System_Type_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterTests_CanConvert_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_Benchmark_CanConvert_0_System_Type_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_CanConvert_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_CreateConverter_1_System_Type_JsonSerializerOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_Benchmark_CanConvert_0_System_Type_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_CanConvert_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_CreateConverter_1_System_Type_JsonSerializerOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_Benchmark_CanConvert_0_System_Type_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_CanConvert_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_CreateConverter_1_System_Type_JsonSerializerOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceHandlerTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceHandlerTests_Benchmark_CreateResolver_0__0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceHandlerTests_CreateResolver_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_AddReference_0_string_object_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_AddReference_0_string_object_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_GetReference_1_object_bool_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_GetReference_1_object_bool_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_ResolveReference_2_string_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_ResolveReference_2_string_1(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_GetReference_1_object_bool_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_GetReference_1_object_bool_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_ResolveReference_2_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_ResolveReference_2_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);





// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Byte,System.Byte,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_System_Byte_System_String(CHAOS_IL2CPP_UINT8 chaos_fn_arg_0, CHAOS_IL2CPP_UINT8 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Byte[],System.Byte[],System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_array_System_Byte_array_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 6) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	CHAOS_IL2CPP_INTPTR _s6;
	CHAOS_IL2CPP_INTPTR _s7;
	CHAOS_IL2CPP_INTPTR _s8;
	CHAOS_IL2CPP_INTPTR _s9;
	_s0 = chaos_args[0];
	_s1 = 0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = chaos_args[1];
	_s2 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
			_s8 = chaos_args[0];
			_s9 = 0;
			_s8 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s8) == static_cast<CHAOS_IL2CPP_INTPTR>(_s9) ? 1 : 0);
			{
				if (_s8 == 0)
				{
					_s2 = chaos_args[0];
					_s2 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s2); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s2));
					_s3 = chaos_args[1];
					_s3 = [&](){ auto* _c_arr = reinterpret_cast<chaos_managed_array*>(_s3); return _c_arr ? static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_c_arr->length)) : CHAOS_IL2CPP_INTPTR{}; }();
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(_s3));
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
					_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
					_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s2) == static_cast<CHAOS_IL2CPP_INTPTR>(_s3) ? 1 : 0);
					{
						if (_s2 == 0)
						{
							_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							chaos_locals[3] = _s2;
							_s4 = chaos_args[0];
							_s5 = chaos_locals[3];
							{
								const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s5);
								auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s4);
								if (chaos_array == nullptr)
								{
									CHAOS_IL2CPP_FAIL_FAST();
								}
								if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
								{
									CHAOS_IL2CPP_FAIL_FAST();
								}
								const auto chaos_element = *reinterpret_cast<CHAOS_IL2CPP_INT8*>(chaos_array_get_elements(chaos_array) + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index));
								_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element);
							}
							_s5 = chaos_args[1];
							_s6 = chaos_locals[3];
							{
								const auto chaos_index = static_cast<CHAOS_IL2CPP_INT32>(_s6);
								auto* chaos_array = reinterpret_cast<chaos_managed_array*>(_s5);
								if (chaos_array == nullptr)
								{
									CHAOS_IL2CPP_FAIL_FAST();
								}
								if (chaos_index < 0 || static_cast<CHAOS_IL2CPP_INTPTR>(chaos_index) >= chaos_array->length)
								{
									CHAOS_IL2CPP_FAIL_FAST();
								}
								const auto chaos_element = *reinterpret_cast<CHAOS_IL2CPP_INT8*>(chaos_array_get_elements(chaos_array) + static_cast<CHAOS_IL2CPP_SIZE>(chaos_index));
								_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_element);
							}
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
							_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
							_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
							{
								if (_s4 == 0)
								{
								}
							}
						}
						else
						{
						}
					}
				}
				else
				{
				}
			}
		}
		else
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Char,System.Char,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Char_System_Char_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Decimal,System.Decimal,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Decimal_System_Decimal_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[0])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Double,System.Double,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Double_System_Double_System_String(double chaos_fn_arg_0, double chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = ChaosStoreFloat64(chaos_fn_arg_0);
	chaos_args[1] = ChaosStoreFloat64(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Int16,System.Int16,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int16_System_Int16_System_String(CHAOS_IL2CPP_INT16 chaos_fn_arg_0, CHAOS_IL2CPP_INT16 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Int32,System.Int32,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int32_System_Int32_System_String(CHAOS_IL2CPP_INT32 chaos_fn_arg_0, CHAOS_IL2CPP_INT32 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Int64,System.Int64,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int64_System_Int64_System_String(CHAOS_IL2CPP_INT64 chaos_fn_arg_0, CHAOS_IL2CPP_INT64 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = ChaosStoreInt64(chaos_fn_arg_0);
	chaos_args[1] = ChaosStoreInt64(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Object,System.Object,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Object_System_Object_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = ChaosObjectEqualsStatic(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 != 0)
		{
			_s0 = chaos_args[0];
			{
				if (_s0 != 0)
				{
					_s0 = chaos_args[0];
					{
						const auto chaos_arg_0 = _s0;
						const auto chaos_result = ChaosObjectToString(chaos_arg_0);
						_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
				}
				else
				{
					_s0 = 0;
				}
				_s2 = _s1;
				{
					if (_s2 == 0)
					{
						{
							_s1 = CHAOS_IL2CPP_STRING_ID("null");
						}
					}
					chaos_locals[1] = _s1;
					_s1 = chaos_args[1];
					{
						if (_s1 != 0)
						{
						}
						else
						{
						}
					}
				}
			}
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.SByte,System.SByte,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_SByte_System_SByte_System_String(CHAOS_IL2CPP_INT8 chaos_fn_arg_0, CHAOS_IL2CPP_INT8 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Single,System.Single,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Single_System_Single_System_String(float chaos_fn_arg_0, float chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = ChaosStoreFloat32(chaos_fn_arg_0);
	chaos_args[1] = ChaosStoreFloat32(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.String,System.String,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_String_System_String_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.UInt16,System.UInt16,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt16_System_UInt16_System_String(CHAOS_IL2CPP_UINT16 chaos_fn_arg_0, CHAOS_IL2CPP_UINT16 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.UInt32,System.UInt32,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt32_System_UInt32_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.UInt64,System.UInt64,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt64_System_UInt64_System_String(CHAOS_IL2CPP_UINT64 chaos_fn_arg_0, CHAOS_IL2CPP_UINT64 chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = chaos_store_uint64(chaos_fn_arg_0);
	chaos_args[1] = chaos_store_uint64(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual`1(!!0,!!0,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_0_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 15) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	CHAOS_IL2CPP_INTPTR _s6;
	CHAOS_IL2CPP_INTPTR _s7;
	CHAOS_IL2CPP_INTPTR _s8;
	CHAOS_IL2CPP_INTPTR _s9;
	CHAOS_IL2CPP_INTPTR _s10;
	CHAOS_IL2CPP_INTPTR _s11;
	CHAOS_IL2CPP_INTPTR _s12;
	CHAOS_IL2CPP_INTPTR _s13;
	CHAOS_IL2CPP_INTPTR _s14;
	CHAOS_IL2CPP_INTPTR _s15;
	CHAOS_IL2CPP_INTPTR _s16;
	CHAOS_IL2CPP_INTPTR _s17;
	CHAOS_IL2CPP_INTPTR _s18;
	CHAOS_IL2CPP_INTPTR _s19;
	CHAOS_IL2CPP_INTPTR _s20;
	CHAOS_IL2CPP_INTPTR _s21;
	CHAOS_IL2CPP_INTPTR _s22;
	CHAOS_IL2CPP_INTPTR _s23;
	CHAOS_IL2CPP_INTPTR _s24;
	CHAOS_IL2CPP_INTPTR _s25;
	_s0 = chaos_args[0];
	{
		const auto chaos_value = _s0;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_Chaos_TestFramework_Sdk___0, {});
		chaos_boxed->header.type_info = chaos_mt_Chaos_TestFramework_Sdk___0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	// brtrue (structured EH branch)
	_s1 = chaos_args[1];
	{
		const auto chaos_value = _s1;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_Chaos_TestFramework_Sdk___0, {});
		chaos_boxed->header.type_info = chaos_mt_Chaos_TestFramework_Sdk___0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	_s2 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s1) == static_cast<CHAOS_IL2CPP_INTPTR>(_s2) ? 1 : 0);
	// br (handled via structured EH branches)
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[2] = _s2;
	_s2 = chaos_locals[2];
	// brfalse (structured EH branch)
	// br (handled via structured EH branches)
	_s3 = chaos_args[0];
	{
		const auto chaos_value = _s3;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_Chaos_TestFramework_Sdk___0, {});
		chaos_boxed->header.type_info = chaos_mt_Chaos_TestFramework_Sdk___0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	// brfalse (structured EH branch)
	_s4 = chaos_args[1];
	{
		const auto chaos_value = _s4;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_Chaos_TestFramework_Sdk___0, {});
		chaos_boxed->header.type_info = chaos_mt_Chaos_TestFramework_Sdk___0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	_s5 = 0;
	_s4 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s4) == static_cast<CHAOS_IL2CPP_INTPTR>(_s5) ? 1 : 0);
	// br (handled via structured EH branches)
	_s5 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	chaos_locals[3] = _s5;
	_s5 = chaos_locals[3];
	// brfalse (structured EH branch)
	// br (handled via structured EH branches)
	_s6 = chaos_args[0];
	{
		const auto chaos_value = _s6;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_Chaos_TestFramework_Sdk___0, {});
		chaos_boxed->header.type_info = chaos_mt_Chaos_TestFramework_Sdk___0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s6 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		const auto chaos_value = _s6;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot());
		}
		_s6 = chaos_matches ? chaos_value : 0;
	}
	// brtrue (structured EH branch)
	_s7 = chaos_args[1];
	{
		const auto chaos_value = _s7;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_Chaos_TestFramework_Sdk___0, {});
		chaos_boxed->header.type_info = chaos_mt_Chaos_TestFramework_Sdk___0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s7 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		const auto chaos_value = _s7;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot());
		}
		_s7 = chaos_matches ? chaos_value : 0;
	}
	// brtrue (structured EH branch)
	_s8 = chaos_args[0];
	{
		const auto chaos_value = _s8;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_Chaos_TestFramework_Sdk___0, {});
		chaos_boxed->header.type_info = chaos_mt_Chaos_TestFramework_Sdk___0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s8 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		const auto chaos_value = _s8;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Collections_IEnumerable.AsTypeInfoHot());
		}
		_s8 = chaos_matches ? chaos_value : 0;
	}
	chaos_locals[0] = _s8;
	_s8 = chaos_locals[0];
	// brfalse (structured EH branch)
	_s9 = chaos_args[1];
	{
		const auto chaos_value = _s9;
		auto* chaos_boxed = CHAOS_IL2CPP_NEW_GC(chaos_boxed_type_Chaos_TestFramework_Sdk___0, {});
		chaos_boxed->header.type_info = chaos_mt_Chaos_TestFramework_Sdk___0.AsTypeInfoHot();
		chaos_boxed->value = chaos_value;
		_s9 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_boxed);
	}
	{
		const auto chaos_value = _s9;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_Collections_IEnumerable.AsTypeInfoHot());
		}
		_s9 = chaos_matches ? chaos_value : 0;
	}
	chaos_locals[1] = _s9;
	_s9 = chaos_locals[1];
	_s10 = 0;
	_s9 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s9) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s10) ? 1 : 0);
	// br (handled via structured EH branches)
	_s10 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[4] = _s10;
	_s10 = chaos_locals[4];
	// brfalse (structured EH branch)
	_s11 = chaos_locals[0];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
		_s12 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[5] = _s12;
	_s12 = chaos_locals[1];
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
		_s13 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[6] = _s13;
	_s13 = chaos_locals[5];
	{
		const auto chaos_value = _s13;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_IDisposable.AsTypeInfoHot());
		}
		_s13 = chaos_matches ? chaos_value : 0;
	}
	chaos_locals[7] = _s13;
	_s13 = chaos_locals[6];
	{
		const auto chaos_value = _s13;
		auto chaos_matches = false;
		if (chaos_value != 0)
		{
			auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
			chaos_matches = chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_mt_System_Private_CoreLib_System_IDisposable.AsTypeInfoHot());
		}
		_s13 = chaos_matches ? chaos_value : 0;
	}
	chaos_locals[8] = _s13;
		auto _chaos_finally = [&]()
		{
			_s13 = chaos_locals[8];
			{
				if (_s13 != 0)
				{
					_s13 = chaos_locals[8];
					{
						reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[13])();
					}
				}
			}
		};
	CHAOS_EH_TRY_FINALLY
				auto _chaos_finally = [&]()
				{
					_s13 = chaos_locals[7];
					{
						if (_s13 != 0)
						{
							_s13 = chaos_locals[7];
							{
								reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[13])();
							}
						}
					}
				};
			CHAOS_EH_TRY_FINALLY
					_s13 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
					chaos_locals[9] = _s13;
					// br (handled via structured EH branches)
					_s13 = chaos_locals[6];
					{
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
						_s14 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
					_s14 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s14) == static_cast<CHAOS_IL2CPP_INTPTR>(_s15) ? 1 : 0);
					chaos_locals[10] = _s14;
					_s14 = chaos_locals[10];
					// brfalse (structured EH branch)
					// leave (handled via structured EH branches)
					{
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
						_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s16 = chaos_locals[5];
					{
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[11])();
						_s17 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s18 = chaos_locals[6];
					{
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[11])();
						_s19 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					{
						const auto chaos_arg_1 = _s19;
						const auto chaos_arg_0 = _s18;
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_arg_0, chaos_arg_1);
						_s18 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s19 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
					_s18 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s18) == static_cast<CHAOS_IL2CPP_INTPTR>(_s19) ? 1 : 0);
					chaos_locals[11] = _s18;
					_s18 = chaos_locals[11];
					// brfalse (structured EH branch)
					// leave (handled via structured EH branches)
					_s19 = chaos_locals[9];
					_s20 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
					_s19 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INT32>(static_cast<CHAOS_IL2CPP_UINT32>(_s19) + static_cast<CHAOS_IL2CPP_UINT32>(_s20)));
					chaos_locals[9] = _s19;
					_s19 = chaos_locals[5];
					{
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
						_s20 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					chaos_locals[12] = _s20;
					_s20 = chaos_locals[12];
					// brtrue (structured EH branch)
					_s21 = chaos_locals[6];
					{
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
						_s22 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					chaos_locals[13] = _s22;
					_s22 = chaos_locals[13];
					// brfalse (structured EH branch)
			CHAOS_EH_FINALLY_END
	CHAOS_EH_FINALLY_END
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
		_s23 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s24 = chaos_args[0];
	_s25 = chaos_args[1];
	{
		const auto chaos_arg_1 = _s25;
		const auto chaos_arg_0 = _s24;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0, chaos_arg_1);
		_s24 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s25 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s24 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s24) == static_cast<CHAOS_IL2CPP_INTPTR>(_s25) ? 1 : 0);
	{
		if (_s24 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreNotEqual`1(!!0,!!0,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreNotEqual_0_0_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_args[0];
	_s2 = chaos_args[1];
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s1 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Complete()
extern "C" CHAOS_IL2CPP_INT32 Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__s_exitCode;
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto chaos_value = _s0;
		chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__s_exitCode = chaos_value;
	}
	_s0 = chaos_locals[0];
	chaos_locals[1] = _s0;
	_s0 = chaos_locals[1];
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Fail(System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Fail_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto chaos_value = _s0;
		chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__s_exitCode = chaos_value;
	}
	{
		const auto chaos_result = ChaosConsoleGetError();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		_s1 = CHAOS_IL2CPP_STRING_ID("[ASSERT FAIL] ");
	}
	_s2 = chaos_args[0];
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		ChaosTextWriterWriteLineStr(chaos_arg_0, chaos_arg_1);
	}
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_1 = _s0;
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException, {});
		chaos_object->header.type_info = chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException.AsTypeInfoHot();
		Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object), chaos_arg_1);
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	CHAOS_EH_THROW(_s0);
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::get_ExitCode()
extern "C" CHAOS_IL2CPP_INT32 Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_get_ExitCode(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__s_exitCode;
	return static_cast<CHAOS_IL2CPP_INT32>(_s0);
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsFalse(System.Boolean,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsFalse_System_Boolean_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNotNull(System.Object,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNotNull_System_Object_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = 0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNull(System.Object,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNull_System_Object_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = 0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_UINTPTR>(_s0) > static_cast<CHAOS_IL2CPP_UINTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsTrue(System.Boolean,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsTrue_System_Boolean_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(static_cast<CHAOS_IL2CPP_INTPTR>(_s0) == static_cast<CHAOS_IL2CPP_INTPTR>(_s1) ? 1 : 0);
	{
		if (_s0 == 0)
		{
		}
		return;
	}
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::RecordFailure()
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_RecordFailure(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	{
		auto chaos_value = _s0;
		chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__s_exitCode = chaos_value;
	}
	return;
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Reset()
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto chaos_value = _s0;
		chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__s_exitCode = chaos_value;
	}
	return;
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws(System.Action)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_EH_TRY
			_s0 = chaos_args[0];
			{
				const auto chaos_delegate_value = _s0;
				if (chaos_delegate_value == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				auto* chaos_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Action*>(chaos_delegate_value);
				if (chaos_delegate->chaos_delegate_invocation_count > 0)
				{
					const auto* chaos_invocation_list = reinterpret_cast<const CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR)*>(chaos_delegate->chaos_delegate_invocation_list);
					if (chaos_invocation_list == nullptr ||
						static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)
					{
						CHAOS_IL2CPP_FAIL_FAST();
					}

					for (CHAOS_IL2CPP_SIZE chaos_delegate_index = 0; chaos_delegate_index < chaos_invocation_list->size(); ++chaos_delegate_index)
					{
						const auto chaos_invocation_delegate_value = (*chaos_invocation_list)[chaos_delegate_index];
						if (chaos_invocation_delegate_value == 0)
						{
							CHAOS_IL2CPP_FAIL_FAST();
						}
						auto* chaos_invocation_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Action*>(chaos_invocation_delegate_value);
						if (chaos_invocation_delegate->chaos_delegate_method_ptr == 0)
						{
							CHAOS_IL2CPP_FAIL_FAST();
						}
						if (chaos_invocation_delegate->chaos_delegate_target == 0)
						{
							const auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							const auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target);
						}
					}
				}
				else
				{
					if (chaos_delegate->chaos_delegate_method_ptr == 0)
					{
						CHAOS_IL2CPP_FAIL_FAST();
					}
					bool __chaos_hotpatch_taken = false;
					uint64_t __chaos_args_buf[1] = {0};
					uint64_t __chaos_ret_buf[2] = {};
					if (chaos_delegate->chaos_delegate_method_token != 0)
					{
						__chaos_hotpatch_taken = ::chaos::il2cpp::runtime_core::DelegateHotpatchCheckpoint(
							chaos_delegate->chaos_delegate_method_token,
							__chaos_args_buf, __chaos_ret_buf, 0);
					}

					if (__chaos_hotpatch_taken)
					{
					}
					else
					{
						if (chaos_delegate->chaos_delegate_target == 0)
						{
							const auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							const auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
							chaos_closed_function(chaos_delegate->chaos_delegate_target);
						}
					}
				}
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
			CHAOS_EH_RETHROW;
	CHAOS_EH_END
	CHAOS_EH_TRY
			_s0 = chaos_args[0];
			{
				const auto chaos_delegate_value = _s0;
				if (chaos_delegate_value == 0)
				{
					::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
				}
				auto* chaos_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Action*>(chaos_delegate_value);
				if (chaos_delegate->chaos_delegate_invocation_count > 0)
				{
					const auto* chaos_invocation_list = reinterpret_cast<const CHAOS_IL2CPP_VECTOR(CHAOS_IL2CPP_INTPTR)*>(chaos_delegate->chaos_delegate_invocation_list);
					if (chaos_invocation_list == nullptr ||
						static_cast<CHAOS_IL2CPP_INTPTR>(chaos_invocation_list->size()) != chaos_delegate->chaos_delegate_invocation_count)
					{
						CHAOS_IL2CPP_FAIL_FAST();
					}

					for (CHAOS_IL2CPP_SIZE chaos_delegate_index = 0; chaos_delegate_index < chaos_invocation_list->size(); ++chaos_delegate_index)
					{
						const auto chaos_invocation_delegate_value = (*chaos_invocation_list)[chaos_delegate_index];
						if (chaos_invocation_delegate_value == 0)
						{
							CHAOS_IL2CPP_FAIL_FAST();
						}
						auto* chaos_invocation_delegate = reinterpret_cast<chaos_type_System_Private_CoreLib_System_Action*>(chaos_invocation_delegate_value);
						if (chaos_invocation_delegate->chaos_delegate_method_ptr == 0)
						{
							CHAOS_IL2CPP_FAIL_FAST();
						}
						if (chaos_invocation_delegate->chaos_delegate_target == 0)
						{
							const auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							const auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_closed_function(chaos_invocation_delegate->chaos_delegate_target);
						}
					}
				}
				else
				{
					if (chaos_delegate->chaos_delegate_method_ptr == 0)
					{
						CHAOS_IL2CPP_FAIL_FAST();
					}
					bool __chaos_hotpatch_taken = false;
					uint64_t __chaos_args_buf[1] = {0};
					uint64_t __chaos_ret_buf[2] = {};
					if (chaos_delegate->chaos_delegate_method_token != 0)
					{
						__chaos_hotpatch_taken = ::chaos::il2cpp::runtime_core::DelegateHotpatchCheckpoint(
							chaos_delegate->chaos_delegate_method_token,
							__chaos_args_buf, __chaos_ret_buf, 0);
					}

					if (__chaos_hotpatch_taken)
					{
					}
					else
					{
						if (chaos_delegate->chaos_delegate_target == 0)
						{
							const auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							const auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
							chaos_closed_function(chaos_delegate->chaos_delegate_target);
						}
					}
				}
			}
	CHAOS_EH_CATCH_BEGIN
		if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, chaos_mt_System_Private_CoreLib_System_Object.AsTypeInfoHot()))
		{ CHAOS_EH_RETHROW; }
		_s0 = CHAOS_EH_EXCEPTION_OBJ;
	CHAOS_EH_END
	return;
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException::.ctor(System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = chaos_args[0];
	_s1 = chaos_args[1];
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		ChaosReflectionSetExceptionMetadata(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute::.ctor()
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[22])();
	}
	return;
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute::.ctor()
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[22])();
	}
	return;
}

// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute::.ctor()
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[22])();
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Add_1_int_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Add_1_int_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[24])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_3_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Add_3_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Clear_4__4()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Clear_4__4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = 0;
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[26])();
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Contains_5_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Contains_5_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[27])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetEnumerator_10__10()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_GetEnumerator_10__10(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[28])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetValues_0__0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_GetValues_0__0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[29])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_IndexOf_6_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_IndexOf_6_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[30])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Insert_7_int_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[31])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Insert_7_int_JsonNode_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[31])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Remove_8_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Remove_8_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[32])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_RemoveAt_9_int_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[33])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_RemoveAt_9_int_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[33])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_WriteTo_2_Utf8JsonWriter_JsonSerializerOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_WriteTo_2_Utf8JsonWriter_JsonSerializerOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[34])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Contains_5_JsonNode_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Contains_5_JsonNode_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetEnumerator_10__10()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_GetEnumerator_10__10(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetValues_0__0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_GetValues_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::IndexOf_6_JsonNode_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_IndexOf_6_JsonNode_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Remove_8_JsonNode_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Remove_8_JsonNode_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsArray_0__0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_AsArray_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsObject_1__1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_AsObject_1__1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsValue_2__2()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_AsValue_2__2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsArray_0__0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_AsArray_0__0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[35])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsObject_1__1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_AsObject_1__1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[36])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsValue_2__2()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_AsValue_2__2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[37])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepClone_5__5()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_DeepClone_5__5(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[38])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepEquals_9_JsonNode_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_DeepEquals_9_JsonNode_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[39])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetElementIndex_8__8()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetElementIndex_8__8(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[40])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPath_3__3()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetPath_3__3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[41])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPropertyName_7__7()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetPropertyName_7__7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[42])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValue_4__4()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetValue_4__4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[43])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValueKind_6__6()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetValueKind_6__6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[44])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[0];
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_address = _s2;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s2 = chaos_locals[1];
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[47])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[0];
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_address = _s2;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s2 = chaos_locals[1];
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[47])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[0];
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_address = _s2;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s2 = chaos_locals[1];
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = chaos_static_System_Private_CoreLib_System_IO_Stream__Null;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[0];
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_address = _s2;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s2 = chaos_locals[1];
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[48])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[0];
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_address = _s2;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s2 = chaos_locals[1];
	_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
	{
		const auto chaos_address = _s3;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s3 = chaos_locals[2];
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[51])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = chaos_static_System_Private_CoreLib_System_IO_Stream__Null;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[0];
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		const auto chaos_address = _s2;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s2 = chaos_locals[1];
	_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[2]);
	{
		const auto chaos_address = _s3;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s3 = chaos_locals[2];
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[51])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ReplaceWith_10_int_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ReplaceWith_10_int_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ToJsonString_14_JsonSerializerOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ToJsonString_14_JsonSerializerOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[53])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_WriteTo_15_Utf8JsonWriter_JsonSerializerOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_WriteTo_15_Utf8JsonWriter_JsonSerializerOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[34])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepClone_5__5()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_DeepClone_5__5(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepEquals_9_JsonNode_JsonNode_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_DeepEquals_9_JsonNode_JsonNode_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[39])(chaos_arg_0, chaos_arg_1);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (_s0 != 0)
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
			_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
		}
		else
		{
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
			_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(ChaosLoadInt64(_s0)));
		}
		chaos_locals[1] = ChaosStoreInt64(_i0);
		_i0 = ChaosLoadInt64(chaos_locals[1]);
		return _i0;
	}
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetElementIndex_8__8()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetElementIndex_8__8(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPath_3__3()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetPath_3__3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPropertyName_7__7()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetPropertyName_7__7(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValue_4__4()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetValue_4__4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValueKind_6__6()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetValueKind_6__6(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ToJsonString_14_JsonSerializerOptions_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_ToJsonString_14_JsonSerializerOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Add_2_string_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[54])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Add_2_string_JsonNode_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[54])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_3_KeyValuePairstringJsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Add_3_KeyValuePairstringJsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s1;
		if ((chaos_address & chaos_managed_pointer_local_slot_tag) != 0)
		{
			auto* chaos_slot = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(static_cast<CHAOS_IL2CPP_UINTPTR>(chaos_address & ~chaos_managed_pointer_local_slot_tag));
			*chaos_slot = 0;
		}
		else
		{
			*reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(chaos_address) = 0;
		}
	}
	_s1 = chaos_locals[0];
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[56])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Clear_4__4()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Clear_4__4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = 0;
	{
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[57])();
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_ContainsKey_5_string_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[58])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_ContainsKey_5_string_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[58])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_GetAt_8_int_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[59])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_GetAt_8_int_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[59])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetEnumerator_7__7()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_GetEnumerator_7__7(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[60])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_IndexOf_11_string_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[61])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_IndexOf_11_string_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[61])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Insert_12_int_string_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = 0;
	_s3 = 0;
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[62])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Insert_12_int_string_JsonNode_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = 0;
	_s3 = 0;
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[62])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_2()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Insert_12_int_string_JsonNode_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	_s3 = 0;
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[62])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Remove_6_string_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[63])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Remove_6_string_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[63])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_RemoveAt_13_int_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[64])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_RemoveAt_13_int_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[64])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_10_int_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[65])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_10_int_JsonNode_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[65])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_9_int_string_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = 0;
	_s3 = 0;
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[66])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_9_int_string_JsonNode_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s2 = 0;
	_s3 = 0;
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[66])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_2()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_9_int_string_JsonNode_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	_s3 = 0;
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[66])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_TryGetPropertyValue_0_string_JsonNode_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	chaos_locals[0] = _s0;
	_s0 = 0;
	_s1 = 0;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[67])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_TryGetPropertyValue_0_string_JsonNode_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	chaos_locals[0] = _s0;
	_s0 = 0;
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[67])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_WriteTo_1_Utf8JsonWriter_JsonSerializerOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_WriteTo_1_Utf8JsonWriter_JsonSerializerOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[34])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_ContainsKey_5_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_ContainsKey_5_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_GetAt_8_int_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_GetAt_8_int_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetEnumerator_7__7()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_GetEnumerator_7__7(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_IndexOf_11_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_IndexOf_11_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Remove_6_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Remove_6_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_TryGetPropertyValue_0_string_JsonNode_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = 0;
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[1] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[1]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_TryGetPropertyValue_0_string_JsonNode_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = 0;
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[1] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[1]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_Benchmark_TryGetValue_0_int_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = 0;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[68])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_Benchmark_TryGetValue_0_int_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = 0;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[68])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_TryGetValue_0_int_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[1] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[1]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_TryGetValue_0_int_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[1] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[1]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporterTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::Benchmark_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporterTests_Benchmark_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[69])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporterTests_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterAttributeTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::Benchmark_CreateConverter_0_System_Type_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterAttributeTests_Benchmark_CreateConverter_0_System_Type_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[70])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::CreateConverter_0_System_Type_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterAttributeTests_CreateConverter_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactoryTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::Benchmark_CreateConverter_0_System_Type_JsonSerializerOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactoryTests_Benchmark_CreateConverter_0_System_Type_JsonSerializerOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[71])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::CreateConverter_0_System_Type_JsonSerializerOptions_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactoryTests_CreateConverter_0_System_Type_JsonSerializerOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_CanConvert_0_System_Type_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_CanConvert_0_System_Type_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[72])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = 0;
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[73])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = 0;
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[73])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = 0;
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[74])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = 0;
	{
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[74])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::CanConvert_0_System_Type_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_CanConvert_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::Benchmark_CanConvert_0_System_Type_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterTests_Benchmark_CanConvert_0_System_Type_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[72])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::CanConvert_0_System_Type_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterTests_CanConvert_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_Benchmark_CanConvert_0_System_Type_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[72])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[71])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_CanConvert_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_CreateConverter_1_System_Type_JsonSerializerOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_Benchmark_CanConvert_0_System_Type_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[72])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[71])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_CanConvert_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_CreateConverter_1_System_Type_JsonSerializerOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CanConvert_0_System_Type_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_Benchmark_CanConvert_0_System_Type_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[72])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[71])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CanConvert_0_System_Type_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_CanConvert_0_System_Type_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CreateConverter_1_System_Type_JsonSerializerOptions_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_CreateConverter_1_System_Type_JsonSerializerOptions_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceHandlerTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::Benchmark_CreateResolver_0__0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceHandlerTests_Benchmark_CreateResolver_0__0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[75])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::CreateResolver_0__0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceHandlerTests_CreateResolver_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		ChaosObjectCtor(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_AddReference_0_string_object_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[76])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_AddReference_0_string_object_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	_s2 = 0;
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[76])(chaos_arg_0, chaos_arg_1);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_GetReference_1_object_bool_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = 0;
	_s1 = 0;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[77])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_GetReference_1_object_bool_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = 0;
	_s1 = 0;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[77])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_ResolveReference_2_string_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[78])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_ResolveReference_2_string_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String);
		chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
		chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(0);
		chaos_string->utf8_data = "";
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
	}
	{
		const auto chaos_arg_0 = _s1;
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[78])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_GetReference_1_object_bool_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[1] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[1]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_GetReference_1_object_bool_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[1] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[1]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_ResolveReference_2_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_ResolveReference_2_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[0] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[0]);
	return _i0;
}



}  // namespace chaos::il2cpp::codegen::CombinedSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 182;

#ifdef CHAOS_IL2CPP_JIT_MODE

// --- JIT Method Entry Table ---
// Auto-generated by chaos-il2cpp codegen for --mode jit.
#include <cstdint>
#include "jit_registration.h"

static const char kJitJson_0[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Boolean,System.Boolean,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Boolean,System.Boolean,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Boolean,System.Boolean,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Boolean,System.Boolean,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Boolean,System.Boolean,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Boolean_System_Boolean_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":2},{\"carrierKindCode\":2,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Boolean\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Boolean\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_1[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Byte,System.Byte,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Byte,System.Byte,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Byte,System.Byte,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Byte,System.Byte,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Byte,System.Byte,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_System_Byte_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":5,\"typeShape\":2},{\"carrierKindCode\":5,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Byte\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Byte\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_2[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Byte[],System.Byte[],System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Byte[],System.Byte[],System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Byte[],System.Byte[],System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Byte[],System.Byte[],System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Byte[],System.Byte[],System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_array_System_Byte_array_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":6,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Byte[]\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":5,\"resultType\":\"System.Byte[]\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":6,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":15,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":21,\"ilOffset\":16,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":86,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Byte[]\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":23,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":26,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":31,\"ilOffset\":27,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":86,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":31,\"resultType\":\"System.Byte[]\",\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":32,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":33,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":34,\"resultType\":\"System.Byte[]\",\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":35,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":36,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":37,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":39,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":40,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":43,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":49,\"ilOffset\":44,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":86,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":50,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":74,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":53,\"resultType\":\"System.Byte[]\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":54,\"isPreserveSig\":false},{\"op\":\"ldelem\",\"opCode\":44,\"operand\":\"System.Byte\",\"ilOffset\":55,\"resultType\":\"System.Byte\",\"runtimeServiceKind\":12,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":56,\"resultType\":\"System.Byte[]\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":57,\"isPreserveSig\":false},{\"op\":\"ldelem\",\"opCode\":44,\"operand\":\"System.Byte\",\"ilOffset\":58,\"resultType\":\"System.Byte\",\"runtimeServiceKind\":12,\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":59,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":61,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":62,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":64,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":66,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":70,\"ilOffset\":68,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":70,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":71,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":72,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":73,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":74,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":75,\"resultType\":\"System.Byte[]\",\"isPreserveSig\":false},{\"op\":\"ldlen\",\"opCode\":46,\"ilOffset\":76,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"conv.i4\",\"opCode\":39,\"ilOffset\":77,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"clt\",\"opCode\":32,\"ilOffset\":78,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":5,\"ilOffset\":80,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":82,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":53,\"ilOffset\":84,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":86,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_3[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Char,System.Char,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Char,System.Char,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Char,System.Char,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Char,System.Char,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Char,System.Char,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Char_System_Char_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":2},{\"carrierKindCode\":2,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Char\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Char\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_4[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Decimal,System.Decimal,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Decimal,System.Decimal,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Decimal,System.Decimal,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Decimal,System.Decimal,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Decimal,System.Decimal,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Decimal_System_Decimal_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Decimal\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Decimal\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Decimal::op_Inequality:System.Boolean(System.Decimal,System.Decimal)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Decimal::op_Inequality:System.Boolean(System.Decimal,System.Decimal)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Decimal::op_Inequality:System.Boolean(System.Decimal,System.Decimal)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_5[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Double,System.Double,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Double,System.Double,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Double,System.Double,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Double,System.Double,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Double,System.Double,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Double_System_Double_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":9,\"typeShape\":2},{\"carrierKindCode\":9,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Double\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_6[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int16,System.Int16,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Int16,System.Int16,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int16,System.Int16,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int16,System.Int16,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Int16,System.Int16,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int16_System_Int16_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":6,\"typeShape\":2},{\"carrierKindCode\":6,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int16\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Int16\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_7[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int32,System.Int32,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Int32,System.Int32,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int32,System.Int32,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int32,System.Int32,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Int32,System.Int32,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int32_System_Int32_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":1,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_8[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int64,System.Int64,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Int64,System.Int64,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int64,System.Int64,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int64,System.Int64,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Int64,System.Int64,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int64_System_Int64_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":10,\"typeShape\":2},{\"carrierKindCode\":10,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_9[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Object,System.Object,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Object,System.Object,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Object,System.Object,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Object,System.Object,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Object,System.Object,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Object_System_Object_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":9,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":11,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":61,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":16,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":22,\"ilOffset\":17,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":19,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":28,\"ilOffset\":20,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":22,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Object::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":37,\"ilOffset\":29,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":31,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"null\",\"ilOffset\":32,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":37,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":38,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":44,\"ilOffset\":39,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":41,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":50,\"ilOffset\":42,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":44,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":45,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.Object::ToString:System.String()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::ToString:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::ToString:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"dup\",\"opCode\":54,\"ilOffset\":50,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":59,\"ilOffset\":51,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":53,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"null\",\"ilOffset\":54,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":59,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":61,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_10[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.SByte,System.SByte,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.SByte,System.SByte,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.SByte,System.SByte,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.SByte,System.SByte,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.SByte,System.SByte,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_SByte_System_SByte_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":4,\"typeShape\":2},{\"carrierKindCode\":4,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.SByte\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.SByte\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_11[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Single,System.Single,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Single,System.Single,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Single,System.Single,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Single,System.Single,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.Single,System.Single,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Single_System_Single_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":8,\"typeShape\":2},{\"carrierKindCode\":8,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Single\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.Single\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_12[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.String,System.String,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.String,System.String,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.String,System.String,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.String,System.String,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.String,System.String,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_String_System_String_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_13[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt16,System.UInt16,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.UInt16,System.UInt16,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt16,System.UInt16,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt16,System.UInt16,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.UInt16,System.UInt16,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt16_System_UInt16_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":7,\"typeShape\":2},{\"carrierKindCode\":7,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.UInt16\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.UInt16\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_14[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt32,System.UInt32,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.UInt32,System.UInt32,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt32,System.UInt32,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt32,System.UInt32,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.UInt32,System.UInt32,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt32_System_UInt32_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":2},{\"carrierKindCode\":2,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.UInt32\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.UInt32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_15[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt64,System.UInt64,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.UInt64,System.UInt64,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt64,System.UInt64,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt64,System.UInt64,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(System.UInt64,System.UInt64,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt64_System_UInt64_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":11,\"typeShape\":2},{\"carrierKindCode\":11,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.UInt64\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":2,\"resultType\":\"System.UInt64\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":6,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":12,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_16[] = 
    "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual\\u00601:System.Void(!!0,!!0,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(!!0,!!0,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual\\u00601:System.Void(!!0,!!0,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual\\u00601:System.Void(!!0,!!0,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreEqual(!!0,!!0,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_0_0_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":15,\"exceptionRegionCount\":2,\"exceptionRegions\":[{\"handlingKindCode\":2,\"tryOffset\":164,\"tryLength\":100,\"handlerOffset\":264,\"handlerLength\":13},{\"handlingKindCode\":2,\"tryOffset\":155,\"tryLength\":124,\"handlerOffset\":279,\"handlerLength\":13}],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"Chaos.TestFramework.Sdk/!!0\",\"ilOffset\":2,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"type\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":20,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":9,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"Chaos.TestFramework.Sdk/!!0\",\"ilOffset\":10,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"type\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":15,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":21,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":20,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":2,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":22,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":30,\"ilOffset\":23,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":315,\"ilOffset\":25,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":30,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"Chaos.TestFramework.Sdk/!!0\",\"ilOffset\":31,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"type\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":49,\"ilOffset\":36,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":38,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"Chaos.TestFramework.Sdk/!!0\",\"ilOffset\":39,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"type\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":44,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":45,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":50,\"ilOffset\":47,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":49,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":3,\"ilOffset\":50,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":3,\"ilOffset\":51,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":60,\"ilOffset\":52,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":315,\"ilOffset\":55,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":60,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"Chaos.TestFramework.Sdk/!!0\",\"ilOffset\":61,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"type\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"isinst\",\"opCode\":38,\"operand\":\"System.Private.CoreLib/System.String\",\"ilOffset\":66,\"resultType\":\"System.Private.CoreLib/System.String\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.String\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":8,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":119,\"ilOffset\":71,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":73,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"Chaos.TestFramework.Sdk/!!0\",\"ilOffset\":74,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"type\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"isinst\",\"opCode\":38,\"operand\":\"System.Private.CoreLib/System.String\",\"ilOffset\":79,\"resultType\":\"System.Private.CoreLib/System.String\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.String\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":8,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":119,\"ilOffset\":84,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":86,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"Chaos.TestFramework.Sdk/!!0\",\"ilOffset\":87,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"type\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"isinst\",\"opCode\":38,\"operand\":\"System.Private.CoreLib/System.Collections.IEnumerable\",\"ilOffset\":92,\"resultType\":\"System.Private.CoreLib/System.Collections.IEnumerable\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerable\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerable\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":8,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":97,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":98,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":119,\"ilOffset\":99,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":101,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"box\",\"opCode\":35,\"operand\":\"Chaos.TestFramework.Sdk/!!0\",\"ilOffset\":102,\"resultType\":\"System.Object\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"type\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/!!0\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":9,\"isPreserveSig\":false},{\"op\":\"isinst\",\"opCode\":38,\"operand\":\"System.Private.CoreLib/System.Collections.IEnumerable\",\"ilOffset\":107,\"resultType\":\"System.Private.CoreLib/System.Collections.IEnumerable\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerable\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerable\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":8,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":112,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":113,\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":114,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":115,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":120,\"ilOffset\":117,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":119,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":4,\"ilOffset\":120,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":4,\"ilOffset\":122,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":294,\"ilOffset\":124,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":130,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":131,\"resultType\":\"System.Collections.IEnumerator\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerable::GetEnumerator:System.Collections.IEnumerator()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerable::GetEnumerator:System.Collections.IEnumerator()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerable::GetEnumerator:System.Collections.IEnumerator()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":5,\"ilOffset\":136,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":138,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":139,\"resultType\":\"System.Collections.IEnumerator\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerable::GetEnumerator:System.Collections.IEnumerator()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerable::GetEnumerator:System.Collections.IEnumerator()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerable::GetEnumerator:System.Collections.IEnumerator()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":6,\"ilOffset\":144,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":146,\"isPreserveSig\":false},{\"op\":\"isinst\",\"opCode\":38,\"operand\":\"System.Private.CoreLib/System.IDisposable\",\"ilOffset\":148,\"resultType\":\"System.Private.CoreLib/System.IDisposable\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.IDisposable\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IDisposable\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":8,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":7,\"ilOffset\":153,\"resultType\":\"System.Void\",\"isPreserveSig"
    "\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":6,\"ilOffset\":155,\"isPreserveSig\":false},{\"op\":\"isinst\",\"opCode\":38,\"operand\":\"System.Private.CoreLib/System.IDisposable\",\"ilOffset\":157,\"resultType\":\"System.Private.CoreLib/System.IDisposable\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.IDisposable\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IDisposable\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":8,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":8,\"ilOffset\":162,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":165,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":9,\"ilOffset\":166,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":233,\"ilOffset\":168,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":6,\"ilOffset\":171,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":173,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":178,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":179,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":10,\"ilOffset\":181,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":10,\"ilOffset\":183,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":190,\"ilOffset\":185,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":315,\"ilOffset\":188,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":190,\"resultType\":\"System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E()\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[System.Object];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[System.Object];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Object\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[System.Object];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[System.Object];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E()\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E::get_Default()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"typeArguments\":[\"System.Object\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":195,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":197,\"resultType\":\"System.Object\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::get_Current:System.Object()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::get_Current:System.Object()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::get_Current:System.Object()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":6,\"ilOffset\":202,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":204,\"resultType\":\"System.Object\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::get_Current:System.Object()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::get_Current:System.Object()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::get_Current:System.Object()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":209,\"resultType\":\"System.Boolean\",\"callee\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E::Equals:System.Boolean(System.Object,System.Object)\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E::Equals:System.Boolean(System.Object,System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E::Equals:System.Boolean(System.Object,System.Object)\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[System.Object];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[System.Object];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"typeArguments\":[\"System.Object\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[System.Object];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[System.Object];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E::Equals:System.Boolean(System.Object,System.Object)\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003CSystem.Object\\u003E::Equals(System.Object,System.Object)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"typeArguments\":[\"System.Object\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":214,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":215,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":11,\"ilOffset\":217,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":11,\"ilOffset\":219,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":226,\"ilOffset\":221,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":315,\"ilOffset\":224,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":9,\"ilOffset\":226,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":228,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"add\",\"opCode\":25,\"ilOffset\":229,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":9,\"ilOffset\":230,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":5,\"ilOffset\":233,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":235,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":12,\"ilOffset\":240,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":12,\"ilOffset\":242,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":170,\"ilOffset\":244,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":6,\"ilOffset\":246,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":248,\"resultType\":\"System.Boolean\",\"callee\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":13,\"ilOffset\":253,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":13,\"ilOffset\":255,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":261,\"ilOffset\":257,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":277,\"ilOffset\":262,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":8,\"ilOffset\":264,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":276,\"ilOffset\":266,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":8,\"ilOffset\":268,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":270,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.IDisposable::Dispose:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IDisposable::Dispose:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IDisposable::Dispose:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"endfinally\",\"opCode\":51,\"ilOffset\":276,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":292,\"ilOffset\":277,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":7,\"ilOffset\":279,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":291,\"ilOffset\":281,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":7,\"ilOffset\":283,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":285,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.IDisposable::Dispose:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IDisposable::Dispose:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"su"
    "bjectId\":\"System.Private.CoreLib/System.IDisposable::Dispose:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"endfinally\",\"opCode\":51,\"ilOffset\":291,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":315,\"ilOffset\":292,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":294,\"resultType\":\"System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E()\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[!!0];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[!!0];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"typeArguments\":[\"!!0\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[!!0];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[!!0];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E()\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::get_Default()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"typeArguments\":[\"!!0\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":299,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":300,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":301,\"resultType\":\"System.Boolean\",\"callee\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::Equals:System.Boolean(!!0,!!0)\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::Equals:System.Boolean(!!0,!!0)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::Equals:System.Boolean(!!0,!!0)\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[!!0];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[!!0];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"typeArguments\":[\"!!0\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[!!0];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[!!0];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::Equals:System.Boolean(!!0,!!0)\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::Equals(!!0,!!0)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"typeArguments\":[\"!!0\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":306,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":307,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":14,\"ilOffset\":309,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":14,\"ilOffset\":311,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":315,\"ilOffset\":313,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":315,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}"
    ;
static const char kJitJson_17[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-not-equal\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreNotEqual\\u00601:System.Void(!!0,!!0,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreNotEqual(!!0,!!0,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreNotEqual\\u00601:System.Void(!!0,!!0,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreNotEqual\\u00601:System.Void(!!0,!!0,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.are-not-equal\",\"signature\":\"System.Void Chaos.TestFramework.Assert::AreNotEqual(!!0,!!0,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreNotEqual_0_0_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":3,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E\",\"callee\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E()\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E()\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[!!0];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[!!0];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"typeArguments\":[\"!!0\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[!!0];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E();type=[!!0];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::get_Default:System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E()\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::get_Default()\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::get_Default:System.Collections.Generic.EqualityComparer\\u003C!0\\u003E()\",\"typeArguments\":[\"!!0\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":6,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":7,\"resultType\":\"!!0\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Boolean\",\"callee\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::Equals:System.Boolean(!!0,!!0)\",\"reference\":{\"assemblyName\":\"System.Collections\",\"subjectKind\":\"method\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::Equals:System.Boolean(!!0,!!0)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Collections\",\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::Equals:System.Boolean(!!0,!!0)\",\"openDefinitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[!!0];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[!!0];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"typeArguments\":[\"!!0\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[!!0];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0);type=[!!0];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":2,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::Equals:System.Boolean(!!0,!!0)\",\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"displaySubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u003C!!0\\u003E::Equals(!!0,!!0)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Collections/System.Collections.Generic.EqualityComparer\\u00601::Equals:System.Boolean(!0,!0)\",\"typeArguments\":[\"!!0\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":13,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":17,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_18[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.complete\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Complete:System.Int32()\",\"signature\":\"System.Int32 Chaos.TestFramework.Assert::Complete()\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Complete:System.Int32()\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Complete:System.Int32()\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.complete\",\"signature\":\"System.Int32 Chaos.TestFramework.Assert::Complete()\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"ilOffset\":1,\"resultType\":\"System.Int32\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"field\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"ilOffset\":8,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"field\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":13,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":17,\"ilOffset\":15,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":17,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":18,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_19[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.fail\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Fail:System.Void(System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::Fail(System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Fail:System.Void(System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Fail:System.Void(System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.fail\",\"signature\":\"System.Void Chaos.TestFramework.Assert::Fail(System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Fail_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"ilOffset\":2,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"field\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":7,\"resultType\":\"System.IO.TextWriter\",\"callee\":\"System.Console/System.Console::get_Error:System.IO.TextWriter()\",\"reference\":{\"assemblyName\":\"System.Console\",\"subjectKind\":\"method\",\"subjectId\":\"System.Console/System.Console::get_Error:System.IO.TextWriter()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Console\",\"subjectId\":\"System.Console/System.Console::get_Error:System.IO.TextWriter()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"[ASSERT FAIL] \",\"ilOffset\":12,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":17,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":18,\"resultType\":\"System.String\",\"callee\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":23,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":29,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"newobj\",\"opCode\":34,\"ilOffset\":30,\"resultType\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException\",\"callee\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException::.ctor:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"method\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException::.ctor:System.Void(System.String)\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"baseTypeSubjectId\":\"System.Private.CoreLib/System.Exception\",\"implementedInterfaceSubjectIds\":[],\"declaringTypeShape\":0},\"runtimeServiceKind\":1,\"isPreserveSig\":false},{\"op\":\"throw\",\"opCode\":48,\"ilOffset\":35,\"resultType\":\"System.Void\",\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_20[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.get-exit-code\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::get_ExitCode:System.Int32()\",\"signature\":\"System.Int32 Chaos.TestFramework.Assert::get_ExitCode()\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::get_ExitCode:System.Int32()\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::get_ExitCode:System.Int32()\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.get-exit-code\",\"signature\":\"System.Int32 Chaos.TestFramework.Assert::get_ExitCode()\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_get_ExitCode\",\"isStatic\":true,\"returnType\":\"System.Int32\",\"returnAbi\":{\"carrierKindCode\":1,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"ilOffset\":0,\"resultType\":\"System.Int32\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"field\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":5,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_21[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.is-false\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsFalse:System.Void(System.Boolean,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::IsFalse(System.Boolean,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsFalse:System.Void(System.Boolean,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsFalse:System.Void(System.Boolean,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.is-false\",\"signature\":\"System.Void Chaos.TestFramework.Assert::IsFalse(System.Boolean,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsFalse_System_Boolean_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Boolean\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":3,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":6,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_22[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.is-not-null\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNotNull:System.Void(System.Object,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::IsNotNull(System.Object,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNotNull:System.Void(System.Object,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNotNull:System.Void(System.Object,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.is-not-null\",\"signature\":\"System.Void Chaos.TestFramework.Assert::IsNotNull(System.Object,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNotNull_System_Object_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":6,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":9,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_23[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.is-null\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNull:System.Void(System.Object,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::IsNull(System.Object,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNull:System.Void(System.Object,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNull:System.Void(System.Object,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.is-null\",\"signature\":\"System.Void Chaos.TestFramework.Assert::IsNull(System.Object,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNull_System_Object_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"cgt.un\",\"opCode\":33,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":6,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":9,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_24[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.is-true\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsTrue:System.Void(System.Boolean,System.String)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::IsTrue(System.Boolean,System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsTrue:System.Void(System.Boolean,System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsTrue:System.Void(System.Boolean,System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.is-true\",\"signature\":\"System.Void Chaos.TestFramework.Assert::IsTrue(System.Boolean,System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsTrue_System_Boolean_System_String\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":2,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":2},{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Boolean\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ceq\",\"opCode\":31,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":6,\"isPreserveSig\":false},{\"op\":\"brfalse\",\"opCode\":19,\"operand\":9,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_25[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.record-failure\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::RecordFailure:System.Void()\",\"signature\":\"System.Void Chaos.TestFramework.Assert::RecordFailure()\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::RecordFailure:System.Void()\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::RecordFailure:System.Void()\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.record-failure\",\"signature\":\"System.Void Chaos.TestFramework.Assert::RecordFailure()\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_RecordFailure\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"ilOffset\":1,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"field\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_26[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.reset\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Reset:System.Void()\",\"signature\":\"System.Void Chaos.TestFramework.Assert::Reset()\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Reset:System.Void()\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Reset:System.Void()\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.reset\",\"signature\":\"System.Void Chaos.TestFramework.Assert::Reset()\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":0,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stsfld\",\"opCode\":13,\"operand\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"ilOffset\":1,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectKind\":\"field\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"declaringTypeShape\":1,\"fieldTypeSubjectId\":\"System.Int32\"},\"runtimeServiceKind\":5,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":6,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_27[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.throws\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::Throws(System.Action)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.throws\",\"signature\":\"System.Void Chaos.TestFramework.Assert::Throws(System.Action)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1}],\"localCount\":0,\"exceptionRegionCount\":2,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":11,\"handlerOffset\":12,\"handlerLength\":4,\"catchTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException\"},{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":11,\"handlerOffset\":16,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Object\"}],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Action\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Action::Invoke:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Action::Invoke:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Action::Invoke:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":21,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"rethrow\",\"opCode\":49,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":21,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":21,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_28[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.throws\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws\\u00601:System.Void(System.Action)\",\"signature\":\"System.Void Chaos.TestFramework.Assert::Throws(System.Action)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws\\u00601:System.Void(System.Action)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws\\u00601:System.Void(System.Action)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assert.throws\",\"signature\":\"System.Void Chaos.TestFramework.Assert::Throws(System.Action)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeShape\":1}],\"localCount\":1,\"exceptionRegionCount\":3,\"exceptionRegions\":[{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":11,\"handlerOffset\":12,\"handlerLength\":4,\"catchTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException\"},{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":11,\"handlerOffset\":16,\"handlerLength\":5,\"catchTypeSubjectId\":\"Chaos.TestFramework.Sdk/!!0\"},{\"handlingKindCode\":1,\"tryOffset\":1,\"tryLength\":11,\"handlerOffset\":21,\"handlerLength\":5,\"catchTypeSubjectId\":\"System.Private.CoreLib/System.Exception\"}],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Action\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Action::Invoke:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Action::Invoke:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Action::Invoke:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":26,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"rethrow\",\"opCode\":49,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":26,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":21,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"leave\",\"opCode\":50,\"operand\":26,\"ilOffset\":24,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_29[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assertion-exception.ctor\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException::.ctor:System.Void(System.String)\",\"signature\":\"System.Void Chaos.TestFramework.AssertionException::.ctor(System.String)\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException::.ctor:System.Void(System.String)\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException::.ctor:System.Void(System.String)\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-assertion-exception.ctor\",\"signature\":\"System.Void Chaos.TestFramework.AssertionException::.ctor(System.String)\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":1,\"parameterAbis\":[{\"carrierKindCode\":2,\"typeSubjectId\":\"System.Private.CoreLib/System.String\",\"typeShape\":1}],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException\",\"isPreserveSig\":false},{\"op\":\"ldarg\",\"opCode\":6,\"operand\":1,\"ilOffset\":1,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":2,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_30[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-benchmark-attribute.ctor\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute::.ctor:System.Void()\",\"signature\":\"System.Void Chaos.TestFramework.BenchmarkAttribute::.ctor()\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute::.ctor:System.Void()\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute::.ctor:System.Void()\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-benchmark-attribute.ctor\",\"signature\":\"System.Void Chaos.TestFramework.BenchmarkAttribute::.ctor()\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Attribute::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Attribute::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Attribute::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_31[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-fact-attribute.ctor\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute::.ctor:System.Void()\",\"signature\":\"System.Void Chaos.TestFramework.FactAttribute::.ctor()\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute::.ctor:System.Void()\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute::.ctor:System.Void()\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-fact-attribute.ctor\",\"signature\":\"System.Void Chaos.TestFramework.FactAttribute::.ctor()\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Attribute::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Attribute::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Attribute::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_32[] = "{\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-hot-update-attribute.ctor\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute::.ctor:System.Void()\",\"signature\":\"System.Void Chaos.TestFramework.HotUpdateAttribute::.ctor()\",\"identity\":{\"assemblyName\":\"Chaos.TestFramework.Sdk\",\"declaringTypeSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute\",\"definitionSubjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute::.ctor:System.Void()\",\"subjectId\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute::.ctor:System.Void()\",\"methodId\":\"chaos-test-framework-sdk.chaos-test-framework-hot-update-attribute.ctor\",\"signature\":\"System.Void Chaos.TestFramework.HotUpdateAttribute::.ctor()\"},\"nativeSymbol\":\"Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Attribute::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Attribute::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Attribute::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"Chaos.TestFramework.Sdk\"}";
static const char kJitJson_33[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_34[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-add-1-int-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-add-1-int-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Add_1_int_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0)\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add\\u003CSystem.Int32\\u003E(System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_35[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-add-1-int-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-add-1-int-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_1_int_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Add_1_int_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0)\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add\\u003CSystem.Int32\\u003E(System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_36[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-add-3-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_3_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_3_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_3_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_3_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-add-3-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Add_3_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Add_3_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Add:System.Void(System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_37[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-clear-4-4\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Clear_4__4:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Clear_4__4()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Clear_4__4:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Clear_4__4:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-clear-4-4\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Clear_4__4()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Clear_4__4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Clear:System.Void()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Clear:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Clear:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_38[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-contains-5-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Contains_5_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Contains_5_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Contains_5_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Contains_5_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-contains-5-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Contains_5_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Contains_5_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Contains:System.Boolean(System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Contains:System.Boolean(System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Contains:System.Boolean(System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_39[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-get-enumerator-10-10\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetEnumerator_10__10:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetEnumerator_10__10()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetEnumerator_10__10:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetEnumerator_10__10:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-get-enumerator-10-10\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetEnumerator_10__10()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_GetEnumerator_10__10\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_40[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-get-values-0-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetValues_0__0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetValues_0__0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetValues_0__0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetValues_0__0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-get-values-0-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_GetValues_0__0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_GetValues_0__0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E()\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E()\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E();type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues\\u003CSystem.Int32\\u003E:System.Collections.Generic.IEnumerable\\u003CSystem.Int32\\u003E()\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E()\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues\\u003CSystem.Int32\\u003E()\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::GetValues:System.Collections.Generic.IEnumerable\\u003C!!0\\u003E()\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_41[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-index-of-6-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_IndexOf_6_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_IndexOf_6_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_IndexOf_6_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_IndexOf_6_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-index-of-6-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_IndexOf_6_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_IndexOf_6_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::IndexOf:System.Int32(System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::IndexOf:System.Int32(System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::IndexOf:System.Int32(System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_42[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-insert-7-int-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-insert-7-int-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Insert_7_int_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Insert:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Insert:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Insert:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_43[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-insert-7-int-json-node-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-insert-7-int-json-node-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Insert_7_int_JsonNode_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Insert_7_int_JsonNode_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Insert:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Insert:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Insert:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_44[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-remove-8-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Remove_8_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Remove_8_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Remove_8_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Remove_8_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-remove-8-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_Remove_8_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_Remove_8_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Remove:System.Boolean(System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Remove:System.Boolean(System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::Remove:System.Boolean(System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_45[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-remove-at-9-int-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-remove-at-9-int-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_RemoveAt_9_int_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAt:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAt:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAt:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_46[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-remove-at-9-int-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-remove-at-9-int-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_RemoveAt_9_int_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_RemoveAt_9_int_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAt:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAt:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonArray::RemoveAt:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_47[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-write-to-2-utf8-json-writer-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_WriteTo_2_Utf8JsonWriter_JsonSerializerOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_WriteTo_2_Utf8JsonWriter_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_WriteTo_2_Utf8JsonWriter_JsonSerializerOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_WriteTo_2_Utf8JsonWriter_JsonSerializerOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.benchmark-write-to-2-utf8-json-writer-json-serializer-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Benchmark_WriteTo_2_Utf8JsonWriter_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Benchmark_WriteTo_2_Utf8JsonWriter_JsonSerializerOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(Utf8JsonWriter,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(Utf8JsonWriter,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(Utf8JsonWriter,JsonSerializerOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_48[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.contains-5-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Contains_5_JsonNode_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Contains_5_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Contains_5_JsonNode_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Contains_5_JsonNode_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.contains-5-json-node-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Contains_5_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Contains_5_JsonNode_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_49[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.get-enumerator-10-10\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetEnumerator_10__10:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetEnumerator_10__10()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetEnumerator_10__10:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetEnumerator_10__10:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.get-enumerator-10-10\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetEnumerator_10__10()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_GetEnumerator_10__10\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_50[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.get-values-0-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetValues_0__0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetValues_0__0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetValues_0__0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetValues_0__0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.get-values-0-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::GetValues_0__0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_GetValues_0__0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_51[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.index-of-6-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::IndexOf_6_JsonNode_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::IndexOf_6_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::IndexOf_6_JsonNode_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::IndexOf_6_JsonNode_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.index-of-6-json-node-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::IndexOf_6_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_IndexOf_6_JsonNode_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_52[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.remove-8-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Remove_8_JsonNode_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Remove_8_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Remove_8_JsonNode_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Remove_8_JsonNode_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-array-tests.remove-8-json-node-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonArrayTests::Remove_8_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonArrayTests_Remove_8_JsonNode_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_53[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_54[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.as-array-0-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsArray_0__0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsArray_0__0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsArray_0__0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsArray_0__0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.as-array-0-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsArray_0__0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_AsArray_0__0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_55[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.as-object-1-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsObject_1__1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsObject_1__1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsObject_1__1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsObject_1__1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.as-object-1-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsObject_1__1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_AsObject_1__1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_56[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.as-value-2-2\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsValue_2__2:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsValue_2__2()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsValue_2__2:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsValue_2__2:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.as-value-2-2\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::AsValue_2__2()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_AsValue_2__2\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_57[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-as-array-0-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsArray_0__0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsArray_0__0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsArray_0__0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsArray_0__0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-as-array-0-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsArray_0__0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_AsArray_0__0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Text.Json.Nodes.JsonArray\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::AsArray:System.Text.Json.Nodes.JsonArray()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::AsArray:System.Text.Json.Nodes.JsonArray()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::AsArray:System.Text.Json.Nodes.JsonArray()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_58[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-as-object-1-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsObject_1__1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsObject_1__1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsObject_1__1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsObject_1__1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-as-object-1-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsObject_1__1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_AsObject_1__1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Text.Json.Nodes.JsonObject\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::AsObject:System.Text.Json.Nodes.JsonObject()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::AsObject:System.Text.Json.Nodes.JsonObject()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::AsObject:System.Text.Json.Nodes.JsonObject()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_59[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-as-value-2-2\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsValue_2__2:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsValue_2__2()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsValue_2__2:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsValue_2__2:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-as-value-2-2\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_AsValue_2__2()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_AsValue_2__2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Text.Json.Nodes.JsonValue\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::AsValue:System.Text.Json.Nodes.JsonValue()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::AsValue:System.Text.Json.Nodes.JsonValue()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::AsValue:System.Text.Json.Nodes.JsonValue()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_60[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-deep-clone-5-5\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepClone_5__5:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepClone_5__5()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepClone_5__5:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepClone_5__5:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-deep-clone-5-5\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepClone_5__5()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_DeepClone_5__5\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Text.Json.Nodes.JsonNode\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepClone:System.Text.Json.Nodes.JsonNode()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepClone:System.Text.Json.Nodes.JsonNode()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepClone:System.Text.Json.Nodes.JsonNode()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_61[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-deep-equals-9-json-node-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepEquals_9_JsonNode_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepEquals_9_JsonNode_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepEquals_9_JsonNode_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepEquals_9_JsonNode_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-deep-equals-9-json-node-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_DeepEquals_9_JsonNode_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_DeepEquals_9_JsonNode_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepEquals:System.Boolean(System.Text.Json.Nodes.JsonNode,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepEquals:System.Boolean(System.Text.Json.Nodes.JsonNode,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepEquals:System.Boolean(System.Text.Json.Nodes.JsonNode,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_62[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-get-element-index-8-8\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetElementIndex_8__8:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetElementIndex_8__8()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetElementIndex_8__8:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetElementIndex_8__8:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-get-element-index-8-8\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetElementIndex_8__8()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetElementIndex_8__8\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetElementIndex:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetElementIndex:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetElementIndex:System.Int32()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_63[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-get-path-3-3\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPath_3__3:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPath_3__3()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPath_3__3:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPath_3__3:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-get-path-3-3\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPath_3__3()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetPath_3__3\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.String\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPath:System.String()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPath:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPath:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_64[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-get-property-name-7-7\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPropertyName_7__7:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPropertyName_7__7()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPropertyName_7__7:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPropertyName_7__7:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-get-property-name-7-7\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetPropertyName_7__7()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetPropertyName_7__7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.String\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPropertyName:System.String()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPropertyName:System.String()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetPropertyName:System.String()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_65[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-get-value-4-4\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValue_4__4:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValue_4__4()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValue_4__4:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValue_4__4:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-get-value-4-4\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValue_4__4()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetValue_4__4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue\\u003CSystem.Int32\\u003E:System.Int32()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue\\u003CSystem.Int32\\u003E:System.Int32()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue\\u003CSystem.Int32\\u003E:System.Int32()\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue:!!0()\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue:!!0();type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue:!!0();type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue:!!0()\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue:!!0();type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue:!!0();type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue\\u003CSystem.Int32\\u003E:System.Int32()\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue:!!0()\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue\\u003CSystem.Int32\\u003E()\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValue:!!0()\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_66[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-get-value-kind-6-6\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValueKind_6__6:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValueKind_6__6()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValueKind_6__6:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValueKind_6__6:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-get-value-kind-6-6\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_GetValueKind_6__6()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_GetValueKind_6__6\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"JsonValueKind\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValueKind:JsonValueKind()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValueKind:JsonValueKind()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::GetValueKind:JsonValueKind()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_67[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-11-string-system-nullable-json-node-options-json-document-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-11-string-system-nullable-json-node-options-json-document-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Text.Json/JsonDocumentOptions\",\"ilOffset\":13,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"type\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Text.Json.Nodes.JsonNode\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.String,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.String,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.String,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_68[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-11-string-system-nullable-json-node-options-json-document-options-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-11-string-system-nullable-json-node-options-json-document-options-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"\",\"ilOffset\":1,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"ilOffset\":8,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Text.Json/JsonDocumentOptions\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"type\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":24,\"resultType\":\"System.Text.Json.Nodes.JsonNode\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.String,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.String,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.String,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":30,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_69[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-12-stream-system-nullable-json-node-options-json-document-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-12-stream-system-nullable-json-node-options-json-document-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Text.Json/JsonDocumentOptions\",\"ilOffset\":13,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"type\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":20,\"resultType\":\"System.Text.Json.Nodes.JsonNode\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":25,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":26,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_70[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-12-stream-system-nullable-json-node-options-json-document-options-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-12-stream-system-nullable-json-node-options-json-document-options-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"System.Private.CoreLib/System.IO.Stream::Null\",\"ilOffset\":1,\"resultType\":\"System.IO.Stream\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"field\",\"subjectId\":\"System.Private.CoreLib/System.IO.Stream::Null\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.Stream::Null\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"System.Private.CoreLib/System.IO.Stream\",\"declaringTypeShape\":1},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"ilOffset\":8,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Text.Json/JsonDocumentOptions\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"type\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":24,\"resultType\":\"System.Text.Json.Nodes.JsonNode\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::Parse:System.Text.Json.Nodes.JsonNode(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":29,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":30,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_71[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-async-13-stream-system-nullable-json-node-options-json-document-options-cancellation-token-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-async-13-stream-system-nullable-json-node-options-json-document-options-cancellation-token-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":11,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Text.Json/JsonDocumentOptions\",\"ilOffset\":13,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"type\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":19,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":20,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":22,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":28,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":29,\"resultType\":\"System.Threading.Tasks.Task\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ParseAsync:System.Threading.Tasks.Task\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ParseAsync:System.Threading.Tasks.Task\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ParseAsync:System.Threading.Tasks.Task\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":34,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":35,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_72[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-async-13-stream-system-nullable-json-node-options-json-document-options-cancellation-token-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-parse-async-13-stream-system-nullable-json-node-options-json-document-options-cancellation-token-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":3,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldsfld\",\"opCode\":12,\"operand\":\"System.Private.CoreLib/System.IO.Stream::Null\",\"ilOffset\":1,\"resultType\":\"System.IO.Stream\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"field\",\"subjectId\":\"System.Private.CoreLib/System.IO.Stream::Null\"},\"targetReference\":{\"kind\":2,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.IO.Stream::Null\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeSubjectId\":\"System.Private.CoreLib/System.IO.Stream\",\"declaringTypeShape\":1},\"runtimeServiceKind\":4,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":6,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"ilOffset\":8,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":14,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":1,\"ilOffset\":15,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Text.Json/JsonDocumentOptions\",\"ilOffset\":17,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"type\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/JsonDocumentOptions\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":23,\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":2,\"ilOffset\":24,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"ilOffset\":26,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Threading.CancellationToken\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":2,\"ilOffset\":32,\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":33,\"resultType\":\"System.Threading.Tasks.Task\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ParseAsync:System.Threading.Tasks.Task\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions,System.Threading.CancellationToken)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ParseAsync:System.Threading.Tasks.Task\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions,System.Threading.CancellationToken)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ParseAsync:System.Threading.Tasks.Task\\u003CSystem.Text.Json.Nodes.JsonNode\\u003E(System.IO.Stream,System.Nullable\\u003CSystem.Text.Json.Nodes.JsonNodeOptions\\u003E,JsonDocumentOptions,System.Threading.CancellationToken)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":38,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":39,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_73[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-replace-with-10-int-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-replace-with-10-int-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ReplaceWith_10_int_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0)\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith\\u003CSystem.Int32\\u003E(System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_74[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-replace-with-10-int-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-replace-with-10-int-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ReplaceWith_10_int_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ReplaceWith_10_int_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith\\u003CSystem.Int32\\u003E:System.Void(System.Int32)\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0)\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith\\u003CSystem.Int32\\u003E(System.Int32)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ReplaceWith:System.Void(!!0)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_75[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-to-json-string-14-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ToJsonString_14_JsonSerializerOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ToJsonString_14_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ToJsonString_14_JsonSerializerOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ToJsonString_14_JsonSerializerOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-to-json-string-14-json-serializer-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_ToJsonString_14_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_ToJsonString_14_JsonSerializerOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.String\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ToJsonString:System.String(JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ToJsonString:System.String(JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::ToJsonString:System.String(JsonSerializerOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_76[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-write-to-15-utf8-json-writer-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_WriteTo_15_Utf8JsonWriter_JsonSerializerOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_WriteTo_15_Utf8JsonWriter_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_WriteTo_15_Utf8JsonWriter_JsonSerializerOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_WriteTo_15_Utf8JsonWriter_JsonSerializerOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.benchmark-write-to-15-utf8-json-writer-json-serializer-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Benchmark_WriteTo_15_Utf8JsonWriter_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Benchmark_WriteTo_15_Utf8JsonWriter_JsonSerializerOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(Utf8JsonWriter,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(Utf8JsonWriter,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(Utf8JsonWriter,JsonSerializerOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_77[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.deep-clone-5-5\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepClone_5__5:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepClone_5__5()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepClone_5__5:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepClone_5__5:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.deep-clone-5-5\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepClone_5__5()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_DeepClone_5__5\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_78[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.deep-equals-9-json-node-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepEquals_9_JsonNode_JsonNode_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepEquals_9_JsonNode_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepEquals_9_JsonNode_JsonNode_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepEquals_9_JsonNode_JsonNode_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.deep-equals-9-json-node-json-node-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::DeepEquals_9_JsonNode_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_DeepEquals_9_JsonNode_JsonNode_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepEquals:System.Boolean(System.Text.Json.Nodes.JsonNode,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepEquals:System.Boolean(System.Text.Json.Nodes.JsonNode,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::DeepEquals:System.Boolean(System.Text.Json.Nodes.JsonNode,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":8,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"brtrue\",\"opCode\":18,\"operand\":16,\"ilOffset\":10,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":12,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":13,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":18,\"ilOffset\":14,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":1,\"ilOffset\":16,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":17,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":18,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":21,\"ilOffset\":19,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":21,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":22,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_79[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.get-element-index-8-8\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetElementIndex_8__8:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetElementIndex_8__8()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetElementIndex_8__8:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetElementIndex_8__8:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.get-element-index-8-8\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetElementIndex_8__8()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetElementIndex_8__8\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_80[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.get-path-3-3\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPath_3__3:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPath_3__3()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPath_3__3:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPath_3__3:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.get-path-3-3\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPath_3__3()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetPath_3__3\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_81[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.get-property-name-7-7\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPropertyName_7__7:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPropertyName_7__7()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPropertyName_7__7:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPropertyName_7__7:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.get-property-name-7-7\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetPropertyName_7__7()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetPropertyName_7__7\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_82[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.get-value-4-4\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValue_4__4:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValue_4__4()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValue_4__4:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValue_4__4:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.get-value-4-4\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValue_4__4()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetValue_4__4\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_83[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.get-value-kind-6-6\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValueKind_6__6:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValueKind_6__6()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValueKind_6__6:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValueKind_6__6:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.get-value-kind-6-6\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::GetValueKind_6__6()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_GetValueKind_6__6\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_84[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-11-string-system-nullable-json-node-options-json-document-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-11-string-system-nullable-json-node-options-json-document-options-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_85[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-11-string-system-nullable-json-node-options-json-document-options-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-11-string-system-nullable-json-node-options-json-document-options-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_11_string_System_NullableJsonNodeOptions_JsonDocumentOptions_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_86[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-12-stream-system-nullable-json-node-options-json-document-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-12-stream-system-nullable-json-node-options-json-document-options-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_87[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-12-stream-system-nullable-json-node-options-json-document-options-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-12-stream-system-nullable-json-node-options-json-document-options-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_Parse_12_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_88[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-async-13-stream-system-nullable-json-node-options-json-document-options-cancellation-token-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-async-13-stream-system-nullable-json-node-options-json-document-options-cancellation-token-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_89[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-async-13-stream-system-nullable-json-node-options-json-document-options-cancellation-token-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.parse-async-13-stream-system-nullable-json-node-options-json-document-options-cancellation-token-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_ParseAsync_13_Stream_System_NullableJsonNodeOptions_JsonDocumentOptions_CancellationToken_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_90[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.to-json-string-14-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ToJsonString_14_JsonSerializerOptions_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ToJsonString_14_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ToJsonString_14_JsonSerializerOptions_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ToJsonString_14_JsonSerializerOptions_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-node-tests.to-json-string-14-json-serializer-options-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonNodeTests::ToJsonString_14_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonNodeTests_ToJsonString_14_JsonSerializerOptions_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_91[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_92[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-add-2-string-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-add-2-string-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Add_2_string_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.String,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.String,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.String,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_93[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-add-2-string-json-node-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-add-2-string-json-node-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_2_string_JsonNode_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Add_2_string_JsonNode_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":7,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.String,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.String,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.String,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":14,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_94[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-add-3-key-value-pairstring-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_3_KeyValuePairstringJsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_3_KeyValuePairstringJsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_3_KeyValuePairstringJsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_3_KeyValuePairstringJsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-add-3-key-value-pairstring-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Add_3_KeyValuePairstringJsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Add_3_KeyValuePairstringJsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"initobj\",\"opCode\":71,\"operand\":\"System.Private.CoreLib/System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E\",\"ilOffset\":4,\"resultType\":\"System.Void\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"type\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E\"},\"targetReference\":{\"kind\":1,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E\",\"typeShape\":1,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"runtimeServiceKind\":14,\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":10,\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Add:System.Void(System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_95[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-clear-4-4\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Clear_4__4:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Clear_4__4()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Clear_4__4:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Clear_4__4:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-clear-4-4\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Clear_4__4()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Clear_4__4\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Clear:System.Void()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Clear:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Clear:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_96[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-contains-key-5-string-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-contains-key-5-string-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_ContainsKey_5_string_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::ContainsKey:System.Boolean(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::ContainsKey:System.Boolean(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::ContainsKey:System.Boolean(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_97[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-contains-key-5-string-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-contains-key-5-string-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_ContainsKey_5_string_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_ContainsKey_5_string_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::ContainsKey:System.Boolean(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::ContainsKey:System.Boolean(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::ContainsKey:System.Boolean(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_98[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-get-at-8-int-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-get-at-8-int-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_GetAt_8_int_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::GetAt:System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::GetAt:System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::GetAt:System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_99[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-get-at-8-int-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-get-at-8-int-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetAt_8_int_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_GetAt_8_int_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::GetAt:System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::GetAt:System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::GetAt:System.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_100[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-get-enumerator-7-7\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetEnumerator_7__7:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetEnumerator_7__7()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetEnumerator_7__7:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetEnumerator_7__7:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-get-enumerator-7-7\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_GetEnumerator_7__7()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_GetEnumerator_7__7\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Collections.Generic.IEnumerator\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E\\u003E\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E\\u003E()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E\\u003E()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::GetEnumerator:System.Collections.Generic.IEnumerator\\u003CSystem.Collections.Generic.KeyValuePair\\u003CSystem.String,System.Text.Json.Nodes.JsonNode\\u003E\\u003E()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_101[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-index-of-11-string-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-index-of-11-string-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_IndexOf_11_string_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::IndexOf:System.Int32(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::IndexOf:System.Int32(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::IndexOf:System.Int32(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_102[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-index-of-11-string-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-index-of-11-string-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_IndexOf_11_string_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_IndexOf_11_string_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Int32\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::IndexOf:System.Int32(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::IndexOf:System.Int32(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::IndexOf:System.Int32(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_103[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-insert-12-int-string-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-insert-12-int-string-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Insert_12_int_string_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":11,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_104[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-insert-12-int-string-json-node-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-insert-12-int-string-json-node-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Insert_12_int_string_JsonNode_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":11,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_105[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-insert-12-int-string-json-node-2\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_2:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_2()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_2:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_2:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-insert-12-int-string-json-node-2\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Insert_12_int_string_JsonNode_2()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Insert_12_int_string_JsonNode_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"\",\"ilOffset\":3,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":8,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":9,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Insert:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":15,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_106[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-remove-6-string-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-remove-6-string-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Remove_6_string_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Remove:System.Boolean(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Remove:System.Boolean(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Remove:System.Boolean(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_107[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-remove-6-string-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-remove-6-string-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_Remove_6_string_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_Remove_6_string_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Remove:System.Boolean(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Remove:System.Boolean(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::Remove:System.Boolean(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_108[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-remove-at-13-int-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-remove-at-13-int-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_RemoveAt_13_int_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::RemoveAt:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::RemoveAt:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::RemoveAt:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_109[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-remove-at-13-int-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-remove-at-13-int-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_RemoveAt_13_int_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_RemoveAt_13_int_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::RemoveAt:System.Void(System.Int32)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::RemoveAt:System.Void(System.Int32)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::RemoveAt:System.Void(System.Int32)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_110[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-set-at-10-int-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-set-at-10-int-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_10_int_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_111[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-set-at-10-int-json-node-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-set-at-10-int-json-node-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_10_int_JsonNode_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_10_int_JsonNode_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_112[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-set-at-9-int-string-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-set-at-9-int-string-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_9_int_string_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":11,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_113[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-set-at-9-int-string-json-node-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-set-at-9-int-string-json-node-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_9_int_string_JsonNode_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":11,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_114[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-set-at-9-int-string-json-node-2\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_2:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_2()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_2:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_2:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-set-at-9-int-string-json-node-2\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_SetAt_9_int_string_JsonNode_2()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_SetAt_9_int_string_JsonNode_2\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"\",\"ilOffset\":3,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":8,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":9,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::SetAt:System.Void(System.Int32,System.String,System.Text.Json.Nodes.JsonNode)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":15,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_115[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-try-get-property-value-0-string-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-try-get-property-value-0-string-json-node-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_TryGetPropertyValue_0_string_JsonNode_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode\\u0026)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_116[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-try-get-property-value-0-string-json-node-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-try-get-property-value-0-string-json-node-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_TryGetPropertyValue_0_string_JsonNode_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_TryGetPropertyValue_0_string_JsonNode_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"\",\"ilOffset\":4,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":9,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":11,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode\\u0026)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonObject::TryGetPropertyValue:System.Boolean(System.String,System.Text.Json.Nodes.JsonNode\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":16,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":17,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_117[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-write-to-1-utf8-json-writer-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_WriteTo_1_Utf8JsonWriter_JsonSerializerOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_WriteTo_1_Utf8JsonWriter_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_WriteTo_1_Utf8JsonWriter_JsonSerializerOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_WriteTo_1_Utf8JsonWriter_JsonSerializerOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.benchmark-write-to-1-utf8-json-writer-json-serializer-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Benchmark_WriteTo_1_Utf8JsonWriter_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Benchmark_WriteTo_1_Utf8JsonWriter_JsonSerializerOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(Utf8JsonWriter,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(Utf8JsonWriter,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonNode::WriteTo:System.Void(Utf8JsonWriter,JsonSerializerOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_118[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.contains-key-5-string-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.contains-key-5-string-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_ContainsKey_5_string_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_119[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.contains-key-5-string-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.contains-key-5-string-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::ContainsKey_5_string_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_ContainsKey_5_string_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_120[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.get-at-8-int-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.get-at-8-int-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_GetAt_8_int_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_121[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.get-at-8-int-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.get-at-8-int-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetAt_8_int_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_GetAt_8_int_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_122[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.get-enumerator-7-7\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetEnumerator_7__7:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetEnumerator_7__7()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetEnumerator_7__7:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetEnumerator_7__7:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.get-enumerator-7-7\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::GetEnumerator_7__7()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_GetEnumerator_7__7\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_123[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.index-of-11-string-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.index-of-11-string-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_IndexOf_11_string_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_124[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.index-of-11-string-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.index-of-11-string-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::IndexOf_11_string_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_IndexOf_11_string_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_125[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.remove-6-string-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.remove-6-string-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Remove_6_string_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_126[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.remove-6-string-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.remove-6-string-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::Remove_6_string_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_Remove_6_string_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_127[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.try-get-property-value-0-string-json-node-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.try-get-property-value-0-string-json-node-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_TryGetPropertyValue_0_string_JsonNode_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":5,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":9,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_128[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.try-get-property-value-0-string-json-node-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-object-tests.try-get-property-value-0-string-json-node-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonObjectTests::TryGetPropertyValue_0_string_JsonNode_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonObjectTests_TryGetPropertyValue_0_string_JsonNode_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":5,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":9,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_129[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-value-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-value-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_130[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-value-tests.benchmark-try-get-value-0-int-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-value-tests.benchmark-try-get-value-0-int-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_Benchmark_TryGetValue_0_int_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue\\u003CSystem.Int32\\u003E:System.Boolean(System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue\\u003CSystem.Int32\\u003E:System.Boolean(System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue\\u003CSystem.Int32\\u003E:System.Boolean(System.Int32\\u0026)\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue\\u003CSystem.Int32\\u003E:System.Boolean(System.Int32\\u0026)\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026)\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue\\u003CSystem.Int32\\u003E(System.Int32\\u0026)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_131[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-value-tests.benchmark-try-get-value-0-int-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-value-tests.benchmark-try-get-value-0-int-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::Benchmark_TryGetValue_0_int_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_Benchmark_TryGetValue_0_int_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":6,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue\\u003CSystem.Int32\\u003E:System.Boolean(System.Int32\\u0026)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue\\u003CSystem.Int32\\u003E:System.Boolean(System.Int32\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue\\u003CSystem.Int32\\u003E:System.Boolean(System.Int32\\u0026)\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026);type=[];method=[System.Int32]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026);type=[];method=[System.Int32]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026);type=[];method=[System.Int32]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:methodSpec\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue\\u003CSystem.Int32\\u003E:System.Boolean(System.Int32\\u0026)\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026)\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue\\u003CSystem.Int32\\u003E(System.Int32\\u0026)\",\"instantiationKey\":{\"contextKind\":2,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Nodes.JsonValue::TryGetValue:System.Boolean(!!0\\u0026)\",\"typeArguments\":[],\"methodArguments\":[\"System.Int32\"]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":11,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":12,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_132[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-value-tests.try-get-value-0-int-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-value-tests.try-get-value-0-int-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_TryGetValue_0_int_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":5,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":9,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_133[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-value-tests.try-get-value-0-int-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-nodes-json-value-tests.try-get-value-0-int-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Nodes_JsonValueTests::TryGetValue_0_int_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Nodes_JsonValueTests_TryGetValue_0_int_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":5,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":9,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_134[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-schema-json-schema-exporter-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-schema-json-schema-exporter-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporterTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_135[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-schema-json-schema-exporter-tests.benchmark-get-json-schema-as-node-0-json-serializer-options-system-type-json-schema-exporter-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::Benchmark_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::Benchmark_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::Benchmark_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::Benchmark_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-schema-json-schema-exporter-tests.benchmark-get-json-schema-as-node-0-json-serializer-options-system-type-json-schema-exporter-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::Benchmark_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporterTests_Benchmark_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":4,\"resultType\":\"System.Text.Json.Nodes.JsonNode\",\"callee\":\"System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::GetJsonSchemaAsNode:System.Text.Json.Nodes.JsonNode(JsonSerializerOptions,System.Type,System.Text.Json.Schema.JsonSchemaExporterOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::GetJsonSchemaAsNode:System.Text.Json.Nodes.JsonNode(JsonSerializerOptions,System.Type,System.Text.Json.Schema.JsonSchemaExporterOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Schema.JsonSchemaExporter::GetJsonSchemaAsNode:System.Text.Json.Nodes.JsonNode(JsonSerializerOptions,System.Type,System.Text.Json.Schema.JsonSchemaExporterOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_136[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-schema-json-schema-exporter-tests.get-json-schema-as-node-0-json-serializer-options-system-type-json-schema-exporter-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-schema-json-schema-exporter-tests.get-json-schema-as-node-0-json-serializer-options-system-type-json-schema-exporter-options-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Schema_JsonSchemaExporterTests::GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Schema_JsonSchemaExporterTests_GetJsonSchemaAsNode_0_JsonSerializerOptions_System_Type_JsonSchemaExporterOptions_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_137[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-attribute-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-attribute-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterAttributeTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_138[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-attribute-tests.benchmark-create-converter-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::Benchmark_CreateConverter_0_System_Type_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::Benchmark_CreateConverter_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::Benchmark_CreateConverter_0_System_Type_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::Benchmark_CreateConverter_0_System_Type_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-attribute-tests.benchmark-create-converter-0-system-type-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::Benchmark_CreateConverter_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterAttributeTests_Benchmark_CreateConverter_0_System_Type_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Text.Json.Serialization.JsonConverter\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterAttribute::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterAttribute::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterAttribute::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_139[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-attribute-tests.create-converter-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::CreateConverter_0_System_Type_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::CreateConverter_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::CreateConverter_0_System_Type_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::CreateConverter_0_System_Type_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-attribute-tests.create-converter-0-system-type-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterAttributeTests::CreateConverter_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterAttributeTests_CreateConverter_0_System_Type_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_140[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-factory-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-factory-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactoryTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_141[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-factory-tests.benchmark-create-converter-0-system-type-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::Benchmark_CreateConverter_0_System_Type_JsonSerializerOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::Benchmark_CreateConverter_0_System_Type_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::Benchmark_CreateConverter_0_System_Type_JsonSerializerOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::Benchmark_CreateConverter_0_System_Type_JsonSerializerOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-factory-tests.benchmark-create-converter-0-system-type-json-serializer-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::Benchmark_CreateConverter_0_System_Type_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactoryTests_Benchmark_CreateConverter_0_System_Type_JsonSerializerOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Text.Json.Serialization.JsonConverter\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_142[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-factory-tests.create-converter-0-system-type-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::CreateConverter_0_System_Type_JsonSerializerOptions_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::CreateConverter_0_System_Type_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::CreateConverter_0_System_Type_JsonSerializerOptions_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::CreateConverter_0_System_Type_JsonSerializerOptions_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-factory-tests.create-converter-0-system-type-json-serializer-options-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterFactoryTests::CreateConverter_0_System_Type_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterFactoryTests_CreateConverter_0_System_Type_JsonSerializerOptions_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_143[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_144[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.benchmark-can-convert-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_CanConvert_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.benchmark-can-convert-0-system-type-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_CanConvert_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_CanConvert_0_System_Type_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_145[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.benchmark-write-1-utf8-json-writer-int-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.benchmark-write-1-utf8-json-writer-int-json-serializer-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::Write:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::Write:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::Write:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::Write:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::Write(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":11,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_146[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.benchmark-write-1-utf8-json-writer-int-json-serializer-options-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.benchmark-write-1-utf8-json-writer-int-json-serializer-options-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_Write_1_Utf8JsonWriter_int_JsonSerializerOptions_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::Write:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::Write:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::Write:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::Write:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::Write(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::Write:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":11,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_147[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.benchmark-write-as-property-name-2-utf8-json-writer-int-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.benchmark-write-as-property-name-2-utf8-json-writer-int-json-serializer-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::WriteAsPropertyName:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::WriteAsPropertyName:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::WriteAsPropertyName:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::WriteAsPropertyName:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::WriteAsPropertyName(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":11,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_148[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.benchmark-write-as-property-name-2-utf8-json-writer-int-json-serializer-options-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.benchmark-write-as-property-name-2-utf8-json-writer-int-json-serializer-options-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_Benchmark_WriteAsPropertyName_2_Utf8JsonWriter_int_JsonSerializerOptions_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":5,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::WriteAsPropertyName:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::WriteAsPropertyName:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::WriteAsPropertyName:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"openDefinitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"runtimeGenericContext\":{\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]},\"sharedGenericBodyId\":{\"value\":\"body:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"instantiationStubId\":{\"value\":\"stub:definition=System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions);type=[System.Int32];method=[]\"},\"supportKindCode\":3,\"specializationKindCode\":1,\"statusReasonCode\":\"loader-demand:memberReference\"},\"genericDiagnostic\":{\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::WriteAsPropertyName:System.Void(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"displaySubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u003CSystem.Int32\\u003E::WriteAsPropertyName(Utf8JsonWriter,System.Int32,JsonSerializerOptions)\",\"instantiationKey\":{\"contextKind\":1,\"definitionSubjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter\\u00601::WriteAsPropertyName:System.Void(Utf8JsonWriter,!0,JsonSerializerOptions)\",\"typeArguments\":[\"System.Int32\"],\"methodArguments\":[]}},\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":11,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_149[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.can-convert-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::CanConvert_0_System_Type_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::CanConvert_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::CanConvert_0_System_Type_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::CanConvert_0_System_Type_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-system-int32-tests.can-convert-0-system-type-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterSystem_Int32Tests::CanConvert_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterSystem_Int32Tests_CanConvert_0_System_Type_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_150[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_151[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-tests.benchmark-can-convert-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::Benchmark_CanConvert_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-tests.benchmark-can-convert-0-system-type-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::Benchmark_CanConvert_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterTests_Benchmark_CanConvert_0_System_Type_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_152[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-tests.can-convert-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::CanConvert_0_System_Type_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::CanConvert_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::CanConvert_0_System_Type_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::CanConvert_0_System_Type_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-converter-tests.can-convert-0-system-type-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonConverterTests::CanConvert_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonConverterTests_CanConvert_0_System_Type_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_153[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-number-enum-converter-system-day-of-week-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-number-enum-converter-system-day-of-week-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_154[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-number-enum-converter-system-day-of-week-tests.benchmark-can-convert-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-number-enum-converter-system-day-of-week-tests.benchmark-can-convert-0-system-type-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_Benchmark_CanConvert_0_System_Type_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_155[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-number-enum-converter-system-day-of-week-tests.benchmark-create-converter-1-system-type-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-number-enum-converter-system-day-of-week-tests.benchmark-create-converter-1-system-type-json-serializer-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Text.Json.Serialization.JsonConverter\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_156[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-number-enum-converter-system-day-of-week-tests.can-convert-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-number-enum-converter-system-day-of-week-tests.can-convert-0-system-type-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_CanConvert_0_System_Type_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_157[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-number-enum-converter-system-day-of-week-tests.create-converter-1-system-type-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-number-enum-converter-system-day-of-week-tests.create-converter-1-system-type-json-serializer-options-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonNumberEnumConverterSystem_DayOfWeekTests_CreateConverter_1_System_Type_JsonSerializerOptions_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_158[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-system-day-of-week-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-system-day-of-week-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_159[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-system-day-of-week-tests.benchmark-can-convert-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-system-day-of-week-tests.benchmark-can-convert-0-system-type-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CanConvert_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_Benchmark_CanConvert_0_System_Type_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_160[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-system-day-of-week-tests.benchmark-create-converter-1-system-type-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-system-day-of-week-tests.benchmark-create-converter-1-system-type-json-serializer-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Text.Json.Serialization.JsonConverter\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_161[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-system-day-of-week-tests.can-convert-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-system-day-of-week-tests.can-convert-0-system-type-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CanConvert_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_CanConvert_0_System_Type_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_162[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-system-day-of-week-tests.create-converter-1-system-type-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-system-day-of-week-tests.create-converter-1-system-type-json-serializer-options-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests::CreateConverter_1_System_Type_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterSystem_DayOfWeekTests_CreateConverter_1_System_Type_JsonSerializerOptions_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_163[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_164[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-tests.benchmark-can-convert-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CanConvert_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CanConvert_0_System_Type_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-tests.benchmark-can-convert-0-system-type-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CanConvert_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_Benchmark_CanConvert_0_System_Type_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Boolean\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverter::CanConvert:System.Boolean(System.Type)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_165[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-tests.benchmark-create-converter-1-system-type-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-tests.benchmark-create-converter-1-system-type-json-serializer-options-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_Benchmark_CreateConverter_1_System_Type_JsonSerializerOptions_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Text.Json.Serialization.JsonConverter\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.JsonConverterFactory::CreateConverter:System.Text.Json.Serialization.JsonConverter(System.Type,JsonSerializerOptions)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_166[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-tests.can-convert-0-system-type-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CanConvert_0_System_Type_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CanConvert_0_System_Type_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CanConvert_0_System_Type_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CanConvert_0_System_Type_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-tests.can-convert-0-system-type-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CanConvert_0_System_Type_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_CanConvert_0_System_Type_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_167[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-tests.create-converter-1-system-type-json-serializer-options-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CreateConverter_1_System_Type_JsonSerializerOptions_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CreateConverter_1_System_Type_JsonSerializerOptions_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-json-string-enum-converter-tests.create-converter-1-system-type-json-serializer-options-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_JsonStringEnumConverterTests::CreateConverter_1_System_Type_JsonSerializerOptions_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_JsonStringEnumConverterTests_CreateConverter_1_System_Type_JsonSerializerOptions_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_168[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-handler-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-handler-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceHandlerTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_169[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-handler-tests.benchmark-create-resolver-0-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::Benchmark_CreateResolver_0__0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::Benchmark_CreateResolver_0__0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::Benchmark_CreateResolver_0__0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::Benchmark_CreateResolver_0__0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-handler-tests.benchmark-create-resolver-0-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::Benchmark_CreateResolver_0__0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceHandlerTests_Benchmark_CreateResolver_0__0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":2,\"resultType\":\"System.Text.Json.Serialization.ReferenceResolver\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceHandler::CreateResolver:System.Text.Json.Serialization.ReferenceResolver()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_170[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-handler-tests.create-resolver-0-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::CreateResolver_0__0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::CreateResolver_0__0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::CreateResolver_0__0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::CreateResolver_0__0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-handler-tests.create-resolver-0-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceHandlerTests::CreateResolver_0__0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceHandlerTests_CreateResolver_0__0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_171[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.ctor\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::.ctor:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::.ctor()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::.ctor:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::.ctor:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.ctor\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::.ctor()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests__ctor\",\"isStatic\":false,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldarg\",\"opCode\":6,\"operand\":0,\"ilOffset\":0,\"resultType\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"isPreserveSig\":false},{\"op\":\"call\",\"opCode\":14,\"ilOffset\":1,\"resultType\":\"System.Void\",\"callee\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"reference\":{\"assemblyName\":\"System.Private.CoreLib\",\"subjectKind\":\"method\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Private.CoreLib\",\"subjectId\":\"System.Private.CoreLib/System.Object::.ctor:System.Void()\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":7,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_172[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-add-reference-0-string-object-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-add-reference-0-string-object-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_AddReference_0_string_object_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":4,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_173[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-add-reference-0-string-object-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-add-reference-0-string-object-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_AddReference_0_string_object_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_AddReference_0_string_object_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":7,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":8,\"resultType\":\"System.Void\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::AddReference:System.Void(System.String,System.Object)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":14,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_174[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-get-reference-1-object-bool-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-get-reference-1-object-bool-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_GetReference_1_object_bool_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.String\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean\\u0026)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_175[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-get-reference-1-object-bool-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-get-reference-1-object-bool-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_GetReference_1_object_bool_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_GetReference_1_object_bool_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":3,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":4,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldloca\",\"opCode\":76,\"operand\":0,\"ilOffset\":5,\"resultType\":\"System.IntPtr\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.String\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean\\u0026)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean\\u0026)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::GetReference:System.String(System.Object,System.Boolean\\u0026)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_176[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-resolve-reference-2-string-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_0:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_0:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_0:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-resolve-reference-2-string-0\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_ResolveReference_2_string_0\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":2,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":3,\"resultType\":\"System.Object\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":8,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":9,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_177[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-resolve-reference-2-string-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_1:System.Void()\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_1:System.Void()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_1:System.Void()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.benchmark-resolve-reference-2-string-1\",\"signature\":\"System.Void AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::Benchmark_ResolveReference_2_string_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_Benchmark_ResolveReference_2_string_1\",\"isStatic\":true,\"returnType\":\"System.Void\",\"returnAbi\":{\"carrierKindCode\":0,\"typeShape\":1},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":0,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldnull\",\"opCode\":5,\"ilOffset\":1,\"resultType\":\"System.Object\",\"isPreserveSig\":false},{\"op\":\"ldstr\",\"opCode\":4,\"operand\":\"\",\"ilOffset\":2,\"resultType\":\"System.String\",\"isPreserveSig\":false},{\"op\":\"callvirt\",\"opCode\":15,\"ilOffset\":7,\"resultType\":\"System.Object\",\"callee\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)\",\"reference\":{\"assemblyName\":\"System.Text.Json\",\"subjectKind\":\"method\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)\"},\"targetReference\":{\"kind\":3,\"assemblyName\":\"System.Text.Json\",\"subjectId\":\"System.Text.Json/System.Text.Json.Serialization.ReferenceResolver::ResolveReference:System.Object(System.String)\",\"typeShape\":0,\"arrayElementTypeShape\":0,\"isSealed\":false,\"isComImport\":false,\"comInterfaceTypeKind\":0,\"declaringTypeShape\":0},\"dispatchKindCode\":4,\"isPreserveSig\":false},{\"op\":\"pop\",\"opCode\":47,\"ilOffset\":12,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":13,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_178[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.get-reference-1-object-bool-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.get-reference-1-object-bool-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_GetReference_1_object_bool_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":5,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":9,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_179[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.get-reference-1-object-bool-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.get-reference-1-object-bool-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::GetReference_1_object_bool_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_GetReference_1_object_bool_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":2,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":0,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":2,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":3,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":5,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":1,\"ilOffset\":6,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":9,\"ilOffset\":7,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":1,\"ilOffset\":9,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":10,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_180[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.resolve-reference-2-string-0\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_0:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_0()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_0:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_0:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.resolve-reference-2-string-0\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_0()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_ResolveReference_2_string_0\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";
static const char kJitJson_181[] = "{\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.resolve-reference-2-string-1\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_1:System.Int64()\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_1()\",\"identity\":{\"assemblyName\":\"CombinedSubjects\",\"declaringTypeSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests\",\"definitionSubjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_1:System.Int64()\",\"subjectId\":\"CombinedSubjects/AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_1:System.Int64()\",\"methodId\":\"combined-subjects.auto-generated-system-text-json-system-text-json-serialization-reference-resolver-tests.resolve-reference-2-string-1\",\"signature\":\"System.Int64 AutoGenerated.System_Text_Json.System_Text_Json_Serialization_ReferenceResolverTests::ResolveReference_2_string_1()\"},\"nativeSymbol\":\"CombinedSubjects_AutoGenerated_System_Text_Json_System_Text_Json_Serialization_ReferenceResolverTests_ResolveReference_2_string_1\",\"isStatic\":false,\"returnType\":\"System.Int64\",\"returnAbi\":{\"carrierKindCode\":10,\"typeShape\":2},\"parameterCount\":0,\"parameterAbis\":[],\"localCount\":1,\"exceptionRegionCount\":0,\"exceptionRegions\":[],\"instructions\":[{\"op\":\"ldc.i4\",\"opCode\":0,\"operand\":42,\"ilOffset\":1,\"resultType\":\"System.Int32\",\"isPreserveSig\":false},{\"op\":\"conv.i8\",\"opCode\":40,\"ilOffset\":3,\"resultType\":\"System.Int64\",\"isPreserveSig\":false},{\"op\":\"stloc\",\"opCode\":8,\"operand\":0,\"ilOffset\":4,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"br\",\"opCode\":17,\"operand\":7,\"ilOffset\":5,\"resultType\":\"System.Void\",\"isPreserveSig\":false},{\"op\":\"ldloc\",\"opCode\":7,\"operand\":0,\"ilOffset\":7,\"isPreserveSig\":false},{\"op\":\"ret\",\"opCode\":53,\"ilOffset\":8,\"isPreserveSig\":false}],\"isPInvoke\":false,\"isUnmanagedCallersOnly\":false,\"importCallingConvention\":0,\"importCharSet\":0,\"importSetLastError\":false,\"isInternalLink\":false,\"isSuppressGCTransition\":false,\"hasBlittableStructReturn\":false,\"declaringAssemblyName\":\"CombinedSubjects\"}";

extern "C" const JitEntry kChaosJitEntries[182] =
{
    { kJitJson_0, 2480u, 0x00000034u, 0u },
    { kJitJson_1, 2438u, 0x0000002Eu, 0u },
    { kJitJson_2, 7431u, 0x0000004Eu, 0u },
    { kJitJson_3, 2438u, 0x00000032u, 0u },
    { kJitJson_4, 2921u, 0x0000003Au, 0u },
    { kJitJson_5, 2466u, 0x00000038u, 0u },
    { kJitJson_6, 2452u, 0x0000002Au, 0u },
    { kJitJson_7, 2452u, 0x00000022u, 0u },
    { kJitJson_8, 2454u, 0x00000026u, 0u },
    { kJitJson_9, 6173u, 0x0000003Eu, 0u },
    { kJitJson_10, 2452u, 0x00000030u, 0u },
    { kJitJson_11, 2466u, 0x00000036u, 0u },
    { kJitJson_12, 3008u, 0x0000003Cu, 0u },
    { kJitJson_13, 2466u, 0x0000002Cu, 0u },
    { kJitJson_14, 2466u, 0x00000024u, 0u },
    { kJitJson_15, 2468u, 0x00000028u, 0u },
    { kJitJson_16, 35914u, 0x0000004Cu, 0u },
    { kJitJson_17, 7607u, 0x00000050u, 0u },
    { kJitJson_18, 3309u, 0x0000001Eu, 0u },
    { kJitJson_19, 5407u, 0x00000052u, 0u },
    { kJitJson_20, 2063u, 0x00000020u, 0u },
    { kJitJson_21, 1962u, 0x00000046u, 0u },
    { kJitJson_22, 2152u, 0x00000042u, 0u },
    { kJitJson_23, 2129u, 0x00000040u, 0u },
    { kJitJson_24, 2144u, 0x00000044u, 0u },
    { kJitJson_25, 2166u, 0x0000001Cu, 0u },
    { kJitJson_26, 2100u, 0x0000001Au, 0u },
    { kJitJson_27, 2838u, 0x00000048u, 0u },
    { kJitJson_28, 3206u, 0x0000004Au, 0u },
    { kJitJson_29, 2442u, 0x00000018u, 0u },
    { kJitJson_30, 2132u, 0x00000054u, 0u },
    { kJitJson_31, 2082u, 0x00000056u, 0u },
    { kJitJson_32, 2134u, 0x00000058u, 0u },
    { kJitJson_33, 2355u, 0x0000003Du, 0u },
    { kJitJson_34, 4188u, 0x0000001Bu, 0u },
    { kJitJson_35, 4188u, 0x0000001Du, 0u },
    { kJitJson_36, 2652u, 0x00000021u, 0u },
    { kJitJson_37, 2422u, 0x00000023u, 0u },
    { kJitJson_38, 2779u, 0x00000027u, 0u },
    { kJitJson_39, 2868u, 0x0000003Bu, 0u },
    { kJitJson_40, 4755u, 0x00000019u, 0u },
    { kJitJson_41, 2762u, 0x0000002Bu, 0u },
    { kJitJson_42, 2859u, 0x0000002Du, 0u },
    { kJitJson_43, 2859u, 0x0000002Fu, 0u },
    { kJitJson_44, 2757u, 0x00000033u, 0u },
    { kJitJson_45, 2621u, 0x00000035u, 0u },
    { kJitJson_46, 2621u, 0x00000037u, 0u },
    { kJitJson_47, 3032u, 0x0000001Fu, 0u },
    { kJitJson_48, 2193u, 0x00000025u, 0u },
    { kJitJson_49, 2183u, 0x00000039u, 0u },
    { kJitJson_50, 2135u, 0x00000017u, 0u },
    { kJitJson_51, 2187u, 0x00000029u, 0u },
    { kJitJson_52, 2177u, 0x00000031u, 0u },
    { kJitJson_53, 2345u, 0x00000071u, 0u },
    { kJitJson_54, 2110u, 0x0000003Fu, 0u },
    { kJitJson_55, 2118u, 0x00000043u, 0u },
    { kJitJson_56, 2110u, 0x00000047u, 0u },
    { kJitJson_57, 2578u, 0x00000041u, 0u },
    { kJitJson_58, 2593u, 0x00000045u, 0u },
    { kJitJson_59, 2578u, 0x00000049u, 0u },
    { kJitJson_60, 2596u, 0x00000055u, 0u },
    { kJitJson_61, 2957u, 0x0000005Fu, 0u },
    { kJitJson_62, 2588u, 0x0000005Du, 0u },
    { kJitJson_63, 2502u, 0x0000004Du, 0u },
    { kJitJson_64, 2592u, 0x0000005Bu, 0u },
    { kJitJson_65, 4062u, 0x00000051u, 0u },
    { kJitJson_66, 2559u, 0x00000059u, 0u },
    { kJitJson_67, 4894u, 0x00000063u, 0u },
    { kJitJson_68, 4906u, 0x00000065u, 0u },
    { kJitJson_69, 4903u, 0x00000067u, 0u },
    { kJitJson_70, 5445u, 0x00000069u, 0u },
    { kJitJson_71, 6145u, 0x0000006Bu, 0u },
    { kJitJson_72, 6687u, 0x0000006Du, 0u },
    { kJitJson_73, 4344u, 0x00000060u, 0u },
    { kJitJson_74, 4344u, 0x00000061u, 0u },
    { kJitJson_75, 2895u, 0x0000006Fu, 0u },
    { kJitJson_76, 3031u, 0x00000070u, 0u },
    { kJitJson_77, 2126u, 0x00000053u, 0u },
    { kJitJson_78, 3870u, 0x0000005Eu, 0u },
    { kJitJson_79, 2176u, 0x0000005Cu, 0u },
    { kJitJson_80, 2110u, 0x0000004Bu, 0u },
    { kJitJson_81, 2176u, 0x0000005Au, 0u },
    { kJitJson_82, 2118u, 0x0000004Fu, 0u },
    { kJitJson_83, 2152u, 0x00000057u, 0u },
    { kJitJson_84, 2568u, 0x00000062u, 0u },
    { kJitJson_85, 2568u, 0x00000064u, 0u },
    { kJitJson_86, 2568u, 0x00000066u, 0u },
    { kJitJson_87, 2568u, 0x00000068u, 0u },
    { kJitJson_88, 2756u, 0x0000006Au, 0u },
    { kJitJson_89, 2756u, 0x0000006Cu, 0u },
    { kJitJson_90, 2334u, 0x0000006Eu, 0u },
    { kJitJson_91, 2365u, 0x00000097u, 0u },
    { kJitJson_92, 2854u, 0x00000077u, 0u },
    { kJitJson_93, 2866u, 0x00000078u, 0u },
    { kJitJson_94, 3882u, 0x00000079u, 0u },
    { kJitJson_95, 2434u, 0x0000007Au, 0u },
    { kJitJson_96, 2754u, 0x0000007Cu, 0u },
    { kJitJson_97, 2768u, 0x0000007Eu, 0u },
    { kJitJson_98, 3000u, 0x00000086u, 0u },
    { kJitJson_99, 3000u, 0x00000088u, 0u },
    { kJitJson_100, 3124u, 0x00000084u, 0u },
    { kJitJson_101, 2710u, 0x0000008Fu, 0u },
    { kJitJson_102, 2724u, 0x00000091u, 0u },
    { kJitJson_103, 3068u, 0x00000092u, 0u },
    { kJitJson_104, 3068u, 0x00000093u, 0u },
    { kJitJson_105, 3080u, 0x00000094u, 0u },
    { kJitJson_106, 2697u, 0x00000080u, 0u },
    { kJitJson_107, 2711u, 0x00000082u, 0u },
    { kJitJson_108, 2641u, 0x00000095u, 0u },
    { kJitJson_109, 2641u, 0x00000096u, 0u },
    { kJitJson_110, 2870u, 0x0000008Cu, 0u },
    { kJitJson_111, 2870u, 0x0000008Du, 0u },
    { kJitJson_112, 3051u, 0x00000089u, 0u },
    { kJitJson_113, 3051u, 0x0000008Au, 0u },
    { kJitJson_114, 3063u, 0x0000008Bu, 0u },
    { kJitJson_115, 3331u, 0x00000073u, 0u },
    { kJitJson_116, 3344u, 0x00000075u, 0u },
    { kJitJson_117, 3041u, 0x00000076u, 0u },
    { kJitJson_118, 2210u, 0x0000007Bu, 0u },
    { kJitJson_119, 2210u, 0x0000007Du, 0u },
    { kJitJson_120, 2138u, 0x00000085u, 0u },
    { kJitJson_121, 2138u, 0x00000087u, 0u },
    { kJitJson_122, 2176u, 0x00000083u, 0u },
    { kJitJson_123, 2186u, 0x0000008Eu, 0u },
    { kJitJson_124, 2186u, 0x00000090u, 0u },
    { kJitJson_125, 2168u, 0x0000007Fu, 0u },
    { kJitJson_126, 2168u, 0x00000081u, 0u },
    { kJitJson_127, 2544u, 0x00000072u, 0u },
    { kJitJson_128, 2544u, 0x00000074u, 0u },
    { kJitJson_129, 2355u, 0x0000009Cu, 0u },
    { kJitJson_130, 4743u, 0x00000099u, 0u },
    { kJitJson_131, 4743u, 0x0000009Bu, 0u },
    { kJitJson_132, 2382u, 0x00000098u, 0u },
    { kJitJson_133, 2382u, 0x0000009Au, 0u },
    { kJitJson_134, 2457u, 0x0000009Fu, 0u },
    { kJitJson_135, 3758u, 0x0000009Eu, 0u },
    { kJitJson_136, 2797u, 0x0000009Du, 0u },
    { kJitJson_137, 2567u, 0x000000ACu, 0u },
    { kJitJson_138, 3194u, 0x000000ABu, 0u },
    { kJitJson_139, 2464u, 0x000000AAu, 0u },
    { kJitJson_140, 2547u, 0x000000AFu, 0u },
    { kJitJson_141, 3508u, 0x000000AEu, 0u },
    { kJitJson_142, 2626u, 0x000000ADu, 0u },
    { kJitJson_143, 2597u, 0x000000A9u, 0u },
    { kJitJson_144, 3019u, 0x000000A4u, 0u },
    { kJitJson_145, 5625u, 0x000000A5u, 0u },
    { kJitJson_146, 5625u, 0x000000A6u, 0u },
    { kJitJson_147, 5925u, 0x000000A7u, 0u },
    { kJitJson_148, 5925u, 0x000000A8u, 0u },
    { kJitJson_149, 2451u, 0x000000A3u, 0u },
    { kJitJson_150, 2475u, 0x000000A2u, 0u },
    { kJitJson_151, 2909u, 0x000000A1u, 0u },
    { kJitJson_152, 2341u, 0x000000A0u, 0u },
    { kJitJson_153, 2745u, 0x000000B4u, 0u },
    { kJitJson_154, 3153u, 0x000000B1u, 0u },
    { kJitJson_155, 3687u, 0x000000B3u, 0u },
    { kJitJson_156, 2585u, 0x000000B0u, 0u },
    { kJitJson_157, 2805u, 0x000000B2u, 0u },
    { kJitJson_158, 2745u, 0x000000BEu, 0u },
    { kJitJson_159, 3153u, 0x000000BBu, 0u },
    { kJitJson_160, 3687u, 0x000000BDu, 0u },
    { kJitJson_161, 2585u, 0x000000BAu, 0u },
    { kJitJson_162, 2805u, 0x000000BCu, 0u },
    { kJitJson_163, 2579u, 0x000000B9u, 0u },
    { kJitJson_164, 3003u, 0x000000B6u, 0u },
    { kJitJson_165, 3537u, 0x000000B8u, 0u },
    { kJitJson_166, 2435u, 0x000000B5u, 0u },
    { kJitJson_167, 2655u, 0x000000B7u, 0u },
    { kJitJson_168, 2505u, 0x000000C1u, 0u },
    { kJitJson_169, 2911u, 0x000000C0u, 0u },
    { kJitJson_170, 2310u, 0x000000BFu, 0u },
    { kJitJson_171, 2515u, 0x000000CCu, 0u },
    { kJitJson_172, 3063u, 0x000000C2u, 0u },
    { kJitJson_173, 3075u, 0x000000C3u, 0u },
    { kJitJson_174, 3352u, 0x000000C5u, 0u },
    { kJitJson_175, 3352u, 0x000000C7u, 0u },
    { kJitJson_176, 2985u, 0x000000C9u, 0u },
    { kJitJson_177, 2999u, 0x000000CBu, 0u },
    { kJitJson_178, 2596u, 0x000000C4u, 0u },
    { kJitJson_179, 2596u, 0x000000C6u, 0u },
    { kJitJson_180, 2385u, 0x000000C8u, 0u },
    { kJitJson_181, 2385u, 0x000000CAu, 0u }
};

extern "C" const uint32_t kChaosJitEntryCount = 182u;

extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module;
extern "C" void ChaosJitRegisterAll() {
    // Register hotpatch module so GetDispatchEntry can resolve tokens → slots
    chaos::il2cpp::runtime_core::RegisterHotpatchModule(chaos_il2cpp_aot_hotpatch_module);
    RegisterJitEntryMethods(kChaosJitEntries, kChaosJitEntryCount);
}
#endif