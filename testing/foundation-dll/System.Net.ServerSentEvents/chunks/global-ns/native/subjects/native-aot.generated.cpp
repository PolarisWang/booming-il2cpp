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
#include "exception_stubs.h"
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
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_NullReferenceException_System_Action__generic(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void chaos_stub_definition_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Throws_System_Void_System_Action__type____method__System_NullReferenceException(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_cctor(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_Enumerate_0__0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_EnumerateAsync_1_CancellationToken_0(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Enumerate_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_EnumerateAsync_1_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_cctor(void);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);

// Runtime prelude dependencies included at translation unit level


MethodTable chaos_mt_Chaos_TestFramework_Sdk___0 = {nullptr, nullptr, 12016523716365422726ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_Chaos_TestFramework_Sdk___0 = static_cast<CHAOS_IL2CPP_UINT64>(12016523716365422726ULL);
// ── Virtual method table arrays ──
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
const void* chaos_vtable_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException[] =
{
	reinterpret_cast<void*>(Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String),
};
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2);
const void* chaos_vtable_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c[] =
{
	nullptr,
	reinterpret_cast<void*>(CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor),
	reinterpret_cast<void*>(CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte),
	reinterpret_cast<void*>(CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte),
};
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
const void* chaos_vtable_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c[] =
{
	nullptr,
	reinterpret_cast<void*>(CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor),
	nullptr,
	nullptr,
	reinterpret_cast<void*>(CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0),
	reinterpret_cast<void*>(CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0),
};
MethodTable chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert = {nullptr, nullptr, 3469195832277345062ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert = static_cast<CHAOS_IL2CPP_UINT64>(3469195832277345062ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Exception = {nullptr, nullptr, 10972282733316558392ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Exception = static_cast<CHAOS_IL2CPP_UINT64>(10972282733316558392ULL);
MethodTable chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Exception), chaos_vtable_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException, 8951892653230046377ULL, 1u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException = static_cast<CHAOS_IL2CPP_UINT64>(8951892653230046377ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_UINT64>(15228727185366376748ULL);
MethodTable chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c, 1391641888938102389ULL, 4u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c = static_cast<CHAOS_IL2CPP_UINT64>(1391641888938102389ULL);
MethodTable chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), chaos_vtable_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c, 15317538086767515416ULL, 6u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c = static_cast<CHAOS_IL2CPP_UINT64>(15317538086767515416ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Delegate = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 7451128447593600616ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_UINT64>(7451128447593600616ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Delegate), nullptr, 6681393039041505440ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_UINT64>(6681393039041505440ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Action = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate), nullptr, 14722794505377461205ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Action = static_cast<CHAOS_IL2CPP_UINT64>(14722794505377461205ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Action_SseItem_System_Int32__System_Buffers_IBufferWriter_System_Byte__ = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate), nullptr, 8735361568882115811ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Action_SseItem_System_Int32__System_Buffers_IBufferWriter_System_Byte__ = static_cast<CHAOS_IL2CPP_UINT64>(8735361568882115811ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Collections_IEnumerable = {nullptr, nullptr, 16749028324345562163ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Collections_IEnumerable = static_cast<CHAOS_IL2CPP_UINT64>(16749028324345562163ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_IDisposable = {nullptr, nullptr, 13420242832086098868ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_IDisposable = static_cast<CHAOS_IL2CPP_UINT64>(13420242832086098868ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_IO_Stream = {nullptr, nullptr, 2618932566973839939ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_IO_Stream = static_cast<CHAOS_IL2CPP_UINT64>(2618932566973839939ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_Assembly = {nullptr, nullptr, 5474029880995115448ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_Assembly = static_cast<CHAOS_IL2CPP_UINT64>(5474029880995115448ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName = {nullptr, nullptr, 17082367815459723707ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_AssemblyName = static_cast<CHAOS_IL2CPP_UINT64>(17082367815459723707ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 4197398748135124456ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_MethodBase = static_cast<CHAOS_IL2CPP_UINT64>(4197398748135124456ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase), nullptr, 4137207361503509124ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_ConstructorInfo = static_cast<CHAOS_IL2CPP_UINT64>(4137207361503509124ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo = {nullptr, nullptr, 17040031516751226236ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_FieldInfo = static_cast<CHAOS_IL2CPP_UINT64>(17040031516751226236ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase), nullptr, 10748947813473285525ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Reflection_MethodInfo = static_cast<CHAOS_IL2CPP_UINT64>(10748947813473285525ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_String = {nullptr, nullptr, 1782325859292956794ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_String = static_cast<CHAOS_IL2CPP_UINT64>(1782325859292956794ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Type = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 3222174513575444759ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Type = static_cast<CHAOS_IL2CPP_UINT64>(3222174513575444759ULL);
inline constexpr CHAOS_IL2CPP_UINT64 chaos_boxed_type_id_Chaos_TestFramework_Sdk___0 = static_cast<CHAOS_IL2CPP_UINT64>(12016523716365422726ULL);

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException[] =
{
	{ 0x00000014u, reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c[] =
{
	{ 0u, nullptr },
	{ 0x00000031u, reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor) },
	{ 0x00000035u, reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte) },
	{ 0x00000033u, reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte) },
};

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c[] =
{
	{ 0u, nullptr },
	{ 0x00000039u, reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor) },
	{ 0u, nullptr },
	{ 0u, nullptr },
	{ 0x0000003Du, reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0) },
	{ 0x0000003Bu, reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0) },
};

bool chaos_is_array_store_compatible(const chaos_managed_array* chaos_array, CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if (chaos_array == nullptr)
	{
		return false;
	}

	if (chaos_value == 0)
	{
		return true;
	}

	// StringId fast path: materialized value carries a heap pointer;
	// this line is reached only if caller skipped materialization.
	if (chaos_is_string_id(chaos_value))
	{
		return chaos_array->element_type_shape == chaos_type_shape_reference            && chaos_is_type_compatible(chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot(), chaos_array->element_type_info);
	}

	auto* chaos_header = reinterpret_cast<ThinLockableHeader*>(chaos_value);
	if (chaos_array->element_type_shape == chaos_type_shape_interface)
	{
		return chaos_does_type_implement_interface(chaos_object_get_type_info(chaos_header), chaos_array->element_type_info);
	}

	if (chaos_array->element_type_shape == chaos_type_shape_reference)
	{
		return chaos_is_type_compatible(chaos_object_get_type_info(chaos_header), chaos_array->element_type_info);
	}

	return false;
}

const char* chaos_reflection_get_string_utf8(CHAOS_IL2CPP_INTPTR chaos_string_value) noexcept
{
	if (chaos_string_value == 0)
	{
		return nullptr;
	}

	if (chaos_is_string_id(chaos_string_value))
	{
		const auto chaos_view = chaos::il2cpp::string_table::Resolve(
			chaos_extract_string_id(chaos_string_value));
		return chaos_view.utf8_data;
	}

	auto* chaos_string = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_string_value);
	return chaos_string->utf8_data;
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_utf8_copy(const char* chaos_utf8_data, CHAOS_IL2CPP_SIZE chaos_length)
{
	if (chaos_utf8_data == nullptr)
	{
		return 0;
	}

	const auto chaos_id = chaos::il2cpp::string_table::Intern(
		chaos_utf8_data, static_cast<CHAOS_IL2CPP_UINT32>(chaos_length));
	return chaos_make_string_id_value(chaos_id);
}

CHAOS_IL2CPP_INTPTR ChaosReflectionConcatStringPairValues(CHAOS_IL2CPP_INTPTR chaos_left_string_value, CHAOS_IL2CPP_INTPTR chaos_right_string_value)
{
	const auto chaos_left_length =
		chaos_left_string_value == 0
			? static_cast<CHAOS_IL2CPP_SIZE>(0)
			: chaos_is_string_id(chaos_left_string_value)
				? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
					chaos_extract_string_id(chaos_left_string_value)).byte_count)
				: static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_left_string_value)->length);
	const auto chaos_right_length =
		chaos_right_string_value == 0
			? static_cast<CHAOS_IL2CPP_SIZE>(0)
			: chaos_is_string_id(chaos_right_string_value)
				? static_cast<CHAOS_IL2CPP_SIZE>(chaos::il2cpp::string_table::Resolve(
					chaos_extract_string_id(chaos_right_string_value)).byte_count)
				: static_cast<CHAOS_IL2CPP_SIZE>(reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_right_string_value)->length);
	const char* chaos_left_utf8 = chaos_reflection_get_string_utf8(chaos_left_string_value);
	const char* chaos_right_utf8 = chaos_reflection_get_string_utf8(chaos_right_string_value);
	if ((chaos_left_string_value != 0 && chaos_left_utf8 == nullptr && chaos_left_length != 0) ||
		(chaos_right_string_value != 0 && chaos_right_utf8 == nullptr && chaos_right_length != 0))
	{
		CHAOS_IL2CPP_FAIL();
	}

	const auto chaos_combined_length = chaos_left_length + chaos_right_length;
	auto* chaos_raw = static_cast<char*>(
		chaos::il2cpp::runtime_core::GcAllocateAtomic(
			sizeof(chaos_type_System_Private_CoreLib_System_String) + chaos_combined_length + 1));
	auto* chaos_str = reinterpret_cast<chaos_type_System_Private_CoreLib_System_String*>(chaos_raw);
	chaos_str->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
	chaos_str->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_combined_length);
	chaos_str->string_id = 0;
	chaos_str->utf8_data = chaos_raw + sizeof(chaos_type_System_Private_CoreLib_System_String);

	if (chaos_left_length != 0)
	{
		CHAOS_IL2CPP_MEMCPY(const_cast<char*>(chaos_str->utf8_data), chaos_left_utf8, chaos_left_length);
	}
	if (chaos_right_length != 0)
	{
		CHAOS_IL2CPP_MEMCPY(const_cast<char*>(chaos_str->utf8_data + chaos_left_length), chaos_right_utf8, chaos_right_length);
	}
	const_cast<char*>(chaos_str->utf8_data)[chaos_combined_length] = '\0';

	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_str);
}

CHAOS_IL2CPP_INTPTR chaos_reflection_create_string_literal(const char* chaos_utf8_data)
{
	if (chaos_utf8_data == nullptr)
	{
		return 0;
	}

	return chaos_reflection_create_string_utf8_copy(chaos_utf8_data, CHAOS_IL2CPP_STRLEN(chaos_utf8_data));
}


CHAOS_IL2CPP_INTPTR chaos_string_materialize(CHAOS_IL2CPP_INTPTR chaos_value) noexcept
{
	if (chaos_value == 0)
	{
		return 0;
	}

	if (!chaos_is_string_id(chaos_value))
	{
		return chaos_value;
	}

	const auto chaos_id = chaos_extract_string_id(chaos_value);
	const auto chaos_view = chaos::il2cpp::string_table::Resolve(chaos_id);

	auto* chaos_string = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_String, {});
	chaos_string->header.type_info = chaos_mt_System_Private_CoreLib_System_String.AsTypeInfoHot();
	chaos_string->length = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_view.byte_count);
	auto* owned_utf8 = static_cast<char*>(chaos::il2cpp::runtime_core::GcAllocateAtomic(chaos_view.byte_count + 1));
	CHAOS_IL2CPP_MEMCPY(owned_utf8, chaos_view.utf8_data, chaos_view.byte_count);
	owned_utf8[chaos_view.byte_count] = '\0';
	chaos_string->utf8_data = owned_utf8;
	chaos_string->string_id = chaos_id;
	return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_string);
}


// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Exception[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _message)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _innerException)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _stackTrace))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Delegate[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Delegate, chaos_delegate_target)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Delegate, chaos_delegate_invocation_list))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_MulticastDelegate[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_MulticastDelegate, chaos_delegate_target)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_MulticastDelegate, chaos_delegate_invocation_list))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};
static constexpr uint16_t kGcOffsets_chaos_type_System_Private_CoreLib_System_Type[] = {static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_type_handle)), static_cast<uint16_t>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	// Register managed_array (variable-size: header + contiguous element data).
	registry.Register(CHAOS_IL2CPP_UINT64(chaos_type_id_managed_array), sizeof(chaos_managed_array), nullptr, 0,
		static_cast<uint16_t>(sizeof(CHAOS_IL2CPP_INTPTR)),
		static_cast<uint16_t>(offsetof(chaos_managed_array, length)));

	registry.Register(12016523716365422726ULL, sizeof(chaos_type_Chaos_TestFramework_Sdk___0), nullptr, 0);
	registry.Register(3469195832277345062ULL, sizeof(chaos_type_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert), nullptr, 0);
	registry.Register(10972282733316558392ULL, sizeof(chaos_type_System_Private_CoreLib_System_Exception), kGcOffsets_chaos_type_System_Private_CoreLib_System_Exception, 3);
	registry.Register(8951892653230046377ULL, sizeof(chaos_type_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(1391641888938102389ULL, sizeof(chaos_type_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c), nullptr, 0);
	registry.Register(15317538086767515416ULL, sizeof(chaos_type_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c), nullptr, 0);
	registry.Register(7451128447593600616ULL, sizeof(chaos_type_System_Private_CoreLib_System_Delegate), kGcOffsets_chaos_type_System_Private_CoreLib_System_Delegate, 2);
	registry.Register(6681393039041505440ULL, sizeof(chaos_type_System_Private_CoreLib_System_MulticastDelegate), kGcOffsets_chaos_type_System_Private_CoreLib_System_MulticastDelegate, 2);
	registry.Register(14722794505377461205ULL, sizeof(chaos_type_System_Private_CoreLib_System_Action), nullptr, 0);
	registry.Register(8735361568882115811ULL, sizeof(chaos_type_System_Private_CoreLib_System_Action_SseItem_System_Int32__System_Buffers_IBufferWriter_System_Byte__), nullptr, 0);
	registry.Register(16749028324345562163ULL, sizeof(chaos_type_System_Private_CoreLib_System_Collections_IEnumerable), nullptr, 0);
	registry.Register(13420242832086098868ULL, sizeof(chaos_type_System_Private_CoreLib_System_IDisposable), nullptr, 0);
	registry.Register(2618932566973839939ULL, sizeof(chaos_type_System_Private_CoreLib_System_IO_Stream), nullptr, 0);
	registry.Register(5474029880995115448ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly, 1);
	registry.Register(17082367815459723707ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, 1);
	registry.Register(4197398748135124456ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodBase), nullptr, 0);
	registry.Register(4137207361503509124ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, 5);
	registry.Register(17040031516751226236ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, 2);
	registry.Register(10748947813473285525ULL, sizeof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo), kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, 5);
	registry.Register(1782325859292956794ULL, sizeof(chaos_type_System_Private_CoreLib_System_String), nullptr, 0);
	registry.Register(3222174513575444759ULL, sizeof(chaos_type_System_Private_CoreLib_System_Type), kGcOffsets_chaos_type_System_Private_CoreLib_System_Type, 2);

	// Register MethodTable address ranges for IsValidTypeInfoPointer.
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_Chaos_TestFramework_Sdk___0), reinterpret_cast<uintptr_t>(&chaos_mt_Chaos_TestFramework_Sdk___0) + sizeof(chaos_mt_Chaos_TestFramework_Sdk___0));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert), reinterpret_cast<uintptr_t>(&chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert) + sizeof(chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Exception), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Exception) + sizeof(chaos_mt_System_Private_CoreLib_System_Exception));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException), reinterpret_cast<uintptr_t>(&chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException) + sizeof(chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Object) + sizeof(chaos_mt_System_Private_CoreLib_System_Object));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c), reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c) + sizeof(chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c), reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c) + sizeof(chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Delegate), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Delegate) + sizeof(chaos_mt_System_Private_CoreLib_System_Delegate));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate) + sizeof(chaos_mt_System_Private_CoreLib_System_MulticastDelegate));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Action), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Action) + sizeof(chaos_mt_System_Private_CoreLib_System_Action));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Action_SseItem_System_Int32__System_Buffers_IBufferWriter_System_Byte__), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Action_SseItem_System_Int32__System_Buffers_IBufferWriter_System_Byte__) + sizeof(chaos_mt_System_Private_CoreLib_System_Action_SseItem_System_Int32__System_Buffers_IBufferWriter_System_Byte__));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Collections_IEnumerable), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Collections_IEnumerable) + sizeof(chaos_mt_System_Private_CoreLib_System_Collections_IEnumerable));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_IDisposable), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_IDisposable) + sizeof(chaos_mt_System_Private_CoreLib_System_IDisposable));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_IO_Stream), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_IO_Stream) + sizeof(chaos_mt_System_Private_CoreLib_System_IO_Stream));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type) + sizeof(chaos_mt_System_Private_CoreLib_System_Type));
}

