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
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaim_1_Claim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_Clone_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_RemoveClaim_4_Claim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_TryRemoveClaim_3_Claim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_WriteTo_11_BinaryWriter_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentity_0_ClaimsIdentity_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_Clone_2__2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_WriteTo_10_BinaryWriter_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_1_ClaimsIdentity_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_WriteTo_2_BinaryWriter_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests_Clone_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" void CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0);

// Runtime prelude dependencies included at translation unit level


MethodTable chaos_mt_Chaos_TestFramework_Sdk___0 = {nullptr, nullptr, 12016523716365422726ULL, 0u, 32, 2, 0, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_Chaos_TestFramework_Sdk___0 = static_cast<CHAOS_IL2CPP_UINT64>(12016523716365422726ULL);
// ── Virtual method table arrays ──
extern "C" void Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0, CHAOS_IL2CPP_INTPTR chaos_fn_arg_1);
const void* chaos_vtable_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException[] =
{
	reinterpret_cast<void*>(Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String),
};
MethodTable chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert = {nullptr, nullptr, 3469195832277345062ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert = static_cast<CHAOS_IL2CPP_UINT64>(3469195832277345062ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Exception = {nullptr, nullptr, 10972282733316558392ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Exception = static_cast<CHAOS_IL2CPP_UINT64>(10972282733316558392ULL);
MethodTable chaos_mt_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Exception), chaos_vtable_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException, 8951892653230046377ULL, 1u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException = static_cast<CHAOS_IL2CPP_UINT64>(8951892653230046377ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Object = {nullptr, nullptr, 15228727185366376748ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Object = static_cast<CHAOS_IL2CPP_UINT64>(15228727185366376748ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Delegate = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 7451128447593600616ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Delegate = static_cast<CHAOS_IL2CPP_UINT64>(7451128447593600616ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_MulticastDelegate = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Delegate), nullptr, 6681393039041505440ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_MulticastDelegate = static_cast<CHAOS_IL2CPP_UINT64>(6681393039041505440ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Action = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate), nullptr, 14722794505377461205ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Action = static_cast<CHAOS_IL2CPP_UINT64>(14722794505377461205ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Collections_IEnumerable = {nullptr, nullptr, 16749028324345562163ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Collections_IEnumerable = static_cast<CHAOS_IL2CPP_UINT64>(16749028324345562163ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_IDisposable = {nullptr, nullptr, 13420242832086098868ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_IDisposable = static_cast<CHAOS_IL2CPP_UINT64>(13420242832086098868ULL);
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
MethodTable chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 4903285098910223694ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c = static_cast<CHAOS_IL2CPP_UINT64>(4903285098910223694ULL);
MethodTable chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 8395387131394307352ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c = static_cast<CHAOS_IL2CPP_UINT64>(8395387131394307352ULL);
MethodTable chaos_mt_System_Private_CoreLib_System_Predicate_Claim_ = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate), nullptr, 923202691029893427ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_System_Private_CoreLib_System_Predicate_Claim_ = static_cast<CHAOS_IL2CPP_UINT64>(923202691029893427ULL);
MethodTable chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests___c = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 10976192477369812671ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests___c = static_cast<CHAOS_IL2CPP_UINT64>(10976192477369812671ULL);
MethodTable chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests___c = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 7076361164028166069ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests___c = static_cast<CHAOS_IL2CPP_UINT64>(7076361164028166069ULL);
MethodTable chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests___c = {reinterpret_cast<const MethodTable*>(&chaos_mt_System_Private_CoreLib_System_Object), nullptr, 977641436165088689ULL, 0u, 32, 1, 1, nullptr, nullptr, 0, 0, 0, 0};
inline constexpr CHAOS_IL2CPP_UINT64 chaos_type_id_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests___c = static_cast<CHAOS_IL2CPP_UINT64>(977641436165088689ULL);

static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException[] =
{
	{ 0x0000000Fu, reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String) },
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
static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Exception[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _message)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _innerException)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Exception, _stackTrace))};
static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Delegate[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Delegate, chaos_delegate_target)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Delegate, chaos_delegate_invocation_list))};
static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_MulticastDelegate[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_MulticastDelegate, chaos_delegate_target)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_MulticastDelegate, chaos_delegate_invocation_list))};
static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_Assembly[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_Assembly, runtime_assembly_name_value))};
static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_AssemblyName, runtime_name_value))};
static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, declaring_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_method_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_definition_method_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, generic_argument_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_ConstructorInfo, runtime_name_value))};
static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, declaring_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_FieldInfo, runtime_name_value))};
static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, declaring_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_method_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_definition_method_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, generic_argument_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, runtime_name_value))};
static constexpr CHAOS_IL2CPP_UINT16 kGcOffsets_chaos_type_System_Private_CoreLib_System_Type[] = {static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_type_handle)), static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_type_System_Private_CoreLib_System_Type, runtime_name_value))};

