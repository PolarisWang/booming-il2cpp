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
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoderTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoderTests_Benchmark_Dispose_0__0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests_Benchmark_Dispose_0__0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests_Benchmark_GetMaxCompressedLength_1_int_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests_Benchmark_GetMaxCompressedLength_1_int_1(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests_GetMaxCompressedLength_1_int_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests_GetMaxCompressedLength_1_int_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_2(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_3(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_4(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_2(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_3(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_4(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_DisposeAsync_14__14(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_EndRead_11_System_IAsyncResult_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_EndWrite_3_System_IAsyncResult_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Flush_6__6(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_FlushAsync_7_CancellationToken_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Read_8_System_Byte_int_int_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Read_8_System_Byte_int_int_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Read_8_System_Byte_int_int_2(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Read_8_System_Byte_int_int_3(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Read_8_System_Byte_int_int_4(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_2(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_3(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_4(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_13_System_Memorybyte_CancellationToken_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadByte_9__9(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Seek_15_long_SeekOrigin_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Seek_15_long_SeekOrigin_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Seek_15_long_SeekOrigin_2(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_SetLength_16_long_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_SetLength_16_long_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Write_0_System_Byte_int_int_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Write_0_System_Byte_int_int_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Write_0_System_Byte_int_int_2(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Write_0_System_Byte_int_int_3(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Write_0_System_Byte_int_int_4(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_1(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_2(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_3(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_4(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_5_System_ReadOnlyMemorybyte_CancellationToken_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteByte_1_byte_0(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteByte_1_byte_1(void);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_DisposeAsync_14__14(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_EndRead_11_System_IAsyncResult_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_FlushAsync_7_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Read_8_System_Byte_int_int_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Read_8_System_Byte_int_int_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Read_8_System_Byte_int_int_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Read_8_System_Byte_int_int_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Read_8_System_Byte_int_int_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_12_System_Byte_int_int_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_12_System_Byte_int_int_CancellationToken_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_12_System_Byte_int_int_CancellationToken_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_12_System_Byte_int_int_CancellationToken_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_12_System_Byte_int_int_CancellationToken_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_13_System_Memorybyte_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadByte_9__9(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Seek_15_long_SeekOrigin_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Seek_15_long_SeekOrigin_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Seek_15_long_SeekOrigin_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_4_System_Byte_int_int_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_4_System_Byte_int_int_CancellationToken_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_4_System_Byte_int_int_CancellationToken_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_4_System_Byte_int_int_CancellationToken_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_4_System_Byte_int_int_CancellationToken_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_5_System_ReadOnlyMemorybyte_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_cctor(void);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_0_b__45_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_1_b__47_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_2_b__49_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_3_b__51_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_4_b__53_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_0_b__7_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_1_b__9_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_2_b__11_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_3_b__13_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_4_b__15_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_DisposeAsync_14__14_b__69_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_EndRead_11_System_IAsyncResult_0_b__55_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_FlushAsync_7_CancellationToken_0_b__31_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Read_8_System_Byte_int_int_0_b__33_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Read_8_System_Byte_int_int_1_b__35_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Read_8_System_Byte_int_int_2_b__37_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Read_8_System_Byte_int_int_3_b__39_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Read_8_System_Byte_int_int_4_b__41_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_ReadAsync_12_System_Byte_int_int_CancellationToken_0_b__57_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_ReadAsync_12_System_Byte_int_int_CancellationToken_1_b__59_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_ReadAsync_12_System_Byte_int_int_CancellationToken_2_b__61_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_ReadAsync_12_System_Byte_int_int_CancellationToken_3_b__63_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_ReadAsync_12_System_Byte_int_int_CancellationToken_4_b__65_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_ReadAsync_13_System_Memorybyte_CancellationToken_0_b__67_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_ReadByte_9__9_b__43_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Seek_15_long_SeekOrigin_0_b__71_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Seek_15_long_SeekOrigin_1_b__73_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Seek_15_long_SeekOrigin_2_b__75_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_WriteAsync_4_System_Byte_int_int_CancellationToken_0_b__18_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_WriteAsync_4_System_Byte_int_int_CancellationToken_1_b__20_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_WriteAsync_4_System_Byte_int_int_CancellationToken_2_b__22_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_WriteAsync_4_System_Byte_int_int_CancellationToken_3_b__24_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_WriteAsync_4_System_Byte_int_int_CancellationToken_4_b__26_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_WriteAsync_5_System_ReadOnlyMemorybyte_CancellationToken_0_b__28_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);





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
			CHAOS_IL2CPP_FAIL("external runtime table index 0 out of range");
		}
		if (kChaosExternalRuntimeFnTable[0] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 0 is null");
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
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__Equals_System_Boolean_System_Object_System_Object_(chaos_arg_0, chaos_arg_1);
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
						if (chaos_arg_0 == 0)
						{
							::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
						}
						const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_Object__ToString_System_String__(chaos_arg_0);
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
			CHAOS_IL2CPP_FAIL("external runtime table index 3 out of range");
		}
		if (kChaosExternalRuntimeFnTable[3] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 3 is null");
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
			CHAOS_IL2CPP_FAIL("external runtime table index 7 out of range");
		}
		if (kChaosExternalRuntimeFnTable[7] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 7 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[7])();
		_s12 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[5] = _s12;
	_s12 = chaos_locals[1];
	{
		if (7 < 0 || 7 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 7 out of range");
		}
		if (kChaosExternalRuntimeFnTable[7] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 7 is null");
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
							CHAOS_IL2CPP_FAIL("external runtime table index 13 out of range");
						}
						if (kChaosExternalRuntimeFnTable[13] == nullptr) {
							CHAOS_IL2CPP_FAIL("external runtime table entry 13 is null");
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
									CHAOS_IL2CPP_FAIL("external runtime table index 13 out of range");
								}
								if (kChaosExternalRuntimeFnTable[13] == nullptr) {
									CHAOS_IL2CPP_FAIL("external runtime table entry 13 is null");
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
							CHAOS_IL2CPP_FAIL("external runtime table index 9 out of range");
						}
						if (kChaosExternalRuntimeFnTable[9] == nullptr) {
							CHAOS_IL2CPP_FAIL("external runtime table entry 9 is null");
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
							CHAOS_IL2CPP_FAIL("external runtime table index 10 out of range");
						}
						if (kChaosExternalRuntimeFnTable[10] == nullptr) {
							CHAOS_IL2CPP_FAIL("external runtime table entry 10 is null");
						}
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[10])();
						_s15 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s16 = chaos_locals[5];
					{
						if (11 < 0 || 11 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL("external runtime table index 11 out of range");
						}
						if (kChaosExternalRuntimeFnTable[11] == nullptr) {
							CHAOS_IL2CPP_FAIL("external runtime table entry 11 is null");
						}
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[11])();
						_s17 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					_s18 = chaos_locals[6];
					{
						if (11 < 0 || 11 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL("external runtime table index 11 out of range");
						}
						if (kChaosExternalRuntimeFnTable[11] == nullptr) {
							CHAOS_IL2CPP_FAIL("external runtime table entry 11 is null");
						}
						const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[11])();
						_s19 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
					}
					{
						const auto chaos_arg_1 = _s19;
						const auto chaos_arg_0 = _s18;
						if (12 < 0 || 12 >= kChaosExternalRuntimeCount) {
							CHAOS_IL2CPP_FAIL("external runtime table index 12 out of range");
						}
						if (kChaosExternalRuntimeFnTable[12] == nullptr) {
							CHAOS_IL2CPP_FAIL("external runtime table entry 12 is null");
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
							CHAOS_IL2CPP_FAIL("external runtime table index 9 out of range");
						}
						if (kChaosExternalRuntimeFnTable[9] == nullptr) {
							CHAOS_IL2CPP_FAIL("external runtime table entry 9 is null");
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
							CHAOS_IL2CPP_FAIL("external runtime table index 9 out of range");
						}
						if (kChaosExternalRuntimeFnTable[9] == nullptr) {
							CHAOS_IL2CPP_FAIL("external runtime table entry 9 is null");
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
			CHAOS_IL2CPP_FAIL("external runtime table index 14 out of range");
		}
		if (kChaosExternalRuntimeFnTable[14] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 14 is null");
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
			CHAOS_IL2CPP_FAIL("external runtime table index 15 out of range");
		}
		if (kChaosExternalRuntimeFnTable[15] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 15 is null");
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
			CHAOS_IL2CPP_FAIL("external runtime table index 14 out of range");
		}
		if (kChaosExternalRuntimeFnTable[14] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 14 is null");
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
			CHAOS_IL2CPP_FAIL("external runtime table index 15 out of range");
		}
		if (kChaosExternalRuntimeFnTable[15] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 15 is null");
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
		const auto chaos_result = chaos_external_runtime_System_Console_System_Console__get_Error_System_IO_TextWriter__();
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
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_IO_TextWriter__WriteLine_System_Void_System_String_(chaos_arg_0, chaos_arg_1);
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
		chaos_external_runtime_System_Private_CoreLib_System_Exception___ctor_System_Void_System_String_(chaos_arg_0, chaos_arg_1);
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
			CHAOS_IL2CPP_FAIL("external runtime table index 22 out of range");
		}
		if (kChaosExternalRuntimeFnTable[22] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 22 is null");
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
			CHAOS_IL2CPP_FAIL("external runtime table index 22 out of range");
		}
		if (kChaosExternalRuntimeFnTable[22] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 22 is null");
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
			CHAOS_IL2CPP_FAIL("external runtime table index 22 out of range");
		}
		if (kChaosExternalRuntimeFnTable[22] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 22 is null");
		}
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[22])();
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliDecoderTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoderTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliDecoderTests::Benchmark_Dispose_0__0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliDecoderTests_Benchmark_Dispose_0__0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = _s0;
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
	{
		if (25 < 0 || 25 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 25 out of range");
		}
		if (kChaosExternalRuntimeFnTable[25] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 25 is null");
		}
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[25])();
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliEncoderTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliEncoderTests::Benchmark_Dispose_0__0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests_Benchmark_Dispose_0__0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	_s1 = _s0;
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
	{
		if (27 < 0 || 27 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 27 out of range");
		}
		if (kChaosExternalRuntimeFnTable[27] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 27 is null");
		}
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[27])();
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliEncoderTests::Benchmark_GetMaxCompressedLength_1_int_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests_Benchmark_GetMaxCompressedLength_1_int_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s0;
		if (28 < 0 || 28 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 28 out of range");
		}
		if (kChaosExternalRuntimeFnTable[28] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 28 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliEncoderTests::Benchmark_GetMaxCompressedLength_1_int_1()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests_Benchmark_GetMaxCompressedLength_1_int_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s0;
		if (28 < 0 || 28 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 28 out of range");
		}
		if (kChaosExternalRuntimeFnTable[28] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 28 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliEncoderTests::GetMaxCompressedLength_1_int_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests_GetMaxCompressedLength_1_int_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s0;
		if (28 < 0 || 28 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 28 out of range");
		}
		if (kChaosExternalRuntimeFnTable[28] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 28 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = chaos_locals[0];
	_s2 = 0;
	{
		[&]() -> void { if ((_s0) != (_s1)) { throw chaos_managed_exception{}; } }();
	}
	_s0 = chaos_locals[0];
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[1] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[1]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliEncoderTests::GetMaxCompressedLength_1_int_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliEncoderTests_GetMaxCompressedLength_1_int_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s0;
		if (28 < 0 || 28 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 28 out of range");
		}
		if (kChaosExternalRuntimeFnTable[28] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 28 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[28])(chaos_arg_0);
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s0;
	_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(1);
	_s1 = chaos_locals[0];
	_s2 = 0;
	{
		[&]() -> void { if ((_s0) != (_s1)) { throw chaos_managed_exception{}; } }();
	}
	_s0 = chaos_locals[0];
	_i0 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s0));
	chaos_locals[1] = ChaosStoreInt64(_i0);
	_i0 = ChaosLoadInt64(chaos_locals[1]);
	return _i0;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__45_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 123)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[123];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__45_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__47_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 124)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[124];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__47_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_2()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__49_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 125)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[125];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__49_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_3()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__51_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 126)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[126];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__51_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_4()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__53_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 127)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[127];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__53_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__7_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 128)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[128];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__7_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__9_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 129)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[129];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__9_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_2()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__11_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 130)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[130];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__11_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_3()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__13_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 131)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[131];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__13_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_4()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__15_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 132)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[132];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__15_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (41 < 0 || 41 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 41 out of range");
		}
		if (kChaosExternalRuntimeFnTable[41] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 41 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_1()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (41 < 0 || 41 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 41 out of range");
		}
		if (kChaosExternalRuntimeFnTable[41] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 41 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_2()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (41 < 0 || 41 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 41 out of range");
		}
		if (kChaosExternalRuntimeFnTable[41] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 41 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_3()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	{
		const auto chaos_result = ChaosArrayEmpty_Inline();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (41 < 0 || 41 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 41 out of range");
		}
		if (kChaosExternalRuntimeFnTable[41] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 41 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_4()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s1 = ChaosArrayNew1D_Inline(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(), 1, _s1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (41 < 0 || 41 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 41 out of range");
		}
		if (kChaosExternalRuntimeFnTable[41] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 41 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (44 < 0 || 44 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 44 out of range");
		}
		if (kChaosExternalRuntimeFnTable[44] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 44 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_1()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (44 < 0 || 44 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 44 out of range");
		}
		if (kChaosExternalRuntimeFnTable[44] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 44 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_2()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (44 < 0 || 44 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 44 out of range");
		}
		if (kChaosExternalRuntimeFnTable[44] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 44 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_3()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	{
		const auto chaos_result = ChaosArrayEmpty_Inline();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (44 < 0 || 44 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 44 out of range");
		}
		if (kChaosExternalRuntimeFnTable[44] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 44 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_4()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s1 = ChaosArrayNew1D_Inline(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(), 1, _s1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (44 < 0 || 44 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 44 out of range");
		}
		if (kChaosExternalRuntimeFnTable[44] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 44 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_DisposeAsync_14__14()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_DisposeAsync_14__14(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		if (45 < 0 || 45 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 45 out of range");
		}
		if (kChaosExternalRuntimeFnTable[45] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 45 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[45])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_EndRead_11_System_IAsyncResult_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_EndRead_11_System_IAsyncResult_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		if (46 < 0 || 46 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 46 out of range");
		}
		if (kChaosExternalRuntimeFnTable[46] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 46 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[46])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_EndWrite_3_System_IAsyncResult_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_EndWrite_3_System_IAsyncResult_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		if (47 < 0 || 47 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 47 out of range");
		}
		if (kChaosExternalRuntimeFnTable[47] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 47 is null");
		}
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[47])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Flush_6__6()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Flush_6__6(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = 0;
	{
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Flush_System_Void__(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_FlushAsync_7_CancellationToken_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_FlushAsync_7_CancellationToken_0(void)
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
		if (50 < 0 || 50 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 50 out of range");
		}
		if (kChaosExternalRuntimeFnTable[50] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 50 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[50])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Read_8_System_Byte_int_int_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Read_8_System_Byte_int_int_0(void)
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Read_System_Int32_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Read_8_System_Byte_int_int_1()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Read_8_System_Byte_int_int_1(void)
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Read_System_Int32_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Read_8_System_Byte_int_int_2()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Read_8_System_Byte_int_int_2(void)
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Read_System_Int32_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Read_8_System_Byte_int_int_3()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Read_8_System_Byte_int_int_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	{
		const auto chaos_result = ChaosArrayEmpty_Inline();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Read_System_Int32_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Read_8_System_Byte_int_int_4()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Read_8_System_Byte_int_int_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s1 = ChaosArrayNew1D_Inline(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(), 1, _s1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Read_System_Int32_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (52 < 0 || 52 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 52 out of range");
		}
		if (kChaosExternalRuntimeFnTable[52] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 52 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_1()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (52 < 0 || 52 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 52 out of range");
		}
		if (kChaosExternalRuntimeFnTable[52] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 52 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_2()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (52 < 0 || 52 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 52 out of range");
		}
		if (kChaosExternalRuntimeFnTable[52] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 52 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_3()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	{
		const auto chaos_result = ChaosArrayEmpty_Inline();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (52 < 0 || 52 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 52 out of range");
		}
		if (kChaosExternalRuntimeFnTable[52] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 52 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_4()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_12_System_Byte_int_int_CancellationToken_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s1 = ChaosArrayNew1D_Inline(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(), 1, _s1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (52 < 0 || 52 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 52 out of range");
		}
		if (kChaosExternalRuntimeFnTable[52] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 52 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_ReadAsync_13_System_Memorybyte_CancellationToken_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadAsync_13_System_Memorybyte_CancellationToken_0(void)
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
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (54 < 0 || 54 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 54 out of range");
		}
		if (kChaosExternalRuntimeFnTable[54] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 54 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[54])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_ReadByte_9__9()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_ReadByte_9__9(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	{
		if (55 < 0 || 55 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 55 out of range");
		}
		if (kChaosExternalRuntimeFnTable[55] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 55 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[55])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Seek_15_long_SeekOrigin_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Seek_15_long_SeekOrigin_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INT64 _i0;
	CHAOS_IL2CPP_INT64 _i1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_i1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _i1;
		if (56 < 0 || 56 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 56 out of range");
		}
		if (kChaosExternalRuntimeFnTable[56] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 56 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[56])(chaos_arg_0, chaos_arg_1);
		_s1 = ChaosStoreInt64(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Seek_15_long_SeekOrigin_1()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Seek_15_long_SeekOrigin_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INT64 _i0;
	CHAOS_IL2CPP_INT64 _i1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_i1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _i1;
		if (56 < 0 || 56 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 56 out of range");
		}
		if (kChaosExternalRuntimeFnTable[56] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 56 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[56])(chaos_arg_0, chaos_arg_1);
		_s1 = ChaosStoreInt64(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Seek_15_long_SeekOrigin_2()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Seek_15_long_SeekOrigin_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INT64 _i0;
	CHAOS_IL2CPP_INT64 _i1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_i1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _i1;
		if (56 < 0 || 56 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 56 out of range");
		}
		if (kChaosExternalRuntimeFnTable[56] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 56 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT64(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[56])(chaos_arg_0, chaos_arg_1);
		_s1 = ChaosStoreInt64(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_SetLength_16_long_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_SetLength_16_long_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	CHAOS_IL2CPP_INT64 _i1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_i1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
	{
		const auto chaos_arg_1 = _i1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__SetLength_System_Void_System_Int64_(chaos_arg_0, ChaosLoadInt64(chaos_arg_1));
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_SetLength_16_long_1()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_SetLength_16_long_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	CHAOS_IL2CPP_INT64 _i1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_i1 = ChaosStoreInt64(static_cast<CHAOS_IL2CPP_INT64>(_s1));
	{
		const auto chaos_arg_1 = _i1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__SetLength_System_Void_System_Int64_(chaos_arg_0, ChaosLoadInt64(chaos_arg_1));
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Write_0_System_Byte_int_int_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Write_0_System_Byte_int_int_0(void)
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Write_System_Void_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Write_0_System_Byte_int_int_1()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Write_0_System_Byte_int_int_1(void)
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Write_System_Void_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Write_0_System_Byte_int_int_2()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Write_0_System_Byte_int_int_2(void)
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
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Write_System_Void_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Write_0_System_Byte_int_int_3()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Write_0_System_Byte_int_int_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	{
		const auto chaos_result = ChaosArrayEmpty_Inline();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Write_System_Void_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_Write_0_System_Byte_int_int_4()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_Write_0_System_Byte_int_int_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s1 = ChaosArrayNew1D_Inline(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(), 1, _s1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Write_System_Void_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (59 < 0 || 59 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 59 out of range");
		}
		if (kChaosExternalRuntimeFnTable[59] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 59 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[59])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_1()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (59 < 0 || 59 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 59 out of range");
		}
		if (kChaosExternalRuntimeFnTable[59] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 59 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[59])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_2()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_2(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (59 < 0 || 59 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 59 out of range");
		}
		if (kChaosExternalRuntimeFnTable[59] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 59 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[59])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_3()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_3(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	{
		const auto chaos_result = ChaosArrayEmpty_Inline();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (59 < 0 || 59 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 59 out of range");
		}
		if (kChaosExternalRuntimeFnTable[59] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 59 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[59])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_4()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_4_System_Byte_int_int_CancellationToken_4(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s1 = ChaosArrayNew1D_Inline(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(), 1, _s1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (59 < 0 || 59 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 59 out of range");
		}
		if (kChaosExternalRuntimeFnTable[59] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 59 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[59])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_WriteAsync_5_System_ReadOnlyMemorybyte_CancellationToken_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteAsync_5_System_ReadOnlyMemorybyte_CancellationToken_0(void)
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
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (61 < 0 || 61 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 61 out of range");
		}
		if (kChaosExternalRuntimeFnTable[61] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 61 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[61])(chaos_arg_0, chaos_arg_1);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_WriteByte_1_byte_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteByte_1_byte_0(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		if (62 < 0 || 62 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 62 out of range");
		}
		if (kChaosExternalRuntimeFnTable[62] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 62 is null");
		}
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[62])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Benchmark_WriteByte_1_byte_1()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Benchmark_WriteByte_1_byte_1(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_0 = _s1;
		if (62 < 0 || 62 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 62 out of range");
		}
		if (kChaosExternalRuntimeFnTable[62] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 62 is null");
		}
		reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[62])(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::DisposeAsync_14__14()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_DisposeAsync_14__14(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__69_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 133)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[133];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__69_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::EndRead_11_System_IAsyncResult_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_EndRead_11_System_IAsyncResult_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__55_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 134)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[134];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__55_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::FlushAsync_7_CancellationToken_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_FlushAsync_7_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__31_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 135)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[135];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__31_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Read_8_System_Byte_int_int_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Read_8_System_Byte_int_int_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__33_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 136)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[136];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__33_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Read_8_System_Byte_int_int_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Read_8_System_Byte_int_int_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__35_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 137)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[137];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__35_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Read_8_System_Byte_int_int_2()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Read_8_System_Byte_int_int_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__37_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 138)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[138];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__37_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Read_8_System_Byte_int_int_3()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Read_8_System_Byte_int_int_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__39_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 139)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[139];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__39_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Read_8_System_Byte_int_int_4()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Read_8_System_Byte_int_int_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__41_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 140)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[140];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__41_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::ReadAsync_12_System_Byte_int_int_CancellationToken_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_12_System_Byte_int_int_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__57_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 141)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[141];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__57_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::ReadAsync_12_System_Byte_int_int_CancellationToken_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_12_System_Byte_int_int_CancellationToken_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__59_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 142)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[142];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__59_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::ReadAsync_12_System_Byte_int_int_CancellationToken_2()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_12_System_Byte_int_int_CancellationToken_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__61_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 143)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[143];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__61_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::ReadAsync_12_System_Byte_int_int_CancellationToken_3()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_12_System_Byte_int_int_CancellationToken_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__63_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 144)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[144];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__63_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::ReadAsync_12_System_Byte_int_int_CancellationToken_4()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_12_System_Byte_int_int_CancellationToken_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__65_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 145)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[145];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__65_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::ReadAsync_13_System_Memorybyte_CancellationToken_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadAsync_13_System_Memorybyte_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__67_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 146)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[146];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__67_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::ReadByte_9__9()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_ReadByte_9__9(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__43_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 147)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[147];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__43_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Seek_15_long_SeekOrigin_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Seek_15_long_SeekOrigin_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__71_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 148)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[148];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__71_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Seek_15_long_SeekOrigin_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Seek_15_long_SeekOrigin_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__73_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 149)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[149];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__73_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::Seek_15_long_SeekOrigin_2()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_Seek_15_long_SeekOrigin_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__75_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 150)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[150];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__75_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::WriteAsync_4_System_Byte_int_int_CancellationToken_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_4_System_Byte_int_int_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__18_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 151)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[151];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__18_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::WriteAsync_4_System_Byte_int_int_CancellationToken_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_4_System_Byte_int_int_CancellationToken_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__20_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 152)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[152];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__20_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::WriteAsync_4_System_Byte_int_int_CancellationToken_2()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_4_System_Byte_int_int_CancellationToken_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__22_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 153)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[153];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__22_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::WriteAsync_4_System_Byte_int_int_CancellationToken_3()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_4_System_Byte_int_int_CancellationToken_3(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__24_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 154)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[154];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__24_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::WriteAsync_4_System_Byte_int_int_CancellationToken_4()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_4_System_Byte_int_int_CancellationToken_4(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__26_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 155)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[155];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__26_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests::WriteAsync_5_System_ReadOnlyMemorybyte_CancellationToken_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests_WriteAsync_5_System_ReadOnlyMemorybyte_CancellationToken_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INT64 _i0;
	_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__28_0;
	_s1 = _s0;
	{
		if (_s1 == 0)
		{
			_s0 = chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9;
			{
				// Hotpatch-aware ldftn wrapper (slot 156)
				static auto* chaos_ftn_thunk = +[](CHAOS_IL2CPP_INTPTR chaos_fn_arg_0) -> void {
					auto& _d_entry = s_hotpatch_entries[156];
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
				chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9__28_0 = chaos_value;
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

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::.cctor()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_cctor(void)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};

	CHAOS_IL2CPP_INTPTR _s0;
	{
		auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c, {});
		chaos_object->header.type_info = chaos_mt_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c.AsTypeInfoHot();
		CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
		_s0 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object);
	}
	{
		auto chaos_value = _s0;
		chaos_static_CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c____9 = chaos_value;
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	_s0 = chaos_args[0];
	{
		const auto chaos_arg_0 = _s0;
		chaos_external_runtime_System_Private_CoreLib_System_Object___ctor_System_Void__(chaos_arg_0);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_0>b__45_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_0_b__45_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (41 < 0 || 41 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 41 out of range");
		}
		if (kChaosExternalRuntimeFnTable[41] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 41 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_1>b__47_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_1_b__47_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (41 < 0 || 41 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 41 out of range");
		}
		if (kChaosExternalRuntimeFnTable[41] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 41 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_2>b__49_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_2_b__49_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (41 < 0 || 41 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 41 out of range");
		}
		if (kChaosExternalRuntimeFnTable[41] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 41 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_3>b__51_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_3_b__51_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	{
		const auto chaos_result = ChaosArrayEmpty_Inline();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (41 < 0 || 41 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 41 out of range");
		}
		if (kChaosExternalRuntimeFnTable[41] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 41 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_4>b__53_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginRead_10_System_Byte_int_int_System_AsyncCallback_object_4_b__53_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s1 = ChaosArrayNew1D_Inline(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(), 1, _s1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (41 < 0 || 41 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 41 out of range");
		}
		if (kChaosExternalRuntimeFnTable[41] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 41 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[41])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_0>b__7_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_0_b__7_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (44 < 0 || 44 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 44 out of range");
		}
		if (kChaosExternalRuntimeFnTable[44] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 44 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_1>b__9_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_1_b__9_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (44 < 0 || 44 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 44 out of range");
		}
		if (kChaosExternalRuntimeFnTable[44] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 44 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_2>b__11_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_2_b__11_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (44 < 0 || 44 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 44 out of range");
		}
		if (kChaosExternalRuntimeFnTable[44] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 44 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_3>b__13_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_3_b__13_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	{
		const auto chaos_result = ChaosArrayEmpty_Inline();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (44 < 0 || 44 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 44 out of range");
		}
		if (kChaosExternalRuntimeFnTable[44] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 44 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_4>b__15_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_BeginWrite_2_System_Byte_int_int_System_AsyncCallback_object_4_b__15_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	CHAOS_IL2CPP_INTPTR _s5;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s1 = ChaosArrayNew1D_Inline(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(), 1, _s1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = 0;
	_s5 = 0;
	{
		const auto chaos_arg_4 = _s5;
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (44 < 0 || 44 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 44 out of range");
		}
		if (kChaosExternalRuntimeFnTable[44] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 44 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[44])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3, chaos_arg_4);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<DisposeAsync_14__14>b__69_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_DisposeAsync_14__14_b__69_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	_s0 = 0;
	{
		if (45 < 0 || 45 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 45 out of range");
		}
		if (kChaosExternalRuntimeFnTable[45] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 45 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[45])();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[0] = _s1;
	_s1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		if (87 < 0 || 87 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 87 out of range");
		}
		if (kChaosExternalRuntimeFnTable[87] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 87 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[87])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s2;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		if (88 < 0 || 88 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 88 out of range");
		}
		if (kChaosExternalRuntimeFnTable[88] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 88 is null");
		}
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[88])();
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<EndRead_11_System_IAsyncResult_0>b__55_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_EndRead_11_System_IAsyncResult_0_b__55_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	_s0 = 0;
	_s1 = 0;
	{
		const auto chaos_arg_0 = _s1;
		if (46 < 0 || 46 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 46 out of range");
		}
		if (kChaosExternalRuntimeFnTable[46] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 46 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[46])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<FlushAsync_7_CancellationToken_0>b__31_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_FlushAsync_7_CancellationToken_0_b__31_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
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
	{
		const auto chaos_arg_0 = _s1;
		if (50 < 0 || 50 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 50 out of range");
		}
		if (kChaosExternalRuntimeFnTable[50] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 50 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[50])(chaos_arg_0);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (89 < 0 || 89 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 89 out of range");
		}
		if (kChaosExternalRuntimeFnTable[89] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 89 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[89])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s2;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		if (90 < 0 || 90 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 90 out of range");
		}
		if (kChaosExternalRuntimeFnTable[90] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 90 is null");
		}
		reinterpret_cast<void(*)(void)>(kChaosExternalRuntimeFnTable[90])();
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<Read_8_System_Byte_int_int_0>b__33_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Read_8_System_Byte_int_int_0_b__33_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Read_System_Int32_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<Read_8_System_Byte_int_int_1>b__35_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Read_8_System_Byte_int_int_1_b__35_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Read_System_Int32_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<Read_8_System_Byte_int_int_2>b__37_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Read_8_System_Byte_int_int_2_b__37_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Read_System_Int32_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<Read_8_System_Byte_int_int_3>b__39_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Read_8_System_Byte_int_int_3_b__39_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	{
		const auto chaos_result = ChaosArrayEmpty_Inline();
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Read_System_Int32_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<Read_8_System_Byte_int_int_4>b__41_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_Read_8_System_Byte_int_int_4_b__41_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	_s0 = 0;
	_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(2);
	_s1 = ChaosArrayNew1D_Inline(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Byte.AsTypeInfoHot(), 1, _s1);
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	{
		const auto chaos_arg_3 = _s3;
		const auto chaos_arg_2 = _s2;
		const auto chaos_arg_1 = _s1;
		const auto chaos_arg_0 = _s0;
		if (chaos_arg_0 == 0)
		{
			::chaos::il2cpp::runtime_core::RaiseNullReferenceException();
		}
		const auto chaos_result = chaos_external_runtime_System_Private_CoreLib_System_IO_Stream__Read_System_Int32_System_Byte___System_Int32_System_Int32_(chaos_arg_0, chaos_arg_1, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_2), static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_3));
		_s0 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<ReadAsync_12_System_Byte_int_int_CancellationToken_0>b__57_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_ReadAsync_12_System_Byte_int_int_CancellationToken_0_b__57_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (52 < 0 || 52 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 52 out of range");
		}
		if (kChaosExternalRuntimeFnTable[52] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 52 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (91 < 0 || 91 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 91 out of range");
		}
		if (kChaosExternalRuntimeFnTable[91] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 91 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[91])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s2;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		if (92 < 0 || 92 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 92 out of range");
		}
		if (kChaosExternalRuntimeFnTable[92] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 92 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[92])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<ReadAsync_12_System_Byte_int_int_CancellationToken_1>b__59_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_ReadAsync_12_System_Byte_int_int_CancellationToken_1_b__59_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (52 < 0 || 52 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 52 out of range");
		}
		if (kChaosExternalRuntimeFnTable[52] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 52 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (91 < 0 || 91 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 91 out of range");
		}
		if (kChaosExternalRuntimeFnTable[91] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 91 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[91])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s2;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		if (92 < 0 || 92 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 92 out of range");
		}
		if (kChaosExternalRuntimeFnTable[92] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 92 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[92])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}

// Managed method: CombinedSubjects/AutoGenerated.System_IO_Compression_Brotli.System_IO_Compression_BrotliStreamTests+<>c::<ReadAsync_12_System_Byte_int_int_CancellationToken_2>b__61_0()
extern "C" void CombinedSubjects_AutoGenerated_System_IO_Compression_Brotli_System_IO_Compression_BrotliStreamTests___c_ReadAsync_12_System_Byte_int_int_CancellationToken_2_b__61_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
{
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
	CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 2) chaos_locals{};
	chaos_args[0] = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_fn_arg_0);
	CHAOS_IL2CPP_INTPTR _s0;
	CHAOS_IL2CPP_INTPTR _s1;
	CHAOS_IL2CPP_INTPTR _s2;
	CHAOS_IL2CPP_INTPTR _s3;
	CHAOS_IL2CPP_INTPTR _s4;
	_s0 = 0;
	_s1 = 0;
	_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(0);
	_s4 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);
	{
		const auto chaos_address = _s4;
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
	_s4 = chaos_locals[0];
	{
		const auto chaos_arg_3 = _s4;
		const auto chaos_arg_2 = _s3;
		const auto chaos_arg_1 = _s2;
		const auto chaos_arg_0 = _s1;
		if (52 < 0 || 52 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 52 out of range");
		}
		if (kChaosExternalRuntimeFnTable[52] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 52 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kChaosExternalRuntimeFnTable[52])(chaos_arg_0, chaos_arg_1, chaos_arg_2, chaos_arg_3);
		_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	{
		if (91 < 0 || 91 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 91 out of range");
		}
		if (kChaosExternalRuntimeFnTable[91] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 91 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INTPTR(*)(void)>(kChaosExternalRuntimeFnTable[91])();
		_s2 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	chaos_locals[1] = _s2;
	_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[1]);
	{
		if (92 < 0 || 92 >= kChaosExternalRuntimeCount) {
			CHAOS_IL2CPP_FAIL("external runtime table index 92 out of range");
		}
		if (kChaosExternalRuntimeFnTable[92] == nullptr) {
			CHAOS_IL2CPP_FAIL("external runtime table entry 92 is null");
		}
		const auto chaos_result = reinterpret_cast<CHAOS_IL2CPP_INT32(*)(void)>(kChaosExternalRuntimeFnTable[92])();
		_s3 = static_cast<CHAOS_IL2CPP_INTPTR>(chaos_result);
	}
	return;
}



}  // namespace chaos::il2cpp::codegen::CombinedSubjects
#pragma warning(pop)

// extern "C" definition for link-time visibility from runtime-entry.cpp
extern "C" const int kAotMethodCount = 158;

extern "C" void ChaosJitRegisterAll() {}