CHAOS_IL2CPP_INT32 chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__s_exitCode = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c____9__6_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c____9__7_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c____9__0_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c____9__2_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_System_Private_CoreLib_System_IO_Stream__Null = 0;


	// AOT-baked string table: sorted by StringId for binary search at runtime.
	constexpr chaos::il2cpp::string_table::StringEntry chaos_aot_string_entries[] = {
		{ 1342797903967929121U, "[ASSERT FAIL] ", 14u },
		{ 6601085983368743141U, "null", 4u },
	};

	constexpr CHAOS_IL2CPP_UINT32 chaos_aot_string_entry_count = sizeof(chaos_aot_string_entries) / sizeof(chaos_aot_string_entries[0]);

	// Register the AOT-baked string table with the runtime before any code uses it.
	static const CHAOS_IL2CPP_UINT32 s_aot_string_table_registered = []()
	{
		::chaos::il2cpp::string_table::InitializeFromAot(
			chaos_aot_string_entries,
			chaos_aot_string_entry_count);
		return 0u;
	}();

CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Console_System_Console__get_Error_System_IO_TextWriter__(void) noexcept
{
	return ChaosConsoleGetError();
}

void chaos_external_runtime_System_Private_CoreLib_System_Exception___ctor_System_Void_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) noexcept
{
	ChaosReflectionSetExceptionMetadata(chaos_fn_arg_0, chaos_fn_arg_1);
}

void chaos_external_runtime_System_Private_CoreLib_System_IO_TextWriter__WriteLine_System_Void_System_String_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) noexcept
{
	ChaosTextWriterWriteLineStr(chaos_fn_arg_0, chaos_fn_arg_1);
}

CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Object__Equals_System_Boolean_System_Object_System_Object_(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1) noexcept
{
	return ChaosObjectEqualsStatic(chaos_fn_arg_0, chaos_fn_arg_1);
}

CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_Object__ToString_System_String__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) noexcept
{
	return ChaosObjectToString(chaos_fn_arg_0);
}

void chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) noexcept
{
	ChaosObjectCtor(chaos_fn_arg_0);
}

CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INTPTR chaos_arg_1) noexcept
{
	return ChaosReflectionConcatStringPairValues(chaos_arg_0, chaos_arg_1);
}

static constexpr CHAOS_IL2CPP_UINT32 kGenericTypeArgTokens[1] = { 0 };

static constexpr GenericTypeRegistrationEntryV0 kGenericTypeEntries[1] = { { 0, 0, 0, 0 } };

static constexpr CHAOS_IL2CPP_UINT32 kGenericMethodArgTokens[1] = { 0 };

static constexpr GenericMethodRegistrationEntryV0 kGenericMethodEntries[1] = { { 0, 0, 0, 0 } };

static constexpr GenericMethodAotEntryV0 s_method_aot_entries[1] = { { 0, 0, 0, 0 } };
static constexpr CHAOS_IL2CPP_UINT32 s_method_aot_entry_args[1] = { 0 };


// Forward declaration for module.image (defined in Step 3 below)
extern const ::chaos::il2cpp::runtime_core::ReflectionQueryImageDescriptor kReflImage;