extern "C" void ChaosRegisterGcLayouts() {
	auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();

	// Register managed_array (variable-size: header + contiguous element data).
	registry.Register(CHAOS_IL2CPP_UINT64(chaos_type_id_managed_array), sizeof(chaos_managed_array), nullptr, 0,
		static_cast<CHAOS_IL2CPP_UINT16>(sizeof(CHAOS_IL2CPP_INTPTR)),
		static_cast<CHAOS_IL2CPP_UINT16>(offsetof(chaos_managed_array, length)));

	registry.Register(12016523716365422726ULL, sizeof(chaos_type_Chaos_TestFramework_Sdk___0), nullptr, 0);
	registry.Register(3469195832277345062ULL, sizeof(chaos_type_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert), nullptr, 0);
	registry.Register(10972282733316558392ULL, sizeof(chaos_type_System_Private_CoreLib_System_Exception), kGcOffsets_chaos_type_System_Private_CoreLib_System_Exception, 3);
	registry.Register(8951892653230046377ULL, sizeof(chaos_type_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException), nullptr, 0);
	registry.Register(15228727185366376748ULL, sizeof(chaos_type_System_Private_CoreLib_System_Object), nullptr, 0);
	registry.Register(7451128447593600616ULL, sizeof(chaos_type_System_Private_CoreLib_System_Delegate), kGcOffsets_chaos_type_System_Private_CoreLib_System_Delegate, 2);
	registry.Register(6681393039041505440ULL, sizeof(chaos_type_System_Private_CoreLib_System_MulticastDelegate), kGcOffsets_chaos_type_System_Private_CoreLib_System_MulticastDelegate, 2);
	registry.Register(14722794505377461205ULL, sizeof(chaos_type_System_Private_CoreLib_System_Action), nullptr, 0);
	registry.Register(16749028324345562163ULL, sizeof(chaos_type_System_Private_CoreLib_System_Collections_IEnumerable), nullptr, 0);
	registry.Register(13420242832086098868ULL, sizeof(chaos_type_System_Private_CoreLib_System_IDisposable), nullptr, 0);
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
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Delegate), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Delegate) + sizeof(chaos_mt_System_Private_CoreLib_System_Delegate));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_MulticastDelegate) + sizeof(chaos_mt_System_Private_CoreLib_System_MulticastDelegate));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Action), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Action) + sizeof(chaos_mt_System_Private_CoreLib_System_Action));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Collections_IEnumerable), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Collections_IEnumerable) + sizeof(chaos_mt_System_Private_CoreLib_System_Collections_IEnumerable));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_IDisposable), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_IDisposable) + sizeof(chaos_mt_System_Private_CoreLib_System_IDisposable));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_Assembly) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_Assembly));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_AssemblyName));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodBase));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_ConstructorInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_FieldInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo) + sizeof(chaos_mt_System_Private_CoreLib_System_Reflection_MethodInfo));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_String) + sizeof(chaos_mt_System_Private_CoreLib_System_String));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Type) + sizeof(chaos_mt_System_Private_CoreLib_System_Type));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c), reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c) + sizeof(chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c), reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c) + sizeof(chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Predicate_Claim_), reinterpret_cast<uintptr_t>(&chaos_mt_System_Private_CoreLib_System_Predicate_Claim_) + sizeof(chaos_mt_System_Private_CoreLib_System_Predicate_Claim_));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests___c), reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests___c) + sizeof(chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests___c));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests___c), reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests___c) + sizeof(chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests___c));
	registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests___c), reinterpret_cast<uintptr_t>(&chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests___c) + sizeof(chaos_mt_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests___c));
}

CHAOS_IL2CPP_INT32 chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__s_exitCode = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__0_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__5_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__8_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__10_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__10_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__11_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__12_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__14_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__16_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__18_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__18_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__19_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__20_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__22_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__24_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__26_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__26_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__27_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__28_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__30_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests___c____9__32_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__3_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__5_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__7_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__7_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__8_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__9_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__11_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__13_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__15_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__15_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__16_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__17_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__19_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__21_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__23_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__23_1 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__24_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__25_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__27_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__29_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__31_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests___c____9__33_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests___c____9__2_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests___c____9__0_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests___c____9 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests___c____9__0_0 = 0;
CHAOS_IL2CPP_INTPTR chaos_static_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests___c____9__2_0 = 0;


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
extern "C" CHAOS_IL2CPP_INT32 kChaosExternalRuntimeCount;
// ── ABI manifest ──────────────────────────────────────────────
// Single contiguous struct: header + entries + params in same object
// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.

// Param offset prefix-sum: [i] = cumulative parameter count before method i
static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[85] = {
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
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
	66u,
};