// Namespace-scoped extern declaration for kAotMethodCount.
// (Definition at file scope via globalDeclarations for runtime-entry.cpp link-time visibility.)
extern "C" const int kAotMethodCount;
extern "C" int32_t kChaosExternalRuntimeCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[57] = {
	0u,
	3u,
	6u,
	9u,
	12u,
	15u,
	18u,
	21u,
	24u,
	27u,
	30u,
	33u,
	36u,
	39u,
	42u,
	45u,
	48u,
	51u,
	54u,
	54u,
	55u,
	55u,
	57u,
	59u,
	61u,
	63u,
	63u,
	63u,
	64u,
	65u,
	66u,
	67u,
	67u,
	67u,
	67u,
	67u,
	67u,
	67u,
	67u,
	67u,
	67u,
	67u,
	67u,
	67u,
	67u,
	67u,
	69u,
	71u,
	71u,
	71u,
	71u,
	71u,
	71u,
	71u,
	71u,
	71u,
	71u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[56];
	CHAOS_IL2CPP_UINT8 params[71];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		56u,
		71u,
		1678799594u,  // FNV-1a over entries+params
		s_abi_manifest_prefix_sum  // O(1) prefix-sum
	},
	{
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Boolean_System_Boolean_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_System_Byte_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_array_System_Byte_array_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Char_System_Char_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Decimal_System_Decimal_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Double_System_Double_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int16_System_Int16_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int32_System_Int32_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int64_System_Int64_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Object_System_Object_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_SByte_System_SByte_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Single_System_Single_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_String_System_String_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt16_System_UInt16_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt32_System_UInt32_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt64_System_UInt64_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_0_0_System_String
		{ 0u, 3u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreNotEqual_0_0_System_String
		{ 1u, 0u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete
		{ 0u, 1u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Fail_System_String
		{ 1u, 0u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_get_ExitCode
		{ 0u, 2u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsFalse_System_Boolean_System_String
		{ 0u, 2u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNotNull_System_Object_System_String
		{ 0u, 2u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNull_System_Object_System_String
		{ 0u, 2u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsTrue_System_Boolean_System_String
		{ 0u, 0u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_RecordFailure
		{ 0u, 0u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset
		{ 0u, 1u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action
		{ 0u, 1u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action
		{ 0u, 1u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_NullReferenceException_System_Action__generic
		{ 0u, 1u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String
		{ 0u, 0u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor
		{ 0u, 0u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor
		{ 0u, 0u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests__ctor
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_cctor
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor
		{ 0u, 2u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte
		{ 0u, 2u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests__ctor
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_Enumerate_0__0
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_EnumerateAsync_1_CancellationToken_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Enumerate_0__0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_EnumerateAsync_1_CancellationToken_0
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_cctor
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0
	},
	{
		2u,
		2u,
		2u,
		5u,
		5u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		9u,
		9u,
		2u,
		6u,
		6u,
		2u,
		1u,
		1u,
		2u,
		10u,
		10u,
		2u,
		2u,
		2u,
		2u,
		4u,
		4u,
		2u,
		8u,
		8u,
		2u,
		2u,
		2u,
		2u,
		7u,
		7u,
		2u,
		2u,
		2u,
		2u,
		11u,
		11u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
		2u,
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[5] = {
		0u,
		1536u,
		1536u,
		1544u,
		1544u,
	};

	static constexpr const char* s_type_names[5] = {
		"<Module>",
		"System_Net_ServerSentEvents_SseFormatterTests",
		"System_Net_ServerSentEvents_SseParserSystem_Int32Tests",
		"<>c",
		"<>c",
	};

	static constexpr const char* s_type_namespaces[5] = {
		"",
		"AutoGenerated.System_Net_ServerSentEvents",
		"AutoGenerated.System_Net_ServerSentEvents",
		"",
		"",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[5] = {
		0u,
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[5] = {
		nullptr,
		nullptr,
		nullptr,
		chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c.AsTypeInfoHot(),
		chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c.AsTypeInfoHot(),
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_children[2] = {
		33554436u,
		33554437u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[6] = {
		0u,
		0u,
		1u,
		2u,
		2u,
		2u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[6] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
	};



	static const ::chaos::il2cpp::runtime_core::ModuleDescriptor s_native_aot_module = {
		/* .name_utf8         = */ "CombinedSubjects",
		/* .image             = */ &kReflImage,
		/* .type_flags        = */ s_type_flags,
		/* .type_names        = */ s_type_names,
		/* .type_namespaces   = */ s_type_namespaces,
		/* .type_parent_tokens= */ s_type_parent_tokens,
		/* .type_info_ptrs    = */ s_type_info_ptrs,
		/* .nested_type_children= */ s_nested_type_children,
		/* .nested_type_offset = */ s_nested_type_offset,
		/* .generic_param_constraint_data= */ nullptr,
		/* .generic_param_constraint_offset= */ s_generic_param_constraint_offset,
		/* .type_count        = */ 5u,
	/* .custom_attribute_blob       = */ nullptr,  // Tier 1 -- deferred
	/* .custom_attribute_offset     = */ nullptr,
	/* .custom_attribute_entity_count = */ 0u,
	/* .custom_attribute_materializer = */ nullptr,
	/* .custom_attribute_method_offset   = */ nullptr,
	/* .custom_attribute_field_offset    = */ nullptr,
	/* .custom_attribute_property_offset = */ nullptr,
	/* .custom_attribute_param_offset    = */ nullptr,
	/* .custom_attribute_method_count    = */ 0u,
	/* .custom_attribute_field_count     = */ 0u,
	/* .custom_attribute_property_count  = */ 0u,
	/* .custom_attribute_param_count   = */ 0u,
		/* .abi_manifest      = */ s_abi_manifest,
	};
	static const CHAOS_IL2CPP_UINT32 s_native_aot_module_id =
		::chaos::il2cpp::runtime_core::RegisterModule("CombinedSubjects", &s_native_aot_module);
// ── Hotpatch name index + dispatch table ────────────────────
// Method name index entries
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[55] = {
	{ ".ctor", 0x00000023u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests
	{ "Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0", 0x00000015u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests
	{ "Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1", 0x00000019u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests
	{ "Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0", 0x0000001Du, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests
	{ "Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1", 0x00000021u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests
	{ "WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0", 0x00000013u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests
	{ "WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1", 0x00000017u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests
	{ "WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0", 0x0000001Bu, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests
	{ "WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1", 0x0000001Fu, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests
	{ ".cctor", 0x0000002Fu, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c
	{ ".ctor", 0x00000031u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c
	{ "<Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1>b__7_0", 0x00000035u, 2u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c
	{ "<WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1>b__6_0", 0x00000033u, 2u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c
	{ ".ctor", 0x0000002Du, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests
	{ "Benchmark_Enumerate_0__0", 0x00000027u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests
	{ "Benchmark_EnumerateAsync_1_CancellationToken_0", 0x0000002Bu, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests
	{ "Enumerate_0__0", 0x00000025u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests
	{ "EnumerateAsync_1_CancellationToken_0", 0x00000029u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests
	{ ".cctor", 0x00000037u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c
	{ ".ctor", 0x00000039u, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c
	{ "<Enumerate_0__0>b__0_0", 0x0000003Bu, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c
	{ "<EnumerateAsync_1_CancellationToken_0>b__2_0", 0x0000003Du, 0u },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c
	{ "AreEqual", 0x00000030u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x0000002Au, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000044u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x0000002Eu, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000036u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000034u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000026u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x0000001Eu, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000022u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x0000003Au, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x0000002Cu, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000032u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000038u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000028u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000020u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000024u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual`1", 0x00000043u, 3u },  // Chaos.TestFramework.Assert
	{ "AreNotEqual`1", 0x00000045u, 3u },  // Chaos.TestFramework.Assert
	{ "Complete", 0x0000001Au, 0u },  // Chaos.TestFramework.Assert
	{ "Fail", 0x00000046u, 1u },  // Chaos.TestFramework.Assert
	{ "get_ExitCode", 0x0000001Cu, 0u },  // Chaos.TestFramework.Assert
	{ "IsFalse", 0x00000040u, 2u },  // Chaos.TestFramework.Assert
	{ "IsNotNull", 0x0000003Eu, 2u },  // Chaos.TestFramework.Assert
	{ "IsNull", 0x0000003Cu, 2u },  // Chaos.TestFramework.Assert
	{ "IsTrue", 0x0000003Fu, 2u },  // Chaos.TestFramework.Assert
	{ "RecordFailure", 0x00000018u, 0u },  // Chaos.TestFramework.Assert
	{ "Reset", 0x00000016u, 0u },  // Chaos.TestFramework.Assert
	{ "Throws", 0x00000041u, 1u },  // Chaos.TestFramework.Assert
	{ "Throws<System.NullReferenceException>", 0x0000004Au, 1u },  // Chaos.TestFramework.Assert
	{ ".ctor", 0x00000014u, 1u },  // Chaos.TestFramework.AssertionException
	{ ".ctor", 0x00000047u, 0u },  // Chaos.TestFramework.BenchmarkAttribute
	{ ".ctor", 0x00000048u, 0u },  // Chaos.TestFramework.FactAttribute
	{ ".ctor", 0x00000049u, 0u },  // Chaos.TestFramework.HotUpdateAttribute
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[9] = {
	{ "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests", "AutoGenerated.System_Net_ServerSentEvents", 0u, 9u },
	{ "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c", "AutoGenerated.System_Net_ServerSentEvents", 9u, 4u },
	{ "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests", "AutoGenerated.System_Net_ServerSentEvents", 13u, 5u },
	{ "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c", "AutoGenerated.System_Net_ServerSentEvents", 18u, 4u },
	{ "Chaos.TestFramework.Assert", "Chaos.TestFramework", 22u, 29u },
	{ "Chaos.TestFramework.AssertionException", "Chaos.TestFramework", 51u, 1u },
	{ "Chaos.TestFramework.BenchmarkAttribute", "Chaos.TestFramework", 52u, 1u },
	{ "Chaos.TestFramework.FactAttribute", "Chaos.TestFramework", 53u, 1u },
	{ "Chaos.TestFramework.HotUpdateAttribute", "Chaos.TestFramework", 54u, 1u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[55] = {
	{ 0x00000013u, 38u },
	{ 0x00000014u, 29u },
	{ 0x00000015u, 34u },
	{ 0x00000016u, 26u },
	{ 0x00000017u, 39u },
	{ 0x00000018u, 25u },
	{ 0x00000019u, 35u },
	{ 0x0000001Au, 18u },
	{ 0x0000001Bu, 40u },
	{ 0x0000001Cu, 20u },
	{ 0x0000001Du, 36u },
	{ 0x0000001Eu, 7u },
	{ 0x0000001Fu, 41u },
	{ 0x00000020u, 14u },
	{ 0x00000021u, 37u },
	{ 0x00000022u, 8u },
	{ 0x00000023u, 33u },
	{ 0x00000024u, 15u },
	{ 0x00000025u, 49u },
	{ 0x00000026u, 6u },
	{ 0x00000027u, 47u },
	{ 0x00000028u, 13u },
	{ 0x00000029u, 50u },
	{ 0x0000002Au, 1u },
	{ 0x0000002Bu, 48u },
	{ 0x0000002Cu, 10u },
	{ 0x0000002Du, 46u },
	{ 0x0000002Eu, 3u },
	{ 0x0000002Fu, 42u },
	{ 0x00000030u, 0u },
	{ 0x00000031u, 43u },
	{ 0x00000032u, 11u },
	{ 0x00000033u, 45u },
	{ 0x00000034u, 5u },
	{ 0x00000035u, 44u },
	{ 0x00000036u, 4u },
	{ 0x00000037u, 51u },
	{ 0x00000038u, 12u },
	{ 0x00000039u, 52u },
	{ 0x0000003Au, 9u },
	{ 0x0000003Bu, 53u },
	{ 0x0000003Cu, 23u },
	{ 0x0000003Du, 54u },
	{ 0x0000003Eu, 22u },
	{ 0x0000003Fu, 24u },
	{ 0x00000040u, 21u },
	{ 0x00000041u, 27u },
	{ 0x00000043u, 16u },
	{ 0x00000044u, 2u },
	{ 0x00000045u, 17u },
	{ 0x00000046u, 19u },
	{ 0x00000047u, 30u },
	{ 0x00000048u, 31u },
	{ 0x00000049u, 32u },
	{ 0x0000004Au, 28u },
};

// Dispatch table (function pointers)
// s_hotpatch_entries — external linkage for verification_dispatch
// NOTE: `static` is deliberately omitted so that the verification dispatch
// TU (verification_dispatch.generated.cpp) can access this array via
// GetHotpatchEntries(). Without external linkage, the linker cannot resolve
// the symbol across translation units.
extern "C" HotpatchEntryV0 s_hotpatch_entries[55] = {
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Boolean_System_Boolean_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_System_Byte_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_array_System_Byte_array_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Char_System_Char_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Decimal_System_Decimal_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Double_System_Double_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int16_System_Int16_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int32_System_Int32_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int64_System_Int64_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Object_System_Object_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_SByte_System_SByte_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Single_System_Single_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_String_System_String_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt16_System_UInt16_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt32_System_UInt32_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt64_System_UInt64_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::AreEqual
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_0_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.Assert::AreEqual`1
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreNotEqual_0_0_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.Assert::AreNotEqual`1
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::Complete
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Fail_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.Assert::Fail
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_get_ExitCode), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::get_ExitCode
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsFalse_System_Boolean_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::IsFalse
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNotNull_System_Object_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::IsNotNull
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNull_System_Object_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::IsNull
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsTrue_System_Boolean_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::IsTrue
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_RecordFailure), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::RecordFailure
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // Chaos.TestFramework.Assert::Reset
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.Assert::Throws
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_NullReferenceException_System_Action__generic), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.Assert::Throws<System.NullReferenceException>
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.AssertionException::.ctor
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.BenchmarkAttribute::.ctor
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.FactAttribute::.ctor
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.HotUpdateAttribute::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::.cctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1>b__7_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1>b__6_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_Enumerate_0__0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::Benchmark_Enumerate_0__0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_EnumerateAsync_1_CancellationToken_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::Benchmark_EnumerateAsync_1_CancellationToken_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Enumerate_0__0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::Enumerate_0__0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_EnumerateAsync_1_CancellationToken_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::EnumerateAsync_1_CancellationToken_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_cctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::.cctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<Enumerate_0__0>b__0_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<EnumerateAsync_1_CancellationToken_0>b__2_0
};

// Accessor functions for verification_dispatch.generated.cpp
extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }
extern "C" int32_t GetHotpatchEntryCount() noexcept { return 55; }

// Module hotpatch bundle (extern linkage — referenced across page files)
extern constexpr HotpatchModuleV0 s_hotpatch_module = {
	"CombinedSubjects",
	s_hotpatch_types,
	9u,
	s_hotpatch_methods,
	55u,
	s_hotpatch_slots,
	55u,
	s_hotpatch_entries,
	55u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[42] = {
	"System.Private.CoreLib/System.Decimal::op_Inequality:System.Boolean(System.Decimal,System.Decimal)",
	"System.Private.CoreLib/System.Object::Equals:System.Boolean(System.Object,System.Object)",
	"System.Private.CoreLib/System.Object::ToString:System.String()",
	"System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)",
	"Chaos.TestFramework.Sdk/!!0",
	"System.Private.CoreLib/System.String",
	"System.Private.CoreLib/System.Collections.IEnumerable",
	"System.Private.CoreLib/System.Collections.IEnumerable::GetEnumerator:System.Collections.IEnumerator()",
	"System.Private.CoreLib/System.IDisposable",
	"System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()",
	"System.Private.CoreLib/System.Collections.Generic.EqualityComparer<System.Object>::get_Default:System.Collections.Generic.EqualityComparer<System.Object>()",
	"System.Private.CoreLib/System.Collections.IEnumerator::get_Current:System.Object()",
	"System.Private.CoreLib/System.Collections.Generic.EqualityComparer<System.Object>::Equals:System.Boolean(System.Object,System.Object)",
	"System.Private.CoreLib/System.IDisposable::Dispose:System.Void()",
	"System.Private.CoreLib/System.Collections.Generic.EqualityComparer<!!0>::get_Default:System.Collections.Generic.EqualityComparer<!!0>()",
	"System.Private.CoreLib/System.Collections.Generic.EqualityComparer<!!0>::Equals:System.Boolean(!!0,!!0)",
	"Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode",
	"System.Console/System.Console::get_Error:System.IO.TextWriter()",
	"System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)",
	"System.Private.CoreLib/System.IO.TextWriter::WriteLine:System.Void(System.String)",
	"System.Private.CoreLib/System.Action::Invoke:System.Void()",
	"System.Private.CoreLib/System.Exception::.ctor:System.Void(System.String)",
	"System.Private.CoreLib/System.Attribute::.ctor:System.Void()",
	"System.Private.CoreLib/System.Object::.ctor:System.Void()",
	"System.Private.CoreLib/System.Threading.CancellationToken",
	"System.Net.ServerSentEvents/SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable<SseItem<System.String>>,System.IO.Stream,System.Threading.CancellationToken)",
	"System.Linq.AsyncEnumerable/System.Linq.AsyncEnumerable::Empty<SseItem<System.String>>:System.Collections.Generic.IAsyncEnumerable<SseItem<System.String>>()",
	"System.Private.CoreLib/System.IO.Stream::Null",
	"System.Net.ServerSentEvents/SseFormatter::WriteAsync<System.Int32>:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable<SseItem<System.Int32>>,System.IO.Stream,System.Action<SseItem<System.Int32>,System.Buffers.IBufferWriter<System.Byte>>,System.Threading.CancellationToken)",
	"System.Linq.AsyncEnumerable/System.Linq.AsyncEnumerable::Empty<SseItem<System.Int32>>:System.Collections.Generic.IAsyncEnumerable<SseItem<System.Int32>>()",
	"CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<>9__7_0",
	"CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<>9",
	"System.Private.CoreLib/System.Action<SseItem<System.Int32>,System.Buffers.IBufferWriter<System.Byte>>::.ctor:System.Void(System.Object,System.IntPtr)",
	"System.Private.CoreLib/System.Threading.Tasks.Task::GetAwaiter:System.Runtime.CompilerServices.TaskAwaiter()",
	"System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter::GetResult:System.Void()",
	"CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<>9__6_0",
	"System.Net.ServerSentEvents/SseParser<System.Int32>::Enumerate:System.Collections.Generic.IEnumerable<SseItem<System.Int32>>()",
	"System.Net.ServerSentEvents/SseParser<System.Int32>::EnumerateAsync:System.Collections.Generic.IAsyncEnumerable<SseItem<System.Int32>>(System.Threading.CancellationToken)",
	"CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<>9__0_0",
	"CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<>9",
	"System.Private.CoreLib/System.Action::.ctor:System.Void(System.Object,System.IntPtr)",
	"CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<>9__2_0",
};

extern "C" void* kChaosExternalRuntimeFnTable[42] = {
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__Equals_System_Boolean_System_Object_System_Object_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object__ToString_System_String__),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Console_System_Console__get_Error_System_IO_TextWriter__),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_String__Concat_System_String_System_String_System_String_),
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_IO_TextWriter__WriteLine_System_Void_System_String_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Exception___ctor_System_Void_System_String_),
	nullptr,
	reinterpret_cast<void*>(&chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__),
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
};

extern "C" int32_t kChaosExternalRuntimeCount = 42;
// (no method AOT entries for this module)
// ── Method table (kMethodTable[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.
// NOTE: Emitted in ALL modes (AOT + JIT). JIT mode verification dispatch
// uses kDefaultArgThunks to bypass JIT precode trampoline (verification
// tests AOT codegen, not JIT compilation).
static void (*kMethodTable[56])() = {
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Boolean_System_Boolean_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_System_Byte_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_array_System_Byte_array_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Char_System_Char_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Decimal_System_Decimal_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Double_System_Double_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int16_System_Int16_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int32_System_Int32_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int64_System_Int64_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Object_System_Object_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_SByte_System_SByte_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Single_System_Single_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_String_System_String_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt16_System_UInt16_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt32_System_UInt32_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt64_System_UInt64_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_0_0_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreNotEqual_0_0_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Fail_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_get_ExitCode),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsFalse_System_Boolean_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNotNull_System_Object_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNull_System_Object_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsTrue_System_Boolean_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_RecordFailure),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_NullReferenceException_System_Action__generic),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_cctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_Enumerate_0__0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_EnumerateAsync_1_CancellationToken_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Enumerate_0__0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_EnumerateAsync_1_CancellationToken_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_cctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0),
};

// ── Default-arg thunks (kDefaultArgThunks[]) ────────────────────────────
// Each thunk supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_null_instance = 0;
extern "C" void (*kDefaultArgThunks[56])() = {
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[0])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[1])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[2])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[3])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[4])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[5])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[6])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[7])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[8])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[9])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[10])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[11])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[12])(chaos_make_string_id_value(6601085983368743141ULL),chaos_make_string_id_value(6601085983368743141ULL),chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[13])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[14])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[15])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[16])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[17])(0,0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {kMethodTable[18]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[19])(chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {kMethodTable[20]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[21])(0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[22])(0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[23])(0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[24])(0,chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {kMethodTable[25]();},
	[]() {kMethodTable[26]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[27])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[28])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[29])(0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[30])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[31])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[32])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[33])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[34])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {kMethodTable[35]();},
	[]() {kMethodTable[36]();},
	[]() {kMethodTable[37]();},
	[]() {kMethodTable[38]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[39])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[40])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[41])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[42])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {kMethodTable[43]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[44])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[45])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[46])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),0,0);},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[47])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {kMethodTable[48]();},
	[]() {kMethodTable[49]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[50])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[51])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {kMethodTable[52]();},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[53])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[54])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[55])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
};

// ── Subject slot map (kSubjectSlotMap[]) ────────────────────────────
// Maps subject index (0-based sequential) to kMethodTable index.
// External consumers use this to route --benchmark N to the correct
// AOT method slot, since kMethodTable[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 6;
extern "C" const int kSubjectSlotMap[6] = {
	39,
	40,
	41,
	42,
	50,
	51,
};

// ── Subject contract map (kSubjectContractMap[]) ───────────────────────
// Maps subject slot index (si) to contract index (position in contract list).
// Used by fact-json output to match native results against golden records
// without assuming slot layout.
extern "C" const int kSubjectContractMap[6] = {
	0,
	1,
	2,
	3,
	4,
	5,
};
// ── GC Slot Map Section ───────────────────────────────────────────
// Auto-generated by chaos-il2cpp codegen for precise stack root scanning.
// One nested struct entry per method with GC-referencing stack slots.
// Iterated by GcRegisterSlotMapsFromSection() which advances by
// entry_total_size bytes per entry.
#if defined(_MSC_VER)
#pragma pack(push, 1)
#endif
#if defined(__GNUC__) || defined(__clang__)
static const struct __attribute__((packed)) {
#else
static const struct {
#endif

	/* ── Entry 0: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Boolean_System_Boolean_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry0;
	/* ── Entry 1: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_System_Byte_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry1;
	/* ── Entry 2: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_array_System_Byte_array_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[12];
	} entry2;
	/* ── Entry 3: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Char_System_Char_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry3;
	/* ── Entry 4: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Decimal_System_Decimal_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry4;
	/* ── Entry 5: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Double_System_Double_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry5;
	/* ── Entry 6: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int16_System_Int16_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry6;
	/* ── Entry 7: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int32_System_Int32_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry7;
	/* ── Entry 8: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int64_System_Int64_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry8;
	/* ── Entry 9: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Object_System_Object_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry9;
	/* ── Entry 10: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_SByte_System_SByte_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry10;
	/* ── Entry 11: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Single_System_Single_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry11;
	/* ── Entry 12: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_String_System_String_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[6];
	} entry12;
	/* ── Entry 13: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt16_System_UInt16_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry13;
	/* ── Entry 14: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt32_System_UInt32_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry14;
	/* ── Entry 15: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt64_System_UInt64_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry15;
	/* ── Entry 16: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_0_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[23];
	} entry16;
	/* ── Entry 17: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreNotEqual_0_0_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry17;
	/* ── Entry 18: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry18;
	/* ── Entry 19: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Fail_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry19;
	/* ── Entry 20: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_get_ExitCode ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry20;
	/* ── Entry 21: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsFalse_System_Boolean_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry21;
	/* ── Entry 22: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNotNull_System_Object_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry22;
	/* ── Entry 23: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNull_System_Object_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry23;
	/* ── Entry 24: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsTrue_System_Boolean_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry24;
	/* ── Entry 25: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_RecordFailure ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry25;
	/* ── Entry 26: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry26;
	/* ── Entry 27: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry27;
	/* ── Entry 28: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry28;
	/* ── Entry 29: Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_NullReferenceException_System_Action__generic ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry29;
	/* ── Entry 30: Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry30;
	/* ── Entry 31: Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry31;
	/* ── Entry 32: Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry32;
	/* ── Entry 33: Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry33;
	/* ── Entry 34: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry34;
	/* ── Entry 35: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry35;
	/* ── Entry 36: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry36;
	/* ── Entry 37: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry37;
	/* ── Entry 38: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[5];
	} entry38;
	/* ── Entry 39: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry39;
	/* ── Entry 40: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[7];
	} entry40;
	/* ── Entry 41: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry41;
	/* ── Entry 42: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[8];
	} entry42;
	/* ── Entry 43: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry43;
	/* ── Entry 44: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry44;
	/* ── Entry 45: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry45;
	/* ── Entry 46: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry46;
	/* ── Entry 47: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry47;
	/* ── Entry 48: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_Enumerate_0__0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry48;
	/* ── Entry 49: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_EnumerateAsync_1_CancellationToken_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry49;
	/* ── Entry 50: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Enumerate_0__0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry50;
	/* ── Entry 51: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_EnumerateAsync_1_CancellationToken_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry51;
	/* ── Entry 52: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_cctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[1];
	} entry52;
	/* ── Entry 53: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry53;
	/* ── Entry 54: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry54;
	/* ── Entry 55: CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry55;
} kChaosGcSlotMapsSection = {
	/* entry0 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Boolean_System_Boolean_System_String */
	.entry0 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Boolean_System_Boolean_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry1 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_System_Byte_System_String */
	.entry1 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_System_Byte_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry2 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_array_System_Byte_array_System_String */
	.entry2 = {
		/* entry_total_size = 68 */ 68u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_array_System_Byte_array_System_String),
		/* frame_size = 96 */ 96u,
		/* num_gc_slots = 12 */ 12u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u }
	},
	/* entry3 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Char_System_Char_System_String */
	.entry3 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Char_System_Char_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry4 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Decimal_System_Decimal_System_String */
	.entry4 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Decimal_System_Decimal_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry5 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Double_System_Double_System_String */
	.entry5 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Double_System_Double_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry6 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int16_System_Int16_System_String */
	.entry6 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int16_System_Int16_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry7 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int32_System_Int32_System_String */
	.entry7 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int32_System_Int32_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry8 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int64_System_Int64_System_String */
	.entry8 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int64_System_Int64_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry9 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Object_System_Object_System_String */
	.entry9 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Object_System_Object_System_String),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry10 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_SByte_System_SByte_System_String */
	.entry10 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_SByte_System_SByte_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry11 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Single_System_Single_System_String */
	.entry11 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Single_System_Single_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry12 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_String_System_String_System_String */
	.entry12 = {
		/* entry_total_size = 44 */ 44u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_String_System_String_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 6 */ 6u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u }
	},
	/* entry13 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt16_System_UInt16_System_String */
	.entry13 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt16_System_UInt16_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry14 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt32_System_UInt32_System_String */
	.entry14 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt32_System_UInt32_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry15 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt64_System_UInt64_System_String */
	.entry15 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt64_System_UInt64_System_String),
		/* frame_size = 48 */ 48u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 16u, 24u, 32u, 40u }
	},
	/* entry16 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_0_0_System_String */
	.entry16 = {
		/* entry_total_size = 112 */ 112u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_0_0_System_String),
		/* frame_size = 184 */ 184u,
		/* num_gc_slots = 23 */ 23u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u, 64u, 72u, 80u, 88u, 96u, 104u, 112u, 120u, 128u, 136u, 144u, 152u, 160u, 168u, 176u }
	},
	/* entry17 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreNotEqual_0_0_System_String */
	.entry17 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreNotEqual_0_0_System_String),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry18 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete */
	.entry18 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry19 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Fail_System_String */
	.entry19 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Fail_System_String),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry20 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_get_ExitCode */
	.entry20 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_get_ExitCode),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry21 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsFalse_System_Boolean_System_String */
	.entry21 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsFalse_System_Boolean_System_String),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 8u, 16u, 24u }
	},
	/* entry22 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNotNull_System_Object_System_String */
	.entry22 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNotNull_System_Object_System_String),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry23 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNull_System_Object_System_String */
	.entry23 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNull_System_Object_System_String),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry24 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsTrue_System_Boolean_System_String */
	.entry24 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsTrue_System_Boolean_System_String),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 8u, 16u, 24u, 32u }
	},
	/* entry25 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_RecordFailure */
	.entry25 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_RecordFailure),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry26 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset */
	.entry26 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry27 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action */
	.entry27 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry28 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action */
	.entry28 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry29 = Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_NullReferenceException_System_Action__generic */
	.entry29 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_NullReferenceException_System_Action__generic),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry30 = Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String */
	.entry30 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry31 = Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor */
	.entry31 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry32 = Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor */
	.entry32 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry33 = Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor */
	.entry33 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry34 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests__ctor */
	.entry34 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry35 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0 */
	.entry35 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry36 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1 */
	.entry36 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry37 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0 */
	.entry37 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry38 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1 */
	.entry38 = {
		/* entry_total_size = 40 */ 40u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1),
		/* frame_size = 40 */ 40u,
		/* num_gc_slots = 5 */ 5u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u }
	},
	/* entry39 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0 */
	.entry39 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry40 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1 */
	.entry40 = {
		/* entry_total_size = 48 */ 48u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1),
		/* frame_size = 56 */ 56u,
		/* num_gc_slots = 7 */ 7u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u }
	},
	/* entry41 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0 */
	.entry41 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry42 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1 */
	.entry42 = {
		/* entry_total_size = 52 */ 52u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1),
		/* frame_size = 64 */ 64u,
		/* num_gc_slots = 8 */ 8u,
		/* slots */ { 0u, 8u, 16u, 24u, 32u, 40u, 48u, 56u }
	},
	/* entry43 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_cctor */
	.entry43 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry44 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor */
	.entry44 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry45 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte */
	.entry45 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry46 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte */
	.entry46 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry47 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests__ctor */
	.entry47 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry48 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_Enumerate_0__0 */
	.entry48 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_Enumerate_0__0),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry49 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_EnumerateAsync_1_CancellationToken_0 */
	.entry49 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_EnumerateAsync_1_CancellationToken_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry50 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Enumerate_0__0 */
	.entry50 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Enumerate_0__0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry51 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_EnumerateAsync_1_CancellationToken_0 */
	.entry51 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_EnumerateAsync_1_CancellationToken_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry52 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_cctor */
	.entry52 = {
		/* entry_total_size = 24 */ 24u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_cctor),
		/* frame_size = 8 */ 8u,
		/* num_gc_slots = 1 */ 1u,
		/* slots */ { 0u }
	},
	/* entry53 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor */
	.entry53 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry54 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0 */
	.entry54 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry55 = CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0 */
	.entry55 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 2072u;