static constexpr struct {
	::ChaosAbiManifestV0 header;
	::ChaosAbiMethodEntryV0 entries[84];
	CHAOS_IL2CPP_UINT8 params[66];
} s_abi_manifest_storage = {
	{
		CHAOS_ABI_MANIFEST_VERSION,
		84u,
		66u,
		1612135547u,  // FNV-1a over entries+params
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
		{ 0u, 1u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String
		{ 0u, 0u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor
		{ 0u, 0u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor
		{ 0u, 0u },  // Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests__ctor
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaim_1_Claim_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_Clone_0__0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_2
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_RemoveClaim_4_Claim_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_TryRemoveClaim_3_Claim_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_WriteTo_11_BinaryWriter_0
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests__ctor
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentity_0_ClaimsIdentity_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_Clone_2__2
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_2
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_1
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_WriteTo_10_BinaryWriter_0
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests__ctor
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_0__0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_1_ClaimsIdentity_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_WriteTo_2_BinaryWriter_0
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests__ctor
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests_Clone_0__0
		{ 0u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests__ctor
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_0
		{ 10u, 0u },  // CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_1
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
	},
};
static const ::ChaosAbiManifestV0* const s_abi_manifest =
	reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);
// ── Module registration ──

	static constexpr CHAOS_IL2CPP_UINT32 s_type_flags[6] = {
		0u,
		1536u,
		1536u,
		1536u,
		1536u,
		1536u,
	};

	static constexpr const char* s_type_names[6] = {
		"<Module>",
		"System_Security_Claims_ClaimTests",
		"System_Security_Claims_ClaimsIdentityTests",
		"System_Security_Claims_ClaimsPrincipalTests",
		"System_Security_Principal_GenericIdentityTests",
		"System_Security_Principal_GenericPrincipalTests",
	};

	static constexpr const char* s_type_namespaces[6] = {
		"",
		"AutoGenerated.System_Security_Claims",
		"AutoGenerated.System_Security_Claims",
		"AutoGenerated.System_Security_Claims",
		"AutoGenerated.System_Security_Claims",
		"AutoGenerated.System_Security_Claims",
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_type_parent_tokens[6] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
	};

	static const TypeInfoHot* const s_type_info_ptrs[6] = {
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_nested_type_offset[7] = {
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
		0u,
	};

	static constexpr CHAOS_IL2CPP_UINT32 s_generic_param_constraint_offset[7] = {
		0u,
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
		/* .nested_type_children= */ nullptr,
		/* .nested_type_offset = */ s_nested_type_offset,
		/* .generic_param_constraint_data= */ nullptr,
		/* .generic_param_constraint_offset= */ s_generic_param_constraint_offset,
		/* .type_count        = */ 6u,
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
static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[83] = {
	{ ".ctor", 0x00000014u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests
	{ "Clone_0__0", 0x0000000Eu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests
	{ "Clone_1_ClaimsIdentity_0", 0x00000010u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests
	{ "WriteTo_2_BinaryWriter_0", 0x00000012u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests
	{ ".ctor", 0x0000003Eu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "AddClaim_1_Claim_0", 0x00000018u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "AddClaims_2_IEnumerableClaim_0", 0x0000001Au, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "AddClaims_2_IEnumerableClaim_1", 0x0000001Cu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "Clone_0__0", 0x00000016u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "FindAll_5_System_PredicateClaim_0", 0x00000022u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "FindAll_5_System_PredicateClaim_1", 0x00000024u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "FindAll_6_string_0", 0x00000026u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "FindAll_6_string_1", 0x00000028u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "FindFirst_7_System_PredicateClaim_0", 0x0000002Au, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "FindFirst_7_System_PredicateClaim_1", 0x0000002Cu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "FindFirst_8_string_0", 0x0000002Eu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "FindFirst_8_string_1", 0x00000030u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "HasClaim_10_string_string_0", 0x00000036u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "HasClaim_10_string_string_1", 0x00000038u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "HasClaim_10_string_string_2", 0x0000003Au, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "HasClaim_9_System_PredicateClaim_0", 0x00000032u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "HasClaim_9_System_PredicateClaim_1", 0x00000034u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "RemoveClaim_4_Claim_0", 0x00000020u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "TryRemoveClaim_3_Claim_0", 0x0000001Eu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ "WriteTo_11_BinaryWriter_0", 0x0000003Cu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests
	{ ".ctor", 0x0000005Cu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "AddIdentities_1_IEnumerableClaimsIdentity_0", 0x00000042u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "AddIdentities_1_IEnumerableClaimsIdentity_1", 0x00000044u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "AddIdentity_0_ClaimsIdentity_0", 0x00000040u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "Clone_2__2", 0x00000046u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "FindAll_3_System_PredicateClaim_0", 0x00000048u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "FindAll_3_System_PredicateClaim_1", 0x0000004Au, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "FindAll_4_string_0", 0x0000004Cu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "FindAll_4_string_1", 0x0000004Eu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "FindFirst_5_System_PredicateClaim_0", 0x00000050u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "FindFirst_5_System_PredicateClaim_1", 0x00000051u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "FindFirst_6_string_0", 0x00000052u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "FindFirst_6_string_1", 0x00000053u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "HasClaim_7_System_PredicateClaim_0", 0x00000054u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "HasClaim_7_System_PredicateClaim_1", 0x00000055u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "HasClaim_8_string_string_0", 0x00000056u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "HasClaim_8_string_string_1", 0x00000057u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "HasClaim_8_string_string_2", 0x00000058u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "IsInRole_9_string_0", 0x00000059u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "IsInRole_9_string_1", 0x0000005Au, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ "WriteTo_10_BinaryWriter_0", 0x0000005Bu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests
	{ ".ctor", 0x0000005Eu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests
	{ "Clone_0__0", 0x0000005Du, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests
	{ ".ctor", 0x00000061u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests
	{ "IsInRole_0_string_0", 0x0000005Fu, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests
	{ "IsInRole_0_string_1", 0x00000060u, 0u },  // AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests
	{ "AreEqual", 0x0000002Bu, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000025u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000045u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000029u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000031u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x0000002Fu, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000021u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000019u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x0000001Du, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000035u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000027u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x0000002Du, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000033u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x00000023u, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x0000001Bu, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual", 0x0000001Fu, 3u },  // Chaos.TestFramework.Assert
	{ "AreEqual`1", 0x00000043u, 3u },  // Chaos.TestFramework.Assert
	{ "AreNotEqual`1", 0x00000047u, 3u },  // Chaos.TestFramework.Assert
	{ "Complete", 0x00000015u, 0u },  // Chaos.TestFramework.Assert
	{ "Fail", 0x00000049u, 1u },  // Chaos.TestFramework.Assert
	{ "get_ExitCode", 0x00000017u, 0u },  // Chaos.TestFramework.Assert
	{ "IsFalse", 0x0000003Du, 2u },  // Chaos.TestFramework.Assert
	{ "IsNotNull", 0x00000039u, 2u },  // Chaos.TestFramework.Assert
	{ "IsNull", 0x00000037u, 2u },  // Chaos.TestFramework.Assert
	{ "IsTrue", 0x0000003Bu, 2u },  // Chaos.TestFramework.Assert
	{ "RecordFailure", 0x00000013u, 0u },  // Chaos.TestFramework.Assert
	{ "Reset", 0x00000011u, 0u },  // Chaos.TestFramework.Assert
	{ "Throws", 0x0000003Fu, 1u },  // Chaos.TestFramework.Assert
	{ ".ctor", 0x0000000Fu, 1u },  // Chaos.TestFramework.AssertionException
	{ ".ctor", 0x0000004Bu, 0u },  // Chaos.TestFramework.BenchmarkAttribute
	{ ".ctor", 0x0000004Du, 0u },  // Chaos.TestFramework.FactAttribute
	{ ".ctor", 0x0000004Fu, 0u },  // Chaos.TestFramework.HotUpdateAttribute
};

// Type name index entries (namespace, short_name)
static constexpr HotpatchTypeEntryV0 s_hotpatch_types[10] = {
	{ "AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests", "AutoGenerated.System_Security_Claims", 0u, 4u },
	{ "AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests", "AutoGenerated.System_Security_Claims", 4u, 21u },
	{ "AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests", "AutoGenerated.System_Security_Claims", 25u, 21u },
	{ "AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests", "AutoGenerated.System_Security_Claims", 46u, 2u },
	{ "AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests", "AutoGenerated.System_Security_Claims", 48u, 3u },
	{ "Chaos.TestFramework.Assert", "Chaos.TestFramework", 51u, 28u },
	{ "Chaos.TestFramework.AssertionException", "Chaos.TestFramework", 79u, 1u },
	{ "Chaos.TestFramework.BenchmarkAttribute", "Chaos.TestFramework", 80u, 1u },
	{ "Chaos.TestFramework.FactAttribute", "Chaos.TestFramework", 81u, 1u },
	{ "Chaos.TestFramework.HotUpdateAttribute", "Chaos.TestFramework", 82u, 1u },
};

// Token→Slot mapping (sorted by token for binary search)
static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[83] = {
	{ 0x0000000Eu, 75u },
	{ 0x0000000Fu, 28u },
	{ 0x00000010u, 76u },
	{ 0x00000011u, 26u },
	{ 0x00000012u, 77u },
	{ 0x00000013u, 25u },
	{ 0x00000014u, 74u },
	{ 0x00000015u, 18u },
	{ 0x00000016u, 36u },
	{ 0x00000017u, 20u },
	{ 0x00000018u, 33u },
	{ 0x00000019u, 7u },
	{ 0x0000001Au, 34u },
	{ 0x0000001Bu, 14u },
	{ 0x0000001Cu, 35u },
	{ 0x0000001Du, 8u },
	{ 0x0000001Eu, 51u },
	{ 0x0000001Fu, 15u },
	{ 0x00000020u, 50u },
	{ 0x00000021u, 6u },
	{ 0x00000022u, 37u },
	{ 0x00000023u, 13u },
	{ 0x00000024u, 38u },
	{ 0x00000025u, 1u },
	{ 0x00000026u, 39u },
	{ 0x00000027u, 10u },
	{ 0x00000028u, 40u },
	{ 0x00000029u, 3u },
	{ 0x0000002Au, 41u },
	{ 0x0000002Bu, 0u },
	{ 0x0000002Cu, 42u },
	{ 0x0000002Du, 11u },
	{ 0x0000002Eu, 43u },
	{ 0x0000002Fu, 5u },
	{ 0x00000030u, 44u },
	{ 0x00000031u, 4u },
	{ 0x00000032u, 48u },
	{ 0x00000033u, 12u },
	{ 0x00000034u, 49u },
	{ 0x00000035u, 9u },
	{ 0x00000036u, 45u },
	{ 0x00000037u, 23u },
	{ 0x00000038u, 46u },
	{ 0x00000039u, 22u },
	{ 0x0000003Au, 47u },
	{ 0x0000003Bu, 24u },
	{ 0x0000003Cu, 52u },
	{ 0x0000003Du, 21u },
	{ 0x0000003Eu, 32u },
	{ 0x0000003Fu, 27u },
	{ 0x00000040u, 56u },
	{ 0x00000042u, 54u },
	{ 0x00000043u, 16u },
	{ 0x00000044u, 55u },
	{ 0x00000045u, 2u },
	{ 0x00000046u, 57u },
	{ 0x00000047u, 17u },
	{ 0x00000048u, 58u },
	{ 0x00000049u, 19u },
	{ 0x0000004Au, 59u },
	{ 0x0000004Bu, 29u },
	{ 0x0000004Cu, 60u },
	{ 0x0000004Du, 30u },
	{ 0x0000004Eu, 61u },
	{ 0x0000004Fu, 31u },
	{ 0x00000050u, 62u },
	{ 0x00000051u, 63u },
	{ 0x00000052u, 64u },
	{ 0x00000053u, 65u },
	{ 0x00000054u, 66u },
	{ 0x00000055u, 67u },
	{ 0x00000056u, 68u },
	{ 0x00000057u, 69u },
	{ 0x00000058u, 70u },
	{ 0x00000059u, 71u },
	{ 0x0000005Au, 72u },
	{ 0x0000005Bu, 73u },
	{ 0x0000005Cu, 53u },
	{ 0x0000005Du, 79u },
	{ 0x0000005Eu, 78u },
	{ 0x0000005Fu, 81u },
	{ 0x00000060u, 82u },
	{ 0x00000061u, 80u },
};

// Dispatch table (function pointers)
// s_hotpatch_entries — external linkage for verification_dispatch
// NOTE: `static` is deliberately omitted so that the verification dispatch
// TU (verification_dispatch.generated.cpp) can access this array via
// GetHotpatchEntries(). Without external linkage, the linker cannot resolve
// the symbol across translation units.
extern "C" HotpatchEntryV0 s_hotpatch_entries[83] = {
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
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.AssertionException::.ctor
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.BenchmarkAttribute::.ctor
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.FactAttribute::.ctor
	{ reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // Chaos.TestFramework.HotUpdateAttribute::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaim_1_Claim_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::AddClaim_1_Claim_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::AddClaims_2_IEnumerableClaim_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::AddClaims_2_IEnumerableClaim_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_Clone_0__0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::Clone_0__0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_5_System_PredicateClaim_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_5_System_PredicateClaim_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_6_string_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_6_string_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_7_System_PredicateClaim_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_7_System_PredicateClaim_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_8_string_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_8_string_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_10_string_string_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_10_string_string_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_10_string_string_2
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_9_System_PredicateClaim_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_9_System_PredicateClaim_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_RemoveClaim_4_Claim_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::RemoveClaim_4_Claim_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_TryRemoveClaim_3_Claim_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::TryRemoveClaim_3_Claim_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_WriteTo_11_BinaryWriter_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::WriteTo_11_BinaryWriter_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::AddIdentities_1_IEnumerableClaimsIdentity_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::AddIdentities_1_IEnumerableClaimsIdentity_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentity_0_ClaimsIdentity_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::AddIdentity_0_ClaimsIdentity_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_Clone_2__2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::Clone_2__2
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_3_System_PredicateClaim_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_3_System_PredicateClaim_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_4_string_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_4_string_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_5_System_PredicateClaim_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_5_System_PredicateClaim_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_6_string_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_6_string_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_7_System_PredicateClaim_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_7_System_PredicateClaim_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_8_string_string_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_8_string_string_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_2), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_8_string_string_2
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::IsInRole_9_string_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::IsInRole_9_string_1
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_WriteTo_10_BinaryWriter_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::WriteTo_10_BinaryWriter_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_0__0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::Clone_0__0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_1_ClaimsIdentity_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::Clone_1_ClaimsIdentity_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_WriteTo_2_BinaryWriter_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::WriteTo_2_BinaryWriter_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests_Clone_0__0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests::Clone_0__0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests__ctor), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, kHotpatchKeepNative },  // AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests::.ctor
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_0), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests::IsInRole_0_string_0
	{ reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_1), reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 },  // AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests::IsInRole_0_string_1
};

// Accessor functions for verification_dispatch.generated.cpp
extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept { return s_hotpatch_entries; }
extern "C" int32_t GetHotpatchEntryCount() noexcept { return 83; }

// Module hotpatch bundle (extern linkage — referenced across page files)
extern constexpr HotpatchModuleV0 s_hotpatch_module = {
	"CombinedSubjects",
	s_hotpatch_types,
	10u,
	s_hotpatch_methods,
	83u,
	s_hotpatch_slots,
	83u,
	s_hotpatch_entries,
	83u,
};

// Expose hotpatch module to BootstrapRuntime
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module
	= &s_hotpatch_module;
// ── External Runtime Dispatch Table ──────────────────────────
// Startup-time-resolved function pointers for cross-assembly calls.

extern "C" const char* kChaosExternalRuntimeSubjects[24] = {
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
};

extern "C" void* kChaosExternalRuntimeFnTable[24] = {
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
};

extern "C" int32_t kChaosExternalRuntimeCount = 24;
// (no method AOT entries for this module)
// ── Method table (kMethodTable[]) ──────────────────────────────
// const function pointer array for dispatch via slot index.
// NOTE: Pure data only — dispatch routing is in <chaos/hotpatch_dispatch.h>.
// NOTE: Emitted in ALL modes (AOT + JIT). JIT mode verification dispatch
// uses kDefaultArgThunks to bypass JIT precode trampoline (verification
// tests AOT codegen, not JIT compilation).
static void (*kMethodTable[84])() = {
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
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor),
	reinterpret_cast<void(*)()>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaim_1_Claim_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_Clone_0__0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_2),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_RemoveClaim_4_Claim_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_TryRemoveClaim_3_Claim_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_WriteTo_11_BinaryWriter_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentity_0_ClaimsIdentity_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_Clone_2__2),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_2),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_1),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_WriteTo_10_BinaryWriter_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_0__0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_1_ClaimsIdentity_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_WriteTo_2_BinaryWriter_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests_Clone_0__0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests__ctor),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_0),
	reinterpret_cast<void(*)()>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_1),
};

// ── Default-arg thunks (kDefaultArgThunks[]) ────────────────────────────
// Each thunk supplies default argument values based on parameter types.
// String params receive a valid StringId; all others receive 0.
// Instance methods receive a sentinel this-pointer so they don't crash on null.
static CHAOS_IL2CPP_UINT8 __g_null_instance = 0;
extern "C" void (*kDefaultArgThunks[84])() = {
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
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR)>(kMethodTable[29])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance),chaos_make_string_id_value(6601085983368743141ULL));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[30])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[31])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[32])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[33])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[34])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[35])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[36])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[37])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[38])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[39])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[40])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[41])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[42])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[43])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[44])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[45])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[46])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[47])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[48])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[49])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[50])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[51])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[52])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[53])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[54])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[55])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[56])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[57])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[58])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[59])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[60])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[61])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[62])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[63])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[64])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[65])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[66])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[67])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[68])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[69])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[70])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[71])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[72])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[73])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[74])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[75])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[76])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[77])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[78])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[79])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[80])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[81])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[82])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
	[]() {reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR)>(kMethodTable[83])(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&__g_null_instance));},
};

// ── Subject slot map (kSubjectSlotMap[]) ────────────────────────────
// Maps subject index (0-based sequential) to kMethodTable index.
// External consumers use this to route --benchmark N to the correct
// AOT method slot, since kMethodTable[] includes lambdas/closures
// that shift subject methods to non-contiguous indices.
extern "C" const int kSubjectEntryCount = 44;
extern "C" const int kSubjectSlotMap[44] = {
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
	44,
	45,
	46,
	47,
	48,
	49,
	50,
	51,
	52,
	53,
	55,
	56,
	57,
	59,
	60,
	61,
	62,
	63,
	64,
	65,
	66,
	67,
	68,
	69,
	70,
	71,
	72,
	73,
	74,
	76,
	77,
	78,
	80,
	82,
};

// ── Subject contract map (kSubjectContractMap[]) ───────────────────────
// Maps subject slot index (si) to contract index (position in contract list).
// Used by fact-json output to match native results against golden records
// without assuming slot layout.
extern "C" const int kSubjectContractMap[44] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	39,
	40,
	41,
	42,
	43,
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
	/* ── Entry 29: Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[4];
	} entry29;
	/* ── Entry 30: Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry30;
	/* ── Entry 31: Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry31;
	/* ── Entry 32: Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry32;
	/* ── Entry 33: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry33;
	/* ── Entry 34: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaim_1_Claim_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry34;
	/* ── Entry 35: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry35;
	/* ── Entry 36: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry36;
	/* ── Entry 37: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_Clone_0__0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry37;
	/* ── Entry 38: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry38;
	/* ── Entry 39: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry39;
	/* ── Entry 40: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry40;
	/* ── Entry 41: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry41;
	/* ── Entry 42: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry42;
	/* ── Entry 43: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry43;
	/* ── Entry 44: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry44;
	/* ── Entry 45: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry45;
	/* ── Entry 46: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry46;
	/* ── Entry 47: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry47;
	/* ── Entry 48: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry48;
	/* ── Entry 49: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry49;
	/* ── Entry 50: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry50;
	/* ── Entry 51: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_RemoveClaim_4_Claim_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry51;
	/* ── Entry 52: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_TryRemoveClaim_3_Claim_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry52;
	/* ── Entry 53: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_WriteTo_11_BinaryWriter_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry53;
	/* ── Entry 54: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry54;
	/* ── Entry 55: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry55;
	/* ── Entry 56: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry56;
	/* ── Entry 57: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentity_0_ClaimsIdentity_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry57;
	/* ── Entry 58: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_Clone_2__2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry58;
	/* ── Entry 59: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry59;
	/* ── Entry 60: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry60;
	/* ── Entry 61: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry61;
	/* ── Entry 62: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry62;
	/* ── Entry 63: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry63;
	/* ── Entry 64: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry64;
	/* ── Entry 65: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry65;
	/* ── Entry 66: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry66;
	/* ── Entry 67: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry67;
	/* ── Entry 68: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry68;
	/* ── Entry 69: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry69;
	/* ── Entry 70: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry70;
	/* ── Entry 71: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_2 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry71;
	/* ── Entry 72: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry72;
	/* ── Entry 73: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry73;
	/* ── Entry 74: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_WriteTo_10_BinaryWriter_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry74;
	/* ── Entry 75: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry75;
	/* ── Entry 76: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_0__0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry76;
	/* ── Entry 77: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_1_ClaimsIdentity_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry77;
	/* ── Entry 78: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_WriteTo_2_BinaryWriter_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry78;
	/* ── Entry 79: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry79;
	/* ── Entry 80: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests_Clone_0__0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry80;
	/* ── Entry 81: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests__ctor ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[2];
	} entry81;
	/* ── Entry 82: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_0 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry82;
	/* ── Entry 83: CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_1 ── */
	struct {
		CHAOS_IL2CPP_UINT32 entry_total_size;
		const void*         code_address;
		CHAOS_IL2CPP_UINT32 frame_size;
		CHAOS_IL2CPP_UINT32 num_gc_slots;
		CHAOS_IL2CPP_UINT32 slots[3];
	} entry83;
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
	/* entry29 = Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String */
	.entry29 = {
		/* entry_total_size = 36 */ 36u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String),
		/* frame_size = 32 */ 32u,
		/* num_gc_slots = 4 */ 4u,
		/* slots */ { 0u, 8u, 16u, 24u }
	},
	/* entry30 = Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor */
	.entry30 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry31 = Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor */
	.entry31 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry32 = Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor */
	.entry32 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry33 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests__ctor */
	.entry33 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry34 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaim_1_Claim_0 */
	.entry34 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaim_1_Claim_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry35 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_0 */
	.entry35 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry36 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_1 */
	.entry36 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry37 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_Clone_0__0 */
	.entry37 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_Clone_0__0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry38 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_0 */
	.entry38 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry39 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_1 */
	.entry39 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry40 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_0 */
	.entry40 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry41 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_1 */
	.entry41 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry42 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_0 */
	.entry42 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry43 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_1 */
	.entry43 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry44 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_0 */
	.entry44 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry45 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_1 */
	.entry45 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry46 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_0 */
	.entry46 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry47 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_1 */
	.entry47 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry48 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_2 */
	.entry48 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry49 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_0 */
	.entry49 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry50 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_1 */
	.entry50 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry51 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_RemoveClaim_4_Claim_0 */
	.entry51 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_RemoveClaim_4_Claim_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry52 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_TryRemoveClaim_3_Claim_0 */
	.entry52 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_TryRemoveClaim_3_Claim_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry53 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_WriteTo_11_BinaryWriter_0 */
	.entry53 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_WriteTo_11_BinaryWriter_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry54 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests__ctor */
	.entry54 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry55 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_0 */
	.entry55 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry56 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_1 */
	.entry56 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry57 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentity_0_ClaimsIdentity_0 */
	.entry57 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentity_0_ClaimsIdentity_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry58 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_Clone_2__2 */
	.entry58 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_Clone_2__2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry59 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_0 */
	.entry59 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry60 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_1 */
	.entry60 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry61 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_0 */
	.entry61 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry62 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_1 */
	.entry62 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry63 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_0 */
	.entry63 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry64 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_1 */
	.entry64 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry65 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_0 */
	.entry65 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry66 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_1 */
	.entry66 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry67 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_0 */
	.entry67 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry68 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_1 */
	.entry68 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry69 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_0 */
	.entry69 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry70 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_1 */
	.entry70 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry71 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_2 */
	.entry71 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_2),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry72 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_0 */
	.entry72 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry73 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_1 */
	.entry73 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry74 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_WriteTo_10_BinaryWriter_0 */
	.entry74 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_WriteTo_10_BinaryWriter_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry75 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests__ctor */
	.entry75 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry76 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_0__0 */
	.entry76 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_0__0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry77 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_1_ClaimsIdentity_0 */
	.entry77 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_1_ClaimsIdentity_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry78 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_WriteTo_2_BinaryWriter_0 */
	.entry78 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_WriteTo_2_BinaryWriter_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry79 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests__ctor */
	.entry79 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry80 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests_Clone_0__0 */
	.entry80 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests_Clone_0__0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry81 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests__ctor */
	.entry81 = {
		/* entry_total_size = 28 */ 28u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests__ctor),
		/* frame_size = 16 */ 16u,
		/* num_gc_slots = 2 */ 2u,
		/* slots */ { 0u, 8u }
	},
	/* entry82 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_0 */
	.entry82 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_0),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	},
	/* entry83 = CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_1 */
	.entry83 = {
		/* entry_total_size = 32 */ 32u,
		/* code_address */ reinterpret_cast<const void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_1),
		/* frame_size = 24 */ 24u,
		/* num_gc_slots = 3 */ 3u,
		/* slots */ { 0u, 8u, 16u }
	}
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