// ── Bridge/import thunks ──
extern "C" void* kChaosExternalRuntimeFnTable[];

// Bridge/import thunk for: System.Private.CoreLib/System.Decimal::op_Inequality:System.Boolean(System.Decimal,System.Decimal)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[0])(chaos_fn_arg_0, chaos_fn_arg_1);
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.String::op_Inequality:System.Boolean(System.String,System.String)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[3])(chaos_fn_arg_0, chaos_fn_arg_1);
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.Collections.Generic.EqualityComparer<System.Object>::Equals:System.Boolean(System.Object,System.Object)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_10(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[12])(chaos_fn_arg_0, chaos_fn_arg_1);
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.IDisposable::Dispose:System.Void()
extern "C" void chaos_bridge_thunk_11(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[13])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Collections.Generic.EqualityComparer<!!0>::get_Default:System.Collections.Generic.EqualityComparer<!!0>()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_12(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[14])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.Collections.Generic.EqualityComparer<!!0>::Equals:System.Boolean(!!0,!!0)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_13(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[15])(chaos_fn_arg_0, chaos_fn_arg_1);
	return result;
}

// Bridge/import thunk for: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode
extern "C" void chaos_bridge_thunk_14(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[16])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Action::Invoke:System.Void()
extern "C" void chaos_bridge_thunk_15(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[20])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Attribute::.ctor:System.Void()
extern "C" void chaos_bridge_thunk_16(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[22])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Threading.CancellationToken
extern "C" void chaos_bridge_thunk_17(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[24])();
}

// Bridge/import thunk for: System.Net.ServerSentEvents/SseFormatter::WriteAsync:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable<SseItem<System.String>>,System.IO.Stream,System.Threading.CancellationToken)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_18(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
	return result;
}

// Bridge/import thunk for: System.Linq.AsyncEnumerable/System.Linq.AsyncEnumerable::Empty<SseItem<System.String>>:System.Collections.Generic.IAsyncEnumerable<SseItem<System.String>>()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_19(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[26])();
	return result;
}

// Bridge/import thunk for: Chaos.TestFramework.Sdk/!!0
extern "C" void chaos_bridge_thunk_2(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[4])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.IO.Stream::Null
extern "C" void chaos_bridge_thunk_20(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[27])();
}

// Bridge/import thunk for: System.Net.ServerSentEvents/SseFormatter::WriteAsync<System.Int32>:System.Threading.Tasks.Task(System.Collections.Generic.IAsyncEnumerable<SseItem<System.Int32>>,System.IO.Stream,System.Action<SseItem<System.Int32>,System.Buffers.IBufferWriter<System.Byte>>,System.Threading.CancellationToken)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_21(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2, CHAOS_IL2CPP_INTPTR chaos_fn_arg_3)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2, chaos_fn_arg_3);
	return result;
}

// Bridge/import thunk for: System.Linq.AsyncEnumerable/System.Linq.AsyncEnumerable::Empty<SseItem<System.Int32>>:System.Collections.Generic.IAsyncEnumerable<SseItem<System.Int32>>()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_22(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[29])();
	return result;
}

// Bridge/import thunk for: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<>9__7_0
extern "C" void chaos_bridge_thunk_23(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[30])();
}

// Bridge/import thunk for: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<>9
extern "C" void chaos_bridge_thunk_24(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[31])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Action<SseItem<System.Int32>,System.Buffers.IBufferWriter<System.Byte>>::.ctor:System.Void(System.Object,System.IntPtr)
extern "C" void chaos_bridge_thunk_25(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[32])(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Bridge/import thunk for: System.Private.CoreLib/System.Threading.Tasks.Task::GetAwaiter:System.Runtime.CompilerServices.TaskAwaiter()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_26(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[33])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter::GetResult:System.Void()
extern "C" void chaos_bridge_thunk_27(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[34])();
}

// Bridge/import thunk for: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<>9__6_0
extern "C" void chaos_bridge_thunk_28(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[35])();
}