static const CHAOS_IL2CPP_UINT32 kChaosGcSlotMapsSize = 2880u;

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

// Bridge/import thunk for: Chaos.TestFramework.Sdk/!!0
extern "C" void chaos_bridge_thunk_2(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[4])();
}

// Bridge/import thunk for: System.Private.CoreLib/System.String
extern "C" void chaos_bridge_thunk_3(void)
{
	reinterpret_cast<void(*)()>(kChaosExternalRuntimeFnTable[5])();
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
static void* const kMethodPointers[84] = {
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
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException__ctor_System_String),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute__ctor),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute__ctor),
	reinterpret_cast<void*>(&Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaim_1_Claim_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_Clone_0__0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_2),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_RemoveClaim_4_Claim_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_TryRemoveClaim_3_Claim_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_WriteTo_11_BinaryWriter_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentity_0_ClaimsIdentity_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_Clone_2__2),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_2),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_1),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_WriteTo_10_BinaryWriter_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_0__0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_1_ClaimsIdentity_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_WriteTo_2_BinaryWriter_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests_Clone_0__0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests__ctor),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_0),
	reinterpret_cast<void*>(&CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_1),
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
};
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration
	= {
	.struct_size               = sizeof(CodeRegistrationV0),
	.method_pointers           = kMethodPointers,
	.method_pointer_count      = 84u,
	.reverse_pinvoke_wrappers  = nullptr,
	.reverse_pinvoke_wrapper_count = 0u,
	.invoker_pointers          = nullptr,
	.invoker_pointer_count     = 0u,
	.unresolved_virtual_calls = nullptr,
	.unresolved_virtual_call_count = 0u,
	.type_capabilities       = nullptr,
	.type_capability_count   = 0u,
	.vtable_descriptors = kChaosVTableDescriptors,
	.vtable_descriptor_count = 1u,
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
static constexpr ReflectionQueryMethodDescriptor kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert[29] = {
	{ 0x0000002Bu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Boolean,System.Boolean,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000025u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Byte,System.Byte,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000045u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Byte[],System.Byte[],System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000029u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Char,System.Char,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000031u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Decimal,System.Decimal,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Fu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Double,System.Double,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000021u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int16,System.Int16,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000019u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int32,System.Int32,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Du, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Int64,System.Int64,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000035u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Object,System.Object,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000027u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.SByte,System.SByte,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Du, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.Single,System.Single,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000033u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.String,System.String,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000023u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt16,System.UInt16,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Bu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt32,System.UInt32,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Fu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual:System.Void(System.UInt64,System.UInt64,System.String)", "AreEqual", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000043u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreEqual`1:System.Void(!!0,!!0,System.String)", "AreEqual`1", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000047u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::AreNotEqual`1:System.Void(!!0,!!0,System.String)", "AreNotEqual`1", "System.Void", 3, nullptr, 0u, nullptr, 0u },
	{ 0x00000015u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Complete:System.Int32()", "Complete", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000049u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Fail:System.Void(System.String)", "Fail", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000017u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::get_ExitCode:System.Int32()", "get_ExitCode", "System.Int32", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Du, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsFalse:System.Void(System.Boolean,System.String)", "IsFalse", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000039u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNotNull:System.Void(System.Object,System.String)", "IsNotNull", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000037u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsNull:System.Void(System.Object,System.String)", "IsNull", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Bu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::IsTrue:System.Void(System.Boolean,System.String)", "IsTrue", "System.Void", 2, nullptr, 0u, nullptr, 0u },
	{ 0x00000013u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::RecordFailure:System.Void()", "RecordFailure", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000011u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Reset:System.Void()", "Reset", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Fu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws:System.Void(System.Action)", "Throws", "System.Void", 1, nullptr, 0u, nullptr, 0u },
	{ 0x00000041u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Throws`1:System.Void(System.Action)", "Throws`1", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException[1] = {
	{ 0x0000000Fu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException::.ctor:System.Void(System.String)", ".ctor", "System.Void", 1, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute[1] = {
	{ 0x0000004Bu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute[1] = {
	{ 0x0000004Du, "Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute[1] = {
	{ 0x0000004Fu, "Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests[21] = {
	{ 0x0000003Eu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000018u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::AddClaim_1_Claim_0:System.Int64()", "AddClaim_1_Claim_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Au, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::AddClaims_2_IEnumerableClaim_0:System.Int64()", "AddClaims_2_IEnumerableClaim_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Cu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::AddClaims_2_IEnumerableClaim_1:System.Int64()", "AddClaims_2_IEnumerableClaim_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000016u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::Clone_0__0:System.Int64()", "Clone_0__0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000022u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_5_System_PredicateClaim_0:System.Int64()", "FindAll_5_System_PredicateClaim_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000024u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_5_System_PredicateClaim_1:System.Int64()", "FindAll_5_System_PredicateClaim_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000026u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_6_string_0:System.Int64()", "FindAll_6_string_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000028u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_6_string_1:System.Int64()", "FindAll_6_string_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Au, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_7_System_PredicateClaim_0:System.Int64()", "FindFirst_7_System_PredicateClaim_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Cu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_7_System_PredicateClaim_1:System.Int64()", "FindFirst_7_System_PredicateClaim_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000002Eu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_8_string_0:System.Int64()", "FindFirst_8_string_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000030u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_8_string_1:System.Int64()", "FindFirst_8_string_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000036u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_10_string_string_0:System.Int64()", "HasClaim_10_string_string_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000038u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_10_string_string_1:System.Int64()", "HasClaim_10_string_string_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Au, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_10_string_string_2:System.Int64()", "HasClaim_10_string_string_2", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000032u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_9_System_PredicateClaim_0:System.Int64()", "HasClaim_9_System_PredicateClaim_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000034u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_9_System_PredicateClaim_1:System.Int64()", "HasClaim_9_System_PredicateClaim_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000020u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::RemoveClaim_4_Claim_0:System.Int64()", "RemoveClaim_4_Claim_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000001Eu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::TryRemoveClaim_3_Claim_0:System.Int64()", "TryRemoveClaim_3_Claim_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000003Cu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::WriteTo_11_BinaryWriter_0:System.Int64()", "WriteTo_11_BinaryWriter_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests[21] = {
	{ 0x0000005Cu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000042u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::AddIdentities_1_IEnumerableClaimsIdentity_0:System.Int64()", "AddIdentities_1_IEnumerableClaimsIdentity_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000044u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::AddIdentities_1_IEnumerableClaimsIdentity_1:System.Int64()", "AddIdentities_1_IEnumerableClaimsIdentity_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000040u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::AddIdentity_0_ClaimsIdentity_0:System.Int64()", "AddIdentity_0_ClaimsIdentity_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000046u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::Clone_2__2:System.Int64()", "Clone_2__2", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000048u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_3_System_PredicateClaim_0:System.Int64()", "FindAll_3_System_PredicateClaim_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000004Au, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_3_System_PredicateClaim_1:System.Int64()", "FindAll_3_System_PredicateClaim_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000004Cu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_4_string_0:System.Int64()", "FindAll_4_string_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000004Eu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_4_string_1:System.Int64()", "FindAll_4_string_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000050u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_5_System_PredicateClaim_0:System.Int64()", "FindFirst_5_System_PredicateClaim_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000051u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_5_System_PredicateClaim_1:System.Int64()", "FindFirst_5_System_PredicateClaim_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000052u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_6_string_0:System.Int64()", "FindFirst_6_string_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000053u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_6_string_1:System.Int64()", "FindFirst_6_string_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000054u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_7_System_PredicateClaim_0:System.Int64()", "HasClaim_7_System_PredicateClaim_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000055u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_7_System_PredicateClaim_1:System.Int64()", "HasClaim_7_System_PredicateClaim_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000056u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_8_string_string_0:System.Int64()", "HasClaim_8_string_string_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000057u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_8_string_string_1:System.Int64()", "HasClaim_8_string_string_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000058u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_8_string_string_2:System.Int64()", "HasClaim_8_string_string_2", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000059u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::IsInRole_9_string_0:System.Int64()", "IsInRole_9_string_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000005Au, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::IsInRole_9_string_1:System.Int64()", "IsInRole_9_string_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000005Bu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::WriteTo_10_BinaryWriter_0:System.Int64()", "WriteTo_10_BinaryWriter_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests[4] = {
	{ 0x00000014u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000000Eu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::Clone_0__0:System.Int64()", "Clone_0__0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000010u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::Clone_1_ClaimsIdentity_0:System.Int64()", "Clone_1_ClaimsIdentity_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000012u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::WriteTo_2_BinaryWriter_0:System.Int64()", "WriteTo_2_BinaryWriter_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests[2] = {
	{ 0x0000005Eu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000005Du, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests::Clone_0__0:System.Int64()", "Clone_0__0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
};

static constexpr ReflectionQueryMethodDescriptor kReflMethods_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests[3] = {
	{ 0x00000061u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests::.ctor:System.Void()", ".ctor", "System.Void", 0, nullptr, 0u, nullptr, 0u },
	{ 0x0000005Fu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests::IsInRole_0_string_0:System.Int64()", "IsInRole_0_string_0", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
	{ 0x00000060u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests::IsInRole_0_string_1:System.Int64()", "IsInRole_0_string_1", "System.Int64", 0, nullptr, 0u, nullptr, 0u },
};

static const ReflectionQueryTypeDescriptor kReflTypes[10] = {
	{ 0x00000004u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert", "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert", "Chaos.TestFramework", "Chaos.TestFramework.Assert", "Chaos.TestFramework.Assert", nullptr, kReflFields_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert, 1u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert, 29u, nullptr, 0u, 0u },
	{ 0x00000003u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException", "Chaos.TestFramework.Sdk/Chaos.TestFramework.AssertionException", "Chaos.TestFramework", "Chaos.TestFramework.AssertionException", "Chaos.TestFramework.AssertionException", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_AssertionException, 1u, nullptr, 0u, 0u },
	{ 0x00000007u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute", "Chaos.TestFramework.Sdk/Chaos.TestFramework.BenchmarkAttribute", "Chaos.TestFramework", "Chaos.TestFramework.BenchmarkAttribute", "Chaos.TestFramework.BenchmarkAttribute", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_BenchmarkAttribute, 1u, nullptr, 0u, 0u },
	{ 0x00000008u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute", "Chaos.TestFramework.Sdk/Chaos.TestFramework.FactAttribute", "Chaos.TestFramework", "Chaos.TestFramework.FactAttribute", "Chaos.TestFramework.FactAttribute", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_FactAttribute, 1u, nullptr, 0u, 0u },
	{ 0x00000009u, "Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute", "Chaos.TestFramework.Sdk/Chaos.TestFramework.HotUpdateAttribute", "Chaos.TestFramework", "Chaos.TestFramework.HotUpdateAttribute", "Chaos.TestFramework.HotUpdateAttribute", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_Chaos_TestFramework_Sdk_Chaos_TestFramework_HotUpdateAttribute, 1u, nullptr, 0u, 0u },
	{ 0x00000005u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests", "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests", "AutoGenerated.System_Security_Claims", "AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests", "AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests, 21u, nullptr, 0u, 0u },
	{ 0x00000006u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests", "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests", "AutoGenerated.System_Security_Claims", "AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests", "AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests, 21u, nullptr, 0u, 0u },
	{ 0x00000002u, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests", "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests", "AutoGenerated.System_Security_Claims", "AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests", "AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests, 4u, nullptr, 0u, 0u },
	{ 0x0000000Au, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests", "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests", "AutoGenerated.System_Security_Claims", "AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests", "AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests, 2u, nullptr, 0u, 0u },
	{ 0x0000000Bu, "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests", "CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests", "AutoGenerated.System_Security_Claims", "AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests", "AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests", nullptr, nullptr, 0u, nullptr, 0u,
nullptr, 0u,    kReflMethods_CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests, 3u, nullptr, 0u, 0u },
};

static const ReflectionQueryTypeDescriptor* kReflTypePtrs[10] = {
	&kReflTypes[0],
	&kReflTypes[1],
	&kReflTypes[2],
	&kReflTypes[3],
	&kReflTypes[4],
	&kReflTypes[5],
	&kReflTypes[6],
	&kReflTypes[7],
	&kReflTypes[8],
	&kReflTypes[9],
};

extern const ReflectionQueryImageDescriptor kReflImage = { "CombinedSubjects", kReflTypePtrs, 10u, 1, 0, 0, 0 };

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
					CHAOS_IL2CPP_UINT64 __chaos_args_buf[1] = {0};
					CHAOS_IL2CPP_UINT64 __chaos_ret_buf[2] = {};
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
					CHAOS_IL2CPP_UINT64 __chaos_args_buf[1] = {0};
					CHAOS_IL2CPP_UINT64 __chaos_ret_buf[2] = {};
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::AddClaim_1_Claim_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaim_1_Claim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::AddClaims_2_IEnumerableClaim_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::AddClaims_2_IEnumerableClaim_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_AddClaims_2_IEnumerableClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::Clone_0__0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_Clone_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_5_System_PredicateClaim_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_5_System_PredicateClaim_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_5_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_6_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindAll_6_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindAll_6_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_7_System_PredicateClaim_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_7_System_PredicateClaim_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_7_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_8_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::FindFirst_8_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_FindFirst_8_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_10_string_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_10_string_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_10_string_string_2()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_10_string_string_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_9_System_PredicateClaim_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::HasClaim_9_System_PredicateClaim_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_HasClaim_9_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::RemoveClaim_4_Claim_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_RemoveClaim_4_Claim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::TryRemoveClaim_3_Claim_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_TryRemoveClaim_3_Claim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsIdentityTests::WriteTo_11_BinaryWriter_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsIdentityTests_WriteTo_11_BinaryWriter_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::AddIdentities_1_IEnumerableClaimsIdentity_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::AddIdentities_1_IEnumerableClaimsIdentity_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentities_1_IEnumerableClaimsIdentity_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::AddIdentity_0_ClaimsIdentity_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_AddIdentity_0_ClaimsIdentity_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::Clone_2__2()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_Clone_2__2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_3_System_PredicateClaim_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_3_System_PredicateClaim_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_3_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_4_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindAll_4_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindAll_4_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_5_System_PredicateClaim_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_5_System_PredicateClaim_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_5_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_6_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::FindFirst_6_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_FindFirst_6_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_7_System_PredicateClaim_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_7_System_PredicateClaim_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_7_System_PredicateClaim_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_8_string_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_8_string_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::HasClaim_8_string_string_2()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_HasClaim_8_string_string_2(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::IsInRole_9_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::IsInRole_9_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_IsInRole_9_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimsPrincipalTests::WriteTo_10_BinaryWriter_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimsPrincipalTests_WriteTo_10_BinaryWriter_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::Clone_0__0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::Clone_1_ClaimsIdentity_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_Clone_1_ClaimsIdentity_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Claims_ClaimTests::WriteTo_2_BinaryWriter_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Claims_ClaimTests_WriteTo_2_BinaryWriter_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericIdentityTests::Clone_0__0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericIdentityTests_Clone_0__0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests::.ctor()
extern "C" void CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests__ctor(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests::IsInRole_0_string_0()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_0(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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

// Managed method: CombinedSubjects/AutoGenerated.System_Security_Claims.System_Security_Principal_GenericPrincipalTests::IsInRole_0_string_1()
extern "C" CHAOS_IL2CPP_INT64 CombinedSubjects_AutoGenerated_System_Security_Claims_System_Security_Principal_GenericPrincipalTests_IsInRole_0_string_1(CHAOS_IL2CPP_INTPTR chaos_fn_arg_0)
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
extern "C" const int kAotMethodCount = 84;

extern "C" void ChaosJitRegisterAll() {}