// Bridge/import thunk for: System.Net.ServerSentEvents/SseParser<System.Int32>::Enumerate:System.Collections.Generic.IEnumerable<SseItem<System.Int32>>()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_29(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[36])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.String
extern "C" void chaos_bridge_thunk_3(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[5])();
}

// Bridge/import thunk for: System.Net.ServerSentEvents/SseParser<System.Int32>::EnumerateAsync:System.Collections.Generic.IAsyncEnumerable<SseItem<System.Int32>>(System.Threading.CancellationToken)
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_30(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[37])(chaos_fn_arg_0);
	return result;
}

// Bridge/import thunk for: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<>9__0_0
extern "C" void chaos_bridge_thunk_31(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[38])();
}

// Bridge/import thunk for: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<>9
extern "C" void chaos_bridge_thunk_32(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[39])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Action::.ctor:System.Void(System.Object,System.IntPtr)
extern "C" void chaos_bridge_thunk_33(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1)
{
	reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[40])(chaos_fn_arg_0, chaos_fn_arg_1);
}

// Bridge/import thunk for: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<>9__2_0
extern "C" void chaos_bridge_thunk_34(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[41])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Collections.IEnumerable
extern "C" void chaos_bridge_thunk_4(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[6])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Collections.IEnumerable::GetEnumerator:System.Collections.IEnumerator()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_5(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[7])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.IDisposable
extern "C" void chaos_bridge_thunk_6(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[8])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.Collections.IEnumerator::MoveNext:System.Boolean()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_7(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[9])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.Collections.Generic.EqualityComparer<System.Object>::get_Default:System.Collections.Generic.EqualityComparer<System.Object>()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_8(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[10])();
	return result;
}

// Bridge/import thunk for: System.Private.CoreLib/System.Collections.IEnumerator::get_Current:System.Object()
extern "C" CHAOS_IL2CPP_INTPTR chaos_bridge_thunk_9(void)
{
	auto result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)()>(kChaosExternalRuntimeFnTable[11])();
	return result;
}

// ── CodeRegistrationV0 ─────────────────────────────────────────
// method_pointers: flat array of all AOT function pointers.
static void* const kMethodPointers[56] = {
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Boolean_System_Boolean_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_System_Byte_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Byte_array_System_Byte_array_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Char_System_Char_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Decimal_System_Decimal_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Double_System_Double_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int16_System_Int16_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int32_System_Int32_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Int64_System_Int64_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Object_System_Object_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_SByte_System_SByte_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Single_System_Single_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_String_System_String_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt16_System_UInt16_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt32_System_UInt32_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_UInt64_System_UInt64_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_0_0_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreNotEqual_0_0_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Complete),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Fail_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_get_ExitCode),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsFalse_System_Boolean_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNotNull_System_Object_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsNull_System_Object_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_IsTrue_System_Boolean_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_RecordFailure),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Reset),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_Action),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_NullReferenceException_System_Action__generic),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_cctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_Enumerate_0__0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_EnumerateAsync_1_CancellationToken_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Enumerate_0__0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_EnumerateAsync_1_CancellationToken_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_cctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0),
};

// CodeRegistrationV0 struct (invoker_pointers = nullptr for native-aot path)
// ── VTable descriptors (for BootstrapRuntime TypeVTable registration) ──
static const VTableDescriptorV0 kChaosVTableDescriptors[] = {
	{
		CHAOS_IL2CPP_UINT64_C(0x7C3B82F05F34BCA9),
		0x00000003u,
		0u,
		1u,
		reinterpret_cast<const void*>(kSlots_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException),
		reinterpret_cast<const void**>(chaos_vtable_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException),
		1u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0x13501AECBCB58E75),
		0x00000006u,
		0u,
		4u,
		reinterpret_cast<const void*>(kSlots_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c),
		reinterpret_cast<const void**>(chaos_vtable_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c),
		4u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
	{
		CHAOS_IL2CPP_UINT64_C(0xD492D3BBEF4B5718),
		0x00000007u,
		0u,
		6u,
		reinterpret_cast<const void*>(kSlots_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c),
		reinterpret_cast<const void**>(chaos_vtable_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c),
		6u,
		1,
		{0, 0, 0},
		nullptr,
		0u,
	},
};
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 56u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 3u,
	.slot_map_section_begin = reinterpret_cast<const void*>(&kChaosGcSlotMapsSection),
	.slot_map_section_end   = reinterpret_cast<const void*>(
		reinterpret_cast<CHAOS_IL2CPP_UINTPTR>(&kChaosGcSlotMapsSection) + kChaosGcSlotMapsSize),
};

// MetadataRegistrationV0
extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration
	= {
	.struct_size              = sizeof(MetadataRegistrationV0),
	.generic_types            = kGenericTypeEntries,
	.generic_type_count       = sizeof(kGenericTypeEntries) / sizeof(kGenericTypeEntries[0]),
	.generic_type_args        = kGenericTypeArgTokens,
	.generic_type_arg_count   = sizeof(kGenericTypeArgTokens) / sizeof(kGenericTypeArgTokens[0]),
	.generic_methods          = kGenericMethodEntries,
	.generic_method_count     = sizeof(kGenericMethodEntries) / sizeof(kGenericMethodEntries[0]),
	.generic_method_args      = kGenericMethodArgTokens,
	.generic_method_arg_count = sizeof(kGenericMethodArgTokens) / sizeof(kGenericMethodArgTokens[0]),
	.method_aot_entries       = s_method_aot_entries,
	.method_aot_entry_count  = sizeof(s_method_aot_entries) / sizeof(s_method_aot_entries[0]),
	.method_aot_entry_args    = s_method_aot_entry_args,
	.method_aot_entry_arg_count = sizeof(s_method_aot_entry_args) / sizeof(s_method_aot_entry_args[0]),
	.field_offsets           = nullptr,
	.field_offset_count      = 0u,
	.metadata_usages         = nullptr,
	.metadata_usage_count    = 0u,
};

// CodegenRegistrationOptionsV0
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options
	= {
	.struct_size       = sizeof(CodegenRegistrationOptionsV0),
	.registration_flags = 0u,
	.image_name_utf8    = "CombinedSubjects",
};
// ── Reflection Query Image Descriptor ──────────────────────────
// Used by ResolveSubjectId to resolve call_target via subjectId
// matching during IR lowering of patched methods.

static constexpr ReflectionQueryFieldDescriptor kReflFields_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert[1] = {
	{ 0x04000001u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::s_exitCode", "s_exitCode", "System.Int32", 0LL, 2u },
};
static constexpr ReflectionQueryMethodDescriptor kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert[30] = {
	{ 0x00000030u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Boolean,System.Boolean,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Au, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Byte,System.Byte,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000044u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Byte[],System.Byte[],System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Eu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Char,System.Char,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000036u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Decimal,System.Decimal,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000034u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Double,System.Double,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000026u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int16,System.Int16,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Eu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int32,System.Int32,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000022u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int64,System.Int64,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Au, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Object,System.Object,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Cu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.SByte,System.SByte,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000032u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Single,System.Single,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000038u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.String,System.String,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000028u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt16,System.UInt16,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000020u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt32,System.UInt32,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000024u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt64,System.UInt64,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000043u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual`1:System.Void(!!0,!!0,System.String)", "AreEqual`1", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000045u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreNotEqual`1:System.Void(!!0,!!0,System.String)", "AreNotEqual`1", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Au, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Complete:System.Int32()", "Complete", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000046u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Fail:System.Void(System.String)", "Fail", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Cu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::get_ExitCode:System.Int32()", "get_ExitCode", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000040u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsFalse:System.Void(System.Boolean,System.String)", "IsFalse", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Eu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNotNull:System.Void(System.Object,System.String)", "IsNotNull", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Cu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNull:System.Void(System.Object,System.String)", "IsNull", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Fu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsTrue:System.Void(System.Boolean,System.String)", "IsTrue", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000018u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::RecordFailure:System.Void()", "RecordFailure", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000016u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Reset:System.Void()", "Reset", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000041u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action)", "Throws", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000042u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws`1:System.Void(System.Action)", "Throws`1", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x0000004Au, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws<System.NullReferenceException>:System.Void(System.Action)", "Throws<System.NullReferenceException>", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException[1] = {
	{ 0x00000014u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException::.ctor:System.Void(System.String)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute[1] = {
	{ 0x00000047u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute[1] = {
	{ 0x00000048u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute[1] = {
	{ 0x00000049u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests[9] = {
	{ 0x00000023u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000015u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0:System.Void()", "Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000019u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1:System.Void()", "Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Du, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0:System.Void()", "Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000021u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1:System.Void()", "Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000013u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0:System.Int64()", "WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000017u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1:System.Int64()", "WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Bu, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0:System.Int64()", "WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Fu, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1:System.Int64()", "WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c[4] = {
	{ 0x0000002Fu, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000031u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000035u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1>b__7_0:System.Void(SseItem<System.Int32>,System.Buffers.IBufferWriter<System.Byte>)", "<Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1>b__7_0", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000033u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1>b__6_0:System.Void(SseItem<System.Int32>,System.Buffers.IBufferWriter<System.Byte>)", "<WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1>b__6_0", "System.Void", 2, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests[5] = {
	{ 0x0000002Du, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000027u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::Benchmark_Enumerate_0__0:System.Void()", "Benchmark_Enumerate_0__0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Bu, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::Benchmark_EnumerateAsync_1_CancellationToken_0:System.Void()", "Benchmark_EnumerateAsync_1_CancellationToken_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000025u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::Enumerate_0__0:System.Int64()", "Enumerate_0__0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000029u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::EnumerateAsync_1_CancellationToken_0:System.Int64()", "EnumerateAsync_1_CancellationToken_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c[4] = {
	{ 0x00000037u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::.cctor:System.Void()", ".cctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000039u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Bu, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<Enumerate_0__0>b__0_0:System.Void()", "<Enumerate_0__0>b__0_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Du, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<EnumerateAsync_1_CancellationToken_0>b__2_0:System.Void()", "<EnumerateAsync_1_CancellationToken_0>b__2_0", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[9] = {
	{ 0x00000004u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert", "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert", "Chaos.TestFramework", "Chaos.TestFramework.Assert", "Chaos.TestFramework.Assert", nullptr, kReflFields_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert, 30u, nullptr, 0u, 0u },
	{ 0x00000003u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException", "Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException", "Chaos.TestFramework", "Chaos.TestFramework.AssertionException", "Chaos.TestFramework.AssertionException", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException, 1u, nullptr, 0u, 0u },
	{ 0x00000008u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute", "Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute", "Chaos.TestFramework", "Chaos.TestFramework.BenchmarkAttribute", "Chaos.TestFramework.BenchmarkAttribute", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute, 1u, nullptr, 0u, 0u },
	{ 0x00000009u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute", "Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute", "Chaos.TestFramework", "Chaos.TestFramework.FactAttribute", "Chaos.TestFramework.FactAttribute", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute, 1u, nullptr, 0u, 0u },
	{ 0x0000000Au, "Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute", "Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute", "Chaos.TestFramework", "Chaos.TestFramework.HotUpdateAttribute", "Chaos.TestFramework.HotUpdateAttribute", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute, 1u, nullptr, 0u, 0u },
	{ 0x00000002u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests", "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests", "AutoGenerated.System_Net_ServerSentEvents", "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests", "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests, 9u, nullptr, 0u, 0u },
	{ 0x00000006u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c", "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c", "AutoGenerated.System_Net_ServerSentEvents", "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c", "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c, 4u, nullptr, 0u, 0u },
	{ 0x00000005u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests", "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests", "AutoGenerated.System_Net_ServerSentEvents", "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests", "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests, 5u, nullptr, 0u, 0u },
	{ 0x00000007u, "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c", "CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c", "AutoGenerated.System_Net_ServerSentEvents", "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c", "AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c, 4u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[9] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
	&kReflTypes[5],
	&kReflTypes[6],
	&kReflTypes[7],
	&kReflTypes[8],
};

extern const ReflectionQueryImageDescriptor kReflImage = { "CombinedSubjects", kReflTypePtrs, 9u, 1, 0, 0, 0 };

// Fake ImageHandle that ResolveSubjectId will decode back to kReflImage.
// BootstrapRuntime's aot_image_handle fallback discovers this via
// LookupModule(mid)->image at lines 311-321 of bootstrap.cpp.
	// Register generated enum metadata (tables + dispatch + type descriptors).
	// ChaosRegisterEnumGeneratedMetadata is defined in enum_metadata.generated.h.
	static const CHAOS_IL2CPP_UINT32 s_enum_registered = []() noexcept {
		ChaosRegisterEnumGeneratedMetadata();
		return 1u;
	}();

// ── Generic registration proof-host helper ─────────────────
// Populate generic registration arrays from this TU.
static void ChaosDoPopulateGenericRegistration(
	CHAOS_IL2CPP_UINT32* out_type_count,
	const GenericTypeRegistrationEntryV0** out_type_entries,
	const CHAOS_IL2CPP_UINT32** out_type_args,
	CHAOS_IL2CPP_UINT32* out_type_arg_count,
	CHAOS_IL2CPP_UINT32* out_method_count,
	const GenericMethodRegistrationEntryV0** out_method_entries,
	const CHAOS_IL2CPP_UINT32** out_method_args,
	CHAOS_IL2CPP_UINT32* out_method_arg_count)
{
	*out_type_count = 0;
	*out_type_entries = kGenericTypeEntries;
	*out_type_args = kGenericTypeArgTokens;
	*out_type_arg_count = 0;
	*out_method_count = 0;
	*out_method_entries = kGenericMethodEntries;
	*out_method_args = kGenericMethodArgTokens;
	*out_method_arg_count = 0;
}

// Static initializer registers the callback.
extern "C" void (*g_chaos_populate_generic_registration)(
	CHAOS_IL2CPP_UINT32*, const GenericTypeRegistrationEntryV0**, const CHAOS_IL2CPP_UINT32**, CHAOS_IL2CPP_UINT32*,
	CHAOS_IL2CPP_UINT32*, const GenericMethodRegistrationEntryV0**, const CHAOS_IL2CPP_UINT32**, CHAOS_IL2CPP_UINT32*);
namespace {
struct ChaosGenericRegistrationInit {
	ChaosGenericRegistrationInit() {
		g_chaos_populate_generic_registration =
			&ChaosDoPopulateGenericRegistration;
	}
} g_chaos_reg_init;
}
// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual(System.Boolean,System.Boolean,System.String)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_AreEqual_System_Boolean_System_Boolean_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
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
		if (0 < 0 || 0 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 0 out of range
		}
		if (kChaosExternalRuntimeFnTable[0] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 0 is null
		}
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
		if (3 < 0 || 3 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 3 out of range
		}
		if (kChaosExternalRuntimeFnTable[3] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 3 is null
		}
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
		if (7 < 0 || 7 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 7 out of range
		}
		if (kChaosExternalRuntimeFnTable[7] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 7 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
		_s12 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[5] = _s12;
	_s12 = chaos_locals[1];
	{
		if (7 < 0 || 7 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 7 out of range
		}
		if (kChaosExternalRuntimeFnTable[7] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 7 is null
		}
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
						if (13 < 0 || 13 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL(); // external runtime table index 13 out of range
						}
						if (kChaosExternalRuntimeFnTable[13] == nullptr) {
							CHAOS_IL2CPP_FAIL(); // external runtime table entry 13 is null
						}
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
								if (13 < 0 || 13 >= kChaosExternalRuntimeCount) {
									CHAOS_IL2CPP_FAIL(); // external runtime table index 13 out of range
								}
								if (kChaosExternalRuntimeFnTable[13] == nullptr) {
									CHAOS_IL2CPP_FAIL(); // external runtime table entry 13 is null
								}
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
						if (9 < 0 || 9 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL(); // external runtime table index 9 out of range
						}
						if (kChaosExternalRuntimeFnTable[9] == nullptr) {
							CHAOS_IL2CPP_FAIL(); // external runtime table entry 9 is null
						}
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
						if (10 < 0 || 10 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL(); // external runtime table index 10 out of range
						}
						if (kChaosExternalRuntimeFnTable[10] == nullptr) {
							CHAOS_IL2CPP_FAIL(); // external runtime table entry 10 is null
						}
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
						_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s16 = chaos_locals[5];
					{
						if (11 < 0 || 11 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL(); // external runtime table index 11 out of range
						}
						if (kChaosExternalRuntimeFnTable[11] == nullptr) {
							CHAOS_IL2CPP_FAIL(); // external runtime table entry 11 is null
						}
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[11])();
						_s17 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s18 = chaos_locals[6];
					{
						if (11 < 0 || 11 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL(); // external runtime table index 11 out of range
						}
						if (kChaosExternalRuntimeFnTable[11] == nullptr) {
							CHAOS_IL2CPP_FAIL(); // external runtime table entry 11 is null
						}
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[11])();
						_s19 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					{
						const auto chaos_arg_1 = _s19;
						const auto chaos_arg_0 = _s18;
						if (12 < 0 || 12 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL(); // external runtime table index 12 out of range
						}
						if (kChaosExternalRuntimeFnTable[12] == nullptr) {
							CHAOS_IL2CPP_FAIL(); // external runtime table entry 12 is null
						}
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
						if (9 < 0 || 9 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL(); // external runtime table index 9 out of range
						}
						if (kChaosExternalRuntimeFnTable[9] == nullptr) {
							CHAOS_IL2CPP_FAIL(); // external runtime table entry 9 is null
						}
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
						_s20 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					chaos_locals[12] = _s20;
					_s20 = chaos_locals[12];
					// brtrue (structured EH branch)
					_s21 = chaos_locals[6];
					{
						if (9 < 0 || 9 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL(); // external runtime table index 9 out of range
						}
						if (kChaosExternalRuntimeFnTable[9] == nullptr) {
							CHAOS_IL2CPP_FAIL(); // external runtime table entry 9 is null
						}
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[9])();
						_s22 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					chaos_locals[13] = _s22;
					_s22 = chaos_locals[13];
					// brfalse (structured EH branch)
			CHAOS_EH_FINALLY_END
	CHAOS_EH_FINALLY_END
	{
		if (14 < 0 || 14 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 14 out of range
		}
		if (kChaosExternalRuntimeFnTable[14] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 14 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
		_s23 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s24 = chaos_args[0];
	_s25 = chaos_args[1];
	{
		const auto chaos_arg_1 = _s25;
		const auto chaos_arg_0 = _s24;
		if (15 < 0 || 15 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 15 out of range
		}
		if (kChaosExternalRuntimeFnTable[15] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 15 is null
		}
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
		if (14 < 0 || 14 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 14 out of range
		}
		if (kChaosExternalRuntimeFnTable[14] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 14 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[14])();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_args[0];
	_s2 = chaos_args[1];
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (15 < 0 || 15 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 15 out of range
		}
		if (kChaosExternalRuntimeFnTable[15] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 15 is null
		}
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
							auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
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
							auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
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
							auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
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
							auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
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

// Generic execution authority: definition=Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action); type=[]; method=[System.NullReferenceException]; support=Specialized; specialization=SharedBody; body=body:definition=Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action);type=[];method=[System.NullReferenceException]; stub=stub:definition=Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action);type=[];method=[System.NullReferenceException]
// Managed method: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws<System.NullReferenceException>(System.Action)
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_NullReferenceException_System_Action__generic(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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
							auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
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
							auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
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
							auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_invocation_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_invocation_delegate->chaos_delegate_method_ptr);
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
							auto chaos_open_function = reinterpret_cast<void(*)()>(chaos_delegate->chaos_delegate_method_ptr);
							chaos_open_function();
						}
						else
						{
							auto chaos_closed_function = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR chaos_delegate_target)>(chaos_delegate->chaos_delegate_method_ptr);
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

// Generic instantiation stub: Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws<System.NullReferenceException>(System.Action)
// Generic execution authority: definition=Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action); type=[]; method=[System.NullReferenceException]; support=Specialized; specialization=SharedBody; body=body:definition=Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action);type=[];method=[System.NullReferenceException]; stub=stub:definition=Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action);type=[];method=[System.NullReferenceException]
extern "C" void chaos_stub_definition_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Throws_System_Void_System_Action__type____method__System_NullReferenceException(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert_Throws_System_NullReferenceException_System_Action__generic(chaos_fn_arg_0);
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
		if (22 < 0 || 22 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 22 out of range
		}
		if (kChaosExternalRuntimeFnTable[22] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 22 is null
		}
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
		if (22 < 0 || 22 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 22 out of range
		}
		if (kChaosExternalRuntimeFnTable[22] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 22 is null
		}
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
		if (22 < 0 || 22 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 22 out of range
		}
		if (kChaosExternalRuntimeFnTable[22] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 22 is null
		}
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[22])();
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	_s1 = 0;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
	_s2 = chaos_locals[0];
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (25 < 0 || 25 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 25 out of range
		}
		if (kChaosExternalRuntimeFnTable[25] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 25 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	{
		if (26 < 0 || 26 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 26 out of range
		}
		if (kChaosExternalRuntimeFnTable[26] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 26 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[26])();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_static_System_Private_CoreLib_System_IO_Stream__Null;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
	_s2 = chaos_locals[0];
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (25 < 0 || 25 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 25 out of range
		}
		if (kChaosExternalRuntimeFnTable[25] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 25 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
	_s3 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (28 < 0 || 28 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 28 out of range
		}
		if (kChaosExternalRuntimeFnTable[28] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 28 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	{
		if (29 < 0 || 29 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 29 out of range
		}
		if (kChaosExternalRuntimeFnTable[29] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 29 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[29])();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_static_System_Private_CoreLib_System_IO_Stream__Null;
	_s2 = chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c____9__7_0;
	_s3 = _s2;
	{
		if (_s3 == 0)
		{
			_s2 = chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 44)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2) -> void {
					auto& _d_entry = s_hotpatch_entries[44];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[24];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_2));
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, nullptr);
						return;
					}
					reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
				};
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s3;
				const auto chaos_target = _s2;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action_SseItem_System_Int32__System_Buffers_IBufferWriter_System_Byte__, {});
				chaos_object->header.type_info = chaos_mt_System_Private_CoreLib_System_Action_SseItem_System_Int32__System_Buffers_IBufferWriter_System_Byte__.AsTypeInfoHot();
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s3 = _s2;
			{
				auto chaos_value = _s3;
				chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c____9__7_0 = chaos_value;
			}
		}
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
		_s3 = chaos_locals[0];
		{
			const auto chaos_arg_3 = _s3;
			const auto chaos_arg_2 = _s2;
			const auto chaos_arg_1 = _s1;
			const auto chaos_arg_0 = _s0;
			if (28 < 0 || 28 >= kChaosExternalRuntimeCount) {
				CHAOS_IL2CPP_FAIL(); // external runtime table index 28 out of range
			}
			if (kChaosExternalRuntimeFnTable[28] == nullptr) {
				CHAOS_IL2CPP_FAIL(); // external runtime table entry 28 is null
			}
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		return;
	}
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INT64 _i0;
	CHAOS_IL2CPP_INT64 _i1;
	CHAOS_IL2CPP_INT64 _i2;
	_s0 = 0;
	_s1 = 0;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
	_s2 = chaos_locals[0];
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (25 < 0 || 25 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 25 out of range
		}
		if (kChaosExternalRuntimeFnTable[25] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 25 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (33 < 0 || 33 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 33 out of range
		}
		if (kChaosExternalRuntimeFnTable[33] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 33 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[33])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		if (34 < 0 || 34 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 34 out of range
		}
		if (kChaosExternalRuntimeFnTable[34] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 34 is null
		}
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[34])();
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
	chaos_locals[2] = ChaosStoreInt64(_i2);
	_i2 = ChaosLoadInt64(chaos_locals[2]);
	return _i2;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_0_IAsyncEnumerableSseItemstring_Stream_CancellationToken_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INT64 _i0;
	CHAOS_IL2CPP_INT64 _i1;
	CHAOS_IL2CPP_INT64 _i2;
	{
		if (26 < 0 || 26 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 26 out of range
		}
		if (kChaosExternalRuntimeFnTable[26] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 26 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[26])();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_static_System_Private_CoreLib_System_IO_Stream__Null;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
	_s2 = chaos_locals[0];
	{
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (25 < 0 || 25 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 25 out of range
		}
		if (kChaosExternalRuntimeFnTable[25] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 25 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[25])(chaos_arg_0, chaos_arg_1, chaos_arg_2);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (33 < 0 || 33 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 33 out of range
		}
		if (kChaosExternalRuntimeFnTable[33] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 33 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[33])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		if (34 < 0 || 34 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 34 out of range
		}
		if (kChaosExternalRuntimeFnTable[34] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 34 is null
		}
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[34])();
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
	chaos_locals[2] = ChaosStoreInt64(_i2);
	_i2 = ChaosLoadInt64(chaos_locals[2]);
	return _i2;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INT64 _i0;
	CHAOS_IL2CPP_INT64 _i1;
	CHAOS_IL2CPP_INT64 _i2;
	_s0 = 0;
	_s1 = 0;
	_s2 = 0;
	_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
	_s3 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (28 < 0 || 28 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 28 out of range
		}
		if (kChaosExternalRuntimeFnTable[28] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 28 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (33 < 0 || 33 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 33 out of range
		}
		if (kChaosExternalRuntimeFnTable[33] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 33 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[33])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		if (34 < 0 || 34 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 34 out of range
		}
		if (kChaosExternalRuntimeFnTable[34] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 34 is null
		}
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[34])();
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
	_i2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
	chaos_locals[2] = ChaosStoreInt64(_i2);
	_i2 = ChaosLoadInt64(chaos_locals[2]);
	return _i2;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests::WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INT64 _i0;
	CHAOS_IL2CPP_INT64 _i1;
	CHAOS_IL2CPP_INT64 _i2;
	{
		if (29 < 0 || 29 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 29 out of range
		}
		if (kChaosExternalRuntimeFnTable[29] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 29 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[29])();
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s1 = chaos_static_System_Private_CoreLib_System_IO_Stream__Null;
	_s2 = chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c____9__6_0;
	_s3 = _s2;
	{
		if (_s3 == 0)
		{
			_s2 = chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 45)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2) -> void {
					auto& _d_entry = s_hotpatch_entries[45];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[24];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_1));
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_2));
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, nullptr);
						return;
					}
					reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0, chaos_fn_arg_1, chaos_fn_arg_2);
				};
			_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s3;
				const auto chaos_target = _s2;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action_SseItem_System_Int32__System_Buffers_IBufferWriter_System_Byte__, {});
				chaos_object->header.type_info = chaos_mt_System_Private_CoreLib_System_Action_SseItem_System_Int32__System_Buffers_IBufferWriter_System_Byte__.AsTypeInfoHot();
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s3 = _s2;
			{
				auto chaos_value = _s3;
				chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c____9__6_0 = chaos_value;
			}
		}
		_s3 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
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
		_s3 = chaos_locals[0];
		{
			const auto chaos_arg_3 = _s3;
			const auto chaos_arg_2 = _s2;
			const auto chaos_arg_1 = _s1;
			const auto chaos_arg_0 = _s0;
			if (28 < 0 || 28 >= kChaosExternalRuntimeCount) {
				CHAOS_IL2CPP_FAIL(); // external runtime table index 28 out of range
			}
			if (kChaosExternalRuntimeFnTable[28] == nullptr) {
				CHAOS_IL2CPP_FAIL(); // external runtime table entry 28 is null
			}
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
			_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		{
			if (33 < 0 || 33 >= kChaosExternalRuntimeCount) {
				CHAOS_IL2CPP_FAIL(); // external runtime table index 33 out of range
			}
			if (kChaosExternalRuntimeFnTable[33] == nullptr) {
				CHAOS_IL2CPP_FAIL(); // external runtime table entry 33 is null
			}
			const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[33])();
			_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
		}
		chaos_locals[1] = _s1;
		_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
		{
			if (34 < 0 || 34 >= kChaosExternalRuntimeCount) {
				CHAOS_IL2CPP_FAIL(); // external runtime table index 34 out of range
			}
			if (kChaosExternalRuntimeFnTable[34] == nullptr) {
				CHAOS_IL2CPP_FAIL(); // external runtime table entry 34 is null
			}
			reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[34])();
		}
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
		_i2 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s2));
		chaos_locals[2] = ChaosStoreInt64(_i2);
		_i2 = ChaosLoadInt64(chaos_locals[2]);
		return _i2;
	}
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::.cctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c, {});
		chaos_object->header.type_info = chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c.AsTypeInfoHot();
		CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
		chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c____9 = chaos_value;
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1>b__7_0(SseItem<System.Int32>,System.Buffers.IBufferWriter<System.Byte>)
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_Benchmark_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__7_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseFormatterTests+<>c::<WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1>b__6_0(SseItem<System.Int32>,System.Buffers.IBufferWriter<System.Byte>)
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseFormatterTests___c_WriteAsync_1_IAsyncEnumerableSseItemint_Stream_System_ActionSseItemintIBufferWriterbyte_CancellationToken_1_b__6_0_SseItem_System_Int32_System_Buffers_IBufferWriter_System_Byte(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1, CHAOS_IL2CPP_INTPTR chaos_fn_arg_2)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 3) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	chaos_args[1] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_1);
	chaos_args[2] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_2);
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::Benchmark_Enumerate_0__0()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_Enumerate_0__0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		if (36 < 0 || 36 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 36 out of range
		}
		if (kChaosExternalRuntimeFnTable[36] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 36 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[36])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::Benchmark_EnumerateAsync_1_CancellationToken_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Benchmark_EnumerateAsync_1_CancellationToken_0(void)
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
		if (37 < 0 || 37 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 37 out of range
		}
		if (kChaosExternalRuntimeFnTable[37] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 37 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[37])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::Enumerate_0__0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_Enumerate_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c____9__0_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 53)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[53];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[8];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, nullptr);
						return;
					}
					reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
				};
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s1;
				const auto chaos_target = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action, {});
				chaos_object->header.type_info = chaos_mt_System_Private_CoreLib_System_Action.AsTypeInfoHot();
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = _s0;
			{
				auto chaos_value = _s1;
				chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c____9__0_0 = chaos_value;
			}
		}
		{
			// Assert.Throws: target method not found via IL lookahead
			// (verification skipped — use direct Assert.Throws in SDK for full coverage)
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
		_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
		chaos_locals[0] = ChaosStoreInt64(_i0);
		_i0 = ChaosLoadInt64(chaos_locals[0]);
		return _i0;
	}
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests::EnumerateAsync_1_CancellationToken_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests_EnumerateAsync_1_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c____9__2_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 54)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[54];
					if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d_entry)
						&& !::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d_entry))
					{
						alignas(16) uint8_t _d_ab[8];
						ArgBuffer _d_bw(_d_ab);
						_d_bw.WritePtr(reinterpret_cast<void*>(chaos_fn_arg_0));
						::chaos::il2cpp::runtime_core::InterpreterEntryDirect(
							_d_entry.method_key, _d_ab, nullptr);
						return;
					}
					reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(_d_entry.direct_ptr)(chaos_fn_arg_0);
				};
			_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_ftn_thunk);
			}
			{
				const auto chaos_method_ptr = _s1;
				const auto chaos_target = _s0;
				auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_System_Private_CoreLib_System_Action, {});
				chaos_object->header.type_info = chaos_mt_System_Private_CoreLib_System_Action.AsTypeInfoHot();
				chaos_object->chaos_delegate_target = chaos_target;
				chaos_object->chaos_delegate_method_ptr = chaos_method_ptr;
				_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
			}
			_s1 = _s0;
			{
				auto chaos_value = _s1;
				chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c____9__2_0 = chaos_value;
			}
		}
		{
			// Assert.Throws: target method not found via IL lookahead
			// (verification skipped — use direct Assert.Throws in SDK for full coverage)
		}
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(42);
		_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
		chaos_locals[0] = ChaosStoreInt64(_i0);
		_i0 = ChaosLoadInt64(chaos_locals[0]);
		return _i0;
	}
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::.cctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c, {});
		chaos_object->header.type_info = chaos_mt_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c.AsTypeInfoHot();
		CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
		chaos_static_CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c____9 = chaos_value;
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<Enumerate_0__0>b__0_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_Enumerate_0__0_b__0_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		if (36 < 0 || 36 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 36 out of range
		}
		if (kChaosExternalRuntimeFnTable[36] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 36 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[36])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_Net_ServerSentEvents.System_Net_ServerSentEvents_SseParserSystem_Int32Tests+<>c::<EnumerateAsync_1_CancellationToken_0>b__2_0()
extern "C" void CombinedSubjects_AutoGenerated_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParserSystem_Int32Tests___c_EnumerateAsync_1_CancellationToken_0_b__2_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
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
		if (37 < 0 || 37 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL(); // external runtime table index 37 out of range
		}
		if (kChaosExternalRuntimeFnTable[37] == nullptr) {
			CHAOS_IL2CPP_FAIL(); // external runtime table entry 37 is null
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[37])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::CombinedSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 56;

extern "C" void ChaosJitRegisterAll() {}